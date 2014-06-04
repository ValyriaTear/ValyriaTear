-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine_3rd_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus Shrine"
map_image_filename = "img/menus/locations/mountain_shrine.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/mountain_shrine.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};
local Script = {};

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
    Script = Map:GetScriptSupervisor();

    Map.unlimited_stamina = false;

    _CreateCharacters();
    _CreateObjects();
    _CreateEnemies()

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    -- The player incarnates Orlinn in this map.
    -- Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay
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
    hero = CreateSprite(Map, "Orlinn", 32.0, 45.0);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.FAST_SPEED);
    Map:AddGroundObject(hero);

    -- The menu is disabled in this map
    Map:SetMenuEnabled(false);
end

-- The trigger state at map load time.
-- Used to restore it, would Orlinn be caught.
local trigger_state = 0;

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}

    _add_flame(39.5, 7);
    _add_flame(25.5, 7);
end

function _add_flame(x, y)
    local object = vt_map.SoundObject("snd/campfire.ogg", x, y, 10.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;

    object = CreateObject(Map, "Flame1", x, y);
    Map:AddGroundObject(object);

    Map:AddHalo("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    Map:AddHalo("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to mountain shrine stairs", "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_script.lua", "from_shrine_third_floor");
    EventManager:RegisterEvent(event);

end

-- zones
local to_shrine_stairs_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_stairs_zone = vt_map.CameraZone(30, 34, 46, 48);
    Map:AddZone(to_shrine_stairs_zone);

end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_stairs_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine stairs");
    end

end

-- Map Custom functions
-- Used through scripted events
map_functions = {

}
