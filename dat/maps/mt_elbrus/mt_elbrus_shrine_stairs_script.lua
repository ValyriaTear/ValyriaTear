-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine_stairs_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus Shrine"
map_image_filename = "img/menus/locations/mountain_shrine.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/icy_wind.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Script = nil

-- the main character handler
local hero = nil

-- Forest dialogue secondary hero
local kalya = nil
local orlinn = nil
local bronann = nil -- A copy of Bronann, used to simplify some scripting.

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

    -- If the event in progress involves being Orlinn, then let's incarnate him.
    if (GlobalManager:GetEventValue("story", "elbrus_shrine_laughing_event_done") == 1
            and GlobalManager:GetEventValue("story", "mt elbrus shrine heroes saved") == 0) then
        orlinn:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        orlinn:SetVisible(true);
        orlinn:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        orlinn:SetDirection(hero:GetDirection());
        Map:SetCamera(orlinn)
        -- hide the hero sprite
        hero:SetVisible(false)
        hero:SetPosition(0, 0);

        -- The menu and status effects are then disabled.
        Map:SetMenuEnabled(false);
        Map:SetStatusEffectsEnabled(false);

        -- Place kalya and bronann laughing
        bronann:SetPosition(37, 11);
        bronann:SetVisible(true);
        bronann:SetDirection(vt_map.MapMode.SOUTH);
        bronann:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);

        kalya:SetPosition(35, 11);
        kalya:SetVisible(true);
        kalya:SetDirection(vt_map.MapMode.SOUTH);
        kalya:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);

        -- If Orlinn hasn't triggered the button, yet, the other heroes are laughing.
        if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 0) then
            bronann:SetCustomAnimation("laughing", 0); -- 0 means forever
            kalya:SetCustomAnimation("laughing", 0); -- 0 means forever
        end

        _UpdateKalyaBronannDialogue();

        -- Loads the funny music if needed.
        if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 1) then
            AudioManager:LoadMusic("mus/Zander Noriega - School of Quirks.ogg", Map);
        end
    end

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay when necessary
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);
end

