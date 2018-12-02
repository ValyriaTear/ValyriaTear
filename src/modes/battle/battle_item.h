///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_ITEM_HEADER__
#define __BATTLE_ITEM_HEADER__

#include "modes/battle/battle_target.h"

#include "common/global/objects/global_item.h"

namespace vt_battle
{

namespace private_battle
{

class BattleActor;

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
    //! \param item A pointer to the item to represent.
    BattleItem(const vt_global::GlobalItem& item);
    ~BattleItem() {}

    //! \brief Class member accessor methods
    //@{
    const vt_global::GlobalItem& GetGlobalItem() {
        return _item;
    }

    inline uint32_t GetBattleCount() const {
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
    uint32_t GetInventoryCount() const {
        return _item.GetCount();
    }

    /** \brief A wrapper function that retrieves the target type of the item
    *** \note Calling this function is equivalent to calling GetItem().GetTargetType()
    **/
    vt_global::GLOBAL_TARGET GetTargetType() const {
        return _item.GetTargetType();
    }

    //! \brief A wrapper function retrieving the warmup time needed to use the item in battles.
    uint32_t GetWarmUpTime() const {
        return _item.GetWarmUpTime();
    }

    //! \brief A wrapper function retrieving the cooldown time needed after the item use in battles.
    uint32_t GetCoolDownTime() const {
        return _item.GetCoolDownTime();
    }

    //! \brief Execute the corresponding item Battle function
    bool ExecuteBattleFunction(BattleActor* battle_actor,
                               BattleTarget target);

private:
    //! \brief The item that this class represents.
    const vt_global::GlobalItem& _item;

    //! \brief The number of instances of this item that are available to be selected to be used
    uint32_t _battle_count;
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_ITEM_HEADER__
