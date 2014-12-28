-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_path2_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "img/menus/locations/mt_elbrus.png"
map_subname = "Low Mountain"

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

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

    Map.unlimited_stamina = false;

    _CreateCharacters();
    _CreateObjects();
    _CreateEnemies();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, -5.0, true);
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");

    -- Make the rain starts or the corresponding dialogue according the need
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 0) then
        Map:GetParticleManager():AddParticleEffect("dat/effects/particles/rain.lua", 512.0, 768.0);
        -- Place an omni ambient sound at the center of the map to add a nice rainy effect.
        local rainy_sound = vt_map.SoundObject("mus/Ove Melaa - Rainy.ogg", 20.0, 16.0, 100.0);
        Map:AddAmbientSoundObject(rainy_sound);
    else
        EventManager:StartEvent("Rain dialogue start", 200);
    end

    -- Enables thunder if it started.
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 1) then
        Map:GetScriptSupervisor():AddScript("dat/maps/common/soft_lightnings_script.lua");
    end

end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from mountain path 1)
    hero = CreateSprite(Map, "Bronann", 76, 21);
    hero:SetDirection(vt_map.MapMode.WEST);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    --hero:SetCollisionMask(vt_map.MapMode.NO_COLLISION); -- dev

    -- Load previous save point data
    local x_position = GlobalManager:GetSaveLocationX();
    local y_position = GlobalManager:GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Use the save point position, and clear the save position data for next maps
        GlobalManager:UnsetSaveLocation();
        -- Make the character look at us in that case
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(x_position, y_position);
    elseif (GlobalManager:GetPreviousLocation() == "from_grotto2_1_exit") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(28.0, 56.5);
    elseif (GlobalManager:GetPreviousLocation() == "from_grotto2_2_exit") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(20.0, 40.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_grotto3_1_exit") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(48.0, 16.5);
    elseif (GlobalManager:GetPreviousLocation() == "from_grotto3_2_exit") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(24.0, 10.5);
    elseif (GlobalManager:GetPreviousLocation() == "from_path3") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(36.0, 4.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_path3_chest") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(5.0, 4.0);
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
end

-- The heal particle effect map object
local heal_effect = {};

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}

    Map:AddSavePoint(67, 37);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject("dat/effects/particles/heal_particle.lua", 0, 0);
    heal_effect:SetObjectID(Map.object_supervisor:GenerateObjectID());
    heal_effect:Stop(); -- Don't run it until the character heals itself
    Map:AddGroundObject(heal_effect);

    -- Heal point
    object = CreateObject(Map, "Layna Statue", 37, 27);
    object:SetEventWhenTalking("Heal dialogue");
    Map:AddGroundObject(object);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Your party feels better.");
    dialogue:AddLineEvent(text, 0, "Heal event", ""); -- 0 means no portrait and no name
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Heal dialogue", dialogue);
    EventManager:RegisterEvent(event);

    -- Treasure box
    local chest = CreateTreasure(Map, "elbrus_path2_chest1", "Wood_Chest1", 7, 7);
    if (chest ~= nil) then
        chest:AddObject(2, 1); -- Medium healing potion
        Map:AddGroundObject(chest);
    end

    -- Objects array
    local map_objects = {

        { "Tree Big2", 73, 50 },
        { "Tree Big1", 61, 62 },
        { "Tree Big2", 57, 64.8 },
        { "Tree Small2", 49, 66 },
        { "Tree Big2", 27.5, 65 },
        { "Tree Big2", 15, 12 },
        { "Tree Big2", 45, 64 },
        { "Tree Big2", 40, 60 },
        { "Tree Big2", 32, 63 },
        { "Tree Big2", 36.6, 62.5 },
        { "Tree Big2", 53.5, 63 },
        { "Tree Big1", 21, 58 },
        { "Tree Big2", 18, 52 },
        { "Tree Big2", 35, 67 },
        { "Tree Big2", 38, 68 },
        { "Tree Small1", 42, 62 },

        { "Tree Big2", 36, 51 },
        { "Tree Big2", 41, 48 },
        { "Tree Big2", 40, 44 },
        { "Tree Big2", 38, 41 },
        { "Tree Big2", 42, 38 },
        { "Tree Big2", 44, 35 },
        { "Tree Big2", 46, 31 },
        { "Tree Big2", 50, 28 },
        { "Tree Big2", 51, 24 },
        { "Tree Big2", 54, 21 },
        { "Tree Big2", 53, 18 },
        { "Tree Big2", 57, 15 },
        { "Tree Big2", 60, 12 },
        { "Tree Small2", 39, 52 },
        { "Tree Big2", 48, 44 },
        { "Tree Big2", 52, 36 },
        { "Tree Big2", 54, 8 },
        { "Tree Big2", 67, 10 },
        { "Tree Big1", 60, 46 },
        { "Tree Big2", 63, 49 },
        { "Tree Big2", 59, 51 },
        { "Tree Small1", 62.5, 8.5 },

        { "Tree Big2", 27, 32 },
        { "Tree Big2", 30, 30 },
        { "Tree Big2", 28, 27 },
        { "Tree Big2", 31, 25 },
        { "Tree Big2", 34, 22 },
        { "Tree Big2", 37, 18 },
        { "Tree Big2", 40, 16 },
        { "Tree Big2", 50, 20 },
        { "Tree Big2", 13, 40 },
        { "Tree Big2", 41, 18.5 },
        { "Tree Big2", 26, 22 },
        { "Tree Big2", 17, 19 },
        { "Tree Big2", 35, 13 },
        { "Tree Big2", 30, 7 },
        { "Tree Big2", 25, 25.5 },
        { "Tree Big2", 46, 37 },
        { "Tree Big2", 52, 30 },
        { "Tree Big2", 49, 35 },
        { "Tree Big2", 43, 42 },

        { "Tree Big2", 1, 9 },
        { "Tree Big2", 4, 15 },
        { "Tree Small1", 2, 12 },
        { "Tree Big2", 7, 23 },

        { "Tree Small1", 22.7, 47 },
        { "Tree Small2", 31, 45 },

        { "Rock2", 74, 27 },
        { "Rock1", 76, 28 },
        { "Rock1", 37, 19.5 },

        { "Rock2", 40, 24 },
        { "Rock2", 34, 24 },
        { "Rock2", 34, 29 },
        { "Rock2", 40, 29 },

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
        { "Grass Clump1", 27, 55 },
        { "Grass Clump1", 29, 55.3 },
        { "Grass Clump1", 75.5, 15 },
        { "Grass Clump1", 70, 13 },
        { "Grass Clump1", 76, 51 },
        { "Grass Clump1", 56, 59 },
        { "Grass Clump1", 25, 60 },
        { "Grass Clump1", 34, 52 },
        { "Grass Clump1", 48, 29 },
        { "Grass Clump1", 46, 47 },
        { "Grass Clump1", 63, 17.5 },
        { "Grass Clump1", 64, 9 },
        { "Grass Clump1", 71, 25 },
        { "Grass Clump1", 63, 53 },
        { "Grass Clump1", 31, 46 },
        { "Grass Clump1", 37.5, 28 },
        { "Grass Clump1", 46, 15 },


    }

    -- Loads the grass clumps according to the array
    for my_index, my_array in pairs(map_grass) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(object);
    end

end

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(54, 62, 52, 60);
    -- Some bats
    enemy = CreateEnemySprite(Map, "bat");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(4);
    enemy:AddEnemy(6);
    enemy:AddEnemy(4);
    roam_zone:AddEnemy(enemy, Map, 1);
    Map:AddZone(roam_zone);

end

-- Special event references which destinations must be updated just before being called.
local kalya_move_next_to_hero_event = {}
local kalya_move_back_to_hero_event = {}
local orlinn_move_next_to_hero_event = {}
local orlinn_move_back_to_hero_event = {}

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- To the first cave
    event = vt_map.MapTransitionEvent("to cave 2-1", "dat/maps/mt_elbrus/mt_elbrus_cave2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_cave2_script.lua", "from_elbrus_entrance2-1");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to cave 2-2", "dat/maps/mt_elbrus/mt_elbrus_cave2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_cave2_script.lua", "from_elbrus_entrance2-2");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to cave 3-1", "dat/maps/mt_elbrus/mt_elbrus_cave3_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_cave3_script.lua", "from_elbrus_entrance3-1");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to cave 3-2", "dat/maps/mt_elbrus/mt_elbrus_cave3_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_cave3_script.lua", "from_elbrus_entrance3-2");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain path 3", "dat/maps/mt_elbrus/mt_elbrus_path3_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path3_script.lua", "from_path2");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain path 3bis", "dat/maps/mt_elbrus/mt_elbrus_path3_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path3_script.lua", "from_path2_chest");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain path 1", "dat/maps/mt_elbrus/mt_elbrus_path1_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path1_script.lua", "from_path2");
    EventManager:RegisterEvent(event);

    -- Heal point
    event = vt_map.ScriptedEvent("Heal event", "heal_party", "heal_done");
    EventManager:RegisterEvent(event);

    -- sprite direction events
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks north", hero, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks south", kalya, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Kalya looks at Orlinn", kalya, orlinn);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Orlinn looks at Kalya", orlinn, kalya);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks west", kalya, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);

    -- rain dialogue events
    event = vt_map.ScriptedEvent("Rain dialogue start", "rain_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 100);
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann", 100);
    EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_hero_event = vt_map.PathMoveSpriteEvent("Kalya moves next to Bronann", kalya, 0, 0, false);
    kalya_move_next_to_hero_event:AddEventLinkAtEnd("Kalya looks north");
    kalya_move_next_to_hero_event:AddEventLinkAtEnd("Bronann looks north");
    kalya_move_next_to_hero_event:AddEventLinkAtEnd("Kalya talks about the rain");
    EventManager:RegisterEvent(kalya_move_next_to_hero_event);
    orlinn_move_next_to_hero_event = vt_map.PathMoveSpriteEvent("Orlinn moves next to Bronann", orlinn, 0, 0, false);
    orlinn_move_next_to_hero_event:AddEventLinkAtEnd("Orlinn looks north");
    EventManager:RegisterEvent(orlinn_move_next_to_hero_event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Dang! And now the rain too.");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("The path seems to lead straight into the storm up there. It's definitely our lucky day.");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks west", "Orlinn looks at Kalya", "thinking dots");
    text = vt_system.Translate("Don't lose faith, Kalya. If we manage to escape from them here, we'll be safe past the mountains.");
    dialogue:AddLineEvent(text, orlinn, "Kalya looks at Orlinn", "Kalya looks south");
    text = vt_system.Translate("If we can reach the great plains, we should indeed be out of trouble.");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks west", "", "thinking dots");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya talks about the rain", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes back to party");
    event:AddEventLinkAtEnd("Kalya goes back to party");
    EventManager:RegisterEvent(event);

    orlinn_move_back_to_hero_event = vt_map.PathMoveSpriteEvent("Orlinn goes back to party", orlinn, hero, false);
    EventManager:RegisterEvent(orlinn_move_back_to_hero_event);

    kalya_move_back_to_hero_event = vt_map.PathMoveSpriteEvent("Kalya goes back to party", kalya, hero, false);
    kalya_move_back_to_hero_event:AddEventLinkAtEnd("End of rain dialogue");
    EventManager:RegisterEvent(kalya_move_back_to_hero_event);

    event = vt_map.ScriptedEvent("End of rain dialogue", "end_of_rain_dialogue", "");
    EventManager:RegisterEvent(event);

end

-- zones
local to_cave2_1_zone = {};
local to_cave2_2_zone = {};
local to_cave3_1_zone = {};
local to_cave3_2_zone = {};
local to_path1_zone = {};
local to_path3_zone = {};
local to_path3_bis_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_cave2_1_zone = vt_map.CameraZone(26, 30, 53, 55);
    Map:AddZone(to_cave2_1_zone);
    to_cave2_2_zone = vt_map.CameraZone(18, 22, 37, 39);
    Map:AddZone(to_cave2_2_zone);
    to_cave3_1_zone = vt_map.CameraZone(46, 50, 13, 15);
    Map:AddZone(to_cave3_1_zone);
    to_cave3_2_zone = vt_map.CameraZone(22, 26, 7, 9);
    Map:AddZone(to_cave3_2_zone);

    to_path1_zone = vt_map.CameraZone(78, 80, 13, 30);
    Map:AddZone(to_path1_zone);
    to_path3_zone = vt_map.CameraZone(29, 48, 0, 2);
    Map:AddZone(to_path3_zone);
    to_path3_bis_zone = vt_map.CameraZone(0, 9, 0, 2);
    Map:AddZone(to_path3_bis_zone);

end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_cave2_1_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to cave 2-1");
    elseif (to_cave2_2_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to cave 2-2");
    elseif (to_cave3_1_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to cave 3-1");
    elseif (to_cave3_2_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to cave 3-2");
    elseif (to_path1_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain path 1");
    elseif (to_path3_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain path 3");
    elseif (to_path3_bis_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain path 3bis");
    end
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    -- default values
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_background.png");
    enemy:AddBattleScript("dat/maps/common/at_night.lua");
    -- Adds the rain right away as its starting on the first entrance in this map.
    enemy:AddBattleScript("dat/maps/common/rain_in_battles_script.lua");
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 1) then
        enemy:AddBattleScript("dat/maps/common/soft_lightnings_script.lua");
    end
end

-- Map Custom functions
-- Used through scripted events

-- Effect time used when applying the heal light effect
local heal_effect_time = 0;
local heal_color = vt_video.Color(0.0, 0.0, 1.0, 1.0);

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

    rain_dialogue_start = function()
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

        kalya_move_next_to_hero_event:SetDestination(hero:GetXPosition(), hero:GetYPosition() - 2.0, false);
        orlinn_move_next_to_hero_event:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition() - 2.0, false);

        Map:PushState(vt_map.MapMode.STATE_SCENE);

        -- Actually start the rain
        Map:GetParticleManager():AddParticleEffect("dat/effects/particles/rain.lua", 512.0, 768.0);
    end,

    end_of_rain_dialogue = function()
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
        GlobalManager:SetEventValue("story", "mt_elbrus_weather_level", 1);

        -- Place an omni ambient sound at the center of the map to add a nice rainy effect.
        local rainy_sound = vt_map.SoundObject("mus/Ove Melaa - Rainy.ogg", 20.0, 16.0, 100.0);
        Map:AddAmbientSoundObject(rainy_sound);
    end,
}
