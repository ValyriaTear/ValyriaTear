------------------------------------------------------------------------------[[
-- Description: This file contains the definitions of all weapon skills.
-- Each attack skill has a unique integer identifier
-- that is used as its key in the skills table below. Some skills are primarily
-- intended for characters to use while others are intended for enemies to use.
-- Normally, we do not want to share skills between characters and enemies as
-- character skills animate the sprites while enemy skills do not.
--
-- Skill IDs 1 through 10,000 are reserved for attack skills.
--
-- Each skill entry requires the following data to be defined:
-- {name}: Text that defines the name of the skill
-- {description}: A brief (one sentence) description of the skill
--                (This field is required only for character skills and is optional for enemy skills)
-- {sp_required}: The number of skill points (SP) that are required to use the skill
--                (Zero is a valid value for this field, but a negative number is not)
-- {warmup_time}: The number of milliseconds that the actor using the skill must wait between
--                selecting the skill and executing it (a value of zero is valid).
-- {cooldown_time}: The number of milliseconds that the actor using the skill must wait after
--                  executing the skill before their stamina begins regenrating (zero is valid).
-- {action_name}: The sprite action played before executing the battle scripted function.
-- {target_type}: The type of target the skill affects, which may be an attack point, actor, or party.
--
-- Each skill entry requires a function called {BattleExecute} to be defined. This function implements the
-- execution of the skill in battle, dealing damage, causing status changes, playing sounds, and animating
-- sprites.
------------------------------------------------------------------------------]]

-- common functions
function trigger_potential_stun(user, target)
    local target_actor = target:GetActor();
    local attack_point = target_actor:GetAttackPoint(target:GetAttackPoint());
    local chance_modifier = (user:GetTotalMagicalAttack(vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL)
                            - attack_point:GetTotalMagicalDefense(vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL)) * 3.0;
    local chance = (vt_utils.RandomFloat() * 100.0);
    --print( chance.. "/".. 50.0 + chance_modifier);
    if (chance > (50.0 + chance_modifier)) then
        return;
    end

    -- Compute an effect duration time based on the characters' stats
    local effect_duration = (user:GetMagAtk() - target_actor:GetMagDef()) * 2000;
    if (effect_duration < 15000) then effect_duration = 15000; end
    target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_PARALYSIS,
                                         vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER,
                                         effect_duration);
    local Battle = ModeManager:GetTop();
    Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/stun_star.lua", target_actor:GetXLocation(), target_actor:GetYLocation());
end

function trigger_potential_attack_lowering(user, target)
    local target_actor = target:GetActor();
    local attack_point = target_actor:GetAttackPoint(target:GetAttackPoint());
    local chance_modifier = (user:GetTotalMagicalAttack(vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL)
                            - attack_point:GetTotalMagicalDefense(vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL)) * 3.0;
    local chance = (vt_utils.RandomFloat() * 100.0);
    --print( chance.. "/".. 50.0 + chance_modifier);
    if (chance > (50.0 + chance_modifier)) then
        return;
    end

    -- Compute an effect duration time based on the characters' stats
    local effect_duration = (user:GetMagAtk() - target_actor:GetMagDef()) * 2000;
    if (effect_duration < 15000) then effect_duration = 15000; end
    target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK,
                                         vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE,
                                         effect_duration);
end

-- All attack skills definitions are stored in this table
if (skills == nil) then
    skills = {}
end

--------------------------------------------------------------------------------
-- IDs 1 - 1,000 are reserved for character attack skills
--------------------------------------------------------------------------------

skills[1] = {
    name = vt_system.Translate("Sword Slash"),
    description = vt_system.Translate("A textbook maneuver that deals an effective blow."),
    sp_required = 0,
    warmup_time = 1200,
    cooldown_time = 200,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();

        if (vt_battle.RndEvade(target_actor) == false) then
            -- Normal +0 attack
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor), target);
            AudioManager:PlaySound("data/sounds/swordslice1.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/sword_swipe.wav");
        end
    end,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/bronann_attack.lua",
        [THANIS] = "data/battles/characters_animations/thanis_attack.lua"
    }
}

