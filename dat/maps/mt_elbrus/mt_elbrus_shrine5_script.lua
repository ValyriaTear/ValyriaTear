-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine5_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus Shrine"
map_image_filename = "img/menus/locations/mountain_shrine.png"
map_subname = "1st Floor"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/mountain_shrine.ogg"

-- c++ objects instances
local Map = nil
local DialogueManager = nil
local EventManager = nil
local Script = nil

-- the main character handler
local hero = nil

-- Forest dialogue secondary hero
local kalya = nil
local orlinn = nil
local bronann = nil -- A copy of Bronann, used to simplify some scripting.

-- Name of the main sprite. Used to reload the good one at the end of dialogue events.
local main_sprite_name = "";

-- the main map loading code
function Load(m)

    Map = m;
    Script = Map:GetScriptSupervisor();
    DialogueManager = Map:GetDialogueSupervisor();
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(false);

    _CreateCharacters();
    _CreateObjects();

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);

    -- Preloads the action sounds to avoid glitches
    AudioManager:LoadSound("snd/opening_sword_unsheathe.wav", Map);
    AudioManager:LoadSound("snd/stone_roll.wav", Map);
    AudioManager:LoadSound("snd/stone_bump.ogg", Map);
    AudioManager:LoadSound("snd/falling.ogg", Map);
    AudioManager:LoadSound("snd/bump.wav", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
    -- Check wether the monsters have been defeated
    _CheckMonstersStates();

    _CheckStoneAndTriggersCollision();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from shrine main room)
    hero = CreateSprite(Map, "Bronann", 10.0, 12.5, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    kalya = CreateSprite(Map, "Kalya", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.EAST);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);

    orlinn = CreateSprite(Map, "Orlinn", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.FAST_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);

    bronann = CreateSprite(Map, "Bronann", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.EAST);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    bronann:SetVisible(false);

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map:SetPartyMemberVisibleSprite(hero);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_SW_left_door") then
        hero:SetPosition(16, 36);
        hero:SetDirection(vt_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_SW_right_door") then
        hero:SetPosition(28, 36);
        hero:SetDirection(vt_map.MapMode.NORTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_2nd_floor") then
        hero:SetPosition(20, 12);
        hero:SetDirection(vt_map.MapMode.SOUTH);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_NE_room") then
        -- In that case, Orlinn is back from the top-right passage,
        -- and the player is incarnating him.
        orlinn:SetPosition(44, 10);
        orlinn:SetVisible(true);
        orlinn:SetDirection(vt_map.MapMode.WEST);
        orlinn:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        Map:SetCamera(orlinn);

        -- Hide the hero sprite for now.
        hero:SetPosition(0, 0);
        hero:SetVisible(false);

        -- Make Bronann and Kalya wait for him
        bronann:SetPosition(33, 18);
        bronann:SetVisible(true);
        bronann:SetDirection(vt_map.MapMode.WEST);

        kalya:SetPosition(31, 18)
        kalya:SetVisible(true);
        kalya:SetDirection(vt_map.MapMode.EAST);

        -- The menu and status effects are disabled for now.
        Map:SetMenuEnabled(false);
        Map:SetStatusEffectsEnabled(false);
    end
end

-- Trigger and stone
local stone_trigger2 = nil
local rolling_stone2 = nil

-- Flames preventing from getting through
local fence1_trigger1 = nil
local fence2_trigger1 = nil
local fence1_trigger2 = nil
local fence2_trigger2 = nil

-- Monster trap object
local trap_spikes = nil

-- The grid preventing from going to the second floor.
local second_floor_gate = nil

-- Object used to trigger Orlinn going up event
local passage_event_object = nil
local passage_back_event_object = nil

function _CreateObjects()
    local object = nil
    local npc = nil
    local dialogue = nil
    local text = nil
    local event = nil

    _add_flame(13.5, 7);
    _add_flame(43.5, 6);

    CreateObject(Map, "Vase3", 24, 35, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Vase2", 8, 27, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Vase4", 26, 13, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Vase4", 27, 15, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Jar1", 6, 33, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Vase3", 8, 35, vt_map.MapMode.GROUND_OBJECT);

    CreateObject(Map, "Candle Holder1", 16, 11, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Candle Holder1", 24, 11, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 43, 26, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 41, 28, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 39, 30, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 37, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 37, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 37, 36, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Stone Fence1", 35, 20, vt_map.MapMode.GROUND_OBJECT);

    trap_spikes = CreateObject(Map, "Spikes1", 14, 26, vt_map.MapMode.GROUND_OBJECT);
    trap_spikes:SetVisible(false);
    trap_spikes:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    -- Add an invisible object permitting to trigger the high passage events
    passage_event_object = CreateObject(Map, "Stone Fence1", 33, 17, vt_map.MapMode.GROUND_OBJECT);
    passage_event_object:SetVisible(false);
    passage_event_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    -- Add an invisible object permitting Orlinn to return with Bronann and Kalya
    passage_back_event_object = CreateObject(Map, "Stone Fence1", 33, 14, vt_map.MapMode.GROUND_OBJECT);
    passage_back_event_object:SetVisible(false);
    passage_back_event_object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    -- Permits to come back when incarnating Orlinn
    if (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor_NE_room") then
        passage_back_event_object:SetEventWhenTalking("Orlinn comes back event start");
    end

    -- The stone trigger that will open the gate to the second floor
    stone_trigger2 = vt_map.TriggerObject.Create("mt elbrus shrine 5 trigger 2",
                                                 vt_map.MapMode.FLATGROUND_OBJECT,
                                                 "img/sprites/map/triggers/rolling_stone_trigger1_off.lua",
                                                 "img/sprites/map/triggers/rolling_stone_trigger1_on.lua",
                                                 "",
                                                 "Open Gate");
    stone_trigger2:SetPosition(43, 22);
    stone_trigger2:SetTriggerableByCharacter(false); -- Only an event can trigger it

    second_floor_gate = CreateObject(Map, "Gate1 closed", 20, 10, vt_map.MapMode.GROUND_OBJECT);

    event = vt_map.ScriptedEvent("Open Gate", "open_gate_animated_start", "open_gate_animated_update")
    EventManager:RegisterEvent(event);

    -- Add blocks preventing from using the doors
    -- Left door: Unlocked by beating monsters
    local fence1_trigger1_x_position = 15.0;
    local fence2_trigger1_x_position = 17.0;
    -- Sets the passage open if the enemies were already beaten
    if (GlobalManager:GetEventValue("story", "mountain_shrine_1st_NW_monsters_defeated") == 1) then
        fence1_trigger1_x_position = 13.0;
        fence2_trigger1_x_position = 19.0;
    end
    fence1_trigger1 = CreateObject(Map, "Stone Fence1", fence1_trigger1_x_position, 38, vt_map.MapMode.GROUND_OBJECT);
    fence2_trigger1 = CreateObject(Map, "Stone Fence1", fence2_trigger1_x_position, 38, vt_map.MapMode.GROUND_OBJECT);

    -- Right door: Using a switch
    local fence1_trigger2_x_position = 27.0;
    local fence2_trigger2_x_position = 29.0;
    -- Sets the passage open if the trigger is pushed
    if (GlobalManager:GetEventValue("triggers", "mt elbrus shrine 6 trigger 1") == 1) then
        fence1_trigger2_x_position = 25.0;
        fence2_trigger2_x_position = 31.0;
    end
    fence1_trigger2 = CreateObject(Map, "Stone Fence1", fence1_trigger2_x_position, 38, vt_map.MapMode.GROUND_OBJECT);
    fence2_trigger2 = CreateObject(Map, "Stone Fence1", fence2_trigger2_x_position, 38, vt_map.MapMode.GROUND_OBJECT);

    rolling_stone2 = CreateObject(Map, "Rolling Stone", 28, 33, vt_map.MapMode.GROUND_OBJECT);
    event = vt_map.IfEvent("Check hero position for rolling stone 2", "check_diagonal_stone2", "Push the rolling stone 2", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedEvent("Push the rolling stone 2", "start_to_move_the_stone2", "move_the_stone_update2")
    EventManager:RegisterEvent(event);

    if (GlobalManager:GetEventValue("story", "mt_shrine_1st_floor_stone2_through_2nd_door") == 0) then
        rolling_stone2:SetVisible(false);
        rolling_stone2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    else
        rolling_stone2:SetEventWhenTalking("Check hero position for rolling stone 2");
    end

    -- Open the gates when the trigger is on and put the stone on it.
    if (GlobalManager:GetEventValue("triggers", "mt elbrus shrine 5 trigger 2") == 1) then
        map_functions.set_gate_opened();
        rolling_stone2:SetPosition(43, 22);
    end

    -- Waterfall
    if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 1) then
        _add_waterfall(44, 46);
    end

end

function _add_waterfall(x, y)
    local object = CreateObject(Map, "Waterfall1", x - 0.1, y - 0.2, vt_map.MapMode.GROUND_OBJECT);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    object:RandomizeCurrentAnimationFrame();

    -- Ambient sound
    object = vt_map.SoundObject.Create("snd/fountain_large.ogg", x, y - 5, 50.0);
    object:SetMaxVolume(0.6);

    -- Particle effects
    object = vt_map.ParticleObject.Create("dat/effects/particles/waterfall_steam.lua", x, y - 15.0, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true);

    object = vt_map.ParticleObject.Create("dat/effects/particles/waterfall_steam_big.lua", x, y + 0.2, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true);
end

function _add_flame(x, y)
    vt_map.SoundObject.Create("snd/campfire.ogg", x, y, 10.0);

    local object = CreateObject(Map, "Flame1", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();

    vt_map.Halo.Create("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    vt_map.Halo.Create("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end


-- high passage event
local kalya_move_next_to_hero_event = nil
local kalya_move_back_to_hero_event = nil
local orlinn_move_next_to_hero_event = nil
local orlinn_move_back_to_hero_event = nil
local kalya_move_next_to_hero_event2 = nil
local orlinn_move_next_to_hero_event2 = nil
local orlinn_move_near_hero_event = nil
local orlinn_goes_above_bronann_event = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    event = vt_map.MapTransitionEvent("to mountain shrine main room", "dat/maps/mt_elbrus/mt_elbrus_shrine2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine2_script.lua", "from_shrine_first_floor");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine main room-waterfalls", "dat/maps/mt_elbrus/mt_elbrus_shrine2_2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine2_script.lua", "from_shrine_first_floor");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine 2nd floor", "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine_stairs_script.lua", "from_shrine_first_floor");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor SW room - left door", "dat/maps/mt_elbrus/mt_elbrus_shrine6_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine6_script.lua", "from_shrine_first_floor_NW_left_door");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor SW room - right door", "dat/maps/mt_elbrus/mt_elbrus_shrine6_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine6_script.lua", "from_shrine_first_floor_NW_right_door");
    EventManager:RegisterEvent(event);
    event = vt_map.MapTransitionEvent("to mountain shrine 1st floor NE room", "dat/maps/mt_elbrus/mt_elbrus_shrine8_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine8_script.lua", "from_shrine_first_floor_NW_room");
    EventManager:RegisterEvent(event);

    -- Generic events
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks south", orlinn, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Orlinn looks west", orlinn, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Kalya looks at Orlinn", kalya, orlinn);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Orlinn looks at Kalya", orlinn, kalya);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Orlinn looks at Bronann", orlinn, hero);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Bronann looks at Orlinn", hero, orlinn);
    EventManager:RegisterEvent(event);

    -- Opens the left passage to the next map.
    event = vt_map.ScriptedEvent("Open south west passage", "open_sw_passage_start", "open_sw_passage_update");
    EventManager:RegisterEvent(event);

    -- 1. Orlinn wants to go to the high passage event (after seeing the stone)
    event = vt_map.ScriptedEvent("High passage discussion event start", "passage_event_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 100);
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann", 100);
    EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_hero_event = vt_map.PathMoveSpriteEvent("Kalya moves next to Bronann", kalya, 0, 0, false);
    kalya_move_next_to_hero_event:AddEventLinkAtEnd("Kalya looks north");
    EventManager:RegisterEvent(kalya_move_next_to_hero_event);
    orlinn_move_next_to_hero_event = vt_map.PathMoveSpriteEvent("Orlinn moves next to Bronann", orlinn, 0, 0, false);
    orlinn_move_next_to_hero_event:AddEventLinkAtEnd("Orlinn looks north");
    orlinn_move_next_to_hero_event:AddEventLinkAtEnd("The heroes discuss about getting to the high passage", 500);
    EventManager:RegisterEvent(orlinn_move_next_to_hero_event);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("There is a passage here but I'm too heavy for those stones jutting out ...");
    dialogue:AddLineEmote(text, hero, "thinking dots");
    text = vt_system.Translate("The fact is, you and me cannot afford standing on those stones. Maybe with a rope?");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("... I'll do it.");
    dialogue:AddLineEvent(text, orlinn, "", "Bronann looks at Orlinn");
    text = vt_system.Translate("What? Are you crazy? There could be monsters up here.");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at Orlinn", "", "exclamation");
    text = vt_system.Translate("Then, I'll deal with them, sis. We can't just stay here, anyway...");
    dialogue:AddLineEvent(text, orlinn, "Orlinn looks at Kalya", "");
    text = vt_system.Translate("Orlinn, you sure have grown up because of all this. Are you sure?");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks north", "", "sweat drop");
    text = vt_system.Translate("I need to ... I guess.");
    dialogue:AddLineEventEmote(text, orlinn, "Orlinn looks north", "", "sweat drop");
    text = vt_system.Translate("Just tell me when you're ready and I'll throw you up here.");
    dialogue:AddLineEvent(text, hero, "Bronann looks at Orlinn", "");
    text = vt_system.Translate("Yiek!");
    dialogue:AddLineEventEmote(text, orlinn, "Orlinn looks at Kalya", "", "sweat drop");
    event = vt_map.DialogueEvent("The heroes discuss about getting to the high passage", dialogue);
    event:AddEventLinkAtEnd("Ready? dialogue");
    EventManager:RegisterEvent(event);

    -- 2. Just ask whether to climb, and show the throw animation
    event = vt_map.ScriptedEvent("Thrown to high passage event start", "thrown_to_passage_event_start", "");
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann2", 100);
    event:AddEventLinkAtEnd("Kalya moves next to Bronann2", 100);
    EventManager:RegisterEvent(event);

    orlinn_move_next_to_hero_event2 = vt_map.PathMoveSpriteEvent("Orlinn moves next to Bronann2", orlinn, 0, 0, false);
    orlinn_move_next_to_hero_event2:AddEventLinkAtEnd("Orlinn looks at Bronann");
    orlinn_move_next_to_hero_event2:AddEventLinkAtEnd("Bronann looks at Orlinn");
    orlinn_move_next_to_hero_event2:AddEventLinkAtEnd("Ready? dialogue", 500);
    EventManager:RegisterEvent(orlinn_move_next_to_hero_event2);
    kalya_move_next_to_hero_event2 = vt_map.PathMoveSpriteEvent("Kalya moves next to Bronann2", kalya, 0, 0, false);
    kalya_move_next_to_hero_event2:AddEventLinkAtEnd("Kalya looks at Orlinn");
    EventManager:RegisterEvent(kalya_move_next_to_hero_event2);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Ready to go up? ...");
    dialogue:AddLineEvent(text, hero, "Bronann looks at Orlinn", "");
    text = vt_system.Translate("Yes, let's go ...");
    dialogue:AddOptionEvent(text, 50, "Orlinn goes closer of the hero"); -- 50 means next line number, past the end of the dialogue
    text = vt_system.Translate("Err, not yet!");
    dialogue:AddOptionEvent(text, 50, "Not thrown event");
    event = vt_map.DialogueEvent("Ready? dialogue", dialogue);
    EventManager:RegisterEvent(event);

    -- Chose "No"
    event = vt_map.ScriptedEvent("Not thrown event", "set_remove_orlinn_at_end", "");
    event:AddEventLinkAtEnd("Orlinn goes back to party");
    event:AddEventLinkAtEnd("Kalya goes back to party");
    EventManager:RegisterEvent(event);

    orlinn_move_back_to_hero_event = vt_map.PathMoveSpriteEvent("Orlinn goes back to party", orlinn, hero, false);
    EventManager:RegisterEvent(orlinn_move_back_to_hero_event);
    kalya_move_back_to_hero_event = vt_map.PathMoveSpriteEvent("Kalya goes back to party", kalya, hero, false);
    kalya_move_back_to_hero_event:AddEventLinkAtEnd("Thrown to high passage event end");
    EventManager:RegisterEvent(kalya_move_back_to_hero_event);

    -- Chose "Yes"
    -- NOTE: The actual destination will be set at event start
    orlinn_move_near_hero_event = vt_map.PathMoveSpriteEvent("Orlinn goes closer of the hero", orlinn, 0, 0, false);
    orlinn_move_near_hero_event:AddEventLinkAtEnd("Jump to high passage");
    EventManager:RegisterEvent(orlinn_move_near_hero_event);

    event = vt_map.ScriptedEvent("Jump to high passage", "jump_to_passage_start", "jump_to_passage_update");
    event:AddEventLinkAtEnd("Post-jump dialogue");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Be careful, ok?");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks north", "", "sweat drop");
    event = vt_map.DialogueEvent("Post-jump dialogue", dialogue);
    event:AddEventLinkAtEnd("Set Camera on Orlinn");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Set Camera on Orlinn", "set_camera_on_orlinn", "set_camera_on_orlinn_update");
    event:AddEventLinkAtEnd("Thrown to high passage event end");
    EventManager:RegisterEvent(event);

    -- Common event end
    event = vt_map.ScriptedEvent("Thrown to high passage event end", "thrown_to_passage_event_end", "");
    EventManager:RegisterEvent(event);

    _UpdatePassageEvent();

    -- Returning back with Bronann and Kalya event.
    event = vt_map.ScriptedEvent("Orlinn comes back event start", "come_back_from_passage_event_start", "");
    event:AddEventLinkAtEnd("Orlinn comes back dialogue", 100);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Wants to come back?");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("Yes!");
    dialogue:AddOptionEvent(text, 50, "Orlinn goes above Bronann"); -- 50 means next line number, past the end of the dialogue
    text = vt_system.Translate("No, not yet.");
    dialogue:AddOptionEvent(text, 50, "Not coming back event");
    event = vt_map.DialogueEvent("Orlinn comes back dialogue", dialogue);
    EventManager:RegisterEvent(event);

    -- Chose "No"
    event = vt_map.ScriptedEvent("Not coming back event", "not_coming_back_event_end", "");
    EventManager:RegisterEvent(event);

    -- Chose "Yes"
    -- NOTE: The actual destination will be set at event start
    orlinn_goes_above_bronann_event = vt_map.PathMoveSpriteEvent("Orlinn goes above Bronann", orlinn, 0, 0, false);
    orlinn_goes_above_bronann_event:AddEventLinkAtEnd("Kalya looks at Orlinn");
    orlinn_goes_above_bronann_event:AddEventLinkAtEnd("Orlinn jumps back animation");
    EventManager:RegisterEvent(orlinn_goes_above_bronann_event);

    -- Orlinns falls on Bronann's head
    event = vt_map.ScriptedEvent("Orlinn jumps back animation", "jumping_back_animation_start", "jumping_back_animation_update");
    event:AddEventLinkAtEnd("Bronann falls on ground");
    event:AddEventLinkAtEnd("Orlinn is surprised", 800);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Orlinn is surprised", "orlinn_is_surprised", "");
    event:AddEventLinkAtEnd("Orlinn moves out of the way");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Bronann falls on ground", "bronann_on_ground_animation", "");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Orlinn moves out of the way", orlinn, 36, 18, true);
    event:AddEventLinkAtEnd("Orlinn is sorry dialogue");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Oops, sorry!");
    dialogue:AddLineEvent(text, orlinn, "Orlinn looks west", "");

    if (GlobalManager:GetEventValue("story", "mt_elbrus_fell_on_bronanns_head") == 0) then
        -- First time
        text = vt_system.Translate("Bronann, are you fine?");
        dialogue:AddLineEventEmote(text, kalya, "Kalya looks at Orlinn", "", "sweat drop");
        text = vt_system.Translate("... I don't know, I guess I felt something crack ...");
        dialogue:AddLine(text, bronann);
    else
        -- Other times
        text = vt_system.Translate("... Not again ...");
        dialogue:AddLine(text, bronann);
    end
    event = vt_map.DialogueEvent("Orlinn is sorry dialogue", dialogue);
    event:AddEventLinkAtEnd("Bronann kneels", 1000);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Bronann kneels", "bronann_kneels", "");
    event:AddEventLinkAtEnd("Bronann gets up", 1000);
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Bronann gets up", "bronann_gets_up", "");
    event:AddEventLinkAtEnd("Set Camera on hero");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Set Camera on hero", "set_camera_on_hero", "set_camera_on_hero_update");
    event:AddEventLinkAtEnd("Orlinn goes back to party2");
    event:AddEventLinkAtEnd("Kalya goes back to party2");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("Orlinn goes back to party2", orlinn, bronann, false);
    EventManager:RegisterEvent(event);
    event = vt_map.PathMoveSpriteEvent("Kalya goes back to party2", kalya, bronann, false);
    event:AddEventLinkAtEnd("Orlinn comes back event end");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Orlinn comes back event end", "come_back_from_passage_event_end", "");
    EventManager:RegisterEvent(event);
end

function _UpdatePassageEvent()
    if (GlobalManager:GetEventValue("story", "mt_elbrus_shrine_high_passage_event_done") == 1) then
        passage_event_object:SetEventWhenTalking("Thrown to high passage event start");
    else
        passage_event_object:SetEventWhenTalking("High passage discussion event start");
    end
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_shrine.png");
    enemy:AddBattleScript("dat/battles/mountain_shrine_battle_anim.lua");
end

-- Enemies to defeat before opening the south-west passage
local roam_zone = nil;
local monsters_defeated = false;

function _CreateEnemies()
    local enemy = nil

    -- Checks whether the enemies there have been already defeated...
    if (GlobalManager:GetEventValue("story", "mountain_shrine_1st_NW_monsters_defeated") == 1) then
        monsters_defeated = true;
        return;
    end

    -- Monsters that can only be beaten once
    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(13, 20, 26, 36);
    if (monsters_defeated == false) then
        enemy = CreateEnemySprite(Map, "Skeleton");
        _SetBattleEnvironment(enemy);
        enemy:NewEnemyParty();
        enemy:AddEnemy(19); -- Skeleton
        enemy:AddEnemy(19);
        enemy:AddEnemy(19);
        enemy:AddEnemy(16); -- Rat
        enemy:NewEnemyParty();
        enemy:AddEnemy(16);
        enemy:AddEnemy(19);
        enemy:AddEnemy(17); -- Thing
        enemy:AddEnemy(16);
        roam_zone:AddEnemy(enemy, 10);
        roam_zone:SetSpawnsLeft(1); -- These monsters shall spawn only one time.
    end
end

-- check whether all the monsters dies, to open the door
function _CheckMonstersStates()
    if (monsters_defeated == true) then
        return
    end

    if (roam_zone == nil) then
        return
    end

    if (roam_zone:GetSpawnsLeft() > 0) then
        return
    end

    -- Open the south left passage
    monsters_defeated = true;
    hero:SetMoving(false);
    EventManager:StartEvent("Open south west passage", 1000);

    GlobalManager:SetEventValue("story", "mountain_shrine_1st_NW_monsters_defeated", 1);
end

-- zones
local to_shrine_main_room_zone = nil
local to_shrine_2nd_floor_room_zone = nil
local to_shrine_SW_left_door_room_zone = nil
local to_shrine_SW_right_door_room_zone = nil
local to_shrine_NE_room_zone = nil

local monster_trap_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_main_room_zone = vt_map.CameraZone.Create(6, 10, 9, 11);
    to_shrine_2nd_floor_room_zone = vt_map.CameraZone.Create(18, 22, 9, 10);
    to_shrine_SW_left_door_room_zone = vt_map.CameraZone.Create(14, 18, 38, 40);
    to_shrine_SW_right_door_room_zone = vt_map.CameraZone.Create(26, 30, 38, 40);
    to_shrine_NE_room_zone = vt_map.CameraZone.Create(46, 48, 8, 12);
    monster_trap_zone = vt_map.CameraZone.Create(11, 21, 29, 38);
end

local trap_triggered = false;

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_main_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.NORTH);
        if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 0) then
            EventManager:StartEvent("to mountain shrine main room");
        else
            EventManager:StartEvent("to mountain shrine main room-waterfalls");
        end
    elseif (to_shrine_2nd_floor_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine 2nd floor");
    elseif (to_shrine_SW_left_door_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine 1st floor SW room - left door");
    elseif (to_shrine_SW_right_door_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine 1st floor SW room - right door");
    elseif (to_shrine_NE_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.EAST);
        EventManager:StartEvent("to mountain shrine 1st floor NE room");
    elseif (trap_triggered == false and monster_trap_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "mountain_shrine_1st_NW_monsters_defeated") == 0) then
            trap_triggered = true;
            -- Show spikes
            trap_spikes:SetVisible(true);
            trap_spikes:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
            AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
            -- Add the enemies.
            _CreateEnemies();
        else
            trap_triggered = true;
        end
    end
end

function _CheckStoneAndTriggersCollision()
    -- Check trigger
    if (stone_trigger2:GetState() == false) then
        if (stone_trigger2:IsCollidingWith(rolling_stone2) == true) then
            stone_trigger2:SetState(true)
        end
    end
end

function _CheckForDiagonals(target)
    -- Check for diagonals. If the player is in diagonal,
    -- whe shouldn't trigger the event at all, as only straight relative position
    -- to the target sprite will work correctly.
    -- (Here used only for shrooms and stones)

    local hero_x = hero:GetXPosition();
    local hero_y = hero:GetYPosition();

    local target_x = target:GetXPosition();
    local target_y = target:GetYPosition();

    -- bottom-left
    if (hero_y > target_y + 0.3 and hero_x < target_x - 1.2) then return false; end
    -- bottom-right
    if (hero_y > target_y + 0.3 and hero_x > target_x + 1.2) then return false; end
    -- top-left
    if (hero_y < target_y - 1.5 and hero_x < target_x - 1.2) then return false; end
    -- top-right
    if (hero_y < target_y - 1.5 and hero_x > target_x + 1.2) then return false; end

    return true;
end

function _UpdateStoneMovement(stone_object, stone_direction)
    local update_time = SystemManager:GetUpdateTime();
    local movement_diff = 0.015 * update_time;

    -- We cap the max movement distance to avoid making the ball go through obstacles
    -- in case of low FPS
    if (movement_diff > 1.0) then
        movement_diff = 1.0;
    end

    local new_pos_x = stone_object:GetXPosition();
    local new_pos_y = stone_object:GetYPosition();

    -- Apply the movement
    if (stone_direction == vt_map.MapMode.NORTH) then
        new_pos_y = stone_object:GetYPosition() - movement_diff;
    elseif (stone_direction == vt_map.MapMode.SOUTH) then
        new_pos_y = stone_object:GetYPosition() + movement_diff;
    elseif (stone_direction == vt_map.MapMode.WEST) then
        new_pos_x = stone_object:GetXPosition() - movement_diff;
    elseif (stone_direction == vt_map.MapMode.EAST) then
        new_pos_x = stone_object:GetXPosition() + movement_diff;
    end

    -- Check the collision
    if (stone_object:IsColliding(new_pos_x, new_pos_y) == true) then
        AudioManager:PlaySound("snd/stone_bump.ogg");
        return true;
    end

    --  and apply the movement if none
    stone_object:SetPosition(new_pos_x, new_pos_y);

    return false;
end

-- returns the direction the stone shall take
function _GetStoneDirection(stone)

    local hero_x = hero:GetXPosition();
    local hero_y = hero:GetYPosition();

    local stone_x = stone:GetXPosition();
    local stone_y = stone:GetYPosition();

    -- Set the stone direction
    local stone_direction = vt_map.MapMode.EAST;

    -- Determine the hero position relative to the stone
    if (hero_y > stone_y + 0.3) then
        -- the hero is below, the stone is pushed upward.
        stone_direction = vt_map.MapMode.NORTH;
    elseif (hero_y < stone_y - 1.5) then
        -- the hero is above, the stone is pushed downward.
        stone_direction = vt_map.MapMode.SOUTH;
    elseif (hero_x < stone_x - 1.2) then
        -- the hero is on the left, the stone is pushed to the right.
        stone_direction = vt_map.MapMode.EAST;
    elseif (hero_x > stone_x + 1.2) then
        -- the hero is on the right, the stone is pushed to the left.
        stone_direction = vt_map.MapMode.WEST;
    end

    return stone_direction;
end

-- The north east gate y position
local gate_y_position = 10.0;

-- The fire pots x position
local sw_passage_pot1_x = 0.0;
local sw_passage_pot2_x = 0.0;

-- Jump event
local jump_canceled = false;

local jump_event_time = 0;
local kneeling_done = false;
local stop_kneeling_done = false;
local hero_looks_north_done = false;
local orlinn_in_place_to_climb = false;
local time_before_climbing = 0;

-- Going up and back events
local orlinn_y_position = 0;

-- Stone direction
local stone_direction2 = vt_map.MapMode.EAST;

-- Map Custom functions
-- Used through scripted events
map_functions = {
    -- A function making the gate slide up with a noise and removing its collision
    open_gate_animated_start = function()
        gate_y_position = 10.0;
        second_floor_gate:SetPosition(20.0, 10.0);
        second_floor_gate:SetDrawOnSecondPass(false);
        second_floor_gate:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        -- Opening gate sound
        AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
    end,

    open_gate_animated_update = function()
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.015 * update_time;
        gate_y_position = gate_y_position - movement_diff;
        second_floor_gate:SetPosition(20.0, gate_y_position);

        if (gate_y_position <= 7.0) then
            map_functions.set_gate_opened();
            return true;
        end
        return false;

    end,

    -- Set the gate directly to the open state
    set_gate_opened = function()
        second_floor_gate:SetPosition(20.0, 7.0);
        second_floor_gate:SetDrawOnSecondPass(true);
        second_floor_gate:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end,

    -- Opens the south west passage, by moving the fire pots out of the way.
    open_sw_passage_start = function()
        sw_passage_pot1_x = 15.0;
        sw_passage_pot2_x = 17.0;
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    open_sw_passage_update = function()
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.005 * update_time;

        sw_passage_pot1_x = sw_passage_pot1_x - movement_diff;
        fence1_trigger1:SetPosition(sw_passage_pot1_x, 38.0);

        sw_passage_pot2_x = sw_passage_pot2_x + movement_diff;
        fence2_trigger1:SetPosition(sw_passage_pot2_x, 38.0);

        if (sw_passage_pot1_x <= 13.0) then
            -- Hide spikes
            trap_spikes:SetVisible(false);
            trap_spikes:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
            return true;
        end
        return false;
    end,

    passage_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        kalya:SetVisible(true);
        orlinn:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        orlinn:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        kalya_move_next_to_hero_event:SetDestination(hero:GetXPosition() - 2.0, hero:GetYPosition(), false);
        orlinn_move_next_to_hero_event:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition(), false);

        -- Near, but not upon
        orlinn_move_near_hero_event:SetDestination(hero:GetXPosition() + 1.0, hero:GetYPosition() + 0.2, false);
    end,

    thrown_to_passage_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        kalya:SetVisible(true);
        orlinn:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        orlinn:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        kalya_move_next_to_hero_event2:SetDestination(hero:GetXPosition() - 2.0, hero:GetYPosition(), false);
        orlinn_move_next_to_hero_event2:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition(), false);

        -- Near, but not upon
        orlinn_move_near_hero_event:SetDestination(hero:GetXPosition() + 1.0, hero:GetYPosition() + 0.2, false);
    end,

    jump_to_passage_start = function()
        jump_event_time = 0;

        jump_canceled = false;
        time_before_climbing = 0;
        orlinn_in_place_to_climb = false;
        orlinn_y_position = orlinn:GetYPosition();

        -- Bronann's movement
        kneeling_done = false;
        stop_kneeling_done = false;
        hero_looks_north_done = false;

        orlinn:SetDirection(vt_map.MapMode.WEST)
        kalya:SetDirection(vt_map.MapMode.EAST)
    end,

    jump_to_passage_update = function()
        local update_time = SystemManager:GetUpdateTime();
        jump_event_time = jump_event_time + update_time;

        -- Bronann's movement
        if (kneeling_done == false and jump_event_time >= 100) then
            -- Bronann kneels so Orlinn can grab Orlinn's feet.
            hero:SetCustomAnimation("kneeling", 0); -- 0 means forever
            kneeling_done = true;
        elseif (stop_kneeling_done == false and jump_event_time >= 600) then
            hero:DisableCustomAnimation();
            hero:SetDirection(vt_map.MapMode.EAST);
            stop_kneeling_done = true;
        elseif (stop_kneeling_done == true and hero_looks_north_done == false and jump_event_time >= 900) then
            hero:SetDirection(vt_map.MapMode.NORTH);
            orlinn:SetDirection(vt_map.MapMode.NORTH);
            hero_looks_north_done = true;
        end

        -- Orlinn's movement.
        if (orlinn_in_place_to_climb == false and jump_event_time >= 600) then
            -- Place Orlinn above Bronann
            local x_position = orlinn:GetXPosition();
            if (x_position > hero:GetXPosition()) then
                x_position = x_position - 0.007 * update_time;
                orlinn:SetXPosition(x_position);
            end
            if (orlinn_y_position > 14.5) then
                orlinn_y_position = orlinn_y_position - 0.015 * update_time;
                orlinn:SetYPosition(orlinn_y_position);
            end

            -- Next step conditions
            if (orlinn:GetXPosition() <= hero:GetXPosition() and orlinn_y_position <= 14.5) then
                time_before_climbing = time_before_climbing + update_time;
                if (time_before_climbing > 1000) then
                    orlinn:SetDirection(vt_map.MapMode.NORTH);
                    orlinn:SetMoving(true);
                    orlinn_in_place_to_climb = true;
                end
            end
        elseif (orlinn_in_place_to_climb == true) then
            -- And then, make him climb up the wall.
            -- Climb
            if (orlinn_y_position > 12.0) then
                orlinn_y_position = orlinn_y_position - 0.005 * update_time;
                orlinn:SetYPosition(orlinn_y_position);
            -- Walk
            elseif (orlinn_y_position > 11.0) then
                orlinn_y_position = orlinn_y_position - 0.010 * update_time;
                orlinn:SetYPosition(orlinn_y_position);
            end
            -- event end
            if (orlinn:GetYPosition() <= 11.0) then
                orlinn:SetDirection(vt_map.MapMode.SOUTH);
                orlinn:SetMoving(false);
                return true;
            end
        end

        return false;
    end,

    set_remove_orlinn_at_end = function()
        jump_canceled = true;
    end,

    set_camera_on_orlinn = function()
        Map:SetCamera(orlinn, 800);
    end,

    set_camera_on_orlinn_update = function()
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        return true;
    end,

    thrown_to_passage_event_end = function()

        -- Set event as done
        GlobalManager:SetEventValue("story", "mt_elbrus_shrine_high_passage_event_done", 1);

        -- If the user chose 'No', Orlinn can now be removed the event end
        if (jump_canceled == true) then
            Map:PopState();
            kalya:SetPosition(0, 0);
            kalya:SetVisible(false);
            kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

            orlinn:SetPosition(0, 0);
            orlinn:SetVisible(false);
            orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

            -- Reload the hero back to default
            hero:ReloadSprite(main_sprite_name);

        else -- The user chose 'Yes', so the camera is now Orlinn.
            Map:PopState();
            orlinn:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
            -- Enables the event to permit coming back
            passage_back_event_object:SetEventWhenTalking("Orlinn comes back event start");

            -- Removes the 'hero' sprite, and add the 'bronann' sprite instead for the coming back event.
            bronann:SetDirection(hero:GetDirection());
            bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition());
            bronann:SetVisible(true);
            hero:SetPosition(0, 0);
            hero:SetVisible(false);

            -- Disable the menu mode & status effects.
            Map:SetMenuEnabled(false);
            Map:SetStatusEffectsEnabled(false);
        end

        _UpdatePassageEvent();
    end,

    come_back_from_passage_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        orlinn:SetMoving(false);
        orlinn:SetDirection(vt_map.MapMode.SOUTH);
        bronann:SetDirection(vt_map.MapMode.NORTH);
        kalya:SetDirection(vt_map.MapMode.NORTH);

        -- Makes Orlinn go above Bronann
        orlinn_goes_above_bronann_event:SetDestination(bronann:GetXPosition(), orlinn:GetYPosition(), false);
    end,

    not_coming_back_event_end = function()
        Map:PopState();
    end,

    jumping_back_animation_start = function()
        orlinn_y_position = orlinn:GetYPosition();
        kalya:SetDirection(vt_map.MapMode.EAST)
        AudioManager:PlaySound("snd/falling.ogg");
    end,

    jumping_back_animation_update = function()
        local update_time = SystemManager:GetUpdateTime();
        orlinn_y_position = orlinn_y_position + 0.015 * update_time;
        orlinn:SetYPosition(orlinn_y_position);

        if (orlinn_y_position >= bronann:GetYPosition()) then
            return true;
        end
        return false;
    end,

    orlinn_is_surprised = function()
        kalya:SetCustomAnimation("kneeling", 0); -- 0 means forever
        orlinn:Emote("exclamation", vt_map.MapMode.SOUTH);
    end,

    bronann_on_ground_animation = function()
        AudioManager:PlaySound("snd/bump.wav");
        bronann:SetCustomAnimation("sleeping", 0); -- 0 means forever
    end,

    bronann_kneels = function()
        kalya:SetDirection(vt_map.MapMode.EAST)
        kalya:DisableCustomAnimation();
        bronann:SetCustomAnimation("kneeling", 0); -- 0 means forever
    end,

    bronann_gets_up = function()
        bronann:DisableCustomAnimation();
        bronann:SetDirection(vt_map.MapMode.SOUTH);
    end,

    set_camera_on_hero = function()
        -- Sets the hero back to Bronann's place
        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        Map:SetCamera(hero, 400);
    end,

    set_camera_on_hero_update = function()
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        return true;
    end,

    come_back_from_passage_event_end = function()
        -- Put back the hero sprite in place
        hero:SetDirection(bronann:GetDirection());
        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        hero:SetVisible(true);
        bronann:SetPosition(0, 0);
        bronann:SetVisible(false);

        -- Make orlinn and kalya go away
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);

        -- Re-enable the menu mode.
        Map:SetMenuEnabled(true);
        Map:SetStatusEffectsEnabled(true);

        -- Reload the hero back to default
        if (main_sprite_name ~= "") then
            hero:ReloadSprite(main_sprite_name);
        end

        Map:PopState();

        GlobalManager:SetEventValue("story", "mt_elbrus_fell_on_bronanns_head", 1)

        -- Disable the come back object event
        passage_back_event_object:ClearEventWhenTalking();
    end,

    check_diagonal_stone2 = function()
        return _CheckForDiagonals(rolling_stone2);
    end,

    start_to_move_the_stone2 = function()
        stone_direction2 = _GetStoneDirection(rolling_stone2);
        AudioManager:PlaySound("snd/stone_roll.wav");
    end,

    move_the_stone_update2 = function()
        return _UpdateStoneMovement(rolling_stone2, stone_direction2)
    end,
}
