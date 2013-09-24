-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_crystal_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest"
map_image_filename = "img/menus/locations/layna_forest.png"
map_subname = "???"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "snd/wind.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- the main character handler
local hero = {};

-- Dialogue secondary hero
local kalya_sprite =  {};

-- Name of the main sprite. Used to reload the good one at the end of the event.
local main_sprite_name = "";

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

    Map.unlimited_stamina = true; -- no other enemies than the boss here.

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);

    -- Permits the display of the crystal ect...
    Map:GetScriptSupervisor():AddScript("dat/maps/layna_forest/crystal_appearance/layna_forest_crystal_appearance_anim.lua");

    -- Preload the useful musics
    AudioManager:LoadMusic("mus/Zander Noriega - School of Quirks.ogg", Map);

    -- Handle time of the day lighting, won't be used when arriving here the first time as expected.
    _HandleTwilight();
end

-- Handle the twilight advancement after the crystal scene
function _HandleTwilight()

    -- If the characters have seen the crystal, then it's time to make the twilight happen
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") < 1) then
        return;
    end

    -- test if the day time is sufficiently advanced
    if (GlobalManager:DoesEventExist("story", "layna_forest_twilight_value") == false) then
        GlobalManager:SetEventValue("story", "layna_forest_twilight_value", 0);
    end

    Map:GetScriptSupervisor():AddScript("dat/maps/layna_forest/after_crystal_twilight.lua");
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 30, 22);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    local x_position = GlobalManager:GetSaveLocationX();
    local y_position = GlobalManager:GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Use the save point position, and clear the save position data for next maps
        GlobalManager:UnsetSaveLocation();
        -- Make the character look at us in that case
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(x_position, y_position);
    end

    Map:AddGroundObject(hero);

    -- Create secondary character - Kalya
    kalya_sprite = CreateSprite(Map, "Kalya",
                            hero:GetXPosition(), hero:GetYPosition());

    kalya_sprite:SetDirection(vt_map.MapMode.NORTH);
    kalya_sprite:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya_sprite:SetVisible(false);
    Map:AddGroundObject(kalya_sprite);
end

-- The boss map sprite
local wolf = {};

local orlinn = {};

local crystal = {};
local crystal_effect = {};

-- The heal particle effect map object
local heal_effect = {};

