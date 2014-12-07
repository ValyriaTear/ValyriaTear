///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    menu.cpp
*** \author  Daniel Steuernol steu@allacrost.org
*** \author  Andy Gardner chopperdave@allacrost.org
*** \author  Nik Nadig (IkarusDowned) nihonnik@gmail.com
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for menu mode interface.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "menu.h"

#include "engine/system.h"
#include "engine/input.h"
#include "engine/audio/audio.h"
#include "modes/pause.h"

#include "engine/mode_manager.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_system;
using namespace vt_mode_manager;
using namespace vt_input;
using namespace vt_global;
using namespace vt_pause;
using namespace vt_menu::private_menu;

namespace vt_menu
{

namespace private_menu {

//! A static variable permitting to update the displayed game time only every 900ms
static int32 _update_of_time = 0;

//! \brief Functions that initialize the numerous option boxes
static void SetupOptionBoxCommonSettings(OptionBox *ob)
{
    // Set all the default options
    ob->SetTextStyle(TextStyle("title22"));
    ob->SetPosition(142.0f, 85.0f);
    ob->SetDimensions(115.0f, 50.0f, 1, 1, 1, 1);
    ob->SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    ob->SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    ob->SetSelectMode(VIDEO_SELECT_SINGLE);
    ob->SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    ob->SetCursorOffset(-52.0f, -20.0f);
}

AbstractMenuState::AbstractMenuState(const char *state_name, MenuMode *menu_mode):
    _state_name(state_name),
    _menu_mode(menu_mode),
    _from_state(NULL)
{
}

void AbstractMenuState::Update()
{
    GlobalMedia& media = GlobalManager->Media();

    // if the current state is set to active, to an active update instead and return
    if(_IsActive())
    {
        _ActiveWindowUpdate();
        return;
    }
    // handle a cancel press. in the case that we are at the main_menu state, pop the ModeManager off
    // the Mode stack as well.
    if(InputManager->CancelPress())
    {
        media.PlaySound("cancel");
        if(_menu_mode->_current_menu_state == &(_menu_mode->_main_menu_state))
            ModeManager->Pop();
        // do instance specific cancel logic
        _OnCancel();
        return;
    }
    // handle left / right option box movement
    else if(InputManager->LeftPress())
    {
        media.PlaySound("bump");
        _options.InputLeft();
        return;
    }
    else if(InputManager->RightPress())
    {
        media.PlaySound("bump");
        _options.InputRight();
        return;
    }
    // play a sound if the option is selected
    else if(InputManager->ConfirmPress())
    {
        if(_options.IsOptionEnabled((_options.GetSelection())))
            media.PlaySound("confirm");
        _options.InputConfirm();
    }
    // return the event type from the option
    int32 event = _options.GetEvent();
    // update the current option box for this state, thus clearing the event flag
    // if we don't do this, then upon return we enter right back into the state we wanted
    // to return from
    _options.Update();

    if(event == VIDEO_OPTION_CONFIRM) {
        uint32 selection = _options.GetSelection();
        AbstractMenuState *next_state = GetTransitionState(selection);
        // if the next state is the state we came from, it is similar to "cancel"
        if(next_state == _from_state)
        {
            _OnCancel();
            return;
        }
        // otherwise, if the state is valid and not this state itself, handle the transition
        else if(next_state != NULL && next_state != this)
        {
            // change the static current menu state
            _menu_mode->_current_menu_state = next_state;

            next_state->_from_state = this;
            next_state->Reset();
        }
        // When we change the state, update the time immediately to avoid
        // showing outdated or empty time info
        _update_of_time = 0;
    }

    // update the current state
    _OnUpdateState();
    // update the options for the currently active state
    _menu_mode->_current_menu_state->GetOptions()->Update();

    _menu_mode->UpdateTimeAndDrunes();
}

void AbstractMenuState::Draw()
{
    // Draw the saved screen background
    // For that, set the system coordinates to the size of the window (same with the save-screen)
    int32 width = VideoManager->GetViewportWidth();
    int32 height = VideoManager->GetViewportHeight();
    VideoManager->SetCoordSys(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);

    VideoManager->Move(0.0f, 0.0f);
    _menu_mode->_saved_screen.Draw();

    // Restore the Coordinate system (that one is menu mode coordinate system)
    VideoManager->SetStandardCoordSys();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);

    // Move to the top left corner
    VideoManager->Move(0.0f, 0.0f);

    _menu_mode->_main_options_window.Draw();

