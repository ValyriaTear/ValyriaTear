-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_bronanns_home_first_floor_script = ns;
setfenv(1, ns);

-- The map name and location image
map_name = "Where it all began..."
map_image_filename = ""
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/koertes-ccby-birdsongloop16s.ogg"

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- the main character handler
local bronann = {};

-- opening objects
local bronann_in_bed = {};
local bed = {};

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

    Map.unlimited_stamina = true;

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on bronann
    Map:SetCamera(bronann);

    _CreateEvents();
    _CreateZones();

    -- If not done, start the opening dialogue
    if (GlobalManager:DoesEventExist("story", "opening_dialogue_done") == false) then
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        EventManager:StartEvent("opening", 10000);
        bronann_in_bed:SetVisible(true);
        bed:SetVisible(false);
        bronann:SetVisible(false);

        -- Also, reset the crystal appearance value to prevent triggering the event when reaching
        -- the crystal map:
        GlobalManager:SetEventValue("story", "layna_forest_crystal_appearance", 0);
    else
        -- The event is done, spawn bronann and the bed normally
        bronann_in_bed:SetVisible(false);
        bed:SetVisible(true);
        bronann:SetVisible(true);
    end

    -- Permits the display of basic game commands
    Map:GetScriptSupervisor():AddScript("dat/help/in_game_move_and_interact_anim.lua");

    Map:AddSavePoint(34, 35);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    bronann = CreateSprite(Map, "Bronann", 23.5, 17.5);
    bronann:SetDirection(vt_map.MapMode.SOUTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    local x_position = GlobalManager:GetSaveLocationX();
    local y_position = GlobalManager:GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Use the save point position, and clear the save position data for next maps
        GlobalManager:UnsetSaveLocation();
        -- Make the character look at us in that case
        bronann:SetDirection(vt_map.MapMode.SOUTH);
        bronann:SetPosition(x_position, y_position);
    elseif (GlobalManager:GetPreviousLocation() == "from_bronanns_home") then
        -- Set up the position according to the previous map
        bronann:SetPosition(37.5, 17.5);
        bronann:SetDirection(vt_map.MapMode.WEST);
    end

    Map:AddGroundObject(bronann);

    -- Add Bronann in bed wake up animation
    bronann_in_bed = vt_map.PhysicalObject();
    bronann_in_bed:SetObjectID(Map.object_supervisor:GenerateObjectID());
    bronann_in_bed:SetPosition(20, 20);
    bronann_in_bed:SetCollHalfWidth(1.75);
    bronann_in_bed:SetCollHeight(5.50);
    bronann_in_bed:SetImgHalfWidth(1.75);
    bronann_in_bed:SetImgHeight(5.68);
    bronann_in_bed:AddAnimation("img/sprites/map/characters/bronann_bed_animation.lua");

    Map:AddGroundObject(bronann_in_bed);
end

function _CreateObjects()
    object = {}

    -- Bronann's room
    bed = CreateObject(Map, "Bed1", 20, 20);
    if (bed ~= nil) then Map:AddGroundObject(bed) end;

    local chest = CreateTreasure(Map, "bronann_room_chest", "Wood_Chest1", 19, 22);
    if (chest ~= nil) then
        chest:SetDrunes(5);
        Map:AddGroundObject(chest);
    end

    chest = CreateTreasure(Map, "bronannsparent_room_chest", "Locker_Chest1", 39, 33.2);
    if (chest ~= nil) then
        chest:AddObject(11, 1);
        chest:SetDrunes(12);
        chest:SetDrawOnSecondPass(true); -- Above the table
        chest:SetCollisionMask(vt_map.MapMode.WALL_COLLISION);
        Map:AddGroundObject(chest);
    end

    object = CreateObject(Map, "Chair1", 23, 26);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Small Wooden Table", 20, 27);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Candle1", 19, 25);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    object:SetDrawOnSecondPass(true); -- Above the table
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Book1", 21, 25);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Left Window Light", 19, 21);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    if (object ~= nil) then Map:AddGroundObject(object) end;

    -- Parent's room
    object = CreateObject(Map, "Bed2", 37.7, 30.0);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Chair1_inverted", 38, 35);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Small Wooden Table", 40, 36);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Paper and Feather", 41, 34);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Box1", 40, 38); -- Prevent from going south of the table.
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Box1", 19, 35);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Box1", 19, 37);
    if (object ~= nil) then Map:AddGroundObject(object) end;
    object = CreateObject(Map, "Box1", 23, 35);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Flower Pot1", 27, 34.5);
    if (object ~= nil) then Map:AddGroundObject(object) end;

    object = CreateObject(Map, "Right Window Light", 41, 33);
    object:SetDrawOnSecondPass(true); -- Above any other ground object
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    if (object ~= nil) then Map:AddGroundObject(object) end;
end


