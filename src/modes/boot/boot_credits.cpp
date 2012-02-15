///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    boot_credits.cpp
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \brief   Source file for the boot credits window
*** ***************************************************************************/

#include "script.h"
#include "system.h"
#include "video.h"

#include "boot_credits.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_script;
using namespace hoa_system;

namespace hoa_boot {

namespace private_boot {

CreditsWindow::CreditsWindow() :
	_active(false),
	_loaded(false),
	_scroll_offset(0.0f)
{
	_window.Create(880.0f, 640.0f);
	_window.SetPosition(512.0f, 384.0f);
	_window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_window.SetDisplayMode(VIDEO_MENU_EXPAND_FROM_CENTER);

	_credits_text.SetStyle(TextStyle("text22"));
}



CreditsWindow::~CreditsWindow() {
	_window.Destroy();
}



void CreditsWindow::Draw() {
	_window.Draw();

	// Don't draw any contents of the window until the window is fully shown
	if (_window.GetState() != VIDEO_MENU_STATE_SHOWN)
		return;

	// Set clip region for the text and draw the visible part of it
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_TOP, 0);
	VideoManager->Move(512.0f, 384.0f + _scroll_offset);
	VideoManager->EnableScissoring();
	// TODO: This returns a bad scissor rect due to a known bug in the GUI code
// 	VideoManager->SetScissorRect(_window.GetScissorRect());
	VideoManager->SetScissorRect(ScreenRect(0.0f, 80.0f, 1024.0f, 610.0f));

	// Initially fade in the text from completely transparent to fully opaque
	float color_alpha = _scroll_offset * 0.025f;
	if (color_alpha >= 1.0f)
		_credits_text.Draw();
	else
		_credits_text.Draw(Color(1.0f, 1.0f, 1.0f, color_alpha));
	VideoManager->DisableScissoring();
}



void CreditsWindow::Update() {
	_window.Update(SystemManager->GetUpdateTime());
	_scroll_offset += static_cast<float>(SystemManager->GetUpdateTime()) * 0.025f;
}



void CreditsWindow::Show() {
	_active = true;
	_window.Show();
	_scroll_offset = 0.0f;

	if (_loaded == true)
		return;

	// Load the credits text from the Lua file
	ReadScriptDescriptor credits_file;
	if (credits_file.OpenFile("dat/credits.lua") == false) {
		IF_PRINT_WARNING(BOOT_DEBUG) << "failed to open the Lua credits file" << endl;
	}
	_credits_text.SetText(MakeUnicodeString(credits_file.ReadString("credits_text")));
	credits_file.CloseFile();
	_loaded = true;
}



void CreditsWindow::Hide() {
	_active = false;
	_window.Hide();
}

} // namespace private_boot

} // namespace hoa_boot
