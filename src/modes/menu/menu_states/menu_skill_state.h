///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_SKILL_STATE_HEADER__
#define __MENU_SKILL_STATE_HEADER__

#include "modes/menu/menu_states/menu_abstract_state.h"

namespace vt_menu {

class MenuMode;

namespace private_menu
{

/**
*** \brief Skills state. Allows user to view their skills and use them
**/
class SkillsState : virtual public AbstractMenuState {
public:
    enum SKILLS_CATEGORY {
        SKILLS_OPTIONS_USE,
        SKILLS_OPTIONS_SKILL_GRAPH,
        SKILLS_OPTIONS_BACK,
        SKILLS_OPTIONS_SIZE

    };

    //! \brief Skills state constructor
    explicit SkillsState(MenuMode* menu_mode):
        AbstractMenuState("Skills State", menu_mode),
        _current_category(SKILLS_OPTIONS_USE)
    {}

    ~SkillsState(){}
    void Reset();
    AbstractMenuState* GetTransitionState(uint32_t selection);

protected:
    void _DrawBottomMenu();
    void _OnDrawSideWindow();
    void _OnDrawMainWindow();
    void _ActiveWindowUpdate();
    bool _IsActive();

private:
    SKILLS_CATEGORY _current_category;
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_SKILL_STATE_HEADER__
