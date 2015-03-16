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

//! \name Screen dimension constants
//@{
//! \brief Battle scenes are visualized via an invisible grid of 64x64 tiles
const uint32 TILE_SIZE     = 64;
//! \brief The length of the screen in number of tiles (16 x 64 = 1024)
const uint32 SCREEN_LENGTH = 16;
//! \brief The height of the screen in number of tiles (12 x 64 = 768)
const uint32 SCREEN_HEIGHT = 12;
//@}


/** \name Action Type Constants
*** \brief Identifications for the types of actions a player's characters may perform
**/
//@{
const int32 CATEGORY_WEAPON    = 0;
const int32 CATEGORY_MAGIC     = 1;
const int32 CATEGORY_SPECIAL   = 2;
const int32 CATEGORY_ITEM      = 3;
//@}


//! \brief Position constants representing the significant locations along the stamina meter
//@{
//! \brief The bottom most position of the stamina bar
const float STAMINA_LOCATION_BOTTOM = 640.0f;

//! \brief The location where each actor is allowed to select a command
const float STAMINA_LOCATION_COMMAND = STAMINA_LOCATION_BOTTOM - 354.0f;

//! \brief The top most position of the stamina bar where actors are ready to execute their actions
const float STAMINA_LOCATION_TOP = STAMINA_LOCATION_BOTTOM - 508.0f;

//! \brief The X and Y position of the stamina bar
const float STAMINA_BAR_POSITION_X = 970.0f;
const float STAMINA_BAR_POSITION_Y = 640.0f;
//@}


//! \brief Returned as an index when looking for a character or enemy and they do not exist
const uint32 INVALID_BATTLE_ACTOR_INDEX = 999;

//! \brief This is the idle state wait time for the fastest actor, used to set idle state timers for all other actors
const uint32 MIN_IDLE_WAIT_TIME = 10000;

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
    ACTOR_STATE_ACTING        =  4, //!< Actor is in the process of executing their selected action
    ACTOR_STATE_COOL_DOWN     =  5, //!< Actor is finished with previous action execution and recovering
    ACTOR_STATE_DYING         =  6, //!< Actor is in the transitive dying state.
    ACTOR_STATE_DEAD          =  7, //!< Actor has perished and is inactive in battle
    ACTOR_STATE_REVIVE        =  8, //!< Actor coming back from coma, and in the process to stand up again.
    ACTOR_STATE_PARALYZED     =  9, //!< Actor is in some state of paralysis and can not act nor recover stamina
    ACTOR_STATE_TOTAL         =  10
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


/** \name Command battle calculation functions
*** These functions perform many of the common calculations that are needed in battle such as determining
*** evasion and the amount of damage dealt. Lua functions that implement the effect of skills and items
*** make the most use of these functions. Thus, these functions are specifically designed for that use
*** and do not utilize C++ features that Lua can not take advantage of, such as references or default
*** values for function arguments.
***
*** There are also many functions that share the same name but have a different function signature. These
*** functions perform the same task but some take extra arguments to make the calculation more flexible. For
*** example, many functions have a version that allows adjustment of the variation by accepting a standard deviation
*** argument.
***
*** \note These calculations only work for valid non-party type targets. If it is desired to use these methods
*** on a party target, a set of targets for each actor in the target party must be extracted and those actor
*** targets used individually for these various methods.
**/
//@{
/** \brief Determines if a target has evaded an attack or other action
*** \param target A pointer to the target to calculate evasion for
*** \return True if the target evasion was successful
**/
bool CalculateStandardEvasion(BattleTarget *target);
// Same but for a precise actor.
bool CalculateStandardEvasion(BattleActor *target_actor);

/** \brief Determines if a target has evaded an attack or other action, utilizing an addition modifier
*** \param target A pointer to the target to calculate evasion for
*** \param add_eva A modifier value to be added/subtracted to the standard evasion rating
*** \return True if the target evasion was successful
***
*** The additional_evasion may be positive or negative. If the total evasion value falls below 0.0f
*** the function will return false and if that value exceeds 100.0f it will return true. Otherwise the total
*** evade value will serve as a standard probability distribution to determine whether the evasion was
*** successful or not.
**/
bool CalculateStandardEvasionAdder(BattleTarget *target, float add_eva);
// Same but for a precise actor.
bool CalculateStandardEvasionAdder(BattleActor *target_actor, float add_eva);

