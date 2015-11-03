////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2014-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_status_effects.h
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for map active status effects handling.
***
*** This file contains the code that manages active status effects
*** that change a global actor status while in the map mode.
*** ***************************************************************************/

#ifndef __MAP_STATUS_EFFECTS_HEADER__
#define __MAP_STATUS_EFFECTS_HEADER__

#include "engine/script/script.h"

#include "common/global/global_effects.h"

namespace vt_global {
class GlobalCharacter;
}

namespace vt_map
{

namespace private_map
{

/** ****************************************************************************
*** \brief Manages all data related to a single status effect in map mode
***
*** \note This class extends the GlobalStatusEffect class.
***
*** This class represents an active status effect on a global character.
*** It is used to know when and how to apply and display status change when
*** the parent map mode is active
*** \note Active status effects are effects coming from potions or enemies attacks.
*** Active status effects fades over time. They are opposed to passive status effects,
*** coming from equipment, which intensities never fade.
*** ***************************************************************************/
class ActiveMapStatusEffect : public vt_global::GlobalStatusEffect
{
public:
    /** \param type The status type that this class object should represent
    *** \param character A pointer to the character affected by the status effect
    *** \param intensity The intensity of the status
    *** \param duration The effect duration, a default value is used when none is given.
    **/
    ActiveMapStatusEffect(vt_global::GlobalCharacter* character,
                          vt_global::GLOBAL_STATUS type,
                          vt_global::GLOBAL_INTENSITY intensity,
                          uint32_t duration = 0);

    ~ActiveMapStatusEffect()
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
    //! \note This will also cause the timer to reset
    void SetIntensity(vt_global::GLOBAL_INTENSITY intensity);

    const std::string& GetName() const {
        return _name;
    }

    vt_global::GlobalCharacter* GetAffectedCharacter() const {
        return _affected_character;
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
    std::string _name;

    //! \brief A pointer to the script function that applies the initial effect
    ScriptObject _apply_function;

    //! \brief A pointer to the script function that updates any necessary changes caused by the effect
    ScriptObject _update_function;

    //! \brief A pointer to the script function that removes the effect and restores the actor to their original state
    ScriptObject _remove_function;

    //! \brief A pointer to the character that is affected by this status effect.
    vt_global::GlobalCharacter* _affected_character;

    //! \brief A timer used to determine how long the status effect lasts
    vt_system::SystemTimer _timer;

    //! \brief A pointer to the icon image that represents the status. Will be nullptr if the status is invalid
    vt_video::StillImage* _icon_image;

    //! \brief A flag set to true when the intensity value was changed and cleared when the Update method is called
    bool _intensity_changed;

    /** \brief Performs necessary operations in response to a change in intensity
    *** \param reset_timer_only If true, this indicates that the intensity level remains unchanged and only the timer needs to be reset
    ***
    *** This method should be called after every change in intensity is made.
    **/
    void _ProcessIntensityChange(bool reset_timer_only);
}; // class ActiveMapStatusEffect : public vt_global::GlobalStatusEffect


/** ****************************************************************************
*** \brief Manages all data related to a single passive status effect in the map mode
***
*** \note This class extends the GlobalStatusEffect class.
***
*** This class represents a passive (from equipment) effect on a single global character.
*** ***************************************************************************/
class PassiveMapStatusEffect : public vt_global::GlobalStatusEffect
{
public:
    /** \param type The status type that this class object should represent
    *** \param intensity The intensity of the status
    *** \param character A pointer to the character affected by the status effect
    **/
    PassiveMapStatusEffect(vt_global::GlobalCharacter* character,
                           vt_global::GLOBAL_STATUS type,
                           vt_global::GLOBAL_INTENSITY intensity);

    ~PassiveMapStatusEffect()
    {}

    //! \brief Class Member Access Functions
    //@{
    const std::string& GetName() const {
        return _name;
    }

    vt_global::GlobalCharacter* GetAffectedCharacter() const {
        return _affected_character;
    }

    //! \brief Returns the update script function of this passive effect.
    const ScriptObject& GetUpdatePassiveFunction() const {
        return _update_passive_function;
    }

    vt_video::StillImage* GetIconImage() const {
        return _icon_image;
    }
    //@}

private:
    //! \brief Holds the translated name of the status effect
    std::string _name;

    //! The UpdatePassive() scripted function of this effect when used as passive one (from equipment)
    ScriptObject _update_passive_function;

    //! \brief A pointer to the character that is affected by this status effect.
    vt_global::GlobalCharacter* _affected_character;

    //! \brief A pointer to the icon image that represents the status. Will be nullptr if the status is invalid
    vt_video::StillImage* _icon_image;
}; // class PassiveMapStatusEffect : public vt_global::GlobalStatusEffect

/** ****************************************************************************
*** \brief A class used to display the character portrait whenever an active
*** status effect is displaying changes. The portrait is displayed next to the
*** status effect visuals so the player can visually link the two.
*** ***************************************************************************/
class CharacterIndication
{
public:
    CharacterIndication(vt_global::GlobalCharacter* character, float x_position, float y_position);

    ~CharacterIndication()
    {}

    //! Make the portrait fade in/out
    void FadeIn(uint32_t display_time) {
        _fade_in = true;
        _fade_out = false;
        // We overwrite the time to display the effect
        _display_time = display_time;
    }

    void FadeOut() {
        _fade_in = false;
        _fade_out = true;
    }

    void Update();

    void Draw();

    vt_global::GlobalCharacter* GetCharacter()
    { return _global_character; }
private:
    //! \brief The portrait position
    float _x_position;
    float _y_position;

    //! \brief The image alpha
    float _image_alpha;

