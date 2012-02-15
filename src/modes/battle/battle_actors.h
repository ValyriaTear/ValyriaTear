////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_actors.h
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \author  Corey Hoffstein, visage@allacrost.org
*** \author  Andy Gardner, chopperdave@allacrost.org
*** \brief   Header file for actors present in battles.
***
*** This code contains the implementation of battle actors (characters and
*** enemies) whom are represented on the field of battle.
*** ***************************************************************************/

#ifndef __BATTLE_ACTORS_HEADER__
#define __BATTLE_ACTORS_HEADER__

#include "defs.h"
#include "utils.h"

#include "global_skills.h"
#include "global_actors.h"
#include "global_effects.h"

#include "battle_utils.h"

namespace hoa_battle {

namespace private_battle {

/** ****************************************************************************
*** \brief An abstract class for representing an actor in the battle
***
*** An "actor" is a term used to represent both characters and enemies in battle.
*** This abstract class contains members and methods that are common to both types of
*** actors. As such, many of the implemented methods in this class are virtual.
***
*** The BattleActor class contains a pointer to a GlobalActor object that represents
*** the character or enemy. BattleActor contains its own members for all actor stats such
*** as HP, strength, evade rating, etc. There are two reasons why BattleActor uses its own
*** members instead of directly accessing and modifying the members of the GlobalActor pointer.
*** First, various effects can occur in battle which can modify otherwise static stats such as
*** agility. We need the ability to restore each stat to its base value, and the GlobalActor
*** class retains that unaltered value. Second, if the player loses the battle and chooses to
*** retry, we need to restore all actors back to their original state before the battle began.
*** Retreiving the values of the GlobalActor class allows us to do so.
***
*** Throughout the battle, actors progress through a series of states. The
*** standard set of states that an actor cylces through while they are "alive"
*** and participating in the battle are as follows.
***
*** -# ACTOR_STATE_IDLE
*** -# ACTOR_STATE_COMMAND
*** -# ACTOR_STATE_WARM_UP
*** -# ACTOR_STATE_READY
*** -# ACTOR_STATE_ACTING
*** -# ACTOR_STATE_COOL_DOWN
***
*** Throughout each cycle, the actor will select or be given an action to execute.
*** This action may be to attack an actor on the opposing team, heal a teammate,
*** use an item, or perform some sort of skill. Each actor is responsible for the
*** management of the action that they intend to take.
*** ***************************************************************************/
class BattleActor : public hoa_global::GlobalActor {
public:
	BattleActor(hoa_global::GlobalActor* actor);

	virtual ~BattleActor();

	//! \brief Returns true if the actor is considered an enemy of the character party
	virtual bool IsEnemy() const = 0;

	//! \brief Returns true so long as the actor is not in the "dead" state
	bool IsAlive() const
		{ return (_state != ACTOR_STATE_DEAD); }

	//! \brief Empty method. Required because this is a pure virtual method of GlobalActor
	void AddSkill(uint32 skill_id)
		{}

	/** \brief Restores an actor to the initial state it was in when the battle began
	***
	*** This includes resetting the actor's state, attributes, status effect, and animations.
	**/
	virtual void ResetActor();

	/** \brief Changes the state of the actor and modifies the actor's properties accordingly
	*** \param new_state The state to set the actor to
	**/
	virtual void ChangeState(ACTOR_STATE new_state);

	//! \brief Returns the width of the actor's sprite image
	virtual float GetSpriteWidth() const = 0;

	//! \brief Returns the height of the actor's sprite image
	virtual float GetSpriteHeight() const = 0;

	/** \brief Changes the actor's current sprite animation image
	*** \param alias The alias text used to identify the animation to change
	***
	*** \note Not all forms of battle sprites have multiple animations or any animations at all. For
	*** example, enemies typically only have a standard set of unanimated damage frames for their
	*** sprites. The reason this method is defined for all actors is so that the same skills may be
	*** reused for both characters and enemies, since some skill implementations will wish to call
	*** this method on the actor performing the skill.
	***
	*** \warning This function is not a pure virtual one because it needs to be accessed by Lua
	*** and there is an issue with using Luabind on pure virtual functions.
	**/
	virtual void ChangeSpriteAnimation(const std::string& alias)
		{}

	/** \brief Deals damage to the actor by reducing its hit points by a certain amount
	*** \param amount The number of hit points to decrease on the actor
	***
	*** If the state of the actor is ACTOR_STATE_DEAD, this function will print a warning and change nothing.
	*** If the amount of damage dealt is greater than the actor's current hit points, the actor will be placed
	*** in the ACTOR_STATE_DEAD state.
	**/
	void RegisterDamage(uint32 amount);

