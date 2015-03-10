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
music_filename = "music/shrine-OGA-yd.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Effects = nil

-- the main character handler
local hero = nil

-- Dialogues sprites
local bronann = nil
local kalya = nil

-- the main map loading code
function Load(m)

    Map = m;
    Effects = Map:GetEffectSupervisor();
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(false);

    Map:SetMinimapImage("dat/maps/layna_forest/minimaps/layna_forest_cave1_2_minimap.png");

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
    hero = CreateSprite(Map, "Bronann", 3, 15, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.EAST);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from forest SE") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(112, 94);
    elseif (GlobalManager:GetPreviousLocation() == "from_layna_wolf_cave") then
        hero:SetDirection(vt_map.MapMode.WEST);
        hero:SetPosition(122, 15);
    end

    bronann = CreateSprite(Map, "Bronann", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.NORTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    bronann:SetVisible(false);

    kalya = CreateSprite(Map, "Kalya", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.NORTH);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);
end

-- Special object blocking the exit
local blocking_rock = nil

-- Special NPC used a sign
local stone_sign = nil

function _CreateObjects()
    local object = nil
    local npc = nil
    local event = nil

    -- Treasure chest, accessible later from the next cave.
    local chest1 = CreateTreasure(Map, "layna_forest_cave1_2_chest", "Wood_Chest1", 116, 38, vt_map.MapMode.GROUND_OBJECT);
    chest1:SetDrunes(50);
    chest1:AddItem(16, 1); -- Candy

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
    vt_map.Halo.Create("img/misc/lights/torch_light_mask.lua", 113, 109,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));
    vt_map.Halo.Create("img/misc/lights/torch_light_mask.lua", 0, 18,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));

    -- Add different halo light, representing holes of light coming from the ceiling
    vt_map.Halo.Create("img/misc/lights/right_ray_light.lua", 23, 17,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));
    vt_map.Halo.Create("img/misc/lights/right_ray_light.lua", 15, 50,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));
    vt_map.Halo.Create("img/misc/lights/right_ray_light.lua", 37, 84,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));

    -- Create the stone sign telling what to do to pass the test...
    CreateObject(Map, "Stone Sign1", 22, 10, vt_map.MapMode.GROUND_OBJECT);

    -- Create an invisible sprite, used to handle the dialogue
    stone_sign = CreateSprite(Map, "Butterfly", 22, 11, vt_map.MapMode.GROUND_OBJECT);
    stone_sign:SetName(vt_system.Translate("Stone sign"));
    stone_sign:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    stone_sign:SetVisible(false);

    _UpdateStoneSignDialogue();

    -- Decorations
    CreateObject(Map, "Rock1", 107, 96, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 117, 96, vt_map.MapMode.GROUND_OBJECT);

    -- The blocking rock
    blocking_rock = CreateObject(Map, "Rock3", 112, 96, vt_map.MapMode.GROUND_OBJECT);

    -- Remove the block if all enemies have already been defeated
    if (GlobalManager:DoesEventExist("story", "layna_forest_cave2_monsters_defeated")) then
        blocking_rock:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        blocking_rock:SetVisible(false);
    end
end

function _UpdateStoneSignDialogue()
    local dialogue = nil
    local text = nil

    stone_sign:ClearDialogueReferences();

    if (GlobalManager:DoesEventExist("story", "kalya_stone_sign_dialogue_done")) then
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Only the last one standing shall pass.");
        dialogue:AddLine(text, stone_sign);
        stone_sign:AddDialogueReference(dialogue);
    else
        -- Start the stone sign dialogue event
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("...");
        dialogue:AddLineEvent(text, kalya, "Start dialogue about stone sign", "Display the stone sign image");
        stone_sign:AddDialogueReference(dialogue);
    end
end

