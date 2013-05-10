-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_kalya_house_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = "Basement"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};
local Effects = {};

-- the main character handler
local bronann = {};
local kalya = {};
local orlinn = {};

-- A moveable wall hiding the exit
local fake_wall = {};
local exit_light = {};

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;
    Effects = Map:GetEffectSupervisor();

    Map.unlimited_stamina = true;

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on bronann
    Map:SetCamera(bronann);

    _CreateEvents();
    _CreateZones();

    -- To be continued script
    Map:GetScriptSupervisor():AddScript("dat/maps/to_be_continued_anim.lua");

    -- If the scene hasn't happened, start it
    if (GlobalManager:GetEventValue("story", "kalya_basement_scene") == 0) then
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        EventManager:StartEvent("Orlinn goes out of the stairs");
    else
        -- Set the sad music
        EventManager:StartEvent("Sad music start", 100);
    end
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    bronann = CreateSprite(Map, "Bronann", 40, 21.5);
    bronann:SetVisible(false);
    bronann:SetDirection(vt_map.MapMode.NORTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    Map:AddGroundObject(bronann);

    kalya = CreateSprite(Map, "Kalya", 40, 21.5);
    kalya:SetDirection(vt_map.MapMode.NORTH);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetVisible(false);
    Map:AddGroundObject(kalya);

    orlinn = CreateSprite(Map, "Orlinn", 40, 21.5);
    orlinn:SetDirection(vt_map.MapMode.NORTH);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    Map:AddGroundObject(orlinn);

    -- Create the fake wall  out of the object catalog as it is used once,
    fake_wall = vt_map.PhysicalObject();
    fake_wall:SetPosition(32.0, 14.0);
    fake_wall:SetObjectID(Map.object_supervisor:GenerateObjectID());
    fake_wall:SetCollHalfWidth(4.0);
    fake_wall:SetCollHeight(10.0);
    fake_wall:SetImgHalfWidth(4.0);
    fake_wall:SetImgHeight(10.0);
    fake_wall:AddStillFrame("dat/maps/layna_village/kalya_house_fake_wall.png");
    Map:AddGroundObject(fake_wall);

    exit_light = CreateObject(Map, "Left Window Light", 34, 2);
    exit_light:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    exit_light:SetVisible(false);
    Map:AddGroundObject(exit_light);

    -- Place the characters according to the scene needs
    if (GlobalManager:GetEventValue("story", "kalya_basement_scene") == 1) then
        bronann:SetVisible(true);

        kalya:SetVisible(true);
        kalya:SetPosition(28.0, 15.1);
        kalya:SetDirection(vt_map.MapMode.SOUTH);

        orlinn:SetVisible(true);
        orlinn:SetPosition(28.0, 17.0);
        orlinn:SetDirection(vt_map.MapMode.EAST);

        fake_wall:SetVisible(false);
        fake_wall:SetPosition(0, 0);
        exit_light:SetVisible(true);
    else
        return;
    end

    -- Load potential previous save point data
    local x_position = GlobalManager:GetSaveLocationX();
    local y_position = GlobalManager:GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Use the save point position, and clear the save position data for next maps
        GlobalManager:UnsetSaveLocation();
        -- Make the character look at us in that case
        bronann:SetDirection(vt_map.MapMode.SOUTH);
        bronann:SetPosition(x_position, y_position);
    end
end

function _CreateObjects()
    object = {}

    Map:AddSavePoint(29.5, 29.0);
    object = CreateObject(Map, "Barrel1", 25, 19);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Box1", 23, 20);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Box1", 22, 22);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Box1", 23, 24);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Box1", 23.5, 27);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Box1", 21, 19);
    Map:AddGroundObject(object);

    object = CreateObject(Map, "Small Wooden Table", 22, 17);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Barrel1", 21, 36);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Barrel1", 22, 37);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Barrel1", 24, 33);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Barrel1", 27, 34);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Barrel1", 29, 33);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Box1", 28, 21);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Chests
    local chest = CreateTreasure(Map, "kalya_house_basement_chest1", "Wood_Chest1", 25, 22);
    if (chest ~= nil) then
        chest:AddObject(10011, 1); -- 1 Iron Sword
        Map:AddGroundObject(chest);
    end

    chest = CreateTreasure(Map, "kalya_house_basement_chest2", "Wood_Chest1", 21, 28);
    if (chest ~= nil) then
        chest:AddObject(1003, 1); -- 1 Elixir
        Map:AddGroundObject(chest);
    end

    chest = CreateTreasure(Map, "kalya_house_basement_chest3", "Wood_Chest1", 24, 35);
    if (chest ~= nil) then
        chest:AddObject(3001, 1); -- 1 Copper Ore
        Map:AddGroundObject(chest);
    end
