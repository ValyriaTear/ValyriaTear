-- This file is scripting Bronann's attack animation, called by attack skills.
-- The initialize() function is called once, followed by calls to the update function.
-- When the update function returns true, the attack is finished.

-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
bronann_attack_forward_thrust = ns;
setfenv(1, ns);

-- local references
local character = nil
local target = nil
local target_actor = nil
local skill = nil

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

local Battle = nil
local sword_slash = nil
local slash_effect_time = 0
local slash_effect_started = false

-- Used to trigger dust
local move_time = 0
-- Used to set up the Forward thrust counter force
local forward_thrust_counter_force = 0.0;

-- character, the BattleActor attacking (here Bronann)
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

    enemy_pos_x = target_actor:GetXLocation() - (character:GetSpriteWidth() / 2.0);
    enemy_pos_y = target_actor:GetYLocation() - 5.0; -- Makes Bronann placed behind the enemy.

    attack_step = 0;
    attack_time = 0;

    damage_triggered = false;

    distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 170.0;
    local x_diff = enemy_pos_x - character_pos_x;
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

    --print("distance x: ", enemy_pos_x - character_pos_x)
    --print("distance y: ", character_pos_y - enemy_pos_y)
    --print (distance_moved_x, a_coeff, distance_moved_y);

    Battle = ModeManager:GetTop();
    -- A sword slash animation
    sword_slash = Battle:CreateBattleAnimation("img/sprites/battle/effects/sword_forward_slash.lua");
    slash_effect_time = 0;
    slash_effect_started = false;
    move_time = 0;
    forward_thrust_counter_force = 0;
end


function Update()
    -- The update time can vary, so update the distance on each update as well.
    distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 170.0;
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
        character:ChangeSpriteAnimation("jump_forward")
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua", character_pos_x, character_pos_y);

        attack_step = 1
    end
    -- Make the player move till it reaches the enemy
    if (attack_step == 1) then
        if (character_pos_x > enemy_pos_x) then
            character_pos_x = character_pos_x - distance_moved_x;
            if character_pos_x < enemy_pos_x then character_pos_x = enemy_pos_x end
        end
        if (character_pos_x < enemy_pos_x) then
            character_pos_x = character_pos_x + distance_moved_x;
            if character_pos_x > enemy_pos_x then character_pos_x = enemy_pos_x end
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
        if (character_pos_x >= enemy_pos_x and character_pos_y == enemy_pos_y) then
            attack_step = 2;
        end
    end

    -- triggers the attack animation
    if (attack_step == 2) then
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua", character_pos_x, character_pos_y);
        character:ChangeSpriteAnimation("attack_forward_thrust")

        -- Init the slash effect life time
        slash_effect_time = 0;
        slash_effect_started = false;

        move_time = 0;

        attack_step = 3;
    end

    -- Wait for it to finish
    if (attack_step == 3) then
        attack_time = attack_time + SystemManager:GetUpdateTime();

        if (attack_time > 410) then
            slash_effect_time = slash_effect_time + SystemManager:GetUpdateTime();
            if (slash_effect_started == false) then
                slash_effect_started = true

                sword_slash:SetXLocation(target_actor:GetXLocation());
                sword_slash:SetYLocation(target_actor:GetYLocation() + 2.0);
                sword_slash:SetVisible(true);
                sword_slash:Reset();
            end
        end

        if (sword_slash ~= nil and slash_effect_time > 75 * 4) then -- 300, 410 + 300 = 710 (< 730).
            sword_slash:SetVisible(false);
            sword_slash:Remove();
            -- The Remove() call will make the engine delete the object, so we set it to nil
            -- to avoid using it again.
            sword_slash = nil;
        end

        -- Triggers the damage in the middle of the attack animation
        if (damage_triggered == false and attack_time > 505.0) then
            skill:ExecuteBattleFunction(character, target);
            -- Remove the skill points at the end of the third attack
            character:SubtractSkillPoints(skill:GetSPRequired());
            damage_triggered = true;
        end

        -- Makes the character go through the enemy for the second blow.
        if (attack_time > 390) then
            character_pos_x = character_pos_x + distance_moved_x - forward_thrust_counter_force;
            character:SetXLocation(character_pos_x);
            forward_thrust_counter_force = forward_thrust_counter_force + (2.0 * SystemManager:GetUpdateTime() / 30.0);

            -- Adds some dust every 15ms
            move_time = move_time + SystemManager:GetUpdateTime();
            if (move_time > 15) then
                Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua", character_pos_x, character_pos_y);
                move_time = 0
            end
        end

        if (attack_time > 730.0) then
            character:ChangeSpriteAnimation("jump_backward")
            Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua", character_pos_x, character_pos_y);

            -- Recompute the a coefficient using the character origin position as the angle has changed.
            distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 170.0;
            local x_diff = character:GetXLocation() - character:GetXOrigin();
            local y_diff = character:GetYOrigin() - character:GetYLocation();
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

            attack_step = 4;
        end
    end

    -- triggers skill
    if (attack_step == 4) then
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
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua", character_pos_x, character_pos_y);
        return true;
    end
    return false;
end
