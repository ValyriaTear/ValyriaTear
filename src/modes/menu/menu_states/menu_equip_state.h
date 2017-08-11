///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_EQUIP_STATE_HEADER__
#define __MENU_EQUIP_STATE_HEADER__

#include "modes/menu/menu_states/menu_abstract_state.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

//! \brief The different bottom window equipment views
enum EQUIP_VIEW {
    EQUIP_VIEW_NONE        = -1,
    //! Show the current character stats using the given object type.
    EQUIP_VIEW_CHAR        = 0,
    //! Show the diffs of characters stats against the given object.
    EQUIP_VIEW_EQUIPPING   = 1,
    //! Show the diffs of characters stats against nothing using the given object type.
    EQUIP_VIEW_UNEQUIPPING = 2
};

/**
*** \brief Equip / Remove state. Allows players to modify the character equipment
**/
class EquipState : virtual public AbstractMenuState {
public:
    enum EQUIP_CATEGORY {
        EQUIP_OPTIONS_BACK,
        EQUIP_OPTIONS_SIZE
    };

    //! Equip state constructor
    explicit EquipState(MenuMode* menu_mode):
        AbstractMenuState("Equip State", menu_mode)
    {}

    ~EquipState()
    {}

    void Reset();

    AbstractMenuState* GetTransitionState(uint32_t /*selection*/)
    { return nullptr; }

protected:
    void _DrawBottomMenu();
    void _OnDrawMainWindow();
    void _ActiveWindowUpdate();
    bool _IsActive();
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_EQUIP_STATE_HEADER__
