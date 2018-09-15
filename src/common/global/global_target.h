////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_TARGET_ENUM_HEADER__
#define __GLOBAL_TARGET_ENUM_HEADER__

#include <string>

namespace vt_global
{

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
    GLOBAL_TARGET_INVALID        = -1,
    GLOBAL_TARGET_SELF_POINT     =  0,
    GLOBAL_TARGET_ALLY_POINT     =  1, //!< \note This includes allowing the user to target itself
    GLOBAL_TARGET_FOE_POINT      =  2,
    GLOBAL_TARGET_SELF           =  3,
    GLOBAL_TARGET_ALLY           =  4, //!< \note This includes allowing the user to target itself
    GLOBAL_TARGET_ALLY_EVEN_DEAD =  5,
    GLOBAL_TARGET_DEAD_ALLY_ONLY =  6,
    GLOBAL_TARGET_FOE            =  7,
    GLOBAL_TARGET_ALL_ALLIES     =  8,
    GLOBAL_TARGET_ALL_FOES       =  9,
    GLOBAL_TARGET_TOTAL          =  10
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

} // namespace vt_global

#endif // __GLOBAL_TARGET_ENUM_HEADER__