-- set up/updates sophia's events
function _UpdateKalyaBronannDialogue()
    local text = nil
    local dialogue = nil
    local event = nil

    kalya:ClearDialogueReferences();
    bronann:ClearDialogueReferences();

    -- If Orlinn has triggered the waterfall button, the heroes are stainding not laughing
    if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 1) then
        kalya:LookAt(bronann);
        bronann:LookAt(kalya);
        return;
    end

    if (GlobalManager:GetEventValue("story", "elbrus_shrine_laughing_event_done") == 0) then
        return
    end

    -- Short version of the dialogue for later
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("We can't move, Orlinn.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("We're gonna...suffocate.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Orlinn, help us... Please, oh oh oh...");
    dialogue:AddLine(text, kalya);
    bronann:AddDialogueReference(dialogue);
    kalya:AddDialogueReference(dialogue);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from_shrine_first_floor)
    hero = CreateSprite(Map, "Bronann", 13.5, 38.0, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.EAST);
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
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_2nd_floor") then
        hero:SetDirection(vt_map.MapMode.WEST);
        hero:SetPosition(57.0, 38.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_2nd_floor_grotto") then
        hero:SetDirection(vt_map.MapMode.EAST);
        hero:SetPosition(13.5, 16.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_third_floor") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(37, 8.5);
    end

    -- Create secondary characters
    kalya = CreateSprite(Map, "Kalya",
                         hero:GetXPosition(), hero:GetYPosition(), vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.EAST);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);

    orlinn = CreateSprite(Map, "Orlinn",
                          hero:GetXPosition(), hero:GetYPosition(), vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.FAST_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);

    bronann = CreateSprite(Map, "Bronann",
                           hero:GetXPosition(), hero:GetYPosition(), vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.EAST);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    bronann:SetVisible(false);
end

-- The heal particle effect map object
local heal_effect = nil
local layna_statue = nil;

function _CreateObjects()
    local object = nil
    local npc = nil
    local dialogue = nil
    local text = nil
    local event = nil

    vt_map.SavePoint.Create(51, 22);
    if (GlobalManager:GetEventValue("story", "elbrus_shrine_laughing_event_done") == 1
            and GlobalManager:GetEventValue("story", "mt elbrus shrine heroes saved") == 0) then
        -- Disable the save point
        Map:SetSavePointsEnabled(false);
    end

    -- We can hear waterfalls in that case
    if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 1) then
        vt_map.SoundObject.Create("snd/fountain_large.ogg", 0, 0, 50.0);
    end

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject.Create("dat/effects/particles/heal_particle.lua", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    heal_effect:Stop(); -- Don't run it until the character heals itself

    layna_statue = CreateObject(Map, "Layna Statue", 57, 22, vt_map.MapMode.GROUND_OBJECT);
    if (GlobalManager:GetEventValue("story", "elbrus_shrine_laughing_event_done") == 0
            or GlobalManager:GetEventValue("story", "mt elbrus shrine heroes saved") == 1) then
        layna_statue:SetEventWhenTalking("Heal dialogue");
    end

    object = CreateObject(Map, "Layna Statue", 37, 30, vt_map.MapMode.GROUND_OBJECT);
    object:SetEventWhenTalking("Heal dialogue");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Your party feels better.");
    dialogue:AddLineEvent(text, nil, "Heal event", ""); -- 'nil' means no portrait and no name
    event = vt_map.DialogueEvent.Create("Heal dialogue", dialogue);


    -- Snow effect
    vt_map.ParticleObject.Create("dat/maps/mt_elbrus/particles_snow_south_entrance.lua", 29, 48, vt_map.MapMode.GROUND_OBJECT);

    vt_map.Halo.Create("img/misc/lights/torch_light_mask.lua", 29, 55,
        vt_video.Color(1.0, 1.0, 1.0, 0.8));

    -- Adds a hidden sign, show just before the opening of the door
    CreateObject(Map, "Ancient_Sign1", 37, 36, vt_map.MapMode.FLATGROUND_OBJECT);

    _add_flame(29.5, 31);
    _add_flame(45.5, 31);
    _add_flame(21.5, 11);
    _add_flame(53.5, 11);

    -- Waterfall
    if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 1) then
        _add_small_waterfall(18, 30);
        _add_waterlight(13, 31)
        _add_waterlight(23, 31)
    else
        _add_bubble(23, 30);
        _add_bubble(19, 29);
        _add_bubble(6, 29.5);
        _add_bubble(4, 39);

        -- The poisonous scent
        Map:GetScriptSupervisor():AddScript("dat/maps/mt_elbrus/mt_elbrus_scent_anim.lua");
    end
end

function _add_small_waterfall(x, y)
    local object = CreateObject(Map, "Waterfall2", x - 0.1, y - 0.2, vt_map.MapMode.GROUND_OBJECT);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    object:RandomizeCurrentAnimationFrame();

    -- Ambient sound
    object = vt_map.SoundObject.CreatObject("snd/fountain_large.ogg", x, y - 5, 50.0);
    object:SetMaxVolume(0.6);

    -- Particle effects
    object = vt_map.ParticleObject.Create("dat/effects/particles/waterfall_steam.lua", x, y - 8.0, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true);

    object = vt_map.ParticleObject.Create("dat/effects/particles/waterfall_steam_big.lua", x, y + 1.0, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true);
end

