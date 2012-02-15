////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2009 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_effects.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for battle actor effects.
*** ***************************************************************************/

#include "script.h"
#include "system.h"
#include "video.h"

#include "global.h"

#include "battle.h"
#include "battle_actors.h"
#include "battle_effects.h"
#include "battle_utils.h"

using namespace std;

using namespace hoa_utils;

using namespace hoa_system;
using namespace hoa_script;
using namespace hoa_video;

using namespace hoa_global;

namespace hoa_battle {

namespace private_battle {

////////////////////////////////////////////////////////////////////////////////
// BattleStatusEffect class
////////////////////////////////////////////////////////////////////////////////

BattleStatusEffect::BattleStatusEffect(GLOBAL_STATUS type, GLOBAL_INTENSITY intensity, BattleActor* actor) :
	GlobalStatusEffect(type, intensity),
	_name(""),
	_icon_index(0),
	_opposite_effect(GLOBAL_STATUS_INVALID),
	_apply_function(NULL),
	_update_function(NULL),
	_remove_function(NULL),
	_affected_actor(actor),
	_timer(0),
	_icon_image(NULL),
	_intensity_changed(false)
{
	// --- (1): Check that the constructor arguments are valid
	if ((type <= GLOBAL_STATUS_INVALID) || (type >= GLOBAL_STATUS_TOTAL)) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received an invalid type argument: " << type << endl;
		return;
	}
	if ((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received an invalid intensity argument: " << intensity << endl;
		return;
	}
	if (actor == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL actor argument" << endl;
		return;
	}

	// --- (2): Make sure that a table entry exists for this status element
	uint32 table_id = static_cast<uint32>(type);
	ReadScriptDescriptor& script_file = GlobalManager->GetStatusEffectsScript();
	if (script_file.DoesTableExist(table_id) == false) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "Lua definition file contained no entry for status effect: " << table_id << endl;
		return;
	}

	// --- (3): Read in the status effect's property data
	script_file.OpenTable(table_id);
	_name = script_file.ReadString("name");
	_timer.SetDuration(script_file.ReadUInt("duration"));
	_icon_index = script_file.ReadUInt("icon_index");
	_opposite_effect = static_cast<GLOBAL_STATUS>(script_file.ReadInt("opposite_effect"));

	if (script_file.DoesFunctionExist("Apply")) {
		_apply_function = new ScriptObject();
		(*_apply_function) = script_file.ReadFunctionPointer("Apply");
	}
	else {
		PRINT_WARNING << "no Apply function found in Lua definition file for status: " << table_id << endl;
	}

	if (script_file.DoesFunctionExist("Update")) {
		_update_function = new ScriptObject();
		(*_update_function) = script_file.ReadFunctionPointer("Update");
	}
	else {
		PRINT_WARNING << "no Update function found in Lua definition file for status: " << table_id << endl;
	}

	if (script_file.DoesFunctionExist("Remove")) {
		_remove_function = new ScriptObject();
		(*_remove_function) = script_file.ReadFunctionPointer("Remove");
	}
	else {
		PRINT_WARNING << "no Remove function found in Lua definition file for status: " << table_id << endl;
	}
	script_file.CloseTable();

	if (script_file.IsErrorDetected()) {
		if (BATTLE_DEBUG) {
			PRINT_WARNING << "one or more errors occurred while reading status effect data - they are listed below" << endl;
			cerr << script_file.GetErrorMessages() << endl;
		}
	}

