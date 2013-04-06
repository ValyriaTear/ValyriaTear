-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_riverbank_house_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = "Lilly's house"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- the main character handler
local bronann = {};

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

    Map.unlimited_stamina = true;

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on bronann
    Map:SetCamera(bronann);

    _CreateEvents();
    _CreateZones();

    -- The only entrance close door sound
    AudioManager:PlaySound("snd/door_close.wav");
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    bronann = CreateSprite(Map, "Bronann", 32, 45);
    bronann:SetDirection(vt_map.MapMode.NORTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    Map:AddGroundObject(bronann);
end

function _CreateObjects()
    object = {}

    -- Bronann's room
    object = CreateObject(Map, "Bed1", 42, 27);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Box1", 23, 18);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Chair1_inverted", 39, 20);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Small Wooden Table", 42, 21);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Candle1", 43, 19);
    object:SetDrawOnSecondPass(true); -- Above the table
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Left Window Light", 20, 38);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Clock1", 33, 13);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Table1", 39, 42);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Chair1_inverted", 35, 41);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Chair1", 43, 40);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Bench2", 39, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Barrel1", 21, 36);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Barrel1", 22, 37);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Barrel1", 21, 41);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Barrel1", 23, 40);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Barrel1", 25, 39);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Box1", 21, 39);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Flower Pot1", 25, 16);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Right Window Light", 44, 38);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    if (object ~= nil) then Map:AddGroundObject(object) end;
end


-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- Triggered events
    event = vt_map.MapTransitionEvent("exit floor", "dat/maps/layna_village/layna_village_riverbank_map.lua",
                                       "dat/maps/layna_village/layna_village_riverbank_script.lua", "from_riverbank_house");
    EventManager:RegisterEvent(event);

end

-- zones
local room_exit_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    room_exit_zone = vt_map.CameraZone(30, 34, 47, 48);
    Map:AddZone(room_exit_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (room_exit_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("exit floor");
        AudioManager:PlaySound("snd/door_open2.wav");
    end
end


-- Map Custom functions
-- Used through scripted events
map_functions = {

}
