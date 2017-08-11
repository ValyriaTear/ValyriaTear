///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_states/menu_inventory_state.h"

#include "modes/menu/menu_mode.h"

#include "utils/ustring.h"

namespace vt_menu {

namespace private_menu {

void InventoryState::Reset()
{
    // Setup the option box
    SetupOptionBoxCommonSettings(&_options);
    _options.SetDimensions(745.0f, 50.0f, INV_OPTIONS_SIZE, 1, INV_OPTIONS_SIZE, 1);

    // Generate the strings
    std::vector<vt_utils::ustring> options;
    options.push_back(vt_system::UTranslate("Items"));
    options.push_back(vt_system::UTranslate("Equip"));
    options.push_back(vt_system::UTranslate("Remove"));
    options.push_back(vt_system::UTranslate("Back"));

    // Add strings and set default selection.
    _options.SetOptions(options);
    _options.SetSelection(INV_OPTIONS_USE);
}

AbstractMenuState* InventoryState::GetTransitionState(uint32_t selection)
{
    switch(selection)
    {
        case INV_OPTIONS_EQUIP:
        case INV_OPTIONS_REMOVE:
            return &(_menu_mode->_equip_state);
        case INV_OPTIONS_BACK:
            return &(_menu_mode->_main_menu_state);
        case INV_OPTIONS_USE:
            _menu_mode->_inventory_window.Activate(true);
            break;
        default:
            break;
    };
    return nullptr;
}

void InventoryState::_ActiveWindowUpdate()
{
    _menu_mode->_inventory_window.Update();
}

bool InventoryState::_IsActive()
{
    return _menu_mode->_inventory_window.IsActive();
}

void InventoryState::_OnDrawMainWindow()
{
    uint32_t draw_window = _options.GetSelection();
    // Inventory state has multiple state types to draw, including the Equip transition state.
    switch(draw_window)
    {
        case INV_OPTIONS_EQUIP:
            _menu_mode->_equip_window.Draw();
            break;
        case INV_OPTIONS_REMOVE:
            _menu_mode->_equip_window.Draw();
            break;
        case INV_OPTIONS_USE:
        case INV_OPTIONS_BACK:
        default:
            if (!_IsActive())
                AbstractMenuState::_DrawBottomMenu();
            _menu_mode->_inventory_window.Draw();
            break;
    }
}

} // namespace private_menu

} // namespace vt_menu
