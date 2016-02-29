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
*** \file    message_window.h
*** \author  Daniel Steuernol steu@allacrost.org
*** \author  Andy Gardner chopperdave@allacrost.org
*** \author  Nik Nadig (IkarusDowned) nihonnik@gmail.com
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for a common message window.
*** ***************************************************************************/

#ifndef __MESSAGE_WINDOW_HEADER__
#define __MESSAGE_WINDOW_HEADER__

#include "common/gui/menu_window.h"

#include "common/gui/textbox.h"

namespace vt_common
{

/** **************************************************************************
*** \brief A window to display a message to the player
*** Displays a message to the user in the center of the screen
*** This class is not private because it's a handy message box and
*** it could be used else where.
*** **************************************************************************/
class MessageWindow : public vt_gui::MenuWindow
{
public:
    //! \brief Message window constructor
    //! \note If the x or y param is set to -1.0f, then the window is centered in the corresponding axis.
    MessageWindow(const vt_utils::ustring& message, float x, float y, float w, float h);
    ~MessageWindow();

    //! \brief Creates the message window using the given coordinates.
    //! \note If the x or y param is set to -1.0f, then the window is centered in the corresponding axis.
    void CreateMessageWindow(float x, float y, float w, float h);

    //! \brief Set the text to display in the window
    void SetText(const vt_utils::ustring& message) {
        _textbox.SetDisplayText(message);
    }

    //! \brief Set the text appearance speed in character per seconds.
    void SetDisplaySpeed(float display_speed) {
        _textbox.SetDisplaySpeed(display_speed);
    }

    //! \brief Updates the text scrolling
    void Update() {
        _textbox.Update();
    }

    //! \brief Standard Window Functions
    void Draw();

private:
    //! \brief used to display the message
    vt_gui::TextBox _textbox;
}; // class MessageWindow

//! \brief A message window that will display itself for a certain amount of time,
//! and will use convenience default location and dimensions based on its text and icon metrics.
//! This one doesn't work at all like the message window as it will set its size according
//! to the given text and optional icon (not the reverse.)
//! It is aimed at displaying short messages.
class ShortNoticeWindow : public vt_gui::MenuWindow
{
public:
    ShortNoticeWindow(const vt_utils::ustring& message,
                       const std::string& image_filename = std::string(),
                       uint32_t display_time = 3000,
                       float x = -1.0f, float y = 50.0f);
    ~ShortNoticeWindow();

    //! \brief Updates the time displayed.
    void Update(uint32_t update_time);

    //! \brief Draw the message window with text centered.
    void Draw();

private:
    //! \brief The amount of time to display the message in milliseconds.
    uint32_t _display_time;

    //! \brief used to display the message
    vt_video::TextImage _text_image;

    //! \brief Optional Icon displayed next to the text.
    vt_video::StillImage _icon_image;

    //! \brief Keeps in mind whether the textbox has got an image to show.
    bool _has_image;

    //! \brief Positions of the icon and text precomputed to avoid doing it at each draw calls.
    float _text_x_pos;
    float _text_y_pos;
    float _icon_x_pos;
    float _icon_y_pos;
}; // class ShortNoticeWindow

} // namespace vt_common

#endif // __MESSAGE_WINDOW_HEADER__
