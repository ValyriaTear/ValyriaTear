-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_minimap_procedural_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest"
map_image_filename = "data/story/common/locations/layna_forest.png"
map_subname = "Forest entrance"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/house_in_a_forest_loop_horrorpen_oga.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

-- the main character handler
local hero = nil

-- Forest dialogue heroes
local bronann = nil
local kalya = nil

-- the main map loading code
function Load(m)

    Map = m;
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(false);

    --Map:SetMinimapImage("data/story/layna_forest/minimaps/layna_forest_entrance_minimap.png");
    Map:ShowMinimap(true)

    _CreateCharacters();
    _CreateObjects();
    _CreateEnemies();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("data/visuals/ambient/clouds.png", 5.0, -5.0, true);

    -- Trigger the save point and spring speech event once
    if (GlobalManager:GetGameEvents():DoesEventExist("story", "kalya_save_points_n_spring_speech_done") == false) then
        hero:SetMoving(false);
        hero:SetDirection(vt_map.MapMode.EAST);
        EventManager:StartEvent("Forest entrance dialogue", 800);

        -- Add the layna forest location on first time in this map.
        GlobalManager:GetWorldMapData():ShowWorldLocation("layna forest");
    end

    -- Update the world map location
    GlobalManager:GetWorldMapData():SetCurrentLocationId("layna forest");

    _HandleTwilight();

    -- Change the music according to the moment of the story.
    EventManager:StartEvent("Music start");
end

-- Handle the twilight advancement after the crystal scene
function _HandleTwilight()

    -- If the characters have seen the crystal, then it's time to make the twilight happen
    if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_crystal_event_done") < 1) then
        return;
    end

    -- test if the day time is sufficiently advanced
    if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_twilight_value") < 3) then
        GlobalManager:GetGameEvents():SetEventValue("story", "layna_forest_twilight_value", 3);
    end

    Map:GetScriptSupervisor():AddScript("data/story/layna_forest/after_crystal_twilight.lua");
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 3, 30, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.EAST);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    local x_position = GlobalManager:GetMapData():GetSaveLocationX();
    local y_position = GlobalManager:GetMapData():GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Make the character look at us in that case
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(x_position, y_position);
    elseif (GlobalManager:GetMapData():GetPreviousLocation() == "from_layna_forest_NW") then
        hero:SetDirection(vt_map.MapMode.WEST);
        hero:SetPosition(61.0, 32.0);
    end

    -- Create dialogue characters
    bronann = CreateSprite(Map, "Bronann", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.EAST);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    bronann:SetVisible(false);

    kalya = CreateSprite(Map, "Kalya", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.EAST);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);
end

-- The heal particle effect map object
local heal_effect = nil

