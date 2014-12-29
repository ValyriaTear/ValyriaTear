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
local Map = nil
local DialogueManager = nil
local EventManager = nil
local Script = nil

-- the main character handler
local hero = nil
local orlinn = nil

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
    hero = CreateSprite(Map, "Bronann", 5.5, 24, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map:SetPartyMemberVisibleSprite(hero);

    orlinn = CreateSprite(Map, "Orlinn", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.FAST_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_SW_top_door") then
        hero:SetPosition(5.5, 24);
        hero:SetDirection(vt_map.MapMode.EAST);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_SW_bottom_door") then
        hero:SetPosition(5.5, 34);
        hero:SetDirection(vt_map.MapMode.EAST);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_NE_room") then
        -- In that case, Orlinn is back from the top-right passage,
        -- and the player is incarnating him.
        orlinn:SetPosition(28, 4);
        orlinn:SetVisible(true);
        orlinn:SetDirection(vt_map.MapMode.SOUTH);
        orlinn:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        Map:SetCamera(orlinn);

        -- Hide the hero sprite for now.
        hero:SetPosition(0, 0);
        hero:SetVisible(false);

        -- The menu and status effects are disabled now.
        Map:SetMenuEnabled(false);
        Map:SetStatusEffectsEnabled(false);
    end
end

local fence1_trigger1 = nil
local fence2_trigger1 = nil

local rolling_stone1 = nil
local rolling_stone2 = nil

-- tells whether the stones already went throught the door.
local rolling_stone1_out = false;
local rolling_stone2_out = false;

function _CreateObjects()
    local object = nil
    local npc = nil
    local dialogue = nil
    local text = nil
    local event = nil

    object = CreateTreasure(Map, "mt_shrine7_chest1", "Wood_Chest3", 16, 13, vt_map.MapMode.GROUND_OBJECT);
    object:AddObject(3002, 1); -- Iron Ore
    object:AddEvent("Monster trap in chest");

    event = vt_map.BattleEncounterEvent("Monster trap in chest");
    event:SetMusic("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    event:SetBackground("img/backdrops/battle/mountain_shrine.png");
    event:AddScript("dat/battles/mountain_shrine_battle_anim.lua");
    event:AddEnemy(16); -- Rat
    event:AddEnemy(16);
    event:AddEnemy(16);
    event:AddEnemy(16);
    event:AddEnemy(16);
    event:AddEnemy(16);
    event:AddEnemy(16);
    event:AddEnemy(16);
    event:AddEnemy(16);
    event:AddEnemy(16);
    event:AddEnemy(16);
    event:AddEnemy(16);
    EventManager:RegisterEvent(event);

    _add_flame(15.5, 5);
    _add_flame(39.5, 5);

    CreateObject(Map, "Vase4", 9, 15, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Jar1", 18, 10, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Jar1", 43, 22, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Candle Holder1", 5, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Candle Holder1", 5, 30, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 11, 10, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 9, 12, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 7, 14, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 5, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 43, 19, vt_map.MapMode.GROUND_OBJECT);

    if (GlobalManager:GetEventValue("story", "mountain_shrine_1stfloor_orlinn_pushed_stone") == 0) then
        -- Add an invisible object permitting to trigger the event when the stone is still in its first place.
        object = CreateObject(Map, "Stone Fence1", 28, 15, vt_map.MapMode.GROUND_OBJECT);
        object:SetVisible(false);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        object:SetEventWhenTalking("Heroes see the missing stone event");
    end

    -- Bottom right door: Unlocked by switch
    local fence1_trigger1_y_position = 34.0;
    local fence2_trigger1_y_position = 36.0;
    -- Sets the passage open if the enemies were already beaten
    if (GlobalManager:GetEventValue("triggers", "mt elbrus shrine 8 gate 7 trigger") == 1) then
        fence1_trigger1_y_position = 31.8;
        fence2_trigger1_y_position = 38.0;
    end

    fence1_trigger1 = CreateObject(Map, "Stone Fence1", 5.0, fence1_trigger1_y_position, vt_map.MapMode.GROUND_OBJECT);
    fence2_trigger1 = CreateObject(Map, "Stone Fence1", 5.0, fence2_trigger1_y_position, vt_map.MapMode.GROUND_OBJECT);

    -- Adds the spikes preventing from getting the rolling stone if the SW trigger wasn't pushed
    if (GlobalManager:GetEventValue("triggers", "mt elbrus shrine 6 trigger 1") == 0) then
        CreateObject(Map, "Spikes1", 25, 7, vt_map.MapMode.GROUND_OBJECT);
        CreateObject(Map, "Spikes1", 27, 7, vt_map.MapMode.GROUND_OBJECT);
        CreateObject(Map, "Spikes1", 29, 7, vt_map.MapMode.GROUND_OBJECT);
        CreateObject(Map, "Spikes1", 31, 7, vt_map.MapMode.GROUND_OBJECT);
    end

    -- The stones used to get through this enigma
    rolling_stone1 = CreateObject(Map, "Rolling Stone", 15, 34, vt_map.MapMode.GROUND_OBJECT);
    event = vt_map.IfEvent("Check hero position for rolling stone 1", "check_diagonal_stone1", "Push the rolling stone 1", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 1", "start_to_move_the_stone1", "move_the_stone_update1")
    EventManager:RegisterEvent(event);

    -- Check whether we put the stone 1 through the door
    if (GlobalManager:GetEventValue("story", "mt_shrine_1st_floor_stone1_through_1st_door") == 1) then
        rolling_stone1_out = true;
        rolling_stone1:SetVisible(false);
        rolling_stone1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    else
        rolling_stone1:SetEventWhenTalking("Check hero position for rolling stone 1");
    end

    rolling_stone2 = CreateObject(Map, "Rolling Stone", 28, 9, vt_map.MapMode.GROUND_OBJECT);
    if (GlobalManager:GetEventValue("story", "mt_shrine_1st_floor_stone2_through_1st_door") == 1) then
        -- Check whether we put the stone 2 through the door
        rolling_stone2_out = true;
        rolling_stone2:SetVisible(false);
        rolling_stone2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    elseif (GlobalManager:GetEventValue("story", "mountain_shrine_1stfloor_orlinn_pushed_stone") == 0
            and GlobalManager:GetEventValue("triggers", "mt elbrus shrine 6 trigger 1") == 1) then
        -- If the spikes are removed, and the stone untouched, Orlinn can pushed downstairs...
        rolling_stone2:SetEventWhenTalking("Make rolling stone2 fall event start");
    elseif (GlobalManager:GetEventValue("story", "mountain_shrine_1stfloor_orlinn_pushed_stone") == 1) then
        -- The stone is downstairs
        rolling_stone2:SetPosition(28, 18);
        rolling_stone2:SetEventWhenTalking("Check hero position for rolling stone 2");
    end

    -- Makes the stone fall
    event = vt_map.ScriptedEvent("Make rolling stone2 fall event start", "stone_falls_event_start", "stone_falls_event_update");
    EventManager:RegisterEvent(event);

    -- Push the stone
    event = vt_map.IfEvent("Check hero position for rolling stone 2", "check_diagonal_stone2", "Push the rolling stone 2", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 2", "start_to_move_the_stone2", "move_the_stone_update2")
    EventManager:RegisterEvent(event);

    -- Add parchment
    object = CreateObject(Map, "Parchment", 9.0, 18.6, vt_map.MapMode.GROUND_OBJECT);
    object:SetEventWhenTalking("Parchment event");

    -- The second parchment note
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("There is a note left here.");
    dialogue:AddLineEmote(text, hero, "thinking dots");
    text = vt_system.Translate("It says: 'I fathomed it! After all, not every ladder is made of wood. Now, I'll have to get back there alive...'");
    dialogue:AddLine(text, hero);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Parchment event", dialogue);
    EventManager:RegisterEvent(event);
end

function _add_flame(x, y)
    vt_map.SoundObject.Create("snd/campfire.ogg", x, y, 10.0);

    local object = CreateObject(Map, "Flame1", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();

    vt_map.Halo.Create("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    vt_map.Halo.Create("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

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
    text = vt_system.Translate("There seems to be one of those rolling stones up here ...");
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
local to_shrine_SW_top_door_room_zone = nil
local to_shrine_SW_bottom_door_room_zone = nil
local to_shrine_NE_room_zone = nil
local see_the_missing_stone_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_SW_top_door_room_zone = vt_map.CameraZone(1, 3, 22, 26);
    Map:AddZone(to_shrine_SW_top_door_room_zone);
    to_shrine_SW_bottom_door_room_zone = vt_map.CameraZone(1, 3, 32, 36);
    Map:AddZone(to_shrine_SW_bottom_door_room_zone);
    to_shrine_NE_room_zone = vt_map.CameraZone(24, 32, 0, 2);
    Map:AddZone(to_shrine_NE_room_zone);
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
    end

    -- Check whether we pushed one stone through the door
    if (rolling_stone1_out == false) then
        if (to_shrine_SW_bottom_door_room_zone:IsInsideZone(rolling_stone1:GetXPosition(), rolling_stone1:GetYPosition()) == true) then
            GlobalManager:SetEventValue("story", "mt_shrine_1st_floor_stone1_through_1st_door", 1);
            rolling_stone1_out = true;
            rolling_stone1:SetVisible(false);
            rolling_stone1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end
    if (rolling_stone2_out == false) then
        if (to_shrine_SW_bottom_door_room_zone:IsInsideZone(rolling_stone2:GetXPosition(), rolling_stone2:GetYPosition()) == true) then
            GlobalManager:SetEventValue("story", "mt_shrine_1st_floor_stone2_through_1st_door", 1);
            rolling_stone2_out = true;
            rolling_stone2:SetVisible(false);
            rolling_stone2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
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

local stone_fall_y_pos = 9;
local stone_fall_hit_ground = false;

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
        Map:MoveVirtualFocus(rolling_stone2:GetXPosition(), rolling_stone2:GetYPosition());
        Map:SetCamera(Map:GetVirtualFocus(), 800);
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

    stone_falls_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        stone_fall_y_pos = 9;
        stone_fall_hit_ground = false;
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    stone_falls_event_update = function()
        local update_time = SystemManager:GetUpdateTime();
        -- change the movement speed according to whether the stone is rolling
        -- or falling
        local diff_speed = 0.007;
        if (stone_fall_y_pos > 10.0 and stone_fall_y_pos < 15.0) then
            diff_speed = 0.018;
        end
        -- Play a sound when it is hitting the ground
        if (stone_fall_hit_ground == false and stone_fall_y_pos >= 15.0) then
            stone_fall_hit_ground = true;
            AudioManager:PlaySound("snd/stone_bump.ogg");
        end
        local movement_diff = diff_speed * update_time;
        stone_fall_y_pos = stone_fall_y_pos + movement_diff;
        rolling_stone2:SetYPosition(stone_fall_y_pos);
        if (stone_fall_y_pos >= 18.0) then
            GlobalManager:SetEventValue("story", "mountain_shrine_1stfloor_orlinn_pushed_stone", 1);
            Map:PopState();
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
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    start_to_move_the_stone2 = function()
        stone_direction2 = _GetStoneDirection(rolling_stone2);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    move_the_stone_update1 = function()
        return _UpdateStoneMovement(rolling_stone1, stone_direction1)
    end,

    move_the_stone_update2 = function()
        return _UpdateStoneMovement(rolling_stone2, stone_direction2)
    end,
}
