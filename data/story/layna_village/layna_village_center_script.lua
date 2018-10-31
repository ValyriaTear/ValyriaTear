-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_center_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "data/story/common/locations/mountain_village.png"
map_subname = "Village center"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Effects = nil

local bronann = nil
local kalya = nil

-- Main npcs
local orlinn = nil
local georges = nil
local carson = nil
local herth = nil
local olivia = nil -- Olivia npc, guarding the forest entrance

local well_entrance_rock = nil

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

    _TriggerPotentialDialogueAfterFadeIn();

    -- Add clouds overlay
    Effects:EnableAmbientOverlay("data/visuals/ambient/clouds.png", 5.0, -5.0, true);

    -- Set the world map current position
    GlobalManager:SetCurrentLocationId("layna village")

    _HandleCredits();
end

-- Handle the display of the new game credits
function _HandleCredits()
    -- Handle small credits triggering
    if (GlobalManager:DoesEventExist("game", "Start_Credits") == false) then
        -- Triggers the small credits display
        GlobalManager:SetEventValue("game", "Start_Credits", 1);
    end
    if (GlobalManager:DoesEventExist("game", "Credits_shown") == false) then
        Map:GetScriptSupervisor():AddScript("data/credits/episode1_credits.lua");
    end
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
    elseif (GlobalManager:GetPreviousLocation() == "from_well_undergrounds") then
        bronann:SetPosition(63, 27.5);
        bronann:SetDirection(vt_map.MapMode.NORTH);
    end
end

