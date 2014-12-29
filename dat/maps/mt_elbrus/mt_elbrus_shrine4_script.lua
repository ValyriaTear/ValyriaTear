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
local Map = nil
local DialogueManager = nil
local EventManager = nil
local Script = nil

-- the main character handler
local hero = nil

-- Forest dialogue secondary hero
local orlinn = nil

-- Name of the main sprite. Used to reload the good one at the end of dialogue events.
local main_sprite_name = "";

-- the main map loading code
function Load(m)

    Map = m;
    Script = Map:GetScriptSupervisor();
    DialogueManager = Map:GetDialogueSupervisor();
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(false);

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map:SetPartyMemberVisibleSprite(hero);

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
    hero = CreateSprite(Map, "Bronann", 60.0, 34.0, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.WEST);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    orlinn = CreateSprite(Map, "Orlinn",
                          hero:GetXPosition(), hero:GetYPosition(), vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);
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
local spike_trigger4 = nil;
local spike_trigger5 = nil;

local blocking_spike1 = nil;
local blocking_spike2 = nil;

local mini_boss = nil;

function _CreateObjects()
    local object = nil
    local npc = nil
    local dialogue = nil
    local text = nil
    local event = nil

    object = CreateTreasure(Map, "mt_shrine4_chest1", "Wood_Chest3", 6, 43, vt_map.MapMode.GROUND_OBJECT);
    object:AddObject(1001, 1); -- Minor Elixir

    _add_flame(61.5, 29);

    if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 1) then
        _add_waterlight(45, 25)
        _add_waterlight(15, 27)
        _add_waterlight(57, 13)
        _add_waterlight(18, 15)
        _add_waterlight(5, 37)
        _add_waterlight(51, 38)
        _add_waterlight(54, 22)
    else
        _add_bubble(39, 39);
        _add_bubble(45, 25);
        _add_bubble(15, 27);
        _add_bubble(57, 13);
        _add_bubble(18, 15);
        _add_bubble(5, 37);
        _add_bubble(51, 38);
        _add_bubble(54, 22);

        -- The poisonous scent
        Map:GetScriptSupervisor():AddScript("dat/maps/mt_elbrus/mt_elbrus_scent_anim.lua");
    end

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
        spike_square1[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2], vt_map.MapMode.GROUND_OBJECT);
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
        spike_square2[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2], vt_map.MapMode.GROUND_OBJECT);
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
        spike_square3[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2], vt_map.MapMode.GROUND_OBJECT);
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
        spike_square4[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2], vt_map.MapMode.GROUND_OBJECT);
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
        spike_square5[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2], vt_map.MapMode.GROUND_OBJECT);
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
        spike_square6[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2], vt_map.MapMode.GROUND_OBJECT);
    end

    -- Spikes blocking while the enigma isn't solved
    blocking_spike1 = CreateObject(Map, "Spikes1", 37, 12, vt_map.MapMode.GROUND_OBJECT);
    blocking_spike2 = CreateObject(Map, "Spikes1", 39, 12, vt_map.MapMode.GROUND_OBJECT);

    -- 5 triggers permitting to play a bit...
    spike_trigger1 = vt_map.TriggerObject.Create("",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "", "Push trigger 1");
    spike_trigger1:SetPosition(32, 27.5);

    spike_trigger2 = vt_map.TriggerObject.Create("",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "", "Push trigger 2");
    spike_trigger2:SetPosition(36, 27.5);

    spike_trigger3 = vt_map.TriggerObject.Create("",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "", "Push trigger 3");
    spike_trigger3:SetPosition(30, 31);

    spike_trigger4 = vt_map.TriggerObject.Create("",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "", "Push trigger 4");
    spike_trigger4:SetPosition(34, 31);

    spike_trigger5 = vt_map.TriggerObject.Create("",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "", "Push trigger 5");
    spike_trigger5:SetPosition(38, 31);

    -- Make sure the spike square states are initialized
    spike1_up = true;
    spike2_up = true;
    spike3_up = true;
    spike4_up = true;
    spike5_up = true;
    spike6_up = true;

    -- The trigger opening the next map.
    object = vt_map.TriggerObject.Create("mt elbrus shrine 4 trigger 1",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "", "Trap map open event");
    object:SetPosition(52, 10);

    -- 2nd Mini-boss
    mini_boss = CreateObject(Map, "Dorver1_left", 45, 11, vt_map.MapMode.GROUND_OBJECT);
    if (GlobalManager:GetEventValue("story", "mt_elbrus_shrine_trap_boss_2_done") == 1) then
        mini_boss:SetPosition(0, 0);
        mini_boss:SetVisible(false);
        mini_boss:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
end