	/** \brief Deals damage to the actor by reducing its hit points by a certain amount
	*** \param amount The number of hit points to decrease on the actor
	*** \param target A pointer to the target information that was used to inflict this damage
	***
	*** In addition to dealing damage, this function will use the second target argument to determine if this
	*** damage was inflicted on an attack point (as opposed to the actor as a whole). If so, it analyzes the
	*** attack point on the actor to see if it has any status effect vulnerabilities. If this is indeed the
	*** case, then it applies the status effect to the actor if the random probability calculation determines
	*** that the effect has been triggered.
	**/
	void RegisterDamage(uint32 amount, BattleTarget* target);

	/** \brief Heals the actor by restoring a certain amount of hit points
	*** \param amount The number of hit points to add to the actor
	***
	*** If the state of the actor is ACTOR_STATE_DEAD, this function will print a warning and change nothing.
	*** The number of hit points on the actor are not allowed to increase beyond the actor's maximum hit
	*** points.
	**/
	void RegisterHealing(uint32 amount);

	//! \brief Indicates that an action failed to connect on this target
	void RegisterMiss();

	/** \brief Causes a change in a character's status
	*** \param status The type of status to change
	*** \param intensity The intensity of the change
	***
	*** This is the single method for registering a change in status for an actor. It can be used to add
	*** a new status, remove an existing status, or change the intensity level of an existing status. A
	*** positive intensity argument will increase the intensity while a negative intensity will decrease
	*** the intensity. Many different changes can occur depending upon the current state of the actor and
	*** any active status effects when this function is called, as the list below describes.
	***
	*** - If this status is not already active on the character and the intensity argument is positive,
	***   the actor will have the new status added at that intensity.
	*** - If this status is not already active on the character and the intensity argument is negative,
	***   no change will occur.
	*** - If this status is active and intensity is positive, intensity will be increased but will not
	***   exceed the maximum intensity level.
	*** - If this status is active, the intensity is positive, and the current intensity of the status
	***   is already at the maximum level, the status timer will be reset and the intensity will not change.
	*** - If this status is active and intensity is negative, intensity will be decreased but not allowed
	***   to go lower than the neutral level. If the neutral level is reached, the status will be removed.
	*** - If this status is active and the intensity is GLOBAL_INTENSITY_NEG_EXTREME, the status will be
	***   removed regardless of its current intensity level.
	*** - If this status has an opposite status type that is active on the actor and the intensity argument
	***   is positive, this will decrease the intensity of the opposite status by the degree of intensity.
	***   This may cause that opposite status to be removed and this new status to be added if the value of
	***   the intensity change is high enough.
	***
	*** If the desired effect does yield a change in status, this function will prepare an indicator image
	*** to be displayed representing the change in status.
	**/
	void RegisterStatusChange(hoa_global::GLOBAL_STATUS status, hoa_global::GLOBAL_INTENSITY intensity);

	/** \brief Increases or decreases the current skill points of the actor
	*** \param amount The number of skill points to increase or decrease
	***
	*** If the actor is dead, no change will take place. If the amount is positive, the actor will
	*** not be allowed to exceed above their maximum skill points.
	***
	*** Any non-zero change in skill points will be reflected via increase/decrease text that will
	*** be drawn to the screen near the actor's sprite. If the value of the amount argument is zero,
	*** the word "Miss" will be drawn instead;
	**/
	void ChangeSkillPoints(int32 amount);

	/** \brief Updates the state of the actor
	*** \param animations_only If true, animations will be updated but actor state will not. Default value is false
	***
	*** The optional boolean parameter is primarily used by battle sequences which desire to update the sprite graphics
	*** but not any battle state.
	**/
	virtual void Update(bool animation_only = false);

	//! \brief Draws the actor's current sprite animation frame
	virtual void DrawSprite() = 0;

	//! \brief Draws all active indicator text and graphics for the actor
	void DrawIndicators() const;

	/** \brief Sets the action that the actor should execute next
	*** \param action A pointer to the action that the actor should execute
	***
	*** The actor assumes responsibility for the memory management of the action that is given to it with
	*** this method and will delete the object at the appropriate time. You should only call the method
	*** when the actor is in the state ACTOR_STATE_COMMAND. Invoking it at any other time will result in a
	*** warning and no operation, and the action object will be deleted immediately. A warning is also
	*** printed in the case where the actor has another action prepared.
	**/
	void SetAction(BattleAction* action);

