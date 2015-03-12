-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_south_entrance_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "data/story/common/locations/mountain_village.png"
map_subname = "Village entrance"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

local bronann = nil
local orlinn = nil

-- the main map loading code
function Load(m)

    Map = m;
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
    Map:GetEffectSupervisor():EnableAmbientOverlay("data/visuals/ambient/clouds.png", 5.0, -5.0, true);

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
    bronann = CreateSprite(Map, "Bronann", 32, 4, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.SOUTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- set up the position according to the previous map
    if (GlobalManager:GetPreviousLocation() == "from_riverbank") then
        bronann:SetPosition(3, 34);
        bronann:SetDirection(vt_map.MapMode.EAST);
    elseif (GlobalManager:GetPreviousLocation() == "from right house") then
        bronann:SetPosition(48, 34);
        bronann:SetDirection(vt_map.MapMode.SOUTH);
        AudioManager:PlaySound("data/sounds/door_close.wav");
    elseif (GlobalManager:GetPreviousLocation() == "from left house") then
        bronann:SetPosition(20, 34);
        bronann:SetDirection(vt_map.MapMode.SOUTH);
        AudioManager:PlaySound("data/sounds/door_close.wav");
    end
end

local chicken2 = nil

function _CreateNPCs()
    local npc = nil
    local text = nil
    local dialogue = nil
    local event = nil

    npc = CreateSprite(Map, "Herth", 45, 39, vt_map.MapMode.GROUND_OBJECT);
    if (GlobalManager:DoesEventExist("story", "Quest2_forest_event_done") == true) then
        -- At that moment, Herth isn't there anymore.
        npc:SetVisible(false);
        npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    else
        npc:SetDirection(vt_map.MapMode.SOUTH);
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Hi Bronann.");
        dialogue:AddLine(text, npc);
        text = vt_system.Translate("Hi Herth. I see you've blocked the gate, why so?");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Don't worry too much. I'm just preventing strangers from being able to sneak in at night.");
        dialogue:AddLine(text, npc);
        text = vt_system.Translate("There have been some reports of theft in the villages nearby recently.");
        dialogue:AddLine(text, npc);
        text = vt_system.Translate("Wow, do you think that they would come here?");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("It's a possibility. But don't worry too much, ok?");
        dialogue:AddLine(text, npc);
        npc:AddDialogueReference(dialogue);
        -- The second time, just repeat the sentence
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_herth_south_entrance_default");
        text = vt_system.Translate("It's a possibility. But don't worry too much, ok?");
        dialogue:AddLine(text, npc);
        npc:AddDialogueReference(dialogue);
    end

    orlinn = CreateSprite(Map, "Orlinn", 29, 22, vt_map.MapMode.GROUND_OBJECT);
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);

    _UpdateOrlinnState();

    -- Adds a chicken that can be taken by Bronann and given back to Grandma.
    if (GlobalManager:GetEventValue("game", "layna_village_chicken2_found") == 0) then
        chicken2 = CreateSprite(Map, "Chicken", 58, 44, vt_map.MapMode.GROUND_OBJECT);

        event = vt_map.RandomMoveSpriteEvent.Create("Chicken2 random move", chicken2, 1000, 1000);
        event:AddEventLinkAtEnd("Chicken2 random move", 4500); -- Loop on itself

        EventManager:StartEvent("Chicken2 random move");

        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("One of grandma's chickens. I should bring it back.");
        dialogue:AddLine(text, bronann);
        dialogue:SetEventAtDialogueEnd("Make bronann take the chicken 2");
        chicken2:AddDialogueReference(dialogue);

        vt_map.ScriptedEvent.Create("Make bronann take the chicken 2", "bronann_takes_chicken2", "fadeoutin_update");
    end
end

function _CreateObjects()
    CreateObject(Map, "Tree Big1", 42, 10, vt_map.MapMode.GROUND_OBJECT);

    -- Small door lights
    vt_map.Light.Create("data/visuals/lights/sun_flare_light_small_main.lua",
                        "data/visuals/lights/sun_flare_light_small_secondary.lua",
                        20.0, 29.5,
                        vt_video.Color(1.0, 1.0, 1.0, 0.6),
                        vt_video.Color(1.0, 1.0, 0.85, 0.3));
    vt_map.Light.Create("data/visuals/lights/sun_flare_light_small_main.lua",
                        "data/visuals/lights/sun_flare_light_small_secondary.lua",
                        48.0, 29.5,
                        vt_video.Color(1.0, 1.0, 1.0, 0.6),
                        vt_video.Color(1.0, 1.0, 0.85, 0.3));
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil

    -- Triggered Events
    vt_map.MapTransitionEvent.Create("to Village center", "data/story/layna_village/layna_village_center_map.lua",
                                     "data/story/layna_village/layna_village_center_script.lua", "from_village_south");

    vt_map.MapTransitionEvent.Create("to Village riverbank", "data/story/layna_village/layna_village_riverbank_map.lua",
                                     "data/story/layna_village/layna_village_riverbank_script.lua", "from_village_south");

    vt_map.MapTransitionEvent.Create("to left house", "data/story/layna_village/layna_village_south_entrance_left_house_map.lua",
                                     "data/story/layna_village/layna_village_south_entrance_left_house_script.lua", "from_village_south");

    vt_map.MapTransitionEvent.Create("to right house", "data/story/layna_village/layna_village_south_entrance_right_house_map.lua",
                                     "data/story/layna_village/layna_village_south_entrance_right_house_script.lua", "from_village_south");

    -- Orlinn events
    event = vt_map.ScriptedEvent.Create("Quest1: Start Orlinn Hide n Seek2", "Quest1_Orlinn_Start_Hide_N_Seek2", "");
    event:AddEventLinkAtEnd("Quest1: Make Orlinn run");

    event = vt_map.PathMoveSpriteEvent.Create("Quest1: Make Orlinn run", orlinn, 30, 2, true);
    event:AddEventLinkAtEnd("Quest1: Make Orlinn disappear");

    vt_map.ScriptedSpriteEvent.Create("Quest1: Make Orlinn disappear", orlinn, "MakeInvisible", "");
