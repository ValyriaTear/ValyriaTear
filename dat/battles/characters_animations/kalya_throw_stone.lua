-- Filename: thanis_attack.lua
-- This file is scripting Bronann's attack animation, called by attack skills.
-- The initialize() function is called once, followed by calls to the update function.
-- When the update function returns true, the attack is finished.

-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
kalya_throw_stone = ns;
setfenv(1, ns);

-- local references
local character = {};
local target = {};
local target_actor = {};
local skill = {};

local stone_pos_x = 0.0;
local stone_pos_y = 0.0;

local a_coeff = 0.0;
local distance_moved_x = 0.0;
local distance_moved_y = 0.0;

local enemy_pos_x = 0.0;
local enemy_pos_y = 0.0;

local attack_step = 0;
local attack_time = 0.0;

local damage_triggered = false;

-- character, the BattleActor attacking (here Kalya)
-- target, the BattleEnemy target
-- The skill id used on target
function Initialize(_character, _target, _skill)
    -- Keep the reference in memory
    character = _character;
    target = _target;
    target_actor = _target:GetActor();
    skill = _skill;

    -- Don't attack if the character isn't alive
    if (character:IsAlive() == false) then
        return;
    end

    -- Set the arrow starting position
    stone_pos_x = character:GetXLocation() + character:GetSpriteWidth() / 2.0;
    stone_pos_y = character:GetYLocation() - (character:GetSpriteHeight() / 2.0) - 5.0;
    character:SetAmmoPosition(stone_pos_x, stone_pos_y);

    -- Make the arrow reach the enemy center
    enemy_pos_x = target_actor:GetXLocation();
    enemy_pos_y = target_actor:GetYLocation() - target_actor:GetSpriteHeight() / 2.0;

    attack_step = 0;
    attack_time = 0;

    damage_triggered = false;

    distance_moved_x = SystemManager:GetUpdateTime() / hoa_map.MapMode.NORMAL_SPEED * 210.0;
    local y_diff = stone_pos_y - enemy_pos_y;
    if (y_diff == 0.0) then
        a_coeff = 0.0;
        distance_moved_y = 0.0;
    else
        a_coeff = (enemy_pos_x - stone_pos_x) / (stone_pos_y - enemy_pos_y);
        if (a_coeff < 0) then a_coeff = -a_coeff; end
        distance_moved_y = (1/a_coeff) * distance_moved_x;
    end

    --print("distance x: ", enemy_pos_x - character_pos_x - 64.0)
    --print("distance y: ", character_pos_y - enemy_pos_y)
    --print (distance_moved_x, 1/a_coeff, distance_moved_y);

    -- Override the ammo as a stone.
    character:GetAmmo():LoadAmmoAnimatedImage("img/sprites/battle/ammo/rock_ammo.lua");
end


function Update()
    -- The update time can vary, so update the distance on each update as well.
    distance_moved_x = SystemManager:GetUpdateTime() / hoa_map.MapMode.NORMAL_SPEED * 210.0;
    if (a_coeff ~= 0.0) then
        distance_moved_y = (1/a_coeff) * distance_moved_x;
    end
    
    -- Attack the enemy
    if (attack_step == 0) then
        character:ChangeSpriteAnimation("throw_stone")
        attack_step = 1
    end
    -- Make the character go back to idle once attacked
    if (attack_step == 1) then
        attack_time = attack_time + SystemManager:GetUpdateTime();
        if (attack_time > 700.0) then
            attack_step = 2;
            AudioManager:PlaySound("snd/throw.wav");
            character:SetShowAmmo(true);
        end
    end

    -- Triggers the arrow animation
    if (attack_step == 2) then
        if (stone_pos_x > enemy_pos_x) then
            stone_pos_x = stone_pos_x - distance_moved_x;
            if stone_pos_x < enemy_pos_x then stone_pos_x = enemy_pos_x end
        end
        if (stone_pos_x < enemy_pos_x) then
            stone_pos_x = stone_pos_x + distance_moved_x;
            if stone_pos_x > enemy_pos_x then stone_pos_x = enemy_pos_x end
        end
        if (stone_pos_y > enemy_pos_y) then
            stone_pos_y = stone_pos_y - distance_moved_y;
            if stone_pos_y < enemy_pos_y then stone_pos_y = enemy_pos_y end
        end
        if (stone_pos_y < enemy_pos_y) then
            stone_pos_y = stone_pos_y + distance_moved_y;
            if stone_pos_y > enemy_pos_y then stone_pos_y = enemy_pos_y end
        end

        character:SetAmmoPosition(stone_pos_x, stone_pos_y);

        if (stone_pos_x >= enemy_pos_x and stone_pos_y == enemy_pos_y) then
            character:ChangeSpriteAnimation("idle");
            attack_step = 3;
        end
    end

    if (attack_step == 3) then
        -- Triggers the damage once the arrow has reached the enemy
        if (damage_triggered == false) then
            skill:ExecuteBattleFunction(character, target);
            -- Remove the skill points at the end of the third attack
            character:SubtractSkillPoints(skill:GetSPRequired());
            damage_triggered = true;
            character:SetShowAmmo(false);
        end
        attack_step = 4
    end

    if (attack_step == 4) then
        return true;
    end

    return false;
end
