-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_village_kalya_house_path_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mountain Village of Layna"
map_image_filename = "data/story/common/locations/mountain_village.png"
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/music/Caketown_1-OGA-mat-pablo.ogg"

-- c++ objects instances
local Map = nil
local EventManager = nil

local bronann = nil

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
    -- Default: From village center
    bronann = CreateSprite(Map, "Bronann", 61, 45, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.WEST);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- set up the position according to the previous map
    if (GlobalManager:GetPreviousLocation() == "from_kalya_house_exterior") then
        bronann:SetPosition(43, 3);
        bronann:SetDirection(vt_map.MapMode.SOUTH);
    end
    if (GlobalManager:GetPreviousLocation() == "from grandma house") then
        bronann:SetPosition(12, 6);
        bronann:SetDirection(vt_map.MapMode.NORTH);
        AudioManager:PlaySound("data/sounds/door_close.wav");
    end

    if (GlobalManager:GetPreviousLocation() == "from_kalya_house_small_passage") then
        bronann:SetPosition(6, 3);
        bronann:SetDirection(vt_map.MapMode.WEST);
    end
end

local grandma = nil

function _ReloadGrandmaDialogue()
    local text = nil
    local dialogue = nil
    local event = nil

    grandma:ClearDialogueReferences();

    local chicken_left = 3;
    if (GlobalManager:GetEventValue("game", "layna_village_chicken1_found") == 1) then chicken_left = chicken_left - 1; end
    if (GlobalManager:GetEventValue("game", "layna_village_chicken2_found") == 1) then chicken_left = chicken_left - 1; end
    if (GlobalManager:GetEventValue("game", "layna_village_chicken3_found") == 1) then chicken_left = chicken_left - 1; end

    if (chicken_left > 0) then
        if (GlobalManager:GetEventValue("game", "layna_village_chicken_dialogue_done") == 1) then
            -- Tell Bronann to keep on searching
            if (chicken_left < 3) then
                dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_granma_chicken_not_found1");
                if (chicken_left == 1) then
                    text = vt_system.Translate("My three chickens have flown away again this morning. Could you find the last one for me?");
                else
                    text = vt_system.Translate("My three chickens have flown away again this morning. Could you find the remaining ones for me?");
                end
                dialogue:AddLine(text, grandma);
                grandma:AddDialogueReference(dialogue);
            else
                dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_granma_chicken_not_found2");
                text = vt_system.Translate("My three chickens have flown away again this morning. Could you find them for me?");
                dialogue:AddLine(text, grandma);
                grandma:AddDialogueReference(dialogue);
            end
        else
            -- Tell Bronann she can't find her chicken
            dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_granma_chicken_not_found");
            text = vt_system.Translate("Ah! Bronann. Could you help your old grandma?");
            dialogue:AddLine(text, grandma);
            text = vt_system.Translate("Sure grandma.");
            dialogue:AddLineEmote(text, bronann, "interrogation");
            text = vt_system.Translate("My three chickens have flown away again. Could you find them for me?");
            dialogue:AddLine(text, grandma);
            text = vt_system.Translate("I'll see what I can do about it.");
            dialogue:AddLine(text, bronann);
            text = vt_system.Translate("Thank you, young one.");
            dialogue:AddLineEvent(text, grandma, "Chicken dialogue done", "");
            grandma:AddDialogueReference(dialogue);

            vt_map.ScriptedEvent.Create("Chicken dialogue done", "set_chicken_dialogue_done", "");

        end
    elseif (GlobalManager:GetEventValue("game", "layna_village_chicken_reward_given") == 0) then
        -- Gives Bronann his reward
        dialogue = vt_map.SpriteDialogue.Create();
        text = vt_system.Translate("Oh, they're all back, my brave hero.");
        dialogue:AddLine(text, grandma);
        text = vt_system.Translate("Don't worry about it.");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("Let me give you something as a reward.");
        dialogue:AddLineEvent(text, grandma, "", "Give Bronann the chicken reward");
        text = vt_system.Translate("Oh, thanks grandma!");
        dialogue:AddLine(text, bronann);
        text = vt_system.Translate("You're very welcome, my dear one.");
        dialogue:AddLineEvent(text, grandma, "Chicken reward given", "");
        grandma:AddDialogueReference(dialogue);

        event = vt_map.TreasureEvent.Create("Give Bronann the chicken reward");
        event:AddItem(1, 2); -- Tiny healing potions
        event:SetDrunes(15);

        vt_map.ScriptedEvent.Create("Chicken reward given", "set_chicken_reward_given", "");
    else
        -- Default dialogue
        dialogue = vt_map.SpriteDialogue.Create("ep1_layna_village_granma_default");
        text = vt_system.Translate("Ah! It's nice to see your dear young face around, Bronann. Come and chat with an old grandma.");
        dialogue:AddLine(text, grandma);
        text = vt_system.Translate("Er, sorry grandma. I have to go! Maybe later?");
        dialogue:AddLineEmote(text, bronann, "exclamation");
        text = vt_system.Translate("Ah! You'll surely want to see the young lady living up there. Ah, youngins nowadays.");
        dialogue:AddLine(text, grandma);
        grandma:AddDialogueReference(dialogue);
    end
