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
music_filename = "data/music/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = nil
local DialogueManager = nil
local EventManager = nil
local Effects = nil

-- The main character handlers
local bronann = nil
local carson = nil
local malta = nil
local quest2_start_scene = false

-- Used at the beginning to show the wooden sword when giving it to Bronann
local wooden_sword = nil

-- the main map loading code
function Load(m)

    Map = m;
    DialogueManager = Map:GetDialogueSupervisor()
    EventManager = Map:GetEventSupervisor()
    Effects = Map:GetEffectSupervisor()

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
        AudioManager:PlaySound("data/sounds/door_close.wav");
    end
end

function _CreateNPCs()
    local event = nil
    local dialogue = nil
    local text = nil

    carson = CreateSprite(Map, "Carson", 33.5, 11.5, vt_map.MapMode.GROUND_OBJECT);

    event = vt_map.RandomMoveSpriteEvent.Create("Dad random move", carson, 2000, 2000);
    event:AddEventLinkAtEnd("Dad random move", 3000); -- Loop on itself

    if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == true) then
        -- Carson isn't here anymore
        carson:SetVisible(false);
        carson:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        carson:SetPosition(0, 0);
    else
        EventManager:StartEvent("Dad random move");
    end

    _UpdateCarsonDialogue()

    malta = CreateSprite(Map, "Malta", 33.1, 17.5, vt_map.MapMode.GROUND_OBJECT);
    malta:SetDirection(vt_map.MapMode.SOUTH);

    _UpdateMotherDialogue();

    -- Make her walk in front of the table to prepare the lunch.
    event = vt_map.PathMoveSpriteEvent.Create("Kitchen: Mother goes middle", malta, 33.1, 19.9, false);
    event:AddEventLinkAtEnd("Kitchen: Mother looks left");
    event = vt_map.ChangeDirectionSpriteEvent.Create("Kitchen: Mother looks left", malta, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Kitchen: Mother goes right", 2000);
    event = vt_map.PathMoveSpriteEvent.Create("Kitchen: Mother goes right", malta, 35, 19.9, false);
    event:AddEventLinkAtEnd("Kitchen: Mother looks down");
    event = vt_map.ChangeDirectionSpriteEvent.Create("Kitchen: Mother looks down", malta, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Kitchen: Mother goes middle 2", 2000);
    event = vt_map.PathMoveSpriteEvent.Create("Kitchen: Mother goes middle 2", malta, 33.1, 19.9, false);
    event:AddEventLinkAtEnd("Kitchen: Mother goes up");
    event = vt_map.PathMoveSpriteEvent.Create("Kitchen: Mother goes up", malta, 33.1, 17.5, false);
    event:AddEventLinkAtEnd("Kitchen: Mother looks left 2");
    event = vt_map.ChangeDirectionSpriteEvent.Create("Kitchen: Mother looks left 2", malta, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Kitchen: Mother goes middle", 2000);

    -- The mother routine event
    EventManager:StartEvent("Kitchen: Mother goes middle");

    -- The Hero's first noble quest briefing...
    vt_map.ScriptedSpriteEvent.Create("Start Quest1", malta, "StartQuest1", "");

    event = vt_map.ChangeDirectionSpriteEvent.Create("Quest1: Mother looks south", malta, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Mother calls Bronann");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann!");
    dialogue:AddLine(text, malta);

    event = vt_map.DialogueEvent.Create("Mother calls Bronann", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("SetCameraOnMother");
    event:AddEventLinkAtEnd("BronannLooksUp");

    vt_map.ChangeDirectionSpriteEvent.Create("BronannLooksUp", bronann, vt_map.MapMode.NORTH);

    event = vt_map.ScriptedSpriteEvent.Create("SetCameraOnMother", malta, "Map_SetCamera", "");
    event:AddEventLinkAtEnd("ClearDialogueRefOnMother");

    event = vt_map.ScriptedSpriteEvent.Create("ClearDialogueRefOnMother", malta, "ClearDialogueReferences", "");
    event:AddEventLinkAtEnd("Mother moves near entrance1");

    event = vt_map.PathMoveSpriteEvent.Create("Mother moves near entrance1", malta, 38, 19, false);
    event:AddEventLinkAtEnd("MotherLooksSouth2");

    event = vt_map.ChangeDirectionSpriteEvent.Create("MotherLooksSouth2", malta, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Mother quest1 dialogue");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Now that you're *finally* up, could you go buy some barley meal for us?");
    dialogue:AddLine(text, malta);
    text = vt_system.Translate("Barley meal? Again?");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    text = vt_system.Translate("Hmph, just go boy. You'll be free after that, ok?");
    dialogue:AddLine(text, malta);

    event = vt_map.DialogueEvent.Create("Mother quest1 dialogue", dialogue);
    event:AddEventLinkAtEnd("Map_PopState");
    event:AddEventLinkAtEnd("SetQuest1DialogueDone");
    event:AddEventLinkAtEnd("SetCameraOnBronann");
    event:AddEventLinkAtEnd("Kitchen: Mother goes middle", 300);

    -- Common events.
    -- Pop Map state
    vt_map.ScriptedEvent.Create("Map_PopState", "Map_PopState", "");

    -- Set the opening dialogue as done
    vt_map.ScriptedEvent.Create("SetQuest1DialogueDone", "Quest1MotherStartDialogueDone", "");

    vt_map.ScriptedSpriteEvent.Create("SetCameraOnBronann", bronann, "Map_SetCamera", "");
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

    -- The wooden sword sprite
    wooden_sword = CreateObject(Map, "Wooden Sword1", 1, 1, vt_map.MapMode.GROUND_OBJECT);
    wooden_sword:SetVisible(false);
    wooden_sword:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    _UpdateDishesAndFood();
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local text = nil
    local dialogue = nil

    -- Triggered Events
    vt_map.MapTransitionEvent.Create("to village", "data/story/layna_village/layna_village_center_map.lua",
                                     "data/story/layna_village/layna_village_center_script.lua", "from_bronanns_home");

    vt_map.MapTransitionEvent.Create("to Bronann's 1st floor", "data/story/layna_village/layna_village_bronanns_home_first_floor_map.lua",
                                     "data/story/layna_village/layna_village_bronanns_home_first_floor_script.lua", "from_bronanns_home");

    -- Generic events
    vt_map.ScriptedEvent.Create("Audio:FadeOutMusic()", "Audio_FadeOutMusic", "");
    vt_map.ScriptedEvent.Create("Audio:ResumeMusic()", "Audio_ResumeMusic", "");

    vt_map.ChangeDirectionSpriteEvent.Create("Carson looks west", carson, vt_map.MapMode.WEST)
    vt_map.ChangeDirectionSpriteEvent.Create("Carson looks east", carson, vt_map.MapMode.EAST)
    vt_map.ChangeDirectionSpriteEvent.Create("Carson looks north", carson, vt_map.MapMode.NORTH)
    vt_map.ChangeDirectionSpriteEvent.Create("Malta looks west", malta, vt_map.MapMode.WEST)
    vt_map.ChangeDirectionSpriteEvent.Create("Malta looks east", malta, vt_map.MapMode.EAST)
    vt_map.ChangeDirectionSpriteEvent.Create("Malta looks north", malta, vt_map.MapMode.NORTH)
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks west", bronann, vt_map.MapMode.WEST)
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks east", bronann, vt_map.MapMode.EAST)
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks north", bronann, vt_map.MapMode.NORTH)
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH)

    vt_map.LookAtSpriteEvent.Create("Carson looks at Bronann", carson, bronann)

    -- Quest events

    -- End quest 1 (Barley meal retrieval) and prepare map for what's next.
    event = vt_map.ScriptedEvent.Create("Quest1: end and transition to after-dinner", "Quest1Done", "");
    event:AddEventLinkAtEnd("Quest1: Terminate mother and father events");

    event = vt_map.ScriptedEvent.Create("Quest1: Terminate mother and father events", "TerminateMotherAndFatherEvents", "");
    event:AddEventLinkAtEnd("Fade out to after dinner");

    event = vt_map.ScriptedEvent.Create("Fade out to after dinner", "FadeOutToAfterDinner", "CheckFadeInOrOut");
    event:AddEventLinkAtEnd("Fade in to after dinner");

    event = vt_map.ScriptedEvent.Create("Fade in to after dinner", "FadeInToAfterDinner", "CheckFadeInOrOut");
    event:AddEventLinkAtEnd("Bronann is told the truth - part 1");
    event:AddEventLinkAtEnd("Carson looks west");

    -- Quest 2 start: Bronann is told the truth
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Thanks for helping me out with the dishes.")
    dialogue:AddLine(text, malta)
    text = vt_system.Translate("Say, mom? Why is the village entrance blocked?")
    dialogue:AddLineEventEmote(text, bronann, "", "Bronann looks at both parents", "thinking dots")
    text = vt_system.Translate("...")
    dialogue:AddLineEmote(text, malta, "sweat drop")
    event = vt_map.DialogueEvent.Create("Bronann is told the truth - part 1", dialogue)
    -- Make a pause here
    event:AddEventLinkAtEnd("Carson looks south to think")
    event:AddEventLinkAtEnd("Audio:FadeOutMusic()")

    vt_map.AnimateSpriteEvent.Create("Bronann looks at both parents", bronann, "searching", 1000)

    event = vt_map.ChangeDirectionSpriteEvent.Create("Carson looks south to think", carson, vt_map.MapMode.SOUTH)
    event:AddEventLinkAtEnd("Carson looks at Bronann for part 2", 2000)

    event = vt_map.LookAtSpriteEvent.Create("Carson looks at Bronann for part 2", carson, bronann)
    event:AddEventLinkAtEnd("Bronann is told the truth - part 2")

    dialogue = vt_map.SpriteDialogue.Create()
    text = vt_system.Translate("All of the village elders' nerves are on edge. There is something going on here! Why won't you tell me?")
    dialogue:AddLineEmote(text, bronann, "interrogation")
    text = vt_system.Translate("Bronann, there is something we, your mother and I, have to tell you.")
    dialogue:AddLine(text, carson)
    text = vt_system.Translate("Sorry son. In fact, we've been willing to tell you since quite a lot of time now.")
    dialogue:AddLineEmote(text, carson, "thinking dots")
    -- tr: sentence is cut on purpose
    text = vt_system.Translate("We chose to live in this village for a reason, we ")
    dialogue:AddLineEvent(text, malta, "Malta looks at Bronann", "")
    event = vt_map.DialogueEvent.Create("Bronann is told the truth - part 2", dialogue)
    event:AddEventLinkAtEnd("Bright light")
    event:AddEventLinkAtEnd("Bronann looks east", 500)
    event:AddEventLinkAtEnd("Carson looks north", 600)
    event:AddEventLinkAtEnd("Malta looks west", 800)
    event:AddEventLinkAtEnd("Bronann looks west", 1000)
    event:AddEventLinkAtEnd("Malta looks east", 1200)
    event:AddEventLinkAtEnd("Carson looks west", 1300)
    event:AddEventLinkAtEnd("Bronann looks south", 2900)
    event:AddEventLinkAtEnd("Malta looks at Bronann", 3100)
    event:AddEventLinkAtEnd("Carson looks at Bronann", 3200)

    event = vt_map.ChangeDirectionSpriteEvent.Create("Malta looks at Bronann", malta, vt_map.MapMode.NORTH)

    event = vt_map.ScriptedEvent.Create("Bright light", "BrightLightStart", "BrightLightUpdate")
    event:AddEventLinkAtEnd("Bronann wonders what was that", 500)
    event:AddEventLinkAtEnd("Bronann looks at both parents")

    dialogue = vt_map.SpriteDialogue.Create()
    text = vt_system.Translate("Huh? What was that light?")
    dialogue:AddLineEmote(text, bronann, "interrogation")
    text = vt_system.Translate("Bronann! Wait!")
    dialogue:AddLine(text, carson)
    event = vt_map.DialogueEvent.Create("Bronann wonders what was that", dialogue)
    event:AddEventLinkAtEnd("Bronann runs out of the house")
    event:AddEventLinkAtEnd("Carson goes after Bronann", 200)

    -- Make Bronann leave house
    event = vt_map.PathMoveSpriteEvent.Create("Bronann runs out of the house", bronann, 40, 24.5, true)
    event:AddEventLinkAtEnd("Bronann disappears after running out of the house")

    vt_map.PathMoveSpriteEvent.Create("Carson goes after Bronann", carson, 40, 22, false)

    event = vt_map.ScriptedSpriteEvent.Create("Bronann disappears after running out of the house", bronann, "MakeInvisible", "")
    event:AddEventLinkAtEnd("Map_PopState")
    event:AddEventLinkAtEnd("Set Meal Event done")
    event:AddEventLinkAtEnd("to village")

    vt_map.ScriptedEvent.Create("Set Meal Event done", "SetMealEventDone", "")
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
            AudioManager:PlaySound("data/sounds/door_open2.wav");
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

function _UpdateCarsonDialogue()
    local dialogue = nil
    local text = nil
    local event = nil

    carson:ClearDialogueReferences()

    if (GlobalManager:DoesEventExist("story", "well_rats_beaten") == true) then
        dialogue = vt_map.SpriteDialogue.Create()
        text = vt_system.Translate("Nice job, son.")
        dialogue:AddLine(text, carson)
        carson:AddDialogueReference(dialogue)
        return;
    elseif (GlobalManager:DoesEventExist("story", "Carson_wooden_sword_given") == true) then
        dialogue = vt_map.SpriteDialogue.Create()
        text = vt_system.Translate("Please have go and see Olivia. She will explain everything to you.")
        dialogue:AddLine(text, carson)
        carson:AddDialogueReference(dialogue)
        return;
    else
        -- Show sword event
        -- First dialogue - speaks of Olivia's quest
        dialogue = vt_map.SpriteDialogue.Create()
        text = vt_system.Translate("Hey son! Did you sleep well? Hmm, now where did I leave that oil lamp?")
        dialogue:AddLine(text, carson);
        text = vt_system.Translate("Hi Dad! Um, I don't know. Sorry.")
        dialogue:AddLineEmote(text, bronann, "thinking dots")
        text = vt_system.Translate("Nah, no problem, I'll find it somewhere, eventually.")
        dialogue:AddLine(text, carson)
        text = vt_system.Translate("By the way, I have great news for you, son.")
        dialogue:AddLine(text, carson)
        text = vt_system.Translate("Something you've been waiting for. Come here I'll show you.")
        dialogue:AddLine(text, carson)
        dialogue:SetEventAtDialogueEnd("Show sword start event")
        carson:AddDialogueReference(dialogue)

        event = vt_map.ScriptedEvent.Create("Show sword start event", "StartSwordShowScene", "")
        event:AddEventLinkAtEnd("Bronann goes in front of Carson")
        event:AddEventLinkAtEnd("Carson goes in front of Bronann")

        event = vt_map.PathMoveSpriteEvent.Create("Carson goes in front of Bronann", carson, 34, 12, false)
        event:AddEventLinkAtEnd("Carson looks at Bronann 2")
        vt_map.ChangeDirectionSpriteEvent.Create("Carson looks at Bronann 2", carson, vt_map.MapMode.EAST)

        event = vt_map.PathMoveSpriteEvent.Create("Bronann goes in front of Carson", bronann, 36, 12, false)
        event:AddEventLinkAtEnd("Bronann looks at Carson")

        event = vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks at Carson", bronann, vt_map.MapMode.WEST)
        event:AddEventLinkAtEnd("Carson gives sword to Bronann dialogue")

        dialogue = vt_map.SpriteDialogue.Create()
        text = vt_system.Translate("Today, you're going to help Olivia with this.")
        dialogue:AddLineEvent(text, carson, "", "Show sword event")
        vt_map.DialogueEvent.Create("Carson gives sword to Bronann dialogue", dialogue)

        -- Show Wooden Sword
        event = vt_map.ScriptedEvent.Create("Show sword event", "ShowSwordEvent", "ShowSwordEventUpdate")
        event:AddEventLinkAtEnd("Carson gives sword part 2")

        dialogue = vt_map.SpriteDialogue.Create()
        text = vt_system.Translate("Wow, your practice sword. Many thanks Dad.")
        dialogue:AddLineEmote(text, bronann, "exclamation")
        text = vt_system.Translate("Your mother and I believe you've now grown enough to start dealing with the critters around the village.")
        dialogue:AddLineEvent(text, carson, "", "Carson hide sword event")
        vt_map.DialogueEvent.Create("Carson gives sword part 2", dialogue)

        -- Hide Wooden Sword
        event = vt_map.ScriptedEvent.Create("Carson hide sword event", "HideSwordEvent", "")
        event:AddEventLinkAtEnd("Add the wooden sword in inventory")

        event = vt_map.TreasureEvent.Create("Add the wooden sword in inventory");
        event:AddItem(10001, 1); -- The wooden sword item
        event:AddEventLinkAtEnd("Carson gives sword part 3");

        dialogue = vt_map.SpriteDialogue.Create()
        text = vt_system.Translate("Go see Olivia. She will explain everything to you.")
        dialogue:AddLineEvent(text, carson, "", "End Sword Giving scene event")
        vt_map.DialogueEvent.Create("Carson gives sword part 3", dialogue)

        event = vt_map.ScriptedEvent.Create("End Sword Giving scene event", "EndSwordShowEvent", "")
    end
end

function _UpdateMotherDialogue()
    local dialogue = nil
    local text = nil
    local event = nil

    malta:ClearDialogueReferences();

    if (GlobalManager:DoesEventExist("story", "Malta_Items_given") == true) then
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Bronann, promise me that you'll be careful, ok?");
        dialogue:AddLine(text, malta);
        malta:AddDialogueReference(dialogue);
        return;
    end
    if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == true) then
        -- Bronann'mother gives some items to Bronann.
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("So, finally you're going away.");
        dialogue:AddLineEmote(text, malta, "thinking dots");
        text = vt_system.Translate("Please, take this. It's not much but I hope it will help.");
        dialogue:AddLineEvent(text, malta, "", "Prepare item giving scene");
        malta:AddDialogueReference(dialogue);

        event = vt_map.ScriptedEvent.Create("Prepare item giving scene", "StartItemGivingScene", "");
        event:AddEventLinkAtEnd("Malta gives items")

        event = vt_map.TreasureEvent.Create("Malta gives items");
        event:AddItem(16, 1) -- Candy
        event:AddItem(11, 2) -- Tiny Moon Juice
        event:AddItem(15, 1) -- Lotus Petal
        event:AddEventLinkAtEnd("Bronann says thanks")

        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Thanks, mom.");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Take care, my son.");
        dialogue:AddLine(text, malta);
        event = vt_map.DialogueEvent.Create("Bronann says thanks", dialogue)
        event:AddEventLinkAtEnd("Update Malta's dialogue after giving items");

        event = vt_map.ScriptedEvent.Create("Update Malta's dialogue after giving items", "EndItemGivingScene", "");
        return;
    end
    if (GlobalManager:DoesEventExist("story", "quest1_barley_meal_done") == true
            and GlobalManager:DoesEventExist("story", "well_rats_beaten") == true) then
        -- Got some barley meal, Mom!
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("(sigh)... Got it, mom!");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Perfect timing, let's have dinner.");
        dialogue:AddLineEvent(text, malta, "", "Quest1: end and transition to after-dinner");
        malta:AddDialogueReference(dialogue);
    elseif (GlobalManager:DoesEventExist("story", "quest1_barley_meal_done") == true
            and GlobalManager:DoesEventExist("story", "well_rats_beaten") == false) then
        -- Still dad's quest to do
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("(sigh)... Got it, mom!");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Perfect timing, I'll cook dinner. Have you helped your father yet?");
        dialogue:AddLine(text, malta);
        malta:AddDialogueReference(dialogue);
    elseif (GlobalManager:DoesEventExist("bronanns_home", "quest1_mother_start_dialogue_done") == true) then
        -- 1st quest dialogue
        dialogue = vt_map.SpriteDialogue.Create("ep1_bronann_home_talk_about_barley_meal");
        text = vt_system.Translate("Could you go and buy some barley meal for the three of us?");
        dialogue:AddLine(text, malta);
        malta:AddDialogueReference(dialogue);
    elseif (GlobalManager:DoesEventExist("bronanns_home", "quest1_mother_start_dialogue_done") == false) then
        -- Begining dialogue
        dialogue = vt_map.SpriteDialogue.Create("ep1_bronann_home_talk_with_mother1");
        text = vt_system.Translate("Hi son, did you have a nightmare again last night?");
        dialogue:AddLine(text, malta);
        text = vt_system.Translate("Hi mom. Huh, how did you know?");
        dialogue:AddLineEmote(text, bronann, "interrogation");
        text = vt_system.Translate("Eh eh? Have you already forgotten that I'm your mother?");
        dialogue:AddLine(text, malta);
        malta:AddDialogueReference(dialogue);
    else
        -- Last default dialogue
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Don't venture too far, I'll need your help soon!");
        dialogue:AddLine(text, malta);
        malta:AddDialogueReference(dialogue);
    end
end

-- Handle sword location
local wooden_sword_x_offset_start = 0.5
local wooden_sword_x_offset = 0

-- Light event
local bright_light_time = 0.0
local bright_light_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

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
        carson:SetMoving(false);
        carson:ClearDialogueReferences();
        EventManager:EndAllEvents(carson);
        malta:SetMoving(false);
        malta:ClearDialogueReferences();
        EventManager:EndAllEvents(malta);

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

        malta:SetPosition(40, 19);
        malta:SetDirection(vt_map.MapMode.EAST);
        carson:SetPosition(48, 19);
        carson:SetDirection(vt_map.MapMode.WEST);

        -- Remove the barley meal key item from inventory
        local barley_meal_item_id = 70002;
        local inventory_handler = GlobalManager:GetInventoryHandler()
        if (inventory_handler:IsItemInInventory(barley_meal_item_id) == true) then
            inventory_handler:RemoveFromInventory(barley_meal_item_id);
        end

        -- Set the quest 2 as started
        GlobalManager:SetEventValue("story", "Quest2_started", 1)
        -- Make the food and dishes not appear anymore, once the dinner is done.
        _UpdateDishesAndFood();

        VideoManager:FadeIn(1000);

        -- Flag used to disable the warp zone temporarily
        quest2_start_scene = true;
    end,

    StartItemGivingScene = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        EventManager:EndAllEvents(malta);
        bronann:SetMoving(false);
    end,

    EndItemGivingScene = function()
        GlobalManager:SetEventValue("story", "Malta_Items_given", 1);
        _UpdateMotherDialogue()
        Map:PopState()
    end,

    StartSwordShowScene = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE)
        EventManager:EndAllEvents(carson)
        carson:SetMoving(false)
        bronann:SetMoving(false)
    end,

    ShowSwordEvent = function()
        -- Show sword
        wooden_sword:SetPosition(carson:GetXPosition() + wooden_sword_x_offset_start, carson:GetYPosition() - 1.0);
        wooden_sword_x_offset = wooden_sword_x_offset_start
        wooden_sword:SetVisible(true)
    end,

    ShowSwordEventUpdate = function()
        -- Move sword slowly to Bronann
        local update_x = SystemManager:GetUpdateTime() / 200.0
        wooden_sword_x_offset = wooden_sword_x_offset + update_x
        wooden_sword:SetPosition(carson:GetXPosition() + wooden_sword_x_offset, carson:GetYPosition() - 1.0);
        if (wooden_sword_x_offset < 1.0) then
            return false
        else
            return true
        end
    end,

    HideSwordEvent = function()
        -- Hide sword
        wooden_sword:SetVisible(false);
        wooden_sword:SetPosition(0.0, 0.0);
    end,

    EndSwordShowEvent = function()
        GlobalManager:SetEventValue("story", "Carson_wooden_sword_given", 1)
        _UpdateCarsonDialogue()
        Map:PopState()
        EventManager:StartEvent("Dad random move")
    end,

    BrightLightStart = function()
        bright_light_time = 0.0;
        Effects:ShakeScreen(0.6, 6000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);
        AudioManager:PlaySound("data/sounds/rumble.wav");
        AudioManager:FadeOutActiveMusic(2000);
    end,

    BrightLightUpdate = function()
        bright_light_time = bright_light_time + 2.5 * SystemManager:GetUpdateTime();

        if (bright_light_time < 5000.0) then
            bright_light_color:SetAlpha(bright_light_time / 5000.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(bright_light_color);
            return false;
        end

        if (bright_light_time < 10000) then
            bright_light_color:SetAlpha((10000.0 - bright_light_time) / 5000.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(bright_light_color);
            return false;
        end

        -- end of the two-step fade in and out
        return true;
    end,

    MakeInvisible = function(sprite)
        if (sprite ~= nil) then
            sprite:SetVisible(false);
            sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end,

    SetMealEventDone = function()
        GlobalManager:SetEventValue("story", "meal_event_done", 1)
    end
}