function _CreateNPCs()
    local npc = nil
    local text = nil
    local dialogue = nil
    local event = nil

    kalya = CreateSprite(Map, "Kalya", 42, 18, vt_map.MapMode.GROUND_OBJECT);

    event = vt_map.RandomMoveSpriteEvent.Create("Kalya random move", kalya, 1000, 2000);
    event:AddEventLinkAtEnd("Kalya random move", 2000); -- Loop on itself

    EventManager:StartEvent("Kalya random move");
    dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_kalya_wants_to_be_alone");
    text = vt_system.Translate("Do you need something, Bronann?");
    dialogue:AddLineEmote(text, kalya, "interrogation");
    kalya:AddDialogueReference(dialogue);

    orlinn = CreateSprite(Map, "Orlinn", 40, 18, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);

    -- Setup Orlinn's state and dialogue depending on the story current context
    _UpdateOrlinnAndKalyaState();

    carson = CreateSprite(Map, "Carson", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    -- Default behaviour - not present on map.
    carson:SetVisible(false);
    carson:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    herth = CreateSprite(Map, "Herth", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    -- Default behaviour - not present on map.
    herth:SetVisible(false);
    herth:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    npc = CreateNPCSprite(Map, "Old Woman1", vt_system.Translate("Brymir"), 72, 64, vt_map.MapMode.GROUND_OBJECT);
    npc:SetDirection(vt_map.MapMode.WEST);
    dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_brymir_gossip");
    text = vt_system.Translate("Ahh sure! (gossip, gossip)");
    dialogue:AddLine(text, npc);
    npc:AddDialogueReference(dialogue);

    npc = CreateNPCSprite(Map, "Woman1", vt_system.Translate("Martha"), 70, 64, vt_map.MapMode.GROUND_OBJECT);
    npc:SetDirection(vt_map.MapMode.EAST);
    dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_martha_gossip");
    text = vt_system.Translate("Did you hear that? (gossip, gossip)");
    dialogue:AddLine(text, npc);
    npc:AddDialogueReference(dialogue);

    npc = CreateNPCSprite(Map, "Woman2", vt_system.Translate("Sophia"), 22, 38, vt_map.MapMode.GROUND_OBJECT);
    npc:SetDirection(vt_map.MapMode.SOUTH);
    if (GlobalManager:DoesEventExist("layna_south_entrance", "quest1_orlinn_hide_n_seek1_done") == true and
            GlobalManager:DoesEventExist("layna_riverbank", "quest1_orlinn_hide_n_seek2_done") == false) then
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_sophia_hint_about_orlinn");
        text = vt_system.Translate("If you're running after Orlinn, I just saw him near your house.")
        dialogue:AddLine(text, npc)
    else
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_sophia_no_trade");
        text = vt_system.Translate("Do you want to trade with me?")
        dialogue:AddLine(text, npc)
        dialogue:SetEventAtDialogueEnd("layna: Sophia trades")

        event = vt_map.ShopEvent.Create("layna: Sophia trades", "Layna Sophia Trades");
        event:SetShopName(vt_system.UTranslate("Sophia"));

        -- Items
        event:AddItem(4001, 0);   -- (infinite) Escape Smoke
        -- Trades
        event:AddTrade(10002, 1); -- Improved Wooden Sword
        event:AddTrade(11002, 1); -- Improved Arbalest
        event:AddTrade(40001, 2); -- Prismatic Rings for both

        event:SetPriceLevels(vt_shop.ShopMode.SHOP_PRICE_VERY_GOOD, -- Sophia is a good friend
                             vt_shop.ShopMode.SHOP_PRICE_STANDARD);
    end

    npc:AddDialogueReference(dialogue);
    -- Add her cat, Nekko
    object = CreateObject(Map, "Cat1", 24, 37.6, vt_map.MapMode.GROUND_OBJECT);
    vt_map.SoundEvent.Create("Nekko says Meoww!", "data/sounds/meow.wav");
    object:SetEventWhenTalking("Nekko says Meoww!");

    georges = CreateNPCSprite(Map, "Man1", vt_system.Translate("Georges"), 42.5, 65, vt_map.MapMode.GROUND_OBJECT);
    georges:SetDirection(vt_map.MapMode.WEST);
    _UpdateGeorgesDialogue(georges);

    -- Olivia, guardian of the forest access
    olivia = CreateNPCSprite(Map, "Girl1", vt_system.Translate("Olivia"), 115, 36, vt_map.MapMode.GROUND_OBJECT)
    olivia:SetDirection(vt_map.MapMode.SOUTH)
    olivia:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED)

    -- Needed look at events
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Olivia", bronann, olivia);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);

end

function _CreateObjects()
    local object = nil

    -- Info sign
    object = CreateObject(Map, "Wood sign info", 29, 68, vt_map.MapMode.GROUND_OBJECT)
    object:SetEventWhenTalking("Info about running")
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.VTranslate("Tired of walking?\nYou can run when holding the Cancel (%s) key", InputManager:GetCancelKeyName());
    dialogue:AddLine(text, nil);
    event = vt_map.DialogueEvent.Create("Info about running", dialogue)

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

    -- A village with drinkable water
    CreateObject(Map, "Well", 59.0, 32.0, vt_map.MapMode.GROUND_OBJECT);

    -- collision bug hidders
    CreateObject(Map, "Barrel1", 14, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Vase1", 15, 39, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 30, 38, vt_map.MapMode.GROUND_OBJECT);

    -- Lights
    -- big round windows light flares
    vt_map.Light.Create("data/visuals/lights/sun_flare_light_main.lua",
                        "data/visuals/lights/sun_flare_light_secondary.lua",
                        74.0, 55.0,
                        vt_video.Color(1.0, 1.0, 1.0, 0.6),
                        vt_video.Color(1.0, 1.0, 0.85, 0.3));

    vt_map.Light.Create("data/visuals/lights/sun_flare_light_main.lua",
                        "data/visuals/lights/sun_flare_light_secondary.lua",
                        86.0, 67.0,
                        vt_video.Color(1.0, 1.0, 1.0, 0.6),
                        vt_video.Color(1.0, 1.0, 0.85, 0.3));
    vt_map.Light.Create("data/visuals/lights/sun_flare_light_main.lua",
                        "data/visuals/lights/sun_flare_light_secondary.lua",
                        22.0, 32.0,
                        vt_video.Color(1.0, 1.0, 1.0, 0.6),
                        vt_video.Color(1.0, 1.0, 0.85, 0.3));

    -- Small door lights
    vt_map.Light.Create("data/visuals/lights/sun_flare_light_small_main.lua",
                        "data/visuals/lights/sun_flare_light_small_secondary.lua",
                        12.0, 57.5,
                        vt_video.Color(1.0, 1.0, 1.0, 0.6),
                        vt_video.Color(1.0, 1.0, 0.85, 0.3));
    vt_map.Light.Create("data/visuals/lights/sun_flare_light_small_main.lua",
                        "data/visuals/lights/sun_flare_light_small_secondary.lua",
                        94.0, 67.5,
                        vt_video.Color(1.0, 1.0, 1.0, 0.6),
                        vt_video.Color(1.0, 1.0, 0.85, 0.3));

    -- Treasure vase
    local nekko_vase = CreateTreasure(Map, "layna_center_nekko_vase", "Vase1", 27, 37, vt_map.MapMode.GROUND_OBJECT);
    nekko_vase:AddItem(11, 1);

    -- Rock hiding the well underground entrance
    well_entrance_rock = CreateObject(Map, "Rock1", 63, 32, vt_map.MapMode.GROUND_OBJECT)
    if (GlobalManager:DoesEventExist("story", "well_rats_beaten") == true) then
        -- The rock has moved
        well_entrance_rock:SetPosition(63, 34)
    end

    -- Surrounding grass
    local well_grass = {
    { "Grass Clump1", 62, 32.5 },
    { "Grass Clump1", 64, 33 },
    { "Grass Clump1", 58, 33 },
    { "Grass Clump1", 60, 33.4 },
    { "Grass Clump1", 65, 32 },
    { "Grass Clump1", 62, 30.3 },
    }

    -- Loads the grass according to the array
    for my_index, my_array in pairs(well_grass) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local text = nil
    local dialogue = nil

    -- Triggered Events
    vt_map.MapTransitionEvent.Create("to Bronann's home", "data/story/layna_village/layna_village_bronanns_home_map.lua",
                                     "data/story/layna_village/layna_village_bronanns_home_script.lua", "from_village_center");

    vt_map.MapTransitionEvent.Create("to Riverbank", "data/story/layna_village/layna_village_riverbank_map.lua",
                                     "data/story/layna_village/layna_village_riverbank_script.lua", "from_village_center");

    vt_map.MapTransitionEvent.Create("to Village south entrance", "data/story/layna_village/layna_village_south_entrance_map.lua",
                                     "data/story/layna_village/layna_village_south_entrance_script.lua", "from_village_center");

    vt_map.MapTransitionEvent.Create("to Kalya house path", "data/story/layna_village/layna_village_kalya_house_path_map.lua",
                                     "data/story/layna_village/layna_village_kalya_house_path_script.lua", "from_village_center");

    vt_map.MapTransitionEvent.Create("to Flora's Shop", "data/story/layna_village/layna_village_center_shop_map.lua",
                                     "data/story/layna_village/layna_village_center_shop_script.lua", "from_village_center");

    vt_map.MapTransitionEvent.Create("to sophia house", "data/story/layna_village/layna_village_center_sophia_house_map.lua",
                                     "data/story/layna_village/layna_village_center_sophia_house_script.lua", "from_village_center");

    vt_map.MapTransitionEvent.Create("to secret cliff", "data/story/layna_village/layna_village_riverbank_map.lua",
                                     "data/story/layna_village/layna_village_riverbank_script.lua", "from_secret_path");

    vt_map.MapTransitionEvent.Create("to layna forest entrance", "data/story/layna_forest/layna_forest_entrance_map.lua",
                                     "data/story/layna_forest/layna_forest_entrance_script.lua", "from_village_center")

    event = vt_map.MapTransitionEvent.Create("To well underground", "data/story/layna_village/layna_village_well_underground_map.lua",
        "data/story/layna_village/layna_village_well_underground_script.lua",
        "from layna village")

    -- Generic events
    vt_map.ScriptedEvent.Create("Map:PushState(SCENE)", "Map_SceneState", "")
    vt_map.ScriptedEvent.Create("Map:PopState()", "Map_PopState", "")

    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks south", kalya, vt_map.MapMode.SOUTH)
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Kalya", orlinn, kalya)
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Bronann", orlinn, bronann)
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Orlinn", kalya, orlinn)
    vt_map.AnimateSpriteEvent.Create("Bronann is sad", bronann, "hero_stance", 1000)
    vt_map.AnimateSpriteEvent.Create("Bronann is frightened", bronann, "frightened", 600)

    -- Init Olivia Dialogue
    _UpdateOliviaDialogue();

    -- Quest events
    -- Bronann wonders where he can find barley meal
    event = vt_map.ScriptedEvent.Create("Quest1: Bronann wonders where he can find some barley meal", "Map_SceneState", "");
    event:AddEventLinkAtEnd("Quest1: Bronann wants to see Flora for the barley meal", 1000);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Hmm, I'll go to Flora's shop. I hope she'll help me.");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    event = vt_map.DialogueEvent.Create("Quest1: Bronann wants to see Flora for the barley meal", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Map:PopState()");

    -- Georges
    vt_map.ScriptedEvent.Create("Quest1: GeorgesDialogueDone", "Quest1GeorgesDialogueDone", "");

    -- Orlinn
    vt_map.ScriptedEvent.Create("Orlinn laughs", "orlinn_laughs", "");

    event = vt_map.ScriptedEvent.Create("Quest1: Make Orlinn run and hide", "Quest1OrlinnRunAndHide", "");
    event:AddEventLinkAtEnd("Quest1: Make Orlinn run");

    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Make Orlinn run", orlinn, 67, 79, true);
    event:AddEventLinkAtEnd("Quest1: Make Orlinn disappear");

    event = vt_map.ScriptedSpriteEvent.Create("Quest1: Make Orlinn disappear", orlinn, "MakeInvisible", "");
    event:AddEventLinkAtEnd("Quest1: Make Orlinn run event end");

    vt_map.ScriptedSpriteEvent.Create("Quest1: Make Orlinn run event end", orlinn, "orlinn_run_event_end", "");

    -- Kalya calls for Orlinn
    event = vt_map.ScriptedEvent.Create("Kalya brings back Orlinn event start", "kalya_brings_orlinn_back_start", "");
    event:AddEventLinkAtEnd("Kalya tells Bronann to follow her");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("As you wish. Follow me.");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Kalya tells Bronann to follow her", dialogue);
    event:AddEventLinkAtEnd("Kalya goes at the center of village");
    event:AddEventLinkAtEnd("Bronann follows Kalya at the center of the village", 1000);

    event = vt_map.PathMoveSpriteEvent.Create("Kalya goes at the center of village", kalya, 52, 47, true);
    event:AddEventLinkAtEnd("Kalya looks south");
    event:AddEventLinkAtEnd("Kalya tells Orlinn to come");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann follows Kalya at the center of the village", bronann, 48, 47, true);
    event:AddEventLinkAtEnd("Bronann looks south");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("ORLINN! Come here, NOW!");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Kalya tells Orlinn to come", dialogue);
    event:AddEventLinkAtEnd("Orlinn comes near Kalya");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn comes near Kalya", orlinn, 52, 50, true);
    event:AddEventLinkAtEnd("Orlinn looks at Kalya");
    event:AddEventLinkAtEnd("Kalya tells Orlinn to give the pen");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Orlinn, give back the pen to Bronann or I shall sma...");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("Yea, yes, here it is.");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    event = vt_map.DialogueEvent.Create("Kalya tells Orlinn to give the pen", dialogue);
    event:AddEventLinkAtEnd("Orlinn comes near Bronann");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn comes near Bronann", orlinn, 48, 50, false);
    event:AddEventLinkAtEnd("Orlinn looks at Bronann");
    event:AddEventLinkAtEnd("Kalya looks at Orlinn");
    event:AddEventLinkAtEnd("Orlinn gives the pen to Bronann");

    event = vt_map.TreasureEvent.Create("Orlinn gives the pen to Bronann");
    event:AddItem(70001, 1); -- The ink key item
    event:AddEventLinkAtEnd("Orlinn apologizes");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I found that pen under a tree near the river. I just wanted to play.");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    text = vt_system.Translate("Don't worry about it.");
    dialogue:AddLine(text, bronann);
    event = vt_map.DialogueEvent.Create("Orlinn apologizes", dialogue);
    event:AddEventLinkAtEnd("Orlinn comes back event end");

    vt_map.ScriptedEvent.Create("Orlinn comes back event end", "orlinn_comes_back_event_end", "");

    -- Georges event
    vt_map.ScriptedEvent.Create("Quest1: Georges thanks Bronann", "Quest1GeorgesTellsBronannAboutLilly", "");

    -- Quest 2: The forest event
    event = vt_map.ScriptedEvent.Create("Quest2: Forest event", "Prepare_forest_event", "");
    event:AddEventLinkAtEnd("Quest2: Forest event - light", 1200);

    event = vt_map.ScriptedEvent.Create("Quest2: Forest event - light", "BrightLightStart", "BrightLightUpdate");
    event:AddEventLinkAtEnd("Bronann moves out of the door")

    event = vt_map.PathMoveSpriteEvent.Create("Bronann moves out of the door", bronann, 12, 66, false)
    event:AddEventLinkAtEnd("Bronann searches")

    vt_map.AnimateSpriteEvent.Create("Bronann searches", bronann, "searching", 1000)
    event:AddEventLinkAtEnd("Quest2: Bronann wonders what was that", 500)

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("What was this?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    text = vt_system.Translate("Bronann! Wait!");
    dialogue:AddLine(text, carson);
    event = vt_map.DialogueEvent.Create("Quest2: Bronann wonders what was that", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Make Carson Visible")
    event:AddEventLinkAtEnd("Quest2: Carson moves to Bronann", 100)
    event:AddEventLinkAtEnd("Quest2: Bronann looks at his father", 100)

    vt_map.ScriptedEvent.Create("Make Carson Visible", "set_carson_visible", "")

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Carson moves to Bronann", carson, 12, 63, false);
    event:AddEventLinkAtEnd("Quest2: Carson starts to talk to Bronann");

    vt_map.ChangeDirectionSpriteEvent.Create("Quest2: Bronann looks at his father", bronann, vt_map.MapMode.NORTH)
    vt_map.ChangeDirectionSpriteEvent.Create("Quest2: Bronann looks at Herth", bronann, vt_map.MapMode.EAST)
    vt_map.ChangeDirectionSpriteEvent.Create("Quest2: Herth looks at Carson", herth, vt_map.MapMode.WEST)
    vt_map.ChangeDirectionSpriteEvent.Create("Quest2: Herth looks at Kalya", herth, vt_map.MapMode.EAST)
    vt_map.ChangeDirectionSpriteEvent.Create("Quest2: Carson looks at Herth", carson, vt_map.MapMode.SOUTH)
    vt_map.ChangeDirectionSpriteEvent.Create("Quest2: Carson looks at Kalya", carson, vt_map.MapMode.SOUTH)
    vt_map.ChangeDirectionSpriteEvent.Create("Quest2: Carson looks at Bronann", carson, vt_map.MapMode.SOUTH)
    vt_map.ChangeDirectionSpriteEvent.Create("Quest2: Kalya looks at Carson", kalya, vt_map.MapMode.NORTH)
    vt_map.ChangeDirectionSpriteEvent.Create("Quest2: Kalya looks at Bronann", kalya, vt_map.MapMode.WEST)

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann, there is something that I have to tell you. We've been fearing for this moment, your mother and I.");
    dialogue:AddLine(text, carson);
    text = vt_system.Translate("They're coming!");
    dialogue:AddLineEvent(text, herth, "Quest2: Bronann looks at Herth", "Quest2: Carson looks at Herth")
    event = vt_map.DialogueEvent.Create("Quest2: Carson starts to talk to Bronann", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest2: Herth moves to Carson");

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Herth moves to Carson", herth, 17, 71, false);
    event:AddEventLinkAtEnd("Quest2: Herth looks at Carson");
    event:AddEventLinkAtEnd("Quest2: Second part of talk");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Carson, they've crossed the river. They shall be here in no time.");
    dialogue:AddLine(text, herth);
    text = vt_system.Translate("...");
    dialogue:AddLineEmote(text, carson, "sweat drop");
    text = vt_system.Translate("Huh? Hey, what's happening here?!");
    dialogue:AddLineEventEmote(text, bronann, "", "Quest2: Carson looks at Bronann", "exclamation");
    text = vt_system.Translate("Bronann, I...");
    dialogue:AddLine(text, carson);
    text = vt_system.Translate("Herth!");
    dialogue:AddLineEvent(text, kalya, "", "Quest2: Herth looks at Kalya");
    event = vt_map.DialogueEvent.Create("Quest2: Second part of talk", dialogue);
    event:SetStopCameraMovement(true);
    event:AddEventLinkAtEnd("Quest2: Kalya runs to her father");

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Kalya runs to her father", kalya, 21, 71, true);
    event:AddEventLinkAtEnd("Quest2: Third part of talk");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Herth! Orlinn has disappeared. I saw him taking the forest pathway!");
    dialogue:AddLineEvent(text, kalya, "", "Quest2: Carson looks at Herth");
    text = vt_system.Translate("Kalya, you were supposed to keep an eye on him!");
    dialogue:AddLineEmote(text, herth, "exclamation");
    text = vt_system.Translate("I did, but he just slipped through my fingers at the very second that strange light appeared.");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("Kalya, the army is coming. I'll deal with them. You go and find Orlinn as fast as possible.");
    dialogue:AddLineEmote(text, herth, "thinking dots");
    text = vt_system.Translate("But you might get hurt!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("Don't worry. We'll simply talk to them and they'll move on. You know what you have to do, right?");
    dialogue:AddLine(text, herth);
    text = vt_system.Translate("Herth, we both know it'll likely...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Do as I say and it'll be alright.");
    dialogue:AddLine(text, herth);
    text = vt_system.Translate("But...");
    dialogue:AddLineEvent(text, kalya, "", "Quest2: Carson looks at Bronann");
    text = vt_system.Translate("Bronann, you should go with her.");
    dialogue:AddLineEventEmote(text, carson, "Quest2: Bronann looks at his father", "Quest2: Kalya looks at Carson", "thinking dots");
    text = vt_system.Translate("What?!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("Huh?");
    dialogue:AddLineEvent(text, bronann, "Bronann is frightened","");
    text = vt_system.Translate("Carson is right, Kalya. Bronann shall go with you.");
    dialogue:AddLineEvent(text, herth, "Quest2: Bronann looks at Herth", "Quest2: Kalya looks at Bronann");
    text = vt_system.Translate("But he would just be a burden!");
    dialogue:AddLineEvent(text, kalya, "", "Bronann is sad");
    text = vt_system.Translate("Huh? Hey! But...");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("He won't slow you down, believe me. Right, Bronann?");
    dialogue:AddLineEvent(text, carson, "", "Quest2: Carson looks at Bronann");
    text = vt_system.Translate("But Herth!");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Carson is right, Kalya. Bronann shall go with you. It's an order.");
    dialogue:AddLineEmote(text, herth, "thinking dots");
    event = vt_map.DialogueEvent.Create("Quest2: Third part of talk", dialogue);
    event:AddEventLinkAtEnd("Quest2: Kalya goes back and forth");

    -- Small event chain making kalya go back and forth.
    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Kalya goes back and forth", kalya, 19, 71, false);
    event:AddEventLinkAtEnd("Quest2: Carson looks at Kalya");
    event:AddEventLinkAtEnd("Quest2: Kalya goes back and forth 2");

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Kalya goes back and forth 2", kalya, 21, 71, false);
    event:AddEventLinkAtEnd("Quest2: Kalya goes back and forth 3");

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Kalya goes back and forth 3", kalya, 19, 71, false);
    event:AddEventLinkAtEnd("Quest2: Kalya goes back and forth 4");

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Kalya goes back and forth 4", kalya, 21, 71, false);
    event:AddEventLinkAtEnd("Quest2: Fourth part of talk");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Gahh, ok.");
    dialogue:AddLineEvent(text, kalya, "", "Quest2: Carson looks at Bronann");
    text = vt_system.Translate("Now go, my son. I'll explain it to you once it is all finished. I trust you will succeed.");
    dialogue:AddLineEvent(text, carson, "Quest2: Bronann looks at his father", "")
    event = vt_map.DialogueEvent.Create("Quest2: Fourth part of talk", dialogue);
    event:AddEventLinkAtEnd("Quest2: Fifth part of talk");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Thanks dad, we'll find him in no time.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("We shall go now. Good luck, both of you.");
    dialogue:AddLineEvent(text, herth, "Quest2: Bronann looks at Herth", "Quest2: Herth looks at Kalya");
    event = vt_map.DialogueEvent.Create("Quest2: Fifth part of talk", dialogue);
    event:AddEventLinkAtEnd("Quest2: Herth leaves to south");
    event:AddEventLinkAtEnd("Quest2: Carson starts to leave to south");

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Herth leaves to south", herth, 26, 78, false);
    event:AddEventLinkAtEnd("Quest2: Herth disappears");

    vt_map.ScriptedSpriteEvent.Create("Quest2: Herth disappears", herth, "MakeInvisible", "");

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Carson starts to leave to south", carson, 26, 74, false);
    event:AddEventLinkAtEnd("Quest2: Carson talks to Bronann once last time");
    event:AddEventLinkAtEnd("Quest2: Carson looks at Bronann from south");

    vt_map.ChangeDirectionSpriteEvent.Create("Quest2: Carson looks at Bronann from south", carson, vt_map.MapMode.NORTH);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Good luck, son.");
    dialogue:AddLine(text, carson);
    event = vt_map.DialogueEvent.Create("Quest2: Carson talks to Bronann once last time", dialogue);
    event:AddEventLinkAtEnd("Quest2: Carson leaves to south");

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Carson leaves to south", carson, 26, 78, false);
    event:AddEventLinkAtEnd("Quest2: Carson disappears");

    event = vt_map.ScriptedSpriteEvent.Create("Quest2: Carson disappears", carson, "MakeInvisible", "");
    event:AddEventLinkAtEnd("Quest2: End part of talk", 1000);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Ok, we'll go together. But slow me down and I'll make you regret it.");
    dialogue:AddLineEvent(text, kalya, "Quest2: Kalya looks at Bronann", "");
    text = vt_system.Translate("Don't worry, we'll find him. Ok?");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Ok.");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Quest2: End part of talk", dialogue);
    event:AddEventLinkAtEnd("Quest2: Kalya joins Bronann's party");

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Kalya joins Bronann's party", kalya, 12, 66, false);
    event:AddEventLinkAtEnd("Quest2: Kalya disappears");
    event:AddEventLinkAtEnd("Quest2: Add Kalya to the party");

    vt_map.ScriptedSpriteEvent.Create("Quest2: Kalya disappears", kalya, "MakeInvisible", "");

    vt_map.ScriptedEvent.Create("Quest2: Add Kalya to the party", "Add_kalya_to_party", "");
    event:AddEventLinkAtEnd("Quest2: Kalya joins speech");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Kalya joins your party!");
    dialogue:AddLine(text, kalya); --used for now to show her portrait
    event = vt_map.DialogueEvent.Create("Quest2: Kalya joins speech", dialogue);
    event:AddEventLinkAtEnd("Map:PopState()");

    -- Kalya explains Bronann about dungeons and equipment,
    event = vt_map.ScriptedEvent.Create("Quest2: Kalya's equipment and dungeons speech start", "Quest2_equip_speech_start", "");
    event:AddEventLinkAtEnd("Quest2: Kalya walks off from Bronann");
    event:AddEventLinkAtEnd("Quest2: Bronann goes in front of Kalya");

    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks east", kalya, vt_map.MapMode.EAST);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks west", bronann, vt_map.MapMode.WEST);

    vt_map.ScriptedSpriteEvent.Create("Turn Kalya's collisions on", kalya, "Sprite_Collision_on", "");

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Kalya walks off from Bronann", kalya, 114, 37, false);
    event:AddEventLinkAtEnd("Kalya looks east");
    event:AddEventLinkAtEnd("Turn Kalya's collisions on");
    event:AddEventLinkAtEnd("Quest2: Kalya's speech about equipment and dungeons.");

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Bronann goes in front of Kalya", bronann, 116, 37, false);
    event:AddEventLinkAtEnd("Bronann looks west");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("By the way, have you ever prepared yourself for something like this before?");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Huh? Well, Orlinn doesn't disappear every day, you know?");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("I see. Then there are two very important things that you need to know.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("First of all, before going there, you might need better equipment. You should visit Flora first, ok?");
    dialogue:AddLine(text, kalya);
    text = vt_system.VTranslate("And I hope that you have already equipped your sword. If you haven't, open your inventory by pressing the menu key (%s), and select 'Equip'. Then you'll be able to select your sword and add it as your main weapon.", InputManager:GetMenuKeyName());
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("You mean I need to push a key to open my bag?");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    text = vt_system.Translate("Nevermind that, just do it.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Also, the person in front of the battle line will lead the group in the forest. What I mean by that is the person on the top of the battle formation will actually appear while the others will stay hidden.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Hmm, right. I'm not sure that I fully get what you mean, but...");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Rrrr, Bronann. Just listen.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("That will be the case only in certain areas. Here in the village, you'll be the one leading, or at least you may believe that.");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Quest2: Kalya's speech about equipment and dungeons.", dialogue);
    event:AddEventLinkAtEnd("Quest2: Kalya re-joins Bronann after speech");

    event = vt_map.PathMoveSpriteEvent.Create("Quest2: Kalya re-joins Bronann after speech", kalya, 116, 37, false);
    event:AddEventLinkAtEnd("Quest2: Kalya disappears after speech");

    event = vt_map.ScriptedSpriteEvent.Create("Quest2: Kalya disappears after speech", kalya, "MakeInvisible", "");
    event:AddEventLinkAtEnd("Map:PopState()");
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
local to_well_undergrounds_zone = nil

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
    to_well_undergrounds_zone = vt_map.CameraZone.Create(62, 64, 29, 30)
end

function _CheckZones()
    if (bronanns_home_entrance_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        AudioManager:PlaySound("data/sounds/door_open2.wav");
        EventManager:StartEvent("to Bronann's home");
    elseif (to_riverbank_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to Riverbank");
    elseif (to_village_entrance_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to Village south entrance");
    elseif (to_kalya_house_path_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to Kalya house path");
    elseif (secret_path_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to secret cliff");
    elseif (to_layna_forest_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == false) then
            EventManager:StartEvent("Bronann can't enter the forest so easily");
        elseif (GlobalManager:DoesEventExist("story", "Quest2_kalya_equip_n_dungeons_speech_done") == false) then
            EventManager:StartEvent("Quest2: Kalya's equipment and dungeons speech start");
        else
            EventManager:StartEvent("to layna forest entrance");
        end
    elseif (shop_entrance_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        AudioManager:PlaySound("data/sounds/door_open2.wav");
        EventManager:StartEvent("to Flora's Shop");
    elseif (sophia_house_entrance_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("to sophia house");
        AudioManager:PlaySound("data/sounds/door_open2.wav");
    elseif (to_well_undergrounds_zone:IsCameraEntering() == true
            -- cannot enter until the well event is done.
            and GlobalManager:DoesEventExist("story", "well_rats_beaten") == true) then
        bronann:SetMoving(false)
        EventManager:StartEvent("To well underground")
    end
end

-- Inner custom functions
function _TriggerPotentialDialogueAfterFadeIn()
    -- Trigger the forest and Orlinn runaway event
    if (GlobalManager:DoesEventExist("story", "meal_event_done") == true
            and GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == false) then
        EventManager:StartEvent("Quest2: Forest event");
        GlobalManager:SetEventValue("story", "Quest2_forest_event_done", 1);
        return;
    end

    if (GlobalManager:DoesEventExist("layna_center", "first_time_in_village_center") == false) then
        EventManager:StartEvent("Quest1: Bronann wonders where he can find some barley meal");
        GlobalManager:SetEventValue("layna_center", "first_time_in_village_center", 1);
        return;
    end
end

-- Updates Olivia dialogues according to the story events
function _UpdateOliviaDialogue()
    local dialogue = nil
    local text = nil
    local event = nil

    olivia:ClearDialogueReferences();

    local default_dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_olivia_no_access_to_forest1");
    text = vt_system.Translate("Bronann! Sorry, you know that you can't access the forest without permission.");
    default_dialogue:AddLineEmote(text, olivia, "exclamation");
    text = vt_system.Translate("Aww...");
    default_dialogue:AddLineEventEmote(text, bronann, "Bronann looks at Olivia", "", "sweat drop");

    -- Don't grant access to the forest so easily
    if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == false) then
        if (GlobalManager:DoesEventExist("story", "Carson_wooden_sword_given") == true
                and GlobalManager:DoesEventExist("story", "well_rats_beaten") == false) then
            -- The well quest dialogue
            dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_olivia_well_quest")
            text = vt_system.Translate("Hey Bronann. I can see you have your father practice sword. Ready for your first real challenge?")
            dialogue:AddLine(text, olivia)
            text = vt_system.Translate("Hey Olivia. I guess so. What is it all about?")
            dialogue:AddLineEmote(text, bronann, "interrogation")
            text = vt_system.Translate("Follow me.")
            dialogue:AddLine(text, olivia)
            dialogue:SetEventAtDialogueEnd("Olivia goes to well with Bronann")
            olivia:AddDialogueReference(dialogue)

            -- Event to go to the well.
            event = vt_map.ScriptedEvent.Create("Olivia goes to well with Bronann", "Well_event_scene_start", "")
            event:AddEventLinkAtEnd("Olivia moves to the well")
            event:AddEventLinkAtEnd("Bronann looks at Olivia for a few seconds")
            event = vt_map.PathMoveSpriteEvent.Create("Olivia moves to the well", olivia, 57, 27, true)
            event:AddEventLinkAtEnd("Olivia looks at Bronann 1")
            event = vt_map.LookAtSpriteEvent.Create("Olivia looks at Bronann 1", olivia, bronann)

            event = vt_map.ScriptedEvent.Create("Bronann looks at Olivia for a few seconds", "bronann_watch_olivia_start", "bronann_watch_olivia_update")
            event:AddEventLinkAtEnd("Bronann moves to the well")

            event = vt_map.PathMoveSpriteEvent.Create("Bronann moves to the well", bronann, 61, 27, true)
            event:AddEventLinkAtEnd("Olivia looks at Bronann")
            event = vt_map.LookAtSpriteEvent.Create("Olivia looks at Bronann", olivia, bronann)
            event:AddEventLinkAtEnd("Olivia opens path to well underground")
            dialogue = vt_map.SpriteDialogue.Create()
            text = vt_system.Translate("Here we go!")
            dialogue:AddLine(text, olivia)
            event = vt_map.DialogueEvent.Create("Olivia opens path to well underground", dialogue)
            event:AddEventLinkAtEnd("Olivia moves close to the well")

            event = vt_map.PathMoveSpriteEvent.Create("Olivia moves close to the well", olivia, 59, 28.5, false)
            event:AddEventLinkAtEnd("Olivia opens well path")
            event = vt_map.ScriptedEvent.Create("Olivia opens well path", "well_open_trigger_start", "well_open_trigger_update")
            event:AddEventLinkAtEnd("Olivia speech before entering well")

            dialogue = vt_map.SpriteDialogue.Create()
            text = vt_system.Translate("Time to dive in, Bronann.")
            dialogue:AddLine(text, olivia)
            text = vt_system.Translate("Follow me inside.")
            dialogue:AddLine(text, olivia)
            event = vt_map.DialogueEvent.Create("Olivia speech before entering well", dialogue)
            event:AddEventLinkAtEnd("Olivia enters the well")
            event = vt_map.PathMoveSpriteEvent.Create("Olivia enters the well", olivia, 63, 32, false)
            event:AddEventLinkAtEnd("Set Olivia invisible")
            event = vt_map.ScriptedEvent.Create("Set Olivia invisible", "set_olivia_invisible", "")
            event:AddEventLinkAtEnd("Bronann enters the well")
            event = vt_map.PathMoveSpriteEvent.Create("Bronann enters the well", bronann, 63, 32, false)
            event:AddEventLinkAtEnd("To well underground") -- map transition
        else
            olivia:AddDialogueReference(default_dialogue)
        end

        -- Add the map even to prevent going to the forest
        event = vt_map.DialogueEvent.Create("Bronann can't enter the forest so easily", default_dialogue)
        event:SetStopCameraMovement(true)
    else
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_olivia_access_to_forest")
        text = vt_system.Translate("Good luck Bronann.")
        dialogue:AddLine(text, olivia)
        olivia:AddDialogueReference(dialogue)
    end
end

-- Updates Georges dialogue depending on how far is the story going.
function _UpdateGeorgesDialogue()
    local text = nil
    local dialogue = nil

    georges:ClearDialogueReferences();

    if (GlobalManager:DoesEventExist("layna_center", "quest1_pen_given_done") == true) then
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_georges_default");
        text = vt_system.Translate("Ah, the river is so beautiful at this time of the year. I feel like writing some poetry...");
        dialogue:AddLine(text, georges);
        georges:AddDialogueReference(dialogue);
    elseif (GlobalManager:DoesEventExist("layna_riverbank", "quest1_orlinn_hide_n_seek3_done") == true) then
        -- Give the pen to Georges
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Here it is, Georges.");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("You're the nicest person I know, Bronann. I will tell everyone how brave you...");
        dialogue:AddLine(text, georges);
        text = vt_system.Translate("(sigh)... Georges!");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Ok ok. Just having a bit of spirit, young man.");
        dialogue:AddLineEvent(text, georges, "", "Quest1: Georges thanks Bronann");
        georges:AddDialogueReference(dialogue);
        return;
    elseif (GlobalManager:DoesEventExist("layna_center", "quest1_georges_dialogue_done") == true) then
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_georges_pen_lost_short");
        text = vt_system.Translate("You see, I lost my beloved pen. Was it near a tree or next to the waving child of the mountain snow?");
        dialogue:AddLine(text, georges);
        text = vt_system.Translate("Shall you find it, I would be extremely obliged to you!");
        dialogue:AddLine(text, georges);
        text = vt_system.Translate("Inquire our comrades in the settlement, perhaps somebody has laid their eyes on it.");
        dialogue:AddLine(text, georges);
        text = vt_system.Translate("(sigh...) Hmm, fine. I'll have a look.");
        dialogue:AddLine(text, bronann);
        georges:AddDialogueReference(dialogue);
    else
        -- Once talked to him after the shop conversation, just put the end of the dialogue
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_georges_pen_lost");
        text = vt_system.Translate("Oh Bronann, sad is the poet today.");
        dialogue:AddLine(text, georges);
        text = vt_system.Translate("Hi Georges, well, I was just passing by you know, and");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("You see, I lost my beloved pen. Was it near a tree or next to the waving child of the mountain snow?");
        dialogue:AddLine(text, georges);
        text = vt_system.Translate("Shall you find it, I would be extremely obliged to you!");
        dialogue:AddLine(text, georges);
        text = vt_system.Translate("Inquire our comrades in the settlement, perhaps somebody has laid their eyes on it.");
        dialogue:AddLine(text, georges);
        text = vt_system.Translate("(sigh...) Hmm, fine. I'll have a look.");
        dialogue:AddLineEvent(text, bronann, "", "Quest1: GeorgesDialogueDone");
        georges:AddDialogueReference(dialogue);
        return;
    end
end

-- Updates Orlinn's dialogue and state depending on how far is the story going.
function _UpdateOrlinnAndKalyaState()
    local text = nil
    local dialogue = nil
    local event = nil

    orlinn:ClearDialogueReferences();
    if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == true) then
        -- At that moment, Orlinn has disappeared and Kalya is now in Bronann's party.
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        kalya:ClearDialogueReferences();
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        EventManager:EndAllEvents(kalya);
        kalya:SetMoving(false);
        return;
    end
    if (GlobalManager:DoesEventExist("layna_riverbank", "quest1_orlinn_hide_n_seek3_done") == true) then
        -- Bronann got Georges' pen, update orlinn dialogue
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_orlinn_wont_bother_again");
        text = vt_system.Translate("I promise I won't bother you again.");
        dialogue:AddLine(text, orlinn);
        text = vt_system.Translate("Don't worry about that, Orlinn. Ok?");
        dialogue:AddLine(text, bronann);
        orlinn:AddDialogueReference(dialogue);

        -- Update kalya's dialogue too
        kalya:ClearDialogueReferences();
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("...");
        dialogue:AddLine(text, kalya);
        kalya:AddDialogueReference(dialogue);

    elseif (GlobalManager:DoesEventExist("layna_center", "quest1_orlinn_dialogue1_done") == true) then
        -- At that time, Orlinn isn't in the village center anymore.
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Kalya propose to seek him for you
        kalya:ClearDialogueReferences();
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Kalya, have you seen Orlinn?");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Why? Oh, don't tell me. He's hiding somewhere.");
        dialogue:AddLineEmote(text, kalya, "thinking dots");
        text = vt_system.Translate("Yes, he took Georges pen and I need to give it back to him...");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Hmpf, no surprise. Do you want me to bring him back?");
        dialogue:AddLineEmote(text, kalya, "sweat drop");
        text = vt_system.Translate("Yes, please.");
        dialogue:AddOptionEvent(text, 15, "Kalya brings back Orlinn event start"); -- 15 is after the dialogue's end on purpose.
        text = vt_system.Translate("No, it's not that bad.");
        dialogue:AddOption(text, 4);
        text = vt_system.Translate("As you wish.");
        dialogue:AddLine(text, kalya);
        kalya:AddDialogueReference(dialogue);

        return;
    elseif (GlobalManager:DoesEventExist("layna_center", "quest1_georges_dialogue_done") == true) then
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Hee hee hee!");
        dialogue:AddLineEvent(text, orlinn, "Orlinn laughs", "");
        text = vt_system.Translate("What are you laughing about, Orlinn?");
        dialogue:AddLineEmote(text, bronann, "interrogation");
        text = vt_system.Translate("You'll never find it!");
        dialogue:AddLine(text, orlinn);
        text = vt_system.Translate("Huh? Wait! Are you talking about Georges's lost pen?");
        dialogue:AddLineEmote(text, bronann, "exclamation");
        text = vt_system.Translate("Hee hee hee! Yes!");
        dialogue:AddLineEvent(text, orlinn, "Orlinn laughs", "");
        text = vt_system.Translate("Please tell me! Have you found it?");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Maybe yes, maybe no!");
        dialogue:AddLine(text, orlinn);
        text = vt_system.Translate("Oh no, please Orlinn! I need it!");
        dialogue:AddLineEmote(text, bronann, "sweat drop");
        text = vt_system.Translate("Sure, I'll help you. But only if you can catch me!");
        dialogue:AddLine(text, orlinn);
        text = vt_system.Translate("Hee hee! (He'll never find me hiding behind the buildings!)");
        dialogue:AddLineEvent(text, orlinn, "Orlinn laughs", "Quest1: Make Orlinn run and hide");

        orlinn:AddDialogueReference(dialogue);
    else
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_orlinn_wanna_play");
        text = vt_system.Translate("Heya bro! Wanna play with me?");
        dialogue:AddLine(text, orlinn);
        orlinn:AddDialogueReference(dialogue);
    end

    -- Default behaviour
    EventManager:EndAllEvents(orlinn);
    -- Add the default event if it doesn't exist
    if (EventManager:DoesEventExist("Orlinn random move") == false) then
        event = vt_map.RandomMoveSpriteEvent.Create("Orlinn random move", orlinn, 4000, 2000);
        event:AddEventLinkAtEnd("Orlinn random move", 3000); -- Loop on itself

    end
    EventManager:StartEvent("Orlinn random move");
end

-- Helps with the two step fade in the forest event
local bright_light_time = 0.0
local bright_light_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

-- Bronann watch Olivia event
local bronann_watch_time = 0

-- move well stone event
local well_rock_y_pos = 32

-- Map Custom functions
map_functions = {

    Quest1GeorgesDialogueDone = function()
        GlobalManager:SetEventValue("layna_center", "quest1_georges_dialogue_done", 1);
        -- Makes Orlinn aware that Bronann has talked to Georges.
        _UpdateOrlinnAndKalyaState();
        _UpdateGeorgesDialogue();
        GlobalManager:AddQuestLog("find_pen");
    end,

    Quest1OrlinnRunAndHide = function()
        orlinn:SetMoving(false); -- in case he's moving
        orlinn:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);
        orlinn:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        orlinn:ClearDialogueReferences();
        EventManager:EndAllEvents(orlinn);
    end,

    orlinn_run_event_end = function()
        -- Updates Orlinn's state
        GlobalManager:SetEventValue("layna_center", "quest1_orlinn_dialogue1_done", 1);
        GlobalManager:AddQuestLog("hide_n_seek_with_orlinn");

        -- Updates Kalya dialogue
        _UpdateOrlinnAndKalyaState();
    end,

    kalya_brings_orlinn_back_start = function()
        -- Use the scene state so that the character can't move by player's input
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        -- Do it twice to ensure this state during the transition to the next dialogue
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        -- Place Orlinn for the event
        orlinn:SetPosition(67, 78);
        orlinn:SetMovementSpeed(vt_map.MapMode.FAST_SPEED);
        orlinn:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        orlinn:SetVisible(true);
        -- Makes her stop wandering
        EventManager:EndAllEvents(kalya);
    end,

    orlinn_comes_back_event_end = function()
        GlobalManager:SetEventValue("layna_riverbank", "quest1_orlinn_hide_n_seek3_done", 1);
        Map:PopState();
        -- Remove it twice since we added it twice.
        Map:PopState();

        -- Updates Kalya, Orlinn and Georges dialogues
        _UpdateOrlinnAndKalyaState();
        _UpdateGeorgesDialogue();
    end,

    Quest1GeorgesTellsBronannAboutLilly = function()
        GlobalManager:SetEventValue("layna_center", "quest1_pen_given_done", 1);

        -- Remove the pen key item from inventory
        local pen_item_id = 70001;
        local inventory_handler = GlobalManager:GetInventoryHandler()
        if (inventory_handler:IsItemInInventory(pen_item_id) == true) then
            inventory_handler:RemoveFromInventory(pen_item_id);
        end

        -- Updates Georges dialogue
        _UpdateGeorgesDialogue();
    end,

    orlinn_laughs = function()
        orlinn:SetCustomAnimation("laughing", 700);
    end,

    Prepare_forest_event = function()
        -- Scene event
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        carson:SetPosition(12, 62);
        carson:SetVisible(false);
        carson:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);

        herth:SetPosition(36, 71);
        herth:SetVisible(true);
        herth:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);

        EventManager:EndAllEvents(kalya);
        kalya:SetPosition(36, 66);
        kalya:SetMoving(false);
        kalya:ClearDialogueReferences();

        -- hide Orlinn has he's into the forest
        EventManager:EndAllEvents(orlinn);
        orlinn:SetMoving(false);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end,

    set_carson_visible = function()
        carson:SetVisible(true)
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

    Add_kalya_to_party = function()
        if (GlobalManager:DoesEventExist("story", "kalya_has_joined") == false) then
            GlobalManager:GetCharacterHandler():AddCharacter(KALYA);
            GlobalManager:SetEventValue("story", "kalya_has_joined", 1);
            GlobalManager:AddQuestLog("bring_orlinn_back");
        end
        AudioManager:FadeInActiveMusic(2000);

        -- Now, the event is done, update Olivia's dialogue as access to the forest is granted
        _UpdateOliviaDialogue();
    end,

    Quest2_equip_speech_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        bronann:SetMoving(false);
        bronann:SetDirection(vt_map.MapMode.WEST);
        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        kalya:SetDirection(vt_map.MapMode.WEST);
        kalya:SetVisible(true);
        kalya:ClearDialogueReferences();

        -- Set the event as done to prevent it to trigger again
        GlobalManager:SetEventValue("story", "Quest2_kalya_equip_n_dungeons_speech_done", 1)
    end,

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

    Well_event_scene_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE)
        EventManager:EndAllEvents(olivia)
        olivia:SetMoving(false)
        bronann:SetMoving(false)
        -- Olivia can get stuck because of Bronann start position*
        olivia:SetCollisionMask(vt_map.MapMode.WALL_COLLISION)
    end,

    bronann_watch_olivia_start = function()
        bronann:LookAt(olivia)
        bronann_watch_time = 0.0
    end,

    bronann_watch_olivia_update = function()
        bronann_watch_time = bronann_watch_time + SystemManager:GetUpdateTime()
        if (bronann_watch_time > 1000) then
            return true
        end
        bronann:LookAt(olivia)
        return false
    end,

    well_open_trigger_start = function()
        Effects:ShakeScreen(0.6, 1000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL)
        AudioManager:PlaySound("data/sounds/cave-in.ogg")
        -- Place the stone
        well_rock_y_pos = 32
    end,

    well_open_trigger_update = function()
        well_rock_y_pos = well_rock_y_pos + (SystemManager:GetUpdateTime() * 0.01)
        well_entrance_rock:SetYPosition(well_rock_y_pos)
        if (well_rock_y_pos < 34) then
            return false
        end
        return true
    end,

    set_olivia_invisible = function()
        olivia:SetVisible(false)
    end
}
