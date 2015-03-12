-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_path3_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "data/story/common/locations/mt_elbrus.png"
map_subname = "Old Cemetery"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/awareness_el_corleo.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

-- the main character handler
local hero = nil

-- Dialogue sprites
local bronann = nil
local kalya = nil
local orlinn = nil

local harlequin_battle_done = false;

-- Soldiers
local soldier1 = nil
local soldier2 = nil
local soldier3 = nil

-- the main map loading code
function Load(m)

    Map = m;
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(false);

    _CreateCharacters();
    _CreateObjects();
    -- Called after the gates are closed
    --_CreateEnemies();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("data/visuals/ambient/clouds.png", 5.0, -5.0, true);
    Map:GetScriptSupervisor():AddScript("data/story/common/at_night.lua");

    -- Enables thunder
    GlobalManager:SetEventValue("story", "mt_elbrus_weather_level", 2)

    -- Make the rain starts or the corresponding dialogue according the need
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 0) then
        Map:GetParticleManager():AddParticleEffect("data/visuals/particle_effects/rain.lua", 512.0, 768.0);
        -- Place an omni ambient sound at the center of the map to add a nice rainy effect.
        vt_map.SoundObject.Create("data/music/Ove Melaa - Rainy.ogg", 20.0, 16.0, 100.0);
    end
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 1) then
        Map:GetScriptSupervisor():AddScript("data/story/common/soft_lightnings_script.lua");
    end

    -- Preload special sounds
    AudioManager:LoadSound("data/sounds/crystal_chime.wav", Map);
    AudioManager:LoadSound("data/sounds/opening_sword_unsheathe.wav", Map);
    AudioManager:LoadMusic("data/music/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg", Map);

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
    hero = CreateSprite(Map, "Bronann", 63, 92.5, vt_map.MapMode.GROUND_OBJECT);
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

    bronann = CreateSprite(Map, "Bronann", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.WEST);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    bronann:SetVisible(false);

    kalya = CreateSprite(Map, "Kalya", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.EAST);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);

    orlinn = CreateSprite(Map, "Orlinn", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);

    soldier1 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 0, 0, vt_map.MapMode.GROUND_OBJECT);
    soldier1:SetVisible(false);
    soldier1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    soldier2 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 0, 0, vt_map.MapMode.GROUND_OBJECT);
    soldier2:SetVisible(false);
    soldier2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    soldier3 = CreateNPCSprite(Map, "Dark Soldier on horse", vt_system.Translate("Soldier"), 0, 0, vt_map.MapMode.GROUND_OBJECT);
    soldier3:SetVisible(false);
    soldier3:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
end

-- The heal particle effect map object
local heal_effect = nil

local north_gate_closed = nil
local south_gate1_closed = nil
local south_gate2_closed = nil
local south_gate3_closed = nil
local north_gate_open = nil
local south_gate1_open = nil
local south_gate2_open = nil
local south_gate3_open = nil
local west_gate_stone1 = nil
local west_gate_stone2 = nil
local west_gate_stone3 = nil
local west_gate_stone4 = nil

local harlequin1 = nil
local harlequin2 = nil
local harlequin3 = nil
local harlequin_focus = nil

local harlequin_beaten_time = 0;

