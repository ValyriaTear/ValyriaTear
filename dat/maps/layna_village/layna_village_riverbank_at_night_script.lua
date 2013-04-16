-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_riverbank_at_night_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = "Riverbank"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/Welcome to Com-Mecha-Mattew_Pablo_OGA.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

local bronann = {};

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
    _CreateZones();

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");

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
    bronann = CreateSprite(Map, "Bronann", 97, 4);
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
        AudioManager:PlaySound("snd/door_close.wav");
    end

    Map:AddGroundObject(bronann);
end

local herth = {};
local orlinn = {};
local kalya = {};
local lilly = {};
local carson = {};
local malta = {};
local brymir = {};
local martha = {};
local georges = {};
local olivia = {};

local soldier1 = {};
local soldier2 = {};
local soldier3 = {};
local soldier4 = {};
local soldier5 = {};
local soldier6 = {};
local soldier7 = {};
local soldier8 = {};
local soldier9 = {};
local soldier10 = {};
local soldier11 = {};
local soldier12 = {};
local soldier13 = {};
local soldier14 = {};
local soldier15 = {};
local soldier16 = {};

local soldier17 = {};
local soldier18 = {};
local soldier19 = {};
local soldier20 = {};

-- Soldiers guarding the surroundings
local soldier21 = {};
local soldier22 = {};

local lord = {};

