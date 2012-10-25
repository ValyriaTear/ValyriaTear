------------------------------------------------------------------------------[[
-- Filename: support.lua
--
-- Description: This file contains the definitions of all support skills.
-- Each support skill has a unique integer identifier
-- that is used as its key in the skills table below. Some skills are primarily
-- intended for characters to use while others are intended for enemies to use.
-- Normally, we do not want to share skills between characters and enemies as
-- character skills animate the sprites while enemy skills do not. Unlike attack
-- and defense skills, support skills may optionally be executed from the character
-- menu and their use is thus not restricted to battles alone.
--
-- Skill IDs 20,001 through 30,000 are reserved for support skills.
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
-- execution of the skill in battle, buffing defense, causing status changes, playing sounds, and animating
-- sprites.
--
-- Each skill may optionally have a {FieldExecute} function defined. This function defines what happens when
-- the skill is used from the character menu in the field. This function will typically be more simple than
-- the battle execute function as it does not need to worry about sprite animation or other battle specific
-- features. If the skill does not have this function defined, then the skill will not be usable from the
-- character menu.
------------------------------------------------------------------------------]]

-- All support skills definitions are stored in this table
if (skills == nil) then
	skills = {}
end


--------------------------------------------------------------------------------
-- IDs 20,001 - 21,000 are reserved for character support skills
--------------------------------------------------------------------------------

skills[20001] = {
	name = hoa_system.Translate("Leader Call"),
	description = hoa_system.Translate("Temporarily increases the strength of all allies."),
	sp_required = 14,
	warmup_time = 4000,
	cooldown_time = 750,
	warmup_action_name = "magic_prepare",
	action_name = "magic_cast",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_ALL_ALLIES,

	BattleExecute = function(user, target)
		local index = 0;
        local effect_duration = user:GetVigor() * 3000;
		while true do
			target_actor = target:GetPartyActor(index);
			if (target_actor == nil) then
				break;
			end
			target_actor:RegisterStatusChange(hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_RAISE,
						hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER,
						effect_duration);
			index = index + 1;
		end
	end,
}

--------------------------------------------------------------------------------
-- IDs 21,001 - 30,000 are reserved for enemy support skills
--------------------------------------------------------------------------------


