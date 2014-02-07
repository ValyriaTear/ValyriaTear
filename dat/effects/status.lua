------------------------------------------------------------------------------[[
-- Filename: status.lua
--
-- Description: This file contains the implementations of all status effects.
-- The list of different types of status effects and their
-- corresponding names may be found in src/modes/common/global/global_effects.*.
--
-- Each status effect implementation requires the following data to be defined.
-- {name} - The name of the status effect as it will be shown to the player
-- {default_duration} - The default duration that the effect lasts, in milliseconds
-- {update_every} - Tells the time the effect is waiting before calling the Update() function again, in milliseconds.
-- {BattleApply} - A function executed when the status effect is applied to the target
-- {BattleUpdate} - A function executed periodically while the status is still in effect
-- {BattleUpdatePassive} - A function executed periodically while the passive (from equipment) status is in effect in battles.
-- {BattleRemove} - A function executed when the status effect is no longer active on the target

-- {MapApply} - A function executed when the status effect is applied to the target
-- {MapUpdate} - A function executed periodically while the status is still in effect
-- {MapUpdatePassive} - A function executed periodically while the passive (from equipment) status is in effect in the map mode.
-- {MapRemove} - A function executed when the status effect is no longer active on the target

-- {ApplyPassive} - This function is called when equipping and should be used on the global actor, not on the battle one.
-- {RemovePassive} - function is called when unequipping and should be used on the global actor, not on the battle one.
--
-- To verify what a status effect's icon_index should be, examine the image file
-- img/icons/effects/status.png and find the appropriate row of icons.
--
-- The Apply, Update, and Remove functions are all called with an argument, {battle_effect},
-- which is a pointer to the BattleStatusEffect object that was constructed to represent
-- this status. Use this object to access data relevant to the status effect. Most
-- implementations of these functions will want to grab pointers to the following pieces of
-- data.
--
-- battle_effect:GetTimer() - returns the BattleTimer object for the effect
-- battle_effect:GetAffectedActor() - returns the BattleActor object that the effect is active on
-- battle_effect:GetIntensity() - returns the current intensity of the active effect
-- battle_effect:HasIntensityChanged() - returns true if the intensity level has undergone a recent change.
------------------------------------------------------------------------------]]

-- All item definitions are stored in this table
if (status_effects == nil) then
    status_effects = {}
end

status_effects.GetStatModifier = function(intensity)
        local attribute_modifier = 1;

        if (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
            attribute_modifier = 1;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
            attribute_modifer = 1.1;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
            attribute_modifier = 1.2;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
            attribute_modifier = 1.3;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
            attribute_modifier = 1.4;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER) then
            attribute_modifier = 0.9;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE) then
            attribute_modifier = 0.8;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_GREATER) then
            attribute_modifier = 0.7;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_EXTREME) then
            attribute_modifier = 0.6;
        else
            print("Lua warning: status effect had an invalid intensity value: " .. intensity);
        end

        return attribute_modifier;
