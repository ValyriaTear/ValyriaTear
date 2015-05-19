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
    icon = "data/skills/magic/shield.png",
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
        target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_FORTITUDE,
                                             vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                             effect_duration);
        local Battle = ModeManager:GetTop();
        if (target_actor:GetSpriteHeight() > 250.0) then
            -- Big sprite version
            Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/shield_big_sprites.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
        else
            Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/shield.lua",
                    target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
        end
        AudioManager:PlaySound("data/sounds/defence1_spell.ogg");
    end,

    FieldExecute = function(user, target) -- GlobalCharacter*
        if (target:IsAlive() == false) then
            return false;
        end

        local effect_duration = user:GetProtection() * 2000;
        if (effect_duration < 10000) then effect_duration = 10000 end
        target:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_FORTITUDE,
                                       vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                       effect_duration);

        AudioManager:PlaySound("data/sounds/defence1_spell.ogg");
        -- Trigger a particle effect on the menu character
        local y_pos = 218.0 + GlobalManager:GetPartyPosition(target) * 118.0;
        local menu_mode = ModeManager:GetTop();
        menu_mode:GetParticleManager():AddParticleEffect("data/visuals/particle_effects/shield.lua", 145.0, y_pos);
        return true;
    end
}

skills[10002] = {
    name = vt_system.Translate("First Aid"),
    description = vt_system.Translate("Performs basic medical assistance, healing the target by a minor degree."),
    icon = "data/skills/magic/first_aid.png",
    sp_required = 4,
    warmup_time = 1500,
    cooldown_time = 200,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (target_actor:IsAlive() == false) then
            return false;
        end

        local hit_points = (user:GetVigor() * 3) +  vt_utils.RandomBoundedInteger(0, 15);
        target_actor:RegisterHealing(hit_points, true);
        AudioManager:PlaySound("data/sounds/heal_spell.wav");
        local Battle = ModeManager:GetTop();
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/heal_particle.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
    end,

    FieldExecute = function(user, target)
        if (target:IsAlive() == false) then
            return false;
        end
        -- Don't heal characters with max HP.
        if (target:GetHitPoints() == target:GetMaxHitPoints()) then
            return false;
        end

        target:AddHitPoints((user:GetVigor() * 5) + vt_utils.RandomBoundedInteger(0, 30));
        AudioManager:PlaySound("data/sounds/heal_spell.wav");
        return true;
    end
}

skills[10003] = {
    name = vt_system.Translate("Leader Call"),
    description = vt_system.Translate("Temporarily increases the strength of all allies."),
    icon = "data/skills/magic/leader_call.png",
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
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STRENGTH,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER,
                                                 effect_duration);
            index = index + 1;
        end
    end,
}

skills[10004] = {
    name = vt_system.Translate("Holy Veil"),
    description = vt_system.Translate("Increases the attack and magical attack of all allies."),
    --icon = "data/skills/magic/leader_call.png",
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
            if (target_actor:IsAlive() == true) then
                target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STRENGTH,
                                                     vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                                     effect_duration);
                target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_VIGOR,
                                                     vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                                     effect_duration);
            end
            index = index + 1;
        end
    end,
}

skills[10005] = {
    name = vt_system.Translate("Ancient Strength"),
    description = vt_system.Translate("Increases every form of elemental resistance for all allies."),
    --icon = "data/skills/magic/leader_call.png",
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
    description = vt_system.Translate("Revives an ally, restoring all of their HP and SP."),
    --icon = "data/skills/magic/leader_call.png",
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
    --icon = "data/skills/magic/leader_call.png",
    sp_required = 40,
    warmup_time = 3000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (target_actor:IsAlive() == true) then
            local effect_duration = user:GetProtection() * 5000;
            if (effect_duration < 10000) then effect_duration = 10000 end
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_SP,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE,
                                                 effect_duration);
            --local Battle = ModeManager:GetTop();
            --AudioManager:PlaySound("data/sounds/defence1_spell.ogg");
        end
    end,
}