function _CreateNPCs()
    local text = {}
    local dialogue = {}
    local event = {}

    herth = CreateSprite(Map, "Herth", 86, 55);
    herth:SetDirection(vt_map.MapMode.EAST);
    Map:AddGroundObject(herth);

    orlinn = CreateSprite(Map, "Orlinn", 76, 44);
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);
    Map:AddGroundObject(orlinn);

    kalya = CreateSprite(Map, "Kalya", 77, 55);
    kalya:SetDirection(vt_map.MapMode.EAST);
    Map:AddGroundObject(kalya);

    lilly = CreateNPCSprite(Map, "Woman3", "Lilly", 95, 54);
    lilly:SetDirection(vt_map.MapMode.EAST);
    Map:AddGroundObject(lilly);

    carson = CreateSprite(Map, "Carson", 97, 61);
    carson:SetDirection(vt_map.MapMode.NORTH);
    Map:AddGroundObject(carson);

    malta = CreateSprite(Map, "Malta", 95, 62);
    malta:SetDirection(vt_map.MapMode.NORTH);
    Map:AddGroundObject(malta);

    brymir = CreateNPCSprite(Map, "Old Woman1", "Brymir", 87, 61.5);
    brymir:SetDirection(vt_map.MapMode.NORTH);
    Map:AddGroundObject(brymir);

    martha = CreateNPCSprite(Map, "Woman1", "Martha", 89, 61);
    martha:SetDirection(vt_map.MapMode.NORTH);
    Map:AddGroundObject(martha);

    georges = CreateNPCSprite(Map, "Man1", "Georges", 105, 61.5);
    georges:SetDirection(vt_map.MapMode.NORTH);
    Map:AddGroundObject(georges);

    olivia = CreateNPCSprite(Map, "Girl1", "Olivia", 107, 61);
    olivia:SetDirection(vt_map.MapMode.NORTH);
    Map:AddGroundObject(olivia);

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
    soldier5 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 99, 59);
    soldier5:SetDirection(vt_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier5);
    soldier6 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 102.5, 59.2);
    soldier6:SetDirection(vt_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier6);
    soldier7 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 93, 59.3);
    soldier7:SetDirection(vt_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier7);
    soldier8 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 90.6, 59.6);
    soldier8:SetDirection(vt_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier8);
    soldier9 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 108, 57);
    soldier9:SetDirection(vt_map.MapMode.WEST);
    Map:AddGroundObject(soldier9);
    soldier10 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 108, 54);
    soldier10:SetDirection(vt_map.MapMode.WEST);
    Map:AddGroundObject(soldier10);
    soldier11 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 103, 50);
    soldier11:SetDirection(vt_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier11);
    soldier12 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 90, 50);
    soldier12:SetDirection(vt_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier12);
    soldier13 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 100, 48);
    soldier13:SetDirection(vt_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier13);
    soldier14 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 96, 48);
    soldier14:SetDirection(vt_map.MapMode.SOUTH);
    Map:AddGroundObject(soldier14);
    soldier15 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 84, 53);
    soldier15:SetDirection(vt_map.MapMode.EAST);
    Map:AddGroundObject(soldier15);
    soldier16 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 84, 57);
    soldier16:SetDirection(vt_map.MapMode.EAST);
    Map:AddGroundObject(soldier16);

    lord = CreateNPCSprite(Map, "Lord", "Lord Banesore", 100, 54);
    lord:SetDirection(vt_map.MapMode.WEST);
    Map:AddGroundObject(lord);

    -- soldiers around kalya 77,55
    soldier17 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 79, 55);
    soldier17:SetDirection(vt_map.MapMode.EAST);
    Map:AddGroundObject(soldier17);
    soldier18 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 77, 53);
    soldier18:SetDirection(vt_map.MapMode.EAST);
    Map:AddGroundObject(soldier18);
    soldier19 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 77, 57);
    soldier19:SetDirection(vt_map.MapMode.EAST);
    Map:AddGroundObject(soldier19);
    soldier20 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 75, 55);
    soldier20:SetDirection(vt_map.MapMode.EAST);
    Map:AddGroundObject(soldier20);

    -- soldiers guarding the surroundings
    soldier21 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 95, 21);
    soldier21:SetDirection(vt_map.MapMode.NORTH);
    Map:AddGroundObject(soldier21);
    event = vt_map.ChangeDirectionSpriteEvent("Soldier21 looks north", soldier21, vt_map.MapMode.NORTH);
    event:AddEventLinkAtEnd("Soldier21 goes east", 3000);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Soldier21 goes east", soldier21, 98, 21, false);
    event:AddEventLinkAtEnd("Soldier21 goes back", 3000);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Soldier21 goes back", soldier21, 95, 21, false);
    event:AddEventLinkAtEnd("Soldier21 looks north");
    EventManager:RegisterEvent(event);
    EventManager:StartEvent("Soldier21 goes east");

    soldier22 = CreateNPCSprite(Map, "Dark Soldier", "Soldier", 83, 28);
    soldier22:SetDirection(vt_map.MapMode.WEST);
    Map:AddGroundObject(soldier22);
    event = vt_map.ChangeDirectionSpriteEvent("Soldier22 looks north", soldier22, vt_map.MapMode.NORTH);
    event:AddEventLinkAtEnd("Soldier22 goes south", 3000);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Soldier22 goes south", soldier22, 83, 31, false);
    event:AddEventLinkAtEnd("Soldier22 looks west2");
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Soldier22 looks west2", soldier22, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Soldier22 goes back", 3000);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Soldier22 goes back", soldier22, 83, 28, false);
    event:AddEventLinkAtEnd("Soldier22 looks west");
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Soldier22 looks west", soldier22, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Soldier22 looks north", 3000);
    EventManager:RegisterEvent(event);
    EventManager:StartEvent("Soldier22 goes south");

end

