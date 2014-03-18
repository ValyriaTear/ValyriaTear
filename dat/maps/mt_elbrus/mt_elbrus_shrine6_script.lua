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

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);

    -- Preloads the action sounds to avoid glitches
    AudioManager:LoadSound("snd/stone_roll.wav", Map);
    AudioManager:LoadSound("snd/stone_bump.ogg", Map);
    AudioManager:LoadSound("snd/trigger_on.wav", Map);

end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 16, 11);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    Map:AddGroundObject(hero);

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
local fence1_trigger1 = {};
local fence2_trigger1 = {};
local fence1_trigger2 = {};
local fence2_trigger2 = {};
local stone_trigger1 = {};

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}

    _add_flame(9.5, 7);
    _add_flame(33.5, 7);

    object = CreateObject(Map, "Candle Holder1", 43, 20);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Candle Holder1", 43, 30);
    Map:AddGroundObject(object);

    object = CreateObject(Map, "Stone Fence1", 13, 11);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 19, 11);
    Map:AddGroundObject(object);

    object = CreateObject(Map, "Stone Fence1", 39, 12);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 41, 14);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 43, 16);
    Map:AddGroundObject(object);

    object = CreateObject(Map, "Stone Fence1", 34, 13);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 26, 19);
    Map:AddGroundObject(object);

    -- Add flames preventing from using the doors
    -- Top Right door: Unlocked by trigger
    local fence1_trigger1_x_position = 27.0;
    local fence2_trigger1_x_position = 29.0;
    -- Sets the passage open if the enemies were already beaten
    if (GlobalManager:GetEventValue("triggers", "mt elbrus shrine 6 trigger 1") == 1) then
        fence1_trigger1_x_position = 25.0;
        fence2_trigger1_x_position = 31.0;
    end

    fence1_trigger1 = CreateObject(Map, "Stone Fence1", fence1_trigger1_x_position, 11);
    Map:AddGroundObject(fence1_trigger1);
    fence2_trigger1 = CreateObject(Map, "Stone Fence1", fence2_trigger1_x_position, 11);
    Map:AddGroundObject(fence2_trigger1);

    -- Bottom right door: Unlocked by switch
    local fence1_trigger2_y_position = 34.0;
    local fence2_trigger2_y_position = 36.0;
    -- Sets the passage open if the enemies were already beaten
    if (GlobalManager:GetEventValue("triggers", "mt elbrus shrine 8 gate 7 trigger") == 1) then
        fence1_trigger2_y_position = 32.0;
        fence2_trigger2_y_position = 38.0;
    end

    fence1_trigger2 = CreateObject(Map, "Stone Fence1", 43.0, fence1_trigger2_y_position);
    Map:AddGroundObject(fence1_trigger2);
    fence2_trigger2 = CreateObject(Map, "Stone Fence1", 43.0, fence2_trigger2_y_position);
    Map:AddGroundObject(fence2_trigger2);

    -- The two stone trigger will open the gate to the second floor
    stone_trigger1 = vt_map.TriggerObject("mt elbrus shrine 6 trigger 1",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Check triggers");
    stone_trigger1:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger1:SetPosition(26, 17);
    stone_trigger1:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger1);

    event = vt_map.ScriptedEvent("Check triggers", "check_triggers", "")
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

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_shrine.png");
    enemy:AddBattleScript("dat/battles/mountain_shrine_battle_anim.lua");
end

function _CreateEnemies()
    local enemy = {};

    -- Monsters that can only be beaten once
    -- Hint: left, right, top, bottom
    local roam_zone = vt_map.EnemyZone(7, 20, 22, 33);
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
    Map:AddZone(roam_zone);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor NW room - left door", "dat/maps/mt_elbrus/mt_elbrus_shrine5_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine5_script.lua", "from_shrine_first_floor_SW_left_door");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor NW room - right door", "dat/maps/mt_elbrus/mt_elbrus_shrine5_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine5_script.lua", "from_shrine_first_floor_SW_right_door");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor SE room - top door", "dat/maps/mt_elbrus/mt_elbrus_shrine7_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine7_script.lua", "from_shrine_first_floor_SW_top_door");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor SE room - bottom door", "dat/maps/mt_elbrus/mt_elbrus_shrine7_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine7_script.lua", "from_shrine_first_floor_SW_bottom_door");
    EventManager:RegisterEvent(event);

    -- Opens the north east passage to the next map.
    event = vt_map.ScriptedEvent("Open north east passage", "open_ne_passage_start", "open_ne_passage_update");
    EventManager:RegisterEvent(event);

end

-- zones
local to_shrine_NW_left_door_room_zone = {};
local to_shrine_NW_right_door_room_zone = {};
local to_shrine_SE_top_door_room_zone = {};
local to_shrine_SE_bottom_door_room_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_NW_left_door_room_zone = vt_map.CameraZone(14, 18, 7, 9);
    Map:AddZone(to_shrine_NW_left_door_room_zone);
    to_shrine_NW_right_door_room_zone = vt_map.CameraZone(26, 30, 7, 9);
    Map:AddZone(to_shrine_NW_right_door_room_zone);
    to_shrine_SE_top_door_room_zone = vt_map.CameraZone(45, 47, 22, 26);
    Map:AddZone(to_shrine_SE_top_door_room_zone);
    to_shrine_SE_bottom_door_room_zone = vt_map.CameraZone(45, 47, 32, 36);
    Map:AddZone(to_shrine_SE_bottom_door_room_zone);

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

end


-- The fire pots x position
local ne_passage_fence1_x = 0.0;
local ne_passage_fence2_x = 0.0;

-- Map Custom functions
-- Used through scripted events
map_functions = {
    -- Check whether both triggers are activated and then free the way.
    check_triggers = function()
        if (stone_trigger1:GetState() == true) then
            -- Play a click sound when a trigger is pushed
            AudioManager:PlaySound("snd/trigger_on.wav");
            -- Free the way
            EventManager:StartEvent("Open north east passage", 1000);
        end
    end,

    -- Opens the south west passage, by moving the fire pots out of the way.
    open_ne_passage_start = function()
        ne_passage_fence1_x = 27.0;
        ne_passage_fence2_x = 29.0;
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    open_ne_passage_update = function()
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.005 * update_time;

        ne_passage_fence1_x = ne_passage_fence1_x - movement_diff;
        fence1_trigger1:SetPosition(ne_passage_fence1_x, 11.0);

        ne_passage_fence2_x = ne_passage_fence2_x + movement_diff;
        fence2_trigger1:SetPosition(ne_passage_fence2_x, 11.0);

        if (ne_passage_fence1_x <= 25.0) then
            return true;
        end
        return false;
    end,
}
