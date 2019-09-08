-- Set the namespace
local ns = {}
setmetatable(ns, {__index = _G})
skeleton_frenzy_attack = ns
setfenv(1, ns)

-- local references
local attacker = nil
local target = nil
local skill = nil

local attacker_pos_x = 0.0
local attacker_pos_y = 0.0

local distance_moved = 0.0
local start_x_position = 0.0
local start_y_position = 0.0
local jump_end_x_position = 0.0
local diff_x = 0.0
local diff_y = 0.0

local targets = {}

local attack_step = 0

local damage_triggered = false

local Battle = nil
local claw_slash = nil
local slash_effect_time = 0
local slash_effect_started = false

-- attacker, the BattleActor attacking
-- target, the BattleActor target
-- The skill id used on target
function Initialize(_attacker, _target, _skill)
    -- Keep the reference in memory
    attacker = _attacker
    target = _target
    skill = _skill

    -- Don't attack if the attacker isn't alive
    if (attacker:IsAlive() == false) then
        return
    end

    -- Get the current attackers' positions
    attacker_pos_x = attacker:GetXLocation()
    attacker_pos_y = attacker:GetYLocation()

    attack_step = 0

    damage_triggered = false

    distance_moved = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 100.0
    start_x_position = attacker_pos_x
    jump_end_x_position = attacker_pos_x - 140
    start_y_position = attacker_pos_y
    diff_x = attacker_pos_x - 70
    diff_y = attacker_pos_y - 50

    Battle = ModeManager:GetTop()
    slash_effect_time = 0
    slash_effect_started = false

    -- Register all the valid actors position in the table
    local index = 0
    while (target:GetPartyActor(index) ~= nil) do
        local actor = target:GetPartyActor(index)
        if (actor:CanFight() == true) then
          targets[index] = {}
          targets[index].actor = actor
          targets[index].slash_anim = Battle:CreateBattleAnimation("data/entities/battle/effects/sword_slash.lua")
          targets[index].hit_anim = Battle:CreateBattleAnimation("data/entities/battle/effects/hit_splash.lua")
        end

        index = index + 1
    end
end


