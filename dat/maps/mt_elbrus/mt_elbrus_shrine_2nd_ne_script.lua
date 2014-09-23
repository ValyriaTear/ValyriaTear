-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine_2nd_ne_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus Shrine"
map_image_filename = "img/menus/locations/mountain_shrine.png"
map_subname = "2nd Floor"

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
    _CreateEnemies();

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);

    -- Preloads the action sounds to avoid glitches
    AudioManager:LoadSound("snd/stone_roll.wav", Map);
    AudioManager:LoadSound("snd/stone_bump.ogg", Map);
    AudioManager:LoadSound("snd/opening_sword_unsheathe.wav", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();

    _CheckStoneAndTriggersCollision();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from shrine main room)
    hero = CreateSprite(Map, "Bronann", 10.0, 12.5);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    Map:AddGroundObject(hero);

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_2nd_floor_south") then
        hero:SetPosition(28, 36);
        hero:SetDirection(vt_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_1st_floor") then
        hero:SetPosition(24, 12);
        hero:SetDirection(vt_map.MapMode.SOUTH);
    end
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
    roam_zone = vt_map.EnemyZone(6, 35, 13, 39);

    enemy = CreateEnemySprite(Map, "Skeleton");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(19);
    enemy:SetTimeToRespawn(25000);
    roam_zone:AddEnemy(enemy, Map, 1);
    Map:AddZone(roam_zone);
end

-- Triggers and stones
local num_of_triggers = 8;

-- arrays storing the objects and their states
local stone_triggers = {};
local rolling_stones = {};
local stone_directions = {};
local stone_rolling = {};

-- Fences preventing from getting through
local fence1_trigger1 = {};
local fence2_trigger1 = {};
local fence1_trigger1_x_position = 27.0;
local fence2_trigger1_x_position = 29.0;

local trap_spikes = {};

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}

    -- Snow effect
    object = vt_map.ParticleObject("dat/maps/mt_elbrus/particles_snow_south_entrance.lua", 28, 40);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 28, 47,
        vt_video.Color(1.0, 1.0, 1.0, 0.8));

    _add_flame(17.5, 7);
    _add_flame(31.5, 7);

    object = CreateObject(Map, "Vase3", 21, 35);
    Map:AddGroundObject(object);

    object = CreateObject(Map, "Candle Holder1", 21, 11);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Candle Holder1", 27, 11);
    Map:AddGroundObject(object);

    object = CreateObject(Map, "Stone Fence1", 5, 26);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 7, 28);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 9, 30);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 11, 32);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 11, 34);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 11, 36);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 11, 38);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 25, 39);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 31, 39);
    Map:AddGroundObject(object);

    object = CreateObject(Map, "Stone Fence1", 15, 24.5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 37.3, 23);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 35, 15);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 13, 31);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 35, 33);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 13, 17);
    Map:AddGroundObject(object);

    trap_spikes = CreateObject(Map, "Spikes1", 25, 24);
    trap_spikes:SetVisible(false);
    trap_spikes:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(trap_spikes);

    -- The stone triggers that will open the passage
    local i = 1;
    local pos_x = 0.0;
    local pos_y = 0.0;

    local passage_open = false;
    if (GlobalManager:GetEventValue("story", "mt_shrine_2nd_floor_NE_open") == 1) then
        passage_open = true;
    end

    for i = 1, num_of_triggers do
        if (i == 1) then
            pos_x = 15;
            pos_y = 17;
        elseif (i == 2) then
            pos_x = 17;
            pos_y = 19;
        elseif (i == 3) then
            pos_x = 35;
            pos_y = 17;
        elseif (i == 4) then
            pos_x = 33;
            pos_y = 19;
        elseif (i == 5) then
            pos_x = 15;
            pos_y = 31;
        elseif (i == 6) then
            pos_x = 17;
            pos_y = 29;
        elseif (i == 7) then
            pos_x = 35;
            pos_y = 31;
        elseif (i == 8) then
            pos_x = 33;
            pos_y = 29;
        end

        stone_triggers[i] = vt_map.TriggerObject("mt elbrus shrine 2nd NE trigger "..i,
                                 "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                                 "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                                 "",
                                 "Check Gate");
        stone_triggers[i]:SetObjectID(Map.object_supervisor:GenerateObjectID());
        stone_triggers[i]:SetTriggerableByCharacter(false); -- Only an event can trigger it
        stone_triggers[i]:SetPosition(pos_x, pos_y);
        Map:AddFlatGroundObject(stone_triggers[i]);

        -- Reset the trigger state when not all of them have been pushed
        if (passage_open == false) then
            stone_triggers[i]:SetState(false);
        end
    end

    -- Add blocks preventing from using the passage
    -- Sets the passage open if the enemies were already beaten
    if (passage_open == true) then
        fence1_trigger1_x_position = 25.0;
        fence2_trigger1_x_position = 31.0;
    end
    fence1_trigger1 = CreateObject(Map, "Stone Fence1", fence1_trigger1_x_position, 39);
    Map:AddGroundObject(fence1_trigger1);
    fence2_trigger1 = CreateObject(Map, "Stone Fence1", fence2_trigger1_x_position, 39);
    Map:AddGroundObject(fence2_trigger1);

    local i = 1;
    local pos_x = 0.0;
    local pos_y = 0.0;
    for i = 1, num_of_triggers do

        if (i == 1) then
            pos_x = 15;
            pos_y = 19;
        elseif (i == 2) then
            pos_x = 17;
            pos_y = 23;
        elseif (i == 3) then
            pos_x = 19;
            pos_y = 29;
        elseif (i == 4) then
            pos_x = 17;
            pos_y = 31;
        elseif (i == 5) then
            pos_x = 33;
            pos_y = 27;
        elseif (i == 6) then
            pos_x = 35;
            pos_y = 29;
        elseif (i == 7) then
            pos_x = 31;
            pos_y = 19;
        elseif (i == 8) then
            pos_x = 33;
            pos_y = 17;
        end

        rolling_stones[i] = CreateObject(Map, "Rolling Stone", pos_x, pos_y);
        rolling_stones[i]:SetEventWhenTalking("Check hero position for rolling stone "..i);
        Map:AddGroundObject(rolling_stones[i]);
        event = vt_map.IfEvent("Check hero position for rolling stone "..i, "check_diagonal_stone"..i, "Push the rolling stone "..i, "");
        EventManager:RegisterEvent(event);
        event = vt_map.ScriptedEvent("Push the rolling stone "..i, "start_to_move_the_stone"..i, "move_the_stone_update"..i)
        EventManager:RegisterEvent(event);

        -- Setup the initial stone direction value
        stone_directions[i] = vt_map.MapMode.EAST;
        stone_rolling[i] = false;
    end
