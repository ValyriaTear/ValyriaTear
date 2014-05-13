-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine_2nd_s2_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "img/menus/locations/mt_elbrus.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/icy_wind.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};
local Effects = {};

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
    Effects = Map:GetEffectSupervisor();

    Map.unlimited_stamina = false;

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);

    -- Preloads the action sounds to avoid glitches
    AudioManager:LoadSound("snd/stone_roll.wav", Map);
    AudioManager:LoadSound("snd/stone_bump.ogg", Map);
    AudioManager:LoadSound("snd/opening_sword_unsheathe.wav", Map);
    AudioManager:LoadSound("snd/magic_blast.ogg", Map);
    AudioManager:LoadSound("snd/battle_encounter_03.ogg", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 10, 68);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_2nd_floor_south_east") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(42.5, 48.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_stairs") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(6.0, 8.0);
    end

    Map:AddGroundObject(hero);
end

function _add_flame(x, y)
    local object = vt_map.SoundObject("snd/campfire.ogg", x, y, 5.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;
    object = vt_map.SoundObject("snd/campfire.ogg", x + 18.0, y, 5.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;

    object = CreateObject(Map, "Flame1", x, y);
    object:RandomizeCurrentAnimationFrame();
    Map:AddGroundObject(object);

    Map:AddHalo("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    Map:AddHalo("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

local rolling_stone1 = {};

-- Arrays of spikes objects
local trap_spikes = {};
local trap_spikes_left = {};
local trap_spikes_right = {};

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}

    -- Objects array
    local map_objects = {

        { "Rock1", 25, 14 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_objects) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        Map:AddGroundObject(object);
    end

    -- Snow effects
    object = vt_map.ParticleObject("dat/maps/mt_elbrus/particles_snow_south_entrance.lua", 10, 72);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 10, 79,
                vt_video.Color(1.0, 1.0, 1.0, 0.8));

    object = vt_map.ParticleObject("dat/maps/mt_elbrus/particles_snow_south_entrance.lua", 44, 50);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 45, 57,
                vt_video.Color(1.0, 1.0, 1.0, 0.8));

    _add_flame(9.5, 3);

    -- The stone used to get through this enigma
    rolling_stone1 = CreateObject(Map, "Rolling Stone2", 28, 34);
    Map:AddGroundObject(rolling_stone1);

    -- events on the lower level
    event = vt_map.IfEvent("Check hero position for rolling stone 1", "check_diagonal_stone1", "Push the rolling stone 1", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 1", "start_to_move_the_stone1", "move_the_stone_update1")
    EventManager:RegisterEvent(event);

    -- events on the upper level
    event = vt_map.ScriptedEvent("Make rolling stone1 fall event start", "stone_falls_event_start", "stone_falls_event_update");
    event:AddEventLinkAtEnd("Hero speaks about the red stone");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Phew, this one sure is heavier and sturdier...");
    dialogue:AddLineEmote(text, hero, "sweat drop");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Hero speaks about the red stone", dialogue);
    event:AddEventLinkAtEnd("Make rolling stone1 fall event end");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Make rolling stone1 fall event end", "stone_falls_event_end", "");
    EventManager:RegisterEvent(event);

    -- Set the stone event according to the events
    if (GlobalManager:GetEventValue("story", "mountain_shrine_2ndfloor_pushed_stone") == 0) then
        -- The stones is on the upper level
        rolling_stone1:SetEventWhenTalking("Make rolling stone1 fall event start");
    else
        -- The stone is on the lower level
        rolling_stone1:SetPosition(16, 34);
        rolling_stone1:SetEventWhenTalking("Check hero position for rolling stone 1");
    end

    -- Creates the spikes
    local spike_objects = {
        -- upper border
        { 3, 23 },
        { 5, 23 },
        { 7, 23 },
        { 9, 23 },
        { 11, 23 },
        { 13, 23 },
        { 15, 23 },
        { 17, 23 },
        { 19, 23 },
        { 21, 23 },
        { 23, 23 },
        -- lower border
        { 3, 43 },
        { 5, 43 },
        { 7, 43 },
        { 9, 43 },
        { 11, 43 },
        { 13, 43 },
        { 15, 43 },
        { 17, 43 },
        { 19, 43 },
        { 21, 43 },
        { 23, 43 },
        -- left
        { 2, 25 },
        { 2, 27 },
        { 2, 29 },
        { 2, 31 },
        { 2, 33 },
        { 2, 35 },
        { 2, 37 },
        { 2, 39 },
        { 2, 41 },
        -- right
        { 24, 25 },
        { 24, 27 },
        { 24, 29 },
        { 24, 31 },
        { 24, 33 },
        { 24, 35 },
        { 24, 37 },
        { 24, 39 },
        { 24, 41 },
    }

    -- Loads the spikes according to the array
    for my_index, my_array in pairs(spike_objects) do
        trap_spikes[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(trap_spikes[my_index]);
        trap_spikes[my_index]:SetVisible(false);
        trap_spikes[my_index]:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    -- Creates the other spikes
    local spike_objects2 = {
        -- from top-left
        { 4, 25 },
        { 6, 25 },
        { 8, 25 },
        { 10, 25 },
        { 12, 25 },
        { 14, 25 },
        { 16, 25 },
        { 18, 25 },
        { 20, 25 },
        { 22, 25 },
        { 22, 27 },
        { 22, 29 },
        { 22, 31 },
        { 22, 33 },
        { 22, 35 },
        { 22, 37 },
        { 22, 39 },
        { 20, 39 },
        { 18, 39 },
        { 16, 39 },
        { 14, 39 },
        { 12, 39 },
        { 10, 39 },
        { 8, 39 },
        { 6, 39 },
        { 6, 37 },
        { 6, 35 },
        { 6, 33 },
        { 6, 31 },
        { 6, 29 },
        { 8, 29 },
        { 10, 29 },
        { 12, 29 },
        { 14, 29 },
        { 16, 29 },
        { 18, 29 },
        { 18, 31 },
        { 18, 33 },
        { 18, 35 },
        { 16, 35 },
        { 14, 35 },
        { 12, 35 },
        { 10, 35 },
        { 10, 33 },
        { 12, 33 },
    }

    -- Loads the other spikes according to the array
    local spike_index = 0;
    for my_index, my_array in pairs(spike_objects2) do
        trap_spikes_left[spike_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(trap_spikes_left[spike_index]);
        trap_spikes_left[spike_index]:SetVisible(false);
        trap_spikes_left[spike_index]:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        spike_index = spike_index + 1
    end

    local spike_objects3 = {
        -- from bottom-right
        { 22, 41 },
        { 20, 41 },
        { 18, 41 },
        { 16, 41 },
        { 14, 41 },
        { 12, 41 },
        { 10, 41 },
        { 8, 41 },
        { 6, 41 },
        { 4, 41 },
        { 4, 39 },
        { 4, 37 },
        { 4, 35 },
        { 4, 33 },
        { 4, 31 },
        { 4, 29 },
        { 4, 27 },
        { 6, 27 },
        { 8, 27 },
        { 10, 27 },
        { 12, 27 },
        { 14, 27 },
        { 16, 27 },
        { 18, 27 },
        { 20, 27 },
        { 20, 29 },
        { 20, 31 },
        { 20, 33 },
        { 20, 35 },
        { 20, 37 },
        { 18, 37 },
        { 16, 37 },
        { 14, 37 },
        { 12, 37 },
        { 10, 37 },
        { 8, 37 },
        { 8, 35 },
        { 8, 33 },
        { 8, 31 },
        { 10, 31 },
        { 12, 31 },
        { 14, 31 },
        { 16, 31 },
        { 16, 33 },
        { 14, 33 },
    }

    -- Loads the other spikes according to the array
    spike_index = 0;
    for my_index, my_array in pairs(spike_objects3) do
        trap_spikes_right[spike_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(trap_spikes_right[spike_index]);
        trap_spikes_right[spike_index]:SetVisible(false);
        trap_spikes_right[spike_index]:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        spike_index = spike_index + 1
    end

end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to mountain shrine 2nd floor South left", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s1_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s1_script.lua", "from_shrine_2nd_floor_grotto_left");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine 2nd floor South right", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s1_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s1_script.lua", "from_shrine_2nd_floor_grotto_right");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine stairs", "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_script.lua", "from_shrine_2nd_floor_grotto");
    EventManager:RegisterEvent(event);

    -- trap
    event = vt_map.ScriptedEvent("start trap", "trap_start", "trap_update");
    EventManager:RegisterEvent(event);

    -- When dying because of the trap
    event = vt_map.MapTransitionEvent("Restart map", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s2_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s2_script.lua", GlobalManager:GetPreviousLocation());
    EventManager:RegisterEvent(event);
end

-- zones
local to_shrine_se_zone = {};
local to_shrine_sw_zone = {};
local to_stairs_zone = {};
local trap_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_se_zone = vt_map.CameraZone(42, 46, 49, 51);
    Map:AddZone(to_shrine_se_zone);
    to_shrine_sw_zone = vt_map.CameraZone(8, 12, 70, 72);
    Map:AddZone(to_shrine_sw_zone);
    to_stairs_zone = vt_map.CameraZone(5, 7, 5, 7);
    Map:AddZone(to_stairs_zone);

    trap_zone = vt_map.CameraZone(0, 26, 25, 39);
    Map:AddZone(trap_zone);
end

-- Trigger damages on the characters present on the battle front.
function _TriggerPartyDamage(damage)
    -- Adds an effect on map
    local x_pos = Map:GetScreenXCoordinate(hero:GetXPosition());
    local y_pos = Map:GetScreenYCoordinate(hero:GetYPosition());
    local map_indicator = Map:GetIndicatorSupervisor();
    map_indicator:AddDamageIndicator(x_pos, y_pos, damage, vt_video.TextStyle("text22", vt_video.Color(1.0, 0.0, 0.0, 0.9)), true);

    local index = 0;
    for index = 0, 3 do
        local char = GlobalManager:GetCharacter(index);
        if (char ~= nil) then
            -- Do not kill characters. though
            local hp_damage = damage;
            if (hp_damage >= char:GetHitPoints()) then
                hp_damage = char:GetHitPoints() - 1;
            end
            if (hp_damage > 0) then
                char:SubtractHitPoints(hp_damage);
            end
        end
    end
end

local trap_started = false;
local caught_by_trap = false;

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_sw_zone:IsCameraEntering() == true) then
        hero:SetMoving(true);
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine 2nd floor South left");
    elseif (to_shrine_se_zone:IsCameraEntering() == true) then
        hero:SetMoving(true);
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine 2nd floor South right");
    elseif (to_stairs_zone:IsCameraEntering() == true) then
        hero:SetMoving(true);
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine stairs");
    elseif (trap_started == false and trap_zone:IsCameraEntering() == true) then
        trap_started = true;
        EventManager:StartEvent("start trap");
    end

    -- Check whether the hero is dead because of trap.
    if (caught_by_trap == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetCustomAnimation("hurt", 0);
        hero:SetMoving(false);
        -- Trigger party damage.
        local hp_change = math.random(25, 40);
        _TriggerPartyDamage(hp_change);

        EventManager:StartEvent("Restart map");
        AudioManager:PlaySound("snd/battle_encounter_03.ogg");
    end
end

function _CheckForDiagonals(target)
    -- Check for diagonals. If the player is in diagonal,
    -- whe shouldn't trigger the event at all, as only straight relative position
    -- to the target sprite will work correctly.
    -- (Here used only for shrooms and stones)

    local hero_x = hero:GetXPosition();
    local hero_y = hero:GetYPosition();

    local target_x = target:GetXPosition();
    local target_y = target:GetYPosition();

    -- bottom-left
    if (hero_y > target_y + 0.3 and hero_x < target_x - 1.2) then return false; end
    -- bottom-right
    if (hero_y > target_y + 0.3 and hero_x > target_x + 1.2) then return false; end
    -- top-left
    if (hero_y < target_y - 1.5 and hero_x < target_x - 1.2) then return false; end
    -- top-right
    if (hero_y < target_y - 1.5 and hero_x > target_x + 1.2) then return false; end

    return true;
end

function _UpdateStoneMovement(stone_object, stone_direction)
    local update_time = SystemManager:GetUpdateTime();
    local movement_diff = 0.015 * update_time;

    -- We cap the max movement distance to avoid making the ball go through obstacles
    -- in case of low FPS
    if (movement_diff > 1.0) then
        movement_diff = 1.0;
    end

    local new_pos_x = stone_object:GetXPosition();
    local new_pos_y = stone_object:GetYPosition();
    local old_pos_x = stone_object:GetXPosition();
    local old_pos_y = stone_object:GetYPosition();

    -- Apply the movement
    if (stone_direction == vt_map.MapMode.NORTH) then
        new_pos_y = stone_object:GetYPosition() - movement_diff;
    elseif (stone_direction == vt_map.MapMode.SOUTH) then
        new_pos_y = stone_object:GetYPosition() + movement_diff;
    elseif (stone_direction == vt_map.MapMode.WEST) then
        new_pos_x = stone_object:GetXPosition() - movement_diff;
    elseif (stone_direction == vt_map.MapMode.EAST) then
        new_pos_x = stone_object:GetXPosition() + movement_diff;
    end

    -- Temporarily apply the new position right away to test collision properly
    stone_object:SetPosition(new_pos_x, new_pos_y);

    -- Check collision with a spike and break the spike if so.
    for my_index, my_object in pairs(trap_spikes) do
        if (my_object ~= nil) then
            if (my_object:IsCollidingWith(stone_object) == true) then
                -- Add broken spikes map object there
                local broken_spike = CreateObject(Map, "Spikes_broken1", my_object:GetXPosition(), my_object:GetYPosition());
                broken_spike:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                Map:AddFlatGroundObject(broken_spike);
                -- Play the shatter sound
                AudioManager:PlaySound("snd/magic_blast.ogg");

                my_object:SetVisible(false);
                my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            end
        end
    end
    -- Check collision with a spike and break the spike if so.
    for my_index, my_object in pairs(trap_spikes_left) do
        if (my_object ~= nil) then
            if (my_object:IsCollidingWith(stone_object) == true) then
                -- Add broken spikes map object there
                local broken_spike = CreateObject(Map, "Spikes_broken1", my_object:GetXPosition(), my_object:GetYPosition());
                broken_spike:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                Map:AddFlatGroundObject(broken_spike);
                -- Play the shatter sound
                AudioManager:PlaySound("snd/magic_blast.ogg");

                my_object:SetVisible(false);
                my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            end
        end
    end
    -- Check collision with a spike and break the spike if so.
    for my_index, my_object in pairs(trap_spikes_right) do
        if (my_object ~= nil) then
            if (my_object:IsCollidingWith(stone_object) == true) then
                -- Add broken spikes map object there
                local broken_spike = CreateObject(Map, "Spikes_broken1", my_object:GetXPosition(), my_object:GetYPosition());
                broken_spike:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                Map:AddFlatGroundObject(broken_spike);
                -- Play the shatter sound
                AudioManager:PlaySound("snd/magic_blast.ogg");

                my_object:SetVisible(false);
                my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            end
        end
    end

    -- Check the collision
    if (stone_object:IsColliding(new_pos_x, new_pos_y) == true) then
        stone_object:SetPosition(old_pos_x, old_pos_y);
        AudioManager:PlaySound("snd/stone_bump.ogg");
        Effects:ShakeScreen(0.6, 600, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);
        return true;
    end

    --  and reapply the movement if none
    stone_object:SetPosition(new_pos_x, new_pos_y);

    return false;
end

-- returns the direction the stone shall take
function _GetStoneDirection(stone)

    local hero_x = hero:GetXPosition();
    local hero_y = hero:GetYPosition();

    local stone_x = stone:GetXPosition();
    local stone_y = stone:GetYPosition();

    -- Set the stone direction
    local stone_direction = vt_map.MapMode.EAST;

    -- Determine the hero position relative to the stone
    if (hero_y > stone_y + 0.3) then
        -- the hero is below, the stone is pushed upward.
        stone_direction = vt_map.MapMode.NORTH;
    elseif (hero_y < stone_y - 1.5) then
        -- the hero is above, the stone is pushed downward.
        stone_direction = vt_map.MapMode.SOUTH;
    elseif (hero_x < stone_x - 1.2) then
        -- the hero is on the left, the stone is pushed to the right.
        stone_direction = vt_map.MapMode.EAST;
    elseif (hero_x > stone_x + 1.2) then
        -- the hero is on the right, the stone is pushed to the left.
        stone_direction = vt_map.MapMode.WEST;
    end

    return stone_direction;
end

-- Stone variables
local stone_direction1 = vt_map.MapMode.EAST;
local stone_fall_x_pos = 28;
local stone_fall_y_pos = 14;
local stone_fall_hit_ground = false;

-- Trap variables
local trap_update_time = 0;
local trap_index = 0;
local hero_is_dead = false;

-- Map Custom functions
-- Used through scripted events
map_functions = {
    stone_falls_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        stone_fall_x_pos = 28;
        stone_fall_y_pos = 34;
        stone_fall_hit_ground = false;
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    stone_falls_event_update = function()
        local update_time = SystemManager:GetUpdateTime();
        -- change the movement speed according to whether the stone is rolling
        -- or falling
        if (stone_fall_x_pos > 24.0 and stone_fall_x_pos < 25.0) then
            stone_fall_y_pos = stone_fall_y_pos + 0.015 * update_time
        end
        -- Play a sound when it is hitting the ground
        if (stone_fall_hit_ground == false and stone_fall_x_pos < 24.0) then
            stone_fall_hit_ground = true;
            AudioManager:PlaySound("snd/stone_bump.ogg");
            Effects:ShakeScreen(0.6, 600, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);
        end
        local movement_diff = 0.010 * update_time;
        stone_fall_x_pos = stone_fall_x_pos - movement_diff;
        rolling_stone1:SetPosition(stone_fall_x_pos, stone_fall_y_pos);
        if (stone_fall_x_pos <= 16.0) then
            return true;
        end
        return false;
    end,

    stone_falls_event_end = function()
        GlobalManager:SetEventValue("story", "mountain_shrine_2ndfloor_pushed_stone", 1);
        Map:PopState();
    end,

    check_diagonal_stone1 = function()
        return _CheckForDiagonals(rolling_stone1);
    end,

    start_to_move_the_stone1 = function()
        stone_direction1 = _GetStoneDirection(rolling_stone1);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    move_the_stone_update1 = function()
        return _UpdateStoneMovement(rolling_stone1, stone_direction1)
    end,

    trap_start = function()
        -- Surrounds the character of even get him
        for my_index, my_object in pairs(trap_spikes) do
            my_object:SetVisible(true);
            my_object:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
            if (my_object:IsCollidingWith(hero) == true) then
                caught_by_trap = true;
            end
        end
        AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
        trap_update_time = 0;
        trap_index = 0;
        hero_is_dead = false;
    end,

    trap_update = function()
        -- Adds one spikes on each borders, every half a second.
        trap_update_time = trap_update_time + SystemManager:GetUpdateTime();
        if (trap_update_time < 500) then
            return false;
        end
        trap_update_time = 0;

        local spike_object = trap_spikes_left[trap_index];
        if (spike_object ~= nil) then
            spike_object:SetVisible(true);
            spike_object:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        end

        spike_object = trap_spikes_right[trap_index];
        if (spike_object ~= nil) then
            spike_object:SetVisible(true);
            spike_object:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
            AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
        end

        trap_index = trap_index + 1

        -- Check collision with player and make it restart the map if so.
        if (caught_by_trap == false) then
            for my_index, my_object in pairs(trap_spikes_left) do
                if (my_object ~= nil and my_object:IsCollidingWith(hero) == true) then
                    caught_by_trap = true;
                end
            end
            for my_index, my_object in pairs(trap_spikes_right) do
                if (my_object ~= nil and my_object:IsCollidingWith(hero) == true) then
                    caught_by_trap = true;
                end
            end
            for my_index, my_object in pairs(trap_spikes) do
                if (my_object ~= nil and my_object:IsCollidingWith(hero) == true) then
                    caught_by_trap = true;
                end
            end
        end

        if (trap_index < 45) then
            return false;
        end

        return true;
    end,
}