/** \brief Determines if a target has evaded an attack or other action, utilizing a multiplication modifier
*** \param target A pointer to the target to calculate evasion for
*** \param mul_eva A modifier value to be multiplied to the standard evasion rating
*** \return True if the target evasion was successful
***
*** This function operates the same as the CalculateStandardEvasion(...) functions with the exception that
*** its float argument is used as a multiple in the evasion calculation instead of an addition. So for instance
*** if the user wants the evasion chance to increase by 20%, 1.2f would be passed in for the multiple_evasion
*** argument. A decrease by 35% would need a value of 0.65f. Negative multiplier values will cause a warning to
*** be printed and the absolute value of the multiplier will be used.
**/
bool CalculateStandardEvasionMultiplier(BattleTarget *target, float mul_eva);
// Same but for a precise actor.
bool CalculateStandardEvasionMultiplier(BattleActor *target_actor, float add_eva);

/** \brief Determines the amount of damage caused with a physical attack
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
***
*** This function uses the physical attack/defense ratings to calculate the total damage caused. This function
*** uses a gaussian random distribution with a standard deviation of ten percent to perform variation in the
*** damage caused. Therefore this function may return different values each time it is called with the same arguments.
*** If the amount of damage calculates out to zero, a small random non-zero value will be returned instead.
**/
uint32 CalculatePhysicalDamage(BattleActor *attacker, BattleTarget *target);
// Same but for a precise actor.
uint32 CalculatePhysicalDamage(BattleActor *attacker, BattleActor *target_actor);

/** \brief Determines the amount of damage caused with a physical attack
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param std_dev The standard deviation to use in the gaussian distribution, where "0.075f" would represent 7.5% standard deviation
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
***
*** The std_dev value is always relative to the amount of absolute damage calculated prior to the gaussian randomization.
*** This means that you can -not- use this function to declare an absolute standard deviation, such as a value of 20 damage
*** points.
**/
uint32 CalculatePhysicalDamage(BattleActor *attacker, BattleTarget *target, float std_dev);
// Same but for a precise actor.
uint32 CalculatePhysicalDamage(BattleActor *attacker, BattleActor *target_actor, float std_dev);

/** \brief Determines the amount of damage caused with a physical attack, utilizing an addition modifier
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param add_atk An additional amount to add to the physical damage dealt
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
***
*** The add_atk argument may be positive or negative. Large negative values can skew the damage calculation
*** and cause the damage dealt to drop to zero, so be cautious when setting this argument to a negative value.
**/
uint32 CalculatePhysicalDamageAdder(BattleActor *attacker, BattleTarget *target, int32 add_atk);
// Same but for a precise actor.
uint32 CalculatePhysicalDamageAdder(BattleActor *attacker, BattleActor *target_actor, int32 add_atk);

/** \brief Determines the amount of damage caused with a physical attack, utilizing an addition modifier
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param add_atk An additional amount to add to the physical damage dealt
*** \param std_dev The standard deviation to use in the gaussian distribution, where "0.075f" would represent 7.5% standard deviation
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
**/
uint32 CalculatePhysicalDamageAdder(BattleActor *attacker, BattleTarget *target, int32 add_atk, float std_dev);
// Same but for a precise actor.
uint32 CalculatePhysicalDamageAdder(BattleActor *attacker, BattleActor *target_actor, int32 add_atk, float std_dev);

