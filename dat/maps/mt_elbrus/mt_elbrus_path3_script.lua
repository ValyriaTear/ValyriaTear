-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_path3_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "img/menus/locations/mt_elbrus.png"
map_subname = "Old Cemetery"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/awareness_el_corleo.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- the main character handler
local hero = {};

-- Forest dialogue secondary hero
local kalya = {};
local orlinn = {};

-- Name of the main sprite. Used to reload the good one at the end of dialogue events.
local main_sprite_name = "";

local harlequin_battle_done = false;

-- Soldiers
local soldier1 = {};
local soldier2 = {};
local soldier3 = {};

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

    Map.unlimited_stamina = false;

    _CreateCharacters();
    _CreateObjects();
    -- Called after the gates are closed
    --_CreateEnemies();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");

    -- Enables thunder
    GlobalManager:SetEventValue("story", "mt_elbrus_weather_level", 2)

    -- Make the rain starts or the corresponding dialogue according the need
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 0) then
        Map:GetParticleManager():AddParticleEffect("dat/effects/particles/rain.lua", 512.0, 768.0);
        -- Place an omni ambient sound at the center of the map to add a nice rainy effect.
        local rainy_sound = vt_map.SoundObject("mus/Ove Melaa - Rainy.ogg", 20.0, 16.0, 100.0);
        Map:AddAmbientSoundObject(rainy_sound);
    end
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 1) then
        Map:GetScriptSupervisor():AddScript("dat/maps/common/soft_lightnings_script.lua");
    end

    -- Preload special sounds
    AudioManager:LoadSound("snd/crystal_chime.wav", Map);
    AudioManager:LoadSound("snd/opening_sword_unsheathe.wav", Map);
    AudioManager:LoadMusic("mus/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg", Map);

    -- Check the map state according to the story state
    harlequin_battle_done = false;
    if (GlobalManager:GetEventValue("story", "mt_elbrus_cemetery_fight_done") == 1) then
        -- Open the north gate, close the south ones. (but no sound)
        _CloseSouthGate();
        _OpenNorthGate();
        harlequin_battle_done = true;
    end

end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
    -- Check the first half battle part
    if (harlequin_battle_done == false) then
        _CheckHarlequinsStatus()
    end
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from mountain path 2)
    hero = CreateSprite(Map, "Bronann", 63, 92.5);
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
    elseif (GlobalManager:GetPreviousLocation() == "from_path2_chest") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(10.0, 90.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_high_mountain") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(50.0, 10.0);
    end

    Map:AddGroundObject(hero);

    -- Create secondary characters
    kalya = CreateSprite(Map, "Kalya",
                         hero:GetXPosition(), hero:GetYPosition());
    kalya:SetDirection(vt_map.MapMode.EAST);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);
    Map:AddGroundObject(kalya);

    orlinn = CreateSprite(Map, "Orlinn",
                          hero:GetXPosition(), hero:GetYPosition());
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);
    Map:AddGroundObject(orlinn);

    soldier1 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 0, 0);
    soldier1:SetVisible(false);
    soldier1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(soldier1);
    soldier2 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 0, 0);
    soldier2:SetVisible(false);
    soldier2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(soldier2);
    soldier3 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 0, 0);
    soldier3:SetVisible(false);
    soldier3:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(soldier3);
end

-- The heal particle effect map object
local heal_effect = {};

local north_gate_closed = {}
local south_gate1_closed = {}
local south_gate2_closed = {}
local south_gate3_closed = {}
local north_gate_open = {}
local south_gate1_open = {}
local south_gate2_open = {}
local south_gate3_open = {}
local west_gate_stone1 = {}
local west_gate_stone2 = {}
local west_gate_stone3 = {}
local west_gate_stone4 = {}

local harlequin1 = {}
local harlequin2 = {}
local harlequin3 = {}
local harlequin_focus = {}

