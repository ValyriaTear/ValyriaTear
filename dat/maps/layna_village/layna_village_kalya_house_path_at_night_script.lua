-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_kalya_house_path_at_night_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

local bronann = {};
local orlinn = {};
local kalya = {};

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

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);

    -- Put last to get a proper night effect
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");

    -- Kalya was previously removed from party, let's add her back now
    GlobalManager:AddCharacter(KALYA);

    -- Set scene mode and start the scene right away
    Map:PushState(vt_map.MapMode.STATE_SCENE);
    EventManager:StartEvent("Orlinn runs to the trees")
end

-- Character creation
function _CreateCharacters()
    -- Default: From village center
    bronann = CreateSprite(Map, "Bronann", 63, 45);
    bronann:SetDirection(vt_map.MapMode.WEST);
    bronann:SetMovementSpeed(vt_map.MapMode.VERY_SLOW_SPEED);
    bronann:SetVisible(false);
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(bronann);

    orlinn = CreateSprite(Map, "Orlinn", 63, 45);
    orlinn:SetDirection(vt_map.MapMode.WEST);
    orlinn:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(orlinn);

    kalya = CreateSprite(Map, "Kalya", 63, 45);
    kalya:SetDirection(vt_map.MapMode.WEST);
    kalya:SetVisible(false);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    Map:AddGroundObject(kalya);
end

local soldier1 = {};
local soldier2 = {};
local soldier3 = {};
local soldier4 = {};

function _CreateNPCs()
    local npc = {}
    local text = {}
    local dialogue = {}
    local event = {}

    -- Create the soldiers
    soldier1 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 88, 59);
    soldier1:SetDirection(vt_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier1);
    soldier2 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 96, 59);
    soldier2:SetDirection(vt_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier2);
    soldier3 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 106, 59.5);
    soldier3:SetDirection(vt_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier3);
    soldier4 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 88, 55);
    soldier4:SetDirection(vt_map.MapMode.WEST);
    Map:AddGroundObject(soldier4);
end

