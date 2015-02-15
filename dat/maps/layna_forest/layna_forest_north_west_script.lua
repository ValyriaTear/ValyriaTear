-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_north_west_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest"
map_image_filename = "img/menus/locations/layna_forest.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/house_in_a_forest_loop_horrorpen_oga.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

-- the main character handler
local hero = nil

-- Orlinn's sprite
local orlinn = nil

-- Forest dialogue secondary hero
local kalya_sprite = nil

-- Name of the main sprite. Used to reload the good one at the end of the first forest entrance event.
local main_sprite_name = "";

-- the main map loading code
function Load(m)

    Map = m;
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(false);

    Map:SetMinimapImage("dat/maps/layna_forest/minimaps/layna_forest_north_west_minimap.png");

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
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, -5.0, true);

    _HandleTwilight();
end

-- Handle the twilight advancement after the crystal scene
function _HandleTwilight()

    -- If the characters have seen the crystal, then it's time to make the twilight happen
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") < 1) then
        return;
    end

    -- test if the day time is sufficiently advanced
    if (GlobalManager:GetEventValue("story", "layna_forest_twilight_value") < 2) then
        GlobalManager:SetEventValue("story", "layna_forest_twilight_value", 2);
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
    hero = CreateSprite(Map, "Bronann", 3, 86, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.EAST);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_layna_forest_NE") then
        hero:SetPosition(124, 42);
        hero:SetDirection(vt_map.MapMode.WEST);
    elseif (GlobalManager:GetPreviousLocation() == "from forest SW") then
        hero:SetPosition(116, 94);
        hero:SetDirection(vt_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_layna_cave_entrance") then
        hero:SetPosition(76, 40);
        hero:SetDirection(vt_map.MapMode.SOUTH);
    end

    orlinn = CreateSprite(Map, "Orlinn", 76, 40, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.NORTH);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    if (GlobalManager:DoesEventExist("story", "layna_forest_kalya sees_orlinn") == true) then
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetVisible(false);
    end

    -- Create secondary character for the scene with Orlinn
    kalya_sprite = CreateSprite(Map, "Kalya",
                                hero:GetXPosition(), hero:GetYPosition(), vt_map.MapMode.GROUND_OBJECT);

    kalya_sprite:SetDirection(vt_map.MapMode.WEST);
    kalya_sprite:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya_sprite:SetVisible(false);
end

function _CreateObjects()
    local object = nil
    local npc = nil
    local event = nil

    -- Only add the squirrels and butterflies when the night isn't about to happen
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") < 1) then

        npc = CreateSprite(Map, "Butterfly", 42, 38, vt_map.MapMode.GROUND_OBJECT);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        event = vt_map.RandomMoveSpriteEvent.Create("Butterfly1 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Butterfly1 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Butterfly1 random move");

        npc = CreateSprite(Map, "Butterfly", 12, 30, vt_map.MapMode.GROUND_OBJECT);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        event = vt_map.RandomMoveSpriteEvent.Create("Butterfly2 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Butterfly2 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Butterfly2 random move", 2400);

        npc = CreateSprite(Map, "Butterfly", 50, 65, vt_map.MapMode.GROUND_OBJECT);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        event = vt_map.RandomMoveSpriteEvent.Create("Butterfly3 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Butterfly3 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Butterfly3 random move", 1050);

        npc = CreateSprite(Map, "Butterfly", 40, 10, vt_map.MapMode.GROUND_OBJECT);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        event = vt_map.RandomMoveSpriteEvent.Create("Butterfly4 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Butterfly4 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Butterfly4 random move", 3050);

        npc = CreateSprite(Map, "Squirrel", 58, 24, vt_map.MapMode.GROUND_OBJECT);
        -- Squirrels don't collide with the npcs.
        npc:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        npc:SetSpriteAsScenery(true);
        event = vt_map.RandomMoveSpriteEvent.Create("Squirrel1 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Squirrel1 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Squirrel1 random move");

        npc = CreateSprite(Map, "Squirrel", 40, 34, vt_map.MapMode.GROUND_OBJECT);
        -- Squirrels don't collide with the npcs.
        npc:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        npc:SetSpriteAsScenery(true);
        event = vt_map.RandomMoveSpriteEvent.Create("Squirrel2 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Squirrel2 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Squirrel2 random move", 1800);
    end

    -- Treasure chests
    local chest1 = CreateTreasure(Map, "layna_forest_nw_chest1", "Wood_Chest1", 6, 3, vt_map.MapMode.GROUND_OBJECT);
    chest1:AddItem(1, 1); -- small potion

    local chest2 = CreateTreasure(Map, "layna_forest_nw_chest2", "Wood_Chest1", 3, 58, vt_map.MapMode.GROUND_OBJECT);
    chest2:AddItem(1, 1); -- small potion

    event = vt_map.BattleEncounterEvent.Create("Monster trap in chest");
    event:SetMusic("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    event:SetBackground("img/backdrops/battle/forest_background.png");

    -- default values
    event:SetMusic("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    event:SetBackground("img/backdrops/battle/forest_background.png");

    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") < 1) then
        -- Add tutorial battle dialog with Kalya and Bronann
        event:AddScript("dat/battles/tutorial_battle_dialogs.lua");
    else
        -- Setup time of the day lighting on battles
        event:AddScript("dat/maps/layna_forest/after_crystal_twilight_battles.lua");
        if (GlobalManager:GetEventValue("story", "layna_forest_twilight_value") > 2) then
            event:SetBackground("img/backdrops/battle/forest_background_evening.png");
        end
    end

    event:AddEnemy(2, 0, 0);
    event:AddEnemy(2, 0, 0);
    event:AddEnemy(2, 0, 0);
    event:AddEnemy(2, 0, 0);

    local chest3 = CreateTreasure(Map, "layna_forest_nw_chest3", "Wood_Chest1", 26, 42, vt_map.MapMode.GROUND_OBJECT);
    chest3:AddItem(40001, 1); -- prismatic ring
    chest3:AddEvent("Monster trap in chest");

    local chest4 = CreateTreasure(Map, "layna_forest_nw_chest4", "Wood_Chest1", 122, 5, vt_map.MapMode.GROUND_OBJECT);
    -- Empty chest, just to bother the player

    -- Trees of first path (the one next to the forest entrance).
    CreateObject(Map, "Tree Small3", 1, 80, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 6, 78, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 11, 80, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 16, 81, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 21, 82, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 26, 80, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 31, 82, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 36, 80, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 41, 78, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 46, 80, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 51, 82, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 56, 80, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 61, 82, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 66, 80, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 71, 82, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 76, 81, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 2, 96, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 5, 93, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 5.5, 100, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 9, 95, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 11, 92, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 13, 99, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 13.5, 93.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 16, 96, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 19, 93, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 21, 93.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 23, 98, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 25, 94, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 30, 96, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 33, 93, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 37, 94, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 39, 92, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 41, 95, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 44, 99, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 49, 98, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 54, 100, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 59, 98, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 64, 97, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 66, 83, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 69, 98, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 73, 99, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 79, 100, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 85, 98, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 90, 97, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 95, 95, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 98, 93, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 99, 96, vt_map.MapMode.GROUND_OBJECT);

    -- going up
    CreateObject(Map, "Tree Small3", 104, 99, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 109, 100, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 103, 94, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 108, 95, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 102, 88, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 107, 89, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 102, 91, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 103, 81, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 104, 90, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 100, 84, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 105, 85, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 100, 79, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 104, 80, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 98, 75, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 103, 76, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 95, 72, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 100, 71, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 91, 69, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 96, 68, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 86, 66, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 90, 65, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 82, 63, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 87, 61, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 88, 83, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 92, 78, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 93, 85, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 80, 70, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 78, 91, vt_map.MapMode.GROUND_OBJECT);

    -- Going left
    CreateObject(Map, "Tree Small4", 77, 63.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 74, 60, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 70, 60.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 66, 58, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 62, 56, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 62, 61, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 58, 54.8, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 56, 71, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 55, 52.1, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 50, 54.8, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 52, 52, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 47, 56.8, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 49, 53, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 41, 62, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 50, 69, vt_map.MapMode.GROUND_OBJECT);

    -- Cutting the road.
    CreateObject(Map, "Tree Small5", 43, 59, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 39, 61, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 37, 63.5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 33, 65.4, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 31, 70, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 28, 67, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 41, 70, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 32, 61, vt_map.MapMode.GROUND_OBJECT);

    -- gap to get back the road.
    CreateObject(Map, "Tree Small5", 20, 66, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 16, 67, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 12, 63, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 8, 60, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 5, 57, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 2, 55, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 0.1, 58, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 0.5, 65, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 0.2, 70, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 3, 71, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 14, 70, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 20, 75, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 32, 76, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 22, 74, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 1, 35, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 11, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 14, 35, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 15, 31, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 12, 40, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 3, 27, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 6, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 1, 23, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 17, 28, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 18, 25, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 16, 22, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 19, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 17, 33, vt_map.MapMode.GROUND_OBJECT);

    -- north-west gap to second item
    CreateObject(Map, "Tree Small3", 11, 14, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 10, 11, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 13, 10, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 13.5, 7, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 18, 8, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 21, 5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 24, 2, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 10, 1, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 3, 2, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 0.4, 7, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 2, 13, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 6, 2, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 1, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 1, 74, vt_map.MapMode.GROUND_OBJECT);

    -- Going up again
    CreateObject(Map, "Tree Small3", 32, 59, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 29, 56, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 26, 54, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 23, 52, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 19, 51, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 17, 48, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 14, 46, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 11, 44, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 8, 42, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 0.1, 44, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 0.2, 47.3, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 14, 52, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 29, 60, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 22, 44, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 31, 47, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 28, 40, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 24, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 22, 31, vt_map.MapMode.GROUND_OBJECT);

    -- Going right
    CreateObject(Map, "Tree Small4", 23, 17, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 26, 14, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 28, 4, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 33, 5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 38, 3, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 43, 6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 47, 2, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 52, 4, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 56, 5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 61, 6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 64, 3, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 66, 1, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 70, 3, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 73, 2, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 77, 5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 81, 3, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 82, 1, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 86, 3, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 91, 2, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 96, 5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 101, 8, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 102, 11, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 31, 12, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 35, 11, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 38, 14, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 105, 13, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 108, 15, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 110, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 110, 21, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 113, 23, vt_map.MapMode.GROUND_OBJECT);

    -- after the last entrance gap
    CreateObject(Map, "Tree Small3", 116, 29, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 118, 31, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 119, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 123, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 126, 40, vt_map.MapMode.GROUND_OBJECT);

    -- east wall of gap
    CreateObject(Map, "Tree Small3", 126, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 127, 31, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 125, 25, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 126, 22, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 127, 17, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 124, 14, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 127, 8, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 125, 4, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 125, 1, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 121, 2, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 116, 1, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 112, 3, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 108, 4, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 103, 5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 112, 7, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 119, 5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 117, 15, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 33, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 30, 23, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 94, 24.1, vt_map.MapMode.GROUND_OBJECT);

    -- tree wall below end line
    CreateObject(Map, "Tree Small3", 42, 15, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 47, 14, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 52, 13, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 56, 15, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 61, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 80, 8, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 75, 19, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 80, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 84, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 89, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 93, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 97, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 99, 21, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 100, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 102, 27, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 104, 30, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small6", 105, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 107, 35, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 105, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small5", 106, 41, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 108, 44, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 111, 46, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 115, 48, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 120, 50, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 125, 51, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 100, 49, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small4", 107, 47, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small3", 116, 50.1, vt_map.MapMode.GROUND_OBJECT);

    -- Trees array
    local map_trees = {
        --  bottom-right entrance right side
        { "Tree Small3", 121, 96 },
        { "Tree Small4", 125, 95 },
        { "Tree Small3", 127, 91 },
        { "Tree Small6", 124, 88 },
        { "Tree Small3", 121, 84 },
        { "Tree Small6", 120, 80 },
        { "Tree Small3", 126, 85 },
        { "Tree Small3", 125, 81 },
        { "Tree Small4", 118, 77 },
        { "Tree Small3", 123, 74 },
        { "Tree Small5", 126, 72 },
        { "Tree Small3", 118, 71 },
        { "Tree Small4", 120, 69 },
        { "Tree Small3", 114, 63 },
        { "Tree Small6", 117, 61 },
        { "Tree Small5", 121, 59 },
        { "Tree Small3", 125, 57 },
        { "Tree Small4", 114, 56 },
        { "Tree Small3", 126, 65 },
        { "Tree Small5", 124, 60 },

        { "Tree Small3", 83, 51 },
        { "Tree Small4", 88, 52 },
        { "Tree Small5", 90, 53 },
        { "Tree Small6", 94, 52 },
        { "Tree Small3", 99, 51 },
        { "Tree Small6", 105, 50 },
        { "Tree Small6", 78, 50 },
        { "Tree Small3", 74, 47 },
        { "Tree Small5", 69, 45 },
        { "Tree Small5", 65, 44 },
        { "Tree Small3", 64, 41 },
        { "Tree Small4", 68, 39 },
        { "Tree Small5", 59, 40 },

        { "Tree Small3", 56, 35 },
        { "Tree Small4", 49, 37 },
        { "Tree Small5", 44, 38 },
        { "Tree Small3", 52, 32 },
        { "Tree Small6", 47, 30 },
        { "Tree Small5", 42, 28 },
        { "Tree Small3", 99, 54 },
        { "Tree Small4", 107, 56 },
        { "Tree Small3", 61, 36 },
        { "Tree Small3", 40, 31 },

        { "Tree Small1", 86, 35 },
        { "Tree Big1", 77, 31 },

        { "Tree Little2", 7, 81 },
        { "Tree Tiny1", 17, 82 },
        { "Tree Little1", 27, 81 },
        { "Tree Little2", 54, 93 },
        { "Tree Tiny2", 41, 69.5 },
        { "Tree Tiny1", 40, 80 },
        { "Tree Little3", 58, 84 },
        { "Tree Small2", 87, 92 },
        { "Tree Tiny1", 74, 83 },
        { "Tree Tiny3", 35, 97 },
        { "Tree Big1", 18, 53 },
        { "Tree Little1", 11, 46.2 },
        { "Tree Tiny3", 26, 17.2 },
        { "Tree Little1", 48, 75 },
        { "Tree Little2", 50, 16 },
        { "Tree Tiny3", 62, 17 },
        { "Tree Little1", 70, 18 },
        { "Tree Little1", 36, 18 },
        { "Tree Big2", 90, 4 },
        { "Tree Tiny2", 111, 33 },
        { "Tree Little2", 100, 29 },
        { "Tree Small1", 112, 49 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_trees) do
        --print(my_array[1], my_array[2], my_array[3]);
        CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
    end

    -- Shortcut trees
    if (GlobalManager:DoesEventExist("story", "layna_forest_trees_shorcut_open") == false) then
        CreateObject(Map, "Tree Small5", 66, 15, vt_map.MapMode.GROUND_OBJECT);
        CreateObject(Map, "Tree Small3", 71, 17, vt_map.MapMode.GROUND_OBJECT);
    else -- If the event happened
        CreateObject(Map, "Tree Small3 Tilting", 73, 17, vt_map.MapMode.GROUND_OBJECT);
        CreateObject(Map, "Tree Small5 Fallen", 70, 16.5, vt_map.MapMode.GROUND_OBJECT);
    end

    -- grass array
    local map_grass = {
    { "Grass Clump1", 76, 38 },
    { "Grass Clump1", 81, 35 },
    { "Grass Clump1", 85, 36 },
    { "Grass Clump1", 90, 38 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_grass) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
end

function _CreateEnemies()
    local enemy = nil
    local roam_zone = nil

    -- Enemy near chest 1
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(2, 10, 58, 75);

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

    -- Enemy near chest 2
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(2, 20, 5, 23);

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

    -- Enemy near chest 3 - Trap chest and zone
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(103, 126, 2, 40);

    enemy = CreateEnemySprite(Map, "slime");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(2);
    roam_zone:AddEnemy(enemy, 2);

    -- Wandering enemy
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(41, 97, 81, 95);

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

    -- Wandering enemy
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(23, 88, 58, 76);

    enemy = CreateEnemySprite(Map, "slime");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:AddEnemy(1);
    enemy:NewEnemyParty();
    enemy:AddEnemy(1);
    enemy:AddEnemy(2);
    roam_zone:AddEnemy(enemy, 2);

    -- Wandering enemy
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(62, 100, 4, 13);

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

-- Special event references which destinations must be updated just before being called.
local move_next_to_hero_event = nil
local move_back_to_hero_event = nil

-- used in the scene when returning to the village
local move_next_to_hero_event2 = nil
local move_next_to_hero_event3 = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    vt_map.MapTransitionEvent.Create("to forest entrance", "dat/maps/layna_forest/layna_forest_entrance_map.lua",
                                     "dat/maps/layna_forest/layna_forest_entrance_script.lua", "from_layna_forest_NW");

    vt_map.MapTransitionEvent.Create("to forest NE", "dat/maps/layna_forest/layna_forest_north_east_map.lua",
                                     "dat/maps/layna_forest/layna_forest_north_east_script.lua", "from_layna_forest_NW");

    vt_map.MapTransitionEvent.Create("to forest SW", "dat/maps/layna_forest/layna_forest_south_west_map.lua",
                                     "dat/maps/layna_forest/layna_forest_south_west_script.lua", "from_layna_forest_NW");

    vt_map.MapTransitionEvent.Create("to cave entrance", "dat/maps/layna_forest/layna_forest_cave1_1_map.lua",
                                     "dat/maps/layna_forest/layna_forest_cave1_1_script.lua", "from_layna_forest_NW");

    -- dialogue events
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya_sprite, hero);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Kalya", hero, kalya_sprite);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Orlinn", kalya_sprite, orlinn);
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Kalya", orlinn, kalya_sprite);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Orlinn", hero, orlinn);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks west", kalya_sprite, vt_map.MapMode.WEST);

    vt_map.ScriptedSpriteEvent.Create("kalya_sprite:SetCollision(NONE)", kalya_sprite, "Sprite_Collision_off", "");
    vt_map.ScriptedSpriteEvent.Create("kalya_sprite:SetCollision(ALL)", kalya_sprite, "Sprite_Collision_on", "");

    vt_map.ScriptedSpriteEvent.Create("orlinn:SetCollision(NONE)", orlinn, "Sprite_Collision_off", "");
    vt_map.ScriptedSpriteEvent.Create("orlinn:SetCollision(ALL)", orlinn, "Sprite_Collision_on", "");

    -- First time forest entrance dialogue about save points and the heal spring.
    event = vt_map.ScriptedEvent.Create("Start - Kalya sees Orlinn", "layna_forest_kalya_sees_orlinn_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 50);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann", kalya_sprite, 0, 0, false);
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya calls Orlinn");
    move_next_to_hero_event:AddEventLinkAtEnd("kalya_sprite:SetCollision(ALL)");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Orlinn!");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Kalya looks at Orlinn", "", "exclamation");
    event = vt_map.DialogueEvent.Create("Kalya calls Orlinn", dialogue);
    event:AddEventLinkAtEnd("Kalya runs above cave entrance");
    event:AddEventLinkAtEnd("Bronann runs above cave entrance next to Kalya");

    vt_map.PathMoveSpriteEvent.Create("Kalya runs above cave entrance", kalya_sprite, 80, 33, true);

    event = vt_map.PathMoveSpriteEvent.Create("Bronann runs above cave entrance next to Kalya", hero, 82, 33, true);
    event:AddEventLinkAtEnd("Kalya tries to discuss with Orlinn");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Orlinn, stop it RIGHT NOW!!");
    dialogue:AddLineEmote(text, kalya_sprite, "exclamation");
    text = vt_system.Translate("I can't sis. I can feel it... calling me.");
    dialogue:AddLine(text, orlinn);
    event = vt_map.DialogueEvent.Create("Kalya tries to discuss with Orlinn", dialogue);
    event:AddEventLinkAtEnd("Orlinn enters the cave");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn enters the cave", orlinn, 76, 38, true);
    event:AddEventLinkAtEnd("Orlinn disappears");

    event = vt_map.ScriptedSpriteEvent.Create("Orlinn disappears", orlinn, "SetVisible_false", "");
    event:AddEventLinkAtEnd("orlinn:SetCollision(NONE)");
    event:AddEventLinkAtEnd("Kalya tells Bronann to follow him");
    event:AddEventLinkAtEnd("Bronann looks at Kalya");
    event:AddEventLinkAtEnd("Kalya looks at Bronann");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Quick! Let's follow him.");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("Right!");
    dialogue:AddLine(text, hero);
    event = vt_map.DialogueEvent.Create("Kalya tells Bronann to follow him", dialogue);
    event:AddEventLinkAtEnd("kalya_sprite:SetCollision(NONE)");
    event:AddEventLinkAtEnd("Set Camera back to Bronann");

    event = vt_map.ScriptedSpriteEvent.Create("Set Camera back to Bronann", hero, "SetCamera", "");
    event:AddEventLinkAtEnd("kalya goes back to party");

    move_back_to_hero_event = vt_map.PathMoveSpriteEvent.Create("kalya goes back to party", kalya_sprite, hero, false);
    move_back_to_hero_event:AddEventLinkAtEnd("end of dialogue with Orlinn");

    vt_map.ScriptedEvent.Create("end of dialogue with Orlinn", "end_of_dialogue_with_orlinn", "");

    -- Dialogue: the hero sees the tree shortcut
    event = vt_map.ScriptedEvent.Create("Show tree shortcut", "show_trees_shortcut", "is_camera_moving_finished");
    event:AddEventLinkAtEnd("The Hero sees the created shortcut");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Great! The tremor has opened a path back to the village!");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("Now we can get back to the village more quickly.");
    dialogue:AddLine(text, hero);
    event = vt_map.DialogueEvent.Create("The Hero sees the created shortcut", dialogue);
    event:AddEventLinkAtEnd("Set Camera back to Hero")

    vt_map.ScriptedSpriteEvent.Create("Set Camera back to Hero", hero, "SetCamera2", "is_camera2_moving_finished");

    -- scene when returning to the village

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event2 = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Orlinn", kalya_sprite, 0, 0, false);
    move_next_to_hero_event2:AddEventLinkAtEnd("Kalya looks at Orlinn");

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event3 = vt_map.PathMoveSpriteEvent.Create("Bronann moves next to Orlinn", hero, 0, 0, false);
    move_next_to_hero_event3:AddEventLinkAtEnd("Bronann looks at Orlinn");

    event = vt_map.ScriptedEvent.Create("Start of dialogue when returning to the village", "start_of_dialogue_return_to_village", "");
    event:AddEventLinkAtEnd("Kalya moves next to Orlinn");
    event:AddEventLinkAtEnd("Bronann moves next to Orlinn");
    event:AddEventLinkAtEnd("Dialogue when returning to the village", 800);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("We're almost there!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Say sis, do you think Herth will be angry at me for what happened?");
    dialogue:AddLineEventEmote(text, orlinn, "Orlinn looks at Kalya", "", "sweat drop");
    text = vt_system.Translate("I don't think so, Orlinn. We both know you didn't do it on purpose, right?");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("Ok.");
    dialogue:AddLine(text, orlinn);
    event = vt_map.DialogueEvent.Create("Dialogue when returning to the village", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes to the village", 800);

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn goes to the village", orlinn, 1, 84, true);
    event:AddEventLinkAtEnd("Make Orlinn invisible");

    event = vt_map.ScriptedEvent.Create("Make Orlinn invisible", "make_orlinn_invisible", "");
    event:AddEventLinkAtEnd("Dialogue when returning to the village - part 2", 800);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Kalya, err...");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("What is it, Bronann?");
    dialogue:AddLineEvent(text, kalya_sprite, "Kalya looks at Bronann", "");
    text = vt_system.Translate("Hmm. Nevermind.");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("Don't worry too much, Bronann. Herth will be able to help you.");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("Me? But...");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("Thanks...");
    dialogue:AddLineEvent(text, kalya_sprite, "Kalya looks west", "");
    text = vt_system.Translate("Thanks for your help.");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("You're welcome.");
    dialogue:AddLine(text, hero);
    event = vt_map.DialogueEvent.Create("Dialogue when returning to the village - part 2", dialogue);
    event:AddEventLinkAtEnd("Bronann goes to the village");
    event:AddEventLinkAtEnd("Kalya goes to the village");
    event:AddEventLinkAtEnd("to forest entrance");

    vt_map.PathMoveSpriteEvent.Create("Bronann goes to the village", hero, 1, 83, false);

    vt_map.PathMoveSpriteEvent.Create("Kalya goes to the village", kalya_sprite, 1, 85, false);
end

-- zones
local to_forest_entrance_zone = nil
local to_forest_NE_zone = nil
local to_forest_SW_zone = nil
local to_cave_entrance_zone = nil
local orlinn_scene_zone = nil
local dialogue_near_forest_entrance_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_forest_entrance_zone = vt_map.CameraZone.Create(0, 1, 80, 90);
    dialogue_near_forest_entrance_zone = vt_map.CameraZone.Create(5, 7, 80, 90);
    to_forest_NE_zone = vt_map.CameraZone.Create(126, 128, 40, 45);
    to_forest_SW_zone = vt_map.CameraZone.Create(111, 119, 95, 97);
    to_cave_entrance_zone = vt_map.CameraZone.Create(74, 78, 36, 38);
    orlinn_scene_zone = vt_map.CameraZone.Create(81, 83, 18, 28);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_forest_entrance_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest entrance");
    elseif (dialogue_near_forest_entrance_zone:IsCameraEntering() == true
            and GlobalManager:GetEventValue("story", "layna_forest_return_to_village_dialogue_done") == 0) then
        if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
            -- Start the return to village dialogue
            hero:SetMoving(false);
            EventManager:StartEvent("Start of dialogue when returning to the village");
        end
    elseif (to_forest_NE_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest NE");
    elseif (to_forest_SW_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to forest SW");
    elseif (to_cave_entrance_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to cave entrance");
    elseif (orlinn_scene_zone:IsCameraEntering() == true) then
        if (Map:CurrentState() ~= vt_map.MapMode.STATE_SCENE) then
            if (GlobalManager:DoesEventExist("story", "layna_forest_kalya sees_orlinn") == false) then
                hero:SetMoving(false);
                EventManager:StartEvent("Start - Kalya sees Orlinn");
            elseif (GlobalManager:DoesEventExist("story", "layna_forest_trees_shorcut_open") == true
                    and GlobalManager:DoesEventExist("story", "layna_forest_trees_shortcut_seen") == false) then
                hero:SetMoving(false);
                EventManager:StartEvent("Show tree shortcut");
                GlobalManager:SetEventValue("story", "layna_forest_trees_shortcut_seen", 1);
            end
        end
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
map_functions = {

    layna_forest_kalya_sees_orlinn_start = function()
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

        move_next_to_hero_event:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition(), false);

        AudioManager:FadeOutActiveMusic(1000);
    end,

    end_of_dialogue_with_orlinn = function()
        Map:PopState();
        kalya_sprite:SetPosition(0, 0);
        kalya_sprite:SetVisible(false);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);

        AudioManager:FadeInActiveMusic(1000);

        -- Set event as done
        GlobalManager:SetEventValue("story", "layna_forest_kalya sees_orlinn", 1);
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

    SetVisible_false = function(sprite)
        if (sprite ~= nil) then
            sprite:SetVisible(false);
        end
    end,

    show_trees_shortcut = function()
        -- Focus the camera on the shortcut
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        Map:MoveVirtualFocus(64, 16);
        Map:SetCamera(Map:GetVirtualFocus(), 2500);
    end,

    SetCamera2 = function(sprite)
        Map:SetCamera(sprite, 2500);
    end,

    is_camera_moving_finished = function()
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        return true;
    end,

    is_camera2_moving_finished = function(sprite)
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        Map:PopState();
        return true;
    end,

    start_of_dialogue_return_to_village = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya_sprite:SetVisible(true);
        kalya_sprite:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        hero:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        orlinn:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        orlinn:SetVisible(true);
        orlinn:SetDirection(vt_map.MapMode.WEST);

        move_next_to_hero_event2:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition(), false);
        move_next_to_hero_event3:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition() - 2.0, false);

        -- Set the event as done now to avoid retriggering it
        GlobalManager:SetEventValue("story", "layna_forest_return_to_village_dialogue_done", 1);
    end,

    make_orlinn_invisible = function()
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
    end
}
