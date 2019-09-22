-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_kalya_house_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "data/story/common/locations/mountain_village.png"
map_subname = "Basement"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Effects = nil

-- the main character handler
local bronann = nil
local kalya = nil
local orlinn = nil

-- A moveable wall hiding the exit
local fake_wall = nil
local exit_light = nil

-- the main map loading code
function Load(m)

    Map = m;
    Effects = Map:GetEffectSupervisor();
    EventManager = Map:GetEventSupervisor();

    Map:SetUnlimitedStamina(true);

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on bronann
    Map:SetCamera(bronann);

    _CreateEvents();
    _CreateZones();

    -- Preloads the sad music
    AudioManager:LoadMusic("data/music/sad_moment.ogg", Map);

    -- If the scene hasn't happened, start it
    if (GlobalManager:GetGameEvents():GetEventValue("story", "kalya_basement_scene") == 0) then
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
    bronann = CreateSprite(Map, "Bronann", 40, 21.5, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetVisible(false);
    bronann:SetDirection(vt_map.MapMode.NORTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    kalya = CreateSprite(Map, "Kalya", 40, 21.5, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.NORTH);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetVisible(false);

    orlinn = CreateSprite(Map, "Orlinn", 40, 21.5, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.NORTH);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Create the fake wall  out of the object catalog as it is used once,
    fake_wall = vt_map.PhysicalObject.Create(vt_map.MapMode.GROUND_OBJECT);
    fake_wall:SetPosition(32.0, 14.0);
    fake_wall:SetCollPixelHalfWidth(4.0 * 16);
    fake_wall:SetCollPixelHeight(10.0 * 16);
    fake_wall:SetImgPixelHalfWidth(4.0 * 16);
    fake_wall:SetImgPixelHeight(10.0 * 16);
    fake_wall:AddStillFrame("data/story/ep1/layna_village/kalya_house_fake_wall.png");

    exit_light = CreateObject(Map, "Left Window Light", 34, 2, vt_map.MapMode.GROUND_OBJECT);
    exit_light:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    exit_light:SetVisible(false);

    -- Place the characters according to the scene needs
    if (GlobalManager:GetGameEvents():GetEventValue("story", "kalya_basement_scene") == 1) then
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
    local x_position = GlobalManager:GetMapData():GetSaveLocationX();
    local y_position = GlobalManager:GetMapData():GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Make the character look at us in that case
        bronann:SetDirection(vt_map.MapMode.SOUTH);
        bronann:SetPosition(x_position, y_position);
    end
end

function _CreateObjects()
    vt_map.SavePoint.Create(29.5, 29.0);

    CreateObject(Map, "Barrel1", 25, 19, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Box1", 23, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Box1", 22, 22, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Box1", 23, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Box1", 23.5, 27, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Box1", 21, 19, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Small Wooden Table", 22, 17, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 21, 36, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 22, 37, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 24, 33, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 27, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 29, 33, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Box1", 28, 21, vt_map.MapMode.GROUND_OBJECT);

    -- Chests
    local chest = CreateTreasure(Map, "kalya_house_basement_chest1", "Wood_Chest1", 25, 22, vt_map.MapMode.GROUND_OBJECT);
    chest:AddItem(50002, 1); -- Sturdy boots

    chest = CreateTreasure(Map, "kalya_house_basement_chest2", "Wood_Chest1", 21, 28, vt_map.MapMode.GROUND_OBJECT);
    chest:AddItem(1003, 1); -- 1 Elixir

    chest = CreateTreasure(Map, "kalya_house_basement_chest3", "Wood_Chest1", 24, 35, vt_map.MapMode.GROUND_OBJECT);
    chest:AddItem(3001, 1); -- 1 Copper Ore
end


-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Triggered events
    vt_map.MapTransitionEvent.Create("to Mt Elbrus", "data/story/ep1/mt_elbrus/mt_elbrus_path1_map.lua",
                                    "data/story/ep1/mt_elbrus/mt_elbrus_path1_script.lua", "from_kalya_house_basement");

    -- Generic events
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks east", orlinn, vt_map.MapMode.EAST);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks east", kalya, vt_map.MapMode.EAST);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks south", kalya, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks west", kalya, vt_map.MapMode.WEST);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks north", bronann, vt_map.MapMode.NORTH);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Orlinn", kalya, orlinn);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Orlinn", bronann, orlinn);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Kalya", bronann, kalya);

    -- The scene before going outside
    event = vt_map.PathMoveSpriteEvent.Create("Orlinn goes out of the stairs", orlinn, 28.0, 18.0, false);
    event:AddEventLinkAtEnd("Orlinn looks east");
    event:AddEventLinkAtEnd("Make Kalya visible", 300);

    event = vt_map.ScriptedEvent.Create("Make Kalya visible", "make_kalya_visible", "");
    event:AddEventLinkAtEnd("Kalya goes out of the stairs");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya goes out of the stairs", kalya, 30.3, 20.0, false);
    event:AddEventLinkAtEnd("Kalya looks east");
    event:AddEventLinkAtEnd("Make Bronann visible");

    event = vt_map.ScriptedEvent.Create("Make Bronann visible", "make_bronann_visible", "");
    event:AddEventLinkAtEnd("Bronann goes in front of Kalya");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann goes in front of Kalya", bronann, 35.0, 20.0, false);
    event:AddEventLinkAtEnd("Dialogue part 1");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann, how do you feel?");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Better. Somehow, the pain faded away the second I entered here.");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("By the way, where are we?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    text = vt_system.Translate("In our basement. The other stairs there are leading to my room. This house is an ancient sanctuary, so it might explain why you're feeling better.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks south", "Kalya looks east");
    text = vt_system.Translate("Orlinn. Go get our packings. We're leaving.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Orlinn", "");
    text = vt_system.Translate("Ok.");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    event = vt_map.DialogueEvent.Create("Dialogue part 1", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes upstairs");
    event:AddEventLinkAtEnd("Kalya looks at Orlinn", 500);
    event:AddEventLinkAtEnd("Kalya looks at Orlinn", 800);
    event:AddEventLinkAtEnd("Bronann looks at Orlinn", 800);
    event:AddEventLinkAtEnd("Kalya looks south", 1500);
    event:AddEventLinkAtEnd("Bronann looks at Orlinn", 1500);

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn goes upstairs", orlinn, 40.0, 27.5, false);
    event:AddEventLinkAtEnd("Make Orlinn invisible");

    event = vt_map.ScriptedEvent.Create("Make Orlinn invisible", "make_orlinn_invisible", "");
    event:AddEventLinkAtEnd("Dialogue part 2");

    vt_map.AnimateSpriteEvent.Create("Bronann looks angry", bronann, "hero_stance", 0); -- 0 means forever

    vt_map.ScriptedEvent.Create("Stop Bronann looks angry", "terminate_bronann_events", "");

    vt_map.ScriptedEvent.Create("Sad music start", "sad_music_start", "");

    vt_map.ScriptedEvent.Create("FadeOutActiveMusic", "fade_out_music", "");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("We're leaving the village now.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("I feel dizzy. So many things have happened in such little time.");
    dialogue:AddLineEventEmote(text, bronann, "Bronann looks at Kalya", "", "sweat drop");
    text = vt_system.Translate("The soldiers will be here any minute. We'll leave to the north and cross the north-west plain up to...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("No.");
    dialogue:AddLineEvent(text, bronann, "FadeOutActiveMusic", "");
    text = vt_system.Translate("No?!");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at Bronann", "", "exclamation");
    text = vt_system.Translate("No, I won't follow you anymore.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Look, I...");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("Enough!");
    dialogue:AddLineEventEmote(text, bronann, "Bronann looks angry", "", "exclamation");
    text = vt_system.Translate("Nobody told me what was happening. Herth and the Lord seemed to know each other. And now you...");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("You seem so calm, ready, and prepared.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Bronann, I ...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Enough! You hid everything from me just like everyone else. You're a liar! I won't be manipulated anymore!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("Bronann, you'll get everything explained in time, I...");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("No! I won't go unless you tell me the truth now!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("... Fine.");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks south", "", "sweat drop");
    text = vt_system.Translate("...");
    dialogue:AddLineEvent(text, bronann, "", "Sad music start");
    text = vt_system.Translate("I knew the crystal would appear.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks west", "");
    text = vt_system.Translate("What?");
    dialogue:AddLineEventEmote(text, bronann, "Stop Bronann looks angry", "", "exclamation");
    text = vt_system.Translate("But I didn't know who it would choose.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks east", "");
    text = vt_system.Translate("Herth prepared me all my life for this very day.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("He trained me, taught me magic, and told me the crystal would appear one day.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("He thought that I would be the one to be chosen by the crystal. However, it seems he was wrong. Now, I am to look after you.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks west", "");
    text = vt_system.Translate("You must believe me Bronann.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks south", "");
    text = vt_system.Translate("The crystal chose me? But I'm just...");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    text = vt_system.Translate("Someone brave enough to jump between it and my brother.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks east", "");
    text = vt_system.Translate("It should have been me. But I couldn't move. And now, I have to fix my mistake.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks east", "");
    text = vt_system.Translate("How?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    text = vt_system.Translate("I...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Sis!");
    dialogue:AddLine(text, orlinn);
    event = vt_map.DialogueEvent.Create("Dialogue part 2", dialogue);
    event:AddEventLinkAtEnd("Make Orlinn visible");

    event = vt_map.ScriptedEvent.Create("Make Orlinn visible", "make_orlinn_visible", "");
    event:AddEventLinkAtEnd("Orlinn comes back to them");
    event:AddEventLinkAtEnd("Kalya looks at Orlinn");
    event:AddEventLinkAtEnd("Bronann looks at Orlinn");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn comes back to them", orlinn, 33.0, 23.0, true);
    event:AddEventLinkAtEnd("Kalya looks at Orlinn");
    event:AddEventLinkAtEnd("Bronann looks at Orlinn");
    event:AddEventLinkAtEnd("Dialogue part 3");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Sis! They're in front of the house!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Quick, we must go now!");
    dialogue:AddLineEventEmote(text, kalya, "Bronann looks at Kalya", "", "exclamation");
    text = vt_system.Translate("We won't be able to come back here once we leave. So feel free to pick up everything you need and prepare yourself.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "");
    text = vt_system.Translate("Wait! There is one more thing. Why are you calling your own father by his actual name?");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("That's because he's not our father.");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Our parents were killed a long time ago.");
    dialogue:AddLineEventEmote(text, orlinn, "Bronann looks at Orlinn", "", "sweat drop");
    text = vt_system.Translate("Orlinn...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Orlinn", "");
    text = vt_system.Translate("Don't worry, sis. It's fine, we can tell him.");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("I also left my parents behind.");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    text = vt_system.Translate("They'll be fine, Bronann. We must prevent Banesore from obtaining the crystal.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "");
    text = vt_system.Translate("Anyway, let's go now.");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Dialogue part 3", dialogue);
    event:AddEventLinkAtEnd("Kalya goes triggering the secret path");

    vt_map.ScriptedEvent.Create("Click sound", "click_sound", "");

    vt_map.ScriptedEvent.Create("Wall sound", "wall_sound", "");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya goes triggering the secret path", kalya, 28.0, 15.1, false);
    event:AddEventLinkAtEnd("Bronann looks north");
    event:AddEventLinkAtEnd("Click sound");
    event:AddEventLinkAtEnd("Wall sound", 700);
    event:AddEventLinkAtEnd("Kalya triggers the secret path", 1000);

    event = vt_map.ScriptedEvent.Create("Kalya triggers the secret path", "kalya_triggers_the_secret_path", "");
    event:AddEventLinkAtEnd("Kalya looks south", 300);
    event:AddEventLinkAtEnd("Orlinn goes near his sister", 700);

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn goes near his sister", orlinn, 28.0, 17.0, false);
    event:AddEventLinkAtEnd("Orlinn looks east");
    event:AddEventLinkAtEnd("Dialogue part 4");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Wow!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("It's time.");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Dialogue part 4", dialogue);
    event:AddEventLinkAtEnd("End of basement dialogue");

    vt_map.ScriptedEvent.Create("End of basement dialogue", "end_of_basement_dialogue", "");

    -- Can't go downstairs
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("We can't go back now.");
    dialogue:AddLine(text, bronann);
    vt_map.DialogueEvent.Create("Can't go downstairs", dialogue);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("It's my room upstairs. Don't even think about it!");
    dialogue:AddLine(text, kalya);
    vt_map.DialogueEvent.Create("Can't go upstairs", dialogue);

    -- Dialogues for after the scene
    dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_kalya_tells_its_time");
    text = vt_system.Translate("It's time.");
    dialogue:AddLine(text, kalya);
    kalya:AddDialogueReference(dialogue);

    dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_orlinn_is_sad");
    text = vt_system.Translate("...");
    dialogue:AddLine(text, orlinn);
    orlinn:AddDialogueReference(dialogue);
end

-- zones
local downstairs_zone = nil
local upstairs_zone = nil
local to_mt_elbrus_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    downstairs_zone = vt_map.CameraZone.Create(38, 42, 20, 22);
    upstairs_zone = vt_map.CameraZone.Create(38, 42, 26, 28);
    to_mt_elbrus_zone = vt_map.CameraZone.Create(30, 36, 12, 14);
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
        EventManager:StartEvent("to Mt Elbrus");
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
        AudioManager:FadeOutActiveMusic(1000);
    end,

    sad_music_start = function()
        AudioManager:PlayMusic("data/music/sad_moment.ogg");
    end,

    terminate_bronann_events = function()
        EventManager:EndAllEvents(bronann);
    end,

    make_orlinn_visible = function()
        orlinn:SetVisible(true);
    end,

    click_sound = function()
        AudioManager:PlaySound("data/sounds/menu_click_01.wav");
    end,

    wall_sound = function()
        AudioManager:PlaySound("data/sounds/cave-in.ogg");
    end,

    kalya_triggers_the_secret_path =  function()
        Effects:ShakeScreen(0.6, 1000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);
        fake_wall:SetVisible(false);
        fake_wall:SetPosition(0, 0);
        exit_light:SetVisible(true);
    end,

    end_of_basement_dialogue = function()
        Map:PopState();
        GlobalManager:GetGameEvents():SetEventValue("story", "kalya_basement_scene", 1);
        GlobalManager:GetGameQuests():AddQuestLog("flee_from_the_dark_soldiers");
    end,
}
