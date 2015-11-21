-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
sprite_test = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = ""
map_image_filename = ""
map_subname = ""

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "data/sounds/wind.ogg"

-- c++ objects instances
local Map = nil
local DialogueManager = nil
local EventManager = nil
local Script = nil

local bronann = nil
local test_image = nil

-- the main map loading code
function Load(m)

    Map = m;
    DialogueManager = Map:GetDialogueSupervisor();
    EventManager = Map:GetEventSupervisor();
    Script = Map:GetScriptSupervisor();

    Map:SetUnlimitedStamina(true)
    Map:SetRunningEnabled(false) -- Hide the stamina bar

    -- Adds a sprite
    bronann = CreateSprite(Map, "Bronann", 32, 43, vt_map.MapMode.GROUND_OBJECT);
    bronann:SetDirection(vt_map.MapMode.SOUTH);
    bronann:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    _CreateObjects();

    _CreateEvents();

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("data/visuals/ambient/clouds.png", 15.0, -5.0, true);

    -- Set the camera focus on Bronann
    Map:SetCamera(bronann);

    -- A scene map only
    Map:PushState(vt_map.MapMode.STATE_SCENE);

    EventManager:StartEvent("Bronann looks south", 3000);
end


function _CreateObjects()
    local object = nil
    local npc = nil

    -- trees, etc
    local map_trees = {
        { "Tree Small4", 8, 62 },
        { "Tree Tiny1", 81, 79 },
        { "Tree Tiny3", 81, 28 },
        { "Tree Small3", 83, 73 },
        { "Tree Small5", 75.5, 45 },
        { "Tree Small6", 3, 65 },
        { "Tree Little2", 3, 55 },
        { "Tree Little3", 6, 61 },
        { "Tree Big1", 54, 78 },
        { "Tree Small6", 2, 83 },
        { "Tree Small4", 6, 85 },
        { "Tree Small3", 10, 87 },
        { "Tree Small5", 12, 91 },
        { "Tree Little1", 15, 87 },
        { "Tree Small4", 6, 89 },
        { "Tree Small2", 3, 87 },
        { "Tree Little1", 17, 90 },
        { "Tree Small4", 16, 92 },
        { "Tree Small3", 18, 95 },
        { "Tree Small6", 22, 96 },
        { "Tree Small3", 8, 93 },
        { "Tree Small4", 3, 95 },
        { "Tree Tiny4", 1, 88 },
        { "Tree Small3", 13, 94 },
        { "Tree Small6", 10, 97 },
        { "Tree Big1", 16, 98 },
        { "Tree Small3", 0, 48 },
        { "Tree Small6", -2, 42 },
        { "Tree Small4", -1, 35 },
        { "Tree Small1", 2.5, 45 },
        { "Tree Small3", 2, 24 },
        { "Tree Small3", 84, 66 },
        { "Tree Small6", 77, 58 },
        { "Tree Big2", 12, 53 },
        { "Tree Big1", 16, 37 },
        { "Tree Small3", 35, 55 },
        { "Tree Small4", 50, 58 },
        { "Tree Small3", 87, 13 },
        { "Tree Small4", 92, 16 },
        { "Tree Small3", 95, 19 },
        { "Tree Small6", 82, 10 },


        -- North part
        { "Tree Big1", 0, 20 },
        { "Tree Small1", -1, 15 },
        { "Tree Small3", 6, 19 },
        { "Tree Little3", 10, 17 },
        { "Tree Small4", 11, 20 },
        { "Tree Small3", 15, 15 },
        { "Tree Small6", 4, 9 },
        { "Tree Small3", 1, 5 },
        { "Tree Small5", 7, 2 },
        { "Tree Tiny2", 13, 6 },
        { "Tree Small3", 19, 14 },
        { "Tree Little1", 10, 10 },
        { "Tree Small3", 19, 17 },
        { "Tree Small4", 24, 13 },
        { "Tree Tiny2", 25, 6 },
        { "Tree Small3", 30, 12 },
        { "Tree Small6", 34, 6 },
        { "Tree Little3", 40, 3 },
        { "Tree Big2", 41, 14 },
        { "Tree Small5", 46, 9 },
        { "Tree Small4", 50, 15 },
        { "Tree Little3", 4, 12 },
        { "Tree Small4", 19, 7 },
        { "Tree Small3", 35, 13 },
        { "Tree Tiny3", 40, 7 },
        { "Tree Small5", 46, 13 },
        { "Tree Little2", 52, 11 },
        { "Tree Small3", 57, 14 },
        { "Tree Small4", 56, 7 },
        { "Tree Small3", 61, 4 },
        { "Tree Small5", 67, 3 },
        { "Tree Small4", 70, 7 },
        { "Tree Small3", 72, 13 },
        { "Tree Small6", 76, 19 },
        { "Tree Small3", 64, 8 },
        { "Tree Small5", 66, 14 },
        { "Tree Small3", 62, 12 },
        { "Tree Small3", 71, 17 },
        { "Tree Small4", 81, 18 },
        { "Tree Small3", 77, 12 },
        { "Tree Small5", 74, 8.2 },
        { "Tree Small3", 86, 20 },
        { "Tree Little2", 90, 23 },
        { "Tree Tiny1", 88, 27 },
        { "Tree Small6", 92, 28 },
        { "Tree Small4", 96, 26 },
        { "Tree Little2", 100, 25 },
        { "Tree Little4", 105, 23 },
        { "Tree Small3", 103, 27.2 },
        { "Tree Small4", 101, 20 },

        -- south part
        { "Tree Little1", 37, 99 },
        { "Tree Small4", 43, 102 },
        { "Tree Small3", 51, 99 },
        { "Tree Little4", 55, 98 },
        { "Tree Tiny3", 61, 98 },
        { "Tree Small3", 68, 100 },
        { "Tree Small6", 75, 101 },
        { "Tree Tiny1", 47, 93 },

        --east part
        { "Tree Big2", 87, 31 },
        { "Tree Small3", 91, 33 },
        { "Tree Little3", 90, 37 },
        { "Tree Small4", 93, 39 },
        { "Tree Little2", 95, 42 },
        { "Tree Small6", 98, 45 },
        { "Tree Tiny1", 96, 48 },
        { "Tree Small2", 99, 51 },
        { "Tree Small4", 100, 54 },
        { "Tree Small1", 99, 57 },
        { "Tree Big1", 98, 60 },
        { "Tree Small4", 99, 63 },
        { "Tree Small3", 101, 66 },
        { "Tree Small5", 102, 69.2 },
        { "Tree Small1", 104, 72 },
        { "Tree Small6", 105, 75 },
        { "Tree Small2", 104, 79 },
        { "Tree Small5", 102, 83 },
        { "Tree Little2", 100, 86 },
        { "Tree Small4", 101, 90 },
        { "Tree Big2", 97, 93 },
        { "Tree Small5", 94, 96 },
        { "Tree Big2", 83, 92 },
        { "Tree Small4", 87, 99 },
        { "Tree Small5", 101, 94 },
        { "Tree Big1", 104, 86.2 },
        { "Tree Little2", 95, 34 },
        { "Tree Small4", 98, 38 },
        { "Tree Tiny1", 102, 44 },
        { "Tree Small6", 103, 50 },
        { "Tree Small3", 98, 31 },
        { "Tree Small3", 102, 40 },
        { "Tree Small5", 105, 55 },
        { "Tree Small6", 103, 33 },
        { "Tree Small3", 93.5, 46 },
        { "Tree Small5", 95, 55 },
        { "Tree Little2", 93, 68 },
        { "Tree Big2", 91, 50 },
        { "Tree Small3", 104, 58 },
        { "Tree Small3", 107, 62 },
        { "Tree Small3", 106, 43 },
        { "Tree Small5", 107, 37 },
        { "Tree Small2", 108, 48 },
        { "Tree Small4", 109, 56 },
        { "Tree Small3", 110, 65 },
        { "Tree Small6", 108, 69 },
        { "Tree Small2", 110, 73 },
        { "Tree Small4", 108, 76 },
        { "Tree Small6", 112, 60 },
        { "Tree Small3", 113, 68 },
        { "Tree Big1", 115, 71 },
        { "Tree Little2", 114, 75 },
        { "Tree Small3", 116, 61 },
        { "Tree Small4", 112, 78 },
        { "Tree Small1", 109, 81 },
        { "Tree Small3", 110, 85 },
        { "Tree Small3", 107, 89 },
        { "Tree Small5", 105, 93 },
        { "Tree Small3", 102, 98 },
        { "Tree Small5", 108, 99 },
        { "Tree Big2", 109, 92 },
        { "Tree Small4", 112, 89.2 },
        { "Tree Small5", 114, 86 },
        { "Tree Tiny1", 83, 23 },
        { "Tree Small2", 116, 79 },
        { "Tree Small3", 118, 76 },
        { "Tree Small3", 117, 66 },
        { "Tree Small3", 117, 85 },
        { "Tree Small4", 115, 90 },
        { "Tree Small6", 113, 93 },
        { "Tree Small3", 114, 100 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_trees) do
        --print(my_array[1], my_array[2], my_array[3]);
        CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
    end

    -- grass array
    local map_grass = {
        -- the grass, hiding a bit the snakes
        { "Grass Clump1", 52, 79 },
        { "Grass Clump1", 9, 63 },
        { "Grass Clump1", 79, 50 },
        { "Grass Clump1", 7, 68 },
        { "Grass Clump1", 5, 76 },
        { "Grass Clump1", 2, 73 },
        { "Grass Clump1", 9, 78 },
        { "Grass Clump1", 14, 80 },
        { "Grass Clump1", 18, 83 },
        { "Grass Clump1", 20, 86 },
        { "Grass Clump1", 27, 92 },
        { "Grass Clump1", 33, 95 },
        { "Grass Clump1", 22, 68 },
        { "Grass Clump1", 30, 66 },
    }

    -- Loads the grass according to the array
    for my_index, my_array in pairs(map_grass) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3], vt_map.MapMode.GROUND_OBJECT);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    end
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = nil
    local text = nil
    local dialogue = nil

    event = vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks south", bronann, vt_map.MapMode.SOUTH);
    event:AddEventLinkAtEnd("Bronann looks west", 2000);
    event = vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks west", bronann, vt_map.MapMode.WEST);
    event:AddEventLinkAtEnd("Bronann looks north", 2000);
    event = vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks north", bronann, vt_map.MapMode.NORTH);
    event:AddEventLinkAtEnd("Bronann looks east", 2000);
    event = vt_map.ChangeDirectionSpriteEvent.Create("Bronann looks east", bronann, vt_map.MapMode.EAST);
    event:AddEventLinkAtEnd("Bronann speaks", 2000);

    dialogue = vt_map.SpriteDialogue.Create();
    text = vt_system.Translate("As you wish. Follow me.");
    dialogue:AddLine(text, bronann);
    event = vt_map.DialogueEvent.Create("Bronann speaks", dialogue);
    event:AddEventLinkAtEnd("Bronann laughs", 2000);

    event = vt_map.ScriptedEvent.Create("Bronann laughs", "bronann_laughs", "");
    event:AddEventLinkAtEnd("Bronann is grayed", 2000);

    event = vt_map.ScriptedEvent.Create("Bronann is grayed", "bronann_gray", "");
    event:AddEventLinkAtEnd("Dump stack state", 2000);

    event = vt_map.ScriptedEvent.Create("Dump stack state", "dump_stack_state", "");
    event:AddEventLinkAtEnd("Random move", 2000);

    event = vt_map.RandomMoveSpriteEvent.Create("Random move", bronann, 1000, 2000);
    event:AddEventLinkAtEnd("Random move", 2000); -- Loop on itself
end

-- Map Custom functions
-- Used through scripted events

map_functions = {
    bronann_laughs = function()
        bronann:SetCustomAnimation("laughing", 700);
    end,

    bronann_gray = function()
        bronann:SetGrayscale(true);
    end,

    dump_stack_state = function()
        ScriptManager:DEBUG_DumpScriptsState()
    end,
}