skills[10008] = {
    name = vt_system.Translate("Steal Song"),
    description = vt_system.Translate("Steals positive effects from an enemy."),
    --icon = "data/skills/magic/leader_call.png",
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
    description = vt_system.Translate("Potentially causes a normal enemy to retreat."),
    --icon = "data/skills/magic/leader_call.png",
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
    --icon = "data/skills/magic/leader_call.png",
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
    --icon = "data/skills/magic/leader_call.png",
    sp_required = 96,
    warmup_time = 3000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_FOES,

    BattleExecute = function(user, target)
        local index = 0;
        local effect_duration = user:GetVigor() * 3000;
        while true do
            local target_actor = target:GetPartyActor(index);
            if (target_actor == nil) then
                break;
            end
            if (target_actor:IsAlive() == true) then
                target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_FORTITUDE,
                                                     vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE,
                                                     effect_duration);
                target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_PROTECTION,
                                                     vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE,
                                                     effect_duration);
            end
            index = index + 1;
        end
        -- TODO: Give it to the party
    end,
}

-------------------------------------------------------------
-- 10100 - 10199 - Shards skills, obtained through equipping shards to equipment
-------------------------------------------------------------

skills[10100] = {
    name = vt_system.Translate("Fire burst"),
    description = vt_system.Translate("Creates a small fire that burns an enemy."),
    icon = "data/skills/magic/fire_burst.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.StdRndEvade(target_actor) == false) then
            target_actor:RegisterDamage(vt_battle.RndMagicalDamage(user, target_actor, vt_global.GameGlobal.GLOBAL_ELEMENTAL_FIRE, 30, 1.0, -1), target);
            -- trigger the fire effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/fire_spell.lua",
                    target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            AudioManager:PlaySound("data/sounds/fire1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10101] = {
    name = vt_system.Translate("Fire Circle"),
    description = vt_system.Translate("Increases the fire elemental defense on a character."),
    icon = "data/skills/magic/fire_circle.png",
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
    description = vt_system.Translate("Magical attack combining earth and fire type damage."),
    icon = "data/skills/magic/lava.png",
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
    icon = "data/skills/magic/fog.png",
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
    description = vt_system.Translate("Magical attack combining air and fire type damage."),
    icon = "data/skills/magic/fire_breath.png",
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
    description = vt_system.Translate("Increases the magical defence of an ally."),
    icon = "data/skills/magic/focus.png",
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
    description = vt_system.Translate("Magical attack combining life and fire damages."),
    icon = "data/skills/magic/holy_fire.png",
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
    icon = "data/skills/magic/life.png",
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
    icon = "data/skills/magic/petrify.png",
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
    description = vt_system.Translate("Enrages an ally, increasing their attack and speed but reducing their defence."),
    icon = "data/skills/magic/rage.png",
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
    icon = "data/skills/magic/rock_fall.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.StdRndEvade(target_actor) == false) then
            target_actor:RegisterDamage(vt_battle.RndMagicalDamage(user, target_actor, vt_global.GameGlobal.GLOBAL_ELEMENTAL_EARTH, 45, 1.0, -1), target);
            -- trigger the fire effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            --Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/wave_spell.lua",
            --        target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            --AudioManager:PlaySound("data/sounds/wave1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10111] = {
    name = vt_system.Translate("Earth Circle"),
    description = vt_system.Translate("Increases the earth elemental defense of a character."),
    icon = "data/skills/magic/earth_circle.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        -- TODO : Balance this
        local effect_duration = user:GetVigor() * 3000;
        target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_EARTH,
                                             vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                             effect_duration);
        -- trigger the effect slightly *above* the sprite to make it appear *below* it from the player's point of view.
        local Battle = ModeManager:GetTop();
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/earth_circle.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() - 5);
        -- trigger the 2nd effect slightly below the sprite to make it appear above it from the player's point of view.
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/earth_circle_outer_particles.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
        --AudioManager:PlaySound("data/sounds/circle_spell.ogg");
    end,
}

