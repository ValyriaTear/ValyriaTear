-- Filename: bronann_attack.lua
-- This file is scripting Bronann's attack animation, called by attack skills.
-- The initialize() function is called once, followed by calls to the update function.
-- When the update function returns true, the attack is finished.

-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
claudius_attack = ns;
setfenv(1, ns);

-- local references
local character = {};
local target = {};
local target_actor = {};
local skill = {};

local character_pos_x = 0.0;
local character_pos_y = 0.0;

local enemy_pos_x = 0.0;
local enemy_pos_y = 0.0;

local attack_step = 0;
local attack_time = 0.0;

local damage_triggered = false;

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

    enemy_pos_x = target_actor:GetXLocation();
    enemy_pos_y = target_actor:GetYLocation();

    attack_step = 0;
    attack_time = 0;

    damage_triggered = false;

    distance_moved_x = SystemManager:GetUpdateTime() / hoa_map.MapMode.NORMAL_SPEED * 120.0;
    local y_diff = character_pos_y - enemy_pos_y;
    if (y_diff == 0.0) then
        a_coeff = 0.0;
        distance_moved_y = 0.0;
    else
        a_coeff = (enemy_pos_x - character_pos_x - 64.0) / (character_pos_y - enemy_pos_y);
        if (a_coeff < 0) then a_coeff = -a_coeff; end
        distance_moved_y = (1/a_coeff) * distance_moved_x;
    end

    --print("distance x: ", enemy_pos_x - character_pos_x - 64.0)
    --print("distance y: ", character_pos_y - enemy_pos_y)
    --print (distance_moved_x, 1/a_coeff, distance_moved_y);
end


function Update()
    -- Start to run towards the enemy
    if (attack_step == 0) then
        character:ChangeSpriteAnimation("jump_forward")
        attack_step = 1
    end
    -- Make the player move till it reaches the enemy
    if (attack_step == 1) then
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
        if (damage_triggered == false and attack_time > 505.0) then
            skill:ExecuteBattleFunction(character, target);
            damage_triggered = true;
        end

        if (attack_time > 730.0) then
            character:ChangeSpriteAnimation("jump_backward")
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
        return true;
    end
    return false;
end
