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
-- {Apply} - A function executed when the status effect is applied to the target
-- {Update} - A function executed periodically while the status is still in effect
-- {Remove} - A function executed when the status effect is no longer active on the target

-- {ApplyPassive} - This function is called when equipping and should be used on the global actor, not on the battle one.
-- {UpdatePassive} - A function executed periodically while the passive (from equipment) status is still in effect.
-- {RemovePassive} - function is called when unequipping and should be used on the global actor, not on the battle one.
--
-- To verify what a status effect's icon_index should be, examine the image file
-- img/icons/effects/status.png and find the appropriate row of icons.
--
-- The Apply, Update, and Remove functions are all called with an argument, {effect},
-- which is a pointer to the BattleStatusEffect object that was constructed to represent
-- this status. Use this object to access data relevant to the status effect. Most
-- implementations of these functions will want to grab pointers to the following pieces of
-- data.
--
-- effect:GetTimer() - returns the BattleTimer object for the effect
-- effect:GetAffectedActor() - returns the BattleActor object that the effect is active on
-- effect:GetIntensity() - returns the current intensity of the active effect
-- effect:HasIntensityChanged() - returns true if the intensity level has undergone a recent change.
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
            attribute_modifer = 1.2;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
            attribute_modifier = 1.4;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
            attribute_modifier = 1.6;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
            attribute_modifier = 1.8;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER) then
            attribute_modifier = 0.8;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE) then
            attribute_modifier = 0.6;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_GREATER) then
            attribute_modifier = 0.4;
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_EXTREME) then
            attribute_modifier = 0.2;
        else
            print("Lua warning: status effect had an invalid intensity value: " .. intensity);
        end

        return attribute_modifier;
end

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_STRENGTH] = {
    name = vt_system.Translate("Strength"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_STRENGTH]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_STRENGTH]._ModifyAttribute(effect);
        end
    end,

    Remove = function(effect)
        local actor = effect:GetAffectedActor();
        actor:SetStrengthModifier(1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        actor:SetStrengthModifier(attribute_modifier);
    end,


    ApplyPassive = function(global_actor, intensity)
        local modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetStrengthModifier(modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetStrengthModifier(1.0);
    end,
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_VIGOR] = {
    name = vt_system.Translate("Vigor"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_VIGOR]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_VIGOR]._ModifyAttribute(effect);
        end
    end,

    Remove = function(effect)
        local actor = effect:GetAffectedActor();
        actor:SetVigorModifier(1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        actor:SetVigorModifier(attribute_modifier);
    end,

    ApplyPassive = function(global_actor, intensity)
        local modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetVigorModifier(modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetVigorModifier(1.0);
    end,
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_FORTITUDE] = {
    name = vt_system.Translate("Fortitude"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_FORTITUDE]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_FORTITUDE]._ModifyAttribute(effect);
        end
    end,

    Remove = function(effect)
        local actor = effect:GetAffectedActor();
        actor:SetFortitudeModifier(1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        actor:SetFortitudeModifier(attribute_modifier);
    end,

    ApplyPassive = function(global_actor, intensity)
        local modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetFortitudeModifier(modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetFortitudeModifier(1.0);
    end,
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_PROTECTION] = {
    name = vt_system.Translate("Protection"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_PROTECTION]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_PROTECTION]._ModifyAttribute(effect);
        end
    end,

    Remove = function(effect)
        local actor = effect:GetAffectedActor();
        actor:SetProtectionModifier(1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        actor:SetProtectionModifier(attribute_modifier);
    end,

    ApplyPassive = function(global_actor, intensity)
        local modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetProtectionModifier(modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetProtectionModifier(1.0);
    end,
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_AGILITY] = {
    name = vt_system.Translate("Agility"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_AGILITY]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_AGILITY]._ModifyAttribute(effect);
        end
    end,

    -- Note: This modifies the actor's idle state wait time accordingly.
    Remove = function(effect)
        local actor = effect:GetAffectedActor();
        actor:SetAgilityModifier(1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);

        -- Note: This modifies the actor's idle state wait time accordingly.
        actor:SetAgilityModifier(attribute_modifier);
    end,

    ApplyPassive = function(global_actor, intensity)
        local modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetAgilityModifier(modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetAgilityModifier(1.0);
    end,
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_EVADE] = {
    name = vt_system.Translate("Evasion"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_EVADE]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_EVADE]._ModifyAttribute(effect);
        end
    end,

    Remove = function(effect)
        local actor = effect:GetAffectedActor();
        actor:SetEvadeModifier(1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        actor:SetEvadeModifier(attribute_modifier);
    end,

    ApplyPassive = function(global_actor, intensity)
        local modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetEvadeModifier(modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetEvadeModifier(1.0);
    end,
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_HP] = {
    name = vt_system.Translate("HP"),
    default_duration = 30000,
    -- Regens the character only every 9 seconds
    update_every = 9000,

    Apply = function(effect)
        -- Nothing to do.
    end,

    Update = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        if (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
            actor:RegisterHealing(1, true);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
            actor:RegisterHealing(2, true);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
            actor:RegisterHealing(4, true);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
            actor:RegisterHealing(8, true);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER) then
            actor:RegisterDamage(2);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE) then
            actor:RegisterDamage(4);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_GREATER) then
            actor:RegisterDamage(8);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_EXTREME) then
            actor:RegisterDamage(16);
        end
    end,

    Remove = function(effect)
        -- Nothing to do.
    end,

    ApplyPassive = function(global_actor, intensity)
        -- Nothing to do.
    end,

    UpdatePassive = function(effect)
        -- TODO: Needs first a status effect handler for the map mode and a passive effect handler in the battle mode.
    end,

    RemovePassive = function(global_actor)
        -- Nothing to do.
    end,
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_SP] = {
    name = vt_system.Translate("SP"),
    default_duration = 30000,
    -- Regens the character only every 9 seconds
    update_every = 9000,

    Apply = function(effect)
        -- Nothing to do.
    end,

    Update = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        if (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
            actor:RegisterHealing(1, false);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
            actor:RegisterHealing(2, false);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
            actor:RegisterHealing(4, false);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
            actor:RegisterHealing(8, false);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER) then
            actor:RegisterSPDamage(1);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE) then
            actor:RegisterSPDamage(2);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_GREATER) then
            actor:RegisterSPDamage(4);
        elseif (intensity == vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_EXTREME) then
            actor:RegisterSPDamage(8);
        end
    end,

    Remove = function(effect)
        -- Nothing to do.
    end,

    ApplyPassive = function(global_actor, intensity)
        -- Nothing to do.
    end,

    UpdatePassive = function(effect)
        -- TODO: Needs first a status effect handler for the map mode and a passive effect handler in the battle mode.
    end,

    RemovePassive = function(global_actor)
        -- Nothing to do.
    end,
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_PARALYSIS] = {
    name = vt_system.Translate("Paralysis"),
    default_duration = 10000,

    Apply = function(effect)
        -- Let's stun only when the effect is negative
        if (effect:GetIntensity() < vt_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
            local battle_actor = effect:GetAffectedActor();
            battle_actor:SetStunned(true);
        end
    end,

    Update = function(effect)
        -- Nothing needs to be updated for this effect
    end,

    Remove = function(effect)
        local battle_actor = effect:GetAffectedActor();
        battle_actor:SetStunned(false);
    end,

    ApplyPassive = function(global_actor, intensity)
        -- TODO: Needs first a status effect handler for the map mode and a passive effect handler in the battle mode.
    end,

    UpdatePassive = function(effect)
        -- Nothing to do.
    end,

    RemovePassive = function(global_actor)
        -- TODO: Needs first a status effect handler for the map mode and a passive effect handler in the battle mode.
    end,
}

-- Elemental status effects
status_effects[vt_global.GameGlobal.GLOBAL_STATUS_FIRE] = {
    name = vt_system.Translate("Fire Elemental Strength"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_FIRE]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_FIRE]._ModifyAttribute(effect);
        end
    end,

    Remove = function(effect)
        effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_FIRE, 1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_FIRE, attribute_modifier);
    end,

    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_FIRE, attribute_modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_FIRE, 1.0);
    end,
}

