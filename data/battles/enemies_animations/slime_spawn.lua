-- The initialize() function is called once, followed by calls to the update function.
-- When the update function returns true, the attack is finished.

-- Set the namespace
local ns = {}
setmetatable(ns, {__index = _G})
slime_spawn = ns
setfenv(1, ns)

-- The current battle mode
local Battle = nil

-- local references
local attacker = nil
local target = nil
local target_actor = nil
local skill = nil

-- The current ball and shadow
local spawn_ball = nil
local spawn_shadow = nil
local splash_image = nil

local spawn_pos_x = 0.0
local spawn_pos_y = 0.0
-- y = a(x - h)^2 + k <-- a
local spawn_parabola_coeff = 0.0
local spawn_start_pos_x = 0.0

local shadow_pos_x = 0.0
local shadow_pos_y = 0.0
-- y = m * x + b
local shadow_m_coeff = 0.0
local shadow_b_value = 0.0

local spawn_start_height = 0.0
local spawn_max_x = 0.0
local spawn_max_y = 0.0

local target_pos_x = 0.0
local target_pos_y = 0.0

local attack_step = 0
local attack_time = 0.0

local skill_triggered = false

-- get a and b in y = ax + b given 2 points
function GetLinearCoefficients(pos_x1, pos_y1, pos_x2, pos_y2)
  local x_diff = pos_x2 - pos_x1;
  local y_diff = pos_y2 - pos_y1;
  local m_coeff = 0.0
  if (y_diff == 0.0) then
      m_coeff = 0.0
  elseif (x_diff == 0.0) then
      m_coeff = 0.0
  else
      m_coeff =  y_diff / x_diff
  end

  local b_value = pos_y1 - m_coeff * pos_x1

  return m_coeff, b_value
end

function Lerp(pos1, pos2, time)
    return (1.0 - time) * pos1 + time * pos2;
end

-- y = a(x - h)^2 + k
-- h = vertex_x on vertex pos
-- k = vertex_y on on vertex pos
-- vertex is the crest of the parabola
-- base_pos is the position of a point along the curve
function GetParabolaCoefficient(vertex_x, vertex_y, point_pos_x, point_pos_y)

    -- use the vertex and point with y = a(x - h)^2 + k
    if ((point_pos_x - vertex_x) == 0) then
      return 0.0;
    end
    local a = (point_pos_y - vertex_y) / math.pow(point_pos_x - vertex_x, 2)

    return a
end

-- attacker, the BattleActor attacking (here Kalya)
-- target, the BattleEnemy target
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

    -- Set the spawn_ball flying height members
    spawn_start_height = (attacker:GetSpriteHeight() / 2.0) + 5.0

    -- Set the spawn_ball starting position
    spawn_pos_x = attacker:GetXLocation()
    spawn_pos_y = attacker:GetYLocation() - spawn_start_height
    spawn_start_pos_x = spawn_pos_x

    shadow_pos_x = attacker:GetXLocation()
    shadow_pos_y = attacker:GetYLocation()

    -- The spawn_ball target spawn location (same for shadow)
    local x_range = 300.0
    if (vt_utils.RandomFloat() > 0.5) then
        x_range = -x_range
    end
    local x_range_base = vt_utils.RandomFloat()
    if (x_range_base <= 0.3) then
        x_range_base = 0.3
    end
    target_pos_x = attacker:GetXLocation() + (x_range_base * x_range)
    target_pos_y = attacker:GetYLocation() - (vt_utils.RandomFloat() * 250.0)

    spawn_max_x = (spawn_pos_x + target_pos_x) / 2.0
    spawn_max_y = ((spawn_pos_y + target_pos_y) / 2.0) - 100.0

    spawn_parabola_coeff = GetParabolaCoefficient(spawn_max_x, spawn_max_y, target_pos_x, target_pos_y)
    shadow_m_coeff, shadow_b_value = GetLinearCoefficients(shadow_pos_x, shadow_pos_y, target_pos_x, target_pos_y)
    --print("parabola coeff: "..spawn_parabola_coeff)
    --print("start pos: "..spawn_pos_x..", "..spawn_pos_y)
    --print("spawn max: "..spawn_max_x..", "..spawn_max_y)
    --print("target pos: "..target_pos_x..", "..target_pos_y)
    --print("m_coeff: "..shadow_m_coeff)

    Battle = ModeManager:GetTop()
    -- The spawn_ball and shadow battle animations.
    local ammo_filename = attacker:GetAmmoAnimationFile()
    spawn_ball = Battle:CreateBattleAnimation("data/entities/battle/effects/hit_splash.lua")
    spawn_ball:GetAnimatedImage():SetColor(vt_video.Color(0.0, 0.8, 0.0, 0.7))
    spawn_ball:SetVisible(false)
    spawn_shadow = Battle:CreateBattleAnimation("data/entities/battle/effects/hit_splash.lua")
    spawn_shadow:GetAnimatedImage():SetColor(vt_video.Color(0.0, 0.0, 0.0, 0.3))
    spawn_shadow:SetVisible(false)

    attack_step = 0
    attack_time = 0

    skill_triggered = false
