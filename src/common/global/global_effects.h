////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2007 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_effects.h
*** \author  Jacob Rudolph, rujasu@allacrost.org
*** \brief   Header file for global game effects
***
*** This file contains the class implementation for status and elemental effects.
*** Status effects are certain states that characters and enemies may fall in
*** to while in battle, such as being poisoned or confused. Elemental effects
*** are special properties that allow an aggressor to take advantage of to
*** expose a weakness on a target.
*** ***************************************************************************/

#ifndef __GLOBAL_EFFECTS_HEADER__
#define __GLOBAL_EFFECTS_HEADER__

#include "defs.h"
#include "utils.h"

#include "global_utils.h"

namespace hoa_global {

/** \brief Retrieves a string representation for any GLOBAL_ELEMENTAL enum value
*** \param type The elemental enum value to find the string for
*** \return Translated text that describes the elemental
**/
std::string GetElementName(GLOBAL_ELEMENTAL type);


/** \brief Retrieves a string representation for any GLOBAL_STATUS enum value
*** \param type The status enum value to find the string for
*** \return Translated text that describes the status
***
*** \note Each call to this function requires the status name to be retrieved from
*** a Lua script so its a slightly costly operation. If you need to make repeated 
*** queries to retrieve status names, consider caching results locally to reduce the
*** number of script file accesses that need to take place.
**/
std::string GetStatusName(GLOBAL_STATUS type);


/** ****************************************************************************
*** \brief Represents an elemental effect in the game
***
*** This class is a simple container of two enumerated values: an elemental type
*** and an intensity. Elemental effects provide for special types of attack and
*** defense bonuses. There are really two types of elemental effects: physical
*** and metaphysical, the same as the two attack damage types. Whether the elemental
*** effect represented by objects of this class are meant to serve as a defensive boost
*** or an offensive boost is determined by the context in which the class object is used.
***
*** \todo Explain any differences between how physical versus metaphyiscal elements
*** function in the game once that decision has been reached.
*** ***************************************************************************/
class GlobalElementalEffect {
public:
	/** \param type The elemental type that this class object should represent
	*** \param intensity The intensity of the elemental (default value == GLOBAL_INTENSITY_NEUTRAL)
	**/
	GlobalElementalEffect(GLOBAL_ELEMENTAL type, GLOBAL_INTENSITY intensity = GLOBAL_INTENSITY_NEUTRAL) :
		_type(type), _intensity(intensity) {}

	~GlobalElementalEffect()
		{}

	//! \brief Class Member Access Functions
	//@{
	GLOBAL_ELEMENTAL GetType() const
		{ return _type; }

	GLOBAL_INTENSITY GetIntensity() const
		{ return _intensity; }

	void SetIntensity(GLOBAL_INTENSITY intensity)
		{ _intensity = intensity; }
	//@}

	/** \brief Increments the elemental effect's intensity
	*** \param amount The number of levels to increase the intensity by (default = 1)
	*** \note The intensity will not be allowed to increase beyond the valid intensity range
	**/
	void IncrementIntensity(uint8 amount = 1);

	/** \brief Decrements the elemental effect's intensity
	*** \param amount The number of levels to decrease the intensity by (default = 1)
	*** \note The intensity will not be allowed to decrease beyond the valid intensity range
	**/
	void DecrementIntensity(uint8 amount = 1);

protected:
	//! \brief The type of elemental that the object represents
	GLOBAL_ELEMENTAL _type;

	//! \brief The intensity level of this elemental effect
	GLOBAL_INTENSITY _intensity;
}; // class GlobalElementalEffect


/** ****************************************************************************
*** \brief Represents a status effect in the game
***
*** Status effects can be either aiding or ailing to the actor with the active
*** status. Unlike elemental effects, status effects have uni-directional intensity
*** levels instead of bi-directional. The intensity of a status effect is never allowed
*** to decrease below the neutral level or to increase above the maximum positive level.
*** Status effects are only active on characters and enemies while they are in battle.
*** ***************************************************************************/
class GlobalStatusEffect {
public:
	/** \param type The status type that this class object should represent
	*** \param intensity The intensity of the status (default value == GLOBAL_INTENSITY_NEUTRAL)
	**/
	GlobalStatusEffect(GLOBAL_STATUS type, GLOBAL_INTENSITY intensity = GLOBAL_INTENSITY_NEUTRAL) :
		_type(type), _intensity(intensity) {}

	virtual ~GlobalStatusEffect()
		{}

	//! \brief Class Member Access Functions
	//@{
	GLOBAL_STATUS GetType() const
		{ return _type; }

	GLOBAL_INTENSITY GetIntensity() const
		{ return _intensity; }

	virtual void SetIntensity(GLOBAL_INTENSITY intensity)
		{ _intensity = intensity; }
	//@}

	/** \brief Increments the status effect intensity by a positive amount
	*** \param amount The number of intensity levels to increase the status effect by
	*** \return True if the intensity level was modified
	*** \note Intensity will not be incremented beyond the maximum valid intensity value
	**/
	virtual bool IncrementIntensity(uint8 amount);

	/** \brief Decrements the status effect intensity by a negative amount
	*** \param amount The number of intensity levels to decrement the status effect by
	*** \return True if the intensity level was modified
	*** \note Intensity will not be decremented below GLOBAL_INTENSITY_NEUTRAL
	**/
	virtual bool DecrementIntensity(uint8 amount);

protected:
	//! \brief The type of status that the object represents
	GLOBAL_STATUS _type;

	//! \brief The intensity level of this status effect
	GLOBAL_INTENSITY _intensity;
}; // class GlobalStatusEffect

} // namespace hoa_global

#endif // __GLOBAL_EFFECTS_HEADER__
