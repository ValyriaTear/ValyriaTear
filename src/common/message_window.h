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

    //! \brief Standard Window Functions
    //@{
    void Draw();
    //@}

private:
    //! \brief used to display the message
    vt_gui::TextBox _textbox;
}; // class MessageWindow

} // namespace vt_common

#endif // __MESSAGE_WINDOW_HEADER__
