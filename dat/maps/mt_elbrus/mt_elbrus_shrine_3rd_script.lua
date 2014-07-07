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
    AudioManager:LoadSound("snd/falling.ogg", Map);
    AudioManager:LoadSound("snd/cave-in.ogg", Map);
    AudioManager:LoadSound("snd/heavy_bump.wav", Map);
    AudioManager:LoadSound("snd/battle_encounter_03.ogg", Map);
    AudioManager:LoadSound("snd/fire1_spell.ogg", Map);
    AudioManager:LoadSound("snd/low_scream.ogg", Map);
    AudioManager:LoadSound("snd/low_scream_long.ogg", Map);

    AudioManager:LoadMusic("mus/dont_close_your_eyes.ogg", Map);
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

    if (GlobalManager:GetEventValue("story", "mt_elbrus_shrine_boss_beaten") == 1) then
        andromalius:SetVisible(false);
        andromalius:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
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

local waterfall_sound = nil;
local rumble_sound = nil;

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}

    _add_flame(39.5, 7);
    _add_flame(25.5, 7);

    -- Add the water fall trigger button
    -- A trigger that will open the gate in the SE map.
    object = vt_map.TriggerObject("mt elbrus waterfall trigger",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "", "Water trigger on event");
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    object:SetPosition(32, 11);
    Map:AddFlatGroundObject(object);

    -- Add the corresponding waterfall ambient sound, ready to be started.
    waterfall_sound = vt_map.SoundObject("snd/fountain_large.ogg", 32, 11, 100.0);
    Map:AddAmbientSoundObject(waterfall_sound)

    -- Add the rumble sound used at boss end
    rumble_sound = vt_map.SoundObject("snd/rumble_continuous.ogg", 19.0, 48.0, 20.0);
    Map:AddAmbientSoundObject(rumble_sound);
    rumble_sound:Stop();

    -- Deactivate the sound when the trigger is not already pushed
    if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 0) then
        waterfall_sound:Stop();
    end

    object = CreateObject(Map, "Layna Statue", 29, 10);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Layna Statue", 35, 10);
    Map:AddGroundObject(object);

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
    upper_fence2 = CreateObject(Map, "Stone Fence1", 33, 16);
    Map:AddGroundObject(upper_fence2);

    if (GlobalManager:GetEventValue("story", "mt_elbrus_shrine_boss_beaten") == 1) then
        upper_fence1:SetXPosition(29);
        upper_fence2:SetXPosition(35);
    end

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

    event = vt_map.MapTransitionEvent("Restart map", "dat/maps/mt_elbrus/mt_elbrus_shrine_3rd_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine_3rd_script.lua", "from_shrine_stairs");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks south", orlinn, vt_map.MapMode.SOUTH);
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

    -- Reset stones action
    event = vt_map.ScriptedEvent("The boss hit the ground", "hit_ground_start", "hit_ground_update")
    event:AddEventLinkAtEnd("The stones are put away");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("The stones are put away", "visible_stones_fly_start", "visible_stones_fly_update")
    event:AddEventLinkAtEnd("The new stones fall");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("The new stones fall", "new_stones_fall_start", "new_stones_fall_update")
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Set camera on Boss 2", "camera_on_boss2_start", "camera_update");
    event:AddEventLinkAtEnd("Boss conclusion");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("You've proven yourself, Chosen One... You deserve to live after all...");
    dialogue:AddLine(text, andromalius);
    text = vt_system.Translate("Me? But I'm not the Chosen One.");
    dialogue:AddLineEmote(text, orlinn, "interrogation");
    text = vt_system.Translate("The Ones being few or many, still each of them will have to prove themselves as it is ought to be...");
    dialogue:AddLine(text, andromalius);
    text = vt_system.Translate("Farewell, Chosen One...");
    dialogue:AddLine(text, andromalius);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Boss conclusion", dialogue);
    event:AddEventLinkAtEnd("The Boss dies");
    EventManager:RegisterEvent(event);

    -- Won the battle!
    event = vt_map.ScriptedEvent("The Boss dies", "boss_die_start", "boss_die_update")
    event:AddEventLinkAtEnd("Set camera on Orlinn 2");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Set camera on Orlinn 2", "camera_on_orlinn2_start", "camera_update");
    event:AddEventLinkAtEnd("Open path");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Open path", "open_fences_start", "open_fences_update")
    EventManager:RegisterEvent(event);

    -- Waterfalls on event
    event = vt_map.ScriptedEvent("Water trigger on event", "waterfall_sound_start", "aterfall_sound_update")
    event:AddEventLinkAtEnd("Water shake event", 1000);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Water shake event", "waterfall_shake_start", "waterfall_shake_update")
    event:AddEventLinkAtEnd("Waterfall dialogue", 1000);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("I can hear water from everywhere...");
    dialogue:AddLineEmote(text, orlinn, "interrogation");
    text = vt_system.Translate("Kalya, Bronann!");
    dialogue:AddLineEventEmote(text, orlinn, "Orlinn looks south", "", "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Waterfall dialogue", dialogue);
    event:AddEventLinkAtEnd("Waterfall event end");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Waterfall event end", "waterfall_end_start", "waterfall_end_update")
    EventManager:RegisterEvent(event);