    // do instance specific main window rendering
    _OnDrawMainWindow();
    // do instance specific side window rendering
    _OnDrawSideWindow();
    // Draw currently active options box
    _options.Draw();

}

void AbstractMenuState::_OnDrawSideWindow()
{
    _menu_mode->_character_window0.Draw();
    _menu_mode->_character_window1.Draw();
    _menu_mode->_character_window2.Draw();
    _menu_mode->_character_window3.Draw();
}

void AbstractMenuState::_DrawBottomMenu()
{
    _menu_mode->_bottom_window.Draw();

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
    VideoManager->Move(150.0f, 580.0f);
    // Display Location
    _menu_mode->_locale_name.Draw();

    // Draw Played Time
    _menu_mode->_time_text.Draw();

    // Display the current funds that the party has
    _menu_mode->_drunes_text.Draw();

    VideoManager->MoveRelative(-50.0f, 60.0f);
    _menu_mode->_clock_icon->Draw();
    VideoManager->MoveRelative(0.0f, 30.0f);
    _menu_mode->_drunes_icon->Draw();

    if(!_menu_mode->_locale_graphic.GetFilename().empty()) {
        VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, 0);
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        VideoManager->Move(390.0f, 685.0f);
        _menu_mode->_locale_graphic.Draw();
    }
}


void AbstractMenuState::_OnCancel()
{
    // as long as the calling state is valid and not equal to this, simply switch back to it
    if(_from_state && _from_state != this)
        _menu_mode->_current_menu_state = _from_state;
}

//! Menu messages
static ustring world_map_window_message;
static ustring quest_view_message;

MainMenuState::MainMenuState(MenuMode *menu_mode):
    AbstractMenuState("Main Menu", menu_mode)
{
    // Setup the main options box
    SetupOptionBoxCommonSettings(&_options);
    _options.SetDimensions(745.0f, 50.0f, MAIN_OPTIONS_SIZE, 1, MAIN_OPTIONS_SIZE, 1);

    // Generate the strings
    std::vector<ustring> options;
    options.push_back(UTranslate("Inventory")); // 0
    options.push_back(UTranslate("Skills")); // 1
    options.push_back(UTranslate("Party")); // 2
    options.push_back(UTranslate("Quests")); // 3
    options.push_back(UTranslate("Map")); // 4

    // Add strings and set default selection.
    _options.SetOptions(options);
    _options.SetSelection(MAIN_OPTIONS_INVENTORY);
    _options.SetSkipDisabled(true);

    // Deactivate menus with empy content
    if (GlobalManager->GetActiveQuestIds().empty())
        _options.EnableOption(3, false);

    if (GlobalManager->GetWorldMapFilename().empty())
        _options.EnableOption(4, false);

    // We set them here in case the language has changed since the game start.
    world_map_window_message = UTranslate("Select to view current world map.\n"
                                          "Use left / right to cycle through locations.\nPress 'cancel' to return");
    quest_view_message = UTranslate("Select to view Quest Log.");
}

AbstractMenuState* MainMenuState::GetTransitionState(uint32 selection)
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
    return NULL;
}

void MainMenuState::_OnUpdateState()
{
    uint32 draw_window = _options.GetSelection();
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
    uint32 draw_window = _options.GetSelection();

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
    if(_options.GetSelection() == MAIN_OPTIONS_QUESTS)
        _menu_mode->_quest_list_window.Draw();
    else if (_options.GetSelection() != MAIN_OPTIONS_WORLDMAP)
        AbstractMenuState::_OnDrawSideWindow();
}

void InventoryState::Reset()
{
    // Setup the option box
    SetupOptionBoxCommonSettings(&_options);
    _options.SetDimensions(555.0f, 50.0f, INV_OPTIONS_SIZE, 1, INV_OPTIONS_SIZE, 1);

    // Generate the strings
    std::vector<ustring> options;
    options.push_back(UTranslate("Items"));
    options.push_back(UTranslate("Equip"));
    options.push_back(UTranslate("Remove"));
    options.push_back(UTranslate("Back"));

    // Add strings and set default selection.
    _options.SetOptions(options);
    _options.SetSelection(INV_OPTIONS_USE);
}

AbstractMenuState* InventoryState::GetTransitionState(uint32 selection)
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
    return NULL;
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

    uint32 draw_window = _options.GetSelection();
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

void PartyState::_ActiveWindowUpdate()
{
    _menu_mode->_party_window.Update();
    _menu_mode->UpdateTimeAndDrunes();
}

bool PartyState::_IsActive()
{
    return _menu_mode->_party_window.GetActiveState();
}

