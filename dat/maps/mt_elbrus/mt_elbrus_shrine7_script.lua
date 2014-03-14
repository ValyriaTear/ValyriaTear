-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine7_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus Shrine"
map_image_filename = "img/menus/locations/mountain_shrine.png"
map_subname = "1st Floor"

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

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay
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
    hero = CreateSprite(Map, "Bronann", 5.5, 24);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    Map:AddGroundObject(hero);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_SW_top_door") then
        hero:SetPosition(5.5, 24);
        hero:SetDirection(vt_map.MapMode.EAST);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_SW_bottom_door") then
        hero:SetPosition(5.5, 34);
        hero:SetDirection(vt_map.MapMode.EAST);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_NE_room") then
        hero:SetPosition(28, 4);
        hero:SetDirection(vt_map.MapMode.SOUTH);
    end
end

local flame1_trigger1 = {};
local flame2_trigger1 = {};

local rolling_stone1 = {};
local rolling_stone2 = {};
local rolling_stone3 = {};

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}

    _add_flame(15.5, 5);
    _add_flame(39.5, 5);

    object = CreateObject(Map, "Candle Holder1", 5, 20);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Candle Holder1", 5, 31);
    Map:AddGroundObject(object);

    object = CreateObject(Map, "Stone Fence1", 11, 10);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 9, 12);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 7, 14);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 5, 16);
    Map:AddGroundObject(object);

    -- Add an invisible object permitting to trigger the same event.
    object = CreateObject(Map, "Stone Fence1", 28, 15);
    object:SetVisible(false);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    object:SetEventWhenTalking("Heroes see the missing stone event");
    Map:AddGroundObject(object);

    -- Bottom right door: Unlocked by switch
    local flame1_trigger1_y_position = 34.0;
    local flame2_trigger1_y_position = 36.0;
    -- Sets the passage open if the enemies were already beaten
    -- FIXME: USE a trigger event instead.
    if (GlobalManager:GetEventValue("story", "mountain_shrine_1st_NE_trigger_pushed") == 1) then
        flame1_trigger1_y_position = 32.0;
        flame2_trigger1_y_position = 38.0;
    end

    flame1_trigger1 = CreateObject(Map, "Flame Pot1", 5.0, flame1_trigger1_y_position);
    flame1_trigger1:RandomizeCurrentAnimationFrame();
    Map:AddGroundObject(flame1_trigger1);
    flame2_trigger1 = CreateObject(Map, "Flame Pot1", 5.0, flame2_trigger1_y_position);
    flame2_trigger1:RandomizeCurrentAnimationFrame();
    Map:AddGroundObject(flame2_trigger1);

    -- The stones used to get through this enigma
    rolling_stone1 = CreateObject(Map, "Rolling Stone", 16, 12);
    Map:AddGroundObject(rolling_stone1);
    rolling_stone1:SetEventWhenTalking("Check hero position for rolling stone 1");
    event = vt_map.IfEvent("Check hero position for rolling stone 1", "check_diagonal_stone1", "Push the rolling stone 1", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 1", "start_to_move_the_stone1", "move_the_stone_update1")
    EventManager:RegisterEvent(event);

    rolling_stone2 = CreateObject(Map, "Rolling Stone", 15, 34);
    Map:AddGroundObject(rolling_stone2);
    rolling_stone2:SetEventWhenTalking("Check hero position for rolling stone 2");
    event = vt_map.IfEvent("Check hero position for rolling stone 2", "check_diagonal_stone2", "Push the rolling stone 2", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 2", "start_to_move_the_stone2", "move_the_stone_update2")
    EventManager:RegisterEvent(event);

    rolling_stone3 = CreateObject(Map, "Rolling Stone", 28, 9);
    Map:AddGroundObject(rolling_stone3);
    rolling_stone3:SetEventWhenTalking("Check hero position for rolling stone 3");
    event = vt_map.IfEvent("Check hero position for rolling stone 3", "check_diagonal_stone3", "Push the rolling stone 3", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 3", "start_to_move_the_stone3", "move_the_stone_update3")
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

    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor SW room - top door", "dat/maps/mt_elbrus/mt_elbrus_shrine6_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine6_script.lua", "from_shrine_first_floor_SE_top_door");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor SW room - bottom door", "dat/maps/mt_elbrus/mt_elbrus_shrine6_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine6_script.lua", "from_shrine_first_floor_SE_bottom_door");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor NE room", "dat/maps/mt_elbrus/mt_elbrus_shrine8_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine8_script.lua", "from_shrine_first_floor_SE_room");
    EventManager:RegisterEvent(event);

    -- The dialogue where the heroes see the missing stones.
    event = vt_map.ScriptedEvent("Heroes see the missing stone event", "missing_stone_event_start", "");
    event:AddEventLinkAtEnd("Set camera on stone", 100);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Set camera on stone", "set_camera_on_stone", "set_camera_update");
    event:AddEventLinkAtEnd("The hero talks about finding a way to get there");
    EventManager:RegisterEvent(event);

    -- Tells about Orlinn's passage
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("There seems to be one of those rolling stones up there. Maybe is there something we missed?");
    dialogue:AddLineEmote(text, hero, "thinking dots");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("The hero talks about finding a way to get there", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Set camera on hero");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Set camera on hero", "set_camera_on_hero", "set_camera_update");
    event:AddEventLinkAtEnd("Heroes see the missing stone event end");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Heroes see the missing stone event end", "missing_stone_event_end", "");
    EventManager:RegisterEvent(event);

end

-- zones
local to_shrine_SW_top_door_room_zone = {};
local to_shrine_SW_bottom_door_room_zone = {};
local to_shrine_NE_room_zone = {};
local see_the_missing_stone_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_SW_top_door_room_zone = vt_map.CameraZone(1, 3, 22, 26);
    Map:AddZone(to_shrine_SW_top_door_room_zone);
    to_shrine_SW_bottom_door_room_zone = vt_map.CameraZone(1, 3, 32, 36);
    Map:AddZone(to_shrine_SW_bottom_door_room_zone);
    to_shrine_NE_room_zone = vt_map.CameraZone(24, 32, 0, 2);
    Map:AddZone(to_shrine_NE_room_zone);

    -- The zone where the characters see the missing stone
    see_the_missing_stone_zone = vt_map.CameraZone(26, 32, 14, 22);
    Map:AddZone(see_the_missing_stone_zone);

end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_SW_top_door_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.WEST);
        EventManager:StartEvent("to mountain shrine 1st floor SW room - top door");
    elseif (to_shrine_SW_bottom_door_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.WEST);
        EventManager:StartEvent("to mountain shrine 1st floor SW room - bottom door");
    elseif (to_shrine_NE_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine 1st floor NE room");
    elseif (see_the_missing_stone_zone:IsCameraEntering() == true
            and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        if (GlobalManager:GetEventValue("story", "mt_elbrus_saw_missing_stone") == 0) then
            hero:SetDirection(vt_map.MapMode.NORTH);
            EventManager:StartEvent("Heroes see the missing stone event");
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


local stone_direction1 = vt_map.MapMode.EAST;
local stone_direction2 = vt_map.MapMode.EAST;
local stone_direction3 = vt_map.MapMode.EAST;

-- Map Custom functions
-- Used through scripted events
map_functions = {

    missing_stone_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:Emote("exclamation", vt_map.MapMode.NORTH)
        hero:SetMoving(false);
    end,

    set_camera_on_stone = function()
        Map:MoveVirtualFocus(rolling_stone3:GetXPosition(), rolling_stone3:GetYPosition());
        Map:SetCamera(ObjectManager.virtual_focus, 800);
    end,

    set_camera_on_hero = function()
        Map:SetCamera(hero, 800);
    end,

    set_camera_update = function()
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        return true;
    end,

    missing_stone_event_end = function()
        Map:PopState();
        GlobalManager:SetEventValue("story", "mt_elbrus_saw_missing_stone", 1);
    end,

    check_diagonal_stone1 = function()
        return _CheckForDiagonals(rolling_stone1);
    end,

    check_diagonal_stone2 = function()
        return _CheckForDiagonals(rolling_stone2);
    end,

    check_diagonal_stone3 = function()
        return _CheckForDiagonals(rolling_stone3);
    end,

    start_to_move_the_stone1 = function()
        stone_direction1 = _GetStoneDirection(rolling_stone1);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    start_to_move_the_stone2 = function()
        stone_direction2 = _GetStoneDirection(rolling_stone2);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    start_to_move_the_stone3 = function()
        stone_direction3 = _GetStoneDirection(rolling_stone3);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    move_the_stone_update1 = function()
        return _UpdateStoneMovement(rolling_stone1, stone_direction1)
    end,

    move_the_stone_update2 = function()
        return _UpdateStoneMovement(rolling_stone2, stone_direction2)
    end,

    move_the_stone_update3 = function()
        return _UpdateStoneMovement(rolling_stone3, stone_direction3)
    end,

}
