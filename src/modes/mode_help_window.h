///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    mode_help_window.h
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the boot welcome window
*** ***************************************************************************/

#ifndef __MODE_HELP_WINDOW__
#define __MODE_HELP_WINDOW__

#include "common/gui/menu_window.h"
#include "engine/video/text.h"
#include "common/gui/option.h"

namespace vt_mode_manager
{

/** ****************************************************************************
*** \brief Displays vital information to the player upon starting the game
***
*** This welcome window only pops up the first time that the player starts the game.
*** Its purpose is to ensure that the player understands the default key commands in
*** case they started up the game prior to reading the game manual. Under normal
*** circumstances, this screen is shown only once when the player first starts up
*** the game after installation. After that the Lua file holding the game settings
*** is marked so that this screen does not pop up a second time.
***
*** The window itself consists of a large MenuWindow, header text explaining the
*** window's purpose of providing default key mappings to the player, a table of
*** the default keys and purposes, and footer text informing the player where
*** they can find more information.
*** *****************************************************************************/
class HelpWindow
{
public:
    HelpWindow();

    ~HelpWindow();

    //! \brief Updates the state of the welcome window and processes user input
    void Update();

    //! \brief Draws the welcome window and its contents to the screen
    void Draw();

    //! \brief Activates and shows the welcome window
    void Show();

    //! \brief Deactivates and hides the welcome window
    void Hide();

    //! \brief Returns true if the welcome window is currently active and visible
    bool IsActive() const {
        return _active;
    }

private:
    //! \brief Set to true when the window is active and should be visible on the screen
    bool _active;

    //! Window for the screen
    vt_gui::MenuWindow _window;

    //! \brief Rendered text of the header string
    vt_video::TextImage _text_header;

    //! \brief An option box used as a header for the key table
    vt_gui::OptionBox _key_table_header;

    //! \brief Stores the text for default key mappings in a table format
    vt_gui::OptionBox _key_table;

    //! \brief Rendered text that tells the player how to move past the window
    vt_video::TextImage _text_additional;

    //! \brief Rendered text that tells the player how to move past the window
    vt_video::TextImage _text_continue;

    //! \brief Refresh the text shown according to the given game mode
    void _RefreshText();
}; // class HelpWindow

} // namespace vt_mode_manager

#endif // __MODE_HELP_WINDOW__
