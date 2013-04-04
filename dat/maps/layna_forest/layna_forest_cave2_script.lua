-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_cave2_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest Cave"
map_image_filename = "img/menus/locations/desert_cave.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/shrine-OGA-yd.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- the main character handler
local hero = {};

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

    -- Add a mediumly dark overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 58, 94);
    hero:SetDirection(hoa_map.MapMode.NORTH);
    hero:SetMovementSpeed(hoa_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from layna forest crystal") then
        hero:SetDirection(hoa_map.MapMode.SOUTH);
        hero:SetPosition(69, 4);
    end

    Map:AddGroundObject(hero);
end


function _CreateObjects()
    local object = {};
    local npc = {};
    local event = {}

    -- Adapt the light color according to the time of the day.
    local light_color_red = 1.0;
    local light_color_green = 1.0;
    local light_color_blue = 1.0;
    local light_color_alpha = 0.8;
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        local tw_value = GlobalManager:GetEventValue("story", "layna_forest_twilight_value");
        if (tw_value >= 4 and tw_value < 6) then
            light_color_red = 0.83;
            light_color_green = 0.72;
            light_color_blue = 0.70;
            light_color_alpha = 0.29;
        elseif (tw_value >= 6 and tw_value < 8) then
            light_color_red = 0.62;
            light_color_green = 0.50;
            light_color_blue = 0.59;
            light_color_alpha = 0.49;
        elseif (tw_value >= 8) then
            light_color_red = 0.30;
            light_color_green = 0.30;
            light_color_blue = 0.46;
            light_color_alpha = 0.60;
        end
    end

    -- Add a halo showing the cave entrance
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 58, 104,
            hoa_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 67, 5,
            hoa_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));

    local chest1 = CreateTreasure(Map, "layna_forest_cave2_chest1", "Wood_Chest1", 107, 28);
    if (chest1 ~= nil) then
        chest1:AddObject(1, 2);
        chest1:AddObject(11, 2);
        Map:AddGroundObject(chest1);
    end
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    enemy:AddBattleScript("dat/battles/desert_cave_battle_anim.lua");
end

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

    -- Hint: left, right, top, bottom
    roam_zone = hoa_map.EnemyZone(12, 18, 17, 31);

    enemy = CreateEnemySprite(Map, "slime");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1); -- green Slime
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(6); -- bat
    enemy:AddEnemy(1);
    roam_zone:AddEnemy(enemy, Map, 1);

    Map:AddZone(roam_zone);

    -- Hint: left, right, top, bottom
    roam_zone = hoa_map.EnemyZone(94, 102, 62, 70);

    enemy = CreateEnemySprite(Map, "bat");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1); -- green slime
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:AddEnemy(1);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(1);
    enemy:AddEnemy(6); -- bat
    enemy:AddEnemy(1);
    roam_zone:AddEnemy(enemy, Map, 1);

    Map:AddZone(roam_zone);

    -- Hint: left, right, top, bottom
    roam_zone = hoa_map.EnemyZone(62, 71, 36, 44);

    enemy = CreateEnemySprite(Map, "big slime");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1); -- green slime
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    roam_zone:AddEnemy(enemy, Map, 1);

    Map:AddZone(roam_zone);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- Map events
    event = hoa_map.MapTransitionEvent("to forest SE", "dat/maps/layna_forest/layna_forest_south_east.lua",
                                       "dat/maps/layna_forest/layna_forest_south_east.lua", "from layna forest cave 2")
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to forest crystal", "dat/maps/layna_forest/layna_forest_crystal_map.lua",
                                       "dat/maps/layna_forest/layna_forest_crystal_script.lua", "from layna forest cave 2")
    EventManager:RegisterEvent(event);

    -- Fade in/out events - to making the character wander from a cave place to another
    event = hoa_map.ScriptedEvent("to 1-1", "to_1_1", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 1-2", "to_1_2", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 2-1", "to_2_1", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 2-2", "to_2_2", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 3-1", "to_3_1", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 3-2", "to_3_2", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 4-1", "to_4_1", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 4-2", "to_4_2", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 5-1", "to_5_1", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 5-2", "to_5_2", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 6-1", "to_6_1", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 6-2", "to_6_2", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 7-1", "to_7_1", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 7-2", "to_7_2", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 8-1", "to_8_1", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 8-2", "to_8_2", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 9-1", "to_9_1", "a_to_b_update");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("to 9-2", "to_9_2", "a_to_b_update");
    EventManager:RegisterEvent(event);
