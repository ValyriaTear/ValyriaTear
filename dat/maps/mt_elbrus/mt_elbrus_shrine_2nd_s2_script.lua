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
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_2nd_floor_south_east") then
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
    rolling_stone1 = CreateObject(Map, "Rolling Stone", 28, 34);
    Map:AddGroundObject(rolling_stone1);

    -- events on the lower level
    event = vt_map.IfEvent("Check hero position for rolling stone 1", "check_diagonal_stone1", "Push the rolling stone 1", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 1", "start_to_move_the_stone1", "move_the_stone_update1")
    EventManager:RegisterEvent(event);

    -- events on the upper level
    event = vt_map.ScriptedEvent("Make rolling stone1 fall event start", "stone_falls_event_start", "stone_falls_event_update");
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
end

-- zones
local to_shrine_se_zone = {};
local to_shrine_sw_zone = {};
local to_stairs_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_se_zone = vt_map.CameraZone(42, 46, 49, 51);
    Map:AddZone(to_shrine_se_zone);
    to_shrine_sw_zone = vt_map.CameraZone(8, 12, 70, 72);
    Map:AddZone(to_shrine_sw_zone);
    to_stairs_zone = vt_map.CameraZone(5, 7, 5, 7);
    Map:AddZone(to_stairs_zone);
end

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

local stone_direction1 = vt_map.MapMode.EAST;

local stone_fall_x_pos = 28;
local stone_fall_y_pos = 14;
local stone_fall_hit_ground = false;

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
        end
        local movement_diff = 0.010 * update_time;
        stone_fall_x_pos = stone_fall_x_pos - movement_diff;
        rolling_stone1:SetPosition(stone_fall_x_pos, stone_fall_y_pos);
        if (stone_fall_x_pos <= 16.0) then
            GlobalManager:SetEventValue("story", "mountain_shrine_2ndfloor_pushed_stone", 1);
            Map:PopState();
            return true;
        end
        return false;
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
}
