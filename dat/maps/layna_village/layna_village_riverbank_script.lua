-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_riverbank_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = "Riverbank"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

local bronann = {};
local kalya = {};
local orlinn = {};
local orlinn_dialogue_npc = {};
local lilly = {};

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
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, -5.0, true);

    _HandleCredits();

    -- Once everything is created, we set up Orlinn and Lilly behaviour
    _SetOrlinnState();
    _SetLillyState();
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
    bronann = CreateSprite(Map, "Bronann", 97, 4);
    bronann:SetDirection(vt_map.MapMode.SOUTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- set up the position according to the previous map and location
    if (GlobalManager:GetPreviousLocation() == "from_village_south") then
        bronann:SetPosition(117, 18);
        bronann:SetDirection(vt_map.MapMode.WEST);
    end

    if (GlobalManager:GetPreviousLocation() == "from_secret_path") then
        bronann:SetPosition(65, 4);
        bronann:SetDirection(vt_map.MapMode.SOUTH);
    end

    if (GlobalManager:GetPreviousLocation() == "from_riverbank_house") then
        bronann:SetPosition(98, 48);
        bronann:SetDirection(vt_map.MapMode.SOUTH);
        AudioManager:PlaySound("snd/door_close.wav");
    end

    Map:AddGroundObject(bronann);
end

function _CreateNPCs()
    local text = {}
    local dialogue = {}
    local event = {}

    lilly = CreateNPCSprite(Map, "Woman3", vt_system.Translate("Lilly"), 67, 40);
    Map:AddGroundObject(lilly);

    object = CreateObject(Map, "Dog1", 66, 42);
    Map:AddGroundObject(object);

    event = vt_map.SoundEvent("Lilly's dog barks", "snd/dog_barking.wav");
    EventManager:RegisterEvent(event);
    object:SetEventWhenTalking("Lilly's dog barks");

    kalya = CreateSprite(Map, "Kalya", 2, 2);
    kalya:SetDirection(vt_map.MapMode.SOUTH);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);
    Map:AddGroundObject(kalya);

    orlinn = CreateSprite(Map, "Orlinn", 82, 5);
    orlinn:SetDirection(vt_map.MapMode.SOUTH);
    orlinn:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);
    Map:AddGroundObject(orlinn);

    -- Create an invisible doppelg\E4nger to permit triggering dialogues when the kid is on the cliff.
    orlinn_dialogue_npc = CreateSprite(Map, "Orlinn", 82, 8);
    orlinn_dialogue_npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn_dialogue_npc:SetVisible(false);
    Map:AddGroundObject(orlinn_dialogue_npc);
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
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(object);
    end

    -- Lights
    -- big round windows light flares
    Map:AddLight("img/misc/lights/sun_flare_light_main.lua",
            "img/misc/lights/sun_flare_light_secondary.lua",
        94.0, 40.0,
        vt_video.Color(1.0, 1.0, 1.0, 0.6),
        vt_video.Color(1.0, 1.0, 0.85, 0.3));
    Map:AddLight("img/misc/lights/sun_flare_light_main.lua",
            "img/misc/lights/sun_flare_light_secondary.lua",
        102.0, 40.0,
        vt_video.Color(1.0, 1.0, 1.0, 0.6),
        vt_video.Color(1.0, 1.0, 0.85, 0.3));
    -- Small door lights
    Map:AddLight("img/misc/lights/sun_flare_light_small_main.lua",
            "img/misc/lights/sun_flare_light_small_secondary.lua",
        98.0, 43.5,
        vt_video.Color(1.0, 1.0, 1.0, 0.6),
        vt_video.Color(1.0, 1.0, 0.85, 0.3));

    object = vt_map.SoundObject("snd/gentle_stream.ogg", 61.0, 27.0, 20.0);
    Map:AddAmbientSoundObject(object);

    object = vt_map.SoundObject("snd/gentle_stream.ogg", 61.0, 47.0, 20.0);
    Map:AddAmbientSoundObject(object);

    object = vt_map.SoundObject("snd/gentle_stream.ogg", 61.0, 67.0, 20.0);
    Map:AddAmbientSoundObject(object);

    object = vt_map.SoundObject("snd/gentle_stream.ogg", 81.0, 67.0, 20.0);
    Map:AddAmbientSoundObject(object);

    object = vt_map.SoundObject("snd/gentle_stream.ogg", 101.0, 67.0, 20.0);
    Map:AddAmbientSoundObject(object);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- Map change Events
    event = vt_map.MapTransitionEvent("to Village center", "dat/maps/layna_village/layna_village_center_map.lua",
                                       "dat/maps/layna_village/layna_village_center_script.lua", "from_riverbank");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to Village south entrance", "dat/maps/layna_village/layna_village_south_entrance_map.lua",
                                       "dat/maps/layna_village/layna_village_south_entrance_script.lua", "from_riverbank");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to Riverbank house", "dat/maps/layna_village/layna_village_riverbank_house_map.lua",
                                       "dat/maps/layna_village/layna_village_riverbank_house_script.lua", "from_riverbank");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to secret path entrance", "dat/maps/layna_village/layna_village_center_map.lua",
                                       "dat/maps/layna_village/layna_village_center_script.lua", "from_secret_path");
    EventManager:RegisterEvent(event);

    -- Quest events - Hide and seek 2
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Wow! You found me!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    text = vt_system.Translate("But I'm not done yet!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("You'll never find me hiding behind the...");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Hey, why would I tell you my hiding spots?!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    DialogueManager:AddDialogue(dialogue);

    event = vt_map.DialogueEvent("Quest1: Orlinn starts hide and seek3 speech", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest1: Hide and seek2: Make Orlinn run");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Quest1: Hide and seek2: Make Orlinn run", orlinn, 72, 2, true);
    event:AddEventLinkAtEnd("Quest1: Hide and seek2: Make Orlinn disappear");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Quest1: Hide and seek2: Make Orlinn disappear", orlinn, "MakeInvisible", "");
    event:AddEventLinkAtEnd("Quest1: Hide and seek2: Bronann end speech");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("That kid is pretty quick. This is going to take all day...");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    DialogueManager:AddDialogue(dialogue);

    event = vt_map.DialogueEvent("Quest1: Hide and seek2: Bronann end speech", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Map:PopState()");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Map:PopState()", "Map_PopState", "");
    EventManager:RegisterEvent(event);

    -- Final hide and seek (3)
    event = vt_map.PathMoveSpriteEvent("Quest1: Hide and Seek3: Orlinn goes top-right", orlinn, 70, 42, false);
    event:AddEventLinkAtEnd("Hide n Seek3: Orlinn looks south");
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Hide n Seek3: Orlinn looks south", orlinn, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Hide n Seek3: Orlinn looks north", 800);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Hide n Seek3: Orlinn looks north", orlinn, vt_map.MapMode.NORTH);
    event:AddEventLinkAtEnd("Hide n Seek3: Orlinn goes bottom-left", 800);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Hide n Seek3: Orlinn goes bottom-left", orlinn, 74, 43.8, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn goes top-right", 8000); -- finish the event loop.
    EventManager:RegisterEvent(event);

    -- Before Kalya's arrival
    event = vt_map.ScriptedEvent("Quest1: Hide and Seek3: Orlinn starts to go away", "Prepare_orlinn_kalya_scene", "");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn tries go to away");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Quest1: Hide and Seek3: Orlinn tries go to away", orlinn, 76, 38, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Bronann follows him");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Quest1: Hide and Seek3: Bronann follows him", bronann, 76, 40, true);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Bronann turns to Orlinn");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn turns to him");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Quest1: Hide and Seek3: Bronann turns to Orlinn", bronann, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Quest1: Hide and Seek3: Orlinn turns to him", orlinn, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Bronann and kalya first speech");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Lilly turns to them");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Quest1: Hide and Seek3: Lilly turns to them", lilly, vt_map.MapMode.EAST);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Please no! Wait!");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Orlinn! Stop this!");
    dialogue:AddLineEvent(text, kalya, "", "Quest1: Hide and Seek3: Kalya comes to Orlinn");
    DialogueManager:AddDialogue(dialogue);

    event = vt_map.DialogueEvent("Quest1: Hide and Seek3: Bronann and kalya first speech", dialogue);
    event:SetStopCameraMovement(true);
    EventManager:RegisterEvent(event);

    -- Kalya's arrival
    event = vt_map.PathMoveSpriteEvent("Quest1: Hide and Seek3: Kalya comes to Orlinn", kalya, 76, 36, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Kalya's speech");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn turns to Kalya");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Quest1: Hide and Seek3: Orlinn turns to Kalya", orlinn, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Orlinn! How many times have I told you not to bother other people?");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("But sis! I...");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    text = vt_system.Translate("I don't want to hear it! Now, come!");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Don't blame him, Kalya. Actually, it was...");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    text = vt_system.Translate("I don't think I was talking to you, now was I?");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Come Orlinn!");
    dialogue:AddLine(text, kalya);
    DialogueManager:AddDialogue(dialogue);

    event = vt_map.DialogueEvent("Quest1: Hide and Seek3: Kalya's speech", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Kalya is going away.");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn is going away.", 2000);
    EventManager:RegisterEvent(event);

    -- Kalya and Orlinn going away
    event = vt_map.PathMoveSpriteEvent("Quest1: Hide and Seek3: Kalya is going away.", kalya, 95, 2, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Kalya disappears.");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Quest1: Hide and Seek3: Kalya disappears.", kalya, "MakeInvisible", "");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Quest1: Hide and Seek3: Orlinn is going away.", orlinn, 76, 32, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn comes back to Bronann", 1000);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Quest1: Hide and Seek3: Orlinn comes back to Bronann", orlinn, 76, 38, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn tells Bronann where the pen was");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("I found that pen near the tree behind you. I just wanted to play...");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    text = vt_system.Translate("Don't worry about that...");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Take it... Thanks.");
    dialogue:AddLine(text, orlinn);
    DialogueManager:AddDialogue(dialogue);

    event = vt_map.DialogueEvent("Quest1: Hide and Seek3: Orlinn tells Bronann where the pen was", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn gives the pen to Bronann");
    EventManager:RegisterEvent(event);

    event = vt_map.TreasureEvent("Quest1: Hide and Seek3: Orlinn gives the pen to Bronann");
    event:AddObject(70001, 1); -- The ink key item
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn is going away for real");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Lilly tells Bronann a bit about Kalya", 2000);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Quest1: Hide and Seek3: Orlinn is going away for real", orlinn, 95, 2, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn disappears.");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Quest1: Hide and Seek3: Orlinn disappears.", orlinn, "MakeInvisible", "");
    EventManager:RegisterEvent(event);

    event = vt_map.AnimateSpriteEvent("Quest1: Bronann is sad", bronann, "hero_stance", 2000);
    EventManager:RegisterEvent(event);

    -- Lilly tells Bronann about Kalya
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Bronann?");
    dialogue:AddLineEvent(text, lilly, "", "Quest1: Hide and Seek3: Bronann turns to Lilly");
    text = vt_system.Translate("Er, yes?");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Don't blame Kalya. She was having a hard time along with her brother before living here, you know?");
    dialogue:AddLineEvent(text, lilly, "", "Quest1: Bronann is sad");
    text = vt_system.Translate("Well anyway, she's been ignoring me since the first day I saw her...");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Eh? You really know nothing about women, do you?");
    dialogue:AddLine(text, lilly);
    text = vt_system.Translate("... Huh?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    text = vt_system.Translate("Heh heh, don't worry about it. It will come in time.");
    dialogue:AddLine(text, lilly);
    DialogueManager:AddDialogue(dialogue);

    event = vt_map.DialogueEvent("Quest1: Hide and Seek3: Lilly tells Bronann a bit about Kalya", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Map:PopState()");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: done");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Quest1: Hide and Seek3: Bronann turns to Lilly", bronann, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Quest1: Hide and Seek3: done", "Set_Hide_And_Seek3_Done", "");
    EventManager:RegisterEvent(event);

    -- Lilly goes and bring back barley meal
    event = vt_map.ScriptedEvent("Quest1: Prepare Lilly for a walk", "Prepare_lilly_walk", "");
    event:AddEventLinkAtEnd("Quest1: Lilly goes in her house");
    event:AddEventLinkAtEnd("Quest1: Bronann moves to his wait place");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Quest1: Bronann moves to his wait place", bronann, 70, 40, false);
    event:AddEventLinkAtEnd("Quest1: Barley Meal: Bronann stares at Lilly");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Quest1: Barley Meal: Bronann stares at Lilly", bronann, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Quest1: Lilly goes in her house", lilly, 90, 60, false);
    event:AddEventLinkAtEnd("Quest1: Lilly comes back");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Quest1: Lilly comes back", lilly, 67, 40, false);
    event:AddEventLinkAtEnd("Quest1: Barley Meal: Lilly turns to Bronann");
    event:AddEventLinkAtEnd("Quest1: Barley Meal: Bronann turns to Lilly");
    event:AddEventLinkAtEnd("Quest1: Barley Meal: Lilly tells Bronann about the barley meal");
    EventManager:RegisterEvent(event);

    event = vt_map.LookAtSpriteEvent("Quest1: Barley Meal: Lilly turns to Bronann", lilly, bronann);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Quest1: Barley Meal: Bronann turns to Lilly", bronann, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Here it is, Bronann.");
    dialogue:AddLine(text, lilly);
    DialogueManager:AddDialogue(dialogue);

    event = vt_map.DialogueEvent("Quest1: Barley Meal: Lilly tells Bronann about the barley meal", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest1: Barley Meal: Lilly give the barley meal to bronann");
    EventManager:RegisterEvent(event);

    event = vt_map.TreasureEvent("Quest1: Barley Meal: Lilly give the barley meal to bronann");
    event:AddEventLinkAtEnd("Quest1: Barley meal is given.");
    event:AddObject(70002, 1); -- The barley meal key item
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Quest1: Barley meal is given.", "GiveBarleyMeal", "");
    event:AddEventLinkAtEnd("Quest1: Barley Meal: Lilly tells Bronann about the restrictions");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Thanks, Lilly.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("We've been having a shortage of food lately. I've been taking... special measures... to ensure that everyone has enough food.");
    dialogue:AddLine(text, lilly);
    text = vt_system.Translate("Try to enjoy this day, Bronann.");
    dialogue:AddLine(text, lilly);
    DialogueManager:AddDialogue(dialogue);

    event = vt_map.DialogueEvent("Quest1: Barley Meal: Lilly tells Bronann about the restrictions", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest1: Reset Lilly dialogue");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Quest1: Reset Lilly dialogue", "Reset_lilly_dialogue", "");
    event:AddEventLinkAtEnd("Map:PopState()");
    EventManager:RegisterEvent(event);
end

-- zones
local village_center_zone = {};
local to_village_entrance_zone = {};
local to_riverbank_house_entrance_zone = {};
local to_secret_path_entrance_zone = {};
local orlinn_hide_n_seek2_zone = {};

function _CreateZones()
    -- N.B.: left, right, top, bottom
    village_center_zone = vt_map.CameraZone(89, 105, 0, 2);
    Map:AddZone(village_center_zone);

    to_village_entrance_zone = vt_map.CameraZone(118, 119, 10, 27);
    Map:AddZone(to_village_entrance_zone);

    to_riverbank_house_entrance_zone = vt_map.CameraZone(96, 100, 46, 47);
    Map:AddZone(to_riverbank_house_entrance_zone);

    to_secret_path_entrance_zone = vt_map.CameraZone(60, 72, 0, 2);
    Map:AddZone(to_secret_path_entrance_zone);

    orlinn_hide_n_seek2_zone = vt_map.CameraZone(75, 80, 0, 7);
    Map:AddZone(orlinn_hide_n_seek2_zone);
end

function _CheckZones()
    if (village_center_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to Village center");
    end

    if (to_village_entrance_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to Village south entrance");
    end

    if (to_riverbank_house_entrance_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        AudioManager:PlaySound("snd/door_open2.wav");
        EventManager:StartEvent("to Riverbank house");
    end

    if (to_secret_path_entrance_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to secret path entrance");
    end

    -- zone based story events
    if (GlobalManager:DoesEventExist("layna_south_entrance", "quest1_orlinn_hide_n_seek1_done") == true) then
        if (orlinn_hide_n_seek2_zone:IsCameraEntering() == true) then
            -- Updates the story state
            if (GlobalManager:DoesEventExist("layna_riverbank", "quest1_orlinn_hide_n_seek2_done") == false) then
                GlobalManager:SetEventValue("layna_riverbank", "quest1_orlinn_hide_n_seek2_done", 1);

                -- Orlinn speaks and flee
                Map:PushState(vt_map.MapMode.STATE_SCENE);
                orlinn:SetDirection(vt_map.MapMode.WEST);
                EventManager:StartEvent("Quest1: Orlinn starts hide and seek3 speech");
            end
        end
    end
end

-- Custom inner map functions
function _SetLillyState()
    lilly:SetDirection(vt_map.MapMode.WEST);
    lilly:ClearDialogueReferences();
    if (GlobalManager:DoesEventExist("story", "kalya_has_joined") == true) then
        -- Once they can go into the forest
        dialogue = vt_map.SpriteDialogue();
        text = vt_system.Translate("We're counting on you.");
        dialogue:AddLine(text, lilly);
        DialogueManager:AddDialogue(dialogue);
        lilly :AddDialogueReference(dialogue);
    elseif (GlobalManager:DoesEventExist("story", "quest1_barley_meal_done") == true) then
        dialogue = vt_map.SpriteDialogue("ep1_layna_village_lilly_go_home");
        text = vt_system.Translate("You should go back home. Your mom must be waiting for you.");
        dialogue:AddLine(text, lilly);
        DialogueManager:AddDialogue(dialogue);
        lilly :AddDialogueReference(dialogue);
    elseif (GlobalManager:DoesEventExist("layna_center", "quest1_pen_given_done") == true) then
        dialogue = vt_map.SpriteDialogue();
        text = vt_system.Translate("What a nice day, isn't it?");
        dialogue:AddLine(text, lilly);
        text = vt_system.Translate("Yes, Lilly. But I have something I have to ask you.");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Sure, just ask.");
        dialogue:AddLine(text, lilly);
        text = vt_system.Translate("Have you still got any barley meal left for my parents and I? My mother asked me to get some and I...");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Unfortunately, I gave it all to Kalya...");
        dialogue:AddLine(text, lilly);
        text = vt_system.Translate("What?!");
        dialogue:AddLineEmote(text, bronann, "sweat drop");
        text = vt_system.Translate("Ah hah! I just wanted to see your reaction. I still have some. Let me go grab it and bring it back to you.");
        dialogue:AddLineEvent(text, lilly, "", "Quest1: Prepare Lilly for a walk");
        DialogueManager:AddDialogue(dialogue);
        lilly :AddDialogueReference(dialogue);
    else
        dialogue = vt_map.SpriteDialogue("ep1_layna_village_lilly_default");
        text = vt_system.Translate("What a nice day, isn't it?");
        dialogue:AddLine(text, lilly);
        DialogueManager:AddDialogue(dialogue);
        lilly :AddDialogueReference(dialogue);
    end
end

function _SetOrlinnState()
    local text = {}
    local dialogue = {}

    orlinn_dialogue_npc:ClearDialogueReferences();
    orlinn:ClearDialogueReferences();

    if (GlobalManager:DoesEventExist("layna_riverbank", "quest1_orlinn_hide_n_seek3_done") == true) then
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetVisible(false);
        return;
    elseif (GlobalManager:DoesEventExist("layna_riverbank", "quest1_orlinn_hide_n_seek2_done") == true) then
        orlinn:SetPosition(74, 44);
    orlinn:SetDirection(vt_map.MapMode.WEST);

        -- Final hide and seek dialogue
        dialogue = vt_map.SpriteDialogue();
        text = vt_system.Translate("You'll never get me!");
        dialogue:AddLineEvent(text, orlinn, "", "Quest1: Hide and Seek3: Orlinn starts to go away");
        DialogueManager:AddDialogue(dialogue);
        orlinn:AddDialogueReference(dialogue);

        EventManager:StartEvent("Quest1: Hide and Seek3: Orlinn goes top-right", 8000);
        return;
    elseif (GlobalManager:DoesEventExist("layna_south_entrance", "quest1_orlinn_hide_n_seek1_done") == true) then
        -- Orlinn is on the cliff and is mocking Bronann.
        dialogue = vt_map.SpriteDialogue();
        text = vt_system.Translate("Hee hee hee!");
        dialogue:AddLine(text, orlinn_dialogue_npc);
        text = vt_system.Translate("Orlinn, how did you get there?");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("(giggle) I won't tell you!");
        dialogue:AddLine(text, orlinn_dialogue_npc);
        DialogueManager:AddDialogue(dialogue);
        orlinn_dialogue_npc:AddDialogueReference(dialogue);
        return;
    end

    -- Orlinn default behaviour
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);
end

-- Map Custom functions
map_functions = {
    Map_PopState = function()
        Map:PopState();
    end,

    MakeInvisible = function(sprite)
        if (sprite ~= nil) then
            sprite:SetVisible(false);
            sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end,

    Prepare_orlinn_kalya_scene = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        -- Prepare Kalya
        kalya:SetPosition(78, 25);
        kalya:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        kalya:SetVisible(true);

        -- Stop Orlinn event loop and prepare him for the next events
        orlinn:SetMoving(false); -- in case he's moving
        orlinn:SetDirection(vt_map.MapMode.NORTH);
        orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
        orlinn:ClearDialogueReferences();
        EventManager:TerminateAllEvents(orlinn);
    end,

    Set_Hide_And_Seek3_Done = function()
        GlobalManager:SetEventValue("layna_riverbank", "quest1_orlinn_hide_n_seek3_done", 1);

        -- Also reset Lilly direction
        lilly:SetDirection(vt_map.MapMode.WEST);
    end,

    Prepare_lilly_walk = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        -- Stop Lilly dialogue and prepare her for the next events
        lilly:SetMoving(false); -- in case he's moving
        lilly:ClearDialogueReferences();
        EventManager:TerminateAllEvents(lilly);
    end,

    GiveBarleyMeal = function()
        GlobalManager:SetEventValue("story", "quest1_barley_meal_done", 1);
    end,

    Reset_lilly_dialogue = function()
        _SetLillyState();
    end
}
