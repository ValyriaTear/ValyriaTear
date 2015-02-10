-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_riverbank_house_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "img/menus/locations/mountain_village.png"
map_subname = "Lilly's house"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = nil
local DialogueManager = nil
local EventManager = nil

-- the main character handler
local bronann = nil

-- the main map loading code
function Load(m)

    Map = m;
    DialogueManager = Map:GetDialogueSupervisor();
    EventManager = Map:GetEventSupervisor();

    Map:SetUnlimitedStamina(true);

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on bronann
    Map:SetCamera(bronann);

    _CreateEvents();
    _CreateZones();

    -- The only entrance close door sound
    AudioManager:PlaySound("snd/door_close.wav");
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    bronann = CreateSprite(Map, "Bronann", 32, 45, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.NORTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
end

local chicken1 = nil

function _CreateObjects()
    local object = nil
    local event = nil
    local dialogue = nil
    local text = nil

    -- Adds a chicken that can be taken by Bronann and given back to Grandma.
    if (GlobalManager:GetEventValue("game", "layna_village_chicken1_found") == 0) then
        chicken1 = CreateSprite(Map, "Chicken", 25, 25, vt_map.MapMode.GROUND_OBJECT);

        event = vt_map.RandomMoveSpriteEvent("Chicken1 random move", chicken1, 1000, 1000);
        event:AddEventLinkAtEnd("Chicken1 random move", 4500); -- Loop on itself
        EventManager:RegisterEvent(event);
        EventManager:StartEvent("Chicken1 random move");

        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("One of grandma's chickens. I should bring it back.");
        dialogue:AddLine(text, bronann);
        dialogue:SetEventAtDialogueEnd("Make bronann take the chicken 1");
        chicken1:AddDialogueReference(dialogue);

        event = vt_map.ScriptedEvent("Make bronann take the chicken 1", "bronann_takes_chicken1", "fadeoutin_update");
        EventManager:RegisterEvent(event);
    end

    CreateObject(Map, "Bed1", 42, 27, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Box1", 23, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Chair1_inverted", 39, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Small Wooden Table", 42, 21, vt_map.MapMode.GROUND_OBJECT);

    object = CreateObject(Map, "Candle1", 43, 19, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true); -- Above the table

    object = CreateObject(Map, "Left Window Light", 20, 38, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    CreateObject(Map, "Clock1", 33, 13, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Table1", 39, 42, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Chair1_inverted", 35, 41, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Chair1", 43, 40, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Bench2", 39, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 21, 36, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 22, 37, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 21, 41, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 23, 40, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 25, 39, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Box1", 21, 39, vt_map.MapMode.GROUND_OBJECT);

    CreateObject(Map, "Flower Pot1", 25, 16, vt_map.MapMode.GROUND_OBJECT);

    object = CreateObject(Map, "Right Window Light", 44, 38, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Triggered events
    event = vt_map.MapTransitionEvent("exit floor", "dat/maps/layna_village/layna_village_riverbank_map.lua",
                                       "dat/maps/layna_village/layna_village_riverbank_script.lua", "from_riverbank_house");
    EventManager:RegisterEvent(event);
end

-- zones
local room_exit_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    room_exit_zone = vt_map.CameraZone.Create(30, 34, 47, 48);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (room_exit_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("exit floor");
        AudioManager:PlaySound("snd/door_open2.wav");
    end
end


-- Map Custom functions
-- Used through scripted events

-- Effect time used when fade out and in
local fade_effect_time = 0;
local fade_color = vt_video.Color(0.0, 0.0, 0.0, 1.0);
local chicken1_taken = false;

map_functions = {

    bronann_takes_chicken1 = function()
        chicken1_taken = false;
        fade_effect_time = 0;
        chicken1:SetMoving(false);
        EventManager:EndAllEvents(chicken1);
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        bronann:SetMoving(false);
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
            if (chicken1_taken == false) then
                chicken1:SetVisible(false);
                chicken1:SetPosition(0, 0);
                GlobalManager:SetEventValue("game", "layna_village_chicken1_found", 1)
                -- Set the quest start dialogue as done if not already, so a possible later
                -- dialogue with grandma sounds more logical
                GlobalManager:SetEventValue("game", "layna_village_chicken_dialogue_done", 1);
                chicken1_taken = true;
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
