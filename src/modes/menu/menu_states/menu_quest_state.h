///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_QUEST_STATE_HEADER__
#define __MENU_QUEST_STATE_HEADER__

#include "modes/menu/menu_states/menu_abstract_state.h"

namespace vt_menu {

class MenuMode;

namespace private_menu
{

/**
*** \brief Quest Log state. Allows users to view their active and finished quests
**/
class QuestState : virtual public AbstractMenuState {
public:
    enum QUEST_CATEGORY {
            QUEST_OPTIONS_VIEW,
            QUEST_OPTIONS_BACK,
            QUEST_OPTIONS_SIZE
    };

    //! \brief Quest state constructor
    explicit QuestState(MenuMode* menu_mode):
        AbstractMenuState("Quest State", menu_mode)
    {}

    ~QuestState()
    {}

    void Reset();

    AbstractMenuState *GetTransitionState(uint32_t /*selection*/)
    { return nullptr; }
protected:
    void _OnDrawMainWindow();
    void _OnDrawSideWindow();
    void _DrawBottomMenu();
    void _ActiveWindowUpdate();
    bool _IsActive();
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_QUEST_STATE_HEADER__
