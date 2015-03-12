-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_north_east_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest"
map_image_filename = "data/story/common/locations/layna_forest.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/house_in_a_forest_loop_horrorpen_oga.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

-- the main character handler
local hero = nil

-- the main map loading code
function Load(m)

    Map = m;
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(false);

    Map:SetMinimapImage("data/story/layna_forest/minimaps/layna_forest_north_east_minimap.png");

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

    _HandleTwilight();
end

-- Handle the twilight advancement after the crystal scene
function _HandleTwilight()
    -- If the characters have seen the crystal, then it's time to make the twilight happen
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") < 1) then
        return;
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
    -- Default hero and position (From forest NW)
    hero = CreateSprite(Map, "Bronann", 3, 88, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.EAST);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from forest SE") then
        hero:SetPosition(71, 94);
        hero:SetDirection(vt_map.MapMode.NORTH);
    end
end

-- The boss map sprite
local wolf = nil

function _CreateObjects()
    local object = nil
    local npc = nil
    local event = nil

    -- The boss map sprite
    wolf = CreateSprite(Map, "Fenrir", 104, 3, vt_map.MapMode.GROUND_OBJECT); -- pre place it at the right place.
    wolf:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    wolf:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);
    wolf:SetVisible(false);
    wolf:SetDirection(vt_map.MapMode.SOUTH);

    -- Only add the squirrels and butterflies when the night isn't about to happen
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") < 1) then

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

        npc = CreateSprite(Map, "Squirrel", 40, 14, vt_map.MapMode.GROUND_OBJECT);
        -- Squirrels don't collide with the npcs.
        npc:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        npc:SetSpriteAsScenery(true);
        event = vt_map.RandomMoveSpriteEvent.Create("Squirrel2 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Squirrel2 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Squirrel2 random move", 1800);
    end

    -- Forest entrance treasure chest
    local chest1 = CreateTreasure(Map, "layna_forest_NE_chest1", "Wood_Chest1", 4, 55, vt_map.MapMode.GROUND_OBJECT);
    chest1:AddItem(1, 1); -- small potion

    -- Trees array
    local map_trees = {
        --  left entrance upper side
        { "Tree Small3", 3, 85 },
        { "Tree Small4", 4, 81 },
        { "Tree Small3", 5, 78 },
        { "Tree Small6", 1, 79 },
        { "Tree Small3", 5, 75 },
        { "Tree Small6", 6, 72 },
        { "Tree Small3", 6.5, 69 },
        { "Tree Small5", 5, 66 },
        { "Tree Small3", 6, 63 },
        { "Tree Small4", 7, 61 },
        { "Tree Small5", 2, 62 },
        { "Tree Small6", 8, 58 },
        { "Tree Small3", 8, 55 },
        { "Tree Small3", 7, 52 },
        { "Tree Small4", 9, 49 },
        { "Tree Small3", 9, 46 },
        { "Tree Small4", 10, 44 },
        { "Tree Small3", 11.5, 41 },
        { "Tree Small5", 12, 38 },
        { "Tree Small3", 13, 35 },
        { "Tree Small6", 14, 32 },
        { "Tree Small3", 15.5, 30 },
        { "Tree Small5", 16, 27 },
        { "Tree Small3", 17, 24 },
        { "Tree Small4", 19, 21 },
        { "Tree Small3", 20, 18.5 },
        { "Tree Small3", 20.5, 15 },

        -- Left entrance - bottom side
        { "Tree Small3", 3, 95 },
        { "Tree Small4", 7, 96 },
        { "Tree Small3", 10, 93 },
        { "Tree Small3", 12, 90 },
        { "Tree Little3", 10, 64.5 },
        { "Tree Small6", 14, 87 },
        { "Tree Small3", 15, 84 },
        { "Tree Small5", 16, 82 },
        { "Tree Small3", 16, 79 },
        { "Tree Small3", 15.5, 76 },
        { "Tree Small4", 17, 74 },
        { "Tree Small3", 18, 73 },
        { "Tree Small5", 18, 70 },
        { "Tree Small3", 18.2, 68 },
        { "Tree Small3", 18.5, 65 },
        { "Tree Small3", 19, 63 },
        { "Tree Small6", 19, 60 },
        { "Tree Small3", 19.2, 57 },
        { "Tree Small6", 19.5, 54 },
        { "Tree Small3", 20, 52 },
        { "Tree Small5", 20, 50 },
        { "Tree Small3", 20.2, 47 },
        { "Tree Small4", 21, 44 },
        { "Tree Small3", 21.5, 41 },
        { "Tree Small4", 21.7, 38 },
        { "Tree Small3", 22, 35 },
        { "Tree Small5", 22.5, 33 },
        { "Tree Small3", 23, 30 },
        { "Tree Small5", 25, 27 },
        { "Tree Small3", 26, 24 },
        { "Tree Small5", 29, 22 },
        { "Tree Small3", 31, 19 },
        { "Tree Small4", 34, 18 },

        -- Top-left map tree wall
        { "Tree Small3", -1, 54 },
        { "Tree Small6", -2, 50 },
        { "Tree Small3", -3, 45 },
        { "Tree Small6", 0, 40 },
        { "Tree Small3", 0, 37 },
        { "Tree Small4", 0, 34 },
        { "Tree Small3", 0, 32 },
        { "Tree Small5", 1, 28 },
        { "Tree Small3", -1, 24 },
        { "Tree Small4", 0, 20 },
        { "Tree Small3", 1, 17 },
        { "Tree Small5", 2, 14 },
        { "Tree Small3", 1, 10 },
        { "Tree Small4", 0, 6 },
        { "Tree Small3", 2, 2 },
        { "Tree Small5", 5, 1 },
        { "Tree Small3", 7, 3 },
        { "Tree Small4", 10, 2 },
        { "Tree Small3", 14, 1 },
        { "Tree Small4", 16, 2 },
        { "Tree Small3", 19, 3 },
        { "Tree Small3", 23, 4 },
        { "Tree Small5", 26, 2 },
        { "Tree Small3", 8, 7 },

        -- Trees in the way
        { "Tree Small3", 12, 15 },
        { "Tree Small3", 15, 17 },
        { "Tree Small3", 7, 24 },
        { "Tree Small5", 3, 30 },
        { "Tree Small3", 12, 27.2 },
        { "Tree Small3", 8, 40.1 },
        { "Tree Small4", 10, 51 },

        -- to the right - bottom side
        { "Tree Small3", 37, 20 },
        { "Tree Small3", 40, 18 },
        { "Tree Small4", 42, 19 },
        { "Tree Small3", 45, 20.1 },
        { "Tree Small5", 48, 21 },
        { "Tree Small3", 51, 19 },
        { "Tree Small6", 54, 20 },
        { "Tree Small3", 57, 21 },
        { "Tree Small6", 60, 23 },
        { "Tree Small3", 62, 24 },
        { "Tree Small5", 65, 22 },
        { "Tree Small4", 68, 24.1 },
        { "Tree Small5", 71, 23 },
        { "Tree Small6", 74, 24 },
        { "Tree Small3", 77, 22 },
        { "Tree Small3", 80, 21 },
        { "Tree Small4", 83, 23 },
        { "Tree Small3", 86, 24 },
        { "Tree Small5", 89, 22 },
        { "Tree Small3", 92, 21 },
        { "Tree Small6", 95, 23 },
        { "Tree Small3", 98, 24 },
        { "Tree Small4", 101, 22 },
        { "Tree Small3", 104, 19 },

        -- to the right - top side
        { "Tree Small3", 30, 1 },
        { "Tree Small3", 32, 4 },
        { "Tree Small4", 35, 7 },
        { "Tree Small3", 36, 10 },
        { "Tree Small5", 40, 8 },
        { "Tree Small3", 43, 7 },
        { "Tree Small6", 45, 6.5 },
        { "Tree Small3", 48, 8 },
        { "Tree Small3", 51, 6 },
        { "Tree Small5", 53, 4 },
        { "Tree Small3", 55, 3 },
        { "Tree Small3", 58, 1 },
        { "Tree Small6", 61, 2 },
        { "Tree Small3", 64, 4 },
        { "Tree Small5", 67, 5 },
        { "Tree Small3", 70, 1 },
        { "Tree Small4", 73, 2 },
        { "Tree Small3", 76, 4 },
        { "Tree Small5", 79, 5 },
        { "Tree Small3", 82, 1 },
        { "Tree Small4", 85, 2 },
        { "Tree Small3", 88, 4 },
        { "Tree Small6", 91, 5 },
        { "Tree Small3", 94, 1 },
        { "Tree Small4", 97, 6 },
        { "Tree Small3", 100, 2 },
        { "Tree Small5", 103, 4 },
        { "Tree Small3", 106, 5 },
        { "Tree Small6", 108, 6 },
        { "Tree Small3", 111, 1 },
        { "Tree Small4", 114, 2 },
        { "Tree Small5", 117, 5.2 },
        { "Tree Small3", 120, 8 },
        { "Tree Small6", 123, 10 },
        { "Tree Small3", 126, 11 },
        { "Tree Small5", 127, 14 },
        { "Tree Small3", 126, 5 },

        -- Going down - right side
        { "Tree Small3", 125, 17 },
        { "Tree Small4", 127, 18 },
        { "Tree Small3", 123, 20 },
        { "Tree Small5", 122, 23 },
        { "Tree Small3", 125, 24 },
        { "Tree Small6", 126, 27 },
        { "Tree Small6", 125, 30 },
        { "Tree Small3", 127, 33 },
        { "Tree Small5", 123, 36 },
        { "Tree Small3", 122, 39 },
        { "Tree Small4", 125, 42 },
        { "Tree Small3", 126, 45 },
        { "Tree Small5", 125, 48 },
        { "Tree Small3", 127, 51 },
        { "Tree Small4", 123, 54 },
        { "Tree Small3", 122, 57 },
        { "Tree Small5", 125, 60 },
        { "Tree Small3", 126, 63 },
        { "Tree Small5", 125, 66 },
        { "Tree Small3", 127, 69 },
        { "Tree Small4", 123, 72 },
        { "Tree Small3", 122, 75 },
        { "Tree Small6", 125, 78 },
        { "Tree Small3", 126, 81 },
        { "Tree Small6", 125, 84 },
        { "Tree Small3", 127, 87 },
        { "Tree Small5", 123, 90 },
        { "Tree Small4", 122, 93 },
        { "Tree Small3", 125, 96 },

        -- going down - left side
        { "Tree Small3", 107, 20 },
        { "Tree Small4", 110, 23 },
        { "Tree Small3", 109, 25 },
        { "Tree Small5", 111, 28 },
        { "Tree Small3", 110, 31 },
        { "Tree Small6", 109, 34 },
        { "Tree Small3", 108, 36 },
        { "Tree Small4", 110, 39 },
        { "Tree Small3", 109, 42 },
        { "Tree Small4", 111, 45 },
        { "Tree Small3", 110, 47 },
        { "Tree Small5", 109, 50 },
        { "Tree Small3", 110, 53 },
        { "Tree Small4", 111, 56 },
        { "Tree Small3", 109, 59 },
        { "Tree Small4", 110, 62 },
        { "Tree Small3", 109, 65 },
        { "Tree Small4", 110, 67 },
        { "Tree Small3", 109, 70 },
        { "Tree Small4", 107, 73 },
        { "Tree Small3", 106, 76 },
        { "Tree Small4", 104, 79 },
        { "Tree Small3", 102, 80 },

        -- going left - bottom side
        { "Tree Small3", 118, 82 },
        { "Tree Small4", 119, 85 },
        { "Tree Small3", 117, 88 },
        { "Tree Small4", 118, 91 },
        { "Tree Small3", 119, 94 },
        { "Tree Small5", 119, 97 },
        { "Tree Small3", 115, 81 },
        { "Tree Small6", 116, 84 },
        { "Tree Small3", 114, 87 },
        { "Tree Small6", 115, 90 },
        { "Tree Small3", 116, 93 },
        { "Tree Small5", 116, 96 },
        { "Tree Small3", 112, 83 },
        { "Tree Small4", 113, 86 },
        { "Tree Small3", 111, 89 },
        { "Tree Small5", 112, 92 },
        { "Tree Small3", 113, 95 },
        { "Tree Small4", 113, 98 },
        { "Tree Small3", 108, 91 },
        { "Tree Small3", 106, 94 },
        { "Tree Small4", 107, 97 },
        { "Tree Small3", 105, 95 },
        { "Tree Small5", 102, 92 },
        { "Tree Small3", 102, 96 },
        { "Tree Small6", 99, 97 },
        { "Tree Small3", 95, 96 },
        { "Tree Small5", 92, 94 },
        { "Tree Small3", 89, 95 },
        { "Tree Small5", 86, 97 },
        { "Tree Small3", 83, 96 },
        { "Tree Small3", 80, 94 },
        { "Tree Small4", 78, 95 },

        -- going left - top side
        { "Tree Small3", 98, 82 },
        { "Tree Small3", 94, 81 },
        { "Tree Small4", 90, 82.5 },
        { "Tree Small3", 86, 83 },
        { "Tree Small5", 82, 82 },
        { "Tree Small3", 78, 84 },
        { "Tree Small6", 74, 85 },

        -- going down - top side
        { "Tree Small4", 70, 87 },
        { "Tree Small3", 68, 90 },
        { "Tree Small3", 67, 93 },
        { "Tree Small5", 66, 96 },

        { "Tree Small5", 12, 95 },
        { "Tree Small6", 16, 92 },
        { "Tree Small4", 20, 88 },
        { "Tree Small3", 21, 93 },
        { "Tree Small4", 20, 80 },
        { "Tree Small3", 25, 83 },
        { "Tree Small5", 19, 96 },
        { "Tree Small6", 26, 90 },
        { "Tree Small5", 27, 93.2 },
        { "Tree Small4", 25, 98 },
        { "Tree Big1", 15, 99 },
        { "Tree Small5", 30, 95 },
        { "Tree Small4", 31, 89 },
        { "Tree Small5", 30, 84 },
        { "Tree Small6", 23, 75 },
        { "Tree Small2", 28, 78 },
        { "Tree Small5", 32, 77 },
        { "Tree Small3", 2, 99 },
        { "Tree Big2", 2, 68 },
        { "Tree Small5", 27, 73 },
        { "Tree Small2", 35, 72 },
        { "Tree Small3", 28, 68 },
        { "Tree Small5", 22, 67 },
        { "Tree Small6", 34, 64 },
        { "Tree Small4", 31, 69 },
        { "Tree Small6", 25, 61 },
        { "Tree Small4", 30, 56 },
        { "Tree Small6", 23, 58 },
        { "Tree Small2", 26, 51 },
        { "Tree Big1", 31, 60 },
        { "Tree Small1", 30, 50 },
        { "Tree Small4", 35, 47 },
        { "Tree Small3", 25, 46 },
        { "Tree Small5", 28, 40 },
        { "Tree Small6", 33, 39 },
        { "Tree Small4", 29, 46.5 },
        { "Tree Small6", 27, 32 },
        { "Tree Small1", 32, 30 },
        { "Tree Small3", 30, 34 },
        { "Tree Small2", 35, 33 },
        { "Tree Small4", 40, 38 },
        { "Tree Little1", 34, 26.2 },
        { "Tree Small5", 30, 26 },
        { "Tree Small6", 35, 23 },
        { "Tree Big2", 37, 42 },
        { "Tree Small6", 38, 29 },
        { "Tree Small5", 39, 26 },
        { "Tree Tiny3", 32, 49 },
        { "Tree Small6", 40, 31 },
        { "Tree Small4", 41, 23.2 },
        { "Tree Small1", 43, 28 },
        { "Tree Small3", 46, 28.2 },
        { "Tree Little1", 50, 27.2 },
        { "Tree Small6", 51, 30 },
        { "Tree Small4", 54, 26 },
        { "Tree Small3", 57, 28 },
        { "Tree Small4", 55, 31 },
        { "Tree Small6", 60, 29 },
        { "Tree Small5", 64, 28.2 },
        { "Tree Small1", 63, 32 },
        { "Tree Small3", 68, 30 },
        { "Tree Small2", 71, 28 },
        { "Tree Small3", 77, 25 },
        { "Tree Small4", 82, 26 },
        { "Tree Small6", 88, 27 },
        { "Tree Small4", 92, 25 },
        { "Tree Small5", 100, 26 },
        { "Tree Small2", 104, 22.2 },
        { "Tree Big1", 103, 28 },
        { "Tree Small3", 101, 31 },
        { "Tree Small3", 102, 35 },
        { "Tree Small4", 105, 40 },
        { "Tree Small6", 96, 27 },
        { "Tree Small4", 90, 28 },
        { "Tree Small3", 83, 30 },
        { "Tree Big2", 80, 28 },
        { "Tree Small1", 77, 31 },
        { "Tree Little1", 75, 29 },
        { "Tree Tiny3", 70, 32 },
        { "Tree Small5", 46.5, 32 },
        { "Tree Small6", 59, 33 },
        { "Tree Small4", 67, 34 },
        { "Tree Small2", 35, 53 },
        { "Tree Small4", 50, 34 },
        { "Tree Small1", 44, 35 },
        { "Tree Small4", 74, 35 },
        { "Tree Big2", 80, 33 },
        { "Tree Little3", 87, 32 },
        { "Tree Small5", 94, 33 },
        { "Tree Small3", 83, 36 },
        { "Tree Small4", 97, 36 },
        { "Tree Small3", 91, 35 },
        { "Tree Small3", 56, 36 },
        { "Tree Small4", 101, 41 },
        { "Tree Small6", 95, 39 },
        { "Tree Small4", 106, 47.2 },
        { "Tree Small5", 104, 52 },
        { "Tree Small2", 105, 60 },
        { "Tree Small3", 96, 44 },
        { "Tree Small1", 100, 54 },
        { "Tree Small5", 99, 65 },
        { "Tree Small5", 102, 71 },
        { "Tree Small5", 97, 76 },
        { "Tree Big1", 90, 78.2 },
        { "Tree Small4", 82, 77 },
        { "Tree Big2", 74, 82 },
        { "Tree Tiny3", 64, 80 },
        { "Tree Small3", 59, 92 },
        { "Tree Small4", 60, 95 },
        { "Tree Small3", 55, 98 },
        { "Tree Small3", 100, 45 },
        { "Tree Small4", 96, 50 },
        { "Tree Small6", 97, 58 },
        { "Tree Small5", 101, 62 },
        { "Tree Little1", 97, 70 },
        { "Tree Big2", 86, 74 },
        { "Tree Small1", 78, 76 },
        { "Tree Tiny1", 69, 83 },
        { "Tree Small3", 60, 87 },
        { "Tree Small4", 53, 91 },
        { "Tree Small5", 70, 74 },
        { "Tree Small1", 55, 80 },
        { "Tree Small2", 92, 72 },
        { "Tree Small5", 70, 81 },
        { "Tree Small2", 65, 84 },
        { "Tree Small4", 63, 94 },
        { "Tree Big1", 76, 78 },
        { "Tree Small4", 66, 77 },
        { "Tree Small6", 59, 82 },
        { "Tree Small4", 94, 68 },
        { "Tree Small4", 104, 67 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_trees) do
        --print(my_array[1], my_array[2], my_array[3]);
        CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
    end

end

function _CreateEnemies()
    local enemy = nil
    local roam_zone = nil

    -- Treasure zone

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(5, 10, 8, 47);

    enemy = CreateEnemySprite(Map, "slime");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(2);
    enemy:AddEnemy(2);
    roam_zone:AddEnemy(enemy, 1);

    -- after fight zone
    roam_zone = vt_map.EnemyZone.Create(112, 120, 34, 80);

    enemy = CreateEnemySprite(Map, "spider");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(2);
    enemy:AddEnemy(1);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(2);
    enemy:AddEnemy(2);
    roam_zone:AddEnemy(enemy, 1);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Map events
    vt_map.MapTransitionEvent.Create("to forest NW", "data/story/layna_forest/layna_forest_north_west_map.lua",
                                     "data/story/layna_forest/layna_forest_north_west_script.lua", "from_layna_forest_NE");

    vt_map.MapTransitionEvent.Create("to forest SE", "data/story/layna_forest/layna_forest_south_east_map.lua",
                                     "data/story/layna_forest/layna_forest_south_east_script.lua", "from_layna_forest_NE");

    -- generic events
    vt_map.ScriptedEvent.Create("Map:PopState()", "Map_PopState", "");

    vt_map.ChangeDirectionSpriteEvent.Create("The Hero looks north", hero, vt_map.MapMode.NORTH);

    -- Warning dialogue
    event = vt_map.SoundEvent.Create("Warning dialogue event", "data/sounds/footstep_grass1.wav")
    event:AddEventLinkAtEnd("Warning dialogue");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("What's that?!");
    dialogue:AddLineEventEmote(text, hero, "The Hero looks north", "", "exclamation");
    event = vt_map.DialogueEvent.Create("Warning dialogue", dialogue);
    event:SetStopCameraMovement(true);

    -- Boss fight scene
    event = vt_map.ScriptedEvent.Create("boss fight scene", "start_boss_fight_scene", "");
    event:AddEventLinkAtEnd("hero looks west");

    event = vt_map.ChangeDirectionSpriteEvent.Create("hero looks west", hero, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("hero looks east", 800);

    event = vt_map.ChangeDirectionSpriteEvent.Create("hero looks east", hero, vt_map.MapMode.EAST);
    event:AddEventLinkAtEnd("The hero looks at wolf", 800);

    event = vt_map.LookAtSpriteEvent.Create("The hero looks at wolf", hero, wolf);
    event:AddEventLinkAtEnd("Wolf runs toward the hero");

    event = vt_map.PathMoveSpriteEvent.Create("Wolf runs toward the hero", wolf, hero, true);
    event:AddEventLinkAtEnd("First Wolf battle");

    event = vt_map.BattleEncounterEvent.Create("First Wolf battle");
    event:SetMusic("data/music/accion-OGA-djsaryon.ogg");
    event:SetBackground("data/battles/battle_scenes/forest_background.png");
    event:AddEnemy(3, 512, 500);
    event:SetBoss(true);
    event:AddEventLinkAtEnd("Make the wolf disappear");

    event = vt_map.ScriptedEvent.Create("Make the wolf disappear", "make_wolf_invisible", "");
    event:AddEventLinkAtEnd("boss fight post-dialogue");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Woah, that was quite a nasty fight. Why on earth was a north arctic fenrir lurking in the forest? I thought it was merely a part of myths.");
    dialogue:AddLineEmote(text, hero, "sweat drop");
    text = vt_system.Translate("It ran away. I'm almost certain that we'll meet it again. We'd better be more prepared next time.");
    dialogue:AddLineEmote(text, hero, "thinking dots");
    text = vt_system.Translate("I'll try not to think about what it could have done to Orlinn. Let's find him... quickly.");
    dialogue:AddLine(text, hero);
    event = vt_map.DialogueEvent.Create("boss fight post-dialogue", dialogue);
    event:AddEventLinkAtEnd("Map:PopState()");
    event:AddEventLinkAtEnd("Restart music");

    vt_map.ScriptedEvent.Create("Restart music", "restart_music", "");
end

-- zones
local to_forest_NW_zone = nil
local to_forest_SE_zone = nil
local music_fade_out_zone = nil
local warning_zone = nil
local boss_fight1_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_forest_NW_zone = vt_map.CameraZone.Create(0, 1, 86, 90);
    to_forest_SE_zone = vt_map.CameraZone.Create(69, 75, 95, 96);
    -- Fade out music zone - used to set a dramatic area
    music_fade_out_zone = vt_map.CameraZone.Create(48, 50, 8, 17);
    warning_zone = vt_map.CameraZone.Create(91, 93, 4, 18);
    boss_fight1_zone = vt_map.CameraZone.Create(103, 105, 4, 18);
end

-- A simple boolean permiting to trigger the dialogue only once...
local warning_dialogue_done = false

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_forest_NW_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest NW");
    elseif (to_forest_SE_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest SE");
    elseif (music_fade_out_zone:IsCameraEntering() == true) then
        -- fade out the music when the first boss fight hasn't been done yet.
        if (GlobalManager:DoesEventExist("story", "layna_forest_boss_fight1") == false) then
            AudioManager:FadeOutActiveMusic(2000);
        end
    elseif (warning_zone:IsCameraEntering() == true) then
        if (warning_dialogue_done == false) then
            warning_dialogue_done = true;
            EventManager:StartEvent("Warning dialogue event");
        end
    elseif (boss_fight1_zone:IsCameraEntering() == true) then
        -- fade out the music when the first boss fight hasn't been done yet.
        if (GlobalManager:DoesEventExist("story", "layna_forest_boss_fight1") == false) then
            GlobalManager:SetEventValue("story", "layna_forest_boss_fight1", 1);
            EventManager:StartEvent("boss fight scene");
        end
    end
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("data/music/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("data/battles/battle_scenes/forest_background.png");

    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") < 1) then
        -- Add tutorial battle dialog with Kalya and Bronann
        enemy:AddBattleScript("data/battles/battle_scenes/tutorial_battle_dialogs.lua");
    else
        -- Setup time of the day lighting on battles
        enemy:AddBattleScript("data/story/layna_forest/after_crystal_twilight_battles.lua");
        if (GlobalManager:GetEventValue("story", "layna_forest_twilight_value") > 2) then
            enemy:SetBattleBackground("data/battles/battle_scenes/forest_background_evening.png");
        end
    end
end

-- Map Custom functions
-- Used through scripted events

map_functions = {

    Map_PopState = function()
        Map:PopState();
    end,

    start_boss_fight_scene = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        -- Play the wolf growling sound
        AudioManager:PlaySound("data/sounds/growl1_IFartInUrGeneralDirection_freesound.wav");
        wolf:SetVisible(true);
    end,

    make_wolf_invisible = function()
        wolf:SetVisible(false);
        wolf:SetPosition(104, 3);
    end,

    restart_music = function()
        AudioManager:FadeInActiveMusic(2000);
    end
}
