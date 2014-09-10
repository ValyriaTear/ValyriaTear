-- Filename: thanis_attack.lua
-- This file is scripting Bronann's attack animation, called by attack skills.
-- The initialize() function is called once, followed by calls to the update function.
-- When the update function returns true, the attack is finished.

-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
thanis_attack = ns;
setfenv(1, ns);

-- local references
local character = {};
local target = {};
local target_actor = {};
local skill = {};
local Battle = {};

local character_pos_x = 0.0;
local character_pos_y = 0.0;

local a_coeff = 0.0;
local distance_moved_x = 0.0;
local distance_moved_y = 0.0;

local enemy_pos_x = 0.0;
local enemy_pos_y = 0.0;

local attack_step = 0;
local attack_time = 0.0;

local damage_triggered = false;

-- Used to trigger dust
local move_time = 0

-- character, the BattleActor attacking
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

    -- Get the current characters' positions
    character_pos_x = character:GetXLocation();
    character_pos_y = character:GetYLocation();

    enemy_pos_x = target_actor:GetXLocation();
    enemy_pos_y = target_actor:GetYLocation();

    attack_step = 0;
    attack_time = 0;

    damage_triggered = false;

    distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 110.0;
    local x_diff = enemy_pos_x - character_pos_x - 64.0;
    local y_diff = character_pos_y - enemy_pos_y;
    if (y_diff == 0.0) then
        a_coeff = 0.0;
        distance_moved_y = 0.0;
    elseif (x_diff == 0.0) then
        a_coeff = 0.0;
        distance_moved_y = distance_moved_x;
        distance_moved_x = 0.0;
    else
        a_coeff = y_diff / x_diff;
        if (a_coeff < 0) then a_coeff = -a_coeff; end
        distance_moved_y = a_coeff * distance_moved_x;
    end

    --print("distance x: ", enemy_pos_x - character_pos_x - 64.0)
    --print("distance y: ", character_pos_y - enemy_pos_y)
    --print (distance_moved_x, a_coeff, distance_moved_y);

    Battle = ModeManager:GetTop();
    move_time = 0;
end


function Update()
    -- The update time can vary, so update the distance on each update as well.
    distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 110.0;
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

    -- Start to run towards the enemy
    if (attack_step == 0) then
        character:ChangeSpriteAnimation("run")
        move_time = 0;
        attack_step = 1
    end
    -- Make the player move till it reaches the enemy
    if (attack_step == 1) then
        -- Adds some dust every 15ms
        move_time = move_time + SystemManager:GetUpdateTime();
        if (move_time > 15) then
            Battle:TriggerBattleParticleEffect("dat/effects/particles/dust.lua", character_pos_x, character_pos_y);
            move_time = 0
        end

        if (character_pos_x > enemy_pos_x - 64.0) then
            character_pos_x = character_pos_x - distance_moved_x;
            if character_pos_x < enemy_pos_x - 64.0 then character_pos_x = enemy_pos_x - 64.0 end
        end
        if (character_pos_x < enemy_pos_x - 64.0) then
            character_pos_x = character_pos_x + distance_moved_x;
            if character_pos_x > enemy_pos_x - 64.0 then character_pos_x = enemy_pos_x - 64.0 end
        end
        if (character_pos_y > enemy_pos_y) then
            character_pos_y = character_pos_y - distance_moved_y;
            if character_pos_y < enemy_pos_y then character_pos_y = enemy_pos_y end
        end
        if (character_pos_y < enemy_pos_y) then
            character_pos_y = character_pos_y + distance_moved_y;
            if character_pos_y > enemy_pos_y then character_pos_y = enemy_pos_y end
        end

        character:SetXLocation(character_pos_x);
        character:SetYLocation(character_pos_y);

        -- Attack when reaching the enemy
        if (character_pos_x >= enemy_pos_x - 64.0 and character_pos_y == enemy_pos_y) then
            attack_step = 2;
        end
    end

    -- triggers the attack animation
    if (attack_step == 2) then
        character:ChangeSpriteAnimation("attack")
        attack_step = 3;
    end

    -- Wait it to finish
    if (attack_step == 3) then
        attack_time = attack_time + SystemManager:GetUpdateTime();
        -- Triggers the damage in the middle of the attack animation
        if (damage_triggered == false and attack_time > 225.0) then
            skill:ExecuteBattleFunction(character, target);
            -- Remove the skill points at the end of the third attack
            character:SubtractSkillPoints(skill:GetSPRequired());
            damage_triggered = true;
        end

        if (attack_time > 375.0) then
            character:ChangeSpriteAnimation("run_left")
            move_time = 0;
            attack_step = 4;
        end
    end

    -- triggers skill
    if (attack_step == 4) then
        -- Adds some dust every 15ms
        move_time = move_time + SystemManager:GetUpdateTime();
        if (move_time > 15) then
            Battle:TriggerBattleParticleEffect("dat/effects/particles/dust.lua", character_pos_x, character_pos_y);
            move_time = 0
        end

        -- Make the character jump back to its place
        if (character_pos_x > character:GetXOrigin()) then
            character_pos_x = character_pos_x - distance_moved_x;
            if character_pos_x < character:GetXOrigin() then character_pos_x = character:GetXOrigin() end
        end
        if (character_pos_x < character:GetXOrigin()) then
            character_pos_x = character_pos_x + distance_moved_x;
            if character_pos_x > character:GetXOrigin() then character_pos_x = character:GetXOrigin() end
        end
        if (character_pos_y > character:GetYOrigin()) then
            character_pos_y = character_pos_y - distance_moved_y;
            if character_pos_y < character:GetYOrigin() then character_pos_y = character:GetYOrigin() end
        end
        if (character_pos_y < character:GetYOrigin()) then
            character_pos_y = character_pos_y + distance_moved_y;
            if character_pos_y > character:GetYOrigin() then character_pos_y = character:GetYOrigin() end
        end

        character:SetXLocation(character_pos_x);
        character:SetYLocation(character_pos_y);

        -- Attack when reaching the enemy
        if (character_pos_x == character:GetXOrigin() and character_pos_y == character:GetYOrigin()) then
            attack_step = 5;
        end
    end

    if (attack_step == 5) then
        character:ChangeSpriteAnimation("idle")
        return true;
    end
    return false;
end
