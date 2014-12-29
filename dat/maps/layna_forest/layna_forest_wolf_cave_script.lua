-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_wolf_cave_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest Cave"
map_image_filename = "img/menus/locations/desert_cave.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/shrine-OGA-yd.ogg"

-- c++ objects instances
local Map = nil
local DialogueManager = nil
local EventManager = nil

-- the main character handler
local hero = nil

-- Forest dialogue secondary hero
local kalya_sprite = nil

-- Name of the main sprite. Used to reload the good one at the end of the dialogue events.
local main_sprite_name = "";

-- the main map loading code
function Load(m)

    Map = m;
    DialogueManager = Map:GetDialogueSupervisor();
    EventManager = Map:GetEventSupervisor();
    Map:SetUnlimitedStamina(false);

    Map:SetMinimapImage("dat/maps/layna_forest/minimaps/layna_forest_wolf_cave_minimap.png");

    _CreateCharacters();
    _CreateObjects();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

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

    -- Create secondary character for dialogue at map entrance
    kalya_sprite = CreateSprite(Map, "Kalya",
                                hero:GetXPosition(), hero:GetYPosition(), vt_map.MapMode.GROUND_OBJECT);

    kalya_sprite:SetDirection(vt_map.MapMode.NORTH);
    kalya_sprite:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya_sprite:SetVisible(false);
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
    vt_map.Halo.Create("img/misc/lights/torch_light_mask.lua", 28, 59,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));

    -- Add different halo light, representing holes of light coming from the ceiling
    vt_map.Halo.Create("img/misc/lights/right_ray_light.lua", 28, 17,
            vt_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha));

    -- Add the wolfpain necklace, triggering the second battle with the fenrir
    -- As this object is special, we're not using the object catalogue to only load that one once.
    necklace = vt_map.PhysicalObject.Create(vt_map.MapMode.GROUND_OBJECT);
    necklace:SetPosition(30, 9);
    necklace:SetCollHalfWidth(0.5);
    necklace:SetCollHeight(1.0);
    necklace:SetImgHalfWidth(0.5);
    necklace:SetImgHeight(1.0);
    necklace:AddAnimation("dat/maps/layna_forest/wolfpain_necklace.lua");

    -- Adds a light tilting to catch the player attention
    light_tilt = vt_map.PhysicalObject.Create(vt_map.MapMode.GROUND_OBJECT);
    light_tilt:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    light_tilt:SetPosition(30, 9.1);
    light_tilt:SetCollHalfWidth(0.5);
    light_tilt:SetCollHeight(1.0);
    light_tilt:SetImgHalfWidth(0.5);
    light_tilt:SetImgHeight(1.0);
    light_tilt:AddAnimation("img/misc/lights/light_reverb.lua");

    -- Adds an associated npc to permit the dialogue to trigger
    necklace_npc = CreateSprite(Map, "Butterfly", 30, 9.2, vt_map.MapMode.GROUND_OBJECT);
    necklace_npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    necklace_npc:SetVisible(false);
    necklace_npc:SetName(""); -- Unset the speaker name

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("(Bronann looks on the ground ...)");
    dialogue:AddLineEvent(text, necklace_npc, "", "wolfpain necklace dialogue start");
    DialogueManager:AddDialogue(dialogue);
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
    dialogue = vt_map.SpriteDialogue();
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
    DialogueManager:AddDialogue(dialogue);
    npc:AddDialogueReference(dialogue);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject.Create("dat/effects/particles/heal_sp_particle.lua", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    heal_effect:Stop(); -- Don't run it until the character heals itself

    vt_map.SoundObject.Create("snd/fountain_small.ogg", 53.0, 8.0, 8.0);
end

-- Special event references which destinations must be updated just before being called.
local move_next_to_hero_event = nil
local move_back_to_hero_event = nil
local move_next_to_hero_event2 = nil
local move_back_to_hero_event2 = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    -- Map transition events
    event = vt_map.MapTransitionEvent("to cave 1-2", "dat/maps/layna_forest/layna_forest_cave1_2_map.lua",
                                       "dat/maps/layna_forest/layna_forest_cave1_2_script.lua", "from_layna_wolf_cave");
    EventManager:RegisterEvent(event);

    event = vt_map.MapTransitionEvent("to south east exit", "dat/maps/layna_forest/layna_forest_south_east_map.lua",
                                       "dat/maps/layna_forest/layna_forest_south_east_script.lua", "from_layna_wolf_cave");
    EventManager:RegisterEvent(event);

    -- SP Heal event on fountain
    event = vt_map.ScriptedEvent("Fountain heal", "heal_party_sp", "heal_done");
    EventManager:RegisterEvent(event);

    -- Dialogue events
    event = vt_map.LookAtSpriteEvent("Kalya looks at Bronann", kalya_sprite, hero);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks north", kalya_sprite, vt_map.MapMode.NORTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks west", kalya_sprite, vt_map.MapMode.WEST);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Kalya looks south", kalya_sprite, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = vt_map.LookAtSpriteEvent("Bronann looks at Kalya", hero, kalya_sprite);
    EventManager:RegisterEvent(event);
    event = vt_map.ChangeDirectionSpriteEvent("Bronann looks south", hero, vt_map.MapMode.SOUTH);
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("kalya_sprite:SetCollision(NONE)", kalya_sprite, "Sprite_Collision_off", "");
    EventManager:RegisterEvent(event);
    event = vt_map.ScriptedSpriteEvent("kalya_sprite:SetCollision(ALL)", kalya_sprite, "Sprite_Collision_on", "");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Wolf cave entrance dialogue", "cave_entrance_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 50);
    EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event = vt_map.PathMoveSpriteEvent("Kalya moves next to Bronann", kalya_sprite, 0, 0, false);
    move_next_to_hero_event:AddEventLinkAtEnd("kalya_sprite:SetCollision(ALL)");
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya looks north");
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya Tells about the smell");
    EventManager:RegisterEvent(move_next_to_hero_event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Yiek, it's stinking in here.");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Bronann looks at Kalya", "Kalya looks at Bronann", "exclamation");
    text = vt_system.Translate("Look at all those bones. We should be careful.");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("I hope Orlinn is alright.");
    dialogue:AddLineEmote(text, kalya_sprite, "sweat drop");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya Tells about the smell", dialogue);
    event:AddEventLinkAtEnd("kalya_sprite:SetCollision(NONE)");
    event:AddEventLinkAtEnd("Set Camera back to Bronann");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Set Camera back to Bronann", hero, "SetCamera", "");
    event:AddEventLinkAtEnd("kalya goes back to party");
    EventManager:RegisterEvent(event);

    move_back_to_hero_event = vt_map.PathMoveSpriteEvent("kalya goes back to party", kalya_sprite, hero, false);
    move_back_to_hero_event:AddEventLinkAtEnd("end of cave entrance dialogue");
    EventManager:RegisterEvent(move_back_to_hero_event);

    event = vt_map.ScriptedEvent("end of cave entrance dialogue", "end_of_cave_entrance_dialogue", "");
    EventManager:RegisterEvent(event);

    -- Wolfpain necklace dialogue
    event = vt_map.ScriptedEvent("wolfpain necklace dialogue start", "wolfpain_necklace_dialogue_start", "");
    event:AddEventLinkAtEnd("necklace event: Kalya moves next to Bronann", 50);
    EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event2 = vt_map.PathMoveSpriteEvent("necklace event: Kalya moves next to Bronann", kalya_sprite, 0, 0, false);
    move_next_to_hero_event2:AddEventLinkAtEnd("kalya_sprite:SetCollision(ALL)");
    move_next_to_hero_event2:AddEventLinkAtEnd("Kalya looks west");
    move_next_to_hero_event2:AddEventLinkAtEnd("Kalya Tells about the necklace");
    EventManager:RegisterEvent(move_next_to_hero_event2);

    event = vt_map.ScriptedEvent("make fenrir appear and necklace disappear", "wolf_appear_n_necklace_disappear", "");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("make fenrir come", wolf, 30, 17, false);
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("What a lovely necklace! I'll take it.");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Bronann looks at Kalya", "Kalya looks north", "exclamation");
    text = vt_system.Translate("Kalya ... We're in the middle of a cave and you're only thinking about wearing jewels.");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("Jewelry is a gift from the heavens to remind us that beauty can be as pure on the outside as it is on the inside. Besides, it suits me better than it would you.");
    dialogue:AddLineEvent(text, kalya_sprite, "make fenrir appear and necklace disappear", "make fenrir come");
    text = vt_system.Translate("Kalya! You'd better be careful!");
    dialogue:AddLineEventEmote(text, hero, "Bronann looks south", "", "sweat drop");
    text = vt_system.Translate("Don't even start, this one is all mine.");
    dialogue:AddLine(text, kalya_sprite);
    text = vt_system.Translate("Not the necklace, the Fenrir!");
    dialogue:AddLineEmote(text, hero, "exclamation");
    text = vt_system.Translate("You can't be serious, we would have heard it come.");
    dialogue:AddLineEvent(text, kalya_sprite, "Kalya looks at Bronann", "Kalya looks south");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya Tells about the necklace", dialogue);
    event:AddEventLinkAtEnd("The Fenrir growls");
    EventManager:RegisterEvent(event);

    event = vt_map.SoundEvent("The Fenrir growls", "snd/growl1_IFartInUrGeneralDirection_freesound.wav");
    event:AddEventLinkAtEnd("Kalya realizes for the Fenrir");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Ah well, you were serious, weren't you?");
    dialogue:AddLineEmote(text, kalya_sprite, "sweat drop");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya realizes for the Fenrir", dialogue);
    event:AddEventLinkAtEnd("The Fenrir runs toward the hero");
    EventManager:RegisterEvent(event);

    event = vt_map.PathMoveSpriteEvent("The Fenrir runs toward the hero", wolf, hero, true);
    event:AddEventLinkAtEnd("Second Fenrir battle");
    EventManager:RegisterEvent(event);

    event = vt_map.BattleEncounterEvent("Second Fenrir battle");
    event:SetMusic("mus/accion-OGA-djsaryon.ogg");
    event:SetBackground("img/backdrops/battle/desert_cave/desert_cave.png");
    event:AddScript("dat/battles/desert_cave_battle_anim.lua");
    event:AddEnemy(7, 512, 500);
    event:SetBoss(true);
    event:AddEventLinkAtEnd("Make the fenrir disappear");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedEvent("Make the fenrir disappear", "make_wolf_invisible", "");
    event:AddEventLinkAtEnd("Get the wolfpain necklace");
    EventManager:RegisterEvent(event);

    event = vt_map.TreasureEvent("Get the wolfpain necklace");
    event:AddObject(70003, 1); -- The wolfpain key item
    event:AddEventLinkAtEnd("Kalya talks after the battle");
    EventManager:RegisterEvent(event);

    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("It ran away again... I'm glad we survived... Let's get out of here before it comes back.");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Kalya looks at Bronann", "", "sweat drop");
    DialogueManager:AddDialogue(dialogue);
    event = vt_map.DialogueEvent("Kalya talks after the battle", dialogue);
    event:AddEventLinkAtEnd("kalya_sprite:SetCollision(NONE)");
    event:AddEventLinkAtEnd("Set Camera back to Bronann2");
    EventManager:RegisterEvent(event);

    event = vt_map.ScriptedSpriteEvent("Set Camera back to Bronann2", hero, "SetCamera", "");
    event:AddEventLinkAtEnd("end of necklace dialogue");
    event:AddEventLinkAtEnd("necklace event: kalya goes back to party");
    EventManager:RegisterEvent(event);

    move_back_to_hero_event2 = vt_map.PathMoveSpriteEvent("necklace event: kalya goes back to party", kalya_sprite, hero, false);
    EventManager:RegisterEvent(move_back_to_hero_event2);

    event = vt_map.ScriptedEvent("end of necklace dialogue", "end_of_necklace_dialogue", "");
    EventManager:RegisterEvent(event);
end

-- zones
local to_cave_1_2_zone = nil
local to_cave_exit_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_cave_1_2_zone = vt_map.CameraZone(0, 1, 24, 28);
    Map:AddZone(to_cave_1_2_zone);

    to_cave_exit_zone = vt_map.CameraZone(24, 29, 47, 48);
    Map:AddZone(to_cave_exit_zone);
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

    -- cave entrance - start event.
    cave_entrance_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya_sprite:SetVisible(true);
        kalya_sprite:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        hero:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        Map:SetCamera(kalya_sprite, 800);

        move_next_to_hero_event:SetDestination(hero:GetXPosition() - 2.0, hero:GetYPosition(), false);
    end,

    end_of_cave_entrance_dialogue = function()
        Map:PopState();
        kalya_sprite:SetPosition(0, 0);
        kalya_sprite:SetVisible(false);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);
        hero:SetDirection(vt_map.MapMode.NORTH);

        -- Set event as done
        GlobalManager:SetEventValue("story", "kalya_speech_in_wolf_cave", 1);
    end,

    -- Kalya takes the wolfpain necklace - start event.
    wolfpain_necklace_dialogue_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya_sprite:SetVisible(true);
        kalya_sprite:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        hero:SetCollisionMask(vt_map.MapMode.ALL_COLLISION);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        Map:SetCamera(kalya_sprite, 800);

        move_next_to_hero_event2:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition(), false);
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
        kalya_sprite:SetPosition(0, 0);
        kalya_sprite:SetVisible(false);
        kalya_sprite:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);
        hero:SetDirection(vt_map.MapMode.NORTH);

        Map:PopState();
    end
}
