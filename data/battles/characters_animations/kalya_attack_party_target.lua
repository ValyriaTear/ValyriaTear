-- This file is scripting Kalya's attack animation, called by attack skills, in case she is attacking every foes at once.
-- The initialize() function is called once, followed by calls to the update function.
-- When the update function returns true, the attack is finished.

-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
kalya_attack_party_target = ns;
setfenv(1, ns);

-- local references
local Battle = nil;
local character = nil
local target = nil
-- An array referencing all the necessary data about the quarrels triggered.
local target_arrows = {}
local skill = nil

local global_attack_step = 0;
local global_attack_time = 0.0;

-- character, the BattleActor attacking (here Kalya)
-- target, the BattleEnemy target
-- The skill id used on target
function Initialize(_character, _target, _skill)
    -- Keep the reference in memory
    character = _character;
    target = _target;
    skill = _skill;
    Battle = ModeManager:GetTop();

    local ammo_filename = character:GetAmmoAnimationFile();

    -- Don't attack if the character isn't alive
    if (character:IsAlive() == false) then
        return;
    end

    global_attack_step = 0;
    global_attack_time = 0.0;

    -- Register all the valid actors in the arrow table
    local index = 0;
    while (target:GetPartyActor(index) ~= nil) do
        local actor = target:GetPartyActor(index)
            if (actor:CanFight() == true) then
            target_arrows[index] = {};
            target_arrows[index].actor = actor
        end

        index = index + 1
    end

    -- Register each arrows data
    for index, arrow_info in pairs(target_arrows) do
        -- Set the arrow flying height members
        arrow_info.arrow_height = (character:GetSpriteHeight() / 2.0) + 5.0;
        arrow_info.total_distance = math.abs(arrow_info.actor:GetXLocation() - character:GetXLocation());
        arrow_info.height_diff = arrow_info.arrow_height - (arrow_info.actor:GetSpriteHeight() / 2.0);
        arrow_info.height_min = math.min(arrow_info.arrow_height, (arrow_info.actor:GetSpriteHeight() / 2.0));

        -- Set the arrow starting position
        arrow_info.arrow_pos_x = character:GetXLocation() + character:GetSpriteWidth() / 2.0;
        arrow_info.arrow_pos_y = character:GetYLocation() - arrow_info.arrow_height;

        -- Make the arrow reach the enemy center
        arrow_info.enemy_pos_x = arrow_info.actor:GetXLocation();
        arrow_info.enemy_pos_y = arrow_info.actor:GetYLocation() - arrow_info.actor:GetSpriteHeight() / 2.0;

        distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 210.0;
        local x_diff = arrow_info.enemy_pos_x - arrow_info.arrow_pos_x;
        local y_diff = arrow_info.arrow_pos_y - arrow_info.enemy_pos_y;

        local a_coeff = 0.0;
        local distance_moved_x = 0.0;
        local distance_moved_y = 0.0;

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
        arrow_info.a_coeff = a_coeff;
        arrow_info.distance_moved_x = distance_moved_x;
        arrow_info.distance_moved_y = distance_moved_y;

        arrow_info.attack_step = 0;
        arrow_info.damage_triggered = false;

        -- Set Kalya's missile sprites
        arrow_info.arrow = Battle:CreateBattleAnimation(ammo_filename);
        arrow_info.arrow_shadow = Battle:CreateBattleAnimation(ammo_filename);
        arrow_info.arrow_shadow:GetAnimatedImage():EnableGrayScale();

        index = index + 1
    end
end


