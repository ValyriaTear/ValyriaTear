-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_center_at_night_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "data/story/common/locations/mountain_village.png"
map_subname = "Village center"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/forest_at_night.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Effects = nil

local bronann = nil
local kalya = nil

-- The soldiers bringing Kalya to the riverbank
local soldier1 = nil
local soldier2 = nil
local soldier3 = nil
local soldier4 = nil

-- the main map loading code
function Load(m)

    Map = m;
    Effects = Map:GetEffectSupervisor();
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
    Effects:EnableAmbientOverlay("data/visuals/ambient/clouds.png", 5.0, -5.0, true);
    Map:GetScriptSupervisor():AddScript("data/story/common/at_night.lua");

    -- Set the world map current position
    GlobalManager:SetCurrentLocationId("layna village");

    -- Start the return to village dialogue if it hasn't been done already.
    if (GlobalManager:GetEventValue("story", "return_to_layna_village_dialogue_done") ~= 1) then
        EventManager:StartEvent("Return to village dialogue");
    end

    -- Preload the soldier music
    AudioManager:LoadMusic("data/music/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg", Map);
end

function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    bronann = CreateSprite(Map, "Bronann", 12, 63, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.SOUTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- set up the position according to the previous map
    if (GlobalManager:GetPreviousLocation() == "from_riverbank") then
        bronann:SetPosition(30, 77);
        bronann:SetDirection(vt_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_village_south") then
        bronann:SetPosition(79, 77);
        bronann:SetDirection(vt_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_kalya_house_path") then
        bronann:SetPosition(3, 11);
        bronann:SetDirection(vt_map.MapMode.EAST);
    elseif (GlobalManager:GetPreviousLocation() == "from_shop") then
        bronann:SetPosition(94, 72);
        bronann:SetDirection(vt_map.MapMode.SOUTH);
        AudioManager:PlaySound("data/sounds/door_close.wav");
    elseif (GlobalManager:GetPreviousLocation() == "from_secret_path") then
        bronann:SetPosition(3, 60);
        bronann:SetDirection(vt_map.MapMode.EAST);
    elseif (GlobalManager:GetPreviousLocation() == "from sophia's house") then
        AudioManager:PlaySound("data/sounds/door_close.wav");
        bronann:SetPosition(22, 20);
        bronann:SetDirection(vt_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_layna_forest_entrance") then
        bronann:SetPosition(115, 37);
        bronann:SetDirection(vt_map.MapMode.WEST);
    elseif (GlobalManager:GetPreviousLocation() == "from_bronanns_home") then
        AudioManager:PlaySound("data/sounds/door_close.wav");
    end
end

function _CreateNPCs()
    local npc = nil
    local text = nil
    local dialogue = nil
    local event = nil

    kalya = CreateSprite(Map, "Kalya", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetVisible(false);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    soldier1 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 0, 0, vt_map.MapMode.GROUND_OBJECT);
    soldier1:SetVisible(false);
    soldier1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    soldier2 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 0, 0, vt_map.MapMode.GROUND_OBJECT);
    soldier2:SetVisible(false);
    soldier2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    soldier3 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 0, 0, vt_map.MapMode.GROUND_OBJECT);
    soldier3:SetVisible(false);
    soldier3:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    soldier4 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 0, 0, vt_map.MapMode.GROUND_OBJECT);
    soldier4:SetVisible(false);
    soldier4:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
end

function _CreateObjects()
    CreateObject(Map, "Tree Big2", 22, 78, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 22, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 9, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 65, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 74, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 67, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 80, 36, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 92, 22, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 98, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small2", 79, 16, vt_map.MapMode.GROUND_OBJECT);

    CreateObject(Map, "Rock1", 3, 64, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 2, 62, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 33, 12, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 29, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 109, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 113, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 117, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 109, 42, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 117, 42, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 113, 42, vt_map.MapMode.GROUND_OBJECT);

    -- collision bug hidders
    CreateObject(Map, "Barrel1", 14, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Vase1", 15, 39, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 30, 38, vt_map.MapMode.GROUND_OBJECT);

    -- A village with drinkable water
    CreateObject(Map, "Well", 59.0, 32.0, vt_map.MapMode.GROUND_OBJECT);

    -- Treasure vase
    local nekko_vase = CreateTreasure(Map, "layna_center_nekko_vase", "Vase1", 27, 37, vt_map.MapMode.GROUND_OBJECT);
    nekko_vase:AddItem(11, 1)

    -- Rock hiding the well underground entrance
    CreateObject(Map, "Rock1", 63, 32, vt_map.MapMode.GROUND_OBJECT)

    -- Surrounding grass
    local well_grass = {
    { "Grass Clump1", 62, 32.5 },
    { "Grass Clump1", 64, 33 },
    { "Grass Clump1", 58, 33 },
    { "Grass Clump1", 60, 33.4 },
    { "Grass Clump1", 65, 32.2 },
    { "Grass Clump1", 62, 30.3 },
    }

    -- Loads the grass according to the array
    for my_index, my_array in pairs(well_grass) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
end

-- Special event references which destinations must be updated just before being called.
local move_next_to_hero_event = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local text = nil
    local dialogue = nil

    -- Triggered Events
    vt_map.MapTransitionEvent.Create("to Riverbank", "data/story/layna_village/layna_village_riverbank_map.lua",
                                     "data/story/layna_village/layna_village_riverbank_at_night_script.lua", "from_village_center");

    vt_map.MapTransitionEvent.Create("to Village south entrance", "data/story/layna_village/layna_village_south_entrance_map.lua",
                                     "data/story/layna_village/layna_village_south_entrance_script.lua", "from_village_center");

    vt_map.MapTransitionEvent.Create("to Kalya house path", "data/story/layna_village/layna_village_kalya_house_path_map.lua",
                                     "data/story/layna_village/layna_village_kalya_house_path_script.lua", "from_village_center");

    -- Generic events
    vt_map.ScriptedEvent.Create("Map:PushState(SCENE)", "Map_SceneState", "");

    vt_map.ScriptedEvent.Create("Map:PopState()", "Map_PopState", "");

    vt_map.ScriptedSpriteEvent.Create("kalya:SetCollision(ALL)", kalya, "Sprite_Collision_on", "");

    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks east", bronann, vt_map.MapMode.EAST);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);

    vt_map.ScriptedSpriteEvent.Create("Make Kalya invisible", kalya, "MakeInvisible", "");
    vt_map.ScriptedSpriteEvent.Create("Make Soldier1 invisible", soldier1, "MakeInvisible", "");
    vt_map.ScriptedSpriteEvent.Create("Make Soldier2 invisible", soldier2, "MakeInvisible", "");
    vt_map.ScriptedSpriteEvent.Create("Make Soldier3 invisible", soldier3, "MakeInvisible", "");
    vt_map.ScriptedSpriteEvent.Create("Make Soldier4 invisible", soldier4, "MakeInvisible", "");

    -- story events

    -- Kalya tells Bronann they'll see each other later after she fetched Herth
    event = vt_map.ScriptedEvent.Create("Return to village dialogue", "return_to_village_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 500);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann", kalya, 0, 0, false);
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya tells Bronann she'll fetch Herth");
    move_next_to_hero_event:AddEventLinkAtEnd("kalya:SetCollision(ALL)");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Finally, we made it back home.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("I'll go and get Herth. Your parents must be worried. Go home. We'll meet there.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "");
    event = vt_map.DialogueEvent.Create("Kalya tells Bronann she'll fetch Herth", dialogue);
    event:AddEventLinkAtEnd("Kalya runs to her home");
    event:AddEventLinkAtEnd("End of return to village dialogue");

    vt_map.ScriptedEvent.Create("End of return to village dialogue", "return_to_village_dialogue_end", "");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya runs to her home", kalya, 2, 13, true);
    event:AddEventLinkAtEnd("Make Kalya invisible");

    -- Zone blocking events
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I should really get back home.");
    dialogue:AddLine(text, bronann);
    event = vt_map.DialogueEvent.Create("Bronann should go home", dialogue);
    event:SetStopCameraMovement(true);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Weird, I don't see any lights on in there. I have a bad feeling about this, let's get back home.");
    dialogue:AddLine(text, bronann);
    event = vt_map.DialogueEvent.Create("Bronann should go home when checking other houses", dialogue);
    event:SetStopCameraMovement(true);

    -- Scene where Kalya is brought to the riverbank by two soldiers
    event = vt_map.ScriptedEvent.Create("Kalya is brought by four soldiers to riverbank start", "kalya_captured_scene_start", "");
    event:AddEventLinkAtEnd("Play locked door sound");
    event:AddEventLinkAtEnd("Bronann can't enter his home 1", 400);

    -- Play locked door sound when bronann tries to enter his home
    vt_map.ScriptedEvent.Create("Play locked door sound", "play_lock_door_sound", "");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Huh?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    event = vt_map.DialogueEvent.Create("Bronann can't enter his home 1", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Play locked door sound");
    event:AddEventLinkAtEnd("Play locked door sound", 400);
    event:AddEventLinkAtEnd("Bronann can't enter his home 2", 800);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("What's happening here?! Where is everyone?");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("Help!");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Kalya?!");
    dialogue:AddLineEventEmote(text, bronann, "Bronann looks east", "", "exclamation");
    event = vt_map.DialogueEvent.Create("Bronann can't enter his home 2", dialogue);
    event:AddEventLinkAtEnd("Set camera on Kalya");
    event:AddEventLinkAtEnd("Kalya is struggling");

    event = vt_map.ScriptedEvent.Create("Set camera on Kalya", "set_camera_on_kalya", "is_camera_moving_finished");
    event:AddEventLinkAtEnd("Kalya screams against the soldiers");

    -- Make kalya struggle against the soldiers
    vt_map.ScriptedEvent.Create("Kalya is struggling", "make_kalya_struggle", "");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Let go of me! You cheap low-grade filthy jerks.");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("Keep quiet, little brat!");
    dialogue:AddLine(text, soldier3);
    event = vt_map.DialogueEvent.Create("Kalya screams against the soldiers", dialogue);
    event:AddEventLinkAtEnd("Set camera on Bronann");
    -- Make the soldiers walk while Bronann tries to cover.
    event:AddEventLinkAtEnd("Soldier1 escorts Kalya to riverbank");
    event:AddEventLinkAtEnd("Soldier2 escorts Kalya to riverbank");
    event:AddEventLinkAtEnd("Soldier3 escorts Kalya to riverbank");
    event:AddEventLinkAtEnd("Soldier4 escorts Kalya to riverbank");
    event:AddEventLinkAtEnd("Kalya is escorted to riverbank");

    -- The soldiers are going to the riverbank with Kalya
    -- Point 1: 41, 41 -> 41, 63
    event = vt_map.PathMoveSpriteEvent.Create("Soldier1 escorts Kalya to riverbank", soldier1, 43, 63, false);
    event:AddEventLinkAtEnd("Soldier1 escorts Kalya to riverbank 2");
    event = vt_map.PathMoveSpriteEvent.Create("Soldier2 escorts Kalya to riverbank", soldier2, 39, 63, false);
    event:AddEventLinkAtEnd("Soldier2 escorts Kalya to riverbank 2");
    event = vt_map.PathMoveSpriteEvent.Create("Soldier3 escorts Kalya to riverbank", soldier3, 41, 61, false);
    event:AddEventLinkAtEnd("Soldier3 escorts Kalya to riverbank 2");
    event = vt_map.PathMoveSpriteEvent.Create("Soldier4 escorts Kalya to riverbank", soldier4, 41, 65, false);
    event:AddEventLinkAtEnd("Soldier4 escorts Kalya to riverbank 2");
    event = vt_map.PathMoveSpriteEvent.Create("Kalya is escorted to riverbank", kalya, 41, 63, false);
    event:AddEventLinkAtEnd("Kalya is escorted to riverbank 2");
    -- Point 2: 41, 63 -> 28, 77
    event = vt_map.PathMoveSpriteEvent.Create("Soldier1 escorts Kalya to riverbank 2", soldier1, 30, 77, false);
    event:AddEventLinkAtEnd("Make Soldier1 invisible");
    event = vt_map.PathMoveSpriteEvent.Create("Soldier2 escorts Kalya to riverbank 2", soldier2, 26, 77, false);
    event:AddEventLinkAtEnd("Make Soldier2 invisible");
    event = vt_map.PathMoveSpriteEvent.Create("Soldier3 escorts Kalya to riverbank 2", soldier3, 28, 77, false);
    event:AddEventLinkAtEnd("Make Soldier3 invisible");
    event = vt_map.PathMoveSpriteEvent.Create("Soldier4 escorts Kalya to riverbank 2", soldier4, 28, 77, false);
    event:AddEventLinkAtEnd("Make Soldier4 invisible");
    event = vt_map.PathMoveSpriteEvent.Create("Kalya is escorted to riverbank 2", kalya, 28, 77, false);
    event:AddEventLinkAtEnd("Make Kalya invisible");

    event = vt_map.ScriptedEvent.Create("Set camera on Bronann", "set_camera_on_bronann", "is_camera_moving_finished");
    event:AddEventLinkAtEnd("Bronann runs north of the hill");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann runs north of the hill", bronann, 25, 53.5, true);
    event:AddEventLinkAtEnd("Bronann is surprised");

    event = vt_map.AnimateSpriteEvent.Create("Bronann is surprised", bronann, "frightened", 600);
    event:AddEventLinkAtEnd("Bronann runs and hide behind the hill");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann runs and hide behind the hill", bronann, 22, 65, true);
    event:AddEventLinkAtEnd("Bronann looks south");
    event:AddEventLinkAtEnd("Bronann dialogue after guards caught Kalya", 2200);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Soldiers wearing black armor... the Lord's personal guard!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("Fortunately, they didn't see me or I would have been caught as well.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("I should follow them silently.");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    event = vt_map.DialogueEvent.Create("Bronann dialogue after guards caught Kalya", dialogue);
    event:AddEventLinkAtEnd("End of Kalya capture dialogue");

    vt_map.ScriptedEvent.Create("End of Kalya capture dialogue", "kalya_captured_scene_end", "");

    -- After Kalya has been caught, Bronann wants to follow her
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("What am I doing? I should go and try to free Kalya!");
    dialogue:AddLine(text, bronann);
    event = vt_map.DialogueEvent.Create("Bronann should follow Kalya", dialogue);
    event:SetStopCameraMovement(true);
end

-- zones
local bronanns_home_entrance_zone = nil
local to_riverbank_zone = nil
local to_village_entrance_zone = nil
local to_kalya_house_path_zone = nil
local shop_entrance_zone = nil
local secret_path_zone = nil
local to_layna_forest_zone = nil
local sophia_house_entrance_zone = nil

function _CreateZones()
    -- N.B.: left, right, top, bottom
    bronanns_home_entrance_zone = vt_map.CameraZone.Create(10, 14, 60, 61);
    to_riverbank_zone = vt_map.CameraZone.Create(19, 35, 78, 79);
    to_village_entrance_zone = vt_map.CameraZone.Create(60, 113, 78, 79);
    to_kalya_house_path_zone = vt_map.CameraZone.Create(0, 1, 8, 15);
    shop_entrance_zone = vt_map.CameraZone.Create(92, 96, 70, 71);
    secret_path_zone = vt_map.CameraZone.Create(0, 1, 55, 61);
    to_layna_forest_zone = vt_map.CameraZone.Create(117, 119, 30, 43);
    sophia_house_entrance_zone = vt_map.CameraZone.Create(21, 23, 21, 22);
end

function _CheckZones()
    if (Map:CurrentState() == vt_map.MapMode.STATE_SCENE) then
        return;
    end

    if (to_layna_forest_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "layna_village_kalya_captured_done") == 1) then
            EventManager:StartEvent("Bronann should follow Kalya");
        else
            EventManager:StartEvent("Bronann should go home");
        end
    elseif (to_riverbank_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "layna_village_kalya_captured_done") == 1) then
            EventManager:StartEvent("to Riverbank");
        else
            EventManager:StartEvent("Bronann should go home");
        end
    elseif (secret_path_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "layna_village_kalya_captured_done") == 1) then
            EventManager:StartEvent("Bronann should follow Kalya");
        else
            EventManager:StartEvent("Bronann should go home");
        end
    elseif (to_village_entrance_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "layna_village_kalya_captured_done") == 1) then
            EventManager:StartEvent("Bronann should follow Kalya");
        else
            EventManager:StartEvent("Bronann should go home");
        end
    elseif (to_kalya_house_path_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "layna_village_kalya_captured_done") == 1) then
            EventManager:StartEvent("Bronann should follow Kalya");
        else
            EventManager:StartEvent("Bronann should go home");
        end
    elseif (shop_entrance_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "layna_village_kalya_captured_done") == 1) then
            EventManager:StartEvent("Bronann should follow Kalya");
        else
            EventManager:StartEvent("Bronann should go home when checking other houses");
        end
    elseif (sophia_house_entrance_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "layna_village_kalya_captured_done") == 1) then
            EventManager:StartEvent("Bronann should follow Kalya");
        else
            EventManager:StartEvent("Bronann should go home when checking other houses");
        end
    elseif (bronanns_home_entrance_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "layna_village_kalya_captured_done") == 1) then
            EventManager:StartEvent("Bronann should follow Kalya");
        else
            EventManager:StartEvent("Kalya is brought by four soldiers to riverbank start");
        end
    end
