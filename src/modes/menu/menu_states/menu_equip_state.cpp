///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_states/menu_equip_state.h"

#include "modes/menu/menu_mode.h"

namespace vt_menu {

namespace private_menu {

void EquipState::Reset()
{
    // equip state must handle removal as well as equip. we check to see where we transitioned from...
    if(_from_state == &_menu_mode->_inventory_state)
    {
        // if its from the inventory EQUIP selection, activate the window with the equip flag set to true
        if(_from_state->GetOptions()->GetSelection() == InventoryState::INV_OPTIONS_EQUIP)
            _menu_mode->_equip_window.Activate(true, true);
        // otherwise, it was from the REMOVE selection, activate the window with the equip flag set to false
        else
            _menu_mode->_equip_window.Activate(true, false);
    }
}

void EquipState::_ActiveWindowUpdate()
{
    _menu_mode->_equip_window.Update();
    if(!_IsActive())
        _OnCancel();
}

bool EquipState::_IsActive()
{
    return _menu_mode->_equip_window.IsActive();
}

void EquipState::_OnDrawMainWindow()
{
    _DrawBottomMenu();
    _menu_mode->_equip_window.Draw();
}

void EquipState::_DrawBottomMenu()
{
    _menu_mode->_bottom_window.Draw();
}

} // namespace private_menu

} // namespace vt_menu
