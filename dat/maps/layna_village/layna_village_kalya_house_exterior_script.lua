-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_kalya_house_exterior_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/Caketown_1-OGA-mat-pablo.ogg"

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

    _HandleCredits();
end

-- Handle the display of the new game credits
function _HandleCredits()
    -- Handle small credits triggering
    if (GlobalManager:DoesEventExist("game", "Start_Credits") == false) then
        -- Triggers the small credits display
        GlobalManager:SetEventValue("game", "Start_Credits", 1);
    end
    if (GlobalManager:DoesEventExist("game", "Credits_shown") == false) then
        Map:GetScriptSupervisor():AddScript("dat/credits/episode1_credits.lua");
    end
end

function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end


-- Character creation
function _CreateCharacters()
    -- Default: From village center
    bronann = CreateSprite(Map, "Bronann", 48, 44);
    bronann:SetDirection(hoa_map.MapMode.NORTH);
    bronann:SetMovementSpeed(hoa_map.MapMode.NORMAL_SPEED);

    -- set up the position according to the previous map
    if (GlobalManager:GetPreviousLocation() == "from_kalya_house_interior") then
        bronann:SetPosition(44, 18);
        bronann:SetDirection(hoa_map.MapMode.SOUTH);
    end

    if (GlobalManager:GetPreviousLocation() == "from_kalya_house_path_small_passage") then
        bronann:SetPosition(2, 20);
        bronann:SetDirection(hoa_map.MapMode.EAST);
    end

    Map:AddGroundObject(bronann);
end

function _CreateNPCs()
    local npc = {}
    local text = {}
    local dialogue = {}
    local event = {}

end

function _CreateObjects()
    local object = {}

    -- right part trees
    object = CreateObject(Map, "Tree Big1", 62, 18);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 63, 21);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 61, 25);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 59, 29);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small2", 63, 28);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 62, 35);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 58, 30);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 63, 39);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 64, 43);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 59, 46);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- trees in the middle
    object = CreateObject(Map, "Tree Small2", 45, 31);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 43, 28);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 41, 40);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Rock1", 17, 13);
    if (object ~= nil) then Map:AddGroundObject(object) end;


    -- Left part
    object = CreateObject(Map, "Tree Small2", 10, 12);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 4, 17);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Tree Big1", 19, 15);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 17, 19);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small2", 15, 17);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 11, 22);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 16, 23);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 12, 25);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 14, 28);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 12, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 9, 35);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 6, 31);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Treasures!
    local kalya_house_exterior_chest = CreateTreasure(Map, "kalya_house_exterior_chest", "Wood_Chest1", 5, 22);
    if (kalya_house_exterior_chest ~= nil) then
        kalya_house_exterior_chest:AddObject(1, 1);
        Map:AddGroundObject(kalya_house_exterior_chest);
    end
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {}

    -- Triggered Events
    event = hoa_map.MapTransitionEvent("to Kalya house path", "dat/maps/layna_village/layna_village_kalya_house_path_map.lua",
                                       "dat/maps/layna_village/layna_village_kalya_house_path_script.lua", "from_kalya_house_exterior");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to kalya house path small passage", "dat/maps/layna_village/layna_village_kalya_house_path_map.lua",
                                       "dat/maps/layna_village/layna_village_kalya_house_path_script.lua", "from_kalya_house_small_passage");
    EventManager:RegisterEvent(event);

    -- Kalya house locked door event
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Hmm, the door is locked.");
    dialogue:AddLine(text, bronann);
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Bronann can't enter kalya house", dialogue);
    event:SetStopCameraMovement(true);
    EventManager:RegisterEvent(event);
end

-- zones
local kalya_house_path_zone = {};
local kalya_house_path_small_passage_zone = {};
local kalya_house_entrance_zone = {};

function _CreateZones()
    -- N.B.: left, right, top, bottom
    kalya_house_path_zone = hoa_map.CameraZone(28, 58, 46, 47, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(kalya_house_path_zone);

    kalya_house_path_small_passage_zone = hoa_map.CameraZone(0, 1, 0, 33, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(kalya_house_path_small_passage_zone);

    kalya_house_entrance_zone = hoa_map.CameraZone(42, 46, 16, 17, hoa_map.MapMode.CONTEXT_01);
    Map:AddZone(kalya_house_entrance_zone);
end

function _CheckZones()
    if (kalya_house_path_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to Kalya house path");
    end

    if (kalya_house_entrance_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("Bronann can't enter kalya house");
        AudioManager:PlaySound("snd/door_close.wav");
    end

    if (kalya_house_path_small_passage_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to kalya house path small passage");
    end
end


-- Map Custom functions
map_functions = {

}
