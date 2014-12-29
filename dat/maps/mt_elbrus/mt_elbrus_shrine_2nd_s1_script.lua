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
local Map = nil
local DialogueManager = nil
local EventManager = nil

-- the main character handler
local hero = nil

-- Name of the main sprite. Used to reload the good one at the end of dialogue events.
local main_sprite_name = "";

-- An actual array of objects
local bridge_middle_parts = {}

-- the main map loading code
function Load(m)

    Map = m;
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

    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/snow_fog.png", 30.0, 10.0, true);
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");

    -- Make the snow start
    Map:GetParticleManager():AddParticleEffect("dat/effects/particles/snow.lua", 512.0, 384.0);

    -- The mountain cliff background
    Map:GetScriptSupervisor():AddScript("dat/maps/mt_elbrus/mt_elbrus_background_anim.lua");

    -- Start the pusher winds
    EventManager:StartEvent("Start winds");

    -- Preload falling sounds
    AudioManager:LoadSound("snd/falling.ogg", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 86, 8, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_2nd_floor_grotto_left") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(8.0, 7.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_2nd_floor_grotto_right") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(34.0, 26.0);
    end
end

local wind_effect1 = nil
local wind_effect2 = nil
local wind_effect3 = nil

local wind_sound1 = nil;
local wind_sound2 = nil;
local wind_sound3 = nil;

