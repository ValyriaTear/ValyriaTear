-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_riverbank_at_night_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "data/story/common/locations/mountain_village.png"
map_subname = "Riverbank"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "music/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Script = nil

local bronann = nil

-- the main map loading code
function Load(m)

    Map = m;
    Script = Map:GetScriptSupervisor();
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
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, -5.0, true);

    -- Event Scripts
    Script:AddScript("dat/maps/layna_village/battle_with_banesore/show_crystals_script.lua");
    Script:AddScript("dat/maps/layna_village/battle_with_banesore/show_smoke_cloud_script.lua");

    -- Put last to get a proper night effect
    Script:AddScript("dat/maps/common/at_night.lua");

    -- Start the return to village dialogue if it hasn't been done already.
    if (GlobalManager:GetEventValue("story", "layna_village_arrival_at_riverbank_done") ~= 1) then
        EventManager:StartEvent("Bronann arrives at the riverbank dialogue");
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
        AudioManager:PlaySound("sounds/door_close.wav");
    end
end

local herth = nil
local orlinn = nil
local kalya = nil
local lilly = nil
local carson = nil
local malta = nil
local brymir = nil
local martha = nil
local georges = nil
local olivia = nil

local soldier1 = nil
local soldier2 = nil
local soldier3 = nil
local soldier4 = nil
local soldier5 = nil
local soldier6 = nil
local soldier7 = nil
local soldier8 = nil
local soldier9 = nil
local soldier10 = nil
local soldier11 = nil
local soldier12 = nil
local soldier13 = nil
local soldier14 = nil
local soldier15 = nil
local soldier16 = nil

local soldier17 = nil
local soldier18 = nil
local soldier19 = nil
local soldier20 = nil

-- Soldiers guarding the surroundings
local soldier21 = nil
local soldier22 = nil

local lord = nil