/** \brief Determines the amount of damage caused with a physical attack, utilizing a mulitplication modifier
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param mul_atk An additional amount to be multiplied to the physical damage dealt
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
***
*** This function operates the same as the CalculatePhysicalDamageAdder(...) functions with the exception that
*** its float argument is used as a multipler in the damage calculation instead of an integer addition modifier.
*** So for instance if the user wants the physical damage to decrease by 20% the value of mul_atk would
*** be 0.8f. If a negative multiplier value is passed to this function, its absoute value will be used and
*** a warning will be printed.
**/
uint32 CalculatePhysicalDamageMultiplier(BattleActor *attacker, BattleTarget *target, float mul_phys);
// Same but for a precise actor.
uint32 CalculatePhysicalDamageMultiplier(BattleActor *attacker, BattleActor *target_actor, float mul_phys);

/** \brief Determines the amount of damage caused with a physical attack, utilizing a multiplication modifier
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param mul_atk A modifier to be multiplied to the physical damage dealt
*** \param std_dev The standard deviation to use in the gaussian distribution, where "0.075f" would represent 7.5% standard deviation
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
***
*** This function signature allows the additional option of setting the standard deviation in the gaussian random value calculation.
**/
uint32 CalculatePhysicalDamageMultiplier(BattleActor *attacker, BattleTarget *target, float mul_atk, float std_dev);
// Same but for a precise actor.
uint32 CalculatePhysicalDamageMultiplier(BattleActor *attacker, BattleActor *target_actor, float mul_atk, float std_dev);

/** \brief Determines the amount of damage caused with a magical attack
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param element The element used when attacking using magic.
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
***
*** This function uses the magical attack/defense ratings to calculate the total damage caused. This function
*** uses a gaussian random distribution with a standard deviation of ten percent to perform variation in the
*** damage caused. Therefore this function may return different values each time it is called with the same arguments.
*** If the amount of damage calculates out to zero, a small random non-zero value will be returned instead.
**/
uint32 CalculateMagicalDamage(BattleActor *attacker, BattleTarget *target, vt_global::GLOBAL_ELEMENTAL element);
// Same but for a precise actor.
uint32 CalculateMagicalDamage(BattleActor *attacker, BattleActor *target_actor, vt_global::GLOBAL_ELEMENTAL element);

/** \brief Determines the amount of damage caused with a magical attack
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param element The element used when attacking using magic.
*** \param std_dev The standard deviation to use in the gaussian distribution, where "0.075f" would represent 7.5% standard deviation
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
***
*** The std_dev value is always relative to the amount of absolute damage calculated prior to the gaussian randomization.
*** This means that you can -not- use this function to declare an absolute standard deviation, such as a value of 20 damage
*** points.
**/
uint32 CalculateMagicalDamage(BattleActor *attacker, BattleTarget *target, vt_global::GLOBAL_ELEMENTAL element, float std_dev);
// Same but for a precise actor.
uint32 CalculateMagicalDamage(BattleActor *attacker, BattleActor *target_actor, vt_global::GLOBAL_ELEMENTAL element, float std_dev);

/** \brief Determines the amount of damage caused with a magical attack, utilizing an addition modifier
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param element The element used when attacking using magic.
*** \param add_atk An additional amount to add to the magical damage dealt
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
***
*** The add_atk argument may be positive or negative. Large negative values can skew the damage calculation
*** and cause the damage dealt to drop to zero, so be cautious when setting this argument to a negative value.
**/
uint32 CalculateMagicalDamageAdder(BattleActor *attacker, BattleTarget *target, vt_global::GLOBAL_ELEMENTAL element, int32 add_atk);
// Same but for a precise actor.
uint32 CalculateMagicalDamageAdder(BattleActor *attacker, BattleActor *target_actor, vt_global::GLOBAL_ELEMENTAL element, int32 add_atk);

/** \brief Determines the amount of damage caused with a magical attack, utilizing an addition modifier
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param element The element used when attacking using magic.
*** \param add_atk An additional amount to add to the magical damage dealt
*** \param std_dev The standard deviation to use in the gaussian distribution, where "0.075f" would represent 7.5% standard deviation
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
**/
uint32 CalculateMagicalDamageAdder(BattleActor *attacker, BattleTarget *target,
                                   vt_global::GLOBAL_ELEMENTAL element, int32 add_atk, float std_dev);
