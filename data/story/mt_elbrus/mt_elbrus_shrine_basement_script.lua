-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine_basement_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "data/story/common/locations/mt_elbrus.png"
map_subname = "?"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/icy_wind.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

-- the main character handler
local hero = nil

-- Forest dialogue secondary hero
local bronann = nil
local kalya = nil
local orlinn = nil
local andromalius = nil

-- the main map loading code
function Load(m)

    Map = m;
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
    Map:GetEffectSupervisor():EnableAmbientOverlay("data/visuals/ambient/dark.png", 0.0, 0.0, false);

    -- Preload sounds
    AudioManager:LoadSound("data/sounds/heavy_bump.wav", Map);

    -- Place Orlinn for the final boss scene
    if (GlobalManager:GetGameEvents():GetEventValue("story", "mt_elbrus_ep1_final_boss_beaten") == 0) then
        orlinn:SetPosition(19, 13);
        orlinn:SetVisible(true);
        orlinn:SetDirection(vt_map.MapMode.WEST);

        andromalius:SetPosition(15, 13);
        andromalius:SetVisible(true);
        andromalius:SetDirection(vt_map.MapMode.EAST);
    end

    -- When falling from above, the heroes start by falling
    if (GlobalManager:GetPreviousLocation() == "from_shrine_stairs1") then
        hero:SetMoving(false);
        EventManager:StartEvent("Falls from above event", 200);
    end

    -- Loads the pre-boss music
    AudioManager:LoadMusic("data/music/dont_close_your_eyes.ogg", Map);
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from falling point)
    hero = CreateSprite(Map, "Bronann", 57, 0, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.SOUTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_shrine_north_exit") then
        hero:SetPosition(3.5, 22.0)
        hero:SetDirection(vt_map.MapMode.EAST);
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

    andromalius = CreateSprite(Map, "Andromalius", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    andromalius:SetName(vt_system.Translate("Andromalius"));
    andromalius:SetDirection(vt_map.MapMode.EAST);
    andromalius:SetMovementSpeed(vt_map.MapMode.FAST_SPEED);
    andromalius:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    andromalius:SetVisible(false);
end

function _CreateObjects()
    local object = nil
    local npc = nil
    local dialogue = nil
    local text = nil
    local event = nil

    vt_map.Halo.Create("data/visuals/lights/torch_light_mask.lua", 0, 28,
        vt_video.Color(1.0, 1.0, 1.0, 0.8));

    vt_map.Halo.Create("data/visuals/lights/right_ray_light.lua", 0, 28,
            vt_video.Color(1.0, 1.0, 1.0, 0.8));

    object = CreateTreasure(Map, "mt_shrine_basement_chest1", "Wood_Chest1", 20, 36, vt_map.MapMode.GROUND_OBJECT);
    object:AddItem(2, 3); -- Medium potion x 3
end

-- Special event references which destinations must be updated just before being called.
local kalya_move_next_to_hero_event1 = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    vt_map.MapTransitionEvent.Create("to mountain shrine exit", "data/story/mt_elbrus/mt_elbrus_north_east_exit_map.lua",
                                     "data/story/mt_elbrus/mt_elbrus_north_east_exit_script.lua", "from_shrine_basement");

    -- Generic events
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks west", orlinn, vt_map.MapMode.WEST);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks south", orlinn, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks north", bronann, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks west", kalya, vt_map.MapMode.WEST);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks east", kalya, vt_map.MapMode.EAST);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks south", kalya, vt_map.MapMode.SOUTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Hero looks west", hero, vt_map.MapMode.WEST);
    vt_map.ChangeDirectionSpriteEvent.Create("Hero looks east", hero, vt_map.MapMode.EAST);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Orlinn", kalya, orlinn);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Kalya", bronann, kalya);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Orlinn", bronann, orlinn);
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Kalya", orlinn, kalya);
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Bronann", orlinn, bronann);

    -- Falling event
    event = vt_map.ScriptedEvent.Create("Falls from above event", "fall_event_start", "fall_event_update");
    event:AddEventLinkAtEnd("After the fall event start", 1500);

    event = vt_map.ScriptedEvent.Create("After the fall event start", "after_fall_event_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann1");

    -- NOTE: The actual destination is set just before the actual start call
    kalya_move_next_to_hero_event1 = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann1", kalya, 0, 0, false);
    kalya_move_next_to_hero_event1:AddEventLinkAtEnd("Where is Orlinn? dialogue");
    kalya_move_next_to_hero_event1:AddEventLinkAtEnd("Kalya looks at Bronann");
    kalya_move_next_to_hero_event1:AddEventLinkAtEnd("Bronann looks at Kalya");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Wow, what a fall. I didn't see that one coming.");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("Are you alright, Kalya?");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    text = vt_system.Translate("I should be fine, but we should heal just in case.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Wait...");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks west", "", "thinking dots");
    text = vt_system.Translate("Where is Orlinn?");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks east", "Bronann looks south", "interrogation");
    text = vt_system.Translate("Orlinn? Orlinn!");
    dialogue:AddLineEventEmote(text, bronann, "Kalya looks south", "Bronann looks north", "exclamation");
    text = vt_system.Translate("Let's find him, once more, before something bad happens.");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    event = vt_map.DialogueEvent.Create("Where is Orlinn? dialogue", dialogue);
    event:AddEventLinkAtEnd("Kalya moves back to party1");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya moves back to party1", kalya, bronann, false);
    event:AddEventLinkAtEnd("After the fall event end");

    vt_map.ScriptedEvent.Create("After the fall event end", "after_fall_event_end", "");

    -- Hero sees the exit
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("This is the blocked passage to the shrine entrance.");
    dialogue:AddLineEventEmote(text, hero, "Hero looks east", "", "thinking dots");
    text = vt_system.Translate("This can only mean we're near the exit!");
    dialogue:AddLineEventEmote(text, hero, "Hero looks west", "", "exclamation");
    vt_map.DialogueEvent.Create("See exit dialogue", dialogue);

    -- Orlinn screams
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Help!");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Orlinn!");
    dialogue:AddLineEmote(text, hero, "exclamation");
    vt_map.DialogueEvent.Create("Orlinn screams dialogue", dialogue);

    -- Final boss event
    event = vt_map.ScriptedEvent.Create("Final boss battle event start", "final_boss_event_start", "");
    event:AddEventLinkAtEnd("Bronann move to the scene start point");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann2");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann move to the scene start point", bronann, 25, 13, true);
    event:AddEventLinkAtEnd("Bronann looks at Orlinn");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann2", kalya, 24, 16, true);
    event:AddEventLinkAtEnd("Before boss dialogue");
    event:AddEventLinkAtEnd("Kalya looks at Orlinn");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Help!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    text = vt_system.Translate("Orlinn! Back off!");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("I, I...");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    text = vt_system.Translate("Orlinn! NOW!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    event = vt_map.DialogueEvent.Create("Before boss dialogue", dialogue);
    event:AddEventLinkAtEnd("Orlinn back off");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn back off", orlinn, 29, 12, true);
    event:AddEventLinkAtEnd("Orlinn looks at Bronann");
    event:AddEventLinkAtEnd("Before boss dialogue2");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("You shall not leave this place without my consent.");
    dialogue:AddLine(text, andromalius);
    text = vt_system.Translate("And you shall not touch my brother without mine.");
    dialogue:AddLineEmote(text, kalya, "exclamation");
    text = vt_system.Translate("You have made it through the holy ordeal alive. I shall not let you survive.");
    dialogue:AddLine(text, andromalius);
    text = vt_system.Translate("Let's fight for our lives then!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    event = vt_map.DialogueEvent.Create("Before boss dialogue2", dialogue);
    event:AddEventLinkAtEnd("Battle with the boss");

    event = vt_map.BattleEncounterEvent.Create("Battle with the boss");
    event:SetMusic("data/music/accion-OGA-djsaryon.ogg");
    event:SetBackground("data/battles/battle_scenes/desert_cave/desert_cave.png");
    event:AddScript("data/battles/battle_scenes/desert_cave_battle_anim.lua");
    event:SetBoss(true);
    event:AddEnemy(21, 812, 312);
    event:AddEnemy(20, 512, 512);
    event:AddEventLinkAtEnd("Make the boss invisible");
    event:AddEventLinkAtEnd("Bronann runs to Orlinn", 1000);
    event:AddEventLinkAtEnd("Kalya runs to Orlinn", 1000);
    event:AddEventLinkAtEnd("Play default music");

    vt_map.ScriptedEvent.Create("Play default music", "play_default_music", "");

    vt_map.ScriptedEvent.Create("Make the boss invisible", "make_boss_invisible", "");

    event = vt_map.PathMoveSpriteEvent.Create("Bronann runs to Orlinn", bronann, 27, 11, true);
    event:AddEventLinkAtEnd("Bronann looks at Orlinn");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya runs to Orlinn", kalya, 27, 13, true);
    event:AddEventLinkAtEnd("Kalya looks at Orlinn");
    event:AddEventLinkAtEnd("Dialogue with Orlinn");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Orlinn, are you alright?");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    text = vt_system.Translate("Yes. I'm sorry I wasn't able to protect myself, sis.");
    dialogue:AddLineEmote(text, orlinn, "sweat drop");
    text = vt_system.Translate("Don't you worry about that, brother. Never leave me like that again, ok?");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("I promise I didn't do it this time. I just fell in front of this hideous green head.");
    dialogue:AddLine(text, orlinn);
    text = vt_system.Translate("Well, let's leave this place before another one wants our lives again.");
    dialogue:AddLineEmote(text, bronann, "sweat drop");
    text = vt_system.Translate("I couldn't agree more.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "");
    event = vt_map.DialogueEvent.Create("Dialogue with Orlinn", dialogue);
    event:AddEventLinkAtEnd("Kalya moves back to party2");
    event:AddEventLinkAtEnd("Orlinn moves back to party2");

    event = vt_map.PathMoveSpriteEvent.Create("Kalya moves back to party2", kalya, bronann, false);
    event:AddEventLinkAtEnd("Final boss event end");

    vt_map.PathMoveSpriteEvent.Create("Orlinn moves back to party2", orlinn, bronann, false);

    vt_map.ScriptedEvent.Create("Final boss event end", "final_boss_event_end", "");
end

-- zones
local see_exit_zone = nil
local orlinn_screams_zone = nil
local final_boss_zone = nil
local to_mountain_exit_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    orlinn_screams_zone = vt_map.CameraZone.Create(36, 40, 36, 48);
    see_exit_zone = vt_map.CameraZone.Create(56, 61, 40, 45);
    final_boss_zone = vt_map.CameraZone.Create(21, 36, 3, 22);
    to_mountain_exit_zone = vt_map.CameraZone.Create(0, 2, 15, 34);
end

-- Booleans preventing from starting the even more than once.
local heard_orlinn_screaming = false;
local saw_exit = false;

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_mountain_exit_zone:IsCameraEntering() == true) then
        EventManager:StartEvent("to mountain shrine exit");
        hero:SetMoving(false);

    elseif (final_boss_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        if (GlobalManager:GetGameEvents():GetEventValue("story", "mt_elbrus_ep1_final_boss_beaten") == 0) then
            hero:SetMoving(false);
            EventManager:StartEvent("Final boss battle event start");
        end
    elseif (heard_orlinn_screaming == false and orlinn_screams_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        if (GlobalManager:GetGameEvents():GetEventValue("story", "mt_elbrus_ep1_final_boss_beaten") == 0) then
            hero:SetMoving(false);
            EventManager:StartEvent("Orlinn screams dialogue");
            heard_orlinn_screaming = true;
        end
    elseif (saw_exit == false and see_exit_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        if (GlobalManager:GetGameEvents():GetEventValue("story", "mt_elbrus_ep1_final_boss_beaten") == 0) then
            hero:SetMoving(false);
            EventManager:StartEvent("See exit dialogue");
            saw_exit = true;
        end
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
        local char = GlobalManager:GetCharacter(index);
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

-- Map Custom functions
-- Used through scripted events
map_functions = {

    fall_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        -- place the character and make it fall
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(57.0, 0.0);
        hero:SetCustomAnimation("frightened_fixed", 0); -- 0 means forever
    end,

    fall_event_update = function()
        if (hero:GetYPosition() >= 8.0) then
            AudioManager:PlaySound("data/sounds/heavy_bump.wav");
            Map:GetEffectSupervisor():ShakeScreen(0.6, 600, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);
            hero:SetCustomAnimation("hurt", 600);
            _TriggerPartyDamage(math.random(15, 30));
            return true;
        end

        -- Push the character down.
        local update_time = SystemManager:GetUpdateTime();
        local movement_diff = 0.011 * update_time;
        hero:SetYPosition(hero:GetYPosition() + movement_diff);
        return false;
    end,

    after_fall_event_start = function()
        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        bronann:SetDirection(hero:GetDirection());
        bronann:SetVisible(true);
        Map:SetCamera(bronann);

        hero:SetPosition(0, 0);

        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        kalya:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        kalya_move_next_to_hero_event1:SetDestination(bronann:GetXPosition(), bronann:GetYPosition() - 2.0, false);
    end,

    after_fall_event_end = function()
        Map:PopState();
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        hero:SetDirection(bronann:GetDirection());
        hero:SetVisible(true);
        Map:SetCamera(hero);

        bronann:SetPosition(0, 0);
        bronann:SetVisible(false);
        bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end,

    final_boss_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        bronann:SetDirection(hero:GetDirection());
        bronann:SetVisible(true);
        Map:SetCamera(bronann);

        hero:SetPosition(0, 0);

        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        kalya:SetVisible(true);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Start the pre-boss music
        AudioManager:PlayMusic("data/music/dont_close_your_eyes.ogg")
    end,

    play_default_music = function()
        AudioManager:PlayMusic("data/music/icy_wind.ogg")
    end,

    make_boss_invisible = function()
        andromalius:SetPosition(0, 0);
        andromalius:SetVisible(false);
    end,

    final_boss_event_end = function()
        Map:PopState();
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        hero:SetDirection(bronann:GetDirection());
        hero:SetVisible(true);
        Map:SetCamera(hero);

        bronann:SetPosition(0, 0);
        bronann:SetVisible(false);
        bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Set event as done
        GlobalManager:GetGameEvents():SetEventValue("story", "mt_elbrus_ep1_final_boss_beaten", 1);
    end,
}
