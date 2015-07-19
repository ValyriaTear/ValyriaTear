-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_entrance_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest"
map_image_filename = "img/menus/locations/layna_forest.png"
map_subname = "Forest entrance"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/house_in_a_forest_loop_horrorpen_oga.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- the main character handler
local hero = {};

-- Forest dialogue secondary hero
local kalya_sprite =  {};

-- Name of the main sprite. Used to reload the good one at the end of the firt forest entrance event.
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

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);

    -- Trigger the save point and spring speech event once
    if (GlobalManager:DoesEventExist("story", "kalya_save_points_n_spring_speech_done") == false) then
        hero:SetMoving(false);
        hero:SetDirection(vt_map.MapMode.EAST);
        EventManager:StartEvent("Forest entrance dialogue", 800);

        -- Add the layna forest location on first time in this map.
        GlobalManager:ShowWorldLocation("layna forest");
    end

    -- Update the world map location
    GlobalManager:SetCurrentLocationId("layna forest");

    _HandleTwilight();

    -- Change the music according to the moment of the story.
    EventManager:StartEvent("Music start", 50);
end

-- Handle the twilight advancement after the crystal scene
function _HandleTwilight()

    -- If the characters have seen the crystal, then it's time to make the twilight happen
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") < 1) then
        return;
    end

    -- test if the day time is sufficiently advanced
    if (GlobalManager:GetEventValue("story", "layna_forest_twilight_value") < 3) then
        GlobalManager:SetEventValue("story", "layna_forest_twilight_value", 3);
    end

    Map:GetScriptSupervisor():AddScript("dat/maps/layna_forest/after_crystal_twilight.lua");
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
    elseif (GlobalManager:GetPreviousLocation() == "from_layna_forest_NW") then
        hero:SetDirection(vt_map.MapMode.WEST);
        hero:SetPosition(61.0, 32.0);
    end

    Map:AddGroundObject(hero);

    -- Create secondary character - Kalya
    kalya_sprite = CreateSprite(Map, "Kalya",
                            hero:GetXPosition(), hero:GetYPosition());

    kalya_sprite:SetDirection(vt_map.MapMode.EAST);
    kalya_sprite:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya_sprite:SetVisible(false);
    Map:AddGroundObject(kalya_sprite);
end

-- The heal particle effect map object
local heal_effect = {};

