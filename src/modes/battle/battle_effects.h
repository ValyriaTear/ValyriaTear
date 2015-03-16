////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_effects.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for battle actor effects.
***
*** This file contains the code that manages effects that influence an actor's
*** behavior and properties.
*** ***************************************************************************/

#ifndef __BATTLE_EFFECTS_HEADER__
#define __BATTLE_EFFECTS_HEADER__

#include "engine/script/script.h"

#include "common/global/global_effects.h"

#include "modes/battle/battle_actors.h"

namespace vt_battle
{

namespace private_battle
{

/** ****************************************************************************
*** \brief Manages all data related to a single passive status effect in battle
***
*** This class extends the GlobalStatusEffect class, which contains nothing
*** more than two enum members representing the status type and intensity.
***
*** This class represents a passive (from equipment) effect on a single actor.
*** ***************************************************************************/
class PassiveBattleStatusEffect : public vt_global::GlobalStatusEffect
{
public:
    /** \param type The status effect type that this class object should represent
    *** \param intensity The intensity of the status effect
    **/
    PassiveBattleStatusEffect(vt_global::GLOBAL_STATUS type, vt_global::GLOBAL_INTENSITY intensity);

    ~PassiveBattleStatusEffect()
    {}

    //! \brief Class Member Access Functions
    //@{
    const std::string& GetName() const {
        return _name;
    }

    //! \brief Returns the update script function of this passive effect.
    const ScriptObject& GetUpdatePassiveFunction() const {
        return _update_passive_function;
    }
    //@}

private:
    //! \brief Holds the translated name of the status effect
    std::string _name;

    //! The UpdatePassive() scripted function of this effect when used as passive one (from equipment)
    ScriptObject _update_passive_function;
}; // class PassiveBattleStatusEffect : public vt_global::GlobalStatusEffect

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
                             uint32 duration = 0);

    ~ActiveBattleStatusEffect()
    {}

    /** \brief Increments the status effect intensity by a positive amount
    *** \param amount The number of intensity levels to increase the status effect by
    *** \return True if the intensity level was modified
    **/
    bool IncrementIntensity(uint8 amount);

    /** \brief Decrements the status effect intensity by a negative amount
    *** \param amount The number of intensity levels to decrement the status effect by
    *** \return True if the intensity level was modified
    *** \note Intensity will not be decremented below GLOBAL_INTENSITY_NEUTRAL
    **/
    bool DecrementIntensity(uint8 amount);

    //! \brief Class Member Access Functions
    //@{
    //! \note This will cause the timer to reset and also
    void SetIntensity(vt_global::GLOBAL_INTENSITY intensity);

    const std::string &GetName() const {
        return _name;
    }

    const ScriptObject& GetApplyFunction() const {
        return _apply_function;
    }

    const ScriptObject& GetUpdateFunction() const {
        return _update_function;
    }

    const ScriptObject& GetRemoveFunction() const {
        return _remove_function;
    }

    //! \note Returns a pointer instead of a reference so that Lua functions can access the timer
    vt_system::SystemTimer *GetTimer() {
        return &_timer;
    }

    vt_video::StillImage *GetIconImage() const {
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
    std::string _name;

    //! \brief A pointer to the script function that applies the initial effect
    ScriptObject _apply_function;

    //! \brief A pointer to the script function that updates any necessary changes caused by the effect
    ScriptObject _update_function;

    //! \brief A pointer to the script function that removes the effect and restores the actor to their original state
    ScriptObject _remove_function;

    //! \brief A timer used to determine how long the status effect lasts
    vt_system::SystemTimer _timer;

    //! \brief A pointer to the icon image that represents the status. Will be NULL if the status is invalid
    vt_video::StillImage* _icon_image;

    //! \brief A flag set to true when the intensity value was changed and cleared when the Update method is called
    bool _intensity_changed;

    /** \brief Performs necessary operations in response to a change in intensity
    *** \param reset_timer_only If true, this indicates that the intensity level remains unchanged and only the timer needs to be reset
    ***
    *** This method should be called after every change in intensity is made.
    **/
    void _ProcessIntensityChange(bool reset_timer_only);
}; // class BattleStatusEffect : public vt_global::GlobalStatusEffect

/** ****************************************************************************
*** \brief Manages all elemental and status elements for an actor
***
*** The class contains all of the active effects on an actor. These effects are
*** updated regularly by this class and are removed when their timers expire or their
*** intensity status is nullified by an external call. This class performs all the
*** calls to the Lua script functions (Apply/Update/Remove) for each status effect at
*** the appropriate time. The class also contains a draw function which will display
*** icons for all the active status effects of an actor to the screen.
*** ***************************************************************************/
class BattleStatusEffectsSupervisor
{
public:
    //! \param actor A valid pointer to the actor object that this class is responsible for
    BattleStatusEffectsSupervisor(BattleActor* actor);

