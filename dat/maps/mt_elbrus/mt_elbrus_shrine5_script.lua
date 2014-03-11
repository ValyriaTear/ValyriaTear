-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine5_script = ns;
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

-- Forest dialogue secondary hero
local orlinn = {};

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

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);

    -- Preloads the action sounds to avoid glitches
    AudioManager:LoadSound("snd/opening_sword_unsheathe.wav", Map);
    AudioManager:LoadSound("snd/stone_roll.wav", Map);
    AudioManager:LoadSound("snd/stone_bump.ogg", Map);
    AudioManager:LoadSound("snd/trigger_on.wav", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
    -- Check wether the monsters have been defeated
    _CheckMonstersStates();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from shrine main room)
    hero = CreateSprite(Map, "Bronann", 10.0, 12.5);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    Map:AddGroundObject(hero);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_SW_left_door") then
        hero:SetPosition(16, 36);
        hero:SetDirection(vt_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_SW_right_door") then
        hero:SetPosition(28, 36);
        hero:SetDirection(vt_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_NE_room") then
        hero:SetPosition(44, 10);
        hero:SetDirection(vt_map.MapMode.WEST);
    end

    orlinn = CreateSprite(Map, "Orlinn",
                          hero:GetXPosition(), hero:GetYPosition());
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);
    Map:AddGroundObject(orlinn);
end

-- Triggers
local stone_trigger1 = {};
local stone_trigger2 = {};

-- Flames preventing from getting through
local flame1_trigger1 = {};
local flame2_trigger1 = {};
local flame1_trigger2 = {};
local flame2_trigger2 = {};

-- The grid preventing from going to the second floor.
local second_floor_gate = {};

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}

    _add_flame(13.5, 7);
    _add_flame(43.5, 6);

    object = CreateObject(Map, "Candle Holder1", 16, 11);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Candle Holder1", 24, 11);
    Map:AddGroundObject(object);
    
    -- TODO: Add stone fence instead when ready.
    object = CreateObject(Map, "Box1", 43, 26);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Box1", 41, 28);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Box1", 39, 30);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Box1", 37, 32);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Box1", 37, 34);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Box1", 37, 36);
    Map:AddGroundObject(object);

    -- Add an invisible object permitting to trigger the high passage dialogue
    object = CreateObject(Map, "Box1", 33, 17);
    object:SetVisible(false);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    object:SetEventWhenTalking("The hero wonders about the high passage");
    Map:AddGroundObject(object);

    -- The two stone trigger will open the gate to the second floor
    stone_trigger1 = vt_map.TriggerObject("mt elbrus shrine 5 trigger 1",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Check triggers");
    stone_trigger1:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger1:SetPosition(29, 18);
    stone_trigger1:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger1);

    stone_trigger2 = vt_map.TriggerObject("mt elbrus shrine 5 trigger 2",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Check triggers");
    stone_trigger2:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger2:SetPosition(43, 20);
    stone_trigger2:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger2);

    event = vt_map.ScriptedEvent("Check triggers", "check_triggers", "")
    EventManager:RegisterEvent(event);

    -- Add flames preventing from using the doors
    -- Left door: Unlocked by beating monsters
    local flame1_trigger1_x_position = 15.0;
    local flame2_trigger1_x_position = 17.0;
    -- Sets the passage open if the enemies were already beaten
    if (GlobalManager:GetEventValue("story", "mountain_shrine_1st_NW_monsters_defeated") == 1) then
        flame1_trigger1_x_position = 13.0;
        flame2_trigger1_x_position = 19.0;
    end
    flame1_trigger1 = CreateObject(Map, "Flame Pot1", flame1_trigger1_x_position, 38);
    flame1_trigger1:RandomizeCurrentAnimationFrame();
    Map:AddGroundObject(flame1_trigger1);
    flame2_trigger1 = CreateObject(Map, "Flame Pot1", flame2_trigger1_x_position, 38);
    flame2_trigger1:RandomizeCurrentAnimationFrame();
    Map:AddGroundObject(flame2_trigger1);

    -- Right door: Using a switch
    flame1_trigger2 = CreateObject(Map, "Flame Pot1", 27, 38);
    flame1_trigger2:RandomizeCurrentAnimationFrame();
    Map:AddGroundObject(flame1_trigger2);
    flame2_trigger2 = CreateObject(Map, "Flame Pot1", 29, 38);
    flame2_trigger2:RandomizeCurrentAnimationFrame();
    Map:AddGroundObject(flame2_trigger2);

    second_floor_gate = CreateObject(Map, "Gate1 closed", 20, 10);
    Map:AddGroundObject(second_floor_gate);

    event = vt_map.ScriptedEvent("Open Gate", "open_gate_animated_start", "open_gate_animated_update")
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

    event = vt_map.MapTransitionEvent("to mountain shrine main room", "dat/maps/mt_elbrus/mt_elbrus_shrine2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine2_script.lua", "from_shrine_first_floor");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine 2nd floor", "dat/maps/mt_elbrus/mt_elbrus_shrine9_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine9_script.lua", "from_shrine_first_floor");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor SW room - left door", "dat/maps/mt_elbrus/mt_elbrus_shrine6_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine6_script.lua", "from_shrine_first_floor_NW_left_door");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor SW room - right door", "dat/maps/mt_elbrus/mt_elbrus_shrine6_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine6_script.lua", "from_shrine_first_floor_NW_right_door");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor NE room", "dat/maps/mt_elbrus/mt_elbrus_shrine8_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine8_script.lua", "from_shrine_first_floor_NW_room");
    EventManager:RegisterEvent(event);

    -- Opens the left passage to the next map.
    event = vt_map.ScriptedEvent("Open south west passage", "open_sw_passage_start", "open_sw_passage_update");
    EventManager:RegisterEvent(event);

    -- Tells about Orlinn's passage
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("There is a passage here but I'm too heavy for those stones jutting out. Maybe later...");
    dialogue:AddLine(text, hero);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("The hero wonders about the high passage", dialogue);
    event:SetStopCameraMovement(true);
    EventManager:RegisterEvent(event);

