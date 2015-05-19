///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_utils.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for battle mode utility code
***
*** This file contains utility code that is shared among the various battle mode
*** classes.
*** ***************************************************************************/

#ifndef __BATTLE_UTILS_HEADER__
#define __BATTLE_UTILS_HEADER__

#include "engine/system.h"
#include "common/global/global_objects.h"

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
bool RndEvade(BattleActor* target_actor, float add_eva, float mul_eva, int32 attack_point);

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
uint32 RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor, uint32 add_atk = 0, float mul_atk = 1.0f, int32 attack_point = -1);

// Aliases
//! Useful to make it work with luabind, as it doesn't function with default parameters.
uint32 RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor, uint32 add_atk, float mul_atk);
uint32 RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor, uint32 add_atk);
uint32 RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor);

/** \brief Determines the amount of damage caused with a magical attack
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param element The element used when attacking using magic.
*** \param add_eva A modifier value to be added to the standard attack.
*** \param mul_eva A modifier value to be multiplied to the standard attack.
*** \param attack_point The attack point target on the given actor. -1 if a standard attack is used.
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
**/
uint32 RndMagicalDamage(BattleActor* attacker, BattleActor* target_actor, vt_global::GLOBAL_ELEMENTAL element,
                        uint32 add_atk = 0, float mul_atk = 1.0f, int32 attack_point = -1);

//! \brief A smaller standard function with no default parameters.
//! Useful to make it work with luabind, as it doesn't function with default parameters.
uint32 StdRndMagicalDamage(BattleActor* attacker, BattleActor* target_actor, vt_global::GLOBAL_ELEMENTAL element);


/** ****************************************************************************
*** \brief Container class for representing the target of a battle action
***
*** Valid target types include attack points, actors, and parties. This class is
*** somewhat of a wrapper and allows a single instance of BattleTarget to represent
*** any of these types. It also contains a handful of methods useful in determining
*** the validity of a selected target and selecting another target of the same type.
***
*** Many of these functions are dependent on receiving a pointer to a BattleActor
*** object that is using or intends to use the BattleTarget object. This is necessary
*** because the different types of the GLOBAL_TARGET enum are relative and the class
*** selects different targets relative to the user. For example, selecting the next
*** actor when the target type is GLOBAL_TARGET_ALLY requires knowing whether the user
*** is a character or an enemy.
*** ***************************************************************************/
class BattleTarget
{
public:
    BattleTarget();

    ~BattleTarget()
    {}

    BattleTarget& operator=(const BattleTarget& copy);

    //! \brief Resets all class members, invalidating the target
    void InvalidateTarget();

    //! \brief Reinit the attack point. Used when changing target.
    void ReinitAttackPoint() {
        _attack_point = 0;
    }

    /** \brief Used to set the initial target
    *** \param type The type of target to set
    *** \param attacker A pointer to the actor which will use the target.
    *** \param target A pointer to the actor which will use the target or NULL. When NULL, the first valid target depending on type is used.
    *** \param attack_point An integer index into the actor's attack points
    ***
    *** If the function fails to find an initial target, the target type will be set to
    *** GLOBAL_TARGET_INVALID. The initial attack point is always the first available point on the
    *** target (index 0). The initial target will always be the first valid actor in its party (index 0).
    **/
    bool SetTarget(BattleActor* attacker, vt_global::GLOBAL_TARGET type, BattleActor* target = NULL, uint32 attack_point = 0);

    /** \brief Returns true if the target is valid
    *** This method assumes that a valid target is one that is alive (non-zero HP). If the target type
    *** is an actor or attack point, the function returns true so long as the target actor is alive.
    *** If the target type is a party, this method will always return true as parties always have at
    *** least one living actor unless the battle has ended.
    ***
    *** Not all actions/skills/items should rely on this method for determining whether or not the
    *** target is valid for their particular circumstances. For example, a revive item is only valid
    *** to use on a dead actor. Other actions/items may have their own criteria for determining what
    *** is a valid target.
    **/
    bool IsValid();

    /** \brief Changes the target attack point to reference the next available attack point target
    *** \param direction Tells the method to look either forward or backward (true/false) for the next target
    *** \return True if the attack point or actor target was changed, false if no change took place
    ***
    *** This method should only be invoked when the _type member is equal to one of the "POINT" types.
    *** Under normal circumstances this method will simply reference the next attack point available on
    *** the targeted actor. However, if the actor is deceased and the valid_criteria member is set to true,
    *** this will cause the method to look for the next available actor and call the SelectNextActor() method.
    ***
    *** If the action/skill/item has special criteria for determining what type of a target is valid, the valid_criteria
    *** member should be set to false. This will ignore whether or not the target actor is deceased and allow external
    *** code to determine the validity of the new attack point target itself.
    **/
    bool SelectNextPoint(bool direction = true);

