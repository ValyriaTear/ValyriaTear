-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine3_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus Shrine"
map_image_filename = "data/story/common/locations/mountain_shrine.png"
map_subname = ""

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

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);

    -- Preload sounds
    AudioManager:LoadSound("sounds/stone_roll.wav", Map);
    AudioManager:LoadSound("sounds/opening_sword_unsheathe.wav", Map);
    AudioManager:LoadSound("sounds/battle_encounter_03.ogg", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from shrine main room)
    hero = CreateSprite(Map, "Bronann", 3.5, 35.5, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.EAST);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    if (GlobalManager:GetPreviousLocation() == "from_shrine_treasure_room") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(19.0, 13.0);
    end
end

local fence1 = nil
local fence2 = nil
-- An array of spikes
local spikes = {}

local spike_wall = nil;
local wall_rumble_sound = nil;

-- An array a breaking spikes when the death wall go over them
local breaking_spikes = {};
-- An array a bones that are pushed upward by the wall.
local pushed_bones = {};

-- Jars with treasures that will break once touched by the wall
local treasure_jars = {};

function _CreateObjects()

    _add_flame(11.5, 6);
    _add_flame(27.5, 6);
    _add_flame(1.5, 31);

    fence1 = CreateObject(Map, "Stone Fence1", 5, 34, vt_map.MapMode.GROUND_OBJECT);
    fence2 = CreateObject(Map, "Stone Fence1", 5, 40, vt_map.MapMode.GROUND_OBJECT);

    spikes[1] = CreateObject(Map, "Spikes1", 17, 12, vt_map.MapMode.GROUND_OBJECT);
    spikes[2] = CreateObject(Map, "Spikes1", 19, 12, vt_map.MapMode.GROUND_OBJECT);
    spikes[3] = CreateObject(Map, "Spikes1", 21, 12, vt_map.MapMode.GROUND_OBJECT);
    if (GlobalManager:GetEventValue("story", "mt_shrine_treasure_trap_done") == 1) then
        for _, spike in pairs(spikes) do
            spike:SetVisible(false);
            spike:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end

    spike_wall = vt_map.PhysicalObject.Create(vt_map.MapMode.GROUND_OBJECT);
    spike_wall:SetPosition(19.0, 48.0);
    spike_wall:SetCollPixelHalfWidth(15.0 * 16);
    spike_wall:SetCollPixelHeight(3.7 * 16);
    spike_wall:SetImgPixelHalfWidth(15.0 * 16);
    spike_wall:SetImgPixelHeight(5.0 * 16);
    spike_wall:AddStillFrame("dat/maps/mt_elbrus/spike_wall.png");
    spike_wall:SetVisible(false);
    spike_wall:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    wall_rumble_sound = vt_map.SoundObject.Create("sounds/rumble_continuous.ogg", 19.0, 48.0, 20.0);
    wall_rumble_sound:Stop();

    -- Adds breaking spikes
    local spikes_array = {
        {27, 39},
        {27, 37},
        {27, 35},
        {27, 33},
        {27, 31},
        {27, 29},
        {27, 27},
        {29, 27},
        {31, 27},
        {33, 27},

        {5, 26},
        {7, 26},
        {9, 26},
        {11, 26},
        {11, 28},
        {11, 30},
        {11, 32},
        {11, 24},
        {11, 22},

        {16, 30},
        {16, 28},
        {16, 26},
        {16, 24},
        {16, 22},
        {16, 20},
        {16, 18},
        {18, 18},
        {20, 18},
        {22, 18},
        {24, 18},

        {25, 23},
        {27, 23},
        {29, 23},
        {31, 23},
        {33, 23},

    };

    for my_index, my_array in pairs(spikes_array) do
        breaking_spikes[my_index] = CreateObject(Map, "Spikes1", my_array[1], my_array[2], vt_map.MapMode.GROUND_OBJECT);
    end

    -- Adds pushed bones
    local bones_array = {
        {23, 43},
        {29, 42},
        {31, 42.3},
        {13, 38},
        {11, 36},
        {6, 21},
        {7, 19},
        {26, 12},
        {27, 11},
        {5, 12},
        {32, 19},
        {9, 11.2},
    };

    for my_index, my_array in pairs(bones_array) do
        pushed_bones[my_index] = CreateObject(Map, "Bones1", my_array[1], my_array[2], vt_map.MapMode.GROUND_OBJECT);
    end

    -- Adds a few tempting but treacherous treasures...
    local jars_array = {
        -- Treasure name, x, y, item
        {"Elbrus_Shrine_trap1", 33, 25, 1004}, -- Periwinkle potion (Strength)
        {"Elbrus_Shrine_trap2", 33, 29, 3001}, -- Copper ore
        {"Elbrus_Shrine_trap3", 8, 23, 1001}, -- Minor Elixir
    };

    for my_index, my_array in pairs(jars_array) do
        treasure_jars[my_index] = CreateTreasure(Map, my_array[1], "Jar1", my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
        treasure_jars[my_index]:AddItem(my_array[4], 1);
    end

    -- If the trap has been deactivated, the treasures aren't there anymore...
    if (GlobalManager:GetEventValue("story", "mt_shrine_treasure_trap_done") == 1) then
        for _, my_object in pairs(treasure_jars) do
            my_object:SetVisible(false);
            my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            my_object:SetPosition(0, 0);
        end
    else
        -- Test whether a treasure has been already taken
        for _, my_object in pairs(treasure_jars) do
            if (GlobalManager:DoesEventExist("treasures", my_object:GetTreasureName()) == true) then
                my_object:SetVisible(false);
                my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                my_object:SetPosition(0, 0);
            end
        end
    end

end

function _add_flame(x, y)
    vt_map.SoundObject.Create("sounds/campfire.ogg", x, y, 10.0);

    CreateObject(Map, "Flame1", x, y, vt_map.MapMode.GROUND_OBJECT);

    vt_map.Halo.Create("data/visuals/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    vt_map.Halo.Create("data/visuals/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    vt_map.MapTransitionEvent.Create("to mountain shrine main room", "dat/maps/mt_elbrus/mt_elbrus_shrine2_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine2_script.lua", "from_shrine_trap_room");

    vt_map.MapTransitionEvent.Create("to mountain shrine main room-waterfalls", "dat/maps/mt_elbrus/mt_elbrus_shrine2_2_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine2_script.lua", "from_shrine_trap_room");

    vt_map.MapTransitionEvent.Create("to mountain shrine treasure room", "dat/maps/mt_elbrus/mt_elbrus_shrine9_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine9_script.lua", "from_shrine_trap_room");

    vt_map.ScriptedEvent.Create("Start trap", "trap_start", "trap_update");
    vt_map.ScriptedEvent.Create("Start trap end", "end_trap_start", "end_trap_update");

    vt_map.ScriptedEvent.Create("Make spike wall go up", "spike_wall_start", "spike_wall_update");

    event = vt_map.ScriptedEvent.Create("Make spike wall go down", "spike_wall_down_start", "spike_wall_down_update");
    event:AddEventLinkAtEnd("Start trap end");
end

-- Tells which is the latest monster group the party has defeated.
local current_monster_id = -1;
-- Array of enemy zones, enabled one after another...
local roam_zones = {}

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("music/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_shrine.png");
    enemy:AddBattleScript("dat/battles/mountain_shrine_battle_anim.lua");
    enemy:AddBattleScript("dat/maps/mt_elbrus/battles_in_trap_map_script.lua");
end

function _CreateEnemiesZones()
    local enemy = nil;

    roam_zones[1] = vt_map.EnemyZone.Create(10, 32, 12, 16);
    enemy = CreateEnemySprite(Map, "slime");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:SetBoss(true);
    roam_zones[1]:AddEnemy(enemy, 1);
    roam_zones[1]:SetSpawnsLeft(1); -- This monster shall spawn only one time.

    --roam_zones[1]:SetEnabled(false); -- Not disabled since it's the first one.

    roam_zones[2] = vt_map.EnemyZone.Create(10, 32, 12, 16);
    enemy = CreateEnemySprite(Map, "spider");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(2);
    enemy:AddEnemy(2);
    enemy:AddEnemy(2);
    enemy:AddEnemy(1);
    enemy:SetBoss(true);
    roam_zones[2]:AddEnemy(enemy, 1);
    roam_zones[2]:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    roam_zones[2]:SetEnabled(false); -- Disabled per default

    roam_zones[3] = vt_map.EnemyZone.Create(10, 32, 12, 16);
    -- Some bats
    enemy = CreateEnemySprite(Map, "bat");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:AddEnemy(4);
    enemy:SetBoss(true);
    roam_zones[3]:AddEnemy(enemy, 1);
    roam_zones[3]:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    roam_zones[3]:SetEnabled(false); -- Disabled per default

    roam_zones[4] = vt_map.EnemyZone.Create(10, 32, 12, 16);
    enemy = CreateEnemySprite(Map, "snake");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(4);
    enemy:AddEnemy(4);
    enemy:AddEnemy(4);
    enemy:AddEnemy(4);
    enemy:AddEnemy(4);
    enemy:AddEnemy(6);
    enemy:SetBoss(true);
    roam_zones[4]:AddEnemy(enemy, 1);
    roam_zones[4]:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    roam_zones[4]:SetEnabled(false); -- Disabled per default

    roam_zones[5] = vt_map.EnemyZone.Create(10, 32, 12, 16);
    enemy = CreateEnemySprite(Map, "big slime");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(5, 812.0, 350.0);
    enemy:AddEnemy(5, 612.0, 450.0);
    enemy:SetBoss(true);
    roam_zones[5]:AddEnemy(enemy, 1);
    roam_zones[5]:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    roam_zones[5]:SetEnabled(false); -- Disabled per default

    roam_zones[6] = vt_map.EnemyZone.Create(10, 32, 12, 16);
    enemy = CreateEnemySprite(Map, "Eyeball");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:SetBoss(true);
    roam_zones[6]:AddEnemy(enemy, 1);
    roam_zones[6]:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    roam_zones[6]:SetEnabled(false); -- Disabled per default

    roam_zones[7] = vt_map.EnemyZone.Create(10, 32, 12, 16);
    enemy = CreateEnemySprite(Map, "Beetle");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(16);
    enemy:AddEnemy(15);
    enemy:AddEnemy(15);
    enemy:AddEnemy(16);
    enemy:AddEnemy(15);
    enemy:AddEnemy(15);
    enemy:AddEnemy(17);
    enemy:SetBoss(true);
    roam_zones[7]:AddEnemy(enemy, 1);
    roam_zones[7]:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    roam_zones[7]:SetEnabled(false); -- Disabled per default

    roam_zones[8] = vt_map.EnemyZone.Create(10, 32, 12, 16);
    enemy = CreateEnemySprite(Map, "Skeleton");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(16);
    enemy:AddEnemy(19);
    enemy:AddEnemy(15);
    enemy:AddEnemy(19);
    enemy:AddEnemy(19);
    enemy:AddEnemy(19);
    enemy:AddEnemy(19);
    enemy:AddEnemy(17);
    enemy:SetBoss(true);
    roam_zones[8]:AddEnemy(enemy, 1);
    roam_zones[8]:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    roam_zones[8]:SetEnabled(false); -- Disabled per default

    roam_zones[9] = vt_map.EnemyZone.Create(10, 32, 12, 16);
    enemy = CreateEnemySprite(Map, "Skeleton");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(16);
    enemy:AddEnemy(19);
    enemy:AddEnemy(15);
    enemy:AddEnemy(19);
    enemy:AddEnemy(20);
    enemy:AddEnemy(19);
    enemy:AddEnemy(19);
    enemy:AddEnemy(17);
    enemy:SetBoss(true);
    roam_zones[9]:AddEnemy(enemy, 1);
    roam_zones[9]:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    roam_zones[9]:SetEnabled(false); -- Disabled per default
end

local monsters_started = false;

function _CheckEnemiesState()

    if (current_monster_id >= 10) then
        return;
    end

    if (roam_zones[current_monster_id] == nil) then
        return;
    end

    if (roam_zones[current_monster_id]:GetSpawnsLeft() == 0) then
        -- Change the active zone.
        roam_zones[current_monster_id]:SetEnabled(false);
        current_monster_id = current_monster_id + 1;

        if (roam_zones[current_monster_id] ~= nil) then
            roam_zones[current_monster_id]:SetEnabled(true);
        end
    end

    if (current_monster_id >= 10) then
        -- Won!!
        -- Deactivate trap check
        monsters_started = false;

        -- Stop the death wall
        EventManager:EndEvent("Make spike wall go up", false);
        EventManager:StartEvent("Make spike wall go down");
        return;
    end
end

-- zones
local to_shrine_main_room_zone = nil
local to_shrine_treasure_room_zone = nil
local trap_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_main_room_zone = vt_map.CameraZone.Create(0, 2, 34, 38);
    to_shrine_treasure_room_zone = vt_map.CameraZone.Create(18, 20, 9, 10);
    trap_zone = vt_map.CameraZone.Create(10, 34, 10, 44);
end

local trap_started = false;
local caught_by_trap = false;

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (monsters_started == true) then
        if (roam_zones[current_monster_id] ~= nil) then
            if (roam_zones[current_monster_id]:GetSpawnsLeft() == 0) then
                _CheckEnemiesState();
            end
        end
    elseif (to_shrine_main_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.WEST);
        if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 0) then
            EventManager:StartEvent("to mountain shrine main room");
        else
            EventManager:StartEvent("to mountain shrine main room-waterfalls")
        end
    elseif (to_shrine_treasure_room_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "mt_shrine_treasure_trap_done") == 1) then
            hero:SetDirection(vt_map.MapMode.NORTH);
            EventManager:StartEvent("to mountain shrine treasure room");
        end
    elseif (trap_started == false and trap_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "mt_shrine_treasure_trap_done") == 0) then
            EventManager:StartEvent("Start trap");
            AudioManager:FadeOutActiveMusic(1500);
            EventManager:StartEvent("Make spike wall go up", 2000);
        end
    end

    -- Check whether the hero is dead because of trap.
    if (caught_by_trap == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetCustomAnimation("hurt", 0);
        hero:SetMoving(false);
        -- Trigger party damage.
        local hp_change = math.random(40, 60);
        _TriggerPartyDamage(hp_change);

        if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 0) then
            EventManager:StartEvent("to mountain shrine main room");
        else
            EventManager:StartEvent("to mountain shrine main room-waterfalls")
        end
        AudioManager:PlaySound("sounds/battle_encounter_03.ogg");
    end
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

