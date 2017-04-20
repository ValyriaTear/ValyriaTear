///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_states/menu_skill_state.h"

#include "modes/menu/menu_mode.h"

namespace vt_menu {

namespace private_menu {

void SkillsState::_ActiveWindowUpdate()
{
    _menu_mode->_skills_window.Update();
}

bool SkillsState::_IsActive()
{
    return _menu_mode->_skills_window.IsActive();
}

void SkillsState::Reset()
{
    // Setup the option box
    SetupOptionBoxCommonSettings(&_options);
    _options.SetDimensions(415.0f, 50.0f, SKILLS_OPTIONS_SIZE, 1, SKILLS_OPTIONS_SIZE, 1);

    // Generate the strings
    std::vector<vt_utils::ustring> options;
    options.push_back(vt_system::UTranslate("Use"));
    options.push_back(vt_system::UTranslate("Back"));

    // Add strings and set default selection.
    _options.SetOptions(options);
    _options.SetSelection(SKILLS_OPTIONS_USE);
}

AbstractMenuState* SkillsState::GetTransitionState(uint32_t selection)
{
    switch(selection)
    {
        case SKILLS_OPTIONS_BACK:
            return &(_menu_mode->_main_menu_state);
        case SKILLS_OPTIONS_USE:
            _menu_mode->_skills_window.Activate(true);
            break;
        default:
            break;

    }
    return nullptr;
}

void SkillsState::_OnDrawMainWindow()
{
    _DrawBottomMenu();
    _menu_mode->_skills_window.Draw();
}

void SkillsState::_DrawBottomMenu()
{
    _menu_mode->_bottom_window.Draw();

    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_TOP, 0);
    vt_video::VideoManager->Move(90, 580);
    _menu_mode->_skills_window._skill_icon.Draw();

    _menu_mode->_skills_window._description.Draw();
}

} // namespace private_menu

} // namespace vt_menu