    /** \brief Changes the target actor to reference the next available actor
    *** \param direction Tells the method to look either forward or backward (true/false) for the next target
    *** \return True if the _actor member was changed, false if it was not
    ***
    *** This method should only be called when the target type is not one of the party types.
    **/
    bool SelectNextActor(bool direction = true);

    /** \brief Retrieves a pointer to the actor of a party at the specified index
    *** \param index The location in the party container of the actor to retrieves
    *** \return NULL if the target is not a party or the index is invalid. Otherwise a pointer to the actor specified
    ***
    *** The primary purpose for the existence of this function is for Lua to be able to access all the actors within a
    *** party target. The GetParty() method can not be used by Lua as Lua does not understand that container format
    *** (std::deque<BattleActor*>). To retrieve each actor, Lua code starts at index 0 and makes repeated calls to this
    *** function while incrementing the index by 1 until it returns a NULL value.
    **/
    BattleActor* GetPartyActor(uint32 index);

    /** \brief Returns the name of the target
    ***
    *** Party type targets will return "All Allies" or "All Enemies". Actor type targets return the name of the character or
    *** enemy, for example "Claudius" or "Red Spider" . Attack point type targets return the name of the character or enemy
    *** seperated by a hyphen and followed by the point name, for example "Bat — Wing". Invalid targets will return "[Invalid Target]".
    **/
    vt_utils::ustring GetName();

    //! \name Class member accessor methods
    //@{
    inline vt_global::GLOBAL_TARGET GetType() const {
        return _type;
    }

    inline uint32 GetAttackPoint() const {
        return _attack_point;
    }

    inline BattleActor* GetActor() const {
        return _actor_target;
    }

    const std::deque<BattleActor *>& GetPartyTarget() const {
        return _party_target;
    }
    //@}

private:
    //! \brief The type of target this object represents (attack point, actor, or party)
    vt_global::GLOBAL_TARGET _type;

    //! \brief The attack point to target, as an index to the proper point on the _actor_target
    uint32 _attack_point;

    //! \brief The current actor to target
    BattleActor* _actor_target;

    //! \brief The current party to target
    std::deque<BattleActor *> _party_target;
}; // class BattleTarget


/** ****************************************************************************
*** \brief A simple container class for items that may be used in battle
***
*** This class adds an additional member to be associated with GlobalItem objects
*** which keeps track of how many of that item are available to use. This is necessary
*** because when an actor selects an item to use, they do not immediately use that
*** item and may ultimately not use the item due to the user becoming incapacitated
*** or having no valid target for the item. At all times, the available count of an item
*** will be less than or equal to the actual count of the item.
*** ***************************************************************************/
class BattleItem
{
public:
    //! \param item A pointer to the item to represent. Should be a non-NULL value.
    BattleItem(vt_global::GlobalItem item);

    ~BattleItem();

    //! \brief Class member accessor methods
    //@{
    vt_global::GlobalItem& GetGlobalItem() {
        return _item;
    }

    inline uint32 GetBattleCount() const {
        return _battle_count;
    }
    //@}

    /** \brief Increases the available count of the item by one for this battle
    *** The available count will not be allowed to exceed the GlobalItem _count member
    *** Note that the battle and inventory counts are separated because the changes are only committed
    *** when the battle is won.
    **/
    void IncrementBattleCount();

    /** \brief Decreases the available count of the item by one for this battle
    *** The available count will not be allowed to decrement below zero
    *** Note that the battle and inventory counts are separated because the changes are only committed
    *** when the battle is won.
    **/
    void DecrementBattleCount();

    /** \brief A wrapper function that retrieves the actual count of the item
    *** \note Calling this function is equivalent to calling GetItem().GetCount()
    *** Note that the battle and inventory counts are separated because the changes are only committed
    *** when the battle is won.
    **/
    uint32 GetInventoryCount() const {
        return _item.GetCount();
    }

    /** \brief A wrapper function that retrieves the target type of the item
    *** \note Calling this function is equivalent to calling GetItem().GetTargetType()
    **/
    vt_global::GLOBAL_TARGET GetTargetType() const {
        return _item.GetTargetType();
    }

    //! \brief A wrapper function retrieving the warmup time needed to use the item in battles.
    uint32 GetWarmUpTime() const {
        return _item.GetWarmUpTime();
    }

    //! \brief A wrapper function retrieving the cooldown time needed after the item use in battles.
    uint32 GetCoolDownTime() const {
        return _item.GetCoolDownTime();
    }

private:
    //! \brief The item that this class represents
    vt_global::GlobalItem _item;

    //! \brief The number of instances of this item that are available to be selected to be used
    uint32 _battle_count;
}; // class BattleItem

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_UTILS_HEADER__
