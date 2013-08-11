-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_cave3_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "img/menus/locations/mt_elbrus.png"
map_subname = "Elbrus Grotto"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/awareness_el_corleo.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- the main character handler
local hero = {};

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

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

    -- Preloads the action sounds to avoid glitches
    AudioManager:LoadSound("snd/cave-in.ogg", Map);
    AudioManager:LoadSound("snd/stone_roll.wav", Map);
    AudioManager:LoadSound("snd/stone_bump.ogg", Map);
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
    -- Default hero and position (entrance 1)
    hero = CreateSprite(Map, "Bronann", 50, 15); -- exit/entrance 1
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_elbrus_entrance3-2") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(3, 21);
    end

    Map:AddGroundObject(hero);
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    enemy:AddBattleScript("dat/battles/desert_cave_battle_anim.lua");
end
function _SetEventBattleEnvironment(event)
    event:SetMusic("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    event:SetBackground("img/backdrops/battle/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    event:AddScript("dat/battles/desert_cave_battle_anim.lua");
end

local shroom1 = {};
local shroom2 = {};
local shroom3 = {};
local shroom4 = {};
local shroom5 = {};

local rolling_stone1 = {};
local rolling_stone2 = {};

-- Triggers
local stone_trigger1 = {};
local stone_trigger2 = {};

-- Blocking rocks
local blocking_rock1 = {};
local blocking_rock2 = {};
local blocking_rock3 = {};
local blocking_rock4 = {};

function _CreateObjects()
    local object = {}
    local npc = {}
    local event = {}
    local dialogue = {}
    local text = {}

    -- Add a halo showing the cave entrances
    -- exit 1
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 4.5, 29,
        vt_video.Color(0.3, 0.3, 0.46, 0.8));
    -- exit 2
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 52, 22,
        vt_video.Color(0.3, 0.3, 0.46, 0.8));

    -- Treasure box
    local chest = CreateTreasure(Map, "elbrus_cave3_chest1", "Wood_Chest1", 51, 38);
    if (chest ~= nil) then
        chest:AddObject(20011, 1); -- Medium healing potion
        Map:AddGroundObject(chest);
    end

    -- Stones
    object = CreateObject(Map, "Rock1", 33.0, 26.0);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Rock1", 43.0, 28.0);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Rock1", 41.0, 32.0);
    Map:AddGroundObject(object);

    blocking_rock1 = CreateObject(Map, "Rock2", 37, 36);
    Map:AddGroundObject(blocking_rock1);
    blocking_rock2 = CreateObject(Map, "Rock2", 39, 36);
    Map:AddGroundObject(blocking_rock2);
    blocking_rock3 = CreateObject(Map, "Rock2", 44, 40);
    Map:AddGroundObject(blocking_rock3);
    blocking_rock4 = CreateObject(Map, "Rock2", 44, 42);
    Map:AddGroundObject(blocking_rock4);

    -- shroom 1
    shroom1 = CreateSprite(Map, "Shroom", 35, 24);
    shroom1:SetName("");
    shroom1:SetDirection(vt_map.MapMode.SOUTH);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("...");
    dialogue:AddLineEvent(text, shroom1, "", "Fight with Shroom 1");
    DialogueManager:AddDialogue(dialogue);
    shroom1:AddDialogueReference(dialogue);
    Map:AddGroundObject(shroom1);
    event = vt_map.BattleEncounterEvent("Fight with Shroom 1");
    event:AddEnemy(11, 512, 384); -- one shroom
    _SetEventBattleEnvironment(event);
    event:AddEventLinkAtEnd("Place Shroom 1 after fight", 100);
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("Place Shroom 1 after fight", shroom1, "place_shroom_after_fight", "")
    EventManager:RegisterEvent(event);

    -- shroom 2
    shroom2 = CreateSprite(Map, "Shroom", 37, 24);
    shroom2:SetName("");
    shroom2:SetDirection(vt_map.MapMode.SOUTH);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("...");
    dialogue:AddLineEvent(text, shroom2, "", "Fight with Shroom 2");
    DialogueManager:AddDialogue(dialogue);
    shroom2:AddDialogueReference(dialogue);
    Map:AddGroundObject(shroom2);
    event = vt_map.BattleEncounterEvent("Fight with Shroom 2");
    event:AddEnemy(11, 512, 384); -- one shroom
    _SetEventBattleEnvironment(event);
    event:AddEventLinkAtEnd("Place Shroom 2 after fight", 100);
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("Place Shroom 2 after fight", shroom2, "place_shroom_after_fight", "")
    EventManager:RegisterEvent(event);

    -- shroom 3
    shroom3 = CreateSprite(Map, "Shroom", 39, 24);
    shroom3:SetName("");
    shroom3:SetDirection(vt_map.MapMode.SOUTH);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("...");
    dialogue:AddLineEvent(text, shroom3, "", "Fight with Shroom 3");
    DialogueManager:AddDialogue(dialogue);
    shroom3:AddDialogueReference(dialogue);
    Map:AddGroundObject(shroom3);
    event = vt_map.BattleEncounterEvent("Fight with Shroom 3");
    event:AddEnemy(11, 512, 384); -- one shroom
    _SetEventBattleEnvironment(event);
    event:AddEventLinkAtEnd("Place Shroom 3 after fight", 100);
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("Place Shroom 3 after fight", shroom3, "place_shroom_after_fight", "")
    EventManager:RegisterEvent(event);

    -- shroom 4
    shroom4 = CreateSprite(Map, "Shroom", 35, 32);
    shroom4:SetName("");
    shroom4:SetDirection(vt_map.MapMode.SOUTH);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("...");
    dialogue:AddLineEvent(text, shroom4, "", "Fight with Shroom 4");
    DialogueManager:AddDialogue(dialogue);
    shroom4:AddDialogueReference(dialogue);
    Map:AddGroundObject(shroom4);
    event = vt_map.BattleEncounterEvent("Fight with Shroom 4");
    event:AddEnemy(11, 512, 384); -- one shroom
    _SetEventBattleEnvironment(event);
    event:AddEventLinkAtEnd("Place Shroom 4 after fight", 100);
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("Place Shroom 4 after fight", shroom4, "place_shroom_after_fight", "")
    EventManager:RegisterEvent(event);

    -- shroom 5
    shroom5 = CreateSprite(Map, "Shroom", 39, 30);
    shroom5:SetName("");
    shroom5:SetDirection(vt_map.MapMode.SOUTH);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("...");
    dialogue:AddLineEvent(text, shroom5, "", "Fight with Shroom 5");
    DialogueManager:AddDialogue(dialogue);
    shroom5:AddDialogueReference(dialogue);
    Map:AddGroundObject(shroom5);
    event = vt_map.BattleEncounterEvent("Fight with Shroom 5");
    event:AddEnemy(11, 512, 384); -- one shroom
    _SetEventBattleEnvironment(event);
    event:AddEventLinkAtEnd("Place Shroom 5 after fight", 100);
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("Place Shroom 5 after fight", shroom5, "place_shroom_after_fight", "")
    EventManager:RegisterEvent(event);

    stone_trigger1 = vt_map.TriggerObject("mt elbrus cave 3 trigger 1",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Check trigger1");
    stone_trigger1:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger1:SetPosition(33, 32);
    stone_trigger1:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger1);

    event = vt_map.ScriptedEvent("Check trigger1", "check_trigger1", "")
    EventManager:RegisterEvent(event);

    stone_trigger2 = vt_map.TriggerObject("mt elbrus cave 3 trigger 2",
                             "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                             "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                             "",
                             "Check trigger2");
    stone_trigger2:SetObjectID(Map.object_supervisor:GenerateObjectID());
    stone_trigger2:SetPosition(41, 20);
    stone_trigger2:SetTriggerableByCharacter(false); -- Only an event can trigger it
    Map:AddFlatGroundObject(stone_trigger2);

    event = vt_map.ScriptedEvent("Check trigger2", "check_trigger2", "")
    EventManager:RegisterEvent(event);

    -- Check the trigger states at load time (but without sound)
    if (stone_trigger1:GetState() == true) then
        -- Free the way
        blocking_rock1:SetVisible(false);
        blocking_rock1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        blocking_rock2:SetVisible(false);
        blocking_rock2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
    if (stone_trigger2:GetState() == true) then
        -- Free the way
        blocking_rock3:SetVisible(false);
        blocking_rock3:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        blocking_rock4:SetVisible(false);
        blocking_rock4:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    rolling_stone1 = CreateObject(Map, "Rolling Stone", 37, 26);
    Map:AddGroundObject(rolling_stone1);
    rolling_stone1:SetEventWhenTalking("Push the rolling stone 1");

    event = vt_map.ScriptedEvent("Push the rolling stone 1", "start_to_move_the_stone1", "move_the_stone_update1")
    EventManager:RegisterEvent(event);

    rolling_stone2 = CreateObject(Map, "Rolling Stone", 35, 30);
    Map:AddGroundObject(rolling_stone2);
    rolling_stone2:SetEventWhenTalking("Push the rolling stone 2");

    event = vt_map.ScriptedEvent("Push the rolling stone 2", "start_to_move_the_stone2", "move_the_stone_update2")
    EventManager:RegisterEvent(event);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to exit 3-1", "dat/maps/mt_elbrus/mt_elbrus_path2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path2_script.lua", "from_grotto3_1_exit");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to exit 3-2", "dat/maps/mt_elbrus/mt_elbrus_path2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path2_script.lua", "from_grotto3_2_exit");
    EventManager:RegisterEvent(event);
