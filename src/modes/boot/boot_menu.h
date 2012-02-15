///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    boot_menu.h
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \brief   Header file for the boot menus
*** ***************************************************************************/

#ifndef __BOOT_MENU__
#define __BOOT_MENU__

#include "defs.h"
#include "utils.h"

#include "video.h"
#include "gui.h"

namespace hoa_boot {

namespace private_boot {

/** ****************************************************************************
*** \brief Used for the construction and operation of all boot mode menus
***
*** This class is an extension of the OptionBox class found in the GUI code. Its
*** primary feature is that it utilizes function pointers to the BootMode class,
*** which makes this class incredibly flexible and versatile. The way it works
*** is to keep several containers of function pointers, where the size of these
*** containers are equal to the number of options in the menu. The function pointers
*** are invoked on the selected option when an input command is received. For example,
*** if the second option is selected and a confirm press is registered, the appropriate
*** function pointed to for that option and that input event will be called. Other
*** than this simple yet powerful feature, this class operates and acts exactly the same
*** as a standard OptionBox object.
***
*** \note There are some OptionBox methods which should not be used for this class. Particularly
*** any methods that add or remove options should be avoided because they do not know to modify
*** the function pointer containers appropriately. Use only the methods specific to this class
*** to add or remove options.
*** ***************************************************************************/
class BootMenu : public hoa_gui::OptionBox {
public:
	BootMenu()
		{}

	~BootMenu()
		{}

	/** \brief Adds a new option to the menu with the desired function pointers attached
	*** \param text A text representing the new option
	*** \param *up_function BootMode handler function for up input events
	*** \param *down_function BootMode handler function for down input events
	*** \param *confirm_function BootMode handler function for confirm input events
	*** \param *left_function BootMode handler function for left input events
	*** \param *right_function BootMode handler function for right input events
	**/
	void AddOption(const hoa_utils::ustring & text, void (BootMode::*confirm_function)() = NULL,
		void (BootMode::*up_function)() = NULL,   void (BootMode::*down_function)() = NULL,
		void (BootMode::*left_function)() = NULL, void (BootMode::*right_function)() = NULL);

	//! \brief
	//@{
	void InputConfirm();
	void InputUp();
	void InputDown();
	void InputLeft();
	void InputRight();
	//@}

private:
	//! \brief Confirm input handlers for all options in the menu
	std::vector<void (BootMode::*)()> _confirm_handlers;

	//! \brief Up input handlers for all options in the menu
	std::vector<void (BootMode::*)()> _up_handlers;

	//! \brief Down input handlers for all options in the menu
	std::vector<void (BootMode::*)()> _down_handlers;

	//! \brief Left input handlers for all options in the menu
	std::vector<void (BootMode::*)()> _left_handlers;

	//! \brief Right input handlers for all options in the menu
	std::vector<void (BootMode::*)()> _right_handlers;
}; // class BootMenu : public hoa_video::OptionBox

} // namespace private_boot

} // namespace hoa_boot

#endif // __BOOT_MENU__
