///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_INVENTORY_STATE_HEADER__
#define __MENU_INVENTORY_STATE_HEADER__

#include "modes/menu/menu_states/menu_abstract_state.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

/**
*** \brief Inventory State. Handles user interactions for item use and equiping
**/
class InventoryState : virtual public AbstractMenuState {
public:
    //! \brief the possible inventory options
    enum INVENTORY_CATEGORY {
        INV_OPTIONS_USE,
        INV_OPTIONS_EQUIP,
        INV_OPTIONS_REMOVE,
        INV_OPTIONS_BACK,
        INV_OPTIONS_SIZE
    };

    //! \brief InventoryState state constructor
    explicit InventoryState(MenuMode* menu_mode):
        AbstractMenuState("Inventory State", menu_mode)
    {}

    ~InventoryState()
    {}

    void Reset();

    AbstractMenuState *GetTransitionState(uint32_t selection);

protected:
    void _OnDrawMainWindow();

    void _ActiveWindowUpdate();

    bool _IsActive();
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_INVENTORY_STATE_HEADER__
