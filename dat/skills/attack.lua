------------------------------------------------------------------------------[[
-- Filename: attack.lua
--
-- Description: This file contains the definitions of all attack skills that
-- exist in Hero of Allacrost. Each attack skill has a unique integer identifier
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
-- {target_type}: The type of target the skill affects, which may be an attack point, actor, or party.
--
-- Each skill entry requires a function called {BattleExecute} to be defined. This function implements the
-- execution of the skill in battle, dealing damage, causing status changes, playing sounds, and animating
-- sprites.
------------------------------------------------------------------------------]]

-- All attack skills definitions are stored in this table
if (skills == nil) then
	skills = {}
end


--------------------------------------------------------------------------------
-- IDs 1 - 1,000 are reserved for character attack skills
--------------------------------------------------------------------------------

skills[1] = {
	name = hoa_system.Translate("Sword Slash"),
	description = hoa_system.Translate("A textbook manuever that deals an effective blow."),
	sp_required = 0,
	warmup_time = 2000,
	cooldown_time = 200,
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		user:ChangeSpriteAnimation("attack");
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasion(target) == false) then
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageAdder(user, target, 5), target);
			AudioManager:PlaySound("snd/swordslice1.wav");
		else
			target_actor:RegisterMiss();
			AudioManager:PlaySound("snd/sword_swipe.wav");
		end
	end
}

skills[2] = {
	name = hoa_system.Translate("Forward Thrust"),
	description = hoa_system.Translate("A more powerful blow than the standard sword slash, but also less likely to hit its intended target."),
	sp_required = 2,
	warmup_time = 500,
	cooldown_time = 0,
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		user:ChangeSpriteAnimation("attack");
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasionAdder(target, 5.0) == false) then
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageAdder(user, target, 0, 0.5), target);
			AudioManager:PlaySound("snd/swordslice1.wav");
		else
			target_actor:RegisterMiss();
			AudioManager:PlaySound("snd/sword_swipe.wav");
		end

	end
}

skills[3] = {
	name = hoa_system.Translate("Stun Strike"),
	description = hoa_system.Translate("A blow which targets vital areas and temporarily stun its target."),
	sp_required = 5,
	warmup_time = 1200,
	cooldown_time = 0,
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE,

	BattleExecute = function(user, target)
		user:ChangeSpriteAnimation("attack");
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasionAdder(target, 5.5) == false) then
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamage(user, target));
			-- TODO: Calculate chance for paralysis effect and activate it
			target_actor:RegisterStatusChange(hoa_global.GameGlobal.GLOBAL_STATUS_PARALYSIS, hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER);
			AudioManager:PlaySound("snd/swordslice1.wav");
		else
			target_actor:RegisterMiss();
			AudioManager:PlaySound("snd/sword_swipe.wav");
		end
	end
}

skills[4] = {
	name = hoa_system.Translate("Blade Rush"),
	description = hoa_system.Translate("A strong and aggressive attack with a blade that deals significant damage."),
	sp_required = 4,
	warmup_time = 2000,
	cooldown_time = 0,
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_FOE_POINT,

	BattleExecute = function(user, target)
		user:ChangeSpriteAnimation("attack");
		target_actor = target:GetActor();

		if (hoa_battle.CalculateStandardEvasionAdder(target, 8.5) == false) then
			target_actor:RegisterDamage(hoa_battle.CalculatePhysicalDamageAdder(user, target, 20), target);
			AudioManager:PlaySound("snd/swordslice2.wav");
		else
			target_actor:RegisterMiss();
			AudioManager:PlaySound("snd/sword_swipe.wav");
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
			target_actor:RegisterMiss();
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
			target_actor:RegisterMiss();
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
			target_actor:RegisterMiss();
		end
	end
}

skills[1004] = {
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
			target_actor:RegisterMiss();
		end
	end
}