local harlequin_beaten_time = 0;

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}

    Map:AddSavePoint(85, 81);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject("dat/effects/particles/heal_particle.lua", 0, 0);
    heal_effect:SetObjectID(Map.object_supervisor:GenerateObjectID());
    heal_effect:Stop(); -- Don't run it until the character heals itself
    Map:AddGroundObject(heal_effect);

    -- Heal point
    npc = CreateSprite(Map, "Butterfly", 60, 70);
    npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    npc:SetVisible(false);
    npc:SetName(""); -- Unset the speaker name
    Map:AddGroundObject(npc);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Your party feels better...");
    dialogue:AddLineEvent(text, npc, "Heal event", "");
    DialogueManager:AddDialogue(dialogue);
    npc:AddDialogueReference(dialogue);
    npc = CreateObject(Map, "Layna Statue", 60, 70);
    Map:AddGroundObject(npc);

    -- Cemetery gates
    north_gate_closed = CreateObject(Map, "Gate1 closed", 51, 16);
    Map:AddGroundObject(north_gate_closed);
    north_gate_open = CreateObject(Map, "Gate1 open", 51, 12);
    north_gate_open:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    north_gate_open:SetVisible(false);
    north_gate_open:SetDrawOnSecondPass(true); -- Above the character
    Map:AddGroundObject(north_gate_open);

    -- The south gate is open at the beginning of the map.
    south_gate1_closed = CreateObject(Map, "Gate1 closed", 0, 0); -- 63, 68
    Map:AddGroundObject(south_gate1_closed);
    south_gate2_closed = CreateObject(Map, "Gate1 closed", 0, 0); -- 67, 68
    Map:AddGroundObject(south_gate2_closed);
    south_gate3_closed = CreateObject(Map, "Gate1 closed", 0, 0); -- 71, 68
    Map:AddGroundObject(south_gate3_closed);

    south_gate1_open = CreateObject(Map, "Gate1 open", 63, 64);
    south_gate1_open:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    south_gate1_open:SetDrawOnSecondPass(true); -- Above the character
    Map:AddGroundObject(south_gate1_open);
    south_gate2_open = CreateObject(Map, "Gate1 open", 67, 64);
    south_gate2_open:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    south_gate2_open:SetDrawOnSecondPass(true); -- Above the character
    Map:AddGroundObject(south_gate2_open);
    south_gate3_open = CreateObject(Map, "Gate1 open", 71, 64);
    south_gate3_open:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    south_gate3_open:SetDrawOnSecondPass(true); -- Above the character
    Map:AddGroundObject(south_gate3_open);

    west_gate_stone1 = CreateObject(Map, "Rock2", 41, 18);
    Map:AddGroundObject(west_gate_stone1);
    west_gate_stone2 = CreateObject(Map, "Rock2", 41, 20);
    Map:AddGroundObject(west_gate_stone2);
    west_gate_stone3 = CreateObject(Map, "Rock2", 41, 22);
    Map:AddGroundObject(west_gate_stone3);
    west_gate_stone4 = CreateObject(Map, "Rock2", 41, 24);
    Map:AddGroundObject(west_gate_stone4);

    harlequin1 = CreateObject(Map, "Harlequin", 0, 0);
    harlequin1:SetEventWhenTalking("Make Harlequin1 disappear");
    Map:AddGroundObject(harlequin1);
    harlequin2 = CreateObject(Map, "Harlequin", 0, 0);
    harlequin2:SetEventWhenTalking("Make Harlequin2 disappear");
    Map:AddGroundObject(harlequin2);
    harlequin3 = CreateObject(Map, "Harlequin", 0, 0);
    harlequin3:SetEventWhenTalking("Make Harlequin3 disappear");
    Map:AddGroundObject(harlequin3);
    -- Harlequin must be beaten third times before being actually fighteable
    harlequin_beaten_time = 0;

    event = vt_map.ScriptedEvent("Make Harlequin1 disappear", "make_harlequin1_disappear", "");
    event:AddEventLinkAtEnd("Fake Harlequin battle");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Make Harlequin2 disappear", "make_harlequin2_disappear", "");
    event:AddEventLinkAtEnd("Fake Harlequin battle");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Make Harlequin3 disappear", "make_harlequin3_disappear", "");
    event:AddEventLinkAtEnd("Fake Harlequin battle");
    EventManager:RegisterEvent(event);

    event = vt_map.BattleEncounterEvent("Fake Harlequin battle");
    event:AddEnemy(13, 512, 484); -- Harlequin
    event:AddEnemy(12, 470, 384); -- Eyeballs
    event:AddEnemy(12, 380, 500); -- Eyeballs
    event:AddEnemy(12, 650, 484); -- Eyeballs
    event:AddEnemy(12, 570, 584); -- Eyeballs
    event:AddEnemy(12, 450, 624); -- Eyeballs
    _SetEventBattleEnvironment(event);
    event:AddEventLinkAtEnd("Increase Harlequin beaten time");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Increase Harlequin beaten time", "increase_harlequin_beaten_time", "");
    EventManager:RegisterEvent(event);

    --harlequin virtual focus
    harlequin_focus = CreateSprite(Map, "Butterfly", 68, 24);
    harlequin_focus:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    harlequin_focus:SetVisible(false);
    harlequin_focus:SetName(vt_system.Translate("Harlequin")); -- Set the corresponding name
    Map:AddGroundObject(harlequin_focus);

    -- Objects array
    local map_objects = {

        { "Tree Big2", 91, 87 },
        { "Tree Big1", 90, 75 },
        { "Tree Big2", 77, 72 },
        { "Tree Big2", 80, 77 },
        { "Tree Big2", 83, 85 },

        { "Tree Small1", 67, 78 },
        { "Tree Big2", 60, 90 },

        { "Tree Big2", 50, 80 },
        { "Tree Big2", 55, 74.5 },
        { "Tree Big2", 42, 75 },
        { "Tree Big2", 38, 43 },

        { "Tree Tiny1", 74, 74 },
        { "Tree Tiny2", 74, 77 },
        { "Tree Tiny3", 74, 80 },
        { "Tree Tiny3", 74, 90 },
        { "Tree Tiny2", 73, 93 },

        { "Tree Tiny4", 55, 90 },
        { "Tree Tiny2", 57, 87 },
        { "Tree Tiny3", 59, 76 },
        { "Tree Tiny1", 59, 73 },


        { "Rock2", 73, 75 },
        { "Rock2", 73, 78 },
        { "Rock2", 73, 81 },
        { "Rock2", 73, 88 },
        { "Rock2", 72, 91 },

        { "Rock1", 54, 91 },
        { "Rock2", 57, 88 },
        { "Rock2", 58, 85 },
        { "Rock2", 60, 74 },

        { "Rock1", 78, 89 },
        { "Rock1", 84, 86 },
        { "Rock1", 81, 70 },
        { "Rock1", 49, 86 },
        { "Rock1", 44, 78 },
        { "Rock1", 53, 70 },
        { "Rock1", 10, 48 },
        { "Rock1", 15, 54 },
        { "Rock1", 9, 65 },
        { "Rock1", 23, 63 },
        { "Rock1", 33, 54 },
        { "Rock1", 5, 35 },
        { "Rock1", 2, 37 },

        -- blocking east cemetery exit
        { "Rock2", 93, 32 },
        { "Rock2", 93, 34 },
        { "Rock2", 93, 36 },
        { "Rock2", 93, 38 },
        { "Rock2", 93, 40 },
        { "Rock2", 95, 32 },
        { "Rock2", 95, 34 },
        { "Rock2", 95, 36 },
        { "Rock2", 95, 38 },
        { "Rock2", 95, 40 },

        { "Rock2", 59, 19 },
        { "Rock2", 77, 19 },
        { "Rock2", 71, 12 },
        { "Rock2", 79, 12 },

        -- The steles
        { "Stele1", 50, 52 },
        { "Stele1", 54, 52 },
        { "Stele1", 58, 52 },
        { "Stele1", 62, 52 },
        { "Stele1", 50, 44 },
        { "Stele1", 54, 44 },
        { "Stele1", 58, 44 },
        { "Stele1", 62, 44 },
        { "Stele1", 50, 36 },
        { "Stele1", 54, 36 },
        { "Stele1", 58, 36 },
        { "Stele1", 62, 36 },
        { "Stele1", 50, 28 },
        { "Stele1", 54, 28 },
        { "Stele1", 58, 28 },
        { "Stele1", 62, 28 },

        { "Stele1", 74, 28 },
        { "Stele1", 78, 28 },
        { "Stele1", 82, 28 },
        { "Stele1", 86, 28 },
        { "Stele1", 74, 36 },
        { "Stele1", 78, 36 },
        { "Stele1", 82, 36 },
        { "Stele1", 86, 36 },
        { "Stele1", 74, 44 },
        { "Stele1", 78, 44 },
        { "Stele1", 82, 44 },
        { "Stele1", 86, 44 },
        { "Stele1", 74, 52 },
        { "Stele1", 78, 52 },
        { "Stele1", 82, 52 },
        { "Stele1", 86, 52 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_objects) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        Map:AddGroundObject(object);
    end

    -- grass array
    local map_grass = {
        --  right border
        { "Grass Clump1", 59.5, 71 },
        { "Grass Clump1", 91, 76 },
        { "Grass Clump1", 42, 69 },
        { "Grass Clump1", 95, 69.2 },
        { "Grass Clump1", 85, 87 },
        { "Grass Clump1", 73, 69 },
        { "Grass Clump1", 70, 76 },
        { "Grass Clump1", 64, 72 },
        { "Grass Clump1", 63, 86 },
        { "Grass Clump1", 53, 87.5 },
        { "Grass Clump1", 40, 71.5 },
        { "Grass Clump1", 37, 53 },
        { "Grass Clump1", 31, 52 },
        { "Grass Clump1", 52, 73 },
        { "Grass Clump1", 28, 43 },
        { "Grass Clump1", 15.5, 37 },
        { "Grass Clump1", 9, 38 },
        { "Grass Clump1", 11, 49 },
        { "Grass Clump1", 14, 56 },
        { "Grass Clump1", 5.5, 81 },
        { "Grass Clump1", 13, 78 },
        { "Grass Clump1", 86, 11 },
        { "Grass Clump1", 89, 11.2 },

    }

    -- Loads the grass clumps according to the array
    for my_index, my_array in pairs(map_grass) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(object);
    end

end

-- A function closing the south cemetery gate
function _CloseSouthGate()
    south_gate1_closed:SetPosition(63, 68);
    south_gate2_closed:SetPosition(67, 68);
    south_gate3_closed:SetPosition(71, 68);

    south_gate1_open:SetVisible(false);
    south_gate2_open:SetVisible(false);
    south_gate3_open:SetVisible(false);
end

-- A function opening the north cemetery gate
function _OpenNorthGate()
    north_gate_open:SetVisible(true);
    north_gate_closed:SetPosition(0, 0);
end

-- Enemy zones later disabled
local enemy_zone1 = {}
local enemy_zone2 = {}
local enemy_zone3 = {}

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

    -- Hint: left, right, top, bottom
    enemy_zone1 = vt_map.EnemyZone(65, 68, 32, 36);
    -- Some bats
    enemy = CreateEnemySprite(Map, "Eyeball");
    _SetBattleEnvironment(enemy);
    -- Adds a quicker respawn time
    enemy:SetTimeToRespawn(3000)
    enemy:NewEnemyParty();
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy_zone1:AddEnemy(enemy, Map, 2);
    enemy_zone1:SetSpawnsLeft(2); -- This monster shall spawn only two times.
    Map:AddZone(enemy_zone1);

    -- Hint: left, right, top, bottom
    enemy_zone2 = vt_map.EnemyZone(45, 48, 32, 36);
    -- Some bats
    enemy = CreateEnemySprite(Map, "Eyeball");
    _SetBattleEnvironment(enemy);
    -- Adds a quicker respawn time
    enemy:SetTimeToRespawn(3000)
    enemy:NewEnemyParty();
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy_zone2:AddEnemy(enemy, Map, 2);
    enemy_zone2:SetSpawnsLeft(2); -- This monster shall spawn only two times.
    Map:AddZone(enemy_zone2);

    -- Hint: left, right, top, bottom
    enemy_zone3 = vt_map.EnemyZone(87, 90, 32, 36);
    -- Some bats
    enemy = CreateEnemySprite(Map, "Eyeball");
    _SetBattleEnvironment(enemy);
    -- Adds a quicker respawn time
    enemy:SetTimeToRespawn(3000)
    enemy:NewEnemyParty();
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy_zone3:AddEnemy(enemy, Map, 2);
    enemy_zone3:SetSpawnsLeft(2); -- This monster shall spawn only two times.
    Map:AddZone(enemy_zone3);
end

-- Special event references which destinations must be updated just before being called.
local kalya_move_next_to_hero_event1 = {}
local kalya_move_back_to_hero_event1 = {}
local orlinn_move_next_to_hero_event1 = {}
local orlinn_move_back_to_hero_event1 = {}

local kalya_move_next_to_hero_event2 = {}
local kalya_move_back_to_hero_event2 = {}
local orlinn_move_next_to_hero_event2 = {}
local orlinn_move_back_to_hero_event2 = {}

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to mountain path 2", "dat/maps/mt_elbrus/mt_elbrus_path2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path2_script.lua", "from_path3");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain path 2bis", "dat/maps/mt_elbrus/mt_elbrus_path2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path2_script.lua", "from_path3_chest");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain path 4", "dat/maps/mt_elbrus/mt_elbrus_path4_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path4_script.lua", "from_path3");
    EventManager:RegisterEvent(event);

    -- Heal point
    event = vt_map.ScriptedEvent("Heal event", "heal_party", "heal_done");
    EventManager:RegisterEvent(event);

    -- sprite direction events
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks north", hero, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks south", hero, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks south", kalya, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks west", kalya, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Kalya looks at Bronann", kalya, hero);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Orlinn looks at Kalya", orlinn, kalya);
    EventManager:RegisterEvent(event);

    -- cemetery entrance scene
    event = vt_map.ScriptedEvent("Set scene state for dialogue about cemetery entrance", "set_scene_state", "");
    event:AddEventLinkAtEnd("The hero moves to a good watch point");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("The hero moves to a good watch point", hero, 67, 73, false);
    event:AddEventLinkAtEnd("Kalya tells about the cemetery");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Kalya tells about the cemetery", "kalya_cemetery_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann1", 100);
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann1", 100);
    EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_hero_event1 = vt_map.PathMoveSpriteEvent("Kalya moves next to Bronann1", kalya, 0, 0, false);
    kalya_move_next_to_hero_event1:AddEventLinkAtEnd("Kalya looks north");
    kalya_move_next_to_hero_event1:AddEventLinkAtEnd("Bronann looks north");
    kalya_move_next_to_hero_event1:AddEventLinkAtEnd("Kalya talks about the cemetery");
    EventManager:RegisterEvent(kalya_move_next_to_hero_event1);
    orlinn_move_next_to_hero_event1 = vt_map.PathMoveSpriteEvent("Orlinn moves next to Bronann1", orlinn, 0, 0, false);
    orlinn_move_next_to_hero_event1:AddEventLinkAtEnd("Orlinn looks north");
    EventManager:RegisterEvent(orlinn_move_next_to_hero_event1);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("This place is the village old cemetery, used when the former villagers lived in Layna...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("The former villagers? What do you mean?");
    dialogue:AddLineEmote(text, hero, "exclamation");
    text = vt_system.Translate("Well, the Layna Village was abandoned a long time ago, before your parents and all the others came living there... Your mother never told you that?");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at Bronann", "", "thinking dots");
    text = vt_system.Translate("I've never been further alone before... This place gives me the chill...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks north", "");
    text = vt_system.Translate("Yiek!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Anyway, once the cemetery is behind us, we should be out of trouble!");
    dialogue:AddLine(text, kalya);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya talks about the cemetery", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes back to party");
    event:AddEventLinkAtEnd("Kalya goes back to party");
    EventManager:RegisterEvent(event);

    orlinn_move_back_to_hero_event1 = vt_map.PathMoveSpriteEvent("Orlinn goes back to party", orlinn, hero, false);
    EventManager:RegisterEvent(orlinn_move_back_to_hero_event1);

    kalya_move_back_to_hero_event1 = vt_map.PathMoveSpriteEvent("Kalya goes back to party", kalya, hero, false);
    kalya_move_back_to_hero_event1:AddEventLinkAtEnd("End of dialogue about the cemetery");
    EventManager:RegisterEvent(kalya_move_back_to_hero_event1);

    event = vt_map.ScriptedEvent("End of dialogue about the cemetery", "end_of_dialogue_about_cemetery", "");
    EventManager:RegisterEvent(event);

    -- West gate dialogue
    -- ------------------
    event = vt_map.ScriptedEvent("Set scene state for dialogue about west gate", "set_scene_state", "");
    event:AddEventLinkAtEnd("The hero notices the soldiers");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Isn't that soldiers up there?");
    dialogue:AddLineEventEmote(text, hero, "Bronann looks north", "", "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("The hero notices the soldiers", dialogue);
    event:AddEventLinkAtEnd("West gate - The hero moves to a good watch point");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("West gate - The hero moves to a good watch point", hero, 16, 40, true);
    event:AddEventLinkAtEnd("Kalya tells about the soldiers");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Kalya tells about the soldiers", "kalya_west_gate_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann2", 100);
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann2", 100);
    EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_hero_event2 = vt_map.PathMoveSpriteEvent("Kalya moves next to Bronann2", kalya, 0, 0, false);
    kalya_move_next_to_hero_event2:AddEventLinkAtEnd("Kalya looks north");
    kalya_move_next_to_hero_event2:AddEventLinkAtEnd("Bronann looks north");
    kalya_move_next_to_hero_event2:AddEventLinkAtEnd("Set focus on soldiers");
    EventManager:RegisterEvent(kalya_move_next_to_hero_event2);
    orlinn_move_next_to_hero_event2 = vt_map.PathMoveSpriteEvent("Orlinn moves next to Bronann2", orlinn, 0, 0, false);
    orlinn_move_next_to_hero_event2:AddEventLinkAtEnd("Orlinn looks north");
    EventManager:RegisterEvent(orlinn_move_next_to_hero_event2);

    event = vt_map.ScriptedEvent("Set focus on soldiers", "set_focus_on_soldiers", "set_focus_update");
    event:AddEventLinkAtEnd("Soldiers dialogue");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("The west gates are condemned, as the Lord commanded.");
    dialogue:AddLine(text, soldier1);
    text = vt_system.Translate("Fine. Let's go back and wait for them.");
    dialogue:AddLine(text, soldier3);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Soldiers dialogue", dialogue);
    event:AddEventLinkAtEnd("Soldier1 moves out of map", 300);
    event:AddEventLinkAtEnd("Soldier2 moves out of map", 300);
    event:AddEventLinkAtEnd("Soldier3 moves out of map");
    event:AddEventLinkAtEnd("West gate - Set focus on hero", 400);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Soldier1 moves out of map", soldier1, 1, 25, false);
    event:AddEventLinkAtEnd("Make soldier1 disappear");
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Soldier2 moves out of map", soldier2, 1, 26, false);
    event:AddEventLinkAtEnd("Make soldier2 disappear");
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Soldier3 moves out of map", soldier3, 1, 27, false);
    event:AddEventLinkAtEnd("Make soldier3 disappear");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Make soldier1 disappear", soldier1, "make_object_disappear", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("Make soldier2 disappear", soldier2, "make_object_disappear", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("Make soldier3 disappear", soldier3, "make_object_disappear", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("West gate - Set focus on hero", "set_focus_back_on_hero", "set_focus_update");
    event:AddEventLinkAtEnd("Dialogue about the west gate");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("The gate to the great plains...");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("Have we got any other way to leave this place?");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("Only one left... The mountain top...");
    dialogue:AddLineEmote(text, kalya, "thinking dots");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Dialogue about the west gate", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes back to party 2");
    event:AddEventLinkAtEnd("Kalya goes back to party 2");
    EventManager:RegisterEvent(event);

    orlinn_move_back_to_hero_event2 = vt_map.PathMoveSpriteEvent("Orlinn goes back to party 2", orlinn, hero, false);
    EventManager:RegisterEvent(orlinn_move_back_to_hero_event2);

    kalya_move_back_to_hero_event2 = vt_map.PathMoveSpriteEvent("Kalya goes back to party 2", kalya, hero, false);
    kalya_move_back_to_hero_event2:AddEventLinkAtEnd("End of dialogue about west gate");
    EventManager:RegisterEvent(kalya_move_back_to_hero_event2);

    event = vt_map.ScriptedEvent("End of dialogue about west gate", "end_of_dialogue_about_west_gate", "");
    EventManager:RegisterEvent(event);


    -- trapped event!
    -- --------------
    event = vt_map.ScriptedEvent("Prepare trapped event", "set_scene_state", "");
    event:AddEventLinkAtEnd("The hero notices about the gate");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Oh no! the gate!");
    dialogue:AddLineEventEmote(text, hero, "Bronann looks south", "", "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("The hero notices about the gate", dialogue);
    event:AddEventLinkAtEnd("The hero rushes to the gate");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("The hero rushes to the gate", hero, 67, 66.5, true);
    event:AddEventLinkAtEnd("The hero is trapped");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("We're trapped!");
    dialogue:AddLine(text, hero);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("The hero is trapped", dialogue);
    event:AddEventLinkAtEnd("Harlequin talks to the hero");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("You're mine now!");
    dialogue:AddLine(text, harlequin_focus);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Harlequin talks to the hero", dialogue);
    event:AddEventLinkAtEnd("The hero is surprised");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("The hero is surprised", "hero_exclamation", "");
    event:AddEventLinkAtEnd("Bronann looks north");
    event:AddEventLinkAtEnd("Set the focus on Harlequin");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Set the focus on Harlequin", "set_focus_on_harlequin", "set_focus_update");
    event:AddEventLinkAtEnd("Harlequin talks to the hero 2");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("I shall bring your spirits to the Master.");
    dialogue:AddLine(text, harlequin_focus);
    text = vt_system.Translate("But let's play together first, shall we?");
    dialogue:AddLine(text, harlequin_focus);
    text = vt_system.Translate("Now catch me... if you can.");
    dialogue:AddLine(text, harlequin_focus);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Harlequin talks to the hero 2", dialogue);
    event:AddEventLinkAtEnd("Set the focus back on hero");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Set the focus back on hero", "set_focus_on_hero", "set_focus_update");
    event:AddEventLinkAtEnd("End of trapped Dialogue");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("End of trapped Dialogue", "end_of_trap_dialogue", "");
    event:AddEventLinkAtEnd("Make the Harlequins move");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Make the Harlequins move", "make_harlequins_move", "make_harlequins_move_update");
    event:AddEventLinkAtEnd("Make the Harlequins teleport");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Make the Harlequins teleport", "make_harlequins_teleport", "");
    event:AddEventLinkAtEnd("Make the Harlequins move");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Harlequin is fed up!", "place_harlequin_for_pre_boss_battle", "");
    event:AddEventLinkAtEnd("Harlequin talks to the hero 3");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Enough of this shallow game. Give me your souls now!...");
    dialogue:AddLineEvent(text, harlequin_focus, "Bronann looks north", "");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Harlequin talks to the hero 3", dialogue);
    event:AddEventLinkAtEnd("True Harlequin battle");
    EventManager:RegisterEvent(event);

    event = vt_map.BattleEncounterEvent("True Harlequin battle");
    event:SetBoss(true);
    event:AddEnemy(14, 512, 484); -- Harlequin
    event:AddEnemy(12, 470, 384); -- Eyeballs
    event:AddEnemy(12, 380, 500); -- Eyeballs
    event:AddEnemy(12, 650, 484); -- Eyeballs
    event:AddEnemy(12, 570, 584); -- Eyeballs
    event:AddEnemy(12, 450, 624); -- Eyeballs
    _SetEventBattleEnvironment(event);
    event:SetMusic("mus/accion-OGA-djsaryon.ogg"); --boss music
    event:AddEventLinkAtEnd("Harlequin talks to the hero 4");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("How could I be hurt by... children...");
    dialogue:AddLineEvent(text, harlequin_focus, "Bronann looks north", "");
    text = vt_system.Translate("We'll see each other again, Chosen One...");
    dialogue:AddLine(text, harlequin_focus);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Harlequin talks to the hero 4", dialogue);
    event:AddEventLinkAtEnd("Ends Harlequin battle");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Ends Harlequin battle", "ends_harlequin_battle1", "");
    event:AddEventLinkAtEnd("Ends Harlequin battle 2", 1200);
    EventManager:RegisterEvent(event);
    -- Adds a bit of time before opening the gate so that action sounds don't get mixed.
    event = vt_map.ScriptedEvent("Ends Harlequin battle 2", "ends_harlequin_battle2", "");
    EventManager:RegisterEvent(event);
end

-- zones
local to_path4_zone = {};
local to_path2_zone = {};
local to_path2_bis_zone = {};

local cemetery_entrance_dialogue_zone = {};
local cemetery_west_gate_dialogue_zone = {};
local cemetery_gates_closed_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_path4_zone = vt_map.CameraZone(40, 55, 0, 2);
    Map:AddZone(to_path4_zone);
    to_path2_zone = vt_map.CameraZone(53, 74, 94, 96);
    Map:AddZone(to_path2_zone);
    to_path2_bis_zone = vt_map.CameraZone(1, 23, 94, 96);
    Map:AddZone(to_path2_bis_zone);

    -- event zones
    cemetery_entrance_dialogue_zone = vt_map.CameraZone(61, 74, 71, 73);
    Map:AddZone(cemetery_entrance_dialogue_zone);
    cemetery_west_gate_dialogue_zone = vt_map.CameraZone(7, 31, 46, 48);
    Map:AddZone(cemetery_west_gate_dialogue_zone);
    -- cemetery gates closed
    cemetery_gates_closed_zone = vt_map.CameraZone(44, 92, 52, 54);
    Map:AddZone(cemetery_gates_closed_zone);

end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_path4_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain path 4");
    elseif (to_path2_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain path 2");
    elseif (to_path2_bis_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain path 2bis");
    elseif (cemetery_entrance_dialogue_zone:IsCameraEntering() == true and Map:CurrentState() ~= vt_map.MapMode.STATE_SCENE) then
        if (GlobalManager:GetEventValue("story", "mt_elbrus_kalya_cemetery_entrance_dialogue") == 0) then
            hero:SetMoving(false);
            EventManager:StartEvent("Set scene state for dialogue about cemetery entrance");
        end
    elseif (cemetery_west_gate_dialogue_zone:IsCameraEntering() == true and Map:CurrentState() ~= vt_map.MapMode.STATE_SCENE) then
        if (GlobalManager:GetEventValue("story", "mt_elbrus_kalya_west_gate_dialogue") == 0) then
            hero:SetMoving(false);
            EventManager:StartEvent("Set scene state for dialogue about west gate");
        end
    elseif (cemetery_gates_closed_zone:IsCameraEntering() == true and Map:CurrentState() ~= vt_map.MapMode.STATE_SCENE) then
        if (harlequin_battle_done == false and GlobalManager:GetEventValue("story", "mt_elbrus_cemetery_south_gate_closed") == 0) then
            hero:SetMoving(false);
            _CloseSouthGate();
            AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
            EventManager:StartEvent("Prepare trapped event");
        end
    end

end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    -- default values
    enemy:SetBattleMusicTheme("mus/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_background.png");
    enemy:AddBattleScript("dat/maps/common/at_night.lua");

    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 0) then
        enemy:AddBattleScript("dat/maps/common/rain_in_battles_script.lua");
    end
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 1) then
        enemy:AddBattleScript("dat/maps/common/soft_lightnings_script.lua");
    end
end
-- The environment for Harlequin's battles
function _SetEventBattleEnvironment(event)
    event:SetMusic("mus/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg");
    event:SetBackground("img/backdrops/battle/mountain_background.png");
    event:AddScript("dat/maps/common/at_night.lua");

    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 0) then
        event:AddScript("dat/maps/common/rain_in_battles_script.lua");
    end
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 1) then
        event:AddScript("dat/maps/common/soft_lightnings_script.lua");
    end
