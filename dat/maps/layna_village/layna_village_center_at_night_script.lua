-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_center_at_night_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = "Village center"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
-- TODO: Add night forest looping sound
music_filename = ""

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};
local Effects = {};

local bronann = {};
local kalya = {};

-- The soldiers bringing Kalya to the riverbank
local soldier1 = {};
local soldier2 = {};
local soldier3 = {};
local soldier4 = {};

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

    -- Add clouds overlay
    Effects:EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");

    -- Set the world map current position
    GlobalManager:SetCurrentLocationId("layna village");

    -- Start the return to village dialogue if it hasn't been done already.
    if (GlobalManager:GetEventValue("story", "return_to_layna_village_dialogue_done") ~= 1) then
        EventManager:StartEvent("Return to village dialogue");
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

    kalya = CreateSprite(Map, "Kalya", 0, 0);
    kalya:SetVisible(false);
    kalya:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(kalya);

    soldier1 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 0, 0);
    soldier1:SetVisible(false);
    soldier1:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(soldier1);
    soldier2 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 0, 0);
    soldier2:SetVisible(false);
    soldier2:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(soldier2);
    soldier3 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 0, 0);
    soldier3:SetVisible(false);
    soldier3:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(soldier3);
    soldier4 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 0, 0);
    soldier4:SetVisible(false);
    soldier4:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(soldier4);
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

    -- Treasure vase
    local nekko_vase = CreateTreasure(Map, "layna_center_nekko_vase", "Vase1", 27, 37);
    if (nekko_vase ~= nil) then
        nekko_vase:AddObject(11, 1);
        Map:AddGroundObject(nekko_vase);
    end
end