skills[2] = {
    name = vt_system.Translate("Forward Thrust"),
    show_notice = true,
    description = vt_system.Translate("A quicker and more powerful blow than the standard sword slash."),
    sp_required = 2,
    warmup_time = 800,
    cooldown_time = 200,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local attack_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0, 1.0, attack_point) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 0, 1.30, attack_point), target);
            AudioManager:PlaySound("data/sounds/swordslice1.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/sword_swipe.wav");
        end

    end,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/bronann_attack_forward_thrust.lua",
        [THANIS] = "data/battles/characters_animations/thanis_attack.lua"
    }
}

skills[3] = {
    name = vt_system.Translate("Stun Strike"),
    show_notice = true,
    description = vt_system.Translate("A blow which temporarily stun its target."),
    sp_required = 4,
    warmup_time = 1200,
    cooldown_time = 200,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 5.5, 1.0, atk_point) == false) then
            -- Calculate chance for paralysis effect and activate it
            trigger_potential_stun(user, target);

            -- The damages are applied after the potential effects, so that a potential target death handles the effect removal properly
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 0, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/swordslice1.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/sword_swipe.wav");
        end
    end,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/bronann_attack.lua",
        [THANIS] = "data/battles/characters_animations/thanis_attack.lua"
    }
}



skills[4] = {
    name = vt_system.Translate("X-Strike"),
    show_notice = true,
    description = vt_system.Translate("A strong and aggressive attack with a blade that deals significant damage."),
    sp_required = 32,
    warmup_time = 2500,
    cooldown_time = 1000,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();

        if (vt_battle.RndEvade(target_actor, 8.5) == false) then
            local effect_duration = user:GetMagAtk() * 2000;
            if (effect_duration < 15000) then effect_duration = 15000 end
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STAMINA,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_GREATER,
                                                 effect_duration);

            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 20), target);
            AudioManager:PlaySound("data/sounds/swordslice2.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/sword_swipe.wav");
        end
    end,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/bronann_attack.lua",
        [THANIS] = "data/battles/characters_animations/thanis_blade_rush_attack.lua"
    }
}

-- Kalya first attack
skills[5] = {
    name = vt_system.Translate("Single Shot"),
    description = vt_system.Translate("A simple shot using an arbalest."),
    sp_required = 0,
    warmup_time = 2500,
    cooldown_time = 200,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();

        if (vt_battle.RndEvade(target_actor) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 5), target);
            AudioManager:PlaySound("data/sounds/crossbow.ogg");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/crossbow_miss.ogg");
        end
    end,

    animation_scripts = {
        [KALYA] = "data/battles/characters_animations/kalya_attack.lua"
    }
}

skills[6] = {
    name = vt_system.Translate("Blade Rush"),
    show_notice = true,
    description = vt_system.Translate("A strong and aggressive attack with a blade that deals significant damage."),
    sp_required = 100,
    warmup_time = 5000,
    cooldown_time = 3000,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();

        if (vt_battle.RndEvade(target_actor, 8.5) == false) then
            local effect_duration = user:GetMagAtk() * 2000;
            if (effect_duration < 15000) then effect_duration = 15000 end
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STAMINA,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_GREATER,
                                                 effect_duration);

            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 20), target);
            AudioManager:PlaySound("data/sounds/swordslice2.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/sword_swipe.wav");
        end
    end,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/bronann_attack.lua",
        [THANIS] = "data/battles/characters_animations/thanis_blade_rush_attack.lua"
    }
}

-- Kalya's attacks
skills[101] = {
    name = vt_system.Translate("Incapacitating Shot"),
    show_notice = true,
    description = vt_system.Translate("A powerful shot aimed at lowering the enemy's ability to attack."),
    sp_required = 14,
    warmup_time = 2700,
    cooldown_time = 600,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            -- Calculate chance for attack lowering effect and activate it
            trigger_potential_attack_lowering(user, target);
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 15, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/crossbow.ogg");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/crossbow_miss.ogg");
        end
    end,

    animation_scripts = {
        [KALYA] = "data/battles/characters_animations/kalya_attack.lua"
    }
}

