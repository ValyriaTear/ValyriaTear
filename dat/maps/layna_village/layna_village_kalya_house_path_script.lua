-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_kalya_house_path_script = ns;
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
    bronann = CreateSprite(Map, "Bronann", 61, 45);
    bronann:SetDirection(vt_map.MapMode.WEST);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- set up the position according to the previous map
    if (GlobalManager:GetPreviousLocation() == "from_kalya_house_exterior") then
        bronann:SetPosition(43, 3);
        bronann:SetDirection(vt_map.MapMode.SOUTH);
    end
    if (GlobalManager:GetPreviousLocation() == "from grandma house") then
        bronann:SetPosition(12, 6);
        bronann:SetDirection(vt_map.MapMode.NORTH);
        AudioManager:PlaySound("snd/door_close.wav");
    end

    if (GlobalManager:GetPreviousLocation() == "from_kalya_house_small_passage") then
        bronann:SetPosition(6, 3);
        bronann:SetDirection(vt_map.MapMode.WEST);
    end

    Map:AddGroundObject(bronann);
end

function _CreateNPCs()
    local npc = {}
    local text = {}
    local dialogue = {}
    local event = {}

    npc = CreateNPCSprite(Map, "Old Woman1", "Brymir", 7, 25);
    Map:AddGroundObject(npc);
    npc:SetDirection(vt_map.MapMode.SOUTH);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Ah! It's nice to see your dear young face around, Bronann. Come and chat with an old grandma.");
    dialogue:AddLine(text, npc);
    text = vt_system.Translate("Er... Sorry grandma, I have to go! Maybe later?");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("Ah! You'll surely want to see the young lady living up there. Ah, youngins nowadays...");
    dialogue:AddLine(text, npc);
    DialogueManager:AddDialogue(dialogue);
    npc:AddDialogueReference(dialogue);

    npc = CreateSprite(Map, "Chicken", 21, 36);
    Map:AddGroundObject(npc);
    event = vt_map.RandomMoveSpriteEvent("Chicken1 random move", npc, 1000, 1000);
    event:AddEventLinkAtEnd("Chicken1 random move", 4500); -- Loop on itself
    EventManager:RegisterEvent(event);
    EventManager:StartEvent("Chicken1 random move");

    npc = CreateSprite(Map, "Chicken", 19, 34);
    Map:AddGroundObject(npc);
    event = vt_map.RandomMoveSpriteEvent("Chicken2 random move", npc, 1000, 1000);
    event:AddEventLinkAtEnd("Chicken2 random move", 4500); -- Loop on itself
    EventManager:RegisterEvent(event);
    EventManager:StartEvent("Chicken2 random move", 1200);

    npc = CreateSprite(Map, "Chicken", 23, 33);
    Map:AddGroundObject(npc);
    event = vt_map.RandomMoveSpriteEvent("Chicken3 random move", npc, 1000, 1000);
    event:AddEventLinkAtEnd("Chicken3 random move", 4500); -- Loop on itself
    EventManager:RegisterEvent(event);
    EventManager:StartEvent("Chicken3 random move", 2100);
end

function _CreateObjects()
    local object = {}

    -- Left tree "wall"
    object = CreateObject(Map, "Tree Big1", 0, 44);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 42);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 40);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 36);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 34);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 0, 30);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 28);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 26);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 24);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 0, 22);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 20);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 18);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 16);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 14);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 12);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock2", 0, 10);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 7);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 5);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 0, 3);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 15, 3);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 15.5, 1.5);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Right tree "Wall"
    object = CreateObject(Map, "Rock2", 63, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 63, 36);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 63, 34);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 63, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 63, 30);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small2", 63.5, 28);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 63, 26);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 63, 24);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 63, 22);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 63, 20);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 63, 18);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small2", 64.5, 16);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 63, 12);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small2", 64, 10);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 63, 8);
    if (object ~= nil) then Map:AddGroundObject(object) end;


    -- Secret shortcut hiders
    object = CreateObject(Map, "Tree Big1", 38, 40);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 40, 42);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 42, 40);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Cliff hiders
    object = CreateObject(Map, "Tree Small1", 14, 30);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 55, 12);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Fence
    object = CreateObject(Map, "Fence1 l top left", 17, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 19, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 21, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 23, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 25, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 27, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 l top right", 29, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Fence1 vertical", 17, 34);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 vertical", 17, 36);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Fence1 l bottom left", 17, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 19, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 21, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 23, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 25, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 27, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 l bottom right", 29, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 vertical", 29, 34);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 vertical", 29, 36);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Bench1", 7, 24);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Barrel1", 20, 24);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Secret treasure chest
    local chest = CreateTreasure(Map, "kalya_house_path_chest", "Wood_Chest1", 8, 8);
    if (chest ~= nil) then
        chest:AddObject(1001, 1);
        Map:AddGroundObject(chest);
    end
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};

    -- Triggered Events
    event = vt_map.MapTransitionEvent("to Village center", "dat/maps/layna_village/layna_village_center_map.lua",
                                       "dat/maps/layna_village/layna_village_center_script.lua", "from_kalya_house_path");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to Kalya house exterior", "dat/maps/layna_village/layna_village_kalya_house_exterior_map.lua",
                                       "dat/maps/layna_village/layna_village_kalya_house_exterior_script.lua", "from_kalya_house_path");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to grandma house", "dat/maps/layna_village/layna_village_kalya_house_path_small_house_map.lua",
                                       "dat/maps/layna_village/layna_village_kalya_house_path_small_house_script.lua", "from_kalya_house_path");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to Kalya house small passage", "dat/maps/layna_village/layna_village_kalya_house_exterior_map.lua",
                                       "dat/maps/layna_village/layna_village_kalya_house_exterior_script.lua", "from_kalya_house_path_small_passage");
    EventManager:RegisterEvent(event);
end

-- zones
local village_center_zone = {};
local kalya_house_exterior_zone = {};
local grandma_house_entrance_zone = {};
local kalya_house_small_passage_zone = {};

function _CreateZones()
    -- N.B.: left, right, top, bottom
    village_center_zone = vt_map.CameraZone(62, 63, 42, 47);
    Map:AddZone(village_center_zone);

    kalya_house_exterior_zone = vt_map.CameraZone(26, 56, 0, 2);
    Map:AddZone(kalya_house_exterior_zone);

    grandma_house_entrance_zone = vt_map.CameraZone(11, 13, 7, 8);
    Map:AddZone(grandma_house_entrance_zone);

    kalya_house_small_passage_zone = vt_map.CameraZone(3, 8, 0, 1);
    Map:AddZone(kalya_house_small_passage_zone);
end

function _CheckZones()
    if (village_center_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        EventManager:StartEvent("to Village center");
    end

    if (kalya_house_exterior_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        EventManager:StartEvent("to Kalya house exterior");
    end

    if (grandma_house_entrance_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        EventManager:StartEvent("to grandma house");
        AudioManager:PlaySound("snd/door_open2.wav");
    end

    if (kalya_house_small_passage_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        EventManager:StartEvent("to Kalya house small passage");
    end
end


-- Map Custom functions
map_functions = {

}