function _CreateObjects()
    local object = nil
    local npc = nil
    local dialogue = nil
    local text = nil
    local event = nil

    vt_map.SavePoint.Create(85, 81);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject.Create("data/visuals/particle_effects/heal_particle.lua", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    heal_effect:Stop(); -- Don't run it until the character heals itself

    -- Heal point
    object = CreateObject(Map, "Layna Statue", 60, 70, vt_map.MapMode.GROUND_OBJECT);
    object:SetEventWhenTalking("Heal dialogue");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Your party feels better.");
    dialogue:AddLineEvent(text, nil, "Heal event", ""); -- nil means no portrait and no name
    vt_map.DialogueEvent.Create("Heal dialogue", dialogue);

    -- Cemetery gates
    north_gate_closed = CreateObject(Map, "Gate1 closed", 51, 16, vt_map.MapMode.GROUND_OBJECT);
    north_gate_open = CreateObject(Map, "Gate1 open", 51, 12, vt_map.MapMode.GROUND_OBJECT);
    north_gate_open:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    north_gate_open:SetVisible(false);
    north_gate_open:SetDrawOnSecondPass(true); -- Above the character

    -- The south gate is open at the beginning of the map.
    south_gate1_closed = CreateObject(Map, "Gate1 closed", 0, 0, vt_map.MapMode.GROUND_OBJECT); -- 63, 68
    south_gate2_closed = CreateObject(Map, "Gate1 closed", 0, 0, vt_map.MapMode.GROUND_OBJECT); -- 67, 68
    south_gate3_closed = CreateObject(Map, "Gate1 closed", 0, 0, vt_map.MapMode.GROUND_OBJECT); -- 71, 68

    south_gate1_open = CreateObject(Map, "Gate1 open", 63, 64, vt_map.MapMode.GROUND_OBJECT);
    south_gate1_open:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    south_gate1_open:SetDrawOnSecondPass(true); -- Above the character
    south_gate2_open = CreateObject(Map, "Gate1 open", 67, 64, vt_map.MapMode.GROUND_OBJECT);
    south_gate2_open:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    south_gate2_open:SetDrawOnSecondPass(true); -- Above the character
    south_gate3_open = CreateObject(Map, "Gate1 open", 71, 64, vt_map.MapMode.GROUND_OBJECT);
    south_gate3_open:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    south_gate3_open:SetDrawOnSecondPass(true); -- Above the character

    west_gate_stone1 = CreateObject(Map, "Rock2", 41, 18, vt_map.MapMode.GROUND_OBJECT);
    west_gate_stone2 = CreateObject(Map, "Rock2", 41, 20, vt_map.MapMode.GROUND_OBJECT);
    west_gate_stone3 = CreateObject(Map, "Rock2", 41, 22, vt_map.MapMode.GROUND_OBJECT);
    west_gate_stone4 = CreateObject(Map, "Rock2", 41, 24, vt_map.MapMode.GROUND_OBJECT);

    harlequin1 = CreateObject(Map, "Harlequin", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    harlequin1:SetEventWhenTalking("Make Harlequin1 disappear");
    harlequin2 = CreateObject(Map, "Harlequin", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    harlequin2:SetEventWhenTalking("Make Harlequin2 disappear");
    harlequin3 = CreateObject(Map, "Harlequin", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    harlequin3:SetEventWhenTalking("Make Harlequin3 disappear");

    -- Harlequin must be beaten third times before being actually fighteable
    harlequin_beaten_time = 0;

    event = vt_map.ScriptedEvent.Create("Make Harlequin1 disappear", "make_harlequin1_disappear", "");
    event:AddEventLinkAtEnd("Fake Harlequin battle");

    event = vt_map.ScriptedEvent.Create("Make Harlequin2 disappear", "make_harlequin2_disappear", "");
    event:AddEventLinkAtEnd("Fake Harlequin battle");

    event = vt_map.ScriptedEvent.Create("Make Harlequin3 disappear", "make_harlequin3_disappear", "");
    event:AddEventLinkAtEnd("Fake Harlequin battle");

    event = vt_map.BattleEncounterEvent.Create("Fake Harlequin battle");
    event:SetBoss(true);
    event:AddEnemy(13, 512, 484); -- Harlequin?
    event:AddEnemy(12, 470, 384); -- Eyeballs
    event:AddEnemy(12, 380, 500); -- Eyeballs
    event:AddEnemy(12, 650, 484); -- Eyeballs
    event:AddEnemy(12, 570, 584); -- Eyeballs
    event:AddEnemy(12, 450, 624); -- Eyeballs
    _SetEventBattleEnvironment(event);
    event:AddEventLinkAtEnd("Increase Harlequin beaten time");

    vt_map.ScriptedEvent.Create("Increase Harlequin beaten time", "increase_harlequin_beaten_time", "");

    --harlequin virtual focus
    harlequin_focus = CreateSprite(Map, "Butterfly", 68, 24, vt_map.MapMode.GROUND_OBJECT);
    harlequin_focus:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    harlequin_focus:SetVisible(false);
    harlequin_focus:SetName(vt_system.Translate("???")); -- At first, the characters don't know his name.

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
        CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
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
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
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
local enemy_zone1 = nil
local enemy_zone2 = nil
local enemy_zone3 = nil

function _CreateEnemies()
    local enemy = nil
    local roam_zone = nil

    -- Hint: left, right, top, bottom
    enemy_zone1 = vt_map.EnemyZone.Create(65, 68, 32, 36);
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
    enemy_zone1:AddEnemy(enemy, 2);
    enemy_zone1:SetSpawnsLeft(2); -- This monster shall spawn only two times.

    -- Hint: left, right, top, bottom
    enemy_zone2 = vt_map.EnemyZone.Create(45, 48, 32, 36);
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
    enemy_zone2:AddEnemy(enemy, 2);
    enemy_zone2:SetSpawnsLeft(2); -- This monster shall spawn only two times.

    -- Hint: left, right, top, bottom
    enemy_zone3 = vt_map.EnemyZone.Create(87, 90, 32, 36);
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
    enemy_zone3:AddEnemy(enemy, 2);
    enemy_zone3:SetSpawnsLeft(2); -- This monster shall spawn only two times.
end

-- Special event references which destinations must be updated just before being called.
local kalya_move_next_to_bronann_event1 = nil
local orlinn_move_next_to_bronann_event1 = nil

local kalya_move_next_to_bronann_event2 = nil
local orlinn_move_next_to_bronann_event2 = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    vt_map.MapTransitionEvent.Create("to mountain path 2", "data/story/mt_elbrus/mt_elbrus_path2_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_path2_script.lua", "from_path3");

    vt_map.MapTransitionEvent.Create("to mountain path 2bis", "data/story/mt_elbrus/mt_elbrus_path2_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_path2_script.lua", "from_path3_chest");

    vt_map.MapTransitionEvent.Create("to mountain path 4", "data/story/mt_elbrus/mt_elbrus_path4_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_path4_script.lua", "from_path3");

    -- Heal point
    vt_map.ScriptedEvent.Create("Heal event", "heal_party", "heal_done");

    -- sprite direction events
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks north", bronann, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Hero looks south", hero, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks south", kalya, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks west", kalya, vt_map.MapMode.WEST);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);

    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Kalya", orlinn, kalya);

    -- cemetery entrance scene
    event = vt_map.ScriptedEvent.Create("Set scene state for dialogue about cemetery entrance", "set_scene_state", "");
    event:AddEventLinkAtEnd("The hero moves to a good watch point");

    event = vt_map.PathMoveSpriteEvent.Create("The hero moves to a good watch point", hero, 67, 73, false);
    event:AddEventLinkAtEnd("Kalya tells about the cemetery");

    event = vt_map.ScriptedEvent.Create("Kalya tells about the cemetery", "kalya_cemetery_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann1", 100);
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann1", 100);

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_bronann_event1 = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann1", kalya, 0, 0, false);
    kalya_move_next_to_bronann_event1:AddEventLinkAtEnd("Kalya looks north");
    kalya_move_next_to_bronann_event1:AddEventLinkAtEnd("Bronann looks north");
    kalya_move_next_to_bronann_event1:AddEventLinkAtEnd("Kalya talks about the cemetery");

    orlinn_move_next_to_bronann_event1 = vt_map.PathMoveSpriteEvent.Create("Orlinn moves next to Bronann1", orlinn, 0, 0, false);
    orlinn_move_next_to_bronann_event1:AddEventLinkAtEnd("Orlinn looks north");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("This place is the village's old cemetery. It was used by the former villagers who lived in Layna.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("The former villagers? What do you mean?");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("Layna Village was abandoned a long time ago, before your parents and all the others settled here. Your mother never told you that?");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at Bronann", "", "thinking dots");
    text = vt_system.Translate("I've never been this far from the village without Herth. This place gives me the chills.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks north", "");
    text = vt_system.Translate("Yiek!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Anyway, once the cemetery is behind us we should be out of trouble!");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Kalya talks about the cemetery", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes back to party");
    event:AddEventLinkAtEnd("Kalya goes back to party");

    vt_map.PathMoveSpriteEvent.Create("Orlinn goes back to party", orlinn, bronann, false);

    event = vt_map.PathMoveSpriteEvent.Create("Kalya goes back to party", kalya, bronann, false);
    event:AddEventLinkAtEnd("End of dialogue about the cemetery");

    vt_map.ScriptedEvent.Create("End of dialogue about the cemetery", "end_of_dialogue_about_cemetery", "");

    -- West gate dialogue
    -- ------------------
    event = vt_map.ScriptedEvent.Create("Set scene state for dialogue about west gate", "set_scene_state", "");
    event:AddEventLinkAtEnd("The hero notices the soldiers");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Are those the soldiers up there?");
    dialogue:AddLineEventEmote(text, hero, "Bronann looks north", "", "exclamation");
    event = vt_map.DialogueEvent.Create("The hero notices the soldiers", dialogue);
    event:AddEventLinkAtEnd("West gate - The hero moves to a good watch point");

    event = vt_map.PathMoveSpriteEvent.Create("West gate - The hero moves to a good watch point", hero, 16, 40, true);
    event:AddEventLinkAtEnd("Kalya tells about the soldiers");

    event = vt_map.ScriptedEvent.Create("Kalya tells about the soldiers", "kalya_west_gate_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann2", 100);
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann2", 100);

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_bronann_event2 = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann2", kalya, 0, 0, false);
    kalya_move_next_to_bronann_event2:AddEventLinkAtEnd("Kalya looks north");
    kalya_move_next_to_bronann_event2:AddEventLinkAtEnd("Bronann looks north");
    kalya_move_next_to_bronann_event2:AddEventLinkAtEnd("Set focus on soldiers");

    orlinn_move_next_to_bronann_event2 = vt_map.PathMoveSpriteEvent.Create("Orlinn moves next to Bronann2", orlinn, 0, 0, false);
    orlinn_move_next_to_bronann_event2:AddEventLinkAtEnd("Orlinn looks north");

    event = vt_map.ScriptedEvent.Create("Set focus on soldiers", "set_focus_on_soldiers", "set_focus_update");
    event:AddEventLinkAtEnd("Soldiers dialogue");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("The west gate is condemned, as the Lord commanded.");
    dialogue:AddLine(text, soldier1);
    text = vt_system.Translate("Good. Let's go back and wait for them.");
    dialogue:AddLine(text, soldier3);
    event = vt_map.DialogueEvent.Create("Soldiers dialogue", dialogue);
    event:AddEventLinkAtEnd("Soldier1 moves out of map", 300);
    event:AddEventLinkAtEnd("Soldier2 moves out of map", 300);
    event:AddEventLinkAtEnd("Soldier3 moves out of map");
    event:AddEventLinkAtEnd("West gate - Set focus on Bronann", 400);

    event = vt_map.PathMoveSpriteEvent.Create("Soldier1 moves out of map", soldier1, 1, 25, false);
    event:AddEventLinkAtEnd("Make soldier1 disappear");

    event = vt_map.PathMoveSpriteEvent.Create("Soldier2 moves out of map", soldier2, 1, 27, false);
    event:AddEventLinkAtEnd("Make soldier2 disappear");

    event = vt_map.PathMoveSpriteEvent.Create("Soldier3 moves out of map", soldier3, 3, 26, false);
    event:AddEventLinkAtEnd("Make soldier3 disappear");

    vt_map.ScriptedSpriteEvent.Create("Make soldier1 disappear", soldier1, "make_object_disappear", "");
    vt_map.ScriptedSpriteEvent.Create("Make soldier2 disappear", soldier2, "make_object_disappear", "");
    vt_map.ScriptedSpriteEvent.Create("Make soldier3 disappear", soldier3, "make_object_disappear", "");

    event = vt_map.ScriptedEvent.Create("West gate - Set focus on Bronann", "set_focus_back_on_bronann", "set_focus_update");
    event:AddEventLinkAtEnd("Dialogue about the west gate");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("The gate to the great plains...");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("Have we got any other way to leave this place?");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Only one. The mountain top.");
    dialogue:AddLineEmote(text, kalya, "thinking dots");
    event = vt_map.DialogueEvent.Create("Dialogue about the west gate", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes back to party 2");
    event:AddEventLinkAtEnd("Kalya goes back to party 2");

    vt_map.PathMoveSpriteEvent.Create("Orlinn goes back to party 2", orlinn, bronann, false);

    event = vt_map.PathMoveSpriteEvent.Create("Kalya goes back to party 2", kalya, bronann, false);
    event:AddEventLinkAtEnd("End of dialogue about west gate");

    vt_map.ScriptedEvent.Create("End of dialogue about west gate", "end_of_dialogue_about_west_gate", "");

    -- trapped event!
    -- --------------
    event = vt_map.ScriptedEvent.Create("Prepare trapped event", "set_scene_state", "");
    event:AddEventLinkAtEnd("The hero notices about the gate");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Oh no! The gate!");
    dialogue:AddLineEventEmote(text, hero, "Hero looks south", "", "exclamation");
    event = vt_map.DialogueEvent.Create("The hero notices about the gate", dialogue);
    event:AddEventLinkAtEnd("The hero rushes to the gate");

    event = vt_map.PathMoveSpriteEvent.Create("The hero rushes to the gate", hero, 67, 66.5, true);
    event:AddEventLinkAtEnd("The hero is trapped");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("We're trapped!");
    dialogue:AddLine(text, hero);
    event = vt_map.DialogueEvent.Create("The hero is trapped", dialogue);
    event:AddEventLinkAtEnd("Harlequin talks to the hero");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("You're mine now!");
    dialogue:AddLine(text, harlequin_focus);
    event = vt_map.DialogueEvent.Create("Harlequin talks to the hero", dialogue);
    event:AddEventLinkAtEnd("The hero is surprised");

    event = vt_map.ScriptedEvent.Create("The hero is surprised", "hero_exclamation", "");
    event:AddEventLinkAtEnd("Bronann looks north");
    event:AddEventLinkAtEnd("Set the focus on Harlequin");

    vt_map.ScriptedEvent.Create("Set Harlequin actual name", "set_harlequin_name", "");

    event = vt_map.ScriptedEvent.Create("Set the focus on Harlequin", "set_focus_on_harlequin", "set_focus_update");
    event:AddEventLinkAtEnd("Harlequin talks to the hero 2");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I shall bring your souls to the master. You don't stand a chance against the Great Harlequin.");
    dialogue:AddLineEvent(text, harlequin_focus, "", "Set Harlequin actual name");
    text = vt_system.Translate("But let's play together first, shall we?");
    dialogue:AddLine(text, harlequin_focus);
    text = vt_system.Translate("Now catch me, if you can.");
    dialogue:AddLine(text, harlequin_focus);
    event = vt_map.DialogueEvent.Create("Harlequin talks to the hero 2", dialogue);
    event:AddEventLinkAtEnd("Set the focus back on hero");

    event = vt_map.ScriptedEvent.Create("Set the focus back on hero", "set_focus_on_hero", "set_focus_update");
    event:AddEventLinkAtEnd("End of trapped Dialogue");

    event = vt_map.ScriptedEvent.Create("End of trapped Dialogue", "end_of_trap_dialogue", "");
    event:AddEventLinkAtEnd("Make the Harlequins move");

    event = vt_map.ScriptedEvent.Create("Make the Harlequins move", "make_harlequins_move", "make_harlequins_move_update");
    event:AddEventLinkAtEnd("Make the Harlequins teleport");

    event = vt_map.ScriptedEvent.Create("Make the Harlequins teleport", "make_harlequins_teleport", "");
    event:AddEventLinkAtEnd("Make the Harlequins move");

    event = vt_map.ScriptedEvent.Create("Harlequin is fed up!", "place_harlequin_for_pre_boss_battle", "");
    event:AddEventLinkAtEnd("Harlequin talks to the hero 3");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Enough of this shallow game. Give me your souls, now!");
    dialogue:AddLineEvent(text, harlequin_focus, "Bronann looks north", "");
    event = vt_map.DialogueEvent.Create("Harlequin talks to the hero 3", dialogue);
    event:AddEventLinkAtEnd("True Harlequin battle");

    event = vt_map.BattleEncounterEvent.Create("True Harlequin battle");
    event:SetBoss(true);
    event:AddEnemy(14, 512, 484); -- Harlequin
    event:AddEnemy(12, 470, 384); -- Eyeballs
    event:AddEnemy(12, 380, 500); -- Eyeballs
    event:AddEnemy(12, 650, 484); -- Eyeballs
    event:AddEnemy(12, 570, 584); -- Eyeballs
    event:AddEnemy(12, 450, 624); -- Eyeballs
    _SetEventBattleEnvironment(event);
    event:SetMusic("data/music/accion-OGA-djsaryon.ogg"); --boss music
    event:AddEventLinkAtEnd("Harlequin talks to the hero 4");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("How could I be hurt by... children...");
    dialogue:AddLineEvent(text, harlequin_focus, "Bronann looks north", "");
    text = vt_system.Translate("We'll see each other again, chosen one...");
    dialogue:AddLine(text, harlequin_focus);
    event = vt_map.DialogueEvent.Create("Harlequin talks to the hero 4", dialogue);
    event:AddEventLinkAtEnd("Ends Harlequin battle");

    event = vt_map.ScriptedEvent.Create("Ends Harlequin battle", "ends_harlequin_battle1", "");
    event:AddEventLinkAtEnd("Ends Harlequin battle 2", 1200);

    -- Adds a bit of time before opening the gate so that action sounds don't get mixed.
    vt_map.ScriptedEvent.Create("Ends Harlequin battle 2", "ends_harlequin_battle2", "");
end

-- zones
local to_path4_zone = nil
local to_path2_zone = nil
local to_path2_bis_zone = nil

local cemetery_entrance_dialogue_zone = nil
local cemetery_west_gate_dialogue_zone = nil
local cemetery_gates_closed_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_path4_zone = vt_map.CameraZone.Create(40, 55, 0, 2);
    to_path2_zone = vt_map.CameraZone.Create(53, 74, 94, 96);
    to_path2_bis_zone = vt_map.CameraZone.Create(1, 23, 94, 96);

    -- event zones
    cemetery_entrance_dialogue_zone = vt_map.CameraZone.Create(61, 74, 71, 73);
    cemetery_west_gate_dialogue_zone = vt_map.CameraZone.Create(7, 31, 46, 48);
    -- cemetery gates closed
    cemetery_gates_closed_zone = vt_map.CameraZone.Create(44, 92, 52, 54);
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
            AudioManager:PlaySound("data/sounds/opening_sword_unsheathe.wav");
            EventManager:StartEvent("Prepare trapped event");
        end
    end
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    -- default values
    enemy:SetBattleMusicTheme("data/music/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg");
    enemy:SetBattleBackground("data/battles/battle_scenes/mountain_background.png");
    enemy:AddBattleScript("data/story/common/at_night.lua");

    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 0) then
        enemy:AddBattleScript("data/story/common/rain_in_battles_script.lua");
    end
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 1) then
        enemy:AddBattleScript("data/story/common/soft_lightnings_script.lua");
    end
