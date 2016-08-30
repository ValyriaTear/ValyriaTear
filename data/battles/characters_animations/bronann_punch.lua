-- Filename: bronann_punch.lua
-- This file is scripting Bronann's attack animation, called by attack skills.
-- The initialize() function is called once, followed by calls to the update function.
-- When the update function returns true, the attack is finished.

-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
bronann_punch = ns;
setfenv(1, ns);

-- local references
local character = nil
local target = nil
local target_actor = nil
local skill = nil
local Battle = nil

local character_pos_x = 0.0;
local character_pos_y = 0.0;
local character_offset_to_enemy = -32.0;

local a_coeff = 0.0;
local distance_moved_x = 0.0;
local distance_moved_y = 0.0;

local enemy_pos_x = 0.0;
local enemy_pos_y = 0.0;

local attack_step = 0;
local attack_time = 0.0;

local splash_image = nil;
local splash_width = 0;
local splash_height = 0;

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
    enemy_pos_y = target_actor:GetYLocation() - 5.0; -- Makes Bronann placed behind the enemy.

    attack_step = 0;
    attack_time = 0;

    damage_triggered = false;

    distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 170.0;
    local x_diff = enemy_pos_x - character_pos_x + character_offset_to_enemy;
    local y_diff = character_pos_y - enemy_pos_y;
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

    --print("distance x: ", enemy_pos_x - character_pos_x + character_offset_to_enemy)
    --print("distance y: ", character_pos_y - enemy_pos_y)
    --print (distance_moved_x, a_coeff, distance_moved_y);

    Battle = ModeManager:GetTop();

    splash_image = Battle:CreateBattleAnimation("data/entities/battle/effects/hit_splash.lua");
    splash_image:SetVisible(false);
    splash_width = splash_image:GetAnimatedImage():GetWidth()
    splash_height = splash_image:GetAnimatedImage():GetHeight()
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
        if (character_pos_x > enemy_pos_x + character_offset_to_enemy) then
            character_pos_x = character_pos_x - distance_moved_x;
            if character_pos_x < enemy_pos_x + character_offset_to_enemy then character_pos_x = enemy_pos_x + character_offset_to_enemy end
        end
        if (character_pos_x < enemy_pos_x + character_offset_to_enemy) then
            character_pos_x = character_pos_x + distance_moved_x;
            if character_pos_x > enemy_pos_x + character_offset_to_enemy then character_pos_x = enemy_pos_x + character_offset_to_enemy end
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
        if (character_pos_x >= enemy_pos_x + character_offset_to_enemy and character_pos_y == enemy_pos_y) then
            attack_step = 2;
        end
    end

    -- triggers the attack animation
    if (attack_step == 2) then
        character:ChangeSpriteAnimation("attack")
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua", character_pos_x, character_pos_y);
        attack_step = 3;
    end

    -- Wait it to finish
    if (attack_step == 3) then
        attack_time = attack_time + SystemManager:GetUpdateTime();
        -- Triggers the damage in the middle of the attack animation
        if (damage_triggered == false and attack_time > 505.0) then
            skill:ExecuteBattleFunction(character, target);
            -- Remove the skill points at the end of the third attack
            character:SubtractSkillPoints(skill:GetSPRequired());
            damage_triggered = true;

            -- Show the hit animation.
            splash_image:SetXLocation(enemy_pos_x);
            splash_image:SetYLocation(enemy_pos_y + 6.0);
            splash_image:SetVisible(true);
            splash_image:Reset();
        end

        if (damage_triggered == true and splash_image ~= nil and attack_time < 605.0) then
            splash_image:GetAnimatedImage():SetDimensions(splash_width * (attack_time - 505.0) / 100.0,
                                                          splash_height * (attack_time - 505.0) / 100.0)
        elseif (attack_time > 605.0 and splash_image ~= nil) then
            splash_image:SetVisible(false);
            splash_image:Remove();
            splash_image = nil;
        end

        if (attack_time > 730.0) then
            character:ChangeSpriteAnimation("jump_backward")
            Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua", character_pos_x, character_pos_y);
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
