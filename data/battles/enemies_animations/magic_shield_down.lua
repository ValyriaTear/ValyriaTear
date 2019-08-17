-- Set the namespace
local ns = {}
setmetatable(ns, {__index = _G})
magic_shield_down = ns
setfenv(1, ns)

-- local references
local attacker = nil
local target = nil
local target_actor = nil
local skill = nil

local attack_step = 0

local shield_anim = nil
local anim_time = 0
local anim_started = false

local Battle = nil

-- attacker, the BattleActor attacking
-- target, the BattleActor target
-- The skill id used on target
function Initialize(_attacker, _target, _skill)
    -- Keep the reference in memory
    attacker = _attacker
    target = _target
    target_actor = _target:GetActor()
    skill = _skill

    -- Don't attack if the attacker isn't alive
    if (attacker:IsAlive() == false) then
        return
    end

    attack_step = 0

    Battle = ModeManager:GetTop()
    -- A common claw slash animation
    shield_anim = Battle:CreateBattleAnimation("data/entities/battle/effects/magic_shield_breaking.lua")
    anim_time = 0
    anim_started = false
end


function Update()
    if (attack_step == 0) then
        attacker:ChangeSpriteAnimation("idle")

        -- Init the animation
        anim_time = 0
        anim_started = false

        attack_step = 1
    end

    if (attack_step == 1) then
        anim_time = anim_time + SystemManager:GetUpdateTime()
        if (anim_started == false) then
            anim_started = true

            shield_anim:SetXLocation(target_actor:GetXLocation())
            shield_anim:SetYLocation(target_actor:GetYLocation() + 0.1) -- To see the effect
            shield_anim:SetVisible(true)
            shield_anim:Reset()

            AudioManager:PlaySound("data/sounds/glass_shatter.wav")

            attack_step = 2
        end
    end

    if (attack_step == 2) then
        anim_time = anim_time + SystemManager:GetUpdateTime();

        -- Stops once the animation is done
        if (shield_anim ~= nil and anim_time > 50 * 16) then
            shield_anim:SetVisible(false);
            shield_anim:Remove();
            -- The Remove() call will make the engine delete the object, so we set it to nil
            -- to avoid using it again.
            shield_anim = nil;

            -- Triggers the skill
            skill:ExecuteBattleFunction(attacker, target)
            attacker:SubtractSkillPoints(skill:GetSPRequired())
            return true;
        end
    end

    return false;
end
