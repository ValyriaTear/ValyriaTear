------------------------------------------------------------------------------[[
-- Filename: attack.lua
--
-- Description: This file contains the definitions of all attack skills.
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
    local attack_point = target_actor:GetAttackPoint(target:GetPoint());
    local chance_modifier = (user:GetTotalMagicalAttack() - attack_point:GetTotalMagicalDefense()) * 3.0;
    local chance = (hoa_utils.RandomFloat() * 100.0);
    --print( chance.. "/".. 50.0 + chance_modifier);
    if (chance > (50.0 + chance_modifier)) then
        target_actor:RegisterMiss(true);
        return;
    end

    -- Compute an effect duration time based on the characters' stats
    local effect_duration = (user:GetVigor() - target_actor:GetProtection()) * 2000;
    if (effect_duration < 15000) then effect_duration = 15000; end
    target_actor:RegisterStatusChange(hoa_global.GameGlobal.GLOBAL_STATUS_PARALYSIS,
                                      hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER,
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
	name = hoa_system.Translate("Sword Slash"),
	description = hoa_system.Translate("A textbook manoeuver that deals an effective blow."),
	sp_required = 0,
	warmup_time = 1200,
	cooldown_time = 200,
	action_name = "attack",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE,

	BattleExecute = function(user, target)
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasion(target) == false) then
			-- Normal +0 attack
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamage(user, target), target);
			AudioManager:PlaySound("snd/swordslice1.wav");
		else
			target_actor:RegisterMiss(true);
			AudioManager:PlaySound("snd/sword_swipe.wav");
		end
	end,

	animation_scripts = {
		[BRONANN] = "dat/skills/battle_animations/bronann_attack.lua",
		[THANIS] = "dat/skills/battle_animations/thanis_attack.lua"
	}
}

skills[2] = {
	name = hoa_system.Translate("Forward Thrust"),
	description = hoa_system.Translate("A quicker and more powerful blow than the standard sword slash."),
	sp_required = 2,
	warmup_time = 800,
	cooldown_time = 200,
	action_name = "attack",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasion(target) == false) then
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageMultiplier(user, target, 1.75), target);
			AudioManager:PlaySound("snd/swordslice1.wav");
		else
			target_actor:RegisterMiss(true);
			AudioManager:PlaySound("snd/sword_swipe.wav");
		end

	end,

	animation_scripts = {
		[BRONANN] = "dat/skills/battle_animations/bronann_attack.lua",
		[THANIS] = "dat/skills/battle_animations/thanis_attack.lua"
	}
}

skills[3] = {
	name = hoa_system.Translate("Stun Strike"),
	description = hoa_system.Translate("A blow which temporarily stun its target."),
	sp_required = 4,
	warmup_time = 1200,
	cooldown_time = 0,
	action_name = "attack",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasionAdder(target, 5.5) == false) then
			-- Calculate chance for paralysis effect and activate it
			trigger_potential_stun(user, target);

			-- The damages are applied after the potential effects, so that a potential target death handles the effect removal properly
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamage(user, target));
			AudioManager:PlaySound("snd/swordslice1.wav");
		else
			target_actor:RegisterMiss(true);
			AudioManager:PlaySound("snd/sword_swipe.wav");
		end
	end,

	animation_scripts = {
		[BRONANN] = "dat/skills/battle_animations/bronann_attack.lua",
		[THANIS] = "dat/skills/battle_animations/thanis_attack.lua"
	}
}

skills[4] = {
	name = hoa_system.Translate("Blade Rush"),
	description = hoa_system.Translate("A strong and aggressive attack with a blade that deals significant damage."),
	sp_required = 4,
	warmup_time = 2000,
	cooldown_time = 0,
	action_name = "attack",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasionAdder(target, 8.5) == false) then
            local effect_duration = user:GetVigor() * 2000;
            if (effect_duration < 15000) then effect_duration = 15000 end
            target_actor:RegisterStatusChange(hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER,
                                          hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER,
                                          effect_duration);

			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageAdder(user, target, 20), target);
			AudioManager:PlaySound("snd/swordslice2.wav");
		else
			target_actor:RegisterMiss(true);
			AudioManager:PlaySound("snd/sword_swipe.wav");
		end
	end,

	animation_scripts = {
		[BRONANN] = "dat/skills/battle_animations/bronann_attack.lua",
		[THANIS] = "dat/skills/battle_animations/thanis_blade_rush_attack.lua"
	}
}