end

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_STRENGTH] = {
    name = vt_system.Translate("Strength"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_STRENGTH]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_STRENGTH]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    BattleRemove = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        battle_actor:SetStrengthModifier(1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        battle_actor:SetStrengthModifier(attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetStrengthModifier(modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetStrengthModifier(1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_VIGOR] = {
    name = vt_system.Translate("Vigor"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_VIGOR]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_VIGOR]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    BattleRemove = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        battle_actor:SetVigorModifier(1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        battle_actor:SetVigorModifier(attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetVigorModifier(modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetVigorModifier(1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_FORTITUDE] = {
    name = vt_system.Translate("Fortitude"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_FORTITUDE]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_FORTITUDE]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    BattleRemove = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        battle_actor:SetFortitudeModifier(1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        battle_actor:SetFortitudeModifier(attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetFortitudeModifier(modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetFortitudeModifier(1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_PROTECTION] = {
    name = vt_system.Translate("Protection"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_PROTECTION]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_PROTECTION]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    BattleRemove = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        battle_actor:SetProtectionModifier(1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        battle_actor:SetProtectionModifier(attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetProtectionModifier(modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetProtectionModifier(1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_AGILITY] = {
    name = vt_system.Translate("Agility"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_AGILITY]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_AGILITY]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    -- Note: This modifies the actor's idle state wait time accordingly.
    BattleRemove = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        battle_actor:SetAgilityModifier(1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);

        -- Note: This modifies the actor's idle state wait time accordingly.
        battle_actor:SetAgilityModifier(attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetAgilityModifier(modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetAgilityModifier(1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_EVADE] = {
    name = vt_system.Translate("Evasion"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_EVADE]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_EVADE]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    BattleRemove = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        battle_actor:SetEvadeModifier(1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        battle_actor:SetEvadeModifier(attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetEvadeModifier(modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetEvadeModifier(1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_HP] = {
    name = vt_system.Translate("HP"),
    default_duration = 30000,
    -- Applies on character only every 9 seconds
    update_every = 9000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        -- Nothing to do.
    end,

    -- Generic function for updates on the battle mode
    _ApplyHPEffectOnBattleActor = function(battle_actor, intensity)
        if (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
            global_actor:RegisterHealing(1, true);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
            battle_actor:RegisterHealing(2, true);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
            battle_actor:RegisterHealing(4, true);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
            battle_actor:RegisterHealing(8, true);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER) then
            battle_actor:RegisterDamage(2);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE) then
            battle_actor:RegisterDamage(4);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_GREATER) then
            battle_actor:RegisterDamage(8);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_EXTREME) then
            battle_actor:RegisterDamage(16);
        end
    end,

    BattleUpdate = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_HP]._ApplyHPEffectOnBattleActor(battle_actor, intensity);
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_HP]._ApplyHPEffectOnBattleActor(battle_actor, intensity);
    end,

    BattleRemove = function(battle_effect)
        -- Nothing to do.
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        -- Nothing to do.
    end,

    RemovePassive = function(global_actor)
        -- Nothing to do.
    end,

    -- Generic function for updates on the map mode
    _ApplyHPEffectOnCharacter = function(global_character, intensity)
        -- Test whether the character is dead.
        -- Dead characters can neither regen nor take HP damages.
        if (global_character:GetHitPoints() == 0) then
            return;
        end

        local hp_change = 0;
        if (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
            hp_change = 1;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
            hp_change = 2;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
            hp_change = 4;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
            hp_change = 8;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER) then
            hp_change = -2;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE) then
            hp_change = -4;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_GREATER) then
            hp_change = -8;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_EXTREME) then
            hp_change = -16;
        end
        local hp_diff = global_character:GetHitPoints() + hp_change;
        -- Don't kill the character or set more than its max HP.
        if (hp_diff <= 0) then
            hp_change = global_character:GetHitPoints() - 1;
            if (hp_change == 0) then return end
        elseif (hp_diff > global_character:GetMaxHitPoints()) then
            hp_change = global_character:GetMaxHitPoints() - global_character:GetHitPoints();
            if (hp_change == 0) then return end
        end
        global_character:SetHitPoints(hp_change);

        -- Adds an effect on map
        local map_mode = ModeManager:GetTop();
        local x_pos = map_mode:GetScreenXCoordinate(map_mode.camera:GetXPosition());
        local y_pos = map_mode:GetScreenYCoordinate(map_mode.camera:GetYPosition());
        local map_indicator = map_mode:GetIndicatorSupervisor();
        if (hp_change > 0) then
            -- We move the healing indicator above the head of the sprite.
            y_pos = y_pos - map_mode.camera:GetImgHeight() * 16;
            map_indicator:AddHealingIndicator(x_pos, y_pos, hp_change, vt_video.TextStyle("text22", vt_video.Color(0.0, 0.0, 1.0, 0.9)), true);
        else
            map_indicator:AddDamageIndicator(x_pos, y_pos, hp_change, vt_video.TextStyle("text22", vt_video.Color(1.0, 0.0, 0.0, 0.9)), true);
        end

    end,

    MapUpdatePassive = function(global_character, intensity)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_HP]._ApplyHPEffectOnCharacter(global_character, intensity);
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        local global_character = map_effect:GetAffectedCharacter();
        local intensity = map_effect:GetIntensity();
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_HP]._ApplyHPEffectOnCharacter(global_character, intensity);
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_SP] = {
    name = vt_system.Translate("SP"),
    default_duration = 30000,
    -- Applies on character only every 9 seconds
    update_every = 9000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        -- Nothing to do.
    end,

    -- Generic function for updates on the battle mode
    _ApplySPEffectOnBattleActor = function(battle_actor, intensity)
        if (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
            battle_actor:RegisterHealing(1, false);
            return
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
            battle_actor:RegisterHealing(2, false);
            return
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
            battle_actor:RegisterHealing(4, false);
            return
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
            battle_actor:RegisterHealing(8, false);
            return
        end

        -- Don't remove skill points when there aren't anymore
        if (battle_actor:GetSkillPoints() == 0) then
            return
        end

        if (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER) then
            battle_actor:RegisterSPDamage(1);
            return
        end

        local sp_damage = 2;
        if (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE) then
            -- sp damage already equal to 2.
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_GREATER) then
            sp_damage = 4
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_EXTREME) then
            sp_damage = 8
        end

        if (battle_actor:GetSkillPoints() < sp_damage) then
            sp_damage = battle_actor:GetSkillPoints();
        end
        battle_actor:RegisterSPDamage(sp_damage);
    end,

    BattleUpdate = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_SP]._ApplySPEffectOnBattleActor(battle_actor, intensity);
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_SP]._ApplySPEffectOnBattleActor(battle_actor, intensity);
    end,

    BattleRemove = function(battle_effect)
        -- Nothing to do.
    end,

    -- Generic passive functions (used with equipment changes)
    ApplyPassive = function(global_actor, intensity)
        -- Nothing to do.
    end,

    RemovePassive = function(global_actor)
        -- Nothing to do.
    end,

    -- Generic function for updates on the map mode
    _ApplySPEffectOnCharacter = function(global_character, intensity)

        local sp_change = 0;
        if (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
            sp_change = 1;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
            sp_change = 2;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
            sp_change = 4;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
            sp_change = 8;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER) then
            sp_change = -1;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE) then
            sp_change = -2;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_GREATER) then
            sp_change = -4;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_EXTREME) then
            sp_change = -8;
        end
        local sp_diff = global_character:GetSkillPoints() + sp_change;
        -- Don't kill the character or set more than its max HP.
        if (sp_diff <= 0) then
            sp_change = global_character:GetSkillPoints() - 1;
            if (sp_change == 0) then return end
        elseif (sp_diff > global_character:GetMaxSkillPoints()) then
            sp_change = global_character:GetMaxSkillPoints() - global_character:GetSkillPoints();
            if (sp_change == 0) then return end
        end
        global_character:SetSkillPoints(sp_change);

        -- Adds an effect on map
        local map_mode = ModeManager:GetTop();
        local x_pos = map_mode:GetScreenXCoordinate(map_mode.camera:GetXPosition());
        local y_pos = map_mode:GetScreenYCoordinate(map_mode.camera:GetYPosition());
        local map_indicator = map_mode:GetIndicatorSupervisor();
        if (sp_change > 0) then
            -- We move the healing indicator above the head of the sprite.
            y_pos = y_pos - map_mode.camera:GetImgHeight() * 16;
            map_indicator:AddHealingIndicator(x_pos, y_pos, sp_change, vt_video.TextStyle("text22", vt_video.Color(0.0, 0.8, 0.8, 0.9)), true);
        else
            map_indicator:AddDamageIndicator(x_pos, y_pos, sp_change, vt_video.TextStyle("text22", vt_video.Color(0.0, 1.0, 0.0, 0.9)), true);
        end

    end,

    MapUpdatePassive = function(global_character, intensity)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_SP]._ApplySPEffectOnCharacter(global_character, intensity);
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        local global_character = map_effect:GetAffectedCharacter();
        local intensity = map_effect:GetIntensity();
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_SP]._ApplySPEffectOnCharacter(global_character, intensity);
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_PARALYSIS] = {
    name = vt_system.Translate("Paralysis"),
    default_duration = 10000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        -- Let's stun only when the effect is negative
        if (battle_effect:GetIntensity() < vt_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
            local battle_actor = battle_effect:GetAffectedActor();
            battle_actor:SetStunned(true);
        end
    end,

    BattleUpdate = function(battle_effect)
        -- Nothing needs to be updated for this effect
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Let's stun only when the effect is negative
        if (intensity < vt_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL
                and battle_actor:IsStunned() == false) then
            battle_actor:SetStunned(true);
        end
    end,

    BattleRemove = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        battle_actor:SetStunned(false);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        -- Nothing to do.
    end,

    RemovePassive = function(global_actor)
        -- Nothing to do.
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

-- Elemental status effects
status_effects[vt_global.GameGlobal.GLOBAL_STATUS_FIRE] = {
    name = vt_system.Translate("Fire Elemental Strength"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_FIRE]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_FIRE]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    BattleRemove = function(battle_effect)
        battle_effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_FIRE, 1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        battle_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_FIRE, attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_FIRE, attribute_modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_FIRE, 1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

-- Water
status_effects[vt_global.GameGlobal.GLOBAL_STATUS_WATER] = {
    name = vt_system.Translate("Water Elemental Strength"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_WATER]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_WATER]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    BattleRemove = function(battle_effect)
        battle_effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_WATER, 1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        battle_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_WATER, attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_WATER, attribute_modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_WATER, 1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_VOLT] = {
    name = vt_system.Translate("Wind Elemental Strength"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_VOLT]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_VOLT]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    BattleRemove = function(battle_effect)
        battle_effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_VOLT, 1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        battle_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_VOLT, attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_VOLT, attribute_modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_VOLT, 1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_EARTH] = {
    name = vt_system.Translate("Earth Elemental Strength"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_EARTH]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_EARTH]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    BattleRemove = function(battle_effect)
        battle_effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_EARTH, 1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        battle_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_EARTH, attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_EARTH, attribute_modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_EARTH, 1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_LIFE] = {
    name = vt_system.Translate("Life Elemental Strength"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_LIFE]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_LIFE]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    BattleRemove = function(battle_effect)
        battle_effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_LIFE, 1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        battle_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_LIFE, attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_LIFE, attribute_modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_LIFE, 1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_DEATH] = {
    name = vt_system.Translate("Death Elemental Strength"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_DEATH]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_DEATH]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    BattleRemove = function(battle_effect)
        battle_effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_DEATH, 1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        battle_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_DEATH, attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_DEATH, attribute_modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_DEATH, 1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}

