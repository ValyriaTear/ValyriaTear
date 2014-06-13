-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine_3rd_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus Shrine"
map_image_filename = "img/menus/locations/mountain_shrine.png"
map_subname = ""

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
    _CreateEnemies()

    -- Set the camera focus on hero
    Map:SetCamera(orlinn);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    -- The player incarnates Orlinn in this map.

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
end

-- The boss!
local andromalius = nil;

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    orlinn = CreateSprite(Map, "Orlinn", 32.0, 45.0);
    orlinn:SetDirection(vt_map.MapMode.NORTH);
    orlinn:SetMovementSpeed(vt_map.MapMode.FAST_SPEED);
    Map:AddGroundObject(orlinn);

    -- The menu is disabled in this map
    Map:SetMenuEnabled(false);

    andromalius = CreateSprite(Map, "Andromalius", 32.0, 25.0);
    andromalius:SetDirection(vt_map.MapMode.SOUTH);
    andromalius:SetMovementSpeed(vt_map.MapMode.FAST_SPEED);
    Map:AddGroundObject(andromalius);
end

-- Arrays of spikes objects
local spikes1 = {};
local spikes2 = {};
local spikes3 = {};
local spikes4 = {};

-- The fences preventing from triggering the waterfalls
local upper_fence1 = nil;
local upper_fence2 = nil;
local lower_fence1 = nil;
local lower_fence2 = nil;