	// --- (4): Finish initialization of members
	_timer.Reset();
	_timer.Run();
	_icon_image = BattleMode::CurrentInstance()->GetMedia().GetStatusIcon(_type, _intensity);
}



BattleStatusEffect::~BattleStatusEffect() {
	if (_apply_function != NULL)
		delete _apply_function;
	if (_update_function != NULL)
		delete _update_function;
	if (_remove_function != NULL)
		delete _remove_function;

	_apply_function = NULL;
	_update_function = NULL;
	_remove_function = NULL;
}



void BattleStatusEffect::SetIntensity(hoa_global::GLOBAL_INTENSITY intensity) {
	if ((intensity < GLOBAL_INTENSITY_NEUTRAL) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "attempted to set status effect to invalid intensity: " << intensity << endl;
		return;
	}

	bool no_intensity_change = (_intensity == intensity);
	_intensity = intensity;
	_ProcessIntensityChange(no_intensity_change);
}



bool BattleStatusEffect::IncrementIntensity(uint8 amount) {
	bool change = GlobalStatusEffect::IncrementIntensity(amount);
	_ProcessIntensityChange(!change);
	return change;
}



bool BattleStatusEffect::DecrementIntensity(uint8 amount) {
	bool change = GlobalStatusEffect::DecrementIntensity(amount);
	_ProcessIntensityChange(!change);
	return change;
}



void BattleStatusEffect::_ProcessIntensityChange(bool reset_timer_only) {
	if (reset_timer_only == true) {
		_timer.Reset();
		_timer.Run();
	}
	else {
		_intensity_changed = true;
		_icon_image = BattleMode::CurrentInstance()->GetMedia().GetStatusIcon(_type, _intensity);
	}
}

////////////////////////////////////////////////////////////////////////////////
// EffectsSupervisor class
////////////////////////////////////////////////////////////////////////////////

EffectsSupervisor::EffectsSupervisor(BattleActor* actor) :
	_actor(actor)
{
	if (actor == NULL)
		IF_PRINT_WARNING(BATTLE_DEBUG) << "contructor received NULL actor argument" << endl;
}



EffectsSupervisor::~EffectsSupervisor() {
	for (map<GLOBAL_STATUS, BattleStatusEffect*>::iterator i = _active_status_effects.begin(); i != _active_status_effects.end(); i++)
		delete (i->second);
	_active_status_effects.clear();
}



void EffectsSupervisor::Update() {
	// As a result of what is done in this update loop, sometimes effects will be removed and this changes the state of the _active_status_effects
	// container. To avoid problems with container resizing, we use a seperate container to iterate through and process updates for all effects,
	// to ensure that we don't get any bad iterator references and that we update each effect only once.
	std::vector<BattleStatusEffect*> effects;
	for (map<GLOBAL_STATUS, BattleStatusEffect*>::iterator i = _active_status_effects.begin(); i != _active_status_effects.end(); i++) {
		effects.push_back(i->second);
	}

	// Update the timers and state for all active status effects
	for (uint32 i = 0; i < effects.size(); i++) {
		bool effect_removed = false;

		effects[i]->GetTimer()->Update();

		// Decrease the intensity of the status by one level when its timer expires. This may result in
		// the status effect being removed from the actor if its intensity changes to the neutral level.
		if (effects[i]->GetTimer()->IsFinished() == true) {
			// If the intensity of the effect is at its weakest, the call that follows will remove the effect from the actor
			effect_removed = (effects[i]->GetIntensity() == GLOBAL_INTENSITY_POS_LESSER);

			// Note that we register the status change on the actor instead of directly calling ChangeStatus() here.
			// We do this because the actor's indicator supervisor needs to be informed of intensity changes so that it
			// may display the appropriate status change information to the user. The call to BattleActor::RegisterStatusChange()
			// will in turn make a call to EffectsSupervisor::ChangeStatus().
			_actor->RegisterStatusChange(effects[i]->GetType(), GLOBAL_INTENSITY_NEG_LESSER);
		}

		// Update the effect according to the script function
		if (effect_removed == false) {
			ScriptCallFunction<void>(*(effects[i]->GetUpdateFunction()), effects[i]);
			effects[i]->ResetIntensityChanged();
		}
	}
}



void EffectsSupervisor::Draw() {
	for (map<GLOBAL_STATUS, BattleStatusEffect*>::iterator i = _active_status_effects.begin(); i != _active_status_effects.end(); i++) {
		i->second->GetIconImage()->Draw();
		VideoManager->MoveRelative(25.0f, 0.0f);
	}
}



bool EffectsSupervisor::IsOppositeStatusActive(GLOBAL_STATUS status) {
	for (map<GLOBAL_STATUS, BattleStatusEffect*>::iterator i = _active_status_effects.begin(); i != _active_status_effects.end(); i++) {
		if (i->second->GetOppositeEffect() == status) {
			return true;
		}
	}

	return false;
}



void EffectsSupervisor::GetAllStatusEffects(vector<GLOBAL_STATUS>& all_status_effects) {
	all_status_effects.empty();

	for (map<GLOBAL_STATUS, BattleStatusEffect*>::iterator i = _active_status_effects.begin(); i != _active_status_effects.end(); i++) {
		all_status_effects.push_back(i->first);
	}
}



void EffectsSupervisor::RemoveAllStatus() {
	vector<BattleStatusEffect*> effects;

	// Calls to _RemoveStatus() alter the _active_status_effects container. To avoid problems with iterating through the _active_status_effects map during
	// the removal calls, we make a temporary copy of all the status effects and operate on that
	for (map<GLOBAL_STATUS, BattleStatusEffect*>::iterator i = _active_status_effects.begin(); i != _active_status_effects.end(); i++) {
		effects.push_back(i->second);
	}

	for (uint32 i = 0; i < effects.size(); i++) {
		_RemoveStatus(effects[i]);
	}
}



bool EffectsSupervisor::ChangeStatus(GLOBAL_STATUS status, GLOBAL_INTENSITY intensity, GLOBAL_STATUS& previous_status, GLOBAL_INTENSITY& previous_intensity,
	GLOBAL_STATUS& new_status, GLOBAL_INTENSITY& new_intensity)
{
	if ((status <= GLOBAL_STATUS_INVALID) || (status >= GLOBAL_STATUS_TOTAL)) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received invalid status argument: " << status << endl;
		return false;
	}

