///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_UTILS_HEADER__
#define __BATTLE_UTILS_HEADER__

#include "common/global/objects/global_item.h"
#include "engine/system.h"

#include <deque>

namespace vt_battle
{

namespace private_battle
{

class BattleActor;
class BattleTarget;

//! \brief Position constants representing the significant locations along the stamina meter
//@{
//! \brief The X and Y position of the stamina bar
const float STAMINA_BAR_POSITION_X = 970.0f;
const float STAMINA_BAR_POSITION_Y = 640.0f;
//@}

//! \brief Used to indicate what state the overall battle is currently operating in
enum BATTLE_STATE {
    BATTLE_STATE_INVALID   = -1,
    BATTLE_STATE_INITIAL   =  0, //!< Character sprites are running in from off-screen to their battle positions
    BATTLE_STATE_NORMAL    =  1, //!< Normal state where player is watching actions play out and waiting for a turn
    BATTLE_STATE_COMMAND   =  2, //!< Player is choosing a command for a character
    BATTLE_STATE_VICTORY   =  3, //!< Battle has ended with the characters victorious
    BATTLE_STATE_DEFEAT    =  4, //!< Battle has ended with the characters defeated
    BATTLE_STATE_EXITING   =  5, //!< Player has closed battle windows and battle mode is fading out
    BATTLE_STATE_TOTAL     =  6
};


//! \brief Represents the possible states that a BattleActor may be in
enum ACTOR_STATE {
    ACTOR_STATE_INVALID       = -1,
    ACTOR_STATE_IDLE          =  0, //!< Actor is recovering stamina so they can execute another action
    ACTOR_STATE_COMMAND       =  1, //!< Actor is finished with the idle state and needs to select an action to execute
    ACTOR_STATE_WARM_UP       =  2, //!< Actor has selected an action and is preparing to execute it
    ACTOR_STATE_READY         =  3, //!< Actor is prepared to execute action and is waiting their turn to act
    ACTOR_STATE_SHOWNOTICE    =  4, //!< Actor is waiting for the special skill short notice to disappear
    ACTOR_STATE_NOTICEDONE    =  5, //!< Actor is has shown the special skill short notice and is ready to act.
    ACTOR_STATE_ACTING        =  6, //!< Actor is in the process of executing their selected action
    ACTOR_STATE_COOL_DOWN     =  7, //!< Actor is finished with previous action execution and recovering
    ACTOR_STATE_DYING         =  8, //!< Actor is in the transitive dying state.
    ACTOR_STATE_DEAD          =  9, //!< Actor has perished and is inactive in battle
    ACTOR_STATE_REVIVE        =  10, //!< Actor coming back from coma, and in the process to stand up again.
    ACTOR_STATE_PARALYZED     =  11, //!< Actor is in some state of paralysis and can not act nor recover stamina
    ACTOR_STATE_TOTAL         =  12
};

//! \brief Enums for the various states that the CommandSupervisor class may be in
enum COMMAND_STATE {
    COMMAND_STATE_INVALID         = -1,
    //! Player is selecting the type of action to execute
    COMMAND_STATE_CATEGORY        = 0,
    //! Player is selecting from a list of actions to execute
    COMMAND_STATE_ACTION          = 1,
    //! Player is selecting the actor target to execute the action on
    COMMAND_STATE_ACTOR           = 2,
    //! Player is selecting the point target to execute the action on
    COMMAND_STATE_POINT           = 3,
    COMMAND_STATE_TOTAL           = 4
};

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
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor, uint32_t add_atk = 0, float mul_atk = 1.0f, int32_t attack_point = -1);

// Aliases
//! Useful to make it work with luabind, as it doesn't function with default parameters.
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor, uint32_t add_atk, float mul_atk);
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor, uint32_t add_atk);
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor);

uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor);
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor, uint32_t add_atk);
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor, uint32_t add_atk, float mul_atk);
uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor, uint32_t add_atk, float mul_atk, int32_t attack_point);

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

#endif // __BATTLE_UTILS_HEADER__
