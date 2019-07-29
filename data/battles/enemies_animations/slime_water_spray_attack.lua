-- The initialize() function is called once, followed by calls to the update function.
-- When the update function returns true, the attack is finished.

-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
slime_water_spray_attack = ns;
setfenv(1, ns);

-- The current battle mode
local Battle = nil

-- local references
local slime = nil
local target = nil
local target_actor = nil
local skill = nil

-- The current spray image and shadow
local spray = nil
local spray_shadow = nil

local spray_pos_x = 0.0;
local spray_pos_y = 0.0;

local spray_height = 0.0;
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
local mid_effect_triggered = false

-- slime, the BattleActor attacking
-- target, the BattleActor target
-- The skill id used on target
function Initialize(_slime, _target, _skill)
    -- Keep the reference in memory
    slime = _slime;
    target = _target;
    target_actor = _target:GetActor();
    skill = _skill;

    -- Don't attack if the monster isn't alive
    if (slime:IsAlive() == false) then
        return;
    end

    -- Set the slime spray flying height members
    spray_height = (slime:GetSpriteHeight() / 2.0) + 5.0;
    total_distance = math.abs(target_actor:GetXLocation() - slime:GetXLocation());
    height_diff = spray_height - (target_actor:GetSpriteHeight() / 2.0);
    height_min = math.min(spray_height, (target_actor:GetSpriteHeight() / 2.0));

    -- Set the spray starting position
    spray_pos_x = slime:GetXLocation() + slime:GetSpriteWidth() / 2.0;
    spray_pos_y = slime:GetYLocation() - spray_height;

    -- Make the spray reach the enemy center
    enemy_pos_x = target_actor:GetXLocation();
    enemy_pos_y = target_actor:GetYLocation() - target_actor:GetSpriteHeight() / 2.0;

    attack_step = 0;
    attack_time = 0;

    damage_triggered = false;

    distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 210.0;
    local x_diff = enemy_pos_x - spray_pos_x;
    local y_diff = spray_pos_y - enemy_pos_y;
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

    Battle = ModeManager:GetTop();
    -- The spray and shadow battle animations.
    local spray_filename = "data/entities/battle/effects/hit_splash.lua"
    spray = Battle:CreateBattleAnimation(spray_filename);
    spray_shadow = Battle:CreateBattleAnimation(spray_filename);
    spray_shadow:GetAnimatedImage():SetGrayscale(true);
    spray:SetVisible(false);
    spray_shadow:SetVisible(false);

    Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/waterspray_skill.lua",
                                       spray_pos_x, spray_pos_y)
end


function Update()
    -- The update time can vary, so update the distance on each update as well.
    distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 210.0;
    if (a_coeff ~= 0.0) then
        distance_moved_y = a_coeff * distance_moved_x;
    end

    -- Make the speed the same whatever the angle between the slime and the enemy is.
    -- We deal only with a coefficients > 1.0 for simplification purpose.
    if (a_coeff > 1.0 and distance_moved_x ~= 0.0 and distance_moved_y ~= 0.0) then
        distance_moved_x = distance_moved_x * (distance_moved_x / distance_moved_y);
        if (a_coeff ~= 0.0) then
            distance_moved_y = a_coeff * distance_moved_x;
        end
        --print ("new_ratio: ", a_coeff, distance_moved_x / distance_moved_y)
    end

    -- Update the spray flying height according to the distance
    -- Get the % of of x distance left
    local distance_left = math.abs((spray_pos_x + distance_moved_x) - enemy_pos_x);

    if (total_distance > 0.0) then
        if (height_diff > 0.0) then
            spray_height = height_min + ((distance_left / total_distance) * height_diff);
        else
            spray_height = height_min + (((total_distance - distance_left) / total_distance) * -height_diff);
        end
    end

    -- Attack the enemy
    if (attack_step == 0) then
        slime:ChangeSpriteAnimation("attack")
        attack_step = 1
    end
    -- Make the slime go back to idle once attacked
    if (attack_step == 1) then
        attack_time = attack_time + SystemManager:GetUpdateTime();
        if (attack_time > 750.0) then
            slime:ChangeSpriteAnimation("idle")
            attack_step = 2;
            spray:SetXLocation(spray_pos_x);
            spray:SetYLocation(spray_pos_y);
            spray:SetVisible(true);
            spray:Reset();
            spray_shadow:SetXLocation(spray_pos_x);
            spray_shadow:SetYLocation(spray_pos_y + spray_height);
            spray_shadow:SetVisible(true);
            spray_shadow:Reset()
            AudioManager:PlaySound("data/sounds/skeleton_attack.wav");
        end
    end

    -- Triggers the spray animation
    if (attack_step == 2) then
        if (mid_effect_triggered == false and attack_time > 1200.0) then
          Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/waterspray_skill.lua",
                                             spray_pos_x, spray_pos_y)
          mid_effect_triggered = true
        end
        if (spray_pos_x > enemy_pos_x) then
            spray_pos_x = spray_pos_x - distance_moved_x;
            if spray_pos_x < enemy_pos_x then spray_pos_x = enemy_pos_x end
        end
        if (spray_pos_x < enemy_pos_x) then
            spray_pos_x = spray_pos_x + distance_moved_x;
            if spray_pos_x > enemy_pos_x then spray_pos_x = enemy_pos_x end
        end
        if (spray_pos_y > enemy_pos_y) then
            spray_pos_y = spray_pos_y - distance_moved_y;
            if spray_pos_y < enemy_pos_y then spray_pos_y = enemy_pos_y end
        end
        if (spray_pos_y < enemy_pos_y) then
            spray_pos_y = spray_pos_y + distance_moved_y;
            if spray_pos_y > enemy_pos_y then spray_pos_y = enemy_pos_y end
        end

        if (spray ~= nil) then
            spray:SetXLocation(spray_pos_x);
            spray:SetYLocation(spray_pos_y);
        end
        if (spray_shadow ~= nil) then
            spray_shadow:SetXLocation(spray_pos_x);
            spray_shadow:SetYLocation(spray_pos_y + spray_height);
        end

        if (spray_pos_x >= enemy_pos_x and spray_pos_y == enemy_pos_y) then
            attack_step = 3;
        end
    end

    if (attack_step == 3) then
        -- Triggers the damage once the spray has reached the enemy
        if (damage_triggered == false) then
            skill:ExecuteBattleFunction(slime, target);
            -- Remove the skill points at the end of the third attack
            slime:SubtractSkillPoints(skill:GetSPRequired());
            damage_triggered = true;
            -- The Remove() call will make the engine delete the objects, so we set them to nil to avoid using them again.
            if (spray ~= nil) then
                spray:SetVisible(false)
                spray:Remove();
                spray = nil;
            end
            if (spray_shadow ~= nil) then
                spray_shadow:SetVisible(false);
                spray_shadow:Remove();
                spray_shadow = nil;
            end
        end
        attack_step = 4
    end

    if (attack_step == 4) then
        return true;
    end

    return false;
end
