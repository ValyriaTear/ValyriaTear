-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_cave1_2_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest Cave"
map_image_filename = "img/menus/locations/desert_cave.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/shrine-OGA-yd.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- the main character handler
local hero = {};

-- Forest dialogue secondary hero
local kalya_sprite = {};

-- Name of the main sprite. Used to reload the good one at the end of the first forest entrance event.
local main_sprite_name = "";

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

    Map.unlimited_stamina = false;
    Map:ShowMinimap(true);

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
    -- Add the background and foreground animations
    Map:GetScriptSupervisor():AddScript("dat/maps/layna_forest/layna_forest_caves_background_anim.lua");

    -- The script file which will handle the display of the stone sign image.
    Map:GetScriptSupervisor():AddScript("dat/maps/layna_forest/layna_forest_cave1_2_stone_sign_image.lua");
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();

    -- Check whether the monsters have been defeated
    _CheckMonstersStates();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 3, 15);
    hero:SetDirection(hoa_map.MapMode.EAST);
    hero:SetMovementSpeed(hoa_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from forest SE") then
        hero:SetDirection(hoa_map.MapMode.NORTH);
        hero:SetPosition(112, 94);
    elseif (GlobalManager:GetPreviousLocation() == "from_layna_wolf_cave") then
        hero:SetDirection(hoa_map.MapMode.WEST);
        hero:SetPosition(122, 15);
    end

    Map:AddGroundObject(hero);

    -- Create secondary character for dialogue at map entrance
    kalya_sprite = CreateSprite(Map, "Kalya",
                                hero:GetXPosition(), hero:GetYPosition());

    kalya_sprite:SetDirection(hoa_map.MapMode.NORTH);
    kalya_sprite:SetMovementSpeed(hoa_map.MapMode.NORMAL_SPEED);
    kalya_sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    kalya_sprite:SetVisible(false);
    Map:AddGroundObject(kalya_sprite);
end

-- Special object blocking the exit
local blocking_rock = {};

-- Special NPC used a sign
local stone_sign = {};

function _CreateObjects()
    local object = {};
    local npc = {};
    local event = {}

    -- Treasure chest, accessible later from the next cave.
    local chest1 = CreateTreasure(Map, "layna_forest_cave1_2_chest", "Wood_Chest1", 116, 38);
    if (chest1 ~= nil) then
        chest1:SetDrunes(50);
        Map:AddGroundObject(chest1);
    end

    -- Adapt the light color according to the time of the day.
    local light_color_red = 1.0;
    local light_color_green = 1.0;
    local light_color_blue = 1.0;
    local light_color_alpha = 0.8;
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        local tw_value = GlobalManager:GetEventValue("story", "layna_forest_twilight_value");
        if (tw_value >= 4 and tw_value < 6) then
            light_color_red = 0.83;
            light_color_green = 0.72;
            light_color_blue = 0.70;
            light_color_alpha = 0.29;
        elseif (tw_value >= 6 and tw_value < 8) then
            light_color_red = 0.62;
            light_color_green = 0.50;
            light_color_blue = 0.59;
            light_color_alpha = 0.49;
        elseif (tw_value >= 8) then
            light_color_red = 0.30;
            light_color_green = 0.30;
            light_color_blue = 0.46;
            light_color_alpha = 0.60;
        end
    end

    -- Add a halo showing the cave entrances
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 113, 109,
            hoa_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha), hoa_map.MapMode.CONTEXT_01);
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 0, 18,
            hoa_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha), hoa_map.MapMode.CONTEXT_01);

    -- Add different halo light, representing holes of light coming from the ceiling
    Map:AddHalo("img/misc/lights/right_ray_light.lua", 23, 17,
            hoa_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha), hoa_map.MapMode.CONTEXT_01);
    Map:AddHalo("img/misc/lights/right_ray_light.lua", 15, 50,
            hoa_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha), hoa_map.MapMode.CONTEXT_01);
    Map:AddHalo("img/misc/lights/right_ray_light.lua", 37, 84,
            hoa_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha), hoa_map.MapMode.CONTEXT_01);

    -- Create the stone sign telling what to do to pass the test...
    object = CreateObject(Map, "Stone Sign1", 22, 10);
    Map:AddGroundObject(object);
    -- Create an invisible sprite, used to handle the dialogue
    stone_sign = CreateSprite(Map, "Butterfly", 22, 11);
    stone_sign:SetName("Stone sign");
    stone_sign:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    stone_sign:SetVisible(false);
    Map:AddGroundObject(stone_sign);

    _UpdateStoneSignDialogue();

    -- Decorations
    object = CreateObject(Map, "Rock1", 107, 96);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Rock1", 117, 96);
    Map:AddGroundObject(object);

    -- The blocking rock
    blocking_rock = CreateObject(Map, "Rock3", 112, 96);
    Map:AddGroundObject(blocking_rock);

    -- Remove the block if all enemies have already been defeated
    if (GlobalManager:DoesEventExist("story", "layna_forest_cave2_monsters_defeated")) then
        blocking_rock:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
        blocking_rock:SetVisible(false);
    end
