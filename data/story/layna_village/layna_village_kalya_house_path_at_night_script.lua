-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_kalya_house_path_at_night_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "data/story/common/locations/mountain_village.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "music/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

local bronann = nil
local orlinn = nil
local kalya = nil

-- the main map loading code
function Load(m)

    Map = m;
    EventManager = Map:GetEventSupervisor();

    Map:SetUnlimitedStamina(true);

    _CreateCharacters();
    -- Set the camera focus on Bronann
    Map:SetCamera(bronann);

    _CreateObjects();

    _CreateEvents();

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, -5.0, true);

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
    bronann = CreateSprite(Map, "Bronann", 63, 45, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.WEST);
    bronann:SetMovementSpeed(vt_map.MapMode.VERY_SLOW_SPEED);
    bronann:SetVisible(false);
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    orlinn = CreateSprite(Map, "Orlinn", 63, 45, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.WEST);
    orlinn:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    kalya = CreateSprite(Map, "Kalya", 63, 45, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.WEST);
    kalya:SetVisible(false);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
end

function _CreateObjects()
    -- Left tree "wall"
    CreateObject(Map, "Tree Big1", 0, 44, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 42, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 40, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 36, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 0, 30, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 28, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 26, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 0, 22, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 14, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 12, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 0, 10, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 7, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 0, 3, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 15, 3, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 15.5, 1.5, vt_map.MapMode.GROUND_OBJECT);

    -- Right tree "Wall"
    CreateObject(Map, "Rock2", 63, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 63, 36, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 63, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 63, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 63, 30, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small2", 63.5, 28, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 63, 26, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 63, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 63, 22, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 63, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 63, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small2", 64.5, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 63, 12, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small2", 64, 10, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 63, 8, vt_map.MapMode.GROUND_OBJECT);

    -- Secret shortcut hiders
    CreateObject(Map, "Tree Big1", 38, 40, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 40, 42, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 42, 40, vt_map.MapMode.GROUND_OBJECT);

    -- Cliff hiders
    CreateObject(Map, "Tree Small1", 14, 30, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 55, 12, vt_map.MapMode.GROUND_OBJECT);

    -- Fence
    CreateObject(Map, "Fence1 l top left", 17, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 19, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 21, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 23, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 25, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 27, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 l top right", 29, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 vertical", 17, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 vertical", 17, 36, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 l bottom left", 17, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 19, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 21, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 23, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 25, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 27, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 l bottom right", 29, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 vertical", 29, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 vertical", 29, 36, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Bench1", 7, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 20, 24, vt_map.MapMode.GROUND_OBJECT);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks east", orlinn, vt_map.MapMode.EAST);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks east", kalya, vt_map.MapMode.EAST);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Orlinn", kalya, orlinn);

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn runs to the trees", orlinn, 40.0, 45.0, true);
    event:AddEventLinkAtEnd("Orlinn looks east");
    event:AddEventLinkAtEnd("Orlinn calls the others", 800);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Hurry up! They're coming!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    event = vt_map.DialogueEvent.Create("Orlinn calls the others", dialogue);
    event:AddEventLinkAtEnd("Kalya arrives");

    event = vt_map.ScriptedEvent.Create("Kalya arrives", "make_kalya_visible", "");
    event:AddEventLinkAtEnd("Kalya runs midway to Orlinn");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya runs midway to Orlinn", kalya, 50.0, 45.0, true);
    event:AddEventLinkAtEnd("Kalya looks east");
    event:AddEventLinkAtEnd("Kalya calls Bronann 1", 1200);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    event = vt_map.DialogueEvent.Create("Kalya calls Bronann 1", dialogue);
    event:AddEventLinkAtEnd("Kalya calls Bronann 2", 800);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    event = vt_map.DialogueEvent.Create("Kalya calls Bronann 2", dialogue);
    event:AddEventLinkAtEnd("Bronann arrives", 600);

    event = vt_map.ScriptedEvent.Create("Bronann arrives", "make_bronann_visible", "");
    event:AddEventLinkAtEnd("Bronann runs midway to Kalya");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann runs midway to Kalya", bronann, 55.0, 45.0, true);
    event:AddEventLinkAtEnd("Bronann kneels due to suffering", 300);
    event:AddEventLinkAtEnd("Kalya goes near Bronann", 800);

    vt_map.AnimateSpriteEvent.Create("Bronann kneels due to suffering", bronann, "kneeling_left", 0); -- 0 means forever

    event = vt_map.PathMoveSpriteEvent.Create("Kalya goes near Bronann", kalya, 53.0, 45.0, true);
    event:AddEventLinkAtEnd("Kalya urges Bronann");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann! Hold on! We're almost there.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("I... can't. My body... Argh!");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("You can do it. Bronann...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Bronann! Move!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    event = vt_map.DialogueEvent.Create("Kalya urges Bronann", dialogue);
    event:AddEventLinkAtEnd("Bronann gets up", 600);

    event = vt_map.ScriptedEvent.Create("Bronann gets up", "bronann_gets_up", "");
    event:AddEventLinkAtEnd("Kalya tells Bronann to move on");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("... Al... right...");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Great! Now come on!");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Kalya tells Bronann to move on", dialogue);
    event:AddEventLinkAtEnd("Kalya goes near Orlinn");
    event:AddEventLinkAtEnd("Bronann goes near Orlinn", 400);

    event = vt_map.PathMoveSpriteEvent.Create("Kalya goes near Orlinn", kalya, 42.0, 45.0, true);
    event:AddEventLinkAtEnd("Kalya looks east");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann goes near Orlinn", bronann, 44.0, 45.0, false);
    event:AddEventLinkAtEnd("Bronann is worrying");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("There must be soldiers everywere, we're never gonna make it.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("This town has many secrets.");
    dialogue:AddLineEvent(text, kalya, "", "Kalya looks at Orlinn");
    text = vt_system.Translate("You can go in there, Orlinn.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Ok!");
    dialogue:AddLineEvent(text, orlinn, "", "Orlinn goes into the secret passage");
    text = vt_system.Translate("See?");
    dialogue:AddLineEvent(text, kalya, "Kalya looks east", "");
    text = vt_system.Translate("I understand why Grandma actually never saw you using the 'normal' path.");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("Now come.");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Bronann is worrying", dialogue);
    event:AddEventLinkAtEnd("Kalya goes into the secret passage");
    event:AddEventLinkAtEnd("Bronann hesitates", 1000);

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn goes into the secret passage", orlinn, 40.0, 40.0, true);
    event:AddEventLinkAtEnd("Orlinn disappears");

    vt_map.ScriptedEvent.Create("Orlinn disappears", "orlinn_disappears", "");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya goes into the secret passage", kalya, 40.0, 40.0, true);
    event:AddEventLinkAtEnd("Kalya disappears");

    vt_map.ScriptedEvent.Create("Kalya disappears", "kalya_disappears", "");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann hesitates", bronann, 46.0, 45.0, false);
    event:AddEventLinkAtEnd("Bronann goes into the secret passage", 2000);

    event = vt_map.PathMoveSpriteEvent.Create("Bronann goes into the secret passage", bronann, 40.0, 40.0, false);
    event:AddEventLinkAtEnd("Bronann disappears");

    event = vt_map.ScriptedEvent.Create("Bronann disappears", "bronann_disappears", "");
    event:AddEventLinkAtEnd("to Kalya's house");

    -- Transition to Kalya's house
    vt_map.MapTransitionEvent.Create("to Kalya's house", "dat/maps/layna_village/layna_village_kalya_house_map.lua",
                                     "dat/maps/layna_village/layna_village_kalya_house_script.lua", "from_kalya_house_path");
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
        EventManager:EndAllEvents(bronann);
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
