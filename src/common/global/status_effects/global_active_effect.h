////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_ACTIVE_EFFECT_HEADER__
#define __GLOBAL_ACTIVE_EFFECT_HEADER__

#include "status_effect_enums.h"

#include <iostream>

namespace vt_global
{

/** \name Active Status effect data
*** \brief Stores the data to load/save a currently active status effect, due to wounds, traps, ....
***
*** It is used to store the current status effects applied on global characters.
*** and update/display their effects in the menu and map modes.
**/
class ActiveStatusEffect {
public:
    // Default constructor - Invalid status effect
    ActiveStatusEffect():
        _status_effect(GLOBAL_STATUS_INVALID),
        _intensity(GLOBAL_INTENSITY_NEUTRAL),
        _effect_time(0),
        _elapsed_time(0)
    {}

    ActiveStatusEffect(GLOBAL_STATUS status_effect, GLOBAL_INTENSITY intensity):
        _status_effect(status_effect),
        _intensity(intensity),
        _effect_time(30000), // default time
        _elapsed_time(0)
    {}

    ActiveStatusEffect(GLOBAL_STATUS status_effect, GLOBAL_INTENSITY intensity, uint32_t effect_time):
        _status_effect(status_effect),
        _intensity(intensity),
        _effect_time(effect_time),
        _elapsed_time(0)
    {}

    ActiveStatusEffect(GLOBAL_STATUS status_effect, GLOBAL_INTENSITY intensity,
                       uint32_t effect_time, uint32_t elapsed_time):
        _status_effect(status_effect),
        _intensity(intensity),
        _effect_time(effect_time),
        _elapsed_time(elapsed_time)
    {}

    void SetEffect(GLOBAL_STATUS status_effect) {
        _status_effect = status_effect;
    }

    void SetIntensity(GLOBAL_INTENSITY intensity) {
        _intensity = intensity;
    }

    void SetEffectTime(uint32_t effect_time) {
        _effect_time = effect_time;
    }

    void SetElapsedTime(uint32_t elapsed_time) {
        _elapsed_time = elapsed_time;
    }

    GLOBAL_STATUS GetEffect() const {
        return _status_effect;
    }

    GLOBAL_INTENSITY GetIntensity() const {
        return _intensity;
    }

    uint32_t GetEffectTime() const {
        return _effect_time;
    }

    uint32_t GetElapsedTime() const {
        return _elapsed_time;
    }

    //! \brief Sets the effect as invalid
    void Disable() {
        _status_effect = GLOBAL_STATUS_INVALID;
        _intensity = GLOBAL_INTENSITY_NEUTRAL;
    }

    //! \brief Checks whether the effect is active (and valid).
    bool IsActive() const {
        return !(_status_effect == GLOBAL_STATUS_INVALID || _status_effect == GLOBAL_STATUS_TOTAL
            || _intensity == GLOBAL_INTENSITY_NEUTRAL || _intensity == GLOBAL_INTENSITY_INVALID
            || _intensity == GLOBAL_INTENSITY_TOTAL);
    }

private:
    //! The active status effect type.
    GLOBAL_STATUS _status_effect;

    //! The active status effect current intensity.
    GLOBAL_INTENSITY _intensity;

    //! The active status effect current intensity total time of appliance
    //! in milliseconds.
    //! Once the time has passed, the intensity goes one step toward the neutral
    //! intensity and its total time of appliance is halved.
    uint32_t _effect_time;

    //! The active status effect current intensity time of appliance
    //! already elapsed in milliseconds.
    uint32_t _elapsed_time;
};

} // namespace vt_global

#endif // __GLOBAL_ACTIVE_EFFECT_HEADER__
