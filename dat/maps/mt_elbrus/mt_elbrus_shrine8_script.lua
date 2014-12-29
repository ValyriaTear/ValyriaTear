-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine8_script = ns;
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

    -- Preload sounds
    AudioManager:LoadSound("snd/trigger_on.wav", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Orlinn", 0.0, 0.0, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.FAST_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_NW_room") then
        hero:SetPosition(3, 10);
        hero:SetDirection(vt_map.MapMode.EAST);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_SE_room") then
        hero:SetPosition(28, 37.5);
        hero:SetDirection(vt_map.MapMode.NORTH);
    end

    -- The menu and status effects are disabled in this map
    Map:SetMenuEnabled(false);
    Map:SetStatusEffectsEnabled(false);
end

-- The trigger state at map load time.
-- Used to restore it, would Orlinn be caught.
local trigger_state = 0;

function _CreateObjects()
    local object = nil
    local npc = nil
    local dialogue = nil
    local text = nil
    local event = nil

    _add_flame(5.5, 6);
    _add_flame(27.5, 7);

    -- A trigger that will open the gate in the SE map.
    object = vt_map.TriggerObject.CreateObject("mt elbrus shrine 8 gate 7 trigger",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "", "Trigger on event");
    object:SetPosition(19, 20);

    trigger_state = GlobalManager:GetEventValue("triggers", "mt elbrus shrine 8 gate 7 trigger");

    -- Waterfalls
    if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 1) then
        _add_small_waterfall(40, 14);
        _add_waterfall(2, 46);
        _add_waterlight(42, 21)
        _add_waterlight(26, 19)
        _add_waterlight(16, 31)
        _add_waterlight(25, 33)
    end
end

function _add_waterfall(x, y)
    local object = CreateObject(Map, "Waterfall1", x - 0.1, y - 0.2, vt_map.MapMode.GROUND_OBJECT);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    object:RandomizeCurrentAnimationFrame();

    -- Ambient sound
    object = vt_map.SoundObject.CreateObject("snd/fountain_large.ogg", x, y - 5, 50.0);
    object:SetMaxVolume(0.6);

    -- Particle effects
    object = vt_map.ParticleObject.CreateObject("dat/effects/particles/waterfall_steam.lua", x, y - 15.0, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true);

    object = vt_map.ParticleObject.CreateObject("dat/effects/particles/waterfall_steam_big.lua", x, y + 0.2, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true);
end

function _add_small_waterfall(x, y)
    local object = CreateObject(Map, "Waterfall2", x - 0.1, y - 0.2, vt_map.MapMode.GROUND_OBJECT);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    object:RandomizeCurrentAnimationFrame();

    -- Ambient sound
    object = vt_map.SoundObject.CreateObject("snd/fountain_large.ogg", x, y - 5, 50.0);
    object:SetMaxVolume(0.6);

    -- Particle effects
    object = vt_map.ParticleObject.CreateObject("dat/effects/particles/waterfall_steam.lua", x, y - 8.0, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true);

    object = vt_map.ParticleObject.CreateObject("dat/effects/particles/waterfall_steam_big.lua", x, y + 1.0, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true);
end

function _add_waterlight(x, y)
    local object = CreateObject(Map, "Water Light1", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
end

function _add_flame(x, y)
    vt_map.SoundObject.CreateObject("snd/campfire.ogg", x, y, 10.0);

    CreateObject(Map, "Flame1", x, y, vt_map.MapMode.GROUND_OBJECT);

    Map:AddHalo("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    Map:AddHalo("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

function _CreateEnemies()
    local enemy = nil
    local roam_zone = nil

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(12, 20, 14, 22);

    enemy = CreateEnemySprite(Map, "Beetle");
    enemy:AddWayPoint(13.1, 15.8);
    enemy:AddWayPoint(18.9, 15.8);
    enemy:AddWayPoint(18.9, 21.8);
    enemy:AddWayPoint(13.1, 21.8);
    enemy:SetAggroRange(5);
    enemy:SetEncounterEvent("Restart Orlinn Map");
    roam_zone:AddEnemy(enemy, 1);
    Map:AddZone(roam_zone);


    roam_zone = vt_map.EnemyZone(28, 40, 16, 24);

    enemy = CreateEnemySprite(Map, "Beetle");
    enemy:AddWayPoint(33.2, 15.8);
    enemy:AddWayPoint(38.2, 15.8);
    enemy:AddWayPoint(38.2, 23.8);
    enemy:AddWayPoint(29.1, 23.8);
    enemy:AddWayPoint(29.1, 17.8);
    enemy:SetAggroRange(5);
    enemy:SetEncounterEvent("Restart Orlinn Map");
    roam_zone:AddEnemy(enemy, 1);
    Map:AddZone(roam_zone);

    roam_zone = vt_map.EnemyZone(28, 40, 22, 30);

    enemy = CreateEnemySprite(Map, "Beetle");
    enemy:AddWayPoint(29.1, 23.8);
    enemy:AddWayPoint(38.1, 23.8);
    enemy:AddWayPoint(38.1, 29.8);
    enemy:AddWayPoint(29.1, 29.8);
    enemy:SetAggroRange(5);
    enemy:SetEncounterEvent("Restart Orlinn Map");
    roam_zone:AddEnemy(enemy, 1);
    Map:AddZone(roam_zone);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor NW room", "dat/maps/mt_elbrus/mt_elbrus_shrine5_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine5_script.lua", "from_shrine_first_floor_NE_room");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor SE room", "dat/maps/mt_elbrus/mt_elbrus_shrine7_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine7_script.lua", "from_shrine_first_floor_NE_room");
    EventManager:RegisterEvent(event);

    -- Restart the map when Orlinn is caught.
    event = vt_map.ScriptedEvent("Restart Orlinn Map", "restart_map_event_start", "");
    event:AddEventLinkAtEnd("to previous entrance");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to previous entrance", "dat/maps/mt_elbrus/mt_elbrus_shrine8_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine8_script.lua", GlobalManager:GetPreviousLocation());
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Trigger on event", "trigger_on_sound", "");
    EventManager:RegisterEvent(event);
end

-- zones
local to_shrine_NW_room_zone = nil
local to_shrine_SE_room_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_NW_room_zone = vt_map.CameraZone(0, 2, 8, 12);
    Map:AddZone(to_shrine_NW_room_zone);
    to_shrine_SE_room_zone = vt_map.CameraZone(24, 32, 38, 40);
    Map:AddZone(to_shrine_SE_room_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_NW_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.WEST);
        EventManager:StartEvent("to mountain shrine 1st floor NW room");
    elseif (to_shrine_SE_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine 1st floor SE room");
    end
end

-- Map Custom functions
-- Used through scripted events
map_functions = {
    restart_map_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        AudioManager:PlayMusic("battle_encounter_01.ogg");
        hero:SetCustomAnimation("frightened_fixed", 0); -- 0 means forever

        -- Restore potential trigger previous state
        GlobalManager:SetEventValue("triggers", "mt elbrus shrine 8 gate 7 trigger", trigger_state);
    end,

    trigger_on_sound = function()
        AudioManager:PlaySound("snd/trigger_on.wav");
    end,
}