function _CreateObjects()
    local object = {}

    object = CreateObject(Map, "Tree Big2", 70, 6);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Add hill treasure chest
    local hill_chest = CreateTreasure(Map, "riverbank_secret_hill_chest", "Wood_Chest1", 72, 5);
    if (hill_chest ~= nil) then
        hill_chest:AddObject(1, 1);
        Map:AddGroundObject(hill_chest);
    end

    -- trees around the house
    object = CreateObject(Map, "Tree Big2", 92, 10);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Small1", 82, 17);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 75, 20);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 72, 35);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big1", 74, 48);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Tree Big2", 76, 50);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Barrel1", 56.3, 56.5);
    if (object ~= nil) then Map:AddGroundObject(object) end;

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
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(object);
    end

    -- Add burning flames halos and objects
    _CreateFire(84, 51.2);
    _CreateFire(83, 59);
    _CreateFire(97, 51);
    _CreateFire(111, 58);
    _CreateFire(109, 52);

    -- Ambient sounds
    object = vt_map.SoundObject("snd/gentle_stream.ogg", 61.0, 27.0, 20.0);
    Map:AddAmbientSoundObject(object);

    object = vt_map.SoundObject("snd/gentle_stream.ogg", 61.0, 47.0, 20.0);
    Map:AddAmbientSoundObject(object);

    object = vt_map.SoundObject("snd/gentle_stream.ogg", 61.0, 67.0, 20.0);
    Map:AddAmbientSoundObject(object);

    object = vt_map.SoundObject("snd/gentle_stream.ogg", 81.0, 67.0, 20.0);
    Map:AddAmbientSoundObject(object);

    object = vt_map.SoundObject("snd/gentle_stream.ogg", 101.0, 67.0, 20.0);
    Map:AddAmbientSoundObject(object);

    -- Fireflies on water
    object = vt_map.ParticleObject("dat/effects/particles/fireflies.lua", 53, 29);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);

    object = vt_map.ParticleObject("dat/effects/particles/fireflies.lua", 52, 38);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);
    object = vt_map.ParticleObject("dat/effects/particles/fireflies.lua", 54, 40);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);
    object = vt_map.ParticleObject("dat/effects/particles/fireflies.lua", 99, 73);
    object:SetObjectID(Map.object_supervisor:GenerateObjectID());
    Map:AddGroundObject(object);
end