function _add_waterlight(x, y)
    local object = CreateObject(Map, "Water Light1", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
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

function _add_bubble(x, y)
    local object = CreateObject(Map, "Bubble", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();

    vt_map.ParticleObject.Create("dat/effects/particles/bubble_steam.lua", x, y, vt_map.MapMode.GROUND_OBJECT);
end

-- Special event references which destinations must be updated just before being called.
-- shrine entrance event
local kalya_move_next_to_bronann_event1 = nil
local kalya_move_back_to_bronann_event1 = nil
local orlinn_move_next_to_bronann_event1 = nil
local orlinn_move_back_to_bronann_event1 = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    event = vt_map.MapTransitionEvent.Create("to mountain shrine 1st floor", "dat/maps/mt_elbrus/mt_elbrus_shrine5_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine5_script.lua", "from_shrine_2nd_floor");


    event = vt_map.MapTransitionEvent.Create("to mountain shrine 2nd floor", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_ne_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_ne_script.lua", "from_shrine_1st_floor");


    event = vt_map.MapTransitionEvent.Create("to mountain shrine 2nd floor grotto", "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s2_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine_2nd_s2_script.lua", "from_shrine_stairs");


    event = vt_map.MapTransitionEvent.Create("to mountain shrine 3rd floor", "dat/maps/mt_elbrus/mt_elbrus_shrine_3rd_map.lua",
                                       "dat/maps/mt_elbrus/mt_elbrus_shrine_3rd_script.lua", "from_shrine_stairs");


    -- Heal point
    event = vt_map.ScriptedEvent.Create("Heal event", "heal_party", "heal_done");


    -- Generic events
    event = vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);

    event = vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks west", orlinn, vt_map.MapMode.WEST);

    event = vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks south", orlinn, vt_map.MapMode.SOUTH);

    event = vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks north", bronann, vt_map.MapMode.NORTH);

    event = vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);

    event = vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks north", kalya, vt_map.MapMode.NORTH);

    event = vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks west", kalya, vt_map.MapMode.WEST);

    event = vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks south", kalya, vt_map.MapMode.SOUTH);

    event = vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);

    event = vt_map.LookAtSpriteEvent.Create("Kalya looks at Orlinn", kalya, orlinn);

    event = vt_map.LookAtSpriteEvent.Create("Bronann looks at Kalya", bronann, kalya);

    event = vt_map.LookAtSpriteEvent.Create("Bronann looks at Orlinn", bronann, orlinn);

    event = vt_map.LookAtSpriteEvent.Create("Orlinn looks at Kalya", orlinn, kalya);

    event = vt_map.LookAtSpriteEvent.Create("Orlinn looks at Bronann", orlinn, bronann);


    -- Kalya and Bronann are falling because of the smoke event
    event = vt_map.PathMoveSpriteEvent.Create("The hero goes in front of the door", hero, 37, 11, false);
    event:AddEventLinkAtEnd("Kalya and Bronann laughs event start");


    event = vt_map.ScriptedEvent.Create("Kalya and Bronann laughs event start", "laughing_event_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann");
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann");


    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_bronann_event1 = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann", kalya, 0, 0, false);
    kalya_move_next_to_bronann_event1:AddEventLinkAtEnd("Kalya looks north");
    EventManager:RegisterEvent(kalya_move_next_to_bronann_event1);
    orlinn_move_next_to_bronann_event1 = vt_map.PathMoveSpriteEvent.Create("Orlinn moves next to Bronann", orlinn, 0, 0, false);
    orlinn_move_next_to_bronann_event1:AddEventLinkAtEnd("Orlinn looks north");
    orlinn_move_next_to_bronann_event1:AddEventLinkAtEnd("Choice to enter the big door", 500);
    EventManager:RegisterEvent(orlinn_move_next_to_bronann_event1);

    -- choice to enter
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("What a large gate. What is behind it must be fearsome.");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    text = vt_system.Translate("Shall we go in?");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Yes, let's end this.");
    dialogue:AddOptionEvent(text, 2, "The heroes decide to go event");
    text = vt_system.Translate("No, let's prepare first.");
    dialogue:AddOptionEvent(text, "Kalya goes back to party2");
    event = vt_map.DialogueEvent.Create("Choice to enter the big door", dialogue);


    -- Chose not to enter event
    event = vt_map.PathMoveSpriteEvent.Create("Kalya goes back to party2", kalya, bronann, false);
    event:AddEventLinkAtStart("Orlinn goes back to party2");
    event:AddEventLinkAtEnd("Hide Kalya and Orlinn");
    event:AddEventLinkAtEnd("Bronann goes away from the door");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn goes back to party2", orlinn, bronann, false);


    event = vt_map.ScriptedEvent.Create("Hide Kalya and Orlinn", "hide_kalya_and_orlinn", "");


    event = vt_map.PathMoveSpriteEvent.Create("Bronann goes away from the door", bronann, 37, 14, false);
    event:AddEventLinkAtEnd("The heroes won't enter event");


    event = vt_map.ScriptedEvent.Create("The heroes won't enter event", "heroes_wont_enter", "");


    -- Chose to enter
    event = vt_map.ScriptedEvent.Create("Kalya laughs", "kalya_laughs", "");

    event = vt_map.ScriptedEvent.Create("Bronann laughs", "bronann_laughs", "");


    -- A link event use to reset the dialogue state properly
    event = vt_map.ScriptedEvent.Create("The heroes decide to go event", "empty_event", "");
    event:AddEventLinkAtEnd("The heroes discuss about the big door");


    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Don't worry, we've managed to come this far. There is nothing that could...");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "Orlinn looks at Kalya");
    text = vt_system.Translate("What is it, Kalya?");
    dialogue:AddLineEventEmote(text, bronann, "", "Bronann looks at Kalya", "interrogation");
    text = vt_system.Translate("...Kalya?!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("Sis'!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    text = vt_system.Translate("I feel... I...");
    dialogue:AddLineEvent(text, kalya, "", "Kalya laughs");
    text = vt_system.Translate("Hi hi... Hi hi hi... I can't stop!!");
    dialogue:AddLineEvent(text, kalya, "Kalya laughs", "");
    text = vt_system.Translate("What's so funny, Kalya? Hi hi.");
    dialogue:AddLineEmote(text, bronann, "interrogation");
    text = vt_system.Translate("I don't know... Ah, I can barely breathe!");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Hi hi hi... What's happening to me...");
    dialogue:AddLineEvent(text, bronann, "", "Bronann laughs");
    text = vt_system.Translate("It's the scent, ah ah ah.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Erm, are you crazy or what? I usually am the one with bad jokes you know.");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    text = vt_system.Translate("We can't move Orlinn.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("We're gonna...suffocate.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Orlinn, help us... Please, oh oh oh...");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("The heroes discuss about the big door", dialogue);
    event:AddEventLinkAtEnd("Set camera on Orlinn");


    event = vt_map.ScriptedEvent.Create("Set camera on Orlinn", "camera_on_orlinn_start", "camera_update");
    event:AddEventLinkAtEnd("Laughing event end");


    event = vt_map.ScriptedEvent.Create("Laughing event end", "laughing_event_end", "");


    -- Orlinn can't go away
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I can't let them die. I have to find out something.");
    dialogue:AddLineEmote(text, orlinn, "thinking dots");
    event = vt_map.DialogueEvent.Create("Orlinn can't go away dialogue", dialogue);


    -- Heroes can't go to the boss throne
    event = vt_map.ScriptedEvent.Create("Heroes can't go there...", "cant_go_in_start", "cant_go_in_update");
    event:AddEventLinkAtEnd("The heroes can't go in dialogue");


    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Something is preventing me from entering.");
    dialogue:AddLineEmote(text, hero, "thinking dots");
    event = vt_map.DialogueEvent.Create("The heroes can't go in dialogue", dialogue);
    event:AddEventLinkAtEnd("Heroes can't go there - end");


    event = vt_map.ScriptedEvent.Create("Heroes can't go there - end", "cant_go_in_end", "");


    -- Heroes are saved event
    event = vt_map.ScriptedEvent.Create("Orlinn laughs", "orlinn_laughs", "");

    event = vt_map.ScriptedEvent.Create("Orlinn stops laughing", "orlinn_stops_laughing", "");


    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("I feel much better now.");
    dialogue:AddLineEmote(text, bronann, "thinking dots");
    text = vt_system.Translate("Indeed. It seems the scent has no effect on us anymore.");
    dialogue:AddLineEmote(text, kalya, "thinking dots");
    text = vt_system.Translate("Orlinn, did you do something?");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at Orlinn", "Bronann looks at Orlinn", "exclamation");
    text = vt_system.Translate("Me? You know, a bit of this, a bit of that.");
    dialogue:AddLineEvent(text, orlinn, "Orlinn laughs", "");
    text = vt_system.Translate("I didn't think I would say this, but thank you Orlinn.");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("Thank you, indeed.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("You're welcome!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Well, please stop laughing now. I'm not at ease with what just happened.");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at Orlinn", "", "sweat drop");
    text = vt_system.Translate("...");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("Stop laughing now or I'll smash your little head against that wall!");
    dialogue:AddLineEmote(text, kalya, "popping veins");
    text = vt_system.Translate("Aww, ok.");
    dialogue:AddLineEventEmote(text, orlinn, "Orlinn stops laughing", "", "sweat drop");
    event = vt_map.DialogueEvent.Create("Orlinn saved the heroes", dialogue);
    event:AddEventLinkAtEnd("Kalya goes back to party");
    event:AddEventLinkAtEnd("Orlinn goes back to party");


    event = vt_map.PathMoveSpriteEvent.Create("Kalya goes back to party", kalya, bronann, false);

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn goes back to party", orlinn, bronann, false);
    event:AddEventLinkAtEnd("Orlinn saved the heroes event end");


    event = vt_map.ScriptedEvent.Create("Orlinn saved the heroes event end", "saved_the_heroes_end", "");

end

-- zones
local to_shrine_1st_floor_zone = nil
local to_shrine_2nd_floor_zone = nil
local to_shrine_2nd_floor_grotto_zone = nil
local to_shrine_3rd_floor_zone = nil
local before_3rd_floor_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_1st_floor_zone = vt_map.CameraZone.Create(9, 12, 36, 39);
    to_shrine_2nd_floor_zone = vt_map.CameraZone.Create(59, 61, 36, 39);
    to_shrine_2nd_floor_grotto_zone = vt_map.CameraZone.Create(9, 12, 14, 17);
    to_shrine_3rd_floor_zone = vt_map.CameraZone.Create(34, 40, 5, 7);
    before_3rd_floor_zone = vt_map.CameraZone.Create(28, 46, 7, 10);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_1st_floor_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.WEST);
        EventManager:StartEvent("to mountain shrine 1st floor");
    elseif (to_shrine_2nd_floor_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.NE_EAST);
        EventManager:StartEvent("to mountain shrine 2nd floor");
    elseif (to_shrine_2nd_floor_grotto_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "elbrus_shrine_laughing_event_done") == 0
                or GlobalManager:GetEventValue("story", "mt elbrus shrine heroes saved") == 1) then
            hero:SetDirection(vt_map.MapMode.WEST);
            EventManager:StartEvent("to mountain shrine 2nd floor grotto");
        else
            -- Orlinn can't go away...
            orlinn:SetMoving(false);
            EventManager:StartEvent("Orlinn can't go away dialogue");
        end
    elseif (to_shrine_3rd_floor_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "mt elbrus shrine heroes saved") == 0) then
            hero:SetDirection(vt_map.MapMode.NORTH);
            EventManager:StartEvent("to mountain shrine 3rd floor");
        else
            EventManager:StartEvent("Heroes can't go there...");
        end
    elseif (before_3rd_floor_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        if (GlobalManager:GetEventValue("story", "mt elbrus shrine heroes saved") == 1) then
            -- Do nothing in that case

        elseif (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 1) then
            -- Start the scene where orlinn has just saved our heroes
            Map:PushState(vt_map.MapMode.STATE_SCENE);
            orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
            EventManager:StartEvent("Orlinn saved the heroes");

        elseif (GlobalManager:GetEventValue("story", "elbrus_shrine_laughing_event_done") == 0) then
            -- Start the laughing scene
            Map:PushState(vt_map.MapMode.STATE_SCENE);
            EventManager:StartEvent("The hero goes in front of the door");
        end
    end
end

-- Effect time used when applying the heal light effect
local heal_effect_time = 0;
local heal_color = vt_video.Color(0.0, 0.0, 1.0, 1.0);

local field_effect_time = 0;
local field_color = vt_video.Color(0.0, 0.0, 0.0, 1.0);

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

    hide_kalya_and_orlinn = function()
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end,

    heroes_wont_enter = function()
        Map:PopState();

        -- Make the player incarnate the default hero again
        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition())
        hero:SetDirection(bronann:GetDirection())
        Map:SetCamera(hero);

        -- Reload the hero back to default
        hero:SetVisible(true);
        bronann:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        bronann:SetPosition(0, 0)
        bronann:SetVisible(false)
    end,

    laughing_event_start = function()
        hero:SetMoving(false);

        kalya:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        kalya:SetVisible(true);
        orlinn:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        orlinn:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        bronann:SetDirection(vt_map.MapMode.NORTH);
        bronann:SetVisible(true);
        hero:SetVisible(false);
        Map:SetCamera(bronann);
        hero:SetPosition(0, 0);

        kalya_move_next_to_bronann_event1:SetDestination(bronann:GetXPosition() - 2.0, bronann:GetYPosition(), false);
        orlinn_move_next_to_bronann_event1:SetDestination(bronann:GetXPosition() + 2.0, bronann:GetYPosition(), false);
    end,

    kalya_laughs = function()
        kalya:SetCustomAnimation("laughing", 0); -- 0 means forever
        AudioManager:FadeOutActiveMusic(1000); -- Remove the music
    end,

    bronann_laughs = function()
        bronann:SetCustomAnimation("laughing", 0); -- 0 means forever
    end,

    orlinn_laughs = function()
        orlinn:SetCustomAnimation("laughing", 0); -- 0 means forever
        -- Adds funny music
        AudioManager:PlayMusic("mus/Zander Noriega - School of Quirks.ogg");
    end,

    orlinn_stops_laughing = function()
        orlinn:DisableCustomAnimation();
    end,

    camera_on_orlinn_start = function()
        Map:SetCamera(orlinn, 500);
    end,

    camera_update = function()
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        return true;
    end,

    laughing_event_end = function()
        -- Set the event as done
        GlobalManager:SetEventValue("story", "elbrus_shrine_laughing_event_done", 1);
        orlinn:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        bronann:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        kalya:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        Map:PopState();
        -- Prevent from healing the team when being Orlinn
        layna_statue:ClearEventWhenTalking();
        -- Disable save point
        Map:SetSavePointsEnabled(false);
        -- Make Bronann and Kalya repeat on need
        _UpdateKalyaBronannDialogue();

        -- The menu and status effects are then disabled.
        Map:SetMenuEnabled(false);
        Map:SetStatusEffectsEnabled(false);
    end,

    cant_go_in_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        field_effect_time = 0;
    end,

    cant_go_in_update = function()
        field_effect_time = field_effect_time + SystemManager:GetUpdateTime();

        if (field_effect_time < 300.0) then
            field_color:SetAlpha(field_effect_time / 300.0 / 2.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(field_color);
            return false;
        end

        if (field_effect_time < 1000.0) then
            field_color:SetAlpha(((1000.0 - field_effect_time) / 700.0) / 2.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(field_color);
            return false;
        end
        return true;
    end,

    cant_go_in_end = function()
        Map:PopState();
    end,

    saved_the_heroes_end = function()
        Map:PopState();

        -- Make the player incarnate the default hero again
        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition())
        Map:SetCamera(hero, 500);

        -- Reload the hero back to default
        hero:SetVisible(true);
        bronann:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);

        bronann:SetPosition(0, 0);
        bronann:SetVisible(false);
        bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Re-enable save point
        Map:SetSavePointsEnabled(true);
        layna_statue:SetEventWhenTalking("Heal dialogue");

        -- The menu and status effects are enabled.
        Map:SetMenuEnabled(true);
        Map:SetStatusEffectsEnabled(true);

        -- Set event as done
        GlobalManager:SetEventValue("story", "mt elbrus shrine heroes saved", 1)

        -- Fade in the default music
        AudioManager:PlayMusic("mus/icy_wind.ogg");
    end,

    empty_event = function()
    end,
}
