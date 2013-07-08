-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_cave1_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "img/menus/locations/mt_elrus.png"
map_subname = "Elbrus Grotto"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/awareness_el_corleo.ogg"

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

    _CreateCharacters();
    _CreateObjects();
    _CreateEnemies();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 88, 77); -- exit/entrance 1
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_entrance2") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(9, 77);
    end

    Map:AddGroundObject(hero);
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    enemy:AddBattleScript("dat/battles/desert_cave_battle_anim.lua");
end
function _SetEventBattleEnvironment(event)
    event:SetMusic("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    event:SetBackground("img/backdrops/battle/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    event:AddScript("dat/battles/desert_cave_battle_anim.lua");
end

local shroom1 = {};
local shroom2 = {};

function _CreateObjects()
    local object = {}
    local npc = {}
    local event = {}
    local dialogue = {}
    local text = {}

    shroom1 = CreateSprite(Map, "Shroom", 45, 42);
    shroom1:SetDirection(vt_map.MapMode.SOUTH);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("...");
    dialogue:AddLineEvent(text, shroom1, "", "Fight with Shroom 1");
    DialogueManager:AddDialogue(dialogue);
    shroom1:AddDialogueReference(dialogue);
    Map:AddGroundObject(shroom1);

    event = vt_map.BattleEncounterEvent("Fight with Shroom 1");
    event:AddEnemy(11, 512, 384); -- one shroom
    _SetEventBattleEnvironment(event);
    event:AddEventLinkAtEnd("Place Shroom 1 after fight", 100);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Place Shroom 1 after fight", shroom1, "place_shroom_after_fight", "")
    EventManager:RegisterEvent(event);


    shroom2 = CreateSprite(Map, "Shroom", 47, 24);
    shroom2:SetDirection(vt_map.MapMode.SOUTH);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("...");
    dialogue:AddLineEvent(text, shroom2, "", "Fight with Shroom 2");
    DialogueManager:AddDialogue(dialogue);
    shroom2:AddDialogueReference(dialogue);
    Map:AddGroundObject(shroom2);

    event = vt_map.BattleEncounterEvent("Fight with Shroom 2");
    event:AddEnemy(11, 512, 384); -- one shroom
    _SetEventBattleEnvironment(event);
    event:AddEventLinkAtEnd("Place Shroom 2 after fight", 100);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Place Shroom 2 after fight", shroom2, "place_shroom_after_fight", "")
    EventManager:RegisterEvent(event);

end

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- To the first cave
    event = vt_map.MapTransitionEvent("to exit 1", "dat/maps/mt_elbrus/mt_elbrus_path1_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path1_script.lua", "from_grotto_exit1");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to exit 2", "dat/maps/mt_elbrus/mt_elbrus_path1_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path1_script.lua", "from_grotto_exit2");
    EventManager:RegisterEvent(event);

end

-- zones
local exit1_zone = {};
local exit2_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    exit1_zone = vt_map.CameraZone(84, 94, 78, 80);
    Map:AddZone(exit1_zone);

    exit2_zone = vt_map.CameraZone(4, 15, 78, 80);
    Map:AddZone(exit2_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (exit1_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to exit 1");
    elseif (exit2_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to exit 2");
    end
end

-- Map Custom functions
-- Used through scripted events

map_functions = {

    place_shroom_after_fight = function(shroom)
        local hero_x = hero:GetXPosition();
        local hero_y = hero:GetYPosition();

        local shroom_x = shroom:GetXPosition();
        local shroom_y = shroom:GetYPosition();

        -- New position of the shroom
        local shroom_new_x = shroom_x;
        local shroom_new_y = shroom_y;

        -- Determine the hero position relative to the shroom
        if (hero_y > shroom_y + 0.3) then
            -- the hero is below, the shroom is pushed upward.
            shroom_new_y = shroom_new_y - 2.1;
        elseif (hero_y < shroom_y - 2.2) then
            -- the hero is above, the shroom is pushed downward.
            shroom_new_y = shroom_new_y + 2.2;
        elseif (hero_x < shroom_x - 1.2) then
            -- the hero is on the left, the shroom is pushed to the right.
            shroom_new_x = shroom_new_x + 2.1;
        elseif (hero_x > shroom_x + 1.2) then
            -- the hero is on the right, the shroom is pushed to the left.
            shroom_new_x = shroom_new_x - 2.1;
        end

        -- Place the shroom
        shroom:SetPosition(shroom_new_x, shroom_new_y);
        shroom:SetCustomAnimation("mushroom_ko", 999999);
        -- Remove its dialogue (preventing a new fight)
        shroom:ClearDialogueReferences();
    end
}