-- creates the necessary objects to display some fire
function _CreateFire(fire_x, fire_y)
    -- Add burning flames halos and objects
    local object = CreateObject(Map, "Campfire1", fire_x, fire_y);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = vt_map.SoundObject("snd/campfire.ogg", fire_x, fire_y, 7.0);
    if (object ~= nil) then Map:AddAmbientSoundObject(object) end;

    Map:AddHalo("img/misc/lights/torch_light_mask2.lua", fire_x, fire_y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    Map:AddHalo("img/misc/lights/sun_flare_light_main.lua", fire_x, fire_y + 1.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.2));
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- Map change Events
    event = vt_map.MapTransitionEvent("to Village center", "dat/maps/layna_village/layna_village_center_map.lua",
                                       "dat/maps/layna_village/layna_village_center_at_night_script.lua", "from_riverbank");
    EventManager:RegisterEvent(event);

    -- generic events
    event = vt_map.ScriptedEvent("Map:PushState(SCENE)", "Map_SceneState", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Map:PopState()", "Map_PopState", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks south", bronann, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks east", bronann, vt_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Bronann looks at Orlinn", bronann, orlinn);
    EventManager:RegisterEvent(event);
    event = vt_map.AnimateSpriteEvent("Bronann is frightened by Orlinn", bronann, "frightened", 0); -- defaut time
    EventManager:RegisterEvent(event);

    event = vt_map.LookAtSpriteEvent("Orlinn looks at Bronann", orlinn, bronann);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks east", orlinn, vt_map.MapMode.EAST);
    EventManager:RegisterEvent(event);

    event = vt_map.ChangeDirectionSpriteEvent("Banesore look east", lord, vt_map.MapMode.EAST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Banesore look west", lord, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);


    -- Bronann can't go back event
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("I can't go back now...");
    dialogue:AddLine(text, bronann);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Bronann can't go back", dialogue);
    EventManager:RegisterEvent(event);

    -- Bronann hides when entering the map
    event = vt_map.ScriptedEvent("Bronann arrives at the riverbank dialogue", "arrival_at_riverbank_dialogue_start", "");
    event:AddEventLinkAtEnd("Bronann hides behind the trees", 50);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Bronann hides behind the trees", bronann, 92.5, 6.0, false);
    event:AddEventLinkAtEnd("Bronann looks south");
    event:AddEventLinkAtEnd("Bronann thinks he needs to get closer");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Guards keeping the surroundings... I need to come closer and see what's happening.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("I should be able to sneak through when they're not watching...");
    dialogue:AddLine(text, bronann);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Bronann thinks he needs to get closer", dialogue);
    event:AddEventLinkAtEnd("End of bronann arrival at the riverbank");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("End of bronann arrival at the riverbank", "arrival_at_riverbank_dialogue_end", "");
    EventManager:RegisterEvent(event);

    -- Bronann is catched
    event = vt_map.ScriptedEvent("Bronann is catched start", "bronann_is_catched_start", "");
    event:AddEventLinkAtEnd("The soldier21 looks at Bronann");
    event:AddEventLinkAtEnd("The soldier22 looks at Bronann");
    event:AddEventLinkAtEnd("The soldier yells at Bronann");
    event:AddEventLinkAtEnd("Bronann is surprised");
    EventManager:RegisterEvent(event);

    event = vt_map.LookAtSpriteEvent("The soldier21 looks at Bronann", soldier21, bronann);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("The soldier22 looks at Bronann", soldier22, bronann);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Who's there?");
    dialogue:AddLine(text, soldier21);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("The soldier yells at Bronann", dialogue);
    event:AddEventLinkAtEnd("Restart map");
    EventManager:RegisterEvent(event);

    event = vt_map.AnimateSpriteEvent("Bronann is surprised", bronann, "frightened_fixed", 999999);
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("Restart map", "dat/maps/layna_village/layna_village_riverbank_map.lua",
                                      "dat/maps/layna_village/layna_village_riverbank_at_night_script.lua", "from_village_center");
    EventManager:RegisterEvent(event);

    -- Lord battle scene
    event = vt_map.ScriptedEvent("Lord battle dialogue start", "lord_battle_scene_start", "");
    event:AddEventLinkAtEnd("Bronann hides behind the trees near Orlinn", 50);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Bronann hides behind the trees near Orlinn", bronann, 70.0, 47.0, true);
    event:AddEventLinkAtEnd("Bronann and Orlinn are discussing");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Orlinn goes near Bronann", orlinn, 71.5, 43.8, false);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("(Whispering) Kalya!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("(What's happening here?)");
    dialogue:AddLineEvent(text, bronann, "Orlinn goes near Bronann", "");
    text = vt_system.Translate("(Also whispering) I don't know...");
    dialogue:AddLineEvent(text, orlinn, "Orlinn looks at Bronann", "Bronann looks at Orlinn");
    text = vt_system.Translate("(Woah, you scared me. Are you mad? You could have make us both caught!)");
    dialogue:AddLineEvent(text, bronann, "Bronann is frightened by Orlinn", "");
    text = vt_system.Translate("(I thought you've seen me, eh.)");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("(Kalya was brought here by those weirdos. They are helding everyone captive. Lilly is discussing with that big guy. He gives me the chill.)");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("(Let's have a look..)");
    dialogue:AddLine(text, bronann);
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Bronann and Orlinn are discussing", dialogue);
    event:AddEventLinkAtEnd("Bronann looks east");
    event:AddEventLinkAtEnd("Orlinn looks east");
    event:AddEventLinkAtEnd("Set Camera on Lilly");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Set Camera on Lilly", lilly, "set_camera_on", "set_camera_update");
    event:AddEventLinkAtEnd("Dialogue with the Lord");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("I already told you again, and again, Banesore, I don't know anything about such a Crystal.");
    dialogue:AddLine(text, lilly);
    text = vt_system.Translate("It is 'Lord Banesore', you scum!");
    dialogue:AddLine(text, soldier1);
    text = vt_system.Translate("So much to lose and still looking at me in the eyes... I admire such a courageous heart, my dear one.");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("Therefore, I'm inclined to ask the questions one last time:");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("Where is the Crystal? Where is... the boy?");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("It's time to think Lilly...");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("Huh? How do you know my name??");
    dialogue:AddLineEmote(text, lilly, "exclamation");
    text = vt_system.Translate("Your very soul is slowly but surely opening. I can feel your sorrow and doubts grow now.");
    dialogue:AddLineEvent(text, lord, "Banesore moves closer to Lilly 1", "");
    text = vt_system.Translate("Fight it, Lilly! Focus!");
    dialogue:AddLineEmote(text, carson, "exclamation");
    text = vt_system.Translate("Keep quiet, you old daft rat!");
    dialogue:AddLine(text, soldier1);
    text = vt_system.Translate("Oh! My apologize, it seems you said the truth about the Crystal...");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("The fact you didn't mention anything about the boy is leading me to think you know him.");
    dialogue:AddLine(text, lord);
    text = vt_system.Translate("...");
    dialogue:AddLine(text, lilly);
    text = vt_system.Translate("Ah? So he must be close...");
    dialogue:AddLineEvent(text, lord, "Banesore moves closer to Lilly 2", "");
    text = vt_system.Translate("What? No!");
    dialogue:AddLineEmote(text, malta, "exclamation");
    text = vt_system.Translate("Oh, I'm getting it. This is her mother. Bring her here, and kill her!");
    dialogue:AddLineEvent(text, lord, "Banesore look east", "Banesore look west");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Dialogue with the Lord", dialogue);
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Banesore moves closer to Lilly 1", lord, 99.0, 54.0, false);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Banesore moves closer to Lilly 2", lord, 98.0, 54.0, false);
    EventManager:RegisterEvent(event);
