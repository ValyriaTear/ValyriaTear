-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_wolf_cave_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest Cave"
map_image_filename = "data/story/common/locations/desert_cave.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "music/shrine-OGA-yd.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

-- the main character handler
local hero = nil

-- Dialogue sprites
local bronann = nil
local kalya = nil

-- the main map loading code
function Load(m)

    Map = m;
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(false);

    Map:SetMinimapImage("dat/maps/layna_forest/minimaps/layna_forest_wolf_cave_minimap.png");

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add a mediumly dark overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);

    -- Trigger the dialogue at entrance if not done yet.
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 0 and
            GlobalManager:DoesEventExist("story", "kalya_speech_in_wolf_cave") == false) then
        hero:SetMoving(false);
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("Wolf cave entrance dialogue", 200);
    end
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 26, 46, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_layna_cave_1_2") then
        hero:SetDirection(vt_map.MapMode.EAST);
        hero:SetPosition(3, 27);
    end

    bronann = CreateSprite(Map, "Bronann", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.WEST);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    bronann:SetVisible(false);

    kalya = CreateSprite(Map, "Kalya", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    kalya:SetDirection(vt_map.MapMode.NORTH);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);
end

-- a necklace obtained by kalya and triggering the seconde fight against the fenrir.
local necklace = nil
local necklace_npc = nil
local light_tilt = nil

local wolf = nil

-- The heal particle effect map object
local heal_effect = nil