void PartyState::Reset()
{
    // Setup the status option box
    SetupOptionBoxCommonSettings(&_options);
    _options.SetDimensions(415.0f, 50.0f, PARTY_OPTIONS_SIZE, 1, PARTY_OPTIONS_SIZE, 1);

    // Generate the strings
    std::vector<ustring> options;
    options.push_back(UTranslate("View/Reorder"));
    options.push_back(UTranslate("Back"));

    // Add strings and set default selection.
    _options.SetOptions(options);
    _options.SetSelection(PARTY_OPTIONS_VIEW_ALTER);

    _menu_mode->_help_information.SetDisplayText(
        UTranslate("View character Information.\nSelect a character to change formation."));

    // Update the current character status at reset, in case of equipment change.
    _menu_mode->_party_window.UpdateStatus();
}

AbstractMenuState* PartyState::GetTransitionState(uint32 selection)
{
    switch(selection)
    {
        case PARTY_OPTIONS_BACK:
            return &(_menu_mode->_main_menu_state);
        case PARTY_OPTIONS_VIEW_ALTER:
            _menu_mode->_party_window.Activate(true);
            break;
        default:
            break;
    };
    return NULL;
}

void PartyState::_DrawBottomMenu()
{
    _menu_mode->_bottom_window.Draw();

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
    VideoManager->Move(150, 580);

    // show a helpful message
    if(!_IsActive())
        _menu_mode->_help_information.Draw();
}

void PartyState::_OnDrawMainWindow()
{
    _DrawBottomMenu();
    _menu_mode->_party_window.Draw();
}

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
    std::vector<ustring> options;
    options.push_back(UTranslate("Use"));
    options.push_back(UTranslate("Back"));

    // Add strings and set default selection.
    _options.SetOptions(options);
    _options.SetSelection(SKILLS_OPTIONS_USE);
}

AbstractMenuState* SkillsState::GetTransitionState(uint32 selection)
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
    return NULL;
}

void EquipState::Reset()
{
    // equip state must handle removal as well as equip. we check to see where we transitioned from...
    if(_from_state == &_menu_mode->_inventory_state)
    {
        // if its from the inventory EQUIP selection, activate the window with the equip flag set to true
        if(_from_state->GetOptions()->GetSelection() == InventoryState::INV_OPTIONS_EQUIP)
            _menu_mode->_equip_window.Activate(true, true);
        // otherwise, it was frmo the REMOVE selection, activate the window with the equip flag set to false
        else
            _menu_mode->_equip_window.Activate(true, false);
    }
}

void SkillsState::_OnDrawMainWindow()
{
    _DrawBottomMenu();
    _menu_mode->_skills_window.Draw();
}

void SkillsState::_DrawBottomMenu()
{
    _menu_mode->_bottom_window.Draw();

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);
    VideoManager->Move(90, 580);
    _menu_mode->_skills_window._skill_icon.Draw();

    _menu_mode->_skills_window._description.Draw();
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

// //////////////////////
// QuestState
// //////////////////////

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

/////////////////////////////////////
// World Map State
/////////////////////////////////////

WorldMapState::WorldMapState(MenuMode *menu_mode):
    AbstractMenuState("WorldMapState", menu_mode),
    _location_image(NULL)
{
    _location_text.SetPosition(102, 556);
    _location_text.SetDimensions(500.0f, 50.0f);
    _location_text.SetTextStyle(TextStyle("title22"));
    _location_text.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

}

void WorldMapState::Reset()
{
    //automatically go into the world map window
    _menu_mode->_world_map_window.Activate(true);

    // defensive update to set up the initial values such as the
    // window offset and such
    _menu_mode->_world_map_window.Update();
}

void WorldMapState::_OnDrawMainWindow()
{
    _menu_mode->_world_map_window.Draw();
    _DrawBottomMenu();
}

void WorldMapState::_DrawBottomMenu()
{
    _menu_mode->_bottom_window.Draw();
    if(_IsActive())
    {
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        VideoManager->Move(150, 580);
        // Display Location
        _location_text.Draw();
        if(_location_image != NULL && !_location_image->GetFilename().empty())
        {
            VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, 0);
            VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
            VideoManager->Move(390, 685);
            _location_image->Draw();
        }

    }
}

bool WorldMapState::_IsActive()
{
    return _menu_mode->_world_map_window.IsActive();
}

void WorldMapState::_ActiveWindowUpdate()
{
    _menu_mode->_world_map_window.Update();
    if(!_IsActive()) {
        _OnCancel();
        return;
    }

    //draw the current viewing location information
    WorldMapLocation *current_location = _menu_mode->_world_map_window.GetCurrentViewingLocation();
    if(current_location == NULL)
    {
        _location_image = NULL;
        _location_text.ClearText();
        return;
    }
    _location_text.SetDisplayText(current_location->_location_name);
    _location_image = &(current_location->_image);
}

} // namespace private_menu

bool MENU_DEBUG = false;