-- Kalya first attack
skills[5] = {
	name = hoa_system.Translate("Single Shot"),
	description = hoa_system.Translate("A simple shot using an arbalest."),
	sp_required = 0,
	warmup_time = 2500,
	cooldown_time = 200,
	action_name = "attack",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE,

	BattleExecute = function(user, target)
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasion(target) == false) then
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageAdder(user, target, 5), target);
			AudioManager:PlaySound("snd/crossbow.ogg");
		else
			target_actor:RegisterMiss(true);
			AudioManager:PlaySound("snd/crossbow_miss.ogg");
		end
	end,

	animation_scripts = {
		[KALYA] = "dat/skills/battle_animations/kalya_attack.lua"
	}
}

-- Sylve first attack
skills[6] = {
	name = hoa_system.Translate("Dagger Slash"),
	description = hoa_system.Translate("A simple but efficient dagger attack."),
	sp_required = 0,
	warmup_time = 1000,
	cooldown_time = 200,
	action_name = "attack",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasion(target) == false) then
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageAdder(user, target, 5), target);
			AudioManager:PlaySound("snd/swordslice2.wav");
		else
			target_actor:RegisterMiss(true);
			AudioManager:PlaySound("snd/missed_target.wav");
		end
	end
}

--------------------------------------------------------------------------------
-- IDs 1,001 - 10,000 are reserved for enemy attack skills
--------------------------------------------------------------------------------

skills[1001] = {
	name = "Slime Attack",
	sp_required = 0,
	warmup_time = 1100,
	cooldown_time = 500,
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasion(target) == false) then
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageAdder(user, target, 10), target);
			AudioManager:PlaySound("snd/slime_attack.wav");
		else
			target_actor:RegisterMiss(true);
		end
	end
}

skills[1002] = {
	name = "Spider Bite",
	sp_required = 0,
	warmup_time = 1400,
	cooldown_time = 0,
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasion(target) == false) then
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageAdder(user, target, 13), target);
			AudioManager:PlaySound("snd/spider_attack.wav");
		else
			target_actor:RegisterMiss(true);
		end
	end
}

skills[1003] = {
	name = "Snake Bite",
	sp_required = 0,
	warmup_time = 900,
	cooldown_time = 0,
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasion(target) == false) then
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageAdder(user, target, 14), target);
			AudioManager:PlaySound("snd/snake_attack.wav");
		else
			target_actor:RegisterMiss(true);
		end
	end
}

skills[1004] = {
	name = "Snake Stun Bite",
	sp_required = 1,
	warmup_time = 900,
	cooldown_time = 0,
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasion(target) == false) then
            -- Calculate chance for paralysis effect and activate it
			trigger_potential_stun(user, target);

            -- The damages are applied after the potential effects, so that a potential target death handles the effect removal properly
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageAdder(user, target, 14), target);
			AudioManager:PlaySound("snd/snake_attack.wav");
		else
			target_actor:RegisterMiss(true);
		end
	end
}

skills[1005] = {
	name = "Snake Dampening Bite",
	sp_required = 1,
	warmup_time = 900,
	cooldown_time = 0,
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasion(target) == false) then
            target_actor = target:GetActor();
            local effect_duration = user:GetVigor() * 2000;
            if (effect_duration < 15000) then effect_duration = 15000 end
            target_actor:RegisterStatusChange(hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER,
                                              hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER,
                                              effect_duration);

            -- The damages are applied after the potential effects, so that a potential target death handles the effect removal properly
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageAdder(user, target, 6), target);
			AudioManager:PlaySound("snd/snake_attack.wav");
		else
			target_actor:RegisterMiss(true);
		end
	end
}

skills[1006] = {
	name = "Skeleton Sword Attack",
	sp_required = 0,
	warmup_time = 1400,
	cooldown_time = 0,
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasion(target) == false) then
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageAdder(user, target, 20), target);
			AudioManager:PlaySound("snd/skeleton_attack.wav");
		else
			target_actor:RegisterMiss(true);
		end
	end
}

skills[1007] = {
   name = "Bat HP Drain",
   sp_required = 0,
   warmup_time = 900,
   cooldown_time = 0,
   target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

   BattleExecute = function(user, target)
       target_actor = target:GetActor();

       if (hoa_battle.CalculateStandardEvasion(target) == false) then
           local hp_drain = hoa_battle.CalculatePhysicalDamageAdder(user, target, 8);
           target_actor:RegisterDamage(hp_drain, target);
           -- If the damage dealt was 1, don't recover any HP from the attack
           if (hp_drain > 1) then
               user:RegisterHealing(hp_drain / 2, true);
           end
           AudioManager:PlaySound("snd/spider_attack.wav");
       else
           target_actor:RegisterMiss(true);
       end
   end
}
