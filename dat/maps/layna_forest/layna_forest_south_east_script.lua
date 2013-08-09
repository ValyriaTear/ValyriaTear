-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_south_east_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest"
map_image_filename = "img/menus/locations/layna_forest.png"
map_subname = ""

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

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

    Map.unlimited_stamina = false;
    Map:SetMinimapImage("dat/maps/layna_forest/minimaps/layna_forest_south_east_minimap.png");

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

    _HandleTwilight();
end

-- Handle the twilight advancement after the crystal scene
function _HandleTwilight()

    -- If the characters have seen the crystal, then it's time to make the twilight happen
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") < 1) then
        return;
    end

    -- test if the day time is sufficiently advanced
    if (GlobalManager:GetEventValue("story", "layna_forest_twilight_value") < 1) then
        GlobalManager:SetEventValue("story", "layna_forest_twilight_value", 1);
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
    -- Default hero and position - from forest North East
    hero = CreateSprite(Map, "Bronann", 40, 4);
    hero:SetDirection(vt_map.MapMode.SOUTH);
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
    elseif (GlobalManager:GetPreviousLocation() == "from forest SW") then
        hero:SetDirection(vt_map.MapMode.EAST);
        hero:SetPosition(4, 54);
    elseif (GlobalManager:GetPreviousLocation() == "from_layna_cave_1_2") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(14, 42);
    elseif (GlobalManager:GetPreviousLocation() == "from_layna_wolf_cave") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(32, 21);
    elseif (GlobalManager:GetPreviousLocation() == "from layna forest cave 2") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(66, 72);
    end

    Map:AddGroundObject(hero);
end

-- The heal particle effect map object
local heal_effect = {};

