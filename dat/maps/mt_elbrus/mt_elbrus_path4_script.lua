-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_path4_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "img/menus/locations/mt_elbrus.png"
map_subname = "Mountain bridge"

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
    --_CreateEnemies();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/snow_fog.png", 30.0, 10.0, true);
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");

    -- Snowing
    GlobalManager:SetEventValue("story", "mt_elbrus_weather_level", 3)

    -- Make the rain starts or the corresponding dialogue according the need
    Map:GetParticleManager():AddParticleEffect("dat/effects/particles/snow.lua", 512.0, 384.0);

    -- The mountain cliff background
    Map:GetScriptSupervisor():AddScript("dat/maps/mt_elbrus/mt_elbrus_background_anim.lua");

end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from mountain path 3)
    hero = CreateSprite(Map, "Bronann", 55, 77);
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
    elseif (GlobalManager:GetPreviousLocation() == "from_path3") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(10.0, 90.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_mountain_grotto_entrance") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(50.0, 10.0);
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

function _CreateObjects()
    local object = {}
    local npc = {}
    local dialogue = {}
    local text = {}
    local event = {}


    -- Objects array
    local map_objects = {

        { "Tree Big2", 91, 87 },
        { "Rock2", 73, 75 },

    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_objects) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        Map:AddGroundObject(object);
    end

    -- Create the bridge
    object = CreateObject(Map, "Bridge1_down", 36.5, 39);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 37);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 35);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 33);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 31);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 29);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 27);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 25);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 23);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 21);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 19);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 17);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 15);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_middle", 36.5, 13);
    Map:AddFlatGroundObject(object);
    object = CreateObject(Map, "Bridge1_up", 36.5, 11);
    Map:AddFlatGroundObject(object);
end

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(65, 68, 32, 36);
    -- Some bats
    enemy = CreateEnemySprite(Map, "Eyeball");
    _SetBattleEnvironment(enemy);
    -- Adds a quicker respawn time
    enemy:NewEnemyParty();
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    enemy:AddEnemy(12);
    roam_zone:AddEnemy(enemy, Map, 1);
    Map:AddZone(roam_zone);

end

-- Special event references which destinations must be updated just before being called.
local kalya_move_next_to_hero_event1 = {}
local kalya_move_back_to_hero_event1 = {}
local orlinn_move_next_to_hero_event1 = {}
local orlinn_move_back_to_hero_event1 = {}

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to mountain path 3", "dat/maps/mt_elbrus/mt_elbrus_path3_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_path3_script.lua", "from_path4");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine", "dat/maps/mt_elbrus/mt_elbrus_shrine1_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine1_script.lua", "from_path4");
    EventManager:RegisterEvent(event);

    -- Heal point
    event = vt_map.ScriptedEvent("Heal event", "heal_party", "heal_done");
    EventManager:RegisterEvent(event);

    -- sprite direction events
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks north", hero, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);


end

-- zones
local to_shrine_zone = {};
local to_path3_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_zone = vt_map.CameraZone(40, 55, 0, 2);
    Map:AddZone(to_shrine_zone);
    to_path3_zone = vt_map.CameraZone(53, 74, 94, 96);
    Map:AddZone(to_path3_zone);

end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_path3_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain path 3");
    end

end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    -- default values
    enemy:SetBattleMusicTheme("mus/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_background.png");
    enemy:AddBattleScript("dat/maps/common/at_night.lua");

    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 0) then
        enemy:AddBattleScript("dat/maps/common/rain_in_battles_script.lua");
    end
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 1) then
        enemy:AddBattleScript("dat/maps/common/soft_lightnings_script.lua");
    end
end
-- The environment for Harlequin's battles
function _SetEventBattleEnvironment(event)
    event:SetBackground("img/backdrops/battle/mountain_background.png");
    event:AddScript("dat/maps/common/at_night.lua");

    event:AddScript("dat/maps/common/snow_in_battles_script.lua");
end


-- Map Custom functions
-- Used through scripted events
map_functions = {


    set_scene_state = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

}
