-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_cave1_1_script = ns;
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
local Effects = {};

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
    Effects = Map:GetEffectSupervisor();
    Map:SetMinimapImage("dat/maps/layna_forest/minimaps/layna_forest_cave1_1_minimap.png");
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
    Effects:EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);
    -- Add the background and foreground animations
    Map:GetScriptSupervisor():AddScript("dat/maps/layna_forest/layna_forest_caves_background_anim.lua");

    if (GlobalManager:DoesEventExist("story", "kalya_speech_at_cave_entrance_done") == false) then
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
    hero = CreateSprite(Map, "Bronann", 116, 92);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    local x_position = GlobalManager:GetSaveLocationX();
    local y_position = GlobalManager:GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Use the save point position, and clear the save position data for next maps
        GlobalManager:UnsetSaveLocation();
        -- Make the character look at us in that case
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(x_position, y_position);
    elseif (GlobalManager:GetPreviousLocation() == "from_layna_cave_1_2") then
        hero:SetDirection(vt_map.MapMode.WEST);
        hero:SetPosition(125.0, 9.0);
    end

    Map:AddGroundObject(hero);

    -- Create secondary character for dialogue at map entrance
    kalya_sprite = CreateSprite(Map, "Kalya",
                                hero:GetXPosition(), hero:GetYPosition());

    kalya_sprite:SetDirection(vt_map.MapMode.NORTH);
    kalya_sprite:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya_sprite:SetVisible(false);
    Map:AddGroundObject(kalya_sprite);
end

-- Keeps in memory whether objects are being loaded.
-- Useful to prevent the trigger_on functions from shaking the screen or playing the tremor sound.
local _loading_objects = true;

