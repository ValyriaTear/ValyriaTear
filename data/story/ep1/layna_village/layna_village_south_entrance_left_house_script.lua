-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_south_entrance_left_house_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "data/story/common/locations/mountain_village.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

-- the main character handler
local bronann = nil

-- the main map loading code
function Load(m)

    Map = m;
    EventManager = Map:GetEventSupervisor();

    Map:SetUnlimitedStamina(true);

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on bronann
    Map:SetCamera(bronann);

    _CreateEvents();
    _CreateZones();

    -- The only entrance close door sound
    AudioManager:PlaySound("data/sounds/door_close.wav");
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    bronann = CreateSprite(Map, "Bronann", 28, 30, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.NORTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
end

function _CreateObjects()
    local object = nil

    CreateObject(Map, "Bed1", 34, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Box1", 29, 17, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Chair1", 25, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Small Wooden Table", 22, 25, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Box1", 22, 27, vt_map.MapMode.GROUND_OBJECT);

    object = CreateObject(Map, "Paper and Feather", 22, 23, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true); -- Above any other ground object

    object = CreateObject(Map, "Left Window Light", 21, 23, vt_map.MapMode.GROUND_OBJECT);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    object:SetDrawOnSecondPass(true); -- Above any other ground object

    CreateObject(Map, "Flower Pot1", 35, 26, vt_map.MapMode.GROUND_OBJECT);

    object = CreateObject(Map, "Right Window Light", 35, 23, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    -- Triggered events
    vt_map.MapTransitionEvent.Create("exit floor", "data/story/ep1/layna_village/layna_village_south_entrance_map.lua",
                                     "data/story/ep1/layna_village/layna_village_south_entrance_script.lua", "from left house");
end

-- zones
local room_exit_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    room_exit_zone = vt_map.CameraZone.Create(26, 30, 33, 34);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (room_exit_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("exit floor");
        AudioManager:PlaySound("data/sounds/door_open2.wav");
    end
end

-- Map Custom functions
-- Used through scripted events
map_functions = {

}
