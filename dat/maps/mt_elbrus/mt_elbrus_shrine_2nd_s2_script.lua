-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine_2nd_s2_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "img/menus/locations/mt_elbrus.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/icy_wind.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- the main character handler
local hero = {};

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

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

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
    hero = CreateSprite(Map, "Bronann", 10, 68);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_2nd_floor_south_east") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(42.5, 48.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_2nd_floor_south_east") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(42.5, 48.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_stairs") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(6.0, 8.0);
    end

    Map:AddGroundObject(hero);
end

function _add_flame(x, y)
    local object = vt_map.SoundObject("snd/campfire.ogg", x, y, 5.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;
    object = vt_map.SoundObject("snd/campfire.ogg", x + 18.0, y, 5.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;

    object = CreateObject(Map, "Flame1", x, y);
    object:RandomizeCurrentAnimationFrame();
    Map:AddGroundObject(object);

    Map:AddHalo("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    Map:AddHalo("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}

    -- Objects array
    local map_objects = {

        { "Rock1", 25, 14 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_objects) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        Map:AddGroundObject(object);
    end

    -- Snow effects
    object = vt_map.ParticleObject("dat/maps/mt_elbrus/particles_snow_south_entrance.lua", 10, 72);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 10, 79,
                vt_video.Color(1.0, 1.0, 1.0, 0.8));

    object = vt_map.ParticleObject("dat/maps/mt_elbrus/particles_snow_south_entrance.lua", 44, 50);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 45, 57,
                vt_video.Color(1.0, 1.0, 1.0, 0.8));

    _add_flame(9.5, 3);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to mountain shrine 2nd floor South left", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s1_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s1_script.lua", "from_shrine_2nd_floor_grotto_left");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine 2nd floor South right", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s1_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s1_script.lua", "from_shrine_2nd_floor_grotto_right");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine stairs", "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_script.lua", "from_shrine_2nd_floor_grotto");
    EventManager:RegisterEvent(event);
end

-- zones
local to_shrine_se_zone = {};
local to_shrine_sw_zone = {};
local to_stairs_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_se_zone = vt_map.CameraZone(42, 46, 49, 51);
    Map:AddZone(to_shrine_se_zone);
    to_shrine_sw_zone = vt_map.CameraZone(8, 12, 70, 72);
    Map:AddZone(to_shrine_sw_zone);
    to_stairs_zone = vt_map.CameraZone(5, 7, 5, 7);
    Map:AddZone(to_stairs_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_sw_zone:IsCameraEntering() == true) then
        hero:SetMoving(true);
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine 2nd floor South left");
    elseif (to_shrine_se_zone:IsCameraEntering() == true) then
        hero:SetMoving(true);
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine 2nd floor South right");
    elseif (to_stairs_zone:IsCameraEntering() == true) then
        hero:SetMoving(true);
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine stairs");
    end
end

-- Map Custom functions
-- Used through scripted events
map_functions = {

}
