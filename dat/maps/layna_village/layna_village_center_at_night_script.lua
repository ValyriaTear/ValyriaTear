-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_center_at_night_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = "Village center"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = ""

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};
local Effects = {};

local bronann = {};
local kalya = {};

-- Main npcs
local orlinn = {};

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;
    Effects = Map:GetEffectSupervisor();

    Map.unlimited_stamina = true;

    _CreateCharacters();
    -- Set the camera focus on Bronann
    Map:SetCamera(bronann);

    _CreateNPCs();
    _CreateObjects();

    _CreateEvents();
    _CreateZones();

    -- Add clouds overlay
    Effects:EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");

    -- Set the world map current position
    GlobalManager:SetCurrentLocationId("layna village");
end

function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end


-- Character creation
function _CreateCharacters()
    bronann = CreateSprite(Map, "Bronann", 12, 63);
    bronann:SetDirection(hoa_map.MapMode.SOUTH);
    bronann:SetMovementSpeed(hoa_map.MapMode.NORMAL_SPEED);

    -- set up the position according to the previous map
    if (GlobalManager:GetPreviousLocation() == "from_riverbank") then
        bronann:SetPosition(30, 77);
        bronann:SetDirection(hoa_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_village_south") then
        bronann:SetPosition(79, 77);
        bronann:SetDirection(hoa_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_kalya_house_path") then
        bronann:SetPosition(3, 11);
        bronann:SetDirection(hoa_map.MapMode.EAST);
    elseif (GlobalManager:GetPreviousLocation() == "from_shop") then
        bronann:SetPosition(94, 72);
        bronann:SetDirection(hoa_map.MapMode.SOUTH);
        AudioManager:PlaySound("snd/door_close.wav");
    elseif (GlobalManager:GetPreviousLocation() == "from_secret_path") then
        bronann:SetPosition(3, 60);
        bronann:SetDirection(hoa_map.MapMode.EAST);
    elseif (GlobalManager:GetPreviousLocation() == "from sophia's house") then
        AudioManager:PlaySound("snd/door_close.wav");
        bronann:SetPosition(22, 20);
        bronann:SetDirection(hoa_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_layna_forest_entrance") then
        bronann:SetPosition(115, 37);
        bronann:SetDirection(hoa_map.MapMode.WEST);
    elseif (GlobalManager:GetPreviousLocation() == "from_bronanns_home") then
        AudioManager:PlaySound("snd/door_close.wav");
    end

    Map:AddGroundObject(bronann);
end

function _CreateNPCs()
    local npc = {}
    local text = {}
    local dialogue = {}
    local event = {}

    kalya = CreateSprite(Map, "Kalya", 0, 0);
    kalya:SetVisible(false);
    kalya:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(kalya);

    orlinn = CreateSprite(Map, "Orlinn", 40, 18);
    Map:AddGroundObject(orlinn);
end

function _CreateObjects()
    local object = {}

    object = CreateObject(Map, "Tree Big2", 22, 78);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 22, 16);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 9, 16);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 65, 18);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 74, 20);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 67, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 80, 36);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 92, 22);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 98, 24);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small2", 79, 16);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Rock1", 3, 64);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock2", 2, 62);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 33, 12);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Rock2", 29, 16);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock2", 109, 34);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock2", 113, 34);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock2", 117, 34);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock2", 109, 42);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock2", 117, 42);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock2", 113, 42);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- collision bug hidders
    object = CreateObject(Map, "Barrel1", 14, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Vase1", 15, 39);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Barrel1", 30, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Treasure vase
    local nekko_vase = CreateTreasure(Map, "layna_center_nekko_vase", "Vase1", 27, 37);
    if (nekko_vase ~= nil) then
        nekko_vase:AddObject(11, 1);
        Map:AddGroundObject(nekko_vase);
    end
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local text = {};
    local dialogue = {};

    -- Triggered Events
    event = hoa_map.MapTransitionEvent("to Riverbank", "dat/maps/layna_village/layna_village_riverbank_map.lua",
                                       "dat/maps/layna_village/layna_village_riverbank_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to Village south entrance", "dat/maps/layna_village/layna_village_south_entrance_map.lua",
                                       "dat/maps/layna_village/layna_village_south_entrance_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to Kalya house path", "dat/maps/layna_village/layna_village_kalya_house_path_map.lua",
                                       "dat/maps/layna_village/layna_village_kalya_house_path_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    -- Generic events
    event = hoa_map.ScriptedEvent("Map:PushState(SCENE)", "Map_SceneState", "");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("Map:PopState()", "Map_PopState", "");
    EventManager:RegisterEvent(event);

    -- Quest events
end

-- zones
local bronanns_home_entrance_zone = {};
local to_riverbank_zone = {};
local to_village_entrance_zone = {};
local to_kalya_house_path_zone = {};
local shop_entrance_zone = {};
local secret_path_zone = {};
local to_layna_forest_zone = {};
local sophia_house_entrance_zone = {};

function _CreateZones()
    -- N.B.: left, right, top, bottom
    bronanns_home_entrance_zone = hoa_map.CameraZone(10, 14, 60, 61, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(bronanns_home_entrance_zone);

    to_riverbank_zone = hoa_map.CameraZone(19, 35, 78, 79, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(to_riverbank_zone);

    to_village_entrance_zone = hoa_map.CameraZone(60, 113, 78, 79, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(to_village_entrance_zone);

    to_kalya_house_path_zone = hoa_map.CameraZone(0, 1, 8, 15, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(to_kalya_house_path_zone);

    shop_entrance_zone = hoa_map.CameraZone(92, 96, 70, 71, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(shop_entrance_zone);

    secret_path_zone = hoa_map.CameraZone(0, 1, 55, 61, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(secret_path_zone);

    to_layna_forest_zone = hoa_map.CameraZone(117, 119, 30, 43, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(to_layna_forest_zone);

    sophia_house_entrance_zone = hoa_map.CameraZone(21, 23, 21, 22, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(sophia_house_entrance_zone);
end

function _CheckZones()

end

-- Map Custom functions
map_functions = {
    Sprite_Collision_on = function(sprite)
        if (sprite ~= nil) then
            sprite:SetCollisionMask(hoa_map.MapMode.ALL_COLLISION);
        end
    end,

    Map_SceneState = function()
        Map:PushState(hoa_map.MapMode.STATE_SCENE);
    end,

    Map_PopState = function()
        Map:PopState();
    end,

    MakeInvisible = function(sprite)
        if (sprite ~= nil) then
            sprite:SetVisible(false);
            sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
        end
    end
}
