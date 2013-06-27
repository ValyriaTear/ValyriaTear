-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
mt_elbrus_path1_script = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Mt. Elbrus"
map_image_filename = "img/menus/locations/mt_elrus.png"
map_subname = "Low Mountain"

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "" -- TODO

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};

-- the main character handler
local hero = {};

-- Forest dialogue secondary hero
local kalya = {};
local orlinn = {};

-- Name of the main sprite. Used to reload the good one at the end of the firt forest entrance event.
local main_sprite_name = "";

-- the main map loading code
function Load(m)

    Map = m;
    ObjectManager = Map.object_supervisor;
    DialogueManager = Map.dialogue_supervisor;
    EventManager = Map.event_supervisor;

    Map.unlimited_stamina = false;

    _CreateCharacters();
    _CreateObjects();
    _CreateEnemies();

    -- Set the camera focus on hero
    Map:SetCamera(hero);
    -- This is a dungeon map, we'll use the front battle member sprite as default sprite.
    Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

    _CreateEvents();
    _CreateZones();

    -- Add clouds overlay
    Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);
    Map:GetScriptSupervisor():AddScript("dat/maps/common/at_night.lua");

end

-- the map update function handles checks done on each game tick.
function Update()
    -- Check whether the character is in one of the zones
    _CheckZones();
end

-- Character creation
function _CreateCharacters()
    -- Default hero and position
    hero = CreateSprite(Map, "Bronann", 3, 30);
    hero:SetDirection(vt_map.MapMode.EAST);
    hero:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);

    -- Load previous save point data
    local x_position = GlobalManager:GetSaveLocationX();
    local y_position = GlobalManager:GetSaveLocationY();
    if (x_position ~= 0 and y_position ~= 0) then
        -- Use the save point position, and clear the save position data for next maps
        GlobalManager:UnsetSaveLocation();
        -- Make the character look at us in that case
        hero:SetDirection(vt_map.MapMode.SOUTH);
        hero:SetPosition(x_position, y_position);
    elseif (GlobalManager:GetPreviousLocation() == "from_kalya_house_basement") then
        hero:SetDirection(vt_map.MapMode.NORTH);
        hero:SetPosition(117.0, 93.0);
    end

    Map:AddGroundObject(hero);

    -- Create secondary characters
    kalya = CreateSprite(Map, "Kalya",
                         hero:GetXPosition(), hero:GetYPosition());
    kalya:SetDirection(vt_map.MapMode.EAST);
    kalya:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    kalya:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    kalya:SetVisible(false);
    Map:AddGroundObject(kalya);

    orlinn = CreateSprite(Map, "Orlinn",
                          hero:GetXPosition(), hero:GetYPosition());
    orlinn:SetDirection(vt_map.MapMode.EAST);
    orlinn:SetMovementSpeed(vt_map.MapMode.NORMAL_SPEED);
    orlinn:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    orlinn:SetVisible(false);
    Map:AddGroundObject(orlinn);
end

-- The heal particle effect map object
local heal_effect = {};