end

function _MakeHarlequinTeleport(harlequin)
    if (harlequin:IsVisible() == true) then
        local new_x = math.random(46.0, 89.0);
        local new_y = math.random(22.0, 55.0);
        harlequin:SetPosition(new_x, new_y);
    end
end

-- Check whether the three harlequin were beaten and then start the mini-boss ending.
function _CheckHarlequinsStatus()
    if (harlequin_beaten_time < 3) then
        return;
    end

    if (harlequin1:IsVisible() == false and harlequin2:IsVisible() == false and harlequin3:IsVisible() == false) then
        harlequin_beaten_time = 0;
        -- Stop the first part event
        EventManager:TerminateEvents("Make the Harlequins move", false)

        -- Remove the other monsters
        Map.object_supervisor:SetAllEnemyStatesToDead();
        enemy_zone1:SetEnabled(false);
        enemy_zone2:SetEnabled(false);
        enemy_zone3:SetEnabled(false);

        hero:SetMoving(false);
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        EventManager:StartEvent("Harlequin is fed up!", 1000);
    end
end

function _GetRandomDirectionDiff()
    local factor = 0;
    if (math.random(0, 1) == 1) then
        factor = 1.0;
    else
        factor = -1.0;
    end

    return ((factor * math.random(7, 12)) / 1000.0)