function Update()
    -- The update time can vary, so update the distance on each update as well.
    distance_moved = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 100.0

    -- Start to jump slightly
    if (attack_step == 0) then
        attacker:ChangeSpriteAnimation("idle")
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua", attacker_pos_x, attacker_pos_y)
        --AudioManager:PlaySound("data/sounds/growl1_IFartInUrGeneralDirection_freesound.wav")

        attack_step = 1
    end
    -- Make the attacker jump
    if (attack_step == 1) then

        if (attacker_pos_x > diff_x) then
            attacker_pos_x = attacker_pos_x - distance_moved
            if attacker_pos_x < diff_x then attacker_pos_x = diff_x end
        end
        if (attacker_pos_x < diff_x) then
            attacker_pos_x = attacker_pos_x + distance_moved
            if attacker_pos_x > diff_x then attacker_pos_x = diff_x end
        end

        if (attacker_pos_y > diff_y) then
            attacker_pos_y = attacker_pos_y - distance_moved
            if attacker_pos_y < diff_y then attacker_pos_y = diff_y end
        end
        if (attacker_pos_y < diff_y) then
            attacker_pos_y = attacker_pos_y + distance_moved
            if attacker_pos_y > diff_y then attacker_pos_y = diff_y end
        end

        attacker:SetXLocation(attacker_pos_x)
        attacker:SetYLocation(attacker_pos_y)

        -- half-jump done
        if (attacker_pos_x == diff_x and attacker_pos_y == diff_y) then
            attack_step = 2
        end
    end

    -- Second half-jump
    if (attack_step == 2) then
        if (attacker_pos_x > jump_end_x_position) then
            attacker_pos_x = attacker_pos_x - distance_moved
            if attacker_pos_x < jump_end_x_position then attacker_pos_x = jump_end_x_position end
        end
        if (attacker_pos_x < jump_end_x_position) then
            attacker_pos_x = attacker_pos_x + distance_moved
            if attacker_pos_x > jump_end_x_position then attacker_pos_x = jump_end_x_position end
        end

        if (attacker_pos_y > start_y_position) then
            attacker_pos_y = attacker_pos_y - distance_moved
            if attacker_pos_y < start_y_position then attacker_pos_y = start_y_position end
        end
        if (attacker_pos_y < start_y_position) then
            attacker_pos_y = attacker_pos_y + distance_moved
            if attacker_pos_y > start_y_position then attacker_pos_y = start_y_position end
        end

        attacker:SetXLocation(attacker_pos_x)
        attacker:SetYLocation(attacker_pos_y)

        -- half-jump done
        if (attacker_pos_y == start_y_position) then
            Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua", attacker_pos_x, attacker_pos_y)

            -- Init the slash effect life time
            slash_effect_time = 0
            slash_effect_started = false

            attack_step = 3
        end
    end

    -- Wait for it to finish
    if (attack_step == 3) then
        slash_effect_time = slash_effect_time + SystemManager:GetUpdateTime()
        if (slash_effect_started == false) then
            slash_effect_started = true

            for index, target_info in pairs(targets) do
                target_info.slash_anim:SetXLocation(target_info.actor:GetXLocation())
                target_info.slash_anim:SetYLocation(target_info.actor:GetYLocation() + 0.1) -- To see the effect
                target_info.slash_anim:SetVisible(true)
                target_info.slash_anim:Reset()

                target_info.hit_anim:SetXLocation(target_info.actor:GetXLocation())
                target_info.hit_anim:SetYLocation(target_info.actor:GetYLocation() + 0.1) -- To see the effect
                target_info.hit_anim:SetVisible(true)
                target_info.hit_anim:Reset()
            end

            -- Triggers the damage in the middle of the attack animation
            skill:ExecuteBattleFunction(attacker, target)
            -- Remove the skill points at the end of the attack
            attacker:SubtractSkillPoints(skill:GetSPRequired())
            attack_step = 4
        end
    end

    -- Return to start pos
    if (attack_step == 4) then
        slash_effect_time = slash_effect_time + SystemManager:GetUpdateTime()

        for index, target_info in pairs(targets) do
            if (target_info.hit_anim ~= nil) then
                target_info.hit_anim:SetXLocation(target_info.actor:GetXLocation() + math.random(-20.0, 20.0))
                target_info.hit_anim:SetYLocation(target_info.actor:GetYLocation() + math.random(-50.0, 0.0)) -- To see the effect
            end
        end

        if (attacker_pos_x > start_x_position) then
            attacker_pos_x = attacker_pos_x - distance_moved;
            if attacker_pos_x < start_x_position then attacker_pos_x = start_x_position end
        end
        if (attacker_pos_x < start_x_position) then
            attacker_pos_x = attacker_pos_x + distance_moved;
            if attacker_pos_x > start_x_position then attacker_pos_x = start_x_position end
        end

        if (attacker_pos_y > start_y_position) then
            attacker_pos_y = attacker_pos_y - distance_moved;
            if attacker_pos_y < start_y_position then attacker_pos_y = start_y_position end
        end
        if (attacker_pos_y < start_y_position) then
            attacker_pos_y = attacker_pos_y + distance_moved;
            if attacker_pos_y > start_y_position then attacker_pos_y = start_y_position end
        end

        attacker:SetXLocation(attacker_pos_x)
        attacker:SetYLocation(attacker_pos_y)

        -- Stops once all the animations are done
        if (slash_effect_time > 75 * 4) then

            local all_anim_are_done = true
            for index, target_info in pairs(targets) do
                if (target_info.hit_anim ~= nil) then
                    target_info.hit_anim:SetVisible(false)
                    target_info.hit_anim:Remove()
                    -- The Remove() call will make the engine delete the object, so we set it to nil
                    -- to avoid using it again.
                    target_info.hit_anim = nil
                end
                if (target_info.slash_anim ~= nil) then
                    target_info.slash_anim:SetVisible(false)
                    target_info.slash_anim:Remove()
                    -- The Remove() call will make the engine delete the object, so we set it to nil
                    -- to avoid using it again.
                    target_info.slash_anim = nil

                    all_anim_are_done = false
                end
            end

            if (all_anim_are_done == false) then
                return true
            end
        end
    end

    return false
end