end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_shrine.png");
    enemy:AddBattleScript("dat/battles/mountain_shrine_battle_anim.lua");
end

-- Enemies to defeat before opening the south-west passage
local roam_zone = {};
local monsters_defeated = false;

function _CreateEnemies()
    local enemy = {};

    -- Checks whether the enemies there have been already defeated...
    if (GlobalManager:GetEventValue("story", "mountain_shrine_1st_NW_monsters_defeated") == 1) then
        monsters_defeated = true;
        return;
    end

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
        roam_zone:AddEnemy(enemy, Map, 1);
        roam_zone:SetSpawnsLeft(1); -- These monsters shall spawn only one time.
    end
    Map:AddZone(roam_zone);
end

-- check whether all the monsters dies, to open the door
function _CheckMonstersStates()
    if (monsters_defeated == true) then
        return
    end

    if (roam_zone:GetSpawnsLeft() > 0) then
        return
    end
    
    -- Open the south left passage
    monsters_defeated = true;
    hero:SetMoving(false);
    EventManager:StartEvent("Open south west passage", 1000);

    GlobalManager:SetEventValue("story", "mountain_shrine_1st_NW_monsters_defeated", 1);
end

-- zones
local to_shrine_main_room_zone = {};
local to_shrine_2nd_floor_room_zone = {};
local to_shrine_SW_left_door_room_zone = {};
local to_shrine_SW_right_door_room_zone = {};
local to_shrine_NE_room_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_main_room_zone = vt_map.CameraZone(6, 10, 9, 11);
    Map:AddZone(to_shrine_main_room_zone);

    to_shrine_2nd_floor_room_zone = vt_map.CameraZone(18, 22, 9, 10);
    Map:AddZone(to_shrine_2nd_floor_room_zone);

    to_shrine_SW_left_door_room_zone = vt_map.CameraZone(14, 18, 38, 40);
    Map:AddZone(to_shrine_SW_left_door_room_zone);

    to_shrine_SW_right_door_room_zone = vt_map.CameraZone(26, 30, 38, 40);
    Map:AddZone(to_shrine_SW_right_door_room_zone);

    to_shrine_NE_room_zone = vt_map.CameraZone(46, 48, 8, 12);
    Map:AddZone(to_shrine_NE_room_zone);

end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_main_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine main room");
    elseif (to_shrine_2nd_floor_room_zone:IsCameraEntering() == true) then
        --hero:SetDirection(vt_map.MapMode.NORTH);
        --EventManager:StartEvent("to mountain shrine 2nd floor");
    elseif (to_shrine_SW_left_door_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine 1st floor SW room - left door");
    elseif (to_shrine_SW_right_door_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine 1st floor SW room - right door");
    elseif (to_shrine_NE_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.EAST);
        EventManager:StartEvent("to mountain shrine 1st floor NE room");
    end

end

-- The north east gate y position
local gate_y_position = 10.0;

-- The fire pots x position
local sw_passage_pot1_x = 0.0;
local sw_passage_pot2_x = 0.0;

-- Map Custom functions
-- Used through scripted events
map_functions = {
    -- Check whether both triggers are activated and then free the way.
    check_triggers = function()
        if (stone_trigger1:GetState() == true
                and stone_trigger2:GetState() == true) then
            -- Free the way
            EventManager:StartEvent("Open Gate", 100);
        else
            -- Play a click sound when a trigger is pushed
            AudioManager:PlaySound("snd/trigger_on.wav");
        end
    end,

    -- A function making the gate slide up with a noise and removing its collision
    open_gate_animated_start = function()
        gate_y_position = 10.0;
        second_floor_gate:SetPosition(20.0, 10.0);
        second_floor_gate:SetDrawOnSecondPass(false);
        second_floor_gate:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        -- Opening gate sound
        AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
    end,

    open_gate_animated_update = function()
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.015 * update_time;
        gate_y_position = gate_y_position - movement_diff;
        second_floor_gate:SetPosition(20.0, gate_y_position);

        if (gate_y_position <= 7.0) then
            map_functions.set_gate_opened();
            return true;
        end
        return false;
        
    end,

    -- Set the gate directly to the open state
    set_gate_opened = function()
        second_floor_gate:SetPosition(20.0, 7.0);
        second_floor_gate:SetDrawOnSecondPass(true);
        second_floor_gate:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end,

    -- Opens the south west passage, by moving the fire pots out of the way.
    open_sw_passage_start = function()
        sw_passage_pot1_x = 15.0;
        sw_passage_pot2_x = 17.0;
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    open_sw_passage_update = function()
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.005 * update_time;

        sw_passage_pot1_x = sw_passage_pot1_x - movement_diff;
        flame1_trigger1:SetPosition(sw_passage_pot1_x, 38.0);

        sw_passage_pot2_x = sw_passage_pot2_x + movement_diff;
        flame2_trigger1:SetPosition(sw_passage_pot2_x, 38.0);

        if (sw_passage_pot1_x <= 13.0) then
            return true;
        end
        return false;
    end,
}
