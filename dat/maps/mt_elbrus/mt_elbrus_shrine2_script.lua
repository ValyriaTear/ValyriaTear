-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_shrine2_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus Shrine"
map_image_filename = "img/menus/locations/mountain_shrine.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/mountain_shrine.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil
local Script = nil

-- the main character handler
local hero = nil

-- Dialogue secondary heroes
local bronann = nil
local kalya = nil
local orlinn = nil

-- the main map loading code
function Load(m)

    Map = m;
    Script = Map:GetScriptSupervisor();
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

    -- Amplify light gradually when entering the first time.
    if (GlobalManager:GetEventValue("story", "mountain_shrine_entrance_light_done") == 0) then
        Map:GetEffectSupervisor():EnableLightingOverlay(vt_video.Color(0.0, 0.0, 0.0, 0.45));
        EventManager:StartEvent("Amplify lights", 1500);
        GlobalManager:SetEventValue("story", "mountain_shrine_entrance_light_done", 1);
    end
end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position (from shrine entrance)
    hero = CreateSprite(Map, "Bronann", 33.5, 76.5, vt_map.MapMode.GROUND_OBJECT);
    hero:SetDirection(vt_map.MapMode.NORTH);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    if (GlobalManager:GetPreviousLocation() == "from_shrine_stairs1") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(50.0, 4.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_trap_room") then
        hero:SetDirection(vt_map.MapMode.WEST);
        hero:SetPosition(58.0, 60.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_enigma_room") then
        hero:SetDirection(vt_map.MapMode.EAST);
        hero:SetPosition(4.0, 60.0);
    elseif (GlobalManager:GetPreviousLocation() == "from_shrine_first_floor") then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(14.0, 6.0);
    end

    bronann = CreateSprite(Map, "Bronann", 0, 0, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.EAST);
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
end

-- The parchment object, used as first event.
local parchment1 = nil

-- Skeletons preventing the heroes from going upstairs.
local skeleton1 = nil
local skeleton2 = nil
local skeleton3 = nil

function _CreateObjects()
    local object = nil

    object = CreateTreasure(Map, "Elbrus_Shrine_entrance1", "Jar1", 59, 16, vt_map.MapMode.GROUND_OBJECT);
    object:AddItem(15, 1); -- Lotus Petal (Regen)
    object = CreateTreasure(Map, "Elbrus_Shrine_entrance2", "Jar1", 20, 46.6, vt_map.MapMode.GROUND_OBJECT);
    object:AddItem(1004, 1); -- Periwinkle potion (Strength)

    vt_map.Halo.Create("img/misc/lights/torch_light_mask.lua", 33.5, 90,
        vt_video.Color(1.0, 1.0, 1.0, 0.8));

    _add_flame(19.5, 2);
    _add_flame(9.5, 2);

    _add_flame_pot(40, 40.6);
    _add_flame_pot(42, 36.6);
    _add_flame_pot(26, 40.6);
    _add_flame_pot(24, 36.6);
    _add_flame_pot(22, 20.6);
    _add_flame_pot(22, 16.6);
    _add_flame_pot(22, 12.6);
    _add_flame_pot(22, 8.6);
    _add_flame_pot(20, 6.6);

    CreateObject(Map, "Vase2", 38, 42, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Vase4", 28, 43, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Vase3", 44, 32.6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Vase3", 43.9, 30.6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Vase3", 44.1, 28.6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Jar1", 30, 40.6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Jar1", 36, 40.6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Vase2", 22, 32.6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Jar1", 21.9, 30.6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Jar1", 22.1, 28.6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Jar1", 20, 58.6, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Jar1", 46, 45, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Jar1", 46, 51.6, vt_map.MapMode.GROUND_OBJECT);

    -- Waterfalls
    if (GlobalManager:GetEventValue("triggers", "mt elbrus waterfall trigger") == 1) then
        _add_very_small_waterfall(32, 7);
        _add_very_small_waterfall(40, 7);
        _add_waterlight(27, 12)
        _add_waterlight(36, 18)
        _add_waterlight(31, 26)
        _add_waterlight(36, 32)
        _add_waterlight(55, 15)
    else
        _add_bubble(30, 30);
        _add_bubble(35, 35);
        _add_bubble(42, 20);
        _add_bubble(28, 12);
        _add_bubble(32, 18);
        _add_bubble(55, 17);

        -- The poisonous scent
        Map:GetScriptSupervisor():AddScript("dat/maps/mt_elbrus/mt_elbrus_scent_anim.lua");
    end

    -- Add the first parchment
    parchment1 = CreateObject(Map, "Parchment", 33.0, 40.6, vt_map.MapMode.GROUND_OBJECT);
    -- Adds a dialogue about the parchment content.
    if (GlobalManager:GetEventValue("story", "mountain_shrine_parchment1_done") == 0) then
        parchment1:SetEventWhenTalking("Parchment 1 event");
    else
        -- A smaller event summarizing the dialogue
        parchment1:SetEventWhenTalking("Parchment 1 event small");
    end

    -- The skeleton triggering the access to upstairs and the arrival of enemies.
    skeleton1 = CreateSprite(Map, "Skeleton", 13, 5, vt_map.MapMode.GROUND_OBJECT);
    skeleton2 = CreateSprite(Map, "Skeleton", 15, 5, vt_map.MapMode.GROUND_OBJECT);
    skeleton3 = CreateSprite(Map, "Skeleton", 14, 7, vt_map.MapMode.GROUND_OBJECT);
    skeleton1:SetDirection(vt_map.MapMode.SOUTH);
    skeleton2:SetDirection(vt_map.MapMode.SOUTH);
    skeleton3:SetDirection(vt_map.MapMode.SOUTH);
    skeleton1:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    skeleton2:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    skeleton3:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

    -- Adds a dialogue about the parchment content.
    if (GlobalManager:GetEventValue("story", "mountain_shrine_skeleton_event_done") == 1) then
        skeleton1:SetPosition(0, 0);
        skeleton1:SetVisible(false);
        skeleton2:SetPosition(0, 0);
        skeleton2:SetVisible(false);
        skeleton3:SetPosition(0, 0);
        skeleton3:SetVisible(false);
        -- Just adds enemies in this case
        _CreateEnemies();
    end

    -- Create the fake walls out of the objects catalog as they are used once,
    -- Left secret passage
    if (GlobalManager:GetEventValue("triggers", "mt elbrus shrine 2nd s2 trigger") == 0) then
        object = vt_map.PhysicalObject.Create(vt_map.MapMode.GROUND_OBJECT);
        object:SetPosition(2.0, 62.0);
        object:SetCollPixelHalfWidth(32);
        object:SetCollPixelHeight(12.0 * 16);
        object:SetImgPixelHalfWidth(2.0 * 16);
        object:SetImgPixelHeight(12.0 * 16);
        object:AddStillFrame("dat/maps/mt_elbrus/fake_wall.png");
    else
        _add_flame(1.5, 53);
    end

    -- Right one
    if (GlobalManager:GetEventValue("triggers", "mt elbrus shrine 4 trigger 1") == 0) then
        object = vt_map.PhysicalObject.Create(vt_map.MapMode.GROUND_OBJECT);
        object:SetPosition(62.0, 62.0);
        object:SetCollPixelHalfWidth(2.0 * 16);
        object:SetCollPixelHeight(12.0 * 16);
        object:SetImgPixelHalfWidth(2.0 * 16);
        object:SetImgPixelHeight(12.0 * 16);
        object:AddStillFrame("dat/maps/mt_elbrus/fake_wall.png");
    else
        _add_flame(63.5, 53);
    end
end

function _add_very_small_waterfall(x, y)
    local object = CreateObject(Map, "Waterfall3", x - 0.1, y - 0.2, vt_map.MapMode.GROUND_OBJECT);
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    object:RandomizeCurrentAnimationFrame();
    -- Ambient sound
    object = vt_map.SoundObject.Create("snd/fountain_large.ogg", x, y - 5, 50.0);
    object:SetMaxVolume(0.6);

    -- Particle effects
    object = vt_map.ParticleObject.Create("dat/effects/particles/waterfall_steam.lua", x, y - 4.0, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true);

    object = vt_map.ParticleObject.Create("dat/effects/particles/waterfall_steam_big.lua", x, y + 1.0, vt_map.MapMode.GROUND_OBJECT);
    object:SetDrawOnSecondPass(true);
end

function _add_waterlight(x, y)
    local object = CreateObject(Map, "Water Light1", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();
    object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
end

function _add_bubble(x, y)
    local object = CreateObject(Map, "Bubble", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();

    vt_map.ParticleObject.Create("dat/effects/particles/bubble_steam.lua", x, y, vt_map.MapMode.GROUND_OBJECT);
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

function _add_flame_pot(x, y)
    vt_map.SoundObject.Create("snd/campfire.ogg", x, y, 10.0);

    local object = CreateObject(Map, "Flame Pot1", x, y, vt_map.MapMode.GROUND_OBJECT);
    object:RandomizeCurrentAnimationFrame();

    vt_map.Halo.Create("img/misc/lights/torch_light_mask2.lua", x, y + 3.0,
        vt_video.Color(0.85, 0.32, 0.0, 0.6));
    vt_map.Halo.Create("img/misc/lights/sun_flare_light_main.lua", x, y + 2.0,
        vt_video.Color(0.99, 1.0, 0.27, 0.1));
end

function _CreateEnemies()
    local enemy = nil
    local roam_zone = nil

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone.Create(6, 19, 10, 49);

    enemy = CreateEnemySprite(Map, "Skeleton");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(19);
    enemy:AddEnemy(19);
    enemy:AddEnemy(19);
    roam_zone:AddEnemy(enemy, 2);
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_shrine.png");
    enemy:AddBattleScript("dat/battles/mountain_shrine_battle_anim.lua");
end
function _SetEventBattleEnvironment(event)
    event:SetMusic("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    event:SetBackground("img/backdrops/battle/mountain_shrine.png");
    event:AddScript("dat/battles/mountain_shrine_battle_anim.lua");
end

-- Special events
local bronann_move_next_to_kalya_event = nil
local orlinn_move_next_to_kalya_event = nil

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local dialogue = nil
    local text = nil

    vt_map.MapTransitionEvent.Create("to mountain shrine entrance", "dat/maps/mt_elbrus/mt_elbrus_shrine1_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine1_script.lua", "from_shrine_main_room");

    vt_map.MapTransitionEvent.Create("to mountain shrine trap room", "dat/maps/mt_elbrus/mt_elbrus_shrine3_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine3_script.lua", "from_shrine_main_room");

    vt_map.MapTransitionEvent.Create("to mountain shrine enigma room", "dat/maps/mt_elbrus/mt_elbrus_shrine4_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine4_script.lua", "from_shrine_main_room");

    vt_map.MapTransitionEvent.Create("to mountain shrine first floor", "dat/maps/mt_elbrus/mt_elbrus_shrine5_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine5_script.lua", "from_shrine_main_room");

    vt_map.MapTransitionEvent.Create("to mountain shrine stairs", "dat/maps/mt_elbrus/mt_elbrus_shrine9_map.lua",
                                     "dat/maps/mt_elbrus/mt_elbrus_shrine9_script.lua", "from_shrine_main_room");

    -- Generic events
    vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks north", bronann, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Kalya looks north", kalya, vt_map.MapMode.NORTH);
    vt_map.ChangeDirectionSpriteEvent.Create("Orlinn looks north", orlinn, vt_map.MapMode.NORTH);

    vt_map.LookAtSpriteEvent.Create("Bronann looks at Kalya", bronann, kalya);
    vt_map.LookAtSpriteEvent.Create("Orlinn looks at Kalya", orlinn, kalya);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Orlinn", kalya, orlinn);
    vt_map.LookAtSpriteEvent.Create("Kalya looks at Bronann", kalya, bronann);

    -- Improve lighting at first entrance
    vt_map.ScriptedEvent.Create("Amplify lights", "amplify_light_start", "amplify_light_update");

    -- Event when reading the shrine first parchment.
    event = vt_map.ScriptedEvent.Create("Parchment 1 event", "parchment1_event_start", "");
    event:AddEventLinkAtEnd("Bronann moves next to Kalya");
    event:AddEventLinkAtEnd("Orlinn moves next to Kalya");

    -- NOTE: The actual destination is set just before the actual start call
    bronann_move_next_to_kalya_event = vt_map.PathMoveSpriteEvent.Create("Bronann moves next to Kalya", bronann, 0, 0, false);
    bronann_move_next_to_kalya_event:AddEventLinkAtEnd("Bronann looks north");
    bronann_move_next_to_kalya_event:AddEventLinkAtEnd("Kalya reads the parchment 1");

    orlinn_move_next_to_kalya_event = vt_map.PathMoveSpriteEvent.Create("Orlinn moves next to Kalya", orlinn, 0, 0, false);
    orlinn_move_next_to_kalya_event:AddEventLinkAtEnd("Orlinn looks north");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("It seems somebody left a note here on purpose.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("You mean in the middle of those scary bones?");
    dialogue:AddLineEventEmote(text, orlinn, "Orlinn looks at Kalya", "", "sweat drop");
    text = vt_system.Translate("Let me see.");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("'To whoever will read this:'");
    dialogue:AddLineEvent(text, kalya, "Bronann looks at Kalya", "");
    text = vt_system.Translate("'Battles have been waging all over the place and the Ancient have now forsaken me into oblivion when they sealed the Goddess Shrine.'");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("'All my comrades have perished in battle and I am the last unfortunate one left.'");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("'My name is Shawn, and I do believe my time is running out in this life as there is neither food nor water in here.'");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("Poor guy. He must have died from starvation.");
    dialogue:AddLineEmote(text, bronann, "exclamation");
    text = vt_system.Translate("'But I will attempt one last time to escape from here and I'm leaving this note in case you'd need to do the same.'");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("'The only way to escape this hellish place is to reach the highest floor and activate the waterfall. At least that's what they said.'");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("What are we waiting for, let's go!");
    dialogue:AddLineEmote(text, orlinn, "exclamation");
    text = vt_system.Translate("Wait Orlinn! That's not all he wrote.");
    dialogue:AddLineEvent(text, kalya, "Kalya looks at Orlinn", "Kalya looks north");
    text = vt_system.Translate("'But be warned, for even the very scent exuding from the dark waters is a trap!'");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("'The entire shrine has been turned into a murderous area and you'll have to fight your way through, just as I will now.'");
    dialogue:AddLine(text, kalya);
    text = vt_system.Translate("That's it.");
    dialogue:AddLineEventEmote(text, kalya, "Kalya looks at Bronann", "", "thinking dots");
    text = vt_system.Translate("Let's find out how to activate that waterfall, then.");
    dialogue:AddLine(text, bronann);
    text = vt_system.Translate("Yes, and let's do it quickly.");
    dialogue:AddLine(text, kalya);
    event = vt_map.DialogueEvent.Create("Kalya reads the parchment 1", dialogue);
    event:AddEventLinkAtEnd("Orlinn goes back to party");
    event:AddEventLinkAtEnd("Bronann goes back to party");

    event = vt_map.PathMoveSpriteEvent.Create("Orlinn goes back to party", orlinn, kalya, false);
    event:AddEventLinkAtEnd("Parchment 1 event end");

    vt_map.PathMoveSpriteEvent.Create("Bronann goes back to party", bronann, kalya, false);

    vt_map.ScriptedEvent.Create("Parchment 1 event end", "parchment1_event_end", "");

    -- A small event, summarizing the whole one
    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Somebody left a note on purpose here. According to it, we need to find out a way to trigger the waterfall from the highest floor.");
    dialogue:AddLine(text, hero);
    text = vt_system.Translate("But let's be careful, as it also said this place is dangerous and the dark waters are some kind of trap.");
    dialogue:AddLine(text, hero);
    vt_map.DialogueEvent.Create("Parchment 1 event small", dialogue);

    -- The event when skeletons wake up
    event = vt_map.ScriptedEvent.Create("Skeleton event start", "skeleton_event_start", "");
    event:AddEventLinkAtEnd("Skeleton dialogue");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("What is this? Skeletons?!");
    dialogue:AddLineEmote(text, hero, "exclamation");
    event = vt_map.DialogueEvent.Create("Skeleton dialogue", dialogue);
    event:AddEventLinkAtEnd("Skeleton1 goes to hero");
    event:AddEventLinkAtEnd("Skeleton2 goes to hero");
    event:AddEventLinkAtEnd("Skeleton3 goes to hero");

    event = vt_map.PathMoveSpriteEvent.Create("Skeleton1 goes to hero", skeleton1, hero, true);
    event:AddEventLinkAtEnd("Skeleton battle");

    vt_map.PathMoveSpriteEvent.Create("Skeleton2 goes to hero", skeleton2, hero, true);
    vt_map.PathMoveSpriteEvent.Create("Skeleton3 goes to hero", skeleton3, hero, true);

    event = vt_map.BattleEncounterEvent.Create("Skeleton battle");
    event:AddEnemy(19); -- three skeletons
    event:AddEnemy(19);
    event:AddEnemy(19);
    _SetEventBattleEnvironment(event);
    event:AddEventLinkAtEnd("Make skeletons invisible");

    event = vt_map.ScriptedEvent.Create("Make skeletons invisible", "make_skeleton_invisible", "");
    event:AddEventLinkAtEnd("Skeleton dialogue 2");

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("Phew.");
    dialogue:AddLineEmote(text, hero, "exclamation");
    event = vt_map.DialogueEvent.Create("Skeleton dialogue 2", dialogue);
    event:AddEventLinkAtEnd("Skeleton event end");

    vt_map.ScriptedEvent.Create("Skeleton event end", "skeleton_event_end", "");
end

-- zones
local to_shrine_entrance_zone = nil
local to_shrine_trap_room_zone = nil
local to_shrine_enigma_room_zone = nil
local to_shrine_stairs_room_zone = nil
local shrine_skeleton_trap_zone = nil

-- Create the different map zones triggering events
function _CreateZones()
    -- N.B.: left, right, top, bottom
    to_shrine_entrance_zone = vt_map.CameraZone.Create(26, 40, 78, 80);
    to_shrine_trap_room_zone = vt_map.CameraZone.Create(62, 64, 56, 62);
    to_shrine_enigma_room_zone = vt_map.CameraZone.Create(0, 2, 56, 62);
    to_shrine_first_floor_zone = vt_map.CameraZone.Create(12, 16, 0, 2);
    to_shrine_stairs_room_zone = vt_map.CameraZone.Create(46, 54, 0, 2);
    shrine_skeleton_trap_zone = vt_map.CameraZone.Create(4, 24, 10, 12);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
    if (to_shrine_entrance_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.SOUTH);
        EventManager:StartEvent("to mountain shrine entrance");
    elseif (to_shrine_trap_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.EAST);
        EventManager:StartEvent("to mountain shrine trap room");
    elseif (to_shrine_enigma_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.WEST);
        EventManager:StartEvent("to mountain shrine enigma room");
    elseif (to_shrine_first_floor_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine first floor");
    elseif (to_shrine_stairs_room_zone:IsCameraEntering() == true) then
        hero:SetDirection(vt_map.MapMode.NORTH);
        EventManager:StartEvent("to mountain shrine stairs");
    elseif (shrine_skeleton_trap_zone:IsCameraEntering() == true and Map:CurrentState() == vt_map.MapMode.STATE_EXPLORE) then
        if (GlobalManager:GetEventValue("story", "mountain_shrine_skeleton_event_done") == 0) then
            EventManager:StartEvent("Skeleton event start");
        end
    end
end

-- Used to display a fading darkness
local light_alpha = 0.45;
local light_color = vt_video.Color(0.0, 0.0, 0.0, 0.45);

-- Map Custom functions
-- Used through scripted events
map_functions = {
    amplify_light_start = function()
        light_alpha = 0.45;
        light_color:SetAlpha(light_alpha);
    end,

    amplify_light_update = function()
        light_alpha = light_alpha - (0.0002 * SystemManager:GetUpdateTime());
        if (light_alpha <= 0.0) then
            Map:GetEffectSupervisor():DisableLightingOverlay();
            return true;
        end
        light_color:SetAlpha(light_alpha);
        Map:GetEffectSupervisor():EnableLightingOverlay(light_color);
        return false;
    end,

    parchment1_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        kalya:SetPosition(hero:GetXPosition(), hero:GetYPosition())
        kalya:SetDirection(hero:GetDirection())
        kalya:SetVisible(true)
        hero:SetVisible(false)
        Map:SetCamera(kalya)
        hero:SetPosition(0, 0)

        bronann:SetPosition(kalya:GetXPosition(), kalya:GetYPosition());
        bronann:SetVisible(true);
        orlinn:SetPosition(kalya:GetXPosition(), kalya:GetYPosition());
        orlinn:SetVisible(true);
        bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        kalya:SetDirection(vt_map.MapMode.NORTH);

        bronann_move_next_to_kalya_event:SetDestination(kalya:GetXPosition() + 2.0, kalya:GetYPosition(), false);
        orlinn_move_next_to_kalya_event:SetDestination(kalya:GetXPosition() - 2.0, kalya:GetYPosition(), false);
    end,

    parchment1_event_end = function()
        Map:PopState();
        bronann:SetPosition(0, 0);
        bronann:SetVisible(false);
        bronann:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        orlinn:SetPosition(0, 0);
        orlinn:SetVisible(false);
        orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);

        hero:SetPosition(kalya:GetXPosition(), kalya:GetYPosition())
        hero:SetDirection(kalya:GetDirection())
        hero:SetVisible(true)
        kalya:SetVisible(false)
        Map:SetCamera(hero)
        kalya:SetPosition(0, 0)

        -- Set event as done
        GlobalManager:SetEventValue("story", "mountain_shrine_parchment1_done", 1);

        -- Set a smaller event now the full dialogue has been run
        parchment1:SetEventWhenTalking("Parchment 1 event small");
    end,

    skeleton_event_start = function()
        Map:PushState(vt_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
    end,

    make_skeleton_invisible = function()
        skeleton1:SetPosition(0, 0);
        skeleton1:SetVisible(false);
        skeleton2:SetPosition(0, 0);
        skeleton2:SetVisible(false);
        skeleton3:SetPosition(0, 0);
        skeleton3:SetVisible(false);
    end,

    skeleton_event_end = function()
        GlobalManager:SetEventValue("story", "mountain_shrine_skeleton_event_done", 1);
        Map:PopState();
        -- Now adds the enemies
        _CreateEnemies();
    end,
}