end

function _CreateNPCs()
    local npc = nil
    local event = nil

    grandma = CreateNPCSprite(Map, "Old Woman1", vt_system.Translate("Grandma"), 7, 25, vt_map.MapMode.GROUND_OBJECT);
    grandma:SetDirection(vt_map.MapMode.SOUTH);

    _ReloadGrandmaDialogue();

    -- Adds the chicken when found.
    if (GlobalManager:GetEventValue("game", "layna_village_chicken1_found") == 1) then
        npc = CreateSprite(Map, "Chicken", 21, 36, vt_map.MapMode.GROUND_OBJECT);
        event = vt_map.RandomMoveSpriteEvent.Create("Chicken1 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Chicken1 random move", 4500); -- Loop on itself
        EventManager:StartEvent("Chicken1 random move");
    end

    if (GlobalManager:GetEventValue("game", "layna_village_chicken2_found") == 1) then
        npc = CreateSprite(Map, "Chicken", 19, 34, vt_map.MapMode.GROUND_OBJECT);
        event = vt_map.RandomMoveSpriteEvent.Create("Chicken2 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Chicken2 random move", 4500); -- Loop on itself
        EventManager:StartEvent("Chicken2 random move", 1200);
    end

    if (GlobalManager:GetEventValue("game", "layna_village_chicken3_found") == 1) then
        npc = CreateSprite(Map, "Chicken", 23, 33, vt_map.MapMode.GROUND_OBJECT);
        event = vt_map.RandomMoveSpriteEvent.Create("Chicken3 random move", npc, 1000, 1000);
        event:AddEventLinkAtEnd("Chicken3 random move", 4500); -- Loop on itself
        EventManager:StartEvent("Chicken3 random move", 2100);
    end
end

