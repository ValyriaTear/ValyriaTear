////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2014-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_status_effects.cpp
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for map active status effects handling.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/map/map_status_effects.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_sprites.h"

#include "engine/script/script_read.h"
#include "common/global/global.h"
#include "engine/video/video.h"

using namespace vt_global;

namespace vt_map
{

namespace private_map
{

//! \brief Gives the X effect position from the given index
float _GetEffectXPositionFromCharacterIndex(uint32_t index)
{
    return 90.0f + (float)(index * 130);
}

//! \brief Where on screen (on y-axis) the active status effects icon should be displayed
const float EFFECTS_Y_POS = 750.0f;

////////////////////////////////////////////////////////////////////////////////
// PassiveMapStatusEffect class
////////////////////////////////////////////////////////////////////////////////

PassiveMapStatusEffect::PassiveMapStatusEffect(vt_global::GlobalCharacter* character,
                                               vt_global::GLOBAL_STATUS type,
                                               vt_global::GLOBAL_INTENSITY intensity):
    GlobalStatusEffect(type, intensity),
    _affected_character(character),
    _icon_image(nullptr)
{
    // Check that the constructor arguments are valid
    if((type <= GLOBAL_STATUS_INVALID) || (type >= GLOBAL_STATUS_TOTAL)) {
        PRINT_WARNING << "The constructor received an invalid type argument: " << type << std::endl;
        return;
    }
    if((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
        PRINT_WARNING << "The constructor received an invalid intensity argument: " << intensity << std::endl;
        return;
    }
    if(character == nullptr) {
        PRINT_WARNING << "The constructor received nullptr character argument" << std::endl;
        return;
    }

    // Make sure that a table entry exists for this status element
    uint32_t table_id = static_cast<uint32_t>(type);
    vt_script::ReadScriptDescriptor &script_file = vt_global::GlobalManager->GetStatusEffectsScript();
    if(!script_file.OpenTable(table_id)) {
        PRINT_WARNING << "Lua definition file contained no entry for status effect: " << table_id << std::endl;
        return;
    }

    // Read in the status effect's property data
    _name = script_file.ReadString("name");

    if(script_file.DoesFunctionExist("MapUpdatePassive")) {
        _update_passive_function = script_file.ReadFunctionPointer("MapUpdatePassive");
    } else {
        PRINT_WARNING << "No MapUpdatePassive() function found in Lua definition file for status: " << table_id << std::endl;
    }

    script_file.CloseTable(); // table_id

    if(script_file.IsErrorDetected()) {
        PRINT_WARNING << "one or more errors occurred while reading status effect data - they are listed below"
            << std::endl << script_file.GetErrorMessages() << std::endl;
    }

    _icon_image = GlobalManager->Media().GetStatusIcon(_type, _intensity);
}

////////////////////////////////////////////////////////////////////////////////
// ActiveMapStatusEffect class
////////////////////////////////////////////////////////////////////////////////

ActiveMapStatusEffect::ActiveMapStatusEffect(vt_global::GlobalCharacter* character,
                                             vt_global::GLOBAL_STATUS type,
                                             vt_global::GLOBAL_INTENSITY intensity,
                                             uint32_t duration) :
    GlobalStatusEffect(type, intensity),
    _affected_character(character),
    _timer(0),
    _icon_image(nullptr),
    _intensity_changed(false)
{
    // Check that the constructor arguments are valid
    if((type <= GLOBAL_STATUS_INVALID) || (type >= GLOBAL_STATUS_TOTAL)) {
        PRINT_WARNING << "The constructor received an invalid type argument: " << type << std::endl;
        return;
    }
    if((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
        PRINT_WARNING << "The constructor received an invalid intensity argument: " << intensity << std::endl;
        return;
    }
    if(character == nullptr) {
        PRINT_WARNING << "The constructor received nullptr character argument" << std::endl;
        return;
    }

    // Make sure that a table entry exists for this status element
    uint32_t table_id = static_cast<uint32_t>(type);
    vt_script::ReadScriptDescriptor &script_file = vt_global::GlobalManager->GetStatusEffectsScript();
    if(!script_file.OpenTable(table_id)) {
        PRINT_WARNING << "Lua definition file contained no entry for status effect: " << table_id << std::endl;
        return;
    }

    // Read in the status effect's property data
    _name = script_file.ReadString("name");

    // Read the fall back duration when none is given.
    if(duration == 0)
        duration = script_file.ReadUInt("default_duration");
    _timer.SetDuration(duration);

    if(script_file.DoesFunctionExist("MapApply")) {
        _apply_function = script_file.ReadFunctionPointer("MapApply");
    } else {
        PRINT_WARNING << "No MapApply() function found in Lua definition file for status: " << table_id << std::endl;
    }

    if(script_file.DoesFunctionExist("MapUpdate")) {
        _update_function = script_file.ReadFunctionPointer("MapUpdate");
    } else {
        PRINT_WARNING << "No MapUpdate() function found in Lua definition file for status: " << table_id << std::endl;
    }

    if(script_file.DoesFunctionExist("MapRemove")) {
        _remove_function = script_file.ReadFunctionPointer("MapRemove");
    } else {
        PRINT_WARNING << "No MapRemove() function found in Lua definition file for status: " << table_id << std::endl;
    }
    script_file.CloseTable(); // table_id

    if(script_file.IsErrorDetected()) {
        PRINT_WARNING << "one or more errors occurred while reading status effect data - they are listed below"
            << std::endl << script_file.GetErrorMessages() << std::endl;
    }

    // Init the effect timer
    _timer.EnableManualUpdate();
    _timer.Reset();
    _timer.Run();

    _icon_image = GlobalManager->Media().GetStatusIcon(_type, _intensity);
}

void ActiveMapStatusEffect::SetIntensity(vt_global::GLOBAL_INTENSITY intensity)
{
    if((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
        PRINT_WARNING << "Attempted to set status effect to invalid intensity: " << intensity << std::endl;
        return;
    }

    bool no_intensity_change = (_intensity == intensity);
    _intensity = intensity;
    _ProcessIntensityChange(no_intensity_change);
}

bool ActiveMapStatusEffect::IncrementIntensity(uint8_t amount)
{
    bool change = GlobalStatusEffect::IncrementIntensity(amount);
    _ProcessIntensityChange(!change);
    return change;
}

bool ActiveMapStatusEffect::DecrementIntensity(uint8_t amount)
{
    bool change = GlobalStatusEffect::DecrementIntensity(amount);
    _ProcessIntensityChange(!change);
    return change;
}

void ActiveMapStatusEffect::_ProcessIntensityChange(bool reset_timer_only)
{
    _timer.Reset();
    _timer.Run();

    if(reset_timer_only)
        return;

    _intensity_changed = true;
    _icon_image = vt_global::GlobalManager->Media().GetStatusIcon(_type, _intensity);
}

////////////////////////////////////////////////////////////////////////////////
// CharacterIndication class
////////////////////////////////////////////////////////////////////////////////

CharacterIndication::CharacterIndication(vt_global::GlobalCharacter* character,
                                         float x_position, float y_position):
        _x_position(x_position),
        _y_position(y_position),
        _image_alpha(0.0f),
        _fade_in(false),
        _fade_out(false),
        _display_time(0),
        _global_character(character)
{
    // Loads a copy of the portrait.
    if (character)
        _portrait.Load(character->GetPortrait().GetFilename());

    if (!_portrait.GetFilename().empty())
        _portrait.SetHeightKeepRatio(65.0f);
}

void CharacterIndication::Update()
{
    uint32_t elapsed_time = vt_system::SystemManager->GetUpdateTime();
    // Apply fading
    if (_fade_out) {
        _image_alpha -= 0.005f * (float)elapsed_time;
        if (_image_alpha <= 0.0f) {
            _image_alpha = 0.0f;
            _fade_out = false;
        }
    }
    else if (_fade_in) {
        _image_alpha += 0.005f * (float)elapsed_time;
        if (_image_alpha >= 1.0f) {
            _image_alpha = 1.0f;
            _fade_in = false;
        }
    }

    // Check time limit
    if (_display_time <= 0) {
        _display_time = 0;
        FadeOut();
        return;
    }

    // Update display time otherwise
    _display_time -= (int32_t)elapsed_time;
}

void CharacterIndication::Draw() {
    if (_image_alpha <= 0.0f)
        return;

    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_RIGHT, vt_video::VIDEO_Y_BOTTOM, vt_video::VIDEO_BLEND, 0);
    vt_video::VideoManager->Move(_x_position , _y_position);

    _portrait.Draw(vt_video::Color(1.0f, 1.0f, 1.0f, _image_alpha));
}

////////////////////////////////////////////////////////////////////////////////
// MapStatusEffectsSupervisor class
////////////////////////////////////////////////////////////////////////////////

MapStatusEffectsSupervisor::MapStatusEffectsSupervisor()
{
    LoadStatusEffects();
}

MapStatusEffectsSupervisor::~MapStatusEffectsSupervisor()
{
    SaveActiveStatusEffects();
}

void MapStatusEffectsSupervisor::LoadStatusEffects()
{
    // First, wipe out every old data
    _active_status_effects.clear();
    _equipment_status_effects.clear();
    _characters_portraits.clear();

    std::vector<GlobalCharacter*>* characters = GlobalManager->GetOrderedCharacters();
    if (!characters)
        return;

    // For each character, we load the status effects data
    for (uint32_t i = 0; i < characters->size(); ++i) {
        GlobalCharacter* character = (*characters)[i];

        if (!character)
            continue;

        // Loads or reloads the Character portraits positions (in case the party order has changed)
        float x_pos = _GetEffectXPositionFromCharacterIndex(i);
        _characters_portraits.push_back(CharacterIndication(character, x_pos, EFFECTS_Y_POS));

        // passive effects
        const std::vector<GLOBAL_INTENSITY>& passives = character->GetEquipementStatusEffects();
        for (uint32_t j = 0; j < passives.size(); ++j) {
            GLOBAL_STATUS status = (vt_global::GLOBAL_STATUS)j;
            GLOBAL_INTENSITY intensity = passives.at(j);

            if (status < GLOBAL_STATUS_TOTAL
                    && intensity != GLOBAL_INTENSITY_INVALID
                    && intensity != GLOBAL_INTENSITY_NEUTRAL) {
                _AddPassiveStatusEffect(character, status, intensity);
            }
        }

        // active effects
        const std::vector<ActiveStatusEffect>& actives = character->GetActiveStatusEffects();
        for (uint32_t j = 0; j < actives.size(); ++j) {
            GLOBAL_STATUS status = actives.at(j).GetEffect();
            GLOBAL_INTENSITY intensity = actives.at(j).GetIntensity();

            if (status < GLOBAL_STATUS_TOTAL
                    && intensity != GLOBAL_INTENSITY_INVALID
                    && intensity != GLOBAL_INTENSITY_NEUTRAL) {
                _AddActiveStatusEffect(character, status, intensity,
                                       actives.at(j).GetEffectTime(),
                                       actives.at(j).GetElapsedTime());
            }
        }
    }
}

void MapStatusEffectsSupervisor::SaveActiveStatusEffects()
{
    // first, we clear the old data from the characters
    std::vector<GlobalCharacter*>* characters = GlobalManager->GetOrderedCharacters();
    if (!characters)
        return;

    for (uint32_t i = 0; i < characters->size(); ++i) {
        GlobalCharacter* character = (*characters)[i];

        if (!character)
            continue;

        character->ResetActiveStatusEffects();
    }

    // Then, we copy every active status effects back to the affected character.
    for (uint32_t i = 0; i < _active_status_effects.size(); ++i) {
        ActiveMapStatusEffect& effect = _active_status_effects[i];
        if (effect.GetType() == GLOBAL_STATUS_INVALID)
            continue;

        GlobalCharacter* character = effect.GetAffectedCharacter();
        if (!character)
            continue;

        vt_system::SystemTimer* timer = effect.GetTimer();
        character->SetActiveStatusEffect(effect.GetType(), effect.GetIntensity(),
                                         timer->GetDuration(), timer->GetTimeExpired());
    }
}

void MapStatusEffectsSupervisor::_UpdatePassive()
{
    for(uint32_t i = 0; i < _equipment_status_effects.size(); ++i) {
        PassiveMapStatusEffect& effect = _equipment_status_effects.at(i);

        if (!effect.GetUpdatePassiveFunction().is_valid())
            continue;

        // Update the update timer if it is running
        vt_system::SystemTimer *update_timer = effect.GetUpdateTimer();
        bool use_update_timer = effect.IsUsingUpdateTimer();
        if (use_update_timer) {
            uint32_t update_time = vt_system::SystemManager->GetUpdateTime();
            update_timer->Update(update_time);
        }

        if (!use_update_timer || update_timer->IsFinished()) {

            // Call the update passive function
            try {
                ScriptCallFunction<void>(effect.GetUpdatePassiveFunction(), effect.GetAffectedCharacter(), effect.GetIntensity());
            } catch(const luabind::error& e) {
                PRINT_ERROR << "Error while loading status effect MapUpdatePassive() function" << std::endl;
                vt_script::ScriptManager->HandleLuaError(e);
            } catch(const luabind::cast_failed& e) {
                PRINT_ERROR << "Error while loading status effect MapUpdatePassive() function" << std::endl;
                vt_script::ScriptManager->HandleCastError(e);
            }

            // Restart the update timer when needed
            if (use_update_timer) {
                update_timer->Reset();
                update_timer->Run();
            }
        }
    }
}

void MapStatusEffectsSupervisor::UpdateEffects()
{
    // Update the timers and state for all active status effects
    std::vector<ActiveMapStatusEffect>::iterator it = _active_status_effects.begin();
    for(; it != _active_status_effects.end();) {
        ActiveMapStatusEffect& effect = *it;
        if(!effect.IsActive()) {
            // Remove from loop
            it = _active_status_effects.erase(it);
            continue;
        }

        bool effect_removed = false;

        vt_system::SystemTimer* effect_timer = effect.GetTimer();
        vt_system::SystemTimer* update_timer = effect.GetUpdateTimer();

        // Update the effect time while taking in account the battle speed
        uint32_t update_time = vt_system::SystemManager->GetUpdateTime();
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

            if (effect.GetIntensity() > GLOBAL_INTENSITY_NEUTRAL) {
                ChangeActiveStatusEffect(effect, GLOBAL_INTENSITY_NEG_LESSER, duration);
            }
            else {
                ChangeActiveStatusEffect(effect, GLOBAL_INTENSITY_POS_LESSER, duration);
            }
        }

        if (effect_removed) {
            // Remove from loop
            it = _active_status_effects.erase(it);
            continue;
        }

        // Update the effect according to the script function
        if (!use_update_timer || update_timer->IsFinished()) {
            if (effect.GetUpdateFunction().is_valid()) {

                try {
                    ScriptCallFunction<void>(effect.GetUpdateFunction(), effect);
                } catch(const luabind::error& e) {
                    PRINT_ERROR << "Error while loading status effect Update function" << std::endl;
                    vt_script::ScriptManager->HandleLuaError(e);
                } catch(const luabind::cast_failed& e) {
                    PRINT_ERROR << "Error while loading status effect Update function" << std::endl;
                    vt_script::ScriptManager->HandleCastError(e);
                }
            }
            else {
                PRINT_WARNING << "No status effect Update function defined." << std::endl;
            }
            // If the character has his effects removed because of the effect update (when dying)
            // The effect doesn't exist anymore, so we have to check this here.
            if(!effect.IsActive()) {
                // Remove from loop
                it = _active_status_effects.erase(it);
                continue;
            }

            effect.ResetIntensityChanged();

            // Restart the update timer when needed
            if (use_update_timer) {
                update_timer->Reset();
                update_timer->Run();
            }
        }

        ++it;
    }

    _UpdatePassive();
}

void MapStatusEffectsSupervisor::UpdatePortraits()
{
    // Update portrait indicators
    for (uint32_t i = 0; i < _characters_portraits.size(); ++i)
        _characters_portraits[i].Update();
}

void MapStatusEffectsSupervisor::Draw()
{
    // Draw character portraits shown when effects changes are triggered.
    for (uint32_t i = 0; i < _characters_portraits.size(); ++i)
        _characters_portraits[i].Draw();
}

bool MapStatusEffectsSupervisor::ChangeActiveStatusEffect(GlobalCharacter* character,
                                                          vt_global::GLOBAL_STATUS status_type,
                                                          vt_global::GLOBAL_INTENSITY intensity,
                                                          uint32_t duration, uint32_t elapsed_time,
                                                          bool display_change)
{
    if (!character)
        return false;

    if (intensity == GLOBAL_INTENSITY_NEUTRAL
            || intensity <= GLOBAL_INTENSITY_INVALID
            || intensity >= GLOBAL_INTENSITY_TOTAL)
        return false;

    for (uint32_t i = 0; i < _active_status_effects.size(); ++i) {
        ActiveMapStatusEffect& active_effect = _active_status_effects[i];

        if (active_effect.GetType() != status_type)
            continue;
        if (active_effect.GetAffectedCharacter() != character)
            continue;

        // If the effect is found, we apply the new change on it.
        return ChangeActiveStatusEffect(active_effect, intensity, duration, elapsed_time, display_change);
    }

    // Add a new status effect
    _AddActiveStatusEffect(character, status_type, intensity, duration);

    if (!display_change)
        return true;

    vt_mode_manager::IndicatorSupervisor& indicator = MapMode::CurrentInstance()->GetIndicatorSupervisor();
    indicator.AddStatusIndicator(_GetEffectAnimationXPosition(character), EFFECTS_Y_POS,
                                 status_type, GLOBAL_INTENSITY_NEUTRAL, intensity);
    _MakeCharacterPortraitAppear(character, vt_mode_manager::INDICATOR_TIME); // default time in milliseconds
    return true;
}

bool MapStatusEffectsSupervisor::ChangeActiveStatusEffect(ActiveMapStatusEffect& active_effect,
                                                          GLOBAL_INTENSITY intensity,
                                                          uint32_t duration, uint32_t elapsed_time,
                                                          bool display_change)
{
    if (!active_effect.IsActive())
        return false;

    // Determine if we are attempting to increment or decrement the intensity of this status
    bool increase_intensity = false;
    if((intensity <= GLOBAL_INTENSITY_POS_EXTREME) && (intensity > GLOBAL_INTENSITY_NEUTRAL))
        increase_intensity = true;

    // Holds the unsigned amount of change in intensity in either a positive or negative degree
    uint8_t intensity_change = abs(static_cast<int8_t>(intensity));

    // variables used to determine the intensity change of the effect.
    GLOBAL_INTENSITY previous_intensity = GLOBAL_INTENSITY_INVALID;
    GLOBAL_INTENSITY new_intensity = GLOBAL_INTENSITY_INVALID;

    // Set the previous status and intensity return values to match the active effect, if one was found to exist
    previous_intensity = active_effect.GetIntensity();

    // Set the coordinates of the status effect next to the corresponding portrait
    vt_mode_manager::IndicatorSupervisor& indicator = MapMode::CurrentInstance()->GetIndicatorSupervisor();
    vt_global::GlobalCharacter* character = active_effect.GetAffectedCharacter();
    float x_pos = _GetEffectAnimationXPosition(character);

    // Perform status changes according to the previously determined information
    if(active_effect.IsActive()) {
        if (increase_intensity)
            active_effect.IncrementIntensity(intensity_change);
        else
            active_effect.DecrementIntensity(intensity_change);

        new_intensity = active_effect.GetIntensity();

        // If the status was decremented to the neutral level, this means it is no longer active and should be removed
        // NOTE: The actual removal of the effect will be done in the Update() loop.
        if(new_intensity == GLOBAL_INTENSITY_NEUTRAL)
            _RemoveActiveStatusEffect(active_effect);

        if (!display_change)
            return true;

        indicator.AddStatusIndicator(x_pos, EFFECTS_Y_POS, active_effect.GetType(), previous_intensity, new_intensity);
        _MakeCharacterPortraitAppear(character, vt_mode_manager::INDICATOR_TIME);
        return true;
    }
    else {
        _AddActiveStatusEffect(active_effect.GetAffectedCharacter(), active_effect.GetType(), intensity, duration, elapsed_time);
        new_intensity = intensity;

        if (!display_change)
            return true;

        indicator.AddStatusIndicator(x_pos, EFFECTS_Y_POS, active_effect.GetType(), previous_intensity, new_intensity);
        _MakeCharacterPortraitAppear(character, vt_mode_manager::INDICATOR_TIME);
        return true;
    }

    return false;
} // bool MapStatusEffectsSupervisor::ChangeActiveStatusEffect( ... )

GLOBAL_INTENSITY MapStatusEffectsSupervisor::GetActiveStatusEffectIntensity(GlobalCharacter* character,
                                                                            GLOBAL_STATUS status_type) const
{
    if (character == nullptr)
        return GLOBAL_INTENSITY_INVALID;

    if (status_type == GLOBAL_STATUS_INVALID || status_type == GLOBAL_STATUS_TOTAL)
        return GLOBAL_INTENSITY_INVALID;

    for (uint32_t i = 0; i < _active_status_effects.size(); ++i) {
        const ActiveMapStatusEffect& effect = _active_status_effects[i];
        if (effect.GetType() != status_type)
            continue;

        if (effect.GetAffectedCharacter() != character)
            continue;

        return effect.GetIntensity();
    }

    return GLOBAL_INTENSITY_NEUTRAL;
}

void MapStatusEffectsSupervisor::_AddActiveStatusEffect(GlobalCharacter* character,
                                                        GLOBAL_STATUS status, GLOBAL_INTENSITY intensity,
                                                        uint32_t duration, uint32_t elapsed_time)
{
    if((status <= GLOBAL_STATUS_INVALID) || (status >= GLOBAL_STATUS_TOTAL)) {
        PRINT_WARNING << "Function received invalid status argument: " << status << std::endl;
        return;
    }

    if((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
        PRINT_WARNING << "Function received invalid intensity argument: " << intensity << std::endl;
        return;
    }

    _active_status_effects.push_back(ActiveMapStatusEffect(character, status, intensity, duration));
    ActiveMapStatusEffect& new_effect = _active_status_effects.back();

    // If there is already some elapsed time, we restore it
    if (elapsed_time > 0 && elapsed_time <= duration)
        new_effect.GetTimer()->SetTimeExpired(elapsed_time);

    if (!new_effect.GetApplyFunction().is_valid()) {
        PRINT_WARNING << "No valid status effect Apply function to call" << std::endl;
        return;
    }

    // Call the apply script function now that this new status is active on the actor
    try {
        ScriptCallFunction<void>(new_effect.GetApplyFunction(), new_effect);
    } catch(const luabind::error& e) {
        PRINT_ERROR << "Error while loading status effect Apply function" << std::endl;
        vt_script::ScriptManager->HandleLuaError(e);
    } catch(const luabind::cast_failed& e) {
        PRINT_ERROR << "Error while loading status effect Apply function" << std::endl;
        vt_script::ScriptManager->HandleCastError(e);
    }
}

void MapStatusEffectsSupervisor::_RemoveActiveStatusEffect(ActiveMapStatusEffect& status_effect)
{
    if(!status_effect.IsActive())
        return;

    // Remove the status effect from the active effects list if it registered there.
    if (status_effect.GetRemoveFunction().is_valid()) {
        try {
            ScriptCallFunction<void>(status_effect.GetRemoveFunction(), status_effect);
        } catch(const luabind::error& e) {
            PRINT_ERROR << "Error while loading status effect Remove function" << std::endl;
            vt_script::ScriptManager->HandleLuaError(e);
        } catch(const luabind::cast_failed& e) {
            PRINT_ERROR << "Error while loading status effect Remove function" << std::endl;
            vt_script::ScriptManager->HandleCastError(e);
        }
    }
    else {
        PRINT_WARNING << "No status effect Remove function defined." << std::endl;
    }

    status_effect.Disable();

}

void MapStatusEffectsSupervisor::_SetActiveStatusEffects(GlobalCharacter* character)
{
    if (!character)
        return;

    character->ResetActiveStatusEffects();
    for(std::vector<ActiveMapStatusEffect>::iterator it = _active_status_effects.begin();
            it != _active_status_effects.end(); ++it) {
        ActiveMapStatusEffect& effect = (*it);
        if (!effect.IsActive())
            continue;

        // Copy the active status effect state
        vt_system::SystemTimer* timer = effect.GetTimer();
        character->SetActiveStatusEffect(effect.GetType(), effect.GetIntensity(),
                                         timer->GetDuration(), timer->GetTimeExpired());
    }
}

void MapStatusEffectsSupervisor::_AddPassiveStatusEffect(vt_global::GlobalCharacter* character,
                                                         vt_global::GLOBAL_STATUS status_effect,
                                                         vt_global::GLOBAL_INTENSITY intensity)
{
    PassiveMapStatusEffect effect(character, status_effect, intensity);
    _equipment_status_effects.push_back(effect);
}

void MapStatusEffectsSupervisor::_MakeCharacterPortraitAppear(vt_global::GlobalCharacter* character, uint32_t time)
{
    for (uint32_t i = 0; i < _characters_portraits.size(); ++i) {
        if (_characters_portraits[i].GetCharacter() == character) {
            _characters_portraits[i].FadeIn(time);
            return;
        }
    }
}

float MapStatusEffectsSupervisor::_GetEffectAnimationXPosition(vt_global::GlobalCharacter* character)
{
    float x_pos = 90.0; // default position.
    for (uint32_t i = 0; i < _characters_portraits.size(); ++i) {
        if (_characters_portraits[i].GetCharacter() == character) {
            x_pos = _GetEffectXPositionFromCharacterIndex(i) + 5.0f; // We add an offset to avoid cluttering
            break;
        }
    }
    return x_pos;
}

} // namespace private_map

} // namespace vt_map
