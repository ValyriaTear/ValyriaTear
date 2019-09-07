-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine_2nd_s2_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "data/story/common/locations/mt_elbrus.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/icy_wind.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Effects = nil

-- the main character handler
local hero = nil

-- Used for dialogues
local bronann = nil

-- the main map loading code
function Load(m)

    Map = m;
    Effects = Map:GetEffectSupervisor();
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(false);

    _CreateCharacters();
    _CreateObjects();
    _CreateEnemies();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    Map:GetEffectSupervisor():EnableAmbientOverlay("data/visuals/ambient/dark.png", 0.0, 0.0, false);

    -- Preloads the action sounds to avoid glitches
    AudioManager:LoadSound("data/sounds/stone_roll.wav", Map);
    AudioManager:LoadSound("data/sounds/stone_bump.ogg", Map);
    AudioManager:LoadSound("data/sounds/opening_sword_unsheathe.wav", Map);
    AudioManager:LoadSound("data/sounds/magic_blast.ogg", Map);
    AudioManager:LoadSound("data/sounds/battle_encounter_03.ogg", Map);
    AudioManager:LoadSound("data/sounds/cave-in.ogg", Map);
    AudioManager:LoadSound("data/sounds/heavy_bump.wav", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 16, 68, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetMapData():GetPreviousLocation() == "from_shrine_2nd_floor_south_east") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(48.5, 48.0);
    elseif (GlobalManager:GetMapData():GetPreviousLocation() == "from_shrine_stairs") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(12.0, 8.0);
    end

    bronann = CreateSprite(Map, "Bronann", 0, 0, vt_map.MapMode.GROUND_OBJECT)
    bronann:SetDirection(vt_map.MapMode.EAST)
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED)
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION)
    bronann:SetVisible(false)
end