local stone1 = nil;
local stone2 = nil;
local stone3 = nil;

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}

    _add_flame(39.5, 7);
    _add_flame(25.5, 7);

    object = CreateObject(Map, "Stone Fence1", 31, 38);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 35, 36);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 53, 26);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 51, 22);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 13, 28);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Stone Fence1", 11, 24);
    Map:AddGroundObject(object);

    -- Create the mobile fences
    upper_fence1 = CreateObject(Map, "Stone Fence1", 31, 16);
    Map:AddGroundObject(upper_fence1);
    upper_fence1 = CreateObject(Map, "Stone Fence1", 33, 16);
    Map:AddGroundObject(upper_fence1);

    lower_fence1 = CreateObject(Map, "Stone Fence1", 29, 46);
    Map:AddGroundObject(lower_fence1);
    lower_fence2 = CreateObject(Map, "Stone Fence1", 35, 46);
    Map:AddGroundObject(lower_fence2);

    -- Create the spikes
    -- Inner circle
    local spike_objects1 = {
        { 29, 20 },
        { 27, 22 },
        { 25, 24 },
        { 25, 26 },
        { 27, 28 },
        { 29, 30 },
        { 31, 32 },
        { 33, 32 },
        { 35, 30 },
        { 37, 28 },
        { 39, 26 },
        { 39, 24 },
        { 37, 22 },
        { 35, 20 },
    }

    -- Loads the spikes according to the array
    for my_index, my_array in pairs(spike_objects1) do
        spikes1[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(spikes1[my_index]);
        spikes1[my_index]:SetVisible(false);
        spikes1[my_index]:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    -- Outer circle
    local spike_objects2 = {
        -- left
        { 27, 16 },
        { 27, 18 },
        { 25, 18 },
        { 23, 20 },
        { 21, 22 },
        { 21, 24 },
        { 21, 26 },
        { 21, 28 },
        { 23, 30 },
        { 25, 32 },
        { 27, 34 },
        -- right
        { 37, 34 },
        { 39, 32 },
        { 41, 30 },
        { 43, 28 },
        { 43, 26 },
        { 43, 24 },
        { 43, 22 },
        { 41, 20 },
        { 39, 18 },
        { 37, 18 },
        { 37, 16 },
    }

    -- Loads the spikes according to the array
    for my_index, my_array in pairs(spike_objects2) do
        spikes2[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(spikes2[my_index]);
        spikes2[my_index]:SetVisible(false);
        spikes2[my_index]:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    -- Inner lines
    local spike_objects3 = {
        -- left
        { 13, 16 },
        { 13, 18 },
        { 13, 20 },
        { 13, 22 },
        { 13, 24 },
        { 13, 26 },
        --{ 13, 28 },
        { 13, 30 },
        { 13, 32 },
        { 13, 34 },
        { 13, 36 },
        -- bottom
        { 15, 36 },
        { 17, 36 },
        { 19, 36 },
        { 21, 36 },
        { 23, 36 },
        { 25, 36 },
        { 27, 36 },
        { 29, 36 },
        { 31, 36 },
        { 33, 36 },
        --{ 35, 36 },
        { 37, 36 },
        { 39, 36 },
        { 41, 36 },
        { 43, 36 },
        { 45, 36 },
        { 47, 36 },
        { 49, 36 },
        { 51, 36 },
        --right
        { 51, 34 },
        { 51, 32 },
        { 51, 30 },
        { 51, 28 },
        { 51, 26 },
        { 51, 24 },
        --{ 51, 22 },
        { 51, 20 },
        { 51, 18 },
        { 51, 16 },
    }

    -- Loads the spikes according to the array
    for my_index, my_array in pairs(spike_objects3) do
        spikes3[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(spikes3[my_index]);
        spikes3[my_index]:SetVisible(false);
        spikes3[my_index]:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    -- Outer lines
    local spike_objects4 = {
        -- right
        { 53, 14 },
        { 53, 16 },
        { 53, 18 },
        { 53, 20 },
        { 53, 22 },
        { 53, 24 },
        --{ 53, 26 },
        { 53, 28 },
        { 53, 30 },
        { 53, 32 },
        { 53, 34 },
        { 53, 36 },
        -- bottom
        { 51, 38 },
        { 49, 38 },
        { 47, 38 },
        { 45, 38 },
        { 43, 38 },
        { 41, 38 },
        { 39, 38 },
        { 37, 38 },
        { 35, 38 },
        { 33, 38 },
        --{ 31, 38 },
        { 29, 38 },
        { 27, 38 },
        { 25, 38 },
        { 23, 38 },
        { 21, 38 },
        { 19, 38 },
        { 17, 38 },
        { 15, 38 },
        { 13, 38 },
        -- left
        { 11, 36 },
        { 11, 34 },
        { 11, 32 },
        { 11, 30 },
        { 11, 28 },
        { 11, 26 },
        --{ 11, 24 },
        { 11, 22 },
        { 11, 20 },
        { 11, 18 },
        { 11, 16 },
        { 11, 14 },
    }

    -- Loads the spikes according to the array
    for my_index, my_array in pairs(spike_objects4) do
        spikes4[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2]);
        Map:AddGroundObject(spikes4[my_index]);
        spikes4[my_index]:SetVisible(false);
        spikes4[my_index]:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    -- Adds the red rolling stones
    stone1 = CreateObject(Map, "Rolling Stone", 30.8, 35.8);
    stone1:SetEventWhenTalking("Check hero position for stone 1");
    Map:AddGroundObject(stone1);
    stone2 = CreateObject(Map, "Rolling Stone", 13, 24.2);
    stone2:SetEventWhenTalking("Check hero position for stone 2");
    Map:AddGroundObject(stone2);
    stone3 = CreateObject(Map, "Rolling Stone", 51, 26.2);
    stone3:SetEventWhenTalking("Check hero position for stone 3");
    Map:AddGroundObject(stone3);

    event = vt_map.IfEvent("Check hero position for stone 1", "check_diagonal_stone1", "Push the stone 1", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the stone 1", "start_to_move_the_stone1", "move_the_stone_update1")
    EventManager:RegisterEvent(event);

    event = vt_map.IfEvent("Check hero position for stone 2", "check_diagonal_stone2", "Push the stone 2", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the stone 2", "start_to_move_the_stone2", "move_the_stone_update2")
    EventManager:RegisterEvent(event);

    event = vt_map.IfEvent("Check hero position for stone 3", "check_diagonal_stone3", "Push the stone 3", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the stone 3", "start_to_move_the_stone3", "move_the_stone_update3")
    EventManager:RegisterEvent(event);
end

function _add_flame(x, y)
    local object = vt_map.SoundObject("snd/campfire.ogg", x, y, 10.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;

    object = CreateObject(Map, "Flame1", x, y);
    Map:AddGroundObject(object);

    Map:AddHalo("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    Map:AddHalo("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to mountain shrine stairs", "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_script.lua", "from_shrine_third_floor");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Close bottom fences", "bottom_fence_start", "bottom_fence_update");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Orlinn goes near boss", orlinn, 32, 33.5, false);
    event:AddEventLinkAtEnd("Set camera on Boss");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Set camera on Boss", "camera_on_boss_start", "camera_update");
    event:AddEventLinkAtEnd("Boss introduction");
    EventManager:RegisterEvent(event);
    
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Yiek! A big monster!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    text = vt_system.Translate("My name is Andromalius, Guardian of the Sacred Seal, and Keeper of the Goddess Shrine...");
    dialogue:AddLine(text, andromalius);
    text = vt_system.Translate("You have trespassed the Holy ground of the Ancient and remained unharmed for too long in this temple...");
    dialogue:AddLine(text, andromalius);
    text = vt_system.Translate("Thus, let death embrace you, little boy, for your comrades are already dying under my charm and you have to join them into the other world...");
    dialogue:AddLine(text, andromalius);
    text = vt_system.Translate("Yiek!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Boss introduction", dialogue);
    event:AddEventLinkAtEnd("Set camera on Orlinn");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Set camera on Orlinn", "camera_on_orlinn_start", "camera_update");
    event:AddEventLinkAtEnd("Start spikes");
    event:AddEventLinkAtEnd("Start battle");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Start spikes", "spikes_start", "spikes_update");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Start battle", "battle_start", "battle_update");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Boss hurt effect", "hurt_effect_start", "hurt_effect_update")
    EventManager:RegisterEvent(event);

end

-- Tells the boss battle state
local boss_started = false;
local boss_damage = 0;
local battle_won = false;

-- zones
local to_shrine_stairs_zone = {};
local start_boss_zone = {};
local boss_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_stairs_zone = vt_map.CameraZone(30, 34, 46, 48);
    Map:AddZone(to_shrine_stairs_zone);
    start_boss_zone = vt_map.CameraZone(30, 34, 38, 40);
    Map:AddZone(start_boss_zone);
    boss_zone = vt_map.CameraZone(28, 36, 20, 28);
    Map:AddZone(boss_zone);

end

function _CheckBossZone(stone)
    if (boss_zone:IsInsideZone(stone:GetXPosition(), stone:GetYPosition())) then
        stone:SetVisible(false);
        stone:SetPosition(0, 0);
        _HurtBoss();
    end
end

function _CheckStones()
    -- check whether a stone hit the boss
    _CheckBossZone(stone1);
    _CheckBossZone(stone2);
    _CheckBossZone(stone3);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_stairs_zone:IsCameraEntering() == true) then
        orlinn:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine stairs");
    elseif (boss_started == false and start_boss_zone:IsCameraEntering() == true) then
        orlinn:SetMoving(false);
        orlinn:SetDirection(vt_map.MapMode.NORTH);
        Map:PushState(vt_map.MapMode.STATE_SCENE);

        boss_started = true;
        EventManager:StartEvent("Close bottom fences");
        EventManager:StartEvent("Orlinn goes near boss");
    elseif (boss_started == true and boss_zone:IsCameraEntering() == true
            and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        _SpawnFireBall(29, 25);
        _SpawnFireBall(32, 28);
        _SpawnFireBall(35, 25);
    elseif (boss_started == true and battle_won == false) then
        _CheckStones()
    end

end

-- Stones handling

function _CheckForDiagonals(target)
    -- Check for diagonals. If the player is in diagonal,
    -- whe shouldn't trigger the event at all, as only straight relative position
    -- to the target sprite will work correctly.
    -- (Here used only for shrooms and stones)

    local hero_x = orlinn:GetXPosition();
    local hero_y = orlinn:GetYPosition();

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

    local hero_x = orlinn:GetXPosition();
    local hero_y = orlinn:GetYPosition();

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


-- Fireballs handling
local fireballs_array = {};

function _SpawnFireBall(x, y)
    local fireball = vt_map.ParticleObject("dat/effects/particles/fire.lua", x, y);
    fireball:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(fireball);
    
    local new_table = {};
    new_table["object"] = fireball;
    new_table["lifetime"] = 5000;
    
    table.insert(fireballs_array, new_table);
end


function _HideAllSpikes()
    for my_index, my_object in pairs(spikes1) do
        my_object:SetVisible(false);
        my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
    for my_index, my_object in pairs(spikes2) do
        my_object:SetVisible(false);
        my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
    for my_index, my_object in pairs(spikes3) do
        my_object:SetVisible(false);
        my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
    for my_index, my_object in pairs(spikes4) do
        my_object:SetVisible(false);
        my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
end

-- Make the next spike visible, and remove the current one
-- returns the new index value.
function _UpdateSpike(spike_array, spike_index, max_size)
    local spike_object = spike_array[spike_index];
    if (spike_object ~= nil) then
        spike_object:SetVisible(false);
        spike_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
    spike_index = spike_index + 1;
    if (spike_index > max_size) then
        spike_index = 0;
    end

    spike_object = spike_array[spike_index];
    if (spike_object ~= nil) then
        spike_object:SetVisible(true);
        spike_object:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
    end
    return spike_index;
end

local spikes_update_time = 0;
local spike1_index1 = 0;
local spike1_index2 = 0;
local spike2_index1 = 0;
local spike2_index2 = 0;
local spike3_index1 = 0;
local spike3_index2 = 0;
local spike3_index3 = 0;
local spike4_index1 = 0;
local spike4_index2 = 0;
local spike4_index3 = 0;

-- battle members
local fireball_timer = 0;
local fireball_timer2 = 0;
local fireball_timer3 = 0;
local fireball_speed = 0;
local andromalius_current_action = "idle";

function _HurtBoss()
    -- TODO: Play hurt sound
    andromalius:SetCustomAnimation("open_mouth_left", -1);
    boss_damage = boss_damage + 1;
    fireball_speed = fireball_speed + 0.001;
    EventManager:StartEvent("Boss hurt effect");

    if (boss_damage >= 9) then
        battle_won = true;
        return;
    end
end

-- Stone variables
local stone_direction1 = vt_map.MapMode.EAST;
local stone_direction2 = vt_map.MapMode.EAST;
local stone_direction3 = vt_map.MapMode.EAST;

local hurt_effect_time = 0;
local hurt_color = vt_video.Color(1.0, 0.0, 0.0, 1.0);

-- Map Custom functions
-- Used through scripted events
map_functions = {
    -- The spikes are started
    spikes_start = function()
        _HideAllSpikes();

        spikes_update_time = 0;
        spike1_index1 = 0;
        spike1_index2 = 7; -- 14/2
        spike2_index1 = 0;
        spike2_index2 = 11; -- 22/2
        spike3_index1 = 0;
        spike3_index2 = 12; -- 37/3
        spike3_index3 = 24; -- 37/3*2
        spike4_index1 = 0;
        spike4_index2 = 13; -- 41/3
        spike4_index3 = 26; -- 41/3*2
    end,
    
    spikes_update = function()
        spikes_update_time = spikes_update_time + SystemManager:GetUpdateTime();
        if (spikes_update_time < 700) then
            return false;
        end
        spikes_update_time = 0;
        
        spike1_index1 = _UpdateSpike(spikes1, spike1_index1, 14); -- size of array
        spike1_index2 = _UpdateSpike(spikes1, spike1_index2, 14);
        spike2_index1 = _UpdateSpike(spikes2, spike2_index1, 22);
        spike2_index2 = _UpdateSpike(spikes2, spike2_index2, 22);
        spike3_index1 = _UpdateSpike(spikes3, spike3_index1, 37);
        spike3_index2 = _UpdateSpike(spikes3, spike3_index2, 37);
        spike3_index3 = _UpdateSpike(spikes3, spike3_index3, 37);
        spike4_index1 = _UpdateSpike(spikes4, spike4_index1, 41);
        spike4_index2 = _UpdateSpike(spikes4, spike4_index2, 41);
        spike4_index3 = _UpdateSpike(spikes4, spike4_index3, 41);

        AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
        return false;
    end,

    bottom_fence_start = function()
        lower_fence1:SetXPosition(29);
        lower_fence2:SetXPosition(35);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,
    
    bottom_fence_update = function()
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = update_time * 0.005;
        lower_fence1:SetXPosition(lower_fence1:GetXPosition() + movement_diff);
        lower_fence2:SetXPosition(lower_fence2:GetXPosition() - movement_diff);

        if (lower_fence1:GetXPosition() >= 31) then
            return true;
        end
        return false;
    end,

    camera_on_boss_start = function()
        Map:SetCamera(andromalius, 800);
        orlinn:SetDirection(vt_map.MapMode.NORTH);
    end,

    camera_on_orlinn_start = function()
        Map:SetCamera(orlinn, 500);
    end,

    camera_update = function()
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        return true;
    end,

    battle_start = function()
        fireball_timer = 0;
        fireball_speed = 0.003;
        boss_damage = 0;
        battle_won = false;
        -- Free the player so he can move.
        Map:PopState();
    end,

    -- TODO: Make boss periodically disable the spikes and throw a stone.
    battle_update = function()
        local update_time = SystemManager:GetUpdateTime();
        fireball_timer = fireball_timer + update_time;
        
        -- Make andromalius watch orlinn
        andromalius:LookAt(orlinn);

        -- Make andromalius start to throw fireballs
        if (fireball_timer > 8000 and andromalius_current_action == "idle") then
            andromalius_current_action = "fireballs";
            if (orlinn:GetXPosition() > andromalius:GetXPosition()) then
                andromalius:SetCustomAnimation("open_mouth_right", 0);
            else
                andromalius:SetCustomAnimation("open_mouth_left", 0);
            end
            _SpawnFireBall(andromalius:GetXPosition(), andromalius:GetYPosition());
            fireball_timer2 = 0;
            fireball_timer3 = 0;
        end
        -- Makes him keep up throwing them 3 times in total.
        if (andromalius_current_action == "fireballs") then
            if (fireball_timer2 < 1000) then
                fireball_timer2 = fireball_timer2 + update_time;
                if (fireball_timer2 >= 1000) then
                    _SpawnFireBall(andromalius:GetXPosition(), andromalius:GetYPosition());
                end
            end
            if (fireball_timer2 >= 1000 and fireball_timer3 < 1000) then
                fireball_timer3 = fireball_timer3 + update_time;
                if (fireball_timer3 >= 1000) then
                    _SpawnFireBall(andromalius:GetXPosition(), andromalius:GetYPosition());
                    andromalius_current_action = "idle";
                    andromalius:DisableCustomAnimation();
                    -- reset the main timer.
                    fireball_timer = 0;
                end
            end
        end

        -- Update fireballs position and lifetime
        for key, my_table in pairs(fireballs_array) do
            if (my_table ~= nil) then
                local object = my_table["object"];
                local lifetime = my_table["lifetime"];
                if (object ~= nil) then
                    -- compute the fireball movement.
                    local x_diff = object:GetXPosition() - orlinn:GetXPosition();
                    if (x_diff > 0.5) then
                        x_diff = -1.0
                    elseif (x_diff < -0.5) then
                        x_diff = 1.0
                    else
                        x_diff = 0.0;
                    end

                    local y_diff = object:GetYPosition() - orlinn:GetYPosition();
                    if (y_diff > 0.5) then
                        y_diff = -1.0
                    elseif (y_diff < -0.5) then
                        y_diff = 1.0
                    else
                        y_diff = 0.0
                    end

                    object:SetXPosition(object:GetXPosition() + update_time * fireball_speed * x_diff);
                    object:SetYPosition(object:GetYPosition() + update_time * fireball_speed * y_diff);
                    my_table["lifetime"] = lifetime - update_time;
                end
            end
        end
        -- remove fireballs when their lifetime has run out
        for key, my_table in pairs(fireballs_array) do
            if (my_table ~= nil) then
                local object = my_table["object"];
                local lifetime = my_table["lifetime"];
                if (object ~= nil and lifetime <= 0.0) then
                    object:Stop();
                    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                    table.remove(fireballs_array, key);
                    -- object:CanDelete(); -- TODO: add support for something like this.
                    -- object = nil;
                end
            end
        end

        return false;
    end,

    check_diagonal_stone1 = function()
        return _CheckForDiagonals(stone1);
    end,

    start_to_move_the_stone1 = function()
        stone_direction1 = _GetStoneDirection(stone1);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    move_the_stone_update1 = function()
        return _UpdateStoneMovement(stone1, stone_direction1)
    end,

    check_diagonal_stone2 = function()
        return _CheckForDiagonals(stone2);
    end,

    start_to_move_the_stone2 = function()
        stone_direction2 = _GetStoneDirection(stone2);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    move_the_stone_update2 = function()
        return _UpdateStoneMovement(stone2, stone_direction2)
    end,

    check_diagonal_stone3 = function()
        return _CheckForDiagonals(stone3);
    end,

    start_to_move_the_stone3 = function()
        stone_direction3 = _GetStoneDirection(stone3);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    move_the_stone_update3 = function()
        return _UpdateStoneMovement(stone3, stone_direction3)
    end,

    hurt_effect_start = function()
        hurt_effect_time = 0;
    end,

    hurt_effect_update = function()
        hurt_effect_time = hurt_effect_time + SystemManager:GetUpdateTime();

        if (hurt_effect_time < 300.0) then
            hurt_color:SetAlpha(hurt_effect_time / 300.0 / 3.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(hurt_color);
            return false;
        end

        if (hurt_effect_time < 600.0) then
            hurt_color:SetAlpha(((600.0 - hurt_effect_time) / 300.0) / 3.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(hurt_color);
            return false;
        end

        -- The boss avenges
        if (battle_won == false) then
            _SpawnFireBall(29, 25);
            _SpawnFireBall(32, 28);
            _SpawnFireBall(35, 25);
        end
        return true;
    end,
}
