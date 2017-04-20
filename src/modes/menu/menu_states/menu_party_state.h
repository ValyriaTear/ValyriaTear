///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_PARTY_STATE_HEADER__
#define __MENU_PARTY_STATE_HEADER__

#include "modes/menu/menu_states/menu_abstract_state.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

/**
*** \brief Party state. shows the user the character status information. Allows
*** switching of formation
**/
class PartyState : virtual public AbstractMenuState {
public:
    enum PARTY_CATEGORY {
        PARTY_OPTIONS_VIEW_REORDER,
        PARTY_OPTIONS_BATTLE_FORMATION,
        PARTY_OPTIONS_BACK,
        PARTY_OPTIONS_SIZE
    };

    //! \brief Party state constructor
    explicit PartyState(MenuMode* menu_mode):
        AbstractMenuState("Party State", menu_mode)
    {}

    ~PartyState()
    {}

    void Reset();
    AbstractMenuState* GetTransitionState(uint32_t selection);
protected:
    void _DrawBottomMenu();
    void _OnDrawMainWindow();
    void _OnDrawSideWindow();
    void _ActiveWindowUpdate();
    bool _IsActive();

};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_PARTY_STATE_HEADER__
