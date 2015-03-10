-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_center_sophia_house_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "music/Caketown_1-OGA-mat-pablo.ogg"

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
    AudioManager:PlaySound("sounds/door_close.wav");
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    bronann = CreateSprite(Map, "Bronann", 28, 15, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.SOUTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
end

function _CreateObjects()
    -- Bronann's room
    local object = nil

    CreateObject(Map, "Bed1", 34, 28, vt_map.MapMode.GROUND_OBJECT);

    local chest = CreateTreasure(Map, "sophia_house_chest", "Wood_Chest1", 21, 22, vt_map.MapMode.GROUND_OBJECT);
    chest:AddItem(1, 1);

    CreateObject(Map, "Chair1_inverted", 31, 19, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Small Wooden Table", 34, 20, vt_map.MapMode.GROUND_OBJECT);

    object = CreateObject(Map, "Left Window Light", 21, 21, vt_map.MapMode.GROUND_OBJECT);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    object:SetDrawOnSecondPass(true); -- Above any other ground object

    CreateObject(Map, "Flower Pot2", 21, 24, vt_map.MapMode.GROUND_OBJECT);

    object = CreateObject(Map, "Right Window Light", 35, 21, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
end


-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Triggered events
    vt_map.MapTransitionEvent.Create("exit floor", "dat/maps/layna_village/layna_village_center_map.lua",
                                     "dat/maps/layna_village/layna_village_center_script.lua", "from sophia's house");
end

-- zones
local room_exit_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    room_exit_zone = vt_map.CameraZone.Create(26, 30, 12, 13);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (room_exit_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("exit floor");
        AudioManager:PlaySound("sounds/door_open2.wav");
    end
end

-- Map Custom functions
-- Used through scripted events
map_functions = {

}
