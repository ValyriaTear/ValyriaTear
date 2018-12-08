-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_cave1_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "data/story/common/locations/mt_elbrus.png"
map_subname = "Elbrus Grotto"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/awareness_el_corleo.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

-- the main character handler
local hero = nil
local bronann = nil

-- the main map loading code
function Load(m)

    Map = m;
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
    Map:GetEffectSupervisor():EnableAmbientOverlay("data/visuals/ambient/dark.png", 0.0, 0.0, false);
    if (GlobalManager:GetGameEvents():GetEventValue("story", "mt_elbrus_weather_level") > 0) then
        -- Place an omni ambient sound at the center of the map to add a nice indoor rainy effect.
        vt_map.SoundObject.Create("data/music/rain_indoors.ogg", 25.0, 20.0, 100.0);
    end
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 88, 77, vt_map.MapMode.GROUND_OBJECT); -- exit/entrance 1
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- For dialogues
    bronann = CreateSprite(Map, "Bronann", 0, 0, vt_map.MapMode.GROUND_OBJECT)
    bronann:SetVisible(false)

    if (GlobalManager:GetMapData():GetPreviousLocation() == "from_entrance2") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(9, 77);
    elseif (GlobalManager:GetMapData():GetPreviousLocation() == "from_entrance3") then
        hero:SetDirection(vt_map.MapMode.WEST);
        hero:SetPosition(93, 24);
    elseif (GlobalManager:GetMapData():GetPreviousLocation() == "from_entrance4") then
        hero:SetDirection(vt_map.MapMode.WEST);
        hero:SetPosition(88, 9);
    end
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("data/music/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("data/battles/battle_scenes/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    enemy:AddBattleScript("data/battles/battle_scenes/desert_cave_battle_anim.lua");
end
function _SetEventBattleEnvironment(event)
    event:SetMusic("data/music/heroism-OGA-Edward-J-Blakeley.ogg");
    event:SetBackground("data/battles/battle_scenes/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    event:AddScript("data/battles/battle_scenes/desert_cave_battle_anim.lua");
end

local shroom1 = nil
local shroom2 = nil

function _CreateObjects()
    local object = nil
    local npc = nil
    local event = nil
    local dialogue = nil
    local text = nil

    -- Add a halo showing the cave entrances
    -- exit 1
    vt_map.Halo.Create("data/visuals/lights/torch_light_mask.lua", 89, 88,
        vt_video.Color(0.3, 0.3, 0.46, 0.8));
    -- exit 2
    vt_map.Halo.Create("data/visuals/lights/torch_light_mask.lua", 9.5, 88,
        vt_video.Color(0.3, 0.3, 0.46, 0.8));

    shroom1 = CreateSprite(Map, "Shroom", 45, 42, vt_map.MapMode.GROUND_OBJECT);
    shroom1:SetName("");
    shroom1:SetDirection(vt_map.MapMode.SOUTH);
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("...");
    dialogue:AddLineEvent(text, shroom1, "", "Fight with Shroom 1");
    shroom1:AddDialogueReference(dialogue);

    event = vt_map.BattleEncounterEvent.Create("Fight with Shroom 1");
    event:AddEnemy(11, 512, 384); -- one shroom
    _SetEventBattleEnvironment(event);
    event:AddEventLinkAtEnd("Place Shroom 1 after fight", 10);

    vt_map.ScriptedSpriteEvent.Create("Place Shroom 1 after fight", shroom1, "place_shroom_after_fight", "")

    shroom2 = CreateSprite(Map, "Shroom", 47, 24, vt_map.MapMode.GROUND_OBJECT);
    shroom2:SetName("");
    shroom2:SetDirection(vt_map.MapMode.SOUTH);
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("...");
    dialogue:AddLineEvent(text, shroom2, "", "Fight with Shroom 2");
    shroom2:AddDialogueReference(dialogue);

    event = vt_map.BattleEncounterEvent.Create("Fight with Shroom 2");
    event:AddEnemy(11, 512, 384); -- one shroom
    _SetEventBattleEnvironment(event);
    event:AddEventLinkAtEnd("Place Shroom 2 after fight", 10);

    vt_map.ScriptedSpriteEvent.Create("Place Shroom 2 after fight", shroom2, "place_shroom_after_fight", "")

    -- left jump event
    event = vt_map.ScriptedEvent.Create("Prepare left jump dialogue", "add_scene_state", "")
    event:AddEventLinkAtEnd("Jump to exit 2 choice");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Shall we jump?");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("...");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Yes, let's go.");
    dialogue:AddOption(text, 2);
    text = vt_system.Translate("Not now.");
    dialogue:AddOptionEvent(text, 4, "Make hero step back from left jump");
    -- [Line 2] Yes
    text = vt_system.Translate("Ok!");
    dialogue:AddLineEvent(text, bronann, 4, "", "Make hero jump to exit 2"); -- 4 = Past the dialogue lines number. Makes the dialogue ends.
    vt_map.DialogueEvent.Create("Jump to exit 2 choice", dialogue);

    -- After dialogue choice
    event = vt_map.ScriptedEvent.Create("Make hero jump to exit 2", "make_hero_jump_to_exit", "jump_update_left")
    event:AddEventLinkAtEnd("Finish left jump dialogue");

    event = vt_map.PathMoveSpriteEvent.Create("Make hero step back from left jump", hero, 6.2, 61.0, false)
    event:AddEventLinkAtEnd("Finish left jump dialogue");

    vt_map.ScriptedEvent.Create("Finish left jump dialogue", "remove_scene_state", "")

    -- right jump event
    event = vt_map.ScriptedEvent.Create("Prepare right jump dialogue", "add_scene_state", "")
    event:AddEventLinkAtEnd("Jump to exit 4 choice");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Shall we jump?");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("...");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Yes, let's go.");
    dialogue:AddOption(text, 2);
    text = vt_system.Translate("Not now.");
    dialogue:AddOptionEvent(text, 4, "Make hero step back from right jump");
    -- [Line 2] Yes
    text = vt_system.Translate("Ok!");
    dialogue:AddLineEvent(text, bronann, 4, "", "Make hero jump to exit 4"); -- 4 = Past the dialogue lines number. Makes the dialogue ends.
    vt_map.DialogueEvent.Create("Jump to exit 4 choice", dialogue);

    -- After dialogue choice
    event = vt_map.ScriptedEvent.Create("Make hero jump to exit 4", "make_hero_jump_to_exit", "jump_update_right")
    event:AddEventLinkAtEnd("Finish right jump dialogue");

    event = vt_map.PathMoveSpriteEvent.Create("Make hero step back from right jump", hero, 84.0, 27.0, false)
    event:AddEventLinkAtEnd("Finish right jump dialogue");

    vt_map.ScriptedEvent.Create("Finish right jump dialogue", "remove_scene_state", "")

    -- Treasure box
    local chest = CreateTreasure(Map, "elbrus_grotto1_chest1", "Wood_Chest1", 9, 17, vt_map.MapMode.GROUND_OBJECT);
    chest:AddItem(3001, 1); -- Copper ore
