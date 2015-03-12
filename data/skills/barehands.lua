------------------------------------------------------------------------------[[
-- Skill IDs 1 through 40,000 are reserved for bare hands skills. (skills when no weapon equipped)
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

-- All attack skills definitions are stored in this table
if (skills == nil) then
    skills = {}
end

skills[30001] = {
   name = vt_system.Translate("Punch"),
   description = vt_system.Translate("A simple punch. Better than nothing."),
   sp_required = 0,
   warmup_time = 1000,
   cooldown_time = 200,
   action_name = "attack",
   target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

   BattleExecute = function(user, target)
       local target_actor = target:GetActor();

       if (vt_battle.CalculateStandardEvasion(target) == false) then
           -- Attack: Strength / 3
           target_actor:RegisterDamage(user:GetStrength() / 3.0, target);
           AudioManager:PlaySound("data/sounds/punch.wav");
       else
           target_actor:RegisterMiss(true);
           AudioManager:PlaySound("data/sounds/missed_target.wav");
       end
   end,

   animation_scripts = {
       [BRONANN] = "data/battles/characters_animations/bronann_punch.lua",
       [THANIS] = "data/battles/characters_animations/thanis_attack.lua"
   }
}

skills[30002] = {
   name = vt_system.Translate("Throw stone"),
   description = vt_system.Translate("Kalya's attack when she has no weapon."),
   sp_required = 0,
   warmup_time = 1000,
   cooldown_time = 200,
   action_name = "throw_stone",
   target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

   BattleExecute = function(user, target)
       local target_actor = target:GetActor();

       if (vt_battle.CalculateStandardEvasion(target) == false) then
           -- Attack: Strength / 3
           target_actor:RegisterDamage(user:GetStrength() / 3.0, target);
           AudioManager:PlaySound("data/sounds/punch.wav");
       else
           target_actor:RegisterMiss(true);
           AudioManager:PlaySound("data/sounds/missed_target.wav");
       end
   end,

   animation_scripts = {
       [KALYA] = "data/battles/characters_animations/kalya_throw_stone.lua"
   }
}

skills[30003] = {
   name = vt_system.Translate("Tornado Punch"),
   description = vt_system.Translate("Spinning knuckle attack that deals wind damage."),
   sp_required = 20,
   warmup_time = 1000,
   cooldown_time = 200,
   action_name = "throw_stone",
   target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

   BattleExecute = function(user, target)
       local target_actor = target:GetActor();
--TODO
       if (vt_battle.CalculateStandardEvasion(target) == false) then
           -- Attack: Strength / 3
           target_actor:RegisterDamage(user:GetStrength() / 3.0, target);
           AudioManager:PlaySound("data/sounds/punch.wav");
       else
           target_actor:RegisterMiss(true);
           AudioManager:PlaySound("data/sounds/missed_target.wav");
       end
   end,

   animation_scripts = {
       [KALYA] = "data/battles/characters_animations/kalya_throw_stone.lua"
   }
}
