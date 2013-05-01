-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_south_entrance_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = "Village entrance"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

local bronann = {};
local orlinn = {};

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
    bronann = CreateSprite(Map, "Bronann", 32, 4);
    bronann:SetDirection(vt_map.MapMode.SOUTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- set up the position according to the previous map
    if (GlobalManager:GetPreviousLocation() == "from_riverbank") then
        bronann:SetPosition(3, 34);
        bronann:SetDirection(vt_map.MapMode.EAST);

    elseif (GlobalManager:GetPreviousLocation() == "from right house") then
        bronann:SetPosition(48, 34);
        bronann:SetDirection(vt_map.MapMode.SOUTH);
        AudioManager:PlaySound("snd/door_close.wav");

    elseif (GlobalManager:GetPreviousLocation() == "from left house") then
        bronann:SetPosition(20, 34);
        bronann:SetDirection(vt_map.MapMode.SOUTH);
        AudioManager:PlaySound("snd/door_close.wav");
    end

    Map:AddGroundObject(bronann);
end

function _CreateNPCs()
    local npc = {}
    local text = {}
    local dialogue = {}
    local event = {}

    npc = CreateSprite(Map, "Herth", 45, 39);
    Map:AddGroundObject(npc);
    if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == true) then
        -- At that moment, Herth isn't there anymore.
        npc:SetVisible(false);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    else
        npc:SetDirection(vt_map.MapMode.SOUTH);
        dialogue = vt_map.SpriteDialogue();
        text = vt_system.Translate("Hi Bronann.");
        dialogue:AddLine(text, npc);
        text = vt_system.Translate("Hi Herth. I see you've blocked the gate, why so?");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Don't worry too much. I'm just preventing strangers from being able to sneak in at night.");
        dialogue:AddLine(text, npc);
        text = vt_system.Translate("There have been some reports of theft in the villages nearby recently.");
        dialogue:AddLine(text, npc);
        text = vt_system.Translate("Wow, do you think that they would come here?");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("It's a possibility. But don't worry too much, ok?");
        dialogue:AddLine(text, npc);
        DialogueManager:AddDialogue(dialogue);
        npc:AddDialogueReference(dialogue);
        -- The second time, just repeat the sentence
        dialogue = vt_map.SpriteDialogue("ep1_layna_village_herth_south_entrance_default");
        text = vt_system.Translate("It's a possibility. But don't worry too much, ok?");
        dialogue:AddLine(text, npc);
        DialogueManager:AddDialogue(dialogue);
        npc:AddDialogueReference(dialogue);
    end

    orlinn = CreateSprite(Map, "Orlinn", 29, 22);
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);
    Map:AddGroundObject(orlinn);
    _UpdateOrlinnState();
end

function _CreateObjects()
    local object = {}

    object = CreateObject(Map, "Tree Big1", 42, 10);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Small door lights
    Map:AddLight("img/misc/lights/sun_flare_light_small_main.lua",
            "img/misc/lights/sun_flare_light_small_secondary.lua",
        20.0, 29.5,
        vt_video.Color(1.0, 1.0, 1.0, 0.6),
        vt_video.Color(1.0, 1.0, 0.85, 0.3));
    Map:AddLight("img/misc/lights/sun_flare_light_small_main.lua",
            "img/misc/lights/sun_flare_light_small_secondary.lua",
        48.0, 29.5,
        vt_video.Color(1.0, 1.0, 1.0, 0.6),
        vt_video.Color(1.0, 1.0, 0.85, 0.3));
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};

    -- Triggered Events
    event = vt_map.MapTransitionEvent("to Village center", "dat/maps/layna_village/layna_village_center_map.lua",
                                       "dat/maps/layna_village/layna_village_center_script.lua", "from_village_south");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to Village riverbank", "dat/maps/layna_village/layna_village_riverbank_map.lua",
                                       "dat/maps/layna_village/layna_village_riverbank_script.lua", "from_village_south");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to left house", "dat/maps/layna_village/layna_village_south_entrance_left_house_map.lua",
                                       "dat/maps/layna_village/layna_village_south_entrance_left_house_script.lua", "from_village_south");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to right house", "dat/maps/layna_village/layna_village_south_entrance_right_house_map.lua",
                                       "dat/maps/layna_village/layna_village_south_entrance_right_house_script.lua", "from_village_south");
    EventManager:RegisterEvent(event);

    -- Orlinn events
    event = vt_map.ScriptedEvent("Quest1: Start Orlinn Hide n Seek2", "Quest1_Orlinn_Start_Hide_N_Seek2", "");
    event:AddEventLinkAtEnd("Quest1: Make Orlinn run");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Quest1: Make Orlinn run", orlinn, 30, 2, true);
    event:AddEventLinkAtEnd("Quest1: Make Orlinn disappear");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Quest1: Make Orlinn disappear", orlinn, "MakeInvisible", "");
    EventManager:RegisterEvent(event);