skills[102] = {
    name = vt_system.Translate("Rain of Arrows"),
    show_notice = true,
    description = vt_system.Translate("An attack striking all enemies."),
    sp_required = 28,
    warmup_time = 3500,
    cooldown_time = 1000,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_FOES,

    BattleExecute = function(user, target)
        local index = 0;
        while (target:GetPartyActor(index) ~= nil) do
            local target_actor = target:GetPartyActor(index)

            if (vt_battle.RndEvade(target_actor) == false) then
                target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 5), target);
                AudioManager:PlaySound("data/sounds/crossbow.ogg");
            else
                target_actor:RegisterMiss(true);
                AudioManager:PlaySound("data/sounds/crossbow_miss.ogg");
            end

            index = index + 1
        end
    end,

    animation_scripts = {
        [KALYA] = "data/battles/characters_animations/kalya_attack_party_target.lua"
    }
}

-- Sylve first attack
skills[200] = {
    name = vt_system.Translate("Dagger Slash"),
    description = vt_system.Translate("A simple but efficient dagger attack."),
    sp_required = 0,
    warmup_time = 1000,
    cooldown_time = 200,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 5, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/swordslice2.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[201] = {
    name = vt_system.Translate("Poison Slash"),
    show_notice = true,
    description = vt_system.Translate("A dagger attack, poisoning the enemy."),
    sp_required = 5,
    warmup_time = 1000,
    cooldown_time = 200,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 5, 1.0, atk_point), target);
            -- TODO: Add poison damage
            AudioManager:PlaySound("data/sounds/swordslice2.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[202] = {
    name = vt_system.Translate("High Strike"),
    show_notice = true,
    description = vt_system.Translate("A strike from above, stunning the enemy and breaking its magical defense."),
    sp_required = 10,
    warmup_time = 1000,
    cooldown_time = 200,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();
--TODO
        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 5, 1.0, atk_point), target);
            -- TODO: Add poison damage
            AudioManager:PlaySound("data/sounds/swordslice2.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[203] = {
    name = vt_system.Translate("Sonic Wind"),
    show_notice = true,
    description = vt_system.Translate("Deals high wind damage to all enemies, stunning them along the way."),
    sp_required = 100,
    warmup_time = 3000,
    cooldown_time = 1200,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();
--TODO
        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 5, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/swordslice2.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

-- Thanis attacks
skills[300] = {
    name = vt_system.Translate("Sword Breaker"),
    show_notice = true,
    description = vt_system.Translate("An attack that breaks the enemy's own attack power."),
    sp_required = 6,
    warmup_time = 2000,
    cooldown_time = 500,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();
--TODO
        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 5, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/swordslice2.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end,

    animation_scripts = {
        [THANIS] = "data/battles/characters_animations/thanis_attack.lua"
    }
}

skills[301] = {
    name = vt_system.Translate("Execution"),
    show_notice = true,
    description = vt_system.Translate("Attacks an enemy with a chance of causing death to catch its soul."),
    sp_required = 48,
    warmup_time = 4000,
    cooldown_time = 1500,
    action_name = "attack",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();
--TODO
        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 5, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/swordslice2.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end,
}

--------------------------------------------------------------------------------
-- IDs 1,001 - 10,000 are reserved for enemy attack skills
--------------------------------------------------------------------------------

skills[1001] = {
    name = "Slime Attack",
    sp_required = 0,
    warmup_time = 1100,
    cooldown_time = 500,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    animation_scripts = {
        -- N.B.: [1] is the enemy ID.
        [1] = "data/battles/enemies_animations/standard_enemy_attack.lua",
        [6] = "data/battles/enemies_animations/standard_enemy_attack.lua",
    },

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 10, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/slime_attack.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[1002] = {
    name = "Spider Bite",
    sp_required = 0,
    warmup_time = 1400,
    cooldown_time = 0,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    animation_scripts = {
        -- N.B.: [2] is the enemy ID.
        [2] = "data/battles/enemies_animations/standard_enemy_attack.lua",
    },

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 13, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/spider_attack.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[1003] = {
    name = "Snake Bite",
    sp_required = 0,
    warmup_time = 900,
    cooldown_time = 0,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    animation_scripts = {
        -- N.B.: [4] is the enemy ID.
        [4] = "data/battles/enemies_animations/standard_enemy_attack.lua",
    },

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 14, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/snake_attack.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[1004] = {
    name = vt_system.Translate("Stun Bite"),
    icon = "data/skills/monster_attacks/animal-fangs.png",
    show_notice = true,
    sp_required = 1,
    warmup_time = 3000,
    cooldown_time = 1000,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    animation_scripts = {
        -- N.B.: [4] is the enemy ID.
        [4] = "data/battles/enemies_animations/standard_enemy_attack.lua",
    },

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            -- Calculate chance for paralysis effect and activate it
            trigger_potential_stun(user, target);

            -- The damages are applied after the potential effects, so that a potential target death handles the effect removal properly
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 14, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/snake_attack.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[1005] = {
    -- Snake Dampening Bite
    name = vt_system.Translate("Dampening Bite"),
    icon = "data/skills/monster_attacks/animal-fangs.png",
    show_notice = true,
    sp_required = 2,
    warmup_time = 5000,
    cooldown_time = 2000,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    animation_scripts = {
        -- N.B.: [4] is the enemy ID.
        [4] = "data/battles/enemies_animations/standard_enemy_attack.lua",
    },

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            local effect_duration = user:GetMagAtk() * 2000;
            if (effect_duration < 15000) then effect_duration = 15000 end
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STAMINA,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER,
                                                 effect_duration);

            -- The damages are applied after the potential effects, so that a potential target death handles the effect removal properly
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 6, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/snake_attack.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[1006] = {
    name = "Skeleton Sword Attack",
    sp_required = 0,
    warmup_time = 1400,
    cooldown_time = 0,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 20, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/skeleton_attack.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[1007] = {
    name = vt_system.Translate("Bat HP Drain"),
    icon = "data/skills/monster_attacks/animal-fangs.png",
    show_notice = true,
    sp_required = 2,
    warmup_time = 7000,
    cooldown_time = 4000,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    animation_scripts = {
        -- N.B.: [6] is the enemy ID.
        [6] = "data/battles/enemies_animations/standard_enemy_attack.lua",
    },

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            local hp_drain = vt_battle.RndPhysicalDamage(user, target_actor, 8, 1.0, atk_point);
            target_actor:RegisterDamage(hp_drain, target);
            -- If the damage dealt was 1, don't recover any HP from the attack
            if (hp_drain > 1) then
                user:RegisterHealing(hp_drain / 2, true);
            end
            AudioManager:PlaySound("data/sounds/spider_attack.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[1008] = {
    name = "Fenrir Attack",
    sp_required = 0,
    warmup_time = 1200,
    cooldown_time = 300,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 20, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/growl1_IFartInUrGeneralDirection_freesound.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[1009] = {
    -- Fenrir Multiple Attack
    name = vt_system.Translate("Multiple Attack"),
    icon = "data/skills/monster_attacks/animal-fangs.png",
    show_notice = true,
    sp_required = 1,
    warmup_time = 1600,
    cooldown_time = 500,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_FOES,

    BattleExecute = function(user, target)
        local index = 0;
        while true do
            local target_actor = target:GetPartyActor(index);
            if (target_actor == nil) then
                break;
            end

            if (target_actor:IsAlive() == true and vt_battle.RndEvade(target_actor) == false) then
                target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 40));
                AudioManager:PlaySound("data/sounds/growl1_IFartInUrGeneralDirection_freesound.wav");
            else
                target_actor:RegisterMiss(true);
            end

            index = index + 1;
        end
    end
}

skills[1010] = {
    name = "Rat Poison Bite",
    sp_required = 5,
    warmup_time = 900,
    cooldown_time = 100,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            local intensity = target_actor:GetActiveStatusEffectIntensity(vt_global.GameGlobal.GLOBAL_STATUS_HP);

            -- Only apply up to a moderate poison
            if (intensity > vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE) then
                local effect_duration = user:GetMagAtk() * 2000;
                if (effect_duration < 15000) then effect_duration = 15000 end
                target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_HP,
                                                     vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER,
                                                     effect_duration);
            end

            -- The damages are applied after the potential effects, so that a potential target death handles the effect removal properly
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 6, 1.0, atk_point), target);
            AudioManager:PlaySound("data/sounds/skeleton_attack.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[1011] = {
    -- Skeleton Frenzy Attack
    name = vt_system.Translate("Frenzy Attack"),
    show_notice = true,
    sp_required = 10,
    warmup_time = 2400,
    cooldown_time = 500,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_FOES,

    BattleExecute = function(user, target)
        local index = 0;
        while true do
            local target_actor = target:GetPartyActor(index);
            if (target_actor == nil) then
                break;
            end

            if (target_actor:IsAlive() == true and vt_battle.RndEvade(target_actor) == false) then
                target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 25), target);
                AudioManager:PlaySound("data/sounds/skeleton_attack.wav");
            else
                target_actor:RegisterMiss(true);
                AudioManager:PlaySound("data/sounds/missed_target.wav");
            end

            index = index + 1;
        end
    end
}

skills[1012] = {
    name = "Beetle Stamina stealing Attack",
    sp_required = 12,
    warmup_time = 2400,
    cooldown_time = 500,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();

        if (vt_battle.RndEvade(target_actor) == false) then
            local effect_duration = user:GetMagAtk() * 2000;
            if (effect_duration < 15000) then effect_duration = 15000 end
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STAMINA,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE,
                                                 effect_duration);
            user:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STAMINA,
                                         vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                         effect_duration);

            -- The damages are applied after the potential effects, so that a potential target death handles the effect removal properly
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 6), target);
            AudioManager:PlaySound("data/sounds/spider_attack.wav");
        else
            target_actor:RegisterMiss(true);
            AudioManager:PlaySound("data/sounds/missed_target.wav");
        end
    end
}