	//! \brief Resets actor stats to their original values
	//@{
	void ResetHitPoints()
		{ SetHitPoints(_global_actor->GetHitPoints()); }

	void ResetMaxHitPoints()
		{ SetMaxHitPoints(_global_actor->GetMaxHitPoints()); }

	void ResetSkillPoints()
		{ SetSkillPoints(_global_actor->GetSkillPoints()); }

	void ResetMaxSkillPoints()
		{ SetMaxSkillPoints(_global_actor->GetMaxSkillPoints()); }

	void ResetStrength()
		{ SetStrength(_global_actor->GetStrength()); }

	void ResetVigor()
		{ SetVigor(_global_actor->GetVigor()); }

	void ResetFortitude()
		{ SetFortitude(_global_actor->GetFortitude()); }

	void ResetProtection()
		{ SetProtection(_global_actor->GetProtection()); }

	void ResetAgility()
		{ SetAgility(_global_actor->GetAgility()); }

	void ResetEvade()
		{ SetEvade(_global_actor->GetEvade()); }
	//@}

	//! \brief Returns the average defense/evasion totals of all of the actor's attack points
	//@{
	uint32 TotalPhysicalDefense();

	uint32 TotalMetaphysicalDefense();

	float TotalEvadeRating();
	//@}

	//! \name Class member access methods
	//@{
	ACTOR_STATE GetState() const
		{ return _state; }

	hoa_global::GlobalActor* GetGlobalActor()
		{ return _global_actor; }

	BattleAction* GetAction()
		{ return _action; }

	bool IsActionSet() const
		{ return (_action != NULL); }

	float GetXOrigin() const
		{ return _x_origin; }

	float GetYOrigin() const
		{ return _y_origin; }

	float GetXLocation() const
		{ return _x_location; }

	float GetYLocation() const
		{ return _y_location; }

	uint32 GetIdleStateTime() const
		{ return _idle_state_time; }

	hoa_video::StillImage& GetStaminaIcon()
		{ return _stamina_icon; }

	BattleTimer& GetStateTimer()
		{ return _state_timer; }

	void SetXOrigin(float x_origin)
		{ _x_origin = x_origin; }

	void SetYOrigin(float y_origin)
		{ _y_origin = y_origin; }

	void SetXLocation(float x_location)
		{ _x_location = x_location; }

	void SetYLocation(float y_location)
		{ _y_location = y_location; }

	void SetStatePaused(bool paused)
		{ _state_paused = paused; }

	//! \note If the actor is in the idle state, this will not affect the state timer
	 void SetIdleStateTime(uint32 time)
		{ _idle_state_time = time; }
	//@}

protected:
	//! \brief The state that the actor is currently in
	ACTOR_STATE _state;

	//! \brief A pointer to the global actor object which the battle actor represents
	hoa_global::GlobalActor* _global_actor;

	//! \brief A pointer to the action that the actor is preparing to perform or is currently performing
	BattleAction* _action;

	//! \brief The "home" coordinates for the actor's default location on the battle field
	float _x_origin, _y_origin;

	//! \brief The x and y coordinates of the actor's current location on the battle field
	float _x_location, _y_location;

	//! \brief Set to true when the actor is in the ACTING state and the execution of the action is complete
	bool _execution_finished;

	/** \brief When set to true, the actor's state timer will not update and their animations will cease
	*** This serves a different purpose than pausing/resuming the _state_timer directly, which the battle code
	*** makes frequent use of. This is a more "long term" effect, primarily intended to aid status effects and
	*** events in allowing an actor to be temporarily put out of action.
	**/
	bool _state_paused;

	//! \brief The amount of time (in milliseconds) that the actor needs to wait to pass through the idle state
	uint32 _idle_state_time;

	//! \brief A timer used as the character progresses through the standard series of actor states
	BattleTimer _state_timer;

	//! \brief Used to assist in the animation of actors as they move on the battlefield
	hoa_system::SystemTimer _animation_timer;

	// TODO: add later for effects such as "petrify"
//	//! \brief When set to true, the actor essentially has immunity and can not be targeted by attacks
//	bool _not_targetable;

	//! \brief The actor's icon for the stamina meter
	hoa_video::StillImage _stamina_icon;

	//! \brief An assistant class to the actor that manages all the actor's status and elemental effects
	EffectsSupervisor* _effects_supervisor;

