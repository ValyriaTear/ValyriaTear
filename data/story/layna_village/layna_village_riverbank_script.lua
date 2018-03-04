-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_riverbank_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "data/story/common/locations/mountain_village.png"
map_subname = "Riverbank"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

local bronann = nil
local kalya = nil
local orlinn = nil
local orlinn_dialogue_npc = nil
local lilly = nil

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
    Map:GetEffectSupervisor():EnableAmbientOverlay("data/visuals/ambient/clouds.png", 5.0, -5.0, true);

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
        Map:GetScriptSupervisor():AddScript("data/credits/episode1_credits.lua");
    end
end

function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    bronann = CreateSprite(Map, "Bronann", 97, 4, vt_map.MapMode.GROUND_OBJECT);
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
        AudioManager:PlaySound("data/sounds/door_close.wav");
    end
end

function _CreateNPCs()
    local text = nil
    local dialogue = nil
    local event = nil

    lilly = CreateNPCSprite(Map, "Woman3", vt_system.Translate("Lilly"), 67, 40, vt_map.MapMode.GROUND_OBJECT);

    local object = CreateObject(Map, "Dog1", 66, 42, vt_map.MapMode.GROUND_OBJECT);
    event = vt_map.SoundEvent.Create("Lilly's dog barks", "data/sounds/dog_barking.wav");

    object:SetEventWhenTalking("Lilly's dog barks");

    kalya = CreateSprite(Map, "Kalya", 2, 2, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.SOUTH);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);

    orlinn = CreateSprite(Map, "Orlinn", 82, 5, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.SOUTH);
    orlinn:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);

    -- Create an invisible doppelganger to permit triggering dialogues when the kid is on the cliff.
    orlinn_dialogue_npc = CreateSprite(Map, "Orlinn", 82, 8, vt_map.MapMode.GROUND_OBJECT);
    orlinn_dialogue_npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn_dialogue_npc:SetVisible(false);
end

