-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_bronanns_home_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = ""
map_image_filename = ""
map_subname = "Bronann's home"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = nil
local DialogueManager = nil
local EventManager = nil

-- The main character handlers
local bronann = nil
local bronanns_dad = nil
local bronanns_mother = nil
local quest2_start_scene = false

-- the main map loading code
function Load(m)

    Map = m;
    DialogueManager = Map:GetDialogueSupervisor();
    EventManager = Map:GetEventSupervisor();

    Map:SetUnlimitedStamina(true);

    _CreateCharacters();
    _CreateNPCs();
    _CreateObjects();

    -- Set the camera focus on bronann
    Map:SetCamera(bronann);

    _CreateEvents();
    _CreateZones();
end

function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end


-- Character creation
function _CreateCharacters()
    -- default position and direction
    bronann = CreateSprite(Map, "Bronann", 46.5, 11.5, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.SOUTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- set up the position according to the previous map
    if (GlobalManager:GetPreviousLocation() == "from_village_center") then
        bronann:SetPosition(39.5, 22.5);
        bronann:SetDirection(vt_map.MapMode.NORTH);
        AudioManager:PlaySound("snd/door_close.wav");
    end
end

function _CreateNPCs()
    local event = nil
    local dialogue = nil
    local text = nil

    bronanns_dad = CreateSprite(Map, "Carson", 33.5, 11.5, vt_map.MapMode.GROUND_OBJECT);

    event = vt_map.RandomMoveSpriteEvent("Dad random move", bronanns_dad, 2000, 2000);
    event:AddEventLinkAtEnd("Dad random move", 3000); -- Loop on itself
    EventManager:RegisterEvent(event);

    if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == true) then
        -- Carson isn't here anymore
        bronanns_dad:SetVisible(false);
        bronanns_dad:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        bronanns_dad:SetPosition(0, 0);
    else
        EventManager:StartEvent("Dad random move");
    end

    dialogue = vt_map.SpriteDialogue.Create("ep1_bronann_home_talk_with_dad");
    text = vt_system.Translate("Hey son! Did you sleep well? Hmm, now where did I leave that oil lamp?");
    dialogue:AddLine(text, bronanns_dad);
    text = vt_system.Translate("Hi Dad! Um, I don't know. Sorry.");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("Nah, no problem, I'll find it somewhere, eventually.");
    dialogue:AddLine(text, bronanns_dad);
    bronanns_dad:AddDialogueReference(dialogue);

    bronanns_mother = CreateSprite(Map, "Malta", 33.1, 17.5, vt_map.MapMode.GROUND_OBJECT);
    bronanns_mother:SetDirection(vt_map.MapMode.SOUTH);

    _UpdateMotherDialogue();

    -- Make her walk in front of the table to prepare the lunch.
    event = vt_map.PathMoveSpriteEvent("Kitchen: Mother goes middle", bronanns_mother, 33.1, 19.9, false);
    event:AddEventLinkAtEnd("Kitchen: Mother looks left");
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kitchen: Mother looks left", bronanns_mother, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Kitchen: Mother goes right", 2000);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Kitchen: Mother goes right", bronanns_mother, 35, 19.9, false);
    event:AddEventLinkAtEnd("Kitchen: Mother looks down");
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kitchen: Mother looks down", bronanns_mother, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Kitchen: Mother goes middle 2", 2000);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Kitchen: Mother goes middle 2", bronanns_mother, 33.1, 19.9, false);
    event:AddEventLinkAtEnd("Kitchen: Mother goes up");
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Kitchen: Mother goes up", bronanns_mother, 33.1, 17.5, false);
    event:AddEventLinkAtEnd("Kitchen: Mother looks left 2");
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kitchen: Mother looks left 2", bronanns_mother, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Kitchen: Mother goes middle", 2000);
    EventManager:RegisterEvent(event);
    -- The mother routine event
    EventManager:StartEvent("Kitchen: Mother goes middle");

    -- The Hero's first noble quest briefing...
    event = vt_map.ScriptedSpriteEvent("Start Quest1", bronanns_mother, "StartQuest1", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Quest1: Mother looks south", bronanns_mother, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Mother calls Bronann");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann!");
    dialogue:AddLine(text, bronanns_mother);

    event = vt_map.DialogueEvent("Mother calls Bronann", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("SetCameraOnMother");
    event:AddEventLinkAtEnd("BronannLooksUp");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("BronannLooksUp", bronann, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("SetCameraOnMother", bronanns_mother, "Map_SetCamera", "");
    event:AddEventLinkAtEnd("ClearDialogueRefOnMother");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("ClearDialogueRefOnMother", bronanns_mother, "ClearDialogueReferences", "");
    event:AddEventLinkAtEnd("Mother moves near entrance1");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Mother moves near entrance1", bronanns_mother, 38, 20, false);
    event:AddEventLinkAtEnd("MotherLooksSouth2");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("MotherLooksSouth2", bronanns_mother, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Mother quest1 dialogue");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Now that you're *finally* up, could you go buy some barley meal for us?");
    dialogue:AddLine(text, bronanns_mother);
    text = vt_system.Translate("Barley meal? Again?");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    text = vt_system.Translate("Hmph, just go boy. You'll be free after that, ok?");
    dialogue:AddLine(text, bronanns_mother);

    event = vt_map.DialogueEvent("Mother quest1 dialogue", dialogue);
    event:AddEventLinkAtEnd("Map_PopState");
    event:AddEventLinkAtEnd("SetQuest1DialogueDone");
    event:AddEventLinkAtEnd("SetCameraOnBronann");
    event:AddEventLinkAtEnd("Kitchen: Mother goes middle", 300);
    EventManager:RegisterEvent(event);

    -- Common events.
    -- Pop Map state
    event = vt_map.ScriptedEvent("Map_PopState", "Map_PopState", "");
    EventManager:RegisterEvent(event);

    -- Set the opening dialogue as done
    event = vt_map.ScriptedEvent("SetQuest1DialogueDone", "Quest1MotherStartDialogueDone", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("SetCameraOnBronann", bronann, "Map_SetCamera", "");
    EventManager:RegisterEvent(event);
end


function _CreateObjects()
    local object = nil

    CreateObject(Map, "Chair1", 47, 18, vt_map.MapMode.GROUND_OBJECT);

    CreateObject(Map, "Chair1_inverted", 41, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Chair1_north", 44, 15.3, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Table1", 44, 19, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 31, 14, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Vase1", 31, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Flower Pot1", 48.5, 11, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Flower Pot1", 31, 9, vt_map.MapMode.GROUND_OBJECT);

    --lights
    object = CreateObject(Map, "Left Window Light 2", 31, 15, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    object = CreateObject(Map, "Right Window Light 2", 49, 15, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    -- Turn the food and dishes are objects to permit the update of their visible status.
    plate_pile = CreateObject(Map, "Plate Pile1", 31, 22, vt_map.MapMode.GROUND_OBJECT);
    salad = CreateObject(Map, "Salad1", 31, 18, vt_map.MapMode.GROUND_OBJECT);
    green_pepper = CreateObject(Map, "Green Pepper1", 31, 20, vt_map.MapMode.GROUND_OBJECT);
    bread = CreateObject(Map, "Bread1", 31, 22, vt_map.MapMode.GROUND_OBJECT);
    sauce_pot = CreateObject(Map, "Sauce Pot1", 33, 22, vt_map.MapMode.GROUND_OBJECT);
    knife = CreateObject(Map, "Knife1", 35, 22, vt_map.MapMode.GROUND_OBJECT);

    _UpdateDishesAndFood();
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local text = nil
    local dialogue = nil

    -- Triggered Events
    event = vt_map.MapTransitionEvent("to village", "dat/maps/layna_village/layna_village_center_map.lua",
                                       "dat/maps/layna_village/layna_village_center_script.lua", "from_bronanns_home");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to Bronann's 1st floor", "dat/maps/layna_village/layna_village_bronanns_home_first_floor_map.lua",
                                       "dat/maps/layna_village/layna_village_bronanns_home_first_floor_script.lua", "from_bronanns_home");
    EventManager:RegisterEvent(event);

    -- Generic events
    event = vt_map.ScriptedEvent("Audio:FadeOutMusic()", "Audio_FadeOutMusic", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Audio:ResumeMusic()", "Audio_ResumeMusic", "");
    EventManager:RegisterEvent(event);

    -- Quest events

    -- End quest 1 (Barley meal retrieval) and prepare map for what's next.
    event = vt_map.ScriptedEvent("Quest1: end and transition to after-dinner", "Quest1Done", "");
    event:AddEventLinkAtEnd("Quest1: Terminate mother and father events");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Quest1: Terminate mother and father events", "TerminateMotherAndFatherEvents", "");
    event:AddEventLinkAtEnd("Fade out to after dinner");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Fade out to after dinner", "FadeOutToAfterDinner", "CheckFadeInOrOut");
    event:AddEventLinkAtEnd("Fade in to after dinner");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Fade in to after dinner", "FadeInToAfterDinner", "CheckFadeInOrOut");
    event:AddEventLinkAtEnd("Quest2: Bronann is told not to leave town - part 1");
    event:AddEventLinkAtEnd("Quest2: Father looks west");
    EventManager:RegisterEvent(event);

    -- Quest 2 start: Bronann is told to not leave town
    event = vt_map.ChangeDirectionSpriteEvent("Quest2: Father looks west", bronanns_dad, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Thanks for helping me out with the dishes.");
    dialogue:AddLine(text, bronanns_mother);
    text = vt_system.Translate("Say, mom? Why is the village entrance blocked?");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("...");
    dialogue:AddLineEmote(text, bronanns_mother, "sweat drop");
    event = vt_map.DialogueEvent("Quest2: Bronann is told not to leave town - part 1", dialogue)
    -- Make a pause here
    event:AddEventLinkAtEnd("Quest2: Father looks south to think");
    event:AddEventLinkAtEnd("Audio:FadeOutMusic()");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Quest2: Father looks south to think", bronanns_dad, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Quest2: Father looks at Bronann", 2000);
    EventManager:RegisterEvent(event);

    event = vt_map.LookAtSpriteEvent("Quest2: Father looks at Bronann", bronanns_dad, bronann);
    event:AddEventLinkAtEnd("Quest2: Bronann is told not to leave town - part 2");
    EventManager:RegisterEvent(event);

    event = vt_map.AnimateSpriteEvent("Quest2: Bronann looks at both parents", bronann, "searching", 1000);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann, I'd like for you to not leave the village today.");
    dialogue:AddLine(text, bronanns_dad);
    text = vt_system.Translate("Huh?! What? Why? You told me that I could go into the forest and...");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("Sorry, son. It's maybe a bit early, but I'd like you to be careful.");
    dialogue:AddLine(text, bronanns_dad);
    text = vt_system.Translate("Hey, wait! All of the village elders' nerves are on edge. There is something going on here! Why won't you tell me?");
    dialogue:AddLineEventEmote(text, bronann, "", "Quest2: Bronann looks at both parents", "interrogation");
    text = vt_system.Translate("Neither of you?");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("You really won't tell me?");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("... It's not that simple, Bronann. Believe me.");
    dialogue:AddLineEmote(text, bronanns_dad, "thinking dots");
    event = vt_map.DialogueEvent("Quest2: Bronann is told not to leave town - part 2", dialogue)
    event:AddEventLinkAtEnd("Quest2: Mother looks at Bronann", 2000);
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Quest2: Mother looks at Bronann", bronanns_mother, vt_map.MapMode.NORTH);
    event:AddEventLinkAtEnd("Quest2: Bronann is told not to leave town - part 3");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann, this time I want you to listen to your father very carefully. Please, my dear.");
    dialogue:AddLine(text, bronanns_mother);
    text = vt_system.Translate("But mom!");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Bronann, please.");
    dialogue:AddLine(text, bronanns_dad);
    text = vt_system.Translate("(grumble)... Crap!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    event = vt_map.DialogueEvent("Quest2: Bronann is told not to leave town - part 3", dialogue);
    event:AddEventLinkAtEnd("Quest2: Bronann is frustrated");
    EventManager:RegisterEvent(event);

    event = vt_map.AnimateSpriteEvent("Quest2: Bronann is frustrated", bronann, "hero_stance", 1000);
    event:AddEventLinkAtEnd("Quest2: Bronann runs out of the house");
    EventManager:RegisterEvent(event);

    -- Make Bronann leave house
    event = vt_map.PathMoveSpriteEvent("Quest2: Bronann runs out of the house", bronann, 40, 24.5, true);
    event:AddEventLinkAtEnd("Quest2: Bronann disappears after running out of the house");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Quest2: Bronann disappears after running out of the house", bronann, "MakeInvisible", "");
    event:AddEventLinkAtEnd("Quest2: Bronann is told not to leave town - part 4", 2000);
    event:AddEventLinkAtEnd("Quest2: Mother looks at father", 1000);
    event:AddEventLinkAtEnd("Quest2: SetCamera on mother", 1000);
    event:AddEventLinkAtEnd("Quest2: Father looks at mother", 2000);
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Quest2: Father looks at mother", bronanns_dad, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Quest2: Mother looks at father", bronanns_mother, vt_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("Quest2: SetCamera on mother", bronanns_mother, "Map_SetCamera", "");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Maybe we should tell him.");
    dialogue:AddLine(text, bronanns_mother);
    text = vt_system.Translate("It's too early, darling. We might be wrong.");
    dialogue:AddLineEmote(text, bronanns_dad, "thinking dots");
    text = vt_system.Translate("I really hope we are.");
    dialogue:AddLine(text, bronanns_dad);
    event = vt_map.DialogueEvent("Quest2: Bronann is told not to leave town - part 4", dialogue);
    event:AddEventLinkAtEnd("Map_PopState");
    event:AddEventLinkAtEnd("to village");
    EventManager:RegisterEvent(event);
end

-- zones
local home_exit_zone = nil
local to_bronanns_room_zone = nil

function _CreateZones()
    -- N.B.: left, right, top, bottom
    home_exit_zone = vt_map.CameraZone.Create(38, 41, 24, 25);
    to_bronanns_room_zone = vt_map.CameraZone.Create(44, 47, 8, 9);

    quest2_start_scene = false;
end

function _CheckZones()
    -- Don't check that zone when dealing with the quest 2 start scene.
    if (quest2_start_scene == false and home_exit_zone:IsCameraEntering() == true) then
        -- Prevent Bronann from exiting until his mother talked to him
        if (GlobalManager:DoesEventExist("bronanns_home", "quest1_mother_start_dialogue_done") == false) then
            Map:PushState(vt_map.MapMode.STATE_SCENE);
            EventManager:StartEvent("Start Quest1");
        else
            EventManager:StartEvent("to village");
            AudioManager:PlaySound("snd/door_open2.wav");
        end
    end
    if (to_bronanns_room_zone:IsCameraEntering() == true) then
        EventManager:StartEvent("to Bronann's 1st floor");
    end
end


-- Internal Custom functions
function _UpdateDishesAndFood()
        if (GlobalManager:DoesEventExist("story", "Quest2_started") == true) then
        -- Show the plate pile, hide the rest
        plate_pile:SetVisible(true);
        plate_pile:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);

        salad:SetVisible(false);
        salad:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        green_pepper:SetVisible(false);
        green_pepper:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        bread:SetVisible(false);
        bread:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        sauce_pot:SetVisible(false);
        sauce_pot:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        knife:SetVisible(false);
        knife:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    else
        -- Show the food, hide the plate pile
        plate_pile:SetVisible(false);
        plate_pile:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        salad:SetVisible(true);
        salad:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        green_pepper:SetVisible(true);
        green_pepper:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        bread:SetVisible(true);
        bread:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        sauce_pot:SetVisible(true);
        sauce_pot:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        knife:SetVisible(true);
        knife:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
    end
end

function _UpdateMotherDialogue()
    bronanns_mother:ClearDialogueReferences();

    if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == true) then
        local dialogue = vt_map.SpriteDialogue.Create();
        local text = vt_system.Translate("Bronann, promise me that you'll be careful, ok?");
        dialogue:AddLine(text, bronanns_mother);
        bronanns_mother:AddDialogueReference(dialogue);
        return;
    end
    if (GlobalManager:DoesEventExist("story", "quest1_barley_meal_done") == true) then
        -- Got some barley meal, Mom!
        -- Begining dialogue
        local dialogue = vt_map.SpriteDialogue.Create();
        local text = vt_system.Translate("(sigh)... Got it, mom!");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Perfect timing, let's have dinner.");
        dialogue:AddLineEvent(text, bronanns_mother, "", "Quest1: end and transition to after-dinner");
        bronanns_mother:AddDialogueReference(dialogue);
    elseif (GlobalManager:DoesEventExist("bronanns_home", "quest1_mother_start_dialogue_done") == true) then
        -- 1st quest dialogue
        local dialogue = vt_map.SpriteDialogue.Create("ep1_bronann_home_talk_about_barley_meal");
        local text = vt_system.Translate("Could you go and buy some barley meal for the three of us?");
        dialogue:AddLine(text, bronanns_mother);
        bronanns_mother:AddDialogueReference(dialogue);
    elseif (GlobalManager:DoesEventExist("bronanns_home", "quest1_mother_start_dialogue_done") == false) then
        -- Begining dialogue
        local dialogue = vt_map.SpriteDialogue.Create("ep1_bronann_home_talk_with_mother1");
        local text = vt_system.Translate("Hi son, did you have a nightmare again last night?");
        dialogue:AddLine(text, bronanns_mother);
        text = vt_system.Translate("Hi mom. Huh, how did you know?");
        dialogue:AddLineEmote(text, bronann, "interrogation");
        text = vt_system.Translate("Eh eh? Have you already forgotten that I'm your mother?");
        dialogue:AddLine(text, bronanns_mother);
        bronanns_mother:AddDialogueReference(dialogue);
    else
        -- Last default dialogue
        local dialogue = vt_map.SpriteDialogue.Create();
        local text = vt_system.Translate("Don't venture too far, I'll need your help soon!");
        dialogue:AddLine(text, bronanns_mother);
        bronanns_mother:AddDialogueReference(dialogue);
    end
end


-- Map Custom functions
map_functions = {

    Map_PopState = function()
        Map:PopState();
    end,

    Map_SetCamera = function(sprite)
        Map:SetCamera(sprite, 800);
    end,

    Audio_FadeOutMusic = function()
        AudioManager:FadeOutActiveMusic(2000);
    end,

    MakeInvisible = function(sprite)
        if (sprite ~= nil) then
            sprite:SetVisible(false);
            sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end,

    ClearDialogueReferences = function(sprite)
        sprite:ClearDialogueReferences();
    end,

    StartQuest1 =  function(sprite)
        EventManager:EndAllEvents(sprite);
        sprite:SetMoving(false); -- in case she's moving
        EventManager:StartEvent("Quest1: Mother looks south");
    end,

    Quest1MotherStartDialogueDone = function()
        GlobalManager:SetEventValue("bronanns_home", "quest1_mother_start_dialogue_done", 1);
        _UpdateMotherDialogue();
        GlobalManager:AddQuestLog("get_barley");
    end,

    Quest1Done = function()
        GlobalManager:SetEventValue("story", "Quest1_done", 1);
    end,

    TerminateMotherAndFatherEvents = function()
        -- Start scene
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        -- Stop everyone
        bronanns_dad:SetMoving(false);
        bronanns_dad:ClearDialogueReferences();
        EventManager:EndAllEvents(bronanns_dad);
        bronanns_mother:SetMoving(false);
        bronanns_mother:ClearDialogueReferences();
        EventManager:EndAllEvents(bronanns_mother);

        bronann:SetMoving(false);
    end,

    FadeOutToAfterDinner = function()
        VideoManager:FadeScreen(vt_video.Color(0.0, 0.0, 0.0, 1.0), 1000);
    end,

    CheckFadeInOrOut = function()
        if (VideoManager:IsFading() == true) then
            return false;
        end
        return true;
    end,

    FadeInToAfterDinner = function()
        -- Place characters
        bronann:SetPosition(41.5, 15.0);
        bronann:SetDirection(vt_map.MapMode.SOUTH);

        bronanns_mother:SetPosition(40, 19);
        bronanns_mother:SetDirection(vt_map.MapMode.EAST);
        bronanns_dad:SetPosition(48, 19);
        bronanns_dad:SetDirection(vt_map.MapMode.WEST);

        -- Remove the barley meal key item from inventory
        local barley_meal_item_id = 70002;
        if (GlobalManager:IsItemInInventory(barley_meal_item_id) == true) then
            GlobalManager:RemoveFromInventory(barley_meal_item_id);
        end

        -- Set the quest 2 as started
        GlobalManager:SetEventValue("story", "Quest2_started", 1);
        GlobalManager:AddQuestLog("wants_to_go_into_the_forest");
        -- Make the food and dishes not appear anymore, once the dinner is done.
        _UpdateDishesAndFood();

        VideoManager:FadeIn(1000);

        -- Flag used to disable the warp zone temporarily
        quest2_start_scene = true;
    end
}
