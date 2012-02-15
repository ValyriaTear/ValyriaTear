///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    boot_welcome.cpp
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \brief   Source file for the boot welcome window
*** ***************************************************************************/

#include "system.h"

#include "boot_welcome.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_script;
using namespace hoa_system;

namespace hoa_boot {

namespace private_boot {

// *****************************************************************************
// ***** WelcomeWindow class methods
// *****************************************************************************

WelcomeWindow::WelcomeWindow() :
	_active(false)
{
	_window.Create(880.0f, 640.0f);
	_window.SetPosition(512.0f, 384.0f);
	_window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);

	_text_header.SetStyle(TextStyle("text20"));
	_text_header.SetText(UTranslate("The table below lists the default game controls.\n") +
		UTranslate("The control mappings can be changed in the options menu on the next screen."));

	_key_table_header.SetOwner(&_window);
	_key_table_header.SetPosition(50.0f, 540.0f);
	_key_table_header.SetDimensions(600.0f, 30.0f, 3, 1, 3, 1);
	_key_table_header.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_key_table_header.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_key_table_header.SetTextStyle(TextStyle("title24"));
	_key_table_header.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

	_key_table_header.AddOption(UTranslate("Command"));
	_key_table_header.AddOption(UTranslate("Default Key"));
	_key_table_header.AddOption(UTranslate("General Purpose"));

	_key_table.SetOwner(&_window);
	_key_table.SetPosition(50.0f, 500.0f);
	_key_table.SetDimensions(600.0f, 380.0f, 3, 12, 3, 12);
	_key_table.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_key_table.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_key_table.SetTextStyle(TextStyle("text22"));
	_key_table.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

	_key_table.AddOption(UTranslate("Up"));
	_key_table.AddOption(UTranslate("Up Arrow"));
	_key_table.AddOption(UTranslate("Move position or cursor upwards"));
	_key_table.AddOption(UTranslate("Down"));
	_key_table.AddOption(UTranslate("Down Arrow"));
	_key_table.AddOption(UTranslate("Move position or cursor downwards"));
	_key_table.AddOption(UTranslate("Left"));
	_key_table.AddOption(UTranslate("Left Arrow"));
	_key_table.AddOption(UTranslate("Move position or cursor to the left"));
	_key_table.AddOption(UTranslate("Right"));
	_key_table.AddOption(UTranslate("Right Arrow"));
	_key_table.AddOption(UTranslate("Move position or cursor to the right"));
	_key_table.AddOption(UTranslate("Confirm"));
	_key_table.AddOption(UTranslate("F"));
	_key_table.AddOption(UTranslate("Confirm an action or menu command"));
	_key_table.AddOption(UTranslate("Cancel"));
	_key_table.AddOption(UTranslate("D"));
	_key_table.AddOption(UTranslate("Cancel an action or menu command"));
	_key_table.AddOption(UTranslate("Menu"));
	_key_table.AddOption(UTranslate("S"));
	_key_table.AddOption(UTranslate("Display the character menu"));
	_key_table.AddOption(UTranslate("Swap"));
	_key_table.AddOption(UTranslate("A"));
	_key_table.AddOption(UTranslate("Swaps active menu or character"));
	_key_table.AddOption(UTranslate("Left Select"));
	_key_table.AddOption(UTranslate("W"));
	_key_table.AddOption(UTranslate("Select multiple or backward page scroll"));
	_key_table.AddOption(UTranslate("Right Select"));
	_key_table.AddOption(UTranslate("E"));
	_key_table.AddOption(UTranslate("Select multiple or forward page scroll"));
	_key_table.AddOption(UTranslate("Pause"));
	_key_table.AddOption(UTranslate("Spacebar"));
	_key_table.AddOption(UTranslate("Pauses the game"));
	_key_table.AddOption(UTranslate("Quit"));
	_key_table.AddOption(UTranslate("Esc"));
	_key_table.AddOption(UTranslate("Quit the application"));

	_text_additional.SetStyle(TextStyle("text20"));
	_text_additional.SetText(UTranslate("There are additional commands available which can be found in the MANUAL file."));

	_text_continue.SetStyle(TextStyle("title24"));
	_text_continue.SetText(UTranslate("Press any key to continue."));
} // WelcomeWindow::WelcomeWindow()



WelcomeWindow::~WelcomeWindow() {
	_window.Destroy();
}



void WelcomeWindow::Update() {
	if (IsActive() == false)
		return;

	_window.Update();
	// TODO: hide window if any key is pressed
}



void WelcomeWindow::Draw() {
	// Draw the background window
	_window.Draw();

	// Don't draw any contents of the window until the window is fully shown
	if (_window.GetState() != VIDEO_MENU_STATE_SHOWN)
		return;

	// Draw the window contents, starting from the top and moving downward
	VideoManager->PushState();
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_TOP, 0);
	VideoManager->Move(512.0f, 675.0f);
	_text_header.Draw();

	_key_table_header.Draw();
	_key_table.Draw();

	VideoManager->Move(512.0f, 150.0f);
	_text_additional.Draw();
	VideoManager->MoveRelative(0.0f, -30.0f);
	_text_continue.Draw();
	VideoManager->PopState();
}



void WelcomeWindow::Show() {
	_active = true;
	_window.Show();
}



void WelcomeWindow::Hide() {
	_active = false;
	_window.Hide();
}

} // namespace private_boot

} // namespace hoa_boot