	// --- (1): Determine if we are attempting to increment or decrement the intensity of this status
	bool increase_intensity;
	if ((intensity < GLOBAL_INTENSITY_NEUTRAL) && (intensity >= GLOBAL_INTENSITY_NEG_EXTREME)) {
		increase_intensity = false;
	}
	else if ((intensity <= GLOBAL_INTENSITY_POS_EXTREME) && (intensity > GLOBAL_INTENSITY_NEUTRAL)) {
		increase_intensity = true;
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received invalid intensity argument: " << intensity << endl;
		return false;
	}

	// Holds the unsigned amount of change in intensity in either a positive or negative dgree
	uint8 intensity_change = abs(static_cast<int8>(intensity));

	// --- (2): Determine if this status (or its opposite) is already active on the actor
	bool status_active = false;
	bool opposite_status_active = false;
	// Holds a pointer to either the active status or the active opposite status
	BattleStatusEffect* active_effect = NULL;

	// Note: We should never run into the case where both the status and its opposite status are active simultaneously
	for (map<GLOBAL_STATUS, BattleStatusEffect*>::iterator i = _active_status_effects.begin(); i != _active_status_effects.end(); i++) {
		if (i->second->GetType() == status) {
			status_active = true;
			active_effect = i->second;
			break;
		}
		else if (i->second->GetOppositeEffect() == status) {
			opposite_status_active = true;
			active_effect = i->second;
			break;
		}
	}

	// Set the previous status and intensity return values to match the active effect, if one was found to exist
	if (active_effect == NULL) {
		previous_status = status;
		previous_intensity = GLOBAL_INTENSITY_NEUTRAL;
	}
	else {
		previous_status = active_effect->GetType();
		previous_intensity = active_effect->GetIntensity();
	}