function _CreateObjects()
    local object = {}
    local npc = {}

    Map:AddSavePoint(113, 56);

    -- Load the spring heal effect.
    heal_effect = vt_map.ParticleObject("dat/effects/particles/heal_particle.lua", 0, 0);
    heal_effect:SetObjectID(Map.object_supervisor:GenerateObjectID());
    heal_effect:Stop(); -- Don't run it until the character heals itself
    Map:AddGroundObject(heal_effect);

    -- Heal point
    npc = CreateSprite(Map, "Butterfly", 104, 56);
    npc:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
    npc:SetVisible(false);
    npc:SetName(""); -- Unset the speaker name
    Map:AddGroundObject(npc);
    dialogue = vt_map.SpriteDialogue();
    text = vt_system.Translate("Your party feels better...");
    dialogue:AddLineEvent(text, npc, "Heal event", "");
    DialogueManager:AddDialogue(dialogue);
    npc:AddDialogueReference(dialogue);
    npc = CreateObject(Map, "Layna Statue", 104, 56);
    Map:AddGroundObject(npc);

    -- Objects array
    local map_objects = {
        --  near the bridge
        { "Tree Big1", 126, 88 },
        { "Tree Big1", 127, 94 },
        { "Tree Big2", 124, 98 },
        { "Tree Big2", 110, 98 },

        -- Right border
        { "Tree Big2", 126, 62 },
        { "Tree Big2", 126, 56 },
        { "Tree Big2", 127, 49 },
        { "Tree Big2", 121, 52 },
        { "Tree Big2", 117, 50 },
        { "Tree Big2", 121, 58 },
        { "Tree Big1", 123, 60 },
        { "Tree Big2", 125, 51 },
        { "Rock2", 124, 64 },
        { "Rock2", 123, 66.5 },
        { "Rock2", 122, 69 },
        { "Rock1", 111, 45 },

        { "Tree Big2", 105, 38 },
        { "Tree Big1", 117, 37 },
        { "Tree Big2", 114, 47 },
        { "Tree Big2", 109, 48 },
        { "Tree Big1", 105, 46 },
        { "Tree Big1", 97, 46.5 },

        { "Tree Big1", 105, 54 },
        { "Tree Big1", 100, 56 },
        { "Tree Big2", 102, 61 },
        { "Tree Big1", 90, 62 },
        { "Tree Big2", 106, 60 },
        { "Tree Big1", 88, 70 },
        { "Tree Small1", 90, 76 },
        { "Tree Small2", 97, 52 },
        { "Tree Big1", 98, 65 },
        { "Tree Big1", 93, 67 },
        { "Tree Big2", 95, 71 },
        { "Tree Small1", 99, 68 },
        { "Tree Small2", 73, 73 },
        { "Tree Small1", 79, 78 },
        { "Tree Big2", 88, 46 },
        { "Tree Big1", 83, 48 },
        { "Tree Big1", 73, 46.2 },
        { "Tree Big1", 93, 49 },
        { "Tree Big1", 85, 60 },
        { "Tree Small1", 80, 58 },

        { "Tree Big1", 67, 54 },
        { "Tree Big1", 64, 57 },
        { "Tree Big1", 70, 59 },

        { "Tree Big1", 44, 89 },
        { "Tree Small1", 27, 87 },
        { "Tree Big1", 21, 83 },
        { "Tree Big1", 6, 87.2 },
        { "Tree Big1", 2, 89 },
        { "Tree Big1", 1, 92 },
        { "Tree Big1", 3, 96 },
        { "Tree Big1", 5, 98 },

        { "Tree Tiny1", 17, 75 },
        { "Tree Big2", 55, 63 },
        { "Tree Big2", 43, 92 },
        { "Tree Big2", 47, 84.5 },
        { "Tree Big2", 45, 80 },
        { "Tree Big2", 42, 77 },
        { "Tree Big2", 51, 79 },
        { "Tree Big2", 52, 73 },
        { "Tree Big2", 44, 71 },
        { "Tree Big2", 70, 51 },
        { "Tree Big2", 73.5, 49 },
        { "Tree Big2", 77, 59 },
        { "Tree Big2", 74, 57 },
        { "Tree Big2", 50, 82 },
        { "Tree Big2", 49, 75.5 },
        { "Tree Big2", 46, 50 },
        { "Tree Big2", 50, 56 },
        { "Tree Big1", 47, 53 },

        { "Rock2", 124, 31 },
        { "Rock1", 127, 30 },
        { "Rock2", 123, 39 },
        { "Rock1", 126, 31.5 },
        { "Rock1", 128, 35 },
        { "Rock1", 108, 72 },
        { "Rock2", 96, 76 },
        { "Rock1", 85, 73.5 },
        { "Rock2", 46, 94 },
        { "Rock1", 48, 96 },

        { "Rock1", 54, 45 },
        { "Rock1", 53, 48 },
        { "Rock1", 50, 47 },
        { "Rock1", 52, 58 },
        { "Rock1", 54, 59.5 },
    }

    -- Loads the trees according to the array
    for my_index, my_array in pairs(map_objects) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        Map:AddGroundObject(object);
    end

    -- grass array
    local map_grass = {
        --  right border
        { "Grass Clump1", 119, 67 },
        { "Grass Clump1", 125, 65 },
        { "Grass Clump1", 127, 68 },
        { "Grass Clump1", 122, 71 },
        { "Grass Clump1", 126, 72 },
        { "Grass Clump1", 118, 51 },
        { "Grass Clump1", 122, 47 },
        { "Grass Clump1", 112, 69 },

        { "Grass Clump1", 105.5, 47 },
        { "Grass Clump1", 125, 32.5 },
        { "Grass Clump1", 122, 31 },
        { "Grass Clump1", 103.5, 57 },
        { "Grass Clump1", 92, 69 },
        { "Grass Clump1", 99, 58 },
        { "Grass Clump1", 84, 70 },

        { "Grass Clump1", 38, 63 },
        { "Grass Clump1", 51, 49 },
        { "Grass Clump1", 53, 46 },
        { "Grass Clump1", 48, 94 },
        { "Grass Clump1", 80, 59.2 },

    }

    -- Loads the grass clumps according to the array
    for my_index, my_array in pairs(map_grass) do
        --print(my_array[1], my_array[2], my_array[3]);
        object = CreateObject(Map, my_array[1], my_array[2], my_array[3]);
        object:SetCollisionMask(vt_map.MapMode.NO_COLLISION);
        Map:AddGroundObject(object);
    end