function _CreateObjects()
    -- Left tree "wall"
    CreateObject(Map, "Tree Big1", 0, 44, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 42, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 40, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 36, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 0, 30, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 28, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 26, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big2", 0, 22, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 14, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 12, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock2", 0, 10, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 0, 7, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 0, 5, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 0, 3, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 15, 3, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 15.5, 1.5, vt_map.MapMode.GROUND_OBJECT);

    -- Right tree "Wall"
    CreateObject(Map, "Rock2", 63, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 63, 36, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 63, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 63, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 63, 30, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small2", 63.5, 28, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 63, 26, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 63, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 63, 22, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 63, 20, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 63, 18, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small2", 64.5, 16, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Rock1", 63, 12, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small2", 64, 10, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 63, 8, vt_map.MapMode.GROUND_OBJECT);

    -- Secret shortcut hiders
    CreateObject(Map, "Tree Big1", 38, 40, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 40, 42, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Big1", 42, 40, vt_map.MapMode.GROUND_OBJECT);

    -- Cliff hiders
    CreateObject(Map, "Tree Small1", 14, 30, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Tree Small1", 55, 12, vt_map.MapMode.GROUND_OBJECT);

    -- Fence
    CreateObject(Map, "Fence1 l top left", 17, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 19, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 21, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 23, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 25, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 27, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 l top right", 29, 32, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 vertical", 17, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 vertical", 17, 36, vt_map.MapMode.GROUND_OBJECT);

    CreateObject(Map, "Fence1 l bottom left", 17, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 19, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 21, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 23, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 25, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 horizontal", 27, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 l bottom right", 29, 38, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 vertical", 29, 34, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Fence1 vertical", 29, 36, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Bench1", 7, 24, vt_map.MapMode.GROUND_OBJECT);
    CreateObject(Map, "Barrel1", 20, 24, vt_map.MapMode.GROUND_OBJECT);

    -- Secret treasure chest
    local chest = CreateTreasure(Map, "kalya_house_path_chest", "Wood_Chest1", 8, 8, vt_map.MapMode.GROUND_OBJECT);
    chest:AddItem(1001, 1);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    -- Triggered Events
    vt_map.MapTransitionEvent.Create("to Village center", "data/story/layna_village/layna_village_center_map.lua",
                                     "data/story/layna_village/layna_village_center_script.lua", "from_kalya_house_path");

    vt_map.MapTransitionEvent.Create("to Kalya house exterior", "data/story/layna_village/layna_village_kalya_house_exterior_map.lua",
                                     "data/story/layna_village/layna_village_kalya_house_exterior_script.lua", "from_kalya_house_path");

    vt_map.MapTransitionEvent.Create("to grandma house", "data/story/layna_village/layna_village_kalya_house_path_small_house_map.lua",
                                     "data/story/layna_village/layna_village_kalya_house_path_small_house_script.lua", "from_kalya_house_path");

    vt_map.MapTransitionEvent.Create("to Kalya house small passage", "data/story/layna_village/layna_village_kalya_house_exterior_map.lua",
                                     "data/story/layna_village/layna_village_kalya_house_exterior_script.lua", "from_kalya_house_path_small_passage");
end

-- zones
local village_center_zone = nil
local kalya_house_exterior_zone = nil
local grandma_house_entrance_zone = nil
local kalya_house_small_passage_zone = nil

function _CreateZones()
    -- N.B.: left, right, top, bottom
    village_center_zone = vt_map.CameraZone.Create(62, 63, 42, 47);
    kalya_house_exterior_zone = vt_map.CameraZone.Create(26, 56, 0, 2);
    grandma_house_entrance_zone = vt_map.CameraZone.Create(11, 13, 7, 8);
    kalya_house_small_passage_zone = vt_map.CameraZone.Create(3, 8, 0, 1);
end

function _CheckZones()
    if (village_center_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        EventManager:StartEvent("to Village center");
    end

    if (kalya_house_exterior_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        EventManager:StartEvent("to Kalya house exterior");
    end

    if (grandma_house_entrance_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        EventManager:StartEvent("to grandma house");
        AudioManager:PlaySound("data/sounds/door_open2.wav");
    end

    if (kalya_house_small_passage_zone:IsCameraEntering() == true) then
        -- Stop the character as it may walk in diagonal, which is looking strange
        -- when entering
        bronann:SetMoving(false);
        EventManager:StartEvent("to Kalya house small passage");
    end
end

-- Map Custom functions
map_functions = {

    set_chicken_dialogue_done = function()
        GlobalManager:SetEventValue("game", "layna_village_chicken_dialogue_done", 1);
        _ReloadGrandmaDialogue();
        -- Adds the quest log about the chicken...
        GlobalManager:AddQuestLog("catch_chicken");
    end,

    set_chicken_reward_given = function()
        GlobalManager:SetEventValue("game", "layna_village_chicken_reward_given", 1);
        _ReloadGrandmaDialogue();
    end
}
