-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_crystal_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest"
map_image_filename = "data/story/common/locations/layna_forest.png"
map_subname = "???"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/sounds/wind.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

-- the main character handler
local hero = nil

-- Dialogue heroes
local bronann = nil
local kalya =  nil
local orlinn = nil

-- the main map loading code
function Load(m)

    Map = m;
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(true); -- no other enemies than the boss here.

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("data/visuals/ambient/clouds.png", 5.0, -5.0, true);

    -- Permits the display of the crystal ect...
    Map:GetScriptSupervisor():AddScript("data/story/layna_forest/crystal_appearance/layna_forest_crystal_appearance_anim.lua");

    -- Preload the useful musics
    AudioManager:LoadMusic("data/music/Zander Noriega - School of Quirks.ogg", Map);

    -- Handle time of the day lighting, won't be used when arriving here the first time as expected.
    _HandleTwilight();
end

-- Handle the twilight advancement after the crystal scene
function _HandleTwilight()

    -- If the characters have seen the crystal, then it's time to make the twilight happen
    if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_crystal_event_done") < 1) then
        return;
    end

    -- test if the day time is sufficiently advanced
    if (GlobalManager:GetGameEvents():DoesEventExist("story", "layna_forest_twilight_value") == false) then
        GlobalManager:GetGameEvents():SetEventValue("story", "layna_forest_twilight_value", 0);
    end

    Map:GetScriptSupervisor():AddScript("data/story/layna_forest/after_crystal_twilight.lua");
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 30, 22, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    local x_position = GlobalManager:GetMapData():GetSaveLocationX();
    local y_position = GlobalManager:GetMapData():GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Make the character look at us in that case
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(x_position, y_position);
    end

    -- Create secondary characters
    bronann = CreateSprite(Map, "Bronann", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.NORTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    bronann:SetVisible(false);

    kalya = CreateSprite(Map, "Kalya", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.NORTH);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);
end

-- The boss map sprite
local wolf = nil

local crystal = nil
local crystal_effect = nil

-- The heal particle effect map object
local heal_effect = nil