function _add_flame(x, y)
    vt_map.SoundObject.Create("data/sounds/campfire.ogg", x, y, 10.0);

    local object = CreateObject(Map, "Flame1", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();

    vt_map.Halo.Create("data/visuals/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    vt_map.Halo.Create("data/visuals/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

local rolling_stone1 = nil

-- Arrays of spikes objects
local trap_spikes = {}
local trap_spikes_left = {}
local trap_spikes_right = {}

local mini_boss = nil

function _CreateObjects()
    local object = nil
    local event = nil

    CreateObject(Map, "Rock1", 31, 14, vt_map.MapMode.GROUND_OBJECT);

    -- Snow effects
    vt_map.ParticleObject.Create("data/story/mt_elbrus/particles_snow_south_entrance.lua", 16, 72, vt_map.MapMode.GROUND_OBJECT);
    vt_map.Halo.Create("data/visuals/lights/torch_light_mask.lua", 16, 79,
                vt_video.Color(1.0, 1.0, 1.0, 0.8));

    vt_map.ParticleObject.Create("data/story/mt_elbrus/particles_snow_south_entrance.lua", 50, 50, vt_map.MapMode.GROUND_OBJECT);
    vt_map.Halo.Create("data/visuals/lights/torch_light_mask.lua", 51, 57,
                vt_video.Color(1.0, 1.0, 1.0, 0.8));

    _add_flame(15.5, 3);

    -- The stone used to get through this enigma
    rolling_stone1 = CreateObject(Map, "Rolling Stone2", 34, 34, vt_map.MapMode.GROUND_OBJECT);

    -- events on the lower level
    vt_map.IfEvent.Create("Check hero position for rolling stone 1", "check_diagonal_stone1", "Push the rolling stone 1", "");
    vt_map.ScriptedEvent.Create("Push the rolling stone 1", "start_to_move_the_stone1", "move_the_stone_update1")

    -- events on the upper level
    event = vt_map.ScriptedEvent.Create("Make rolling stone1 fall event start", "stone_falls_event_start", "stone_falls_event_update");
    event:AddEventLinkAtEnd("Hero speaks about the red stone");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Phew, this one sure is heavier and more sturdy.");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    event = vt_map.DialogueEvent.Create("Hero speaks about the red stone", dialogue);
    event:AddEventLinkAtEnd("Make rolling stone1 fall event end");

    vt_map.ScriptedEvent.Create("Make rolling stone1 fall event end", "stone_falls_event_end", "");

    -- Set the stone event according to the events
    if (GlobalManager:GetGameEvents():GetEventValue("story", "mountain_shrine_2ndfloor_pushed_stone") == 0) then
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
        { 9, 23 },
        { 11, 23 },
        { 13, 23 },
        { 15, 23 },
        { 17, 23 },
        { 19, 23 },
        { 21, 23 },
        { 23, 23 },
        { 25, 23 },
        { 27, 23 },
        { 29, 23 },
        -- lower border
        { 9, 43 },
        { 11, 43 },
        { 13, 43 },
        { 15, 43 },
        { 17, 43 },
        { 19, 43 },
        { 21, 43 },
        { 23, 43 },
        { 25, 43 },
        { 27, 43 },
        { 29, 43 },
        -- left
        { 8, 25 },
        { 8, 27 },
        { 8, 29 },
        { 8, 31 },
        { 8, 33 },
        { 8, 35 },
        { 8, 37 },
        { 8, 39 },
        { 8, 41 },
        -- right
        { 30, 25 },
        { 30, 27 },
        { 30, 29 },
        { 30, 31 },
        { 30, 33 },
        { 30, 35 },
        { 30, 37 },
        { 30, 39 },
        { 30, 41 },
    }

    -- Loads the spikes according to the array
    for my_index, my_array in pairs(spike_objects) do
        trap_spikes[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2], vt_map.MapMode.GROUND_OBJECT);
        trap_spikes[my_index]:SetVisible(false);
        trap_spikes[my_index]:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    -- Creates the other spikes
    local spike_objects2 = {
        -- from top-left
        { 10, 25 },
        { 12, 25 },
        { 14, 25 },
        { 16, 25 },
        { 18, 25 },
        { 20, 25 },
        { 22, 25 },
        { 24, 25 },
        { 26, 25 },
        { 28, 25 },
        { 28, 27 },
        { 28, 29 },
        { 28, 31 },
        { 28, 33 },
        { 28, 35 },
        { 28, 37 },
        { 28, 39 },
        { 26, 39 },
        { 24, 39 },
        { 22, 39 },
        { 20, 39 },
        { 18, 39 },
        { 16, 39 },
        { 14, 39 },
        { 12, 39 },
        { 12, 37 },
        { 12, 35 },
        { 12, 33 },
        { 12, 31 },
        { 12, 29 },
        { 14, 29 },
        { 16, 29 },
        { 18, 29 },
        { 20, 29 },
        { 22, 29 },
        { 24, 29 },
        { 24, 31 },
        { 24, 33 },
        { 24, 35 },
        { 22, 35 },
        { 20, 35 },
        { 18, 35 },
        { 16, 35 },
        { 16, 33 },
        { 18, 33 },
    }

    -- Loads the other spikes according to the array
    local spike_index = 0;
    for my_index, my_array in pairs(spike_objects2) do
        trap_spikes_left[spike_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2], vt_map.MapMode.GROUND_OBJECT);
        trap_spikes_left[spike_index]:SetVisible(false);
        trap_spikes_left[spike_index]:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        spike_index = spike_index + 1
    end

    local spike_objects3 = {
        -- from bottom-right
        { 28, 41 },
        { 26, 41 },
        { 24, 41 },
        { 22, 41 },
        { 20, 41 },
        { 18, 41 },
        { 16, 41 },
        { 14, 41 },
        { 12, 41 },
        { 10, 41 },
        { 10, 39 },
        { 10, 37 },
        { 10, 35 },
        { 10, 33 },
        { 10, 31 },
        { 10, 29 },
        { 10, 27 },
        { 12, 27 },
        { 14, 27 },
        { 16, 27 },
        { 18, 27 },
        { 20, 27 },
        { 22, 27 },
        { 24, 27 },
        { 26, 27 },
        { 26, 29 },
        { 26, 31 },
        { 26, 33 },
        { 26, 35 },
        { 26, 37 },
        { 24, 37 },
        { 22, 37 },
        { 20, 37 },
        { 18, 37 },
        { 16, 37 },
        { 14, 37 },
        { 14, 35 },
        { 14, 33 },
        { 14, 31 },
        { 16, 31 },
        { 18, 31 },
        { 20, 31 },
        { 22, 31 },
        { 22, 33 },
        { 20, 33 },
    }

    -- Loads the other spikes according to the array
    spike_index = 0;
    for my_index, my_array in pairs(spike_objects3) do
        trap_spikes_right[spike_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2], vt_map.MapMode.GROUND_OBJECT);
        trap_spikes_right[spike_index]:SetVisible(false);
        trap_spikes_right[spike_index]:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        spike_index = spike_index + 1
    end

    -- Mini-boss
    mini_boss = CreateObject(Map, "Dorver1", 33, 10, vt_map.MapMode.GROUND_OBJECT);
    if (GlobalManager:GetGameEvents():GetEventValue("story", "mt_elbrus_shrine_trap_boss_done") == 1) then
        mini_boss:SetPosition(0, 0);
        mini_boss:SetVisible(false);
        mini_boss:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    -- A trigger that will open ground floor enigma map.
    object = vt_map.TriggerObject.Create("mt elbrus shrine 2nd s2 trigger",
                                         vt_map.MapMode.FLATGROUND_OBJECT,
                                         "data/entities/map/triggers/stone_trigger1_off.lua",
                                         "data/entities/map/triggers/stone_trigger1_on.lua",
                                         "", "Enigma map open event");
    object:SetPosition(26, 10);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    vt_map.MapTransitionEvent.Create("to mountain shrine 2nd floor South left", "data/story/mt_elbrus/mt_elbrus_shrine_2nd_s1_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_shrine_2nd_s1_script.lua", "from_shrine_2nd_floor_grotto_left");

    vt_map.MapTransitionEvent.Create("to mountain shrine 2nd floor South right", "data/story/mt_elbrus/mt_elbrus_shrine_2nd_s1_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_shrine_2nd_s1_script.lua", "from_shrine_2nd_floor_grotto_right");

    vt_map.MapTransitionEvent.Create("to mountain shrine stairs", "data/story/mt_elbrus/mt_elbrus_shrine_stairs_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_shrine_stairs_script.lua", "from_shrine_2nd_floor_grotto");

    -- trap
    vt_map.ScriptedEvent.Create("start trap", "trap_start", "trap_update");

    -- When dying because of the trap
    vt_map.MapTransitionEvent.Create("Restart map", "data/story/mt_elbrus/mt_elbrus_shrine_2nd_s2_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_shrine_2nd_s2_script.lua", GlobalManager:GetMapData():GetPreviousLocation());

    -- Mini boss event
    event = vt_map.ScriptedEvent.Create("Mini-Boss fight", "mini_boss_start", "");
    event:AddEventLinkAtEnd("Mini-boss fight battle start");

    event = vt_map.BattleEncounterEvent.Create("Mini-boss fight battle start");
    event:SetMusic("data/music/accion-OGA-djsaryon.ogg");
    event:SetBackground("data/battles/battle_scenes/desert_cave/desert_cave.png");
    event:AddScript("data/battles/battle_scenes/desert_cave_battle_anim.lua");
    event:SetBoss(true);
    event:AddEnemy(20);
    event:AddEventLinkAtEnd("Mini-boss fight end");

    vt_map.ScriptedEvent.Create("Mini-boss fight end", "mini_boss_end", "");

    -- Enigma map open event
    event = vt_map.ScriptedEvent.Create("Enigma map open event", "enigma_map_start", "");
    event:AddEventLinkAtEnd("Enigma map dialogue");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I can feel something moving far below.");
    dialogue:AddLine(text, bronann);
    event = vt_map.DialogueEvent.Create("Enigma map dialogue", dialogue);
    event:AddEventLinkAtEnd("Enigma map open event end");

    vt_map.ScriptedEvent.Create("Enigma map open event end", "enigma_map_end", "");
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("data/music/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("data/battles/battle_scenes/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    enemy:AddBattleScript("data/battles/battle_scenes/desert_cave_battle_anim.lua");
end

function _CreateEnemies()
    local enemy = nil
    local roam_zone = nil

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(38, 52, 12, 32);

    enemy = CreateEnemySprite(Map, "Skeleton");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(19);
    enemy:AddEnemy(19);
    enemy:AddEnemy(19);
    enemy:AddEnemy(19);
    roam_zone:AddEnemy(enemy, 2);
end

-- zones
local to_shrine_se_zone = nil
local to_shrine_sw_zone = nil
local to_stairs_zone = nil
local trap_zone = nil
local mini_boss_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_se_zone = vt_map.CameraZone.Create(48, 52, 49, 51);
    to_shrine_sw_zone = vt_map.CameraZone.Create(14, 18, 70, 72);
    to_stairs_zone = vt_map.CameraZone.Create(11, 13, 5, 7);

    trap_zone = vt_map.CameraZone.Create(6, 32, 25, 39);
    trap_zone:AddSection(27, 32, 20, 25);

    mini_boss_zone = vt_map.CameraZone.Create(36, 38, 5, 10);
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
    elseif (mini_boss_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetGameEvents():GetEventValue("story", "mt_elbrus_shrine_trap_boss_done") == 0) then
            EventManager:StartEvent("Mini-Boss fight");
        end
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
        AudioManager:PlaySound("data/sounds/battle_encounter_03.ogg");
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
                local broken_spike = CreateObject(Map, "Spikes_broken1", my_object:GetXPosition(), my_object:GetYPosition(),
                                                  vt_map.MapMode.FLATGROUND_OBJECT);
                broken_spike:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                -- Play the shatter sound
                AudioManager:PlaySound("data/sounds/magic_blast.ogg");

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
                local broken_spike = CreateObject(Map, "Spikes_broken1", my_object:GetXPosition(), my_object:GetYPosition(),
                                                  vt_map.MapMode.FLATGROUND_OBJECT);
                broken_spike:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                -- Play the shatter sound
                AudioManager:PlaySound("data/sounds/magic_blast.ogg");

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
                local broken_spike = CreateObject(Map, "Spikes_broken1", my_object:GetXPosition(), my_object:GetYPosition(),
                                                  vt_map.MapMode.FLATGROUND_OBJECT);
                broken_spike:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                -- Play the shatter sound
                AudioManager:PlaySound("data/sounds/magic_blast.ogg");

                my_object:SetVisible(false);
                my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            end
        end
    end

    -- Check the collision
    if (stone_object:IsColliding(new_pos_x, new_pos_y) == true) then
        stone_object:SetPosition(old_pos_x, old_pos_y);
        AudioManager:PlaySound("data/sounds/heavy_bump.wav");
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
local stone_fall_x_pos = 34;
local stone_fall_y_pos = 14;
local stone_fall_hit_ground = false;
local stone_x_force = 0.0150;
local stone_friction = 0.0002;

-- Trap variables
local trap_update_time = 0;
local trap_index = 0;
local hero_is_dead = false;

-- Map Custom functions
-- Used through scripted events
map_functions = {
    stone_falls_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        stone_fall_x_pos = 34;
        stone_fall_y_pos = 34;
        stone_fall_hit_ground = false;
        AudioManager:PlaySound("data/sounds/stone_roll.wav");

        stone_x_force = 0.0150;
        stone_friction = 0.0002;
    end,

    stone_falls_event_update = function()
        local update_time = SystemManager:GetUpdateTime();
        -- change the movement speed according to whether the stone is rolling
        -- or falling
        if (stone_fall_x_pos > 30.0 and stone_fall_x_pos < 31.0) then
            stone_fall_y_pos = stone_fall_y_pos + 0.015 * update_time
        end
        -- Play a sound when it is hitting the ground
        if (stone_fall_hit_ground == false and stone_fall_x_pos < 30.0) then
            stone_fall_hit_ground = true;
            AudioManager:PlaySound("data/sounds/heavy_bump.wav");
            Effects:ShakeScreen(0.6, 600, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);
        end
        local movement_diff = stone_x_force * update_time;
        stone_fall_x_pos = stone_fall_x_pos - movement_diff;
        stone_x_force = stone_x_force - stone_friction;
        if (stone_x_force <= 0.0) then
            return true;
        end
        rolling_stone1:SetPosition(stone_fall_x_pos, stone_fall_y_pos);

        return false;
    end,

    stone_falls_event_end = function()
        GlobalManager:GetGameEvents():SetEventValue("story", "mountain_shrine_2ndfloor_pushed_stone", 1);
        Map:PopState();
    end,

    check_diagonal_stone1 = function()
        return _CheckForDiagonals(rolling_stone1);
    end,

    start_to_move_the_stone1 = function()
        stone_direction1 = _GetStoneDirection(rolling_stone1);
        AudioManager:PlaySound("data/sounds/stone_roll.wav");
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
        AudioManager:PlaySound("data/sounds/opening_sword_unsheathe.wav");
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
            AudioManager:PlaySound("data/sounds/opening_sword_unsheathe.wav");
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

    mini_boss_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

    mini_boss_end = function()
        Map:PopState();
        GlobalManager:GetGameEvents():SetEventValue("story", "mt_elbrus_shrine_trap_boss_done", 1)

        mini_boss:SetPosition(0, 0);
        mini_boss:SetVisible(false);
        mini_boss:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        mini_boss:ClearEventWhenTalking();
    end,

    enigma_map_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        Effects:ShakeScreen(0.6, 2000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_LINEAR);
        AudioManager:PlaySound("data/sounds/cave-in.ogg");
    end,

    enigma_map_end = function()
        Map:PopState();
    end,
}
