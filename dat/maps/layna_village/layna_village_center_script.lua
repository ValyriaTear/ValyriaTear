-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_center_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = "Village center"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/Caketown_1-OGA-mat-pablo.ogg"

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
local georges = {};
local carson = {};
local herth = {};
local olivia = {}; -- Olivia npc, guarding the forest entrance

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

    _TriggerPotentialDialogueAfterFadeIn();

    -- Add clouds overlay
    Effects:EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);

    -- Set the world map current position
    GlobalManager:SetCurrentLocationId("layna village")

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

    kalya = CreateSprite(Map, "Kalya", 42, 18);
    Map:AddGroundObject(kalya);
    event = hoa_map.RandomMoveSpriteEvent("Kalya random move", kalya, 1000, 2000);
    event:AddEventLinkAtEnd("Kalya random move", 2000); -- Loop on itself
    EventManager:RegisterEvent(event);
    EventManager:StartEvent("Kalya random move");
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Please, leave me alone, Bronann...");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    kalya:AddDialogueReference(dialogue);

    orlinn = CreateSprite(Map, "Orlinn", 40, 18);
    Map:AddGroundObject(orlinn);
    -- Setup Orlinn's state and dialogue depending on the story current context
    _UpdateOrlinnAndKalyaState();

    carson = CreateSprite(Map, "Carson", 0, 0);
    -- Default behaviour - not present on map.
    carson:SetVisible(false);
    carson:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(carson);

    herth = CreateSprite(Map, "Herth", 0, 0);
    -- Default behaviour - not present on map.
    herth:SetVisible(false);
    herth:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(herth);

    npc = CreateNPCSprite(Map, "Old Woman1", "Brymir", 72, 64);
    Map:AddGroundObject(npc);
    npc:SetDirection(hoa_map.MapMode.WEST);
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Ahh sure! (gossip, gossip)");
    dialogue:AddLine(text, npc);
    DialogueManager:AddDialogue(dialogue);
    npc:AddDialogueReference(dialogue);

    npc = CreateNPCSprite(Map, "Woman1", "Martha", 70, 64);
    Map:AddGroundObject(npc);
    npc:SetDirection(hoa_map.MapMode.EAST);
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Did you hear that? (gossip, gossip)");
    dialogue:AddLine(text, npc);
    DialogueManager:AddDialogue(dialogue);
    npc:AddDialogueReference(dialogue);

    npc = CreateNPCSprite(Map, "Woman2", "Sophia", 22, 38);
    Map:AddGroundObject(npc);
    npc:SetDirection(hoa_map.MapMode.SOUTH);
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("You're too young to trade stuff with me!");
    if (GlobalManager:DoesEventExist("layna_south_entrance", "quest1_orlinn_hide_n_seek1_done") == true) then
        if (GlobalManager:DoesEventExist("layna_riverbank", "quest1_orlinn_hide_n_seek2_done") == false) then
            text = hoa_system.Translate("If you're running after Orlinn, I just saw him disappear near your house.");
        end
    end
    dialogue:AddLine(text, npc);
    DialogueManager:AddDialogue(dialogue);
    npc:AddDialogueReference(dialogue);
    -- Add her cat, Nekko
    object = CreateObject(Map, "Cat1", 24, 37.6);
    if (object ~= nil) then
        Map:AddGroundObject(object)

        event = hoa_map.SoundEvent("Nekko says Meoww!", "snd/meow.wav");
        EventManager:RegisterEvent(event);

        object:SetEventWhenTalking("Nekko says Meoww!");
    end;

    georges = CreateNPCSprite(Map, "Man1", "Georges", 32, 76);
    Map:AddGroundObject(georges);
    georges:SetDirection(hoa_map.MapMode.WEST);
    _UpdateGeorgesDialogue(georges);

    -- Olivia, guardian of the forest access
    olivia = CreateNPCSprite(Map, "Girl1", "Olivia", 115, 34);
    olivia:SetDirection(hoa_map.MapMode.SOUTH);
    Map:AddGroundObject(olivia);
    _UpdateOliviaDialogue();

    -- Needed look at events
    event = hoa_map.LookAtSpriteEvent("Bronann looks at Olivia", bronann, olivia);
    EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Bronann looks south", bronann, hoa_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
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

    -- Lights
    -- big round windows light flares
    Map:AddLight("img/misc/lights/sun_flare_light_main.lua",
            "img/misc/lights/sun_flare_light_secondary.lua",
        74.0, 55.0,
        hoa_video.Color(1.0, 1.0, 1.0, 0.6),
        hoa_video.Color(1.0, 1.0, 0.85, 0.3),
        hoa_map.MapMode.CONTEXT_01);

    Map:AddLight("img/misc/lights/sun_flare_light_main.lua",
            "img/misc/lights/sun_flare_light_secondary.lua",
        86.0, 67.0,
        hoa_video.Color(1.0, 1.0, 1.0, 0.6),
        hoa_video.Color(1.0, 1.0, 0.85, 0.3),
        hoa_map.MapMode.CONTEXT_01);
    Map:AddLight("img/misc/lights/sun_flare_light_main.lua",
            "img/misc/lights/sun_flare_light_secondary.lua",
        22.0, 32.0,
        hoa_video.Color(1.0, 1.0, 1.0, 0.6),
        hoa_video.Color(1.0, 1.0, 0.85, 0.3),
        hoa_map.MapMode.CONTEXT_01);

    -- Small door lights
    Map:AddLight("img/misc/lights/sun_flare_light_small_main.lua",
            "img/misc/lights/sun_flare_light_small_secondary.lua",
        12.0, 57.5,
        hoa_video.Color(1.0, 1.0, 1.0, 0.6),
        hoa_video.Color(1.0, 1.0, 0.85, 0.3),
      hoa_map.MapMode.CONTEXT_01);
    Map:AddLight("img/misc/lights/sun_flare_light_small_main.lua",
            "img/misc/lights/sun_flare_light_small_secondary.lua",
        94.0, 67.5,
        hoa_video.Color(1.0, 1.0, 1.0, 0.6),
        hoa_video.Color(1.0, 1.0, 0.85, 0.3),
        hoa_map.MapMode.CONTEXT_01);

    -- Treasure vase
    local nekko_vase = CreateTreasure(Map, "layna_center_nekko_vase", "Vase1", 27, 37);
    if (nekko_vase ~= nil) then
        nekko_vase:AddObject(11, 1);
        Map:AddGroundObject(nekko_vase);
    end

    -- Quest 2: Forest event
    -- The wooden sword sprite
    wooden_sword = CreateObject(Map, "Wooden Sword1", 1, 1);
    Map:AddGroundObject(wooden_sword);
    wooden_sword:SetVisible(false);
    wooden_sword:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local text = {};
    local dialogue = {};

    -- Triggered Events
    event = hoa_map.MapTransitionEvent("to Bronann's home", "dat/maps/layna_village/layna_village_bronanns_home_map.lua",
                                       "dat/maps/layna_village/layna_village_bronanns_home_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to Riverbank", "dat/maps/layna_village/layna_village_riverbank_map.lua",
                                       "dat/maps/layna_village/layna_village_riverbank_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to Village south entrance", "dat/maps/layna_village/layna_village_south_entrance_map.lua",
                                       "dat/maps/layna_village/layna_village_south_entrance_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to Kalya house path", "dat/maps/layna_village/layna_village_kalya_house_path_map.lua",
                                       "dat/maps/layna_village/layna_village_kalya_house_path_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to Flora's Shop", "dat/maps/layna_village/layna_village_center_shop_map.lua",
                                       "dat/maps/layna_village/layna_village_center_shop_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to sophia house", "dat/maps/layna_village/layna_village_center_sophia_house_map.lua",
                                       "dat/maps/layna_village/layna_village_center_sophia_house_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to secret cliff", "dat/maps/layna_village/layna_village_riverbank_map.lua",
                                       "dat/maps/layna_village/layna_village_riverbank_script.lua", "from_secret_path");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to layna forest entrance", "dat/maps/layna_forest/layna_forest_entrance_map.lua",
                                       "dat/maps/layna_forest/layna_forest_entrance_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    -- Generic events
    event = hoa_map.ScriptedEvent("Map:PushState(SCENE)", "Map_SceneState", "");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("Map:PopState()", "Map_PopState", "");
    EventManager:RegisterEvent(event);

    -- Quest events
    -- Bronann wonders where he can find barley meal
    event = hoa_map.ScriptedEvent("Quest1: Bronann wonders where he can find some barley meal", "Map_SceneState", "");
    event:AddEventLinkAtEnd("Quest1: Bronann wants to see Flora for the barley meal", 1000);
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Hmm, I'll go to Flora's shop. I hope she'll help me...");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest1: Bronann wants to see Flora for the barley meal", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Map:PopState()");
    EventManager:RegisterEvent(event);

    -- Georges
    event = hoa_map.ScriptedEvent("Quest1: GeorgesDialogueDone", "Quest1GeorgesDialogueDone", "");
    EventManager:RegisterEvent(event);

    -- Orlinn
    event = hoa_map.ScriptedEvent("Quest1: Make Orlinn run and hide", "Quest1OrlinnRunAndHide", "");
    event:AddEventLinkAtEnd("Quest1: Make Orlinn run");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Quest1: Make Orlinn run", orlinn, 30, 79, true);
    event:AddEventLinkAtEnd("Quest1: Make Orlinn disappear");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedSpriteEvent("Quest1: Make Orlinn disappear", orlinn, "MakeInvisible", "");
    EventManager:RegisterEvent(event);

    -- Georges event
    event = hoa_map.ScriptedEvent("Quest1: Georges tells whom the barley meal was for", "Quest1GeorgesTellsBronannAboutLilly", "");
    EventManager:RegisterEvent(event);

    -- Quest 2: Bronann wants to go to Flora's and buy a sword to go in the forest
    event = hoa_map.ScriptedEvent("Quest2: Bronann wants to buy a sword from Flora", "Map_SceneState", "");
    event:AddEventLinkAtEnd("Quest2: Bronann wants to see Flora for equipment", 1000);
    event:AddEventLinkAtEnd("Bronann is sad");
    EventManager:RegisterEvent(event);

    event = hoa_map.AnimateSpriteEvent("Bronann is sad", bronann, "hero_stance", 2000);
    EventManager:RegisterEvent(event);
    event = hoa_map.AnimateSpriteEvent("Bronann is frightnened", bronann, "frightened", 0);
    EventManager:RegisterEvent(event);
    event = hoa_map.AnimateSpriteEvent("Bronann searches", bronann, "searching", 0);
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Why doesn't anyone want to tell me what's going on!!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = hoa_system.Translate("Still, I have go there and figure out what they're trying to hide from me.");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest2: Bronann wants to see Flora for equipment", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Map:PopState()");
    EventManager:RegisterEvent(event);

    -- Quest 2: Bronann doesn't want to see his parents for the moment
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("No, I won't go there. I just can't talk to them at the moment...");
    dialogue:AddLine(text, bronann);
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest2: Bronann doesn't want to see his parents", dialogue);
    event:SetStopCameraMovement(true);
    EventManager:RegisterEvent(event);

    -- Quest 2: The forest event
    event = hoa_map.ScriptedEvent("Quest2: Forest event", "Prepare_forest_event", "");
    event:AddEventLinkAtEnd("Quest2: Forest event - light", 1200);
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("Quest2: Forest event - light", "BrightLightStart", "BrightLightUpdate");
    event:AddEventLinkAtEnd("Quest2: Bronann wonders what was that", 500);
    event:AddEventLinkAtEnd("Bronann searches");
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Huh? What was that light?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    text = hoa_system.Translate("... Bronann! Wait!");
    dialogue:AddLine(text, carson);
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest2: Bronann wonders what was that", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest2: Carson moves to Bronann");
    event:AddEventLinkAtEnd("Quest2: Bronann looks at his father");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Quest2: Carson moves to Bronann", carson, 89.0, 74.0, false);
    event:AddEventLinkAtEnd("Quest2: Carson starts to talk to Bronann");
    EventManager:RegisterEvent(event);

    event = hoa_map.ChangeDirectionSpriteEvent("Quest2: Bronann looks at his father", bronann, hoa_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Quest2: Herth looks at Carson", herth, hoa_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Quest2: Herth looks at Kalya", herth, hoa_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Quest2: Carson looks at Herth", carson, hoa_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Quest2: Carson looks at Kalya", carson, hoa_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Quest2: Carson looks at Bronann", carson, hoa_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Quest2: Kalya looks at Carson", kalya, hoa_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Quest2: Kalya looks at Bronann", kalya, hoa_map.MapMode.EAST);
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("I suppose you just saw that light, right?");
    dialogue:AddLine(text, carson);
    text = hoa_system.Translate("... Hmmm...");
    dialogue:AddLine(text, bronann);
    text = hoa_system.Translate("Bronann, there is something that I have to tell you. We've been fearing for this moment. I mean your mother and I...");
    dialogue:AddLine(text, carson);
    text = hoa_system.Translate("They're coming!");
    dialogue:AddLineEvent(text, herth, "", "Quest2: Carson looks at Herth");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest2: Carson starts to talk to Bronann", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest2: Herth moves to Carson");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Quest2: Herth moves to Carson", herth, 90.0, 76.0, false);
    event:AddEventLinkAtEnd("Quest2: Herth looks at Carson");
    event:AddEventLinkAtEnd("Quest2: Second part of talk");
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Carson, they've passed the river. They shall be here in no time.");
    dialogue:AddLine(text, herth);
    text = hoa_system.Translate("...");
    dialogue:AddLineEmote(text, carson, "sweat drop");
    text = hoa_system.Translate("Huh? Hey, what's happening here?!");
    dialogue:AddLineEventEmote(text, bronann, "", "Quest2: Carson looks at Bronann", "exclamation");
    text = hoa_system.Translate("Bronann, I ...");
    dialogue:AddLine(text, carson);
    text = hoa_system.Translate("Father!");
    dialogue:AddLineEvent(text, kalya, "", "Quest2: Herth looks at Kalya");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest2: Second part of talk", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest2: Kalya runs to her father");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Quest2: Kalya runs to her father", kalya, 88.0, 76.0, true);
    event:AddEventLinkAtEnd("Quest2: Third part of talk");
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Father! Orlinn has disappeared. I saw him taking the forest pathway!");
    dialogue:AddLineEvent(text, kalya, "", "Quest2: Carson looks at Herth");
    text = hoa_system.Translate("Kalya! You were supposed to keep an eye on him!");
    dialogue:AddLineEmote(text, herth, "exclamation");
    text = hoa_system.Translate("I did, but he just slipped through my fingers at the very second that strange light appeared.");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = hoa_system.Translate("Kalya, the army is coming. I'll deal with them, you, go and find Orlinn as fast as possible.");
    dialogue:AddLineEmote(text, herth, "thinking dots");
    text = hoa_system.Translate("But you might get hurt!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = hoa_system.Translate("No, don't worry. We'll simply talk to them and they'll move on. You know what you have to do, right?");
    dialogue:AddLine(text, herth);
    text = hoa_system.Translate("Herth, we both know it'll likely...");
    dialogue:AddLine(text, kalya);
    text = hoa_system.Translate("Do as I say and it'll be alright.");
    dialogue:AddLine(text, herth);
    text = hoa_system.Translate("But...");
    dialogue:AddLineEvent(text, kalya, "", "Quest2: Carson looks at Bronann");
    text = hoa_system.Translate("Bronann, you should go with her.");
    dialogue:AddLineEventEmote(text, carson, "", "Quest2: Kalya looks at Carson", "thinking dots");
    text = hoa_system.Translate("What?!?");
    dialogue:AddLineEventEmote(text, kalya, "", "Quest2: Kalya looks at Herth", "exclamation");
    text = hoa_system.Translate("Huh?");
    dialogue:AddLineEvent(text, bronann, "","Bronann is frightnened");
    text = hoa_system.Translate("Carson is right, Kalya. Bronann shall go with you.");
    dialogue:AddLineEvent(text, herth, "", "Quest2: Kalya looks at Bronann");
    text = hoa_system.Translate("But he would just be a burden!");
    dialogue:AddLineEvent(text, kalya, "", "Bronann is sad");
    text = hoa_system.Translate("Huh? Hey! But...");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = hoa_system.Translate("He won't slow you down, believe me. Right, Bronann?");
    dialogue:AddLineEvent(text, carson, "", "Quest2: Carson looks at Bronann");
    text = hoa_system.Translate("But father!");
    dialogue:AddLineEvent(text, kalya, "", "Quest2: Kalya looks at Herth");
    text = hoa_system.Translate("Carson is right, Kalya. Bronann shall go with you. It's... it's an order.");
    dialogue:AddLineEmote(text, herth, "thinking dots");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest2: Third part of talk", dialogue);
    event:AddEventLinkAtEnd("Quest2: Kalya goes back and forth");
    EventManager:RegisterEvent(event);

    -- Small event chain making kalya go back and forth.
    event = hoa_map.PathMoveSpriteEvent("Quest2: Kalya goes back and forth", kalya, 85.0, 76.0, false);
    event:AddEventLinkAtEnd("Quest2: Carson looks at Kalya");
    event:AddEventLinkAtEnd("Quest2: Kalya goes back and forth 2");
    EventManager:RegisterEvent(event);
    event = hoa_map.PathMoveSpriteEvent("Quest2: Kalya goes back and forth 2", kalya, 88.0, 76.0, false);
    event:AddEventLinkAtEnd("Quest2: Kalya goes back and forth 3");
    EventManager:RegisterEvent(event);
    event = hoa_map.PathMoveSpriteEvent("Quest2: Kalya goes back and forth 3", kalya, 85.0, 76.0, false);
    event:AddEventLinkAtEnd("Quest2: Kalya goes back and forth 4");
    EventManager:RegisterEvent(event);
    event = hoa_map.PathMoveSpriteEvent("Quest2: Kalya goes back and forth 4", kalya, 88.0, 76.0, false);
    event:AddEventLinkAtEnd("Quest2: Fourth part of talk");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("Quest2: Show the wooden sword item in front of carson", "Show_wooden_sword", "");
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Gahh... ok.");
    dialogue:AddLineEvent(text, kalya, "", "Quest2: Carson looks at Bronann");
    text = hoa_system.Translate("Bronann, take this sword. You'll probably need it to make your way through there.");
    dialogue:AddLineEvent(text, carson, "", "Quest2: Show the wooden sword item in front of carson");
    text = hoa_system.Translate("What? But one minute ago you said...");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = hoa_system.Translate("I know, but everything has changed. I'll explain it to you once it is all finished. Now go, my son.");
    dialogue:AddLine(text, carson);
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest2: Fourth part of talk", dialogue);
    event:AddEventLinkAtEnd("Quest2: Bronann goes and take the sword");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Quest2: Bronann goes and take the sword", bronann, 92.0, 73.0, false);
    event:AddEventLinkAtEnd("Quest2: Hide the wooden sword item");
    event:AddEventLinkAtEnd("Quest2: Add the wooden sword in inventory");
    EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedEvent("Quest2: Hide the wooden sword item", "Hide_wooden_sword", "");
    EventManager:RegisterEvent(event);
    event = hoa_map.TreasureEvent("Quest2: Add the wooden sword in inventory");
    event:AddObject(10001, 1); -- The wooden sword item
    event:AddEventLinkAtEnd("Quest2: Fifth part of talk");
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Thanks dad, we'll find him in no time.");
    dialogue:AddLine(text, bronann);
    text = hoa_system.Translate("We shall go now... Good luck, both of you.");
    dialogue:AddLineEvent(text, herth, "", "Quest2: Herth looks at Kalya");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest2: Fifth part of talk", dialogue);
    event:AddEventLinkAtEnd("Quest2: Herth leaves to south");
    event:AddEventLinkAtEnd("Quest2: Carson starts to leave to south");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Quest2: Herth leaves to south", herth, 92.0, 78.0, false);
    event:AddEventLinkAtEnd("Quest2: Herth disappears");
    EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedSpriteEvent("Quest2: Herth disappears", herth, "MakeInvisible", "");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Quest2: Carson starts to leave to south", carson, 90.0, 76.0, false);
    event:AddEventLinkAtEnd("Quest2: Carson talks to Bronann once last time");
    event:AddEventLinkAtEnd("Quest2: Carson looks at Bronnan from south");
    EventManager:RegisterEvent(event);

    event = hoa_map.ChangeDirectionSpriteEvent("Quest2: Carson looks at Bronnan from south", carson, hoa_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Good luck, son.");
    dialogue:AddLine(text, carson);
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest2: Carson talks to Bronann once last time", dialogue);
    event:AddEventLinkAtEnd("Quest2: Carson leaves to south");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Quest2: Carson leaves to south", carson, 92.0, 78.0, false);
    event:AddEventLinkAtEnd("Quest2: Carson disappears");
    EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedSpriteEvent("Quest2: Carson disappears", carson, "MakeInvisible", "");
    event:AddEventLinkAtEnd("Quest2: End part of talk", 1000);
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Ok, we'll go together. But slow me down and I'll make you regret it...");
    dialogue:AddLine(text, kalya);
    text = hoa_system.Translate("Don't worry, we'll find him. Ok?");
    dialogue:AddLine(text, bronann);
    text = hoa_system.Translate("Ok ...");
    dialogue:AddLine(text, kalya);
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest2: End part of talk", dialogue);
    event:AddEventLinkAtEnd("Quest2: Kalya joins Bronann's party");
    EventManager:RegisterEvent(event);

    -- TODO: Turns this into an actual join party event once functional
    event = hoa_map.PathMoveSpriteEvent("Quest2: Kalya joins Bronann's party", kalya, 92.0, 73.0, false);
    event:AddEventLinkAtEnd("Quest2: Kalya disappears");
    event:AddEventLinkAtEnd("Quest2: Add Kalya to the party");
    EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedSpriteEvent("Quest2: Kalya disappears", kalya, "MakeInvisible", "");
    EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedEvent("Quest2: Add Kalya to the party", "Add_kalya_to_party", "");
    event:AddEventLinkAtEnd("Quest2: Kalya joins speech");
    EventManager:RegisterEvent(event);
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Kalya joins your party!");
    dialogue:AddLine(text, kalya); --used for now to show her portrait
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest2: Kalya joins speech", dialogue);
    event:AddEventLinkAtEnd("Map:PopState()");
    EventManager:RegisterEvent(event);

    -- Kalya explains Bronnan about dungeons and equipment,
    event = hoa_map.ScriptedEvent("Quest2: Kalya's equipment and dungeons speech start", "Quest2_equip_speech_start", "");
    event:AddEventLinkAtEnd("Quest2: Kalya walks off from Bronann");
    event:AddEventLinkAtEnd("Quest2: Bronann goes in front of Kalya");
    EventManager:RegisterEvent(event);

    event = hoa_map.ChangeDirectionSpriteEvent("Kalya looks east", kalya, hoa_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Bronann looks west", bronann, hoa_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedSpriteEvent("Turn Kalya's collisions on", kalya, "Sprite_Collision_on", "");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Quest2: Kalya walks off from Bronann", kalya, 114, 37, false);
    event:AddEventLinkAtEnd("Kalya looks east");
    event:AddEventLinkAtEnd("Turn Kalya's collisions on");
    event:AddEventLinkAtEnd("Quest2: Kalya's speech about equipment and dungeons.");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Quest2: Bronann goes in front of Kalya", bronann, 116, 37, false);
    event:AddEventLinkAtEnd("Bronann looks west");
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("By the way, did you ever prepare yourself for something like this before?");
    dialogue:AddLine(text, kalya);
    text = hoa_system.Translate("Huh? Well, Orlinn doesn't disappear every day, you know?");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = hoa_system.Translate("I see... Then there are two very important things that you need to know:");
    dialogue:AddLine(text, kalya);
    text = hoa_system.Translate("First of all before going there, you might need a better equipment. Go and see Flora first, ok?");
    dialogue:AddLine(text, kalya);
    text = hoa_system.VTranslate("And tell me that you equipped your sword ... If you haven't, open your inventory by pressing the menu key (%s), and select 'Equip'. Then you'll be able to select your sword and add it as your main weapon, ok?", InputManager:GetMenuKeyName());
    dialogue:AddLine(text, kalya);
    text = hoa_system.Translate("You mean, like, I need to push a key to open my bag?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    text = hoa_system.Translate("Nevermind that, just do it.");
    dialogue:AddLine(text, kalya);
    text = hoa_system.Translate("The second thing is that the person in front of the battle line will lead the group in the forest. By that I mean that the person on the top of the battle formation will actually appear, while the others will stay hidden.");
    dialogue:AddLine(text, kalya);
    text = hoa_system.Translate("Hmm, right. I'm not sure that I fully get what you mean, but...");
    dialogue:AddLine(text, bronann);
    text = hoa_system.Translate("Rrrr... Bronann. Just listen.");
    dialogue:AddLine(text, kalya);
    text = hoa_system.Translate("That will be the case only in certain areas. Here in the village, you'll be the one leading, or at least you may believe that ...");
    dialogue:AddLine(text, kalya);
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Quest2: Kalya's speech about equipment and dungeons.", dialogue);
    event:AddEventLinkAtEnd("Quest2: Kalya re-joins Bronann after speech");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Quest2: Kalya re-joins Bronann after speech", kalya, 116, 37, false);
    event:AddEventLinkAtEnd("Quest2: Kalya disappears after speech");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedSpriteEvent("Quest2: Kalya disappears after speech", kalya, "MakeInvisible", "");
    event:AddEventLinkAtEnd("Map:PopState()");
    EventManager:RegisterEvent(event);
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
    if (bronanns_home_entrance_zone:IsCameraEntering() == true) then
        -- If Bronann has started the quest 2, he doesn't want to go back and see his parents.
        if (GlobalManager:DoesEventExist("story", "Quest2_started") == true
            and GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == false) then
            EventManager:StartEvent("Quest2: Bronann doesn't want to see his parents");
            return;
        end
        bronann:SetMoving(false);
        AudioManager:PlaySound("snd/door_open2.wav");
        EventManager:StartEvent("to Bronann's home");
    elseif (to_riverbank_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to Riverbank");
    elseif (to_village_entrance_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to Village south entrance");
    elseif (to_kalya_house_path_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to Kalya house path");
    elseif (secret_path_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to secret cliff");
    elseif (to_layna_forest_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == false) then
            EventManager:StartEvent("Bronann can't enter the forest so easily");
        elseif (GlobalManager:DoesEventExist("story", "Quest2_kalya_equip_n_dungeons_speech_done") == false) then
            EventManager:StartEvent("Quest2: Kalya's equipment and dungeons speech start");
        else
            EventManager:StartEvent("to layna forest entrance");
        end
    elseif (shop_entrance_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        AudioManager:PlaySound("snd/door_open2.wav");
        EventManager:StartEvent("to Flora's Shop");
    elseif (sophia_house_entrance_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to sophia house");
        AudioManager:PlaySound("snd/door_open2.wav");
    end
end

-- Inner custom functions
function _TriggerPotentialDialogueAfterFadeIn()
    -- Trigger the forest and Orlinn runaway event
    if (GlobalManager:DoesEventExist("story", "Quest2_flora_dialogue_done") == true
            and GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == false) then
        EventManager:StartEvent("Quest2: Forest event");
        GlobalManager:SetEventValue("story", "Quest2_forest_event_done", 1);
        return;
    end

    if (GlobalManager:DoesEventExist("story", "Quest2_started") == true) then
        if (GlobalManager:DoesEventExist("story", "Quest2_wants_to_buy_sword_dialogue") == false) then
            EventManager:StartEvent("Quest2: Bronann wants to buy a sword from Flora");
            GlobalManager:SetEventValue("story", "Quest2_wants_to_buy_sword_dialogue", 1);
            return;
        end
    elseif (GlobalManager:DoesEventExist("layna_center", "first_time_in_village_center") == false) then
        EventManager:StartEvent("Quest1: Bronann wonders where he can find some barley meal");
        GlobalManager:SetEventValue("layna_center", "first_time_in_village_center", 1);
        return;
    end
end

-- Updates Olivia dialogues according to the story events
function _UpdateOliviaDialogue()
    olivia:ClearDialogueReferences();

    -- Don't grant access to the forest so easily
    if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == false) then
        if (GlobalManager:DoesEventExist("story", "Quest2_wants_to_buy_sword_dialogue") == false
            and GlobalManager:DoesEventExist("story", "Quest2_started") == true) then
            dialogue = hoa_map.SpriteDialogue();
            text = hoa_system.Translate("Bronann! Sorry, you can't access the forest without permission. You don't even have a sword...");
            dialogue:AddLineEmote(text, olivia, "exclamation");
            text = hoa_system.Translate("Aww...");
            dialogue:AddLineEventEmote(text, bronann, "Bronann looks at Olivia", "", "sweat drop");
            text = hoa_system.Translate("(Hmm, maybe I should get a sword then.)");
            dialogue:AddLineEventEmote(text, bronann, "Bronann looks south", "", "thinking dots");
            DialogueManager:AddDialogue(dialogue);
            olivia:AddDialogueReference(dialogue);
        else
            dialogue = hoa_map.SpriteDialogue();
            text = hoa_system.Translate("Bronann! Sorry, you know you can't access the forest without permission.");
            dialogue:AddLineEmote(text, olivia, "exclamation");
            text = hoa_system.Translate("Aww...");
            dialogue:AddLineEventEmote(text, bronann, "Bronann looks at Olivia", "", "sweat drop");
            DialogueManager:AddDialogue(dialogue);
            olivia:AddDialogueReference(dialogue);
        end
    else
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("Good luck Bronann.");
        dialogue:AddLine(text, olivia);
        DialogueManager:AddDialogue(dialogue);
        olivia:AddDialogueReference(dialogue);
    end

    -- Special event triggered when Bronann hasn't go the right to enter the forest yet.
    -- Shouldn't trigger once access is granted.
    event = hoa_map.DialogueEvent("Bronann can't enter the forest so easily", dialogue);
    event:SetStopCameraMovement(true);
    EventManager:RegisterEvent(event);
end

-- Updates Georges dialogue depending on how far is the story going.
function _UpdateGeorgesDialogue()
    local text = {}
    local dialogue = {}

    georges:ClearDialogueReferences();

    if (GlobalManager:DoesEventExist("story", "quest1_barley_meal_done") == true) then
    -- default behaviour once the barley meal is given
    elseif (GlobalManager:DoesEventExist("layna_center", "quest1_pen_given_done") == true
        and GlobalManager:DoesEventExist("story", "quest1_barley_meal_done") == false) then
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("Actually, the barley meal was for Lilly.");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("!! What?");
        dialogue:AddLineEmote(text, bronann, "exclamation");
        text = hoa_system.Translate("Don't thank me for that. It's my pleasure.");
        dialogue:AddLine(text, georges);
        DialogueManager:AddDialogue(dialogue);
        georges:AddDialogueReference(dialogue);
    return;
        -- Quest 1 done as for Georges
    elseif (GlobalManager:DoesEventExist("layna_riverbank", "quest1_orlinn_hide_n_seek3_done") == true) then
        -- Give the pen to Georges
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("Here it is, Georges.");
        dialogue:AddLine(text, bronann);
        text = hoa_system.Translate("You're the nicest person I know, Bronnan. I well tell everyone how brave you...");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("(Sigh...) Georges!");
        dialogue:AddLine(text, bronann);
        text = hoa_system.Translate("Ok ok. Just having a bit of spirit, young man.");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("Actually, the barley meal was for Lilly.");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("!! What?");
        dialogue:AddLineEmote(text, bronann, "exclamation");
        text = hoa_system.Translate("Don't thank me for that, it's my pleasure.");
        dialogue:AddLineEvent(text, georges, "", "Quest1: Georges tells whom the barley meal was for");
        DialogueManager:AddDialogue(dialogue);
        georges:AddDialogueReference(dialogue);
        return;
    elseif (GlobalManager:DoesEventExist("layna_center", "quest1_georges_dialogue_done") == true) then
        -- Once talked to him after the shop conversation, just put the end of the dialogue
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("You see, I lost my beloved pen. Was it near a tree or next to the waving child of the mountain snow?");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("Shall you find it, I would be entrustfully obliged to you!");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("Inquire our comrades in the settlement, perhaps somebody has laid their eyes on it.");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("(Sigh...) Hmm, fine.");
        dialogue:AddLine(text, bronann);
        DialogueManager:AddDialogue(dialogue);
        georges:AddDialogueReference(dialogue);
        return;
    elseif (GlobalManager:DoesEventExist("layna_center_shop", "quest1_flora_dialogue_done") == true) then
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("Hi Georges. Erm, I'm coming from the shop and I ...");
        dialogue:AddLine(text, bronann);
        text = hoa_system.Translate("Can you hear this?");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("Erm, ... Well, I don't hear anything special...");
        dialogue:AddLineEmote(text, bronann, "interrogation");
        text = hoa_system.Translate("That's the point! Can't you hear the magnificient sound of nature, so invisible to our adapted ears?");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("Huh, please Georges. I do not want to run away like the last time...");
        dialogue:AddLine(text, bronann);
        text = hoa_system.Translate("... The incredible and amazing. I could even say, the stunning feel of it in the wind...");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("Georges, I simply wanted to ask you whether you had some barley meal left!");
        dialogue:AddLineEmote(text, bronann, "sweat drop");
        text = hoa_system.Translate("Ah, I see. Well unfortunately, I'm so sad about a recent loss that, I can't find the right words with which to tell you.");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("Huh?");
        dialogue:AddLineEmote(text, bronann, "interrogation");
        text = hoa_system.Translate("You see, I lost my beloved pen. Was it near a tree or next to the waving child of the mountain snow?");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("Shall you find it, I would be entrustfully obliged to you!");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("Inquire our comrades in the settlement, perhaps somebody has laid their eyes on it.");
        dialogue:AddLine(text, georges);
        text = hoa_system.Translate("(Sigh...) Hmm, fine.");
        dialogue:AddLineEvent(text, bronann, "", "Quest1: GeorgesDialogueDone");
        DialogueManager:AddDialogue(dialogue);
        georges:AddDialogueReference(dialogue);
        return;
    end

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Ah, the river is so beautiful at this time of the year. I feel like writing some poetry...");
    dialogue:AddLine(text, georges);
    DialogueManager:AddDialogue(dialogue);
    georges:AddDialogueReference(dialogue);
end

-- Updates Orlinn's dialogue and state depending on how far is the story going.
function _UpdateOrlinnAndKalyaState()
    local text = {};
    local dialogue = {};
    local event = {};

    orlinn:ClearDialogueReferences();
    if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == true) then
        -- At that moment, Orlinn has disappeared and Kalya is now in Bronann's party.
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
        kalya:ClearDialogueReferences();
        kalya:SetVisible(false);
        kalya:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
        EventManager:TerminateAllEvents(kalya);
        kalya:SetMoving(false);
        return;
    end
    if (GlobalManager:DoesEventExist("layna_riverbank", "quest1_orlinn_hide_n_seek3_done") == true) then
        -- Bronann got Georges' pen, update orlinn dialogue
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("I promise I won't bother you again ...");
        dialogue:AddLine(text, orlinn);
        text = hoa_system.Translate("Don't worry about that, Orlinn. Ok?");
        dialogue:AddLine(text, bronann);
        DialogueManager:AddDialogue(dialogue);
        orlinn:AddDialogueReference(dialogue);

        -- Update kalya's dialogue too
        kalya:ClearDialogueReferences();
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("...");
        dialogue:AddLine(text, kalya);
        DialogueManager:AddDialogue(dialogue);
        kalya:AddDialogueReference(dialogue);


    elseif (GlobalManager:DoesEventExist("layna_center", "quest1_orlinn_dialogue1_done") == true) then
        -- At that time, Orlinn isn't in the village center anymore.
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
        return;
    elseif (GlobalManager:DoesEventExist("layna_center", "quest1_georges_dialogue_done") == true) then
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("Hee hee hee!");
        dialogue:AddLine(text, orlinn);
        text = hoa_system.Translate("What are you laughing about, Orlinn?");
        dialogue:AddLineEmote(text, bronann, "interrogation");
        text = hoa_system.Translate("You'll never find it!");
        dialogue:AddLine(text, orlinn);
        text = hoa_system.Translate("Huh? Wait! Are you talking about Georges' lost pen?");
        dialogue:AddLineEmote(text, bronann, "exclamation");
        text = hoa_system.Translate("Hee hee hee! Yes!");
        dialogue:AddLine(text, orlinn);
        text = hoa_system.Translate("Please tell me! Have you found it?");
        dialogue:AddLine(text, bronann);
        text = hoa_system.Translate("Maybe yes, maybe no!");
        dialogue:AddLine(text, orlinn);
        text = hoa_system.Translate("Oh no, please Orlinn! I need it!");
        dialogue:AddLineEmote(text, bronann, "sweat drop");
        text = hoa_system.Translate("Sure, I'll help you. But only if you can catch me!");
        dialogue:AddLine(text, orlinn);
        text = hoa_system.Translate("Hee hee! (He'll never find me hiding behind the buildings!)");
        dialogue:AddLineEvent(text, orlinn, "", "Quest1: Make Orlinn run and hide");

        DialogueManager:AddDialogue(dialogue);
        orlinn:AddDialogueReference(dialogue);
    else
        dialogue = hoa_map.SpriteDialogue();
        text = hoa_system.Translate("Heya bro! Wanna play with me?");
        dialogue:AddLine(text, orlinn);
        DialogueManager:AddDialogue(dialogue);
        orlinn:AddDialogueReference(dialogue);
    end

    -- Default behaviour
    EventManager:TerminateAllEvents(orlinn);
    event = hoa_map.RandomMoveSpriteEvent("Orlinn random move", orlinn, 4000, 2000);
    event:AddEventLinkAtEnd("Orlinn random move", 3000); -- Loop on itself
    EventManager:RegisterEvent(event);
    EventManager:StartEvent("Orlinn random move");
end

-- Helps with the two step fade in the forest event
local bright_light_time = {}

-- Map Custom functions
map_functions = {

    Quest1GeorgesDialogueDone = function()
        GlobalManager:SetEventValue("layna_center", "quest1_georges_dialogue_done", 1);
        -- Makes Orlinn aware that Bronann has talked to Georges.
        _UpdateOrlinnAndKalyaState();
        _UpdateGeorgesDialogue();
        GlobalManager:AddQuestLog("find_pen");
    end,

    Quest1OrlinnRunAndHide = function()
        orlinn:SetMoving(false); -- in case he's moving
        orlinn:SetMovementSpeed(hoa_map.MapMode.VERY_FAST_SPEED);
        orlinn:SetCollisionMask(hoa_map.MapMode.WALL_COLLISION);
        orlinn:ClearDialogueReferences();
        EventManager:TerminateAllEvents(orlinn);

        -- Updates Orlinn's state
        GlobalManager:SetEventValue("layna_center", "quest1_orlinn_dialogue1_done", 1);
        GlobalManager:AddQuestLog("hide_n_seek_with_orlinn");
    end,

    Quest1GeorgesTellsBronannAboutLilly = function()
        GlobalManager:SetEventValue("layna_center", "quest1_pen_given_done", 1);

        -- Remove the pen key item from inventory
        local pen_item_id = 70001;
        if (GlobalManager:IsObjectInInventory(pen_item_id) == true) then
            GlobalManager:RemoveFromInventory(pen_item_id);
        end

        -- Updates Georges dialogue
        _UpdateGeorgesDialogue();
    end,

    Prepare_forest_event = function()
        -- Scene event
        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        carson:SetPosition(75.0, 72.0);
        carson:SetVisible(true);
        carson:SetCollisionMask(hoa_map.MapMode.ALL_COLLISION);

        herth:SetPosition(75.0, 77.0);
        herth:SetVisible(true);
        herth:SetCollisionMask(hoa_map.MapMode.ALL_COLLISION);

        EventManager:TerminateAllEvents(kalya);
        kalya:SetPosition(75.0, 68.0);
        kalya:SetMoving(false);
        kalya:ClearDialogueReferences();

        -- hide Orlinn has he's into the forest
        EventManager:TerminateAllEvents(orlinn);
        orlinn:SetMoving(false);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    end,

    BrightLightStart = function()
        bright_light_time = 0.0;
        Effects:ShakeScreen(0.6, 6000, hoa_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);
        AudioManager:PlaySound("snd/rumble.wav");
        AudioManager:FadeOutAllMusic(2000);
    end,

    BrightLightUpdate = function()
        bright_light_time = bright_light_time + 2.5 * SystemManager:GetUpdateTime();

        if (bright_light_time < 5000.0) then
            Map:GetEffectSupervisor():EnableLightingOverlay(hoa_video.Color(1.0, 1.0, 1.0, bright_light_time / 5000.0));
            return false;
        end

        if (bright_light_time < 10000) then
            Map:GetEffectSupervisor():EnableLightingOverlay(hoa_video.Color(1.0, 1.0, 1.0, ((10000.0 - bright_light_time) / 5000.0)));
            return false;
        end

        -- end of the two-step fade in and out
        return true;
    end,

    Show_wooden_sword = function()
        wooden_sword:SetPosition(carson:GetXPosition() + 1.5, carson:GetYPosition() - 2.0);
        wooden_sword:SetVisible(true);
        wooden_sword:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    end,

    Hide_wooden_sword = function()
        wooden_sword:SetVisible(false);
        wooden_sword:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    end,

    Add_kalya_to_party = function()
        if (GlobalManager:DoesEventExist("story", "kalya_has_joined") == false) then
            GlobalManager:AddCharacter(KALYA);
            GlobalManager:SetEventValue("story", "kalya_has_joined", 1);
            GlobalManager:AddQuestLog("bring_orlinn_back");
        end
        AudioManager:FadeInAllMusic(2000);

        -- Now, the event is done, update Olivia's dialogue as access to the forest is granted
        _UpdateOliviaDialogue();
    end,

    Quest2_equip_speech_start = function()
        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        bronann:SetMoving(false);
        bronann:SetDirection(hoa_map.MapMode.WEST);
        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        kalya:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
        kalya:SetDirection(hoa_map.MapMode.WEST);
        kalya:SetVisible(true);
        kalya:ClearDialogueReferences();

        -- Set the event as done to prevent it to trigger again
        GlobalManager:SetEventValue("story", "Quest2_kalya_equip_n_dungeons_speech_done", 1)
    end,

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