function _CreateObjects()
    local object = nil
    local npc = nil

    -- save point
    vt_map.SavePoint.Create(58, 87);

    local chest1 = CreateTreasure(Map, "layna_forest_crystal_chest", "Wood_Chest1", 16, 38, vt_map.MapMode.GROUND_OBJECT);
    chest1:AddItem(15, 1); -- Lotus Petal

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject.Create("data/visuals/particle_effects/heal_particle.lua", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    heal_effect:Stop(); -- Don't run it until the character heals itself

    -- Heal point
    npc = CreateSprite(Map, "Butterfly", 69, 86, vt_map.MapMode.GROUND_OBJECT);
    npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    npc:SetVisible(false);
    npc:SetName(""); -- Unset the speaker name

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Your party feels better.");
    dialogue:AddLineEvent(text, npc, "heal point", "");
    npc:AddDialogueReference(dialogue);

    npc = CreateObject(Map, "Layna Statue", 69, 86, vt_map.MapMode.GROUND_OBJECT);
    npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    npc:SetInteractionIcon("data/gui/map/heal_anim.lua")

    -- The boss map sprite, placed for final battle
    wolf = CreateSprite(Map, "Fenrir", 42, 63, vt_map.MapMode.GROUND_OBJECT);
    wolf:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);
    wolf:SetDirection(vt_map.MapMode.SOUTH);

    if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        wolf:SetPosition(0, 0);
        wolf:SetVisible(false);
        wolf:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    -- Orlinn, waiting...
    orlinn = CreateSprite(Map, "Orlinn", 42, 58, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.NORTH);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    crystal = CreateSprite(Map, "Crystal", 41, 45, vt_map.MapMode.GROUND_OBJECT);
    crystal:SetDirection(vt_map.MapMode.SOUTH);
    crystal:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    crystal:SetVisible(false);

    if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        crystal:SetPosition(0, 0);
        crystal:SetVisible(false);
        crystal:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    crystal_effect = vt_map.ParticleObject.Create("data/visuals/particle_effects/inactive_save_point.lua", 41, 46, vt_map.MapMode.GROUND_OBJECT);
    crystal_effect:Stop(); -- Don't run it until the character heals itself

    -- trees, etc
    local map_trees = {
        { "Tree Small4", 8, 62 },
        { "Tree Tiny1", 81, 79 },
        { "Tree Tiny3", 81, 28 },
        { "Tree Small3", 83, 73 },
        { "Tree Small3", 88, 35 },
        { "Tree Small5", 75.5, 45 },
        { "Tree Small6", 3, 65 },
        { "Tree Little2", 3, 55 },
        { "Tree Little3", 6, 61 },
        { "Tree Big1", 54, 78 },
        { "Tree Small6", 2, 83 },
        { "Tree Small4", 6, 85 },
        { "Tree Small3", 10, 87 },
        { "Tree Small5", 12, 91 },
        { "Tree Little1", 15, 87 },
        { "Tree Small4", 6, 89 },
        { "Tree Small2", 3, 87 },
        { "Tree Little1", 17, 90 },
        { "Tree Small4", 16, 92 },
        { "Tree Small3", 18, 95 },
        { "Tree Small6", 22, 96 },
        { "Tree Small3", 8, 93 },
        { "Tree Small4", 3, 95 },
        { "Tree Tiny4", 1, 88 },
        { "Tree Small3", 13, 94 },
        { "Tree Small6", 10, 97 },
        { "Tree Big1", 16, 98 },
        { "Tree Small3", 0, 48 },
        { "Tree Small6", -2, 42 },
        { "Tree Small4", -1, 35 },
        { "Tree Small1", 2.5, 45 },
        { "Tree Small3", 2, 24 },
        { "Tree Small3", 84, 66 },
        { "Tree Small6", 77, 58 },
        { "Tree Big2", 12, 53 },
        { "Tree Big1", 16, 37 },
        { "Tree Small3", 35, 55 },
        { "Tree Small4", 50, 58 },
        { "Tree Small3", 87, 13 },
        { "Tree Small4", 92, 16 },
        { "Tree Small3", 95, 19 },
        { "Tree Small6", 82, 10 },

        -- North part
        { "Tree Big1", 0, 20 },
        { "Tree Small1", -1, 15 },
        { "Tree Small3", 6, 19 },
        { "Tree Little3", 10, 17 },
        { "Tree Small4", 11, 20 },
        { "Tree Small3", 15, 15 },
        { "Tree Small6", 4, 9 },
        { "Tree Small3", 1, 5 },
        { "Tree Small5", 7, 2 },
        { "Tree Tiny2", 13, 6 },
        { "Tree Small3", 19, 14 },
        { "Tree Little1", 10, 10 },
        { "Tree Small3", 19, 17 },
        { "Tree Small4", 24, 13 },
        { "Tree Tiny2", 25, 6 },
        { "Tree Small3", 30, 12 },
        { "Tree Small6", 34, 6 },
        { "Tree Little3", 40, 3 },
        { "Tree Big2", 41, 14 },
        { "Tree Small5", 46, 9 },
        { "Tree Small4", 50, 15 },
        { "Tree Little3", 4, 12 },
        { "Tree Small4", 19, 7 },
        { "Tree Small3", 35, 13 },
        { "Tree Tiny3", 40, 7 },
        { "Tree Small5", 46, 13 },
        { "Tree Little2", 52, 11 },
        { "Tree Small3", 57, 14 },
        { "Tree Small4", 56, 7 },
        { "Tree Small3", 61, 4 },
        { "Tree Small5", 67, 3 },
        { "Tree Small4", 70, 7 },
        { "Tree Small3", 72, 13 },
        { "Tree Small6", 76, 19 },
        { "Tree Small3", 64, 8 },
        { "Tree Small5", 66, 14 },
        { "Tree Small3", 62, 12 },
        { "Tree Small3", 71, 17 },
        { "Tree Small4", 81, 18 },
        { "Tree Small3", 77, 12 },
        { "Tree Small5", 74, 8.2 },
        { "Tree Small3", 86, 20 },
        { "Tree Little2", 90, 23 },
        { "Tree Tiny1", 88, 27 },
        { "Tree Small6", 92, 28 },
        { "Tree Small4", 96, 26 },
        { "Tree Little2", 100, 25 },
        { "Tree Little4", 105, 23 },
        { "Tree Small3", 103, 27.2 },
        { "Tree Small4", 101, 20 },

        -- south part
        { "Tree Little1", 37, 99 },
        { "Tree Small4", 43, 102 },
        { "Tree Small3", 51, 99 },
        { "Tree Little4", 55, 98 },
        { "Tree Tiny3", 61, 98 },
        { "Tree Small3", 68, 100 },
        { "Tree Small6", 75, 101 },
        { "Tree Tiny1", 47, 93 },

        --east part
        { "Tree Big2", 87, 31 },
        { "Tree Small3", 91, 33 },
        { "Tree Little3", 90, 37 },
        { "Tree Small4", 93, 39 },
        { "Tree Little2", 95, 42 },
        { "Tree Small6", 98, 45 },
        { "Tree Tiny1", 96, 48 },
        { "Tree Small2", 99, 51 },
        { "Tree Small4", 100, 54 },
        { "Tree Small1", 99, 57 },
        { "Tree Big1", 98, 60 },
        { "Tree Small4", 99, 63 },
        { "Tree Small3", 101, 66 },
        { "Tree Small5", 102, 69.2 },
        { "Tree Small1", 104, 72 },
        { "Tree Small6", 105, 75 },
        { "Tree Small2", 104, 79 },
        { "Tree Small5", 102, 83 },
        { "Tree Little2", 100, 86 },
        { "Tree Small4", 101, 90 },
        { "Tree Big2", 97, 93 },
        { "Tree Small5", 94, 96 },
        { "Tree Big2", 83, 92 },
        { "Tree Small4", 87, 99 },
        { "Tree Small5", 101, 94 },
        { "Tree Big1", 104, 86.2 },
        { "Tree Little2", 95, 34 },
        { "Tree Small4", 98, 38 },
        { "Tree Tiny1", 102, 44 },
        { "Tree Small6", 103, 50 },
        { "Tree Small3", 98, 31 },
        { "Tree Small3", 102, 40 },
        { "Tree Small5", 105, 55 },
        { "Tree Small6", 103, 33 },
        { "Tree Small3", 93.5, 46 },
        { "Tree Small5", 95, 55 },
        { "Tree Little2", 93, 68 },
        { "Tree Big2", 91, 50 },
        { "Tree Small3", 104, 58 },
        { "Tree Small3", 107, 62 },
        { "Tree Small3", 106, 43 },
        { "Tree Small5", 107, 37 },
        { "Tree Small2", 108, 48 },
        { "Tree Small4", 109, 56 },
        { "Tree Small3", 110, 65 },
        { "Tree Small6", 108, 69 },
        { "Tree Small2", 110, 73 },
        { "Tree Small4", 108, 76 },
        { "Tree Small6", 112, 60 },
        { "Tree Small3", 113, 68 },
        { "Tree Big1", 115, 71 },
        { "Tree Little2", 114, 75 },
        { "Tree Small3", 116, 61 },
        { "Tree Small4", 112, 78 },
        { "Tree Small1", 109, 81 },
        { "Tree Small3", 110, 85 },
        { "Tree Small3", 107, 89 },
        { "Tree Small5", 105, 93 },
        { "Tree Small3", 102, 98 },
        { "Tree Small5", 108, 99 },
        { "Tree Big2", 109, 92 },
        { "Tree Small4", 112, 89.2 },
        { "Tree Small5", 114, 86 },
        { "Tree Tiny1", 83, 23 },
        { "Tree Small2", 116, 79 },
        { "Tree Small3", 118, 76 },
        { "Tree Small3", 117, 66 },
        { "Tree Small3", 117, 85 },
        { "Tree Small4", 115, 90 },
        { "Tree Small6", 113, 93 },
        { "Tree Small3", 114, 100 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_trees) do
        --print(my_array[1], my_array[2], my_array[3]);
        CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
    end

    -- grass array
    local map_grass = {
        -- the grass, hiding a bit the snakes
        { "Grass Clump1", 52, 79 },
        { "Grass Clump1", 9, 63 },
        { "Grass Clump1", 79, 50 },
        { "Grass Clump1", 7, 68 },
        { "Grass Clump1", 5, 76 },
        { "Grass Clump1", 2, 73 },
        { "Grass Clump1", 9, 78 },
        { "Grass Clump1", 14, 80 },
        { "Grass Clump1", 18, 83 },
        { "Grass Clump1", 20, 86 },
        { "Grass Clump1", 27, 92 },
        { "Grass Clump1", 33, 95 },
        { "Grass Clump1", 22, 68 },
        { "Grass Clump1", 30, 66 },
    }

    -- Loads the grass according to the array
    for my_index, my_array in pairs(map_grass) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
end

-- Special event references which destinations must be updated just before being called.
local move_next_to_bronann_event = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Triggered events
    vt_map.MapTransitionEvent.Create("to forest cave 2", "data/story/layna_forest/layna_forest_cave2_map.lua",
                                     "data/story/layna_forest/layna_forest_cave2_script.lua", "from layna forest crystal");

    -- Heal point
    vt_map.ScriptedEvent.Create("heal point", "heal_party", "heal_done");

    -- Wolf final fight
    event = vt_map.BattleEncounterEvent.Create("Fenrir Battle");
    event:SetMusic("data/music/accion-OGA-djsaryon.ogg");
    event:SetBackground("data/battles/battle_scenes/forest_background.png");
    event:AddEnemy(8, 512, 500);
    event:SetBoss(true);
    event:AddEventLinkAtEnd("Make the wolf disappear");

    event = vt_map.ScriptedEvent.Create("Make the wolf disappear", "make_wolf_invisible", "");
    event:AddEventLinkAtEnd("boss fight post-dialogue");

    -- Start of ending dialogue.
    event = vt_map.ScriptedEvent.Create("boss fight post-dialogue", "post_boss_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 50);

    vt_map.ScriptedEvent.Create("Map:Popstate()", "Map_PopState", "");
    vt_map.ScriptedSpriteEvent.Create("kalya:SetCollision(ALL)", kalya, "Sprite_Collision_on", "");

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_bronann_event = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann", kalya, 0, 0, false);
    move_next_to_bronann_event:AddEventLinkAtEnd("first dialogue part");
    move_next_to_bronann_event:AddEventLinkAtEnd("Kalya looks at Orlinn");
    move_next_to_bronann_event:AddEventLinkAtEnd("kalya:SetCollision(ALL)");

    vt_map.LookAtSpriteEvent.Create("Kalya looks at Orlinn", kalya, orlinn);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Bronann", orlinn, bronann);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks east", kalya, vt_map.MapMode.EAST);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks west", kalya, vt_map.MapMode.WEST);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks south", kalya, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks east", bronann, vt_map.MapMode.EAST);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks west", bronann, vt_map.MapMode.WEST);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks north", bronann, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks east", orlinn, vt_map.MapMode.EAST);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks west", orlinn, vt_map.MapMode.WEST);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Orlinn!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("I can feel it, sis'. It's coming!");
    dialogue:AddLineEvent(text, orlinn, "", "Orlinn comes closer of the crystal spawn point");
    text = vt_system.Translate("Orlinn, stop!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    event = vt_map.DialogueEvent.Create("first dialogue part", dialogue);
    event:AddEventLinkAtEnd("Kalya runs to Orlinn");
    event:AddEventLinkAtEnd("Bronann runs to Orlinn");

    vt_map.PathMoveSpriteEvent.Create("Orlinn comes closer of the crystal spawn point", orlinn, 42, 54, false);
    vt_map.PathMoveSpriteEvent.Create("Bronann runs to Orlinn", bronann, 40, 56, true);

    event = vt_map.PathMoveSpriteEvent.Create("Kalya runs to Orlinn", kalya, 44, 56, true);
    event:AddEventLinkAtEnd("First tremor");

    event = vt_map.ScriptedEvent.Create("First tremor", "first_tremor", "");
    event:AddEventLinkAtEnd("second dialogue part");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("What's happening!?");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    event = vt_map.DialogueEvent.Create("second dialogue part", dialogue);
    event:AddEventLinkAtEnd("Kalya looks east 1", 500);
    event:AddEventLinkAtEnd("kalya:SetCollision(NONE)");

    event = vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks east 1", kalya, vt_map.MapMode.EAST);
    event:AddEventLinkAtEnd("Bronann looks west 1", 200);

    event = vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks west 1", bronann, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Kalya looks west 1", 800);

    event = vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks west 1", kalya, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Bronann looks east 1", 800);

    event = vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks east 1", bronann, vt_map.MapMode.EAST);
    event:AddEventLinkAtEnd("Bronann looks north", 1000);
    event:AddEventLinkAtEnd("Kalya looks north", 1000);
    event:AddEventLinkAtEnd("Both are surprised", 1000);
    event:AddEventLinkAtEnd("Set Camera on crystal", 1000);

    vt_map.ScriptedSpriteEvent.Create("Set Camera on crystal", crystal, "SetCamera", "");

    event = vt_map.ScriptedEvent.Create("Both are surprised", "bronann_kalya_exclamation", "");
    event:AddEventLinkAtEnd("Make crystal appear");

    event = vt_map.ScriptedEvent.Create("Make crystal appear", "make_crystal_appear", "make_crystal_appear_update");
    event:AddEventLinkAtEnd("third dialogue part", 1000);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Gosh! By all heavens, is this...?");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("A crystal stone?!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("This is her. I heard her voice calling.");
    dialogue:AddLine(text, orlinn);
    event = vt_map.DialogueEvent.Create("third dialogue part", dialogue);
    event:AddEventLinkAtEnd("crystal dialogue part");

    vt_map.SoundEvent.Create("crystal voice sound 1", "data/story/layna_forest/crystal_appearance/crystal-sentence1.ogg")
    vt_map.SoundEvent.Create("crystal voice sound 2", "data/story/layna_forest/crystal_appearance/crystal-sentence2.ogg")
    vt_map.SoundEvent.Create("crystal voice sound 3", "data/story/layna_forest/crystal_appearance/crystal-sentence3.ogg")
    vt_map.SoundEvent.Create("crystal voice sound 4", "data/story/layna_forest/crystal_appearance/crystal-sentence4.ogg")
    vt_map.SoundEvent.Create("crystal voice sound 5", "data/story/layna_forest/crystal_appearance/crystal-sentence5.ogg")
    vt_map.SoundEvent.Create("crystal voice sound 6", "data/story/layna_forest/crystal_appearance/crystal-sentence6.ogg")

    dialogue = vt_map.SpriteDialogue.Create();
    dialogue:SetInputBlocked(true);
    text = vt_system.Translate("Dear chosen one, the time has finally come.");
    dialogue:AddLineTimedEvent(text, crystal, 5000, "crystal voice sound 1", "");
    text = vt_system.Translate("For us, it will be but an instant.");
    dialogue:AddLineTimedEvent(text, crystal, 5000, "crystal voice sound 2", "");
    text = vt_system.Translate("For you, it might be decades.");
    dialogue:AddLineTimedEvent(text, crystal, 5000, "crystal voice sound 3", "");
    text = vt_system.Translate("May you bring a happy end to this foolish destiny of ours.");
    dialogue:AddLineTimedEvent(text, crystal, 6800, "crystal voice sound 4", "");
    text = vt_system.Translate("We're all anxiously awaiting your return.");
    dialogue:AddLineTimedEvent(text, crystal, 5000, "crystal voice sound 5", "");
    text = vt_system.Translate("Now, come. Become one with our holy hope.");
    dialogue:AddLineTimedEvent(text, crystal, 5000, "crystal voice sound 6", "");
    event = vt_map.DialogueEvent.Create("crystal dialogue part", dialogue);
    event:AddEventLinkAtEnd("fourth dialogue part");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("What does all of this mean?");
    dialogue:AddLineEmote(text, kalya, "thinking dots");
    text = vt_system.Translate("I shall become one with our only hope.");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("What! Orlinn, no!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    event = vt_map.DialogueEvent.Create("fourth dialogue part", dialogue);
    event:AddEventLinkAtEnd("Orlinn comes even closer of the crystal");
    event:AddEventLinkAtEnd("Bronann runs in front of Orlinn", 300);

    vt_map.PathMoveSpriteEvent.Create("Orlinn comes even closer of the crystal", orlinn, 42, 50, false);

    event = vt_map.PathMoveSpriteEvent.Create("Bronann runs in front of Orlinn", bronann, 42, 48, true);
    event:AddEventLinkAtEnd("Bronann looks south");
    event:AddEventLinkAtEnd("fifth dialogue part");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann, the crystal! No!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    event = vt_map.DialogueEvent.Create("fifth dialogue part", dialogue);
    event:AddEventLinkAtEnd("kalya:SetCollision(NONE)");
    event:AddEventLinkAtEnd("Bronann is hurt");
    event:AddEventLinkAtEnd("white flash");

    vt_map.ScriptedSpriteEvent.Create("kalya:SetCollision(NONE)", kalya, "Sprite_Collision_off", "");

    event = vt_map.AnimateSpriteEvent.Create("Bronann is hurt", bronann, "hurt", 1200);
    event:AddEventLinkAtEnd("Bronann is sleeping");

    vt_map.AnimateSpriteEvent.Create("Bronann is sleeping", bronann, "sleeping", 0); -- 0 means forever

    event = vt_map.ScriptedEvent.Create("white flash", "white_flash", "white_flash_update");
    event:AddEventLinkAtEnd("sixth dialogue part");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    event = vt_map.DialogueEvent.Create("sixth dialogue part", dialogue);
    event:AddEventLinkAtEnd("Kalya runs to Bronann");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya runs to Bronann", kalya, 39, 47, true);
    event:AddEventLinkAtEnd("Kalya looks east");
    event:AddEventLinkAtEnd("Kalya kneels", 300);
    event:AddEventLinkAtEnd("seventh dialogue part", 600);

    vt_map.AnimateSpriteEvent.Create("Kalya kneels", kalya, "kneeling", 0); -- 0 means forever

    -- Simply stop the custom animation
    vt_map.ScriptedSpriteEvent.Create("Kalya gets up", kalya, "Terminate_all_events", "");

    event = vt_map.ScriptedEvent.Create("Kalya and orlinn are surprised", "orlinn_kalya_exclamation", "");
    event:AddEventLinkAtEnd("Kalya looks east");
    event:AddEventLinkAtEnd("Bronann kneels");

    vt_map.AnimateSpriteEvent.Create("Bronann kneels", bronann, "kneeling", 0); -- 0 means forever

    -- Simply stop the custom animation
    vt_map.ScriptedSpriteEvent.Create("Bronann gets up", bronann, "Terminate_all_events", "");

    vt_map.ScriptedEvent.Create("Fade out music", "fade_out_music", "");

    vt_map.ScriptedEvent.Create("Play funny music", "play_funny_music", "");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann! Are you all right? Answer me!!");
    dialogue:AddLineEventEmote(text, kalya, "", "Orlinn looks west", "exclamation");
    text = vt_system.Translate("Bronann!");
    dialogue:AddLineEventEmote(text, kalya, "", "Orlinn looks east", "exclamation");
    text = vt_system.Translate("Woah, where am I?");
    dialogue:AddLineEmote(text, orlinn, "interrogation");
    text = vt_system.Translate("Bronann!");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Sis! What's the matter? What happened?");
    dialogue:AddLineEventEmote(text, orlinn, "Orlinn looks north", "Kalya gets up", "interrogation");
    text = vt_system.Translate("What happened? Orlinn, what have you done?");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks south", "", "exclamation");
    text = vt_system.Translate("... My head.");
    dialogue:AddLineEvent(text, bronann, "", "Kalya and orlinn are surprised");
    text = vt_system.Translate("Bronann! Are you alright?");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks east", "", "exclamation");
    text = vt_system.Translate("I, I guess so.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("The wolf! The crystal! Orlinn! Are you ok?");
    dialogue:AddLineEventEmote(text, bronann, "Bronann gets up", "", "exclamation");
    text = vt_system.Translate("Don't worry, I'm fine. Err, what crystal? What... wolf??");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("You won't be fine for long after I'm done with you!!");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks south", "Fade out music", "thinking dots");
    text = vt_system.Translate("Huh? Wait, I didn't do anything this time!");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    text = vt_system.Translate("Oh? You didn't?");
    dialogue:AddLineEventEmote(text, kalya, "Play funny music", "", "exclamation");
    event = vt_map.DialogueEvent.Create("seventh dialogue part", dialogue);
    event:AddEventLinkAtEnd("Kalya runs after Orlinn");
    event:AddEventLinkAtEnd("Orlinn runs to point 3");
    event:AddEventLinkAtEnd("Bronann sighs and think", 2000);

    -- Kalya runs after Orlinn's loop
    event = vt_map.PathMoveSpriteEvent.Create("Kalya runs after Orlinn", kalya, orlinn, true);
    event:AddEventLinkAtEnd("Kalya runs after Orlinn");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn runs to point 1", orlinn, 32, 38, true);
    event:AddEventLinkAtEnd("Orlinn runs to point 2");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn runs to point 2", orlinn, 32, 45, true);
    event:AddEventLinkAtEnd("Orlinn runs to point 3");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn runs to point 3", orlinn, 50, 45, true);
    event:AddEventLinkAtEnd("Orlinn runs to point 4");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn runs to point 4", orlinn, 50, 38, true);
    event:AddEventLinkAtEnd("Orlinn runs to point 1");

    vt_map.ScriptedSpriteEvent.Create("Kalya stops running", kalya, "Terminate_all_events", "");

    vt_map.AnimateSpriteEvent.Create("Bronann sighs", bronann, "hero_stance", 6000);

    event = vt_map.PathMoveSpriteEvent.Create("Kalya comes back next to Bronann", kalya, 44, 49, true);
    event:AddEventLinkAtEnd("Kalya looks at Bronann");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("When I catch you, I'll make you wish you were never born! Rahh!");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Yiek!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Come here, now!");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Yiek!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("(Sigh)... I should have stayed home today.");
    dialogue:AddLineEvent(text, bronann, "Bronann sighs", "");
    text = vt_system.Translate("Ouch, why is my head hurting so much?");
    dialogue:AddLineEvent(text, bronann, "", "Kalya stops running");
    text = vt_system.Translate("Are you ok? We should head back to the village so Lilly can examine you.");
    dialogue:AddLineEvent(text, kalya, "Kalya comes back next to Bronann", "");
    text = vt_system.Translate("I still feel really dizzy, but I'll be ok.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("I'm relieved that Orlinn is fine as well. Speaking of that, Orlinn! Stop running and come here right now!");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Bronann sighs and think", dialogue);
    event:AddEventLinkAtEnd("Orlinn stops running");
    event:AddEventLinkAtEnd("Last dialogue");

    vt_map.ScriptedSpriteEvent.Create("Orlinn stops running", orlinn, "Terminate_all_events", "");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn comes back next to Bronann", orlinn, 40, 49, true);
    event:AddEventLinkAtEnd("Orlinn looks at Bronann");

    vt_map.ScriptedEvent.Create("Play wind music", "play_wind_music", "");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Owww. Alright.");
    dialogue:AddLineEventEmote(text, orlinn, "Fade out music", "Orlinn comes back next to Bronann", "sweat drop");
    text = vt_system.Translate("Now, Orlinn, tell us. Why on earth did you go into the forest alone, and how did you get there so quickly?");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("I don't know.");
    dialogue:AddLineEmote(text, orlinn, "interrogation");
    text = vt_system.Translate("Why was that giant wolf protecting you?");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("What was that crystal? Why did you want to 'become one' with it?");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("I, I, I sincerely don't know. I swear! It was like I was... dreaming.");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    text = vt_system.Translate("I merely remember a voice in my head, telling me to come.");
    dialogue:AddLineEmote(text, orlinn, "thinking dots");
    text = vt_system.Translate("You know you won't get away with this. When you see Herth...");
    dialogue:AddLineEmote(text, kalya, "thinking dots");
    text = vt_system.Translate("I swear it's the truth!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    text = vt_system.Translate("Making one with the crystal, huh?");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    text = vt_system.Translate("Let's not panic. Bronann, you look fine. Anyway, we'd better get back to the village and see the elders as soon as possible.");
    dialogue:AddLineEventEmote(text, kalya, "", "Play wind music", "thinking dots");
    event = vt_map.DialogueEvent.Create("Last dialogue", dialogue);
    event:AddEventLinkAtEnd("kalya goes back to party");
    event:AddEventLinkAtEnd("orlinn goes back to party");

    event = vt_map.PathMoveSpriteEvent.Create("kalya goes back to party", kalya, bronann, false);
    event:AddEventLinkAtEnd("end of crystal event");

    event = vt_map.PathMoveSpriteEvent.Create("orlinn goes back to party", orlinn, bronann, false);

    event = vt_map.ScriptedEvent.Create("end of crystal event", "end_of_crystal_event", "");
    event:AddEventLinkAtEnd("Map:Popstate()");
end

-- zones
local to_forest_cave2_zone = nil
local wolf_battle_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_forest_cave2_zone = vt_map.CameraZone.Create(28, 33, 23, 25);
    to_forest_cave2_zone:SetInteractionIcon("data/gui/map/exit_anim.lua")

    wolf_battle_zone = vt_map.CameraZone.Create(38, 46, 63, 66);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_forest_cave2_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest cave 2");
    elseif (wolf_battle_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetGameEvents():DoesEventExist("story", "Fenrir beaten") == false) then
            hero:SetMoving(false);
            EventManager:StartEvent("Fenrir Battle");
        end
    end
end

-- Map Custom functions
-- Used through scripted events

-- Effect time used when applying the heal light effect
local heal_effect_time = 0;
local heal_color = vt_video.Color(0.0, 0.0, 1.0, 1.0);

local flash_effect_time = 0;
local flash_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

local crystal_appearance_time = 0;
local crystal_visible = false;
local crystal_light_effect = nil

map_functions = {

    SetCamera = function(sprite)
        Map:SetCamera(sprite, 800);
    end,

    Sprite_Collision_on = function(sprite)
        if (sprite ~= nil) then
            sprite:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        end
    end,

    Sprite_Collision_off = function(sprite)
        if (sprite ~= nil) then
            sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end,

    Map_PopState = function()
        Map:PopState();
    end,

    heal_party = function()
        hero:SetMoving(false);
        -- Should be sufficient to heal anybody
        local character_handler = GlobalManager:GetCharacterHandler()
        character_handler:GetActiveParty():AddHitPoints(10000)
        character_handler:GetActiveParty():AddSkillPoints(10000)
        Map:SetStamina(10000)
        Map:RemoveNegativeActiveStatusEffects()
        AudioManager:PlaySound("data/sounds/heal_spell.wav");
        heal_effect:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        heal_effect:Start();
        heal_effect_time = 0;
    end,

    heal_done = function()
        heal_effect_time = heal_effect_time + SystemManager:GetUpdateTime();

        if (heal_effect_time < 300.0) then
            heal_color:SetAlpha(heal_effect_time / 300.0 / 3.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(heal_color);
            return false;
        end

        if (heal_effect_time < 1000.0) then
            heal_color:SetAlpha(((1000.0 - heal_effect_time) / 700.0) / 3.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(heal_color);
            return false;
        end

        return true;
    end,

    make_wolf_invisible = function()
        wolf:SetVisible(false);
        wolf:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        GlobalManager:GetGameEvents():SetEventValue("story", "Fenrir beaten", 1);
    end,

    post_boss_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        hero:SetDirection(vt_map.MapMode.NORTH);

        bronann:SetDirection(hero:GetDirection())
        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition())
        bronann:SetVisible(true)
        Map:SetCamera(bronann)

        hero:SetVisible(false)
        hero:SetPosition(0, 0)

        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        kalya:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        move_next_to_bronann_event:SetDestination(bronann:GetXPosition() + 2.0, bronann:GetYPosition(), false);
    end,

    first_tremor = function()
        Map:GetEffectSupervisor():ShakeScreen(1.0, 2000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_SUDDEN);
        AudioManager:PlaySound("data/sounds/rumble.wav");
    end,

    bronann_kalya_exclamation = function()
        bronann:Emote("exclamation", vt_map.MapMode.ANIM_NORTH);
        kalya:Emote("exclamation", vt_map.MapMode.ANIM_NORTH);
        AudioManager:PlaySound("data/sounds/rumble.wav");
    end,

    make_crystal_appear = function()
        GlobalManager:GetGameEvents():SetEventValue("story", "layna_forest_crystal_appearance", 1);
    end,

    make_crystal_appear_update = function()
        if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_crystal_appearance") == 0) then
            return true;
        end

        crystal_appearance_time = crystal_appearance_time + SystemManager:GetUpdateTime();

        if (crystal_visible == false and crystal_appearance_time >= 10000) then
            -- Add a light upon the crystal
            crystal_light_effect = vt_map.Light.Create("data/visuals/lights/sun_flare_light_secondary.lua",
                    "data/visuals/lights/sun_flare_light_secondary.lua",
                    41.2, 43.0,
                    vt_video.Color(0.8, 0.8, 1.0, 0.3),
                    vt_video.Color(0.8, 0.8, 0.85, 0.2));

            -- Set the  crystal to visible while the white flash
            crystal:SetVisible(true);
            crystal_effect:Start();
            crystal_visible = true;
        end
        return false;
    end,

    white_flash = function()
        flash_effect_time = 0;
    end,

    white_flash_update = function()
        flash_effect_time = flash_effect_time + SystemManager:GetUpdateTime();

        if (flash_effect_time < 300.0) then
            flash_color:SetAlpha(flash_effect_time / 300.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(flash_color);
            return false;
        elseif (flash_effect_time >= 300.0 and flash_effect_time <= 1000.0) then
            if (crystal_visible == true) then
                -- hide the crystal and the effect
                crystal_effect:Stop();
                crystal:SetVisible(false);
                crystal:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                crystal_light_effect:SetVisible(false);
                AudioManager:PlaySound("data/sounds/crystal_chime.wav");
                crystal_visible = false;
            end
            return false; -- do nothing
        elseif (flash_effect_time > 1000.0 and flash_effect_time < 2500.0) then
            flash_color:SetAlpha(1.0 - (flash_effect_time - 1000.0) / (2500.0 - 1000.0));
            Map:GetEffectSupervisor():EnableLightingOverlay(flash_color);
            return false;
        end

        return true;
    end,

    Terminate_all_events = function(sprite)
        EventManager:EndAllEvents(sprite);
    end,

    orlinn_kalya_exclamation = function()
        orlinn:Emote("exclamation", vt_map.MapMode.ANIM_NORTH);
        kalya:Emote("exclamation", vt_map.MapMode.ANIM_NORTH);

        -- Restore also their collision mask
        orlinn:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        kalya:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        -- And prepare for the funny scene
        orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED - 7.0);
    end,

    fade_out_music = function()
        AudioManager:FadeOutActiveMusic(2000);
    end,

    play_funny_music = function()
        AudioManager:PlayMusic("data/music/Zander Noriega - School of Quirks.ogg");
    end,

    play_wind_music = function()
        AudioManager:PlayMusic("data/sounds/wind.ogg");
    end,

    end_of_crystal_event = function()
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Reset the hero
        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        hero:SetDirection(bronann:GetDirection())
        hero:SetVisible(true)
        bronann:SetVisible(false)
        bronann:SetPosition(0, 0)
        Map:SetCamera(hero, 800);

        -- Set event as done
        GlobalManager:GetGameEvents():SetEventValue("story", "layna_forest_crystal_event_done", 1);
        -- Start the twilight
        _HandleTwilight();
    end
}
