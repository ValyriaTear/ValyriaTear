-- Filename: thanis_attack.lua
-- This file is scripting Bronann's attack animation, called by attack skills.
-- The initialize() function is called once, followed by calls to the update function.
-- When the update function returns true, the attack is finished.

-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
kalya_attack = ns;
setfenv(1, ns);

-- local references
local character = nil
local target = nil
local target_actor = nil
local skill = nil

local arrow_pos_x = 0.0;
local arrow_pos_y = 0.0;

local arrow_height = 0.0;
local total_distance = 0.0;
local height_diff = 0.0;
local height_min = 0.0;

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

    -- Set the arrow flying height members
    arrow_height = (character:GetSpriteHeight() / 2.0) + 5.0;
    total_distance = math.abs(target_actor:GetXLocation() - character:GetXLocation());
    height_diff = arrow_height - (target_actor:GetSpriteHeight() / 2.0);
    height_min = math.min(arrow_height, (target_actor:GetSpriteHeight() / 2.0));

    -- Set the arrow starting position
    arrow_pos_x = character:GetXLocation() + character:GetSpriteWidth() / 2.0;
    arrow_pos_y = character:GetYLocation() - arrow_height;

    -- Make the arrow reach the enemy center
    enemy_pos_x = target_actor:GetXLocation();
    enemy_pos_y = target_actor:GetYLocation() - target_actor:GetSpriteHeight() / 2.0;

    attack_step = 0;
    attack_time = 0;

    damage_triggered = false;

    distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 210.0;
    local x_diff = enemy_pos_x - arrow_pos_x;
    local y_diff = arrow_pos_y - enemy_pos_y;
    if (y_diff == 0.0) then
        a_coeff = 0.0;
        distance_moved_y = 0.0;
    elseif (x_diff == 0.0) then
        a_coeff = 0.0;
        distance_moved_y = distance_moved_x;
        distance_moved_x = 0.0;
    else
        a_coeff =  y_diff / x_diff;
        if (a_coeff < 0) then a_coeff = -a_coeff; end
        distance_moved_y = a_coeff * distance_moved_x;
    end

    --print("distance x: ", enemy_pos_x - character_pos_x)
    --print("distance y: ", character_pos_y - enemy_pos_y)
    --print (distance_moved_x, a_coeff, distance_moved_y);
end


function Update()
    -- The update time can vary, so update the distance on each update as well.
    distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 210.0;
    if (a_coeff ~= 0.0) then
        distance_moved_y = a_coeff * distance_moved_x;
    end

    -- Make the speed the same whatever the angle between the character and the enemy is.
    -- We deal only with a coefficients > 1.0 for simplification purpose.
    if (a_coeff > 1.0 and distance_moved_x ~= 0.0 and distance_moved_y ~= 0.0) then
        distance_moved_x = distance_moved_x * (distance_moved_x / distance_moved_y);
        if (a_coeff ~= 0.0) then
            distance_moved_y = a_coeff * distance_moved_x;
        end
        --print ("new_ratio: ", a_coeff, distance_moved_x / distance_moved_y)
    end

    -- Update the arrow flying height according to the distance
    -- Get the % of of x distance left
    local distance_left = math.abs((arrow_pos_x + distance_moved_x) - enemy_pos_x);

    if (total_distance > 0.0) then
        if (height_diff > 0.0) then
            arrow_height = height_min + ((distance_left / total_distance) * height_diff);
        else
            arrow_height = height_min + (((total_distance - distance_left) / total_distance) * -height_diff);
        end
    end

    -- Attack the enemy
    if (attack_step == 0) then
        character:ChangeSpriteAnimation("attack")
        attack_step = 1
    end
    -- Make the character go back to idle once attacked
    if (attack_step == 1) then
        attack_time = attack_time + SystemManager:GetUpdateTime();
        if (attack_time > 750.0) then
            character:ChangeSpriteAnimation("idle")
            attack_step = 2;
            character:SetShowAmmo(true);
        end
    end

    -- Triggers the arrow animation
    if (attack_step == 2) then
        if (arrow_pos_x > enemy_pos_x) then
            arrow_pos_x = arrow_pos_x - distance_moved_x;
            if arrow_pos_x < enemy_pos_x then arrow_pos_x = enemy_pos_x end
        end
        if (arrow_pos_x < enemy_pos_x) then
            arrow_pos_x = arrow_pos_x + distance_moved_x;
            if arrow_pos_x > enemy_pos_x then arrow_pos_x = enemy_pos_x end
        end
        if (arrow_pos_y > enemy_pos_y) then
            arrow_pos_y = arrow_pos_y - distance_moved_y;
            if arrow_pos_y < enemy_pos_y then arrow_pos_y = enemy_pos_y end
        end
        if (arrow_pos_y < enemy_pos_y) then
            arrow_pos_y = arrow_pos_y + distance_moved_y;
            if arrow_pos_y > enemy_pos_y then arrow_pos_y = enemy_pos_y end
        end

        character:SetAmmoPosition(arrow_pos_x, arrow_pos_y + arrow_height);
        character:GetAmmo():SetFlyingHeight(arrow_height);

        if (arrow_pos_x >= enemy_pos_x and arrow_pos_y == enemy_pos_y) then
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