end

function _UpdateStoneSignDialogue()
    local dialogue = {};
    local text = {};

    stone_sign:ClearDialogueReferences();

    if (GlobalManager:DoesEventExist("story", "kalya_stone_sign_dialogue_done")) then
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("Only the last one standing shall pass ...");
        dialogue:AddLine(text, stone_sign);
        DialogueManager:AddDialogue(dialogue);
        stone_sign:AddDialogueReference(dialogue);
    else
        -- Start the stone sign dialogue event
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("...");
        dialogue:AddLineEvent(text, kalya_sprite, "Start dialogue about stone sign", "Display the stone sign image");
        DialogueManager:AddDialogue(dialogue);
        stone_sign:AddDialogueReference(dialogue);
    end
end

-- Special event references which destinations must be updated just before being called.
local move_next_to_hero_event = {}
local move_back_to_hero_event = {}

local move_next_to_hero_event2 = {}
local move_back_to_hero_event2 = {}

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- Map transition events
    event = hoa_map.MapTransitionEvent("to cave 1-1", "dat/maps/layna_forest/layna_forest_cave1_1.lua",
                                       "dat/maps/layna_forest/layna_forest_cave1_1.lua", "from_layna_cave_1_2");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to south east exit", "dat/maps/layna_forest/layna_forest_south_east.lua",
                                       "dat/maps/layna_forest/layna_forest_south_east.lua", "from_layna_cave_1_2");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to wolf cave", "dat/maps/layna_forest/layna_forest_wolf_cave_map.lua",
                                       "dat/maps/layna_forest/layna_forest_wolf_cave_script.lua",  "from_layna_cave_1_2");
    EventManager:RegisterEvent(event);

    -- Generic events
    event = hoa_map.ChangeDirectionSpriteEvent("Kalya looks north", kalya_sprite, hoa_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Bronann looks north", hero, hoa_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = hoa_map.LookAtSpriteEvent("Kalya looks at Bronann", kalya_sprite, hero);
    EventManager:RegisterEvent(event);
    event = hoa_map.LookAtSpriteEvent("Bronann looks at Kalya", hero, kalya_sprite);
    EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedSpriteEvent("kalya_sprite:SetCollision(NONE)", kalya_sprite, "Sprite_Collision_off", "");
    EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedSpriteEvent("kalya_sprite:SetCollision(ALL)", kalya_sprite, "Sprite_Collision_on", "");
    EventManager:RegisterEvent(event);

    -- Dialogue
    event = hoa_map.ScriptedEvent("Start dialogue about stone sign", "stone_sign_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann");
    EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event = hoa_map.PathMoveSpriteEvent("Kalya moves next to Bronann", kalya_sprite, 0, 0, false);
    move_next_to_hero_event:AddEventLinkAtEnd("kalya_sprite:SetCollision(ALL)");
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya looks north");
    EventManager:RegisterEvent(move_next_to_hero_event);

    event = hoa_map.ScriptedEvent("Display the stone sign image", "stone_sign_image_start", "stone_sign_image_update")
    event:AddEventLinkAtEnd("Kalya reads the scripture");
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("'Only the last one standing shall pass.' ...");
    dialogue:AddLineEventEmote(text, kalya_sprite, "", "Bronann looks at Kalya", "thinking dots");
    text = hoa_system.Translate("You are able to decipher this writing, Kalya?");
    dialogue:AddLineEmote(text, hero, "thinking dots");
    text = hoa_system.Translate("Somehow... I don't know why, but yes ...");
    dialogue:AddLineEvent(text, kalya_sprite, "Kalya looks at Bronann", "Bronann looks north");
    text = hoa_system.Translate("Still, I don't know what it means, though.");
    dialogue:AddLineEmote(text, hero, "thinking dots");
    text = hoa_system.Translate("Let's look around. We might find out.");
    dialogue:AddLine(text, kalya_sprite);
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Kalya reads the scripture", dialogue);
    event:AddEventLinkAtEnd("kalya_sprite:SetCollision(NONE)");
    event:AddEventLinkAtEnd("kalya goes back to party");
    EventManager:RegisterEvent(event);

    move_back_to_hero_event = hoa_map.PathMoveSpriteEvent("kalya goes back to party", kalya_sprite, hero, false);
    move_back_to_hero_event:AddEventLinkAtEnd("end of stone sign dialogue");
    EventManager:RegisterEvent(move_back_to_hero_event);

    event = hoa_map.ScriptedEvent("end of stone sign dialogue", "end_of_stone_sign_dialogue", "");
    event:AddEventLinkAtEnd("Bronann looks north");
    EventManager:RegisterEvent(event);


    -- Dialogue when all the enemies are dead.
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("... Something heavy seems to have fallen nearby.");
    dialogue:AddLineEmote(text, hero, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Hero dialogue during tremor", dialogue);
    EventManager:RegisterEvent(event);


    -- Kalya sees the mechanisms to go out.
    event = hoa_map.ScriptedEvent("Kalya sees the cave exit", "see_the_exit_dialogue_start", "");
    event:AddEventLinkAtEnd("Exit seen: Kalya moves next to Bronann");
    EventManager:RegisterEvent(event);

    -- NOTE the actual coords will be set at event start.
    move_next_to_hero_event2 = hoa_map.PathMoveSpriteEvent("Exit seen: Kalya moves next to Bronann", kalya_sprite, 0, 0, false);
    move_next_to_hero_event2:AddEventLinkAtEnd("Kalya looks at the rock");
    move_next_to_hero_event2:AddEventLinkAtEnd("Kalya says 'Look!'");
    EventManager:RegisterEvent(move_next_to_hero_event2);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("... Look!");
    dialogue:AddLineEmote(text, kalya_sprite, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Kalya says 'Look!'", dialogue);
    event:AddEventLinkAtEnd("Set_Camera(kalya_sprite)");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedSpriteEvent("Set_Camera(kalya_sprite)", kalya_sprite, "Set_Camera", "");
    event:AddEventLinkAtEnd("Kalya comes close to the rock");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Kalya comes close to the rock", kalya_sprite, 112, 89, false);
    event:AddEventLinkAtEnd("Kalya looks at the rock");
    event:AddEventLinkAtEnd("Bronann looks at Kalya");
    event:AddEventLinkAtEnd("Kalya tells there is an exit behind the rock");
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("I can feel fresh air and some light coming from around this rock.");
    dialogue:AddLine(text, kalya_sprite);
    text = hoa_system.Translate("There are signs engraved on this rock, too. This sounds like some kind of mechanism my family used long ago...");
    dialogue:AddLineEvent(text, kalya_sprite, "Kalya looks at Bronann", "");
    text = hoa_system.Translate("Your family? Long ago? ...");
    dialogue:AddLineEmote(text, hero, "interrogation");
    text = hoa_system.Translate("Err... Nevermind that, we must find a way to make this move. Let's look around.");
    dialogue:AddLineEmote(text, kalya_sprite, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Kalya tells there is an exit behind the rock", dialogue);
    event:AddEventLinkAtEnd("Set_Camera(hero)");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedSpriteEvent("Set_Camera(hero)", hero, "Set_Camera", "");
    event:AddEventLinkAtEnd("Exit seen: kalya goes back to party");
    EventManager:RegisterEvent(event);

    move_back_to_hero_event2 = hoa_map.PathMoveSpriteEvent("Exit seen: kalya goes back to party", kalya_sprite, hero, false);
    move_back_to_hero_event2:AddEventLinkAtEnd("end of exit seen dialogue");
    EventManager:RegisterEvent(move_back_to_hero_event2);

    event = hoa_map.ScriptedEvent("end of exit seen dialogue", "end_of_exit_seen_dialogue", "");
    event:AddEventLinkAtEnd("Bronann looks south");
    EventManager:RegisterEvent(event);

    event = hoa_map.LookAtSpriteEvent("Kalya looks at the rock", kalya_sprite, 112, 92);
    EventManager:RegisterEvent(event);
end

-- local members used to know whether the monsters have been defeated.
local monster1_defeated = false;
local monster2_defeated = false;
local monster3_defeated = false;
local monster4_defeated = false;
local monsters_defeated = false;

local roam_zone1 = {};
local roam_zone2 = {};
local roam_zone3 = {};
local roam_zone4 = {};

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

    if (GlobalManager:DoesEventExist("story", "layna_forest_cave2_monsters_defeated")) then
        monsters_defeated = true;
    end

    -- Monsters that can only be beaten once
    -- Hint: left, right, top, bottom
    roam_zone1 = hoa_map.EnemyZone(26, 30, 43, 50, hoa_map.MapMode.CONTEXT_01);
    if (monsters_defeated == false) then
        enemy = CreateEnemySprite(Map, "slime");
        _SetBattleEnvironment(enemy);
        enemy:NewEnemyParty();
        enemy:AddEnemy(1);
        enemy:AddEnemy(1);
        enemy:AddEnemy(1);
        enemy:AddEnemy(1);
        enemy:AddEnemy(1);
        roam_zone1:AddEnemy(enemy, Map, 1);
        roam_zone1:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    end
    Map:AddZone(roam_zone1);

    -- Hint: left, right, top, bottom
    roam_zone2 = hoa_map.EnemyZone(35, 43, 13, 20, hoa_map.MapMode.CONTEXT_01);
    if (monsters_defeated == false) then
        enemy = CreateEnemySprite(Map, "slime");
        _SetBattleEnvironment(enemy);
        enemy:NewEnemyParty();
        enemy:AddEnemy(1);
        enemy:AddEnemy(2);
        enemy:AddEnemy(1);
        enemy:AddEnemy(2);
        enemy:AddEnemy(1);
        roam_zone2:AddEnemy(enemy, Map, 1);
        roam_zone2:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    end
    Map:AddZone(roam_zone2);

    roam_zone3 = hoa_map.EnemyZone(61, 67, 40, 48, hoa_map.MapMode.CONTEXT_01);
    if (monsters_defeated == false) then
        enemy = CreateEnemySprite(Map, "slime");
        _SetBattleEnvironment(enemy);
        enemy:NewEnemyParty();
        enemy:AddEnemy(1);
        enemy:AddEnemy(2);
        enemy:AddEnemy(2);
        enemy:AddEnemy(2);
        enemy:AddEnemy(1);
        roam_zone3:AddEnemy(enemy, Map, 1);
        roam_zone3:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    end
    Map:AddZone(roam_zone3);

    roam_zone4 = hoa_map.EnemyZone(89, 100, 12, 15, hoa_map.MapMode.CONTEXT_01);
    if (monsters_defeated == false) then
        enemy = CreateEnemySprite(Map, "bat");
        _SetBattleEnvironment(enemy);
        enemy:NewEnemyParty();
        enemy:AddEnemy(6);
        enemy:AddEnemy(2);
        enemy:AddEnemy(6);
        enemy:AddEnemy(2);
        enemy:AddEnemy(1);
        roam_zone4:AddEnemy(enemy, Map, 1);
        roam_zone4:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    end
    Map:AddZone(roam_zone4);
end

-- check whether all the monsters dies, to open the door
function _CheckMonstersStates()
    if (monster1_defeated == false and roam_zone1:GetSpawnsLeft() == 0) then
        monster1_defeated = true;
    end

    if (monster2_defeated == false and roam_zone2:GetSpawnsLeft() == 0) then
        monster2_defeated = true;
    end

    if (monster3_defeated == false and roam_zone3:GetSpawnsLeft() == 0) then
        monster3_defeated = true;
    end

    if (monster4_defeated == false and roam_zone4:GetSpawnsLeft() == 0) then
        monster4_defeated = true;
    end

    -- Open the door when every monster is defeated, and set the event has done.
    if (monsters_defeated == false
            and monster1_defeated and monster2_defeated
            and monster3_defeated and monster4_defeated) then
        monsters_defeated = true;

        blocking_rock:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
        blocking_rock:SetVisible(false);
        AudioManager:PlaySound("snd/cave-in.ogg");
        VideoManager:ShakeScreen(0.6, 1000, hoa_video.GameVideo.VIDEO_FALLOFF_GRADUAL);

        hero:SetMoving(false);
        -- Trigger the dialogue event about the shaking...
        EventManager:StartEvent("Hero dialogue during tremor");

        GlobalManager:SetEventValue("story", "layna_forest_cave2_monsters_defeated", 1);
    end

end

-- zones
local to_cave_1_1 = {};
local to_cave_exit = {};
local to_wolf_cave_zone = {};
local seeing_the_exit_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_cave_1_1_zone = hoa_map.CameraZone(0, 1, 11, 16, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(to_cave_1_1_zone);

    to_cave_exit_zone = hoa_map.CameraZone(108, 116, 95, 96, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(to_cave_exit_zone);

    to_wolf_cave_zone = hoa_map.CameraZone(123, 124, 12, 13, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(to_wolf_cave_zone);

    seeing_the_exit_zone = hoa_map.CameraZone(99, 122, 80, 96, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(seeing_the_exit_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_cave_1_1_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to cave 1-1");
    elseif (to_cave_exit_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to south east exit");
    elseif (to_wolf_cave_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to wolf cave");
    elseif (monsters_defeated == false
            and GlobalManager:GetEventValue("story", "kalya_layna_forest_cave1_2_exit_dialogue_done") == 0) then
        if (seeing_the_exit_zone:IsCameraEntering() == true) then
            EventManager:StartEvent("Kalya sees the cave exit");
        end
    end
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    enemy:AddBattleScript("dat/battles/desert_cave_battle_anim.lua");
end

-- Map Custom functions
-- Used through scripted events
map_functions = {
    Sprite_Collision_on = function(sprite)
        if (sprite ~= nil) then
            sprite:SetCollisionMask(hoa_map.MapMode.ALL_COLLISION);
        end
    end,

    Sprite_Collision_off = function(sprite)
        if (sprite ~= nil) then
            sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
        end
    end,

    Set_Camera = function(sprite)
        if (sprite ~= nil) then Map:SetCamera(sprite, 800); end
    end,

    -- Kalya and Bronann read the engraved text
    stone_sign_dialogue_start = function()
        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya_sprite:SetVisible(true);
        kalya_sprite:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        hero:SetCollisionMask(hoa_map.MapMode.ALL_COLLISION);
        hero:SetDirection(hoa_map.MapMode.NORTH);
        kalya_sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);

        move_next_to_hero_event:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition(), false);
    end,

    stone_sign_image_start = function()
        -- Trigger the display of the image.
        GlobalManager:SetEventValue("story", "layna_forest_cave1_2_show_sign_image", 1)
    end,

    -- Returns true when the image has finished to display.
    stone_sign_image_update = function()
        if (GlobalManager:GetEventValue("story", "layna_forest_cave1_2_show_sign_image") == 0) then
            return true;
        end
        return false;
    end,

    end_of_stone_sign_dialogue = function()
        Map:PopState();
        kalya_sprite:SetPosition(0, 0);
        kalya_sprite:SetVisible(false);
        kalya_sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);

        -- Set event as done
        GlobalManager:SetEventValue("story", "kalya_stone_sign_dialogue_done", 1);
        _UpdateStoneSignDialogue();
    end,

    -- Kalya and Bronann see the exit
    see_the_exit_dialogue_start = function()
        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya_sprite:SetVisible(true);
        kalya_sprite:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        hero:SetCollisionMask(hoa_map.MapMode.ALL_COLLISION);
        hero:SetDirection(hoa_map.MapMode.SOUTH);
        kalya_sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);

        move_next_to_hero_event2:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition(), false);
    end,

    end_of_exit_seen_dialogue = function()
        kalya_sprite:SetPosition(0, 0);
        kalya_sprite:SetVisible(false);
        kalya_sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);

        -- Set event as done
        GlobalManager:SetEventValue("story", "kalya_layna_forest_cave1_2_exit_dialogue_done", 1);
        Map:PopState();
    end

}
