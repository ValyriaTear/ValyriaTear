////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2009 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_effects.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for battle actor effects.
***
*** This file contains the code that manages effects that influence an actor's
*** behavior and properties.
*** ***************************************************************************/

#ifndef __BATTLE_EFFECTS_HEADER__
#define __BATTLE_EFFECTS_HEADER__

#include "defs.h"
#include "utils.h"

#include "script.h"
#include "system.h"

#include "global_effects.h"

namespace hoa_battle {

namespace private_battle {

/** ****************************************************************************
*** \brief Manages all data related to a single status effect in battle
***
*** This class extends the GlobalStatusEffect class, which contains nothing
*** more than two enum members representing the status type and intensity. This
*** class provides a complete implementation of a status effect, including an
*** image icon, a timer, and script functions to implement the effect.
***
*** This class represents an active effect on a single actor. Objects of this
*** class are not shared on multiple actors in any form. Status effects only
*** have positive intensity values and will naturally decrease in intensity over
*** time until they reach the neutral intensity level. Some types of status
*** effects have an opposite type. For example, one status effect may boost the
*** actor's strength while another drains strength. We do not allow these two
*** statuses to co-exist on the same actor, thus the two have a cancelation effect
*** on each other and the stronger (more intense) effect will remain.
***
*** \todo Implement opposite types for status effects and possibly add a boolean
*** member to indicate whether the status is aiding or ailing.
*** ***************************************************************************/
class BattleStatusEffect : public hoa_global::GlobalStatusEffect {
public:
	/** \param type The status type that this class object should represent
	*** \param intensity The intensity of the status
	*** \param actor A pointer to the actor affected by the status
	**/
	BattleStatusEffect(hoa_global::GLOBAL_STATUS type, hoa_global::GLOBAL_INTENSITY intensity, BattleActor* actor);

	~BattleStatusEffect();

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
	void SetIntensity(hoa_global::GLOBAL_INTENSITY intensity);

	const std::string& GetName() const
		{ return _name; }

	const uint32 GetIconIndex() const
		{ return _icon_index; }

	hoa_global::GLOBAL_STATUS GetOppositeEffect() const
		{ return _opposite_effect; }

	BattleActor* GetAffectedActor() const
		{ return _affected_actor; }

	ScriptObject* GetApplyFunction() const
		{ return _apply_function; }

	ScriptObject* GetUpdateFunction() const
		{ return _update_function; }

	ScriptObject* GetRemoveFunction() const
		{ return _remove_function; }

	//! \note Returns a pointer instead of a reference so that Lua functions can access the timer
	hoa_system::SystemTimer* GetTimer()
		{ return &_timer; }

	hoa_video::StillImage* GetIconImage() const
		{ return _icon_image; }

	bool IsIntensityChanged() const
		{ return _intensity_changed; }

	void ResetIntensityChanged()
		{ _intensity_changed = false; }
	//@}

private:
	//! \brief Holds the translated name of the status effect
	std::string _name;

	//! \brief Holds the index to the row where the icons for this effect are stored in the status effect multi image
	uint32 _icon_index;

	//! \brief The opposte effect for the status, set to GLOBAL_STATUS_INVALID if no opposite effect exists
	hoa_global::GLOBAL_STATUS _opposite_effect;

	//! \brief A pointer to the script function that applies the initial effect
	ScriptObject* _apply_function;

	//! \brief A pointer to the script function that updates any necessary changes caused by the effect
	ScriptObject* _update_function;

	//! \brief A pointer to the script function that removes the effect and restores the actor to their original state
	ScriptObject* _remove_function;

	//! \brief A pointer to the actor that is affected by this status
	BattleActor* _affected_actor;

	//! \brief A timer used to determine how long the status effect lasts
	hoa_system::SystemTimer _timer;

	//! \brief A pointer to the icon image that represents the status. Will be NULL if the status is invalid
	hoa_video::StillImage* _icon_image;

	//! \brief A flag set to true when the intensity value was changed and cleared when the Update method is called
	bool _intensity_changed;

	/** \brief Performs necessary operations in response to a change in intensity
	*** \param reset_timer_only If true, this indicates that the intensity level remains unchanged and only the timer needs to be reset
	*** 
	*** This method should be called after every change in intensity is made.
	**/
	void _ProcessIntensityChange(bool reset_timer_only);
}; // class BattleStatusEffect : public hoa_global::GlobalStatusEffect


/** ****************************************************************************
*** \brief Manages all elemental and status elements for an actor
***
*** The class contains all of the active effects on an actor. These effects are
*** updated regularly by this class and are removed when their timers expire or their
*** intensity status is nullified by an external call. This class performs all the
*** calls to the Lua script functions (Apply/Update/Remove) for each status effect at
*** the appropriate time. The class also contains a draw function which will display 
*** icons for all the active status effects of an actor to the screen.
***
*** \todo The Draw function probably should be renamed to something more specific
*** and should check whether or not the actor is a character. Its intended to be
*** used only for character actors to draw on the bottom menu. There should also
*** probably be another draw function for drawing the status of an actor to the
*** command window.
***
*** \todo Elemental effects are not yet implemented or supported by this class.
*** They should be added when elemental effects in battle are ready to be used.
*** ***************************************************************************/
class EffectsSupervisor {
public:
	//! \param actor A valid pointer to the actor object that this class is responsible for
	EffectsSupervisor(BattleActor* actor);