function _CreateObjects()
    local object = {}

    -- Left tree "wall"
    object = CreateObject(Map, "Tree Big1", 0, 44);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 42);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 40);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 36);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 34);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 0, 30);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 28);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 26);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 24);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 0, 22);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 20);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 18);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 16);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 14);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 12);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock2", 0, 10);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 0, 7);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 0, 5);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 0, 3);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 15, 3);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 15.5, 1.5);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Right tree "Wall"
    object = CreateObject(Map, "Rock2", 63, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 63, 36);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 63, 34);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 63, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 63, 30);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small2", 63.5, 28);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 63, 26);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 63, 24);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 63, 22);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 63, 20);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 63, 18);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small2", 64.5, 16);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Rock1", 63, 12);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small2", 64, 10);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 63, 8);
    if (object ~= nil) then Map:AddGroundObject(object) end;


    -- Secret shortcut hiders
    object = CreateObject(Map, "Tree Big1", 38, 40);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 40, 42);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 42, 40);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Cliff hiders
    object = CreateObject(Map, "Tree Small1", 14, 30);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 55, 12);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Fence
    object = CreateObject(Map, "Fence1 l top left", 17, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 19, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 21, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 23, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 25, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 27, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 l top right", 29, 32);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Fence1 vertical", 17, 34);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 vertical", 17, 36);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Fence1 l bottom left", 17, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 19, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 21, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 23, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 25, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 horizontal", 27, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 l bottom right", 29, 38);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 vertical", 29, 34);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Fence1 vertical", 29, 36);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Bench1", 7, 24);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Barrel1", 20, 24);
    if (object ~= nil) then Map:AddGroundObject(object) end;
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};

    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks east", orlinn, vt_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks east", kalya, vt_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Kalya looks at Orlinn", kalya, orlinn);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Orlinn runs to the trees", orlinn, 40.0, 45.0, true);
    event:AddEventLinkAtEnd("Orlinn looks east");
    event:AddEventLinkAtEnd("Orlinn calls the others", 800);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Hurry up! They're coming!!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Orlinn calls the others", dialogue);
    event:AddEventLinkAtEnd("Kalya arrives");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Kalya arrives", "make_kalya_visible", "");
    event:AddEventLinkAtEnd("Kalya runs midway to Orlinn");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Kalya runs midway to Orlinn", kalya, 50.0, 45.0, true);
    event:AddEventLinkAtEnd("Kalya looks east");
    event:AddEventLinkAtEnd("Kalya calls Bronann 1", 1200);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Bronann!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya calls Bronann 1", dialogue);
    event:AddEventLinkAtEnd("Kalya calls Bronann 2", 800);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Bronann!!!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya calls Bronann 2", dialogue);
    event:AddEventLinkAtEnd("Bronann arrives", 600);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Bronann arrives", "make_bronann_visible", "");
    event:AddEventLinkAtEnd("Bronann runs midway to Kalya");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Bronann runs midway to Kalya", bronann, 55.0, 45.0, true);
    event:AddEventLinkAtEnd("Bronann looks south before suffering", 300);
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks south before suffering", bronann, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Bronann kneels due to suffering", 500);
    event:AddEventLinkAtEnd("Kalya goes near Bronann", 800);
    EventManager:RegisterEvent(event);

    event = vt_map.AnimateSpriteEvent("Bronann kneels due to suffering", bronann, "kneeling", 999999);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Kalya goes near Bronann", kalya, 53.0, 45.0, true);
    event:AddEventLinkAtEnd("Kalya urges Bronann");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Bronann!!! Hold on! We're almost there...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("I ... can't. My body ... Argh!");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("You can do it. Bronann...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Bronann!! Move!!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya urges Bronann", dialogue);
    event:AddEventLinkAtEnd("Bronann gets up", 600);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Bronann gets up", "bronann_gets_up", "");
    event:AddEventLinkAtEnd("Kalya tells Bronann to move on");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("... Al ... right...");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Great! Now come on!");
    dialogue:AddLine(text, kalya);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya tells Bronann to move on", dialogue);
    event:AddEventLinkAtEnd("Kalya goes near Orlinn");
    event:AddEventLinkAtEnd("Bronann goes near Orlinn", 400);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Kalya goes near Orlinn", kalya, 42.0, 45.0, true);
    event:AddEventLinkAtEnd("Kalya looks east");
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Bronann goes near Orlinn", bronann, 44.0, 45.0, false);
    event:AddEventLinkAtEnd("Bronann is worrying");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("There must be soldiers everywere, we're never gonna make it...");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Stop worrying. We also have our little secrets...");
    dialogue:AddLineEvent(text, kalya, "", "Kalya looks at Orlinn");
    text = vt_system.Translate("You can go in there, Orlinn.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Ok!");
    dialogue:AddLineEvent(text, orlinn, "", "Orlinn goes into the secret passage");
    text = vt_system.Translate("See?");
    dialogue:AddLineEvent(text, kalya, "Kalya looks east", "");
    text = vt_system.Translate("... I understand now why Grandma actually never saw you using the 'normal' path...");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("Now come...");
    dialogue:AddLine(text, kalya);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Bronann is worrying", dialogue);
    event:AddEventLinkAtEnd("Kalya goes into the secret passage");
    event:AddEventLinkAtEnd("Bronann hesitates", 1000);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Orlinn goes into the secret passage", orlinn, 40.0, 40.0, true);
    event:AddEventLinkAtEnd("Orlinn disappears");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Orlinn disappears", "orlinn_disappears", "");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Kalya goes into the secret passage", kalya, 40.0, 40.0, true);
    event:AddEventLinkAtEnd("Kalya disappears");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Kalya disappears", "kalya_disappears", "");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Bronann hesitates", bronann, 46.0, 45.0, false);
    event:AddEventLinkAtEnd("Bronann goes into the secret passage", 2000);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Bronann goes into the secret passage", bronann, 40.0, 40.0, false);
    event:AddEventLinkAtEnd("Bronann disappears");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Bronann disappears", "bronann_disappears", "");
    event:AddEventLinkAtEnd("to Kalya's house");
    EventManager:RegisterEvent(event);

    -- Transition to Kalya's house
    event = vt_map.MapTransitionEvent("to Kalya's house", "dat/maps/layna_village/layna_village_kalya_house_map.lua",
                                       "dat/maps/layna_village/layna_village_kalya_house_script.lua", "from_kalya_house_path");
    EventManager:RegisterEvent(event);
end

-- Map Custom functions
map_functions = {

    make_kalya_visible = function()
        kalya:SetVisible(true);
    end,

    make_bronann_visible = function()
        bronann:SetVisible(true);
    end,

    bronann_gets_up = function()
        -- Actually terminates the animate event
        EventManager:TerminateAllEvents(bronann);
        -- Also, slows down Orlinn to let the player see he's going to the secret passage
        orlinn:SetMovementSpeed(vt_map.MapMode.VERY_SLOW_SPEED);
    end,

    orlinn_disappears =  function()
        orlinn:SetVisible(false);
    end,

    kalya_disappears =  function()
        kalya:SetVisible(false);
    end,

    bronann_disappears =  function()
        bronann:SetVisible(false);
    end,
}
