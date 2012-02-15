///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/*!****************************************************************************
 * \file    boot_menu.cpp
 * \author  Viljami Korhonen, mindflayer@allacrost.org
 * \brief   Source file for the boot menus
 *****************************************************************************/

#include "boot.h"
#include "boot_menu.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_video;

namespace hoa_boot {

namespace private_boot {

void BootMenu::AddOption(const ustring& text, void (BootMode::*confirm_function)(),
	void (BootMode::*up_function)(), void (BootMode::*down_function)(),
	void (BootMode::*left_function)(), void (BootMode::*right_function)())
{
	OptionBox::AddOption(text);

	_confirm_handlers.push_back(confirm_function);
	_up_handlers.push_back(up_function);
	_down_handlers.push_back(down_function);
	_left_handlers.push_back(left_function);
	_right_handlers.push_back(right_function);
}



void BootMenu::InputConfirm() {
	OptionBox::InputConfirm();

	int32 selection = OptionBox::GetSelection();
	if ((selection != -1) && (_confirm_handlers.empty() == false)) {
		void (BootMode::*confirm_function)() = _confirm_handlers.at(selection);
		if (confirm_function != NULL)
			(BootMode::CurrentInstance()->*confirm_function)();
	}
}



void BootMenu::InputUp() {
	OptionBox::InputUp();

	int32 selection = OptionBox::GetSelection();
	if ((selection != -1) && (_up_handlers.empty() == false)) {
		void (BootMode::*up_function)() = _up_handlers.at(selection);
		if (up_function != NULL)
			(BootMode::CurrentInstance()->*up_function)();
	}
}



void BootMenu::InputDown() {
	OptionBox::InputDown();

	int32 selection = OptionBox::GetSelection();
	if ((selection != -1) && (_down_handlers.empty() == false)) {
		void (BootMode::*down_function)() = _down_handlers.at(selection);
		if (down_function != NULL)
			(BootMode::CurrentInstance()->*down_function)();
	}
}



void BootMenu::InputLeft() {
	OptionBox::InputLeft();

	int32 selection = OptionBox::GetSelection();
	if ((selection != -1) && (_left_handlers.empty() == false)) {
		void (BootMode::*left_function)() = _left_handlers.at(selection);
		if (left_function != NULL)
			(BootMode::CurrentInstance()->*left_function)();
	}
}



void BootMenu::InputRight() {
	OptionBox::InputRight();

	int32 selection = OptionBox::GetSelection();
	if ((selection != -1) && (_right_handlers.empty() == false)) {
		void (BootMode::*right_function)() = _right_handlers.at(selection);
		if (right_function != NULL)
			(BootMode::CurrentInstance()->*right_function)();
	}
}

} // namespace private_boot

} // namespace hoa_boot