MenuMode *MenuMode::_current_instance = NULL;

// Window size helpers
const uint32 win_start_x = (1024 - 800) / 2 - 40;
const uint32 win_start_y = (768 - 600) / 2 + 15;
const uint32 win_width = 208;

////////////////////////////////////////////////////////////////////////////////
// MenuMode class
////////////////////////////////////////////////////////////////////////////////

MenuMode::MenuMode() :
    GameMode(MODE_MANAGER_MENU_MODE),
    _main_menu_state(this),
    _inventory_state(this),
    _party_state(this),
    _skills_state(this),
    _equip_state(this),
    _quests_state(this),
    _world_map_state(this),
    _message_window(NULL),
    _object(NULL),
    _character(NULL),
    _equip_view_type(EQUIP_VIEW_NONE),
    _is_weapon(false),
    _spirit_number(0)

{
    _current_instance = this;

    // Init the controls parameters.
    _time_text.SetTextStyle(TextStyle("text22"));
    _time_text.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _time_text.SetDimensions(200.0f, 30.0f);
    _time_text.SetPosition(140.0f, 620.0f);

    _drunes_text.SetTextStyle(TextStyle("text22"));
    _drunes_text.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _drunes_text.SetDimensions(200.0f, 30.0f);
    _drunes_text.SetPosition(140.0f, 650.0f);

    // Display the game time right away
    _update_of_time = 0;

    _locale_name.SetPosition(win_start_x + 40, win_start_y + 457);
    _locale_name.SetDimensions(500.0f, 50.0f);
    _locale_name.SetTextStyle(TextStyle("title22"));
    _locale_name.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _locale_name.SetDisplayText(GlobalManager->GetMapHudName());

    // Initialize the location graphic
    _locale_graphic = GlobalManager->GetMapImage();
    if(!_locale_graphic.GetFilename().empty())
        _locale_graphic.SetDimensions(480, 95);
    else
        _locale_graphic.SetDimensions(0, 0);

    try {
        _saved_screen = VideoManager->CaptureScreen();
    } catch(const Exception &e) {
        PRINT_ERROR << e.ToString() << std::endl;
    }

    GlobalMedia& media = GlobalManager->Media();
    _key_item_icon = media.GetKeyItemIcon();

    _key_item_description.SetPosition(165, 600);
    _key_item_description.SetDimensions(700.0f, 50.0f);
    _key_item_description.SetTextStyle(TextStyle("text20"));
    _key_item_description.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _key_item_description.SetDisplayText(UTranslate("This item is a key item and can be neither consumed nor sold."));

    _spirit_icon = media.GetSpiritSlotIcon();

    _spirit_description.SetPosition(165, 600);
    _spirit_description.SetDimensions(700.0f, 50.0f);
    _spirit_description.SetTextStyle(TextStyle("text20"));
    _spirit_description.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _spirit_description.SetDisplayText(UTranslate("This item is an elemental spirit and can be associated with equipment."));

    _help_information.SetPosition(250, 570);
    _help_information.SetDimensions(500, 100);
    _help_information.SetTextStyle(TextStyle("text20"));
    _help_information.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _help_information.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);

    _atk_icon = media.GetStatusIcon(GLOBAL_STATUS_STRENGTH, GLOBAL_INTENSITY_NEUTRAL);
    _matk_icon = media.GetStatusIcon(GLOBAL_STATUS_VIGOR, GLOBAL_INTENSITY_NEUTRAL);
    _def_icon = media.GetStatusIcon(GLOBAL_STATUS_FORTITUDE, GLOBAL_INTENSITY_NEUTRAL);
    _mdef_icon = media.GetStatusIcon(GLOBAL_STATUS_PROTECTION, GLOBAL_INTENSITY_NEUTRAL);

    _clock_icon = vt_global::GlobalManager->Media().GetClockIcon();
    _clock_icon->SetWidthKeepRatio(30.0f);
    _drunes_icon = vt_global::GlobalManager->Media().GetDrunesIcon();
    _drunes_icon->SetWidthKeepRatio(30.0f);

    //////////// Setup the menu windows
    // Width of each character window is 360 px.
    // Each char window will have an additional 16 px for the left border
    // The 4th (last) char window will have another 16 px for the right border
    // Height of the char window is 98 px.
    // The bottom window in the main view is 192 px high, and the full width which will be 216 * 4 + 16
    _character_window0.Create(360, 98, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM);
    _character_window0.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    _character_window1.Create(360, 98, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM | VIDEO_MENU_EDGE_TOP);
    _character_window1.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 118));

    _character_window2.Create(360, 98, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM | VIDEO_MENU_EDGE_TOP);
    _character_window2.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 226));

    _character_window3.Create(360, 98, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_TOP | VIDEO_MENU_EDGE_BOTTOM);
    _character_window3.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 334));

    // The character windows
    ReloadCharacterWindows();

    // The bottom window for the menu
    _bottom_window.Create(static_cast<float>(win_width * 4 + 16), 140 + 16, VIDEO_MENU_EDGE_ALL);
    _bottom_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 442));

    _main_options_window.Create(static_cast<float>(win_width * 4 + 16), 60, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM);
    _main_options_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y - 50));

    // Set up the party window
    _party_window.Create(static_cast<float>(win_width * 4 + 16), 448, VIDEO_MENU_EDGE_ALL);
    _party_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    //Set up the skills window
    _skills_window.Create(static_cast<float>(win_width * 4 + 16), 448, VIDEO_MENU_EDGE_ALL);
    _skills_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    //Set up the equipment window
    _equip_window.Create(static_cast<float>(win_width * 4 + 16), 448, VIDEO_MENU_EDGE_ALL);
    _equip_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    // Set up the inventory window
    _inventory_window.Create(static_cast<float>(win_width * 4 + 16), 448, VIDEO_MENU_EDGE_ALL);
    _inventory_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    //set up the quest window
    _quest_window.Create(static_cast<float>(win_width * 4 + 16), 448, VIDEO_MENU_EDGE_ALL);
    _quest_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    //set up the quest list window
    _quest_list_window.Create(360,448, VIDEO_MENU_EDGE_ALL, VIDEO_MENU_EDGE_TOP | VIDEO_MENU_EDGE_BOTTOM);
    _quest_list_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    //set up the world map window
    _world_map_window.Create(static_cast<float>(win_width * 4 + 16), 448, VIDEO_MENU_EDGE_ALL);
    _world_map_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    _current_menu_state = &_main_menu_state;

    // Reset states
    _main_menu_state.Reset();
    _inventory_state.Reset();
    _party_state.Reset();
    _skills_state.Reset();
    _equip_state.Reset();
    _quests_state.Reset();

    // Show all windows (make them visible)
    _bottom_window.Show();
    _main_options_window.Show();
    _character_window0.Show();
    _character_window1.Show();
    _character_window2.Show();
    _character_window3.Show();
    _inventory_window.Show();
    _quest_list_window.Show();
    _party_window.Show();
    _skills_window.Show();
    _equip_window.Show();
    _quest_window.Show();
    _world_map_window.Show();

    // Init the equipment view members
    _phys_header.SetStyle(TextStyle("text18"));
    _mag_header.SetStyle(TextStyle("text18"));

    _phys_stat.SetStyle(TextStyle("text18"));
    _mag_stat.SetStyle(TextStyle("text18"));

    _object_name.SetStyle(TextStyle("text20"));

    _phys_stat_diff.SetStyle(TextStyle("text18"));
    _mag_stat_diff.SetStyle(TextStyle("text18"));

    _equip_skills_header.SetStyle(TextStyle("title20"));
    _equip_skills_header.SetText(UTranslate("Skills obtained:"));
} // MenuMode::MenuMode()