function _add_waterlight(x, y)
    local object = CreateObject(Map, "Water Light1", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
end

function _add_flame(x, y)
    vt_map.SoundObject.Create("snd/campfire.ogg", x, y, 10.0);

    CreateObject(Map, "Flame1", x, y, vt_map.MapMode.GROUND_OBJECT);

    vt_map.Halo.Create("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    vt_map.Halo.Create("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

function _add_bubble(x, y)
    local object = CreateObject(Map, "Bubble", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();

    vt_map.ParticleObject.Create("dat/effects/particles/bubble_steam.lua", x, y, vt_map.MapMode.GROUND_OBJECT);
end

function _add_flame_pot(x, y)
    vt_map.SoundObject.Create("snd/campfire.ogg", x, y, 10.0);

    local object = CreateObject(Map, "Flame Pot1", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();

    vt_map.Halo.Create("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    vt_map.Halo.Create("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    event = vt_map.MapTransitionEvent("to mountain shrine main room", "dat/maps/mt_elbrus/mt_elbrus_shrine2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine2_script.lua", "from_shrine_enigma_room");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine main room-waterfalls", "dat/maps/mt_elbrus/mt_elbrus_shrine2_2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine2_script.lua", "from_shrine_enigma_room");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Push trigger 1", "push_trigger_1", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push trigger 2", "push_trigger_2", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push trigger 3", "push_trigger_3", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push trigger 4", "push_trigger_4", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push trigger 5", "push_trigger_5", "");
    EventManager:RegisterEvent(event);

    -- Trap map open event
    event = vt_map.ScriptedEvent("Trap map open event", "trap_map_start", "");
    event:AddEventLinkAtEnd("Trap map dialogue");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Again, I can feel something move not far from here...");
    dialogue:AddLine(text, hero);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Trap map dialogue", dialogue);
    event:AddEventLinkAtEnd("Trap map open event end");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Trap map open event end", "trap_map_end", "");
    EventManager:RegisterEvent(event);

    -- Mini boss event
    event = vt_map.ScriptedEvent("Mini-Boss fight", "mini_boss_start", "");
    event:AddEventLinkAtEnd("Mini-boss fight battle start");
    EventManager:RegisterEvent(event);

    event = vt_map.BattleEncounterEvent("Mini-boss fight battle start");
    event:SetMusic("mus/accion-OGA-djsaryon.ogg");
    event:SetBackground("img/backdrops/battle/mountain_shrine.png");
    event:AddScript("dat/battles/mountain_shrine_battle_anim.lua");
    event:SetBoss(true);
    event:AddEnemy(20);
    event:AddEventLinkAtEnd("Mini-boss fight end");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Mini-boss fight end", "mini_boss_end", "");
    EventManager:RegisterEvent(event);
end

-- zones
local to_shrine_main_room_zone = nil
local to_shrine_treasure_room_zone = nil
local mini_boss_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_main_room_zone = vt_map.CameraZone(62, 64, 32, 36);
    Map:AddZone(to_shrine_main_room_zone);

    mini_boss_zone = vt_map.CameraZone(40, 42, 6, 11);
    Map:AddZone(mini_boss_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_main_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.EAST);
        if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 0) then
            EventManager:StartEvent("to mountain shrine main room");
        else
            EventManager:StartEvent("to mountain shrine main room-waterfalls");
        end
    elseif (mini_boss_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "mt_elbrus_shrine_trap_boss_2_done") == 0) then
            EventManager:StartEvent("Mini-Boss fight");
        end
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

    -- Then check whether the enigma is solved.
    if (spike1_up == false and spike2_up == false and spike3_up == false and
        spike4_up == false and spike5_up == false and spike6_up == false) then

        -- free the exit
        blocking_spike1:SetVisible(false);
        blocking_spike2:SetVisible(false);
        blocking_spike1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        blocking_spike2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Disable the triggers
        spike_trigger1:SetOnEvent("");
        spike_trigger2:SetOnEvent("");
        spike_trigger3:SetOnEvent("");
        spike_trigger4:SetOnEvent("");
        spike_trigger5:SetOnEvent("");

        spike_trigger1:SetState(true);
        spike_trigger2:SetState(true);
        spike_trigger3:SetState(true);
        spike_trigger4:SetState(true);
        spike_trigger5:SetState(true);
    end
end

-- Map Custom functions
-- Used through scripted events
map_functions = {

    push_trigger_1 = function()
        spike_trigger2:SetState(false);
        spike_trigger3:SetState(false);
        spike_trigger4:SetState(false);
        spike_trigger5:SetState(false);

        spike1_up = not spike1_up;
        spike5_up = not spike5_up;
        spike6_up = not spike6_up;
        _UpdateSpikeState();
    end,

    push_trigger_2 = function()
        spike_trigger1:SetState(false);
        spike_trigger3:SetState(false);
        spike_trigger4:SetState(false);
        spike_trigger5:SetState(false);

        spike3_up = not spike3_up;
        spike4_up = not spike4_up;
        spike5_up = not spike5_up;
        _UpdateSpikeState();
    end,

    push_trigger_3 = function()
        spike_trigger1:SetState(false);
        spike_trigger2:SetState(false);
        spike_trigger4:SetState(false);
        spike_trigger5:SetState(false);

        --spike1_up = not spike1_up;
        spike3_up = not spike3_up;
        spike5_up = not spike5_up;
        _UpdateSpikeState();
    end,

    push_trigger_4 = function()
        spike_trigger1:SetState(false);
        spike_trigger2:SetState(false);
        spike_trigger3:SetState(false);
        spike_trigger5:SetState(false);

        spike2_up = not spike2_up;
        spike6_up = not spike6_up;
        _UpdateSpikeState();
    end,

    push_trigger_5 = function()
        spike_trigger1:SetState(false);
        spike_trigger2:SetState(false);
        spike_trigger3:SetState(false);
        spike_trigger4:SetState(false);

        spike1_up = not spike1_up;
        _UpdateSpikeState();
    end,

    trap_map_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        Map:GetEffectSupervisor():ShakeScreen(0.6, 2000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_LINEAR);
        AudioManager:PlaySound("snd/cave-in.ogg");
    end,

    trap_map_end = function()
        Map:PopState();
    end,

    mini_boss_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

    mini_boss_end = function()
        Map:PopState();
        GlobalManager:SetEventValue("story", "mt_elbrus_shrine_trap_boss_2_done", 1)

        mini_boss:SetPosition(0, 0);
        mini_boss:SetVisible(false);
        mini_boss:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        mini_boss:ClearEventWhenTalking();
    end,
}
