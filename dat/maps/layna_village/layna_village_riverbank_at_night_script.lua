-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_riverbank_at_night_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = "Riverbank"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
-- TODO: Add dark music
music_filename = ""

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

local bronann = {};

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

    Map.unlimited_stamina = true;

    _CreateCharacters();
    -- Set the camera focus on Bronann
    Map:SetCamera(bronann);

    _CreateNPCs();
    _CreateObjects();

    _CreateEvents();
    _CreateZones();

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");
end

function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end


-- Character creation
function _CreateCharacters()
    bronann = CreateSprite(Map, "Bronann", 97, 4);
    bronann:SetDirection(hoa_map.MapMode.SOUTH);
    bronann:SetMovementSpeed(hoa_map.MapMode.NORMAL_SPEED);

    -- set up the position according to the previous map and location
    if (GlobalManager:GetPreviousLocation() == "from_village_south") then
        bronann:SetPosition(117, 18);
        bronann:SetDirection(hoa_map.MapMode.WEST);
    end

    if (GlobalManager:GetPreviousLocation() == "from_secret_path") then
        bronann:SetPosition(65, 4);
        bronann:SetDirection(hoa_map.MapMode.SOUTH);
    end

    if (GlobalManager:GetPreviousLocation() == "from_riverbank_house") then
        bronann:SetPosition(98, 48);
        bronann:SetDirection(hoa_map.MapMode.SOUTH);
        AudioManager:PlaySound("snd/door_close.wav");
    end

    Map:AddGroundObject(bronann);
end

function _CreateNPCs()
    local text = {}
    local dialogue = {}
    local event = {}

end

function _CreateObjects()
    local object = {}

    object = CreateObject(Map, "Tree Big2", 70, 6);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Add hill treasure chest
    local hill_chest = CreateTreasure(Map, "riverbank_secret_hill_chest", "Wood_Chest1", 72, 5);
    if (hill_chest ~= nil) then
        hill_chest:AddObject(1, 1);
        Map:AddGroundObject(hill_chest);
    end

    -- trees around the house
    object = CreateObject(Map, "Tree Big2", 92, 10);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 82, 17);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 75, 20);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 72, 35);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 74, 48);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 76, 50);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Barrel1", 56.3, 56.5);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- grass array, used to hide defects
    local map_grass = {
    { "Grass Clump1", 111, 7 },
    { "Grass Clump1", 108, 6 },
    { "Grass Clump1", 64, 15 },
    { "Grass Clump1", 62, 13 },
    { "Grass Clump1", 61, 6.2 },
    { "Grass Clump1", 65, 7 },
    { "Grass Clump1", 68, 10 },
    { "Grass Clump1", 72, 8.2 },
    { "Grass Clump1", 76, 8 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_grass) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        object:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(object);
    end

end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- Map change Events
    event = hoa_map.MapTransitionEvent("to Village center", "dat/maps/layna_village/layna_village_center_map.lua",
                                       "dat/maps/layna_village/layna_village_center_at_night_script.lua", "from_riverbank");
    EventManager:RegisterEvent(event);

end

-- zones
local village_center_zone = {};
local to_village_entrance_zone = {};
local to_riverbank_house_entrance_zone = {};

function _CreateZones()
    -- N.B.: left, right, top, bottom
    village_center_zone = hoa_map.CameraZone(89, 105, 0, 2, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(village_center_zone);

    to_village_entrance_zone = hoa_map.CameraZone(118, 119, 10, 27, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(to_village_entrance_zone);

    to_riverbank_house_entrance_zone = hoa_map.CameraZone(96, 100, 46, 47, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(to_riverbank_house_entrance_zone);
end

function _CheckZones()
    if (village_center_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to Village center");
    end

end

-- Map Custom functions
map_functions = {
    Map_PopState = function()
        Map:PopState();
    end,

    MakeInvisible = function(sprite)
        if (sprite ~= nil) then
            sprite:SetVisible(false);
            sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
        end
    end,

}
