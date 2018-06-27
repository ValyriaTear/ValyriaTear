///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_states/menu_skill_state.h"

#include "modes/menu/menu_mode.h"

namespace vt_menu {

namespace private_menu {

void SkillsState::_ActiveWindowUpdate()
{
    switch (_current_category) {
        default:
            _menu_mode->_skills_window.Update();
            break;
        case SKILLS_OPTIONS_SKILL_GRAPH:
            _menu_mode->_skilltree_window.Update();
        break;
    }
}

bool SkillsState::_IsActive()
{
    // Make the skill graph menu be the sole menu available in end battle mode
    if (_menu_mode->_end_battle_mode && !_menu_mode->_skilltree_window.IsActive()) {
        vt_mode_manager::ModeManager->Pop();
    }

    if (_current_category == SKILLS_OPTIONS_SKILL_GRAPH) {
        return _menu_mode->_skilltree_window.IsActive();
    }
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
    options.push_back(vt_system::UTranslate("Improve"));
    options.push_back(vt_system::UTranslate("Back"));

    // Add strings and set default selection.
    _options.SetOptions(options);
    _current_category = SKILLS_OPTIONS_USE;
    _options.SetSelection(_current_category);
}

AbstractMenuState* SkillsState::GetTransitionState(uint32_t selection)
{
    _current_category = static_cast<SKILLS_CATEGORY>(selection);
    switch(_current_category)
    {
        case SKILLS_OPTIONS_BACK:
            return &(_menu_mode->_main_menu_state);
        case SKILLS_OPTIONS_SKILL_GRAPH:
            _menu_mode->_skilltree_window.SetActive(true);
            break;
        case SKILLS_OPTIONS_USE:
            _menu_mode->_skills_window.Activate(true);
            break;
        default:
            break;
    }
    return nullptr;
}

void SkillsState::_OnDrawSideWindow()
{
    if ((_current_category == SKILLS_OPTIONS_SKILL_GRAPH
            && _menu_mode->_skilltree_window.GetSkillGraphState() != SKILLGRAPH_STATE_LIST)
            || _current_category != SKILLS_OPTIONS_SKILL_GRAPH) {
        _menu_mode->_character_window0.Draw();
        _menu_mode->_character_window1.Draw();
        _menu_mode->_character_window2.Draw();
        _menu_mode->_character_window3.Draw();
    }
}

void SkillsState::_OnDrawMainWindow()
{
    _DrawBottomMenu();
    switch (_current_category) {
        default:
            _menu_mode->_skills_window.Draw();
            break;
        case SKILLS_OPTIONS_SKILL_GRAPH:
            _menu_mode->_skilltree_window.Draw();
        break;
    }
}

void SkillsState::_DrawBottomMenu()
{
    _menu_mode->_bottom_window.Draw();

    switch (_current_category) {
        default:
            vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_TOP, 0);
            vt_video::VideoManager->Move(90, 580);
            _menu_mode->_skills_window._skill_icon.Draw();
            _menu_mode->_skills_window._description.Draw();
            break;
        case SKILLS_OPTIONS_SKILL_GRAPH:
            _menu_mode->_skilltree_window.DrawBottomWindow();
            break;
    }
}

} // namespace private_menu

} // namespace vt_menu