end

function _CreateEnemies()
    local enemy = {};
    local roam_zone = {};

    -- Hint: left, right, top, bottom
    roam_zone = vt_map.EnemyZone(49, 62, 26, 39);

    -- Dark soldier 1
    enemy = CreateEnemySprite(Map, "Dark Soldier");
    _SetBattleEnvironment(enemy);
    enemy:NewEnemyParty();
    enemy:AddEnemy(9);
    roam_zone:AddEnemy(enemy, Map, 1);
    roam_zone:SetSpawnsLeft(1); -- This monster shall spawn only one time.
    Map:AddZone(roam_zone);
end

-- Special event references which destinations must be updated just before being called.
local move_next_to_hero_event = {}
local move_back_to_hero_event = {}

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
    local event = {};
    local dialogue = {};
    local text = {};

    -- After the forest dungeon
    event = vt_map.MapTransitionEvent("exit forest at night", "dat/maps/layna_village/layna_village_center_map.lua",
                                       "dat/maps/layna_village/layna_village_center_at_night_script.lua", "from_layna_forest_entrance");
    EventManager:RegisterEvent(event);

    -- Heal point
    event = vt_map.ScriptedEvent("Heal event", "heal_party", "heal_done");
    EventManager:RegisterEvent(event);

end

-- zones


-- Create the different map zones triggering events
function _CreateZones()
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()

end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
    -- default values
    enemy:SetBattleMusicTheme("mus/heroism-OGA-Edward-J-Blakeley.ogg");
    enemy:SetBattleBackground("img/backdrops/battle/mountain_background.png");
    enemy:AddBattleScript("dat/maps/common/at_night.lua");
    -- TODO: add script showing the scenery.
end

-- Map Custom functions
-- Used through scripted events

-- Effect time used when applying the heal light effect
local heal_effect_time = 0;
local heal_color = vt_video.Color(0.0, 0.0, 1.0, 1.0);

map_functions = {

    heal_party = function()
        hero:SetMoving(false);
        -- Should be sufficient to heal anybody
        GlobalManager:GetActiveParty():AddHitPoints(10000);
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

    SetCamera = function(sprite)
        Map:SetCamera(sprite, 800);
    end
}
