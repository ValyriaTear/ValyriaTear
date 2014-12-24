-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
dev_overworld_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = ""
map_image_filename = ""
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/overworld_present.ogg"

-- c++ objects instances
local Map = nil
local DialogueManager = nil
local EventManager = nil
local Effects = nil

-- the main character handler
local hero = nil

-- the main map loading code
function Load(m)

    Map = m;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;
    Effects = Map:GetEffectSupervisor();

    Map.unlimited_stamina = false;
    -- No running in overworlds
    Map.running_disabled = true;

    _CreateCharacters();
    --_CreateObjects();
    _CreateZones();
    _CreateEnemies();

    -- Set the camera focus on hero
    Map:SetCamera(hero);

    --_CreateEvents();

    -- Add clouds overlay
    Effects:EnableAmbientOverlay("img/ambient/clouds_overworld.png", 10.0, 5.0, true);
end

-- the map update function handles checks done on each game tick.
function Update()
    _CheckZones()
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Mini_Bronann", 37, 22, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.VERY_SLOW_SPEED);
end

function _CreateEnemies()
    local enemy = nil
    local roam_zone = nil

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(21, 43, 11, 33);

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
    roam_zone:AddEnemy(enemy, Map, 2);

    Map:AddZone(roam_zone);
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    -- default values
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    -- Put a default battle background
    enemy:SetBattleBackground("img/backdrops/battle/plains_background.png");
    -- This is an overworld enemy, so we reduce its speed, make it invisible
    -- and reduce also its agressive range.
    enemy:SetMovementSpeed(vt_map.MapMode.VERY_SLOW_SPEED);
    enemy:SetVisible(false)
    enemy:SetAggroRange(3.0);
    -- Add a workaround script permitting to dinamically change the battle background
    enemy:AddBattleScript("dat/maps/overworld/present/overworld_battle_background_script.lua");
end

local desert_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    desert_zone = vt_map.CameraZone(19, 29.1, 20.6, 25.5);
    desert_zone:AddSection(21, 28, 25.5, 27.5)
    desert_zone:AddSection(23, 25.7, 27.5, 28.5)
    desert_zone:AddSection(24, 25.3, 28.5, 29)
    Map:AddZone(desert_zone);
end

function _CheckZones()
    -- Update the battle background event value.
    if (desert_zone:IsCameraEntering() == true) then
        GlobalManager:SetEventValue("overworld", "battle_background", 3); -- desert
    elseif (desert_zone:IsCameraExiting() == true) then
        GlobalManager:SetEventValue("overworld", "battle_background", 1); -- plains
    end
end

-- Map Custom functions
-- Used through scripted events

map_functions = {
}
