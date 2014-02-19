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
*** \file    mode_help_window.cpp
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for the help window
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/mode_help_window.h"

#include "engine/video/video.h"
#include "engine/input.h"
#include "engine/mode_manager.h"

using namespace vt_utils;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_script;
using namespace vt_system;

namespace vt_mode_manager
{

// *****************************************************************************
// ***** WelcomeWindow class methods
// *****************************************************************************

HelpWindow::HelpWindow() :
    _active(false)
{
    _window.Create(880.0f, 640.0f);
    _window.SetPosition(512.0f, 384.0f);
    _window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);

    _key_table_header.SetOwner(&_window);
    _key_table.SetOwner(&_window);
} // HelpWindow::helpWindow()

HelpWindow::~HelpWindow()
{
    _window.Destroy();
}

void HelpWindow::Update()
{
    if(!IsActive())
        return;

    _window.Update();
}

void HelpWindow::Draw()
{
    // Draw the background window
    VideoManager->PushState();
    VideoManager->SetStandardCoordSys();
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_TOP, VIDEO_BLEND, 0);
    _window.Draw();

    // Don't draw any contents of the window until the window is fully shown
    if(_window.GetState() != VIDEO_MENU_STATE_SHOWN) {
        VideoManager->PopState();
        return;
    }

    // Draw the window contents, starting from the top and moving downward
    VideoManager->Move(512.0f, 93.0f);
    _text_header.Draw();

    _key_table_header.Draw();
    _key_table.Draw();

    VideoManager->Move(512.0f, 618.0f);
    _text_additional.Draw();
    VideoManager->MoveRelative(0.0f, 30.0f);
    _text_continue.Draw();

    VideoManager->PopState();
}

void HelpWindow::Show()
{
    _RefreshText();
    _active = true;
    _window.Show();
}

void HelpWindow::Hide()
{
    _active = false;
    _window.Hide();
}