-- Water
status_effects[vt_global.GameGlobal.GLOBAL_STATUS_WATER] = {
    name = vt_system.Translate("Water Elemental Strength"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_WATER]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_WATER]._ModifyAttribute(effect);
        end
    end,

    Remove = function(effect)
        effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_WATER, 1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_WATER, attribute_modifier);
    end,

    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_WATER, attribute_modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_WATER, 1.0);
    end,
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_VOLT] = {
    name = vt_system.Translate("Wind Elemental Strength"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_VOLT]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_VOLT]._ModifyAttribute(effect);
        end
    end,

    Remove = function(effect)
        effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_VOLT, 1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_VOLT, attribute_modifier);
    end,

    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_VOLT, attribute_modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_VOLT, 1.0);
    end,
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_EARTH] = {
    name = vt_system.Translate("Earth Elemental Strength"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_EARTH]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_EARTH]._ModifyAttribute(effect);
        end
    end,

    Remove = function(effect)
        effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_EARTH, 1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_EARTH, attribute_modifier);
    end,

    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_EARTH, attribute_modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_EARTH, 1.0);
    end,
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_LIFE] = {
    name = vt_system.Translate("Life Elemental Strength"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_LIFE]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_LIFE]._ModifyAttribute(effect);
        end
    end,

    Remove = function(effect)
        effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_LIFE, 1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_LIFE, attribute_modifier);
    end,

    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_LIFE, attribute_modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_LIFE, 1.0);
    end,
}

status_effects[vt_global.GameGlobal.GLOBAL_STATUS_DEATH] = {
    name = vt_system.Translate("Death Elemental Strength"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_DEATH]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_DEATH]._ModifyAttribute(effect);
        end
    end,

    Remove = function(effect)
        effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_DEATH, 1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_DEATH, attribute_modifier);
    end,

    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_DEATH, attribute_modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_DEATH, 1.0);
    end,
}

-- Neutral (default magical strength)
status_effects[vt_global.GameGlobal.GLOBAL_STATUS_NEUTRAL] = {
    name = vt_system.Translate("Neutral Elemental Strength"),
    default_duration = 30000,

    Apply = function(effect)
        status_effects[vt_global.GameGlobal.GLOBAL_STATUS_NEUTRAL]._ModifyAttribute(effect);
    end,

    Update = function(effect)
        if (effect:HasIntensityChanged() == true) then
            status_effects[vt_global.GameGlobal.GLOBAL_STATUS_NEUTRAL]._ModifyAttribute(effect);
        end
    end,

    Remove = function(effect)
        effect:GetAffectedActor():SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL, 1.0);
    end,

    _ModifyAttribute = function(effect)
        local actor = effect:GetAffectedActor();
        local intensity = effect:GetIntensity();

        local attribute_modifier = status_effects.GetStatModifier(intensity);
        actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL, attribute_modifier);
    end,

    ApplyPassive = function(global_actor, intensity)
        local attribute_modifier = status_effects.GetStatModifier(intensity);
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL, attribute_modifier);
    end,

    UpdatePassive = function(effect)
        -- Nothing to do
    end,

    RemovePassive = function(global_actor)
        global_actor:SetElementalModifier(vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL, 1.0);
    end,
}
