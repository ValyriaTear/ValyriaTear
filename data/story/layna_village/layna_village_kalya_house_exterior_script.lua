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
music_filename = "music/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

local bronann = nil

-- the main map loading code
function Load(m)

    Map = m;
    EventManager = Map:GetEventSupervisor();

    Map:SetUnlimitedStamina(true);

    _CreateCharacters();
    -- Set the camera focus on Bronann
    Map:SetCamera(bronann);

    _CreateNPCs();
    _CreateObjects();

    _CreateEvents();
    _CreateZones();

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, -5.0, true);

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
    bronann = CreateSprite(Map, "Bronann", 48, 44, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.NORTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- set up the position according to the previous map
    if (GlobalManager:GetPreviousLocation() == "from_kalya_house_interior") then
        bronann:SetPosition(44, 18);
        bronann:SetDirection(vt_map.MapMode.SOUTH);
    end

    if (GlobalManager:GetPreviousLocation() == "from_kalya_house_path_small_passage") then
        bronann:SetPosition(2, 20);
        bronann:SetDirection(vt_map.MapMode.EAST);
    end
end

local chicken3 = nil

function _CreateNPCs()
    local npc = nil
    local text = nil
    local dialogue = nil
    local event = nil

    -- Adds a chicken that can be taken by Bronann and given back to Grandma.
    if (GlobalManager:GetEventValue("game", "layna_village_chicken3_found") == 0) then
        chicken3 = CreateSprite(Map, "Chicken", 55, 23, vt_map.MapMode.GROUND_OBJECT);

        event = vt_map.RandomMoveSpriteEvent.Create("Chicken3 random move", chicken3, 1000, 1000);
        event:AddEventLinkAtEnd("Chicken3 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Chicken3 random move");

        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("One of Grandma's chickens. I should bring it back.");
        dialogue:AddLine(text, bronann);
        dialogue:SetEventAtDialogueEnd("Make bronann take the chicken 3");
        chicken3:AddDialogueReference(dialogue);

        vt_map.ScriptedEvent.Create("Make bronann take the chicken 3", "bronann_takes_chicken3", "fadeoutin_update");
    end
end

function _CreateObjects()
    -- right part trees
    CreateObject(Map, "Tree Big1", 62, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 63, 21, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 61, 25, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 59, 29, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small2", 63, 28, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 62, 35, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 58, 30, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 63, 39, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 64, 43, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 59, 46, vt_map.MapMode.GROUND_OBJECT);

    -- trees in the middle
    CreateObject(Map, "Tree Small2", 45, 31, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 43, 28, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 41, 40, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 17, 13, vt_map.MapMode.GROUND_OBJECT);

    -- Left part
    CreateObject(Map, "Tree Small2", 10, 12, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 4, 17, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 19, 15, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 17, 19, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small2", 15, 17, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 11, 22, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 16, 23, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 12, 25, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 14, 28, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 12, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 9, 35, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 6, 31, vt_map.MapMode.GROUND_OBJECT);

    -- Treasures!
    local kalya_house_exterior_chest = CreateTreasure(Map, "kalya_house_exterior_chest", "Wood_Chest1", 5, 22, vt_map.MapMode.GROUND_OBJECT);
    kalya_house_exterior_chest:AddItem(1, 1);

    vt_map.SoundObject.Create("sounds/gentle_stream.ogg", 10.0, 46.0, 20.0);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Triggered Events
    vt_map.MapTransitionEvent.Create("to Kalya house path", "dat/maps/layna_village/layna_village_kalya_house_path_map.lua",
                                     "dat/maps/layna_village/layna_village_kalya_house_path_script.lua", "from_kalya_house_exterior");

    vt_map.MapTransitionEvent.Create("to kalya house path small passage", "dat/maps/layna_village/layna_village_kalya_house_path_map.lua",
                                     "dat/maps/layna_village/layna_village_kalya_house_path_script.lua", "from_kalya_house_small_passage");

    -- Kalya house locked door event
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Hmm, the door is locked.");
    dialogue:AddLine(text, bronann);
    event = vt_map.DialogueEvent.Create("Bronann can't enter kalya house", dialogue);
    event:SetStopCameraMovement(true);
end

-- zones
local kalya_house_path_zone = nil
local kalya_house_path_small_passage_zone = nil
local kalya_house_entrance_zone = nil

function _CreateZones()
    -- N.B.: left, right, top, bottom
    kalya_house_path_zone = vt_map.CameraZone.Create(28, 58, 46, 47);
    kalya_house_path_small_passage_zone = vt_map.CameraZone.Create(0, 1, 0, 33);
    kalya_house_entrance_zone = vt_map.CameraZone.Create(42, 46, 16, 17);
end

function _CheckZones()
    if (kalya_house_path_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to Kalya house path");
    end

    if (kalya_house_entrance_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("Bronann can't enter kalya house");
        AudioManager:PlaySound("sounds/door_close.wav");
    end

    if (kalya_house_path_small_passage_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to kalya house path small passage");
    end
end

-- Effect time used when fade out and in
local fade_effect_time = 0;
local fade_color = vt_video.Color(0.0, 0.0, 0.0, 1.0);
local chicken3_taken = false;

-- Map Custom functions
map_functions = {
    bronann_takes_chicken3 = function()
        chicken3_taken = false;
        fade_effect_time = 0;
        chicken3:SetMoving(false);
        EventManager:EndAllEvents(chicken3);
        bronann:SetMoving(false);
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

    fadeoutin_update = function()
        fade_effect_time = fade_effect_time + SystemManager:GetUpdateTime();

        if (fade_effect_time < 300.0) then
            fade_color:SetAlpha(fade_effect_time / 300.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(fade_color);
            return false;
        elseif (fade_effect_time >= 300.0 and fade_effect_time < 1300.0) then
            -- do nothing
            return false;
        elseif (fade_effect_time < 2000.0) then
            -- actually remove the chicken
            if (chicken3_taken == false) then
                chicken3:SetVisible(false);
                chicken3:SetPosition(0, 0);
                GlobalManager:SetEventValue("game", "layna_village_chicken3_found", 1)
                -- Set the quest start dialogue as done if not already, so a possible later
                -- dialogue with grandma sounds more logical
                GlobalManager:SetEventValue("game", "layna_village_chicken_dialogue_done", 1);
                chicken3_taken = true;
            end
            fade_color:SetAlpha((2000.0 - fade_effect_time) / 700.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(fade_color);
            return false;
        end
        -- Free Bronann's movement
        Map:PopState();
        return true;
    end,
}
