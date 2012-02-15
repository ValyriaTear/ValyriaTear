///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    boot_credits.h
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \brief   Header file for the boot credits window
*** ***************************************************************************/

#ifndef __BOOT_CREDITS__
#define __BOOT_CREDITS__

#include "utils.h"
#include "video.h"
#include "gui.h"

namespace hoa_boot {

namespace private_boot {

/** ****************************************************************************
*** \brief Displays the game credits in a vertical scrolling fashion
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
class CreditsWindow {
public:
	CreditsWindow();

	~CreditsWindow();

	//! \brief Updates the state of the credits window and processes user input
	void Update();

	//! \brief Draws the credits window and its contents to the screen
	void Draw();

	//! \brief Activates and shows the credits window
	void Show();

	//! \brief Deactivates and hides the welcome window
	void Hide();

	//! \brief Returns true if the credits window is currently active and visible
	bool IsActive() const
		{ return _active; }

private:
	//! \brief Set to true when the window is active and should be visible on the screen
	bool _active;

	//! \brief Set to true only after the credits text has been loaded and rendered
	bool _loaded;

	//! \brief A vertical offset used for the scrolling credits text
	float _scroll_offset;

	//! \brief The GUI menu window that all other content is drawn upon
	hoa_gui::MenuWindow _window;

	//! \brief The rendered image of the credits text
	hoa_video::TextImage _credits_text;
}; // class CreditsWindow

} // namespace private_boot

} // namespace hoa_boot

#endif // __BOOT_CREDITS__