function _CreateObjects()
    local object = nil
    local npc = nil
    local event = nil
    local text = nil

    -- Adapt the light color according to the time of the day.
    local light_color_red = 1.0;
    local light_color_green = 1.0;
    local light_color_blue = 1.0;
    local light_color_alpha = 0.8;
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        local tw_value = GlobalManager:GetEventValue("story", "layna_forest_twilight_value");
        if (tw_value >= 4 and tw_value < 6) then
            light_color_red = 0.83;
            light_color_green = 0.72;
            light_color_blue = 0.70;
            light_color_alpha = 0.29;
        elseif (tw_value >= 6 and tw_value < 8) then
            light_color_red = 0.62;
            light_color_green = 0.50;
            light_color_blue = 0.59;
            light_color_alpha = 0.49;
        elseif (tw_value >= 8) then
            light_color_red = 0.30;
            light_color_green = 0.30;
            light_color_blue = 0.46;
            light_color_alpha = 0.60;
        end
    end

    -- Add a halo showing the cave entrances
    vt_map.Halo.Create("data/visuals/lights/torch_light_mask.lua", 28, 59,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));

    -- Add different halo light, representing holes of light coming from the ceiling
    vt_map.Halo.Create("data/visuals/lights/right_ray_light.lua", 28, 17,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));

    -- Add the wolfpain necklace, triggering the second battle with the fenrir
    -- As this object is special, we're not using the object catalogue to only load that one once.
    necklace = vt_map.PhysicalObject.Create(vt_map.MapMode.GROUND_OBJECT);
    necklace:SetPosition(30, 9);
    necklace:SetCollPixelHalfWidth(16);
    necklace:SetCollPixelHeight(32);
    necklace:SetImgPixelHalfWidth(16);
    necklace:SetImgPixelHeight(32);
    necklace:AddAnimation("dat/maps/layna_forest/wolfpain_necklace.lua");

    -- Adds a light tilting to catch the player attention
    light_tilt = vt_map.PhysicalObject.Create(vt_map.MapMode.GROUND_OBJECT);
    light_tilt:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    light_tilt:SetPosition(30, 9.1);
    light_tilt:SetCollPixelHalfWidth(16);
    light_tilt:SetCollPixelHeight(32);
    light_tilt:SetImgPixelHalfWidth(16);
    light_tilt:SetImgPixelHeight(32);
    light_tilt:AddAnimation("data/visuals/lights/light_reverb.lua");

    -- Adds an associated npc to permit the dialogue to trigger
    necklace_npc = CreateSprite(Map, "Butterfly", 30, 9.2, vt_map.MapMode.GROUND_OBJECT);
    necklace_npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    necklace_npc:SetVisible(false);
    necklace_npc:SetName(""); -- Unset the speaker name

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("(Bronann looks at the ground)");
    dialogue:AddLineEvent(text, necklace_npc, "", "wolfpain necklace dialogue start");
    necklace_npc:AddDialogueReference(dialogue);

    -- Place all the jewel related object out of reach when the event is already done
    -- or the wolf beaten in the crystal map.
    if (GlobalManager:DoesEventExist("story", "wolfpain_necklace_obtained") == true
            or GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        necklace_npc:SetPosition(0, 0);

        light_tilt:SetVisible(false);
        necklace:SetVisible(false);
        necklace:SetPosition(0, 0);
    end

    -- The boss map sprite
    wolf = CreateSprite(Map, "Fenrir", 0, 0, vt_map.MapMode.GROUND_OBJECT); -- pre place it at the right place.
    wolf:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    wolf:SetMovementSpeed(vt_map.MapMode.VERY_FAST_SPEED);
    wolf:SetVisible(false);
    wolf:SetDirection(vt_map.MapMode.NORTH);

    -- Drink at the fountain
    npc = CreateSprite(Map, "Butterfly", 53, 12, vt_map.MapMode.GROUND_OBJECT);
    npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    npc:SetVisible(false);
    npc:SetName(""); -- Unset the speaker name

    -- Add the dialogue options on the fountain
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("This water looks weird. Shall we drink it anyway?");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("...");
    dialogue:AddLine(text, npc);
    text = vt_system.Translate("Yes, I'm so thirsty.");
    dialogue:AddOption(text, 2);
    text = vt_system.Translate("No way, we'd get sick.");
    dialogue:AddOption(text, 3);
    -- [Line 2] Drink it
    text = vt_system.Translate("The party drinks the water and feels... alright?");
    dialogue:AddLineEvent(text, npc, 4, "", "Fountain heal"); -- 4 = Past the dialogue lines number. Makes the dialogue ends.
    -- [Line 3] Won't drink it
    text = vt_system.Translate("The party won't drink it.");
    dialogue:AddLine(text, npc);
    npc:AddDialogueReference(dialogue);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject.Create("dat/effects/particles/heal_sp_particle.lua", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    heal_effect:Stop(); -- Don't run it until the character heals itself

    vt_map.SoundObject.Create("sounds/fountain_small.ogg", 53.0, 8.0, 8.0);
end

-- Special event references which destinations must be updated just before being called.
local move_next_to_bronann_event = nil
local move_next_to_bronann_event2 = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Map transition events
    vt_map.MapTransitionEvent.Create("to cave 1-2", "dat/maps/layna_forest/layna_forest_cave1_2_map.lua",
                                     "dat/maps/layna_forest/layna_forest_cave1_2_script.lua", "from_layna_wolf_cave");

    vt_map.MapTransitionEvent.Create("to south east exit", "dat/maps/layna_forest/layna_forest_south_east_map.lua",
                                     "dat/maps/layna_forest/layna_forest_south_east_script.lua", "from_layna_wolf_cave");

    -- SP Heal event on fountain
    vt_map.ScriptedEvent.Create("Fountain heal", "heal_party_sp", "heal_done");

    -- Dialogue events
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks west", kalya, vt_map.MapMode.WEST);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks south", kalya, vt_map.MapMode.SOUTH);
    vt_map.LookAtSpriteEvent.Create("Bronann looks at Kalya", hero, kalya);
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);

    vt_map.ScriptedSpriteEvent.Create("kalya:SetCollision(NONE)", kalya, "Sprite_Collision_off", "");
    vt_map.ScriptedSpriteEvent.Create("kalya:SetCollision(ALL)", kalya, "Sprite_Collision_on", "");

    event = vt_map.ScriptedEvent.Create("Wolf cave entrance dialogue", "cave_entrance_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 50);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_bronann_event = vt_map.PathMoveSpriteEvent.Create("Kalya moves next to Bronann", kalya, 0, 0, false);
    move_next_to_bronann_event:AddEventLinkAtEnd("kalya:SetCollision(ALL)");
    move_next_to_bronann_event:AddEventLinkAtEnd("Kalya looks north");
    move_next_to_bronann_event:AddEventLinkAtEnd("Kalya Tells about the smell");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Yiek, it stinks in here.");
    dialogue:AddLineEventEmote(text, kalya, "Bronann looks at Kalya", "Kalya looks at Bronann", "exclamation");
    text = vt_system.Translate("Look at all those bones. We should be careful.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("I hope Orlinn is alright.");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    event = vt_map.DialogueEvent.Create("Kalya Tells about the smell", dialogue);
    event:AddEventLinkAtEnd("kalya:SetCollision(NONE)");
    event:AddEventLinkAtEnd("Set Camera back to Bronann");

    event = vt_map.ScriptedSpriteEvent.Create("Set Camera back to Bronann", bronann, "SetCamera", "");
    event:AddEventLinkAtEnd("kalya goes back to party");

    event = vt_map.PathMoveSpriteEvent.Create("kalya goes back to party", kalya, bronann, false);
    event:AddEventLinkAtEnd("end of cave entrance dialogue");

    vt_map.ScriptedEvent.Create("end of cave entrance dialogue", "end_of_cave_entrance_dialogue", "");

    -- Wolfpain necklace dialogue
    event = vt_map.ScriptedEvent.Create("wolfpain necklace dialogue start", "wolfpain_necklace_dialogue_start", "");
    event:AddEventLinkAtEnd("necklace event: Kalya moves next to Bronann", 50);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_bronann_event2 = vt_map.PathMoveSpriteEvent.Create("necklace event: Kalya moves next to Bronann", kalya, 0, 0, false);
    move_next_to_bronann_event2:AddEventLinkAtEnd("kalya:SetCollision(ALL)");
    move_next_to_bronann_event2:AddEventLinkAtEnd("Kalya looks west");
    move_next_to_bronann_event2:AddEventLinkAtEnd("Kalya Tells about the necklace");

    vt_map.ScriptedEvent.Create("make fenrir appear and necklace disappear", "wolf_appear_n_necklace_disappear", "");

    vt_map.PathMoveSpriteEvent.Create("make fenrir come", wolf, 30, 17, false);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("What a lovely necklace! I'll take it.");
    dialogue:AddLineEventEmote(text, kalya, "Bronann looks at Kalya", "Kalya looks north", "exclamation");
    text = vt_system.Translate("Kalya. We're in the middle of a cave and you're only thinking about wearing jewels?");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Jewelry is a gift from the heavens to remind us that beauty can be as pure on the outside as it is on the inside. Besides, it suits me better than it would you.");
    dialogue:AddLineEvent(text, kalya, "make fenrir appear and necklace disappear", "make fenrir come");
    text = vt_system.Translate("Kalya! You'd better be careful!");
    dialogue:AddLineEventEmote(text, bronann, "Bronann looks south", "", "sweat drop");
    text = vt_system.Translate("Don't even start, this one is all mine.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Not the necklace, the fenrir!");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("You can't be serious, we would have heard it come.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Bronann", "Kalya looks south");
    event = vt_map.DialogueEvent.Create("Kalya Tells about the necklace", dialogue);
    event:AddEventLinkAtEnd("The Fenrir growls");

    event = vt_map.SoundEvent.Create("The Fenrir growls", "sounds/growl1_IFartInUrGeneralDirection_freesound.wav");
    event:AddEventLinkAtEnd("Kalya realizes for the Fenrir");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Ah, well. You were serious, weren't you?");
    dialogue:AddLineEmote(text, kalya, "sweat drop");
    event = vt_map.DialogueEvent.Create("Kalya realizes for the Fenrir", dialogue);
    event:AddEventLinkAtEnd("The Fenrir runs toward the hero");

    event = vt_map.PathMoveSpriteEvent.Create("The Fenrir runs toward the hero", wolf, bronann, true);
    event:AddEventLinkAtEnd("Second Fenrir battle");

    event = vt_map.BattleEncounterEvent.Create("Second Fenrir battle");
    event:SetMusic("music/accion-OGA-djsaryon.ogg");
    event:SetBackground("img/backdrops/battle/desert_cave/desert_cave.png");
    event:AddScript("dat/battles/desert_cave_battle_anim.lua");
    event:AddEnemy(7, 512, 500);
    event:SetBoss(true);
    event:AddEventLinkAtEnd("Make the fenrir disappear");

    event = vt_map.ScriptedEvent.Create("Make the fenrir disappear", "make_wolf_invisible", "");
    event:AddEventLinkAtEnd("Get the wolfpain necklace");

    event = vt_map.TreasureEvent.Create("Get the wolfpain necklace");
    event:AddItem(70003, 1); -- The wolfpain key item
    event:AddEventLinkAtEnd("Kalya talks after the battle");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("It ran away again. I'm glad we survived. Let's get out of here before it comes back.");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at Bronann", "", "sweat drop");
    event = vt_map.DialogueEvent.Create("Kalya talks after the battle", dialogue);
    event:AddEventLinkAtEnd("kalya:SetCollision(NONE)");
    event:AddEventLinkAtEnd("Set Camera back to Bronann2");

    event = vt_map.ScriptedSpriteEvent.Create("Set Camera back to Bronann2", bronann, "SetCamera", "");
    event:AddEventLinkAtEnd("end of necklace dialogue");
    event:AddEventLinkAtEnd("necklace event: kalya goes back to party");

    vt_map.PathMoveSpriteEvent.Create("necklace event: kalya goes back to party", kalya, bronann, false);

    vt_map.ScriptedEvent.Create("end of necklace dialogue", "end_of_necklace_dialogue", "");
end

-- zones
local to_cave_1_2_zone = nil
local to_cave_exit_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_cave_1_2_zone = vt_map.CameraZone.Create(0, 1, 24, 28);
    to_cave_exit_zone = vt_map.CameraZone.Create(24, 29, 47, 48);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_cave_1_2_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to cave 1-2");
    elseif (to_cave_exit_zone:IsCameraEntering() == true) then
        hero:SetMoving(false);
        EventManager:StartEvent("to south east exit");
    end
end

-- Effect time used when applying the heal light effect
local heal_effect_time = 0;
local heal_color = vt_video.Color(0.0, 1.0, 0.0, 1.0); -- green effect

-- Map Custom functions
-- Used through scripted events
map_functions = {
    SetCamera = function(sprite)
        Map:SetCamera(sprite, 800);
    end,

    Sprite_Collision_on = function(sprite)
        if (sprite ~= nil) then
            sprite:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        end
    end,

    Sprite_Collision_off = function(sprite)
        if (sprite ~= nil) then
            sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        end
    end,

    heal_party_sp = function()
        hero:SetMoving(false);
        -- Should be sufficient to heal anybody's SP
        GlobalManager:GetActiveParty():AddSkillPoints(10000);
        Map:SetStamina(10000);
        AudioManager:PlaySound("sounds/heal_spell.wav");
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

    -- cave entrance - start event.
    cave_entrance_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition())
        bronann:SetDirection(hero:GetDirection())
        bronann:SetVisible(true)
        hero:SetVisible(false)
        Map:SetCamera(bronann)
        hero:SetPosition(0, 0)

        kalya:SetVisible(true);
        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        bronann:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        Map:SetCamera(kalya, 800);

        move_next_to_bronann_event:SetDestination(bronann:GetXPosition() - 2.0, bronann:GetYPosition(), false);
    end,

    end_of_cave_entrance_dialogue = function()
        Map:PopState();
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition())
        hero:SetDirection(bronann:GetDirection())
        hero:SetVisible(true)
        bronann:SetVisible(false)
        Map:SetCamera(hero)
        bronann:SetPosition(0, 0)
        hero:SetDirection(vt_map.MapMode.NORTH);

        -- Set event as done
        GlobalManager:SetEventValue("story", "kalya_speech_in_wolf_cave", 1);
    end,

    -- Kalya takes the wolfpain necklace - start event.
    wolfpain_necklace_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);

        bronann:SetPosition(hero:GetXPosition(), hero:GetYPosition())
        bronann:SetDirection(hero:GetDirection())
        bronann:SetVisible(true)
        hero:SetVisible(false)
        Map:SetCamera(bronann)
        hero:SetPosition(0, 0)

        kalya:SetVisible(true);
        kalya:SetPosition(bronann:GetXPosition(), bronann:GetYPosition());
        bronann:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        Map:SetCamera(kalya, 800);

        move_next_to_bronann_event2:SetDestination(bronann:GetXPosition() + 2.0, bronann:GetYPosition(), false);
    end,

    wolf_appear_n_necklace_disappear = function()
        -- Make the necklace disappear
        necklace:SetVisible(false);
        necklace:SetPosition(0, 0);
        -- Prevents the event to start again
        necklace_npc:SetPosition(0, 0);
        light_tilt:SetVisible(false);

        -- Place the wolf
        wolf:SetVisible(true);
        wolf:SetPosition(27, 26);
        wolf:SetDirection(vt_map.MapMode.NORTH);

        -- Set event as done
        GlobalManager:SetEventValue("story", "wolfpain_necklace_obtained", 1);
    end,

    make_wolf_invisible = function()
        wolf:SetVisible(false);
        wolf:SetPosition(0, 0);
    end,

    end_of_necklace_dialogue = function()
        kalya:SetPosition(0, 0);
        kalya:SetVisible(false);
        kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        hero:SetPosition(bronann:GetXPosition(), bronann:GetYPosition())
        hero:SetDirection(bronann:GetDirection())
        hero:SetVisible(true)
        bronann:SetVisible(false)
        Map:SetCamera(hero)
        bronann:SetPosition(0, 0)
        hero:SetDirection(vt_map.MapMode.NORTH);

        Map:PopState();
    end
}
