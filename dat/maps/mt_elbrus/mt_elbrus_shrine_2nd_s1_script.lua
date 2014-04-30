-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine_2nd_s1_script = ns;
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
local hero = {};

-- Name of the main sprite. Used to reload the good one at the end of dialogue events.
local main_sprite_name = "";

-- An actual array of objects
local bridge_middle_parts = {}

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

    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/snow_fog.png", 30.0, 10.0, true);
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");

    -- Make the snow start
    Map:GetParticleManager():AddParticleEffect("dat/effects/particles/snow.lua", 512.0, 384.0);

    -- The mountain cliff background
    Map:GetScriptSupervisor():AddScript("dat/maps/mt_elbrus/mt_elbrus_background_anim.lua");

end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 86, 8);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_path3") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(10.0, 90.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_entrance") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(40.0, 7.0);
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

        { "Tree Small1 snow", 81.5, 20 },
        { "Tree Small1 snow", 80, 28 },
        { "Tree Small2 snow", 71, 72 },
        { "Tree Small1 snow", 60, 73 },
        { "Tree Small1 snow", 86, 56 },
        { "Tree Small1 snow", 62, 56.5 },
        { "Tree Small2 snow", 51, 50 },
        { "Tree Small2 snow", 49, 45 },
        { "Tree Small1 snow", 40, 41 },
        { "Tree Small2 snow", 36, 46 },
        { "Tree Small2 snow", 67, 51 },
        { "Tree Small2 snow", 47, 70 },
        { "Tree Small1 snow", 60, 52.2 },
        { "Tree Small2 snow", 29, 25 },
        { "Tree Small1 snow", 39, 26 },
        { "Tree Small1 snow", 8, 71 },
        { "Tree Small1 snow", 7, 64 },
        { "Tree Small1 snow", 82, 11 },
        { "Tree Small1 snow", 26, 3 },

        { "Tree Big2 snow", 79, 12 },
        { "Tree Big1 snow", 96.5, 29 },
        { "Tree Big2 snow", 90, 51 },
        { "Tree Big2 snow", 93, 12 },
        { "Tree Big1 snow", 79, 42 },
        { "Tree Big2 snow", 96, 37 },
        { "Tree Big1 snow", 78, 51 },
        { "Tree Big2 snow", 82, 63 },
        { "Tree Big1 snow", 66, 65 },
        { "Tree Big1 snow", 8, 61 },
        { "Tree Big1 snow", 12, 51 },
        { "Tree Big2 snow", 15, 55 },
        { "Tree Big2 snow", 22.5, 65 },
        { "Tree Big2 snow", 31, 35 },
        { "Tree Big1 snow", 38, 36 },
        { "Tree Big1 snow", 15, 10 },
        { "Tree Big2 snow", 3, 9 },
        { "Tree Big2 snow", 49, 27 },

        { "Rock1 snow", 92, 18 },
        { "Rock1 snow", 91.5, 39 },
        { "Rock1 snow", 59, 47 },
        { "Rock1 snow", 42, 47.2 },
        { "Rock1 snow", 26, 67 },
        { "Rock1 snow", 24, 75 },
        { "Rock1 snow", 30, 72.1 },
        { "Rock1 snow", 28, 73.1 },
        { "Rock1 snow", 40, 72.2 },
        { "Rock1 snow", 33.5, 39 },
        { "Rock1 snow", 18, 3 },
        { "Rock1 snow", 38.5, 10 },
        { "Rock1 snow", 40, 22.5 },
        { "Rock1 snow", 94, 23 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_objects) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        Map:AddGroundObject(object);
    end

    -- Create the bridge
    object = CreateObject(Map, "Bridge1_up", 10, 11);
    Map:AddFlatGroundObject(object);

    bridge_middle_parts[1] = CreateObject(Map, "Bridge1_middle", 10, 11);
    Map:AddFlatGroundObject(bridge_middle_parts[1]);
    bridge_middle_parts[2] = CreateObject(Map, "Bridge1_middle", 10, 13);
    Map:AddFlatGroundObject(bridge_middle_parts[2]);
    bridge_middle_parts[3] = CreateObject(Map, "Bridge1_middle", 10, 15);
    Map:AddFlatGroundObject(bridge_middle_parts[3]);
    bridge_middle_parts[4] = CreateObject(Map, "Bridge1_middle", 10, 17);
    Map:AddFlatGroundObject(bridge_middle_parts[4]);
    bridge_middle_parts[5] = CreateObject(Map, "Bridge1_middle", 10, 19);
    Map:AddFlatGroundObject(bridge_middle_parts[5]);
    bridge_middle_parts[6] = CreateObject(Map, "Bridge1_middle", 10, 20);
    Map:AddFlatGroundObject(bridge_middle_parts[6]);
    bridge_middle_parts[7] = CreateObject(Map, "Bridge1_middle", 10, 22);
    Map:AddFlatGroundObject(bridge_middle_parts[7]);
    bridge_middle_parts[8] = CreateObject(Map, "Bridge1_middle", 10, 24);
    Map:AddFlatGroundObject(bridge_middle_parts[8]);
    bridge_middle_parts[9] = CreateObject(Map, "Bridge1_middle", 10, 26);
    Map:AddFlatGroundObject(bridge_middle_parts[9]);
    bridge_middle_parts[10] = CreateObject(Map, "Bridge1_middle", 10, 28);
    Map:AddFlatGroundObject(bridge_middle_parts[10]);
    bridge_middle_parts[11] = CreateObject(Map, "Bridge1_middle", 10, 30);
    Map:AddFlatGroundObject(bridge_middle_parts[11]);
    bridge_middle_parts[12] = CreateObject(Map, "Bridge1_middle", 10, 32);
    Map:AddFlatGroundObject(bridge_middle_parts[12]);
    bridge_middle_parts[13] = CreateObject(Map, "Bridge1_middle", 10, 34);
    Map:AddFlatGroundObject(bridge_middle_parts[13]);
    bridge_middle_parts[14] = CreateObject(Map, "Bridge1_middle", 10, 36);
    Map:AddFlatGroundObject(bridge_middle_parts[14]);

    object = CreateObject(Map, "Bridge1_down", 10, 38);
    Map:AddFlatGroundObject(object);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to mountain shrine 2nd floor NE", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_ne_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_ne_script.lua", "from_shrine_2nd_floor_south");
    EventManager:RegisterEvent(event);
end

-- zones
local to_shrine_ne_zone = {};
local to_grotto_zone = {};
local to_shrine_nw_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_ne_zone = vt_map.CameraZone(84, 88, 3, 5);
    Map:AddZone(to_shrine_ne_zone);
    to_shrine_nw_zone = vt_map.CameraZone(6, 10, 3, 5);
    Map:AddZone(to_shrine_nw_zone);
    to_grotto_zone = vt_map.CameraZone(33, 35, 22, 24);
    Map:AddZone(to_grotto_zone);

end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_ne_zone:IsCameraEntering() == true) then
        hero:SetMoving(true);
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine 2nd floor NE");
    elseif (to_grotto_zone:IsCameraEntering() == true) then
        --EventManager:StartEvent("Cut the bridge Event");
    elseif (to_shrine_nw_zone:IsCameraEntering() == true) then
        --hero:SetMoving(false);
        --EventManager:StartEvent("Can't go back dialogue");
    end
end

-- Map Custom functions
-- Used through scripted events
map_functions = {

}