end


-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- Triggered events
    event = vt_map.MapTransitionEvent("to Mt Elbrus", "dat/maps/mt_elbrus/mt_elbrus_low_part1_map.lua",
                                      "dat/maps/mt_elbrus/mt_elbrus_low_part1_script.lua", "from_kalya_house_basement");
    EventManager:RegisterEvent(event);

    -- Generic events
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks east", orlinn, vt_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks east", kalya, vt_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks south", kalya, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks west", kalya, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks north", bronann, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Kalya looks at Orlinn", kalya, orlinn);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Bronann looks at Orlinn", bronann, orlinn);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Kalya looks at Bronann", kalya, bronann);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Bronann looks at Kalya", bronann, kalya);
    EventManager:RegisterEvent(event);

    -- The scene before going outside
    event = vt_map.PathMoveSpriteEvent("Orlinn goes out of the stairs", orlinn, 28.0, 18.0, false);
    event:AddEventLinkAtEnd("Orlinn looks east");
    event:AddEventLinkAtEnd("Make Kalya visible", 300);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Make Kalya visible", "make_kalya_visible", "");
    event:AddEventLinkAtEnd("Kalya goes out of the stairs");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Kalya goes out of the stairs", kalya, 30.3, 20.0, false);
    event:AddEventLinkAtEnd("Kalya looks east");
    event:AddEventLinkAtEnd("Make Bronann visible");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Make Bronann visible", "make_bronann_visible", "");
    event:AddEventLinkAtEnd("Bronann goes in front of Kalya");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Bronann goes in front of Kalya", bronann, 35.0, 20.0, false);
    event:AddEventLinkAtEnd("Dialogue part 1");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Bronann, how do you feel?");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Better... Somehow, the pain faded away the second I entered here...");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("By the way, where are we?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    text = vt_system.Translate("In our basement. The other stairs there are leading to my room. This house is an ancient sanctuary, so it might explain you're feeling better...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks south", "Kalya looks east");
    text = vt_system.Translate("Orlinn. Go get the packings. We're leaving...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Orlinn", "");
    text = vt_system.Translate("... Ok...");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Dialogue part 1", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes upstairs");
    event:AddEventLinkAtEnd("Kalya looks at Orlinn", 500);
    event:AddEventLinkAtEnd("Kalya looks at Orlinn", 800);
    event:AddEventLinkAtEnd("Bronann looks at Orlinn", 800);
    event:AddEventLinkAtEnd("Kalya looks south", 1500);
    event:AddEventLinkAtEnd("Bronann looks at Orlinn", 1500);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Orlinn goes upstairs", orlinn, 40.0, 29.5, false);
    event:AddEventLinkAtEnd("Make Orlinn invisible");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Make Orlinn invisible", "make_orlinn_invisible", "");
    event:AddEventLinkAtEnd("Dialogue part 2");
    EventManager:RegisterEvent(event);

    event = vt_map.AnimateSpriteEvent("Bronann looks angry", bronann, "hero_stance", 999999);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Stop Bronann looks angry", "terminate_bronann_events", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Sad music start", "sad_music_start", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("FadeOutAllMusic", "fade_out_music", "");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("We're leaving the village now...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("I feel dizzy... So much things happened in so little time...");
    dialogue:AddLineEventEmote(text, bronann, "Bronann looks at Kalya", "", "sweat drop");
    text = vt_system.Translate("The soldiers might be here within minutes. We'll leave to the north and cross the north-west plain up to...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("No.");
    dialogue:AddLineEvent(text, bronann, "FadeOutAllMusic", "");
    text = vt_system.Translate("No?!");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at Bronann", "", "exclamation");
    text = vt_system.Translate("No, I won't follow you anymore...");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Look, I...");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("Enough!");
    dialogue:AddLineEventEmote(text, bronann, "Bronann looks angry", "", "exclamation");
    text = vt_system.Translate("Nobody told me what was happening. Herth and the Lord seemed to know each other... And now you...");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("You seem so calm, ready and prepared...");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Bronann, I ...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Enough! You hid things to me. You're a liar, like everyone else!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("Bronann, you'll get everything explained in time, I...");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("No! I want the truth now, or I won't come!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("... Fine.");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks south", "", "sweat drop");
    text = vt_system.Translate("...");
    dialogue:AddLineEvent(text, bronann, "", "Sad music start");
    text = vt_system.Translate("I knew the crystal would appear...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks west", "");
    text = vt_system.Translate("What?");
    dialogue:AddLineEventEmote(text, bronann, "Stop Bronann looks angry", "", "exclamation");
    text = vt_system.Translate("But I didn't know who it would choose.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks east", "");
    text = vt_system.Translate("Herth prepared me... All my entire life... For this very day...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("He trained me, taught me magic, and told me the crystal would appear one day...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("He did think I would be the one. But it seems he was wrong, and that now my duty is to look after you...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks west", "");
    text = vt_system.Translate("You must believe me Bronann...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks south", "");
    text = vt_system.Translate("The crystal chose me? But I'm just...");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    text = vt_system.Translate("... someone brave enough to jump between my brother and it...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks east", "");
    text = vt_system.Translate("It should have been me... But I couldn't move. And now, I have to fix my mistake...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks east", "");
    text = vt_system.Translate("How?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    text = vt_system.Translate("I...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Sis!!");
    dialogue:AddLine(text, orlinn);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Dialogue part 2", dialogue);
    event:AddEventLinkAtEnd("Make Orlinn visible");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Make Orlinn visible", "make_orlinn_visible", "");
    event:AddEventLinkAtEnd("Orlinn comes back to them");
    event:AddEventLinkAtEnd("Kalya looks at Orlinn");
    event:AddEventLinkAtEnd("Bronann looks at Orlinn");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Orlinn comes back to them", orlinn, 33.0, 23.0, true);
    event:AddEventLinkAtEnd("Kalya looks at Orlinn");
    event:AddEventLinkAtEnd("Bronann looks at Orlinn");
    event:AddEventLinkAtEnd("Dialogue part 3");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Sis!! They're in front of the house!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Quick, we must go now!!");
    dialogue:AddLineEventEmote(text, kalya, "Bronann looks at Kalya", "", "exclamation");
    text = vt_system.Translate("We won't be able to come back here once out. So, feel free to pick up everything needed and prepare yourself...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "");
    text = vt_system.Translate("Wait! There is one more thing... Why are you calling your own dad by his actual name?");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("That's because he's not our father...");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Our parents have been killed a long time ago...");
    dialogue:AddLineEventEmote(text, orlinn, "Bronann looks at Orlinn", "", "sweat drop");
    text = vt_system.Translate("Orlinn...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Orlinn", "");
    text = vt_system.Translate("Don't worry, sis. It's fine, we can tell him.");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("I also left my parents behind...");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    text = vt_system.Translate("... They'll be fine, Bronann. We must prevent Banesore from obtaining the crystal.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "");
    text = vt_system.Translate("Anyway, let's go now...");
    dialogue:AddLine(text, kalya);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Dialogue part 3", dialogue);
    event:AddEventLinkAtEnd("Kalya goes triggering the secret path");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Click sound", "click_sound", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Wall sound", "wall_sound", "");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Kalya goes triggering the secret path", kalya, 28.0, 15.1, false);
    event:AddEventLinkAtEnd("Bronann looks north");
    event:AddEventLinkAtEnd("Click sound");
    event:AddEventLinkAtEnd("Wall sound", 700);
    event:AddEventLinkAtEnd("Kalya triggers the secret path", 1000);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Kalya triggers the secret path", "kalya_triggers_the_secret_path", "");
    event:AddEventLinkAtEnd("Kalya looks south", 300);
    event:AddEventLinkAtEnd("Orlinn goes near his sister", 700);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Orlinn goes near his sister", orlinn, 28.0, 17.0, false);
    event:AddEventLinkAtEnd("Orlinn looks east");
    event:AddEventLinkAtEnd("Dialogue part 4");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Wow!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("It's time...");
    dialogue:AddLine(text, kalya);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Dialogue part 4", dialogue);
    event:AddEventLinkAtEnd("End of basement dialogue");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("End of basement dialogue", "end_of_basement_dialogue", "");
    EventManager:RegisterEvent(event);

    -- Can't go downstairs
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("We can't go back now...");
    dialogue:AddLine(text, bronann);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Can't go downstairs", dialogue);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("It's my room upstairs. Don't even think about it!");
    dialogue:AddLine(text, kalya);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Can't go upstairs", dialogue);
    EventManager:RegisterEvent(event);

    -- Dialogues for after the scene
    dialogue = vt_map.SpriteDialogue("ep1_layna_village_kalya_tells_its_time");
    text = vt_system.Translate("It's time...");
    dialogue:AddLine(text, kalya);
    DialogueManager:AddDialogue(dialogue);
    kalya:AddDialogueReference(dialogue);

    dialogue = vt_map.SpriteDialogue("ep1_layna_village_orlinn_is_sad");
    text = vt_system.Translate("...");
    dialogue:AddLine(text, orlinn);
    DialogueManager:AddDialogue(dialogue);
    orlinn:AddDialogueReference(dialogue);

    -- NOTE temp event until what's next is done
    event = vt_map.ScriptedEvent("to be continued", "to_be_continued", "");
    EventManager:RegisterEvent(event);
end

-- zones
local downstairs_zone = {};
local upstairs_zone = {};
local to_mt_elbrus_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    downstairs_zone = vt_map.CameraZone(38, 42, 20, 22);
    Map:AddZone(downstairs_zone);

    upstairs_zone = vt_map.CameraZone(38, 42, 28, 30);
    Map:AddZone(upstairs_zone);

    to_mt_elbrus_zone = vt_map.CameraZone(30, 36, 12, 14);
    Map:AddZone(to_mt_elbrus_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (Map:CurrentState() ~= vt_map.MapMode.STATE_EXPLORE) then
        return;
    end

    if (downstairs_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("Can't go downstairs");
    elseif (upstairs_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("Can't go upstairs");
    elseif (to_mt_elbrus_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        --EventManager:StartEvent("to Mt Elbrus");
        EventManager:StartEvent("to be continued");
    end
end


-- Map Custom functions
-- Used through scripted events
map_functions = {
    make_kalya_visible =  function()
        kalya:SetVisible(true);
    end,

    make_bronann_visible =  function()
        bronann:SetVisible(true);
    end,

    make_orlinn_invisible =  function()
        orlinn:SetVisible(false);
    end,

    fade_out_music = function()
        AudioManager:FadeOutAllMusic(1000);
    end,

    sad_music_start = function()
        AudioManager:PlayMusic("mus/sad_moment.ogg");
    end,

    terminate_bronann_events = function()
        EventManager:TerminateAllEvents(bronann);
    end,

    make_orlinn_visible = function()
        orlinn:SetVisible(true);
    end,

    click_sound = function()
        AudioManager:PlaySound("snd/menu_click_01.wav");
    end,

    wall_sound = function()
        AudioManager:PlaySound("snd/cave-in.ogg");
    end,

    kalya_triggers_the_secret_path =  function()
        Effects:ShakeScreen(0.6, 1000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);
        fake_wall:SetVisible(false);
        fake_wall:SetPosition(0, 0);
        exit_light:SetVisible(true);
    end,

    end_of_basement_dialogue = function()
        Map:PopState();
        GlobalManager:SetEventValue("story", "kalya_basement_scene", 1);
    end,

    to_be_continued = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        bronann:SetMoving(false);
        GlobalManager:SetEventValue("game", "to_be_continued", 1);
    end
}