skills[10112] = {
    name = vt_system.Translate("Entangle"),
    description = vt_system.Translate("Decreases the evasion rate of an enemy."),
    icon = "data/skills/magic/entangle.png",
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
    description = vt_system.Translate("Increases the magical defence of an ally."),
    icon = "data/skills/magic/magical_armor.png",
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
    description = vt_system.Translate("Increases the battle speed of an ally."),
    icon = "data/skills/magic/haste.png",
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
    icon = "data/skills/magic/moon.png",
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
    icon = "data/skills/magic/cure.png",
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
    icon = "data/skills/monster_attacks/animal-fangs.png",
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
    icon = "data/skills/magic/curse.png",
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
    description = vt_system.Translate("High earth damage on all foes."),
    icon = "data/skills/magic/meteor.png",
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
    icon = "data/skills/magic/wave.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.StdRndEvade(target_actor) == false) then
            target_actor:RegisterDamage(vt_battle.RndMagicalDamage(user, target_actor, vt_global.GameGlobal.GLOBAL_ELEMENTAL_WATER, 45, 1.0, -1), target);
            -- trigger the fire effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/wave_spell.lua",
                    target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            AudioManager:PlaySound("data/sounds/wave1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10121] = {
    name = vt_system.Translate("Water Circle"),
    description = vt_system.Translate("Increases the water elemental defense on a character."),
    icon = "data/skills/magic/water_circle.png",
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
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/water_circle.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() - 5);
        -- trigger the 2nd effect slightly below the sprite to make it appear above it from the player's point of view.
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/water_circle_outer_particles.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
        --AudioManager:PlaySound("data/sounds/circle_spell.ogg");
    end,
}

skills[10122] = {
    name = vt_system.Translate("Whirlwind"),
    description = vt_system.Translate("Magical attack combining water and air type damage."),
    icon = "data/skills/magic/whirlwind.png",
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
    icon = "data/skills/magic/slow.png",
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
    description = vt_system.Translate("Prevents an enemy from succeeding to strike with any physical attacks."),
    icon = "data/skills/magic/blindness.png",
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
    description = vt_system.Translate("Restores the health of an ally."),
    icon = "data/skills/magic/heal.png",
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
    description = vt_system.Translate("Inflicts poison on an enemy."),
    icon = "data/skills/magic/poison.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        -- TODO : Balance this
        local effect_duration = user:GetVigor() * 3000;

        target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_HP,
                                             vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE,
                                             effect_duration);
    end,
}

skills[10127] = {
    name = vt_system.Translate("Stun"),
    description = vt_system.Translate("Stuns an enemy."),
    --icon = "data/skills/magic/fireball.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        local Battle = ModeManager:GetTop();
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/stun_star.lua", target_actor:GetXLocation(), target_actor:GetYLocation());
        -- TODO: The rest ...
    end,
}

skills[10128] = {
    name = vt_system.Translate("Tsunami"),
    description = vt_system.Translate("High water damage inflicted on all foes."),
    --icon = "data/skills/magic/fireball.png",
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
    description = vt_system.Translate("Improves the health of all allies."),
    icon = "data/skills/magic/heal_rain.png",
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
    --icon = "data/skills/magic/fireball.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.StdRndEvade(target_actor) == false) then
            target_actor:RegisterDamage(vt_battle.RndMagicalDamage(user, target_actor, vt_global.GameGlobal.GLOBAL_ELEMENTAL_VOLT, 45, 1.0, -1), target);
            -- trigger the fire effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            --Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/wave_spell.lua",
            --        target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            --AudioManager:PlaySound("data/sounds/wave1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10131] = {
    name = vt_system.Translate("Wind Circle"),
    description = vt_system.Translate("Increases the air elemental defense of a character."),
    icon = "data/skills/magic/wind_circle.png",
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
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/wind_circle.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() - 5);
        -- trigger the 2nd effect slightly below the sprite to make it appear above it from the player's point of view.
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/wind_circle_outer_particles.lua",
                target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
        --AudioManager:PlaySound("data/sounds/circle_spell.ogg");
    end,
}

skills[10132] = {
    name = vt_system.Translate("Lightning"),
    description = vt_system.Translate("Magical attack combining life and air type damage."),
    icon = "data/skills/magic/lightning.png",
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
    description = vt_system.Translate("Causes an ally to slowly recover their health."),
    icon = "data/skills/magic/regen.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        -- TODO : Balance this
        local effect_duration = user:GetVigor() * 3000;

        target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_HP,
                                             vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                             effect_duration);
    end,
}