end

-- zones
local exit3_1_zone = {};
local exit3_2_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    exit3_1_zone = vt_map.CameraZone(46, 58, 16, 18);
    Map:AddZone(exit3_1_zone);
    exit3_2_zone = vt_map.CameraZone(0, 8, 22, 24);
    Map:AddZone(exit3_2_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (exit3_1_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to exit 3-1");
    elseif (exit3_2_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to exit 3-2");
    end
end

function _CheckStoneAndTriggersCollision()
    -- Check first trigger
    if (stone_trigger1:GetState() == false) then
        if (stone_trigger1:IsCollidingWith(rolling_stone1) == true
                or stone_trigger1:IsCollidingWith(rolling_stone2) == true) then
            stone_trigger1:SetState(true)
        end
    end
    -- Check second trigger
    if (stone_trigger2:GetState() == false) then
        if (stone_trigger2:IsCollidingWith(rolling_stone1) == true
                or stone_trigger2:IsCollidingWith(rolling_stone2) == true) then
            stone_trigger2:SetState(true)
        end
    end
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

-- Map Custom functions
-- Used through scripted events

local stone_direction1 = vt_map.MapMode.EAST;
local stone_direction2 = vt_map.MapMode.EAST;

map_functions = {

    add_scene_state = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

    remove_scene_state = function()
        Map:PopState();
    end,

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

        -- Only place the shroom when nothing is in the way.
        if (shroom:IsColliding(shroom_new_x, shroom_new_y) == false) then
            shroom:SetPosition(shroom_new_x, shroom_new_y);
        end

        -- Place the shroom
        shroom:SetCustomAnimation("mushroom_ko", 0); -- 0 means forever
        -- Remove its dialogue (preventing a new fight)
        shroom:ClearDialogueReferences();
    end,

    start_to_move_the_stone1 = function()

        local hero_x = hero:GetXPosition();
        local hero_y = hero:GetYPosition();

        local stone_x = rolling_stone1:GetXPosition();
        local stone_y = rolling_stone1:GetYPosition();

        -- Set the stone direction

        -- Determine the hero position relative to the stone
        if (hero_y > stone_y + 0.3) then
            -- the hero is below, the stone is pushed upward.
            stone_direction1 = vt_map.MapMode.NORTH;
        elseif (hero_y < stone_y - 1.5) then
            -- the hero is above, the stone is pushed downward.
            stone_direction1 = vt_map.MapMode.SOUTH;
        elseif (hero_x < stone_x - 1.2) then
            -- the hero is on the left, the stone is pushed to the right.
            stone_direction1 = vt_map.MapMode.EAST;
        elseif (hero_x > stone_x + 1.2) then
            -- the hero is on the right, the stone is pushed to the left.
            stone_direction1 = vt_map.MapMode.WEST;
        end

        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    start_to_move_the_stone2 = function()

        local hero_x = hero:GetXPosition();
        local hero_y = hero:GetYPosition();

        local stone_x = rolling_stone2:GetXPosition();
        local stone_y = rolling_stone2:GetYPosition();

        -- Set the stone direction

        -- Determine the hero position relative to the stone
        if (hero_y > stone_y + 0.3) then
            -- the hero is below, the stone is pushed upward.
            stone_direction2 = vt_map.MapMode.NORTH;
        elseif (hero_y < stone_y - 1.5) then
            -- the hero is above, the stone is pushed downward.
            stone_direction2 = vt_map.MapMode.SOUTH;
        elseif (hero_x < stone_x - 1.2) then
            -- the hero is on the left, the stone is pushed to the right.
            stone_direction2 = vt_map.MapMode.EAST;
        elseif (hero_x > stone_x + 1.2) then
            -- the hero is on the right, the stone is pushed to the left.
            stone_direction2 = vt_map.MapMode.WEST;
        end

        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    move_the_stone_update1 = function()
        return _UpdateStoneMovement(rolling_stone1, stone_direction1)
    end,

    move_the_stone_update2 = function()
        return _UpdateStoneMovement(rolling_stone2, stone_direction2)
    end,

    -- Check whether triggers are activated and then free the way accordingly.
    check_trigger1 = function()
        if (stone_trigger1:GetState() == true) then
            -- Free the way
            blocking_rock1:SetVisible(false);
            blocking_rock1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            blocking_rock2:SetVisible(false);
            blocking_rock2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            AudioManager:PlaySound("snd/cave-in.ogg");
        end
    end,

    check_trigger2 = function()
        if (stone_trigger2:GetState() == true) then
            -- Free the way
            blocking_rock3:SetVisible(false);
            blocking_rock3:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            blocking_rock4:SetVisible(false);
            blocking_rock4:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            AudioManager:PlaySound("snd/cave-in.ogg");
        end
    end,
}
