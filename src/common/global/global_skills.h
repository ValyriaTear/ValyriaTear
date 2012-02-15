////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_skills.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for global game skills.
***
*** This file contains the class implementation for "skills", which are the
*** actions that characters or enemies may take during a battle or sometimes
*** in another context.
*** ***************************************************************************/

#ifndef __GLOBAL_SKILLS_HEADER__
#define __GLOBAL_SKILLS_HEADER__

#include "defs.h"
#include "utils.h"

#include "script.h"

#include "global_utils.h"

namespace hoa_global {

/** ****************************************************************************
*** \brief Represents skills that are used in the game by both characters and enemies
***
*** Skills are actions that a character or enemy may take in battle. Some skills
*** may also be used on the field in menus (such as a healing spell). The functional
*** execution of a skill is done by a Lua function, which this class manages pointers
*** to. Typically skills are not shared between characters and enemies, primarily
*** because character sprites are fully animated when executing actions, while enemies
*** are not animated.
***
*** There are three types of skills: attack, defend, and support. Functionally the skills
*** are no different and this class represents all three types of skills. The skill types
*** exist for convenience purposes to lump skills into groups roughly based on the type
*** of effect they cause. Skill definitions and execution functions are retained in
*** one of three Lua files (a file exists for each type of skill) and these files are used
*** to initialize the skill's data.
***
*** Because skills are scripted and can achieve almost any possible effect, this class
*** only retains the common properties that all skills share. For example, the skill's
*** name, type of target, and the amount of time it takes an actor to "warmup" to use
*** the skill or "cooldown" after the skill execution is finished.
*** ***************************************************************************/
class GlobalSkill {
public:
	//! \param id The identification number of the skill to construct
	GlobalSkill(uint32 id);

	~GlobalSkill();

	GlobalSkill(const GlobalSkill& copy);

	GlobalSkill& operator=(const GlobalSkill& copy);

	//! \brief Returns true if the skill is properly initialized and ready to be used
	bool IsValid() const
		{ return (_id != 0); }

	//! \brief Returns true if the skill can be executed in battles
	bool IsExecutableInBattle() const
		{ return (_battle_execute_function != NULL); }

	//! \brief Returns true if the skill can be executed in menus
	bool IsExecutableInField() const
		{ return (_field_execute_function != NULL); }

	/** \name Class member access functions
	*** \note No set functions are defined because the class members should only be intialized within Lua
	**/
	//@{
	hoa_utils::ustring GetName() const
		{ return _name; }

	hoa_utils::ustring GetDescription() const
		{ return _description; }

	uint32 GetID() const
		{ return _id; }

	uint8 GetType() const
		{ return _type; }

	uint32 GetSPRequired() const
		{ return _sp_required; }

	uint32 GetWarmupTime() const
		{ return _warmup_time; }

	uint32 GetCooldownTime() const
		{ return _cooldown_time; }

	GLOBAL_TARGET GetTargetType() const
		{ return _target_type; }

	/** \brief Returns a pointer to the ScriptObject of the battle execution function
	*** \note This function will return NULL if the skill is not executable in battle
	**/
	const ScriptObject* GetBattleExecuteFunction() const
		{ return _battle_execute_function; }

	/** \brief Returns a pointer to the ScriptObject of the menu execution function
	*** \note This function will return NULL if the skill is not executable in menus
	**/
	const ScriptObject* GetFieldExecuteFunction() const
		{ return _field_execute_function; }
	//@}

private:
	//! \brief The unique identifier number of the skill.
	uint32 _id;

	//! \brief The name of the skill as it will be displayed on the screen.
	hoa_utils::ustring _name;

	/** \brief A short description of what the skill does when executed
	*** \note Not all defined skills have a description. For example, skills used only by enemies are
	*** typically missing a description
	**/
	hoa_utils::ustring _description;

	//! \brief The type identifier for the skill
	GLOBAL_SKILL _type;

	/** \brief The amount of skill points (SP) that the skill requires to be used
	*** Zero is a valid value for this member and means that no skill points are required to use the
	*** skill. Skills with this property are known as "innate skills".
	**/
	uint32 _sp_required;

	/** \brief The amount of time (in milliseconds) that must expire before a skill can be used after it is selected
	*** When a character or enemy has selected to use the skill in a battle, this value instructs how
	*** much time must pass before the skill may be executed. It is acceptable for this member to be zero.
	**/
	uint32 _warmup_time;

	/** \brief The amount of time (in milliseconds) that must expire after a skill hase been used
	*** After a character or enemy uses a skill, this value instructs how much time must pass before
	*** the actor who executed the skill can recover and begin recharging their battle stamina bar.
	*** It is acceptable for this member to be zero.
	**/
	uint32 _cooldown_time;

	/** \brief The type of target for the skill
	*** Target types include attack points, actors, and parties. This enum type is defined in global_actors.h
	**/
	GLOBAL_TARGET _target_type;

	//! \brief A pointer to the skill's execution function for battles
	ScriptObject* _battle_execute_function;

	//! \brief A pointer to the skill's execution function for menus
	ScriptObject* _field_execute_function;
}; // class GlobalSkill

} // namespace hoa_global

#endif // __GLOBAL_SKILLS_HEADER__
