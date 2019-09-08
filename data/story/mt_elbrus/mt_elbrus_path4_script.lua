-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_path4_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "data/story/common/locations/mt_elbrus.png"
map_subname = "Mountain bridge"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/sounds/wind.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Effects = nil

-- the main character handler
local hero = nil

-- Dialogue sprites
local bronann = nil
local kalya = nil
local orlinn = nil

-- Soldiers
local soldier1 = nil
local soldier2 = nil
local soldier3 = nil

-- An actual array of objects
local bridge_middle_parts = {}

local blocking_bridge = nil

-- the main map loading code
function Load(m)

    Map = m;
    Effects = Map:GetEffectSupervisor();
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(false);

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    Map:GetEffectSupervisor():EnableAmbientOverlay("data/visuals/ambient/snow_fog.png", 30.0, 10.0, true);
    Map:GetScriptSupervisor():AddScript("data/story/common/at_night.lua");

    -- Make the snow start
    Map:GetParticleManager():AddParticleEffect("data/visuals/particle_effects/snow.lua", 512.0, 384.0);

    -- The mountain cliff background
    Map:GetScriptSupervisor():AddScript("data/story/mt_elbrus/mt_elbrus_background_anim.lua");

    if (GlobalManager:GetMapData():GetPreviousLocation() == "from_shrine_2nd_floor_wind_trap") then
        hero:SetMoving(false);
        EventManager:StartEvent("Falls from above event", 200);
    elseif (GlobalManager:GetGameEvents():GetEventValue("story", "mt_elbrus_snowing_dialogue") ~= 1) then
        -- Start the dialogue about snow and the bridge if not done
        hero:SetMoving(false);
        EventManager:StartEvent("Snowing Dialogue", 200);
    end

    -- If the bridge has fallen, hide it and block the way
    if (GlobalManager:GetGameEvents():GetEventValue("story", "mt_elbrus_bridge_cut_event") == 1) then
        -- Hide the bridge's parts
        for i, my_object in ipairs(bridge_middle_parts) do
            my_object:SetPosition(0, 0);
            my_object:SetVisible(false);
        end
        blocking_bridge:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
    end

    -- Preloads certain sounds
    AudioManager:LoadSound("data/sounds/sword_swipe.wav", Map);
    AudioManager:LoadSound("data/sounds/footstep_grass2.wav", Map);
    AudioManager:LoadSound("data/sounds/heavy_bump.wav", Map);
    AudioManager:LoadMusic("data/music/Zander Noriega - School of Quirks.ogg", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from mountain path 3)
    hero = CreateSprite(Map, "Bronann", 55, 77, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    local x_position = GlobalManager:GetMapData():GetSaveLocationX();
    local y_position = GlobalManager:GetMapData():GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Make the character look at us in that case
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(x_position, y_position);
    elseif (GlobalManager:GetMapData():GetPreviousLocation() == "from_path3") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(55.0, 77.0);
    elseif (GlobalManager:GetMapData():GetPreviousLocation() == "from_shrine_entrance") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(40.0, 7.0);
    elseif (GlobalManager:GetMapData():GetPreviousLocation() == "from_shrine_2nd_floor_wind_trap") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(45.0, 2.0);
    end

    bronann = CreateSprite(Map, "Bronann", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.WEST);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    bronann:SetVisible(false);

    kalya = CreateSprite(Map, "Kalya", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.EAST);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);

    orlinn = CreateSprite(Map, "Orlinn", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);

    soldier1 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 0, 0, vt_map.MapMode.GROUND_OBJECT);
    soldier1:SetVisible(false);
    soldier1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    soldier2 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 0, 0, vt_map.MapMode.GROUND_OBJECT);
    soldier2:SetVisible(false);
    soldier2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    soldier3 = CreateNPCSprite(Map, "Dark Soldier", vt_system.Translate("Soldier"), 0, 0, vt_map.MapMode.GROUND_OBJECT);
    soldier3:SetVisible(false);
    soldier3:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
end