skills[10134] = {
    name = vt_system.Translate("Magic Drain"),
    description = vt_system.Translate("Steals skill points from an enemy."),
    icon = "data/skills/magic/magic_drain.png",
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
    --icon = "data/skills/magic/fireball.png",
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
    description = vt_system.Translate("High air type damage on all foes."),
    icon = "data/skills/magic/storm.png",
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
    description = vt_system.Translate("Stops an enemy from taking any actions for quite a long time."),
    --icon = "data/skills/magic/fireball.png",
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
    icon = "data/skills/magic/holy.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.StdRndEvade(target_actor) == false) then
            target_actor:RegisterDamage(vt_battle.RndMagicalDamage(user, target_actor, vt_global.GameGlobal.GLOBAL_ELEMENTAL_LIFE, 45, 1.0, -1), target);
            -- trigger the fire effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            --Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/wave_spell.lua",
            --        target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            --AudioManager:PlaySound("data/sounds/wave1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10141] = {
    name = vt_system.Translate("Life Circle"),
    description = vt_system.Translate("Increases the life elemental defense of a character."),
    icon = "data/skills/magic/life_circle.png",
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
    description = vt_system.Translate("Cures all bad effects on a character, except for poison."),
    --icon = "data/skills/magic/fireball.png",
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
    description = vt_system.Translate("High life type damage on all foes."),
    icon = "data/skills/magic/sun_haze.png",
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
    description = vt_system.Translate("Boost an ally's speed and slowly recovers their health."),
    --icon = "data/skills/magic/fireball.png",
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
    description = vt_system.Translate("Death tries to take hold of an enemy."),
    icon = "data/skills/magic/death.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.StdRndEvade(target_actor) == false) then
            target_actor:RegisterDamage(vt_battle.RndMagicalDamageAdder(user, target_actor, vt_global.GameGlobal.GLOBAL_ELEMENTAL_DEATH, 45, 1.0, -1), target);
            -- trigger the fire effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            --Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/wave_spell.lua",
            --        target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            --AudioManager:PlaySound("data/sounds/wave1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10151] = {
    name = vt_system.Translate("Death Circle"),
    description = vt_system.Translate("Increases the death elemental defense of an ally."),
    icon = "data/skills/magic/death_circle.png",
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
    description = vt_system.Translate("Places a death counter upon the enemy's head."),
    icon = "data/skills/magic/doom.png",
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
    description = vt_system.Translate("Makes the next enemy spell cast reflect back upon the attacker."),
    --icon = "data/skills/magic/fireball.png",
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
    --icon = "data/skills/magic/fireball.png",
    sp_required = 7,
    warmup_time = 4000,
    cooldown_time = 750,
    warmup_action_name = "magic_prepare",
    action_name = "magic_cast",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,

    BattleExecute = function(user, target)
        local target_actor = target:GetActor();
        if (vt_battle.StdRndEvade(target_actor) == false) then
            -- TODO
            -- trigger the Neutral effect slightly under the sprite to make it appear before it from the player's point of view.
            local Battle = ModeManager:GetTop();
            --Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/wave_spell.lua",
            --        target_actor:GetXLocation(), target_actor:GetYLocation() + 5);
            --AudioManager:PlaySound("data/sounds/wave1_spell.ogg");
        else
            target_actor:RegisterMiss(true);
        end
    end,
}

skills[10161] = {
    name = vt_system.Translate("Shell"),
    description = vt_system.Translate("Increases all elemental effects of one character."),
    --icon = "data/skills/magic/fireball.png",
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
    description = vt_system.Translate("An offensive mystic spell formed by combining opposite elementals."),
    icon = "data/skills/magic/antipode.png",
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
    description = vt_system.Translate("Makes an ally counter-attack if injured."),
    icon = "data/skills/magic/counter.png",
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
    icon = "data/skills/magic/phoenix_call.png",
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
    icon = "data/skills/magic/bio.png",
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