-- Neutral (default magical strength)
status_effects[vt_global.GameGlobal.GLOBAL_STATUS_NEUTRAL] = {
    name = vt_system.Translate("Neutral Elemental Strength"),
    default_duration = 30000,

    -- Battle status effects related functions
    BattleApply = function(battle_effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_NEUTRAL]._ModifyAttribute(battle_effect);
    end,

    BattleUpdate = function(battle_effect)
        if (battle_effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_NEUTRAL]._ModifyAttribute(battle_effect);
        end
    end,

    BattleUpdatePassive = function(battle_actor, intensity)
        -- Nothing to do
    end,

    BattleRemove = function(battle_effect)
        battle_effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL, 1.0);
    end,

    _ModifyAttribute = function(battle_effect)
        local battle_actor = battle_effect:GetAffectedActor();
        local intensity = battle_effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        battle_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL, attribute_modifier);
    end,

    -- Generic passive functions (used with equipment)
    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL, attribute_modifier);
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL, 1.0);
    end,

    MapUpdatePassive = function(global_character, intensity)
        -- Nothing to do here
    end,

    MapApply = function(map_effect)
        -- Nothing to do here
    end,

    MapUpdate = function(map_effect)
        -- Nothing to do here
    end,

    MapRemove = function(map_effect)
        -- Nothing to do here
    end
}