end

-- zones
local village_center_zone = {};
local to_village_riverbank_zone = {};
local to_left_house_zone = {};
local to_right_house_zone = {};

function _CreateZones()
    -- N.B.: left, right, top, bottom
    village_center_zone = vt_map.CameraZone(8, 62, 0, 2);
    Map:AddZone(village_center_zone);

    to_village_riverbank_zone = vt_map.CameraZone(0, 1, 26, 43);
    Map:AddZone(to_village_riverbank_zone);

    to_left_house_zone = vt_map.CameraZone(18, 22, 32, 33);
    Map:AddZone(to_left_house_zone);

    to_right_house_zone = vt_map.CameraZone(46, 50, 32, 33);
    Map:AddZone(to_right_house_zone);
end

function _CheckZones()
    if (village_center_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        EventManager:StartEvent("to Village center");
    end

    if (to_village_riverbank_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        EventManager:StartEvent("to Village riverbank");
    end

    if (to_left_house_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        AudioManager:PlaySound("snd/door_open2.wav");
        EventManager:StartEvent("to left house");
    end

    if (to_right_house_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        AudioManager:PlaySound("snd/door_open2.wav");
        EventManager:StartEvent("to right house");
    end
end

-- Custom inner map functions

function _UpdateOrlinnState()
    local text = {}
    local dialogue = {}
    local event = {}

    event = vt_map.PathMoveSpriteEvent("Hide n Seek1: Orlinn goes right", orlinn, 31, 22, false);
    event:AddEventLinkAtEnd("Hide n Seek1: Orlinn looks south");
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Hide n Seek1: Orlinn looks south", orlinn, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Hide n Seek1: Orlinn goes left", 800);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Hide n Seek1: Orlinn goes left", orlinn, 29, 22, false);
    event:AddEventLinkAtEnd("Hide n Seek1: Orlinn goes right", 8000); -- finish the event loop.
    EventManager:RegisterEvent(event);

    if (GlobalManager:DoesEventExist("layna_south_entrance", "quest1_orlinn_hide_n_seek1_done") == true) then
        -- Orlinn shouldn't be here, so we make him invisible
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetVisible(false);
        return;
    elseif (GlobalManager:DoesEventExist("layna_center", "quest1_orlinn_dialogue1_done") == true) then
        -- Start the hide and seek 1 position when it has to happen
        EventManager:StartEvent("Hide n Seek1: Orlinn goes right", 8000);

        -- Set up the dialogue.
        dialogue = vt_map.SpriteDialogue();
        text = vt_system.Translate("Yiek!!! Hey, you scared me.");
        dialogue:AddLineEmote(text, orlinn, "exclamation");
        text = vt_system.Translate("But You'll never find me hiding on top of the cliffs!");
        dialogue:AddLineEvent(text, orlinn, "", "Quest1: Start Orlinn Hide n Seek2");
        DialogueManager:AddDialogue(dialogue);
        orlinn:AddDialogueReference(dialogue);
        return;
    end

    -- Orlinn default behaviour
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);
end


-- Map Custom functions
map_functions = {

    Quest1_Orlinn_Start_Hide_N_Seek2 = function()
        orlinn:SetMoving(false); -- in case he's moving
        orlinn:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);
        orlinn:ClearDialogueReferences();
        EventManager:TerminateAllEvents(orlinn);

        -- Updates Orlinn's state
        GlobalManager:SetEventValue("layna_south_entrance", "quest1_orlinn_hide_n_seek1_done", 1);
    end,

    MakeInvisible = function(sprite)
        if (sprite ~= nil) then
            sprite:SetVisible(false);
            sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end
}