function _CreateObjects()
    local object = nil
    local npc = nil
    local event = nil

    vt_map.SavePoint.Create(19, 27);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject.Create("data/visuals/particle_effects/heal_particle.lua", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    heal_effect:Stop(); -- Don't run it until the character heals itself

    -- Heal point
    npc = CreateSprite(Map, "Butterfly", 27, 23, vt_map.MapMode.GROUND_OBJECT);
    npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    npc:SetVisible(false);
    npc:SetName(""); -- Unset the speaker name
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Your party feels better.");
    dialogue:AddLineEvent(text, npc, "Forest entrance heal", "");
    npc:AddDialogueReference(dialogue);

    -- Only add the squirrels and butterflies when the night isn't about to happen
    if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_crystal_event_done") < 1) then
        npc = CreateSprite(Map, "Butterfly", 42, 18, vt_map.MapMode.GROUND_OBJECT);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        event = vt_map.RandomMoveSpriteEvent.Create("Butterfly1 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Butterfly1 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Butterfly1 random move");

        npc = CreateSprite(Map, "Butterfly", 12, 30, vt_map.MapMode.GROUND_OBJECT);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        event = vt_map.RandomMoveSpriteEvent.Create("Butterfly2 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Butterfly2 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Butterfly2 random move", 2400);

        npc = CreateSprite(Map, "Butterfly", 50, 25, vt_map.MapMode.GROUND_OBJECT);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        event = vt_map.RandomMoveSpriteEvent.Create("Butterfly3 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Butterfly3 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Butterfly3 random move", 1050);

        npc = CreateSprite(Map, "Butterfly", 40, 30, vt_map.MapMode.GROUND_OBJECT);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        event = vt_map.RandomMoveSpriteEvent.Create("Butterfly4 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Butterfly4 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Butterfly4 random move", 3050);

        npc = CreateSprite(Map, "Squirrel", 18, 24, vt_map.MapMode.GROUND_OBJECT);
        -- Squirrels don't collide with the npcs.
        npc:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        npc:SetSpriteAsScenery(true);
        event = vt_map.RandomMoveSpriteEvent.Create("Squirrel1 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Squirrel1 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Squirrel1 random move");

        npc = CreateSprite(Map, "Squirrel", 40, 16, vt_map.MapMode.GROUND_OBJECT);
        -- Squirrels don't collide with the npcs.
        npc:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        npc:SetSpriteAsScenery(true);
        event = vt_map.RandomMoveSpriteEvent.Create("Squirrel2 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Squirrel2 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Squirrel2 random move", 1800);
    else
        -- add fireflies near the statue at night
        vt_map.ParticleObject.Create("data/visuals/particle_effects/fireflies.lua", 27, 22, vt_map.MapMode.GROUND_OBJECT);
        vt_map.ParticleObject.Create("data/visuals/particle_effects/fireflies.lua", 25, 23, vt_map.MapMode.GROUND_OBJECT);
        vt_map.ParticleObject.Create("data/visuals/particle_effects/fireflies.lua", 29, 22.5, vt_map.MapMode.GROUND_OBJECT);
    end

    CreateObject(Map, "Tree Small3", 23, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 40, 14, vt_map.MapMode.GROUND_OBJECT);

    -- Trees above the pathway
    CreateObject(Map, "Tree Big1", 4, 41, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 5, 21, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 12, 7, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small2", 1, 17.2, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 1, 27, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 5, 27, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 1, 33, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 5, 33, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 3, 5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 8, 6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 11, 2, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 15, 4.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 19, 6.2, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 23, 7.2, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 26, 4, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 30, 5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 34, 7, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 36, 8, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 39, 9, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 42, 7.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 45, 6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 48, 10, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 47.5, 8, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 50, 12, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 54, 15, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 59, 17, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 62, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 62, 12, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 54, 8, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 60, 4, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 55, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 61, 26, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 45, 23, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 11, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 2, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 3, 14, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 7, 10, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 2, 8, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 4, 17, vt_map.MapMode.GROUND_OBJECT);

    -- Trees below the pathway
    CreateObject(Map, "Tree Small3", 2, 40, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 7, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 9, 42, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 4, 47, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 12, 41, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 11, 45, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 14, 46, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 16.5, 48, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 19, 46.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 23, 48, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 26, 39, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 36, 43, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 27, 49, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 30.5, 51, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 33, 50, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 37, 48.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 40.5, 51, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 44, 50.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 47, 49.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 49.5, 48.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 52, 50, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 55, 48, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 58, 50.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 58, 47, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 62, 46.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 62, 49, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Little2", 15, 8, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Tiny4", 29, 37, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Little3", 16, 40.2, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Little1", 9, 12, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Tiny3", 58, 22, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Little1", 58, 9, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Tiny3", 28, 8.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Little3", 11, 44, vt_map.MapMode.GROUND_OBJECT);
end

function _CreateEnemies()
    local enemy = nil
    local roam_zone = nil

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(49, 62, 26, 39);

    if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_twilight_value") >= 6) then
        -- Encounters at night (snakes, spiders and bats)
        enemy = CreateEnemySprite(Map, "bat");
        _SetBattleEnvironment(enemy);
        enemy:NewEnemyParty();
        enemy:AddEnemy(2);
        enemy:AddEnemy(2);
        enemy:AddEnemy(6);
        enemy:NewEnemyParty();
        enemy:AddEnemy(4);
        enemy:AddEnemy(6);
        roam_zone:AddEnemy(enemy, 1);
    else
        -- Day and first encounters (slimes and spiders)
        enemy = CreateEnemySprite(Map, "slime");
        _SetBattleEnvironment(enemy);
        enemy:NewEnemyParty();
        enemy:AddEnemy(1);
        enemy:AddEnemy(1);
        enemy:AddEnemy(1);
        enemy:NewEnemyParty();
        enemy:AddEnemy(1);
        enemy:AddEnemy(2);
        roam_zone:AddEnemy(enemy, 1);
    end
end

-- Special event references which destinations must be updated just before being called.
local move_next_to_bronann_event = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Music event
    vt_map.ScriptedEvent.Create("Music start", "music_start", "");

    -- Triggered events
    vt_map.MapTransitionEvent.Create("exit forest", "data/story/layna_village/layna_village_center_map.lua",
                                     "data/story/layna_village/layna_village_center_script.lua", "from_layna_forest_entrance");

    vt_map.MapTransitionEvent.Create("to forest NW", "data/story/layna_forest/layna_forest_north_west_map.lua",
                                     "data/story/layna_forest/layna_forest_north_west_script.lua", "from_layna_forest_entrance");

    -- After the forest dungeon
    vt_map.MapTransitionEvent.Create("exit forest at night", "data/story/layna_village/layna_village_center_map.lua",
                                     "data/story/layna_village/layna_village_center_at_night_script.lua", "from_layna_forest_entrance");

    -- Heal point
    vt_map.ScriptedEvent.Create("Forest entrance heal", "heal_party", "heal_done");

    -- Generic events
    vt_map.ScriptedEvent.Create("Map:Popstate()", "Map_PopState", "");

    vt_map.ScriptedSpriteEvent.Create("kalya:SetCollision(ALL)", kalya, "Sprite_Collision_on", "");
    vt_map.ScriptedSpriteEvent.Create("bronann:SetCollision(ALL)", bronann, "Sprite_Collision_on", "");
    vt_map.ScriptedSpriteEvent.Create("kalya:SetCollision(NONE)", kalya, "Sprite_Collision_off", "");

    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Kalya", bronann, kalya);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at the statue", kalya, 27, 23);

    -- First time forest entrance dialogue about save points and the heal spring.
    event = vt_map.ScriptedEvent.Create("Forest entrance dialogue", "forest_statue_event_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 50);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_bronann_event = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann", kalya, 0, 0, false);
    move_next_to_bronann_event:AddEventLinkAtEnd("Kalya looks at the statue");
    move_next_to_bronann_event:AddEventLinkAtEnd("Kalya talks about the statue");
    move_next_to_bronann_event:AddEventLinkAtEnd("kalya:SetCollision(ALL)");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Look!");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at the statue", "", "exclamation");
    event = vt_map.DialogueEvent.Create("Kalya talks about the statue", dialogue);
    event:AddEventLinkAtEnd("Kalya moves near the statue");
    event:AddEventLinkAtEnd("Bronann gets nearer as well", 1000);

    event = vt_map.PathMoveSpriteEvent.Create("Kalya moves near the statue", kalya, 21, 20, true);
    event:AddEventLinkAtEnd("Kalya talks about the statue 2", 1000);

    vt_map.PathMoveSpriteEvent.Create("Bronann gets nearer as well", bronann, 14, 25, false);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Have you seen one of these before? This is a Layna statue. Praying near it heals both your mind and body.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "");
    text = vt_system.VTranslate("Just stand in front of the goddess below the spring and push '%s'.", InputManager:GetConfirmKeyName());
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Ok, thanks.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("See? Now, let's find my brother before he gets hurt.");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Kalya talks about the statue 2", dialogue);
    event:AddEventLinkAtEnd("kalya:SetCollision(NONE)");
    event:AddEventLinkAtEnd("Set Camera");

    event = vt_map.ScriptedSpriteEvent.Create("Set Camera", bronann, "SetCamera", "");
    event:AddEventLinkAtEnd("2nd hero goes back to party");

    event = vt_map.PathMoveSpriteEvent.Create("2nd hero goes back to party", kalya, bronann, false);
    event:AddEventLinkAtEnd("Map:Popstate()");
    event:AddEventLinkAtEnd("end of statue event");

    vt_map.ScriptedEvent.Create("end of statue event", "end_of_statue_event", "");