function _CreateObjects()
    local object = {};
    local npc = {};
    local event = {}

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

    -- Add a halo showing the cave entrance
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 116, 109,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));

    -- Add a halo showing the next cave entrance
    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 132, 14,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));


    Map:AddSavePoint(50, 6);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject("dat/effects/particles/heal_particle.lua", 0, 0);
    heal_effect:SetObjectID(Map.object_supervisor:GenerateObjectID());
    heal_effect:Stop(); -- Don't run it until the character heals itself
    Map:AddGroundObject(heal_effect);

    -- Heal point
    npc = CreateSprite(Map, "Butterfly", 35, 7);
    npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    npc:SetVisible(false);
    npc:SetName(""); -- Unset the speaker name
    Map:AddGroundObject(npc);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Your party feels better...");
    dialogue:AddLineEvent(text, npc, "Cave heal", "");
    DialogueManager:AddDialogue(dialogue);
    npc:AddDialogueReference(dialogue);

    -- The triggers

    --near entrance
    trigger = vt_map.TriggerObject("layna_cave_entrance_trigger",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "",
                             "Remove entrance rock");
    trigger:SetObjectID(Map.object_supervisor:GenerateObjectID());
    trigger:SetPosition(113, 90);
    Map:AddFlatGroundObject(trigger);

    entrance_trigger_rock = CreateObject(Map, "Rock1", 115, 79);
    if (trigger:GetState() == true) then
        map_functions.make_entrance_rock_invisible();
    end
    Map:AddGroundObject(entrance_trigger_rock);

    event = vt_map.ScriptedEvent("Remove entrance rock", "make_entrance_rock_invisible_n_speech", "");
    EventManager:RegisterEvent(event);

    -- 2nd trigger
    trigger = vt_map.TriggerObject("layna_cave_2nd_trigger",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "",
                             "Remove 2nd rock");
    trigger:SetObjectID(Map.object_supervisor:GenerateObjectID());
    trigger:SetPosition(11, 88);
    Map:AddFlatGroundObject(trigger);

    second_trigger_rock = CreateObject(Map, "Rock1", 95, 58);
    if (trigger:GetState() == true) then
        map_functions.make_2nd_rock_invisible();
    end
    Map:AddGroundObject(second_trigger_rock);

    event = vt_map.ScriptedEvent("Remove 2nd rock", "make_2nd_rock_invisible", "");
    EventManager:RegisterEvent(event);

    -- 3rd trigger
    trigger = vt_map.TriggerObject("layna_cave_3rd_trigger",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "",
                             "Remove 3rd rock");
    trigger:SetObjectID(Map.object_supervisor:GenerateObjectID());
    trigger:SetPosition(39, 55);
    Map:AddFlatGroundObject(trigger);

    third_trigger_rock = CreateObject(Map, "Rock1", 15, 36);
    if (trigger:GetState() == true) then
        map_functions.make_3rd_rock_invisible();
    end
    Map:AddGroundObject(third_trigger_rock);

    event = vt_map.ScriptedEvent("Remove 3rd rock", "make_3rd_rock_invisible", "");
    EventManager:RegisterEvent(event);

    -- 4th trigger
    trigger = vt_map.TriggerObject("layna_cave_4th_trigger",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "",
                             "Remove 4th rock");
    trigger:SetObjectID(Map.object_supervisor:GenerateObjectID());
    trigger:SetPosition(62, 26);
    Map:AddFlatGroundObject(trigger);

    fourth_trigger_rock = CreateObject(Map, "Rock1", 45, 11);
    if (trigger:GetState() == true) then
        map_functions.make_4th_rock_invisible();
    end
    Map:AddGroundObject(fourth_trigger_rock);

    event = vt_map.ScriptedEvent("Remove 4th rock", "make_4th_rock_invisible", "");
    EventManager:RegisterEvent(event);

    -- 5th trigger
    trigger = vt_map.TriggerObject("layna_cave_5th_trigger",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "",
                             "Remove 5th rock");
    trigger:SetObjectID(Map.object_supervisor:GenerateObjectID());
    trigger:SetPosition(9, 6);
    Map:AddFlatGroundObject(trigger);

    fifth_trigger_rock = CreateObject(Map, "Rock1", 77, 33);
    if (trigger:GetState() == true) then
        map_functions.make_5th_rock_invisible();
    end
    Map:AddGroundObject(fifth_trigger_rock);

    event = vt_map.ScriptedEvent("Remove 5th rock", "make_5th_rock_invisible", "");
    EventManager:RegisterEvent(event);

    -- 6th trigger
    trigger = vt_map.TriggerObject("layna_cave_6th_trigger",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "",
                             "Remove 6th rock");
    trigger:SetObjectID(Map.object_supervisor:GenerateObjectID());
    trigger:SetPosition(114, 28);
    Map:AddFlatGroundObject(trigger);

    sixth_trigger_rock = CreateObject(Map, "Rock1", 115, 43);
    if (trigger:GetState() == true) then
        map_functions.make_6th_rock_invisible();
    end
    Map:AddGroundObject(sixth_trigger_rock);

    event = vt_map.ScriptedEvent("Remove 6th rock", "make_6th_rock_invisible", "");
    EventManager:RegisterEvent(event);

    -- 7th trigger
    trigger = vt_map.TriggerObject("layna_cave_7th_trigger",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "",
                             "Remove 7th rock");
    trigger:SetObjectID(Map.object_supervisor:GenerateObjectID());
    trigger:SetPosition(4, 26);
    Map:AddFlatGroundObject(trigger);

    seventh_trigger_rock = CreateObject(Map, "Rock1", 115, 47);
    if (trigger:GetState() == true) then
        map_functions.make_7th_rock_invisible();
    end
    Map:AddGroundObject(seventh_trigger_rock);

    event = vt_map.ScriptedEvent("Remove 7th rock", "make_7th_rock_invisible", "");
    EventManager:RegisterEvent(event);

    -- 8th trigger
    trigger = vt_map.TriggerObject("layna_cave_8th_trigger",
                             "img/sprites/map/triggers/stone_trigger1_off.lua",
                             "img/sprites/map/triggers/stone_trigger1_on.lua",
                             "",
                             "Remove 8th rock");
    trigger:SetObjectID(Map.object_supervisor:GenerateObjectID());
    trigger:SetPosition(115, 53);
    Map:AddFlatGroundObject(trigger);

    eighth_trigger_rock = CreateObject(Map, "Rock1", 93, 14);
    if (trigger:GetState() == true) then
        map_functions.make_8th_rock_invisible();
    end
    Map:AddGroundObject(eighth_trigger_rock);

    event = vt_map.ScriptedEvent("Remove 8th rock", "make_8th_rock_invisible", "");
    EventManager:RegisterEvent(event);

    -- Tells the other functions the objects are now loaded.
    -- This will permit to trigger the tremor shaking and sound again
    _loading_objects = false;
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    enemy:AddBattleScript("dat/battles/desert_cave_battle_anim.lua");
end

