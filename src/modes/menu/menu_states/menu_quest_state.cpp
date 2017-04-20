///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_states/menu_quest_state.h"

#include "modes/menu/menu_mode.h"

namespace vt_menu {

namespace private_menu {

void QuestState::Reset()
{
    // Clear the bottom info.
    _menu_mode->_quest_window.ClearBottom();

    // Automatically go into the quest list window.
    _menu_mode->_quest_list_window._active_box = true;
}

void QuestState::_ActiveWindowUpdate()
{
    _menu_mode->_quest_window.Update();
    _menu_mode->_quest_list_window.Update();
    if (!_IsActive())
        _OnCancel();
}

bool QuestState::_IsActive()
{
    return _menu_mode->_quest_list_window.IsActive();
}

void QuestState::_OnDrawMainWindow()
{
    _DrawBottomMenu();
    _menu_mode->_quest_window.Draw();

}

void QuestState::_OnDrawSideWindow()
{
    _menu_mode->_quest_list_window.Draw();

}

void QuestState::_DrawBottomMenu()
{
    _menu_mode->_bottom_window.Draw();
    if(_IsActive())
        _menu_mode->_quest_window.DrawBottom();
}

} // namespace private_menu

} // namespace vt_menu