function _CreateNPCs()
    local text = nil
    local dialogue = nil
    local event = nil

    herth = CreateSprite(Map, "Herth", 86, 55, vt_map.MapMode.GROUND_OBJECT);
    herth:SetDirection(vt_map.MapMode.EAST);

    orlinn = CreateSprite(Map, "Orlinn", 76, 44, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);

    kalya = CreateSprite(Map, "Kalya", 77, 55, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.EAST);

    lilly = CreateNPCSprite(Map, "Woman3", vt_system.Translate("Lilly"), 95, 54, vt_map.MapMode.GROUND_OBJECT);
    lilly:SetDirection(vt_map.MapMode.EAST);

    carson = CreateSprite(Map, "Carson", 97, 61, vt_map.MapMode.GROUND_OBJECT);
    carson:SetDirection(vt_map.MapMode.NORTH);

    malta = CreateSprite(Map, "Malta", 95, 62, vt_map.MapMode.GROUND_OBJECT);
    malta:SetDirection(vt_map.MapMode.NORTH);

    brymir = CreateNPCSprite(Map, "Old Woman1", vt_system.Translate("Brymir"), 87, 61.5, vt_map.MapMode.GROUND_OBJECT);
    brymir:SetDirection(vt_map.MapMode.NORTH);

    martha = CreateNPCSprite(Map, "Woman1", vt_system.Translate("Martha"), 89, 61, vt_map.MapMode.GROUND_OBJECT);
    martha:SetDirection(vt_map.MapMode.NORTH);

    georges = CreateNPCSprite(Map, "Man1", vt_system.Translate("Georges"), 105, 61.5, vt_map.MapMode.GROUND_OBJECT);
    georges:SetDirection(vt_map.MapMode.NORTH);

    olivia = CreateNPCSprite(Map, "Girl1", vt_system.Translate("Olivia"), 107, 61, vt_map.MapMode.GROUND_OBJECT);
    olivia:SetDirection(vt_map.MapMode.NORTH);

    -- Create the soldiers
    soldier1 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 88, 59, vt_map.MapMode.GROUND_OBJECT);
    soldier1:SetDirection(vt_map.MapMode.SOUTH);
    soldier2 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 96, 59, vt_map.MapMode.GROUND_OBJECT);
    soldier2:SetDirection(vt_map.MapMode.SOUTH);
    soldier3 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 106, 59.5, vt_map.MapMode.GROUND_OBJECT);
    soldier3:SetDirection(vt_map.MapMode.SOUTH);
    soldier4 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 88, 55, vt_map.MapMode.GROUND_OBJECT);
    soldier4:SetDirection(vt_map.MapMode.WEST);
    soldier5 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 99, 59, vt_map.MapMode.GROUND_OBJECT);
    soldier5:SetDirection(vt_map.MapMode.SOUTH);
    soldier6 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 102.5, 59.2, vt_map.MapMode.GROUND_OBJECT);
    soldier6:SetDirection(vt_map.MapMode.SOUTH);
    soldier7 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 93, 59.3, vt_map.MapMode.GROUND_OBJECT);
    soldier7:SetDirection(vt_map.MapMode.SOUTH);
    soldier8 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 90.6, 59.6, vt_map.MapMode.GROUND_OBJECT);
    soldier8:SetDirection(vt_map.MapMode.SOUTH);
    soldier9 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 108, 57, vt_map.MapMode.GROUND_OBJECT);
    soldier9:SetDirection(vt_map.MapMode.WEST);
    soldier10 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 108, 54, vt_map.MapMode.GROUND_OBJECT);
    soldier10:SetDirection(vt_map.MapMode.WEST);
    soldier11 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 103, 50, vt_map.MapMode.GROUND_OBJECT);
    soldier11:SetDirection(vt_map.MapMode.SOUTH);
    soldier12 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 90, 50, vt_map.MapMode.GROUND_OBJECT);
    soldier12:SetDirection(vt_map.MapMode.SOUTH);
    soldier13 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 100, 48, vt_map.MapMode.GROUND_OBJECT);
    soldier13:SetDirection(vt_map.MapMode.SOUTH);
    soldier14 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 96, 48, vt_map.MapMode.GROUND_OBJECT);
    soldier14:SetDirection(vt_map.MapMode.SOUTH);
    soldier15 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 84, 53, vt_map.MapMode.GROUND_OBJECT);
    soldier15:SetDirection(vt_map.MapMode.EAST);
    soldier16 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 84, 57, vt_map.MapMode.GROUND_OBJECT);
    soldier16:SetDirection(vt_map.MapMode.EAST);

    lord = CreateNPCSprite(Map, "Lord", vt_system.Translate("Lord Banesore"), 105, 54, vt_map.MapMode.GROUND_OBJECT);
    lord:SetDirection(vt_map.MapMode.WEST);

    -- soldiers around kalya 77,55
    soldier17 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 79, 55, vt_map.MapMode.GROUND_OBJECT);
    soldier17:SetDirection(vt_map.MapMode.EAST);
    soldier18 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 77, 53, vt_map.MapMode.GROUND_OBJECT);
    soldier18:SetDirection(vt_map.MapMode.EAST);
    soldier19 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 77, 57, vt_map.MapMode.GROUND_OBJECT);
    soldier19:SetDirection(vt_map.MapMode.EAST);
    soldier20 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 75, 55, vt_map.MapMode.GROUND_OBJECT);
    soldier20:SetDirection(vt_map.MapMode.EAST);

    -- soldiers guarding the surroundings
    soldier21 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 95, 21, vt_map.MapMode.GROUND_OBJECT);
    soldier21:SetDirection(vt_map.MapMode.NORTH);
    event = vt_map.ChangeDirectionSpriteEvent.Create("Soldier21 looks north", soldier21, vt_map.MapMode.NORTH);
    event:AddEventLinkAtEnd("Soldier21 goes east", 3000);

    event = vt_map.PathMoveSpriteEvent.Create("Soldier21 goes east", soldier21, 98, 21, false);
    event:AddEventLinkAtEnd("Soldier21 goes back", 3000);

    event = vt_map.PathMoveSpriteEvent.Create("Soldier21 goes back", soldier21, 95, 21, false);
    event:AddEventLinkAtEnd("Soldier21 looks north");

    EventManager:StartEvent("Soldier21 goes east");

    soldier22 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 83, 28, vt_map.MapMode.GROUND_OBJECT);
    soldier22:SetDirection(vt_map.MapMode.WEST);
    event = vt_map.ChangeDirectionSpriteEvent.Create("Soldier22 looks north", soldier22, vt_map.MapMode.NORTH);
    event:AddEventLinkAtEnd("Soldier22 goes south", 3000);

    event = vt_map.PathMoveSpriteEvent.Create("Soldier22 goes south", soldier22, 83, 31, false);
    event:AddEventLinkAtEnd("Soldier22 looks west2");

    event = vt_map.ChangeDirectionSpriteEvent.Create("Soldier22 looks west2", soldier22, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Soldier22 goes back", 3000);

    event = vt_map.PathMoveSpriteEvent.Create("Soldier22 goes back", soldier22, 83, 28, false);
    event:AddEventLinkAtEnd("Soldier22 looks west");

    event = vt_map.ChangeDirectionSpriteEvent.Create("Soldier22 looks west", soldier22, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Soldier22 looks north", 3000);

    EventManager:StartEvent("Soldier22 goes south");
end

function _CreateObjects()
    local object = nil

    CreateObject(Map, "Tree Big2", 70, 6, vt_map.MapMode.GROUND_OBJECT);

    -- Add hill treasure chest
    local hill_chest = CreateTreasure(Map, "riverbank_secret_hill_chest", "Wood_Chest1", 72, 5, vt_map.MapMode.GROUND_OBJECT);
    hill_chest:AddItem(1, 1);

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

    -- Add burning flames halos and objects
    _CreateFire(84, 51.2);
    _CreateFire(83, 59);
    _CreateFire(97, 51);
    _CreateFire(111, 58);
    _CreateFire(109, 52);

    -- Ambient sounds
    vt_map.SoundObject.Create("sounds/gentle_stream.ogg", 61.0, 27.0, 20.0);
    vt_map.SoundObject.Create("sounds/gentle_stream.ogg", 61.0, 47.0, 20.0);
    vt_map.SoundObject.Create("sounds/gentle_stream.ogg", 61.0, 67.0, 20.0);
    vt_map.SoundObject.Create("sounds/gentle_stream.ogg", 81.0, 67.0, 20.0);
    vt_map.SoundObject.Create("sounds/gentle_stream.ogg", 101.0, 67.0, 20.0);

    -- Fireflies on water
    vt_map.ParticleObject.Create("data/visuals/particle_effects/fireflies.lua", 53, 29, vt_map.MapMode.GROUND_OBJECT);
    vt_map.ParticleObject.Create("data/visuals/particle_effects/fireflies.lua", 52, 38, vt_map.MapMode.GROUND_OBJECT);
    vt_map.ParticleObject.Create("data/visuals/particle_effects/fireflies.lua", 54, 40, vt_map.MapMode.GROUND_OBJECT);
    vt_map.ParticleObject.Create("data/visuals/particle_effects/fireflies.lua", 99, 73, vt_map.MapMode.GROUND_OBJECT);
end

-- creates the necessary objects to display some fire
function _CreateFire(fire_x, fire_y)
    -- Add burning flames halos and objects
    CreateObject(Map, "Campfire1", fire_x, fire_y, vt_map.MapMode.GROUND_OBJECT);

    vt_map.SoundObject.Create("sounds/campfire.ogg", fire_x, fire_y, 7.0);

    vt_map.Halo.Create("data/visuals/lights/torch_light_mask2.lua", fire_x, fire_y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    vt_map.Halo.Create("data/visuals/lights/sun_flare_light_main.lua", fire_x, fire_y + 1.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.2));
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Map change Events
    vt_map.MapTransitionEvent.Create("to Village center", "dat/maps/layna_village/layna_village_center_map.lua",
                                       "dat/maps/layna_village/layna_village_center_at_night_script.lua", "from_riverbank");

    -- generic events
    vt_map.ScriptedEvent.Create("Map:PushState(SCENE)", "Map_SceneState", "");
    vt_map.ScriptedEvent.Create("Map:PopState()", "Map_PopState", "");

    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks east", bronann, vt_map.MapMode.EAST);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Orlinn", bronann, orlinn);
    vt_map.AnimateSpriteEvent.Create("Bronann is frightened by Orlinn", bronann, "frightened", -1); -- defaut time

    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Bronann", orlinn, bronann);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks east", orlinn, vt_map.MapMode.EAST);

    vt_map.ChangeDirectionSpriteEvent.Create("Banesore looks south", lord, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Banesore looks west", lord, vt_map.MapMode.WEST);

    -- Bronann can't go back event
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I can't go back now.");
    dialogue:AddLine(text, bronann);
    vt_map.DialogueEvent.Create("Bronann can't go back", dialogue);

    -- Bronann hides when entering the map
    event = vt_map.ScriptedEvent.Create("Bronann arrives at the riverbank dialogue", "arrival_at_riverbank_dialogue_start", "");
    event:AddEventLinkAtEnd("Bronann hides behind the trees", 50);

    event = vt_map.PathMoveSpriteEvent.Create("Bronann hides behind the trees", bronann, 92.5, 6.0, false);
    event:AddEventLinkAtEnd("Bronann looks south");
    event:AddEventLinkAtEnd("Bronann thinks he needs to get closer");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Guards are carefully watching their surroundings. I need to get closer to see what's happening.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("I should be able to sneak through when they're not watching.");
    dialogue:AddLine(text, bronann);
    event = vt_map.DialogueEvent.Create("Bronann thinks he needs to get closer", dialogue);
    event:AddEventLinkAtEnd("End of bronann arrival at the riverbank");

    vt_map.ScriptedEvent.Create("End of bronann arrival at the riverbank", "arrival_at_riverbank_dialogue_end", "");

    -- Bronann is catched
    event = vt_map.ScriptedEvent.Create("Bronann is catched start", "bronann_is_catched_start", "");
    event:AddEventLinkAtEnd("The soldier21 looks at Bronann");
    event:AddEventLinkAtEnd("The soldier22 looks at Bronann");
    event:AddEventLinkAtEnd("The soldier yells at Bronann");
    event:AddEventLinkAtEnd("Bronann is surprised");

    vt_map.LookAtSpriteEvent.Create("The soldier21 looks at Bronann", soldier21, bronann);
    vt_map.LookAtSpriteEvent.Create("The soldier22 looks at Bronann", soldier22, bronann);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Who's there?");
    dialogue:AddLine(text, soldier21);
    event = vt_map.DialogueEvent.Create("The soldier yells at Bronann", dialogue);
    event:AddEventLinkAtEnd("Restart map");

    vt_map.AnimateSpriteEvent.Create("Bronann is surprised", bronann, "frightened_fixed", 0); -- 0 means forever

    vt_map.MapTransitionEvent.Create("Restart map", "dat/maps/layna_village/layna_village_riverbank_map.lua",
                                     "dat/maps/layna_village/layna_village_riverbank_at_night_script.lua", "from_village_center");

    -- Lord battle scene
    event = vt_map.ScriptedEvent.Create("Lord battle dialogue start", "lord_battle_scene_start", "");
    event:AddEventLinkAtEnd("Bronann hides behind the trees near Orlinn", 50);

    event = vt_map.PathMoveSpriteEvent.Create("Bronann hides behind the trees near Orlinn", bronann, 70.0, 47.0, true);
    event:AddEventLinkAtEnd("Bronann and Orlinn are discussing");

    vt_map.PathMoveSpriteEvent.Create("Orlinn goes near Bronann", orlinn, 71.0, 43.8, false);
    vt_map.PathMoveSpriteEvent.Create("Orlinn hides again", orlinn, 76.0, 43.8, false);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("(Whispering) Kalya!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("(What's happening here?)");
    dialogue:AddLineEvent(text, bronann, "Orlinn goes near Bronann", "");
    text = vt_system.Translate("(Also whispering) I don't know.");
    dialogue:AddLineEvent(text, orlinn, "Orlinn looks at Bronann", "Bronann looks at Orlinn");
    text = vt_system.Translate("(Woah, you scared me. Are you mad? You could have got us both caught!)");
    dialogue:AddLineEvent(text, bronann, "Bronann is frightened by Orlinn", "");
    text = vt_system.Translate("(I thought you saw me.)");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("(Kalya was brought here by those weirdos. They are holding everyone captive. Lilly is talking with that big guy. He gives me the chills.)");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("(Let's have a look.)");
    dialogue:AddLine(text, bronann);
    event = vt_map.DialogueEvent.Create("Bronann and Orlinn are discussing", dialogue);
    event:AddEventLinkAtEnd("Bronann looks east");
    event:AddEventLinkAtEnd("Orlinn hides again");
    event:AddEventLinkAtEnd("Set Camera on Lilly");

    event = vt_map.ScriptedSpriteEvent.Create("Set Camera on Lilly", lilly, "set_camera_on", "set_camera_update");
    event:AddEventLinkAtEnd("Dialogue with the Lord");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I already told you, again and again, Banesore. I don't know anything about such a crystal.");
    dialogue:AddLine(text, lilly);
    text = vt_system.Translate("It is 'Lord Banesore', you scum!");
    dialogue:AddLine(text, soldier1);
    text = vt_system.Translate("Even when confronted by someone who can wipe out everything you hold dear, you dare to stare me in the eyes? I admire such a courageous heart, my dear one.");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("Therefore, I'm inclined to ask these questions one last time:");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("Where is the crystal? Where is the boy?");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("It's time to think Lilly.");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("Huh? How do you know my name?");
    dialogue:AddLineEmote(text, lilly, "exclamation");
    text = vt_system.Translate("Your very soul is slowly but surely opening. I can feel your sorrow and doubts grow.");
    dialogue:AddLineEvent(text, lord, "Banesore moves closer to Lilly 1", "");
    text = vt_system.Translate("Fight it, Lilly! Focus!");
    dialogue:AddLineEmote(text, carson, "exclamation");
    text = vt_system.Translate("Keep quiet, you old daft rat!");
    dialogue:AddLine(text, soldier1);
    text = vt_system.Translate("Oh! My apologies, it seems you weren't lying.");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("The fact you didn't mention anything about the boy leads me to believe that you know him.");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("...");
    dialogue:AddLine(text, lilly);
    text = vt_system.Translate("Ah? So he must be close.");
    dialogue:AddLineEvent(text, lord, "Banesore moves closer to Lilly 2", "");
    text = vt_system.Translate("What? No!");
    dialogue:AddLineEmote(text, malta, "exclamation");
    text = vt_system.Translate("Oh, I see now. So this is her mother. Bring her here, and kill her!");
    dialogue:AddLineEvent(text, lord, "Banesore looks south", "");
    event = vt_map.DialogueEvent.Create("Dialogue with the Lord", dialogue);
    event:AddEventLinkAtEnd("All villagers are surprised");

    vt_map.PathMoveSpriteEvent.Create("Banesore moves closer to Lilly 1", lord, 103.0, 54.0, false);
    vt_map.PathMoveSpriteEvent.Create("Banesore moves closer to Lilly 2", lord, 101.0, 54.0, false);

    event = vt_map.ScriptedEvent.Create("All villagers are surprised", "exclamation_all_villagers", "");
    event:AddEventLinkAtEnd("Carson protects Malta", 1000);
    event:AddEventLinkAtEnd("Set Camera on Carson");

    event = vt_map.PathMoveSpriteEvent.Create("Carson protects Malta", carson, 95.0, 60.5, false);
    event:AddEventLinkAtEnd("Carson looks north");

    vt_map.ChangeDirectionSpriteEvent.Create("Carson looks north", carson, vt_map.MapMode.NORTH);

    event = vt_map.ScriptedSpriteEvent.Create("Set Camera on Carson", carson, "set_camera_on", "set_camera_update");
    event:AddEventLinkAtEnd("Dialogue with the Lord 2");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Let her out of this! You can have me instead.");
    dialogue:AddLine(text, carson);
    text = vt_system.Translate("So, here we have a gentleman! I'll start with you, then.");
    dialogue:AddLineEvent(text, lord, "Banesore looks south", "");
    text = vt_system.Translate("Fine.");
    dialogue:AddLine(text, carson);
    text = vt_system.Translate("But she will be the next one.");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("You filthy demon!");
    dialogue:AddLineEmote(text, carson, "exclamation");
    text = vt_system.Translate("It's fine, I'll come.");
    dialogue:AddLine(text, malta);
    text = vt_system.Translate("No!");
    dialogue:AddLine(text, bronann);
    event = vt_map.DialogueEvent.Create("Dialogue with the Lord 2", dialogue);
    event:AddEventLinkAtEnd("Set Camera on Bronann");
    event:AddEventLinkAtEnd("Banesore looks west");
    event:AddEventLinkAtEnd("Bronann runs to the soldiers 1");
    event:AddEventLinkAtEnd("Exclamation of all soldiers");
    event:AddEventLinkAtEnd("All villagers are surprised 2");
    event:AddEventLinkAtEnd("All people look at Bronann");

    vt_map.ScriptedEvent.Create("Exclamation of all soldiers", "exclamation_all_soldiers", "");

    vt_map.ScriptedEvent.Create("All villagers are surprised 2", "exclamation_all_villagers", "");

    vt_map.ScriptedEvent.Create("All people look at Bronann", "all_people_look_at_bronann", "");

    vt_map.ScriptedSpriteEvent.Create("Set Camera on Bronann", bronann, "set_camera_on", "");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann runs to the soldiers 1", bronann, 85.0, 49.0, true);
    event:AddEventLinkAtEnd("Bronann runs to the soldiers 2");
    event:AddEventLinkAtEnd("All people look at Bronann");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann runs to the soldiers 2", bronann, 91.0, 54.0, true);
    event:AddEventLinkAtEnd("Bronann looks east");
    event:AddEventLinkAtEnd("All people look at Bronann");
    event:AddEventLinkAtEnd("Dialogue with the Lord 3");

    vt_map.PathMoveSpriteEvent.Create("A soldier goes closer to stop Bronann", soldier12, 91.0, 52.0, false);

    event = vt_map.PathMoveSpriteEvent.Create("The soldier goes back to his place", soldier12, 90.0, 50.0, false);
    event:AddEventLinkAtEnd("The soldier looks south");

    vt_map.ChangeDirectionSpriteEvent.Create("The soldier looks south", soldier12, vt_map.MapMode.SOUTH);

    vt_map.PathMoveSpriteEvent.Create("Lilly goes near Bronann", lilly, 93.0, 54.0, false);

    vt_map.ChangeDirectionSpriteEvent.Create("Lilly looks east", lilly, vt_map.MapMode.EAST);

    event = vt_map.PathMoveSpriteEvent.Create("Lilly let Bronann advance", lilly, 93.0, 50.0, false);
    event:AddEventLinkAtEnd("Lilly looks south");

    vt_map.ChangeDirectionSpriteEvent.Create("Lilly looks south", lilly, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Lilly looks west", lilly, vt_map.MapMode.WEST);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("At last, the boy.");
    dialogue:AddLineEventEmote(text, lord, "", "A soldier goes closer to stop Bronann", "exclamation");
    text = vt_system.Translate("No! Let him come closer.");
    dialogue:AddLineEvent(text, lord, "", "The soldier goes back to his place");
    text = vt_system.Translate("Bronann!");
    dialogue:AddLine(text, malta);
    text = vt_system.Translate("Bronann! No!");
    dialogue:AddLine(text, carson);
    text = vt_system.Translate("Bronann, why?");
    dialogue:AddLineEvent(text, lilly, "Lilly goes near Bronann", "");
    text = vt_system.Translate("Clear the way, Lilly. I need to speak to him.");
    dialogue:AddLineEvent(text, lord, "", "Lilly looks east");
    text = vt_system.Translate("Let's have a talk, Bronann!");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("I don't have any other choice, Lilly.");
    dialogue:AddLineEvent(text, bronann, "Lilly looks west", "");
    text = vt_system.Translate("Bronann...");
    dialogue:AddLineEvent(text, lilly, "", "Lilly let Bronann advance");
    event = vt_map.DialogueEvent.Create("Dialogue with the Lord 3", dialogue);
    event:AddEventLinkAtEnd("Bronann goes closer from Banesore", 1000);

    event = vt_map.PathMoveSpriteEvent.Create("Bronann goes closer from Banesore", bronann, 93.0, 54.0, false);
    event:AddEventLinkAtEnd("Dialogue with the Lord 4");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("You, after all these years...");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("Let everyone go. We don't have your 'crystal'.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Of course you have it. I can sense it within your flesh.");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("So, the crystal is inside me?");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("Now, show it to me.");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("But I...");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("Do you need some kind of motivation? If so, let me handle this with my greatest pleasure!");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("Prepare yourself!");
    dialogue:AddLine(text, lord);
    event = vt_map.DialogueEvent.Create("Dialogue with the Lord 4", dialogue);
    event:AddEventLinkAtEnd("Battle with Banesore");

    event = vt_map.BattleEncounterEvent.Create("Battle with Banesore");
    event:SetMusic("music/the_recon_mission.ogg");
    event:SetBackground("img/backdrops/battle/mountain_village_single_house.png");
    event:AddEnemy(10, 640, 575);
    event:SetBoss(true);
    event:AddScript("dat/maps/layna_village/battle_with_banesore/battle_with_banesore_script.lua");
    event:AddScript("dat/maps/common/at_night.lua");
    event:AddEventLinkAtEnd("Soldier4 is KO");
    event:AddEventLinkAtEnd("Soldier15 is KO");
    event:AddEventLinkAtEnd("Soldier16 is KO");
    event:AddEventLinkAtEnd("Place Herth next to Bronann");
    event:AddEventLinkAtEnd("Bronann is looking poor");
    event:AddEventLinkAtEnd("Show both crystals", 2000);

    -- The after-fight scene
    -- The soldiers 4, 15 and 16 have been ko'd by Herth
    vt_map.AnimateSpriteEvent.Create("Soldier4 is KO", soldier4, "ko", 0); -- 0 means forever
    vt_map.AnimateSpriteEvent.Create("Soldier15 is KO", soldier15, "ko", 0); -- 0 means forever
    vt_map.AnimateSpriteEvent.Create("Soldier16 is KO", soldier16, "ko", 0); -- 0 means forever

    -- Herth is in front of Bronann, protecting him from Banesore
    vt_map.ScriptedEvent.Create("Place Herth next to Bronann", "place_herth_next_to_bronann", "");

    -- Bronann is hurt by the summoned Tear
    vt_map.AnimateSpriteEvent.Create("Bronann is looking poor", bronann, "kneeling", 0); -- 0 means forever

    event = vt_map.ScriptedEvent.Create("Show both crystals", "show_both_crystals", "show_both_crystals_update");
    event:AddEventLinkAtEnd("Dialogue after crystals appearance");

    vt_map.LookAtSpriteEvent.Create("Herth looks at Kalya", herth, kalya);
    vt_map.LookAtSpriteEvent.Create("Herth looks at Banesore", herth, lord);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Ah, there it is. Finally!");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("Kalya! Now!");
    dialogue:AddLineEvent(text, herth, "Herth looks at Kalya", "Herth looks at Banesore");
    event = vt_map.DialogueEvent.Create("Dialogue after crystals appearance", dialogue);
    event:AddEventLinkAtEnd("Kalya uses smoke to flee with Bronann");

    event = vt_map.AnimateSpriteEvent.Create("Kalya uses smoke to flee with Bronann", kalya, "kneeling", 1000);
    event:AddEventLinkAtEnd("Kalya uses smoke to flee with Bronann 2");
    event:AddEventLinkAtEnd("Kalya, Bronann and Orlinn disappear", 600);

    vt_map.ScriptedEvent.Create("Kalya, Bronann and Orlinn disappear", "make_bronann_orlinn_kalya_disappear", "");

    -- All soldiers, except the one on the ground
    vt_map.ScriptedEvent.Create("Exclamation of all soldiers 2", "exclamation_all_soldiers_2", "");

    event = vt_map.ScriptedEvent.Create("Kalya uses smoke to flee with Bronann 2", "smoke_event_start", "smoke_event_update");
    event:AddEventLinkAtEnd("Exclamation of all soldiers 2");
    event:AddEventLinkAtEnd("Dialogue between Herth and Banesore");
    event:AddEventLinkAtEnd("Soldier17 looks at Kalya");
    event:AddEventLinkAtEnd("Soldier18 looks at Kalya", 150);
    event:AddEventLinkAtEnd("Soldier19 looks at Kalya", 300);
    event:AddEventLinkAtEnd("Soldier20 looks at Kalya", 400);
    event:AddEventLinkAtEnd("Soldiers wonders where Kalya is", 500);

    -- The soldiers guarding Kalya wonder whe she is...
    vt_map.LookAtSpriteEvent.Create("Soldier17 looks at Kalya", soldier17, kalya);
    vt_map.LookAtSpriteEvent.Create("Soldier18 looks at Kalya", soldier18, kalya);
    vt_map.LookAtSpriteEvent.Create("Soldier19 looks at Kalya", soldier19, kalya);
    vt_map.LookAtSpriteEvent.Create("Soldier20 looks at Kalya", soldier20, kalya);

    vt_map.ScriptedEvent.Create("Soldiers wonders where Kalya is", "interrogation_soldiers_guarding_kalya", "");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("You won't get him, Banesore! It seems we were smarter than you this time.");
    dialogue:AddLine(text, herth);
    text = vt_system.Translate("Ah ah ah, but this is exactly what I wanted.");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("What?!");
    dialogue:AddLineEmote(text, herth, "exclamation");
    text = vt_system.Translate("After all, I simply wanted to make sure that boy would reach Mt. Elbrus safe and sound.");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("He is the only one that can get what I'm looking for.");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("Soldiers! Give them a few minutes before starting the chase.");
    dialogue:AddLineEvent(text, lord, "Banesore looks south", "");
    text = vt_system.Translate("Yes, my Lord!");
    dialogue:AddLine(text, soldier1);
    text = vt_system.Translate("Why the hell...");
    dialogue:AddLineEmote(text, herth, "exclamation");
    text = vt_system.Translate("...am I telling you all that? It's simple.");
    dialogue:AddLineEvent(text, lord, "Banesore looks west", "");
    text = vt_system.Translate("But it doesn't matter, as you're all going to die now.");
    dialogue:AddLine(text, lord);
    event = vt_map.DialogueEvent.Create("Dialogue between Herth and Banesore", dialogue);
    event:AddEventLinkAtEnd("To Kalya house path scene");

    vt_map.MapTransitionEvent.Create("To Kalya house path scene", "dat/maps/layna_village/layna_village_kalya_house_path_map.lua",
                                     "dat/maps/layna_village/layna_village_kalya_house_path_at_night_script.lua", "from_riverbank_at_night_scene");
end

-- zones
local village_center_zone = nil
local to_village_entrance_zone = nil
local to_riverbank_house_entrance_zone = nil

local soldier21_watching_zone = nil
local soldier21_watching_right_zone = nil
local soldier22_watching_zone = nil
local soldier22_watching_north_zone = nil
local soldier22_watching_west_zone = nil

local battle_dialogue_start_zone = nil

function _CreateZones()
    -- N.B.: left, right, top, bottom
    village_center_zone = vt_map.CameraZone.Create(89, 105, 0, 2);
    to_village_entrance_zone = vt_map.CameraZone.Create(118, 119, 10, 27);
    to_riverbank_house_entrance_zone = vt_map.CameraZone.Create(96, 100, 46, 47);

    soldier21_watching_zone = vt_map.CameraZone.Create(84, 88, 12, 22);
    soldier21_watching_zone:AddSection(88, 94, 9, 22);
    soldier21_watching_zone:AddSection(95, 105, 3, 14);

    soldier21_watching_right_zone = vt_map.CameraZone.Create(94, 105, 14, 22);
    soldier21_watching_right_zone:AddSection(105, 120, 9, 24);

    soldier22_watching_zone = vt_map.CameraZone.Create(74, 85, 17, 38);
    soldier22_watching_north_zone = vt_map.CameraZone.Create(78, 80, 12, 17);
    soldier22_watching_west_zone = vt_map.CameraZone.Create(64, 74, 20, 32);
    battle_dialogue_start_zone = vt_map.CameraZone.Create(64, 86, 39, 40);
end

function _CheckZones()
    if (village_center_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        if (GlobalManager:GetEventValue("story", "layna_village_arrival_at_riverbank_done") ~= 1) then
            EventManager:StartEvent("to Village center");
        else
            EventManager:StartEvent("Bronann can't go back");
        end
    elseif (soldier21_watching_right_zone:IsCameraEntering() == true) then
        EventManager:StartEvent("Bronann is catched start");
    elseif (soldier21_watching_zone:IsCameraInside() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        if (GlobalManager:GetEventValue("story", "layna_village_arrival_at_riverbank_done") == 1
                and soldier21:GetDirection() == vt_map.MapMode.NORTH) then
            EventManager:StartEvent("Bronann is catched start");
        end
    elseif (soldier22_watching_zone:IsCameraEntering() == true) then
        EventManager:StartEvent("Bronann is catched start");
    elseif (soldier22_watching_north_zone:IsCameraInside() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        if (soldier22:GetDirection() == vt_map.MapMode.NORTH) then
            EventManager:StartEvent("Bronann is catched start");
        end
    elseif (soldier22_watching_west_zone:IsCameraInside() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        if (soldier22:GetDirection() == vt_map.MapMode.WEST) then
            EventManager:StartEvent("Bronann is catched start");
        end
    elseif (battle_dialogue_start_zone:IsCameraEntering() == true) then
        EventManager:StartEvent("Lord battle dialogue start");
    end
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

    set_camera_on = function(sprite)
        Map:SetCamera(sprite, 800);
    end,

    set_camera_update = function()
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        return true;
    end,

    arrival_at_riverbank_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

    arrival_at_riverbank_dialogue_end = function()
        -- Set event as done
        GlobalManager:SetEventValue("story", "layna_village_arrival_at_riverbank_done", 1);
        Map:PopState();
    end,

    bronann_is_catched_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        bronann:SetMoving(false);
        EventManager:EndAllEvents(soldier21);
        EventManager:EndAllEvents(soldier22);
        soldier21:Emote("exclamation", soldier21:GetDirection());
        soldier22:Emote("exclamation", soldier22:GetDirection());
        -- Undo the last event, as the map is restarting
        GlobalManager:SetEventValue("story", "layna_village_arrival_at_riverbank_done", 0);
    end,

    lord_battle_scene_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

    exclamation_all_villagers = function()
        kalya:Emote("exclamation", kalya:GetDirection());
        herth:Emote("exclamation", herth:GetDirection());
        lilly:Emote("exclamation", lilly:GetDirection());
        carson:Emote("exclamation", carson:GetDirection());
        malta:Emote("exclamation", malta:GetDirection());
        brymir:Emote("exclamation", brymir:GetDirection());
        martha:Emote("exclamation", martha:GetDirection());
        georges:Emote("exclamation", georges:GetDirection());
        olivia:Emote("exclamation", olivia:GetDirection());
    end,

    exclamation_all_soldiers = function()
        soldier1:Emote("exclamation", soldier1:GetDirection());
        soldier2:Emote("exclamation", soldier2:GetDirection());
        soldier3:Emote("exclamation", soldier3:GetDirection());
        soldier4:Emote("exclamation", soldier4:GetDirection());
        soldier5:Emote("exclamation", soldier5:GetDirection());
        soldier6:Emote("exclamation", soldier6:GetDirection());
        soldier7:Emote("exclamation", soldier7:GetDirection());
        soldier8:Emote("exclamation", soldier8:GetDirection());
        soldier9:Emote("exclamation", soldier9:GetDirection());
        soldier10:Emote("exclamation", soldier10:GetDirection());
        soldier11:Emote("exclamation", soldier11:GetDirection());
        soldier12:Emote("exclamation", soldier12:GetDirection());
        soldier13:Emote("exclamation", soldier13:GetDirection());
        soldier14:Emote("exclamation", soldier14:GetDirection());
        soldier15:Emote("exclamation", soldier15:GetDirection());
        soldier16:Emote("exclamation", soldier16:GetDirection());
    end,

    all_people_look_at_bronann = function()
        soldier1:LookAt(bronann);
        soldier2:LookAt(bronann);
        soldier3:LookAt(bronann);
        soldier4:LookAt(bronann);
        soldier5:LookAt(bronann);
        soldier6:LookAt(bronann);
        soldier7:LookAt(bronann);
        soldier8:LookAt(bronann);
        soldier9:LookAt(bronann);
        soldier10:LookAt(bronann);
        soldier11:LookAt(bronann);
        soldier12:LookAt(bronann);
        soldier13:LookAt(bronann);
        soldier14:LookAt(bronann);
        soldier15:LookAt(bronann);
        soldier16:LookAt(bronann);
        soldier17:LookAt(bronann);
        soldier18:LookAt(bronann);
        soldier19:LookAt(bronann);
        soldier20:LookAt(bronann);

        kalya:LookAt(bronann);
        herth:LookAt(bronann);
        malta:LookAt(bronann);
        carson:LookAt(bronann);
        lilly:LookAt(bronann);
        brymir:LookAt(bronann);
        martha:LookAt(bronann);
        georges:LookAt(bronann);
        olivia:LookAt(bronann);
    end,

    place_herth_next_to_bronann = function()
        herth:SetPosition(95.0, 55.0);
    end,

    show_both_crystals = function()
        -- Triggers the crystal appearance
        GlobalManager:SetEventValue("scripts_events", "layna_village_riverbank_show_crystals", 1)
    end,

    show_both_crystals_update = function()
        if (GlobalManager:GetEventValue("scripts_events", "layna_village_riverbank_show_crystals") == 0) then
            return true;
        end
        return false;
    end,

    smoke_event_start = function()
        -- Triggers the smoke
        GlobalManager:SetEventValue("scripts_events", "layna_village_riverbank_smoke", 1)
    end,

    smoke_event_update = function()
        if (GlobalManager:GetEventValue("scripts_events", "layna_village_riverbank_smoke") == 0) then
            return true;
        end
        return false;
    end,

    exclamation_all_soldiers_2 = function()
        soldier1:Emote("exclamation", soldier1:GetDirection());
        soldier2:Emote("exclamation", soldier2:GetDirection());
        soldier3:Emote("exclamation", soldier3:GetDirection());
        soldier5:Emote("exclamation", soldier5:GetDirection());
        soldier6:Emote("exclamation", soldier6:GetDirection());
        soldier7:Emote("exclamation", soldier7:GetDirection());
        soldier8:Emote("exclamation", soldier8:GetDirection());
        soldier9:Emote("exclamation", soldier9:GetDirection());
        soldier10:Emote("exclamation", soldier10:GetDirection());
        soldier11:Emote("exclamation", soldier11:GetDirection());
        soldier12:Emote("exclamation", soldier12:GetDirection());
        soldier13:Emote("exclamation", soldier13:GetDirection());
        soldier14:Emote("exclamation", soldier14:GetDirection());
    end,

    make_bronann_orlinn_kalya_disappear = function()
        bronann:SetVisible(false);
        kalya:SetVisible(false);
        orlinn:SetVisible(false);
        herth:SetPosition(92.0, 54.0);
    end,

    interrogation_soldiers_guarding_kalya = function()
        soldier17:Emote("interrogation", soldier17:GetDirection());
        soldier18:Emote("interrogation", soldier18:GetDirection());
        soldier19:Emote("interrogation", soldier19:GetDirection());
        soldier20:Emote("interrogation", soldier20:GetDirection());
    end
}
