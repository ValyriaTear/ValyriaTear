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

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);

    -- Preloads the action sounds to avoid glitches
    AudioManager:LoadSound("snd/stone_roll.wav", Map);
    AudioManager:LoadSound("snd/stone_bump.ogg", Map);
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

    if (GlobalManager:GetPreviousLocation() == "from_shrine_2nd_floor_SE_passage") then
        hero:SetPosition(28, 36);
        hero:SetDirection(vt_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_1st_floor") then
        hero:SetPosition(20, 12);
        hero:SetDirection(vt_map.MapMode.SOUTH);
    end
end

-- Triggers
local stone_trigger1 = {};
local stone_trigger2 = {};
local stone_trigger3 = {};
local stone_trigger4 = {};
local stone_trigger5 = {};
local stone_trigger6 = {};
local stone_trigger7 = {};
local stone_trigger8 = {};

-- stones
local rolling_stone1 = {};
local rolling_stone2 = {};
local rolling_stone3 = {};
local rolling_stone4 = {};
local rolling_stone5 = {};
local rolling_stone6 = {};
local rolling_stone7 = {};
local rolling_stone8 = {};

-- Fences preventing from getting through
local fence1_trigger1 = {};
local fence2_trigger1 = {};

local trap_spikes = {};

-- Object used to trigger Orlinn going up event
local passage_event_object = {};
local passage_back_event_object = {};

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}

    _add_flame(13.5, 7);
    _add_flame(43.5, 6);

    object = CreateObject(Map, "Vase3", 24, 35);
    Map:AddGroundObject(object);

    object = CreateObject(Map, "Candle Holder1", 16, 11);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Candle Holder1", 24, 11);
    Map:AddGroundObject(object);

    object = CreateObject(Map, "Stone Fence1", 43, 26);
    Map:AddGroundObject(object);

    trap_spikes = CreateObject(Map, "Spikes1", 14, 26);
    trap_spikes:SetVisible(false);
    trap_spikes:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(trap_spikes);

    -- The stone triggers that will open the passage
    stone_trigger1 = vt_map.TriggerObject("mt elbrus shrine 2nd NE trigger 1",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Open Gate");
    stone_trigger1:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger1:SetPosition(15, 17);
    stone_trigger1:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger1);

    stone_trigger2 = vt_map.TriggerObject("mt elbrus shrine 2nd NE trigger 2",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Open Gate");
    stone_trigger2:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger2:SetPosition(17, 19);
    stone_trigger2:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger2);

    stone_trigger3 = vt_map.TriggerObject("mt elbrus shrine 2nd NE trigger 3",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Open Gate");
    stone_trigger3:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger3:SetPosition(35, 17);
    stone_trigger3:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger3);

    stone_trigger4 = vt_map.TriggerObject("mt elbrus shrine 2nd NE trigger 4",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Open Gate");
    stone_trigger4:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger4:SetPosition(33, 19);
    stone_trigger4:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger4);

    stone_trigger5 = vt_map.TriggerObject("mt elbrus shrine 2nd NE trigger 5",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Open Gate");
    stone_trigger5:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger5:SetPosition(15, 31);
    stone_trigger5:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger5);

    stone_trigger6 = vt_map.TriggerObject("mt elbrus shrine 2nd NE trigger 6",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Open Gate");
    stone_trigger6:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger6:SetPosition(17, 29);
    stone_trigger6:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger6);

    stone_trigger7 = vt_map.TriggerObject("mt elbrus shrine 2nd NE trigger 7",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Open Gate");
    stone_trigger7:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger7:SetPosition(35, 31);
    stone_trigger7:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger7);

    stone_trigger8 = vt_map.TriggerObject("mt elbrus shrine 2nd NE trigger 8",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Open Gate");
    stone_trigger8:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger8:SetPosition(33, 29);
    stone_trigger8:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger8);

    -- Add blocks preventing from using the doors
    -- Left door: Unlocked by beating monsters
    local fence1_trigger1_x_position = 15.0;
    local fence2_trigger1_x_position = 17.0;
    -- Sets the passage open if the enemies were already beaten
    if (GlobalManager:GetEventValue("story", "mountain_shrine_1st_NW_monsters_defeated") == 1) then
        fence1_trigger1_x_position = 13.0;
        fence2_trigger1_x_position = 19.0;
    end
    fence1_trigger1 = CreateObject(Map, "Stone Fence1", fence1_trigger1_x_position, 38);
    Map:AddGroundObject(fence1_trigger1);
    fence2_trigger1 = CreateObject(Map, "Stone Fence1", fence2_trigger1_x_position, 38);
    Map:AddGroundObject(fence2_trigger1);

    -- bottom passage: Needs all the switches to trigger.
    local fence1_trigger2_x_position = 27.0;
    local fence2_trigger2_x_position = 29.0;
    -- Sets the passage open if the trigger is pushed
    --if (GlobalManager:GetEventValue("triggers", "mt elbrus shrine 2nd NE trigger 1") == 1) then
    --    fence1_trigger2_x_position = 25.0;
    --    fence2_trigger2_x_position = 31.0;
    --end
    fence1_trigger2 = CreateObject(Map, "Stone Fence1", fence1_trigger2_x_position, 38);
    Map:AddGroundObject(fence1_trigger2);
    fence2_trigger2 = CreateObject(Map, "Stone Fence1", fence2_trigger2_x_position, 38);
    Map:AddGroundObject(fence2_trigger2);

    rolling_stone1 = CreateObject(Map, "Rolling Stone", 15, 19);
    rolling_stone1:SetEventWhenTalking("Check hero position for rolling stone 1");
    Map:AddGroundObject(rolling_stone1);
    event = vt_map.IfEvent("Check hero position for rolling stone 1", "check_diagonal_stone1", "Push the rolling stone 1", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 1", "start_to_move_the_stone1", "move_the_stone_update1")
    EventManager:RegisterEvent(event);

    rolling_stone2 = CreateObject(Map, "Rolling Stone", 17, 21);
    rolling_stone2:SetEventWhenTalking("Check hero position for rolling stone 2");
    Map:AddGroundObject(rolling_stone2);
    event = vt_map.IfEvent("Check hero position for rolling stone 2", "check_diagonal_stone2", "Push the rolling stone 2", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 2", "start_to_move_the_stone2", "move_the_stone_update2")
    EventManager:RegisterEvent(event);

    rolling_stone3 = CreateObject(Map, "Rolling Stone", 19, 29);
    rolling_stone3:SetEventWhenTalking("Check hero position for rolling stone 3");
    Map:AddGroundObject(rolling_stone3);
    event = vt_map.IfEvent("Check hero position for rolling stone 3", "check_diagonal_stone3", "Push the rolling stone 3", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 3", "start_to_move_the_stone3", "move_the_stone_update3")
    EventManager:RegisterEvent(event);

    rolling_stone4 = CreateObject(Map, "Rolling Stone", 17, 31);
    rolling_stone4:SetEventWhenTalking("Check hero position for rolling stone 4");
    Map:AddGroundObject(rolling_stone4);
    event = vt_map.IfEvent("Check hero position for rolling stone 4", "check_diagonal_stone4", "Push the rolling stone 4", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 4", "start_to_move_the_stone4", "move_the_stone_update4")
    EventManager:RegisterEvent(event);

    rolling_stone5 = CreateObject(Map, "Rolling Stone", 33, 27);
    rolling_stone5:SetEventWhenTalking("Check hero position for rolling stone 5");
    Map:AddGroundObject(rolling_stone5);
    event = vt_map.IfEvent("Check hero position for rolling stone 5", "check_diagonal_stone5", "Push the rolling stone 5", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 5", "start_to_move_the_stone5", "move_the_stone_update5")
    EventManager:RegisterEvent(event);

    rolling_stone6 = CreateObject(Map, "Rolling Stone", 35, 29);
    rolling_stone6:SetEventWhenTalking("Check hero position for rolling stone 6");
    Map:AddGroundObject(rolling_stone6);
    event = vt_map.IfEvent("Check hero position for rolling stone 6", "check_diagonal_stone6", "Push the rolling stone 6", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 6", "start_to_move_the_stone6", "move_the_stone_update6")
    EventManager:RegisterEvent(event);

    rolling_stone7 = CreateObject(Map, "Rolling Stone", 31, 19);
    rolling_stone7:SetEventWhenTalking("Check hero position for rolling stone 7");
    Map:AddGroundObject(rolling_stone7);
    event = vt_map.IfEvent("Check hero position for rolling stone 7", "check_diagonal_stone7", "Push the rolling stone 7", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 7", "start_to_move_the_stone7", "move_the_stone_update7")
    EventManager:RegisterEvent(event);

    rolling_stone8 = CreateObject(Map, "Rolling Stone", 33, 17);
    rolling_stone8:SetEventWhenTalking("Check hero position for rolling stone 8");
    Map:AddGroundObject(rolling_stone8);
    event = vt_map.IfEvent("Check hero position for rolling stone 8", "check_diagonal_stone8", "Push the rolling stone 8", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 8", "start_to_move_the_stone8", "move_the_stone_update8")
    EventManager:RegisterEvent(event);
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


-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor", "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_script.lua", "from_shrine_2nd_floor");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine 2nd floor SE room", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_SE_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_SE_script.lua", "from_shrine_2nd_floor_NE_room");
    EventManager:RegisterEvent(event);

    -- Generic events
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks south", orlinn, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks west", orlinn, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Kalya looks at Orlinn", kalya, orlinn);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Orlinn looks at Kalya", orlinn, kalya);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Orlinn looks at Bronann", orlinn, hero);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Bronann looks at Orlinn", hero, orlinn);
    EventManager:RegisterEvent(event);


end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_shrine.png");
    enemy:AddBattleScript("dat/battles/mountain_shrine_battle_anim.lua");
end

-- Enemies to defeat before opening the south-west passage
local roam_zone = nil;
local monsters_defeated = false;

function _CreateEnemies()
    local enemy = {};

    -- Monsters that can only be beaten once
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(13, 20, 26, 36);
    if (monsters_defeated == false) then
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
        roam_zone:AddEnemy(enemy, Map, 10);
    end
    Map:AddZone(roam_zone);
end

-- zones
local to_shrine_1st_floor_room_zone = {};
local to_shrine_SE_room_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_1st_floor_room_zone = vt_map.CameraZone(18, 22, 9, 11);
    Map:AddZone(to_shrine_1st_floor_room_zone);

    to_shrine_SE_room_zone = vt_map.CameraZone(24, 32, 38, 42);
    Map:AddZone(to_shrine_SE_room_zone);
end

local trap_triggered = false;

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_1st_floor_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine 1st floor");
    elseif (to_shrine_SE_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.EAST);
        EventManager:StartEvent("to mountain shrine 2nd floor SE room");
    end
end

function _CheckStoneAndTriggersCollision()
    -- Check trigger
    if (stone_trigger2:GetState() == false) then
        if (stone_trigger2:IsCollidingWith(rolling_stone2) == true) then
            stone_trigger2:SetState(true)
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

-- Stones directions
local stone_direction1 = vt_map.MapMode.EAST;
local stone_direction2 = vt_map.MapMode.EAST;
local stone_direction3 = vt_map.MapMode.EAST;
local stone_direction4 = vt_map.MapMode.EAST;
local stone_direction5 = vt_map.MapMode.EAST;
local stone_direction6 = vt_map.MapMode.EAST;
local stone_direction7 = vt_map.MapMode.EAST;
local stone_direction8 = vt_map.MapMode.EAST;

-- Map Custom functions
-- Used through scripted events
map_functions = {

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

    check_diagonal_stone2 = function()
        return _CheckForDiagonals(rolling_stone2);
    end,
    start_to_move_the_stone2 = function()
        stone_direction2 = _GetStoneDirection(rolling_stone2);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,
    move_the_stone_update2 = function()
        return _UpdateStoneMovement(rolling_stone2, stone_direction2)
    end,

    check_diagonal_stone3 = function()
        return _CheckForDiagonals(rolling_stone3);
    end,
    start_to_move_the_stone3 = function()
        stone_direction3 = _GetStoneDirection(rolling_stone3);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,
    move_the_stone_update3 = function()
        return _UpdateStoneMovement(rolling_stone3, stone_direction3)
    end,

    check_diagonal_stone4 = function()
        return _CheckForDiagonals(rolling_stone4);
    end,
    start_to_move_the_stone3 = function()
        stone_direction4 = _GetStoneDirection(rolling_stone4);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,
    move_the_stone_update4 = function()
        return _UpdateStoneMovement(rolling_stone4, stone_direction4)
    end,

    check_diagonal_stone5 = function()
        return _CheckForDiagonals(rolling_stone5);
    end,
    start_to_move_the_stone5 = function()
        stone_direction5 = _GetStoneDirection(rolling_stone5);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,
    move_the_stone_update5 = function()
        return _UpdateStoneMovement(rolling_stone5, stone_direction5)
    end,

    check_diagonal_stone6 = function()
        return _CheckForDiagonals(rolling_stone6);
    end,
    start_to_move_the_stone6 = function()
        stone_direction6 = _GetStoneDirection(rolling_stone6);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,
    move_the_stone_update6 = function()
        return _UpdateStoneMovement(rolling_stone6, stone_direction6)
    end,

    check_diagonal_stone7 = function()
        return _CheckForDiagonals(rolling_stone7);
    end,
    start_to_move_the_stone7 = function()
        stone_direction7 = _GetStoneDirection(rolling_stone7);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,
    move_the_stone_update7 = function()
        return _UpdateStoneMovement(rolling_stone7, stone_direction7)
    end,

    check_diagonal_stone8 = function()
        return _CheckForDiagonals(rolling_stone8);
    end,
    start_to_move_the_stone8 = function()
        stone_direction8 = _GetStoneDirection(rolling_stone8);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,
    move_the_stone_update8 = function()
        return _UpdateStoneMovement(rolling_stone8, stone_direction8)
    end,
}