MenuMode::~MenuMode()
{
    IF_PRINT_WARNING(MENU_DEBUG)
            << "MENU: MenuMode destructor invoked." << std::endl;

    // Destroy all menu windows
    _bottom_window.Destroy();
    _character_window0.Destroy();
    _character_window1.Destroy();
    _character_window2.Destroy();
    _character_window3.Destroy();
    _inventory_window.Destroy();
    _quest_list_window.Destroy();
    _party_window.Destroy();
    _skills_window.Destroy();
    _main_options_window.Destroy();
    _equip_window.Destroy();
    _quest_window.Destroy();
    _world_map_window.Destroy();

    _current_instance = NULL;

    if(_message_window != NULL)
        delete _message_window;
} // MenuMode::~MenuMode()

void MenuMode::Reset()
{
    _current_instance = this;

    // Reload characters information,
    // as active status effects may have changed.
    ReloadCharacterWindows();
}

void MenuMode::UpdateTimeAndDrunes()
{
    // Only update the time every 900ms
    _update_of_time -= (int32) vt_system::SystemManager->GetUpdateTime();
    if (_update_of_time > 0)
        return;
    _update_of_time = 900;

    std::ostringstream os_time;
    uint8 hours = SystemManager->GetPlayHours();
    uint8 minutes = SystemManager->GetPlayMinutes();
    uint8 seconds = SystemManager->GetPlaySeconds();
    os_time << (hours < 10 ? "0" : "") << static_cast<uint32>(hours) << ":";
    os_time << (minutes < 10 ? "0" : "") << static_cast<uint32>(minutes) << ":";
    os_time << (seconds < 10 ? "0" : "") << static_cast<uint32>(seconds);

    _time_text.SetDisplayText(MakeUnicodeString(os_time.str()));
    _drunes_text.SetDisplayText(MakeUnicodeString(NumberToString(GlobalManager->GetDrunes())));
}