function _CreateObjects()
    local object = {}
    local npc = {}

    Map:AddSavePoint(19, 27);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject("dat/effects/particles/heal_particle.lua", 0, 0);
    heal_effect:SetObjectID(Map.object_supervisor:GenerateObjectID());
    heal_effect:Stop(); -- Don't run it until the character heals itself
    Map:AddGroundObject(heal_effect);

    -- Heal point
    npc = CreateSprite(Map, "Butterfly", 27, 23);
    npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    npc:SetVisible(false);
    npc:SetName(""); -- Unset the speaker name
    Map:AddGroundObject(npc);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Your party feels better...");
    dialogue:AddLineEvent(text, npc, "Forest entrance heal", "");
    DialogueManager:AddDialogue(dialogue);
    npc:AddDialogueReference(dialogue);

    -- Only add the squirrels and butterflies when the night isn't about to happen
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") < 1) then
        npc = CreateSprite(Map, "Butterfly", 42, 18);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(npc);
        event = vt_map.RandomMoveSpriteEvent("Butterfly1 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Butterfly1 random move", 4500); -- Loop on itself
        EventManager:RegisterEvent(event);
        EventManager:StartEvent("Butterfly1 random move");

        npc = CreateSprite(Map, "Butterfly", 12, 30);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(npc);
        event = vt_map.RandomMoveSpriteEvent("Butterfly2 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Butterfly2 random move", 4500); -- Loop on itself
        EventManager:RegisterEvent(event);
        EventManager:StartEvent("Butterfly2 random move", 2400);

        npc = CreateSprite(Map, "Butterfly", 50, 25);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(npc);
        event = vt_map.RandomMoveSpriteEvent("Butterfly3 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Butterfly3 random move", 4500); -- Loop on itself
        EventManager:RegisterEvent(event);
        EventManager:StartEvent("Butterfly3 random move", 1050);

        npc = CreateSprite(Map, "Butterfly", 40, 30);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(npc);
        event = vt_map.RandomMoveSpriteEvent("Butterfly4 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Butterfly4 random move", 4500); -- Loop on itself
        EventManager:RegisterEvent(event);
        EventManager:StartEvent("Butterfly4 random move", 3050);

        npc = CreateSprite(Map, "Squirrel", 18, 24);
        -- Squirrels don't collide with the npcs.
        npc:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        npc:SetSpriteAsScenery(true);
        Map:AddGroundObject(npc);
        event = vt_map.RandomMoveSpriteEvent("Squirrel1 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Squirrel1 random move", 4500); -- Loop on itself
        EventManager:RegisterEvent(event);
        EventManager:StartEvent("Squirrel1 random move");

        npc = CreateSprite(Map, "Squirrel", 40, 16);
        -- Squirrels don't collide with the npcs.
        npc:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        npc:SetSpriteAsScenery(true);
        Map:AddGroundObject(npc);
        event = vt_map.RandomMoveSpriteEvent("Squirrel2 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Squirrel2 random move", 4500); -- Loop on itself
        EventManager:RegisterEvent(event);
        EventManager:StartEvent("Squirrel2 random move", 1800);
    else
        -- add fireflies near the statue at night
        object = vt_map.ParticleObject("dat/effects/particles/fireflies.lua", 27, 22);
        object:SetObjectID(Map.object_supervisor:GenerateObjectID());
        Map:AddGroundObject(object);
        object = vt_map.ParticleObject("dat/effects/particles/fireflies.lua", 25, 23);
        object:SetObjectID(Map.object_supervisor:GenerateObjectID());
        Map:AddGroundObject(object);
        object = vt_map.ParticleObject("dat/effects/particles/fireflies.lua", 29, 22.5);
        object:SetObjectID(Map.object_supervisor:GenerateObjectID());
        Map:AddGroundObject(object);
    end

    object = CreateObject(Map, "Tree Small3", 23, 18);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 40, 14);
    Map:AddGroundObject(object);

    -- Trees above the pathway
    object = CreateObject(Map, "Tree Big1", 4, 41);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small1", 5, 21);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Big2", 12, 7);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small2", 1, 17.2);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Rock2", 1, 27);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Rock2", 5, 27);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Rock2", 1, 33);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Rock2", 5, 33);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 3, 5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small5", 8, 6);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small6", 11, 2);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small4", 15, 4.5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small6", 19, 6.2);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 23, 7.2);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small5", 26, 4);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 30, 5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small4", 34, 7);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 36, 8);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small6", 39, 9);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small6", 42, 7.5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 45, 6);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small5", 48, 10);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small4", 47.5, 8);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 50, 12);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small5", 54, 15);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 59, 17);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small4", 62, 20);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small6", 62, 12);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 54, 8);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small5", 60, 4);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small4", 55, 20);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 61, 26);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small6", 45, 23);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small5", 11, 20);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 2, 24);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 3, 14);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small6", 7, 10);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small4", 2, 8);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small5", 4, 17);
    Map:AddGroundObject(object);

    -- Trees below the pathway
    object = CreateObject(Map, "Tree Small3", 2, 40);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small6", 7, 38);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small5", 9, 42);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small4", 4, 47);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 12, 41);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 11, 45);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 14, 46);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small4", 16.5, 48);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 19, 46.5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small5", 23, 48);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 26, 39);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small6", 36, 43);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 27, 49);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small4", 30.5, 51);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small5", 33, 50);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 37, 48.5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small4", 40.5, 51);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 44, 50.5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small6", 47, 49.5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 49.5, 48.5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small4", 52, 50);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 55, 48);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 58, 50.5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small5", 58, 47);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small3", 62, 46.5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Small5", 62, 49);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Little2", 15, 8);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Tiny4", 29, 37);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Little3", 16, 40.2);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Little1", 9, 12);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Tiny3", 58, 22);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Little1", 58, 9);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Tiny3", 28, 8.5);
    Map:AddGroundObject(object);
    object = CreateObject(Map, "Tree Little3", 11, 44);
    Map:AddGroundObject(object);