-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- fade out after the bed animation
    event = vt_map.ScriptedEvent("opening", "begin_fade_out", "fade_out_update");
    event:AddEventLinkAtEnd("opening_dialogue");
    EventManager:RegisterEvent(event);

    -- Bronann's opening dialogue
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("I had that same nightmare again. This time, however, I still feel dizzy even after getting up.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("I might as well move on and forget about it.");
    dialogue:AddLine(text, bronann);
    DialogueManager:AddDialogue(dialogue);

    -- Bronann's opening dialogue event
    event = vt_map.DialogueEvent("opening_dialogue", dialogue);
    event:AddEventLinkAtEnd("opening2");
    EventManager:RegisterEvent(event);

    -- Unblock Bronann so he can start walking
    event = vt_map.ScriptedEvent("opening2", "Map_PopState", "");
    event:AddEventLinkAtEnd("opening3");
    EventManager:RegisterEvent(event);

    -- Set the opening dialogue as done
    event = vt_map.ScriptedEvent("opening3", "OpeningDialogueDone", "");
    EventManager:RegisterEvent(event);

    -- Triggered events
    event = vt_map.MapTransitionEvent("exit floor", "dat/maps/layna_village/layna_village_bronanns_home_map.lua",
                                       "dat/maps/layna_village/layna_village_bronanns_home_script.lua", "From Bronann's first floor");
    EventManager:RegisterEvent(event);

    -- Adds a small dialogue describing the save point use.
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.VTranslate("This circle is a save point. Here, you can save your game by pushing %s.", InputManager:GetConfirmKeyName());
    dialogue:AddLine(text, 0); -- nameless speaker
    text = vt_system.Translate("You can come back here and save as many times as you want.");
    dialogue:AddLine(text, 0); -- nameless speaker
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Save point dialogue", dialogue);
    event:AddEventLinkAtEnd("Disable save point dialogue");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Disable save point dialogue", "save_point_dialogue_done", "");
    EventManager:RegisterEvent(event);
end

-- zones
local room_exit_zone = {};
local save_point_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    room_exit_zone = vt_map.CameraZone(38, 39, 16, 19);
    Map:AddZone(room_exit_zone);

    save_point_zone = vt_map.CameraZone(32, 36, 31, 35);
    Map:AddZone(save_point_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (room_exit_zone:IsCameraEntering() == true) then
        bronann:SetMoving(false);
        EventManager:StartEvent("exit floor");

        -- Disable the game commands display
        GlobalManager:SetEventValue("game", "show_move_interact_info", 0);
    elseif (save_point_zone:IsCameraEntering() == true) then
        if (GlobalManager:GetEventValue("story", "save_point_dialogue_done") == 0) then
            bronann:SetMoving(false);
            EventManager:StartEvent("Save point dialogue");
        end
    end
end


local fade_effect_time = 0;
local fade_set = false;

local black_layer = vt_video.Color(0.0, 0.0, 0.0, 1.0);

-- Map Custom functions
-- Used through scripted events
map_functions = {

    Map_PopState = function()
        Map:PopState();
    end,

    begin_fade_out = function()
        fade_effect_time = 0.0;
        fade_set = false;
    end,

    fade_out_update = function()
        fade_effect_time = fade_effect_time + SystemManager:GetUpdateTime();

        if (fade_effect_time < 1000.0) then
            black_layer:SetAlpha(fade_effect_time / 1000.0);
            Map:GetEffectSupervisor():EnableLightingOverlay(black_layer);
            return false;
        end

        if (fade_effect_time >= 1000.0 and fade_effect_time < 2000.0) then
            -- Once the fade out is done, move the character to its new place.
            if (fade_set == false) then
                bronann:SetVisible(true);
                bed:SetVisible(true);
                bronann_in_bed:SetVisible(false);
                -- play a sound of clothes, meaning Bronann get dressed
                AudioManager:PlaySound("snd/cloth_sound.wav");
                fade_set = true;
            end
            return false;
        end
        if (fade_effect_time >= 2000.0 and fade_effect_time < 3000.0) then
            black_layer:SetAlpha(((3000.0 - fade_effect_time) / 1000.0));
            Map:GetEffectSupervisor():EnableLightingOverlay(black_layer);
            return false;
        end

        Map:GetEffectSupervisor():DisableLightingOverlay();
        return true;
    end,

    OpeningDialogueDone = function()
        GlobalManager:SetEventValue("story", "opening_dialogue_done", 1);

        -- Trigger the basic commands so that player knows what to do.
        GlobalManager:SetEventValue("game", "show_move_interact_info", 1);
    end,

    save_point_dialogue_done = function()
        GlobalManager:SetEventValue("story", "save_point_dialogue_done", 1);
    end
}