end

-- zones
local village_center_zone = {};
local to_village_entrance_zone = {};
local to_riverbank_house_entrance_zone = {};

local soldier21_watching_zone = {};
local soldier21_watching_right_zone = {};
local soldier22_watching_zone = {};
local soldier22_watching_north_zone = {};
local soldier22_watching_west_zone = {};

local battle_dialogue_start_zone = {};

function _CreateZones()
    -- N.B.: left, right, top, bottom
    village_center_zone = vt_map.CameraZone(89, 105, 0, 2);
    Map:AddZone(village_center_zone);

    to_village_entrance_zone = vt_map.CameraZone(118, 119, 10, 27);
    Map:AddZone(to_village_entrance_zone);

    to_riverbank_house_entrance_zone = vt_map.CameraZone(96, 100, 46, 47);
    Map:AddZone(to_riverbank_house_entrance_zone);

    soldier21_watching_zone = vt_map.CameraZone(84, 88, 12, 22);
    soldier21_watching_zone:AddSection(88, 94, 9, 22);
    soldier21_watching_zone:AddSection(95, 105, 3, 14);
    Map:AddZone(soldier21_watching_zone);

    soldier21_watching_right_zone = vt_map.CameraZone(94, 105, 14, 22);
    soldier21_watching_right_zone:AddSection(105, 120, 9, 24);
    Map:AddZone(soldier21_watching_right_zone);

    soldier22_watching_zone = vt_map.CameraZone(74, 85, 17, 38);
    Map:AddZone(soldier22_watching_zone);

    soldier22_watching_north_zone = vt_map.CameraZone(78, 80, 12, 17);
    Map:AddZone(soldier22_watching_north_zone);

    soldier22_watching_west_zone = vt_map.CameraZone(64, 74, 20, 32);
    Map:AddZone(soldier22_watching_west_zone);

    battle_dialogue_start_zone = vt_map.CameraZone(64, 86, 39, 40);
    Map:AddZone(battle_dialogue_start_zone);
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
        EventManager:TerminateAllEvents(soldier21);
        EventManager:TerminateAllEvents(soldier22);
        -- Undo the last event, as the map is restarting
        GlobalManager:SetEventValue("story", "layna_village_arrival_at_riverbank_done", 0);
    end,

    lord_battle_scene_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

}
