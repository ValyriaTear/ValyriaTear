-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine9_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus Shrine"
map_image_filename = "img/menus/locations/mountain_shrine.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "music/mountain_shrine.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Effects = nil

-- the main character handler
local hero = nil

local bronann = nil;
local kalya = nil;
local orlinn = nil;

-- the main map loading code
function Load(m)

    Map = m;
    Effects = Map:GetEffectSupervisor();
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
    Effects:EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);
    -- Add the background and foreground animations
    Map:GetScriptSupervisor():AddScript("dat/maps/layna_forest/layna_forest_caves_background_anim.lua");

    -- Preload sounds to avoid glitches
    AudioManager:LoadSound("sounds/heavy_bump.wav", Map);
    AudioManager:LoadSound("sounds/falling.ogg", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 22, 45, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    local x_position = GlobalManager:GetSaveLocationX();
    local y_position = GlobalManager:GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Use the save point position, and clear the save position data for next maps
        GlobalManager:UnsetSaveLocation();
        -- Make the character look at us in that case
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(x_position, y_position);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_main_room") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(22.0, 45.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_trap_room") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(51.0, 45.0);
    end

    -- Create other characters
    bronann = CreateSprite(Map, "Bronann", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.NORTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    bronann:SetVisible(false);

    kalya = CreateSprite(Map, "Kalya", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.NORTH);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);

    orlinn = CreateSprite(Map, "Orlinn", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.NORTH);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);
end

-- falling hole objects
local falling_hole = nil
local falling_hole_wall = nil

