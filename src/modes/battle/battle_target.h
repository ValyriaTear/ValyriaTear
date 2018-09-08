///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_TARGET_HEADER__
#define __BATTLE_TARGET_HEADER__

#include "modes/battle/battle_objects/battle_actor.h"

namespace vt_battle
{

namespace private_battle
{

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
    *** \param target A pointer to the actor which will use the target or nullptr. When nullptr, the first valid target depending on type is used.
    *** \param attack_point An integer index into the actor's attack points
    ***
    *** If the function fails to find an initial target, the target type will be set to
    *** GLOBAL_TARGET_INVALID. The initial attack point is always the first available point on the
    *** target (index 0). The initial target will always be the first valid actor in its party (index 0).
    **/
    bool SetTarget(BattleActor* attacker,
                   vt_global::GLOBAL_TARGET type,
                   BattleActor* target = nullptr,
                   uint32_t attack_point = 0);

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
    *** \return nullptr if the target is not a party or the index is invalid. Otherwise a pointer to the actor specified
    ***
    *** The primary purpose for the existence of this function is for Lua to be able to access all the actors within a
    *** party target. The GetParty() method can not be used by Lua as Lua does not understand that container format
    *** (std::deque<BattleActor*>). To retrieve each actor, Lua code starts at index 0 and makes repeated calls to this
    *** function while incrementing the index by 1 until it returns a nullptr value.
    **/
    BattleActor* GetPartyActor(uint32_t index);

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

    inline uint32_t GetAttackPoint() const {
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
    uint32_t _attack_point;

    //! \brief The current actor to target
    BattleActor* _actor_target;

    //! \brief The current party to target
    std::deque<BattleActor *> _party_target;
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_TARGET_HEADER__