end
-- The environment for Harlequin's battles
function _SetEventBattleEnvironment(event)
    event:SetMusic("data/music/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg");
    event:SetBackground("data/battles/battle_scenes/mountain_background.png");
    event:AddScript("data/story/common/at_night.lua");

    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 0) then
        event:AddScript("data/story/common/rain_in_battles_script.lua");
    end
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 1) then
        event:AddScript("data/story/common/soft_lightnings_script.lua");
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
        EventManager:EndEvent("Make the Harlequins move", false)

        -- Remove the other monsters
        Map:SetAllEnemyStatesToDead();
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

    set_scene_state = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

    kalya_cemetery_dialogue_start = function()
        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition())
        bronann:SetDirection(hero:GetDirection())
        bronann:SetVisible(true)
        hero:SetVisible(false)
        Map:SetCamera(bronann)
        hero:SetPosition(0, 0)

        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        kalya:SetVisible(true);
        orlinn:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        orlinn:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        kalya_move_next_to_bronann_event1:SetDestination(bronann:GetXPosition() + 2.0, bronann:GetYPosition(), false);
        orlinn_move_next_to_bronann_event1:SetDestination(bronann:GetXPosition() - 2.0, bronann:GetYPosition(), false);
    end,

    end_of_dialogue_about_cemetery = function()
        Map:PopState();
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition())
        hero:SetDirection(bronann:GetDirection())
        hero:SetVisible(true)
        bronann:SetVisible(false)
        Map:SetCamera(hero)
        bronann:SetPosition(0, 0)

        -- Set event as done
        GlobalManager:SetEventValue("story", "mt_elbrus_kalya_cemetery_entrance_dialogue", 1);
    end,

    kalya_west_gate_dialogue_start = function()
        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition())
        bronann:SetDirection(hero:GetDirection())
        bronann:SetVisible(true)
        hero:SetVisible(false)
        Map:SetCamera(bronann)
        hero:SetPosition(0, 0)

        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        kalya:SetVisible(true);
        orlinn:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        orlinn:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        kalya_move_next_to_bronann_event2:SetDestination(bronann:GetXPosition() + 2.0, bronann:GetYPosition(), false);
        orlinn_move_next_to_bronann_event2:SetDestination(bronann:GetXPosition() - 2.0, bronann:GetYPosition(), false);

        -- Place the soldiers
        soldier1:SetPosition(19, 25);
        soldier1:SetDirection(vt_map.MapMode.WEST)
        soldier1:SetVisible(true);
        soldier2:SetPosition(19, 27);
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

    set_focus_back_on_bronann = function()
        Map:SetCamera(bronann, 1000);
    end,

    end_of_dialogue_about_west_gate = function()
        Map:PopState();
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition())
        hero:SetDirection(bronann:GetDirection())
        hero:SetVisible(true)
        bronann:SetVisible(false)
        Map:SetCamera(hero)
        bronann:SetPosition(0, 0)

        -- Set event as done
        GlobalManager:SetEventValue("story", "mt_elbrus_kalya_west_gate_dialogue", 1);
    end,

    -- Trapped scripted events functions
    -- ---------------------------------
    hero_exclamation = function()
        hero:Emote("exclamation", hero:GetDirection());
        AudioManager:FadeOutActiveMusic(1000);
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
        AudioManager:PlayMusic("data/music/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg");
    end,

    set_harlequin_name = function()
        harlequin_focus:SetName(vt_system.Translate("Harlequin"));
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
        AudioManager:PlaySound("data/sounds/crystal_chime.wav");
    end,

    place_harlequin_for_pre_boss_battle = function()
        harlequin1:SetVisible(true);
        harlequin1:SetPosition(hero:GetXPosition(), hero:GetYPosition() - 3.0);
    end,

    ends_harlequin_battle1 = function()
        -- Harlequin disappears
        AudioManager:PlaySound("data/sounds/crystal_chime.wav");
        harlequin1:SetVisible(false);
        harlequin1:SetPosition(0, 0);
    end,

    ends_harlequin_battle2 = function()
        _OpenNorthGate();
        AudioManager:PlaySound("data/sounds/opening_sword_unsheathe.wav");
        Map:PopState();
        GlobalManager:SetEventValue("story", "mt_elbrus_cemetery_fight_done", 1);
        harlequin_battle_done = true;
    end,
}