function _CreateObjects()
    -- Treasure box
    local chest = CreateTreasure(Map, "elbrus_path4_chest1", "Wood_Chest1", 9, 62, vt_map.MapMode.GROUND_OBJECT);
    chest:AddItem(1001, 1); -- Minor Elixir, in case something went wrong during the Harlequin battle

    -- Bridge blocker
    blocking_bridge = CreateObject(Map, "Rock1", 36.5, 13, vt_map.MapMode.GROUND_OBJECT);
    blocking_bridge:SetVisible(false);
    blocking_bridge:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    -- Objects array
    local map_objects = {
        { "Tree Small1 snow", 7, 66 },
        { "Tree Small1 snow", 50, 63 },
        { "Tree Small2 snow", 19, 5 },
        { "Tree Small1 snow", 23, 7 },
        { "Tree Big2 snow", 39, 56 },
        { "Tree Big2 snow", 2, 70 },
        { "Tree Big2 snow", 36, 72 },
        { "Tree Big1 snow", 0, 75 },
        { "Tree Big1 snow", 48, 70 },
        { "Rock1 snow", 32, 56 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_objects) do
        --print(my_array[1], my_array[2], my_array[3]);
        CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
    end

    -- Create the bridge
    CreateObject(Map, "Bridge1_up", 36.5, 11, vt_map.MapMode.FLATGROUND_OBJECT);

    bridge_middle_parts[1] = CreateObject(Map, "Bridge1_middle", 36.5, 13, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[2] = CreateObject(Map, "Bridge1_middle", 36.5, 15, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[3] = CreateObject(Map, "Bridge1_middle", 36.5, 17, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[4] = CreateObject(Map, "Bridge1_middle", 36.5, 19, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[5] = CreateObject(Map, "Bridge1_middle", 36.5, 21, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[6] = CreateObject(Map, "Bridge1_middle", 36.5, 23, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[7] = CreateObject(Map, "Bridge1_middle", 36.5, 25, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[8] = CreateObject(Map, "Bridge1_middle", 36.5, 27, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[9] = CreateObject(Map, "Bridge1_middle", 36.5, 29, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[10] = CreateObject(Map, "Bridge1_middle", 36.5, 31, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[11] = CreateObject(Map, "Bridge1_middle", 36.5, 33, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[12] = CreateObject(Map, "Bridge1_middle", 36.5, 35, vt_map.MapMode.FLATGROUND_OBJECT);
    bridge_middle_parts[13] = CreateObject(Map, "Bridge1_middle", 36.5, 37, vt_map.MapMode.FLATGROUND_OBJECT);

    CreateObject(Map, "Bridge1_down", 36.5, 39, vt_map.MapMode.FLATGROUND_OBJECT);
end

-- Special event references which destinations must be updated just before being called.
local kalya_move_next_to_bronann_event1 = nil
local orlinn_move_next_to_bronann_event1 = nil

local kalya_move_next_to_bronann_event2 = nil
local orlinn_move_next_to_bronann_event2 = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    vt_map.MapTransitionEvent.Create("to mountain shrine entrance", "data/story/mt_elbrus/mt_elbrus_shrine1_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_shrine1_script.lua", "from_path4");

    -- sprite direction events
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks north", bronann, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("The hero looks north", hero, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("The hero looks south", hero, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks south", orlinn, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks south", kalya, vt_map.MapMode.SOUTH);
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Kalya", orlinn, kalya);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Kalya", bronann, kalya);

    -- cant't go back event
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("We can't go back now.");
    dialogue:AddLine(text, bronann);
    vt_map.DialogueEvent.Create("Can't go back dialogue", dialogue);

    -- Snowing! event
    event = vt_map.ScriptedEvent.Create("Snowing Dialogue", "snowing_dialogue_start", "")
    event:AddEventLinkAtEnd("Kalya moves next to Bronann1", 100);
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann1", 100);

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_bronann_event1 = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann1", kalya, 0, 0, false);
    kalya_move_next_to_bronann_event1:AddEventLinkAtEnd("Kalya looks north");
    kalya_move_next_to_bronann_event1:AddEventLinkAtEnd("Dialogue about snow and bridge", 500);

    orlinn_move_next_to_bronann_event1 = vt_map.PathMoveSpriteEvent.Create("Orlinn moves next to Bronann1", orlinn, 0, 0, false);
    orlinn_move_next_to_bronann_event1:AddEventLinkAtEnd("Orlinn looks north");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("We made it.");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("Brr. It's quite cold up here.");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("The bridge is a few feet away. Let's reach the mountain pass before the frost kills us.");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("Let's hurry. I can't feel my toes anymore.");
    dialogue:AddLineEventEmote(text, orlinn, "Orlinn looks at Kalya", "", "sweat drop");
    event = vt_map.DialogueEvent.Create("Dialogue about snow and bridge", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes back to party");
    event:AddEventLinkAtEnd("Kalya goes back to party");

    vt_map.PathMoveSpriteEvent.Create("Orlinn goes back to party", orlinn, bronann, false);

    event = vt_map.PathMoveSpriteEvent.Create("Kalya goes back to party", kalya, bronann, false);
    event:AddEventLinkAtEnd("End of dialogue about snow");

    vt_map.ScriptedEvent.Create("End of dialogue about snow", "snowing_dialogue_end", "");

    -- afraid of the bridge
    event = vt_map.ScriptedEvent.Create("Afraid of the bridge Dialogue", "bridge_dialogue_start", "")
    event:AddEventLinkAtEnd("The hero looks north");
    event:AddEventLinkAtEnd("Dialogue about the bridge");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("(Woah, this bridge doesn't look very sturdy.)");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Well, here we go.");
    dialogue:AddLine(text, bronann);
    event = vt_map.DialogueEvent.Create("Dialogue about the bridge", dialogue);
    event:AddEventLinkAtEnd("End of bridge dialogue");

    vt_map.ScriptedEvent.Create("End of bridge dialogue", "bridge_dialogue_end", "")

    -- Cut the bridge! event
    event = vt_map.ScriptedEvent.Create("Cut the bridge Event", "bridge_cut_event_start", "")
    event:AddEventLinkAtEnd("Soldiers catching up Dialogue");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("There!");
    dialogue:AddLine(text, soldier1);
    text = vt_system.Translate("Huh?");
    dialogue:AddLineEvent(text, bronann, "The hero looks south", "");
    event = vt_map.DialogueEvent.Create("Soldiers catching up Dialogue", dialogue);
    event:AddEventLinkAtEnd("Set focus on soldiers");

    event = vt_map.ScriptedEvent.Create("Set focus on soldiers", "set_focus_on_soldiers", "set_focus_update")
    event:AddEventLinkAtEnd("Soldiers catching up Dialogue2");

    vt_map.PathMoveSpriteEvent.Create("Soldier1 starts running", soldier1, 43.5, 60.0, true);
    vt_map.PathMoveSpriteEvent.Create("Soldier2 starts running", soldier2, 41.5, 61.5, true);
    vt_map.PathMoveSpriteEvent.Create("Soldier3 starts running", soldier3, 40.0, 61.5, true);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Catch them before they reach the shrine!");
    dialogue:AddLineEmote(text, soldier1, "exclamation");
    event = vt_map.DialogueEvent.Create("Soldiers catching up Dialogue2", dialogue);
    event:AddEventLinkAtEnd("Set focus on Hero");
    event:AddEventLinkAtEnd("Soldier1 starts running");
    event:AddEventLinkAtEnd("Soldier2 starts running");
    event:AddEventLinkAtEnd("Soldier3 starts running");

    event = vt_map.ScriptedEvent.Create("Set focus on Hero", "set_focus_on_hero", "set_focus_update")
    event:AddEventLinkAtEnd("Soldiers catching up Dialogue3");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Let's run!");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Soldiers catching up Dialogue3", dialogue);
    event:AddEventLinkAtEnd("The hero runs north of the bridge");

    event = vt_map.PathMoveSpriteEvent.Create("The hero runs north of the bridge", hero, 36.5, 8, true);
    event:AddEventLinkAtEnd("Set Kalya and Orlinn position up the bridge");

    vt_map.PathMoveSpriteEvent.Create("Soldier1 runs to the cliff", soldier1, 36.5, 40.0, true);
    vt_map.PathMoveSpriteEvent.Create("Soldier2 runs to the cliff", soldier2, 32.5, 41.5, true);
    vt_map.PathMoveSpriteEvent.Create("Soldier3 runs to the cliff", soldier3, 40.0, 41.5, true);

    event = vt_map.ScriptedEvent.Create("Set Kalya and Orlinn position up the bridge", "set_kalya_orlinn_positions", "")
    event:AddEventLinkAtEnd("Kalya moves next to Bronann2");
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann2");
    event:AddEventLinkAtEnd("Bronann looks south");
    event:AddEventLinkAtEnd("Soldier1 runs to the cliff");
    event:AddEventLinkAtEnd("Soldier2 runs to the cliff");
    event:AddEventLinkAtEnd("Soldier3 runs to the cliff");

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_bronann_event2 = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann2", kalya, 0, 0, false);
    kalya_move_next_to_bronann_event2:AddEventLinkAtEnd("Kalya looks south");
    kalya_move_next_to_bronann_event2:AddEventLinkAtEnd("Dialogue about cutting bridge");

    orlinn_move_next_to_bronann_event2 = vt_map.PathMoveSpriteEvent.Create("Orlinn moves next to Bronann2", orlinn, 0, 0, false);
    orlinn_move_next_to_bronann_event2:AddEventLinkAtEnd("Orlinn looks south");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("I'll cut those ropes!");
    dialogue:AddLine(text, bronann);
    event = vt_map.DialogueEvent.Create("Dialogue about cutting bridge", dialogue);
    event:AddEventLinkAtEnd("The hero comes close the bridge's edge");

    event = vt_map.PathMoveSpriteEvent.Create("The hero comes close the bridge's edge", bronann, 36.5, 10.5, true);
    event:AddEventLinkAtEnd("The hero cuts the bridge's ropes");

    event = vt_map.AnimateSpriteEvent.Create("The hero cuts the bridge's ropes", bronann, "attack_south", -1); -- -1 = default time: (375ms)
    event:AddEventLinkAtEnd("Sword sound");
    event:AddEventLinkAtEnd("Bridge starting to fall sound", 300);
    event:AddEventLinkAtEnd("The bridge falls apart");

    vt_map.SoundEvent.Create("Sword sound", "data/sounds/sword_swipe.wav");

    vt_map.SoundEvent.Create("Bridge starting to fall sound", "data/sounds/footstep_grass2.wav");

    event = vt_map.ScriptedEvent.Create("The bridge falls apart", "init_bridge_break", "bridge_break_update")
    event:AddEventLinkAtEnd("The party relaxes");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("That was close.");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    event = vt_map.DialogueEvent.Create("The party relaxes", dialogue);
    event:AddEventLinkAtEnd("Set focus on soldiers2");

    event = vt_map.ScriptedEvent.Create("Set focus on soldiers2", "set_focus_on_soldiers", "set_focus_update")
    event:AddEventLinkAtEnd("The soldiers threaten the party");

    vt_map.LookAtSpriteEvent.Create("Soldier2 looks at Soldier1", soldier2, soldier1);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Damn!");
    dialogue:AddLine(text, soldier1);
    text = vt_system.Translate("The master won't be happy with this.");
    dialogue:AddLineEvent(text, soldier2, "Soldier2 looks at Soldier1", "");
    text = vt_system.Translate("You won't get away so easily. We will catch you sooner or later.");
    dialogue:AddLineEmote(text, soldier1, "sweat drop");
    event = vt_map.DialogueEvent.Create("The soldiers threaten the party", dialogue);
    event:AddEventLinkAtEnd("Set focus on Bronann2");

    event = vt_map.ScriptedEvent.Create("Set focus on Bronann2", "set_focus_on_bronann", "set_focus_update")
    event:AddEventLinkAtEnd("Play funny music");
    event:AddEventLinkAtEnd("The party relaxes 2");
    event:AddEventLinkAtEnd("The hero laughs");
    event:AddEventLinkAtEnd("Kalya laughs");
    event:AddEventLinkAtEnd("Orlinn laughs");

    vt_map.ScriptedEvent.Create("Play funny music", "play_funny_music", "");

    vt_map.AnimateSpriteEvent.Create("The hero laughs", bronann, "laughing", 0); -- 0 = infinite time.

    vt_map.AnimateSpriteEvent.Create("Kalya laughs", kalya, "laughing", 0); -- infinite time.
    vt_map.AnimateSpriteEvent.Create("Orlinn laughs", orlinn, "laughing", 0); -- infinite time.

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Hurray! Those idiots will have a hard time catching us now!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Indeed! They've been had!");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Who are the little brats now, eh?");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("The party relaxes 2", dialogue);
    event:AddEventLinkAtEnd("Set focus on soldiers3");

    event = vt_map.ScriptedEvent.Create("Set focus on soldiers3", "set_focus_on_soldiers", "set_focus_update")
    event:AddEventLinkAtEnd("The soldiers retreat");

    vt_map.ScriptedEvent.Create("The party stops laughing", "stop_party_animation", "")

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Let's go back and inform the others.");
    dialogue:AddLine(text, soldier1);
    text = vt_system.Translate("Yes, sir.");
    dialogue:AddLineEvent(text, soldier2, "Soldier2 looks at Soldier1", "");
    event = vt_map.DialogueEvent.Create("The soldiers retreat", dialogue);
    event:AddEventLinkAtEnd("The party stops laughing");
    event:AddEventLinkAtEnd("Set focus on Bronann3");
    event:AddEventLinkAtEnd("Soldier1 goes back");
    event:AddEventLinkAtEnd("Soldier2 goes back");
    event:AddEventLinkAtEnd("Soldier3 goes back");

    vt_map.PathMoveSpriteEvent.Create("Soldier1 goes back", soldier1, 41.5, 68, false);
    vt_map.PathMoveSpriteEvent.Create("Soldier2 goes back", soldier2, 45.5, 73, false);
    vt_map.PathMoveSpriteEvent.Create("Soldier3 goes back", soldier3, 51.5, 72, false);

    event = vt_map.ScriptedEvent.Create("Set focus on Bronann3", "set_focus_on_bronann_slow", "set_focus_update")
    event:AddEventLinkAtEnd("The party wonders what to do");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("They're gone. We should move on before they actually find a way to cross the gap.");
    dialogue:AddLineEvent(text, kalya, "Bronann looks at Kalya", "");
    event = vt_map.DialogueEvent.Create("The party wonders what to do", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes back to party2");
    event:AddEventLinkAtEnd("Kalya goes back to party2");

    vt_map.PathMoveSpriteEvent.Create("Orlinn goes back to party2", orlinn, bronann, false);

    event = vt_map.PathMoveSpriteEvent.Create("Kalya goes back to party2", kalya, bronann, false);
    event:AddEventLinkAtEnd("End of cutting the bridge Event");

    vt_map.ScriptedEvent.Create("End of cutting the bridge Event", "cut_the_bridge_event_end", "");

    vt_map.ScriptedEvent.Create("Falls from above event", "fall_event_start", "fall_event_update");
end

-- zones
local to_shrine_zone = nil
local bridge_south_zone = nil
local bridge_middle_zone = nil
local to_path3_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_zone = vt_map.CameraZone.Create(39, 41, 3, 5);
    to_path3_zone = vt_map.CameraZone.Create(48, 64, 78, 80);

    bridge_south_zone = vt_map.CameraZone.Create(33, 39, 39, 41);
    bridge_middle_zone = vt_map.CameraZone.Create(33, 39, 24, 26);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain shrine entrance");
    elseif (bridge_south_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        if (GlobalManager:GetGameEvents():GetEventValue("story", "mt_elbrus_bridge_dialogue") ~= 1) then
            EventManager:StartEvent("Afraid of the bridge Dialogue");
        end
    elseif (bridge_middle_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        if (GlobalManager:GetGameEvents():GetEventValue("story", "mt_elbrus_bridge_cut_event") ~= 1) then
            EventManager:StartEvent("Cut the bridge Event");
        end
    elseif (to_path3_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        hero:SetMoving(false);
        EventManager:StartEvent("Can't go back dialogue");
    end
end

-- Trigger damages on the characters present on the battle front.
function _TriggerPartyDamage(damage)
    -- Adds an effect on map
    local x_pos = Map:GetScreenXCoordinate(hero:GetXPosition());
    local y_pos = Map:GetScreenYCoordinate(hero:GetYPosition());
    local map_indicator = Map:GetIndicatorSupervisor();
    map_indicator:AddDamageIndicator(x_pos, y_pos, damage, vt_video.TextStyle("text22", vt_video.Color(1.0, 0.0, 0.0, 0.9)), true);

    local index = 0;
    for index = 0, 3 do
        local char = GlobalManager:GetCharacterHandler():GetCharacter(index);
        if (char ~= nil) then
            -- Do not kill characters. though
            local hp_damage = damage;
            if (hp_damage >= char:GetHitPoints()) then
                hp_damage = char:GetHitPoints() - 1;
            end
            if (hp_damage > 0) then
                char:SubtractHitPoints(hp_damage);
            end
        end
    end
end

local bridge_parts_time = 0;
local bridge_parts_to_move = 1;
local total_bridge_time = 0;
local bridge_parts_position_x = {}
local bridge_parts_position_y = {}

-- Map Custom functions
-- Used through scripted events
map_functions = {

    snowing_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition())
        bronann:SetDirection(hero:GetDirection())
        bronann:SetVisible(true)
        hero:SetVisible(false)
        Map:SetCamera(bronann)
        hero:SetPosition(0, 0)

        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        kalya:SetVisible(true);
        orlinn:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        orlinn:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        kalya_move_next_to_bronann_event1:SetDestination(bronann:GetXPosition() + 2.0, bronann:GetYPosition(), false);
        orlinn_move_next_to_bronann_event1:SetDestination(bronann:GetXPosition() - 2.0, bronann:GetYPosition(), false);
    end,

    snowing_dialogue_end = function()
        Map:PopState();
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition())
        hero:SetDirection(bronann:GetDirection())
        hero:SetVisible(true)
        bronann:SetVisible(false)
        Map:SetCamera(hero)
        bronann:SetPosition(0, 0)

        -- Set event as done
        GlobalManager:GetGameEvents():SetEventValue("story", "mt_elbrus_snowing_dialogue", 1);
    end,

    bridge_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    bridge_dialogue_end = function()
        Map:PopState();
        GlobalManager:GetGameEvents():SetEventValue("story", "mt_elbrus_bridge_dialogue", 1);
    end,

    bridge_cut_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

        -- Place the soldiers
        soldier1:SetPosition(41.5, 68);
        soldier1:SetVisible(true);
        soldier1:SetDirection(vt_map.MapMode.NORTH);

        soldier2:SetPosition(45.5, 73);
        soldier2:SetVisible(true);
        soldier2:SetDirection(vt_map.MapMode.WEST);

        soldier3:SetPosition(51.5, 72);
        soldier3:SetVisible(true);
        soldier3:SetDirection(vt_map.MapMode.NORTH);
    end,

    set_focus_on_soldiers = function()
        Map:SetCamera(soldier1, 1200);
    end,
    set_focus_on_hero = function()
        Map:SetCamera(hero, 1000);
    end,
    set_focus_on_bronann = function()
        Map:SetCamera(bronann, 1000);
    end,
    set_focus_on_bronann_slow = function()
        Map:SetCamera(bronann, 2000);
    end,

    set_focus_update = function()
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        return true;
    end,

    set_kalya_orlinn_positions = function()
        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition())
        bronann:SetDirection(hero:GetDirection())
        bronann:SetVisible(true)
        hero:SetVisible(false)
        Map:SetCamera(bronann)
        hero:SetPosition(0, 0)

        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        kalya:SetVisible(true);
        orlinn:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        orlinn:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        kalya_move_next_to_bronann_event2:SetDestination(bronann:GetXPosition() + 2.0, bronann:GetYPosition(), false);
        orlinn_move_next_to_bronann_event2:SetDestination(bronann:GetXPosition() - 2.0, bronann:GetYPosition(), false);
    end,

    init_bridge_break = function()
        bridge_parts_time = 200;
        -- Tells the number of bridge parts that move
        bridge_parts_to_move = 1;
        total_bridge_time = 0;
        -- Init the bridge parts positions
        for i, my_object in ipairs(bridge_middle_parts) do
            --print(i)
            --print(my_object:GetXPosition(), my_object:GetYPosition());
            bridge_parts_position_x[i] = my_object:GetXPosition();
            bridge_parts_position_y[i] = my_object:GetYPosition();
        end
    end,

    bridge_break_update = function()
        -- Get the time elapsed
        local elapsed_time = SystemManager:GetUpdateTime();
        total_bridge_time = total_bridge_time + elapsed_time;
        bridge_parts_time = bridge_parts_time - elapsed_time;

        -- Break another piece when the time has come to
        if (bridge_parts_time < 0) then
            bridge_parts_time = 200;
            bridge_parts_to_move = bridge_parts_to_move + 1;
        end

        -- Make the concerned bridge parts move
        local i = 0;
        for index, my_object in ipairs(bridge_middle_parts) do
            bridge_parts_position_x[index] = bridge_parts_position_x[index] + elapsed_time * 0.0030;
            bridge_parts_position_y[index] = bridge_parts_position_y[index] + elapsed_time * 0.015;
            my_object:SetPosition(bridge_parts_position_x[index], bridge_parts_position_y[index]);

            i = i + 1;
            if (i >= bridge_parts_to_move) then
                break;
            end
        end

        if (total_bridge_time > 2500) then
            -- Hide the bridge's parts
            for i, my_object in ipairs(bridge_middle_parts) do
                my_object:SetPosition(0, 0);
                my_object:SetVisible(false);
            end

            -- and keep on
            return true;
        else
            return false;
        end
    end,

    play_funny_music = function()
        AudioManager:PlayMusic("data/music/Zander Noriega - School of Quirks.ogg");
    end,

    stop_party_animation = function(sprite)
        -- Stops the laughing animation in that particular case
        EventManager:EndAllEvents(hero);
        EventManager:EndAllEvents(kalya);
        EventManager:EndAllEvents(orlinn);
    end,

    cut_the_bridge_event_end = function()
        Map:PopState();
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition())
        hero:SetDirection(bronann:GetDirection())
        hero:SetVisible(true)
        bronann:SetVisible(false)
        Map:SetCamera(hero)
        bronann:SetPosition(0, 0)

        -- Actually block the player's bridge access.
        blocking_bridge:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);

        -- Fade in the default music
        AudioManager:PlayMusic("data/sounds/wind.ogg");

        -- Set event as done
        GlobalManager:GetGameEvents():SetEventValue("story", "mt_elbrus_bridge_cut_event", 1);
    end,

    fall_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        -- place the character and make it fall
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(45.0, 2.0);
        hero:SetCustomAnimation("frightened_fixed", 0); -- 0 means forever
    end,

    fall_event_update = function()
        if (hero:GetYPosition() >= 6.0) then
            AudioManager:PlaySound("data/sounds/heavy_bump.wav");
            Effects:ShakeScreen(0.6, 600, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);
            hero:SetCustomAnimation("hurt", 800);
            _TriggerPartyDamage(math.random(25, 40));
            Map:PopState();
            return true;
        end

        -- Push the character down.
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.010 * update_time;
        hero:SetYPosition(hero:GetYPosition() + movement_diff);
        return false;
    end,
}