local fence1_x = 0;
local fence1_y = 0;
local fence2_x = 0;
local fence2_y = 0;
local moving_step = 1;

-- Map Custom functions
-- Used through scripted events
map_functions = {
    trap_start = function()
        trap_started = true;
        fence1_x = 5;
        fence1_y = 34;
        fence2_x = 5;
        fence2_y = 40;
        moving_step = 1;
        current_monster_id = -1;
        monsters_started = false;
        AudioManager:PlaySound("sounds/stone_roll.wav");
    end,

    trap_update = function()
        local update_time = SystemManager:GetUpdateTime();

        -- Moving vertically first
        if (moving_step == 1) then
            if (fence1_y < 36) then
                fence1_y = fence1_y + 0.010 * update_time;
            end
            if (fence2_y > 38) then
                fence2_y = fence2_y - 0.010 * update_time;
            end
            if (fence1_y >= 36 and fence2_y <= 38) then
                AudioManager:PlaySound("sounds/stone_roll.wav");
                moving_step = 2;
            end
        -- Moving horizontally
        elseif(moving_step == 2) then
            if (fence1_x > 3) then
                fence1_x = fence1_x - 0.010 * update_time;
            end
            if (fence2_x > 3) then
                fence2_x = fence2_x - 0.010 * update_time;
            end
            if (fence1_x <= 3 and fence2_x <= 3) then
                moving_step = 3;
            end
        end

        fence1:SetPosition(fence1_x, fence1_y);
        fence2:SetPosition(fence2_x, fence2_y);
        if (moving_step < 3) then
            return false;
        end

        -- Spawn the first monster
        current_monster_id = 1;
        monsters_started = true;
        _CreateEnemiesZones();

        return true;
    end,

    end_trap_start = function()
        fence1_x = 3;
        fence1_y = 36;
        fence2_x = 3;
        fence2_y = 38;
        moving_step = 1;
        AudioManager:PlaySound("sounds/stone_roll.wav");
    end,

    end_trap_update = function()
        local update_time = SystemManager:GetUpdateTime();

        -- Moving horizontally first
        if(moving_step == 1) then
            if (fence1_x < 5) then
                fence1_x = fence1_x + 0.010 * update_time;
            end
            if (fence2_x < 5) then
                fence2_x = fence2_x + 0.010 * update_time;
            end
            if (fence1_x >= 5 and fence2_x >= 5) then
                moving_step = 2;
                AudioManager:PlaySound("sounds/stone_roll.wav");
            end
        -- Moving vertically
        elseif (moving_step == 2) then
            if (fence1_y > 34) then
                fence1_y = fence1_y - 0.010 * update_time;
            end
            if (fence2_y < 40) then
                fence2_y = fence2_y + 0.010 * update_time;
            end
            if (fence1_y <= 34 and fence2_y >= 40) then
                moving_step = 3;
            end
        end

        fence1:SetPosition(fence1_x, fence1_y);
        fence2:SetPosition(fence2_x, fence2_y);
        if (moving_step < 3) then
            return false;
        end

        -- Remove spikes
        AudioManager:PlaySound("sounds/opening_sword_unsheathe.wav");
        for my_index, spike in pairs(spikes) do
            spike:SetVisible(false);
            spike:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end

        return true;
    end,

    spike_wall_start = function()
        spike_wall:SetPosition(19, 50);
        spike_wall:SetVisible(true);
        spike_wall:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        caught_by_trap = false;

        -- Start the tremor
        Map:GetEffectSupervisor():ShakeScreen(0.6, 0, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_NONE); -- 0 means infinite time.
        wall_rumble_sound:SetPosition(19, 50);
        wall_rumble_sound:Start();
    end,

    spike_wall_update = function()
        local update_time = SystemManager:GetUpdateTime();
        local spike_y = spike_wall:GetYPosition();
        local movement_diff = 0.0007 * update_time;
        spike_y = spike_y - movement_diff;
        spike_wall:SetYPosition(spike_y);
        wall_rumble_sound:SetYPosition(spike_y);

        -- Break spikes along the way up
        local one_thing_broke = false;
        for _, my_object in pairs(breaking_spikes) do
            if (spike_wall:IsCollidingWith(my_object) == true) then
                -- Add broken spikes map object there
                local broken_spike = CreateObject(Map, "Spikes_broken1", my_object:GetXPosition(), my_object:GetYPosition());
                broken_spike:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                Map:AddFlatGroundObject(broken_spike);

                my_object:SetVisible(false);
                my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                one_thing_broke = true;
            end
        end

        -- Push bones along the way up
        for _, my_object in pairs(pushed_bones) do
            if (spike_wall:IsCollidingWith(my_object) == true) then
                my_object:SetYPosition(my_object:GetYPosition() - movement_diff);
            end
        end

        -- Break treasure jars
        for _, my_object in pairs(treasure_jars) do
            if (spike_wall:IsCollidingWith(my_object) == true) then
                my_object:SetVisible(false);
                my_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                my_object:SetPosition(0, 0);
                one_thing_broke = true;
            end
        end

        -- Play the shatter sound if at least one thing broke
        if (one_thing_broke == true) then
            AudioManager:PlaySound("sounds/magic_blast.ogg");
        end

        -- Check collision with camera
        if (spike_wall:IsCollidingWith(hero) == true) then
            caught_by_trap = true;
            return true;
        end

        -- Check collision with camera
        if (spike_wall:GetYPosition() > 14) then
            return false;
        end

        caught_by_trap = true;

        return true;
    end,

    spike_wall_down_start = function()
        -- Stops the tremor
        Map:GetEffectSupervisor():StopShaking();
        wall_rumble_sound:Stop();

        -- Set event as done
        GlobalManager:SetEventValue("story", "mt_shrine_treasure_trap_done", 1);
    end,

    spike_wall_down_update = function()
        local update_time = SystemManager:GetUpdateTime();
        local spike_y = spike_wall:GetYPosition();
        spike_y = spike_y + 0.002 * update_time;
        spike_wall:SetYPosition(spike_y);

        -- Check collision with camera
        if (spike_wall:GetYPosition() < 50) then
            return false;
        end

        spike_wall:SetVisible(false);
        spike_wall:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        return true;
    end,
}