end

-- Tells the boss battle state
local boss_started = false;
local boss_damage = 0;
local battle_won = false;
-- boolean set to true when the stones are to be reset.
local ok_to_hit_the_ground = true;

-- zones
local to_shrine_stairs_zone = nil;
local start_boss_zone = nil;
local boss_zone = nil;

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
    -- Check whether the boss is hurt
    if (stone:GetCollisionMask() == vt_map.MapMode.NO_COLLISION) then
        return;
    end

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

-- Check whether orlinn is hurt and restart the map if so...
local orlinn_is_hurt = false

-- The array containing fireballs
local fireballs_array = {};

function _RestartMap()
    AudioManager:PlaySound("snd/battle_encounter_03.ogg");
    Map:PushState(vt_map.MapMode.STATE_SCENE);
    orlinn:SetMoving(false);
    orlinn:SetCustomAnimation("frightened_fixed", 0);
    EventManager:StartEvent("Restart map");
    orlinn_is_hurt = true;
end

function _CheckOrlinnCollisions()
    if (orlinn_is_hurt == true) then
        return;
    end

    -- check collision with fireballs
    for key, my_table in pairs(fireballs_array) do
        if (my_table ~= nil) then
            local object = my_table["object"];
            local lifetime = my_table["lifetime"];
            if (object ~= nil and lifetime > 0.0) then
                if (orlinn:IsCollidingWith(object) == true) then
                    _RestartMap();
                    return;
                end
            end
        end
    end

    -- Check collision with spikes
    for my_index, object in pairs(spikes1) do
        if (object ~= nil) then
            if (orlinn:IsCollidingWith(object) == true) then
                _RestartMap();
                return;
            end
        end
    end
    for my_index, object in pairs(spikes2) do
        if (object ~= nil) then
            if (orlinn:IsCollidingWith(object) == true) then
                _RestartMap();
                return;
            end
        end
    end
    for my_index, object in pairs(spikes3) do
        if (object ~= nil) then
            if (orlinn:IsCollidingWith(object) == true) then
                _RestartMap();
                return;
            end
        end
    end
    for my_index, object in pairs(spikes4) do
        if (object ~= nil) then
            if (orlinn:IsCollidingWith(object) == true) then
                _RestartMap();
                return;
            end
        end
    end
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_stairs_zone:IsCameraEntering() == true) then
        orlinn:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine stairs");
    end

    -- Stop checking for battle conditions as soon as the player won.
    if (battle_won == true) then
        return;
    end

    -- Disable the boss battle when it is won.
    if (GlobalManager:GetEventValue("story", "mt_elbrus_shrine_boss_beaten") == 1) then
        battle_won = true;
        return;
    end

    if (boss_started == false and start_boss_zone:IsCameraEntering() == true) then
        orlinn:SetMoving(false);
        orlinn:SetDirection(vt_map.MapMode.NORTH);
        Map:PushState(vt_map.MapMode.STATE_SCENE);

        EventManager:StartEvent("Close bottom fences");
        EventManager:StartEvent("Orlinn goes near boss");
    elseif (boss_started == true and boss_zone:IsCameraEntering() == true
            and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        _SpawnFireBall(29, 25);
        _SpawnFireBall(32, 28);
        _SpawnFireBall(35, 25);
    elseif (boss_started == true) then
        _CheckOrlinnCollisions();
        _CheckStones()
        -- Check whether we can reset stones.
        if (ok_to_hit_the_ground == true) then
            EventManager:StartEvent("The boss hit the ground");
            ok_to_hit_the_ground = false;
            return;
        end
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
function _SpawnFireBall(x, y)
    local fireball = vt_map.ParticleObject("dat/effects/particles/fire.lua", x, y);
    fireball:SetObjectID(Map.object_supervisor:GenerateObjectID());
    fireball:SetCollisionMask(vt_map.MapMode.ALL_COLLISION)
    Map:AddGroundObject(fireball);
    AudioManager:PlaySound("snd/fire1_spell.ogg");

    local new_table = {};
    new_table["object"] = fireball;
    new_table["lifetime"] = 5000;

    table.insert(fireballs_array, new_table);
end

-- Remove all fireballs
function _KillAllFireBalls()
    -- First tell the engine to get rid of the object
    for key, my_table in pairs(fireballs_array) do
        if (my_table ~= nil) then
            local object = my_table["object"];
            if (object ~= nil) then
                object:Stop();
                Map:RemoveGroundObject(object);
                object = nil;
            end
        end
    end

    -- Then empty the table, (table.remove() is reordering the keys.)
    fireballs_array = {};
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

-- reset balls action
local stone1_hit_ground = false;
local stone2_hit_ground = false;
local stone3_hit_ground = false;
local stones_reset_timer = 0;

function _HurtBoss()
    andromalius:SetCustomAnimation("open_mouth_left", -1);
    AudioManager:PlaySound("snd/low_scream.ogg");
    boss_damage = boss_damage + 1;
    fireball_speed = fireball_speed + 0.0005;
    EventManager:StartEvent("Boss hurt effect");
    if (boss_damage >= 6) then
        battle_won = true;
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        orlinn:SetMoving(false);
        orlinn:LookAt(andromalius);
        -- Stop the music very quickly.
        AudioManager:FadeOutAllMusic(400);

        EventManager:TerminateEvents("Start battle", false);
        EventManager:TerminateEvents("Start spikes", false);

        _HideAllSpikes();
        _KillAllFireBalls();
        EventManager:StartEvent("Set camera on Boss 2", 2000);
        return;
    end
end

-- Stone variables
local stone_direction1 = vt_map.MapMode.EAST;
local stone_direction2 = vt_map.MapMode.EAST;
local stone_direction3 = vt_map.MapMode.EAST;

local hurt_effect_time = 0;
local hurt_color = vt_video.Color(1.0, 0.0, 0.0, 1.0);
local boss_die_time = 0;
local boss_turn_head_time = 0;
local boss_head_to_right = true;
local boss_dying_sound = false;

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
        -- Fade out the music
        AudioManager:FadeOutAllMusic(1000);

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
        -- Play the special boss music
        AudioManager:PlayMusic("mus/dont_close_your_eyes.ogg");

        fireball_timer = 0;
        fireball_speed = 0.003;
        stones_reset_timer = 0;
        ok_to_hit_the_ground = false;
        boss_damage = 0;
        battle_won = false;
        boss_started = true;
        -- Free the player so he can move.
        Map:PopState();
    end,

    battle_update = function()
        local update_time = SystemManager:GetUpdateTime();

        -- Make andromalius watch orlinn
        andromalius:LookAt(orlinn);

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
        -- Stop and/or remove fireballs when their lifetime has run out
        for key, my_table in pairs(fireballs_array) do
            if (my_table ~= nil) then
                local object = my_table["object"];
                local lifetime = my_table["lifetime"];
                if (object ~= nil) then
                    -- Check whether the flame should stop
                    if (lifetime <= 0.0) then
                        object:Stop();
                        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
                    end
                    -- Once the particle effect is dead, we can remove the object.
                    if (object:IsAlive() == false) then
                        table.remove(fireballs_array, key);
                        Map:RemoveGroundObject(object);
                        object = nil;
                    end
                end
            end
        end

        -- Make the boss reset stones position from time to time.
        -- This will slow down the player for a few sec.
        stones_reset_timer = stones_reset_timer + update_time;
        if (stones_reset_timer > 30000 and andromalius_current_action == "idle") then
            andromalius_current_action = "stones";
            ok_to_hit_the_ground = true;
            return false;
        end

        -- Wait for the action to be finished.
        if (andromalius_current_action == "stones") then
            return false;
        end

        -- Make andromalius start to throw fireballs
        fireball_timer = fireball_timer + update_time;
        if (fireball_timer > 8000 and andromalius_current_action == "idle") then
            andromalius_current_action = "fireballs";
            if (orlinn:GetXPosition() > andromalius:GetXPosition()) then
                andromalius:SetCustomAnimation("open_mouth_right", 0);
            else
                andromalius:SetCustomAnimation("open_mouth_left", 0);
            end
            _SpawnFireBall(andromalius:GetXPosition(), andromalius:GetYPosition() - 2.0);
            fireball_timer2 = 0;
            fireball_timer3 = 0;
        end
        -- Makes him keep up throwing them 3 times in total.
        if (andromalius_current_action == "fireballs") then
            if (fireball_timer2 < 1000) then
                fireball_timer2 = fireball_timer2 + update_time;
                if (fireball_timer2 >= 1000) then
                    _SpawnFireBall(andromalius:GetXPosition(), andromalius:GetYPosition() - 2.0);
                end
            end
            if (fireball_timer2 >= 1000 and fireball_timer3 < 1000) then
                fireball_timer3 = fireball_timer3 + update_time;
                if (fireball_timer3 >= 1000) then
                    _SpawnFireBall(andromalius:GetXPosition(), andromalius:GetYPosition() - 2.0);
                    andromalius_current_action = "idle";
                    andromalius:DisableCustomAnimation();
                    -- reset the main timer.
                    fireball_timer = 0;
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

    hit_ground_start = function()
        -- The boss will hit the ground and make the visible stones fly away.
        -- Then, new ones will fall in place.
        hit_ground_update_time = 0;
        andromalius:SetPosition(32.0, 25.0);
    end,

    hit_ground_update = function()
        local update_time = SystemManager:GetUpdateTime();
        andromalius:SetYPosition(andromalius:GetYPosition() + 0.017 * update_time);

        if (andromalius:GetYPosition() > 29.0) then
            AudioManager:PlaySound("snd/heavy_bump.wav");
            Map:GetEffectSupervisor():ShakeScreen(6.0, 4000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_SUDDEN);

            -- Make Orlinn walk slowly because of the hit for a few sec.
            orlinn:SetMovementSpeed(vt_map.MapMode.SLOW_SPEED);
            return true;
        end

        return false;
    end,

    visible_stones_fly_start = function()
        AudioManager:PlaySound("snd/cave-in.ogg");
        stone1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        stone2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        stone3:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end,

    visible_stones_fly_update = function()
        -- Make Andromalius slowly come back to its former location.
        local update_time = SystemManager:GetUpdateTime();
        if (andromalius:GetYPosition() > 25.0) then
            andromalius:SetYPosition(andromalius:GetYPosition() - 0.003 * update_time);
        end

        -- Make the stones fly away from screen
        if (stone1:IsVisible() == true) then
            local x_diff = 0.030 * update_time;
            if (stone1:GetXPosition() < orlinn:GetXPosition()) then
                x_diff = -x_diff;
            end
            stone1:SetPosition(stone1:GetXPosition() + x_diff, stone1:GetYPosition() - 0.005 * update_time);
        end
        if (stone2:IsVisible() == true) then
            local x_diff = 0.030 * update_time;
            if (stone2:GetXPosition() < orlinn:GetXPosition()) then
                x_diff = -x_diff;
            end
            stone2:SetPosition(stone2:GetXPosition() + x_diff, stone2:GetYPosition() - 0.005 * update_time);
        end
        if (stone3:IsVisible() == true) then
            local x_diff = 0.030 * update_time;
            if (stone3:GetXPosition() < orlinn:GetXPosition()) then
                x_diff = -x_diff;
            end
            stone3:SetPosition(stone3:GetXPosition() + x_diff, stone3:GetYPosition() - 0.005 * update_time);
        end

        -- Make the stones invisible when they leave the map
        if (stone1:GetXPosition() <= 0 or stone1:GetXPosition() >= 70) then
            stone1:SetVisible(false);
        end
        if (stone2:GetXPosition() <= 0 or stone2:GetXPosition() >= 70) then
            stone2:SetVisible(false);
        end
        if (stone3:GetXPosition() <= 0 or stone3:GetXPosition() >= 70) then
            stone3:SetVisible(false);
        end

        -- Check that both Andromalius and the stones are in place
        if (andromalius:GetYPosition() <= 25.0 and stone1:IsVisible() == false
                and stone2:IsVisible() == false and stone3:IsVisible() == false) then

            -- Restore Orlinn speed
            orlinn:SetMovementSpeed(vt_map.MapMode.FAST_SPEED);
            return true;
        end

        return false;
    end,

    new_stones_fall_start = function()
        -- Place the stones, ready to fall
        stone1_hit_ground = false;
        stone2_hit_ground = false;
        stone3_hit_ground = false;

        stone1:SetPosition(30.8, 30.8);
        stone1:SetVisible(true);
        stone1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        stone2:SetPosition(13, 19.2);
        stone2:SetVisible(true);
        stone2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        stone3:SetPosition(51, 21.2);
        stone3:SetVisible(true);
        stone3:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        -- Play the falling sound
        AudioManager:PlaySound("snd/falling.ogg");
    end,

    new_stones_fall_update = function()
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.011 * update_time;

        if (stone1_hit_ground == false) then
            stone1:SetYPosition(stone1:GetYPosition() + movement_diff)
        end
        if (stone1:GetYPosition() >= 35.8) then
            stone1:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
            AudioManager:PlaySound("snd/stone_bump.ogg");
            stone1_hit_ground = true;
        end

        if (stone2_hit_ground == false) then
            stone2:SetYPosition(stone2:GetYPosition() + movement_diff)
        end
        if (stone2:GetYPosition() >= 24.2) then
            stone2:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
            AudioManager:PlaySound("snd/stone_bump.ogg");
            stone2_hit_ground = true;
        end

        if (stone3_hit_ground == false) then
            stone3:SetYPosition(stone3:GetYPosition() + movement_diff)
        end
        if (stone3:GetYPosition() >= 26.2) then
            stone3:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
            AudioManager:PlaySound("snd/stone_bump.ogg");
            stone3_hit_ground = true;
        end

        -- Check whether all the stones are ready.
        if (stone1_hit_ground == true and stone2_hit_ground == true
                and stone3_hit_ground == true) then

            -- Restart the battle actions
            andromalius_current_action = "idle";
            stones_reset_timer = 0;
            return true;
        end

        return false;
    end,

    camera_on_boss2_start = function()
        Map:SetCamera(andromalius, 800);
        orlinn:SetMoving(false);
        Map:GetEffectSupervisor():ShakeScreen(4.0, 0, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_NONE);
        rumble_sound:SetPosition(andromalius:GetXPosition(), andromalius:GetYPosition());
        rumble_sound:Start();
    end,

    camera_on_orlinn2_start = function()
        Map:SetCamera(orlinn, 500);
    end,

    boss_die_start = function()
        boss_die_time = 0;
        boss_turn_head_time = 0;
        Map:GetEffectSupervisor():StopShaking()
        rumble_sound:Stop();
        Map:GetEffectSupervisor():ShakeScreen(6.0, 10000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_LINEAR);
        AudioManager:PlaySound("snd/cave-in.ogg");
        andromalius:SetCustomAnimation("open_mouth_right", 0);
        boss_head_to_right = true;
        boss_dying_sound = false;
    end,

    boss_die_update = function()
        boss_die_time = boss_die_time + SystemManager:GetUpdateTime();
        boss_turn_head_time = boss_turn_head_time + SystemManager:GetUpdateTime();

        if (boss_turn_head_time > 1000) then
            boss_turn_head_time = 0;
            if (boss_head_to_right == false) then
                andromalius:SetCustomAnimation("open_mouth_right", 0);
                boss_head_to_right = true;
            elseif (boss_head_to_right == true) then
                andromalius:SetCustomAnimation("open_mouth_left", 0);
                boss_head_to_right = false;
            end
            if (boss_dying_sound == false) then
                AudioManager:PlaySound("snd/low_scream_long.ogg");
                boss_dying_sound = true;
            end
        end

        if (boss_die_time < 3300.0) then
            hurt_color:SetAlpha(boss_die_time / 3300.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(hurt_color);
            return false;
        end

        -- Once we've reached this code, the boss can be hidden
        if (andromalius:IsVisible() == true) then
            -- Add dying particle effect
            local dying_particles = vt_map.ParticleObject("dat/effects/particles/slow_burst_particles.lua",
                                                          andromalius:GetXPosition(), andromalius:GetYPosition());
            dying_particles:SetObjectID(Map.object_supervisor:GenerateObjectID());
            Map:AddGroundObject(dying_particles);

            -- Make sure all the fireballs are removed
            _KillAllFireBalls();

            andromalius:SetVisible(false);
            andromalius:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end

        if (boss_die_time < 8600.0) then
            hurt_color:SetAlpha(((8600.0 - boss_die_time) / 3300.0));
            Map:GetEffectSupervisor():EnableLightingOverlay(hurt_color);
            return false;
        end

        -- Restore Orlinn speed just in case
        orlinn:SetMovementSpeed(vt_map.MapMode.FAST_SPEED);
        -- Free the player
        Map:PopState();
        GlobalManager:SetEventValue("story", "mt_elbrus_shrine_boss_beaten", 1);
        return true;
    end,

    open_fences_start = function()
        lower_fence1:SetXPosition(31);
        lower_fence2:SetXPosition(33);
        upper_fence1:SetXPosition(31);
        upper_fence2:SetXPosition(33);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    open_fences_update = function()
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = update_time * 0.005;
        lower_fence1:SetXPosition(lower_fence1:GetXPosition() - movement_diff);
        lower_fence2:SetXPosition(lower_fence2:GetXPosition() + movement_diff);
        upper_fence1:SetXPosition(upper_fence1:GetXPosition() - movement_diff);
        upper_fence2:SetXPosition(upper_fence2:GetXPosition() + movement_diff);

        if (lower_fence1:GetXPosition() <= 29) then
            return true;
        end
        return false;
    end,

    waterfall_sound_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        orlinn:SetMoving(false);
        AudioManager:PlaySound("snd/heavy_bump.wav");
    end,

    waterfall_shake_start = function()
        AudioManager:PlaySound("snd/cave-in.ogg");
        waterfall_sound:Start();
        Map:GetEffectSupervisor():ShakeScreen(6.0, 4000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_LINEAR);
    end,

    waterfall_end_start = function()
        Map:PopState();
    end,

}