    //! \brief Used to make the portrait fade in/out.
    bool _fade_in;
    bool _fade_out;

    //! \brief The time the portrait should be displayed.
    int32_t _display_time;

    //! \brief The corresponding global character
    vt_global::GlobalCharacter* _global_character;

    //! \brief The character portrait (own instance with custom dimensions)
    vt_video::StillImage _portrait;
};

/** ****************************************************************************
*** \brief Manages all elemental and status elements for the global party
*** when in map mode.
***
*** The class contains all of the active effects on an actor. These effects are
*** updated regularly by this class and are removed when their timers expire or their
*** intensity status is nullified by an external call. This class performs all the
*** calls to the Lua script functions (Apply/Update/Remove) for each status effect at
*** the appropriate time. The class also contains a draw function which will display
*** icons for all the active status effects of the characters to the screen.
*** ***************************************************************************/
class MapStatusEffectsSupervisor
{
public:
    MapStatusEffectsSupervisor();

    ~MapStatusEffectsSupervisor();

    //! \brief Loads the current state of active and passive status effects from global characters
    //! Use at load time and when the map mode calls Reset(), to handle updates from a potential menu mode.
    void LoadStatusEffects();

    //! \brief Sets back the current state of active and passive status effects on global characters
    void SaveActiveStatusEffects();

    //! \brief Updates the timers and state of every effects for every living characters.
    void UpdateEffects();

    //! \brief Updates the portraits animations.
    void UpdatePortraits();

    //! \brief Draws the element and status effect icons to the bottom of the screen
    //! and on which characters they apply to.
    void Draw();

    /** \brief Changes the intensity level of a given status effect on a given character
    *** \param active_effect The current active status effect to modify
    *** \param intensity The amount of intensity to increase or decrease the status effect by
    *** \param duration A potential custom effect duration (in milliseconds)
    *** \param elapsed_time The time already elapsed of the effect (in milliseconds).
    *** \param display_change Whether the change must be notified to the player.
    *** It must be <= to the duration or it will be ignored. This parameter is useful to resume active status effects
    *** activated in other game modes.
    *** \return True if a change in status took place
    **/
    bool ChangeActiveStatusEffect(ActiveMapStatusEffect& active_effect,
                                  vt_global::GLOBAL_INTENSITY intensity,
                                  uint32_t duration = 0, uint32_t elapsed_time = 0, bool display_change = true);

    //! \brief The same function but searching and/or creating the effect when necessary
    //! before applying it.
    bool ChangeActiveStatusEffect(vt_global::GlobalCharacter* character,
                                  vt_global::GLOBAL_STATUS status_type,
                                  vt_global::GLOBAL_INTENSITY intensity,
                                  uint32_t duration = 0, uint32_t elapsed_time = 0, bool display_change = true);

    //! \brief Tells the current active status effect intensity applied on the given character.
    //! \note As the map mode is not syncing the status effects with the global characters
    //! the map mode is activated for performance reasons, we need to get info from here,
    //! and not form the global characters directly when the map mode is active.
    vt_global::GLOBAL_INTENSITY GetActiveStatusEffectIntensity(vt_global::GlobalCharacter* character,
                                                               vt_global::GLOBAL_STATUS status_type) const;

private:
    //! \brief Contains all possible status effects.
    //! The vector is initialized with the size of all possible status effects slots.
    //! Inactive status effect are set to GLOBAL_STATUS_INVALID and removed in the UpdateEffects() loop.
    std::vector<ActiveMapStatusEffect> _active_status_effects;

    //! \brief Passive (from equipment) status effects.
    //! Those status effects can never be canceled. They are simply updated.
    std::vector<PassiveMapStatusEffect> _equipment_status_effects;

    //! \brief The character portraits display on the bottom of the screen used
    //! to visually link on what character a status effect change happens.
    std::vector<CharacterIndication> _characters_portraits;

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
    void _AddActiveStatusEffect(vt_global::GlobalCharacter* character,
                                vt_global::GLOBAL_STATUS status, vt_global::GLOBAL_INTENSITY intensity,
                                uint32_t duration = 0, uint32_t elapsed_time = 0);

    /** \brief Removes an existing status effect from the actor
    *** \param status_effect A reference to the status effect to be removed (disabled)
    *** This doesn't erase the status effect data (as this is done in Update())
    **/
    void _RemoveActiveStatusEffect(ActiveMapStatusEffect& status_effect);

    //! \brief Updates the passive (equipment) status effects
    //! \note This method is called from within Update()
    void _UpdatePassive();

    //! \brief Adds a passive (neverending) status effect and only updates it,
    //! calling the respective UpdatePassive() script function.
    void _AddPassiveStatusEffect(vt_global::GlobalCharacter* character, vt_global::GLOBAL_STATUS status_effect,
                                 vt_global::GLOBAL_INTENSITY intensity);

    //! \brief Copy the Active status effects back to the given global Character
    //! thus they can remain after the status effect supervisor deletion for other game modes.
    void _SetActiveStatusEffects(vt_global::GlobalCharacter* character);

    //! \brief Check whether certain portraits should appear for a brief period of time.
    //! If the function is called several times, the portrait will simply keep on appearing
    //! for the latest desired duration
    void _MakeCharacterPortraitAppear(vt_global::GlobalCharacter* character, uint32_t time);

    //! \brief Gives where the effect should be displayed (on bottom of the screen)
    //! according to the character it is applied on.
    float _GetEffectAnimationXPosition(vt_global::GlobalCharacter* character);

}; // class MapStatusEffectsSupervisor

} // namespace private_map
} // namespace vt_map

#endif // __MAP_STATUS_EFFECTS_HEADER__