skills[1013] = {
    name = "Dorver Frenzy",
    sp_required = 10,
    warmup_time = 2900,
    cooldown_time = 1000,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_SELF,

    BattleExecute = function(user, target) -- target is self, we'll use user...
        -- Add phys_atk & stamina, but decrease defense
        local effect_duration = user:GetMagAtk() * 2000;
        user:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK,
                                     vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                     effect_duration);
        user:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STAMINA,
                                     vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                     effect_duration);
        user:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_PHYS_DEF,
                                     vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE,
                                     effect_duration);

        AudioManager:PlaySound("data/sounds/defense1_spell.ogg");
    end
}

skills[1014] = {
    name = "Andromalius Ground Hit",
    sp_required = 0,
    warmup_time = 1900,
    cooldown_time = 1000,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();

        if (vt_battle.RndEvade(target_actor) == false) then
            local effect_duration = user:GetMagAtk() * 2000;
            if (effect_duration < 15000) then effect_duration = 15000 end
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STAMINA,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE,
                                                 effect_duration);

            -- The damages are applied after the potential effects, so that a potential target death handles the effect removal properly
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 6), target);
            AudioManager:PlaySound("data/sounds/cave-in.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end
}

skills[1015] = {
    name = vt_system.Translate("Water Spray"),
    icon = "data/skills/monster_attacks/waterspray.png",
    show_notice = true,
    sp_required = 0,
    warmup_time = 1100,
    cooldown_time = 500,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            local effect_duration = user:GetMagAtk() * 2000;
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER,
                                                 effect_duration);
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 10, 1.0, atk_point), target);

            local Battle = ModeManager:GetTop();
            Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/waterspray_skill.lua",
                                               target_actor:GetXLocation(), target_actor:GetYLocation());
        else
            target_actor:RegisterMiss(true);
        end
        -- Play the sound in any case
        AudioManager:PlaySound("data/sounds/watersplash.wav");
    end
}

skills[1016] = {
    name = vt_system.Translate("Spider Web"),
    icon = "data/skills/monster_attacks/web.png",
    show_notice = true,
    sp_required = 0,
    warmup_time = 1100,
    cooldown_time = 500,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

    animation_scripts = {
        -- N.B.: [2] is the spider enemy ID.
        [2] = "data/battles/enemies_animations/spider_web_attack.lua",
    },

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local atk_point = target:GetAttackPoint();

        if (vt_battle.RndEvade(target_actor, 0.0, 1.0, atk_point) == false) then
            local effect_duration = user:GetMagAtk() * 2000;
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STAMINA,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE,
                                                 effect_duration);
            target_actor:RegisterDamage(vt_battle.RndPhysicalDamage(user, target_actor, 13, 1.0, atk_point), target);
        else
            target_actor:RegisterMiss(true);
        end
        -- Play the sound in any case
        AudioManager:PlaySound("data/sounds/missed_target.wav");
    end
}
