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
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");

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

function _CreateObjects()
    local object = {}
    local npc = {}

    Map:AddSavePoint(67, 37);

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
        { "Tree Big2", 17, 12 },
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

        { "Rock2", 74, 27 },
        { "Rock1", 76, 28 },

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
        { "Grass Clump1", 72, 13 },
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
--[[
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(28, 47, 22, 38);
    -- Dark soldier 3
    enemy = CreateEnemySprite(Map, "Dark Soldier");
    _SetBattleEnvironment(enemy);
    -- Add special timer script
    enemy:AddBattleScript("dat/maps/mt_elbrus/battle_with_dark_soldiers_script.lua");
    enemy:NewEnemyParty();
    enemy:AddEnemy(9);
    roam_zone:AddEnemy(enemy, Map, 1);
    roam_zone:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    enemy:AddWayPoint(29, 23);
    enemy:AddWayPoint(46, 23);
    enemy:AddWayPoint(46, 37);
    enemy:AddWayPoint(29, 37);
    Map:AddZone(roam_zone);

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(10, 21, 86, 92);
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
    enemy:AddEnemy(4);
    enemy:AddEnemy(6);
    roam_zone:AddEnemy(enemy, Map, 1);
    Map:AddZone(roam_zone);
]]--
end

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
                                       "dat/maps/mt_elbrus/mt_elbrus_path2_script.lua", "from_path2");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain path 3bis", "dat/maps/mt_elbrus/mt_elbrus_path3_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path2_script.lua", "from_path2_chest");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain path 1", "dat/maps/mt_elbrus/mt_elbrus_path1_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path1_script.lua", "from_path2");
    EventManager:RegisterEvent(event);

    -- Heal point
    event = vt_map.ScriptedEvent("Heal event", "heal_party", "heal_done");
    EventManager:RegisterEvent(event);


end

-- zones
local see_first_guard_zone = {};
local to_cave2_1_zone = {};
local to_cave2_2_zone = {};
local to_cave3_1_zone = {};
local to_cave3__2zone = {};
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
    to_path3_bis_zone = vt_map.CameraZone(0, 8, 0, 2);
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
    -- TODO: add script showing the scenery.
end

-- Map Custom functions
-- Used through scripted events

map_functions = {

}
