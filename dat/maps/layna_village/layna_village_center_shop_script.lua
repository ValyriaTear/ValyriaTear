-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_center_shop_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = "Flora's Boutique"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- The main character handlers
local bronann = {};

-- NPCs
local flora = {};

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

    Map.unlimited_stamina = true;

    _CreateCharacters();
    _CreateNPCs();
    _CreateObjects();

    -- Set the camera focus on bronann
    Map:SetCamera(bronann);

    _CreateEvents();
    _CreateZones();

    -- The only entrance close door sound
    AudioManager:PlaySound("snd/door_close.wav");
end

function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- default position and direction
    bronann = CreateSprite(Map, "Bronann", 32.0, 27.0);
    bronann:SetDirection(hoa_map.MapMode.NORTH);
    bronann:SetMovementSpeed(hoa_map.MapMode.NORMAL_SPEED);

    Map:AddGroundObject(bronann);
end

function _CreateNPCs()
    npc = CreateNPCSprite(Map, "Woman1", "Flora", 39, 20);
    npc:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(npc);

    -- The npc is too far away from the Hero so we make an invisible doppelgänger
    flora = CreateNPCSprite(Map, "Woman1", "Flora", 39, 22);
    Map:AddGroundObject(flora);
    flora:SetVisible(false);
    flora:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    _UpdateFloraDialogue();
end

function _CreateObjects()
    local object = {}

    object = CreateObject(Map, "Flower Pot1", 41, 20);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Flower Pot2", 35, 20);
    if (object ~= nil) then Map:AddGroundObject(object) end;


    object = CreateObject(Map, "Table1", 27, 17);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Table1", 33, 17);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- lights
    object = CreateObject(Map, "Right Window Light 2", 41, 10);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Right Window Light 2", 41, 17);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    if (object ~= nil) then Map:AddGroundObject(object) end;
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};

    -- Triggered Events
    event = hoa_map.MapTransitionEvent("to village", "dat/maps/layna_village/layna_village_center_map.lua",
                                       "dat/maps/layna_village/layna_village_center_script.lua", "from_shop");
    EventManager:RegisterEvent(event);

    event = hoa_map.ShopEvent("layna: open shop");
    event:AddObject(1, 10); -- minor potion
    event:AddObject(1001, 10); -- minor elixir
    event:AddObject(30003, 1); -- tunic for Bronann
    event:AddObject(30004, 1); -- leather cloak for Kalya
    event:AddObject(40001, 3); -- leather cloak for Kalya
    event:SetPriceLevels(hoa_shop.ShopMode.SHOP_PRICE_VERY_GOOD, -- Flora is a good friend
                hoa_shop.ShopMode.SHOP_PRICE_STANDARD);

    EventManager:RegisterEvent(event);

    -- Quest events
    event = hoa_map.ScriptedEvent("SetQuest1DialogueDone", "Quest1FloraDialogueDone", "");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("Quest2: Talked to Flora", "Quest2FloraDialogueDone", "");
    EventManager:RegisterEvent(event);
end

-- zones
local shop_exit_zone = {};

function _CreateZones()
    -- N.B.: left, right, top, bottom
    shop_exit_zone = hoa_map.CameraZone(30, 34, 28, 29);
    Map:AddZone(shop_exit_zone);
end

function _CheckZones()
    if (shop_exit_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to village");
        -- The only entrance close door sound
        AudioManager:PlaySound("snd/door_open2.wav");
    end
end

-- Custom inner map functions
function _UpdateFloraDialogue()
    local dialogue = {}
    local text = {}

    flora:ClearDialogueReferences();

    if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == true) then
        -- nothing special
    elseif (GlobalManager:DoesEventExist("story", "Quest2_started") == true) then
        -- The dialogue before the forest event
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("Hi Bronann! What can I do for you?");
        dialogue:AddLine(text, flora);
        text = hoa_system.Translate("Hi Flora! Err, could you lend me one of your training swords? I'd like to practise a bit.");
        dialogue:AddLine(text, bronann);
        text = hoa_system.Translate("Ah! Sure, as soon as your father will stop lending his sword to you to practise with him. Are you sure everything is alright?");
        dialogue:AddLine(text, flora);
        text = hoa_system.Translate("Err, nevermind...");
        dialogue:AddLineEventEmote(text, bronann, "", "Quest2: Talked to Flora", "sweat drop");
        DialogueManager:AddDialogue(dialogue);
        flora:AddDialogueReference(dialogue);
        return;
    elseif (GlobalManager:DoesEventExist("layna_center_shop", "quest1_flora_dialogue_done") == true) then
        -- Just repeat the last dialogue sentence, when the dialogue is already done.
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("Just find our *poet* and he should give you some barley meal, ok?");
        dialogue:AddLine(text, flora);
        text = hoa_system.Translate("He's probably 'musing' around the cliffs in the village center right now.");
        dialogue:AddLine(text, flora);
        DialogueManager:AddDialogue(dialogue);
        flora:AddDialogueReference(dialogue);
        return;
    elseif (GlobalManager:DoesEventExist("bronanns_home", "quest1_mother_start_dialogue_done") == true) then
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("Hi Bronnan! What can I do for you?");
        dialogue:AddLine(text, flora);
        text = hoa_system.Translate("Hi Flora! Do you have some barley meal left?");
        dialogue:AddLine(text, bronann);
        text = hoa_system.Translate("Oh sorry, our 'great' poet came earlier and took all the rest of it.");
        dialogue:AddLine(text, flora);
        text = hoa_system.Translate("Times are becoming harder now. We've got less food than before...");
        dialogue:AddLine(text, flora);
        text = hoa_system.Translate("? ... This is the first time that I've see you wear such a worrisome expression.");
        dialogue:AddLine(text, bronann);
        text = hoa_system.Translate("Nevermind... Don't worry about me. Just find him and he should give you some, ok?");
        dialogue:AddLine(text, flora);
        text = hoa_system.Translate("He's probably 'musing' around the cliffs in the village center right now.");
        -- Set the quest dialogue as seen by the player.
        dialogue:AddLineEvent(text, flora, "", "SetQuest1DialogueDone");
        DialogueManager:AddDialogue(dialogue);
        flora:AddDialogueReference(dialogue);
        return;
    end
    --default behaviour
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Hi Bronnan! What can I do for you?");
    dialogue:AddLineEvent(text, flora, "", "layna: open shop");
    DialogueManager:AddDialogue(dialogue);
    flora:AddDialogueReference(dialogue);
end

-- Map Custom functions
map_functions = {

    Quest1FloraDialogueDone = function()
        GlobalManager:SetEventValue("layna_center_shop", "quest1_flora_dialogue_done", 1);
        _UpdateFloraDialogue();
    end,

    Quest2FloraDialogueDone = function()
        GlobalManager:SetEventValue("story", "Quest2_flora_dialogue_done", 1);
    end
}