// Same but for a precise actor.
uint32 CalculateMagicalDamageAdder(BattleActor *attacker, BattleActor *target_actor,
                                   vt_global::GLOBAL_ELEMENTAL element, int32 add_atk, float std_dev);

/** \brief Determines the amount of damage caused with a magical attack, utilizing a multiplication modifier
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param element The element used when attacking using magic.
*** \param mul_atk An additional amount to be multiplied to the magical damage dealt
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
***
*** This function operates the same as the CalculateMagicalDamageAdder(...) functions with the exception that
*** its float argument is used as a multipler in the damage calculation instead of an integer addition modifier.
*** So for instance if the user wants the magical damage to decrease by 20% the value of mul_atk would
*** be 0.8f. If a negative multiplier value is passed to this function, its absoute value will be used and
*** a warning will be printed.
**/
uint32 CalculateMagicalDamageMultiplier(BattleActor *attacker, BattleTarget *target,
                                        vt_global::GLOBAL_ELEMENTAL element, float mul_atk);
// Same but for a precise actor.
uint32 CalculateMagicalDamageMultiplier(BattleActor *attacker, BattleActor *target_actor,
                                        vt_global::GLOBAL_ELEMENTAL element, float mul_atk);

/** \brief Determines the amount of damage caused with a magical attack, utilizing a multiplication modifier
*** \param attacker A pointer to the attacker who is causing the damage
*** \param target A pointer to the target that will be receiving the damage
*** \param element The element used when attacking using magic.
*** \param mul_atk A modifier to be multiplied to the magical damage dealt
*** \param std_dev The standard deviation to use in the gaussian distribution, where "0.075f" would represent 7.5% standard deviation
*** \return The amount of damage dealt, which will always be a non-zero value unless there was an error
***
*** This function signature allows the additional option of setting the standard deviation in the gaussian random value calculation.
**/
uint32 CalculateMagicalDamageMultiplier(BattleActor *attacker, BattleTarget *target,
                                        vt_global::GLOBAL_ELEMENTAL element, float mul_atk, float std_dev);
// Same but for a precise actor.
uint32 CalculateMagicalDamageMultiplier(BattleActor *attacker, BattleActor *target_actor,
                                        vt_global::GLOBAL_ELEMENTAL element, float mul_atk, float std_dev);
