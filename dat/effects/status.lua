------------------------------------------------------------------------------[[
-- Filename: status.lua
--
-- Description: This file contains the implementations of all status effects in
-- Hero of Allacrost. The list of different types of status effects and their
-- corresponding names may be found in src/modes/common/global/global_effects.*.
--
-- Each status effect implementation requires the following data to be defined.
-- {name} - The name of the status effect as it will be shown to the player
-- {duration} - The duration that the effect lasts, in milliseconds
-- {icon_index} - A numeric index to the row of images where the icons for this effect
-- {opposite_effect} - The status which acts as an opposite status to this one
-- {Apply} - A function executed when the status effect is applied to the target
-- {Update} - A function executed periodically while the status is still in effect
-- {Remove} - A function executed when the status effect is no longer active on the target
--
-- To verify what a status effect's icon_index should be, examine the image file 
-- img/icons/effects/status.png and find the appropriate row of icons.
-- 
-- The opposite_status property is only applicable to some pairs of status effects.
-- For example, a pair of effects that increase strength and decrease strength. Each status
-- effect can have only one opposite effect, not several. If the status effect has no opposite
-- effect, this member should be set to hoa_global.GameGlobal.GLOBAL_STATUS_INVALID.
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
-- effect:IsIntensityChanged() - returns true if the intensity level has undergone a recent change
--
-- NOTE: Unlike elemental effects, status effects only ever have intensity levels that are
-- neutral or in the positive range of values, never the negative range. You should not concern
-- yourself with negative intensity values in this file.
------------------------------------------------------------------------------]]

-- All item definitions are stored in this table
if (status_effects == nil) then
   status_effects = {}
end