function _CreateObjects()
    local object = {}
    local npc = {}

    -- Save point
    Map:AddSavePoint(61, 9);

    -- Load the heal effect.
    heal_effect = vt_map.ParticleObject("dat/effects/particles/heal_particle.lua", 0, 0);
    heal_effect:SetObjectID(Map.object_supervisor:GenerateObjectID());
    heal_effect:Stop(); -- Don't run it until the character heals itself
    Map:AddGroundObject(heal_effect);

    -- Heal point
    npc = CreateSprite(Map, "Butterfly", 67, 11);
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

        npc = CreateSprite(Map, "Squirrel", 40, 14);
        -- Squirrels don't collide with the npcs.
        npc:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        npc:SetSpriteAsScenery(true);
        Map:AddGroundObject(npc);
        event = vt_map.RandomMoveSpriteEvent("Squirrel2 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Squirrel2 random move", 4500); -- Loop on itself
        EventManager:RegisterEvent(event);
        EventManager:StartEvent("Squirrel2 random move", 1800);

    end

    -- Treasures
    local chest1 = CreateTreasure(Map, "layna_forest_SE_chest1", "Wood_Chest1", 63.7, 30);
    if (chest1 ~= nil) then
        chest1:AddObject(40001, 1); -- Prismatic ring
        Map:AddGroundObject(chest1);
    end

    chest1 = CreateTreasure(Map, "layna_forest_SE_chest2", "Wood_Chest1", 88, 80);
    if (chest1 ~= nil) then
        chest1:AddObject(11, 2); -- Small Moon juice potion x 2
        chest1:SetDrunes(15);
        Map:AddGroundObject(chest1);
    end

    -- Trees array
    local map_trees = {
        --  right entrance upper side -left
        { "Tree Small3", 34, 3 },
        { "Tree Small3", 35, 6 },
        { "Tree Small5", 35, 9 },
        { "Tree Small3", 36, 12 },
        { "Tree Small4", 38, 15 },
        { "Tree Small3", 41, 18 },
        { "Tree Small6", 43, 20 },
        { "Tree Small5", 44, 22 },
        { "Tree Small3", 46, 24 },
        { "Tree Small4", 49, 27 },
        { "Tree Small3", 50, 30 },
        { "Tree Small5", 54, 32 },
        { "Tree Small3", 59, 31 },
        { "Tree Small4", 63, 28 },
        { "Tree Small3", 69, 27 },
        { "Tree Small4", 69.5, 30 },
        { "Tree Small5", 64, 32 },
        { "Tree Small4", 73, 26 },
        { "Tree Small3", 76, 24 },
        { "Tree Small6", 78, 21 },
        { "Tree Small3", 81, 22 },
        { "Tree Small6", 84, 21.2 },
        { "Tree Small5", 87, 20 },
        { "Tree Small4", 89, 19 },
        { "Tree Small3", 92, 18 },
        { "Tree Small4", 95, 17.5 },
        { "Tree Small5", 97, 16 },
        { "Tree Small6", 100, 17 },
        { "Tree Small5", 102, 18 },
        { "Tree Small5", 105, 16.5 },
        { "Tree Small3", 108, 17.1 },
        { "Tree Small4", 110, 16 },
        { "Tree Small4", 112, 14 },
        { "Tree Small5", 114, 13 },
        { "Tree Small6", 116, 10 },
        { "Tree Small3", 118, 11 },

        -- right
        { "Tree Small4", 43, 2 },
        { "Tree Small3", 45, 5 },
        { "Tree Small5", 46, 8 },
        { "Tree Small3", 49, 2 },
        { "Tree Small4", 49, 10 },
        { "Tree Small5", 49, 14 },
        { "Tree Small6", 52, 13 },
        { "Tree Small4", 54, 18 },
        { "Tree Small3", 55, 16 },
        { "Tree Small4", 56, 1 },
        { "Tree Small3", 58, 19 },
        { "Tree Small6", 56, 10.2 },
        { "Tree Small4", 53, 4 },
        { "Tree Small3", 62, 16 },
        { "Tree Small5", 62.4, 20 },
        { "Tree Small3", 61, 5 },
        { "Tree Small3", 65, 1 },
        { "Tree Small3", 70, 3 },
        { "Tree Small4", 73, 5 },
        { "Tree Small3", 72, 11 },
        { "Tree Small5", 71.5, 15 },
        { "Tree Small3", 76, 8 },
        { "Tree Small6", 78, 1 },
        { "Tree Small3", 65, 17 },
        { "Tree Small4", 82, 11.1 },
        { "Tree Small3", 83, 1 },
        { "Tree Small5", 87, 2 },
        { "Tree Small3", 91, 10 },
        { "Tree Small6", 92, 1 },
        { "Tree Small3", 95, 4 },
        { "Tree Small4", 100, 2 },
        { "Tree Small3", 102, 12 },
        { "Tree Small5", 104, 1 },
        { "Tree Small3", 108, 7 },
        { "Tree Small6", 111, 2 },
        { "Tree Small3", 115, 1 },
        { "Tree Small5", 121, 2 },
        { "Tree Small3", 125, 1 },
        { "Tree Small4", 126, 4 },
        { "Tree Small3", 117, 13.1 },
        { "Tree Small5", 118, 18.9 },
        { "Tree Small3", 98, 19 },
        { "Tree Small6", 88, 21 },
        { "Tree Small3", 82, 23 },
        { "Tree Small5", 77, 30 },
        { "Tree Small4", 56, 33 },

        -- going left
        { "Tree Small3", 121, 42 },
        { "Tree Small4", 117, 39 },
        { "Tree Small5", 115, 37 },
        { "Tree Small3", 110, 43 },
        { "Tree Small6", 111, 35 },
        { "Tree Small3", 107, 33 },
        { "Tree Small5", 103, 34 },
        { "Tree Small4", 116, 24 },
        { "Tree Small3", 113, 28.1 },
        { "Tree Small4", 105, 22 },
        { "Tree Small5", 98, 25 },
        { "Tree Small6", 94, 20 },
        { "Tree Small3", 99, 31 },
        { "Tree Small4", 94, 30 },
        { "Tree Small5", 90, 30.5 },
        { "Tree Small3", 86, 28 },
        { "Tree Small4", 82, 29 },

        -- After path break bottom part
        { "Tree Small3", 68, 33 },
        { "Tree Small6", 82, 35 },
        { "Tree Small6", 76, 36 },
        { "Tree Small6", 72, 35.1 },
        { "Tree Small5", 80, 38 },
        { "Tree Small5", 83, 41 },
        { "Tree Small5", 86, 42 },
        { "Tree Small3", 90, 45 },
        { "Tree Small4", 93, 48 },
        { "Tree Small5", 96, 51 },
        { "Tree Small6", 94, 54 },
        { "Tree Small5", 95, 57 },
        { "Tree Small6", 96, 60 },
        { "Tree Small5", 97, 63 },
        { "Tree Small4", 98, 66 },
        { "Tree Small3", 97, 69 },
        { "Tree Small4", 94, 72 },
        { "Tree Small3", 96, 75 },
        { "Tree Small5", 95, 78 },
        { "Tree Small6", 96, 81 },
        { "Tree Small3", 99, 82 },

        -- Going left
        { "Tree Small3", 95, 84 },
        { "Tree Small4", 91, 86 },
        { "Tree Small3", 89, 87 },
        { "Tree Small5", 89, 91 },
        { "Tree Small3", 83, 91.2 },
        { "Tree Small4", 85, 85 },
        { "Tree Small4", 82, 87 },
        { "Tree Small3", 78, 86 },
        { "Tree Small5", 75, 84 },
        { "Tree Small3", 70, 85 },
        { "Tree Small5", 65, 86 },
        { "Tree Tiny3", 64, 91.2 },
        { "Tree Small4", 60, 83 },
        { "Tree Small4", 55, 85.1 },
        { "Tree Small5", 50, 84 },
        { "Tree Small3", 46, 86 },
        { "Tree Small5", 41, 85 },
        { "Tree Small3", 36, 87 },
        { "Tree Small6", 39, 92 },
        { "Tree Small3", 31, 84 },
        { "Tree Small5", 30, 91 },
        { "Tree Small3", 30, 80 },
        { "Tree Small5", 29, 86 },
        { "Tree Small3", 25, 81 },
        { "Tree Small4", 22, 78 },
        { "Tree Small3", 21, 74 },
        { "Tree Small6", 16, 72 },
        { "Tree Small3", 17, 68 },
        { "Tree Small6", 18, 64 },
        { "Tree Small3", 17, 60 },
        { "Tree Small5", 15, 57 },
        { "Tree Small3", 12, 53 },
        { "Tree Small6", 11, 57.2 },
        { "Tree Small3", 8, 50 },
        { "Tree Small4", 4, 48 },
        { "Tree Small3", 2, 47 },

        -- in the way
        { "Tree Small3", 95, 37 },
        { "Tree Small4", 103, 47 },
        { "Tree Small5", 120, 48 },
        { "Tree Small6", 116, 54 },
        { "Tree Small5", 101, 56 },
        { "Tree Small4", 108, 61 },
        { "Tree Small3", 104, 65 },
        { "Tree Small4", 110, 68 },
        { "Tree Small5", 120, 69 },
        { "Tree Small6", 118, 75 },
        { "Tree Small5", 104, 74 },
        { "Tree Small4", 113, 82 },
        { "Tree Small3", 118, 89.5 },
        { "Tree Small3", 107, 93 },
        { "Tree Small4", 88, 78 },
        { "Tree Small5", 122, 56 },
        { "Tree Small6", 74, 87 },
        { "Tree Small5", 4, 52 },
        { "Tree Small4", 8, 80 },
        { "Tree Small3", 9, 70 },
        { "Tree Small3", 22, 67.5 },
        { "Tree Small4", 30, 72 },
        { "Tree Small3", 36, 77 },
        { "Tree Small4", 17, 55 },
        { "Tree Small3", 20, 61 },
        { "Tree Small5", 20, 71 },
        { "Tree Small5", 19.5, 76 },
        { "Tree Small3", 25, 84 },

        -- map right border tree wall
        { "Tree Small3", 127, 7 },
        { "Tree Small4", 126, 10 },
        { "Tree Small5", 128, 13 },
        { "Tree Small6", 129, 15 },
        { "Tree Small5", 126, 18 },
        { "Tree Small4", 125, 20 },
        { "Tree Small3", 124, 23 },
        { "Tree Small4", 125, 25 },
        { "Tree Small5", 126, 30 },
        { "Tree Small5", 124, 34 },
        { "Tree Small6", 125, 38 },
        { "Tree Small5", 123, 40 },
        { "Tree Small6", 126, 42 },
        { "Tree Small5", 124, 44 },
        { "Tree Small4", 126, 4 },
        { "Tree Small3", 126, 47 },
        { "Tree Small6", 125, 50 },
        { "Tree Small3", 127, 53 },
        { "Tree Small4", 126, 57 },
        { "Tree Small3", 125, 60 },
        { "Tree Small5", 128, 63 },
        { "Tree Small3", 127, 66 },
        { "Tree Small4", 125, 69.2 },
        { "Tree Small3", 126, 72 },
        { "Tree Small6", 127, 75 },
        { "Tree Small3", 128, 78 },
        { "Tree Small5", 129, 80 },
        { "Tree Small3", 126, 83 },
        { "Tree Small4", 125, 86 },
        { "Tree Small6", 124, 89 },
        { "Tree Small5", 123, 92 },
        { "Tree Small3", 121, 95 },
        { "Tree Small5", 119, 98 },
        { "Tree Small4", 127, 89.2 },
        { "Tree Small3", 126, 93 },
        { "Tree Small6", 127, 96 },
        { "Tree Small5", 124, 95.5 },

        -- map border bottom
        { "Tree Small4", 115, 97 },
        { "Tree Small5", 110, 98 },
        { "Tree Small4", 105, 99 },
        { "Tree Small3", 99, 96 },
        { "Tree Small6", 94, 97 },
        { "Tree Small4", 91, 99 },
        { "Tree Small5", 86, 98 },
        { "Tree Small4", 81, 97 },
        { "Tree Small4", 75, 96 },
        { "Tree Small4", 70, 94 },
        { "Tree Small3", 66, 97 },
        { "Tree Small6", 61, 96 },
        { "Tree Small3", 56, 98 },
        { "Tree Small5", 51, 97 },
        { "Tree Small3", 47, 96 },
        { "Tree Small4", 42, 98 },
        { "Tree Small3", 36, 99 },
        { "Tree Small5", 32, 98 },
        { "Tree Small3", 26, 97 },
        { "Tree Small6", 22, 95 },
        { "Tree Small3", 16, 94 },
        { "Tree Small4", 14, 92 },
        { "Tree Small3", 8, 91 },
        { "Tree Small5", 4, 89 },
        { "Tree Small3", 1, 88 },
        { "Tree Small6", 4, 92 },
        { "Tree Small3", 0, 91 },
        { "Tree Small5", 2, 95 },
        { "Tree Small3", 9, 93 },
        { "Tree Small4", 12, 95.2 },
        { "Tree Small3", 18, 97 },
        { "Tree Small5", 8, 98 },
        { "Tree Small6", 4.5, 96 },
        { "Tree Small3", 20, 100 },

        -- left wall
        { "Tree Small3", 0, 82 },
        { "Tree Small4", 2, 79 },
        { "Tree Small3", 0, 76 },
        { "Tree Small5", -1, 73 },
        { "Tree Small3", -2, 70 },
        { "Tree Small6", 2, 68 },
        { "Tree Small3", 1, 64 },

        -- Starting inner part
        { "Tree Small4", 4, 43 },
        { "Tree Small3", 7, 41 },
        { "Tree Small6", 10, 42 },
        { "Tree Small3", 12, 36 },
        { "Tree Small4", 16, 32 },
        { "Tree Small5", 19, 29 },
        { "Tree Small3", 22, 26 },
        { "Tree Small5", 21, 22 },
        { "Tree Small3", 25, 19 },
        { "Tree Small6", 22, 16 },
        { "Tree Small3", 24, 14 },
        { "Tree Small4", 25, 11 },
        { "Tree Small4", 27, 8 },
        { "Tree Small4", 30, 5 },
        { "Tree Little4", 16, 35 },
        { "Tree Tiny2", 14, 35.5 },

        -- Left part - final passage
        { "Tree Small3", 0, 32 },
        { "Tree Small5", 2, 29 },
        { "Tree Small3", -1, 26 },
        { "Tree Small6", 1, 23 },
        { "Tree Small4", 2, 20 },
        { "Tree Small3", 3, 17 },
        { "Tree Small3", -1, 15 },
        { "Tree Small4", 1, 12 },
        { "Tree Small4", 0, 9 },
        { "Tree Small4", 1, 7 },
        { "Tree Small4", 0, 4 },
        { "Tree Small4", -1, 1 },

        -- Upper part of final passage
        { "Tree Small4", 6, 3 },
        { "Tree Small4", 9, 1 },
        { "Tree Small4", 16, 3 },
        { "Tree Small4", 19, 2 },
        { "Tree Small4", 21, 1 },
        { "Tree Small4", 24, 2 },
        { "Tree Little3", 9, 5 },
        { "Tree Tiny3", 20, 4 },
        { "Tree Small4", 16, 8 },
        { "Tree Small4", 7, 11 },
        { "Tree Little3", 6, 15.2 },
        { "Tree Little3", 20, 10 },
        { "Tree Tiny1", 9, 20.2 },
        { "Tree Little2", 18, 15.4 },
        { "Tree Small5", 6, 24 },

        -- Working on inner part of map
        { "Tree Little1", 17, 41 },
        { "Tree Tiny1", 19, 52 },
        { "Tree Small5", 21, 49 },
        { "Tree Little2", 28, 46 },
        { "Tree Tiny1", 23, 51 },
        { "Tree Small3", 24, 45 },
        { "Tree Small6", 26, 49.5 },
        { "Tree Small4", 16, 50 },
        { "Tree Small4", 28, 41 },
        { "Tree Tiny4", 31, 42 },
        { "Tree Little4", 21.5, 30 },
        { "Tree Small3", 30, 38 },
        { "Tree Small4", 32, 35 },
        { "Tree Small4", 28, 32 },
        { "Tree Small4", 33, 29 },
        { "Tree Small3", 30, 10 },
        { "Tree Tiny3", 28, 12 },
        { "Tree Little1", 32.3, 11 },
        { "Tree Little3", 28, 20.2 },
        { "Tree Little2", 36, 19 },
        { "Tree Tiny1", 38, 17 },

        -- Right inner part (with trees here and there and hills)
        { "Tree Small3", 35, 31 },
        { "Tree Small3", 40, 34 },
        { "Tree Small5", 41, 45 },
        { "Tree Little2", 36, 36 },
        { "Tree Tiny2", 44, 39 },
        { "Tree Small3", 57, 39 },
        { "Tree Small6", 63, 42 },
        { "Tree Small3", 43, 49 },
        { "Tree Small4", 33, 53 },
        { "Tree Small3", 32, 62 },
        { "Tree Small4", 52, 56 },
        { "Tree Small3", 55, 45 },
        { "Tree Small6", 72, 48 },
        { "Tree Small3", 83, 53 },
        { "Tree Small5", 71, 59 },
        { "Tree Small3", 77, 66 },
        { "Tree Little3", 51, 86 },
        { "Tree Small3", 76, 74 },
        { "Tree Small4", 53, 68 },
        { "Tree Small3", 41, 65 },
        { "Tree Little1", 46, 25 },
        { "Tree Tiny4", 53, 34 },
        { "Tree Little4", 35, 45 },
        { "Tree Little2", 61, 33 },
        { "Tree Little1", 45, 28 },
        { "Tree Tiny2", 39, 20 },
        { "Tree Little3", 53, 26 },
        { "Tree Tiny3", 49, 43 },
        { "Tree Tiny4", 60, 47 },
        { "Tree Tiny1", 71, 40 },
        { "Tree Little1", 68, 35 },
        { "Tree Tiny4", 88, 37 },
        { "Tree Little1", 73, 37 },
        { "Tree Small3", 78, 41.4 },
        { "Tree Little4", 82, 45 },
        { "Tree Tiny1", 89, 47 },
        { "Tree Small5", 89, 55 },
        { "Tree Little4", 91, 57.2 },
        { "Tree Small3", 79, 58 },
        { "Tree Tiny3", 86, 69 },
        { "Tree Little1", 81, 75 },
        { "Tree Small3", 92, 64 },
        { "Tree Little3", 91, 79.2 },
        { "Tree Tiny1", 91, 71.5 },
        { "Tree Tiny3", 80, 80 },
        { "Tree Small3", 66, 79 },
        { "Tree Small3", 88, 68 },
        { "Tree Small4", 72, 79.2 },
        { "Tree Tiny3", 62, 78 },
        { "Tree Little3", 91, 39 },
        { "Tree Tiny2", 56, 78 },
        { "Tree Small6", 52, 77 },
        { "Tree Small4", 69, 64 },
        { "Tree Small6", 44, 81 },
        { "Tree Little1", 37, 79 },
        { "Tree Little2", 25, 71 },
        { "Tree Little4", 24, 61.2 },
        { "Tree Little2", 26.5, 54 },
        { "Tree Tiny3", 35, 64 },
        { "Tree Tiny2", 46, 52 },
        { "Tree Little1", 40, 55 },
        { "Tree Little3", 64, 52 },
        { "Tree Little4", 57, 58 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_trees) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        Map:AddGroundObject(object);
    end

    -- Trees array
    local map_grass = {
        --  right entrance upper side -left
        { "Grass Clump1", 32, 19 },
        { "Grass Clump1", 14, 40.5 },
        { "Grass Clump1", 67, 71 },
        { "Grass Clump1", 65, 72 },
        { "Grass Clump1", 76, 50 },
        { "Grass Clump1", 65, 55 }
    }

    -- Loads the grass clumps according to the array
    for my_index, my_array in pairs(map_grass) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(object);
    end

end

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

    -- Hint: left, right, top, bottom

    -- top right
    roam_zone = vt_map.EnemyZone(77, 123, 2, 5);

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

    Map:AddZone(roam_zone);

    roam_zone = vt_map.EnemyZone(106, 121, 18, 25);

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

    Map:AddZone(roam_zone);

    -- wide passage
    roam_zone = vt_map.EnemyZone(97, 118, 40, 89);

    enemy = CreateEnemySprite(Map, "slime");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(2);
    roam_zone:AddEnemy(enemy, Map, 2);

    Map:AddZone(roam_zone);

    -- near the exit
    roam_zone = vt_map.EnemyZone(12, 14, 77, 79);

    enemy = CreateEnemySprite(Map, "spider");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(2);
    roam_zone:AddEnemy(enemy, Map, 1);

    Map:AddZone(roam_zone);

    -- in the inner part
    roam_zone = vt_map.EnemyZone(24, 84, 46, 78);

    enemy = CreateEnemySprite(Map, "slime");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(2);
    roam_zone:AddEnemy(enemy, Map, 3);

    Map:AddZone(roam_zone);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- Map events
    event = vt_map.MapTransitionEvent("to forest NE", "dat/maps/layna_forest/layna_forest_north_east_map.lua",
                                       "dat/maps/layna_forest/layna_forest_north_east_script.lua", "from forest SE")
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to forest SW", "dat/maps/layna_forest/layna_forest_south_west_map.lua",
                                       "dat/maps/layna_forest/layna_forest_south_west_script.lua", "from forest SE")
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to cave 1_2", "dat/maps/layna_forest/layna_forest_cave1_2_map.lua",
                                       "dat/maps/layna_forest/layna_forest_cave1_2_script.lua", "from forest SE")
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to wolf cave", "dat/maps/layna_forest/layna_forest_wolf_cave_map.lua",
                                       "dat/maps/layna_forest/layna_forest_wolf_cave_script.lua", "from forest SE")
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to cave 2", "dat/maps/layna_forest/layna_forest_cave2_map.lua",
                                       "dat/maps/layna_forest/layna_forest_cave2_script.lua", "from forest SE")
    EventManager:RegisterEvent(event);

    -- Heal point
    event = vt_map.ScriptedEvent("Forest entrance heal", "heal_party", "heal_done");
    EventManager:RegisterEvent(event);
