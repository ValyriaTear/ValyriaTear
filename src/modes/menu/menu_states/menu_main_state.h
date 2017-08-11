///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_MAIN_STATE_HEADER__
#define __MENU_MAIN_STATE_HEADER__

#include "modes/menu/menu_states/menu_abstract_state.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

/**
*** \brief Main Menu state. This is the entry point into the menu
***
*** The main menu has no "active" state
**/
class MainMenuState : virtual public AbstractMenuState {
public:
    //! \brief possible transition states from the main menu
    enum MAIN_CATEGORY {
        MAIN_OPTIONS_INVENTORY,
        MAIN_OPTIONS_SKILLS,
        MAIN_OPTIONS_PARTY,
        MAIN_OPTIONS_QUESTS,
        MAIN_OPTIONS_WORLDMAP,
        MAIN_OPTIONS_SIZE
    };

    //! \brief Main Menu State constructor
    explicit MainMenuState(MenuMode* menu_mode);

    ~MainMenuState()
    {};

    AbstractMenuState* GetTransitionState(uint32_t selection);

protected:
    void _OnDrawMainWindow();
    void _OnDrawSideWindow();
    void _OnUpdateState();
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_MAIN_STATE_HEADER__