function _CreateObjects()
    local object = {}
    local npc = {}

    -- save point
    Map:AddSavePoint(58, 87);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject("dat/effects/particles/heal_particle.lua", 0, 0);
	heal_effect:SetObjectID(Map.object_supervisor:GenerateObjectID());
    heal_effect:Stop(); -- Don't run it until the character heals itself
    Map:AddGroundObject(heal_effect);

    -- Heal point
    npc = CreateSprite(Map, "Butterfly", 69, 86);
    npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    npc:SetVisible(false);
    npc:SetName(""); -- Unset the speaker name
    Map:AddGroundObject(npc);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Your party feels better...");
    dialogue:AddLineEvent(text, npc, "heal point", "");
    DialogueManager:AddDialogue(dialogue);
    npc:AddDialogueReference(dialogue);

    -- The boss map sprite, placed for final battle
    wolf = CreateSprite(Map, "Fenrir", 42, 63);
    wolf:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);
    wolf:SetDirection(vt_map.MapMode.SOUTH);
    Map:AddGroundObject(wolf);
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        wolf:SetPosition(0, 0);
        wolf:SetVisible(false);
        wolf:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    -- Orlinn, waiting...
    orlinn = CreateSprite(Map, "Orlinn", 42, 58);
    orlinn:SetDirection(vt_map.MapMode.NORTH);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    Map:AddGroundObject(orlinn);
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    crystal = CreateSprite(Map, "Crystal", 41, 45);
    crystal:SetDirection(vt_map.MapMode.SOUTH);
    crystal:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    crystal:SetVisible(false);
    Map:AddGroundObject(crystal);
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        crystal:SetPosition(0, 0);
        crystal:SetVisible(false);
        crystal:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    crystal_effect = vt_map.ParticleObject("dat/effects/particles/inactive_save_point.lua", 41, 46);
	crystal_effect:SetObjectID(Map.object_supervisor:GenerateObjectID());
    crystal_effect:Stop(); -- Don't run it until the character heals itself
    Map:AddGroundObject(crystal_effect);

    -- trees, etc
    local map_trees = {
        { "Tree Small4", 8, 62 },
        { "Tree Tiny1", 81, 79 },
        { "Tree Tiny3", 81, 28 },
        { "Tree Small3", 83, 73 },
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
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        Map:AddGroundObject(object);
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
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(object);
    end

end

-- Special event references which destinations must be updated just before being called.
local move_next_to_hero_event = {}
local move_back_to_hero_event = {}
local orlinn_move_to_hero_event = {}

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- Triggered events
    event = vt_map.MapTransitionEvent("to forest cave 2", "dat/maps/layna_forest/layna_forest_cave2_map.lua",
                                       "dat/maps/layna_forest/layna_forest_cave2_script.lua", "from layna forest crystal");
    EventManager:RegisterEvent(event);

    -- Heal point
    event = vt_map.ScriptedEvent("heal point", "heal_party", "heal_done");
    EventManager:RegisterEvent(event);

    -- Wolf final fight
    event = vt_map.BattleEncounterEvent("Fenrir Battle");
    event:SetMusic("mus/accion-OGA-djsaryon.ogg");
    event:SetBackground("img/backdrops/battle/forest_background.png");
    event:AddEnemy(8, 512, 500);
    event:SetBoss(true);
    event:AddEventLinkAtEnd("Make the wolf disappear");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Make the wolf disappear", "make_wolf_invisible", "");
    event:AddEventLinkAtEnd("boss fight post-dialogue");
    EventManager:RegisterEvent(event);

    -- Start of ending dialogue.
    event = vt_map.ScriptedEvent("boss fight post-dialogue", "post_boss_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 50);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Map:Popstate()", "Map_PopState", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("kalya:SetCollision(ALL)", kalya_sprite, "Sprite_Collision_on", "");
    EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event = vt_map.PathMoveSpriteEvent("Kalya moves next to Bronann", kalya_sprite, 0, 0, false);
    move_next_to_hero_event:AddEventLinkAtEnd("first dialogue part");
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya looks at Orlinn");
    move_next_to_hero_event:AddEventLinkAtEnd("kalya:SetCollision(ALL)");
    EventManager:RegisterEvent(move_next_to_hero_event);

    event = vt_map.LookAtSpriteEvent("Kalya looks at Orlinn", kalya_sprite, orlinn);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Kalya looks at Bronann", kalya_sprite, hero);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Orlinn looks at Bronann", orlinn, hero);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks east", kalya_sprite, vt_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks west", kalya_sprite, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks north", kalya_sprite, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks south", kalya_sprite, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks east", hero, vt_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks west", hero, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks north", hero, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks south", hero, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks east", orlinn, vt_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks west", orlinn, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);


    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Orlinn!!");
    dialogue:AddLineEmote(text, kalya_sprite, "exclamation");
    text = vt_system.Translate("I can feel it, sis'. It's coming!");
    dialogue:AddLineEvent(text, orlinn, "", "Orlinn comes closer of the crystal spawn point");
    text = vt_system.Translate("Orlinn, stop!");
    dialogue:AddLineEmote(text, kalya_sprite, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("first dialogue part", dialogue);
    event:AddEventLinkAtEnd("Kalya runs to Orlinn");
    event:AddEventLinkAtEnd("Bronann runs to Orlinn");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Orlinn comes closer of the crystal spawn point", orlinn, 42, 54, false);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Bronann runs to Orlinn", hero, 40, 56, true);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Kalya runs to Orlinn", kalya_sprite, 44, 56, true);
    event:AddEventLinkAtEnd("First tremor");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("First tremor", "first_tremor", "");
    event:AddEventLinkAtEnd("second dialogue part");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("What's happening!?!");
    dialogue:AddLineEmote(text, kalya_sprite, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("second dialogue part", dialogue);
    event:AddEventLinkAtEnd("Kalya looks east 1", 500);
    event:AddEventLinkAtEnd("kalya:SetCollision(NONE)");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks east 1", kalya_sprite, vt_map.MapMode.EAST);
    event:AddEventLinkAtEnd("Bronann looks west 1", 200);
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks west 1", hero, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Kalya looks west 1", 800);
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks west 1", kalya_sprite, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Bronann looks east 1", 800);
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks east 1", hero, vt_map.MapMode.EAST);
    event:AddEventLinkAtEnd("Bronann looks north", 1000);
    event:AddEventLinkAtEnd("Kalya looks north", 1000);
    event:AddEventLinkAtEnd("Both are surprised", 1000);
    event:AddEventLinkAtEnd("Set Camera on crystal", 1000);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Set Camera on crystal", crystal, "SetCamera", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Both are surprised", "bronann_kalya_exclamation", "");
    event:AddEventLinkAtEnd("Make crystal appear");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Make crystal appear", "make_crystal_appear", "make_crystal_appear_update");
    event:AddEventLinkAtEnd("third dialogue part", 1000);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Gosh!! By all heavens, what is this?");
    dialogue:AddLineEmote(text, kalya_sprite, "sweat drop");
    text = vt_system.Translate("A crystal stone?!?");
    dialogue:AddLineEmote(text, hero, "exclamation");
    text = vt_system.Translate("This is her. I heard her voice calling...");
    dialogue:AddLine(text, orlinn);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("third dialogue part", dialogue);
    event:AddEventLinkAtEnd("crystal dialogue part");
    EventManager:RegisterEvent(event);

    event = vt_map.SoundEvent("crystal voice sound 1", "dat/maps/layna_forest/crystal_appearance/crystal-sentence1.ogg")
    EventManager:RegisterEvent(event);
    event = vt_map.SoundEvent("crystal voice sound 2", "dat/maps/layna_forest/crystal_appearance/crystal-sentence2.ogg")
    EventManager:RegisterEvent(event);
    event = vt_map.SoundEvent("crystal voice sound 3", "dat/maps/layna_forest/crystal_appearance/crystal-sentence3.ogg")
    EventManager:RegisterEvent(event);
    event = vt_map.SoundEvent("crystal voice sound 4", "dat/maps/layna_forest/crystal_appearance/crystal-sentence4.ogg")
    EventManager:RegisterEvent(event);
    event = vt_map.SoundEvent("crystal voice sound 5", "dat/maps/layna_forest/crystal_appearance/crystal-sentence5.ogg")
    EventManager:RegisterEvent(event);
    event = vt_map.SoundEvent("crystal voice sound 6", "dat/maps/layna_forest/crystal_appearance/crystal-sentence6.ogg")
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    dialogue:SetInputBlocked(true);
    text = vt_system.Translate("Dear chosen one, the time has finally come.");
    dialogue:AddLineTimedEvent(text, crystal, 5000, "crystal voice sound 1", "");
    text = vt_system.Translate("For us, it will take an instant.");
    dialogue:AddLineTimedEvent(text, crystal, 5000, "crystal voice sound 2", "");
    text = vt_system.Translate("For you, it might have been decades.");
    dialogue:AddLineTimedEvent(text, crystal, 5000, "crystal voice sound 3", "");
    text = vt_system.Translate("May you bring a happy end to this foolish destiny of ours.");
    dialogue:AddLineTimedEvent(text, crystal, 6800, "crystal voice sound 4", "");
    text = vt_system.Translate("We're all anxiously awaiting your return.");
    dialogue:AddLineTimedEvent(text, crystal, 5000, "crystal voice sound 5", "");
    text = vt_system.Translate("Now, come. And make one with our holy hope.");
    dialogue:AddLineTimedEvent(text, crystal, 5000, "crystal voice sound 6", "");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("crystal dialogue part", dialogue);
    event:AddEventLinkAtEnd("fourth dialogue part");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("What does all of this mean?");
    dialogue:AddLineEmote(text, kalya_sprite, "thinking dots");
    text = vt_system.Translate("I shall come and make one with our only hope...");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("What! No, Orlinn!");
    dialogue:AddLineEmote(text, hero, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("fourth dialogue part", dialogue);
    event:AddEventLinkAtEnd("Orlinn comes even closer of the crystal");
    event:AddEventLinkAtEnd("Bronann runs in front of Orlinn", 300);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Orlinn comes even closer of the crystal", orlinn, 42, 50, false);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Bronann runs in front of Orlinn", hero, 42, 48, true);
    event:AddEventLinkAtEnd("Bronann looks south");
    event:AddEventLinkAtEnd("fifth dialogue part");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Bronann, the crystal! No!");
    dialogue:AddLineEmote(text, kalya_sprite, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("fifth dialogue part", dialogue);
    event:AddEventLinkAtEnd("kalya:SetCollision(NONE)");
    event:AddEventLinkAtEnd("Bronann is hurt");
    event:AddEventLinkAtEnd("white flash");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("kalya:SetCollision(NONE)", kalya_sprite, "Sprite_Collision_off", "");
    EventManager:RegisterEvent(event);

    event = vt_map.AnimateSpriteEvent("Bronann is hurt", hero, "hurt", 1200);
    event:AddEventLinkAtEnd("Bronann is sleeping");
    EventManager:RegisterEvent(event);

    event = vt_map.AnimateSpriteEvent("Bronann is sleeping", hero, "sleeping", 0); -- 0 means forever
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("white flash", "white_flash", "white_flash_update");
    event:AddEventLinkAtEnd("sixth dialogue part");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Bronann!");
    dialogue:AddLineEmote(text, kalya_sprite, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("sixth dialogue part", dialogue);
    event:AddEventLinkAtEnd("Kalya runs to Bronann");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Kalya runs to Bronann", kalya_sprite, 39, 47, true);
    event:AddEventLinkAtEnd("Kalya looks east");
    event:AddEventLinkAtEnd("Kalya kneels", 300);
    event:AddEventLinkAtEnd("seventh dialogue part", 600);
    EventManager:RegisterEvent(event);

    event = vt_map.AnimateSpriteEvent("Kalya kneels", kalya_sprite, "kneeling", 0); -- 0 means forever
    EventManager:RegisterEvent(event);

    -- Simply stop the custom animation
    event = vt_map.ScriptedSpriteEvent("Kalya gets up", kalya_sprite, "Terminate_all_events", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Kalya and orlinn are surprised", "orlinn_kalya_exclamation", "");
    event:AddEventLinkAtEnd("Kalya looks east");
    event:AddEventLinkAtEnd("Bronann kneels");
    EventManager:RegisterEvent(event);

    event = vt_map.AnimateSpriteEvent("Bronann kneels", hero, "kneeling", 0); -- 0 means forever
    EventManager:RegisterEvent(event);

    -- Simply stop the custom animation
    event = vt_map.ScriptedSpriteEvent("Bronann gets up", hero, "Terminate_all_events", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Fade out music", "fade_out_music", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Play funny music", "play_funny_music", "");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Bronann! Are you all right? Answer me!!");
    dialogue:AddLineEventEmote(text, kalya_sprite, "", "Orlinn looks west", "exclamation");
    text = vt_system.Translate("Bronann!");
    dialogue:AddLineEventEmote(text, kalya_sprite, "", "Orlinn looks east", "exclamation");
    text = vt_system.Translate("Woah, where am I?");
    dialogue:AddLineEmote(text, orlinn, "interrogation");
    text = vt_system.Translate("Bronann!");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("Sis! What's the matter? What happened?");
    dialogue:AddLineEventEmote(text, orlinn, "Orlinn looks north", "Kalya gets up", "interrogation");
    text = vt_system.Translate("What happened?? ... Orlinn ... What have you done? ...");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Kalya looks south", "", "exclamation");
    text = vt_system.Translate("... My head ...");
    dialogue:AddLineEvent(text, hero, "", "Kalya and orlinn are surprised");
    text = vt_system.Translate("Bronann! Are you alright?");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Kalya looks east", "", "exclamation");
    text = vt_system.Translate("I... I guess so. ...");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("... The wolf! The crystal! Orlinn! Are you ok?? ...");
    dialogue:AddLineEventEmote(text, hero, "Bronann gets up", "", "exclamation");
    text = vt_system.Translate("Don't worry, I'm fine. Err, what crystal? What ... wolf??");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("... You won't be fine for long after I'm done with you!!");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Kalya looks south", "Fade out music", "thinking dots");
    text = vt_system.Translate("... Huh? Wait, I didn't do anything this time!!");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    text = vt_system.Translate("... Oh? You didn't??");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Play funny music", "", "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("seventh dialogue part", dialogue);
    event:AddEventLinkAtEnd("Kalya runs after Orlinn");
    event:AddEventLinkAtEnd("Orlinn runs to point 3");
    event:AddEventLinkAtEnd("Bronann sighs and think", 2000);
    EventManager:RegisterEvent(event);

    -- Kalya runs after Orlinn's loop
    event = vt_map.PathMoveSpriteEvent("Kalya runs after Orlinn", kalya_sprite, orlinn, true);
    event:AddEventLinkAtEnd("Kalya runs after Orlinn");
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Orlinn runs to point 1", orlinn, 32, 38, true);
    event:AddEventLinkAtEnd("Orlinn runs to point 2");
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Orlinn runs to point 2", orlinn, 32, 45, true);
    event:AddEventLinkAtEnd("Orlinn runs to point 3");
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Orlinn runs to point 3", orlinn, 50, 45, true);
    event:AddEventLinkAtEnd("Orlinn runs to point 4");
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Orlinn runs to point 4", orlinn, 50, 38, true);
    event:AddEventLinkAtEnd("Orlinn runs to point 1");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Kalya stops running", kalya_sprite, "Terminate_all_events", "");
    EventManager:RegisterEvent(event);

    event = vt_map.AnimateSpriteEvent("Bronann sighs", hero, "hero_stance", 6000);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Kalya comes back next to Bronann", kalya_sprite, 44, 49, true);
    event:AddEventLinkAtEnd("Kalya looks at Bronann");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("When I'll catch you, I'll make you wish you weren't born!! Rahh!");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("Yiek!!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Come here, now!!!");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("Yiek!!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("(Sigh)... I should have stayed home today...");
    dialogue:AddLineEvent(text, hero, "Bronann sighs", "");
    text = vt_system.Translate("... Ouch, why is my head hurting so much? ...");
    dialogue:AddLineEvent(text, hero, "", "Kalya stops running");
    text = vt_system.Translate("Are you ok? We should get back home so Lilly can examine you...");
    dialogue:AddLineEvent(text, kalya_sprite, "Kalya comes back next to Bronann", "");
    text = vt_system.Translate("I still really feel dizzy but I'll be ok...");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("I'm relieved Orlinn is fine, too. And speaking of that: Orlinn! Stop running and come here, right away!");
    dialogue:AddLine(text, kalya_sprite);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Bronann sighs and think", dialogue);
    event:AddEventLinkAtEnd("Orlinn stops running");
    event:AddEventLinkAtEnd("Last dialogue");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Orlinn stops running", orlinn, "Terminate_all_events", "");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Orlinn comes back next to Bronann", orlinn, 40, 49, true);
    event:AddEventLinkAtEnd("Orlinn looks at Bronann");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Play wind music", "play_wind_music", "");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Owww... Alright.");
    dialogue:AddLineEventEmote(text, orlinn, "Fade out music", "Orlinn comes back next to Bronann", "sweat drop");
    text = vt_system.Translate("Now, Orlinn. Tell us: Why on earth did you go into the forest alone, and how did you just get there so quickly?");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("I don't know...");
    dialogue:AddLineEmote(text, orlinn, "interrogation");
    text = vt_system.Translate("Why was that giant wolf protecting you?");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("What was that crystal? Why did you want to make one with it?");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("I... I... I sincerely don't know... I swear! I was like... dreaming...");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    text = vt_system.Translate("I merely remember a voice in my head, telling me to come...");
    dialogue:AddLineEmote(text, orlinn, "thinking dots");
    text = vt_system.Translate("You know you won't get away with this, when you'll see Herth...");
    dialogue:AddLineEmote(text, kalya_sprite, "thinking dots");
    text = vt_system.Translate("I swear it's the truth!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    text = vt_system.Translate("Making one with the crystal, huh?");
    dialogue:AddLineEmote(text, hero, "sweat drop");
    text = vt_system.Translate("Let's not panic, Bronann, you look fine... Anyway, we'd better get back to the village and see the Elders as soon as possible.");
    dialogue:AddLineEventEmote(text, kalya_sprite, "", "Play wind music", "thinking dots");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Last dialogue", dialogue);
    event:AddEventLinkAtEnd("kalya goes back to party");
    event:AddEventLinkAtEnd("orlinn goes back to party");
    EventManager:RegisterEvent(event);

    -- for later
    move_back_to_hero_event = vt_map.PathMoveSpriteEvent("kalya goes back to party", kalya_sprite, hero, false);
    move_back_to_hero_event:AddEventLinkAtEnd("end of crystal event");
    EventManager:RegisterEvent(move_back_to_hero_event);

    orlinn_move_to_hero_event = vt_map.PathMoveSpriteEvent("orlinn goes back to party", orlinn, hero, false);
    EventManager:RegisterEvent(orlinn_move_to_hero_event);

    event = vt_map.ScriptedEvent("end of crystal event", "end_of_crystal_event", "");
    event:AddEventLinkAtEnd("Map:Popstate()");
    EventManager:RegisterEvent(event);
end

-- zones
local to_forest_cave2_zone = {};
local wolf_battle_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_forest_cave2_zone = vt_map.CameraZone(28, 33, 23, 25);
    Map:AddZone(to_forest_cave2_zone);

    wolf_battle_zone = vt_map.CameraZone(38, 46, 63, 66);
    Map:AddZone(wolf_battle_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_forest_cave2_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest cave 2");
    elseif (wolf_battle_zone:IsCameraEntering() == true) then
        if (GlobalManager:DoesEventExist("story", "Fenrir beaten") == false) then
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
local crystal_light_effect = {};

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
        GlobalManager:GetActiveParty():AddHitPoints(10000);
        GlobalManager:GetActiveParty():AddSkillPoints(10000);
        Map:SetStamina(10000);
        AudioManager:PlaySound("snd/heal_spell.wav");
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

        GlobalManager:SetEventValue("story", "Fenrir beaten", 1);
    end,

    post_boss_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        hero:SetDirection(vt_map.MapMode.NORTH);
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya_sprite:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        kalya_sprite:SetVisible(true);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        move_next_to_hero_event:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition(), false);
    end,

    first_tremor = function()
        Map:GetEffectSupervisor():ShakeScreen(1.0, 2000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_SUDDEN);
        AudioManager:PlaySound("snd/rumble.wav");
    end,

    bronann_kalya_exclamation = function()
        hero:Emote("exclamation", vt_map.MapMode.ANIM_NORTH);
        kalya_sprite:Emote("exclamation", vt_map.MapMode.ANIM_NORTH);
        AudioManager:PlaySound("snd/rumble.wav");
    end,

    make_crystal_appear = function()
        GlobalManager:SetEventValue("story", "layna_forest_crystal_appearance", 1);
    end,

    make_crystal_appear_update = function()
        if (GlobalManager:GetEventValue("story", "layna_forest_crystal_appearance") == 0) then
            return true;
        end

        crystal_appearance_time = crystal_appearance_time + SystemManager:GetUpdateTime();

        if (crystal_visible == false and crystal_appearance_time >= 10000) then
            -- Add a light upon the crystal
            crystal_light_effect = vt_map.Light("img/misc/lights/sun_flare_light_secondary.lua",
                    "img/misc/lights/sun_flare_light_secondary.lua",
                    41.2, 43.0,
                    vt_video.Color(0.8, 0.8, 1.0, 0.3),
                    vt_video.Color(0.8, 0.8, 0.85, 0.2));
            Map:AddLight(crystal_light_effect);
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
                AudioManager:PlaySound("snd/crystal_chime.wav");
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
        EventManager:TerminateAllEvents(sprite);
    end,

    orlinn_kalya_exclamation = function()
        orlinn:Emote("exclamation", vt_map.MapMode.ANIM_NORTH);
        kalya_sprite:Emote("exclamation", vt_map.MapMode.ANIM_NORTH);

        -- Restore also their collision mask
        orlinn:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        -- And prepare for the funny scene
        orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED - 7.0);
    end,

    fade_out_music = function()
        AudioManager:FadeOutAllMusic(2000);
    end,

    play_funny_music = function()
        AudioManager:PlayMusic("mus/Zander Noriega - School of Quirks.ogg");
    end,

    play_wind_music = function()
        AudioManager:PlayMusic("snd/wind.ogg");
    end,

    end_of_crystal_event = function()
        kalya_sprite:SetPosition(0, 0);
        kalya_sprite:SetVisible(false);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);
        Map:SetCamera(hero, 800);

        -- Set event as done
        GlobalManager:SetEventValue("story", "layna_forest_crystal_event_done", 1);
        -- Start the twilight
        _HandleTwilight();
    end
}