end

-- zones
local village_center_zone = nil
local to_village_riverbank_zone = nil
local to_left_house_zone = nil
local to_right_house_zone = nil

function _CreateZones()
    -- N.B.: left, right, top, bottom
    village_center_zone = vt_map.CameraZone.Create(8, 62, 0, 2);
    to_village_riverbank_zone = vt_map.CameraZone.Create(0, 1, 26, 43);
    to_left_house_zone = vt_map.CameraZone.Create(18, 22, 32, 33);
    to_right_house_zone = vt_map.CameraZone.Create(46, 50, 32, 33);
end

function _CheckZones()
    if (village_center_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        EventManager:StartEvent("to Village center");
    end

    if (to_village_riverbank_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        EventManager:StartEvent("to Village riverbank");
    end

    if (to_left_house_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        AudioManager:PlaySound("data/sounds/door_open2.wav");
        EventManager:StartEvent("to left house");
    end

    if (to_right_house_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        AudioManager:PlaySound("data/sounds/door_open2.wav");
        EventManager:StartEvent("to right house");
    end
end

-- Custom inner map functions

function _UpdateOrlinnState()
    local text = nil
    local dialogue = nil
    local event = nil

    event = vt_map.PathMoveSpriteEvent.Create("Hide n Seek1: Orlinn goes right", orlinn, 31, 22, false);
    event:AddEventLinkAtEnd("Hide n Seek1: Orlinn looks south");

    event = vt_map.ChangeDirectionSpriteEvent.Create("Hide n Seek1: Orlinn looks south", orlinn, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Hide n Seek1: Orlinn goes left", 800);

    event = vt_map.PathMoveSpriteEvent.Create("Hide n Seek1: Orlinn goes left", orlinn, 29, 22, false);
    event:AddEventLinkAtEnd("Hide n Seek1: Orlinn goes right", 8000); -- finish the event loop.

    if (GlobalManager:DoesEventExist("layna_south_entrance", "quest1_orlinn_hide_n_seek1_done") == true) then
        -- Orlinn shouldn't be here, so we make him invisible
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetVisible(false);
        return;
    elseif (GlobalManager:DoesEventExist("layna_center", "quest1_orlinn_dialogue1_done") == true) then
        -- Start the hide and seek 1 position when it has to happen
        EventManager:StartEvent("Hide n Seek1: Orlinn goes right", 8000);

        -- Set up the dialogue.
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Yiek! Hey, you scared me.");
        dialogue:AddLineEmote(text, orlinn, "exclamation");
        text = vt_system.Translate("But you'll never find me hiding on top of the cliffs!");
        dialogue:AddLineEvent(text, orlinn, "", "Quest1: Start Orlinn Hide n Seek2");
        orlinn:AddDialogueReference(dialogue);
        return;
    end

    -- Orlinn default behaviour
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);
end

-- Effect time used when fade out and in
local fade_effect_time = 0;
local fade_color = vt_video.Color(0.0, 0.0, 0.0, 1.0);
local chicken2_taken = false;

-- Map Custom functions
map_functions = {

    Quest1_Orlinn_Start_Hide_N_Seek2 = function()
        orlinn:SetMoving(false); -- in case he's moving
        orlinn:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);
        orlinn:ClearDialogueReferences();
        EventManager:EndAllEvents(orlinn);

        -- Updates Orlinn's state
        GlobalManager:SetEventValue("layna_south_entrance", "quest1_orlinn_hide_n_seek1_done", 1);
    end,

    MakeInvisible = function(sprite)
        if (sprite ~= nil) then
            sprite:SetVisible(false);
            sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end,

    bronann_takes_chicken2 = function()
        chicken2_taken = false;
        fade_effect_time = 0;
        chicken2:SetMoving(false);
        EventManager:EndAllEvents(chicken2);
        bronann:SetMoving(false);
        Map:PushState(vt_map.MapMode.STATE_SCENE);
    end,

    fadeoutin_update = function()
        fade_effect_time = fade_effect_time + SystemManager:GetUpdateTime();

        if (fade_effect_time < 300.0) then
            fade_color:SetAlpha(fade_effect_time / 300.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(fade_color);
            return false;
        elseif (fade_effect_time >= 300.0 and fade_effect_time < 1300.0) then
            -- do nothing
            return false;
        elseif (fade_effect_time < 2000.0) then
            -- actually remove the chicken
            if (chicken2_taken == false) then
                chicken2:SetVisible(false);
                chicken2:SetPosition(0, 0);
                GlobalManager:SetEventValue("game", "layna_village_chicken2_found", 1)
                -- Set the quest start dialogue as done if not already, so a possible later
                -- dialogue with grandma sounds more logical
                GlobalManager:SetEventValue("game", "layna_village_chicken_dialogue_done", 1);
                chicken2_taken = true;
            end
            fade_color:SetAlpha((2000.0 - fade_effect_time) / 700.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(fade_color);
            return false;
        end
        -- Free Bronann's movement
        Map:PopState();
        return true;
    end,
}
