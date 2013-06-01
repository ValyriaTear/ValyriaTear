------------------------------------------------------------------------------[[
-- Filename: magic.lua
--
-- Description: This file contains the definitions of all support skills.
-- Each support skill has a unique integer identifier
-- that is used as its key in the skills table below. Some skills are primarily
-- intended for characters to use while others are intended for enemies to use.
-- Normally, we do not want to share skills between characters and enemies as
-- character skills animate the sprites while enemy skills do not.
--
-- Skill IDs 10,001 through 20,000 are reserved for support skills.
--
-- Each skill entry requires the following data to be defined:
-- {name}: Text that defines the name of the skill
-- {description}: A brief (one sentence) description of the skill
--                (This field is required only for character skills and is optional for enemy skills)
-- {icon}: the skill icon filename
--                (This field is is optional)
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
------------------------------------------------------------------------------]]

-- All support skills definitions are stored in this table
if (skills == nil) then
    skills = {}
end


--------------------------------------------------------------------------------
-- IDs 10,001 - 11,000 are reserved for character magic skills
--------------------------------------------------------------------------------

-------------------------------------------------------------
-- 10001 - 10099 - Natural skills, obtained through levelling
-------------------------------------------------------------
skills[10001] = {
    name = vt_system.Translate("Shield"),
    description = vt_system.Translate("Increases an ally's physical defense by a slight degree."),
    icon = "img/icons/magic/shield.png",
    sp_required = 3,
    warmup_time = 300,
    cooldown_time = 0,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local effect_duration = user:GetProtection() * 2000;
        if (effect_duration < 10000) then effect_duration = 10000 end
        target_actor:RegisterStatusChange(vt_global.GameGlobal.GLOBAL_STATUS_FORTITUDE_RAISE,
                                          vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                          effect_duration);
        local Battle = ModeManager:GetTop();
        if (target_actor:GetSpriteHeight() > 250.0) then
            -- Big sprite version
            Battle:TriggerBattleParticleEffect("dat/effects/particles/shield_big_sprites.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
        else
            Battle:TriggerBattleParticleEffect("dat/effects/particles/shield.lua",
                    target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
        end
        AudioManager:PlaySound("snd/defence1_spell.ogg");
    end
}

skills[10002] = {
    name = vt_system.Translate("First Aid"),
    description = vt_system.Translate("Performs basic medical assistance, healing the target by a minor degree."),
    icon = "img/icons/magic/first_aid.png",
    sp_required = 4,
    warmup_time = 1500,
    cooldown_time = 200,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local hit_points = (user:GetVigor() * 3) +  vt_utils.RandomBoundedInteger(0, 15);
        target_actor:RegisterHealing(hit_points, true);
        AudioManager:PlaySound("snd/heal_spell.wav");
        local Battle = ModeManager:GetTop();
        Battle:TriggerBattleParticleEffect("dat/effects/particles/heal_particle.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
    end,

    FieldExecute = function(target, instigator)
        target:AddHitPoints((instigator:GetVigor() * 5) + vt_utils.RandomBoundedInteger(0, 30));
        AudioManager:PlaySound("snd/heal_spell.wav");
    end
}

skills[10003] = {
    name = vt_system.Translate("Leader Call"),
    description = vt_system.Translate("Temporarily increases the strength of all allies."),
    icon = "img/icons/magic/leader_call.png",
    sp_required = 14,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_ALLIES,

    BattleExecute = function(user, target)
        local index = 0;
        local effect_duration = user:GetVigor() * 3000;
        while true do
            local target_actor = target:GetPartyActor(index);
            if (target_actor == nil) then
                break;
            end
            target_actor:RegisterStatusChange(vt_global.GameGlobal.GLOBAL_STATUS_STRENGTH_RAISE,
                        vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER,
                        effect_duration);
            index = index + 1;
        end
    end,
}

skills[10004] = {
    name = vt_system.Translate("Holy Veil"),
    description = vt_system.Translate("Increases the attack and magical attack of all allies."),
    --icon = "img/icons/magic/leader_call.png",
    sp_required = 64,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_ALLIES,

    BattleExecute = function(user, target)
        local index = 0;
        local effect_duration = user:GetVigor() * 3000;
        while true do
            local target_actor = target:GetPartyActor(index);
            if (target_actor == nil) then
                break;
            end
            target_actor:RegisterStatusChange(vt_global.GameGlobal.GLOBAL_STATUS_STRENGTH_RAISE,
                        vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                        effect_duration);
            target_actor:RegisterStatusChange(vt_global.GameGlobal.GLOBAL_STATUS_VIGOR_RAISE,
                        vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                        effect_duration);
            index = index + 1;
        end
    end,
}

skills[10005] = {
    name = vt_system.Translate("Ancient Strength"),
    description = vt_system.Translate("Increases every elemental resistance of all allies."),
    --icon = "img/icons/magic/leader_call.png",
    sp_required = 56,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_ALLIES,

    BattleExecute = function(user, target)
        local index = 0;
        local effect_duration = user:GetVigor() * 3000;
        while true do
            local target_actor = target:GetPartyActor(index);
            if (target_actor == nil) then
                break;
            end
            --TODO: Add elemental effect
            index = index + 1;
        end
    end,
}

skills[10006] = {
    name = vt_system.Translate("Divine Wish"),
    description = vt_system.Translate("Revives an ally, restoring all his/her HP and SP."),
    --icon = "img/icons/magic/leader_call.png",
    sp_required = 100,
    warmup_time = 6000,
    cooldown_time = 1750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY_EVEN_DEAD,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (target_actor:GetHitPoints() <= 0) then
            target_actor:RegisterRevive(10000);
            target_actor:RegisterHealing(10000, false);
        end
    end,
}

skills[10007] = {
    name = vt_system.Translate("Magical Poison"),
    description = vt_system.Translate("Makes an enemy lose its SP little by little."),
    --icon = "img/icons/magic/leader_call.png",
    sp_required = 40,
    warmup_time = 3000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        --TODO
    end,
}

skills[10008] = {
    name = vt_system.Translate("Steal Song"),
    description = vt_system.Translate("Steals positive effects from an enemy."),
    --icon = "img/icons/magic/leader_call.png",
    sp_required = 40,
    warmup_time = 2000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        --TODO
    end,
}

skills[10009] = {
    name = vt_system.Translate("Terrify"),
    description = vt_system.Translate("Makes a normal enemy leave."),
    --icon = "img/icons/magic/leader_call.png",
    sp_required = 12,
    warmup_time = 3000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        --TODO
    end,
}

skills[10010] = {
    name = vt_system.Translate("Decay Dirge"),
    description = vt_system.Translate("Steals both physical and magical defense of an ally."),
    --icon = "img/icons/magic/leader_call.png",
    sp_required = 24,
    warmup_time = 3000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        --TODO
    end,
}

skills[10011] = {
    name = vt_system.Translate("War God"),
    description = vt_system.Translate("Steals both the physical and magical defense of all enemies and gives it to the party."),
    --icon = "img/icons/magic/leader_call.png",
    sp_required = 96,
    warmup_time = 3000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_FOES,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        --TODO
    end,
}

-------------------------------------------------------------
-- 10100 - 10199 - Shards skills, obtained through equipping shards to equipment
-------------------------------------------------------------

skills[10100] = {
    name = vt_system.Translate("Fire burst"),
    description = vt_system.Translate("Creates a small fire that burns an enemy."),
    icon = "img/icons/magic/fire_burst.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.CalculateStandardEvasion(target) == false) then
            -- TODO: Set fire elemental damage type based on character stats
            target_actor:RegisterDamage(vt_battle.CalculateMagicalDamageAdder(user, target, 30), target);
            -- trigger the fire effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            Battle:TriggerBattleParticleEffect("dat/effects/particles/fire_spell.lua",
                    target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            AudioManager:PlaySound("snd/fire1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10101] = {
    name = vt_system.Translate("Fire Circle"),
    description = vt_system.Translate("Increases the Fire elemental defense on a character."),
    icon = "img/icons/magic/fire_circle.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10102] = {
    name = vt_system.Translate("Lava"),
    description = vt_system.Translate("Magical attack combining Earth and Fire damages."),
    icon = "img/icons/magic/lava.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10103] = {
    name = vt_system.Translate("Fog"),
    description = vt_system.Translate("Increases the evade rate of an ally."),
    icon = "img/icons/magic/fog.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10104] = {
    name = vt_system.Translate("Fire Breath"),
    description = vt_system.Translate("Magical attack combining Air and Fire damages."),
    icon = "img/icons/magic/fire_breath.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10105] = {
    name = vt_system.Translate("Focus"),
    description = vt_system.Translate("Increases the Magical Defence of an ally."),
    icon = "img/icons/magic/focus.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10106] = {
    name = vt_system.Translate("Holy Fire"),
    description = vt_system.Translate("Magical attack combining Life and Fire damages."),
    icon = "img/icons/magic/holy_fire.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10107] = {
    name = vt_system.Translate("Life"),
    description = vt_system.Translate("Revives a fallen ally."),
    icon = "img/icons/magic/life.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10108] = {
    name = vt_system.Translate("Petrify"),
    description = vt_system.Translate("Turns an enemy into stone."),
    icon = "img/icons/magic/petrify.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10109] = {
    name = vt_system.Translate("Rage"),
    description = vt_system.Translate("Enrages an ally, increasing its Attack and Speed, but reducing its Defence."),
    icon = "img/icons/magic/rage.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10110] = {
    name = vt_system.Translate("Rock Fall"),
    description = vt_system.Translate("Rocks fall on an enemy."),
    icon = "img/icons/magic/rock_fall.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.CalculateStandardEvasion(target) == false) then
            -- TODO: Set Earth elemental damage type based on character stats
            target_actor:RegisterDamage(vt_battle.CalculateMagicalDamageAdder(user, target, 45), target);
            -- trigger the fire effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            --Battle:TriggerBattleParticleEffect("dat/effects/particles/wave_spell.lua",
            --        target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            --AudioManager:PlaySound("snd/wave1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10111] = {
    name = vt_system.Translate("Earth Circle"),
    description = vt_system.Translate("Increases the Earth elemental defense on a character."),
    icon = "img/icons/magic/earth_circle.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        -- TODO: Set elemental defense type based on character stats
        -- trigger the effect slightly *above* the sprite to make it appear *below* it from the player's point of view.
        local Battle = ModeManager:GetTop();
        Battle:TriggerBattleParticleEffect("dat/effects/particles/earth_circle.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() - 5);
        -- trigger the 2nd effect slightly below the sprite to make it appear above it from the player's point of view.
        Battle:TriggerBattleParticleEffect("dat/effects/particles/earth_circle_outer_particles.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
        --AudioManager:PlaySound("snd/circle_spell.ogg");
    end,
}

skills[10112] = {
    name = vt_system.Translate("Entangle"),
    description = vt_system.Translate("Decreases the evade rate of an enemy."),
    icon = "img/icons/magic/entangle.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10113] = {
    name = vt_system.Translate("Magical Armor"),
    description = vt_system.Translate("Increases the Magical Defence of an ally."),
    icon = "img/icons/magic/magical_armor.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10114] = {
    name = vt_system.Translate("Haste"),
    description = vt_system.Translate("Increases the Battle Speed of an ally."),
    icon = "img/icons/magic/haste.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10115] = {
    name = vt_system.Translate("Moon"),
    description = vt_system.Translate("Makes an enemy attack its own allies."),
    icon = "img/icons/magic/moon.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10116] = {
    name = vt_system.Translate("Cure"),
    description = vt_system.Translate("Cures the poison from an ally."),
    icon = "img/icons/magic/cure.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10117] = {
    name = vt_system.Translate("Drain"),
    description = vt_system.Translate("Steals HP from an enemy."),
    icon = "img/icons/monster_attacks/animal-fangs.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10118] = {
    name = vt_system.Translate("Curse"),
    description = vt_system.Translate("Severely decreases several random stats of an enemy."),
    icon = "img/icons/magic/curse.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10119] = {
    name = vt_system.Translate("Meteor"),
    description = vt_system.Translate("High Earth damages on all foes."),
    icon = "img/icons/magic/meteor.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_FOES,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10120] = {
    name = vt_system.Translate("Wave"),
    description = vt_system.Translate("Makes waves fall on an enemy."),
    icon = "img/icons/magic/wave.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.CalculateStandardEvasion(target) == false) then
            -- TODO: Set water elemental damage type based on character stats
            target_actor:RegisterDamage(vt_battle.CalculateMagicalDamageAdder(user, target, 45), target);
            -- trigger the fire effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            Battle:TriggerBattleParticleEffect("dat/effects/particles/wave_spell.lua",
                    target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            AudioManager:PlaySound("snd/wave1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10121] = {
    name = vt_system.Translate("Water Circle"),
    description = vt_system.Translate("Increases the Water elemental defense on a character."),
    icon = "img/icons/magic/water_circle.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        -- TODO: Set elemental defense type based on character stats
        -- trigger the effect slightly *above* the sprite to make it appear *below* it from the player's point of view.
        local Battle = ModeManager:GetTop();
        Battle:TriggerBattleParticleEffect("dat/effects/particles/water_circle.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() - 5);
        -- trigger the 2nd effect slightly below the sprite to make it appear above it from the player's point of view.
        Battle:TriggerBattleParticleEffect("dat/effects/particles/water_circle_outer_particles.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
        --AudioManager:PlaySound("snd/circle_spell.ogg");
    end,
}

skills[10122] = {
    name = vt_system.Translate("Whirlwind"),
    description = vt_system.Translate("Magical attack combining Water and Air damages."),
    icon = "img/icons/magic/whirlwind.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10123] = {
    name = vt_system.Translate("Slow"),
    description = vt_system.Translate("Decreases the battle speed of an enemy."),
    icon = "img/icons/magic/slow.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10124] = {
    name = vt_system.Translate("Blindness"),
    description = vt_system.Translate("Prevents an enemy from succeeding any physical attacks."),
    icon = "img/icons/magic/blindness.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10125] = {
    name = vt_system.Translate("Heal"),
    description = vt_system.Translate("Restores Health of an ally."),
    icon = "img/icons/magic/heal.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}


skills[10126] = {
    name = vt_system.Translate("Poison"),
    description = vt_system.Translate("Inflicts poison to an enemy."),
    icon = "img/icons/magic/poison.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10127] = {
    name = vt_system.Translate("Stun"),
    description = vt_system.Translate("Stuns an enemy."),
    --icon = "img/icons/magic/fireball.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10128] = {
    name = vt_system.Translate("Tsunami"),
    description = vt_system.Translate("High Water damages on all foes."),
    --icon = "img/icons/magic/fireball.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_FOES,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10129] = {
    name = vt_system.Translate("Healing Rain"),
    description = vt_system.Translate("Cures health of all allies."),
    icon = "img/icons/magic/heal_rain.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_FOES,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10130] = {
    name = vt_system.Translate("Wind"),
    description = vt_system.Translate("The wind slashes an enemy."),
    --icon = "img/icons/magic/fireball.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.CalculateStandardEvasion(target) == false) then
            -- TODO: Set Air elemental damage type based on character stats
            target_actor:RegisterDamage(vt_battle.CalculateMagicalDamageAdder(user, target, 45), target);
            -- trigger the fire effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            --Battle:TriggerBattleParticleEffect("dat/effects/particles/wave_spell.lua",
            --        target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            --AudioManager:PlaySound("snd/wave1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10131] = {
    name = vt_system.Translate("Wind Circle"),
    description = vt_system.Translate("Increases the Air elemental defense on an ally."),
    icon = "img/icons/magic/wind_circle.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        -- TODO: Set elemental defense type based on character stats
        -- trigger the effect slightly *above* the sprite to make it appear *below* it from the player's point of view.
        local Battle = ModeManager:GetTop();
        Battle:TriggerBattleParticleEffect("dat/effects/particles/wind_circle.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() - 5);
        -- trigger the 2nd effect slightly below the sprite to make it appear above it from the player's point of view.
        Battle:TriggerBattleParticleEffect("dat/effects/particles/wind_circle_outer_particles.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
        --AudioManager:PlaySound("snd/circle_spell.ogg");
    end,
}

skills[10132] = {
    name = vt_system.Translate("Lightning"),
    description = vt_system.Translate("Magical attack combining Life and Air damages."),
    icon = "img/icons/magic/lightning.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10133] = {
    name = vt_system.Translate("Regen"),
    description = vt_system.Translate("Makes an ally recover slowly its health."),
    icon = "img/icons/magic/regen.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10134] = {
    name = vt_system.Translate("Magic Drain"),
    description = vt_system.Translate("Steals Skill Points from an enemy."),
    icon = "img/icons/magic/magic_drain.png",
    sp_required = 1,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10135] = {
    name = vt_system.Translate("Silence"),
    description = vt_system.Translate("Prevents an enemy from using any magic skills."),
    --icon = "img/icons/magic/fireball.png",
    sp_required = 1,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10136] = {
    name = vt_system.Translate("Storm"),
    description = vt_system.Translate("High Air damages on all foes."),
    icon = "img/icons/magic/storm.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_FOES,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10137] = {
    name = vt_system.Translate("Stop"),
    description = vt_system.Translate("Stops an enemy for quite a long time."),
    --icon = "img/icons/magic/fireball.png",
    sp_required = 1,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10140] = {
    name = vt_system.Translate("Holy"),
    description = vt_system.Translate("A holy light strikes an enemy."),
    icon = "img/icons/magic/holy.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.CalculateStandardEvasion(target) == false) then
            -- TODO: Set Life elemental damage type based on character stats
            target_actor:RegisterDamage(vt_battle.CalculateMagicalDamageAdder(user, target, 45), target);
            -- trigger the fire effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            --Battle:TriggerBattleParticleEffect("dat/effects/particles/wave_spell.lua",
            --        target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            --AudioManager:PlaySound("snd/wave1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10141] = {
    name = vt_system.Translate("Life Circle"),
    description = vt_system.Translate("Increases the Life elemental defense on a character."),
    icon = "img/icons/magic/life_circle.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10142] = {
    name = vt_system.Translate("Purify"),
    description = vt_system.Translate("Cures all bad effects on a character, except the poison."),
    --icon = "img/icons/magic/fireball.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10143] = {
    name = vt_system.Translate("Sun Haze"),
    description = vt_system.Translate("High Life damages on all foes."),
    icon = "img/icons/magic/sun_haze.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_FOES,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10144] = {
    name = vt_system.Translate("Aura"),
    description = vt_system.Translate("Boost an ally speed and recovers slowly its health."),
    --icon = "img/icons/magic/fireball.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10150] = {
    name = vt_system.Translate("Death"),
    description = vt_system.Translate("Death tries and take an enemy with it."),
    icon = "img/icons/magic/death.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.CalculateStandardEvasion(target) == false) then
            -- TODO: Set Death elemental damage type based on character stats
            target_actor:RegisterDamage(vt_battle.CalculateMagicalDamageAdder(user, target, 45), target);
            -- trigger the fire effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            --Battle:TriggerBattleParticleEffect("dat/effects/particles/wave_spell.lua",
            --        target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            --AudioManager:PlaySound("snd/wave1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10151] = {
    name = vt_system.Translate("Death Circle"),
    description = vt_system.Translate("Increases the Death elemental defense on an ally."),
    icon = "img/icons/magic/death_circle.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10152] = {
    name = vt_system.Translate("Doom"),
    description = vt_system.Translate("Adds a Death counter upon the enemy's head."),
    icon = "img/icons/magic/doom.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10153] = {
    name = vt_system.Translate("Magical Reflection"),
    description = vt_system.Translate("Makes the next enemy spell targeting the ally be cast upon itself."),
    --icon = "img/icons/magic/fireball.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10160] = {
    name = vt_system.Translate("Dispell"),
    description = vt_system.Translate("Removes any status effect, elemental effect, and active spell on an enemy."),
    --icon = "img/icons/magic/fireball.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.CalculateStandardEvasion(target) == false) then
            -- TODO: Set Neutral elemental damage type based on character stats
            target_actor:RegisterDamage(vt_battle.CalculateMagicalDamageAdder(user, target, 45), target);
            -- trigger the Neutral effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            --Battle:TriggerBattleParticleEffect("dat/effects/particles/wave_spell.lua",
            --        target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            --AudioManager:PlaySound("snd/wave1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10161] = {
    name = vt_system.Translate("Shell"),
    description = vt_system.Translate("Increases all elemental effects on one character."),
    --icon = "img/icons/magic/fireball.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10162] = {
    name = vt_system.Translate("?"),
    description = vt_system.Translate("A mystic spell happening when offensively combining opposite elementals."),
    icon = "img/icons/magic/antipode.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10163] = {
    name = vt_system.Translate("Counter"),
    description = vt_system.Translate("Makes an ally counter-attack if hurt."),
    icon = "img/icons/magic/counter.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10164] = {
    name = vt_system.Translate("Phoenix Call"),
    description = vt_system.Translate("Revives all allies."),
    icon = "img/icons/magic/phoenix_call.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_ALLIES,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}

skills[10165] = {
    name = vt_system.Translate("Bio"),
    description = vt_system.Translate("Cures poison on all characters."),
    icon = "img/icons/magic/bio.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_ALLIES,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
    end,
}
