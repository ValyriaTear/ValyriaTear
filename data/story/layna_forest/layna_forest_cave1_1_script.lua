-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_cave1_1_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest Cave"
map_image_filename = "data/story/common/locations/desert_cave.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/shrine-OGA-yd.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Effects = nil

-- the main character handler
local hero = nil

-- Forest dialogue characters
local bronann = nil
local kalya = nil

-- the main map loading code
function Load(m)
    Map = m;
    Effects = Map:GetEffectSupervisor();
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(false);

    Map:SetMinimapImage("data/story/layna_forest/minimaps/layna_forest_cave1_1_minimap.png");

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
    Effects:EnableAmbientOverlay("data/visuals/ambient/dark.png", 0.0, 0.0, false);
    -- Add the background and foreground animations
    Map:GetScriptSupervisor():AddScript("data/story/layna_forest/layna_forest_caves_background_anim.lua");

    if (GlobalManager:GetGameEvents():DoesEventExist("story", "kalya_speech_at_cave_entrance_done") == false) then
        hero:SetMoving(false);
        hero:SetDirection(vt_map.MapMode.WEST);
        -- Add 200 ms here to permit the engine to setup the correct hero sprite first
        EventManager:StartEvent("Cave entrance dialogue", 200);
    end
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();

    -- Check whether the slime mother boss has been defeated,
    _CheckSlimeMotherState();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 116, 92, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    local x_position = GlobalManager:GetMapData():GetSaveLocationX();
    local y_position = GlobalManager:GetMapData():GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Make the character look at us in that case
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(x_position, y_position);
    elseif (GlobalManager:GetMapData():GetPreviousLocation() == "from_layna_cave_1_2") then
        hero:SetDirection(vt_map.MapMode.WEST);
        hero:SetPosition(125.0, 9.0);
    end

    -- Create characters for dialogues
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

-- Keeps in memory whether objects are being loaded.
-- Useful to prevent the trigger_on functions from shaking the screen or playing the tremor sound.
local _loading_objects = true;

local entrance_trigger_rock = nil
local second_trigger_rock = nil
local third_trigger_rock = nil
local fourth_trigger_rock = nil
local fifth_trigger_rock = nil
local sixth_trigger_rock = nil
local seventh_trigger_rock = nil
local eighth_trigger_rock = nil

