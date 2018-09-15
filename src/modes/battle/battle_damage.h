///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_DAMAGE_HEADER__
#define __BATTLE_DAMAGE_HEADER__

#include "common/global/objects/global_item.h"
#include "common/global/status_effects/status_effect_enums.h"

namespace vt_battle
{

namespace private_battle
{

class BattleActor;
class BattleTarget;

/** \brief Determines if a target has evaded an attack or other action
*** \param target_actor A pointer to the target to calculate evasion for
*** \param add_eva A modifier value to be added to the standard evasion rating
*** \param mul_eva A modifier value to be multiplied to the standard evasion rating
*** \param attack_point The attack point target on the given actor. -1 if a standard attack is used.
*** \return True if the target evasion was successful
**/
bool RndEvade(BattleActor* target_actor, float add_eva, float mul_eva, int32_t attack_point);

// Aliases
//! Useful to make it work with luabind, as it doesn't function with default parameters.
bool RndEvade(BattleActor* target_actor, float add_eva, float mul_eva);
bool RndEvade(BattleActor* target_actor, float add_eva);
bool RndEvade(BattleActor* target_actor);

/** \brief Determines at random the amount of damage caused with a physical attack
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param add_eva A modifier value to be added to the standard attack.
*** \param mul_eva A modifier value to be multiplied to the standard attack.
*** \param attack_point The attack point target on the given actor. -1 if a standard attack is used.
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
**/
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor,
                           uint32_t add_atk = 0, float mul_atk = 1.0f, int32_t attack_point = -1);

// Aliases
//! Useful to make it work with luabind, as it doesn't function with default parameters.
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor, uint32_t add_atk, float mul_atk);
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor, uint32_t add_atk);
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor);

uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor);
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor, uint32_t add_atk);
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor, uint32_t add_atk, float mul_atk);
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor,
                           uint32_t add_atk, float mul_atk, int32_t attack_point);

/** \brief Determines the amount of damage caused with a magical attack
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param element The element used when attacking using magic.
*** \param add_eva A modifier value to be added to the standard attack.
*** \param mul_eva A modifier value to be multiplied to the standard attack.
*** \param attack_point The attack point target on the given actor. -1 if a standard attack is used.
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
**/
uint32_t RndMagicalDamage(BattleActor* attacker, BattleActor* target_actor, vt_global::GLOBAL_ELEMENTAL element,
                          uint32_t add_atk, float mul_atk, int32_t attack_point);

// Aliases
//! Useful to make it work with luabind, as it doesn't function with default parameters.
uint32_t RndMagicalDamage(BattleActor* attacker,
                          BattleActor* target_actor,
                          vt_global::GLOBAL_ELEMENTAL element,
                          uint32_t add_atk,
                          float mul_atk);

uint32_t RndMagicalDamage(BattleActor* attacker,
                          BattleActor* target_actor,
                          vt_global::GLOBAL_ELEMENTAL element,
                          uint32_t add_atk);

uint32_t RndMagicalDamage(BattleActor* attacker,
                          BattleActor* target_actor,
                          vt_global::GLOBAL_ELEMENTAL element);

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_DAMAGE_HEADER__
