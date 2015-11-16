-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_center_shop_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "data/story/common/locations/mountain_village.png"
map_subname = "Flora's Boutique"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

-- The main character handlers
local bronann = nil

-- NPCs
local flora = nil

-- the main map loading code
function Load(m)

    Map = m;
    EventManager = Map:GetEventSupervisor();

    Map:SetUnlimitedStamina(true);

    _CreateCharacters();
    _CreateNPCs();
    _CreateObjects();

    -- Set the camera focus on bronann
    Map:SetCamera(bronann);

    _CreateEvents();
    _CreateZones();

    -- The only entrance close door sound
    AudioManager:PlaySound("data/sounds/door_close.wav");
end

function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- default position and direction
    bronann = CreateSprite(Map, "Bronann", 32.0, 27.0, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.NORTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
end

function _CreateNPCs()
    local npc = CreateNPCSprite(Map, "Woman1", vt_system.Translate("Flora"), 39, 20, vt_map.MapMode.GROUND_OBJECT);
    npc:SetDirection(vt_map.MapMode.SOUTH);

    -- The npc is too far away from the Hero so we make an invisible doppelgänger
    flora = CreateNPCSprite(Map, "Woman1", vt_system.Translate("Flora"), 39, 22, vt_map.MapMode.GROUND_OBJECT);
    flora:SetVisible(false);
    flora:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    _UpdateFloraDialogue();
end

function _CreateObjects()
    local object = nil

    CreateObject(Map, "Flower Pot1", 41, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Flower Pot2", 35, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Table1", 27, 17, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Table1", 33, 17, vt_map.MapMode.GROUND_OBJECT);

    -- lights
    object = CreateObject(Map, "Right Window Light 2", 41, 10, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    object = CreateObject(Map, "Right Window Light 2", 41, 17, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil

    -- Triggered Events
    vt_map.MapTransitionEvent.Create("to village", "data/story/layna_village/layna_village_center_map.lua",
                                     "data/story/layna_village/layna_village_center_script.lua", "from_shop");

    event = vt_map.ShopEvent.Create("layna: open shop", "Flora's Shop");
    event:AddItem(1, 0); -- infinite minor potions
    event:AddItem(1001, 0); -- infinite minor elixirs
    event:AddItem(30003, 1); -- tunic for Bronann
    event:AddItem(30004, 1); -- leather cloak for Kalya
    event:AddItem(40001, 3); -- prismatic rings for both
    event:SetPriceLevels(vt_shop.ShopMode.SHOP_PRICE_VERY_GOOD, -- Flora is a good friend
                         vt_shop.ShopMode.SHOP_PRICE_STANDARD);
    event:AddScript("data/story/layna_village/tutorial_shop_dialogs.lua");

    -- Quest events
    vt_map.ScriptedEvent.Create("SetQuest1DialogueDone", "Quest1FloraDialogueDone", "");
    vt_map.ScriptedEvent.Create("Quest2: Talked to Flora", "Quest2FloraDialogueDone", "");
end

-- zones
local shop_exit_zone = nil

function _CreateZones()
    -- N.B.: left, right, top, bottom
    shop_exit_zone = vt_map.CameraZone.Create(30, 34, 28, 29);
end

function _CheckZones()
    if (shop_exit_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to village");
        -- The only entrance close door sound
        AudioManager:PlaySound("data/sounds/door_open2.wav");
    end
end

-- Custom inner map functions
function _UpdateFloraDialogue()
    local dialogue = nil
    local text = nil

    flora:ClearDialogueReferences();

    if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == true) then
        -- nothing special
    elseif (GlobalManager:DoesEventExist("story", "Quest2_started") == true) then
        -- The dialogue before the forest event
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Hi Bronann! What can I do for you?");
        dialogue:AddLine(text, flora);
        text = vt_system.Translate("Hi Flora! Err, could you lend me one of your training swords? I'd like to practice a bit.");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Ah! Sure, as soon as your father will stop lending his sword to you to practice with him. Are you sure everything is alright?");
        dialogue:AddLine(text, flora);
        text = vt_system.Translate("Err, nevermind.");
        dialogue:AddLineEventEmote(text, bronann, "", "Quest2: Talked to Flora", "sweat drop");
        flora:AddDialogueReference(dialogue);
        return;
    elseif (GlobalManager:DoesEventExist("layna_center_shop", "quest1_flora_dialogue_done") == true) then
        -- Just repeat the last dialogue sentence, when the dialogue is already done.
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_flora_about_georges");
        text = vt_system.Translate("Just find our *poet* and he should give you some barley meal, ok?");
        dialogue:AddLine(text, flora);
        text = vt_system.Translate("He's probably 'musing' around the cliffs in the village center right now.");
        dialogue:AddLine(text, flora);
        flora:AddDialogueReference(dialogue);
        return;
    elseif (GlobalManager:DoesEventExist("bronanns_home", "quest1_mother_start_dialogue_done") == true) then
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Hi Bronann! What can I do for you?");
        dialogue:AddLine(text, flora);
        text = vt_system.Translate("Hi Flora! Do you have some barley meal left?");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Oh sorry, our 'great' poet came earlier and took all the rest of it.");
        dialogue:AddLine(text, flora);
        text = vt_system.Translate("Times are becoming harder now. We've got less food than before.");
        dialogue:AddLine(text, flora);
        text = vt_system.Translate("This is the first time that I've seen you wear such a worrisome expression.");
        dialogue:AddLineEmote(text, bronann, "interrogation");
        text = vt_system.Translate("Nevermind. Don't worry about me. Just find him and he should give you some, ok?");
        dialogue:AddLine(text, flora);
        text = vt_system.Translate("He's probably 'musing' around the cliffs in the village center right now.");
        -- Set the quest dialogue as seen by the player.
        dialogue:AddLineEvent(text, flora, "", "SetQuest1DialogueDone");
        flora:AddDialogueReference(dialogue);
        return;
    end
    --default behaviour
    dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_flora_default");
    text = vt_system.Translate("Hi Bronann! What can I do for you?");
    dialogue:AddLineEvent(text, flora, "", "layna: open shop");
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
