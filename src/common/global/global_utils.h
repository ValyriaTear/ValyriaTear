////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2007 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_utils.h
*** \author  Tyler Olsen, rootslinux@allacrost.org
*** \brief   Header file for global game utility code
***
*** This file contains several common constants, enums, and functions that are
*** used by various global classes.
*** ***************************************************************************/

#ifndef __GLOBAL_UTILS_HEADER__
#define __GLOBAL_UTILS_HEADER__

#include "defs.h"
#include "utils.h"

namespace hoa_global {

namespace private_global {

/** \name Object ID Range Constants
*** These constants set the maximum valid ID ranges for each object category.
*** The full valid range for each object category ID is:
*** - Items:            1-10000
*** - Weapons:      10001-20000
*** - Head Armor:   20001-30000
*** - Torso Armor:  30001-40000
*** - Arm Armor:    40001-50000
*** - Leg Armor:    50001-60000
*** - Shards:       60001-70000
*** - Key Items:    70001-80000
**/
//@{
const uint32 OBJECT_ID_INVALID   = 0;
const uint32 MAX_ITEM_ID         = 10000;
const uint32 MAX_WEAPON_ID       = 20000;
const uint32 MAX_HEAD_ARMOR_ID   = 30000;
const uint32 MAX_TORSO_ARMOR_ID  = 40000;
const uint32 MAX_ARM_ARMOR_ID    = 50000;
const uint32 MAX_LEG_ARMOR_ID    = 60000;
const uint32 MAX_SHARD_ID        = 70000;
const uint32 MAX_KEY_ITEM_ID     = 80000;
const uint32 OBJECT_ID_EXCEEDS   = 80001;
//@}


/** \name Skill ID Range Constants
*** These constants set the maximum valid ID ranges for each skill category.
*** The full valid range for each skill category ID is:
*** - Attack:        1-10000
*** - Defend:    10001-20000
*** - Support:   20001-30000
**/
//@{
const uint32 MAX_ATTACK_ID   = 10000;
const uint32 MAX_DEFEND_ID   = 20000;
const uint32 MAX_SUPPORT_ID  = 30000;
//@}

} // namespace private_global

//! \brief A return value used for when a specified event name fails to be found
const int32 GLOBAL_BAD_EVENT = 0xFFFFFFFF;


/** \name Target Types
*** \brief Enum values used for declaring the type of targets for items, skills, and actions.
***
*** There are three types of groups within these target types:
*** - Attack Point Types: GLOBAL_TARGET_SELF_POINT, GLOBAL_TARGET_ALLY_POINT, GLOBAL_TARGET_FOE_POINT
*** - Actor Types: GLOBAL_TARGET_SELF, GLOBAL_TARGET_ALLY, GLOBAL_TARGET_FOE
*** - Party Types: GLOBAL_TARGET_ALL_ALLIES, GLOBAL_TARGET_ALL_FOES
***
*** \note The phrasing of "ally" and "foe" in these enum names is relative to the context it is used in.
*** For example, a character using a skill that targets an ally will target one of the other characters in
*** the party while a foe target will select an enemy on the opposing party. On the other hand, if an enemy
*** to the character party is selecting an ally target it will target one of the other enemies in the party
*** and the foe of the enemy is a character.
**/
enum GLOBAL_TARGET {
	GLOBAL_TARGET_INVALID      = -1,
	GLOBAL_TARGET_SELF_POINT   =  0,
	GLOBAL_TARGET_ALLY_POINT   =  1, //!< \note This includes allowing the user to target itself
	GLOBAL_TARGET_FOE_POINT    =  2,
	GLOBAL_TARGET_SELF         =  3,
	GLOBAL_TARGET_ALLY         =  4, //!< \note This includes allowing the user to target itself
	GLOBAL_TARGET_FOE          =  5,
	GLOBAL_TARGET_ALL_ALLIES   =  6,
	GLOBAL_TARGET_ALL_FOES     =  7,
	GLOBAL_TARGET_TOTAL        =  8
};


/** \name GlobalItem and GlobalSkill Usage Cases
*** \brief Enum values used for identification of different game object types
**/
enum GLOBAL_USE {
	GLOBAL_USE_INVALID = -1,
	GLOBAL_USE_FIELD   =  0, //!< Usable only in maps and menus
	GLOBAL_USE_BATTLE  =  1, //!< Usable only in battle
	GLOBAL_USE_ALL     =  2, //!< Usable at any time and any location
	GLOBAL_USE_TOTAL   =  3
};


/** \name Game Character IDs
*** \brief Integers that are used for identification of characters
*** These series of constants are used as bit-masks for determining things such as if the character
*** may use a certain item. Only one bit should be set for each character ID.
***
*** \note The IDs for each character are defined in the dat/global.lua file.
**/
//@{
const uint32 GLOBAL_CHARACTER_INVALID     = 0x00000000;
const uint32 GLOBAL_CHARACTER_ALL         = 0xFFFFFFFF;
//@}


/** \name Character Attack Point Positions
*** \brief Integers that represent the index location of the four attack points and armor types for characters
**/
//@{
const uint32 GLOBAL_POSITION_HEAD  = 0;
const uint32 GLOBAL_POSITION_TORSO = 1;
const uint32 GLOBAL_POSITION_ARMS  = 2;
const uint32 GLOBAL_POSITION_LEGS  = 3;
//@}


//! \brief The maximum number of characters that can be in the active party
const uint32 GLOBAL_MAX_PARTY_SIZE = 4;


/** \name GlobalObject Types
*** \brief Used for identification of different game object types
**/
enum GLOBAL_OBJECT {
	GLOBAL_OBJECT_INVALID     = -1,
	GLOBAL_OBJECT_ITEM        =  0,
	GLOBAL_OBJECT_WEAPON      =  1,
	GLOBAL_OBJECT_HEAD_ARMOR  =  2,
	GLOBAL_OBJECT_TORSO_ARMOR =  3,
	GLOBAL_OBJECT_ARM_ARMOR   =  4,
	GLOBAL_OBJECT_LEG_ARMOR   =  5,
	GLOBAL_OBJECT_SHARD       =  6,
	GLOBAL_OBJECT_KEY_ITEM    =  7,
	GLOBAL_OBJECT_TOTAL       =  8
};


/** \name Elemental Effect Types
*** \brief Used to identify the eight different types of elementals
*** There are a total of four physical and four metaphysical elemental effects
**/
enum GLOBAL_ELEMENTAL {
	GLOBAL_ELEMENTAL_INVALID    = -1,
	GLOBAL_ELEMENTAL_FIRE       =  0,
	GLOBAL_ELEMENTAL_WATER      =  1,
	GLOBAL_ELEMENTAL_VOLT       =  2,
	GLOBAL_ELEMENTAL_EARTH      =  3,
	GLOBAL_ELEMENTAL_SLICING    =  4,
	GLOBAL_ELEMENTAL_SMASHING   =  5,
	GLOBAL_ELEMENTAL_MAULING    =  6,
	GLOBAL_ELEMENTAL_PIERCING   =  7,
	GLOBAL_ELEMENTAL_TOTAL      =  8
};


/** \name Status Effect Types
*** \brief Used to identify the various types of status effects
**/
enum GLOBAL_STATUS {
	GLOBAL_STATUS_INVALID           = -1,
	GLOBAL_STATUS_STRENGTH_RAISE    =  0,
	GLOBAL_STATUS_STRENGTH_LOWER    =  1,
	GLOBAL_STATUS_VIGOR_RAISE       =  2,
	GLOBAL_STATUS_VIGOR_LOWER       =  3,
	GLOBAL_STATUS_FORTITUDE_RAISE   =  4,
	GLOBAL_STATUS_FORTITUDE_LOWER   =  5,
	GLOBAL_STATUS_PROTECTION_RAISE  =  6,
	GLOBAL_STATUS_PROTECTION_LOWER  =  7,
	GLOBAL_STATUS_AGILITY_RAISE     =  8,
	GLOBAL_STATUS_AGILITY_LOWER     =  9,
	GLOBAL_STATUS_EVADE_RAISE       = 10,
	GLOBAL_STATUS_EVADE_LOWER       = 11,
	GLOBAL_STATUS_HP_REGEN          = 12,
	GLOBAL_STATUS_HP_DRAIN          = 13,
	GLOBAL_STATUS_SP_REGEN          = 14,
	GLOBAL_STATUS_SP_DRAIN          = 15,
	GLOBAL_STATUS_PARALYSIS         = 16,
	GLOBAL_STATUS_STASIS            = 17,
	GLOBAL_STATUS_TOTAL             = 18
};


/** \name Effect Intensity Levels
*** \brief Used to reflect the potency of elemental and status effects
*** There are nine valid intensity levels. Four negative, four positive, and one neutral.
*** The neutral intensity level essentially equates to "no effect".
**/
enum GLOBAL_INTENSITY {
	GLOBAL_INTENSITY_INVALID       = -5,
	GLOBAL_INTENSITY_NEG_EXTREME   = -4,
	GLOBAL_INTENSITY_NEG_GREATER   = -3,
	GLOBAL_INTENSITY_NEG_MODERATE  = -2,
	GLOBAL_INTENSITY_NEG_LESSER    = -1,
	GLOBAL_INTENSITY_NEUTRAL       =  0,
	GLOBAL_INTENSITY_POS_LESSER    =  1,
	GLOBAL_INTENSITY_POS_MODERATE  =  2,
	GLOBAL_INTENSITY_POS_GREATER   =  3,
	GLOBAL_INTENSITY_POS_EXTREME   =  4,
	GLOBAL_INTENSITY_TOTAL         =  5
};


/** \name Skill Types
*** \brief Enum values used to identify the type of a skill.
**/
enum GLOBAL_SKILL {
	GLOBAL_SKILL_INVALID  = -1,
	GLOBAL_SKILL_ATTACK   =  0,
	GLOBAL_SKILL_DEFEND   =  1,
	GLOBAL_SKILL_SUPPORT  =  2,
	GLOBAL_SKILL_TOTAL    =  3
};


/** \name Battle setting type
*** \brief Represents the play types of battle that the player may choose between
**/
enum GLOBAL_BATTLE_SETTING {
	GLOBAL_BATTLE_INVALID   = -1,
	GLOBAL_BATTLE_WAIT      =  0, //!< Battles will pause ("wait") while player selects commands
	GLOBAL_BATTLE_ACTIVE    =  1, //!< Battles will continue progressing while player selects commands
	GLOBAL_BATTLE_TOTAL     =  2
};


/** \brief Retrieves a string representation for any GLOBAL_TARGET enum value
*** \param target The target enum value to find the string for
*** \return Translated text that describes the target
**/
std::string GetTargetText(GLOBAL_TARGET target);


/** \brief Returns true if the target selects an attack point
*** \param target The target enum value to analyze
**/
bool IsTargetPoint(GLOBAL_TARGET target);


/** \brief Returns true if the target selects an actor
*** \param target The target enum value to analyze
**/
bool IsTargetActor(GLOBAL_TARGET target);


/** \brief Returns true if the target selects a party
*** \param target The target enum value to analyze
**/
bool IsTargetParty(GLOBAL_TARGET target);


/** \brief Returns true if the target selects the user
*** \param target The target enum value to analyze
**/
bool IsTargetSelf(GLOBAL_TARGET target);


/** \brief Returns true if the target selects an ally
*** \param target The target enum value to analyze
**/
bool IsTargetAlly(GLOBAL_TARGET target);


/** \brief Returns true if the target selects a foe
*** \param target The target enum value to analyze
**/
bool IsTargetFoe(GLOBAL_TARGET target);


/** \brief Creates a new GlobalObject and returns a pointer to it
*** \param id The id value of the object to create
*** \param count The count of the new object to create (default value == 1)
*** \return A pointer to the newly created GlobalObject, or NULL if the object could not be created
***
*** This function does not actually create a GlobalObject (it can't since its an abstract class).
*** It creates one of the derived object class types depending on the value of the id argument.
**/
GlobalObject* GlobalCreateNewObject(uint32 id, uint32 count = 1);


/** \brief Increments a GLOBAL_INTENSITY enumerated value
*** \param intensity A reference to the intensity data to modify
*** \param amount The number of levels to increase the intensity by (default == 1)
*** \return True if the intensity data was modified or false if it was left unchanged
*** \note The intensity will not be allowed to increase beyond the valid intensity range
**/
bool IncrementIntensity(GLOBAL_INTENSITY& intensity, uint8 amount = 1);


/** \brief Decrements a GLOBAL_INTENSITY enumerated value
*** \param intensity A reference to the intensity data to modify
*** \param amount The number of levels to decrease the intensity by (default == 1)
*** \return True if the intensity data was modified or false if it was left unchanged
*** \note The intensity will not be allowed to decrease beyond the valid intensity range
**/
bool DecrementIntensity(GLOBAL_INTENSITY& intensity, uint8 amount = 1);

} // namespace hoa_global

#endif // __GLOBAL_UTILS_HEADER__