-- A special roam zone used to make the slime mother spawn only once.
local slime_mother_roam_zone = {};
-- A local variable making quicker the test on whether the slime mother boss is defeated
local slime_mother_defeated = false;

function _CheckSlimeMotherState()
    -- Tells the game that the slime mother was defeated
    -- TODO: This will have to be improved by adding battle event support in the slime mother
    if (slime_mother_defeated == false and slime_mother_roam_zone:GetSpawnsLeft() == 0) then
        if (GlobalManager:DoesEventExist("story", "layna_forest_slime_mother_defeated") == false) then
            GlobalManager:SetEventValue("story", "layna_forest_slime_mother_defeated", 1);
            slime_mother_defeated = true;
        end
    end
end

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

    -- Extra boss near the save point - Can only be beaten once.
    -- Hint: left, right, top, bottom
    slime_mother_roam_zone = vt_map.EnemyZone(8, 10, 6, 6);

    if (GlobalManager:DoesEventExist("story", "layna_forest_slime_mother_defeated")) then
        slime_mother_defeated = true;
    else
        enemy = CreateEnemySprite(Map, "big slime");
        _SetBattleEnvironment(enemy);
        enemy:SetBattleMusicTheme("mus/accion-OGA-djsaryon.ogg"); -- set the boss music for that one
        enemy:NewEnemyParty();
        enemy:AddEnemy(5, 812.0, 350.0);
        enemy:SetBoss(true);
        slime_mother_roam_zone:AddEnemy(enemy, Map, 1);
        slime_mother_roam_zone:SetSpawnsLeft(1); -- The Slime Mother boss shall spawn only one time.
    end
    Map:AddZone(slime_mother_roam_zone);

    -- A bat spawn point
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(7, 38, 25, 27);

    enemy = CreateEnemySprite(Map, "bat");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    roam_zone:AddEnemy(enemy, Map, 1);
    Map:AddZone(roam_zone);

    -- A bat spawn point
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(97, 115, 38, 40);

    enemy = CreateEnemySprite(Map, "bat");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    roam_zone:AddEnemy(enemy, Map, 1);
    Map:AddZone(roam_zone);

    -- A bat spawn point
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(51, 81, 58, 61);

    enemy = CreateEnemySprite(Map, "bat");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(6);
    enemy:AddEnemy(6);
    roam_zone:AddEnemy(enemy, Map, 1);
    Map:AddZone(roam_zone);
end

