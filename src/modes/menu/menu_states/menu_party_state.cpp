///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_states/menu_party_state.h"

#include "modes/menu/menu_mode.h"

namespace vt_menu {

namespace private_menu {

void PartyState::_ActiveWindowUpdate()
{
    switch (_options.GetSelection()) {
    default:
    case PARTY_OPTIONS_BACK:
    case PARTY_OPTIONS_VIEW_REORDER:
        _menu_mode->_party_window.Update();
        break;
    case PARTY_OPTIONS_BATTLE_FORMATION:
        _menu_mode->_battle_formation_window.Update();
        break;
    }
    _menu_mode->UpdateTimeAndDrunes();
}

bool PartyState::_IsActive()
{
    switch (_options.GetSelection()) {
    default:
    case PARTY_OPTIONS_BACK:
    case PARTY_OPTIONS_VIEW_REORDER:
        return static_cast<bool>(_menu_mode->_party_window.GetActiveState());
        break;
    case PARTY_OPTIONS_BATTLE_FORMATION:
        return _menu_mode->_battle_formation_window.GetActiveState();
        break;
    }

    return false;
}

void PartyState::Reset()
{
    // Setup the status option box
    SetupOptionBoxCommonSettings(&_options);
    _options.SetDimensions(700.0f, 50.0f, PARTY_OPTIONS_SIZE, 1, PARTY_OPTIONS_SIZE, 1);

    // Generate the strings
    std::vector<vt_utils::ustring> options;
    options.push_back(vt_system::UTranslate("View/Reorder"));
    options.push_back(vt_system::UTranslate("Battle Formation"));
    options.push_back(vt_system::UTranslate("Back"));

    // Add strings and set default selection.
    _options.SetOptions(options);
    _options.SetSelection(PARTY_OPTIONS_VIEW_REORDER);

    // FIXME: Re-enable this once the battle formation is taken in account in battles.
    _options.SetSkipDisabled(true);
    _options.EnableOption(1, false); // Disable 'Battle Formation'.

    _menu_mode->_help_information.SetDisplayText(
        vt_system::UTranslate("View character Information.\nSelect a character to change formation."));

    // Update the current character status at reset, in case of equipment change.
    _menu_mode->_party_window.UpdateStatus();
}

AbstractMenuState* PartyState::GetTransitionState(uint32_t selection)
{
    switch(selection)
    {
        case PARTY_OPTIONS_BACK:
            return &(_menu_mode->_main_menu_state);
        case PARTY_OPTIONS_VIEW_REORDER:
            _menu_mode->_party_window.Activate(true);
            break;
        case PARTY_OPTIONS_BATTLE_FORMATION:
            _menu_mode->_battle_formation_window.Activate(true);
            break;
        default:
            break;
    };
    return nullptr;
}

void PartyState::_DrawBottomMenu()
{
    _menu_mode->_bottom_window.Draw();

    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_BOTTOM, 0);
    vt_video::VideoManager->Move(150, 580);

    // show a helpful message
    if(!_IsActive())
        _menu_mode->_help_information.Draw();
}

void PartyState::_OnDrawMainWindow()
{
    _DrawBottomMenu();
    switch (_options.GetSelection()) {
    default:
    case PARTY_OPTIONS_BACK:
    case PARTY_OPTIONS_VIEW_REORDER:
        _menu_mode->_party_window.Draw();
        break;
    case PARTY_OPTIONS_BATTLE_FORMATION:
        _menu_mode->_battle_formation_window.Draw();
        break;
    }
}

void PartyState::_OnDrawSideWindow()
{
    switch (_options.GetSelection()) {
    default:
    case PARTY_OPTIONS_BACK:
    case PARTY_OPTIONS_VIEW_REORDER:
        AbstractMenuState::_OnDrawSideWindow();
        break;
    case PARTY_OPTIONS_BATTLE_FORMATION:
        break;
    }
}

} // namespace private_menu

} // namespace vt_menu
