-- The initialize() function is called once, followed by calls to the update function.
-- When the update function returns true, the attack is finished.

-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
spider_web_attack = ns;
setfenv(1, ns);

-- The current battle mode
local Battle = nil

-- local references
local spider = nil
local target = nil
local target_actor = nil
local skill = nil

-- The current web image and shadow
local web = nil
local web_shadow = nil

local web_pos_x = 0.0;
local web_pos_y = 0.0;

local web_height = 0.0;
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

-- spider, the BattleActor attacking
-- target, the BattleActor target
-- The skill id used on target
function Initialize(_spider, _target, _skill)
    -- Keep the reference in memory
    spider = _spider;
    target = _target;
    target_actor = _target:GetActor();
    skill = _skill;

    -- Don't attack if the monster isn't alive
    if (spider:IsAlive() == false) then
        return;
    end

    -- Set the spider web flying height members
    web_height = (spider:GetSpriteHeight() / 2.0) + 5.0;
    total_distance = math.abs(target_actor:GetXLocation() - spider:GetXLocation());
    height_diff = web_height - (target_actor:GetSpriteHeight() / 2.0);
    height_min = math.min(web_height, (target_actor:GetSpriteHeight() / 2.0));

    -- Set the web starting position
    web_pos_x = spider:GetXLocation() + spider:GetSpriteWidth() / 2.0;
    web_pos_y = spider:GetYLocation() - web_height;

    -- Make the web reach the enemy center
    enemy_pos_x = target_actor:GetXLocation();
    enemy_pos_y = target_actor:GetYLocation() - target_actor:GetSpriteHeight() / 2.0;

    attack_step = 0;
    attack_time = 0;

    damage_triggered = false;

    distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 210.0;
    local x_diff = enemy_pos_x - web_pos_x;
    local y_diff = web_pos_y - enemy_pos_y;
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
    -- The web and shadow battle animations.
    local web_filename = "data/battles/enemies_animations/spider_web.lua"
    web = Battle:CreateBattleAnimation(web_filename);
    web_shadow = Battle:CreateBattleAnimation(web_filename);
    web_shadow:GetAnimatedImage():SetGrayscale(true);
    web:SetVisible(false);
    web_shadow:SetVisible(false);
end


function Update()
    -- The update time can vary, so update the distance on each update as well.
    distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 210.0;
    if (a_coeff ~= 0.0) then
        distance_moved_y = a_coeff * distance_moved_x;
    end

    -- Make the speed the same whatever the angle between the spider and the enemy is.
    -- We deal only with a coefficients > 1.0 for simplification purpose.
    if (a_coeff > 1.0 and distance_moved_x ~= 0.0 and distance_moved_y ~= 0.0) then
        distance_moved_x = distance_moved_x * (distance_moved_x / distance_moved_y);
        if (a_coeff ~= 0.0) then
            distance_moved_y = a_coeff * distance_moved_x;
        end
        --print ("new_ratio: ", a_coeff, distance_moved_x / distance_moved_y)
    end

    -- Update the web flying height according to the distance
    -- Get the % of of x distance left
    local distance_left = math.abs((web_pos_x + distance_moved_x) - enemy_pos_x);

    if (total_distance > 0.0) then
        if (height_diff > 0.0) then
            web_height = height_min + ((distance_left / total_distance) * height_diff);
        else
            web_height = height_min + (((total_distance - distance_left) / total_distance) * -height_diff);
        end
    end

    -- Attack the enemy
    if (attack_step == 0) then
        spider:ChangeSpriteAnimation("attack")
        AudioManager:PlaySound("data/sounds/throw.wav");
        attack_step = 1
    end
    -- Make the spider go back to idle once attacked
    if (attack_step == 1) then
        attack_time = attack_time + SystemManager:GetUpdateTime();
        if (attack_time > 750.0) then
            spider:ChangeSpriteAnimation("idle")
            attack_step = 2;
            web:SetXLocation(web_pos_x);
            web:SetYLocation(web_pos_y);
            web:SetVisible(true);
            web:Reset();
            web_shadow:SetXLocation(web_pos_x);
            web_shadow:SetYLocation(web_pos_y + web_height);
            web_shadow:SetVisible(true);
            web_shadow:Reset()
        end
    end

    -- Triggers the web animation
    if (attack_step == 2) then
        if (web_pos_x > enemy_pos_x) then
            web_pos_x = web_pos_x - distance_moved_x;
            if web_pos_x < enemy_pos_x then web_pos_x = enemy_pos_x end
        end
        if (web_pos_x < enemy_pos_x) then
            web_pos_x = web_pos_x + distance_moved_x;
            if web_pos_x > enemy_pos_x then web_pos_x = enemy_pos_x end
        end
        if (web_pos_y > enemy_pos_y) then
            web_pos_y = web_pos_y - distance_moved_y;
            if web_pos_y < enemy_pos_y then web_pos_y = enemy_pos_y end
        end
        if (web_pos_y < enemy_pos_y) then
            web_pos_y = web_pos_y + distance_moved_y;
            if web_pos_y > enemy_pos_y then web_pos_y = enemy_pos_y end
        end

        if (web ~= nil) then
            web:SetXLocation(web_pos_x);
            web:SetYLocation(web_pos_y);
        end
        if (web_shadow ~= nil) then
            web_shadow:SetXLocation(web_pos_x);
            web_shadow:SetYLocation(web_pos_y + web_height);
        end

        if (web_pos_x >= enemy_pos_x and web_pos_y == enemy_pos_y) then
            attack_step = 3;
        end
    end

    if (attack_step == 3) then
        -- Triggers the damage once the web has reached the enemy
        if (damage_triggered == false) then
            skill:ExecuteBattleFunction(spider, target);
            -- Remove the skill points at the end of the third attack
            spider:SubtractSkillPoints(skill:GetSPRequired());
            damage_triggered = true;
            -- The Remove() call will make the engine delete the objects, so we set them to nil to avoid using them again.
            if (web ~= nil) then
                web:SetVisible(false)
                web:Remove();
                web = nil;
            end
            if (web_shadow ~= nil) then
                web_shadow:SetVisible(false);
                web_shadow:Remove();
                web_shadow = nil;
            end
        end
        attack_step = 4
    end

    if (attack_step == 4) then
        return true;
    end

    return false;
end
