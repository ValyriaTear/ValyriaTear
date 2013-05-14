------------------------------------------------------------------------------[[
-- Filename: special.lua
--
-- Description: This file contains the definitions of all special skills.
-- Each special skill has a unique integer identifier
-- that is used as its key in the skills table below. Some skills are primarily
-- intended for characters to use while others are intended for enemies to use.
-- Normally, we do not want to share skills between characters and enemies as
-- character skills animate the sprites while enemy skills do not. Unlike attack
-- and defense skills, special skills may optionally be executed from the character
-- menu and their use is thus not restricted to battles alone.
--
-- Skill IDs 20,001 through 30,000 are reserved for special skills.
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

-- All special skills definitions are stored in this table
if (skills == nil) then
	skills = {}
end


--------------------------------------------------------------------------------
-- IDs 20,001 - 21,000 are reserved for character special skills
--------------------------------------------------------------------------------

-------------------------
-- Bronann's special skills
-------------------------

skills[20001] = {
    name = "", -- TODO
    description = "", -- TODO
    icon = "",
    sp_required = 30,
    warmup_time = 6000,
    cooldown_time = 1000,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_SELF,

    BattleExecute = function(user, target)
        -- TODO: Add skill
    end,
}


-------------------------
-- Kalya's special skills
-------------------------

skills[20011] = {
    name = vt_system.Translate("Wolf Pain"),
    description = vt_system.Translate("Calls the defender of the Arctic."),
    icon = "img/icons/specials/moon.png",
    sp_required = 14,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_SELF,

    BattleExecute = function(user, target)
        -- TODO: Add the friendly AI monster in battle, next to Kalya
    end,
}

skills[20012] = {
    name = "", -- TODO
    description = "", -- TODO
    icon = "",
    sp_required = 30,
    warmup_time = 6000,
    cooldown_time = 1000,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_SELF,

    BattleExecute = function(user, target)
        -- TODO: Add the friendly AI monster in battle, next to Kalya
    end,
}

skills[20013] = {
    name = "", -- TODO
    description = "", -- TODO
    icon = "",
    sp_required = 30,
    warmup_time = 6000,
    cooldown_time = 1000,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_SELF,

    BattleExecute = function(user, target)
        -- TODO: Add the friendly AI monster in battle, next to Kalya
    end,
}

skills[20014] = {
    name = "", -- TODO
    description = "", -- TODO
    icon = "",
    sp_required = 30,
    warmup_time = 6000,
    cooldown_time = 1000,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_SELF,

    BattleExecute = function(user, target)
        -- TODO: Add the friendly AI monster in battle, next to Kalya
    end,
}

skills[20015] = {
    name = "", -- TODO
    description = "", -- TODO
    icon = "",
    sp_required = 30,
    warmup_time = 6000,
    cooldown_time = 1000,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_SELF,

    BattleExecute = function(user, target)
        -- TODO: Add the friendly AI monster in battle, next to Kalya
    end,
}

skills[20016] = {
    name = "", -- TODO
    description = "", -- TODO
    icon = "",
    sp_required = 30,
    warmup_time = 6000,
    cooldown_time = 1000,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_SELF,

    BattleExecute = function(user, target)
        -- TODO: Add the friendly AI monster in battle, next to Kalya
    end,
}

skills[20017] = {
    name = "", -- TODO
    description = "", -- TODO
    icon = "",
    sp_required = 30,
    warmup_time = 6000,
    cooldown_time = 1000,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_SELF,

    BattleExecute = function(user, target)
        -- TODO: Add the friendly AI monster in battle, next to Kalya
    end,
}

--------------------------------------------------------------------------------
-- IDs 21,001 - 30,000 are reserved for enemy special skills
--------------------------------------------------------------------------------

skills[21001] = {
    name = vt_system.Translate("Slime Spawn"),
    description = vt_system.Translate("Creates a slime."),
    sp_required = 0,
    warmup_time = 1400,
    cooldown_time = 750,
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_SELF,

    BattleExecute = function(user, target)
        local x_position = 250.0 + (vt_utils.RandomFloat() * 400.0)
        local y_position = 350.0 + (vt_utils.RandomFloat() * 250.0)
        local Battle = ModeManager:GetTop();
        Battle:AddEnemy(1, x_position, y_position);
    end
}
