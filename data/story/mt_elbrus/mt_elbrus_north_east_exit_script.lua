-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_north_east_exit_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "data/story/common/locations/mt_elbrus.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "sounds/wind.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

-- the main character handler
local hero = nil

local bronann = nil
local kalya = nil
local orlinn = nil

-- the main map loading code
function Load(m)

    Map = m;
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

    -- The far hills background
    Map:GetScriptSupervisor():AddScript("dat/maps/mt_elbrus/mt_elbrus_landscape_anim.lua");

    -- Start the far away village event
    EventManager:StartEvent("Heroes see the village event", 200);

    -- Funny music
    AudioManager:LoadMusic("music/Zander Noriega - School of Quirks.ogg", Map);
    -- Sad one
    AudioManager:LoadMusic("music/sad_moment.ogg", Map);

    -- TEMP: Credits place here in the wait of adding the last episode map.
    Map:GetScriptSupervisor():AddScript("dat/credits/end_credits.lua");
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 30, 16, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_basement") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(30.0, 16.0);
    end

    -- Create secondary characters
    bronann = CreateSprite(Map, "Bronann", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.NORTH);
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
    orlinn:SetMovementSpeed(vt_map.MapMode.FAST_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);
end

local kalya_tear = nil
local fire_particle_effect = nil

