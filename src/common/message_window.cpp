///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
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
    // Center the window when it's requested.
    float start_x = x == -1.0f ? (1024 - w) / 2 : x;
    float start_y = y == -1.0f ? (768 - h) / 2 : y;

    MenuWindow::Create(w, h);
    MenuWindow::SetPosition(start_x, start_y);
    MenuWindow::Show();

    _textbox.SetPosition(15.0f, -10.0f);
    _textbox.SetDimensions(w - 30.0f, h - 20.0f);
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

// Class ShortNoticeWindow

ShortNoticeWindow::ShortNoticeWindow(const vt_utils::ustring& message,
                                       const std::string& image_filename,
                                       uint32_t display_time,
                                       float x, float y):
    _display_time(display_time)
{
    _text_image.SetText(message, vt_video::TextStyle("text36"));

    // Test whether an image can be shown.
    _has_image = image_filename.empty() ? false : _icon_image.Load(image_filename);

    // Determine the width and height of the window based on the text.
    float width = _text_image.GetWidth() + 30.0f;
    float height = _text_image.GetHeight() + 30.0f;

    if (_has_image) {

        // Adapt the image to the text
        if (_icon_image.GetHeight() > _text_image.GetHeight())
            _icon_image.SetHeightKeepRatio(_text_image.GetHeight());
        else if (_icon_image.GetHeight() < _text_image.GetHeight() / 2.0f)
            _icon_image.SetHeightKeepRatio(_text_image.GetHeight() / 2.0f);

        if (_icon_image.GetWidth() > 50.0f)
            _icon_image.SetWidthKeepRatio(50.0f);

        // Give some space for the icon
        width += _icon_image.GetWidth() + 5.0f;
    }

    // Center the window when it's requested.
    float start_x = x == -1.0f ? (1024 - width) / 2 : x;
    float start_y = y == -1.0f ? (768 - height) / 2 : y;

    // Create but don't show the window per default.
    Create(width, height);
    SetPosition(start_x, start_y);
    Hide();

    // Set the icon and text draw positions
    float initial_width = _has_image ? _icon_image.GetWidth() + 5.0f + _text_image.GetWidth() : _text_image.GetWidth();
    _text_pos.x = MenuWindow::GetXPosition() + (MenuWindow::GetWidth() - initial_width) / 2.0f;
    _text_pos.y = MenuWindow::GetYPosition() + (MenuWindow::GetHeight() - _text_image.GetHeight()) / 2.0f;

    if (_has_image)
        _text_pos.x += _icon_image.GetWidth() + 5.0f;

    _icon_pos.x = MenuWindow::GetXPosition() + 15.0f;
    _icon_pos.y = MenuWindow::GetYPosition() + (MenuWindow::GetHeight() - _icon_image.GetHeight()) / 2.0f;
}

ShortNoticeWindow::~ShortNoticeWindow()
{
    MenuWindow::Destroy();
}

void ShortNoticeWindow::Update(uint32_t update_time)
{
    if (_display_time > update_time) {
        _display_time -= update_time;
        if (!IsVisible())
            Show();
    }
    else {
        _display_time = 0;
        Hide();
    }
}

void ShortNoticeWindow::Draw()
{
    MenuWindow::Draw();

    vt_video::VideoManager->PushState();
    vt_video::VideoManager->SetStandardCoordSys();
    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_TOP, vt_video::VIDEO_BLEND, 0);

    vt_video::VideoManager->Move(_text_pos.x, _text_pos.y);
    // Determine the text and icon position based on the window current size
    _text_image.Draw();

    if (_has_image) {
        vt_video::VideoManager->Move(_icon_pos.x, _icon_pos.y);
        _icon_image.Draw();
    }
    vt_video::VideoManager->PopState();
}

} // namespace vt_common