end

function _CreateEnemies()
    local enemy = nil
    local roam_zone = nil

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(7, 13, 36, 41);
    -- Lonely bats
    enemy = CreateEnemySprite(Map, "bat");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:AddEnemy(4);
    enemy:AddEnemy(6);
    enemy:AddEnemy(4);
    roam_zone:AddEnemy(enemy, 1);

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(7, 13, 16, 21);
    -- bats, again
    enemy = CreateEnemySprite(Map, "bat");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    enemy:AddEnemy(4);
    enemy:AddEnemy(6);
    enemy:AddEnemy(4);
    roam_zone:AddEnemy(enemy, 1);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    -- To the first cave
    vt_map.MapTransitionEvent.Create("to exit 1", "data/story/mt_elbrus/mt_elbrus_path1_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_path1_script.lua", "from_grotto_exit1");

    vt_map.MapTransitionEvent.Create("to exit 2", "data/story/mt_elbrus/mt_elbrus_path1_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_path1_script.lua", "from_grotto_exit2");

    vt_map.MapTransitionEvent.Create("to exit 3", "data/story/mt_elbrus/mt_elbrus_path1_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_path1_script.lua", "from_grotto_exit3");

    vt_map.MapTransitionEvent.Create("to exit 4", "data/story/mt_elbrus/mt_elbrus_path1_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_path1_script.lua", "from_grotto_exit4");