//@}

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

    //! \brief Resets all class members, invalidating the target
    void InvalidateTarget();

    //! \brief Reinit the attack point. Used when changing target.
    void ReinitAttackPoint() {
        _point = 0;
    }

    /** \brief Used to set the initial target
    *** \param user A pointer to the actor which will use the target
    *** \param type The type of target to set
    ***
    *** If the function fails to find an initial target, the target type will be set to
    *** GLOBAL_TARGET_INVALID. The initial attack point is always the first available point on the
    *** actor (index 0). The initial actor will always be the first valid actor in their respective
    *** party (index 0).
    **/
    bool SetInitialTarget(BattleActor *user, vt_global::GLOBAL_TARGET type);

    /** \brief Sets the target to a specific attack point on an actor
    *** \param type The type of target to set, must be one of the point type targets
    *** \param attack_point An integer index into the actor's attack points
    *** \param actor The actor to set for the target (default value == NULL)
    *** A NULL actor simply means that the class should continue pointing to the current actor.
    *** This is useful for cycling through the available attack points on an actor. Note that if the
    *** actor argument is NULL, the _actor member should not be NULL when the function is called.
    *** If both are NULL, calling this method will perform no changes.
    **/
    bool SetPointTarget(vt_global::GLOBAL_TARGET type, uint32 attack_point, BattleActor *actor = NULL);

    /** \brief Sets the target to an actor
    *** \param type The type of target to set, must be one of the actor type targets
    *** \param actor A pointer to the actor to set for the target
    **/
    bool SetActorTarget(vt_global::GLOBAL_TARGET type, BattleActor *actor);

    /** \brief Sets the target to a party
    *** \param type The type of target to set, must be one of the party type targets
    *** \param actor A pointer to the party to set for the target
    **/
    bool SetPartyTarget(vt_global::GLOBAL_TARGET type, std::deque<BattleActor *>* party);

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
    *** \Note, we can't select dead enemies since it's not part of the enemy logic.
    ***
    *** \param permit_dead_targets Tells whether dead or dying target are valid.
    **/
    bool IsValid(bool permit_dead_targets = false);

    /** \brief Changes the target attack point to reference the next available attack point target
    *** \param user A pointer to the actor which is using this target
    *** \param direction Tells the method to look either forward or backward (true/false) for the next target
    *** \param valid_criteria When true the method will only select targets determined to be valid by IsValid()
    *** \param permit_dead_targets Tells whether dead or dying target are valid.
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
    bool SelectNextPoint(BattleActor *user, bool direction = true, bool valid_criteria = true,
                         bool permit_dead_targets = false);

    /** \brief Changes the target actor to reference the next available actor
    *** \param user A pointer to the actor which is using this target
    *** \param direction Tells the method to look either forward or backward (true/false) for the next target
    *** \param valid_criteria When true the method will only select actors determined to be valid by IsValid()
    *** \param permit_dead_targets Tells whether dead or dying target are valid.
    *** \return True if the _actor member was changed, false if it was not
    ***
    *** This method should only be called when the target type is not one of the party types.
    **/
    bool SelectNextActor(BattleActor *user, bool direction = true, bool valid_criteria = true,
                         bool permit_dead_targets = false);

    /** \brief Retrieves a pointer to the actor of a party at the specified index
    *** \param index The location in the party container of the actor to retrieves
    *** \return NULL if the target is not a party or the index is invalid. Otherwise a pointer to the actor specified
    ***
    *** The primary purpose for the existence of this function is for Lua to be able to access all the actors within a
    *** party target. The GetParty() method can not be used by Lua as Lua does not understand that container format
    *** (std::deque<BattleActor*>). To retrieve each actor, Lua code starts at index 0 and makes repeated calls to this
    *** function while incrementing the index by 1 until it returns a NULL value.
    **/
    BattleActor *GetPartyActor(uint32 index);

    /** \brief Returns the name of the target
    ***
    *** Party type targets will return "All Allies" or "All Enemies". Actor type targets return the name of the character or
    *** enemy, for example "Claudius" or "Red Spider" . Attack point type targets return the name of the character or enemy
    *** seperated by a hyphen and followed by the point name, for example "Bat — Wing". Invalid targets will return "[Invalid Target]".
    **/
    vt_utils::ustring GetName();

    //! \name Class member accessor methods
    //@{
    vt_global::GLOBAL_TARGET GetType() const {
        return _type;
    }

    uint32 GetPoint() const {
        return _point;
    }

    BattleActor *GetActor() const {
        return _actor;
    }

    std::deque<BattleActor *>* GetParty() const {
        return _party;
    }
    //@}

private:
    //! \brief The type of target this object represents (attack point, actor, or party)
    vt_global::GLOBAL_TARGET _type;

    //! \brief The attack point to target, as an index to the proper point on the _actor
    uint32 _point;

    //! \brief The actor to target
    BattleActor *_actor;

    //! \brief The party to target
    std::deque<BattleActor *>* _party;
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
***
*** The proper way to use this class is to call the following methods for the following
*** situations.
***
*** - DecrementAvailableCount(): call when an actor has selected to use an item
*** - IncrementAvaiableAcount(): call when an actor does not use an item that it selected
*** - DecrementCount(): call when the item is actually used
***
*** \note Do not call the IncrementCount(), DecrementCount(), or SetCount() methods on the GlobalItem
*** pointer. This will circumvent the ability of this class to keep an accurate and correct available
*** count. Instead, use the IncrementCount() and DecrementCount() methods of this BattleItem class
*** directly.
*** ***************************************************************************/
class BattleItem
{
public:
    //! \param item A pointer to the item to represent. Should be a non-NULL value.
    BattleItem(vt_global::GlobalItem item);

    ~BattleItem();

    //! \brief Class member accessor methods
    //@{
    vt_global::GlobalItem &GetItem() {
        return _item;
    }

    uint32 GetBattleCount() const {
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
