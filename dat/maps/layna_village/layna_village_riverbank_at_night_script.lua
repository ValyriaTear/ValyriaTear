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
music_filename = "mus/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg"

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

local herth = {};
local orlinn = {};
local kalya = {};
local lilly = {};
local carson = {};
local malta = {};
local brymir = {};
local martha = {};
local georges = {};
local olivia = {};

local soldier1 = {};
local soldier2 = {};
local soldier3 = {};
local soldier4 = {};
local soldier5 = {};
local soldier6 = {};
local soldier7 = {};
local soldier8 = {};
local soldier9 = {};
local soldier10 = {};
local soldier11 = {};
local soldier12 = {};
local soldier13 = {};
local soldier14 = {};
local soldier15 = {};
local soldier16 = {};

local soldier17 = {};
local soldier18 = {};
local soldier19 = {};
local soldier20 = {};

local lord = {};

function _CreateNPCs()
    local text = {}
    local dialogue = {}
    local event = {}

    herth = CreateSprite(Map, "Herth", 86, 55);
    herth:SetDirection(hoa_map.MapMode.EAST);
    Map:AddGroundObject(herth);

    orlinn = CreateSprite(Map, "Orlinn", 76, 44);
    orlinn:SetDirection(hoa_map.MapMode.EAST);
    orlinn:SetMovementSpeed(hoa_map.MapMode.VERY_FAST_SPEED);
    Map:AddGroundObject(orlinn);

    kalya = CreateSprite(Map, "Kalya", 77, 55);
    kalya:SetDirection(hoa_map.MapMode.EAST);
    Map:AddGroundObject(kalya);

    lilly = CreateNPCSprite(Map, "Woman3", "Lilly", 95, 54);
    lilly:SetDirection(hoa_map.MapMode.EAST);
    Map:AddGroundObject(lilly);

    carson = CreateSprite(Map, "Carson", 97, 61);
    carson:SetDirection(hoa_map.MapMode.NORTH);
    Map:AddGroundObject(carson);

    malta = CreateSprite(Map, "Malta", 95, 62);
    malta:SetDirection(hoa_map.MapMode.NORTH);
    Map:AddGroundObject(malta);

    brymir = CreateNPCSprite(Map, "Old Woman1", "Brymir", 87, 61.5);
    brymir:SetDirection(hoa_map.MapMode.NORTH);
    Map:AddGroundObject(brymir);

    martha = CreateNPCSprite(Map, "Woman1", "Martha", 89, 61);
    martha:SetDirection(hoa_map.MapMode.NORTH);
    Map:AddGroundObject(martha);

    georges = CreateNPCSprite(Map, "Man1", "Georges", 105, 61.5);
    georges:SetDirection(hoa_map.MapMode.NORTH);
    Map:AddGroundObject(georges);

    olivia = CreateNPCSprite(Map, "Girl1", "Olivia", 107, 61);
    olivia:SetDirection(hoa_map.MapMode.NORTH);
    Map:AddGroundObject(olivia);

    -- Create the soldiers
    soldier1 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 88, 59);
    soldier1:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier1);
    soldier2 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 96, 59);
    soldier2:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier2);
    soldier3 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 106, 59.5);
    soldier3:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier3);
    soldier4 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 88, 55);
    soldier4:SetDirection(hoa_map.MapMode.WEST);
    Map:AddGroundObject(soldier4);
    soldier5 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 99, 59);
    soldier5:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier5);
    soldier6 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 102.5, 59.2);
    soldier6:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier6);
    soldier7 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 93, 59.3);
    soldier7:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier7);
    soldier8 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 90.6, 59.6);
    soldier8:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier8);
    soldier9 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 108, 57);
    soldier9:SetDirection(hoa_map.MapMode.WEST);
    Map:AddGroundObject(soldier9);
    soldier10 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 108, 54);
    soldier10:SetDirection(hoa_map.MapMode.WEST);
    Map:AddGroundObject(soldier10);
    soldier11 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 103, 50);
    soldier11:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier11);
    soldier12 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 90, 50);
    soldier12:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier12);
    soldier13 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 100, 48);
    soldier13:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier13);
    soldier14 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 96, 48);
    soldier14:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier14);
    soldier15 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 84, 53);
    soldier15:SetDirection(hoa_map.MapMode.EAST);
    Map:AddGroundObject(soldier15);
    soldier16 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 84, 57);
    soldier16:SetDirection(hoa_map.MapMode.EAST);
    Map:AddGroundObject(soldier16);

    lord = CreateNPCSprite(Map, "Lord", "??", 100, 54);
    lord:SetDirection(hoa_map.MapMode.WEST);
    Map:AddGroundObject(lord);

    -- soldiers around kalya 77,55
    soldier17 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 79, 55);
    soldier17:SetDirection(hoa_map.MapMode.EAST);
    Map:AddGroundObject(soldier17);
    soldier18 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 77, 53);
    soldier18:SetDirection(hoa_map.MapMode.EAST);
    Map:AddGroundObject(soldier18);
    soldier19 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 77, 57);
    soldier19:SetDirection(hoa_map.MapMode.EAST);
    Map:AddGroundObject(soldier19);
    soldier20 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 75, 55);
    soldier20:SetDirection(hoa_map.MapMode.EAST);
    Map:AddGroundObject(soldier20);

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

    -- Add burning flames halos and objects
    _CreateFire(84, 51.2);
    _CreateFire(83, 59);
    _CreateFire(97, 51);
    _CreateFire(111, 58);
    _CreateFire(109, 52);

    -- Ambient sounds
    object = hoa_map.SoundObject("snd/gentle_stream.ogg", 61.0, 27.0, 20.0);
    Map:AddAmbientSoundObject(object);

    object = hoa_map.SoundObject("snd/gentle_stream.ogg", 61.0, 47.0, 20.0);
    Map:AddAmbientSoundObject(object);

    object = hoa_map.SoundObject("snd/gentle_stream.ogg", 61.0, 67.0, 20.0);
    Map:AddAmbientSoundObject(object);

    object = hoa_map.SoundObject("snd/gentle_stream.ogg", 81.0, 67.0, 20.0);
    Map:AddAmbientSoundObject(object);

    object = hoa_map.SoundObject("snd/gentle_stream.ogg", 101.0, 67.0, 20.0);
    Map:AddAmbientSoundObject(object);

    -- Fireflies on water
    object = hoa_map.ParticleObject("dat/effects/particles/fireflies.lua", 53, 29);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);

    object = hoa_map.ParticleObject("dat/effects/particles/fireflies.lua", 52, 38);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);
    object = hoa_map.ParticleObject("dat/effects/particles/fireflies.lua", 54, 40);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);
    object = hoa_map.ParticleObject("dat/effects/particles/fireflies.lua", 99, 73);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);
end

-- creates the necessary objects to display some fire
function _CreateFire(fire_x, fire_y)
    -- Add burning flames halos and objects
    local object = CreateObject(Map, "Campfire1", fire_x, fire_y);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    Map:AddHalo("img/misc/lights/torch_light_mask2.lua", fire_x, fire_y + 3.0,
        hoa_video.Color(0.85, 0.32, 0.0, 0.6));
    Map:AddHalo("img/misc/lights/sun_flare_light_main.lua", fire_x, fire_y + 1.0,
        hoa_video.Color(0.99, 1.0, 0.27, 0.2));
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
    village_center_zone = hoa_map.CameraZone(89, 105, 0, 2);
    Map:AddZone(village_center_zone);

    to_village_entrance_zone = hoa_map.CameraZone(118, 119, 10, 27);
    Map:AddZone(to_village_entrance_zone);

    to_riverbank_house_entrance_zone = hoa_map.CameraZone(96, 100, 46, 47);
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
