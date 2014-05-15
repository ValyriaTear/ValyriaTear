-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine4_script = ns;
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

-- Forest dialogue secondary hero
local orlinn = {};

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

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);

    -- Preload sounds
    AudioManager:LoadSound("snd/opening_sword_unsheathe.wav", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from shrine main room)
    hero = CreateSprite(Map, "Bronann", 60.0, 34.0);
    hero:SetDirection(vt_map.MapMode.WEST);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    Map:AddGroundObject(hero);

    orlinn = CreateSprite(Map, "Orlinn",
                          hero:GetXPosition(), hero:GetYPosition());
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);
    Map:AddGroundObject(orlinn);
end

-- Arrays of spikes used to block the way
local spike_square1 = {};
local spike_square2 = {};
local spike_square3 = {};
local spike_square4 = {};
local spike_square5 = {};
local spike_square6 = {};

-- state of spike squares
local spike1_up = true;
local spike2_up = true;
local spike3_up = true;
local spike4_up = true;
local spike5_up = true;
local spike6_up = true;

local spike_trigger1 = nil;
local spike_trigger2 = nil;
local spike_trigger3 = nil;

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}

    _add_flame(61.5, 29);

    _add_bubble(39, 34);
    _add_bubble(45, 25);
    _add_bubble(15, 27);
    _add_bubble(57, 13);
    _add_bubble(18, 15);
    _add_bubble(5, 37);
    _add_bubble(51, 38);
    _add_bubble(54, 22);

    _add_flame_pot(26, 24);
    _add_flame_pot(26, 20);
    _add_flame_pot(26, 16);
    _add_flame_pot(29, 14.3);
    _add_flame_pot(33, 14.3);

    _add_flame_pot(42, 24);
    _add_flame_pot(42, 20);
    _add_flame_pot(42, 16);

    -- Creates the spikes
    local spike_objects1 = {
        -- upper left border
        { 29, 18 },
        { 31, 18 },
        { 29, 20 },
        { 31, 20 },
    }

    -- Loads the spikes according to the array
    for my_index, my_array in pairs(spike_objects1) do
        spike_square1[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(spike_square1[my_index]);
    end

    local spike_objects2 = {
        -- upper left border
        { 33, 18 },
        { 35, 18 },
        { 33, 20 },
        { 35, 20 },
    }

    -- Loads the spikes according to the array
    for my_index, my_array in pairs(spike_objects2) do
        spike_square2[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(spike_square2[my_index]);
    end

    local spike_objects3 = {
        -- upper left border
        { 37, 18 },
        { 39, 18 },
        { 37, 20 },
        { 39, 20 },
    }

    -- Loads the spikes according to the array
    for my_index, my_array in pairs(spike_objects3) do
        spike_square3[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(spike_square3[my_index]);
    end

    local spike_objects4 = {
        -- upper left border
        { 29, 22 },
        { 31, 22 },
        { 29, 24 },
        { 31, 24 },
    }

    -- Loads the spikes according to the array
    for my_index, my_array in pairs(spike_objects4) do
        spike_square4[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(spike_square4[my_index]);
    end

    local spike_objects5 = {
        -- upper left border
        { 33, 22 },
        { 35, 22 },
        { 33, 24 },
        { 35, 24 },
    }

    -- Loads the spikes according to the array
    for my_index, my_array in pairs(spike_objects5) do
        spike_square5[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(spike_square5[my_index]);
    end

    local spike_objects6 = {
        -- upper left border
        { 37, 22 },
        { 39, 22 },
        { 37, 24 },
        { 39, 24 },
    }

    -- Loads the spikes according to the array
    for my_index, my_array in pairs(spike_objects6) do
        spike_square6[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(spike_square6[my_index]);
    end

    -- 3 triggers permitting to play a bit...
    spike_trigger1 = vt_map.TriggerObject("",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "", "Push trigger 1");
    spike_trigger1:SetObjectID(Map.object_supervisor:GenerateObjectID());
    spike_trigger1:SetPosition(30, 26);
    Map:AddFlatGroundObject(spike_trigger1);

    spike_trigger2 = vt_map.TriggerObject("",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "", "Push trigger 2");
    spike_trigger2:SetObjectID(Map.object_supervisor:GenerateObjectID());
    spike_trigger2:SetPosition(34, 26);
    Map:AddFlatGroundObject(spike_trigger2);

    spike_trigger3 = vt_map.TriggerObject("",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "", "Push trigger 3");
    spike_trigger3:SetObjectID(Map.object_supervisor:GenerateObjectID());
    spike_trigger3:SetPosition(38, 26);
    Map:AddFlatGroundObject(spike_trigger3);

    -- Make sure the spike square states are initialized
    spike1_up = true;
    spike2_up = true;
    spike3_up = true;
    spike4_up = true;
    spike5_up = true;
    spike6_up = true;

end

function _add_flame(x, y)
    local object = vt_map.SoundObject("snd/campfire.ogg", x, y, 5.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;
    object = vt_map.SoundObject("snd/campfire.ogg", x + 18.0, y, 5.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;

    object = CreateObject(Map, "Flame1", x, y);
    Map:AddGroundObject(object);

    Map:AddHalo("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    Map:AddHalo("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

function _add_bubble(x, y)
    local object = CreateObject(Map, "Bubble", x, y);
    object:RandomizeCurrentAnimationFrame();
    Map:AddGroundObject(object);
    object = vt_map.ParticleObject("dat/effects/particles/bubble_steam.lua", x, y);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);
end

function _add_flame_pot(x, y)
    local object = vt_map.SoundObject("snd/campfire.ogg", x, y, 5.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;
    object = vt_map.SoundObject("snd/campfire.ogg", x + 18.0, y, 5.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;

    object = CreateObject(Map, "Flame Pot1", x, y);
    object:RandomizeCurrentAnimationFrame();
    Map:AddGroundObject(object);

    Map:AddHalo("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    Map:AddHalo("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to mountain shrine main room", "dat/maps/mt_elbrus/mt_elbrus_shrine2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine2_script.lua", "from_shrine_enigma_room");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Push trigger 1", "push_trigger_1", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push trigger 2", "push_trigger_2", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push trigger 3", "push_trigger_3", "");
    EventManager:RegisterEvent(event);
end

-- zones
local to_shrine_main_room_zone = {};
local to_shrine_treasure_room_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_main_room_zone = vt_map.CameraZone(62, 64, 32, 36);
    Map:AddZone(to_shrine_main_room_zone);

end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_main_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.EAST);
        EventManager:StartEvent("to mountain shrine main room");
    end

end

-- Updates the spikes states
function _UpdateSpikeState()
    for my_index, my_object in pairs(spike_square1) do
        if (spike1_up == true) then
            my_object:SetVisible(true);
            my_object:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        else
            my_object:SetVisible(false);
            my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end

    for my_index, my_object in pairs(spike_square2) do
        if (spike2_up == true) then
            my_object:SetVisible(true);
            my_object:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        else
            my_object:SetVisible(false);
            my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end

    for my_index, my_object in pairs(spike_square3) do
        if (spike3_up == true) then
            my_object:SetVisible(true);
            my_object:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        else
            my_object:SetVisible(false);
            my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end

    for my_index, my_object in pairs(spike_square4) do
        if (spike4_up == true) then
            my_object:SetVisible(true);
            my_object:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        else
            my_object:SetVisible(false);
            my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end

    for my_index, my_object in pairs(spike_square5) do
        if (spike5_up == true) then
            my_object:SetVisible(true);
            my_object:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        else
            my_object:SetVisible(false);
            my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end

    for my_index, my_object in pairs(spike_square6) do
        if (spike6_up == true) then
            my_object:SetVisible(true);
            my_object:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        else
            my_object:SetVisible(false);
            my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end
    AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
end

-- Map Custom functions
-- Used through scripted events
map_functions = {

    push_trigger_1 = function()
        spike_trigger2:SetState(false);
        spike_trigger3:SetState(false);

        spike1_up = not spike1_up;
        spike3_up = not spike3_up;
        spike5_up = not spike5_up;
        _UpdateSpikeState();
    end,

    push_trigger_2 = function()
        spike_trigger1:SetState(false);
        spike_trigger3:SetState(false);

        spike2_up = not spike2_up;
        spike4_up = not spike4_up;
        _UpdateSpikeState();
    end,

    push_trigger_3 = function()
        spike_trigger1:SetState(false);
        spike_trigger2:SetState(false);

        spike3_up = not spike3_up;
        spike4_up = not spike4_up;
        _UpdateSpikeState();
    end,
}