	~EffectsSupervisor();

	//! \brief Updates the timers and state of any effects
	void Update();

	//! \brief Draws the element and status effect icons to the bottom status menu
	void Draw();

	/** \brief Returns true if the requested status is active on the managed actor
	*** \param status The type of status to check for
	**/
	bool IsStatusActive(hoa_global::GLOBAL_STATUS status)
		{ return (_active_status_effects.find(status) != _active_status_effects.end()); }

	/** \brief Reurns true if the opposite status to that of the argument is active
	*** \param status The type of opposite status to check for
	**/
	bool IsOppositeStatusActive(hoa_global::GLOBAL_STATUS status);

	/** \brief Populates the argument vector with all status effects that are active on the actor
	*** \param all_status_effects A reference to the data vector to populate
	**/
	void GetAllStatusEffects(std::vector<hoa_global::GLOBAL_STATUS>& all_status_effects);

	/** \brief Immediately removes all active status effects from the actor
	*** \note This function is typically used in the case of an actor's death. Because it returns no value, indicator icons
	*** illustrating the removal of status effects can not be shown, as the indicators need to know which status effects were
	*** active and at what intensity before they were removed. If you wish to remove all status while displaying indicators,
	*** use a combination of GetActiveStatusEffects() and repeated calls to ChangeStatus() for each effect.
	**/
	void RemoveAllStatus();
	
	/** \brief Changes the intensity level of a status effect
	*** \param status The status effect type to change
	*** \param intensity The amount of intensity to increase or decrease the status effect by
	*** \param old_status A reference to hold the previous status type as a result of this operation
	*** \param old_intensity A reference to hold the previous intensity of the previous status
	*** \param new_status A reference to the new status as a result of the change
	*** \param new_intensity A reference to new intensity fo the new status
	*** \return True if a change in status took place
	***
	*** Primary function for performing status changes on an actor. Depending upon the current state of the actor and
	*** the first two status and intensity arguments, this function may add new status effects, remove existing effects,
	*** or modify the intensity of existing effects. This function also takes into account status effects which have an
	*** opposite type (e.g., strength gain status versus strength depletion status) and change the state of both effects
	*** accordingly. So, for example, a single call to this function could remove an old effect -and- add a new effect, if
	*** the effect to be added has an opposite effect that is currently active.
	***
	*** \note The old/new status/intensity arguments are used to store additional return values, so don't pass references to
	*** variables that have data you wish to retain. If the function returns false, the value of these members is meaningless
	*** and should be disregarded.
	***
	*** \note To be absolutely certain that a particular status effect is removed from the actor regardless of its current
	*** intensity, use the value GLOBAL_INTENSITY_NEG_EXTREME for the intensity argument.
	***
	*** \note This function only changes the state of the status and does <i>not</i> display any visual or other indicator
	*** to the player that the status was modified. Typically you should invoke BattleActor::RegisterStatusChange(...)
	*** when you want to change the status of the actor. That method will call this one as well as activating the proper
	*** indicator based on the return values from this function
	**/
	bool ChangeStatus(hoa_global::GLOBAL_STATUS status, hoa_global::GLOBAL_INTENSITY intensity,
		hoa_global::GLOBAL_STATUS& previous_status, hoa_global::GLOBAL_INTENSITY& previous_intensity,
		hoa_global::GLOBAL_STATUS& new_status, hoa_global::GLOBAL_INTENSITY& new_intensity
	);

private:
	//! \brief A pointer to the actor that this class supervises effects for
	BattleActor* _actor;

	// TODO: support for elemental effects may be added here at a later time
//	//! \brief Contains all active element effects
// 	std::map<hoa_global::GLOBAL_ELEMENTAL, BattleElementEffect*> _element_effects;

	//! \brief Contains all active status effects
	std::map<hoa_global::GLOBAL_STATUS, BattleStatusEffect*> _active_status_effects;

	/** \brief Creates a new status effect and applies it to the actor
	*** \param status The type of the status to create
	*** \param intensity The intensity level that the effect should be initialized at
	***
	*** \note This method does not check if the requested status effect already exists or not in the map of active effects.
	*** Do not call this method unless you are certain that the given status is not already active on the actor, otherwise
	*** memory leaks and other problems may arise.
	**/
	void _CreateNewStatus(hoa_global::GLOBAL_STATUS status, hoa_global::GLOBAL_INTENSITY intensity);

	/** \brief Removes an existing status effect from the actor
	*** \param status_effect A pointer to the status effect to be removed
	*** \note After this function completes, if it was successful, the object pointed to by the status_effect argument will
	*** be invalid and should not be used. It is good practice for the caller to set the pointer passed in to this function to
	*** NULL immediately after the function call returns.
	**/
	void _RemoveStatus(BattleStatusEffect* status_effect);
}; // class EffectsSupervisor

} // namespace private_battle

} // namespace hoa_battle

#endif // __BATTLE_EFFECTS_HEADER__