void MenuMode::Update()
{
    if(InputManager->QuitPress() == true) {
        ModeManager->Push(new PauseMode(true));
        return;
    } else if(InputManager->PausePress() == true) {
        ModeManager->Push(new PauseMode(false));
        return;
    }

    // check the message window
    if(_message_window != NULL) {
        _message_window->Update();
        if(InputManager->ConfirmPress() || InputManager->CancelPress()) {
            delete _message_window;
            _message_window = NULL;
        }
        return;
    }

    _current_menu_state->Update();

    // Update the overall game mode
    GameMode::Update();

} // void MenuMode::Update()

void MenuMode::UpdateEquipmentInfo(GlobalCharacter *character, GlobalObject *object, EQUIP_VIEW view_type)
{
    // Only update when necessary
    if ((_object == object) && (_character == character) && (_equip_view_type == view_type))
        return;

    _object = object;
    _character = character;
    _equip_view_type = view_type;

    // Clear the corresponding texts when there is no corresponding data
    if (!_object) {
        _object_name.Clear();
        _status_icons.clear();
        _spirit_number = 0;
        _equip_skills.clear();
        _equip_skill_icons.clear();

        _phys_header.Clear();
        _mag_header.Clear();
        _phys_stat.Clear();
        _mag_stat.Clear();
    }

    if (view_type == EQUIP_VIEW_NONE)
        return;

    // Don't show any diff when there no selected character,
    // or not showing equip/unequip diffs.
    if (!_character || view_type == EQUIP_VIEW_CHAR) {
        _phys_stat_diff.Clear();
        _mag_stat_diff.Clear();
    }

    // If there is no object, we can return here.
    if (!_object)
        return;

    _object_name.SetText(_object->GetName());

    // Loads status effects.
    const std::vector<std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY> >& status_effects = _object->GetStatusEffects();
    _status_icons.clear();
    for(std::vector<std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY> >::const_iterator it = status_effects.begin();
            it != status_effects.end(); ++it) {
        if(it->second != GLOBAL_INTENSITY_NEUTRAL)
            _status_icons.push_back(GlobalManager->Media().GetStatusIcon(it->first, it->second));
    }

    uint32 equip_phys_stat = 0;
    uint32 equip_mag_stat = 0;

    switch (_object->GetObjectType()) {
        default: // Should never happen
            return;
        case GLOBAL_OBJECT_WEAPON: {
            _is_weapon = true;
            GlobalWeapon* wpn = NULL;
            // If character view or unequipping, we take the character current weapon as a base
            if (view_type == EQUIP_VIEW_CHAR || view_type == EQUIP_VIEW_UNEQUIPPING)
                wpn = _character ? _character->GetWeaponEquipped() : NULL;
            else // We can take the given object as a base
                wpn = dynamic_cast<GlobalWeapon *>(_object);

            _spirit_number = wpn ? wpn->GetSpiritSlots().size() : 0;
            equip_phys_stat = wpn ? wpn->GetPhysicalAttack() : 0;
            equip_mag_stat = wpn ? wpn->GetMagicalAttack() : 0;

            const std::vector<uint32>& equip_skills = wpn->GetEquipmentSkills();
            _equip_skills.clear();
            _equip_skill_icons.clear();
            // Display a max of 5 skills
            for (uint32 i = 0; i < equip_skills.size() && i < 5; ++i) {
                GlobalSkill *skill = new GlobalSkill(equip_skills[i]);
                if (skill && skill->IsValid()) {
                    _equip_skills.push_back(vt_video::TextImage(skill->GetName(), TextStyle("text20")));
                    _equip_skill_icons.push_back(vt_video::StillImage());
                    vt_video::StillImage& img = _equip_skill_icons.back();
                    img.Load(skill->GetIconFilename());
                    img.SetWidthKeepRatio(15.0f);
                }
                delete skill;
            }
            break;
        }

        case GLOBAL_OBJECT_HEAD_ARMOR:
        case GLOBAL_OBJECT_TORSO_ARMOR:
        case GLOBAL_OBJECT_ARM_ARMOR:
        case GLOBAL_OBJECT_LEG_ARMOR:
        {
            _is_weapon = false;
            GlobalArmor* armor = NULL;

            // If character view or unequipping, we take the character current armor as a base
            if (view_type == EQUIP_VIEW_CHAR || view_type == EQUIP_VIEW_UNEQUIPPING) {
                uint32 equip_index = GetEquipmentPositionFromObjectType(_object->GetObjectType());
                armor = _character ? _character->GetArmorEquipped(equip_index) : NULL;
            }
            else { // We can take the given object as a base
                armor = dynamic_cast<GlobalArmor *>(_object);
            }

            _spirit_number = armor ? armor->GetSpiritSlots().size() : 0;
            equip_phys_stat = armor ? armor->GetPhysicalDefense() : 0;
            equip_mag_stat = armor ? armor->GetMagicalDefense() : 0;

            const std::vector<uint32>& equip_skills = armor->GetEquipmentSkills();
            _equip_skills.clear();
            _equip_skill_icons.clear();
            // Display a max of 5 skills
            for (uint32 i = 0; i < equip_skills.size() && i < 5; ++i) {
                GlobalSkill *skill = new GlobalSkill(equip_skills[i]);
                if (skill && skill->IsValid()) {
                    _equip_skills.push_back(vt_video::TextImage(skill->GetName(), TextStyle("text20")));
                    _equip_skill_icons.push_back(vt_video::StillImage());
                    vt_video::StillImage& img = _equip_skill_icons.back();
                    img.Load(skill->GetIconFilename());
                    img.SetWidthKeepRatio(15.0f);
                }
                delete skill;
            }
            break;
        }
    }

    if (_is_weapon) {
        _phys_header.SetText(UTranslate("ATK:"));
        _mag_header.SetText(UTranslate("M.ATK:"));
    }
    else {
        _phys_header.SetText(UTranslate("DEF:"));
        _mag_header.SetText(UTranslate("M.DEF:"));
    }

    _phys_stat.SetText(NumberToString(equip_phys_stat));
    _mag_stat.SetText(NumberToString(equip_mag_stat));

    // We can stop here if there is no valid character, or simply showing
    // the object stats
    if (!_character || view_type == EQUIP_VIEW_CHAR)
        return;

    int32 phys_stat_diff = 0;
    int32 mag_stat_diff = 0;

    if (_is_weapon) {
        // Get the character's current attack
        GlobalWeapon* wpn = _character->GetWeaponEquipped();
        uint32 char_phys_stat = 0;
        uint32 char_mag_stat = 0;
        if (_equip_view_type == EQUIP_VIEW_EQUIPPING) {
            char_phys_stat = (wpn ? wpn->GetPhysicalAttack() : 0);
            char_mag_stat = (wpn ? wpn->GetMagicalAttack() : 0);

            phys_stat_diff = equip_phys_stat - char_phys_stat;
            mag_stat_diff = equip_mag_stat - char_mag_stat;
        }
        else { // unequipping
            phys_stat_diff = char_phys_stat - equip_phys_stat;
            mag_stat_diff = char_mag_stat - equip_mag_stat;
        }
    }
    else { // armors
        uint32 equip_index = GetEquipmentPositionFromObjectType(_object->GetObjectType());
        GlobalArmor* armor = _character->GetArmorEquipped(equip_index);
        uint32 char_phys_stat = 0;
        uint32 char_mag_stat = 0;
        if (_equip_view_type == EQUIP_VIEW_EQUIPPING) {
            char_phys_stat = (armor ? armor->GetPhysicalDefense() : 0);
            char_mag_stat = (armor ? armor->GetMagicalDefense() : 0);

            phys_stat_diff = equip_phys_stat - char_phys_stat;
            mag_stat_diff = equip_mag_stat - char_mag_stat;
        }
        else { // unequiping
            phys_stat_diff = char_phys_stat - equip_phys_stat;
            mag_stat_diff = char_mag_stat - equip_mag_stat;
        }
    }

    // Compute the overall stats diff with selected equipment
    if (phys_stat_diff > 0) {
        _phys_stat_diff.SetText("+" + NumberToString(phys_stat_diff));
        _phys_diff_color.SetColor(Color::green);
    }
    else if (phys_stat_diff < 0) {
        _phys_stat_diff.SetText(NumberToString(phys_stat_diff));
        _phys_diff_color.SetColor(Color::red);
    }
    else {
        _phys_stat_diff.Clear();
    }

    if (mag_stat_diff > 0) {
        _mag_stat_diff.SetText("+" + NumberToString(mag_stat_diff));
        _mag_diff_color.SetColor(Color::green);
    }
    else if (mag_stat_diff < 0) {
        _mag_stat_diff.SetText(NumberToString(mag_stat_diff));
        _mag_diff_color.SetColor(Color::red);
    }
    else {
        _mag_stat_diff.Clear();
    }
}