end

-- Map Custom functions
-- Used through scripted events

-- Effect time used when applying the heal light effect
local heal_effect_time = 0;
local heal_color = vt_video.Color(0.0, 0.0, 1.0, 1.0);

-- local members used to make the harlequins move
local h1_x_direction = 0.0;
local h1_y_direction = 0.0;
local h2_x_direction = 0.0;
local h2_y_direction = 0.0;
local h3_x_direction = 0.0;
local h3_y_direction = 0.0;
local total_time = 0;

map_functions = {

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

    set_scene_state = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

    kalya_cemetery_dialogue_start = function()
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        kalya:SetVisible(true);
        orlinn:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        orlinn:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        kalya_move_next_to_hero_event1:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition(), false);
        orlinn_move_next_to_hero_event1:SetDestination(hero:GetXPosition() - 2.0, hero:GetYPosition(), false);
    end,

    end_of_dialogue_about_cemetery = function()
        Map:PopState();
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);

        -- Set event as done
        GlobalManager:SetEventValue("story", "mt_elbrus_kalya_cemetery_entrance_dialogue", 1);
    end,

    kalya_west_gate_dialogue_start = function()
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        kalya:SetVisible(true);
        orlinn:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        orlinn:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        kalya_move_next_to_hero_event2:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition(), false);
        orlinn_move_next_to_hero_event2:SetDestination(hero:GetXPosition() - 2.0, hero:GetYPosition(), false);

        -- Place the soldiers
        soldier1:SetPosition(16, 25);
        soldier1:SetDirection(vt_map.MapMode.WEST)
        soldier1:SetVisible(true);
        soldier2:SetPosition(16, 27);
        soldier2:SetDirection(vt_map.MapMode.WEST)
        soldier2:SetVisible(true);
        soldier3:SetPosition(14, 26);
        soldier3:SetDirection(vt_map.MapMode.EAST)
        soldier3:SetVisible(true);
    end,

    set_focus_on_soldiers = function()
        Map:SetCamera(soldier1, 1200);
    end,

    make_object_disappear = function(object)
        object:SetVisible(false);
    end,

    set_focus_back_on_hero = function()
        Map:SetCamera(hero, 1500);
    end,

    end_of_dialogue_about_west_gate = function()
        Map:PopState();
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);

        -- Set event as done
        GlobalManager:SetEventValue("story", "mt_elbrus_kalya_west_gate_dialogue", 1);
    end,

    -- Trapped scripted events functions
    -- ---------------------------------
    hero_exclamation = function()
        hero:Emote("exclamation", hero:GetDirection());
        AudioManager:FadeOutAllMusic(1000);
    end,

    set_focus_on_harlequin = function()
        Map:SetCamera(harlequin_focus, 1200);
        harlequin1:SetPosition(68, 24);
        harlequin1:SetVisible(true);
    end,
    set_focus_update = function()
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        return true;
    end,

    set_focus_on_hero = function()
        Map:SetCamera(hero, 1500);
        AudioManager:PlayMusic("mus/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg");
    end,

    end_of_trap_dialogue = function()
        Map:PopState();
        _CreateEnemies();

        harlequin1:SetPosition(52, 21);
        harlequin2:SetPosition(85, 18);
        harlequin3:SetPosition(66, 21);

        -- Set event as done
        GlobalManager:SetEventValue("story", "mt_elbrus_cemetery_south_gate_closed", 1);
    end,

    make_harlequin1_disappear = function()
        harlequin1:SetPosition(0, 0);
        harlequin1:SetVisible(false);
        harlequin1:ClearEventWhenTalking();
        -- Empty the character stamina
        Map:SetStamina(0);
    end,
    make_harlequin2_disappear = function()
        harlequin2:SetPosition(0, 0);
        harlequin2:SetVisible(false);
        harlequin2:ClearEventWhenTalking();
        -- Empty the character stamina
        Map:SetStamina(0);
    end,
    make_harlequin3_disappear = function()
        harlequin3:SetPosition(0, 0);
        harlequin3:SetVisible(false);
        harlequin3:ClearEventWhenTalking();
        -- Empty the character stamina
        Map:SetStamina(0);
    end,

    increase_harlequin_beaten_time = function()
        harlequin_beaten_time = harlequin_beaten_time + 1;
    end,

    make_harlequins_move = function()

        h1_x_direction = _GetRandomDirectionDiff()
        h1_y_direction = _GetRandomDirectionDiff()
        h2_x_direction = _GetRandomDirectionDiff()
        h2_y_direction = _GetRandomDirectionDiff()
        h3_x_direction = _GetRandomDirectionDiff()
        h3_y_direction = _GetRandomDirectionDiff()
        total_time = 0;
    end,

    make_harlequins_move_update = function()
        local update_time = SystemManager:GetUpdateTime();
        total_time = total_time + update_time;

        -- harlequin 1
        -- -----------
        if (harlequin1:IsVisible() == true) then
            local movement_diff_x = h1_x_direction * update_time;
            if (movement_diff_x > 1.0) then movement_diff_x = 1.0 end;

            local movement_diff_y = h1_y_direction * update_time;
            if (movement_diff_y > 1.0) then movement_diff_y = 1.0 end;

            harlequin1:SetPosition(harlequin1:GetXPosition() + movement_diff_x, harlequin1:GetYPosition() + movement_diff_y);

            -- Change the direction on borders
            if (harlequin1:GetXPosition() > 89.0) then h1_x_direction = -math.abs(h1_x_direction) end;
            if (harlequin1:GetXPosition() < 46.0) then h1_x_direction = math.abs(h1_x_direction) end;
            if (harlequin1:GetYPosition() > 55.0) then h1_y_direction = -math.abs(h1_y_direction) end;
            if (harlequin1:GetYPosition() < 22.0) then h1_y_direction = math.abs(h1_y_direction) end;
        end

        -- harlequin 2
        -- -----------
        if (harlequin2:IsVisible() == true) then
            movement_diff_x = h2_x_direction * update_time;
            if (movement_diff_x > 1.0) then movement_diff_x = 1.0 end;

            movement_diff_y = h2_y_direction * update_time;
            if (movement_diff_y > 1.0) then movement_diff_y = 1.0 end;

            harlequin2:SetPosition(harlequin2:GetXPosition() + movement_diff_x, harlequin2:GetYPosition() + movement_diff_y);

            -- Change the direction on borders
            if (harlequin2:GetXPosition() > 89.0) then h2_x_direction = -math.abs(h2_x_direction) end;
            if (harlequin2:GetXPosition() < 46.0) then h2_x_direction = math.abs(h2_x_direction) end;
            if (harlequin2:GetYPosition() > 55.0) then h2_y_direction = -math.abs(h2_y_direction) end;
            if (harlequin2:GetYPosition() < 22.0) then h2_y_direction = math.abs(h2_y_direction) end;
        end

        -- harlequin 3
        -- -----------
        if (harlequin3:IsVisible() == true) then
            movement_diff_x = h3_x_direction * update_time;
            if (movement_diff_x > 1.0) then movement_diff_x = 1.0 end;

            movement_diff_y = h3_y_direction * update_time;
            if (movement_diff_y > 1.0) then movement_diff_y = 1.0 end;

            harlequin3:SetPosition(harlequin3:GetXPosition() + movement_diff_x, harlequin3:GetYPosition() + movement_diff_y);

            -- Change the direction on borders
            if (harlequin3:GetXPosition() > 89.0) then h3_x_direction = -math.abs(h3_x_direction) end;
            if (harlequin3:GetXPosition() < 46.0) then h3_x_direction = math.abs(h3_x_direction) end;
            if (harlequin3:GetYPosition() > 55.0) then h3_y_direction = -math.abs(h3_y_direction) end;
            if (harlequin3:GetYPosition() < 22.0) then h3_y_direction = math.abs(h3_y_direction) end;
        end

        -- Stop the movement after 5 secs.
        if (total_time >= 5000) then return true end
        return false;
    end,

    make_harlequins_teleport = function()
        _MakeHarlequinTeleport(harlequin1)
        _MakeHarlequinTeleport(harlequin2)
        _MakeHarlequinTeleport(harlequin3)
        AudioManager:PlaySound("snd/crystal_chime.wav");
    end,

    place_harlequin_for_pre_boss_battle = function()
        harlequin1:SetVisible(true);
        harlequin1:SetPosition(hero:GetXPosition(), hero:GetYPosition() - 3.0);
    end,

    ends_harlequin_battle1 = function()
        -- Harlequin disappears
        AudioManager:PlaySound("snd/crystal_chime.wav");
        harlequin1:SetVisible(false);
        harlequin1:SetPosition(0, 0);
    end,

    ends_harlequin_battle2 = function()
        _OpenNorthGate();
        AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
        Map:PopState();
        GlobalManager:SetEventValue("story", "mt_elbrus_cemetery_fight_done", 1);
        harlequin_battle_done = true;
    end,
}