end

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(49, 62, 26, 39);

    if (GlobalManager:GetEventValue("story", "layna_forest_twilight_value") >= 6) then
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
        roam_zone:AddEnemy(enemy, Map, 1);
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
        roam_zone:AddEnemy(enemy, Map, 1);
    end

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

    -- Music event
    event = vt_map.ScriptedEvent("Music start", "music_start", "");
    EventManager:RegisterEvent(event);

    -- Triggered events
    event = vt_map.MapTransitionEvent("exit forest", "dat/maps/layna_village/layna_village_center_map.lua",
                                       "dat/maps/layna_village/layna_village_center_script.lua", "from_layna_forest_entrance");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to forest NW", "dat/maps/layna_forest/layna_forest_north_west_map.lua",
                                       "dat/maps/layna_forest/layna_forest_north_west_script.lua", "from_layna_forest_entrance");
    EventManager:RegisterEvent(event);

    -- After the forest dungeon
    event = vt_map.MapTransitionEvent("exit forest at night", "dat/maps/layna_village/layna_village_center_map.lua",
                                       "dat/maps/layna_village/layna_village_center_at_night_script.lua", "from_layna_forest_entrance");
    EventManager:RegisterEvent(event);

    -- Heal point
    event = vt_map.ScriptedEvent("Forest entrance heal", "heal_party", "heal_done");
    EventManager:RegisterEvent(event);

    -- Generic events
    event = vt_map.ScriptedEvent("Map:Popstate()", "Map_PopState", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("kalya:SetCollision(ALL)", kalya_sprite, "Sprite_Collision_on", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("hero:SetCollision(ALL)", hero, "Sprite_Collision_on", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("second_hero:SetCollision(NONE)", kalya_sprite, "Sprite_Collision_off", "");
    EventManager:RegisterEvent(event);

    event = vt_map.LookAtSpriteEvent("Kalya looks at Bronann", kalya_sprite, hero);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Bronann looks at Kalya", hero, kalya_sprite);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Kalya looks at the save point", kalya_sprite, 19, 26);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Kalya looks at the spring", kalya_sprite, 27, 23);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Bronann looks at the save point", hero, 19, 26);
    EventManager:RegisterEvent(event);

    -- First time forest entrance dialogue about save points and the heal spring.
    event = vt_map.ScriptedEvent("Forest entrance dialogue", "forest_save_point_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 50);
    EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event = vt_map.PathMoveSpriteEvent("Kalya moves next to Bronann", kalya_sprite, 0, 0, false);
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya looks at the save point");
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya is surprised before running to the save point");
    move_next_to_hero_event:AddEventLinkAtEnd("kalya:SetCollision(ALL)");
    EventManager:RegisterEvent(move_next_to_hero_event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("What's that?!");
    dialogue:AddLineEmote(text, kalya_sprite, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya is surprised before running to the save point", dialogue);
    event:AddEventLinkAtEnd("Kalya runs to the save point");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Kalya runs to the save point", kalya_sprite, 15, 26, true);
    event:AddEventLinkAtEnd("Bronann runs to the save point");
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Bronann runs to the save point", hero, 15, 28, true);
    event:AddEventLinkAtEnd("Kalya starts to talk about the save point");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Can you also see this strange circle surrounded by light?");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Kalya looks at Bronann", "", "exclamation");
    text = vt_system.Translate("When I'm standing near you, I can.");
    dialogue:AddLineEventEmote(text, hero, "Bronann looks at Kalya", "", "thinking dots");
    text = vt_system.Translate("Strange... it sounds familiar to me. Like... A safe place to be.");
    dialogue:AddLineEvent(text, kalya_sprite, "Kalya looks at the save point", "");
    text = vt_system.Translate("I feel like I can go in there...");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("Be careful, Kalya!");
    dialogue:AddLineEmote(text, hero, "exclamation");
    text = vt_system.Translate("It is like... it's calling me.");
    dialogue:AddLine(text, kalya_sprite);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya starts to talk about the save point", dialogue);
    event:AddEventLinkAtEnd("Kalya moves into the save point");
    event:AddEventLinkAtEnd("Bronann looks at the save point");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Kalya moves into the save point", kalya_sprite, 19, 26, false);
    event:AddEventLinkAtEnd("Kalya talks about the save point - part 2", 1000);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("See... It brings no harm.");
    dialogue:AddLineEvent(text, kalya_sprite, "Kalya looks at Bronann", "");
    text = vt_system.Translate("Err... Are you sure?");
    dialogue:AddLineEmote(text, hero, "sweat drop");
    text = vt_system.Translate("Sure! Do you trust me?");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("... I do trust you, Kalya.");
    dialogue:AddLine(text, hero);
    text = vt_system.VTranslate("If so, then come and try for yourself. Push '%s' and you'll feel safe, too.", InputManager:GetConfirmKeyName());
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("Ok, I... I'll do it.");
    dialogue:AddLineEmote(text, hero, "sweat drop");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya talks about the save point - part 2", dialogue);
    event:AddEventLinkAtEnd("Kalya moves slightly right of the save point");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Kalya moves slightly right of the save point", kalya_sprite, 20, 26, false);
    event:AddEventLinkAtEnd("Kalya talks about the save point - part 3", 1000);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Oh, I almost forgot, have you seen up here? There is a Layna spring! We can heal our wounds there.");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Kalya looks at the spring", "", "exclamation");
    text = vt_system.VTranslate("Just stand in front of the goddess statue below the spring and push '%s'.", InputManager:GetConfirmKeyName());
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("Ok, thanks.");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("... What are you waiting for? Come.");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Kalya looks at Bronann", "", "thinking dots");
    text = vt_system.Translate("Huh? I... I'm coming.");
    dialogue:AddLineEmote(text, hero, "sweat drop");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya talks about the save point - part 3", dialogue);
    event:AddEventLinkAtEnd("Bronann moves into the save point");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Bronann moves into the save point", hero, 18, 26, false);
    event:AddEventLinkAtEnd("Kalya talks about the save point - part 4", 1000);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("You're right. This place makes me feel good.");
    dialogue:AddLineEventEmote(text, hero, "Bronann looks at Kalya", "", "thinking dots");
    text = vt_system.Translate("See? Now let's find my brother before he gets hurt.");
    dialogue:AddLine(text, kalya_sprite);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya talks about the save point - part 4", dialogue);
    event:AddEventLinkAtEnd("second_hero:SetCollision(NONE)");
    event:AddEventLinkAtEnd("Set Camera");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Set Camera", hero, "SetCamera", "");
    event:AddEventLinkAtEnd("2nd hero goes back to party");
    EventManager:RegisterEvent(event);

    move_back_to_hero_event = vt_map.PathMoveSpriteEvent("2nd hero goes back to party", kalya_sprite, hero, false);
    move_back_to_hero_event:AddEventLinkAtEnd("Map:Popstate()");
    move_back_to_hero_event:AddEventLinkAtEnd("end of save point event");
    EventManager:RegisterEvent(move_back_to_hero_event);

    event = vt_map.ScriptedEvent("end of save point event", "end_of_save_point_event", "");
    EventManager:RegisterEvent(event);
end

-- zones
local forest_entrance_exit_zone = {};
local to_forest_nw_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    forest_entrance_exit_zone = vt_map.CameraZone(0, 1, 26, 34);
    Map:AddZone(forest_entrance_exit_zone);

    to_forest_nw_zone = vt_map.CameraZone(62, 64, 29, 35);
    Map:AddZone(to_forest_nw_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (forest_entrance_exit_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 0) then
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
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/forest_background.png");

    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") < 1) then
        -- Add tutorial battle dialog with Kalya and Bronann
        enemy:AddBattleScript("dat/battles/tutorial_battle_dialogs.lua");
    else
        -- Setup time of the day lighting on battles
        enemy:AddBattleScript("dat/maps/layna_forest/after_crystal_twilight_battles.lua");
        if (GlobalManager:GetEventValue("story", "layna_forest_twilight_value") > 2) then
            enemy:SetBattleBackground("img/backdrops/battle/forest_background_evening.png");
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
        GlobalManager:GetActiveParty():AddHitPoints(10000);
        GlobalManager:GetActiveParty():AddSkillPoints(10000);
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

    -- Kalya runs to the save point and tells Bronann about the spring.
    forest_save_point_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya_sprite:SetVisible(true);
        kalya_sprite:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        hero:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        Map:SetCamera(kalya_sprite, 800);

        move_next_to_hero_event:SetDestination(hero:GetXPosition(), hero:GetYPosition() + 2.0, false);
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

    end_of_save_point_event = function()
        kalya_sprite:SetPosition(0, 0);
        kalya_sprite:SetVisible(false);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);

        -- Set event as done
        GlobalManager:SetEventValue("story", "kalya_save_points_n_spring_speech_done", 1);
    end,

    music_start = function()
        -- If the night has fallen, let the music change
        if (GlobalManager:GetEventValue("story", "layna_forest_twilight_value") >= 6) then
            AudioManager:PlayMusic("mus/forest_at_night.ogg");
        end
    end
}
