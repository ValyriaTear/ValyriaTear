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

MessageWindow::MessageWindow(const vt_utils::ustring& message, float w, float h)
{
    float start_x = (1024 - w) / 2;
    float start_y = (768 - h) / 2;

    MenuWindow::Create(w, h);
    MenuWindow::SetPosition(start_x, start_y);
    MenuWindow::Show();

    _textbox.SetPosition(30, 5);
    _textbox.SetDimensions(w, h);
    _textbox.SetTextStyle(vt_video::TextStyle("text22"));
    _textbox.SetDisplayMode(vt_gui::VIDEO_TEXT_INSTANT);
    _textbox.SetTextAlignment(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_CENTER);
    _textbox.SetDisplayText(message);
    _textbox.SetOwner(this);
}

MessageWindow::~MessageWindow()
{
    MenuWindow::Destroy();
}

void MessageWindow::Draw()
{
    MenuWindow::Draw();
    _textbox.Draw();
}

} // namespace vt_common
