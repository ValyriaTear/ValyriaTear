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
        hero:SetPosition(12, 77); -- TODO
    end

    Map:AddGroundObject(hero);
end

-- The heal particle effect map object
local heal_effect = {};

function _CreateObjects()
    local object = {}
    local npc = {}

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

end

-- zones
local exit1_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    exit1_zone = vt_map.CameraZone(84, 94, 78, 80);
    Map:AddZone(exit1_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (exit1_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to exit 1");
    end
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/desert_cave/desert_cave.png");
    -- Add the background and foreground animations
    enemy:AddBattleScript("dat/battles/desert_cave_battle_anim.lua");
end

-- Map Custom functions
-- Used through scripted events

map_functions = {

}