function Update()

    -- Attack the enemy
    if (global_attack_step == 0) then
        character:ChangeSpriteAnimation("attack")
        global_attack_step = 1
        return false;
    end
    -- Make the character go back to idle once attacked
    if (global_attack_step == 1) then
        global_attack_time = global_attack_time + SystemManager:GetUpdateTime();
        if (global_attack_time > 750.0) then
            character:ChangeSpriteAnimation("idle")
            global_attack_step = 2;
        end
        return false;
    end

    for index, arrow_info in pairs(target_arrows) do

        -- Make the shadow visible
        if (arrow_info.attack_step == 0) then

            if (arrow_info.arrow ~= nil) then
                arrow_info.arrow:SetVisible(true);
                arrow_info.arrow:SetXLocation(arrow_info.arrow_pos_x);
                arrow_info.arrow:SetYLocation(arrow_info.arrow_pos_y + arrow_info.arrow_height);
            end

            if (arrow_info.arrow_shadow ~= nil) then
                arrow_info.arrow_shadow:SetVisible(true);
                arrow_info.arrow_shadow:SetXLocation(arrow_info.arrow_pos_x);
                arrow_info.arrow_shadow:SetYLocation(arrow_info.arrow_pos_y);
            end

            arrow_info.attack_step = 1
        end

        -- The update time can vary, so update the distance on each update as well.
        arrow_info.distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 210.0;
        if (arrow_info.a_coeff ~= 0.0) then
            arrow_info.distance_moved_y = arrow_info.a_coeff * arrow_info.distance_moved_x;
        end

        -- Make the speed the same whatever the angle between the character and the enemy is.
        -- We deal only with a coefficients > 1.0 for simplification purpose.
        if (arrow_info.a_coeff > 1.0 and arrow_info.distance_moved_x ~= 0.0 and arrow_info.distance_moved_y ~= 0.0) then
            arrow_info.distance_moved_x = arrow_info.distance_moved_x * (arrow_info.distance_moved_x / arrow_info.distance_moved_y);
            if (arrow_info.a_coeff ~= 0.0) then
                arrow_info.distance_moved_y = arrow_info.a_coeff * arrow_info.distance_moved_x;
            end
            --print ("new_ratio: ", arrow_info.a_coeff, arrow_info.distance_moved_x / arrow_info.distance_moved_y)
        end

        -- Update the arrow flying height according to the distance
        -- Get the % of of x distance left
        local distance_left = math.abs((arrow_info.arrow_pos_x + arrow_info.distance_moved_x) - arrow_info.enemy_pos_x);

        if (arrow_info.total_distance > 0.0) then
            if (arrow_info.height_diff > 0.0) then
                arrow_info.arrow_height = arrow_info.height_min + ((distance_left / arrow_info.total_distance) * arrow_info.height_diff);
            else
                arrow_info.arrow_height = arrow_info.height_min + (((arrow_info.total_distance - distance_left) / arrow_info.total_distance) * -arrow_info.height_diff);
            end
        end

        -- Triggers the arrow animation
        if (arrow_info.attack_step == 1) then
            if (arrow_info.arrow_pos_x > arrow_info.enemy_pos_x) then
                arrow_info.arrow_pos_x = arrow_info.arrow_pos_x - arrow_info.distance_moved_x;
                if arrow_info.arrow_pos_x < arrow_info.enemy_pos_x then arrow_info.arrow_pos_x = arrow_info.enemy_pos_x end
            end
            if (arrow_info.arrow_pos_x < arrow_info.enemy_pos_x) then
                arrow_info.arrow_pos_x = arrow_info.arrow_pos_x + arrow_info.distance_moved_x;
                if arrow_info.arrow_pos_x > arrow_info.enemy_pos_x then arrow_info.arrow_pos_x = arrow_info.enemy_pos_x end
            end
            if (arrow_info.arrow_pos_y > arrow_info.enemy_pos_y) then
                arrow_info.arrow_pos_y = arrow_info.arrow_pos_y - arrow_info.distance_moved_y;
                if arrow_info.arrow_pos_y < arrow_info.enemy_pos_y then arrow_info.arrow_pos_y = arrow_info.enemy_pos_y end
            end
            if (arrow_info.arrow_pos_y < arrow_info.enemy_pos_y) then
                arrow_info.arrow_pos_y = arrow_info.arrow_pos_y + arrow_info.distance_moved_y;
                if arrow_info.arrow_pos_y > arrow_info.enemy_pos_y then arrow_info.arrow_pos_y = arrow_info.enemy_pos_y end
            end

            -- Updates the arrow location
            if (arrow_info.arrow ~= nil) then
                arrow_info.arrow:SetXLocation(arrow_info.arrow_pos_x);
                arrow_info.arrow:SetYLocation(arrow_info.arrow_pos_y + arrow_info.arrow_height);
            end

            if (arrow_info.arrow_shadow ~= nil) then
                arrow_info.arrow_shadow:SetXLocation(arrow_info.arrow_pos_x);
                arrow_info.arrow_shadow:SetYLocation(arrow_info.arrow_pos_y);
            end

            if (arrow_info.arrow_pos_x >= arrow_info.enemy_pos_x and arrow_info.arrow_pos_y == arrow_info.enemy_pos_y) then
                arrow_info.attack_step = 2;
            end
        end

        if (arrow_info.attack_step == 2) then
            -- Remove the arrow has reached the enemy
            if (arrow_info.damage_triggered == false) then
                arrow_info.damage_triggered = true;
                if (arrow_info.arrow ~= nil) then
                    arrow_info.arrow:SetVisible(false);
                    arrow_info.arrow:Remove();
                    arrow_info.arrow = nil;
                end
                if (arrow_info.arrow_shadow ~= nil) then
                    arrow_info.arrow_shadow:SetVisible(false);
                    arrow_info.arrow_shadow:Remove();
                    arrow_info.arrow_shadow = nil;
                end
            end
            arrow_info.attack_step = 3
        end

    end

    -- Check whether every target has received an arrow
    for index, arrow_info in pairs(target_arrows) do
        if (arrow_info.damage_triggered == false) then
            return false;
        end
    end

    -- Remove the skill points at the end of all attacks
    skill:ExecuteBattleFunction(character, target);
    character:SubtractSkillPoints(skill:GetSPRequired());
    return true;
end