end

function _add_flame(x, y)
    local object = vt_map.SoundObject("snd/campfire.ogg", x, y, 10.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;

    object = CreateObject(Map, "Flame1", x, y);
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

    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor", "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_script.lua", "from_shrine_2nd_floor");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine 2nd floor South", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s1_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s1_script.lua", "from_shrine_2nd_floor_NE_room");
    EventManager:RegisterEvent(event);

    event = vt_map.IfEvent("Check Gate", "check_triggers", "Open Gate", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Open Gate", "open_passage_start", "open_passage_update");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Trigger spikes", "trigger_spikes_start", "trigger_spikes_update");
    EventManager:RegisterEvent(event);
end

-- zones
local to_shrine_1st_floor_room_zone = {};
local to_shrine_SE_room_zone = {};
local spike_trap_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_1st_floor_room_zone = vt_map.CameraZone(22, 26, 9, 11);
    Map:AddZone(to_shrine_1st_floor_room_zone);

    to_shrine_SE_room_zone = vt_map.CameraZone(24, 32, 38, 42);
    Map:AddZone(to_shrine_SE_room_zone);

    spike_trap_zone = vt_map.CameraZone(24, 26, 22, 24);
    Map:AddZone(spike_trap_zone);
end

local trap_triggered = false;

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_1st_floor_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine 1st floor");
    elseif (to_shrine_SE_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine 2nd floor South");
    elseif (spike_trap_zone:IsCameraEntering() == true) then
        if (trap_spikes:IsVisible() == false) then
            EventManager:StartEvent("Trigger spikes", 200);
        end
    end
end

function _CheckStoneAndTriggersCollision()
    -- Check triggers
    -- NOTE: Push the trigger only when the stone is not rolling.
    local tr = 1
    local st = 1

    for tr = 1, num_of_triggers do
        for st = 1, num_of_triggers do
            -- If the trigger is pushed, then don't look into it.
            if (stone_triggers[tr]:GetState() == true) then
                break;
            end

            if (stone_rolling[st] == false and stone_triggers[tr]:IsCollidingWith(rolling_stones[st]) == true) then
                stone_triggers[tr]:SetState(true)
                break;
            end
        end
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

    -- Check the collision
    if (stone_object:IsColliding(new_pos_x, new_pos_y) == true) then
        AudioManager:PlaySound("snd/stone_bump.ogg");
        return true;
    end

    --  and apply the movement if none
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

local spike_appearance_time = 0;

-- Map Custom functions
-- Used through scripted events
map_functions = {
    check_triggers = function()
        -- check whether all triggers are pushed
        local i = 1;
        for i = 1, num_of_triggers do
            if (stone_triggers[i]:GetState() == false) then
                return false;
            end
        end

        return true;
    end,

    -- Opens the south passage, by moving the fences out of the way.
    open_passage_start = function()
        fence1_trigger1_x_position = 27.0;
        fence2_trigger1_x_position = 29.0;
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    open_passage_update = function()
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.005 * update_time;

        fence1_trigger1_x_position = fence1_trigger1_x_position - movement_diff;
        fence1_trigger1:SetXPosition(fence1_trigger1_x_position);

        fence2_trigger1_x_position = fence2_trigger1_x_position + movement_diff;
        fence2_trigger1:SetXPosition(fence2_trigger1_x_position);

        if (fence1_trigger1_x_position <= 25.0) then
            GlobalManager:SetEventValue("story", "mt_shrine_2nd_floor_NE_open", 1);
            return true;
        end
        return false;
    end,

    trigger_spikes_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        spike_appearance_time = 0;
        trap_spikes:SetVisible(true);
        AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
        hero:SetCustomAnimation("hurt", 800);
        hero:SetMoving(false);
        -- Trigger party damage.
        local hp_change = math.random(25, 40);
        _TriggerPartyDamage(hp_change);
    end,

    trigger_spikes_update = function()
        spike_appearance_time = spike_appearance_time + SystemManager:GetUpdateTime();
        if (spike_appearance_time > 800) then
            Map:PopState();
            -- Hide spikes
            trap_spikes:SetVisible(false);
            AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
            return true;
        end
        return false;
    end,
}

-- Init all the stones map_functions...
local index = 1;
for index = 1, num_of_triggers do
    map_functions["check_diagonal_stone"..index] = function()
        return _CheckForDiagonals(rolling_stones[index]);
    end

    map_functions["start_to_move_the_stone"..index] = function()
        stone_directions[index] = _GetStoneDirection(rolling_stones[index]);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end

    map_functions["move_the_stone_update"..index] = function()
        -- Stores whether the stone is rolling
        stone_rolling[index] = not _UpdateStoneMovement(rolling_stones[index], stone_directions[index])
        return not stone_rolling[index];
    end
end

--for i, v in ipairs(map_functions) do print(i, v) end

