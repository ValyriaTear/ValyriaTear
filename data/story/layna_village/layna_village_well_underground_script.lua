-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_well_underground_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "data/story/common/locations/mountain_village.png"
map_subname = "Well's undergrounds"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/shrine-OGA-yd.ogg"

-- c++ objects instances
local Map = nil
local Script = nil
local EventManager = nil

-- Characters handler
local bronann = nil
local olivia = nil

-- the main map loading code
function Load(m)

    Map = m;
    Script = Map:GetScriptSupervisor()
    EventManager = Map:GetEventSupervisor()

    -- Put last to get a proper night effect
    Script:AddScript("data/story/common/at_night.lua")


    _CreateCharacters()
    _CreateObjects()

    -- Set the camera focus on bronann
    Map:SetCamera(bronann)

    _CreateEvents()
    _CreateZones()
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones()

    _UpdateLampLocation()
end

-- Character creation
function _CreateCharacters()
    bronann = CreateSprite(Map, "Bronann", 18, 2, vt_map.MapMode.GROUND_OBJECT)
    bronann:SetDirection(vt_map.MapMode.NORTH)
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED)

    olivia = CreateNPCSprite(Map, "Girl1", vt_system.Translate("Olivia"), 18, 2, vt_map.MapMode.GROUND_OBJECT)
    olivia:SetDirection(vt_map.MapMode.SOUTH)
    olivia:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED)
end

local oil_lamp = nil

function _CreateObjects()
    local object = nil

    oil_lamp = CreateObject(Map, "Oil Lamp", 15, 14, vt_map.MapMode.GROUND_OBJECT);

end

function _UpdateLampLocation()
    -- TODO: Sets the lamp location depending on the character's location
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Triggered events
    vt_map.MapTransitionEvent.Create("exit floor", "data/story/layna_village/layna_village_center_map.lua",
                                     "data/story/layna_village/layna_village_center_script.lua", "from_well_undergrounds")

    -- Generic events
    vt_map.LookAtSpriteEvent.Create("Olivia looks at Bronann", olivia, bronann)

    if (GlobalManager:DoesEventExist("story", "well_intro_event_done") == false) then
        event = vt_map.PathMoveSpriteEvent.Create("Olivia goes near the fountain", olivia, 10, 14, false)
        event:AddEventLinkAtEnd("Olivia looks at Bronann")

        event = vt_map.PathMoveSpriteEvent.Create("Bronann goes near Olivia", bronann, 12, 14, false)
        --event:AddEventLinkAtEnd("Olivia looks at Bronann")
    end
end

-- zones
local room_exit_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    room_exit_zone = vt_map.CameraZone.Create(16, 18, 0, 1)
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (room_exit_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("exit floor")
    end
end


-- Map Custom functions
-- Used through scripted events

map_functions = {


}