end

-- Map Custom functions
map_functions = {
    Sprite_Collision_on = function(sprite)
        if (sprite ~= nil) then
            sprite:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        end
    end,

    Map_SceneState = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

    Map_PopState = function()
        Map:PopState();
    end,

    MakeInvisible = function(sprite)
        if (sprite ~= nil) then
            sprite:SetVisible(false);
            sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end,

    -- Kalya runs to the save point and tells Bronann about the spring.
    return_to_village_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        bronann:SetMoving(false);

        kalya:SetVisible(true);
        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        bronann:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        move_next_to_hero_event:SetDestination(bronann:GetXPosition() - 2.0, bronann:GetYPosition(), false);

        -- Disable Kalya from the party menu
        GlobalManager:RemoveCharacter(KALYA, false);
    end,

    return_to_village_dialogue_end = function()

        -- Set event as done
        GlobalManager:SetEventValue("story", "return_to_layna_village_dialogue_done", 1);
        Map:PopState();
    end,

    play_lock_door_sound = function()
        -- Play locked door sound when bronann tries to enter his home
        AudioManager:PlaySound("data/sounds/door_close.wav");
    end,

    kalya_captured_scene_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        bronann:SetMoving(false);

        -- In case she hasn't finished running
        EventManager:EndAllEvents(kalya);

        kalya:SetVisible(true);
        kalya:SetPosition(41, 41);
        kalya:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        kalya:SetMovementSpeed(vt_map.MapMode.SLOW_SPEED);
        kalya:SetDirection(vt_map.MapMode.SOUTH);

        soldier1:SetVisible(true);
        soldier1:SetPosition(43, 41);
        soldier1:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        soldier1:SetMovementSpeed(vt_map.MapMode.SLOW_SPEED);
        soldier1:SetDirection(vt_map.MapMode.SOUTH);

        soldier2:SetVisible(true);
        soldier2:SetPosition(39, 41);
        soldier2:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        soldier2:SetMovementSpeed(vt_map.MapMode.SLOW_SPEED);
        soldier2:SetDirection(vt_map.MapMode.SOUTH);

        soldier3:SetVisible(true);
        soldier3:SetPosition(41, 39);
        soldier3:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        soldier3:SetMovementSpeed(vt_map.MapMode.SLOW_SPEED);
        soldier3:SetDirection(vt_map.MapMode.SOUTH);

        soldier4:SetVisible(true);
        soldier4:SetPosition(41, 43);
        soldier4:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        soldier4:SetMovementSpeed(vt_map.MapMode.SLOW_SPEED);
        soldier4:SetDirection(vt_map.MapMode.SOUTH);

        -- Fade out the music
        AudioManager:FadeOutActiveMusic(2000);
    end,

    set_camera_on_kalya = function()
        Map:SetCamera(kalya, 800);

        -- Play the soldier music
        AudioManager:PlayMusic("data/music/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg");
    end,

    set_camera_on_bronann = function()
        Map:SetCamera(bronann, 800);
    end,

    is_camera_moving_finished = function()
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        return true;
    end,

    make_kalya_struggle = function()
        kalya:SetCustomAnimation("struggling", 0); -- 0 means forever
    end,

    kalya_captured_scene_end = function()

        -- Set event as done
        GlobalManager:SetEventValue("story", "layna_village_kalya_captured_done", 1);
        Map:PopState();
    end,
}
