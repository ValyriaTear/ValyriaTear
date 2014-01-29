////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_effects.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for battle actor effects.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/battle/battle_effects.h"

#include "engine/script/script.h"
#include "engine/system.h"
#include "engine/video/video.h"

#include "common/global/global.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actors.h"
#include "modes/battle/battle_utils.h"

using namespace vt_utils;
using namespace vt_system;
using namespace vt_script;
using namespace vt_video;

using namespace vt_global;

namespace vt_battle
{

namespace private_battle
{

////////////////////////////////////////////////////////////////////////////////
// PassiveBattleStatusEffect class
////////////////////////////////////////////////////////////////////////////////

PassiveBattleStatusEffect::PassiveBattleStatusEffect(GLOBAL_STATUS type,
                                                     GLOBAL_INTENSITY intensity,
                                                     BattleActor *actor):
    GlobalStatusEffect(type, intensity),
    _affected_actor(actor),
    _icon_image(NULL)
{
    // Check that the constructor arguments are valid
    if((type <= GLOBAL_STATUS_INVALID) || (type >= GLOBAL_STATUS_TOTAL)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received an invalid type argument: " << type << std::endl;
        return;
    }
    if((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received an invalid intensity argument: " << intensity << std::endl;
        return;
    }
    if(actor == NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL actor argument" << std::endl;
        return;
    }

    // Make sure that a table entry exists for this status element
    uint32 table_id = static_cast<uint32>(type);
    ReadScriptDescriptor &script_file = GlobalManager->GetStatusEffectsScript();
    if(!script_file.OpenTable(table_id)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "Lua definition file contained no entry for status effect: " << table_id << std::endl;
        return;
    }

    // Read in the status effect's property data
    _name = script_file.ReadString("name");

    if(script_file.DoesFunctionExist("BattleUpdatePassive")) {
        _update_passive_function = script_file.ReadFunctionPointer("BattleUpdatePassive");
    } else {
        PRINT_WARNING << "No BattleUpdatePassive() function found in Lua definition file for status: " << table_id << std::endl;
    }

    script_file.CloseTable(); // table_id

    if(script_file.IsErrorDetected()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "one or more errors occurred while reading status effect data - they are listed below"
            << std::endl << script_file.GetErrorMessages() << std::endl;
    }

    _icon_image = GlobalManager->Media().GetStatusIcon(_type, _intensity);
}

////////////////////////////////////////////////////////////////////////////////
// ActiveBattleStatusEffect class
////////////////////////////////////////////////////////////////////////////////

ActiveBattleStatusEffect::ActiveBattleStatusEffect(GLOBAL_STATUS type, GLOBAL_INTENSITY intensity, BattleActor *actor, uint32 duration) :
    GlobalStatusEffect(type, intensity),
    _affected_actor(actor),
    _timer(0),
    _icon_image(NULL),
    _intensity_changed(false)
{
    // Check that the constructor arguments are valid
    if((type <= GLOBAL_STATUS_INVALID) || (type >= GLOBAL_STATUS_TOTAL)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received an invalid type argument: " << type << std::endl;
        return;
    }
    if((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received an invalid intensity argument: " << intensity << std::endl;
        return;
    }
    if(actor == NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL actor argument" << std::endl;
        return;
    }

    // Make sure that a table entry exists for this status element
    uint32 table_id = static_cast<uint32>(type);
    ReadScriptDescriptor &script_file = GlobalManager->GetStatusEffectsScript();
    if(!script_file.OpenTable(table_id)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "Lua definition file contained no entry for status effect: " << table_id << std::endl;
        return;
    }

    // Read in the status effect's property data
    _name = script_file.ReadString("name");

    // Read the fall back duration when none is given.
    if(duration == 0)
        duration = script_file.ReadUInt("default_duration");
    _timer.SetDuration(duration);

    if(script_file.DoesFunctionExist("BattleApply")) {
        _apply_function = script_file.ReadFunctionPointer("BattleApply");
    } else {
        PRINT_WARNING << "No BattleApply() function found in Lua definition file for status: " << table_id << std::endl;
    }

    if(script_file.DoesFunctionExist("BattleUpdate")) {
        _update_function = script_file.ReadFunctionPointer("BattleUpdate");
    } else {
        PRINT_WARNING << "No BattleUpdate() function found in Lua definition file for status: " << table_id << std::endl;
    }

    if(script_file.DoesFunctionExist("BattleRemove")) {
        _remove_function = script_file.ReadFunctionPointer("BattleRemove");
    } else {
        PRINT_WARNING << "No BattleRemove() function found in Lua definition file for status: " << table_id << std::endl;
    }
    script_file.CloseTable(); // table_id

    if(script_file.IsErrorDetected()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "one or more errors occurred while reading status effect data - they are listed below"
            << std::endl << script_file.GetErrorMessages() << std::endl;
    }

    // Init the effect timer
    _timer.EnableManualUpdate();
    _timer.Reset();
    _timer.Run();

    _icon_image = GlobalManager->Media().GetStatusIcon(_type, _intensity);
}

void ActiveBattleStatusEffect::SetIntensity(vt_global::GLOBAL_INTENSITY intensity)
{
    if((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "attempted to set status effect to invalid intensity: " << intensity << std::endl;
        return;
    }

    bool no_intensity_change = (_intensity == intensity);
    _intensity = intensity;
    _ProcessIntensityChange(no_intensity_change);
}

bool ActiveBattleStatusEffect::IncrementIntensity(uint8 amount)
{
    bool change = GlobalStatusEffect::IncrementIntensity(amount);
    _ProcessIntensityChange(!change);
    return change;
}

bool ActiveBattleStatusEffect::DecrementIntensity(uint8 amount)
{
    bool change = GlobalStatusEffect::DecrementIntensity(amount);
    _ProcessIntensityChange(!change);
    return change;
}

void ActiveBattleStatusEffect::_ProcessIntensityChange(bool reset_timer_only)
{
    _timer.Reset();
    _timer.Run();

    if(reset_timer_only)
        return;

    _intensity_changed = true;
    _icon_image = GlobalManager->Media().GetStatusIcon(_type, _intensity);
}

////////////////////////////////////////////////////////////////////////////////
// EffectsSupervisor class
////////////////////////////////////////////////////////////////////////////////

EffectsSupervisor::EffectsSupervisor(BattleActor *actor) :
    _actor(actor)
{
    // Reserve space for potential status effects,
    _status_effects.resize(GLOBAL_STATUS_TOTAL, NULL);

    if(!actor)
        PRINT_WARNING << "Invalid BattleActor* when initializing the Battle status effects supervisor." << std::endl;
}

EffectsSupervisor::~EffectsSupervisor()
{
    for(std::vector<ActiveBattleStatusEffect *>::iterator it = _status_effects.begin();
            it != _status_effects.end(); ++it) {
        delete (*it);
    }
    _status_effects.clear();
}

void EffectsSupervisor::SetActiveStatusEffects(GlobalCharacter* character)
{
    if (!character)
        return;

    character->ResetActiveStatusEffects();
    for(std::vector<ActiveBattleStatusEffect *>::iterator it = _status_effects.begin();
            it != _status_effects.end(); ++it) {
        ActiveBattleStatusEffect* effect = (*it);
        if (!effect)
            continue;

        // Copy the active status effect state
        SystemTimer* timer = effect->GetTimer();
        character->SetActiveStatusEffect(effect->GetType(), effect->GetIntensity(),
                                         timer->GetDuration(), timer->GetTimeExpired());
    }
}

void EffectsSupervisor::_UpdatePassive()
{
    for(uint32 i = 0; i < _equipment_status_effects.size(); ++i) {
        PassiveBattleStatusEffect& effect = _equipment_status_effects.at(i);

        if (!effect.GetUpdatePassiveFunction().is_valid())
            continue;

        // Update the update timer if it is running
        vt_system::SystemTimer *update_timer = effect.GetUpdateTimer();
        bool use_update_timer = effect.IsUsingUpdateTimer();
        if (use_update_timer) {
            BattleMode *BM = BattleMode::CurrentInstance();
            uint32 update_time = SystemManager->GetUpdateTime() * BM->GetBattleTypeTimeFactor();
            update_timer->Update(update_time);
        }

        if (!use_update_timer || update_timer->IsFinished()) {

            // Call the update passive function
            try {
                ScriptCallFunction<void>(effect.GetUpdatePassiveFunction(), _actor, effect.GetIntensity());
            } catch(const luabind::error &e) {
                PRINT_ERROR << "Error while loading status effect BattleUpdatePassive() function" << std::endl;
                ScriptManager->HandleLuaError(e);
            } catch(const luabind::cast_failed &e) {
                PRINT_ERROR << "Error while loading status effect BattleUpdatePassive() function" << std::endl;
                ScriptManager->HandleCastError(e);
            }

            // Restart the update timer when needed
            if (use_update_timer) {
                update_timer->Reset();
                update_timer->Run();
            }
        }
    }
}

void EffectsSupervisor::Update()
{
    // Do not update when states are paused
    BattleMode *BM = BattleMode::CurrentInstance();
    if (BM->IsInSceneMode() || BM->AreActorStatesPaused())
        return;

    // Update the timers and state for all active status effects
    for(uint32 i = 0; i < _status_effects.size(); ++i) {
        if(!_status_effects.at(i))
            continue;

        bool effect_removed = false;

        vt_system::SystemTimer *effect_timer = _status_effects[i]->GetTimer();
        vt_system::SystemTimer *update_timer = _status_effects[i]->GetUpdateTimer();

        // Update the effect time while taking in account the battle speed
        uint32 update_time = SystemManager->GetUpdateTime() * BM->GetBattleTypeTimeFactor();
        effect_timer->Update(update_time);

        // Update the update timer if it is running
        bool use_update_timer = _status_effects[i]->IsUsingUpdateTimer();
        if (use_update_timer)
            update_timer->Update(update_time);

        // Decrease the intensity of the status by one level when its timer expires. This may result in
        // the status effect being removed from the actor if its intensity changes to the neutral level.
        if(effect_timer->IsFinished()) {
            // If the intensity of the effect is at its weakest, the call that follows will remove the effect from the actor
            effect_removed = (_status_effects[i]->GetIntensity() == GLOBAL_INTENSITY_POS_LESSER
                              || _status_effects[i]->GetIntensity() == GLOBAL_INTENSITY_NEG_LESSER);

            // As the effect is fading, we divide the effect duration time per 2, with at least 1 second of duration.
            // This is done to give more a fading out style onto the effect and not to advantage/disadvantage the target
            // too much.
            uint32 duration = effect_timer->GetDuration() / 2;
            effect_timer->SetDuration(duration < 1000 ? 1000 : duration);

            if (_status_effects[i]->GetIntensity() > GLOBAL_INTENSITY_NEUTRAL)
                ChangeStatus(_status_effects[i]->GetType(), GLOBAL_INTENSITY_NEG_LESSER, duration);
            else
                ChangeStatus(_status_effects[i]->GetType(), GLOBAL_INTENSITY_POS_LESSER, duration);
        }

        if (effect_removed)
            continue;

        // Update the effect according to the script function
        if (!use_update_timer || update_timer->IsFinished()) {
            if (_status_effects[i]->GetUpdateFunction().is_valid()) {

                try {
                    ScriptCallFunction<void>(_status_effects[i]->GetUpdateFunction(), _status_effects[i]);
                } catch(const luabind::error &e) {
                    PRINT_ERROR << "Error while loading status effect Update function" << std::endl;
                    ScriptManager->HandleLuaError(e);
                } catch(const luabind::cast_failed &e) {
                    PRINT_ERROR << "Error while loading status effect Update function" << std::endl;
                    ScriptManager->HandleCastError(e);
                }
            }
            else {
                PRINT_WARNING << "No status effect Update function defined." << std::endl;
            }
            // If the character has his effects removed because of the effect update (when dying)
            // The effect doesn't exist anymore, so we have to check this here.
            if (!_status_effects[i])
                continue;

            _status_effects[i]->ResetIntensityChanged();

            // Restart the update timer when needed
            if (use_update_timer) {
                update_timer->Reset();
                update_timer->Run();
            }
        }
    }

    _UpdatePassive();
}

void EffectsSupervisor::Draw()
{
    // Draw in reverse to not overlap the arrow symbol
    VideoManager->MoveRelative(6.0f * 16.0f, 0.0f);

    for(std::vector<ActiveBattleStatusEffect *>::iterator it = _status_effects.begin(); it != _status_effects.end(); ++it) {
        if(*it) {
            (*it)->GetIconImage()->Draw();
            VideoManager->MoveRelative(-16.0f, 0.0f);
        }
    }
}

void EffectsSupervisor::DrawVertical()
{
    for(std::vector<ActiveBattleStatusEffect *>::reverse_iterator it = _status_effects.rbegin(); it != _status_effects.rend(); ++it) {
        if(*it) {
            (*it)->GetIconImage()->Draw();
            VideoManager->MoveRelative(0.0f, 16.0f);
        }
    }
}

void EffectsSupervisor::RemoveAllActiveStatusEffects()
{
    for(uint32 i = 0; i < _status_effects.size(); ++i) {
        _RemoveStatus(_status_effects[i]);
    }
}

bool EffectsSupervisor::ChangeStatus(GLOBAL_STATUS status, GLOBAL_INTENSITY intensity,
                                     uint32 duration, uint32 elapsed_time)
{
    if((status <= GLOBAL_STATUS_INVALID) || (status >= GLOBAL_STATUS_TOTAL)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received invalid status argument: " << status << std::endl;
        return false;
    }

    // Determine if we are attempting to increment or decrement the intensity of this status
    bool increase_intensity;
    if((intensity < GLOBAL_INTENSITY_NEUTRAL) && (intensity >= GLOBAL_INTENSITY_NEG_EXTREME)) {
        increase_intensity = false;
    } else if((intensity <= GLOBAL_INTENSITY_POS_EXTREME) && (intensity > GLOBAL_INTENSITY_NEUTRAL)) {
        increase_intensity = true;
    } else {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received invalid intensity argument: " << intensity << std::endl;
        return false;
    }

    // Holds the unsigned amount of change in intensity in either a positive or negative degree
    uint8 intensity_change = abs(static_cast<int8>(intensity));

    // Determine if this status (or its opposite) is already active on the actor
    // Holds a pointer to the active status
    ActiveBattleStatusEffect *active_effect = NULL;

    // Note: We should never run into the case where both the status and its opposite status are active simultaneously
    for(std::vector<ActiveBattleStatusEffect *>::iterator it = _status_effects.begin(); it != _status_effects.end(); ++it) {
        if(!(*it))
            continue;

        if((*it)->GetType() == status) {
            active_effect = *it;
            break;
        }
    }

    // variables used to determine the intensity change of the effect.
    GLOBAL_INTENSITY previous_intensity = GLOBAL_INTENSITY_INVALID;
    GLOBAL_INTENSITY new_intensity = GLOBAL_INTENSITY_INVALID;

    // Set the previous status and intensity return values to match the active effect, if one was found to exist
    if(active_effect == NULL) {
        previous_intensity = GLOBAL_INTENSITY_NEUTRAL;
    } else {
        previous_intensity = active_effect->GetIntensity();
    }

    BattleMode* BM = BattleMode::CurrentInstance();
    vt_mode_manager::IndicatorSupervisor& indicator = BM->GetIndicatorSupervisor();
    float x_pos = _actor->GetXLocation();
    float y_pos = _actor->GetYLocation() - (_actor->GetSpriteHeight() / 3 * 2);

    // Perform status changes according to the previously determined information
    if(active_effect) {
        if (increase_intensity)
            active_effect->IncrementIntensity(intensity_change);
        else
            active_effect->DecrementIntensity(intensity_change);

        new_intensity = active_effect->GetIntensity();

        // If the status was decremented to the neutral level, this means it is no longer active and should be removed
        if(new_intensity == GLOBAL_INTENSITY_NEUTRAL) {
            _RemoveStatus(active_effect);
            active_effect = NULL;
        }

        indicator.AddStatusIndicator(x_pos, y_pos, status, previous_intensity, new_intensity);
        return true;
    }
    else {
        _CreateNewStatus(status, intensity, duration, elapsed_time);
        new_intensity = intensity;

        indicator.AddStatusIndicator(x_pos, y_pos, status, previous_intensity, new_intensity);
    }

    return false;
} // bool EffectsSupervisor::ChangeStatus( ... )

void EffectsSupervisor::AddPassiveStatusEffect(vt_global::GLOBAL_STATUS status_effect, vt_global::GLOBAL_INTENSITY intensity)
{
    PassiveBattleStatusEffect effect(status_effect, intensity, _actor);
    _equipment_status_effects.push_back(effect);
}

void EffectsSupervisor::_CreateNewStatus(GLOBAL_STATUS status, GLOBAL_INTENSITY intensity,
                                         uint32 duration, uint32 elapsed_time)
{
    if((status <= GLOBAL_STATUS_INVALID) || (status >= GLOBAL_STATUS_TOTAL)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received invalid status argument: " << status << std::endl;
        return;
    }

    if((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received invalid intensity argument: " << intensity << std::endl;
        return;
    }

    // First, delete the potential former one.
    if(_status_effects[status])
        _RemoveStatus(_status_effects[status]);

    ActiveBattleStatusEffect *new_effect = new ActiveBattleStatusEffect(status, intensity, _actor, duration);

    // If there is already some elapsed time, we restore it
    if (elapsed_time > 0 && elapsed_time <= duration)
        new_effect->GetTimer()->SetTimeExpired(elapsed_time);

    _status_effects[status] = new_effect;

    if (!new_effect->GetApplyFunction().is_valid()) {
        PRINT_WARNING << "No valid status effect Apply function to call" << std::endl;
        return;
    }

    // Call the apply script function now that this new status is active on the actor
    try {
        ScriptCallFunction<void>(new_effect->GetApplyFunction(), new_effect);
    } catch(const luabind::error &e) {
        PRINT_ERROR << "Error while loading status effect Apply function" << std::endl;
        ScriptManager->HandleLuaError(e);
    } catch(const luabind::cast_failed &e) {
        PRINT_ERROR << "Error while loading status effect Apply function" << std::endl;
        ScriptManager->HandleCastError(e);
    }
}



void EffectsSupervisor::_RemoveStatus(ActiveBattleStatusEffect *status_effect)
{
    if(!status_effect)
        return;

    // Remove the status effect from the active effects list if it registered there.
    GLOBAL_STATUS effect_type = status_effect->GetType();
    if(_status_effects[effect_type] && _status_effects[effect_type] == status_effect) {

        if (status_effect->GetRemoveFunction().is_valid()) {
            try {
                ScriptCallFunction<void>(status_effect->GetRemoveFunction(), status_effect);
            } catch(const luabind::error &e) {
                PRINT_ERROR << "Error while loading status effect Remove function" << std::endl;
                ScriptManager->HandleLuaError(e);
            } catch(const luabind::cast_failed &e) {
                PRINT_ERROR << "Error while loading status effect Remove function" << std::endl;
                ScriptManager->HandleCastError(e);
            }
        }
        else {
            PRINT_WARNING << "No status effect Remove function defined." << std::endl;
        }

        _status_effects[effect_type] = 0;
    }
    // But delete the effect anyway.
    delete status_effect;
}

} // namespace private_battle

} // namespace vt_battle