    ~BattleStatusEffectsSupervisor()
    {}

    //! \brief Updates the timers and state of any effects
    void Update();

    //! \brief Draws the element and status effect icons to the bottom status menu
    void Draw();
    //! \brief Draws the same active effects but vertically
    void DrawVertical();

    /** \brief Returns true if the requested status is active on the managed actor
    *** \param status The type of status effect to check for
    **/
    bool IsStatusActive(vt_global::GLOBAL_STATUS status) {
        return _active_status_effects[status].IsActive();
    }

    /** \brief Returns the intensity level of the current status effect, or neutral.
    *** \param status The type of status effect to check for
    **/
    vt_global::GLOBAL_INTENSITY GetActiveStatusIntensity(vt_global::GLOBAL_STATUS status) {
        return _active_status_effects[status].GetIntensity();
    }

    /** \brief Immediately removes all active status effects from the actor
    *** \note This function is typically used in the case of an actor's death. Because it returns no value, indicator icons
    *** illustrating the removal of status effects can not be shown, as the indicators need to know which status effects were
    *** active and at what intensity before they were removed. If you wish to remove all status while displaying indicators,
    *** use a combination of GetActiveStatusEffects() and repeated calls to ChangeStatus() for each effect.
    **/
    void RemoveAllActiveStatusEffects();

    /** \brief Changes the intensity level of an active status effect
    *** \param status The status effect type to change
    *** \param intensity The amount of intensity to increase or decrease the status effect by
    *** \param duration A potential custom effect duration (in milliseconds)
    *** \param elapsed_time The time already elapsed of the effect (in milliseconds).
    *** It must be <= to the duration or it will be ignored. This parameter is useful to resume active status effects
    *** activated in other game modes.
    *** \return True if a change in status took place
    ***
    *** Primary function for performing status changes on an actor. Depending upon the current state of the actor and
    *** the first two status and intensity arguments, this function may add new status effects, remove existing effects,
    *** or modify the intensity of existing effects. This function also takes into account status effects which have an
    *** opposite type (e.g., strength gain status versus strength depletion status) and change the state of both effects
    *** accordingly. So, for example, a single call to this function could remove an old effect -and- add a new effect, if
    *** the effect to be added has an opposite effect that is currently active.
    **/
    bool ChangeActiveStatusEffect(vt_global::GLOBAL_STATUS status, vt_global::GLOBAL_INTENSITY intensity,
                                  uint32 duration = 0, uint32 elapsed_time = 0);

    //! \brief Adds a passive (neverending) status effect and only updates it,
    //! calling the respective UpdatePassive() script function.
    void AddPassiveStatusEffect(vt_global::GLOBAL_STATUS status_effect, vt_global::GLOBAL_INTENSITY intensity);

    //! \brief Copy the Active status effects back to the given global Character
    //! thus they can remain after the effect supervisor deletion for other game modes.
    void SetActiveStatusEffects(vt_global::GlobalCharacter* character);

    /** \brief Removes an existing status effect from the actor
    *** \param status_effect_type The status effect to be removed
    *** \param remove_anyway Call the Remove() script function even if the effect is already disabled.
    **/
    void RemoveActiveStatusEffect(vt_global::GLOBAL_STATUS status_effect_type, bool remove_anyway = false);

private:
    //! \brief A pointer to the actor that this class supervises effects for
    BattleActor* _actor;

    //! \brief Contains all possible status effects.
    //! The vector is initialized with the size of all possible status effects slots.
    std::vector<ActiveBattleStatusEffect> _active_status_effects;

    //! \brief Passive (from equipment) status effects.
    //! Those status effects can never be cancelled. They are simply updated.
    std::vector<PassiveBattleStatusEffect> _equipment_status_effects;

    /** \brief Creates a new status effect and applies it to the actor
    *** \param status The type of the status to create
    *** \param intensity The intensity level that the effect should be initialized at
    *** \param duration The potential custom effect duration in milliseconds.
    *** \param elapsed_time The time already elapsed of the effect (in milliseconds).
    *** It must be <= to the duration or it will be ignored. This parameter is useful to resume active status effects
    *** activated in other game modes.
    ***
    *** \note This method does not check if the requested status effect already exists or not in the map of active effects.
    *** Do not call this method unless you are certain that the given status is not already active on the actor, otherwise
    *** memory leaks and other problems may arise.
    **/
    void _CreateNewStatus(vt_global::GLOBAL_STATUS status, vt_global::GLOBAL_INTENSITY intensity,
                          uint32 duration = 0, uint32 elapsed_time = 0);

    //! \brief Updates the passive (equipment) status effects
    //! \note This method is called from within Update()
    void _UpdatePassive();
}; // class BattleStatusEffectsSupervisor

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_EFFECTS_HEADER__
