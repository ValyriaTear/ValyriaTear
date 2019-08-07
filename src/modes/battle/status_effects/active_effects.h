////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_ACTIVE_EFFECTS_HEADER__
#define __BATTLE_ACTIVE_EFFECTS_HEADER__

#include "common/global/status_effects/global_effects.h"
#include "engine/video/text.h"

#include "luabind/object.hpp"

namespace vt_battle
{

namespace private_battle
{

/** ****************************************************************************
*** \brief Manages all data related to a single status effect in battle
***
*** This class extends the GlobalStatusEffect class, which contains nothing
*** more than two enum members representing the status type and intensity. This
*** class provides a complete implementation of a status effect, including an
*** image icon, a timer, and script functions to implement the effect.
***
*** This class represents an active effect on a single actor. Objects of this
*** class are not shared on multiple actors in any form. Status effects
*** intensity values will naturally decrease in intensity over
*** time until they reach the neutral intensity level.
*** ***************************************************************************/
class ActiveBattleStatusEffect : public vt_global::GlobalStatusEffect
{
public:
    //! \brief Empty constructor
    ActiveBattleStatusEffect();

    /** \param type The status type that this class object should represent
    *** \param intensity The intensity of the status
    *** \param duration The effect duration, a default value is used when none is given.
    **/
    ActiveBattleStatusEffect(vt_global::GLOBAL_STATUS type,
                             vt_global::GLOBAL_INTENSITY intensity,
                             uint32_t duration = 0);

    ~ActiveBattleStatusEffect()
    {}

    /** \brief Increments the status effect intensity by a positive amount
    *** \param amount The number of intensity levels to increase the status effect by
    *** \return True if the intensity level was modified
    **/
    bool IncrementIntensity(uint8_t amount);

    /** \brief Decrements the status effect intensity by a negative amount
    *** \param amount The number of intensity levels to decrement the status effect by
    *** \return True if the intensity level was modified
    *** \note Intensity will not be decremented below GLOBAL_INTENSITY_NEUTRAL
    **/
    bool DecrementIntensity(uint8_t amount);

    //! \brief Class Member Access Functions
    //@{
    //! \note This will cause the timer to reset and also
    void SetIntensity(vt_global::GLOBAL_INTENSITY intensity);

    //! \brief Update the time left text
    void UpdateTimeLeftText();

    const vt_video::TextImage& GetName() const {
        return _name;
    }

    const vt_video::TextImage& GetTimeLeftText() const {
        return _time_left_text;
    }

    const luabind::object& GetApplyFunction() const {
        return _apply_function;
    }

    const luabind::object& GetUpdateFunction() const {
        return _update_function;
    }

    const luabind::object& GetRemoveFunction() const {
        return _remove_function;
    }

    //! \note Returns a pointer instead of a reference so that Lua functions can access the timer
    vt_system::SystemTimer* GetTimer() {
        return &_timer;
    }

    vt_video::StillImage* GetIconImage() const {
        return _icon_image;
    }

    bool HasIntensityChanged() const {
        return _intensity_changed;
    }

    void ResetIntensityChanged() {
        _intensity_changed = false;
    }
    //@}

private:
    //! \brief Holds the translated name of the status effect
    vt_video::TextImage _name;

    //! \brief A pointer to the script function that applies the initial effect
    luabind::object _apply_function;

    //! \brief A pointer to the script function that updates any necessary changes caused by the effect
    luabind::object _update_function;

    //! \brief A pointer to the script function that removes the effect and restores the actor to their original state
    luabind::object _remove_function;

    //! \brief A timer used to determine how long the status effect lasts
    vt_system::SystemTimer _timer;

    //! \brief A pointer to the icon image that represents the status. Will be nullptr if the status is invalid
    vt_video::StillImage* _icon_image;

    //! \brief Holds the time left text of the effect
    vt_video::TextImage _time_left_text;

    //! \brief A flag set to true when the intensity value was changed and cleared when the Update method is called
    bool _intensity_changed;

    /** \brief Performs necessary operations in response to a change in intensity
    *** \param reset_timer_only If true, this indicates that the intensity level remains unchanged and only the timer needs to be reset
    ***
    *** This method should be called after every change in intensity is made.
    **/
    void _ProcessIntensityChange(bool reset_timer_only);
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_ACTIVE_EFFECTS_HEADER__
