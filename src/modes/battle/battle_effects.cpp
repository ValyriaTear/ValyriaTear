////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_effects.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for battle actor effects.
*** ***************************************************************************/

#include "modes/battle/battle_effects.h"

#include "script/script.h"
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
                                                     GLOBAL_INTENSITY intensity):
    GlobalStatusEffect(type, intensity),
    _icon_image(nullptr)
{
    // Check that status effect base value are making it actually active
    if (!IsActive())
        return;

    // Make sure that a table entry exists for this status element
    uint32_t table_id = static_cast<uint32_t>(type);
    ReadScriptDescriptor &script_file = GlobalManager->GetStatusEffectsScript();
    if(!script_file.OpenTable(table_id)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "Lua definition file contained no entry for status effect: " << table_id << std::endl;
        return;
    }

    // Read in the status effect's property data
    std::string name = script_file.ReadString("name");
    _name.SetText(name, TextStyle("text14"));

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

    _icon_image = GlobalManager->Media().GetStatusIcon(type, intensity);
}

////////////////////////////////////////////////////////////////////////////////
// ActiveBattleStatusEffect class
////////////////////////////////////////////////////////////////////////////////

ActiveBattleStatusEffect::ActiveBattleStatusEffect():
    GlobalStatusEffect(GLOBAL_STATUS_INVALID, GLOBAL_INTENSITY_NEUTRAL),
    _timer(0),
    _icon_image(nullptr),
    _intensity_changed(false)
{}