end

-- zones
local to_forest_NE_zone = {};
local to_forest_SW_zone = {};
local to_cave1_2_zone = {};
local to_cave2_1_zone = {};
local to_wolf_cave_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_forest_NE_zone = vt_map.CameraZone(36, 41, 0, 2);
    Map:AddZone(to_forest_NE_zone);

    to_forest_SW_zone = vt_map.CameraZone(0, 2, 52, 56);
    Map:AddZone(to_forest_SW_zone);

    to_cave1_2_zone = vt_map.CameraZone(12, 16, 39, 40);
    Map:AddZone(to_cave1_2_zone);

    to_cave2_1_zone = vt_map.CameraZone(64, 68, 69, 70);
    Map:AddZone(to_cave2_1_zone);

    to_wolf_cave_zone = vt_map.CameraZone(30, 34, 17, 18);
    Map:AddZone(to_wolf_cave_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_forest_NE_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest NE");
    elseif (to_forest_SW_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest SW");
    elseif (to_cave1_2_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to cave 1_2");
    elseif (to_wolf_cave_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to wolf cave");
    elseif (to_cave2_1_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to cave 2");
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
        if (GlobalManager:GetEventValue("story", "layna_forest_twilight_value") > 2 ) then
            enemy:SetBattleBackground("img/backdrops/battle/forest_background_evening.png");
        end
    end
end

-- Effect time used when applying the heal light effect
local heal_effect_time = 0;
local heal_color = vt_video.Color(0.0, 0.0, 1.0, 1.0);

-- Map Custom functions
-- Used through scripted events
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

}
