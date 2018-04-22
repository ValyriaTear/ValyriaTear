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

    _CreateCharacters()
    _CreateObjects()

    -- Put last to get a proper night effect
    Script:AddScript("data/story/common/lost_in_darkness.lua");

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
local lamp_halo = nil
local lamp_flare = nil

function _CreateObjects()
    local object = nil

    oil_lamp = CreateObject(Map, "Oil Lamp", 15, 14, vt_map.MapMode.GROUND_OBJECT)
    local lamp_x_pos = oil_lamp:GetXPosition()
    local lamp_y_pos = oil_lamp:GetYPosition()

    lamp_halo = vt_map.Halo.Create("data/visuals/lights/torch_light_mask2.lua", lamp_x_pos, lamp_y_pos,
        vt_video.Color(0.9, 0.9, 0.4, 0.5))
    lamp_flare = vt_map.Halo.Create("data/visuals/lights/sun_flare_light_main.lua", lamp_x_pos, lamp_y_pos,
        vt_video.Color(0.99, 1.0, 0.27, 0.2))
    _SetLampPosition(lamp_x_pos, lamp_y_pos)

    -- hide it when the intro event is not done yet.
    if (GlobalManager:DoesEventExist("story", "well_intro_event_done") == false) then
        oil_lamp:SetVisible(false)
        lamp_halo:SetVisible(false)
        lamp_flare:SetVisible(false)
    end

end

local lamp_character = nil

function _UpdateLampLocation()
    -- Stick the lamp to a given character reference
    -- if the reference is nil, just let the lamp where it is.
    if (lamp_character == nil) then
        return
    end

    -- Sets the lamp location depending on the character's location and orientation
    local character_direction = lamp_character:GetDirection()
    local x_pos = lamp_character:GetXPosition()
    local y_pos = lamp_character:GetYPosition()
    local x_offset = 0.0
    local y_offset = 0.0
    if (character_direction == vt_map.MapMode.WEST) then
        x_offset = -0.5
        y_offset = -0.5
    elseif (character_direction == vt_map.MapMode.EAST) then
        x_offset = 0.5
        y_offset = -0.5
    elseif (character_direction == vt_map.MapMode.NORTH) then
        x_offset = -0.5
        y_offset = -0.5
    elseif (character_direction == vt_map.MapMode.SOUTH) then
        x_offset = 0.5
        y_offset = -0.5
    end

    _SetLampPosition(x_pos + x_offset, y_pos + y_offset)
end

-- Handles the fixed offsets betwwen the oil_lamp object, and the related ones
function _SetLampPosition(x, y)
    oil_lamp:SetPosition(x, y)
    lamp_halo:SetPosition(x, y + 4.0)
    lamp_flare:SetPosition(x, y + 2.0)
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Triggered events
    vt_map.MapTransitionEvent.Create("exit floor", "data/story/layna_village/layna_village_center_map.lua",
                                     "data/story/layna_village/layna_village_center_script.lua", 
                                     "from_well_undergrounds")

    -- Generic events
    vt_map.LookAtSpriteEvent.Create("Olivia looks at Bronann", olivia, bronann)
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Olivia", bronann, olivia)

    if (GlobalManager:DoesEventExist("story", "well_intro_event_done") == false) then
        event = vt_map.ScriptedEvent.Create("Well undergrounds scene start", "well_intro_scene_start", "")
        event:AddEventLinkAtEnd("Olivia takes a few steps")

        event = vt_map.PathMoveSpriteEvent.Create("Olivia takes a few steps", olivia, 18, 12, false)
        event:AddEventLinkAtEnd("Olivia shows the oil lamp", 1000)

        event = vt_map.ScriptedEvent.Create("Olivia shows the oil lamp", "oil_lamp_move_start", "oil_lamp_move_update")
        event:AddEventLinkAtEnd("Olivia lights the oil lamp", 1000)

        event = vt_map.ScriptedEvent.Create("Olivia lights the oil lamp", "oil_lamp_light", "")
        event:AddEventLinkAtEnd("Olivia looks at Bronann")
        event:AddEventLinkAtEnd("Olivia goes near the fountain", 2000)

        event = vt_map.PathMoveSpriteEvent.Create("Olivia goes near the fountain", olivia, 10, 14, false)
        event:AddEventLinkAtEnd("Olivia looks at Bronann")

        event = vt_map.PathMoveSpriteEvent.Create("Bronann goes near Olivia", bronann, 12, 14, false)
        --event:AddEventLinkAtEnd("Olivia looks at Bronann")

        EventManager:StartEvent("Well undergrounds scene start");
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

oil_lamp_move_y_pos = 0
oil_lamp_move_y_pos_end = 0

map_functions = {

    well_intro_scene_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE)
        bronann:SetMoving(false)
    end,

    -- make oil lamp appear in front of Olivia
    oil_lamp_move_start = function()
        oil_lamp_move_y_pos = olivia:GetYPosition() - 1.0
        _SetLampPosition(olivia:GetXPosition() + 0.5, oil_lamp_move_y_pos)
        oil_lamp:SetVisible(true)
        oil_lamp_move_y_pos_end = oil_lamp_move_y_pos + 0.5
    end,

    oil_lamp_move_update = function()
        oil_lamp_move_y_pos = oil_lamp_move_y_pos + SystemManager:GetUpdateTime() / 200
        _SetLampPosition(olivia:GetXPosition() + 0.5, oil_lamp_move_y_pos)
        if (oil_lamp_move_y_pos < oil_lamp_move_y_pos_end) then
            return false
        end
        -- Stick the lamp to Olivia
        lamp_character = olivia
        return true
    end,

    oil_lamp_light = function()
        lamp_flare:SetVisible(true)
        lamp_halo:SetVisible(true)
    end,

    well_intro_scene_end = function()
        Map:PopState()
    end,
}
