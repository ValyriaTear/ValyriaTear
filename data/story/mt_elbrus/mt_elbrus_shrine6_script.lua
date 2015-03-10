-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine6_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus Shrine"
map_image_filename = "img/menus/locations/mountain_shrine.png"
map_subname = "1st Floor"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "music/mountain_shrine.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Script = nil

-- the main character handler
local hero = nil

-- the main map loading code
function Load(m)

    Map = m;
    Script = Map:GetScriptSupervisor();
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

    -- Add a mediumly dark overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);

    -- Preloads the action sounds to avoid glitches
    AudioManager:LoadSound("sounds/stone_roll.wav", Map);
    AudioManager:LoadSound("sounds/stone_bump.ogg", Map);
    AudioManager:LoadSound("sounds/trigger_on.wav", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();

    -- Check whether the triggers are toggled.
    _CheckStoneAndTriggersCollision();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 16, 11, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_NW_left_door") then
        hero:SetPosition(16, 11);
        hero:SetDirection(vt_map.MapMode.SOUTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_NW_right_door") then
        hero:SetPosition(28, 11);
        hero:SetDirection(vt_map.MapMode.SOUTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_SE_top_door") then
        hero:SetPosition(42.5, 24);
        hero:SetDirection(vt_map.MapMode.WEST);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_SE_bottom_door") then
        hero:SetPosition(42.5, 34);
        hero:SetDirection(vt_map.MapMode.WEST);
    end
end

-- Flames preventing from getting through
local fence1_trigger1 = nil
local fence2_trigger1 = nil
local fence1_trigger2 = nil
local fence2_trigger2 = nil
local fence3_trigger2 = nil

local stone_trigger1 = nil

local rolling_stone1 = nil
local rolling_stone2 = nil
local rolling_stone2_out = false;

function _CreateObjects()
    _add_flame(9.5, 7);
    _add_flame(33.5, 7);

    local chest = CreateTreasure(Map, "mt_shrine6_chest1", "Wood_Chest2", 7, 26, vt_map.MapMode.GROUND_OBJECT);
    chest:AddItem(15, 1); -- Cure poison
    chest:AddItem(4001, 1); -- Escape smoke

    CreateObject(Map, "Candle Holder1", 43, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Candle Holder1", 43, 30, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 13, 11, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 19, 11, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 39, 12, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 41, 14, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 43, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 26, 19, vt_map.MapMode.GROUND_OBJECT);

    -- Add flames preventing from using the doors
    -- Top Right door: Unlocked by trigger
    local fence1_trigger1_x_position = 27.0;
    local fence2_trigger1_x_position = 29.0;
    local fence3_trigger1_y_position = 13.0;
    -- Sets the passage open if the enemies were already beaten
    if (GlobalManager:GetEventValue("triggers", "mt elbrus shrine 6 trigger 1") == 1) then
        fence1_trigger1_x_position = 25.0;
        fence2_trigger1_x_position = 31.0;
        fence3_trigger1_y_position = 14.0;
    end

    fence1_trigger1 = CreateObject(Map, "Stone Fence1", fence1_trigger1_x_position, 11, vt_map.MapMode.GROUND_OBJECT);
    fence2_trigger1 = CreateObject(Map, "Stone Fence1", fence2_trigger1_x_position, 11, vt_map.MapMode.GROUND_OBJECT);
    fence3_trigger1 = CreateObject(Map, "Stone Fence1", 34, fence3_trigger1_y_position, vt_map.MapMode.GROUND_OBJECT);

    -- Bottom right door: Unlocked by switch
    local fence1_trigger2_y_position = 34.0;
    local fence2_trigger2_y_position = 36.0;
    -- Sets the passage open if the enemies were already beaten
    if (GlobalManager:GetEventValue("triggers", "mt elbrus shrine 8 gate 7 trigger") == 1) then
        fence1_trigger2_y_position = 32.0;
        fence2_trigger2_y_position = 38.0;
    end

    fence1_trigger2 = CreateObject(Map, "Stone Fence1", 43.0, fence1_trigger2_y_position, vt_map.MapMode.GROUND_OBJECT);
    fence2_trigger2 = CreateObject(Map, "Stone Fence1", 43.0, fence2_trigger2_y_position, vt_map.MapMode.GROUND_OBJECT);

    -- The two stone trigger will open the gate to the second floor
    stone_trigger1 = vt_map.TriggerObject.Create("mt elbrus shrine 6 trigger 1",
                                                 vt_map.MapMode.FLATGROUND_OBJECT,
                                                 "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                                                 "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                                                 "",
                                                 "Check triggers");
    stone_trigger1:SetPosition(26, 17);
    stone_trigger1:SetTriggerableByCharacter(false); -- Only an event can trigger it

    vt_map.ScriptedEvent.Create("Check triggers", "check_triggers", "")


    -- The stones used to get through this enigma
    rolling_stone1 = CreateObject(Map, "Rolling Stone", 38, 34, vt_map.MapMode.GROUND_OBJECT);
    vt_map.IfEvent.Create("Check hero position for rolling stone 1", "check_diagonal_stone1", "Push the rolling stone 1", "");
    vt_map.ScriptedEvent.Create("Push the rolling stone 1", "start_to_move_the_stone1", "move_the_stone_update1")

    if (GlobalManager:GetEventValue("story", "mt_shrine_1st_floor_stone1_through_1st_door") == 1) then
        if (GlobalManager:GetEventValue("triggers", "mt elbrus shrine 6 trigger 1") == 1) then
            rolling_stone1:SetPosition(26, 17);
        end
        rolling_stone1:SetEventWhenTalking("Check hero position for rolling stone 1");
    else
        rolling_stone1:SetVisible(false);
        rolling_stone1:SetVisible(false);
        rolling_stone1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    rolling_stone2 = CreateObject(Map, "Rolling Stone", 38, 34, vt_map.MapMode.GROUND_OBJECT);
    vt_map.IfEvent.Create("Check hero position for rolling stone 2", "check_diagonal_stone2", "Push the rolling stone 2", "");
    vt_map.ScriptedEvent.Create("Push the rolling stone 2", "start_to_move_the_stone2", "move_the_stone_update2")


    if (GlobalManager:GetEventValue("story", "mt_shrine_1st_floor_stone2_through_1st_door") == 1
            and GlobalManager:GetEventValue("story", "mt_shrine_1st_floor_stone2_through_2nd_door") == 0) then
        rolling_stone2:SetEventWhenTalking("Check hero position for rolling stone 2");
    else
        rolling_stone2:SetVisible(false);
        rolling_stone2:SetVisible(false);
        rolling_stone2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
end

function _add_flame(x, y)
    vt_map.SoundObject.Create("sounds/campfire.ogg", x, y, 10.0);

    local object = CreateObject(Map, "Flame1", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();

    vt_map.Halo.Create("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    vt_map.Halo.Create("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("music/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_shrine.png");
    enemy:AddBattleScript("dat/battles/mountain_shrine_battle_anim.lua");
end

function _CreateEnemies()
    local enemy = nil

    -- Adds 6 zones with one enemy each
    local i = 0;
    while i < 6 do
        -- Monsters that can only be beaten once
        -- Hint: left, right, top, bottom
        local roam_zone = vt_map.EnemyZone.Create(7, 20, 22, 33);
        enemy = CreateEnemySprite(Map, "Skeleton");
        _SetBattleEnvironment(enemy);
        enemy:NewEnemyParty();
        enemy:AddEnemy(19); -- Skeleton
        enemy:AddEnemy(19);
        enemy:AddEnemy(19);
        enemy:AddEnemy(16); -- Rat
        enemy:NewEnemyParty();
        enemy:AddEnemy(16);
        enemy:AddEnemy(19);
        enemy:AddEnemy(17); -- Thing
        enemy:AddEnemy(16);
        roam_zone:AddEnemy(enemy, 1);

        i = i + 1;
    end
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    vt_map.MapTransitionEvent.Create("to mountain shrine 1st floor NW room - left door", "dat/maps/mt_elbrus/mt_elbrus_shrine5_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine5_script.lua", "from_shrine_first_floor_SW_left_door");

    vt_map.MapTransitionEvent.Create("to mountain shrine 1st floor NW room - right door", "dat/maps/mt_elbrus/mt_elbrus_shrine5_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine5_script.lua", "from_shrine_first_floor_SW_right_door");

    vt_map.MapTransitionEvent.Create("to mountain shrine 1st floor SE room - top door", "dat/maps/mt_elbrus/mt_elbrus_shrine7_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine7_script.lua", "from_shrine_first_floor_SW_top_door");

    vt_map.MapTransitionEvent.Create("to mountain shrine 1st floor SE room - bottom door", "dat/maps/mt_elbrus/mt_elbrus_shrine7_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine7_script.lua", "from_shrine_first_floor_SW_bottom_door");

    -- Opens the north east passage to the next map.
    vt_map.ScriptedEvent.Create("Open north east passage", "open_ne_passage_start", "open_ne_passage_update");
end

-- zones
local to_shrine_NW_left_door_room_zone = nil
local to_shrine_NW_right_door_room_zone = nil
local to_shrine_SE_top_door_room_zone = nil
local to_shrine_SE_bottom_door_room_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_NW_left_door_room_zone = vt_map.CameraZone.Create(14, 18, 7, 9);
    to_shrine_NW_right_door_room_zone = vt_map.CameraZone.Create(26, 30, 7, 9);
    to_shrine_SE_top_door_room_zone = vt_map.CameraZone.Create(45, 47, 22, 26);
    to_shrine_SE_bottom_door_room_zone = vt_map.CameraZone.Create(45, 47, 32, 36);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_NW_left_door_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine 1st floor NW room - left door");
    elseif (to_shrine_NW_right_door_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine 1st floor NW room - right door");
    elseif (to_shrine_SE_top_door_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.EAST);
        EventManager:StartEvent("to mountain shrine 1st floor SE room - top door");
    elseif (to_shrine_SE_bottom_door_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.EAST);
        EventManager:StartEvent("to mountain shrine 1st floor SE room - bottom door");
    end

    if (rolling_stone2_out == false) then
        if (to_shrine_NW_right_door_room_zone:IsInsideZone(rolling_stone2:GetXPosition(), rolling_stone2:GetYPosition()) == true) then
            GlobalManager:SetEventValue("story", "mt_shrine_1st_floor_stone2_through_2nd_door", 1);
            rolling_stone2_out = true;
            rolling_stone2:SetVisible(false);
            rolling_stone2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end
end

function _CheckStoneAndTriggersCollision()
    -- Check trigger
    if (stone_trigger1:GetState() == false) then
        if (stone_trigger1:IsCollidingWith(rolling_stone1) == true) then
            stone_trigger1:SetState(true)
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
        AudioManager:PlaySound("sounds/stone_bump.ogg");
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

-- The fire pots x position
local ne_passage_fence1_x = 0.0;
local ne_passage_fence2_x = 0.0;

local stone_direction1 = vt_map.MapMode.EAST;
local stone_direction2 = vt_map.MapMode.EAST;

-- Map Custom functions
-- Used through scripted events
map_functions = {
    -- Check whether both triggers are activated and then free the way.
    check_triggers = function()
        if (stone_trigger1:GetState() == true) then
            -- Play a click sound when a trigger is pushed
            AudioManager:PlaySound("sounds/trigger_on.wav");
            -- Free the way
            EventManager:StartEvent("Open north east passage", 1000);
        end
    end,

    -- Opens the south west passage, by moving the fire pots out of the way.
    open_ne_passage_start = function()
        ne_passage_fence1_x = 27.0;
        ne_passage_fence2_x = 29.0;
        AudioManager:PlaySound("sounds/stone_roll.wav");
    end,

    open_ne_passage_update = function()
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.005 * update_time;

        ne_passage_fence1_x = ne_passage_fence1_x - movement_diff;
        fence1_trigger1:SetPosition(ne_passage_fence1_x, 11.0);

        ne_passage_fence2_x = ne_passage_fence2_x + movement_diff;
        fence2_trigger1:SetPosition(ne_passage_fence2_x, 11.0);

        if (fence3_trigger1:GetYPosition() < 14.0) then
            fence3_trigger1:SetYPosition(fence3_trigger1:GetYPosition() + movement_diff)
        end

        if (ne_passage_fence1_x <= 25.0) then
            return true;
        end
        return false;
    end,

    check_diagonal_stone1 = function()
        return _CheckForDiagonals(rolling_stone1);
    end,

    check_diagonal_stone2 = function()
        return _CheckForDiagonals(rolling_stone2);
    end,

    start_to_move_the_stone1 = function()
        stone_direction1 = _GetStoneDirection(rolling_stone1);
        AudioManager:PlaySound("sounds/stone_roll.wav");
    end,

    start_to_move_the_stone2 = function()
        stone_direction2 = _GetStoneDirection(rolling_stone2);
        AudioManager:PlaySound("sounds/stone_roll.wav");
    end,

    move_the_stone_update1 = function()
        return _UpdateStoneMovement(rolling_stone1, stone_direction1)
    end,

    move_the_stone_update2 = function()
        return _UpdateStoneMovement(rolling_stone2, stone_direction2)
    end,
}
