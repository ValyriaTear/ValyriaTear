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

        { "Tree Small1 snow", 7, 66 },
        { "Tree Small1 snow", 50, 63 },
        { "Tree Small2 snow", 19, 5 },
        { "Tree Small1 snow", 23, 7 },
        { "Tree Big2 snow", 39, 56 },
        { "Tree Big2 snow", 2, 70 },
        { "Tree Big2 snow", 36, 72 },
        { "Tree Big1 snow", 0, 75 },
        { "Tree Big1 snow", 48, 70 },
        { "Rock1 snow", 32, 56 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_objects) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        Map:AddGroundObject(object);
    end

    -- Create the bridge
    object = CreateObject(Map, "Bridge1_up", 10, 9);
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

    object = CreateObject(Map, "Bridge1_down", 10, 36);
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
    to_shrine_nw_zone = vt_map.CameraZone(5, 9, 3, 5);
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