end


-- zones
local to_forest_SE_zone = {};
local to_forest_crystal_zone = {};
local to_1_1_zone = {};
local to_1_2_zone = {};
local to_2_1_zone = {};
local to_2_2_zone = {};
local to_3_1_zone = {};
local to_3_2_zone = {};
local to_4_1_zone = {};
local to_4_2_zone = {};
local to_5_1_zone = {};
local to_5_2_zone = {};
local to_6_1_zone = {};
local to_6_2_zone = {};
local to_7_1_zone = {};
local to_7_1_bis_zone = {};
local to_7_2_zone = {};
local to_8_1_zone = {};
local to_8_2_zone = {};
local to_9_1_zone = {};
local to_9_2_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_forest_SE_zone = hoa_map.CameraZone(56, 60, 95, 96);
    Map:AddZone(to_forest_SE_zone);

    to_forest_crystal_zone = hoa_map.CameraZone(60, 74, 0, 1);
    Map:AddZone(to_forest_crystal_zone);

    -- cave zones
    to_1_1_zone = hoa_map.CameraZone(8, 10, 39, 40);
    Map:AddZone(to_1_1_zone);

    to_1_2_zone = hoa_map.CameraZone(42, 44, 83, 84);
    Map:AddZone(to_1_2_zone);

    to_2_1_zone = hoa_map.CameraZone(28, 30, 43, 44);
    Map:AddZone(to_2_1_zone);

    to_2_2_zone = hoa_map.CameraZone(36, 38, 3, 4);
    Map:AddZone(to_2_2_zone);

    to_3_1_zone = hoa_map.CameraZone(120, 122, 89, 90);
    Map:AddZone(to_3_1_zone);

    to_3_2_zone = hoa_map.CameraZone(52, 54, 13, 14);
    Map:AddZone(to_3_2_zone);

    to_4_1_zone = hoa_map.CameraZone(46, 48, 65, 66);
    Map:AddZone(to_4_1_zone);

    to_4_2_zone = hoa_map.CameraZone(92, 94, 45, 46);
    Map:AddZone(to_4_2_zone);

    to_5_1_zone = hoa_map.CameraZone(76, 78, 7, 8);
    Map:AddZone(to_5_1_zone);

    to_5_2_zone = hoa_map.CameraZone(76, 78, 25, 26);
    Map:AddZone(to_5_2_zone);

    to_6_1_zone = hoa_map.CameraZone(2, 4, 73, 74);
    Map:AddZone(to_6_1_zone);

    to_6_2_zone = hoa_map.CameraZone(66, 68, 79, 80);
    Map:AddZone(to_6_2_zone);

    to_7_1_zone = hoa_map.CameraZone(36, 38, 71, 72);
    Map:AddZone(to_7_1_zone);

    to_7_1_bis_zone = hoa_map.CameraZone(96, 98, 23, 24);
    Map:AddZone(to_7_1_bis_zone);

    to_7_2_zone = hoa_map.CameraZone(26, 28, 81, 82);
    Map:AddZone(to_7_2_zone);

    to_8_1_zone = hoa_map.CameraZone(2, 4, 3, 4);
    Map:AddZone(to_8_1_zone);

    to_8_2_zone = hoa_map.CameraZone(22, 24, 57, 58);
    Map:AddZone(to_8_2_zone);

    to_9_1_zone = hoa_map.CameraZone(118, 120, 7, 8);
    Map:AddZone(to_9_1_zone);

    to_9_2_zone = hoa_map.CameraZone(78, 80, 69, 70);
    Map:AddZone(to_9_2_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_forest_SE_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest SE");
    elseif (to_forest_crystal_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest crystal");
    elseif (to_1_1_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 1-1");
    elseif (to_1_2_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 1-2");
    elseif (to_2_1_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 2-1");
    elseif (to_2_2_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 2-2");
    elseif (to_3_1_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 3-1");
    elseif (to_3_2_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 3-2");
    elseif (to_4_1_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 4-1");
    elseif (to_4_2_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 4-2");
    elseif (to_5_1_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 5-1");
    elseif (to_5_2_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 5-2");
    elseif (to_6_1_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 6-1");
    elseif (to_6_2_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 6-2");
    elseif (to_7_1_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 7-1");
    elseif (to_7_1_bis_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 7-1");
    elseif (to_7_2_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 7-2");
    elseif (to_8_1_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 8-1");
    elseif (to_8_2_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 8-2");
    elseif (to_9_1_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 9-1");
    elseif (to_9_2_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to 9-2");
    end
end

-- Keeps track of time elapsed during fade effcts
local fade_effect_time = 0.0;
-- Tells where the hero character will reappear
local destination_x = 0.0;
local destination_y = 0.0;
local destination_orientation = {};
local destination_set = false;

-- Map Custom functions
-- Used through scripted events
map_functions = {
    to_1_1 = function()
        fade_effect_time = 0.0;

        destination_x = 44;
        destination_y = 86;
        destination_orientation = hoa_map.MapMode.SOUTH;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_1_2 = function()
        fade_effect_time = 0.0;

        destination_x = 11;
        destination_y = 41;
        destination_orientation = hoa_map.MapMode.EAST;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_2_1 = function()
        fade_effect_time = 0.0;

        destination_x = 36;
        destination_y = 6;
        destination_orientation = hoa_map.MapMode.SOUTH;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_2_2 = function()
        fade_effect_time = 0.0;

        destination_x = 30;
        destination_y = 46;
        destination_orientation = hoa_map.MapMode.SOUTH;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_3_1 = function()
        fade_effect_time = 0.0;

        destination_x = 51;
        destination_y = 16;
        destination_orientation = hoa_map.MapMode.SOUTH;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_3_2 = function()
        fade_effect_time = 0.0;

        destination_x = 118;
        destination_y = 90;
        destination_orientation = hoa_map.MapMode.WEST;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_4_1 = function()
        fade_effect_time = 0.0;

        destination_x = 95;
        destination_y = 48;
        destination_orientation = hoa_map.MapMode.SOUTH;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_4_2 = function()
        fade_effect_time = 0.0;

        destination_x = 50;
        destination_y = 67;
        destination_orientation = hoa_map.MapMode.EAST;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_5_1 = function()
        fade_effect_time = 0.0;

        destination_x = 76;
        destination_y = 28;
        destination_orientation = hoa_map.MapMode.SOUTH;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_5_2 = function()
        fade_effect_time = 0.0;

        destination_x = 74;
        destination_y = 9;
        destination_orientation = hoa_map.MapMode.WEST;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_6_1 = function()
        fade_effect_time = 0.0;

        destination_x = 66;
        destination_y = 82;
        destination_orientation = hoa_map.MapMode.SOUTH;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_6_2 = function()
        fade_effect_time = 0.0;

        destination_x = 4;
        destination_y = 76;
        destination_orientation = hoa_map.MapMode.EAST;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_7_1 = function()
        fade_effect_time = 0.0;

        destination_x = 24;
        destination_y = 82;
        destination_orientation = hoa_map.MapMode.WEST;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_7_2 = function()
        fade_effect_time = 0.0;

        destination_x = 34.5;
        destination_y = 71.5;
        destination_orientation = hoa_map.MapMode.WEST;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_8_1 = function()
        fade_effect_time = 0.0;

        destination_x = 24;
        destination_y = 59;
        destination_orientation = hoa_map.MapMode.SOUTH;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_8_2 = function()
        fade_effect_time = 0.0;

        destination_x = 4;
        destination_y = 6;
        destination_orientation = hoa_map.MapMode.SOUTH;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_9_1 = function()
        fade_effect_time = 0.0;

        destination_x = 80;
        destination_y = 72;
        destination_orientation = hoa_map.MapMode.EAST;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    to_9_2 = function()
        fade_effect_time = 0.0;

        destination_x = 119;
        destination_y = 10;
        destination_orientation = hoa_map.MapMode.SOUTH;
        destination_set = false;

        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    a_to_b_update = function()
        fade_effect_time = fade_effect_time + SystemManager:GetUpdateTime();

        if (fade_effect_time < 200.0) then
            Map:GetEffectSupervisor():EnableLightingOverlay(hoa_video.Color(0.0, 0.0, 0.0, fade_effect_time / 200.0));
            return false;
        end

        if (fade_effect_time < 1000.0) then
            -- Once the fade out is done, move the character to its new place.
            if (destination_set == false) then
                hero:SetPosition(destination_x, destination_y);
                hero:SetDirection(destination_orientation);
                destination_set = true;
            end

            Map:GetEffectSupervisor():EnableLightingOverlay(hoa_video.Color(0.0, 0.0, 0.0, ((1000.0 - fade_effect_time) / 200.0)));
            return false;
        end

        Map:PopState();
        return true;
    end

}