-- Special event references which destinations must be updated just before being called.
local move_next_to_bronann_event = nil
local move_next_to_bronann_event2 = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Map transition events
    vt_map.MapTransitionEvent.Create("to cave 1-1", "dat/maps/layna_forest/layna_forest_cave1_1_map.lua",
                                     "dat/maps/layna_forest/layna_forest_cave1_1_script.lua", "from_layna_cave_1_2");

    vt_map.MapTransitionEvent.Create("to south east exit", "dat/maps/layna_forest/layna_forest_south_east_map.lua",
                                     "dat/maps/layna_forest/layna_forest_south_east_script.lua", "from_layna_cave_1_2");

    vt_map.MapTransitionEvent.Create("to wolf cave", "dat/maps/layna_forest/layna_forest_wolf_cave_map.lua",
                                     "dat/maps/layna_forest/layna_forest_wolf_cave_script.lua",  "from_layna_cave_1_2");

    -- Generic events
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks north", bronann, vt_map.MapMode.NORTH);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Kalya", bronann, kalya);

    vt_map.ScriptedSpriteEvent.Create("kalya:SetCollision(NONE)", kalya, "Sprite_Collision_off", "");
    vt_map.ScriptedSpriteEvent.Create("kalya:SetCollision(ALL)", kalya, "Sprite_Collision_on", "");

    -- Dialogue
    event = vt_map.ScriptedEvent.Create("Start dialogue about stone sign", "stone_sign_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann");

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_bronann_event = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann", kalya, 0, 0, false);
    move_next_to_bronann_event:AddEventLinkAtEnd("kalya:SetCollision(ALL)");
    move_next_to_bronann_event:AddEventLinkAtEnd("Kalya looks north");

    event = vt_map.ScriptedEvent.Create("Display the stone sign image", "stone_sign_image_start", "stone_sign_image_update")
    event:AddEventLinkAtEnd("Kalya reads the scripture");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("'Only the last one standing shall pass.'");
    dialogue:AddLineEventEmote(text, kalya, "", "Bronann looks at Kalya", "thinking dots");
    text = vt_system.Translate("You are able to decipher this writing, Kalya?");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("Somehow. I'm not sure how I am able to, but I can read it.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "Bronann looks north");
    text = vt_system.Translate("However, I don't know what it means.");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("Let's look around. We might find out.");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Kalya reads the scripture", dialogue);
    event:AddEventLinkAtEnd("kalya:SetCollision(NONE)");
    event:AddEventLinkAtEnd("kalya goes back to party");

    event = vt_map.PathMoveSpriteEvent.Create("kalya goes back to party", kalya, bronann, false);
    event:AddEventLinkAtEnd("end of stone sign dialogue");

    event = vt_map.ScriptedEvent.Create("end of stone sign dialogue", "end_of_stone_sign_dialogue", "");
    event:AddEventLinkAtEnd("Bronann looks north");

    -- Dialogue when all the enemies are dead.
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Something heavy seems to have fallen nearby.");
    dialogue:AddLineEmote(text, hero, "exclamation");
    vt_map.DialogueEvent.Create("Hero dialogue during tremor", dialogue);

    -- Kalya sees the mechanisms to go out.
    event = vt_map.ScriptedEvent.Create("Kalya sees the cave exit", "see_the_exit_dialogue_start", "");
    event:AddEventLinkAtEnd("Exit seen: Kalya moves next to Bronann");

    -- NOTE the actual coords will be set at event start.
    move_next_to_bronann_event2 = vt_map.PathMoveSpriteEvent.Create("Exit seen: Kalya moves next to Bronann", kalya, 0, 0, false);
    move_next_to_bronann_event2:AddEventLinkAtEnd("Kalya says 'Look!'");

    vt_map.LookAtSpriteEvent.Create("Kalya looks at the rock", kalya, 112, 92);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Look!");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at the rock", "", "exclamation");
    event = vt_map.DialogueEvent.Create("Kalya says 'Look!'", dialogue);
    event:AddEventLinkAtEnd("Set_Camera(kalya)");

    event = vt_map.ScriptedSpriteEvent.Create("Set_Camera(kalya)", kalya, "Set_Camera", "");
    event:AddEventLinkAtEnd("Kalya comes close to the rock");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya comes close to the rock", kalya, 112, 89, false);
    event:AddEventLinkAtEnd("Kalya looks at the rock");
    event:AddEventLinkAtEnd("Bronann looks at Kalya");
    event:AddEventLinkAtEnd("Kalya tells there is an exit behind the rock");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I can feel a breeze and see light entering the cave from around this rock. Orlinn must have gone through here.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("There are signs engraved on this rock, too. This looks like some kind of mechanism my family used long ago.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "");
    text = vt_system.Translate("Your family? Long ago?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    text = vt_system.Translate("Err, nevermind. We must find a way to make this move. Let's look around.");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    event = vt_map.DialogueEvent.Create("Kalya tells there is an exit behind the rock", dialogue);
    event:AddEventLinkAtEnd("Set_Camera(bronann)");

    event = vt_map.ScriptedSpriteEvent.Create("Set_Camera(bronann)", bronann, "Set_Camera", "");
    event:AddEventLinkAtEnd("Exit seen: kalya goes back to party");

    event = vt_map.PathMoveSpriteEvent.Create("Exit seen: kalya goes back to party", kalya, bronann, false);
    event:AddEventLinkAtEnd("end of exit seen dialogue");

    event = vt_map.ScriptedEvent.Create("end of exit seen dialogue", "end_of_exit_seen_dialogue", "");
    event:AddEventLinkAtEnd("Bronann looks south");
end

-- local members used to know whether the monsters have been defeated.
local monster1_defeated = false;
local monster2_defeated = false;
local monster3_defeated = false;
local monster4_defeated = false;
local monsters_defeated = false;

local roam_zone1 = nil
local roam_zone2 = nil
local roam_zone3 = nil
local roam_zone4 = nil

function _CreateEnemies()
    local enemy = nil

    if (GlobalManager:DoesEventExist("story", "layna_forest_cave2_monsters_defeated")) then
        monsters_defeated = true;
    end

    -- Monsters that can only be beaten once
    -- Hint: left, right, top, bottom
    roam_zone1 = vt_map.EnemyZone.Create(26, 30, 43, 50);
    if (monsters_defeated == false) then
        enemy = CreateEnemySprite(Map, "slime");
        _SetBattleEnvironment(enemy);
        enemy:NewEnemyParty();
        enemy:AddEnemy(1);
        enemy:AddEnemy(1);
        enemy:AddEnemy(1);
        enemy:AddEnemy(1);
        enemy:AddEnemy(1);
        roam_zone1:AddEnemy(enemy, 1);
        roam_zone1:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    end

    -- Hint: left, right, top, bottom
    roam_zone2 = vt_map.EnemyZone.Create(35, 43, 13, 20);
    if (monsters_defeated == false) then
        enemy = CreateEnemySprite(Map, "slime");
        _SetBattleEnvironment(enemy);
        enemy:NewEnemyParty();
        enemy:AddEnemy(1);
        enemy:AddEnemy(2);
        enemy:AddEnemy(1);
        enemy:AddEnemy(2);
        enemy:AddEnemy(1);
        roam_zone2:AddEnemy(enemy, 1);
        roam_zone2:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    end

    roam_zone3 = vt_map.EnemyZone.Create(61, 67, 40, 48);
    if (monsters_defeated == false) then
        enemy = CreateEnemySprite(Map, "slime");
        _SetBattleEnvironment(enemy);
        enemy:NewEnemyParty();
        enemy:AddEnemy(1);
        enemy:AddEnemy(2);
        enemy:AddEnemy(2);
        enemy:AddEnemy(2);
        enemy:AddEnemy(1);
        roam_zone3:AddEnemy(enemy, 1);
        roam_zone3:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    end

    roam_zone4 = vt_map.EnemyZone.Create(89, 100, 12, 15);
    if (monsters_defeated == false) then
        enemy = CreateEnemySprite(Map, "bat");
        _SetBattleEnvironment(enemy);
        enemy:NewEnemyParty();
        enemy:AddEnemy(6);
        enemy:AddEnemy(2);
        enemy:AddEnemy(6);
        enemy:AddEnemy(2);
        enemy:AddEnemy(1);
        roam_zone4:AddEnemy(enemy, 1);
        roam_zone4:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    end
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

        blocking_rock:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        blocking_rock:SetVisible(false);
        AudioManager:PlaySound("sounds/cave-in.ogg");
        Effects:ShakeScreen(0.6, 1000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);

        hero:SetMoving(false);
        -- Trigger the dialogue event about the shaking...
        EventManager:StartEvent("Hero dialogue during tremor");

        GlobalManager:SetEventValue("story", "layna_forest_cave2_monsters_defeated", 1);
    end
end

-- zones
local to_cave_1_1 = nil
local to_cave_exit = nil
local to_wolf_cave_zone = nil
local seeing_the_exit_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_cave_1_1_zone = vt_map.CameraZone.Create(0, 1, 11, 16);
    to_cave_exit_zone = vt_map.CameraZone.Create(108, 116, 95, 96);
    to_wolf_cave_zone = vt_map.CameraZone.Create(122, 124, 12, 14);
    seeing_the_exit_zone = vt_map.CameraZone.Create(99, 122, 80, 96);
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
        if (seeing_the_exit_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
            EventManager:StartEvent("Kalya sees the cave exit");
        end
    end
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("music/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    enemy:AddBattleScript("dat/battles/desert_cave_battle_anim.lua");
end

-- Map Custom functions
-- Used through scripted events
map_functions = {
    Sprite_Collision_on = function(sprite)
        if (sprite ~= nil) then
            sprite:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        end
    end,

    Sprite_Collision_off = function(sprite)
        if (sprite ~= nil) then
            sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end,

    Set_Camera = function(sprite)
        if (sprite ~= nil) then Map:SetCamera(sprite, 800); end
    end,

    -- Kalya and Bronann read the engraved text
    stone_sign_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        bronann:SetDirection(hero:GetDirection())
        Map:SetCamera(bronann)
        bronann:SetVisible(true)
        hero:SetVisible(false)
        hero:SetPosition(0, 0)

        kalya:SetVisible(true);
        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        bronann:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        bronann:SetDirection(vt_map.MapMode.NORTH);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        move_next_to_bronann_event:SetDestination(bronann:GetXPosition() + 2.0, bronann:GetYPosition(), false)
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
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        hero:SetDirection(bronann:GetDirection())
        Map:SetCamera(hero)
        hero:SetVisible(true)
        bronann:SetVisible(false)
        bronann:SetPosition(0, 0)

        -- Set event as done
        GlobalManager:SetEventValue("story", "kalya_stone_sign_dialogue_done", 1);
        _UpdateStoneSignDialogue();
    end,

    -- Kalya and Bronann see the exit
    see_the_exit_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        bronann:SetDirection(hero:GetDirection())
        Map:SetCamera(bronann)
        bronann:SetVisible(true)
        hero:SetVisible(false)
        hero:SetPosition(0, 0)

        kalya:SetVisible(true);
        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        bronann:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        bronann:SetDirection(vt_map.MapMode.SOUTH);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        move_next_to_bronann_event2:SetDestination(bronann:GetXPosition() + 2.0, bronann:GetYPosition(), false);
    end,

    end_of_exit_seen_dialogue = function()
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        hero:SetDirection(bronann:GetDirection())
        Map:SetCamera(hero)
        hero:SetVisible(true)
        bronann:SetVisible(false)
        bronann:SetPosition(0, 0)

        -- Set event as done
        GlobalManager:SetEventValue("story", "kalya_layna_forest_cave1_2_exit_dialogue_done", 1);
        Map:PopState();
    end
}