end

-- zones
local forest_entrance_exit_zone = nil
local to_forest_nw_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    forest_entrance_exit_zone = vt_map.CameraZone.Create(0, 1, 26, 34);
    to_forest_nw_zone = vt_map.CameraZone.Create(62, 64, 29, 35);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (forest_entrance_exit_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_crystal_event_done") == 0) then
            hero:SetMoving(false);
            EventManager:StartEvent("exit forest");
        else
            hero:SetMoving(false);
            EventManager:StartEvent("exit forest at night");
        end
    elseif (to_forest_nw_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest NW");
    end
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    -- default values
    enemy:SetBattleMusicTheme("data/music/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("data/battles/battle_scenes/forest_background.png");

    if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        -- Setup time of the day lighting on battles
        enemy:AddBattleScript("data/story/layna_forest/after_crystal_twilight_battles.lua");
        if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_twilight_value") > 2) then
            enemy:SetBattleBackground("data/battles/battle_scenes/forest_background_evening.png");
        end
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

    -- Kalya runs to the save point and tells Bronann about the spring.
    forest_statue_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

        -- use bronann and kalya sprite for events.
        bronann:SetDirection(hero:GetDirection());
        bronann:SetVisible(true);
        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        Map:SetCamera(bronann);
        -- Hide the hero
        hero:SetVisible(false);
        hero:SetPosition(0, 0);

        kalya:SetVisible(true);
        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        bronann:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        Map:SetCamera(kalya, 800);

        move_next_to_bronann_event:SetDestination(bronann:GetXPosition(), bronann:GetYPosition() + 2.0, false);
    end,

    SetCamera = function(sprite)
        Map:SetCamera(sprite, 800);
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

    Map_PopState = function()
        Map:PopState();
    end,

    end_of_statue_event = function()
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Move the hero back on Bronann position
        hero:SetDirection(bronann:GetDirection());
        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        hero:SetVisible(true);
        Map:SetCamera(hero);
        bronann:SetVisible(false);
        bronann:SetPosition(0, 0);

        -- Set event as done
        GlobalManager:GetGameEvents():SetEventValue("story", "kalya_save_points_n_spring_speech_done", 1);
    end,

    music_start = function()
        -- If the night has fallen, let the music change
        if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_twilight_value") >= 6) then
            AudioManager:PlayMusic("data/music/forest_at_night.ogg");
        end
    end
}