-- Special event references which destinations must be updated just before being called.
local move_next_to_hero_event = {};

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local text = {};
    local dialogue = {};

    -- Triggered Events
    event = hoa_map.MapTransitionEvent("to Riverbank", "dat/maps/layna_village/layna_village_riverbank_map.lua",
                                       "dat/maps/layna_village/layna_village_riverbank_at_night_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to Village south entrance", "dat/maps/layna_village/layna_village_south_entrance_map.lua",
                                       "dat/maps/layna_village/layna_village_south_entrance_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    event = hoa_map.MapTransitionEvent("to Kalya house path", "dat/maps/layna_village/layna_village_kalya_house_path_map.lua",
                                       "dat/maps/layna_village/layna_village_kalya_house_path_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    -- Generic events
    event = hoa_map.ScriptedEvent("Map:PushState(SCENE)", "Map_SceneState", "");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("Map:PopState()", "Map_PopState", "");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedSpriteEvent("kalya:SetCollision(ALL)", kalya, "Sprite_Collision_on", "");
    EventManager:RegisterEvent(event);

    event = hoa_map.LookAtSpriteEvent("Kalya looks at Bronann", kalya, bronann);
    EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Bronann looks east", bronann, hoa_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Bronann looks south", bronann, hoa_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedSpriteEvent("Make Kalya invisible", kalya, "MakeInvisible", "");
    EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedSpriteEvent("Make Soldier1 invisible", soldier1, "MakeInvisible", "");
    EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedSpriteEvent("Make Soldier2 invisible", soldier2, "MakeInvisible", "");
    EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedSpriteEvent("Make Soldier3 invisible", soldier3, "MakeInvisible", "");
    EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedSpriteEvent("Make Soldier4 invisible", soldier4, "MakeInvisible", "");
    EventManager:RegisterEvent(event);

    -- story events

    -- Kalya tells Bronann they'll see each other later after she fetched Herth
    event = hoa_map.ScriptedEvent("Return to village dialogue", "return_to_village_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 500);
    EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event = hoa_map.PathMoveSpriteEvent("Kalya moves next to Bronann", kalya, 0, 0, false);
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya tells Bronann she'll fetch Herth");
    move_next_to_hero_event:AddEventLinkAtEnd("kalya:SetCollision(ALL)");
    EventManager:RegisterEvent(move_next_to_hero_event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Back home, finally...");
    dialogue:AddLine(text, kalya);
    text = hoa_system.Translate("I'll go and get Herth. Your parents must be worried, you should get back home and we'll meet there.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Kalya tells Bronann she'll fetch Herth", dialogue);
    event:AddEventLinkAtEnd("Kalya runs to her home");
    event:AddEventLinkAtEnd("End of return to village dialogue");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("End of return to village dialogue", "return_to_village_dialogue_end", "");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Kalya runs to her home", kalya, 2, 13, true);
    event:AddEventLinkAtEnd("Make Kalya invisible");
    EventManager:RegisterEvent(event);

    -- Zone blocking events
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("I should really get back home...");
    dialogue:AddLine(text, bronann);
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Bronann should go home", dialogue);
    event:SetStopCameraMovement(true);
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Weird, I can see no lights in there... I have a bad feeling about this, lets get back home...");
    dialogue:AddLine(text, bronann);
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Bronann should go home when checking other houses", dialogue);
    event:SetStopCameraMovement(true);
    EventManager:RegisterEvent(event);

    -- Scene where Kalya is brought to the riverbank by two soldiers
    event = hoa_map.ScriptedEvent("Kalya is brought by four soldiers to riverbank start", "kalya_captured_scene_start", "");
    event:AddEventLinkAtEnd("Play locked door sound");
    event:AddEventLinkAtEnd("Bronann can't enter his home 1", 400);
    EventManager:RegisterEvent(event);

    -- Play locked door sound when bronann tries to enter his home
    event = hoa_map.ScriptedEvent("Play locked door sound", "play_lock_door_sound", "");
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Huh?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Bronann can't enter his home 1", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Play locked door sound");
    event:AddEventLinkAtEnd("Play locked door sound", 400);
    event:AddEventLinkAtEnd("Bronann can't enter his home 2", 800);
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("What's happening here?!? Where is everyone??");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = hoa_system.Translate("Help!!!");
    dialogue:AddLine(text, kalya);
    text = hoa_system.Translate("Kalya?!");
    dialogue:AddLineEventEmote(text, bronann, "Bronann looks east", "", "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Bronann can't enter his home 2", dialogue);
    event:AddEventLinkAtEnd("Set camera on Kalya");
    event:AddEventLinkAtEnd("Kalya is struggling");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("Set camera on Kalya", "set_camera_on_kalya", "is_camera_moving_finished");
    event:AddEventLinkAtEnd("Kalya screams against the soldiers");
    EventManager:RegisterEvent(event);

    -- Make kalya struggle against the soldiers
    event = hoa_map.ScriptedEvent("Kalya is struggling", "make_kalya_struggle", "");
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Lay off me, you cheap low-grade filthy jerks...");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = hoa_system.Translate("Do keep quiet little brat!");
    dialogue:AddLine(text, soldier3);
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Kalya screams against the soldiers", dialogue);
    event:AddEventLinkAtEnd("Set camera on Bronann");
    -- Make the soldiers walk while Bronann tries to cover.
    event:AddEventLinkAtEnd("Soldier1 escorts Kalya to riverbank");
    event:AddEventLinkAtEnd("Soldier2 escorts Kalya to riverbank");
    event:AddEventLinkAtEnd("Soldier3 escorts Kalya to riverbank");
    event:AddEventLinkAtEnd("Soldier4 escorts Kalya to riverbank");
    event:AddEventLinkAtEnd("Kalya is escorted to riverbank");
    EventManager:RegisterEvent(event);

    -- The soldiers are going to the riverbank with Kalya
    -- Point 1: 41, 41 -> 41, 63
    event = hoa_map.PathMoveSpriteEvent("Soldier1 escorts Kalya to riverbank", soldier1, 43, 63, false);
    event:AddEventLinkAtEnd("Soldier1 escorts Kalya to riverbank 2");
    EventManager:RegisterEvent(event);
    event = hoa_map.PathMoveSpriteEvent("Soldier2 escorts Kalya to riverbank", soldier2, 39, 63, false);
    event:AddEventLinkAtEnd("Soldier2 escorts Kalya to riverbank 2");
    EventManager:RegisterEvent(event);
    event = hoa_map.PathMoveSpriteEvent("Soldier3 escorts Kalya to riverbank", soldier3, 41, 61, false);
    event:AddEventLinkAtEnd("Soldier3 escorts Kalya to riverbank 2");
    EventManager:RegisterEvent(event);
    event = hoa_map.PathMoveSpriteEvent("Soldier4 escorts Kalya to riverbank", soldier4, 41, 65, false);
    event:AddEventLinkAtEnd("Soldier4 escorts Kalya to riverbank 2");
    EventManager:RegisterEvent(event);
    event = hoa_map.PathMoveSpriteEvent("Kalya is escorted to riverbank", kalya, 41, 63, false);
    event:AddEventLinkAtEnd("Kalya is escorted to riverbank 2");
    EventManager:RegisterEvent(event);
    -- Point 2: 41, 63 -> 28, 77
    event = hoa_map.PathMoveSpriteEvent("Soldier1 escorts Kalya to riverbank 2", soldier1, 30, 77, false);
    event:AddEventLinkAtEnd("Make Soldier1 invisible");
    EventManager:RegisterEvent(event);
    event = hoa_map.PathMoveSpriteEvent("Soldier2 escorts Kalya to riverbank 2", soldier2, 26, 77, false);
    event:AddEventLinkAtEnd("Make Soldier2 invisible");
    EventManager:RegisterEvent(event);
    event = hoa_map.PathMoveSpriteEvent("Soldier3 escorts Kalya to riverbank 2", soldier3, 28, 77, false);
    event:AddEventLinkAtEnd("Make Soldier3 invisible");
    EventManager:RegisterEvent(event);
    event = hoa_map.PathMoveSpriteEvent("Soldier4 escorts Kalya to riverbank 2", soldier4, 28, 77, false);
    event:AddEventLinkAtEnd("Make Soldier4 invisible");
    EventManager:RegisterEvent(event);
    event = hoa_map.PathMoveSpriteEvent("Kalya is escorted to riverbank 2", kalya, 28, 77, false);
    event:AddEventLinkAtEnd("Make Kalya invisible");
    EventManager:RegisterEvent(event);


    event = hoa_map.ScriptedEvent("Set camera on Bronann", "set_camera_on_bronann", "is_camera_moving_finished");
    event:AddEventLinkAtEnd("Bronann runs north of the hill");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Bronann runs north of the hill", bronann, 25, 53.5, true);
    event:AddEventLinkAtEnd("Bronann is surprised");
    EventManager:RegisterEvent(event);

    event = hoa_map.AnimateSpriteEvent("Bronann is surprised", bronann, "frightened", 600);
    event:AddEventLinkAtEnd("Bronann runs and hide behind the hill");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("Bronann runs and hide behind the hill", bronann, 22, 65, true);
    event:AddEventLinkAtEnd("Bronann looks south");
    event:AddEventLinkAtEnd("Bronann dialogue after guards caught Kalya", 2200);
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Soldiers wearing black armors: the Lord's personal guards!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = hoa_system.Translate("Fortunately, they didn't see me or I would have been caught as well.");
    dialogue:AddLine(text, bronann);
    text = hoa_system.Translate("I should follow them silently. Maybe I'll be able to free Kalya on time.");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Bronann dialogue after guards caught Kalya", dialogue);
    event:AddEventLinkAtEnd("End of Kalya capture dialogue");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("End of Kalya capture dialogue", "kalya_captured_scene_end", "");
    EventManager:RegisterEvent(event);

    -- After Kalya has been caught, Bronann wants to follow her
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("What am I doing? I should go an try to free Kalya!");
    dialogue:AddLine(text, bronann);
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Bronann should follow Kalya", dialogue);
    event:SetStopCameraMovement(true);
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
    if (Map:CurrentState() == hoa_map.MapMode.STATE_SCENE) then
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
    end,

    -- Kalya runs to the save point and tells Bronann about the spring.
    return_to_village_dialogue_start = function()
        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        bronann:SetMoving(false);

        kalya:SetVisible(true);
        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        bronann:SetCollisionMask(hoa_map.MapMode.ALL_COLLISION);
        kalya:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);

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
        AudioManager:PlaySound("snd/door_close.wav");
    end,

    kalya_captured_scene_start = function()
        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        bronann:SetMoving(false);

        -- In case she has finished running
        EventManager:TerminateAllEvents(kalya);

        kalya:SetVisible(true);
        kalya:SetPosition(41, 41);
        kalya:SetCollisionMask(hoa_map.MapMode.WALL_COLLISION);
        kalya:SetMovementSpeed(hoa_map.MapMode.SLOW_SPEED);
        kalya:SetDirection(hoa_map.MapMode.SOUTH);

        soldier1:SetVisible(true);
        soldier1:SetPosition(43, 41);
        soldier1:SetCollisionMask(hoa_map.MapMode.WALL_COLLISION);
        soldier1:SetDirection(hoa_map.MapMode.SOUTH);

        soldier2:SetVisible(true);
        soldier2:SetPosition(39, 41);
        soldier2:SetCollisionMask(hoa_map.MapMode.WALL_COLLISION);
        soldier2:SetDirection(hoa_map.MapMode.SOUTH);

        soldier3:SetVisible(true);
        soldier3:SetPosition(41, 39);
        soldier3:SetCollisionMask(hoa_map.MapMode.WALL_COLLISION);
        soldier3:SetDirection(hoa_map.MapMode.SOUTH);

        soldier4:SetVisible(true);
        soldier4:SetPosition(41, 43);
        soldier4:SetCollisionMask(hoa_map.MapMode.WALL_COLLISION);
        soldier4:SetDirection(hoa_map.MapMode.SOUTH);
    end,

    set_camera_on_kalya = function()
        Map:SetCamera(kalya, 800);
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
        kalya:SetCustomAnimation("struggling", 999999);
    end,

    kalya_captured_scene_end = function()

        -- Set event as done
        GlobalManager:SetEventValue("story", "layna_village_kalya_captured_done", 1);
        Map:PopState();
    end,
}
