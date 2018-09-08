////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ITEM_ACTION_HEADER__
#define __ITEM_ACTION_HEADER__

#include "battle_action.h"
#include "modes/battle/battle_item.h"

namespace vt_battle
{

namespace private_battle
{

/** ****************************************************************************
*** \brief A battle action which involves the use of an item
***
*** This class invokes the usage of a GlobalItem. The item's count is decremented
*** as soon as the action goes into the FIFO queue. After the action is executed,
*** the item is removed if its count has become zero. If the action is removed
*** from the queue before it is executed (because the source actor perished, or
*** the battle ended, or other circumstances), then the item's count is
*** incremented back to its original value since it was not used.
*** ***************************************************************************/
class ItemAction : public BattleAction
{
public:
    ItemAction(BattleActor* source, BattleTarget target, const std::shared_ptr<BattleItem>& item);

    virtual ~ItemAction();

    bool IsItemAction() const {
        return true;
    }

    bool Initialize();

    bool Update();

    bool Execute();

    ///! \brief Cancel a waiting action, putting back the item in available battle items.
    void Cancel();

    vt_utils::ustring GetName() const;

    std::string GetIconFilename() const;

    bool ShouldShowSkillNotice() const {
        return true;
    }

    uint32_t GetWarmUpTime() const;

    uint32_t GetCoolDownTime() const;

    std::string GetWarmupActionName() const {
        return "idle";
    }

    std::string GetActionName() const {
        return "item";
    }

    std::shared_ptr<BattleItem> GetBattleItem() {
        return _battle_item;
    }

private:
    //! \brief Pointer to the item attached to this script
    const std::shared_ptr<BattleItem> _battle_item;

    //! \brief Tells whether the action has already been canceled.
    bool _action_canceled;

    //! \brief Initialize (Calling #Initialize) a scripted battle animation when one is existing.
    void _InitAnimationScript();
};

} // namespace private_battle

} // namespace vt_battle

#endif // __ITEM_ACTION_HEADER__