function _CreateObjects()
    local object = CreateTreasure(Map, "mt_shrine9_chest1", "Wood_Chest1", 47, 42, vt_map.MapMode.GROUND_OBJECT);
    object:AddItem(3001, 1); -- Copper Ore x 1

    object = CreateTreasure(Map, "mt_shrine9_chest2", "Wood_Chest1", 51, 42, vt_map.MapMode.GROUND_OBJECT);
    object:AddItem(3001, 1); -- Copper Ore x 1

    object = CreateTreasure(Map, "mt_shrine9_chest3", "Wood_Chest1", 55, 42, vt_map.MapMode.GROUND_OBJECT);
    object:AddItem(3002, 1); -- Iron Ore x 1

    vt_map.Halo.Create("img/misc/lights/right_ray_light.lua", 26, 18,
            vt_video.Color(1.0, 1.0, 1.0, 0.8));
    vt_map.Halo.Create("img/misc/lights/right_ray_light.lua", 53, 44,
            vt_video.Color(1.0, 1.0, 1.0, 0.8));

    vt_map.SavePoint.Create(8, 42);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject.Create("dat/effects/particles/heal_particle.lua", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    heal_effect:Stop(); -- Don't run it until the character heals itself

    object = CreateObject(Map, "Layna Statue", 6, 35, vt_map.MapMode.GROUND_OBJECT);
    object:SetEventWhenTalking("Heal dialogue");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Your party feels better.");
    dialogue:AddLineEvent(text, nil, "Heal event", ""); -- 'nil' means no portrait and no name
    vt_map.DialogueEvent.Create("Heal dialogue", dialogue);

    vt_map.ScriptedEvent.Create("Heal event", "heal_party", "heal_done");

    falling_hole = vt_map.PhysicalObject.Create(vt_map.MapMode.FLATGROUND_OBJECT);
    falling_hole:SetPosition(22.02, 18.0);
    falling_hole:SetCollPixelHalfWidth(4.0 * 16);
    falling_hole:SetCollPixelHeight(8.0 * 16);
    falling_hole:SetImgPixelHalfWidth(4.0 * 16);
    falling_hole:SetImgPixelHeight(8.0 * 16);
    falling_hole:AddStillFrame("dat/maps/mt_elbrus/falling_hole.png");
    falling_hole:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    falling_hole:SetVisible(false);

    falling_hole_wall = vt_map.PhysicalObject.Create(vt_map.MapMode.SKY_OBJECT);
    falling_hole_wall:SetPosition(22.02, 25.35);
    falling_hole_wall:SetCollPixelHalfWidth(4.0 * 16);
    falling_hole_wall:SetCollPixelHeight(7.375 * 16);
    falling_hole_wall:SetImgPixelHalfWidth(4.0 * 16);
    falling_hole_wall:SetImgPixelHeight(7.375 * 16);
    falling_hole_wall:AddStillFrame("dat/maps/mt_elbrus/falling_hole_above.png");
    falling_hole_wall:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    falling_hole_wall:SetVisible(false);
end

local kalya_move_next_to_bronann = nil;
local orlinn_move_next_to_bronann = nil;

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    vt_map.MapTransitionEvent.Create("to mountain shrine entrance", "dat/maps/mt_elbrus/mt_elbrus_shrine2_2_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine2_script.lua", "from_shrine_stairs1");

    vt_map.MapTransitionEvent.Create("to mountain shrine trap room", "dat/maps/mt_elbrus/mt_elbrus_shrine3_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine3_script.lua", "from_shrine_treasure_room");

    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Orlinn", kalya, orlinn);
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Kalya", orlinn, kalya);

    -- The party is trapped ... again ...
    event = vt_map.ScriptedEvent.Create("Start trap event", "trap_event_start", "");
    event:AddEventLinkAtEnd("Kalya moves up", 300);
    event:AddEventLinkAtEnd("Orlinn moves up", 300);
    event:AddEventLinkAtEnd("Bronann moves up");

    vt_map.PathMoveSpriteEvent.Create("Kalya moves up", kalya, 21, 15, false);
    vt_map.PathMoveSpriteEvent.Create("Orlinn moves up", orlinn, 23, 15, false);

    event = vt_map.PathMoveSpriteEvent.Create("Bronann moves up", bronann, 22, 13, false);
    event:AddEventLinkAtEnd("Trigger trap");

    event = vt_map.ScriptedEvent.Create("Trigger trap", "trap_trigger", "");
    event:AddEventLinkAtEnd("The heroes wonder", 1000);

    event = vt_map.ScriptedEvent.Create("The heroes wonder", "heroes_wonder", "");
    event:AddEventLinkAtEnd("Bronann looks south", 500);
    event:AddEventLinkAtEnd("Kalya looks at Orlinn", 500);
    event:AddEventLinkAtEnd("Orlinn looks at Kalya", 500);
    event:AddEventLinkAtEnd("The heroes exclamate", 1500);

    event = vt_map.ScriptedEvent.Create("The heroes exclamate", "heroes_exclamate", "");
    event:AddEventLinkAtEnd("The heroes fall", 800);
    event:AddEventLinkAtEnd("to mountain shrine basement", 1400);

    vt_map.ScriptedEvent.Create("The heroes fall", "heroes_fall_start", "heroes_fall_update");

    vt_map.MapTransitionEvent.Create("to mountain shrine basement", "dat/maps/mt_elbrus/mt_elbrus_shrine_basement_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine_basement_script.lua", "from_shrine_stairs1");
end

-- zones
local to_shrine_entrance_zone = nil
local to_shrine_trap_zone = nil
local falling_event_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_entrance_zone = vt_map.CameraZone.Create(20, 24, 46, 48);
    to_shrine_trap_zone = vt_map.CameraZone.Create(50, 52, 46, 48);
    falling_event_zone = vt_map.CameraZone.Create(19, 25, 26, 28);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_entrance_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine entrance");
    elseif (to_shrine_trap_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine trap room");
    elseif (falling_event_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        Map:PushState(vt_map.MapMode.STATE_SCENE)
        EventManager:StartEvent("Start trap event");
    end
end

-- Map Custom functions
-- Used through scripted events

-- Effect time used when applying the heal light effect
local heal_effect_time = 0;
local heal_color = vt_video.Color(0.0, 0.0, 1.0, 1.0);

map_functions = {

    heal_party = function()
        hero:SetMoving(false);
        -- Should be sufficient to heal anybody
        GlobalManager:GetActiveParty():AddHitPoints(20000);
        GlobalManager:GetActiveParty():AddSkillPoints(20000);
        Map:SetStamina(10000);
        AudioManager:PlaySound("sounds/heal_spell.wav");
        heal_effect:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        heal_effect:Start();
        heal_effect_time = 0;
    end,

    heal_done = function()
        heal_effect_time = heal_effect_time + SystemManager:GetUpdateTime();

        if (heal_effect_time < 300.0) then
            heal_color:SetAlpha(heal_effect_time / 300.0 / 3.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(heal_color);
            return false;
        end

        if (heal_effect_time < 1000.0) then
            heal_color:SetAlpha(((1000.0 - heal_effect_time) / 700.0) / 3.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(heal_color);
            return false;
        end
        return true;
    end,

    trap_event_start = function()
        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        kalya:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        orlinn:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        Map:SetCamera(bronann);
        hero:SetVisible(false);
        bronann:SetDirection(vt_map.MapMode.NORTH)
        kalya:SetDirection(vt_map.MapMode.NORTH)
        orlinn:SetDirection(vt_map.MapMode.NORTH)

        bronann:SetVisible(true);
        kalya:SetVisible(true);
        orlinn:SetVisible(true);
    end,

    trap_trigger = function()
        -- hole opening, and fall hiding graphics
        falling_hole:SetVisible(true)
        falling_hole_wall:SetVisible(true)
        AudioManager:PlaySound("sounds/heavy_bump.wav");
        Map:GetEffectSupervisor():ShakeScreen(3.0, 1000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_SUDDEN);
        AudioManager:FadeOutActiveMusic(400);
    end,

    heroes_wonder = function()
        kalya:Emote("interrogation", kalya:GetDirection());
        orlinn:Emote("interrogation", orlinn:GetDirection());
        bronann:Emote("interrogation", bronann:GetDirection());
    end,

    heroes_exclamate = function()
        kalya:Emote("exclamation", kalya:GetDirection());
        orlinn:Emote("exclamation", orlinn:GetDirection());
        bronann:Emote("exclamation", bronann:GetDirection());
    end,

    heroes_fall_start = function()
        AudioManager:PlaySound("sounds/falling.ogg");
        bronann:SetCustomAnimation("frightened_fixed", 0) -- 0 means forever
        kalya:SetCustomAnimation("frightened_fixed", 0) -- 0 means forever
        orlinn:SetCustomAnimation("frightened_fixed", 0) -- 0 means forever
    end,

    heroes_fall_update = function()
        local update_time = SystemManager:GetUpdateTime();
        -- Make the heroes fall
        if (bronann:GetYPosition() < 23.0) then
            bronann:SetYPosition(bronann:GetYPosition() + update_time * 0.011)
        end
        if (kalya:GetYPosition() < 23.0) then
            kalya:SetYPosition(kalya:GetYPosition() + update_time * 0.011)
        end
        if (orlinn:GetYPosition() < 23.0) then
            orlinn:SetYPosition(orlinn:GetYPosition() + update_time * 0.011)
        end
        -- never actually ends
        return false;
    end,
}
