-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine2_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus Shrine"
map_image_filename = "img/menus/locations/mountain_shrine.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/mountain_shrine.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};
local Script = {};

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
    Script = Map:GetScriptSupervisor();

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

    -- Amplify light gradually when entering the first time.
    if (GlobalManager:GetEventValue("story", "mountain_shrine_entrance_light_done") == 0) then
        Map:GetEffectSupervisor():EnableLightingOverlay(vt_video.Color(0.0, 0.0, 0.0, 0.45));
        EventManager:StartEvent("Amplify lights", 1500);
        GlobalManager:SetEventValue("story", "mountain_shrine_entrance_light_done", 1);
    end

end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from shrine entrance)
    hero = CreateSprite(Map, "Bronann", 33.5, 76.5);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    if (GlobalManager:GetPreviousLocation() == "from_shrine_stairs1") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(50.0, 10.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_trap_room") then
        hero:SetDirection(vt_map.MapMode.WEST);
        hero:SetPosition(58.0, 60.0);
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

    Map:AddHalo("img/misc/lights/torch_light_mask.lua", 33.5, 90,
        vt_video.Color(1.0, 1.0, 1.0, 0.8));

    _add_flame(1.5, 53);
    _add_flame(63.5, 53);
    _add_flame(19.5, 2);
    _add_flame(9.5, 2);
end

function _add_flame(x, y)
    local object = vt_map.SoundObject("snd/campfire.ogg", x, y, 5.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;
    object = vt_map.SoundObject("snd/campfire.ogg", x + 18.0, y, 5.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;

    object = CreateObject(Map, "Flame1", x, y);
    Map:AddGroundObject(object);

    Map:AddHalo("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    Map:AddHalo("img/misc/lights/sun_flare_light_main.lua", x, y + 1.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.2));
end

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(6, 19, 10, 49);

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
    enemy:SetBattleBackground("img/backdrops/battle/mountain_shrine.png");
    -- TODO: add some animations there.
    --enemy:AddBattleScript("dat/battles/tutorial_battle_dialogs.lua");
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    event = vt_map.MapTransitionEvent("to mountain shrine entrance", "dat/maps/mt_elbrus/mt_elbrus_shrine1_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine1_script.lua", "from_shrine_main_room");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine trap room", "dat/maps/mt_elbrus/mt_elbrus_shrine3_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine3_script.lua", "from_shrine_main_room");
    EventManager:RegisterEvent(event);

    -- Improve lighting at first entrance
    event = vt_map.ScriptedEvent("Amplify lights", "amplify_light_start", "amplify_light_update");
    EventManager:RegisterEvent(event);

end

-- zones
local to_shrine_entrance_zone = {};
local to_shrine_trap_room_zone = {};

-- Create the different map zones triggering events
function _CreateZones()

    -- N.B.: left, right, top, bottom
    to_shrine_entrance_zone = vt_map.CameraZone(26, 40, 78, 80);
    Map:AddZone(to_shrine_entrance_zone);

    to_shrine_trap_room_zone = vt_map.CameraZone(62, 64, 56, 62);
    Map:AddZone(to_shrine_trap_room_zone);

end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_entrance_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain shrine entrance");
    elseif (to_shrine_trap_room_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain shrine trap room");
    end
end

-- Used to display a fading darkness
local light_alpha = 0.45;
local light_color = vt_video.Color(0.0, 0.0, 0.0, 0.45);

-- Map Custom functions
-- Used through scripted events
map_functions = {
    amplify_light_start = function()
        light_alpha = 0.45;
        light_color:SetAlpha(light_alpha);
    end,

    amplify_light_update = function()
        light_alpha = light_alpha - (0.0002 * SystemManager:GetUpdateTime());
        if (light_alpha <= 0.0) then
            Map:GetEffectSupervisor():DisableLightingOverlay();
            return true;
        end
        light_color:SetAlpha(light_alpha);
        Map:GetEffectSupervisor():EnableLightingOverlay(light_color);
        return false;
    end,
}