end

-- zones
local exit1_zone = nil
local exit2_zone = nil
local exit3_zone = nil
local exit4_zone = nil

local left_jump_zone = nil
local right_jump_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    exit1_zone = vt_map.CameraZone.Create(84, 94, 78, 80);
    exit2_zone = vt_map.CameraZone.Create(4, 15, 78, 80);
    exit3_zone = vt_map.CameraZone.Create(94, 96, 21, 22);
    exit4_zone = vt_map.CameraZone.Create(90, 92, 7, 8);
    left_jump_zone = vt_map.CameraZone.Create(4, 8, 63, 64);
    right_jump_zone = vt_map.CameraZone.Create(77, 81, 29, 30);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (exit1_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to exit 1");
    elseif (exit2_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to exit 2");
    elseif (exit3_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to exit 3");
    elseif (exit4_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to exit 4");
    elseif (left_jump_zone:IsCameraEntering() == true
            and Map:CurrentState() ~= vt_map.MapMode.STATE_SCENE) then
        hero:SetMoving(false);
        EventManager:StartEvent("Prepare left jump dialogue");
    elseif (right_jump_zone:IsCameraEntering() == true
            and Map:CurrentState() ~= vt_map.MapMode.STATE_SCENE) then
        hero:SetMoving(false);
        EventManager:StartEvent("Prepare right jump dialogue");
    end
end

-- Map Custom functions
-- Used through scripted events

map_functions = {

    place_shroom_after_fight = function(shroom)
        local hero_x = hero:GetXPosition();
        local hero_y = hero:GetYPosition();

        local shroom_x = shroom:GetXPosition();
        local shroom_y = shroom:GetYPosition();

        -- New position of the shroom
        local shroom_new_x = shroom_x;
        local shroom_new_y = shroom_y;

        -- Determine the hero position relative to the shroom
        if (hero_y > shroom_y + 0.3) then
            -- the hero is below, the shroom is pushed upward.
            shroom_new_y = shroom_new_y - 2.1;
        elseif (hero_y < shroom_y - 1.5) then
            -- the hero is above, the shroom is pushed downward.
            shroom_new_y = shroom_new_y + 2.2;
        elseif (hero_x < shroom_x - 1.2) then
            -- the hero is on the left, the shroom is pushed to the right.
            shroom_new_x = shroom_new_x + 2.1;
        elseif (hero_x > shroom_x + 1.2) then
            -- the hero is on the right, the shroom is pushed to the left.
            shroom_new_x = shroom_new_x - 2.1;
        end

        -- Place the shroom
        shroom:SetPosition(shroom_new_x, shroom_new_y);
        shroom:SetCustomAnimation("mushroom_ko", 0); -- 0 means forever
        -- Remove its dialogue (preventing a new fight)
        shroom:ClearDialogueReferences();
    end,

    add_scene_state = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

    remove_scene_state = function()
        Map:PopState();
    end,

    make_hero_jump_to_exit = function()
        -- Set the jump south animation
        hero:SetCustomAnimation("jump_south", 0); -- 0 means forever
    end,

    jump_update_left = function()
        if (hero:GetYPosition() >= 68.0) then
            -- Remove the custom animation
            hero:DisableCustomAnimation();
            -- Make him/her look south after the jump.
            hero:SetDirection(vt_map.MapMode.SOUTH);
            AudioManager:PlaySound("data/sounds/footstep_grass2.wav");
            return true;
        end
        local update_time = SystemManager:GetUpdateTime();
        if (hero:GetYPosition() < 68.0) then
            hero:SetYPosition(hero:GetYPosition() + 0.020 * update_time);
        end
        return false;
    end,

    jump_update_right = function()
        if (hero:GetYPosition() >= 33.6) then
            -- Remove the custom animation
            hero:DisableCustomAnimation();
            -- Make him/her look south after the jump.
            hero:SetDirection(vt_map.MapMode.SOUTH);
            AudioManager:PlaySound("data/sounds/footstep_grass2.wav");
            return true;
        end
        local update_time = SystemManager:GetUpdateTime();
        if (hero:GetYPosition() < 33.6) then
            hero:SetYPosition(hero:GetYPosition() + 0.020 * update_time);
        end
        return false;
    end,
}