ActiveBattleStatusEffect::ActiveBattleStatusEffect(GLOBAL_STATUS type, GLOBAL_INTENSITY intensity, uint32_t duration) :
    GlobalStatusEffect(type, intensity),
    _timer(0),
    _icon_image(nullptr),
    _intensity_changed(false)
{
    // Check that status effect base value are making it actually active
    if (!IsActive())
        return;

    // Make sure that a table entry exists for this status element
    uint32_t table_id = static_cast<uint32_t>(type);
    ReadScriptDescriptor &script_file = GlobalManager->GetStatusEffectsScript();
    if(!script_file.OpenTable(table_id)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "Lua definition file contained no entry for status effect: " << table_id << std::endl;
        return;
    }

    // Read in the status effect's property data
    std::string name = script_file.ReadString("name");
    _name.SetText(name, TextStyle("text14"));

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

bool ActiveBattleStatusEffect::IncrementIntensity(uint8_t amount)
{
    bool change = GlobalStatusEffect::IncrementIntensity(amount);
    _ProcessIntensityChange(!change);
    return change;
}

bool ActiveBattleStatusEffect::DecrementIntensity(uint8_t amount)
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

BattleStatusEffectsSupervisor::BattleStatusEffectsSupervisor(BattleActor* actor) :
    _actor(actor)
{
    // Reserve space for potential status effects,
    _active_status_effects.resize(GLOBAL_STATUS_TOTAL, ActiveBattleStatusEffect());

    if(!actor)
        PRINT_WARNING << "Invalid BattleActor* when initializing the Battle status effects supervisor." << std::endl;

    _infinite_text.SetText(" ∞ ");
}

void BattleStatusEffectsSupervisor::SetActiveStatusEffects(GlobalCharacter* character)
{
    if (!character)
        return;

    character->ResetActiveStatusEffects();
    for(std::vector<ActiveBattleStatusEffect>::iterator it = _active_status_effects.begin();
            it != _active_status_effects.end(); ++it) {
        ActiveBattleStatusEffect& effect = (*it);
        if (!effect.IsActive())
            continue;

        // Copy the active status effect state
        SystemTimer* timer = effect.GetTimer();
        character->SetActiveStatusEffect(effect.GetType(), effect.GetIntensity(),
                                         timer->GetDuration(), timer->GetTimeExpired());
    }
}

void BattleStatusEffectsSupervisor::_UpdatePassive()
{
    for(uint32_t i = 0; i < _equipment_status_effects.size(); ++i) {
        PassiveBattleStatusEffect& effect = _equipment_status_effects.at(i);

        if (!effect.GetUpdatePassiveFunction().is_valid())
            continue;

        // Update the update timer if it is running
        vt_system::SystemTimer *update_timer = effect.GetUpdateTimer();
        bool use_update_timer = effect.IsUsingUpdateTimer();
        if (use_update_timer) {
            BattleMode *BM = BattleMode::CurrentInstance();
            uint32_t update_time = SystemManager->GetUpdateTime() * BM->GetBattleTypeTimeFactor();
            update_timer->Update(update_time);
        }

        if (!use_update_timer || update_timer->IsFinished()) {

            // Call the update passive function
            try {
                luabind::call_function<void>(effect.GetUpdatePassiveFunction(), _actor, effect.GetIntensity());
            } catch(const luabind::error& e) {
                PRINT_ERROR << "Error while loading status effect BattleUpdatePassive() function" << std::endl;
                ScriptManager->HandleLuaError(e);
            } catch(const luabind::cast_failed& e) {
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

void BattleStatusEffectsSupervisor::Update()
{
    // Do not update when states are paused
    BattleMode* BM = BattleMode::CurrentInstance();
    if (BM->IsInSceneMode() || BM->AreActorStatesPaused())
        return;

    // Update the timers and state for all active status effects
    for(uint32_t i = 0; i < _active_status_effects.size(); ++i) {
        ActiveBattleStatusEffect& effect = _active_status_effects[i];
        if(!effect.IsActive())
            continue;

        bool effect_removed = false;

        vt_system::SystemTimer* effect_timer = effect.GetTimer();
        vt_system::SystemTimer* update_timer = effect.GetUpdateTimer();

        // Update the effect time while taking in account the battle speed
        uint32_t update_time = SystemManager->GetUpdateTime() * BM->GetBattleTypeTimeFactor();
        effect_timer->Update(update_time);

        // Update the update timer if it is running
        bool use_update_timer = effect.IsUsingUpdateTimer();
        if (use_update_timer)
            update_timer->Update(update_time);

        // Decrease the intensity of the status by one level when its timer expires. This may result in
        // the status effect being removed from the actor if its intensity changes to the neutral level.
        if(effect_timer->IsFinished()) {

            // If the intensity of the effect is at its weakest, the call that follows will remove the effect from the actor
            effect_removed = (effect.GetIntensity() == GLOBAL_INTENSITY_POS_LESSER
                              || effect.GetIntensity() == GLOBAL_INTENSITY_NEG_LESSER);

            // As the effect is fading, we divide the effect duration time per 2, with at least 1 second of duration.
            // This is done to give more a fading out style onto the effect and not to advantage/disadvantage the target
            // too much.
            uint32_t duration = effect_timer->GetDuration() / 2;
            effect_timer->SetDuration(duration < 1000 ? 1000 : duration);

            if (effect.GetIntensity() > GLOBAL_INTENSITY_NEUTRAL)
                ChangeActiveStatusEffect(effect.GetType(), GLOBAL_INTENSITY_NEG_LESSER, duration);
            else
                ChangeActiveStatusEffect(effect.GetType(), GLOBAL_INTENSITY_POS_LESSER, duration);
        }

        if (effect_removed)
            continue;

        // Update the effect according to the script function
        if (!use_update_timer || update_timer->IsFinished()) {
            if (effect.GetUpdateFunction().is_valid()) {

                try {
                    luabind::call_function<void>(effect.GetUpdateFunction(), _actor, effect);
                } catch(const luabind::error& e) {
                    PRINT_ERROR << "Error while loading status effect BattleUpdate() function" << std::endl;
                    ScriptManager->HandleLuaError(e);
                } catch(const luabind::cast_failed& e) {
                    PRINT_ERROR << "Error while loading status effect BattleUpdate() function" << std::endl;
                    ScriptManager->HandleCastError(e);
                }
            }

            // If the character has his effects removed because of the effect update (when dying)
            // The effect isn't active anymore, so we have to check this here.
            if (!effect.IsActive())
                continue;

            effect.ResetIntensityChanged();

            // Restart the update timer when needed
            if (use_update_timer) {
                update_timer->Reset();
                update_timer->Run();
            }
        }
    }

    _UpdatePassive();
}

void BattleStatusEffectsSupervisor::Draw()
{
    // Draw in reverse to not overlap the arrow symbol
    VideoManager->MoveRelative(6.0f * 16.0f, 0.0f);

    for(std::vector<PassiveBattleStatusEffect>::iterator it = _equipment_status_effects.begin();
            it != _equipment_status_effects.end(); ++it) {
        PassiveBattleStatusEffect& effect = *it;
        if (!effect.IsActive())
            continue;

        effect.GetIconImage()->Draw();
        VideoManager->MoveRelative(0.0f, 5.0f);
        _infinite_text.Draw();
        VideoManager->MoveRelative(0.0f, -5.0f);
        VideoManager->MoveRelative(-16.0f, 0.0f);
    }

    for(std::vector<ActiveBattleStatusEffect>::iterator it = _active_status_effects.begin();
            it != _active_status_effects.end(); ++it) {
        ActiveBattleStatusEffect& effect = *it;
        if (!effect.IsActive())
            continue;

        effect.GetIconImage()->Draw();

        // Draw remaining effect time
        vt_system::SystemTimer* effect_timer = effect.GetTimer();
        uint32_t duration = effect_timer->GetDuration();
        uint32_t time_left = effect_timer->TimeLeft();
        VideoManager->DrawRectangle(20.0f, 5.0f, Color::blue);
        uint32_t length_left = 20.0f / duration * time_left;
        VideoManager->DrawRectangle(length_left, 5.0f, Color::blue_sp);

        VideoManager->MoveRelative(-16.0f, 0.0f);

    }
}

void BattleStatusEffectsSupervisor::DrawVertical()
{
    for(std::vector<PassiveBattleStatusEffect>::reverse_iterator it = _equipment_status_effects.rbegin();
            it != _equipment_status_effects.rend(); ++it) {
        PassiveBattleStatusEffect& effect = *it;
        if (!effect.IsActive())
            continue;

        effect.GetIconImage()->Draw();
        VideoManager->MoveRelative(0.0f, 5.0f);
        _infinite_text.Draw();
        VideoManager->MoveRelative(20.0f, -5.0f);
        effect.GetName().Draw();
        VideoManager->MoveRelative(-20.0f, 0.0f);
        VideoManager->MoveRelative(0.0f, 16.0f);
    }

    for(std::vector<ActiveBattleStatusEffect>::reverse_iterator it = _active_status_effects.rbegin();
            it != _active_status_effects.rend(); ++it) {
        ActiveBattleStatusEffect& effect = *it;
        if (!effect.IsActive())
            continue;

        effect.GetIconImage()->Draw();

        // Draw remaining effect time
        vt_system::SystemTimer* effect_timer = effect.GetTimer();
        uint32_t duration = effect_timer->GetDuration();
        uint32_t time_left = effect_timer->TimeLeft();
        VideoManager->DrawRectangle(20.0f, 5.0f, Color::blue);
        uint32_t length_left = 20.0f / duration * time_left;
        VideoManager->DrawRectangle(length_left, 5.0f, Color::blue_sp);

        VideoManager->MoveRelative(20.0f, 0.0f);
        effect.GetName().Draw();
        VideoManager->MoveRelative(-20.0f, 0.0f);

        VideoManager->MoveRelative(0.0f, 16.0f);
    }
}

void BattleStatusEffectsSupervisor::RemoveAllActiveStatusEffects()
{
    for(uint32_t i = 0; i < _active_status_effects.size(); ++i) {
        RemoveActiveStatusEffect((GLOBAL_STATUS)i);
    }
}

bool BattleStatusEffectsSupervisor::ChangeActiveStatusEffect(GLOBAL_STATUS status, GLOBAL_INTENSITY intensity,
                                                 uint32_t duration, uint32_t elapsed_time)
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
    uint8_t intensity_change = abs(static_cast<int8_t>(intensity));

    // Determine if this status (or its opposite) is already active on the actor
    // Holds a reference to the active status
    ActiveBattleStatusEffect& active_effect = _active_status_effects[status];

    // variables used to determine the intensity change of the effect.
    GLOBAL_INTENSITY previous_intensity = active_effect.GetIntensity();
    GLOBAL_INTENSITY new_intensity = GLOBAL_INTENSITY_INVALID;

    BattleMode* BM = BattleMode::CurrentInstance();
    vt_mode_manager::IndicatorSupervisor& indicator = BM->GetIndicatorSupervisor();
    float x_pos = _actor->GetXLocation();
    float y_pos = _actor->GetYLocation() - (_actor->GetSpriteHeight() / 3 * 2);

    // Perform status changes according to the previously determined information
    if(active_effect.IsActive()) {
        if (increase_intensity)
            active_effect.IncrementIntensity(intensity_change);
        else
            active_effect.DecrementIntensity(intensity_change);

        new_intensity = active_effect.GetIntensity();

        // If the status was decremented to the neutral level, this means it is no longer active and should be removed
        if(new_intensity == GLOBAL_INTENSITY_NEUTRAL)
            RemoveActiveStatusEffect(status, true);

        indicator.AddStatusIndicator(x_pos, y_pos, status, previous_intensity, new_intensity);
        return true;
    } else {
        _CreateNewStatus(status, intensity, duration, elapsed_time);
        new_intensity = intensity;

        indicator.AddStatusIndicator(x_pos, y_pos, status, previous_intensity, new_intensity);
    }
    return false;
} // bool EffectsSupervisor::ChangeStatus( ... )

void BattleStatusEffectsSupervisor::AddPassiveStatusEffect(vt_global::GLOBAL_STATUS status_effect, vt_global::GLOBAL_INTENSITY intensity)
{
    PassiveBattleStatusEffect effect(status_effect, intensity);
    _equipment_status_effects.push_back(effect);
}

void BattleStatusEffectsSupervisor::_CreateNewStatus(GLOBAL_STATUS status, GLOBAL_INTENSITY intensity,
                                         uint32_t duration, uint32_t elapsed_time)
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
    RemoveActiveStatusEffect(status);

    _active_status_effects[status] = ActiveBattleStatusEffect(status, intensity, duration);
    ActiveBattleStatusEffect& new_effect = _active_status_effects[status];

    // If there is already some elapsed time, we restore it
    if (elapsed_time > 0 && elapsed_time <= duration)
        new_effect.GetTimer()->SetTimeExpired(elapsed_time);

    if (!new_effect.GetApplyFunction().is_valid())
        return;

    // Call the apply script function now that this new status is active on the actor
    try {
        luabind::call_function<void>(new_effect.GetApplyFunction(), _actor, new_effect);
    } catch(const luabind::error& e) {
        PRINT_ERROR << "Error while loading status effect BattleApply() function" << std::endl;
        ScriptManager->HandleLuaError(e);
    } catch(const luabind::cast_failed& e) {
        PRINT_ERROR << "Error while loading status effect BattleApply() function" << std::endl;
        ScriptManager->HandleCastError(e);
    }
}

void BattleStatusEffectsSupervisor::RemoveActiveStatusEffect(GLOBAL_STATUS status_effect_type, bool remove_anyway)
{
    ActiveBattleStatusEffect& status_effect = _active_status_effects[status_effect_type];

    if(!remove_anyway && !status_effect.IsActive())
        return;

    if (status_effect.GetRemoveFunction().is_valid()) {
        try {
            luabind::call_function<void>(status_effect.GetRemoveFunction(), _actor, status_effect);
        } catch(const luabind::error& e) {
            PRINT_ERROR << "Error while loading status effect BattleRemove() function" << std::endl;
            ScriptManager->HandleLuaError(e);
        } catch(const luabind::cast_failed& e) {
            PRINT_ERROR << "Error while loading status effect BattleRemove() function" << std::endl;
            ScriptManager->HandleCastError(e);
        }
    }

    status_effect.Disable();
}

} // namespace private_battle

} // namespace vt_battle