function _CreateObjects()
    -- Objects array
    local map_objects = {

        { "Tree Small1 snow", 35, 28 },
        { "Tree Small1 snow", 3, 46 },
        { "Tree Small1 snow", 18, 17 },
        { "Tree Small1 snow", 23, 30 },
        { "Tree Small1 snow", 18, 32 },
        { "Tree Small1 snow", 15, 34 },
        { "Tree Small1 snow", 52, 6 },
        { "Tree Small2 snow", 39, 26 },
        { "Tree Small2 snow", 11, 35 },

        { "Tree Big2 snow", 17, 40 },
        { "Tree Big2 snow", 9, 39 },
        { "Tree Big1 snow", 13, 20 },
        { "Tree Big2 snow", 13, 47 },
        { "Tree Big1 snow", 2, 34 },
        { "Tree Big1 snow", 19, 7 },
        { "Tree Big1 snow", 21, 41 },
        { "Tree Big2 snow", 24, 15 },
        { "Tree Big2 snow", 9, 29 },

        { "Rock1 snow", 20, 13 },
        { "Rock1 snow", 40, 30 },
        { "Rock1 snow", 12, 41 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_objects) do
        --print(my_array[1], my_array[2], my_array[3]);
        CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
    end

    -- Kalya's tears
    kalya_tear = vt_map.PhysicalObject.Create(vt_map.MapMode.GROUND_OBJECT);
    kalya_tear:SetPosition(0, 0);
    kalya_tear:SetCollPixelHalfWidth(0.156 * 16);
    kalya_tear:SetCollPixelHeight(0.312 * 16);
    kalya_tear:SetImgPixelHalfWidth(0.156 * 16);
    kalya_tear:SetImgPixelHeight(0.312 * 16);
    kalya_tear:AddStillFrame("data/particle_effects/outlined_circle_small.png");
    kalya_tear:SetDrawOnSecondPass(true);

    -- Village burning effect
    fire_particle_effect = vt_map.ParticleObject.Create("dat/maps/mt_elbrus/particles_fire_smoke.lua", 66, 24, vt_map.MapMode.GROUND_OBJECT);
    fire_particle_effect:Stop();
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- TODO: For Episode II
    --vt_map.MapTransitionEvent.Create("to mountain shrine basement", "dat/maps/mt_elbrus/mt_elbrus_shrine_basement_map.lua",
    --                                 "dat/maps/mt_elbrus/mt_elbrus_shrine_basement_script.lua", "from_shrine_north_exit");

    --vt_map.MapTransitionEvent.Create("to overworld", "dat/maps/xx/xx_map.lua",
    --                                 "dat/maps/xx/xx_script.lua", "from_shrine_north_exit");

    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Orlinn", kalya, orlinn);
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Kalya", orlinn, kalya);

    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks east", kalya, vt_map.MapMode.EAST);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks east", orlinn, vt_map.MapMode.EAST);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks east", bronann, vt_map.MapMode.EAST);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks west", bronann, vt_map.MapMode.WEST);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);

    -- Starts the episode I ending scene...
    event = vt_map.ScriptedEvent.Create("Heroes see the village event", "village_scene_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann1");
    event:AddEventLinkAtEnd("Orlinn moves next to Bronann1");
    event:AddEventLinkAtEnd("Bronann gets out of the cave");

    -- NOTE: the actual position will be set at event start.
    event = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann1", kalya, 33, 21, false);
    event:AddEventLinkAtEnd("Kalya looks at Bronann");
    event:AddEventLinkAtEnd("Bronann looks south");
    event:AddEventLinkAtEnd("Orlinn looks at Kalya");
    event:AddEventLinkAtEnd("free at last dialogue");

    vt_map.PathMoveSpriteEvent.Create("Orlinn moves next to Bronann1", orlinn, 27, 21, false);
    vt_map.PathMoveSpriteEvent.Create("Bronann gets out of the cave", bronann, 30, 19, false);

    vt_map.ScriptedEvent.Create("Orlinn laughs", "orlinn_laughs", "");
    vt_map.ScriptedEvent.Create("Orlinn stops laughing", "orlinn_stops_laughing", "");

    vt_map.ScriptedEvent.Create("Kalya laughs", "kalya_laughs", "");

    event = vt_map.ScriptedEvent.Create("Kalya stops laughing", "kalya_stops_laughing", "");
    event:AddEventLinkAtEnd("Orlinn stops laughing");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("We made it, at last!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("Indeed! Yeepee!");
    dialogue:AddLineEvent(text, orlinn, "Orlinn laughs", "");
    text = vt_system.Translate("I'm so relieved we made it safely, hee hee.");
    dialogue:AddLineEvent(text, kalya, "Kalya laughs", "");
    text = vt_system.Translate("Wait.");
    dialogue:AddLineEventEmote(text, bronann, "Bronann looks east", "Kalya stops laughing", "thinking dots");
    text = vt_system.Translate("Can you smell that stench?");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Yes. Is it... smoke?");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks east", "Orlinn looks east", "interrogation");
    text = vt_system.Translate("Something's burning.");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    event = vt_map.DialogueEvent.Create("free at last dialogue", dialogue);
    event:AddEventLinkAtEnd("Bronann runs to the cliff");
    event:AddEventLinkAtEnd("Orlinn runs to the cliff");
    event:AddEventLinkAtEnd("Kalya runs to the cliff");
    event:AddEventLinkAtEnd("Set Camera on village");

    vt_map.PathMoveSpriteEvent.Create("Bronann runs to the cliff", bronann, 55, 19, true);
    vt_map.PathMoveSpriteEvent.Create("Orlinn runs to the cliff", orlinn, 56, 17, true);
    vt_map.PathMoveSpriteEvent.Create("Kalya runs to the cliff", kalya, 57, 21, true);

    event = vt_map.ScriptedEvent.Create("Set Camera on village", "set_camera_on_village_start", "camera_update");
    event:AddEventLinkAtEnd("Start village burning effect");
    event:AddEventLinkAtEnd("Village burning dialogue", 2000);

    vt_map.ScriptedEvent.Create("Start village burning effect", "burning_particle_effect_start", "");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Our village. It's burning.");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("My parents.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Herth.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Everyone.");
    dialogue:AddLine(text, orlinn);
    event = vt_map.DialogueEvent.Create("Village burning dialogue", dialogue);
    event:AddEventLinkAtEnd("Kalya kneels", 2000);

    event = vt_map.ScriptedEvent.Create("Kalya kneels", "kalya_kneels", "");
    event:AddEventLinkAtEnd("Kalya cries", 1500);

    event = vt_map.ScriptedEvent.Create("Kalya cries", "kalya_cries_start", "kalya_cries_update");
    event:AddEventLinkAtEnd("Village burning dialogue 2", 2000);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("May they rest in peace.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("All this...");
    dialogue:AddLineEvent(text, bronann, "Bronann looks south", "Orlinn looks north");
    text = vt_system.Translate("All this was for a crystal?");
    dialogue:AddLineEvent(text, bronann, "Bronann looks south", "Orlinn looks north");
    text = vt_system.Translate("...");
    dialogue:AddLineEvent(text, bronann, "", "Bronann looks west");
    text = vt_system.Translate("I'll fix this.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("If this crystal is so powerful, then I'll use it. And I'll fix this.");
    dialogue:AddLineEvent(text, bronann, "Bronann looks east", "");
    event = vt_map.DialogueEvent.Create("Village burning dialogue 2", dialogue);
    event:AddEventLinkAtEnd("Kalya stops kneeling", 2000);

    event = vt_map.ScriptedEvent.Create("Kalya stops kneeling", "kalya_stops_kneeling", "");
    event:AddEventLinkAtEnd("Kalya looks at Bronann", 1000);
    event:AddEventLinkAtEnd("Village burning dialogue 3", 2000);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Then, I'll help you.");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Village burning dialogue 3", dialogue);
    event:AddEventLinkAtEnd("Episode I End Credits", 1000);

    -- TEMP: Episode I credits
    vt_map.ScriptedEvent.Create("Episode I End Credits", "start_credits", "");
end

-- zones
local to_basement_zone = nil
local to_overworld_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_basement_zone = vt_map.CameraZone.Create(29, 31, 12, 14);
    to_overworld_zone = vt_map.CameraZone.Create(0, 2, 34, 44);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_basement_zone:IsCameraEntering() == true) then
        hero:SetMoving(true);
        hero:SetDirection(vt_map.MapMode.NORTH);
        --EventManager:StartEvent("to mountain shrine basement");
    elseif (to_overworld_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.WEST);
        --EventManager:StartEvent("to overworld");
    end
end

-- counts kalya's tears
local number_of_tears = 0;
local tear_time = 0;

-- Map Custom functions
-- Used through scripted events
map_functions = {

    village_scene_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

        -- Place Bronann, and remove hero
        bronann:SetDirection(hero:GetDirection());
        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        Map:SetCamera(bronann);
        bronann:SetVisible(true);
        hero:SetVisible(false);
        hero:SetPosition(0, 0);

        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        kalya:SetVisible(true);
        orlinn:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        orlinn:SetVisible(true);
    end,

    orlinn_laughs = function()
        orlinn:SetCustomAnimation("laughing", 0); -- 0 means forever

        -- Play funny music
        AudioManager:PlayMusic("music/Zander Noriega - School of Quirks.ogg");
    end,

    orlinn_stops_laughing = function()
        orlinn:DisableCustomAnimation();
    end,

    kalya_laughs = function()
        kalya:SetCustomAnimation("laughing", 0); -- 0 means forever
    end,

    kalya_stops_laughing = function()
        kalya:DisableCustomAnimation();

        -- Make music fade out
        AudioManager:FadeOutActiveMusic(1000)
    end,

    set_camera_on_village_start = function()
        Map:MoveVirtualFocus(67, 19);
        Map:SetCamera(Map:GetVirtualFocus(), 1700);
    end,

    camera_update = function()
        if (Map:IsCameraMoving() == true) then
            return false;
        end
        return true;
    end,

    burning_particle_effect_start = function()
        fire_particle_effect:Start();
    end,

    kalya_kneels = function()
        kalya:SetCustomAnimation("kneeling", 0); -- 0 means forever
        AudioManager:PlayMusic("music/sad_moment.ogg");
    end,

    kalya_cries_start = function()
        kalya_tear:SetPosition(kalya:GetXPosition() + 0.5, kalya:GetYPosition() - 2.0);
        number_of_tears = 0;
        tear_time = 0;
    end,

    kalya_cries_update = function()
        local update_time = SystemManager:GetUpdateTime();


        -- Handle the tears falling
        if (kalya_tear:IsVisible() == true) then
            local movement_diff = 0.004 * update_time;
            kalya_tear:SetYPosition(kalya_tear:GetYPosition() + movement_diff);

            if (kalya_tear:GetYPosition() > kalya:GetYPosition() + 0.2) then
                number_of_tears = number_of_tears + 1;
                kalya_tear:SetYPosition(kalya:GetYPosition() - 2.0)
                kalya_tear:SetVisible(false)
            end
        else
            tear_time = tear_time + update_time;
            if (tear_time > 2000) then
                tear_time = 0;
                kalya_tear:SetVisible(true)
            end
        end

        if (number_of_tears > 1) then
            kalya_tear:SetPosition(0, 0)
            return true;
        end
        return false;
    end,

    kalya_stops_kneeling = function()
        kalya:DisableCustomAnimation();
    end,

    -- TEMP: Credits place here in the wait of adding the last episode map.
    start_credits = function()
        GlobalManager:SetEventValue("game", "Start_End_Credits", 1);
    end
}