function _CreateObjects()
    local object = nil
    local npc = nil
    local dialogue = nil
    local text = nil
    local event = nil

    object = CreateTreasure.Create(Map, "mt_shrine_2nd_s1_chest1", "Wood_Chest1", 73, 49, vt_map.MapMode.GROUND_OBJECT);
    object:AddItem(12, 1); -- Medium Moon Juice potion x 1

    -- Objects array
    local map_objects = {

        { "Tree Small1 snow", 81.5, 20 },
        { "Tree Small1 snow", 80, 28 },
        { "Tree Small2 snow", 71, 72 },
        { "Tree Small1 snow", 60, 73 },
        { "Tree Small1 snow", 86, 56 },
        { "Tree Small1 snow", 62, 56.5 },
        { "Tree Small2 snow", 51, 50 },
        { "Tree Small2 snow", 49, 45 },
        { "Tree Small1 snow", 40, 41 },
        { "Tree Small2 snow", 36, 46 },
        { "Tree Small2 snow", 67, 51 },
        { "Tree Small2 snow", 47, 70 },
        { "Tree Small1 snow", 60, 52.2 },
        { "Tree Small2 snow", 29, 25 },
        { "Tree Small1 snow", 39, 26 },
        { "Tree Small1 snow", 8, 71 },
        { "Tree Small1 snow", 7, 64 },
        { "Tree Small1 snow", 82, 11 },
        { "Tree Small1 snow", 26, 3 },

        { "Tree Big2 snow", 79, 12 },
        { "Tree Big1 snow", 96.5, 29 },
        { "Tree Big2 snow", 90, 51 },
        { "Tree Big2 snow", 93, 12 },
        { "Tree Big1 snow", 79, 42 },
        { "Tree Big2 snow", 96, 37 },
        { "Tree Big1 snow", 78, 51 },
        { "Tree Big2 snow", 82, 63 },
        { "Tree Big1 snow", 66, 65 },
        { "Tree Big1 snow", 8, 61 },
        { "Tree Big1 snow", 12, 51 },
        { "Tree Big2 snow", 15, 55 },
        { "Tree Big2 snow", 22.5, 65 },
        { "Tree Big2 snow", 31, 35 },
        { "Tree Big1 snow", 38, 36 },
        { "Tree Big1 snow", 15, 10 },
        { "Tree Big2 snow", 3, 9 },
        { "Tree Big2 snow", 49, 27 },

        { "Rock1 snow", 92, 18 },
        { "Rock1 snow", 91.5, 39 },
        { "Rock1 snow", 59, 47 },
        { "Rock1 snow", 42, 47.2 },
        { "Rock1 snow", 26, 67 },
        { "Rock1 snow", 24, 75 },
        { "Rock1 snow", 30, 72.1 },
        { "Rock1 snow", 28, 73.1 },
        { "Rock1 snow", 40, 72.2 },
        { "Rock1 snow", 33.5, 39 },
        { "Rock1 snow", 18, 3 },
        { "Rock1 snow", 38.5, 10 },
        { "Rock1 snow", 40, 22.5 },
        { "Rock1 snow", 94, 23 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_objects) do
        --print(my_array[1], my_array[2], my_array[3]);
        CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
    end

    -- Create the bridge
    CreateObject(Map, "Bridge1_up", 10, 11, vt_map.MapMode.FLATGROUND_OBJECT);

    bridge_middle_parts[1] = CreateObject(Map, "Bridge1_middle", 10, 11, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[2] = CreateObject(Map, "Bridge1_middle", 10, 13, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[3] = CreateObject(Map, "Bridge1_middle", 10, 15, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[4] = CreateObject(Map, "Bridge1_middle", 10, 17, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[5] = CreateObject(Map, "Bridge1_middle", 10, 19, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[6] = CreateObject(Map, "Bridge1_middle", 10, 20, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[7] = CreateObject(Map, "Bridge1_middle", 10, 22, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[8] = CreateObject(Map, "Bridge1_middle", 10, 24, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[9] = CreateObject(Map, "Bridge1_middle", 10, 26, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[10] = CreateObject(Map, "Bridge1_middle", 10, 28, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[11] = CreateObject(Map, "Bridge1_middle", 10, 30, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[12] = CreateObject(Map, "Bridge1_middle", 10, 32, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[13] = CreateObject(Map, "Bridge1_middle", 10, 34, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[14] = CreateObject(Map, "Bridge1_middle", 10, 36, vt_map.MapMode.FLATGROUND_OBJECT);

    CreateObject(Map, "Bridge1_down", 10, 38, vt_map.MapMode.FLATGROUND_OBJECT);

    -- Adds the wind particle effects
    wind_effect1 = vt_map.ParticleObject.Create("dat/maps/mt_elbrus/particles_snow_pushing.lua", 34, 80, vt_map.MapMode.GROUND_OBJECT);
    wind_effect1:Stop(); -- Don't run it at start

    wind_effect2 = vt_map.ParticleObject.Create("dat/maps/mt_elbrus/particles_snow_pushing.lua", 45, 55, vt_map.MapMode.GROUND_OBJECT);
    wind_effect2:Stop(); -- Don't run it at start

    -- An harmless wind pusher, for once
    wind_effect3 = vt_map.ParticleObject.Create("dat/maps/mt_elbrus/particles_snow_pushing.lua", 87, 29, vt_map.MapMode.GROUND_OBJECT);
    wind_effect3:Stop(); -- Don't run it at start

    wind_sound1 = vt_map.SoundObject.Create("snd/mountain_wind.ogg", 34.0, 68.0, 15.0);
    wind_sound1:Stop();

    wind_sound2 = vt_map.SoundObject.Create("snd/mountain_wind.ogg", 46.0, 47.0, 15.0);
    wind_sound2:Stop();

    wind_sound3 = vt_map.SoundObject.Create("snd/mountain_wind.ogg", 87.0, 19.0, 15.0);
    wind_sound3:Stop();
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    event = vt_map.MapTransitionEvent("to mountain shrine 2nd floor NE", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_ne_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_ne_script.lua", "from_shrine_2nd_floor_south");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine 2nd floor north west", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s2_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s2_script.lua", "from_shrine_2nd_floor_south_west");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine 2nd floor north east", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s2_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s2_script.lua", "from_shrine_2nd_floor_south_east");
    EventManager:RegisterEvent(event);

    -- A never-ending event that triggers winds every few seconds pushing the character down and back to the start if he's heedless enough.
    event = vt_map.ScriptedEvent("Start winds", "winds_start", "winds_update");
    EventManager:RegisterEvent(event);

    -- Makes the character slowly fall downward
    event = vt_map.ScriptedEvent("Falling to shrine entrance", "falling_start", "falling_update");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("To mountain shrine entrance", "dat/maps/mt_elbrus/mt_elbrus_path4_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_path4_script.lua", "from_shrine_2nd_floor_wind_trap");
    EventManager:RegisterEvent(event);
end

-- zones
local to_shrine_ne_zone = nil
local to_grotto_zone = nil
local to_shrine_nw_zone = nil

local windy1_zone = nil
local windy2_zone = nil
local windy3_zone = nil
local falling_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_ne_zone = vt_map.CameraZone(84, 88, 3, 5);
    Map:AddZone(to_shrine_ne_zone);
    to_shrine_nw_zone = vt_map.CameraZone(6, 10, 3, 5);
    Map:AddZone(to_shrine_nw_zone);
    to_grotto_zone = vt_map.CameraZone(33, 35, 22, 24);
    Map:AddZone(to_grotto_zone);

    windy1_zone = vt_map.CameraZone(32, 36, 65, 71);
    Map:AddZone(windy1_zone);
    windy2_zone = vt_map.CameraZone(43, 47, 40, 48);
    Map:AddZone(windy2_zone);
    falling_zone = vt_map.CameraZone(32, 36, 71, 74);
    falling_zone:AddSection(43, 47, 48, 52);
    Map:AddZone(falling_zone);

    windy3_zone = vt_map.CameraZone(85, 89, 10, 29);
    Map:AddZone(windy3_zone);
end

-- Tells whether the winds are on.
local wind1_started = false;
local wind2_started = false;
local wind3_started = false;

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_ne_zone:IsCameraEntering() == true) then
        hero:SetMoving(true);
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine 2nd floor NE");
    elseif (to_grotto_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain shrine 2nd floor north east");
    elseif (to_shrine_nw_zone:IsCameraEntering() == true) then
        hero:SetMoving(true);
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine 2nd floor north west");
    elseif (wind1_started == true and windy1_zone:IsCameraInside() == true) then
        -- Push the character down.
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.007 * update_time;
        hero:SetYPosition(hero:GetYPosition() + movement_diff);
    elseif (wind2_started == true and windy2_zone:IsCameraInside() == true) then
        -- Push the character down.
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.007 * update_time;
        hero:SetYPosition(hero:GetYPosition() + movement_diff);
    elseif (wind3_started == true and windy3_zone:IsCameraInside() == true) then
        -- Push the character down.
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.007 * update_time;
        -- Check for collision since one doesn't want to get the player stuck this time
        if (hero:IsColliding(hero:GetXPosition(), hero:GetYPosition() + movement_diff) == false) then
            hero:SetYPosition(hero:GetYPosition() + movement_diff);
        end
    end

    -- Check whether the hero is south-enough to fall.
    if (falling_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        EventManager:StartEvent("Falling to shrine entrance");
        EventManager:StartEvent("To mountain shrine entrance");
    end
end

local wind1_update_time = 0;
local wind2_update_time = 0;
local wind3_update_time = 0;

-- Map Custom functions
-- Used through scripted events
map_functions = {

    winds_start = function()
        wind1_update_time = 0;
        wind2_update_time = 0;
        wind3_update_time = 0;
        wind_effect1:Stop();
        wind_effect2:Stop();
        wind_effect3:Stop();
        wind1_started = false;
        wind2_started = false;
        wind3_started = false;
    end,

    winds_update = function()
        local update_time = SystemManager:GetUpdateTime();
        wind1_update_time = wind1_update_time + update_time;
        wind2_update_time = wind2_update_time + update_time;
        wind3_update_time = wind3_update_time + update_time;

        if (wind1_started == false and wind1_update_time > 6000) then
            wind1_update_time = 0;
            wind_effect1:Start();
            wind_sound1:Start();
            wind1_started = true;
        elseif (wind1_started == true and wind1_update_time > 4500) then
            wind1_update_time = 0;
            wind_effect1:Stop();
            wind_sound1:Stop();
            wind1_started = false;
        end

        if (wind2_started == false and wind2_update_time > 5000) then
            wind2_update_time = 0;
            wind_sound2:Start();
            wind_effect2:Start();
            wind2_started = true;
        elseif (wind2_started == true and wind2_update_time > 4500) then
            wind2_update_time = 0;
            wind_sound2:Stop();
            wind_effect2:Stop();
            wind2_started = false;
        end

        if (wind3_started == false and wind3_update_time > 3000) then
            wind3_update_time = 0;
            wind_effect3:Start();
            wind_sound3:Start();
            wind3_started = true;
        elseif (wind3_started == true and wind3_update_time > 4500) then
            wind3_update_time = 0;
            wind_effect3:Stop();
            wind_sound3:Stop();
            wind3_started = false;
        end
        return false;
    end,

    falling_start = function()
        hero:SetCustomAnimation("frightened_fixed", 0); -- 0 means forever
        AudioManager:PlaySound("snd/falling.ogg");
    end,

    falling_update = function()
        if (hero:GetYPosition() > 120) then
            return false;
        end

        -- Push the character down.
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.010 * update_time;
        hero:SetYPosition(hero:GetYPosition() + movement_diff);
        return false;
    end,
}