end

function Update()
    -- The update time can vary, so update the distance on each update as well.
    local move_diff = SystemManager:GetUpdateTime() / vt_map.MapMode.NORMAL_SPEED * 35.0
    if (spawn_start_pos_x > target_pos_x) then
        move_diff = -move_diff
    end

    -- a(x - h)^2 + k
    spawn_pos_x = spawn_pos_x + move_diff
    spawn_pos_y = spawn_parabola_coeff * math.pow(spawn_pos_x - spawn_max_x, 2) + spawn_max_y

    -- ax + b
    shadow_pos_x = shadow_pos_x + move_diff
    shadow_pos_y = shadow_m_coeff * shadow_pos_x + shadow_b_value

    -- Attack the enemy
    if (attack_step == 0) then
        --Battle:TriggerBattleParticleEffect("smth", x, y)
        attack_step = 1
    end
    -- Make the attacker go back to idle once attacked
    if (attack_step == 1) then
        attack_step = 2
        spawn_ball:SetXLocation(spawn_pos_x)
        spawn_ball:SetYLocation(spawn_pos_y)
        spawn_ball:SetVisible(true)
        spawn_ball:Reset()
        spawn_shadow:SetXLocation(shadow_pos_x)
        spawn_shadow:SetYLocation(shadow_pos_y)
        spawn_shadow:SetVisible(true)
        spawn_shadow:Reset()
    end

    -- Triggers the spawn_ball animation
    if (attack_step == 2) then
        if (spawn_ball ~= nil) then
            spawn_ball:SetXLocation(spawn_pos_x)
            spawn_ball:SetYLocation(spawn_pos_y)
        end
        if (spawn_shadow ~= nil) then
            spawn_shadow:SetXLocation(shadow_pos_x)
            spawn_shadow:SetYLocation(shadow_pos_y)
        end

        if ((spawn_start_pos_x < target_pos_x and spawn_pos_x >= target_pos_x)
            or (spawn_start_pos_x >= target_pos_x and spawn_pos_x <= target_pos_x)) then
            attack_step = 3
        end
    end

    if (attack_step == 3) then
        -- Triggers the damage once the spawn_ball has reached the enemy
        if (skill_triggered == false) then
            -- Add the slime when reaching the ground
            Battle:AddEnemy(1, target_pos_x, target_pos_y)
            -- Remove the skill points at the end of spawn
            attacker:SubtractSkillPoints(skill:GetSPRequired())
            AudioManager:PlaySound("data/sounds/footstep_grass2.wav")
            Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/dust.lua", target_pos_x, target_pos_y)
            Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/small_burst_particles.lua", target_pos_x, target_pos_y)
            skill_triggered = true
            -- The Remove() call will make the engine delete the objects, so we set them to nil to avoid using them again.
            if (spawn_ball ~= nil) then
                spawn_ball:SetVisible(false)
                spawn_ball:Remove()
                spawn_ball = nil

                attack_time = 0.0
            end
            if (spawn_shadow ~= nil) then
                spawn_shadow:SetVisible(false)
                spawn_shadow:Remove()
                spawn_shadow = nil
            end
        end
        attack_step = 4
    end

    if (attack_step == 4) then
        attack_time = attack_time + SystemManager:GetUpdateTime()

        if (attack_time > 100.0) then
            attack_step = 5
        end
    end

    if (attack_step == 5) then
        return true
    end

    return false
end