status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_RAISE] = {
	name = hoa_system.Translate("Raise Strength"),
	duration = 30000,
	icon_index = 0,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_LOWER, 

	Apply = function(effect)
		status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_RAISE].ModifyAttribute(effect);
	end,

	Update = function(effect)
		if (effect:IsIntensityChanged() == true) then
			status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_RAISE].ModifyAttribute(effect);
		end
	end,

	Remove = function(effect)
		effect:GetAffectedActor():ResetStrength();
	end,

	ModifyAttribute = function(effect)
		actor = effect:GetAffectedActor();
		intensity = effect:GetIntensity();

		actor:ResetStrength();
		base_value = actor:GetStrength();
		attribute_modifier = 1;

		if (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
			attribute_modifier = 1;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
			attribute_modifer = 1.2;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
			attribute_modifier = 1.4;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
			attribute_modifier = 1.6;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
			attribute_modifier = 1.8;
		else
			print("Lua warning: status effect had an invalid intensity value: " .. intensity);
		end

		actor:SetStrength(base_value * attribute_modifier);
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_LOWER] = {
	name = hoa_system.Translate("Lower Strength"),
	duration = 30000,
	icon_index = 1,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_RAISE, 

	Apply = function(effect)
		status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_LOWER].ModifyAttribute(effect);
	end,

	Update = function(effect)
		if (effect:IsIntensityChanged() == true) then
			status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_LOWER].ModifyAttribute(effect);
		end
	end,

	Remove = function(effect)
		effect:GetAffectedActor():ResetStrength();
	end,

	ModifyAttribute = function(effect)
		actor = effect:GetAffectedActor();
		intensity = effect:GetIntensity();

		actor:ResetStrength();
		base_value = actor:GetStrength();
		attribute_modifier = 1;

		if (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
			attribute_modifier = 1;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
			attribute_modifier = 0.8;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
			attribute_modifier = 0.6;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
			attribute_modifier = 0.4;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
			attribute_modifier = 0.2;
		else
			print("Lua warning: status effect had an invalid intensity value: " .. intensity);
		end

		actor:SetStrength(base_value * attribute_modifier);
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_RAISE] = {
	name = hoa_system.Translate("Raise Vigor"),
	duration = 30000,
	icon_index = 2,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_LOWER, 

	Apply = function(effect)
		status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_RAISE].ModifyAttribute(effect);
	end,

	Update = function(effect)
		if (effect:IsIntensityChanged() == true) then
			status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_RAISE].ModifyAttribute(effect);
		end
	end,

	Remove = function(effect)
		effect:GetAffectedActor():ResetVigor();
	end,

	ModifyAttribute = function(effect)
		actor = effect:GetAffectedActor();
		intensity = effect:GetIntensity();

		actor:ResetVigor();
		base_value = actor:GetVigor();
		attribute_modifier = 1;

		if (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
			attribute_modifier = 1;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
			attribute_modifer = 1.2;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
			attribute_modifier = 1.4;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
			attribute_modifier = 1.6;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
			attribute_modifier = 1.8;
		else
			print("Lua warning: status effect had an invalid intensity value: " .. intensity);
		end

		actor:SetVigor(base_value * attribute_modifier);
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_LOWER] = {
	name = hoa_system.Translate("Lower Vigor"),
	duration = 30000,
	icon_index = 3,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_RAISE, 

	Apply = function(effect)
		status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_LOWER].ModifyAttribute(effect);
	end,

	Update = function(effect)
		if (effect:IsIntensityChanged() == true) then
			status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_LOWER].ModifyAttribute(effect);
		end
	end,

	Remove = function(effect)
		effect:GetAffectedActor():ResetVigor();
	end,

	ModifyAttribute = function(effect)
		actor = effect:GetAffectedActor();
		intensity = effect:GetIntensity();

		actor:ResetVigor();
		base_value = actor:GetVigor();
		attribute_modifier = 1;

		if (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
			attribute_modifier = 1;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
			attribute_modifier = 0.8;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
			attribute_modifier = 0.6;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
			attribute_modifier = 0.4;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
			attribute_modifier = 0.2;
		else
			print("Lua warning: status effect had an invalid intensity value: " .. intensity);
		end

		actor:SetVigor(base_value * attribute_modifier);
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_FORTITUDE_RAISE] = {
	name = hoa_system.Translate("Raise Fortitude"),
	duration = 30000,
	icon_index = 4,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_FORTITUDE_LOWER, 

	Apply = function(effect)
		status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_FORTITUDE_RAISE].ModifyAttribute(effect);
	end,

	Update = function(effect)
		if (effect:IsIntensityChanged() == true) then
			status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_FORTITUDE_RAISE].ModifyAttribute(effect);
		end
	end,

	Remove = function(effect)
		effect:GetAffectedActor():ResetFortitude();
	end,

	ModifyAttribute = function(effect)
		actor = effect:GetAffectedActor();
		intensity = effect:GetIntensity();

		actor:ResetFortitude();
		base_value = actor:GetFortitude();
		attribute_modifier = 1;

		if (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
			attribute_modifier = 1;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
			attribute_modifer = 1.2;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
			attribute_modifier = 1.4;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
			attribute_modifier = 1.6;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
			attribute_modifier = 1.8;
		else
			print("Lua warning: status effect had an invalid intensity value: " .. intensity);
		end

		actor:SetFortitude(base_value * attribute_modifier);
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_FORTITUDE_LOWER] = {
	name = hoa_system.Translate("Lower Fortitude"),
	duration = 30000,
	icon_index = 5,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_FORTITUDE_RAISE, 

	Apply = function(effect)
		status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_FORTITUDE_LOWER].ModifyAttribute(effect);
	end,

	Update = function(effect)
		if (effect:IsIntensityChanged() == true) then
			status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_FORTITUDE_LOWER].ModifyAttribute(effect);
		end
	end,

	Remove = function(effect)
		effect:GetAffectedActor():ResetFortitude();
	end,

	ModifyAttribute = function(effect)
		actor = effect:GetAffectedActor();
		intensity = effect:GetIntensity();

		actor:ResetFortitude();
		base_value = actor:GetFortitude();
		attribute_modifier = 1;

		if (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
			attribute_modifier = 1;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
			attribute_modifier = 0.8;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
			attribute_modifier = 0.6;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
			attribute_modifier = 0.4;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
			attribute_modifier = 0.2;
		else
			print("Lua warning: status effect had an invalid intensity value: " .. intensity);
		end

		actor:SetFortitude(base_value * attribute_modifier);
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_PROTECTION_RAISE] = {
	name = hoa_system.Translate("Raise Protection"),
	duration = 30000,
	icon_index = 6,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_PROTECTION_LOWER, 

	Apply = function(effect)
		status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_PROTECTION_RAISE].ModifyAttribute(effect);
	end,

	Update = function(effect)
		if (effect:IsIntensityChanged() == true) then
			status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_PROTECTION_RAISE].ModifyAttribute(effect);
		end
	end,

	Remove = function(effect)
		effect:GetAffectedActor():ResetProtection();
	end,

	ModifyAttribute = function(effect)
		actor = effect:GetAffectedActor();
		intensity = effect:GetIntensity();

		actor:ResetProtection();
		base_value = actor:GetProtection();
		attribute_modifier = 1;

		if (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
			attribute_modifier = 1;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
			attribute_modifer = 1.2;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
			attribute_modifier = 1.4;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
			attribute_modifier = 1.6;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
			attribute_modifier = 1.8;
		else
			print("Lua warning: status effect had an invalid intensity value: " .. intensity);
		end

		actor:SetProtection(base_value * attribute_modifier);
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_PROTECTION_LOWER] = {
	name = hoa_system.Translate("Lower Protection"),
	duration = 30000,
	icon_index = 7,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_PROTECTION_RAISE, 

	Apply = function(effect)
		status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_PROTECTION_LOWER].ModifyAttribute(effect);
	end,

	Update = function(effect)
		if (effect:IsIntensityChanged() == true) then
			status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_PROTECTION_LOWER].ModifyAttribute(effect);
		end
	end,

	Remove = function(effect)
		effect:GetAffectedActor():ResetProtection();
	end,

	ModifyAttribute = function(effect)
		actor = effect:GetAffectedActor();
		intensity = effect:GetIntensity();

		actor:ResetProtection();
		base_value = actor:GetProtection();
		attribute_modifier = 1;

		if (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
			attribute_modifier = 1;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
			attribute_modifier = 0.8;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
			attribute_modifier = 0.6;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
			attribute_modifier = 0.4;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
			attribute_modifier = 0.2;
		else
			print("Lua warning: status effect had an invalid intensity value: " .. intensity);
		end

		actor:SetProtection(base_value * attribute_modifier);
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_RAISE] = {
	name = hoa_system.Translate("Raise Agility"),
	duration = 30000,
	icon_index = 8,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER, 

	Apply = function(effect)
		status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_RAISE].ModifyAttribute(effect);
	end,

	Update = function(effect)
		if (effect:IsIntensityChanged() == true) then
			status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_RAISE].ModifyAttribute(effect);
		end
	end,

	Remove = function(effect)
		effect:GetAffectedActor():ResetAgility();
	end,

	ModifyAttribute = function(effect)
		actor = effect:GetAffectedActor();
		intensity = effect:GetIntensity();

		actor:ResetAgility();
		base_value = actor:GetAgility();
		attribute_modifier = 1;

		if (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
			attribute_modifier = 1;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
			attribute_modifer = 1.2;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
			attribute_modifier = 1.4;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
			attribute_modifier = 1.6;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
			attribute_modifier = 1.8;
		else
			print("Lua warning: status effect had an invalid intensity value: " .. intensity);
		end

		actor:SetStrength(base_value * attribute_modifier);
		-- TODO: Need to modify actor's idle state wait time accordingly here
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER] = {
	name = hoa_system.Translate("Lower Agility"),
	duration = 30000,
	icon_index = 9,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_RAISE, 

	Apply = function(effect)
		status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER].ModifyAttribute(effect);
	end,

	Update = function(effect)
		if (effect:IsIntensityChanged() == true) then
			status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER].ModifyAttribute(effect);
		end
	end,

	Remove = function(effect)
		effect:GetAffectedActor():ResetAgility();
	end,

	ModifyAttribute = function(effect)
		actor = effect:GetAffectedActor();
		intensity = effect:GetIntensity();

		actor:ResetAgility();
		base_value = actor:GetAgility();
		attribute_modifier = 1;

		if (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
			attribute_modifier = 1;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
			attribute_modifier = 0.8;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
			attribute_modifier = 0.6;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
			attribute_modifier = 0.4;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
			attribute_modifier = 0.2;
		else
			print("Lua warning: status effect had an invalid intensity value: " .. intensity);
		end

		actor:SetAgility(base_value * attribute_modifier);
		-- TODO: Need to modify actor's idle state wait time accordingly here
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_EVADE_RAISE] = {
	name = hoa_system.Translate("Raise Evasion"),
	duration = 30000,
	icon_index = 10,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_EVADE_LOWER, 

	Apply = function(effect)
		status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_EVADE_RAISE].ModifyAttribute(effect);
	end,

	Update = function(effect)
		if (effect:IsIntensityChanged() == true) then
			status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_EVADE_RAISE].ModifyAttribute(effect);
		end
	end,

	Remove = function(effect)
		effect:GetAffectedActor():ResetEvade();
	end,

	ModifyAttribute = function(effect)
		actor = effect:GetAffectedActor();
		intensity = effect:GetIntensity();

		actor:ResetEvae();
		base_value = actor:GetEvade();
		attribute_modifier = 1;

		if (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
			attribute_modifier = 1;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
			attribute_modifer = 1.2;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
			attribute_modifier = 1.4;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
			attribute_modifier = 1.6;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
			attribute_modifier = 1.8;
		else
			print("Lua warning: status effect had an invalid intensity value: " .. intensity);
		end

		-- Note: this has a possiblitiy to set the evade above the max value of 1.0 (100%). This shouldn't
		-- pose a problem though, and its probably very unlikely that any actor would have an evade rating
		-- high enough to cause this condition.
		actor:SetEvade(base_value * attribute_modifier);
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_EVADE_LOWER] = {
	name = hoa_system.Translate("Lower Evasion"),
	duration = 30000,
	icon_index = 11,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_EVADE_RAISE, 

	Apply = function(effect)
		status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_EVADE_LOWER].ModifyAttribute(effect);
	end,

	Update = function(effect)
		if (effect:IsIntensityChanged() == true) then
			status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_EVADE_LOWER].ModifyAttribute(effect);
		end
	end,

	Remove = function(effect)
		effect:GetAffectedActor():ResetEvade();
	end,

	ModifyAttribute = function(effect)
		actor = effect:GetAffectedActor();
		intensity = effect:GetIntensity();

		actor:ResetEvade();
		base_value = actor:GetEvade();
		attribute_modifier = 1;

		if (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
			attribute_modifier = 1;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
			attribute_modifier = 0.8;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE) then
			attribute_modifier = 0.6;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then
			attribute_modifier = 0.4;
		elseif (intensity == hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME) then
			attribute_modifier = 0.2;
		else
			print("Lua warning: status effect had an invalid intensity value: " .. intensity);
		end

		actor:SetEvade(base_value * attribute_modifier);
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_HP_REGEN] = {
	name = hoa_system.Translate("Regenerate HP"),
	duration = 30000,
	icon_index = 12,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_HP_DRAIN,
	
	Apply = function(effect)
		-- TODO
	end,

	Update = function(effect)
		-- TODO
	end,

	Remove = function(effect)
		-- TODO
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_HP_DRAIN] = {
	name = hoa_system.Translate("Drain HP"),
	duration = 30000,
	icon_index = 13,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_HP_REGEN,
	
	Apply = function(effect)
		-- TODO
	end,

	Update = function(effect)
		-- TODO
	end,

	Remove = function(effect)
		-- TODO
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_SP_REGEN] = {
	name = hoa_system.Translate("Regenerate SP"),
	duration = 30000,
	icon_index = 14,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_SP_DRAIN,
	
	Apply = function(effect)
		-- TODO
	end,

	Update = function(effect)
		-- TODO
	end,

	Remove = function(effect)
		-- TODO
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_SP_DRAIN] = {
	name = hoa_system.Translate("Drain SP"),
	duration = 30000,
	icon_index = 15,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_SP_REGEN,
	
	Apply = function(effect)
		-- TODO
	end,

	Update = function(effect)
		-- TODO
	end,

	Remove = function(effect)
		-- TODO
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_PARALYSIS] = {
	name = hoa_system.Translate("Paralysis"),
	duration = 10000,
	icon_index = 16,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_INVALID,
	
	Apply = function(effect)
		effect:GetAffectedActor():SetStatePaused(true);
	end,

	Update = function(effect)
		-- Nothing needs to be updated for this effect
	end,

	Remove = function(effect)
		effect:GetAffectedActor():SetStatePaused(false);
	end,
}

status_effects[hoa_global.GameGlobal.GLOBAL_STATUS_STASIS] = {
	name = hoa_system.Translate("Stasis"),
	duration = 10000,
	icon_index = 17,
	opposite_effect = hoa_global.GameGlobal.GLOBAL_STATUS_INVALID,
	
	Apply = function(effect)
		-- TODO
	end,

	Update = function(effect)
		-- TODO
	end,

	Remove = function(effect)
		-- TODO
	end,
}