	// --- (3): Perform status changes according to the previously determined information
	// Case 1: We are attempting to decrement the intensity of the status
	if (increase_intensity == false) {
		if (status_active == true) {
			active_effect->DecrementIntensity(intensity_change);

			new_status = status;
			new_intensity = active_effect->GetIntensity();

			// If the status was decremented to the neutral level, this means it is no longer active and should be removed
			if (new_intensity == GLOBAL_INTENSITY_NEUTRAL) {
				_RemoveStatus(active_effect);
				active_effect = NULL;
			}
			return true;
		}

		// No change can take place if the status we wish to decrease the intensity of is not active. Even if its opposite status is active.
		return false;
	}
	// Case 2: Increase the intensity of an already active effect
	else if ((increase_intensity == true) && (status_active == true)) {
		active_effect->IncrementIntensity(intensity_change);

		new_status = status;
		new_intensity = active_effect->GetIntensity();
		// Note: it is possible that the intensity won't increment if the status is already at its highest intensity level.
		// We still want to act like a status change did occur though, as we want the player to see that the action that caused the change
		// did achieve a result (and it actually does, since this condition causes the status effect's timer to get reset).
		return true;
	}
	// Case 3: Increase the intensity of an effect that was not active and had no active opposite effect
	else if ((increase_intensity == true) && (status_active == false) && (opposite_status_active == false)) {
		_CreateNewStatus(status, intensity);

		new_status = status;
		new_intensity = intensity;
		return true;
	}
	// Case 4: Increase the intensity of an effect when its opposing effect is currently active
	else if ((increase_intensity == true) && (opposite_status_active == true)) {
		active_effect->DecrementIntensity(intensity_change);

		// Case 4a: The opposite status effect intensity was decreased. However the opposite status is still active.
		if (intensity < previous_intensity) {
			new_status = active_effect->GetType();
			new_intensity = active_effect->GetIntensity();
		}
		// Case 4b: The opposite status effect was completely nullifed. No new status is to be created
		else if (intensity == previous_intensity) {
			new_status = previous_status;
			new_intensity = GLOBAL_INTENSITY_NEUTRAL;
			_RemoveStatus(active_effect);
		}
		// Case 4c: The opposite status effect was completely nullified and the new status is to be created
		else { // (intensity > previous_intensity)
			new_status = status;
			DecrementIntensity(new_intensity, static_cast<int8>(previous_intensity));
			_RemoveStatus(active_effect);
			_CreateNewStatus(new_status, new_intensity);
		}

		return true;
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "unknown/unhandled condition occured when trying to perform a status change, aborting operation" << endl;
	}

	return false;
} // bool EffectsSupervisor::ChangeStatus( ... )



void EffectsSupervisor::_CreateNewStatus(GLOBAL_STATUS status, GLOBAL_INTENSITY intensity) {
	if ((status <= GLOBAL_STATUS_INVALID) || (status >= GLOBAL_STATUS_TOTAL)) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received invalid status argument: " << status << endl;
		return;
	}

	if ((intensity <= GLOBAL_INTENSITY_NEUTRAL) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received invalid intensity argument: " << intensity << endl;
		return;
	}

	BattleStatusEffect* new_effect = new BattleStatusEffect(status, intensity, _actor);
	_active_status_effects.insert(make_pair(status, new_effect));

	// Call the apply script function now that this new status is active on the actor
	ScriptCallFunction<void>(*(new_effect->GetApplyFunction()), new_effect);
}



void EffectsSupervisor::_RemoveStatus(BattleStatusEffect* status_effect) {
	if (status_effect == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL status_effect argument" << endl;
		return;
	}

	// Remove the status effect from the active effects list. If successful, call the remove function and then delete the status effect object
	if (_active_status_effects.erase(status_effect->GetType()) == 0) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "attempted to remove a status effect not present on the actor with type: " << status_effect->GetType() << endl;
	}
	else {
		ScriptCallFunction<void>(*(status_effect->GetRemoveFunction()), status_effect);
		delete status_effect;
	}
}

} // namespace private_battle

} // namespace hoa_battle
