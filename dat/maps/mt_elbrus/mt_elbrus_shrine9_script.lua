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
music_filename = "mus/mountain_shrine.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};
local Effects = {};

-- the main character handler
local hero = {};

local bronann = nil;
local kalya = nil;
local orlinn = nil;

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;
    Effects = Map:GetEffectSupervisor();
    Map.unlimited_stamina = false;

    _CreateCharacters();
    _CreateObjects();
    _CreateEnemies();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay
    Effects:EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);
    -- Add the background and foreground animations
    Map:GetScriptSupervisor():AddScript("dat/maps/layna_forest/layna_forest_caves_background_anim.lua");

    -- Preload sounds to avoid glitches
    AudioManager:LoadSound("snd/heavy_bump.wav", Map);
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
    hero = CreateSprite(Map, "Bronann", 22, 45);
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

    Map:AddGroundObject(hero);

    -- Create other characters
    bronann = CreateSprite(Map, "Bronann", 0, 0);
    bronann:SetDirection(vt_map.MapMode.NORTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    bronann:SetVisible(false);
    Map:AddGroundObject(bronann);

    kalya = CreateSprite(Map, "Kalya", 0, 0);
    kalya:SetDirection(vt_map.MapMode.NORTH);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);
    Map:AddGroundObject(kalya);

    orlinn = CreateSprite(Map, "Orlinn", 0, 0);
    orlinn:SetDirection(vt_map.MapMode.NORTH);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);
    Map:AddGroundObject(orlinn);
end

function _CreateObjects()
    local object = {};
    local npc = {};
    local event = {}

    Map:AddHalo("img/misc/lights/right_ray_light.lua", 26, 18,
            vt_video.Color(1.0, 1.0, 1.0, 0.8));
    Map:AddHalo("img/misc/lights/right_ray_light.lua", 53, 44,
            vt_video.Color(1.0, 1.0, 1.0, 0.8));

    Map:AddSavePoint(8, 42);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject("dat/effects/particles/heal_particle.lua", 0, 0);
    heal_effect:SetObjectID(Map.object_supervisor:GenerateObjectID());
    heal_effect:Stop(); -- Don't run it until the character heals itself
    Map:AddGroundObject(heal_effect);

    object = CreateObject(Map, "Layna Statue", 6, 35);
    object:SetEventWhenTalking("Heal dialogue");
    Map:AddGroundObject(object);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Your party feels better...");
    dialogue:AddLineEvent(text, 0, "Heal event", ""); -- 0 means no portrait and no name
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Heal dialogue", dialogue);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Heal event", "heal_party", "heal_done");
    EventManager:RegisterEvent(event);
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_shrine.png");
    enemy:AddBattleScript("dat/battles/mountain_shrine_battle_anim.lua");
end

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(26, 37, 33, 45);

    enemy = CreateEnemySprite(Map, "Skeleton");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(19);
    enemy:AddEnemy(19);
    enemy:AddEnemy(19);
    roam_zone:AddEnemy(enemy, Map, 1);
    Map:AddZone(roam_zone);
end

local kalya_move_next_to_bronann = nil;
local orlinn_move_next_to_bronann = nil;

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to mountain shrine entrance", "dat/maps/mt_elbrus/mt_elbrus_shrine2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine2_script.lua", "from_shrine_stairs1");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine trap room", "dat/maps/mt_elbrus/mt_elbrus_shrine3_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine3_script.lua", "from_shrine_stairs1");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks south", bronann, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Kalya looks at Orlinn", kalya, orlinn);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Orlinn looks at Kalya", orlinn, kalya);
    EventManager:RegisterEvent(event);

    -- The party is trapped ... again ...
    event = vt_map.ScriptedEvent("Start trap event", "trap_event_start", "");
    event:AddEventLinkAtEnd("Kalya moves up");
    event:AddEventLinkAtEnd("Orlinn moves up");
    event:AddEventLinkAtEnd("Bronann moves up");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Kalya moves up", kalya, 20, 15, false);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Orlinn moves up", orlinn, 24, 15, false);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Bronann moves up", bronann, 22, 13, false);
    event:AddEventLinkAtEnd("Trigger trap");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Trigger trap", "trap_trigger", "");
    event:AddEventLinkAtEnd("The heroes wonder", 1000);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("The heroes wonder", "heroes_wonder", "");
    event:AddEventLinkAtEnd("Bronann looks south", 500);
    event:AddEventLinkAtEnd("Kalya looks at Orlinn", 500);
    event:AddEventLinkAtEnd("Orlinn looks at Kalya", 500);
    event:AddEventLinkAtEnd("The heroes exclamate", 1500);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("The heroes exclamate", "heroes_exclamate", "");
    event:AddEventLinkAtEnd("The heroes fall", 800);
    event:AddEventLinkAtEnd("to mountain shrine basement", 800);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("The heroes fall", "heroes_fall_start", "heroes_fall_update");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine basement", "dat/maps/mt_elbrus/mt_elbrus_shrine_basement_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine_basement_script.lua", "from_shrine_stairs1");
    EventManager:RegisterEvent(event);

end

-- zones
local to_shrine_entrance_zone = nil
local to_shrine_trap_zone = nil
local falling_event_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_entrance_zone = vt_map.CameraZone(20, 24, 46, 48);
    Map:AddZone(to_shrine_entrance_zone);

    to_shrine_trap_zone = vt_map.CameraZone(50, 52, 46, 48);
    Map:AddZone(to_shrine_trap_zone);

    falling_event_zone = vt_map.CameraZone(19, 25, 28, 33);
    Map:AddZone(falling_event_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_entrance_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain shrine entrance");
    elseif (to_shrine_trap_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain shrine trap room");
    elseif (falling_event_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        Map:PushState(vt_map.MapMode.STATE_SCENE)
        hero:SetMoving(false);
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
        AudioManager:PlaySound("snd/heal_spell.wav");
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
        -- TODO: hole opening, and fall hiding graphics
        AudioManager:PlaySound("snd/heavy_bump.wav");
        Map:GetEffectSupervisor():ShakeScreen(3.0, 1000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_SUDDEN);
        AudioManager:FadeOutAllMusic(400);
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
        AudioManager:PlaySound("snd/falling.ogg");
        bronann:SetCustomAnimation("frightened_fixed", 0) -- 0 means forever
        kalya:SetCustomAnimation("frightened_fixed", 0) -- 0 means forever
        orlinn:SetCustomAnimation("frightened_fixed", 0) -- 0 means forever
    end,

    heroes_fall_update = function()
        local update_time = SystemManager:GetUpdateTime();
        -- Make the heroes fall
        if (bronann:GetYPosition() < 17.0) then
            bronann:SetYPosition(bronann:GetYPosition() + update_time * 0.005)
        end
        if (kalya:GetYPosition() < 17.0) then
            kalya:SetYPosition(kalya:GetYPosition() + update_time * 0.005)
        end
        if (orlinn:GetYPosition() < 17.0) then
            orlinn:SetYPosition(orlinn:GetYPosition() + update_time * 0.005)
        end
        -- never actually ends
        return false;
    end,

}
