-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_path1_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "img/menus/locations/mt_elrus.png"
map_subname = "Low Mountain"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "" -- TODO

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- the main character handler
local hero = {};

-- Forest dialogue secondary hero
local kalya = {};
local orlinn = {};

-- Name of the main sprite. Used to reload the good one at the end of dialogue events.
local main_sprite_name = "";

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

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

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");

end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 3, 30);
    hero:SetDirection(vt_map.MapMode.EAST);
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
    elseif (GlobalManager:GetPreviousLocation() == "from_grotto_exit1") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(64.0, 47.0);
    end

    Map:AddGroundObject(hero);

    -- Create secondary characters
    kalya = CreateSprite(Map, "Kalya",
                         hero:GetXPosition(), hero:GetYPosition());
    kalya:SetDirection(vt_map.MapMode.EAST);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);
    Map:AddGroundObject(kalya);

    orlinn = CreateSprite(Map, "Orlinn",
                          hero:GetXPosition(), hero:GetYPosition());
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);
    Map:AddGroundObject(orlinn);
end

-- The heal particle effect map object
local heal_effect = {};

function _CreateObjects()
    local object = {}
    local npc = {}

    Map:AddSavePoint(113, 56);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject("dat/effects/particles/heal_particle.lua", 0, 0);
    heal_effect:SetObjectID(Map.object_supervisor:GenerateObjectID());
    heal_effect:Stop(); -- Don't run it until the character heals itself
    Map:AddGroundObject(heal_effect);

    -- Heal point
    npc = CreateSprite(Map, "Butterfly", 104, 56);
    npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    npc:SetVisible(false);
    npc:SetName(""); -- Unset the speaker name
    Map:AddGroundObject(npc);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Your party feels better...");
    dialogue:AddLineEvent(text, npc, "Heal event", "");
    DialogueManager:AddDialogue(dialogue);
    npc:AddDialogueReference(dialogue);
    npc = CreateObject(Map, "Layna Statue", 104, 56);
    Map:AddGroundObject(npc);

    -- Objects array
    local map_objects = {
        --  near the bridge
        { "Tree Big1", 126, 88 },
        { "Tree Big1", 127, 94 },
        { "Tree Big2", 124, 98 },
        { "Tree Big2", 110, 98 },

        -- Right border
        { "Tree Big2", 126, 62 },
        { "Tree Big2", 126, 56 },
        { "Tree Big2", 127, 49 },
        { "Tree Big2", 121, 52 },
        { "Tree Big2", 117, 50 },
        { "Tree Big2", 121, 58 },
        { "Tree Big1", 123, 60 },
        { "Tree Big2", 125, 51 },
        { "Rock2", 124, 64 },
        { "Rock2", 123, 66.5 },
        { "Rock2", 122, 69 },
        { "Rock1", 111, 45 },

        { "Tree Big2", 105, 38 },
        { "Tree Big1", 117, 37 },
        { "Tree Big2", 114, 47 },
        { "Tree Big2", 109, 48 },
        { "Tree Big1", 105, 46 },
        { "Tree Big1", 97, 46.5 },

        { "Tree Big1", 105, 54 },
        { "Tree Big1", 100, 56 },
        { "Tree Big2", 102, 61 },
        { "Tree Big1", 90, 62 },
        { "Tree Big2", 106, 60 },
        { "Tree Big1", 88, 70 },
        { "Tree Small1", 90, 76 },
        { "Tree Small2", 97, 52 },
        { "Tree Big1", 98, 65 },
        { "Tree Big1", 93, 67 },
        { "Tree Big2", 95, 71 },
        { "Tree Small1", 99, 68 },
        { "Tree Small2", 73, 73 },
        { "Tree Small1", 79, 78 },
        { "Tree Big2", 88, 46 },
        { "Tree Big1", 83, 48 },
        { "Tree Big1", 73, 46.2 },
        { "Tree Big1", 93, 49 },
        { "Tree Big1", 85, 60 },
        { "Tree Small1", 80, 58 },

        { "Tree Big1", 67, 54 },
        { "Tree Big1", 64, 57 },
        { "Tree Big1", 70, 59 },

        { "Tree Big1", 44, 89 },
        { "Tree Small1", 27, 87 },
        { "Tree Big1", 21, 83 },
        { "Tree Big1", 6, 87.2 },
        { "Tree Big1", 2, 89 },
        { "Tree Big1", 1, 92 },
        { "Tree Big1", 3, 96 },
        { "Tree Big1", 5, 98 },

        { "Tree Tiny1", 17, 75 },
        { "Tree Big2", 55, 63 },
        { "Tree Big2", 43, 92 },
        { "Tree Big2", 47, 84.5 },
        { "Tree Big2", 45, 80 },
        { "Tree Big2", 42, 77 },
        { "Tree Big2", 51, 79 },
        { "Tree Big2", 52, 73 },
        { "Tree Big2", 44, 71 },
        { "Tree Big2", 70, 51 },
        { "Tree Big2", 73.5, 49 },
        { "Tree Big2", 77, 59 },
        { "Tree Big2", 74, 57 },
        { "Tree Big2", 50, 82 },
        { "Tree Big2", 49, 75.5 },
        { "Tree Big2", 46, 50 },
        { "Tree Big2", 50, 56 },
        { "Tree Big1", 47, 53 },
        { "Tree Big1", 68, 74 },

        { "Rock2", 124, 31 },
        { "Rock1", 127, 30 },
        { "Rock2", 123, 39 },
        { "Rock1", 126, 31.5 },
        { "Rock1", 128, 35 },
        { "Rock1", 108, 72 },
        { "Rock2", 96, 76 },
        { "Rock1", 85, 73.5 },
        { "Rock2", 46, 94 },
        { "Rock1", 48, 96 },

        { "Rock1", 54, 45 },
        { "Rock1", 53, 48 },
        { "Rock1", 50, 47 },
        { "Rock1", 52, 58 },
        { "Rock1", 54, 59.5 },

        { "Rock2", 81, 82 },
        { "Rock2", 64, 78 },
        { "Rock2", 54, 87 },
        { "Rock2", 73, 67 },
        { "Rock2", 85, 63 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_objects) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        Map:AddGroundObject(object);
    end

    -- grass array
    local map_grass = {
        --  right border
        { "Grass Clump1", 119, 67 },
        { "Grass Clump1", 125, 65 },
        { "Grass Clump1", 127, 68 },
        { "Grass Clump1", 122, 71 },
        { "Grass Clump1", 126, 72 },
        { "Grass Clump1", 118, 51 },
        { "Grass Clump1", 122, 47 },
        { "Grass Clump1", 112, 69 },

        { "Grass Clump1", 105.5, 47 },
        { "Grass Clump1", 125, 32.5 },
        { "Grass Clump1", 122, 31 },
        { "Grass Clump1", 103.5, 57 },
        { "Grass Clump1", 92, 69 },
        { "Grass Clump1", 99, 58 },
        { "Grass Clump1", 84, 70.2 },

        { "Grass Clump1", 38, 63 },
        { "Grass Clump1", 51, 49 },
        { "Grass Clump1", 53, 46 },
        { "Grass Clump1", 48, 94 },
        { "Grass Clump1", 80, 59.2 },

    }

    -- Loads the grass clumps according to the array
    for my_index, my_array in pairs(map_grass) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(object);
    end

end

local dark_soldier1 = {}

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(59, 62, 61, 85);

    -- Dark soldier 1
    dark_soldier1 = CreateEnemySprite(Map, "Dark Soldier");
    _SetBattleEnvironment(dark_soldier1);
    dark_soldier1:NewEnemyParty();
    dark_soldier1:AddEnemy(9);
    roam_zone:AddEnemy(dark_soldier1, Map, 1);
    roam_zone:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    Map:AddZone(roam_zone);
end

-- Special event references which destinations must be updated just before being called.
local kalya_move_next_to_hero_event = {}
local orlinn_move_next_to_hero_event = {}
local move_back_to_hero_event = {}

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- To the first cave
    event = vt_map.MapTransitionEvent("to first cave", "dat/maps/mt_elbrus/mt_elbrus_cave1_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_cave1_script.lua", "from_entrance1");
    EventManager:RegisterEvent(event);

    -- Heal point
    event = vt_map.ScriptedEvent("Heal event", "heal_party", "heal_done");
    EventManager:RegisterEvent(event);

    -- Generic event
    event = vt_map.LookAtSpriteEvent("Orlinn looks at Kalya", orlinn, kalya);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Kalya looks at Bronann", kalya, hero);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Bronann looks at Kalya", hero, kalya);
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks west", hero, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks west", kalya, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks west", orlinn, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);

    -- Kalya sees the first guard and tells more about the heroes destination.
    event = vt_map.ScriptedEvent("Set scene state for dialogue about soldiers", "soldiers_dialogue_set_scene_state", "");
    event:AddEventLinkAtEnd("The hero moves to a good watch point");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("The hero moves to a good watch point", hero, 88, 79, false);
    event:AddEventLinkAtEnd("Kalya tells about the soliders and their destination");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Kalya tells about the soliders and their destination", "kalya_sees_the_soldiers_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 100);
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann", 100);
    EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_hero_event = vt_map.PathMoveSpriteEvent("Kalya moves next to Bronann", kalya, 0, 0, false);
    kalya_move_next_to_hero_event:AddEventLinkAtEnd("Kalya looks west");
    kalya_move_next_to_hero_event:AddEventLinkAtEnd("Bronann looks west");
    kalya_move_next_to_hero_event:AddEventLinkAtEnd("Kalya sees the soldier");
    EventManager:RegisterEvent(kalya_move_next_to_hero_event);
    orlinn_move_next_to_hero_event = vt_map.PathMoveSpriteEvent("Orlinn moves next to Bronann", orlinn, 0, 0, false);
    orlinn_move_next_to_hero_event:AddEventLinkAtEnd("Orlinn looks west");
    EventManager:RegisterEvent(orlinn_move_next_to_hero_event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Look!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya sees the soldier", dialogue);
    event:AddEventLinkAtEnd("Set the Camera on the soldier", 200);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Set the Camera on the soldier", "set_the_camera_on_the_soldier", "is_camera_moving_finished");
    event:AddEventLinkAtEnd("Set the camera back to Bronann", 1000);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Set the camera back to Bronann", "set_the_camera_back_on_bronann", "is_camera_moving_finished");
    event:AddEventLinkAtEnd("Kalya tells the plan");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Banesore's minions are already all over the place ...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "Bronann looks at Kalya");
    text = vt_system.Translate("The dark soldiers as we call them are fanatics. They'll follow Banesore's orders even if it means death.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("We don't how he does that, but he can turn anybody into a servile subject.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Their strength is also increased by the transformation. That's why we call them the zombified army.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("We must absolutely try to avoid them or they'll call reinforcement.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks west", "");
    text = vt_system.Translate("They are too strong. If they catch us, we're doomed.");
    dialogue:AddLine(text, kalya);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya tells the plan", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes back to party");
    event:AddEventLinkAtEnd("Kalya goes back to party");
    EventManager:RegisterEvent(event);

    orlinn_move_back_to_hero_event = vt_map.PathMoveSpriteEvent("Orlinn goes back to party", orlinn, hero, false);
    EventManager:RegisterEvent(orlinn_move_back_to_hero_event);

    kalya_move_back_to_hero_event = vt_map.PathMoveSpriteEvent("Kalya goes back to party", kalya, hero, false);
    kalya_move_back_to_hero_event:AddEventLinkAtEnd("End of dialogue about the soldiers");
    EventManager:RegisterEvent(kalya_move_back_to_hero_event);

    event = vt_map.ScriptedEvent("End of dialogue about the soldiers", "end_of_dialogue_about_soldiers", "");
    EventManager:RegisterEvent(event);
end

-- zones
local see_first_guard_zone = {};
local to_first_cave_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    see_first_guard_zone = vt_map.CameraZone(86, 88, 70, 86);
    Map:AddZone(see_first_guard_zone);

    to_first_cave_zone = vt_map.CameraZone(62, 66, 43, 45);
    Map:AddZone(to_first_cave_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (see_first_guard_zone:IsCameraEntering() == true and Map:CurrentState() ~= vt_map.MapMode.STATE_SCENE) then
        if (GlobalManager:GetEventValue("story", "mt_elbrus_kalya_sees_the_soldiers") == 0) then
            hero:SetMoving(false);
            EventManager:StartEvent("Set scene state for dialogue about soldiers");
        end
    elseif (to_first_cave_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to first cave");
    end
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    -- default values
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_background.png");
    enemy:AddBattleScript("dat/maps/common/at_night.lua");
    -- TODO: add script showing the scenery.
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
        GlobalManager:GetActiveParty():AddHitPoints(10000);
        GlobalManager:GetActiveParty():AddSkillPoints(10000);
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

    SetCamera = function(sprite)
        Map:SetCamera(sprite, 800);
    end,

    soldiers_dialogue_set_scene_state = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

    kalya_sees_the_soldiers_dialogue_start = function()
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        kalya:SetVisible(true);
        orlinn:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        orlinn:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        kalya_move_next_to_hero_event:SetDestination(hero:GetXPosition(), hero:GetYPosition() - 2.0, false);
        orlinn_move_next_to_hero_event:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition() - 2.0, false);
    end,

    end_of_dialogue_about_soldiers = function()
        Map:PopState();
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);

        -- Set event as done
        GlobalManager:SetEventValue("story", "mt_elbrus_kalya_sees_the_soldiers", 1);
    end,

    set_the_camera_on_the_soldier = function()
        Map:SetCamera(dark_soldier1, 1500);
    end,

    is_camera_moving_finished = function()
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        return true;
    end,

    set_the_camera_back_on_bronann = function()
        Map:SetCamera(hero, 1500);
    end,
}
