///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_states/menu_main_state.h"

#include "modes/menu/menu_mode.h"

#include "utils/ustring.h"

namespace vt_menu {

namespace private_menu {

//! Menu messages
static vt_utils::ustring world_map_window_message;
static vt_utils::ustring quest_view_message;

MainMenuState::MainMenuState(MenuMode *menu_mode):
    AbstractMenuState("Main Menu", menu_mode)
{
    // Setup the main options box
    SetupOptionBoxCommonSettings(&_options);
    _options.SetDimensions(745.0f, 50.0f, MAIN_OPTIONS_SIZE, 1, MAIN_OPTIONS_SIZE, 1);

    // Generate the strings
    std::vector<vt_utils::ustring> options;
    options.push_back(vt_system::UTranslate("Inventory")); // 0
    options.push_back(vt_system::UTranslate("Skills")); // 1
    options.push_back(vt_system::UTranslate("Party")); // 2
    options.push_back(vt_system::UTranslate("Quests")); // 3
    options.push_back(vt_system::UTranslate("Map")); // 4

    // Add strings and set default selection.
    _options.SetOptions(options);
    _options.SetSelection(MAIN_OPTIONS_INVENTORY);
    _options.SetSkipDisabled(true);

    // Deactivate menus with empty content
    if (vt_global::GlobalManager->GetGameQuests().GetActiveQuestIds().empty())
        _options.EnableOption(3, false);

    if (vt_global::GlobalManager->GetWorldMapData().GetWorldMapImageFilename().empty())
        _options.EnableOption(4, false);

    // We set them here in case the language has changed since the game start.
    world_map_window_message = vt_system::UTranslate("Select to view current world map.\n"
                               "Use left / right to cycle through locations.\nPress 'cancel' to return");
    quest_view_message = vt_system::UTranslate("Select to view Quest Log.");
}

AbstractMenuState* MainMenuState::GetTransitionState(uint32_t selection)
{
    switch(selection)
    {
        case MAIN_OPTIONS_INVENTORY:
            return &(_menu_mode->_inventory_state);
            break;
        case MAIN_OPTIONS_SKILLS:
            return &(_menu_mode->_skills_state);
            break;
        case MAIN_OPTIONS_PARTY:
            return &(_menu_mode->_party_state);
            break;
        case MAIN_OPTIONS_QUESTS:
            return &(_menu_mode->_quests_state);
            break;
        case MAIN_OPTIONS_WORLDMAP:
            return &(_menu_mode->_world_map_state);
            break;
        default:
            PRINT_ERROR << "MENU ERROR: Invalid option in " << GetStateName() << "::GetTransitionState" << std::endl;
            break;
    }
    return nullptr;
}

void MainMenuState::_OnUpdateState()
{
    uint32_t draw_window = _options.GetSelection();
    switch(draw_window) {
    case MAIN_OPTIONS_WORLDMAP:
    {
        _menu_mode->_world_map_window.Update();
        break;
    }
    default:
        break;
    };
}

void MainMenuState::_OnDrawMainWindow()
{
    uint32_t draw_window = _options.GetSelection();

    // Draw the chosen window
    switch(draw_window) {
        case MAIN_OPTIONS_INVENTORY: {
            AbstractMenuState::_DrawBottomMenu();
            _menu_mode->_inventory_window.Draw();
            break;
        }
        case MAIN_OPTIONS_SKILLS: {
            AbstractMenuState::_DrawBottomMenu();
            _menu_mode->_skills_window.Draw();
            break;
        }
        case MAIN_OPTIONS_QUESTS: {
            _menu_mode->_bottom_window.Draw();
            _menu_mode->_help_information.SetDisplayText(quest_view_message);
            _menu_mode->_help_information.Draw();
            _menu_mode->_quest_window.Draw();
            break;
        }
        case MAIN_OPTIONS_WORLDMAP:
        {
            _menu_mode->_bottom_window.Draw();
            _menu_mode->_help_information.SetDisplayText(world_map_window_message);
            _menu_mode->_help_information.Draw();
            // Actual drawing of the bottom window will occur upon transition
            // to the world map state
            _menu_mode->_world_map_window.Draw();
            break;
        }
        case MAIN_OPTIONS_PARTY:
        default: {
            AbstractMenuState::_DrawBottomMenu();
            _menu_mode->_party_window.Draw();
            break;
        }
    } // switch draw_window
}

void MainMenuState::_OnDrawSideWindow()
{
    switch (_options.GetSelection()) {
    case MAIN_OPTIONS_QUESTS:
        _menu_mode->_quest_list_window.Draw();
        break;
    case MAIN_OPTIONS_WORLDMAP:
        break;
    case MAIN_OPTIONS_PARTY:
        if (!_menu_mode->_battle_formation_window.GetActiveState())
            AbstractMenuState::_OnDrawSideWindow();
        break;
    default:
        AbstractMenuState::_OnDrawSideWindow();
        break;
    }
}

} // namespace private_menu

} // namespace vt_menu
