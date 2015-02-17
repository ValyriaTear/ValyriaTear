-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine1_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "img/menus/locations/mt_elbrus.png"
map_subname = "Underpass"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/icy_wind.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Script = nil

-- the main character handler
local hero = nil

-- Dialogue sprites
local bronann = nil
local kalya = nil
local orlinn = nil
local sophia = nil
local nekko = nil

-- Objects used during the door opening scene
local shrine_entrance_door = nil
local shrine_entrance_sign = nil

local shrine_flame1 = nil
local shrine_flame2 = nil

-- the main map loading code
function Load(m)

    Map = m;
    Script = Map:GetScriptSupervisor();
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(true);

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay when necessary
    if (GlobalManager:GetEventValue("story", "mountain_shrine_entrance_light_done") == 0) then
        Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);
    end

    -- Event Scripts
    Script:AddScript("dat/maps/mt_elbrus/shrine_entrance_show_crystal_script.lua");

    -- Start the dialogue about the shrine entrance if not done
    if (GlobalManager:GetEventValue("story", "mt_elbrus_shrine_entrance_event") ~= 1) then
        hero:SetMoving(false);
        EventManager:StartEvent("Shrine entrance event start", 200);
    elseif (GlobalManager:GetEventValue("story", "mt_elbrus_shrine_sophia_dialogue_event") == 0 and
            GlobalManager:GetEventValue("story", "mountain_shrine_entrance_light_done") == 1) then
        if (GlobalManager:GetPreviousLocation() == "from_shrine_main_room") then
            -- (Re)Introduce Sophia when the characters leave the shrine the first time from the north entrance.
            hero:SetMoving(false);
            EventManager:StartEvent("Sophia introduction event", 200);
        end
    end

    if (GlobalManager:GetEventValue("story", "mt_elbrus_shrine_door_opening_event") == 1) then
        _set_shrine_door_open();
        shrine_entrance_sign:SetVisible(true);
        _show_flames();
    end

    -- Preload sounds
    AudioManager:LoadSound("snd/heartbeat_slow.wav", Map);
    AudioManager:LoadSound("snd/ancient_invocation.wav", Map);
    AudioManager:LoadSound("snd/cave-in.ogg", Map);

    -- Loads the funny music if needed.
    if (GlobalManager:GetEventValue("story", "mt_elbrus_shrine_sophia_dialogue_event") == 0) then
        AudioManager:LoadMusic("mus/Zander Noriega - School of Quirks.ogg", Map);
    end
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- set up/updates sophia's events
function _UpdateSophiaDialogue()
    local text = nil
    local dialogue = nil
    local event = nil

    sophia:ClearDialogueReferences();
    if (GlobalManager:GetEventValue("story", "mt_elbrus_shrine_sophia_dialogue_event") == 0) then
        return
    end

    -- Shopping dialogue.
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I'll stay here as long as you need me. Do you want to buy something?");
    dialogue:AddLineEvent(text, sophia, "", "Trade with Sophia");
    sophia:AddDialogueReference(dialogue);

    -- Shopping event
    event = vt_map.ShopEvent.Create("Trade with Sophia");
    event:SetShopName(vt_system.UTranslate("Sophia's Items"));
    event:SetGreetingText(vt_system.UTranslate("Don't forget my trade offers!"));
    event:SetSellModeEnabled(false); -- prevents selling items there.
    event:AddItem(1, 0); -- infinite minor potions
    event:AddItem(11, 0); -- infinite minor moon juices
    event:AddItem(1001, 0); -- infinite minor elixirs
    event:AddItem(15, 0); -- infinite Lotus petals (cure poison)
    event:AddItem(16, 0); -- infinite Candies (regen)
    -- The interesting part!
    event:AddTrade(10002, 1); -- Reinforced Wooden Sword (with magical attack)
    event:AddTrade(10012, 1); -- Soldier sword. (a strong sword)
    event:AddTrade(11002, 1); -- Noble Arbalest (Strong attack arbalest)
    event:AddTrade(11003, 1); -- Arbalest of force
    event:SetPriceLevels(vt_shop.ShopMode.SHOP_PRICE_STANDARD,
                         vt_shop.ShopMode.SHOP_PRICE_STANDARD);
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from mountain path 4)
    hero = CreateSprite(Map, "Bronann", 29, 44.5, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    local x_position = GlobalManager:GetSaveLocationX();
    local y_position = GlobalManager:GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Use the save point position, and clear the save position data for next maps
        GlobalManager:UnsetSaveLocation();
        -- Make the character look at us in that case
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(x_position, y_position);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_main_room") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(42.0, 9.0);
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

    sophia = CreateNPCSprite(Map, "Woman2", vt_system.Translate("Sophia"), 42, 21, vt_map.MapMode.GROUND_OBJECT);
    sophia:SetDirection(vt_map.MapMode.NORTH);
    sophia:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    sophia:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    sophia:SetVisible(false);

    -- Add her cat, Nekko
    nekko = CreateObject(Map, "Cat1", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    nekko:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    nekko:SetVisible(false);

    vt_map.SoundEvent.Create("Nekko says Meoww!", "snd/meow.wav");
    nekko:SetEventWhenTalking("Nekko says Meoww!");

    -- When returning from a first trip in the dungeon, the characters fall on Sophia.
    if (GlobalManager:GetEventValue("story", "mountain_shrine_entrance_light_done") == 1
            or GlobalManager:GetEventValue("story", "mt_elbrus_shrine_sophia_dialogue_event") == 1) then
        sophia:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        sophia:SetVisible(true);
        sophia:SetPosition(42.0, 21.0);
        sophia:SetDirection(vt_map.MapMode.NORTH);

        nekko:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        nekko:SetVisible(true);
        nekko:SetPosition(44.0, 24.0);
    end
    _UpdateSophiaDialogue();
end

-- The heal particle effect map object
local heal_effect = nil

function _CreateObjects()
    local object = nil
    local npc = nil
    local dialogue = nil
    local text = nil
    local event = nil

    vt_map.SavePoint.Create(51, 29);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject.Create("dat/effects/particles/heal_particle.lua", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    heal_effect:Stop(); -- Don't run it until the character heals itself

    object = CreateObject(Map, "Layna Statue", 41, 28, vt_map.MapMode.GROUND_OBJECT);
    object:SetEventWhenTalking("Heal dialogue");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Your party feels better.");
    dialogue:AddLineEvent(text, nil, "Heal event", ""); -- 'nil' means no portrait and no name
    vt_map.DialogueEvent.Create("Heal dialogue", dialogue);

    -- Snow effect at shrine entrance
    vt_map.ParticleObject.Create("dat/maps/mt_elbrus/particles_snow_south_entrance.lua", 29, 48, vt_map.MapMode.GROUND_OBJECT);
    vt_map.Halo.Create("img/misc/lights/torch_light_mask.lua", 29, 55,
        vt_video.Color(1.0, 1.0, 1.0, 0.8));

    -- Adds the north gate
    shrine_entrance_door = CreateObject(Map, "Door1_big", 42, 4, vt_map.MapMode.GROUND_OBJECT);

    -- Adds a hidden sign, show just before the opening of the door
    shrine_entrance_sign = CreateObject(Map, "Ancient_Sign1", 42, 10, vt_map.MapMode.FLATGROUND_OBJECT);
    shrine_entrance_sign:SetVisible(false);

    -- Flames that are burning after the opening of the shrine.
    shrine_flame1 = CreateObject(Map, "Flame1", 33, 9.1, vt_map.MapMode.GROUND_OBJECT);
    shrine_flame2 = CreateObject(Map, "Flame1", 51, 9.1, vt_map.MapMode.GROUND_OBJECT);
    shrine_flame1:SetVisible(false);
    shrine_flame2:SetVisible(false);
    shrine_flame1:RandomizeCurrentAnimationFrame();
    shrine_flame2:RandomizeCurrentAnimationFrame();

    -- When the lighting has improved, show the source of it.
    if (GlobalManager:GetEventValue("story", "mountain_shrine_entrance_light_done") == 1) then
        vt_map.Halo.Create("img/misc/lights/torch_light_mask.lua", 42, 8, vt_video.Color(1.0, 1.0, 1.0, 0.6));
        -- Adds a door horizon...
        object = vt_map.PhysicalObject.Create(vt_map.MapMode.FLATGROUND_OBJECT);
        object:SetPosition(42, 0.8);
        object:SetCollHalfWidth(0.5);
        object:SetCollHeight(1.0);
        object:SetImgHalfWidth(0.5);
        object:SetImgHeight(1.0);
        object:AddStillFrame("dat/maps/mt_elbrus/shrine_entrance_light.png");
    end
end

-- Special event references which destinations must be updated just before being called.
-- shrine entrance event
local kalya_move_next_to_bronann_event1 = nil
local orlinn_move_next_to_bronann_event1 = nil
-- Shrine door opening event
local kalya_move_next_to_bronann_event2 = nil
local orlinn_move_next_to_bronann_event2 = nil
-- Sophia event
local kalya_move_next_to_bronann_event3 = nil
local orlinn_move_next_to_bronann_event3 = nil
local sophia_move_next_to_bronann_event = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    vt_map.MapTransitionEvent.Create("to mountain shrine", "dat/maps/mt_elbrus/mt_elbrus_shrine2_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine2_script.lua", "from_shrine_entrance");

    vt_map.MapTransitionEvent.Create("to mountain shrine-waterfalls", "dat/maps/mt_elbrus/mt_elbrus_shrine2_2_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine2_script.lua", "from_shrine_entrance");

    vt_map.MapTransitionEvent.Create("to mountain bridge", "dat/maps/mt_elbrus/mt_elbrus_path4_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_path4_script.lua", "from_shrine_entrance");

    -- Heal point
    vt_map.ScriptedEvent.Create("Heal event", "heal_party", "heal_done");

    -- Generic events
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks west", orlinn, vt_map.MapMode.WEST);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks south", orlinn, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks north", bronann, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks west", kalya, vt_map.MapMode.WEST);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks south", kalya, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Sophia looks north", sophia, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Sophia looks west", sophia, vt_map.MapMode.WEST);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Orlinn", kalya, orlinn);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Kalya", bronann, kalya);
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Kalya", orlinn, kalya);
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Bronann", orlinn, bronann);

    -- entrance in the map event
    event = vt_map.ScriptedEvent.Create("Shrine entrance event start", "shrine_entrance_event_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann1", 100);
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann1", 100);

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_bronann_event1 = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann1", kalya, 0, 0, false);
    kalya_move_next_to_bronann_event1:AddEventLinkAtEnd("Kalya moves a bit");
    kalya_move_next_to_bronann_event1:AddEventLinkAtEnd("Bronann moves a bit");

    orlinn_move_next_to_bronann_event1 = vt_map.PathMoveSpriteEvent.Create("Orlinn moves next to Bronann1", orlinn, 0, 0, false);
    orlinn_move_next_to_bronann_event1:AddEventLinkAtEnd("Orlinn moves near the passway");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya moves a bit", kalya, 31, 39, false);
    event:AddEventLinkAtEnd("Kalya looks at Bronann");
    event:AddEventLinkAtEnd("Dialogue about the passage to Estoria", 500);

    event = vt_map.PathMoveSpriteEvent.Create("Bronann moves a bit", bronann, 29, 39, false);
    event:AddEventLinkAtEnd("Bronann looks at Kalya");

    -- Orlinn move near the passway
    event = vt_map.PathMoveSpriteEvent.Create("Orlinn moves near the passway", orlinn, 29, 33, true);
    event:AddEventLinkAtEnd("Orlinn looks west");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("You'll see. There's plenty of things I need to show you there. Plus, it's a safe place.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Err, sis?");
    dialogue:AddLineEventEmote(text, orlinn, "Orlinn looks at Kalya", "", "sweat drop");
    text = vt_system.Translate("Our elder will also be able to help us. And you'll get a better explanation than I could ever...");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Sis!");
    dialogue:AddLineEvent(text, orlinn, "Orlinn looks at Kalya", "");
    text = vt_system.Translate("One second, Orlinn. I'm trying to...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks north", "Kalya looks at Bronann");
    text = vt_system.Translate("But Kalya, look at the passageway!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    text = vt_system.Translate("What about the...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks north", "");
    text = vt_system.Translate("No!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    event = vt_map.DialogueEvent.Create("Dialogue about the passage to Estoria", dialogue);
    event:AddEventLinkAtEnd("Kalya runs to the blocked passage");
    event:AddEventLinkAtEnd("Bronann looks north");
    event:AddEventLinkAtEnd("Move camera to the obstructed passage");

    vt_map.ScriptedEvent.Create("Move camera to the obstructed passage", "camera_to_passage", "");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya runs to the blocked passage", kalya, 27, 34, true);
    event:AddEventLinkAtEnd("Kalya looks west");
    event:AddEventLinkAtEnd("Dialogue about the passage to Estoria 2");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("No, it can't be.");
    dialogue:AddLineEventEmote(text, kalya, "Orlinn looks at Kalya", "", "sweat drop");
    text = vt_system.Translate("After all we've been through, this...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks north", "");
    event = vt_map.DialogueEvent.Create("Dialogue about the passage to Estoria 2", dialogue);
    event:AddEventLinkAtEnd("Bronann goes near both");

    vt_map.ScriptedEvent.Create("Move camera back to Bronann", "camera_to_bronann", "");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann goes near both", bronann, 28, 36, false);
    event:AddEventLinkAtEnd("Bronann looks north");
    event:AddEventLinkAtEnd("Move camera back to Bronann");
    event:AddEventLinkAtEnd("Dialogue about the passage to Estoria 3");

    vt_map.AnimateSpriteEvent.Create("Orlinn laughs", orlinn, "laughing", 0); -- infinite time.

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Calm down Kalya, there must be a way to go through this somehow.");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    text = vt_system.Translate("Unfortunately, yes. There is one.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks west", "");
    text = vt_system.Translate("We'll have to enter the ancient shrine.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks north", "");
    text = vt_system.Translate("If we wouldn't have gotten here all by ourselves, I would swear Banesore's army pushed us here on purpose.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("What's with this 'ancient shrine'?");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("Some say it is haunted. It was sealed a long time ago, long before I was even born.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "");
    text = vt_system.Translate("Indeed, it must be a long time ago.");
    dialogue:AddLineEventEmote(text, orlinn, "", "Orlinn laughs", "thinking dots");
    text = vt_system.Translate("Orlinn!");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at Orlinn", "", "exclamation");
    text = vt_system.Translate("Anyway, we might not be able to enter there at all. But you're right, let's have a look around first. Who knows?");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "");
    event = vt_map.DialogueEvent.Create("Dialogue about the passage to Estoria 3", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes back to party");
    event:AddEventLinkAtEnd("Kalya goes back to party");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn goes back to party", orlinn, bronann, false);
    event:AddEventLinkAtEnd("Shrine entrance event end");

    vt_map.PathMoveSpriteEvent.Create("Kalya goes back to party", kalya, bronann, false);

    vt_map.ScriptedEvent.Create("Shrine entrance event end", "shrine_entrance_event_end", "");

    -- Event where Bronann opens the shrine's door...
    event = vt_map.ScriptedEvent.Create("Shrine door opening event start", "shrine_door_opening_event_start", "");
    event:AddEventLinkAtEnd("Bronann moves in the middle of platform");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann moves in the middle of platform", hero, 42.0, 8.0, false);
    event:AddEventLinkAtEnd("Bronann looks north");
    event:AddEventLinkAtEnd("Shrine door opening event actual start");

    event = vt_map.ScriptedEvent.Create("Shrine door opening event actual start", "shrine_door_opening_event_start2", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann2", 100);
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann2", 100);

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_bronann_event2 = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann2", kalya, 0, 0, false);
    kalya_move_next_to_bronann_event2:AddEventLinkAtEnd("Kalya looks north");

    orlinn_move_next_to_bronann_event2 = vt_map.PathMoveSpriteEvent.Create("Orlinn moves next to Bronann2", orlinn, 0, 0, false);
    orlinn_move_next_to_bronann_event2:AddEventLinkAtEnd("Orlinn looks north");
    orlinn_move_next_to_bronann_event2:AddEventLinkAtEnd("Dialogue before opening the door", 500);

    vt_map.AnimateSpriteEvent.Create("Bronann kneels", bronann, "kneeling", 0); -- 0 means forever

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Here we are, looking at this huge and wonderful, yet creepy door. I don't like this.");
    dialogue:AddLineEmote(text, kalya, "thinking dots");
    text = vt_system.Translate("It's not like I actually want to open this, but how are we going to do so?");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at Bronann", "Orlinn looks at Bronann", "sweat drop");
    event = vt_map.DialogueEvent.Create("Dialogue before opening the door", dialogue);
    event:AddEventLinkAtEnd("Show hurt effect");

    event = vt_map.ScriptedEvent.Create("Show hurt effect", "hurt_effect_start", "hurt_effect_update")
    event:AddEventLinkAtEnd("Dialogue before opening the door2");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Ow, my chest. It hurts!");
    dialogue:AddLineEventEmote(text, bronann, "Bronann looks south", "Bronann kneels", "exclamation");
    text = vt_system.Translate("The crystal! Orlinn! Let's stand back!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    event = vt_map.DialogueEvent.Create("Dialogue before opening the door2", dialogue);
    event:AddEventLinkAtEnd("Orlinn rushes down the stairs");
    event:AddEventLinkAtEnd("Kalya rushes down the stairs");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya rushes down the stairs", kalya, 43.0, 16.0, true);
    event:AddEventLinkAtEnd("Kalya looks north");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn rushes down the stairs", orlinn, 41.0, 16.0, true);
    event:AddEventLinkAtEnd("Orlinn goes behind Kalya");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn goes behind Kalya", orlinn, 42.6, 17.0, true);
    event:AddEventLinkAtEnd("Orlinn looks north");
    event:AddEventLinkAtEnd("The crystal opens the door");

    event = vt_map.ScriptedEvent.Create("The crystal opens the door", "show_crystal", "show_crystal_update");
    event:AddEventLinkAtEnd("Dialogue after crystals appearance");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("That sign, it is the sign of the ancients! Bronann! Are you alright?");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    event = vt_map.DialogueEvent.Create("Dialogue after crystals appearance", dialogue);
    event:AddEventLinkAtEnd("Bronann gets up", 1200);

    -- Simply stop the custom animation
    event = vt_map.ScriptedSpriteEvent.Create("Bronann gets up", bronann, "Terminate_all_events", "");
    event:AddEventLinkAtEnd("Dialogue after crystals appearance2", 1000);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I'm fine, I guess. The pain faded away.");
    dialogue:AddLineEvent(text, bronann, "Bronann looks south", "");
    text = vt_system.Translate("Thank the goddess.");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("Well, the door is open now.");
    dialogue:AddLineEmote(text, kalya, "thinking dots");
    text = vt_system.Translate("Yiek! Do you really want to go there??");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    text = vt_system.Translate("I believe we don't really have a choice. Somehow this place... called me.");
    dialogue:AddLineEventEmote(text, bronann, "Bronann looks north", "", "thinking dots");
    text = vt_system.Translate("Anyway, let's stick together and we'll be fine as always, right?");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Orlinn", "");
    text = vt_system.Translate("I have a bad feeling about all this now.");
    dialogue:AddLineEvent(text, orlinn, "Orlinn looks at Kalya", "");
    event = vt_map.DialogueEvent.Create("Dialogue after crystals appearance2", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes back to party2");
    event:AddEventLinkAtEnd("Kalya goes back to party2");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn goes back to party2", orlinn, bronann, false);
    event:AddEventLinkAtEnd("Shrine door opening event end");

    vt_map.PathMoveSpriteEvent.Create("Kalya goes back to party2", kalya, bronann, false);

    vt_map.ScriptedEvent.Create("Shrine door opening event end", "shrine_door_opening_event_end", "");

    -- Sophia introduction event
    event = vt_map.ScriptedEvent.Create("Sophia introduction event", "sophia_event_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann3", 100);
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann3", 100);

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_bronann_event3 = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann3", kalya, 0, 0, false);
    kalya_move_next_to_bronann_event3:AddEventLinkAtEnd("Kalya looks south");

    orlinn_move_next_to_bronann_event3 = vt_map.PathMoveSpriteEvent.Create("Orlinn moves next to Bronann3", orlinn, 0, 0, false);
    orlinn_move_next_to_bronann_event3:AddEventLinkAtEnd("Orlinn looks south");
    orlinn_move_next_to_bronann_event3:AddEventLinkAtEnd("Everyone is suprised", 500);

    event = vt_map.ScriptedEvent.Create("Everyone is suprised", "everyone_exclamation", "");
    event:AddEventLinkAtEnd("Sophia Dialogue 1");
    event:AddEventLinkAtEnd("Fade out music");

    vt_map.ScriptedEvent.Create("Fade out music", "fade_out_music", "");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Bronann!");
    dialogue:AddLine(text, sophia);
    event = vt_map.DialogueEvent.Create("Sophia Dialogue 1", dialogue);
    event:AddEventLinkAtEnd("Sophia moves near the heroes");
    event:AddEventLinkAtEnd("Play funny music");

    vt_map.ScriptedEvent.Create("Play funny music", "play_funny_music", "");

    -- NOTE: The actual destination is set just before the actual start call
    sophia_move_next_to_bronann_event = vt_map.PathMoveSpriteEvent.Create("Sophia moves near the heroes", sophia, 0.0, 0.0, false);
    sophia_move_next_to_bronann_event:AddEventLinkAtEnd("Sophia Dialogue 2");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Thank the goddess you're all safe and sound. I was worried sick!");
    dialogue:AddLine(text, sophia);
    text = vt_system.Translate("Sophia, you're here?");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Yes. It's a miracle that you were kept away from harm.");
    dialogue:AddLine(text, sophia);
    text = vt_system.Translate("How in Layna did you reach Mt Elbrus all alone?");
    dialogue:AddLine(text, sophia);
    text = vt_system.Translate("And how did you reach up here? Weren't you caught by the soldiers with the others?");
    dialogue:AddLineEmote(text, kalya, "thinking dots");
    text = vt_system.Translate("Well, it's a long story. But in short, I was out of the village when they caught everyone.");
    dialogue:AddLineEventEmote(text, sophia, "Sophia looks west", "Sophia looks north", "sweat drop");
    text = vt_system.Translate("Do you know how the others are doing? Are my parents okay?");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("Unfortunately no. I left them behind.");
    dialogue:AddLine(text, sophia);
    text = vt_system.Translate("When I saw you leaving the village to the sanctuary, I decided to follow you.");
    dialogue:AddLineEvent(text, sophia, "Sophia looks west", "Sophia looks north");
    text = vt_system.Translate("But by the time I reached the area, the three of you were gone. And I had to find out where you were heading.");
    dialogue:AddLine(text, sophia);
    text = vt_system.Translate("But the bridge, how did you cross?");
    dialogue:AddLineEmote(text, kalya, "thinking dots");
    text = vt_system.Translate("Using my rope and ranger skills, of course.");
    dialogue:AddLine(text, sophia);
    text = vt_system.Translate("Sophia could be of great help, Kalya. The passage has collapsed, and we're stuck finding a way through this dreadful place.");
    dialogue:AddLineEvent(text, bronann, "Bronann looks at Kalya", "Bronann looks south");
    text = vt_system.Translate("Maybe, maybe not. Sophia was out the time they attacked? Somehow I don't buy it.");
    dialogue:AddLineEmote(text, kalya, "thinking dots");
    text = vt_system.Translate("I wasn't out for no reason. Herth sent me to watch for soldiers in the west parts of the forest.");
    dialogue:AddLineEmote(text, sophia, "exclamation");
    text = vt_system.Translate("He was afraid they'd corner us to prevent an escape, and he was right.");
    dialogue:AddLine(text, sophia);
    text = vt_system.Translate("Herth sent you? But he told me it was nothing, that he would handle it!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("I fear he just wanted to protect you and make you all meet your destiny at the same time.");
    dialogue:AddLine(text, sophia);
    text = vt_system.Translate("What?! You mean you all knew about the crystal?");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("We didn't know exactly what would happen, Bronann. But we knew one day the three of you would have to carry a special burden. That is what Herth told us.");
    dialogue:AddLineEventEmote(text, sophia, "Sophia looks west", "Sophia looks north", "sweat drop");
    text = vt_system.Translate("Listen kids, I feel bad about what happened. But I can't help you as a magical force is preventing me from entering the shrine.");
    dialogue:AddLine(text, sophia);
    text = vt_system.Translate("But then you're stuck here, too, right?");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("Me? Not at all. I don't need bridges to cross a moutain.");
    dialogue:AddLineEmote(text, sophia, "interrogation");
    text = vt_system.Translate("But maybe?");
    dialogue:AddLineEmote(text, sophia, "thinking dots");
    text = vt_system.Translate("Maybe?");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Maybe I could sell you a few things to help you find a way to open a path in there?");
    dialogue:AddLineEmote(text, sophia, "exclamation");
    text = vt_system.Translate("So that's how you help, by selling things to us...");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("Yes, it's a great privilege as I usually don't do such things for children. I'll stay here as long as you are stuck, isn't this quite nice?");
    dialogue:AddLine(text, sophia);
    text = vt_system.Translate("But you'll let us deal with this place, alone?");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("This place is part of your burden. Herth was right. You managed to come here all by yourselves, after all.");
    dialogue:AddLine(text, sophia);
    text = vt_system.Translate("I guess it's better than nothing. But Orlinn, you stay with us.");
    dialogue:AddLineEventEmote(text, kalya, "Orlinn looks at Kalya", "Orlinn looks south", "thinking dots");
    text = vt_system.Translate("Thanks anyway, Sophia.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("You're welcome, my dear!");
    dialogue:AddLine(text, sophia);

    event = vt_map.DialogueEvent.Create("Sophia Dialogue 2", dialogue);
    event:AddEventLinkAtEnd("Sophia moves to her former place");

    event = vt_map.PathMoveSpriteEvent.Create("Sophia moves to her former place", sophia, 42.0, 21.0, false);
    event:AddEventLinkAtEnd("Sophia looks north");
    event:AddEventLinkAtEnd("Orlinn goes back to party3");
    event:AddEventLinkAtEnd("Kalya goes back to party3");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn goes back to party3", orlinn, bronann, false);
    event:AddEventLinkAtEnd("Sophia introduction event end");

    vt_map.PathMoveSpriteEvent.Create("Kalya goes back to party3", kalya, bronann, false);

    vt_map.ScriptedEvent.Create("Sophia introduction event end", "sophia_event_end", "");
end

-- zones
local to_shrine_zone = nil
local to_mountain_bridge_zone = nil
local shrine_door_opening_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_zone = vt_map.CameraZone.Create(40, 44, 2, 4);
    to_mountain_bridge_zone = vt_map.CameraZone.Create(26, 32, 46, 48);
    shrine_door_opening_zone = vt_map.CameraZone.Create(40, 44, 8, 10);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 0) then
            EventManager:StartEvent("to mountain shrine");
        else
            EventManager:StartEvent("to mountain shrine-waterfalls");
        end
    elseif (to_mountain_bridge_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to mountain bridge");
    elseif (shrine_door_opening_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        if (GlobalManager:GetEventValue("story", "mt_elbrus_shrine_door_opening_event") == 0) then
            hero:SetMoving(false);
            EventManager:StartEvent("Shrine door opening event start");
        end
    end
end

-- Opens the shrine door
function _open_shrine_door()
    -- Permit the entrance into the shrine...
    shrine_entrance_door:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    -- Makes the door open
    local opening_anim_id = shrine_entrance_door:AddAnimation("img/sprites/map/objects/door_big1_opening.lua");
    shrine_entrance_door:SetCurrentAnimation(opening_anim_id);
end

-- Set the shrine door as opened
function _set_shrine_door_open()
    -- Permit the entrance into the shrine...
    shrine_entrance_door:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    -- Makes the door open
    local open_anim_id = shrine_entrance_door:AddAnimation("img/sprites/map/objects/door_big1_open.lua");
    shrine_entrance_door:SetCurrentAnimation(open_anim_id);
end

function _show_flames()
    vt_map.SoundObject.Create("snd/campfire.ogg", 33.0, 9.1, 10.0);
    vt_map.SoundObject.Create("snd/campfire.ogg", 51.0, 9.1, 10.0);

    vt_map.Halo.Create("img/misc/lights/torch_light_mask2.lua", 33.0, 9.1 + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    vt_map.Halo.Create("img/misc/lights/sun_flare_light_main.lua", 33.0, 9.1 + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
    vt_map.Halo.Create("img/misc/lights/torch_light_mask2.lua", 51.0, 9.1 + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    vt_map.Halo.Create("img/misc/lights/sun_flare_light_main.lua", 51.0, 9.1 + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));

    shrine_flame1:SetVisible(true);
    shrine_flame2:SetVisible(true);
end

-- Effect time used when applying the heal light effect
local heal_effect_time = 0;
local heal_color = vt_video.Color(0.0, 0.0, 1.0, 1.0);

-- Shown when Bronnan feels bad.
local hurt_effect_time = 0;
local hurt_color = vt_video.Color(1.0, 0.0, 0.0, 1.0);

-- Used in the crystal appearance scene.
local crystal_appearance_time = 0;
local ancient_sign_visible = false;

-- Map Custom functions
-- Used through scripted events
map_functions = {
    heal_party = function()
        hero:SetMoving(false);
        -- Should be sufficient to heal anybody
        GlobalManager:GetActiveParty():AddHitPoints(10000);
        GlobalManager:GetActiveParty():AddSkillPoints(10000);
        Map:SetStamina(10000);
        AudioManager:PlaySound("snd/heal_spell.wav");
        heal_effect:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        heal_effect:Start();
        heal_effect_time = 0;
    end,

    heal_done = function()
        heal_effect_time = heal_effect_time + SystemManager:GetUpdateTime();

        if (heal_effect_time < 300.0) then
            heal_color:SetAlpha(heal_effect_time / 300.0 / 3.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(heal_color);
            return false;
        end

        if (heal_effect_time < 1000.0) then
            heal_color:SetAlpha(((1000.0 - heal_effect_time) / 700.0) / 3.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(heal_color);
            return false;
        end
        return true;
    end,

    shrine_entrance_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

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

    camera_to_passage = function()
        Map:MoveVirtualFocus(15, 30);
        Map:SetCamera(Map:GetVirtualFocus(), 1000);
    end,

    camera_to_hero = function()
        Map:SetCamera(hero, 2000);
    end,

    camera_to_bronann = function()
        Map:SetCamera(bronann, 2000);
    end,

    shrine_entrance_event_end = function()
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
        GlobalManager:SetEventValue("story", "mt_elbrus_shrine_entrance_event", 1);
    end,

    shrine_door_opening_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    shrine_door_opening_event_start2 = function()
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

    hurt_effect_start = function()
        hurt_effect_time = 0;
        AudioManager:PlaySound("snd/heartbeat_slow.wav");
    end,

    hurt_effect_update = function()
        hurt_effect_time = hurt_effect_time + SystemManager:GetUpdateTime();

        if (hurt_effect_time < 300.0) then
            hurt_color:SetAlpha(hurt_effect_time / 300.0 / 3.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(hurt_color);
            return false;
        end

        if (hurt_effect_time < 600.0) then
            hurt_color:SetAlpha(((600.0 - hurt_effect_time) / 300.0) / 3.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(hurt_color);
            return false;
        end
        return true;
    end,

    show_crystal = function()
        -- Triggers the crystal appearance
        GlobalManager:SetEventValue("scripts_events", "shrine_entrance_show_crystal", 1)
        crystal_appearance_time = 0;
        ancient_sign_visible = false;
    end,

    show_crystal_update = function()
        -- Show the ancient sign on ground.
        if (ancient_sign_visible == false) then
            crystal_appearance_time = crystal_appearance_time + SystemManager:GetUpdateTime();
            if (crystal_appearance_time >= 8000) then
                shrine_entrance_sign:SetVisible(true);
                ancient_sign_visible = true;
                AudioManager:PlaySound("snd/ancient_invocation.wav");
            end
        end
        if (GlobalManager:GetEventValue("scripts_events", "shrine_entrance_show_crystal") == 0) then
            Map:GetEffectSupervisor():ShakeScreen(0.4, 2200, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);
            AudioManager:PlaySound("snd/cave-in.ogg");
            _open_shrine_door();
            -- Show a slight fire spiral effect.
            Map:GetParticleManager():AddParticleEffect("dat/effects/particles/fire_spiral.lua", 512.0, 284.0);
            _show_flames();
            return true;
        end
        return false;
    end,

    shrine_door_opening_event_end = function()
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
        GlobalManager:SetEventValue("story", "mt_elbrus_shrine_door_opening_event", 1);
    end,

    sophia_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

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

        kalya_move_next_to_bronann_event3:SetDestination(bronann:GetXPosition() - 2.0, bronann:GetYPosition(), false);
        orlinn_move_next_to_bronann_event3:SetDestination(bronann:GetXPosition() + 2.0, bronann:GetYPosition(), false);

        sophia_move_next_to_bronann_event:SetDestination(bronann:GetXPosition(), bronann:GetYPosition() + 3.0, false);
    end,

    everyone_exclamation = function()
        hero:Emote("exclamation", vt_map.MapMode.ANIM_SOUTH);
        kalya:Emote("exclamation", vt_map.MapMode.ANIM_SOUTH);
        orlinn:Emote("exclamation", vt_map.MapMode.ANIM_SOUTH);
        sophia:Emote("exclamation", vt_map.MapMode.ANIM_NORTH);
    end,

    fade_out_music = function()
        AudioManager:FadeOutActiveMusic(300);
    end,

    play_funny_music = function()
        AudioManager:PlayMusic("mus/Zander Noriega - School of Quirks.ogg");
    end,

    sophia_event_end = function()
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
        GlobalManager:SetEventValue("story", "mt_elbrus_shrine_sophia_dialogue_event", 1);

        -- Adds Sophia's shop dialogue
        _UpdateSophiaDialogue();

        -- Fade in the default music
        AudioManager:PlayMusic("mus/icy_wind.ogg");
    end,
}
