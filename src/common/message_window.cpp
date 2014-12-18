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
*** \file    message_window.cpp
*** \author  Daniel Steuernol steu@allacrost.org
*** \author  Andy Gardner chopperdave@allacrost.org
*** \author  Nik Nadig (IkarusDowned) nihonnik@gmail.com
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for a common message window.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "message_window.h"

#include "engine/video/video.h"

namespace vt_common
{

MessageWindow::MessageWindow(const vt_utils::ustring& message, float x, float y, float w, float h)
{
    CreateMessageWindow(x, y, w, h);
    _textbox.SetDisplayText(message);
}

MessageWindow::~MessageWindow()
{
    MenuWindow::Destroy();
}

void MessageWindow::CreateMessageWindow(float x, float y, float w, float h)
{
    float start_x = x == -1.0f ? (1024 - w) / 2 : x;
    float start_y = y == -1.0f ? (768 - h) / 2 : y;

    MenuWindow::Create(w, h);
    MenuWindow::SetPosition(start_x, start_y);
    MenuWindow::Show();

    _textbox.SetPosition(20.0f, 5.0f);
    _textbox.SetDimensions(w - 30.0f, h);
    _textbox.SetTextStyle(vt_video::TextStyle("text22"));
    _textbox.SetDisplayMode(vt_gui::VIDEO_TEXT_CHAR);
    _textbox.SetTextAlignment(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_CENTER);
    _textbox.SetOwner(this);
}

void MessageWindow::Draw()
{
    MenuWindow::Draw();
    _textbox.Draw();
}

} // namespace vt_common