function _CreateObjects()
    local object = nil

    CreateObject(Map, "Tree Big2", 70, 6, vt_map.MapMode.GROUND_OBJECT);

    -- Add hill treasure chest
    local hill_chest = CreateTreasure(Map, "riverbank_secret_hill_chest", "Wood_Chest1", 72, 5, vt_map.MapMode.GROUND_OBJECT);
    hill_chest:AddItem(1, 3); -- 3 small potions

    -- trees around the house
    CreateObject(Map, "Tree Big2", 92, 10, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 82, 17, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 75, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 72, 35, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 74, 48, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 76, 50, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 56.3, 56.5, vt_map.MapMode.GROUND_OBJECT);

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
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end

    -- Lights
    -- big round windows light flares
    vt_map.Light.Create("data/visuals/lights/sun_flare_light_main.lua",
                        "data/visuals/lights/sun_flare_light_secondary.lua",
                        94.0, 40.0,
                        vt_video.Color(1.0, 1.0, 1.0, 0.6),
                        vt_video.Color(1.0, 1.0, 0.85, 0.3));
    vt_map.Light.Create("data/visuals/lights/sun_flare_light_main.lua",
                        "data/visuals/lights/sun_flare_light_secondary.lua",
                        102.0, 40.0,
                        vt_video.Color(1.0, 1.0, 1.0, 0.6),
                        vt_video.Color(1.0, 1.0, 0.85, 0.3));
    -- Small door lights
    vt_map.Light.Create("data/visuals/lights/sun_flare_light_small_main.lua",
                        "data/visuals/lights/sun_flare_light_small_secondary.lua",
                        98.0, 43.5,
                        vt_video.Color(1.0, 1.0, 1.0, 0.6),
                        vt_video.Color(1.0, 1.0, 0.85, 0.3));

    vt_map.SoundObject.Create("data/sounds/gentle_stream.ogg", 61.0, 27.0, 20.0);
    vt_map.SoundObject.Create("data/sounds/gentle_stream.ogg", 61.0, 47.0, 20.0);
    vt_map.SoundObject.Create("data/sounds/gentle_stream.ogg", 61.0, 67.0, 20.0);
    vt_map.SoundObject.Create("data/sounds/gentle_stream.ogg", 81.0, 67.0, 20.0);
    vt_map.SoundObject.Create("data/sounds/gentle_stream.ogg", 101.0, 67.0, 20.0);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Map change Events
    vt_map.MapTransitionEvent.Create("to Village center", "data/story/layna_village/layna_village_center_map.lua",
                                     "data/story/layna_village/layna_village_center_script.lua", "from_riverbank");

    vt_map.MapTransitionEvent.Create("to Village south entrance", "data/story/layna_village/layna_village_south_entrance_map.lua",
                                     "data/story/layna_village/layna_village_south_entrance_script.lua", "from_riverbank");

    vt_map.MapTransitionEvent.Create("to Riverbank house", "data/story/layna_village/layna_village_riverbank_house_map.lua",
                                     "data/story/layna_village/layna_village_riverbank_house_script.lua", "from_riverbank");

    vt_map.MapTransitionEvent.Create("to secret path entrance", "data/story/layna_village/layna_village_center_map.lua",
                                     "data/story/layna_village/layna_village_center_script.lua", "from_secret_path");

    vt_map.ScriptedEvent.Create("Orlinn laughs", "orlinn_laughs", "");

    -- Quest events - Hide and seek 2
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Wow! You found me!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    text = vt_system.Translate("But I'm not done yet!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("You'll never find me hiding behind the...");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Hey, why would I tell you my hiding spots?!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");

    event = vt_map.DialogueEvent.Create("Quest1: Orlinn starts hide and seek3 speech", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest1: Hide and seek2: Make Orlinn run");

    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Hide and seek2: Make Orlinn run", orlinn, 72, 2, true);
    event:AddEventLinkAtEnd("Quest1: Hide and seek2: Make Orlinn disappear");

    event = vt_map.ScriptedSpriteEvent.Create("Quest1: Hide and seek2: Make Orlinn disappear", orlinn, "MakeInvisible", "");
    event:AddEventLinkAtEnd("Quest1: Hide and seek2: Bronann end speech");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("That kid is pretty quick. This is going to take all day.");
    dialogue:AddLineEmote(text, bronann, "sweat drop");

    event = vt_map.DialogueEvent.Create("Quest1: Hide and seek2: Bronann end speech", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Map:PopState()");

    vt_map.ScriptedEvent.Create("Map:PopState()", "Map_PopState", "");

    -- Final hide and seek (3)
    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Hide and Seek3: Orlinn goes top-right", orlinn, 70, 42, false);
    event:AddEventLinkAtEnd("Hide n Seek3: Orlinn looks south");

    event = vt_map.ChangeDirectionSpriteEvent.Create("Hide n Seek3: Orlinn looks south", orlinn, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Hide n Seek3: Orlinn looks north", 800);

    event = vt_map.ChangeDirectionSpriteEvent.Create("Hide n Seek3: Orlinn looks north", orlinn, vt_map.MapMode.NORTH);
    event:AddEventLinkAtEnd("Hide n Seek3: Orlinn goes bottom-left", 800);

    event = vt_map.PathMoveSpriteEvent.Create("Hide n Seek3: Orlinn goes bottom-left", orlinn, 74, 43.8, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn goes top-right", 8000); -- finish the event loop.

    -- Before Kalya's arrival
    event = vt_map.ScriptedEvent.Create("Quest1: Hide and Seek3: Orlinn starts to go away", "Prepare_orlinn_kalya_scene", "");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn tries go to away");

    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Hide and Seek3: Orlinn tries go to away", orlinn, 76, 38, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Bronann follows him");

    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Hide and Seek3: Bronann follows him", bronann, 76, 40, true);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Bronann turns to Orlinn");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn turns to him");

    vt_map.ChangeDirectionSpriteEvent.Create("Quest1: Hide and Seek3: Bronann turns to Orlinn", bronann, vt_map.MapMode.NORTH);

    event = vt_map.ChangeDirectionSpriteEvent.Create("Quest1: Hide and Seek3: Orlinn turns to him", orlinn, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Bronann and kalya first speech");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Lilly turns to them");

    vt_map.ChangeDirectionSpriteEvent.Create("Quest1: Hide and Seek3: Lilly turns to them", lilly, vt_map.MapMode.EAST);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Please no! Wait!");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Orlinn! Stop this!");
    dialogue:AddLineEvent(text, kalya, "", "Quest1: Hide and Seek3: Kalya comes to Orlinn");

    event = vt_map.DialogueEvent.Create("Quest1: Hide and Seek3: Bronann and kalya first speech", dialogue);
    event:SetStopCameraMovement(true);

    -- Kalya's arrival
    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Hide and Seek3: Kalya comes to Orlinn", kalya, 76, 36, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Kalya's speech");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn turns to Kalya");

    vt_map.ChangeDirectionSpriteEvent.Create("Quest1: Hide and Seek3: Orlinn turns to Kalya", orlinn, vt_map.MapMode.NORTH);

    dialogue = vt_map.SpriteDialogue.Create();
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

    event = vt_map.DialogueEvent.Create("Quest1: Hide and Seek3: Kalya's speech", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Kalya is going away.");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn is going away.", 2000);

    -- Kalya and Orlinn going away
    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Hide and Seek3: Kalya is going away.", kalya, 95, 2, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Kalya disappears.");

    vt_map.ScriptedSpriteEvent.Create("Quest1: Hide and Seek3: Kalya disappears.", kalya, "MakeInvisible", "");

    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Hide and Seek3: Orlinn is going away.", orlinn, 76, 32, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn comes back to Bronann", 1000);

    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Hide and Seek3: Orlinn comes back to Bronann", orlinn, 76, 38, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn tells Bronann where the pen was");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I found that pen near the tree behind you. I just wanted to play.");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    text = vt_system.Translate("Don't worry about it.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Take it. Thanks.");
    dialogue:AddLine(text, orlinn);

    event = vt_map.DialogueEvent.Create("Quest1: Hide and Seek3: Orlinn tells Bronann where the pen was", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn gives the pen to Bronann");

    event = vt_map.TreasureEvent.Create("Quest1: Hide and Seek3: Orlinn gives the pen to Bronann");
    event:AddItem(70001, 1); -- The ink key item
    event:SetDrunes(20); -- The reward for running after Orlinn for so long.
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn is going away for real");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Lilly tells Bronann a bit about Kalya", 2000);

    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Hide and Seek3: Orlinn is going away for real", orlinn, 95, 2, false);
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: Orlinn disappears.");

    vt_map.ScriptedSpriteEvent.Create("Quest1: Hide and Seek3: Orlinn disappears.", orlinn, "MakeInvisible", "");

    vt_map.AnimateSpriteEvent.Create("Quest1: Bronann is sad", bronann, "hero_stance", 2000);

    -- Lilly tells Bronann about Kalya
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann?");
    dialogue:AddLineEvent(text, lilly, "", "Quest1: Hide and Seek3: Bronann turns to Lilly");
    text = vt_system.Translate("Er, yes?");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Don't blame Kalya. She was having a hard time along with her brother before living here, you know?");
    dialogue:AddLineEvent(text, lilly, "", "Quest1: Bronann is sad");
    text = vt_system.Translate("Well anyway, she's been ignoring me since the first day I saw her.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Eh? You really know nothing about women, do you?");
    dialogue:AddLine(text, lilly);
    text = vt_system.Translate("Huh?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    text = vt_system.Translate("Heh heh, don't worry about it. It will come in time.");
    dialogue:AddLine(text, lilly);

    event = vt_map.DialogueEvent.Create("Quest1: Hide and Seek3: Lilly tells Bronann a bit about Kalya", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Map:PopState()");
    event:AddEventLinkAtEnd("Quest1: Hide and Seek3: done");

    vt_map.ChangeDirectionSpriteEvent.Create("Quest1: Hide and Seek3: Bronann turns to Lilly", bronann, vt_map.MapMode.WEST);

    vt_map.ScriptedEvent.Create("Quest1: Hide and Seek3: done", "Set_Hide_And_Seek3_Done", "");

    -- Lilly goes and bring back barley meal
    event = vt_map.ScriptedEvent.Create("Quest1: Prepare Lilly for a walk", "Prepare_lilly_walk", "");
    event:AddEventLinkAtEnd("Quest1: Lilly goes in her house");
    event:AddEventLinkAtEnd("Quest1: Bronann moves to his wait place");

    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Bronann moves to his wait place", bronann, 70, 40, false);
    event:AddEventLinkAtEnd("Quest1: Barley Meal: Bronann stares at Lilly");

    vt_map.ChangeDirectionSpriteEvent.Create("Quest1: Barley Meal: Bronann stares at Lilly", bronann, vt_map.MapMode.SOUTH);

    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Lilly goes in her house", lilly, 90, 60, false);
    event:AddEventLinkAtEnd("Quest1: Lilly comes back");

    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Lilly comes back", lilly, 67, 40, false);
    event:AddEventLinkAtEnd("Quest1: Barley Meal: Lilly turns to Bronann");
    event:AddEventLinkAtEnd("Quest1: Barley Meal: Bronann turns to Lilly");
    event:AddEventLinkAtEnd("Quest1: Barley Meal: Lilly tells Bronann about the barley meal");

    vt_map.LookAtSpriteEvent.Create("Quest1: Barley Meal: Lilly turns to Bronann", lilly, bronann);

    vt_map.ChangeDirectionSpriteEvent.Create("Quest1: Barley Meal: Bronann turns to Lilly", bronann, vt_map.MapMode.WEST);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Here it is, Bronann.");
    dialogue:AddLine(text, lilly);

    event = vt_map.DialogueEvent.Create("Quest1: Barley Meal: Lilly tells Bronann about the barley meal", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest1: Barley Meal: Lilly give the barley meal to bronann");

    event = vt_map.TreasureEvent.Create("Quest1: Barley Meal: Lilly give the barley meal to bronann");
    event:AddEventLinkAtEnd("Quest1: Barley meal is given.");
    event:AddItem(70002, 1); -- The barley meal key item

    event = vt_map.ScriptedEvent.Create("Quest1: Barley meal is given.", "GiveBarleyMeal", "");
    event:AddEventLinkAtEnd("Quest1: Barley Meal: Lilly tells Bronann about the restrictions");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Thanks, Lilly.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("We've been having a shortage of food lately. I've been taking... special measures to ensure that everyone has enough food.");
    dialogue:AddLine(text, lilly);
    text = vt_system.Translate("Try to enjoy this day, Bronann.");
    dialogue:AddLine(text, lilly);

    event = vt_map.DialogueEvent.Create("Quest1: Barley Meal: Lilly tells Bronann about the restrictions", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest1: Reset Lilly dialogue");

    event = vt_map.ScriptedEvent.Create("Quest1: Reset Lilly dialogue", "Reset_lilly_dialogue", "");
    event:AddEventLinkAtEnd("Map:PopState()");
end

-- zones
local village_center_zone = nil
local to_village_entrance_zone = nil
local to_riverbank_house_entrance_zone = nil
local to_secret_path_entrance_zone = nil
local orlinn_hide_n_seek2_zone = nil

function _CreateZones()
    -- N.B.: left, right, top, bottom
    village_center_zone = vt_map.CameraZone.Create(89, 105, 0, 2);
    to_village_entrance_zone = vt_map.CameraZone.Create(118, 119, 10, 27);
    to_riverbank_house_entrance_zone = vt_map.CameraZone.Create(96, 100, 46, 47);
    to_secret_path_entrance_zone = vt_map.CameraZone.Create(60, 72, 0, 2);
    orlinn_hide_n_seek2_zone = vt_map.CameraZone.Create(75, 80, 0, 7);
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
        AudioManager:PlaySound("data/sounds/door_open2.wav");
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
    local text = nil
    local dialogue = nil

    lilly:SetDirection(vt_map.MapMode.WEST);
    lilly:ClearDialogueReferences();
    if (GlobalManager:DoesEventExist("story", "kalya_has_joined") == true) then
        -- Once they can go into the forest
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("We're counting on you.");
        dialogue:AddLine(text, lilly);
        lilly :AddDialogueReference(dialogue);
    elseif (GlobalManager:DoesEventExist("story", "quest1_barley_meal_done") == true) then
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_lilly_go_home");
        text = vt_system.Translate("You should go back home. Your mom must be waiting for you.");
        dialogue:AddLine(text, lilly);
        lilly :AddDialogueReference(dialogue);
    elseif (GlobalManager:DoesEventExist("layna_center_shop", "quest1_flora_dialogue_done") == true) then
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("What a nice day, isn't it?");
        dialogue:AddLine(text, lilly);
        text = vt_system.Translate("Yes, Lilly. But I have something I have to ask you.");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Sure, just ask.");
        dialogue:AddLine(text, lilly);
        text = vt_system.Translate("Have you still got any barley meal left for my parents and I? My mother asked me to get some and I...");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Unfortunately, I gave it all to Kalya.");
        dialogue:AddLine(text, lilly);
        text = vt_system.Translate("What?!");
        dialogue:AddLineEmote(text, bronann, "sweat drop");
        text = vt_system.Translate("Ah hah! I just wanted to see your reaction. I still have some. Let me go grab it and bring it back to you.");
        dialogue:AddLine(text, lilly);
        dialogue:SetEventAtDialogueEnd("Quest1: Prepare Lilly for a walk")
        lilly :AddDialogueReference(dialogue);
    else
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_lilly_default");
        text = vt_system.Translate("What a nice day, isn't it?");
        dialogue:AddLine(text, lilly);
        lilly :AddDialogueReference(dialogue);
    end
end

function _SetOrlinnState()
    local text = nil
    local dialogue = nil

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
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("You'll never get me!");
        dialogue:AddLineEvent(text, orlinn, "", "Quest1: Hide and Seek3: Orlinn starts to go away");
        orlinn:AddDialogueReference(dialogue);

        EventManager:StartEvent("Quest1: Hide and Seek3: Orlinn goes top-right", 8000);
        return;
    elseif (GlobalManager:DoesEventExist("layna_south_entrance", "quest1_orlinn_hide_n_seek1_done") == true) then
        -- Orlinn is on the cliff and is mocking Bronann.
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Hee hee hee!");
        dialogue:AddLineEvent(text, orlinn_dialogue_npc, "Orlinn laughs", "");
        text = vt_system.Translate("Orlinn, how did you get there?");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("(giggle) I won't tell you!");
        dialogue:AddLineEvent(text, orlinn_dialogue_npc, "Orlinn laughs", "");
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

    orlinn_laughs = function()
        orlinn:SetCustomAnimation("laughing", 1000);
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
        EventManager:EndAllEvents(orlinn);
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
        EventManager:EndAllEvents(lilly);
    end,

    GiveBarleyMeal = function()
        GlobalManager:SetEventValue("story", "quest1_barley_meal_done", 1);
    end,

    Reset_lilly_dialogue = function()
        _SetLillyState();
    end
}
