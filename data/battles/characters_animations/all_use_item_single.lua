-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
all_use_item_single = ns;
setfenv(1, ns);

-- local references
local character = nil
local target = nil
local target_actor = nil
local battle_item = nil

local character_pos_x = 0.0;
local character_pos_y = 0.0;

local target_pos_x = 0.0;
local target_pos_y = 0.0;

local a_coeff = 0.0
local distance_moved_x = 0.0
local distance_moved_y = 0.0

local item_step = 0;
local item_time = 0.0;

local effect_triggered = false;

local Battle = nil

-- character, the BattleActor using the item (here Bronann)
-- target, the BattleEnemy target
-- The battle item used on target
function Initialize(_character, _target, _battle_item)
    -- Keep the reference in memory
    character = _character
    target = _target
    target_actor = _target:GetActor()
    battle_item = _battle_item

    -- Don't attack if the character isn't alive
    if (character:IsAlive() == false) then
        return;
    end

    -- Get the current characters' positions
    character_pos_x = character:GetXLocation();
    character_pos_y = character:GetYLocation();

    if (character == target_actor) then
        target_pos_x = character_pos_x
        target_pos_y = character_pos_y - 5.0 -- Makes the item placed ablove the target.
    else
        target_pos_x = target_actor:GetXLocation() - (character:GetSpriteWidth() / 2.0);
        target_pos_y = target_actor:GetYLocation() - 5.0; -- Makes the character placed above the target.
    end

    item_step = 0;
    item_time = 0;

    effect_triggered = false;

    distance_moved_x = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 170.0;
    local x_diff = target_pos_x - character_pos_x;
    local y_diff = character_pos_y - target_pos_y;
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

    --print("distance x: ", target_pos_x - character_pos_x)
    --print("distance y: ", character_pos_y - target_pos_y)
    --print (distance_moved_x, a_coeff, distance_moved_y);

    Battle = ModeManager:GetTop();
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

    if (character == target_actor) then
        item_step = 2
    end

    -- Start to run towards the enemy
    if (item_step == 0) then
        character:ChangeSpriteAnimation("jump_forward")
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua", character_pos_x, character_pos_y);

        item_step = 1
    end
    -- Make the player move till it reaches the enemy
    if (item_step == 1) then

        if (character_pos_x > target_pos_x) then
            character_pos_x = character_pos_x - distance_moved_x;
            if character_pos_x < target_pos_x then character_pos_x = target_pos_x end
        end
        if (character_pos_x < target_pos_x) then
            character_pos_x = character_pos_x + distance_moved_x;
            if character_pos_x > target_pos_x then character_pos_x = target_pos_x end
        end
        if (character_pos_y > target_pos_y) then
            character_pos_y = character_pos_y - distance_moved_y;
            if character_pos_y < target_pos_y then character_pos_y = target_pos_y end
        end
        if (character_pos_y < target_pos_y) then
            character_pos_y = character_pos_y + distance_moved_y;
            if character_pos_y > target_pos_y then character_pos_y = target_pos_y end
        end

        character:SetXLocation(character_pos_x);
        character:SetYLocation(character_pos_y);

        -- Attack when reaching the enemy
        if (character_pos_x >= target_pos_x and character_pos_y == target_pos_y) then
            item_step = 2;
        end
    end

    -- triggers the attack animation
    if (item_step == 2) then
        character:ChangeSpriteAnimation("magic_cast")
        if (character ~= target_actor) then
            Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua",
                                               character_pos_x, character_pos_y);
        end

        item_step = 3;
    end

    -- Wait for it to finish
    if (item_step == 3) then
        item_time = item_time + SystemManager:GetUpdateTime();

        -- Triggers the damage in the middle of the attack animation
        if (effect_triggered == false and item_time > 505.0) then
            Battle:GetIndicatorSupervisor():AddItemIndicator(target_actor:GetXLocation() + (target_actor:GetSpriteWidth() / 2.0),
                                                             target_actor:GetYLocation() - target_actor:GetSpriteHeight(),
                                                             battle_item:GetGlobalItem());
            battle_item:ExecuteBattleFunction(character, target)
            effect_triggered = true;
        end

        if (item_time > 730.0) then
            if (character == target_actor) then
                item_step = 5
            else
                character:ChangeSpriteAnimation("jump_backward")
                Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua", character_pos_x, character_pos_y);
                item_step = 4;
            end
        end
    end

    if (item_step == 4) then
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

        if (character_pos_x == character:GetXOrigin() and character_pos_y == character:GetYOrigin()) then
            item_step = 5;
        end
    end

    if (item_step == 5) then
        character:ChangeSpriteAnimation("idle")
        if (character ~= target_actor) then
            Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua",
                                               character_pos_x, character_pos_y);
        end
        return true;
    end
    return false;
end