function _CreateObjects()
    local object = nil
    local npc = nil
    local event = nil

    -- Info sign
    object = CreateObject(Map, "Wood sign info", 122, 60, vt_map.MapMode.GROUND_OBJECT)
    object:SetEventWhenTalking("Info about moon juice")
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Running out of magic points?\nYou can restore you PMs using Moon juice.");
    dialogue:AddLine(text, nil);
    event = vt_map.DialogueEvent.Create("Info about moon juice", dialogue)

    -- Adapt the light color according to the time of the day.
    local light_color_red = 1.0;
    local light_color_green = 1.0;
    local light_color_blue = 1.0;
    local light_color_alpha = 0.8;
    if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        local tw_value = GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_twilight_value");
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

    -- Add a halo showing the cave entrance
    vt_map.Halo.Create("data/visuals/lights/torch_light_mask.lua", 116, 109,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));

    -- Add a halo showing the next cave entrance
    vt_map.Halo.Create("data/visuals/lights/torch_light_mask.lua", 132, 14,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));

    vt_map.SavePoint.Create(50, 6);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject.Create("data/visuals/particle_effects/heal_particle.lua", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    heal_effect:Stop(); -- Don't run it until the character heals itself

    -- Heal point
    npc = CreateSprite(Map, "Butterfly", 35, 7, vt_map.MapMode.GROUND_OBJECT);
    npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    npc:SetVisible(false);
    npc:SetName(""); -- Unset the speaker name

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Your party feels better.");
    dialogue:AddLineEvent(text, npc, "Cave heal", "");
    npc:AddDialogueReference(dialogue);

    npc = CreateObject(Map, "Layna Statue", 35, 6, vt_map.MapMode.GROUND_OBJECT);
    npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    npc:SetInteractionIcon("data/gui/map/heal_anim.lua")

    -- The triggers

    -- near entrance
    local trigger = vt_map.TriggerObject.Create("layna_cave_entrance_trigger",
                                                vt_map.MapMode.FLATGROUND_OBJECT,
                                                "data/entities/map/triggers/stone_trigger1_off.lua",
                                                "data/entities/map/triggers/stone_trigger1_on.lua",
                                                "",
                                                "Remove entrance rock");
    trigger:SetPosition(113, 90);

    entrance_trigger_rock = CreateObject(Map, "Rock1", 115, 79, vt_map.MapMode.GROUND_OBJECT);
    if (trigger:GetState() == true) then
        map_functions.make_entrance_rock_invisible();
    end

    vt_map.ScriptedEvent.Create("Remove entrance rock", "make_entrance_rock_invisible_n_speech", "");

    -- 2nd trigger
    trigger = vt_map.TriggerObject.Create("layna_cave_2nd_trigger", vt_map.MapMode.FLATGROUND_OBJECT,
                                          "data/entities/map/triggers/stone_trigger1_off.lua",
                                          "data/entities/map/triggers/stone_trigger1_on.lua",
                                          "",
                                          "Remove 2nd rock");
    trigger:SetPosition(11, 88);

    second_trigger_rock = CreateObject(Map, "Rock1", 95, 58, vt_map.MapMode.GROUND_OBJECT);
    if (trigger:GetState() == true) then
        map_functions.make_2nd_rock_invisible();
    end

    vt_map.ScriptedEvent.Create("Remove 2nd rock", "make_2nd_rock_invisible", "");

    -- 3rd trigger
    trigger = vt_map.TriggerObject.Create("layna_cave_3rd_trigger", vt_map.MapMode.FLATGROUND_OBJECT,
                                          "data/entities/map/triggers/stone_trigger1_off.lua",
                                          "data/entities/map/triggers/stone_trigger1_on.lua",
                                          "",
                                          "Remove 3rd rock");
    trigger:SetPosition(39, 55);

    third_trigger_rock = CreateObject(Map, "Rock1", 15, 36, vt_map.MapMode.GROUND_OBJECT);
    if (trigger:GetState() == true) then
        map_functions.make_3rd_rock_invisible();
    end

    vt_map.ScriptedEvent.Create("Remove 3rd rock", "make_3rd_rock_invisible", "");

    -- 4th trigger
    trigger = vt_map.TriggerObject.Create("layna_cave_4th_trigger", vt_map.MapMode.FLATGROUND_OBJECT,
                                          "data/entities/map/triggers/stone_trigger1_off.lua",
                                          "data/entities/map/triggers/stone_trigger1_on.lua",
                                          "",
                                          "Remove 4th rock");
    trigger:SetPosition(62, 26);

    fourth_trigger_rock = CreateObject(Map, "Rock1", 45, 11, vt_map.MapMode.GROUND_OBJECT);
    if (trigger:GetState() == true) then
        map_functions.make_4th_rock_invisible();
    end

    vt_map.ScriptedEvent.Create("Remove 4th rock", "make_4th_rock_invisible", "");

    -- 5th trigger
    trigger = vt_map.TriggerObject.Create("layna_cave_5th_trigger", vt_map.MapMode.FLATGROUND_OBJECT,
                                          "data/entities/map/triggers/stone_trigger1_off.lua",
                                          "data/entities/map/triggers/stone_trigger1_on.lua",
                                          "",
                                          "Remove 5th rock");
    trigger:SetPosition(9, 6);

    fifth_trigger_rock = CreateObject(Map, "Rock1", 77, 33, vt_map.MapMode.GROUND_OBJECT);
    if (trigger:GetState() == true) then
        map_functions.make_5th_rock_invisible();
    end

    vt_map.ScriptedEvent.Create("Remove 5th rock", "make_5th_rock_invisible", "");

    -- 6th trigger
    trigger = vt_map.TriggerObject.Create("layna_cave_6th_trigger", vt_map.MapMode.FLATGROUND_OBJECT,
                                          "data/entities/map/triggers/stone_trigger1_off.lua",
                                          "data/entities/map/triggers/stone_trigger1_on.lua",
                                          "",
                                          "Remove 6th rock");
    trigger:SetPosition(114, 28);

    sixth_trigger_rock = CreateObject(Map, "Rock1", 115, 43, vt_map.MapMode.GROUND_OBJECT);
    if (trigger:GetState() == true) then
        map_functions.make_6th_rock_invisible();
    end

    vt_map.ScriptedEvent.Create("Remove 6th rock", "make_6th_rock_invisible", "");

    -- 7th trigger
    trigger = vt_map.TriggerObject.Create("layna_cave_7th_trigger", vt_map.MapMode.FLATGROUND_OBJECT,
                                          "data/entities/map/triggers/stone_trigger1_off.lua",
                                          "data/entities/map/triggers/stone_trigger1_on.lua",
                                          "",
                                          "Remove 7th rock");
    trigger:SetPosition(4, 26);

    seventh_trigger_rock = CreateObject(Map, "Rock1", 115, 47, vt_map.MapMode.GROUND_OBJECT);
    if (trigger:GetState() == true) then
        map_functions.make_7th_rock_invisible();
    end

    vt_map.ScriptedEvent.Create("Remove 7th rock", "make_7th_rock_invisible", "");

    -- 8th trigger
    trigger = vt_map.TriggerObject.Create("layna_cave_8th_trigger", vt_map.MapMode.FLATGROUND_OBJECT,
                                          "data/entities/map/triggers/stone_trigger1_off.lua",
                                          "data/entities/map/triggers/stone_trigger1_on.lua",
                                          "",
                                          "Remove 8th rock");
    trigger:SetPosition(115, 53);

    eighth_trigger_rock = CreateObject(Map, "Rock1", 93, 14, vt_map.MapMode.GROUND_OBJECT);
    if (trigger:GetState() == true) then
        map_functions.make_8th_rock_invisible();
    end

    vt_map.ScriptedEvent.Create("Remove 8th rock", "make_8th_rock_invisible", "");

    -- Tells the other functions the objects are now loaded.
    -- This will permit to trigger the tremor shaking and sound again
    _loading_objects = false;
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("data/music/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("data/battles/battle_scenes/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    enemy:AddBattleScript("data/battles/battle_scenes/desert_cave_battle_anim.lua");
end

-- A special roam zone used to make the slime mother spawn only once.
local slime_mother_roam_zone = nil
-- A local variable making quicker the test on whether the slime mother boss is defeated
local slime_mother_defeated = false;

function _CheckSlimeMotherState()
    -- Tells the game that the slime mother was defeated
    -- TODO: This will have to be improved by adding battle event support in the slime mother
    if (slime_mother_defeated == false and slime_mother_roam_zone:GetSpawnsLeft() == 0) then
        if (GlobalManager:GetGameEvents():DoesEventExist("story", "layna_forest_slime_mother_defeated") == false) then
            GlobalManager:GetGameEvents():SetEventValue("story", "layna_forest_slime_mother_defeated", 1);
            slime_mother_defeated = true;
        end
    end
end

function _CreateEnemies()
    local enemy = nil
    local roam_zone = nil

    -- Extra boss near the save point - Can only be beaten once.
    -- Hint: left, right, top, bottom
    slime_mother_roam_zone = vt_map.EnemyZone.Create(8, 10, 6, 6);

    if (GlobalManager:GetGameEvents():DoesEventExist("story", "layna_forest_slime_mother_defeated")) then
        slime_mother_defeated = true;
    else
        enemy = CreateEnemySprite(Map, "big slime");
        _SetBattleEnvironment(enemy);
        enemy:SetBattleMusicTheme("data/music/accion-OGA-djsaryon.ogg"); -- set the boss music for that one
        enemy:NewEnemyParty();
        enemy:AddEnemy(5, 512.0, 600.0);
        enemy:SetBoss(true);
        slime_mother_roam_zone:AddEnemy(enemy, 1);
        slime_mother_roam_zone:SetSpawnsLeft(1); -- The Slime Mother boss shall spawn only one time.
    end

    -- A bat spawn point
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(7, 38, 25, 27);

    enemy = CreateEnemySprite(Map, "bat");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    roam_zone:AddEnemy(enemy, 1);

    -- A bat spawn point
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(97, 115, 38, 40);

    enemy = CreateEnemySprite(Map, "bat");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    roam_zone:AddEnemy(enemy, 1);

    -- A bat spawn point
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(51, 81, 58, 61);

    enemy = CreateEnemySprite(Map, "bat");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    roam_zone:AddEnemy(enemy, 1);
end

-- Special event references which destinations must be updated just before being called.
local move_next_to_bronann_event = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    vt_map.MapTransitionEvent.Create("to forest NW", "data/story/layna_forest/layna_forest_north_west_map.lua",
                                     "data/story/layna_forest/layna_forest_north_west_script.lua", "from_layna_cave_entrance");

    vt_map.MapTransitionEvent.Create("to cave 1-2", "data/story/layna_forest/layna_forest_cave1_2_map.lua",
                                     "data/story/layna_forest/layna_forest_cave1_2_script.lua", "from_layna_cave_entrance");

    -- Heal point
    vt_map.ScriptedEvent.Create("Cave heal", "heal_party", "heal_done");

    -- Dialogue events
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Kalya", bronann, kalya);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks north", bronann, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks west", bronann, vt_map.MapMode.WEST);
    vt_map.ScriptedSpriteEvent.Create("kalya:SetCollision(NONE)", kalya, "Sprite_Collision_off", "");
    vt_map.ScriptedSpriteEvent.Create("kalya:SetCollision(ALL)", kalya, "Sprite_Collision_on", "");

    -- Dialogue
    event = vt_map.ScriptedEvent.Create("Cave entrance dialogue", "cave_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 50);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_bronann_event = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann", kalya, 0, 0, false);
    move_next_to_bronann_event:AddEventLinkAtEnd("Kalya looks north");
    move_next_to_bronann_event:AddEventLinkAtEnd("Kalya wonders about Orlinn");
    move_next_to_bronann_event:AddEventLinkAtEnd("kalya:SetCollision(ALL)");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I didn't know that there was a cave here. It's kind of creepy. Let's hurry and find Orlinn.");
    dialogue:AddLineEventEmote(text, kalya, "Bronann looks at Kalya", "Kalya looks at Bronann", "exclamation");
    text = vt_system.Translate("It seems Orlinn was able to go through those platforms. Somehow.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Let's make our way through this before something bad happens to him.");
    dialogue:AddLineEvent(text, kalya, "", "Bronann looks west");
    text = vt_system.Translate("Look, there is a stone slab on the ground. Let's check this out.");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    event = vt_map.DialogueEvent.Create("Kalya wonders about Orlinn", dialogue);
    event:AddEventLinkAtEnd("kalya:SetCollision(NONE)");
    event:AddEventLinkAtEnd("kalya goes back to party");

    event = vt_map.PathMoveSpriteEvent.Create("kalya goes back to party", kalya, bronann, false);
    event:AddEventLinkAtEnd("end of cave entrance dialogue");

    event = vt_map.ScriptedEvent.Create("end of cave entrance dialogue", "end_of_cave_dialogue", "");
    event:AddEventLinkAtEnd("Bronann looks north");

    -- Dialogue when pushing first trigger
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("It seems the way is clear now.");
    dialogue:AddLineEventEmote(text, bronann, "", "Play falling tree sound", "exclamation");
    text = vt_system.Translate("(I also heard something falling outside. Let's hope it's nothing bad.)");
    dialogue:AddLineEventEmote(text, bronann, "Hero looks south", "", "interrogation");
    vt_map.DialogueEvent.Create("First trigger dialogue", dialogue);

    vt_map.SoundEvent.Create("Play falling tree sound", "data/sounds/falling_tree.ogg");

    vt_map.ChangeDirectionSpriteEvent.Create("Hero looks south", hero, vt_map.MapMode.SOUTH);
end

-- zones
local to_forest_NW_zone = nil
local to_cave_1_2_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_forest_NW_zone = vt_map.CameraZone.Create(114, 118, 95, 97);
    to_cave_1_2_zone = vt_map.CameraZone.Create(126, 128, 3, 13);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_forest_NW_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest NW");
    elseif (to_cave_1_2_zone:IsCameraEntering()) then
        hero:SetMoving(false);
        EventManager:StartEvent("to cave 1-2");
    end
end

function _MakeRockInvisible(object)
    object:SetVisible(false);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    -- Only triggers the sound and shaking if the triggers states are not being loaded.
    if (_loading_objects == false) then
        AudioManager:PlaySound("data/sounds/cave-in.ogg");
        Effects:ShakeScreen(0.6, 1000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);
    end
end

-- Map Custom functions
-- Used through scripted events

-- Effect time used when applying the heal light effect
local heal_effect_time = 0;
local heal_color = vt_video.Color(0.0, 0.0, 1.0, 1.0);

map_functions = {

    heal_party = function()
        hero:SetMoving(false);
        -- Should be sufficient to heal anybody
        local character_handler = GlobalManager:GetCharacterHandler()
        character_handler:GetActiveParty():AddHitPoints(10000)
        character_handler:GetActiveParty():AddSkillPoints(10000)
        Map:SetStamina(10000)
        Map:RemoveNegativeActiveStatusEffects()
        AudioManager:PlaySound("data/sounds/heal_spell.wav");
        heal_effect:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        heal_effect:Start();
        heal_effect_time = 0;
    end,

    heal_done = function()
        heal_effect_time = heal_effect_time + SystemManager:GetUpdateTime();

        if (heal_effect_time < 300.0) then
            heal_color:SetAlpha(heal_effect_time / 300.0 / 3.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(heal_color);
            return false;
        end

        if (heal_effect_time < 1000.0) then
            heal_color:SetAlpha(((1000.0 - heal_effect_time) / 700.0) / 3.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(heal_color);
            return false;
        end
        return true;
    end,

    make_entrance_rock_invisible = function()
        _MakeRockInvisible(entrance_trigger_rock);
    end,

    -- Same function than above + speech about shortcut
    make_entrance_rock_invisible_n_speech = function()
        map_functions.make_entrance_rock_invisible();
        EventManager:StartEvent("First trigger dialogue");
        -- Set the shortcut as open
        GlobalManager:GetGameEvents():SetEventValue("story", "layna_forest_trees_shorcut_open", 1);
    end,
    make_2nd_rock_invisible = function()
        _MakeRockInvisible(second_trigger_rock);
    end,
    make_3rd_rock_invisible = function()
        _MakeRockInvisible(third_trigger_rock);
    end,
    make_4th_rock_invisible = function()
        _MakeRockInvisible(fourth_trigger_rock);
    end,
    make_5th_rock_invisible = function()
        _MakeRockInvisible(fifth_trigger_rock);
    end,
    make_6th_rock_invisible = function()
        _MakeRockInvisible(sixth_trigger_rock);
    end,
    make_7th_rock_invisible = function()
        _MakeRockInvisible(seventh_trigger_rock);
    end,
    make_8th_rock_invisible = function()
        _MakeRockInvisible(eighth_trigger_rock);
    end,

    make_visible = function(sprite)
        if (sprite ~= nil) then
            sprite:SetVisible(true);
            sprite:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        end
    end,

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

    -- Kalya talks with Bronann at cave entrance - start event.
    cave_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition())
        bronann:SetDirection(hero:GetDirection())
        bronann:SetVisible(true)
        hero:SetVisible(false)
        Map:SetCamera(bronann)
        hero:SetPosition(0, 0)

        kalya:SetVisible(true);
        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        bronann:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        bronann:SetDirection(vt_map.MapMode.NORTH);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        move_next_to_bronann_event:SetDestination(bronann:GetXPosition() + 2.0, bronann:GetYPosition(), false);
    end,

    end_of_cave_dialogue = function()
        Map:PopState();
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition())
        hero:SetDirection(bronann:GetDirection())
        Map:SetCamera(hero)
        hero:SetVisible(true)
        bronann:SetVisible(false)
        bronann:SetPosition(0, 0)

        -- Set event as done
        GlobalManager:GetGameEvents():SetEventValue("story", "kalya_speech_at_cave_entrance_done", 1);
    end
}