	//! \brief An assistant class to the actor that manages all the actor's indicator text and graphics
	IndicatorSupervisor* _indicator_supervisor;
}; // class BattleActor


/** ****************************************************************************
*** \brief Represents a player-controlled character in the battle
***
*** Character actors have a series of animated images that reflect their current
*** state and actions. Each character also has a custom set of progressive damage
*** battle portraits (5 in total) that are drawn when the character is selected.
*** ***************************************************************************/
class BattleCharacter : public BattleActor {
public:
	BattleCharacter(hoa_global::GlobalCharacter* character);

	~BattleCharacter();

	bool IsEnemy() const
		{ return false; }

	void ResetActor();

	void ChangeState(ACTOR_STATE new_state);

	float GetSpriteWidth() const
		{ return 0.0f; } // TEMP: should retrieve width of current sprite animation

	float GetSpriteHeight() const
		{ return 0.0f; } // TEMP: should retrieve height of current sprite animation

	void ChangeSpriteAnimation(const std::string& alias);

	//! \brief Changes the action and target selection text to reflect the character's current state
	void ChangeActionText();

	//! \brief Returns true if the player may select a command for the character to execute
	bool CanSelectCommand() const
		{ return (_state == ACTOR_STATE_IDLE) || (_state == ACTOR_STATE_COMMAND); }

	//! \brief Updates the state of the character. Must be called every frame loop.
	void Update(bool animation_only = false);

	//! \brief Draws the character's current sprite animation frame
	void DrawSprite();

	//! \brief Draws the character's damage-blended face portrait
	void DrawPortrait();

	/** \brief Draws the character's status in the bottom area of the screen
	*** \param order The order position of the character [0-3] used to determine draw positions
	**/
	void DrawStatus(uint32 order);

	hoa_global::GlobalCharacter* GetGlobalCharacter()
		{ return _global_character; }

	const std::string& GetSpriteAnimationAlias() const
		{ return _sprite_animation_alias; }

protected:
	//! \brief A pointer to the global character object which the battle character represents
	hoa_global::GlobalCharacter* _global_character;

	//! \brief Retrains the last HP and SP values that were rendered to text
	uint32 _last_rendered_hp, _last_rendered_sp;

	//! \brief Contains the identifier text of the current sprite animation
	std::string _sprite_animation_alias;

	//! \brief Rendered text of the character's name
	hoa_video::TextImage _name_text;

	//! \brief Rendered text of the character's current hit points
	hoa_video::TextImage _hit_points_text;

	//! \brief Rendered text of the character's current skill points
	hoa_video::TextImage _skill_points_text;

	//! \brief Rendered text of the character's currently selected action
	hoa_video::TextImage _action_selection_text;

	//! \brief Rendered text of the character's currently selected target
	hoa_video::TextImage _target_selection_text;
}; // class BattleCharacter


/** ****************************************************************************
*** \brief Represents the entity for an enemy in the battle
***
*** This class is a wrapper around a GlobalEnemy object.
*** ***************************************************************************/
class BattleEnemy : public BattleActor {
public:
	BattleEnemy(hoa_global::GlobalEnemy* enemy);

	~BattleEnemy();

	bool IsEnemy() const
		{ return true; }

	void ResetActor();

	void ChangeState(ACTOR_STATE new_state);

	float GetSpriteWidth() const
		{ return _global_enemy->GetBattleSpriteFrames()->at(0).GetWidth(); }

	float GetSpriteHeight() const
		{ return _global_enemy->GetBattleSpriteFrames()->at(0).GetHeight(); }

	//! \note Enemies do not have animations so calling this function will achieve nothing
	void ChangeSpriteAnimation(const std::string& alias)
		{ return; }

	void Update(bool animation_only = false);

	//! \brief Draws the damage blended enemy sprite image on to the battle field
	void DrawSprite();

	hoa_global::GlobalEnemy* GetGlobalEnemy()
		{ return _global_enemy; }

	//! \brief Compares the Y-coordinates of the actors, used for sorting the actors up-down when drawing
	bool operator<(const BattleEnemy & other) const;

protected:
	//! \brief A pointer to the global enemy object which the battle enemy represents
	hoa_global::GlobalEnemy* _global_enemy;

	//! \brief An unsorted vector containing all the skills that the enemy may use
	std::vector<hoa_global::GlobalSkill*> _enemy_skills;

	/** \brief Decides what action that the enemy should execute and the target
	*** \todo This function is extremely rudimentary right now. Later, it should be given a more complete
	*** AI decision making algorithm
	**/
	void _DecideAction();
}; // class BattleEnemy

} // namespace private_battle

} // namespace hoa_battle

#endif // __BATTLE_ACTORS_HEADER__