void MenuMode::Draw()
{
    _current_menu_state->Draw();

    if(_message_window)
        _message_window->Draw();
} // void MenuMode::Draw()


void MenuMode::DrawEquipmentInfo()
{
    if (!_object)
        return;

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);
    VideoManager->Move(100.0f, 560.0f);
    _object_name.Draw();

    VideoManager->MoveRelative(0.0f, 30.0f);
    const StillImage& obj_icon = _object->GetIconImage();
    obj_icon.Draw();

    // Key item/spirit part
    if (_object->IsKeyItem()) {
        int32 key_pos_x = obj_icon.GetWidth() - _key_item_icon->GetWidth() - 3;
        int32 key_pos_y = obj_icon.GetHeight() - _key_item_icon->GetHeight() - 3;
        VideoManager->MoveRelative(key_pos_x, key_pos_y);
        _key_item_icon->Draw();
        VideoManager->MoveRelative(-key_pos_x, -key_pos_y);
    }

    // Draw weapon stats
    VideoManager->MoveRelative(70.0f, 0.0f);
    if (_is_weapon)
        _atk_icon->Draw();
    else
        _def_icon->Draw();
    VideoManager->MoveRelative(25.0f, 0.0f);
    _phys_header.Draw();

    VideoManager->MoveRelative(-25.0f, 30.0f);
    if (_is_weapon)
        _matk_icon->Draw();
    else
        _mdef_icon->Draw();
    VideoManager->MoveRelative(25.0f, 0.0f);
    _mag_header.Draw();

    VideoManager->SetDrawFlags(VIDEO_X_RIGHT, 0);
    VideoManager->MoveRelative(110.0f, -30.0f);
    _phys_stat.Draw();
    VideoManager->MoveRelative(0.0f, 30.0f);
    _mag_stat.Draw();

    // Draw diff with current weapon stat, if any
    VideoManager->MoveRelative(50.0f, -30.0f);
    _phys_stat_diff.Draw(_phys_diff_color);
    VideoManager->MoveRelative(0.0f, 30.0f);
    _mag_stat_diff.Draw(_mag_diff_color);

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, 0);
    VideoManager->MoveRelative(20.0f, 0.0f);
    float j = 0;
    for (uint32 i = 0; i < _spirit_number; ++i) {
        _spirit_icon->Draw();
        if (i % 2 == 0) {
            VideoManager->MoveRelative(15.0f , 0.0f);
            j -= 15.0f;
        }
        else {
            VideoManager->MoveRelative(25.0f , 0.0f);
            j -= 25.0f;
        }
    }
    VideoManager->MoveRelative(j, -55.0f);

    // Draw status effects icons
    uint32 element_size = _status_icons.size() > 9 ? 9 : _status_icons.size();
    VideoManager->MoveRelative((18.0f * element_size), 0.0f);
    for(uint32 i = 0; i < element_size; ++i) {
        _status_icons[i]->Draw();
        VideoManager->MoveRelative(-18.0f, 0.0f);
    }
    VideoManager->MoveRelative(0.0f, 20.0f);
    if (_status_icons.size() > 9) {
        element_size = _status_icons.size();
        VideoManager->MoveRelative((18.0f * (element_size - 9)), 0.0f);
        for(uint32 i = 9; i < element_size; ++i) {
            _status_icons[i]->Draw();
            VideoManager->MoveRelative(-18.0f, 0.0f);
        }
    }

    // Draw possible equipment skills
    VideoManager->MoveRelative(250.0f, -20.0f);
    element_size = _equip_skills.size();
    if (element_size > 0)
        _equip_skills_header.Draw();
    VideoManager->MoveRelative(10.0f, 20.0f);
    for (uint32 i = 0; i < element_size; ++i) {
        _equip_skills[i].Draw();
        VideoManager->MoveRelative(-20.0f, 5.0f);
        _equip_skill_icons[i].Draw();
        VideoManager->MoveRelative(20.0f, 15.0f);
    }
}

void MenuMode::ReloadCharacterWindows()
{
    GlobalParty &characters = *GlobalManager->GetActiveParty();

    // Setup character windows based on active party size
    switch(characters.GetPartySize()) {
    case 4:
        _character_window3.SetCharacter(characters.GetCharacterAtIndex(3));
    case 3:
        _character_window2.SetCharacter(characters.GetCharacterAtIndex(2));
    case 2:
        _character_window1.SetCharacter(characters.GetCharacterAtIndex(1));
    case 1:
        _character_window0.SetCharacter(characters.GetCharacterAtIndex(0));
        break;
    default:
        PRINT_ERROR << "No characters in party!" << std::endl;
        ModeManager->Pop();
        break;
    }
}

} // namespace vt_menu
