////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_EFFECTS_SUPERVISOR_HEADER__
#define __BATTLE_EFFECTS_SUPERVISOR_HEADER__

#include "active_effects.h"
#include "passive_effects.h"

namespace vt_global {
class GlobalCharacter;
}

namespace vt_battle
{

namespace private_battle
{

class BattleActor;

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
    *** opposite type (e.g., phys_atk gain status versus phys_atk depletion status) and change the state of both effects
    *** accordingly. So, for example, a single call to this function could remove an old effect -and- add a new effect, if
    *** the effect to be added has an opposite effect that is currently active.
    **/
    bool ChangeActiveStatusEffect(vt_global::GLOBAL_STATUS status, vt_global::GLOBAL_INTENSITY intensity,
                                  uint32_t duration = 0, uint32_t elapsed_time = 0);

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

    //! \brief Infinite TextImage
    vt_video::TextImage _infinite_text;

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
                          uint32_t duration = 0, uint32_t elapsed_time = 0);

    //! \brief Updates the passive (equipment) status effects
    //! \note This method is called from within Update()
    void _UpdatePassive();
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_EFFECTS_SUPERVISOR_HEADER__
