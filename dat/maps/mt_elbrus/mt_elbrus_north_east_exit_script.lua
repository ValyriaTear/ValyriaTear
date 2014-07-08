-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_north_east_exit_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "img/menus/locations/mt_elbrus.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "snd/wind.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- the main character handler
local hero = nil

local bronann = nil
local kalya = nil
local orlinn = nil

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

    Map.unlimited_stamina = false;

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- The far hills background
    Map:GetScriptSupervisor():AddScript("dat/maps/mt_elbrus/mt_elbrus_landscape_anim.lua");

end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 30, 16);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_basement") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(30.0, 16.0);
    end

    Map:AddGroundObject(hero);
end

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}


    -- Objects array
    local map_objects = {

        { "Tree Small1 snow", 35, 28 },
        { "Tree Small1 snow", 3, 46 },
        { "Tree Small1 snow", 18, 17 },
        { "Tree Small1 snow", 23, 30 },
        { "Tree Small1 snow", 18, 32 },
        { "Tree Small1 snow", 15, 34 },
        { "Tree Small1 snow", 52, 6 },
        { "Tree Small2 snow", 39, 26 },
        { "Tree Small2 snow", 11, 35 },

        { "Tree Big2 snow", 17, 40 },
        { "Tree Big2 snow", 9, 39 },
        { "Tree Big1 snow", 13, 20 },
        { "Tree Big2 snow", 13, 47 },
        { "Tree Big1 snow", 2, 34 },
        { "Tree Big1 snow", 19, 7 },
        { "Tree Big1 snow", 21, 41 },
        { "Tree Big2 snow", 24, 15 },
        { "Tree Big2 snow", 9, 29 },

        { "Rock1 snow", 20, 13 },
        { "Rock1 snow", 40, 30 },
        { "Rock1 snow", 12, 41 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_objects) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        Map:AddGroundObject(object);
    end

end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    --event = vt_map.MapTransitionEvent("to mountain shrine basement", "dat/maps/mt_elbrus/mt_elbrus_shrine_basement_map.lua",
    --                                  "dat/maps/mt_elbrus/mt_elbrus_shrine_basement_script.lua", "from_shrine_north_exit");
    --EventManager:RegisterEvent(event);

    --event = vt_map.MapTransitionEvent("to overworld", "dat/maps/xx/xx_map.lua",
    --                                  "dat/maps/xx/xx_script.lua", "from_shrine_north_exit");
    --EventManager:RegisterEvent(event);

    -- Starts the episode I ending scene...
    event = vt_map.ScriptedEvent("Heroes see the village", "village_scene_start", "");
    EventManager:RegisterEvent(event);
end

-- zones
local to_basement_zone = {};
local to_overworld_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_basement_zone = vt_map.CameraZone(28, 32, 13, 15);
    Map:AddZone(to_basement_zone);
    to_overworld_zone = vt_map.CameraZone(0, 2, 34, 44);
    Map:AddZone(to_overworld_zone);

end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_basement_zone:IsCameraEntering() == true) then
        hero:SetMoving(true);
        hero:SetDirection(vt_map.MapMode.NORTH);
        --EventManager:StartEvent("to mountain shrine basement");
    elseif (to_overworld_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.WEST);
        --EventManager:StartEvent("to overworld");
    end
end

-- Map Custom functions
-- Used through scripted events
map_functions = {

    village_scene_start = function()

    end,


}