void HelpWindow::_RefreshText()
{
    _text_header.Clear();
    _key_table_header.ClearOptions();
    _key_table.ClearOptions();
    _text_additional.Clear();
    _text_continue.Clear();

    // Common text
    _text_continue.SetStyle(TextStyle("title24"));
    _text_continue.SetText(VTranslate("Press '%s' anytime to show/hide help.",
                           vt_input::InputManager->GetHelpKeyName()));

    uint8 game_type = ModeManager->GetGameType();

    if(game_type == MODE_MANAGER_BOOT_MODE) {
        _text_header.SetStyle(TextStyle("text24"));
        _text_header.SetText(UTranslate("Welcome to Valyria Tear"));

        _key_table_header.SetPosition(50.0f, 128.0f);
        _key_table_header.SetDimensions(600.0f, 30.0f, 3, 1, 3, 1);
        _key_table_header.SetTextStyle(TextStyle("title24"));
        _key_table_header.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _key_table_header.AddOption(UTranslate("Command"));
        _key_table_header.AddOption(UTranslate("Current Key"));
        _key_table_header.AddOption(UTranslate("Purpose"));

        _key_table.SetPosition(50.0f, 168.0f);
        _key_table.SetDimensions(600.0f, 380.0f, 3, 12, 3, 12);
        _key_table.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
        _key_table.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
        _key_table.SetTextStyle(TextStyle("text22"));
        _key_table.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _key_table.AddOption(UTranslate("Left"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetLeftKeyName()));
        _key_table.AddOption(UTranslate("Move menu cursor to the left"));
        _key_table.AddOption(UTranslate("Right"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetRightKeyName()));
        _key_table.AddOption(UTranslate("Move menu cursor to the right"));
        _key_table.AddOption(UTranslate("Confirm"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetConfirmKeyName()));
        _key_table.AddOption(UTranslate("Confirm a menu command"));
        _key_table.AddOption(UTranslate("Cancel"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetCancelKeyName()));
        _key_table.AddOption(UTranslate("Cancel a menu command"));
        // Note for later: For 'quit', only show the keyboard key as the joystick won't make the game quit.
        _key_table.AddOption(UTranslate("Quit"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetQuitKeyName()));
        _key_table.AddOption(UTranslate("Quit the application\n(when using the keyboard)"));
        return;
    } else if(game_type == MODE_MANAGER_SHOP_MODE) {
        _text_header.SetStyle(TextStyle("text22"));
        _text_header.SetText(UTranslate("Shop mode\n\n"
                                        "Use the Left/Right keys to add/remove items into the Buy and Sell modes.\n"
                                        "Then go into the Confirm mode to complete the transaction."));

        _key_table_header.SetPosition(50.0f, 128.0f);
        _key_table_header.SetDimensions(600.0f, 30.0f, 3, 1, 3, 1);
        _key_table_header.SetTextStyle(TextStyle("title24"));
        _key_table_header.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _key_table_header.AddOption(UTranslate("Command"));
        _key_table_header.AddOption(UTranslate("Current Key"));
        _key_table_header.AddOption(UTranslate("Purpose"));

        _key_table.SetPosition(50.0f, 168.0f);
        _key_table.SetDimensions(600.0f, 380.0f, 3, 12, 3, 12);
        _key_table.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
        _key_table.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
        _key_table.SetTextStyle(TextStyle("text22"));
        _key_table.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _key_table.AddOption(UTranslate("Left"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetLeftKeyName()));
        _key_table.AddOption(UTranslate("Remove an item from the buy/sell list"));
        _key_table.AddOption(UTranslate("Right"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetRightKeyName()));
        _key_table.AddOption(UTranslate("Add an item to the buy/sell list."));
        _key_table.AddOption(UTranslate("Up"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetUpKeyName()));
        _key_table.AddOption(UTranslate("Move up in the item list."));
        _key_table.AddOption(UTranslate("Down"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetDownKeyName()));
        _key_table.AddOption(UTranslate("Move down in the item list."));
        _key_table.AddOption(UTranslate("Confirm"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetConfirmKeyName()));
        _key_table.AddOption(UTranslate("Show details or confirm a transaction"));
        _key_table.AddOption(UTranslate("Menu"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetMenuKeyName()));
        _key_table.AddOption(UTranslate("Change the item displayed category."));
        _key_table.AddOption(UTranslate("Cancel"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetCancelKeyName()));
        _key_table.AddOption(UTranslate("Get out the Buy/Sell/Confirm mode,\nor quit the shop."));
        _key_table.AddOption(); // Spacer
        _key_table.AddOption();
        _key_table.AddOption();
        _key_table.AddOption(UTranslate("Pause"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetPauseKeyName()));
        _key_table.AddOption(UTranslate("Pauses the game"));
        _key_table.AddOption(UTranslate("Quit"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetQuitKeyName()));
        _key_table.AddOption(UTranslate("Close this window / Show the system menu"));
        return;
    }

    // MODE_MANAGER_DUMMY_MODE text
    _text_header.SetStyle(TextStyle("text20"));
    _text_header.SetText(UTranslate("The table below lists the default game controls.\n") +
                         UTranslate("The control mappings can be changed in the options menu."));

    _key_table_header.SetPosition(50.0f, 128.0f);
    _key_table_header.SetDimensions(600.0f, 30.0f, 3, 1, 3, 1);
    _key_table_header.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _key_table_header.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _key_table_header.SetTextStyle(TextStyle("title24"));
    _key_table_header.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _key_table_header.AddOption(UTranslate("Command"));
    _key_table_header.AddOption(UTranslate("Default Key"));
    _key_table_header.AddOption(UTranslate("General Purpose"));

    _key_table.SetPosition(50.0f, 168.0f);
    _key_table.SetDimensions(600.0f, 380.0f, 3, 12, 3, 12);
    _key_table.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _key_table.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _key_table.SetTextStyle(TextStyle("text22"));
    _key_table.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _key_table.AddOption(UTranslate("Up"));
    _key_table.AddOption(UTranslate(vt_input::InputManager->GetDownKeyName()));
    _key_table.AddOption(UTranslate("Move position or cursor upwards"));
    _key_table.AddOption(UTranslate("Down"));
    _key_table.AddOption(UTranslate(vt_input::InputManager->GetDownKeyName()));
    _key_table.AddOption(UTranslate("Move position or cursor downwards"));
    _key_table.AddOption(UTranslate("Left"));
    _key_table.AddOption(UTranslate(vt_input::InputManager->GetLeftKeyName()));
    _key_table.AddOption(UTranslate("Move position or cursor to the left"));
    _key_table.AddOption(UTranslate("Right"));
    _key_table.AddOption(UTranslate(vt_input::InputManager->GetRightKeyName()));
    _key_table.AddOption(UTranslate("Move position or cursor to the right"));
    _key_table.AddOption(UTranslate("Confirm"));
    _key_table.AddOption(UTranslate(vt_input::InputManager->GetConfirmKeyName()));
    _key_table.AddOption(UTranslate("Confirm an action or menu command"));
    _key_table.AddOption(UTranslate("Cancel"));
    _key_table.AddOption(UTranslate(vt_input::InputManager->GetCancelKeyName()));
    _key_table.AddOption(UTranslate("Cancel an action or menu command"));

    if (game_type != MODE_MANAGER_MENU_MODE && game_type != MODE_MANAGER_BATTLE_MODE) {
        _key_table.AddOption(UTranslate("Menu"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetMenuKeyName()));
        _key_table.AddOption(UTranslate("Opens the party menu"));
    }
    else if (game_type == MODE_MANAGER_BATTLE_MODE) {
        _key_table.AddOption(UTranslate("Menu"));
        _key_table.AddOption(UTranslate(vt_input::InputManager->GetMenuKeyName()));
        _key_table.AddOption(UTranslate("Show/Hide items and skills descriptions."));
    }
    _key_table.AddOption(UTranslate("Pause"));
    _key_table.AddOption(UTranslate(vt_input::InputManager->GetPauseKeyName()));
    _key_table.AddOption(UTranslate("Pauses the game"));
    _key_table.AddOption(UTranslate("Quit"));
    _key_table.AddOption(UTranslate(vt_input::InputManager->GetQuitKeyName()));
    _key_table.AddOption(UTranslate("Quit the application"));

    _text_additional.SetStyle(TextStyle("text20"));
    _text_additional.SetText(UTranslate("There are additional commands available which can be found in the MANUAL file."));
}

} // namespace vt_mode_manager