-- Special event references which destinations must be updated just before being called.
local move_next_to_hero_event = {}
local move_back_to_hero_event = {}

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to forest NW", "dat/maps/layna_forest/layna_forest_north_west_map.lua",
                                       "dat/maps/layna_forest/layna_forest_north_west_script.lua", "from_layna_cave_entrance");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to cave 1-2", "dat/maps/layna_forest/layna_forest_cave1_2_map.lua",
                                       "dat/maps/layna_forest/layna_forest_cave1_2_script.lua", "from_layna_cave_entrance");
    EventManager:RegisterEvent(event);

    -- Heal point
    event = vt_map.ScriptedEvent("Cave heal", "heal_party", "heal_done");
    EventManager:RegisterEvent(event);

    -- Dialogue events
    event = vt_map.LookAtSpriteEvent("Kalya looks at Bronann", kalya_sprite, hero);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Bronann looks at Kalya", hero, kalya_sprite);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks north", kalya_sprite, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks north", hero, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks west", hero, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("kalya_sprite:SetCollision(NONE)", kalya_sprite, "Sprite_Collision_off", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("kalya_sprite:SetCollision(ALL)", kalya_sprite, "Sprite_Collision_on", "");
    EventManager:RegisterEvent(event);

    -- Dialogue
    event = vt_map.ScriptedEvent("Cave entrance dialogue", "cave_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 50);
    EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event = vt_map.PathMoveSpriteEvent("Kalya moves next to Bronann", kalya_sprite, 0, 0, false);
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya looks north");
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya wonders about Orlinn");
    move_next_to_hero_event:AddEventLinkAtEnd("kalya_sprite:SetCollision(ALL)");
    EventManager:RegisterEvent(move_next_to_hero_event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("What a creepy place. I had never heard about this before ...");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Bronann looks at Kalya", "Kalya looks at Bronann", "exclamation");
    text = vt_system.Translate("It seems Orlinn was able to go through those platforms somehow.");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("Let's make our way through this before something bad happens to him.");
    dialogue:AddLineEvent(text, kalya_sprite, "", "Bronann looks west");
    text = vt_system.Translate("Look, there is a stone slab on the ground. Let's check this out.");
    dialogue:AddLineEmote(text, hero, "thinking dots");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya wonders about Orlinn", dialogue);
    event:AddEventLinkAtEnd("kalya_sprite:SetCollision(NONE)");
    event:AddEventLinkAtEnd("kalya goes back to party");
    EventManager:RegisterEvent(event);

    move_back_to_hero_event = vt_map.PathMoveSpriteEvent("kalya goes back to party", kalya_sprite, hero, false);
    move_back_to_hero_event:AddEventLinkAtEnd("end of cave entrance dialogue");
    EventManager:RegisterEvent(move_back_to_hero_event);

    event = vt_map.ScriptedEvent("end of cave entrance dialogue", "end_of_cave_dialogue", "");
    event:AddEventLinkAtEnd("Bronann looks north");
    EventManager:RegisterEvent(event);

    -- Dialogue when pushing first trigger
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("It seems the way is clear now.");
    dialogue:AddLineEventEmote(text, hero, "", "Play falling tree sound", "exclamation");
    text = vt_system.Translate("(I also heard something falling outside. Let's hope it's nothing bad.)");
    dialogue:AddLineEventEmote(text, hero, "Hero looks south", "", "interrogation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("First trigger dialogue", dialogue);
    EventManager:RegisterEvent(event);

    event = vt_map.SoundEvent("Play falling tree sound", "snd/falling_tree.ogg");
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Hero looks south", hero, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);

end

-- zones
local to_forest_NW_zone = {};
local to_cave_1_2_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_forest_NW_zone = vt_map.CameraZone(114, 118, 95, 97);
    Map:AddZone(to_forest_NW_zone);

    to_cave_1_2_zone = vt_map.CameraZone(126, 128, 3, 13);
    Map:AddZone(to_cave_1_2_zone);
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
        AudioManager:PlaySound("snd/cave-in.ogg");
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
        GlobalManager:GetActiveParty():AddHitPoints(20000);
        GlobalManager:GetActiveParty():AddSkillPoints(20000);
        Map:SetStamina(10000);
        AudioManager:PlaySound("snd/heal_spell.wav");
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
        GlobalManager:SetEventValue("story", "layna_forest_trees_shorcut_open", 1);
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
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya_sprite:SetVisible(true);
        kalya_sprite:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        hero:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        hero:SetDirection(vt_map.MapMode.NORTH);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        move_next_to_hero_event:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition(), false);
    end,

    end_of_cave_dialogue = function()
        Map:PopState();
        kalya_sprite:SetPosition(0, 0);
        kalya_sprite:SetVisible(false);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);

        -- Set event as done
        GlobalManager:SetEventValue("story", "kalya_speech_at_cave_entrance_done", 1);
    end
}
