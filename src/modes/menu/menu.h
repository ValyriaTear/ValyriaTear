///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    menu.h
*** \author  Daniel Steuernol steu@allacrost.org
*** \author  Andy Gardner chopperdave@allacrost.org
*** \brief   Header file for menu mode interface.
***
*** This code handles the game event processing and frame drawing when the user
*** is in menu mode. This mode's primary objectives are to allow the user to
*** view stastics about their party and manage inventory and equipment.
*** ***************************************************************************/

#ifndef __MENU_HEADER__
#define __MENU_HEADER__

#include <string>
#include <vector>

#include "utils.h"
#include "defs.h"

#include "video.h"

#include "global.h"

#include "mode_manager.h"
#include "menu_views.h"

//! \brief All calls to menu mode are wrapped in this namespace.
namespace hoa_menu {

//! \brief Determines whether the code in the hoa_menu namespace should print debug statements or not.
extern bool MENU_DEBUG;

//! \brief An internal namespace to be used only within the menu code. Don't use this namespace anywhere else!
namespace private_menu {

//! \brief The different item categories
enum MAIN_CATEGORY {
	MAIN_INVENTORY      = 0,
	MAIN_SKILLS         = 1,
	MAIN_EQUIP          = 2,
	MAIN_STATUS         = 3,
//	MAIN_OPTIONS        = 3;
	MAIN_SAVE           = 4,
	MAIN_FORMATION      = 5,
	MAIN_SIZE           = 6
};

//! \name Inventory Menu Options Constants
//@{
const uint32 INV_USE    = 0;
// const uint32 INV_SORT   = 1;
const uint32 INV_BACK   = 1;
const uint32 INV_SIZE   = 2;
//@}

//! \name Skills Menu Options Constants
//@{
const uint32 SKILLS_USE     = 0;
const uint32 SKILLS_BACK    = 1;
const uint32 SKILLS_SIZE    = 2;
//@}

//! \name Equipment Menu Options Constants
//@{
const uint32 EQUIP_EQUIP   = 0;
const uint32 EQUIP_REMOVE  = 1;
const uint32 EQUIP_BACK    = 2;
const uint32 EQUIP_SIZE    = 3;
//@}

//! \name Status Menu Options Constants
//@{
const uint32 STATUS_VIEW    = 0;
const uint32 STATUS_BACK    = 1;
const uint32 STATUS_SIZE    = 2;
//@}

//! \name Formation Menu Options Constants
//@{
const uint32 FORMATION_SWITCH  = 0;
const uint32 FORMATION_BACK    = 1;
const uint32 FORMATION_SIZE    = 2;
//@}

//! \name Options Menu Options Constants
//@{
const uint32 OPTIONS_EDIT    = 0;
const uint32 OPTIONS_SAVE    = 1;
const uint32 OPTIONS_BACK    = 2;
const uint32 OPTIONS_SIZE    = 3;
//@}

//! \name Save Menu Options Constants
//@{
const uint32 SAVE_SAVE    = 0;
const uint32 SAVE_BACK    = 1;
const uint32 SAVE_SIZE    = 2;
//@}

//! \name MenuMode OptionBox Show Flags
//! \brief Constants used to determine which option box is currently showing.
//@{
const uint32 SHOW_MAIN          = 0;
const uint32 SHOW_INVENTORY     = 1;
const uint32 SHOW_SKILLS        = 2;
const uint32 SHOW_EQUIP         = 3;
const uint32 SHOW_STATUS        = 4;
//const uint32 SHOW_OPTIONS       = 5;
const uint32 SHOW_SAVE          = 5;
const uint32 SHOW_FORMATION     = 6;
const uint32 SHOW_EXIT          = 7;
//@}

/** \name MenuMode Active Window Flags
*** \brief Constants used to determine which window is currently showing.
**/
//@{
const uint32 WINDOW_INVENTORY      = 1;
const uint32 WINDOW_SKILLS         = 2;
const uint32 WINDOW_STATUS         = 3;
const uint32 WINDOW_EQUIP          = 4;
const uint32 WINDOW_FORMATION      = 5;
//@}

} // namespace private_menu

/** ****************************************************************************
*** \brief Handles game executing while in the main in-game menu.
***
*** This mode of game operation allows the player to examine and manage their
*** party, inventory, options, and save their game.
***
*** \note MenuMode is always entered from an instance of MapMode. However, there
*** may be certain conditions where MenuMode is entered from other game modes.
***
*** \note MenuMode does not play its own music, but rather it continues playing
*** music from the previous GameMode that created it.
*** ***************************************************************************/
class MenuMode : public hoa_mode_manager::GameMode {
	friend class private_menu::CharacterWindow;
	friend class private_menu::InventoryWindow;
	friend class private_menu::StatusWindow;
	friend class private_menu::SkillsWindow;
	friend class private_menu::EquipWindow;
	friend class private_menu::FormationWindow;

public:
	/** \param location_name The name of the current map that will be displayed on the menu screen.
	*** \param locale_image The filename for the location image that is displayed in the menus.
	**/
	MenuMode(hoa_utils::ustring locale_name, std::string locale_image);

	~MenuMode();

	//! \brief Returns a pointer to the active instance of menu mode
	static MenuMode* CurrentInstance()
		{ return _current_instance; }

	//! \brief Resets the menu mode back to its default setup.
	void Reset();

	//! \brief Updates the menu. Calls Update() on active window if there is one
	void Update();

	//! \brief Draws the menu. Calls Draw() on active window if there is one.
	void Draw();

	// TEMP: other menu classes need access to this member
	hoa_gui::OptionBox& GetCharSelect()
		{ return _char_select; }

private:
	//! \brief A static pointer to the last instantiated MenuMode object
	static MenuMode* _current_instance;

	//! \brief Text image which displays the name of the location in the game where MenuMode was invoked
	hoa_video::TextImage _locale_name;

	/** \brief The graphic that represents the current map that the player is exploring
	*** This image is set using the string in the MenuMode constructor
	**/
	hoa_video::StillImage _locale_graphic;

	/** \brief Retains a snap-shot of the screen just prior to when menu mode was entered
	*** This image is perpetually drawn as the background while in menu mode
	**/
	hoa_video::StillImage _saved_screen;

	/** \name Main Display Windows
	*** \brief The various menu windows that are displayed in menu mode
	**/
	//@{
	hoa_gui::MenuWindow _bottom_window;
	hoa_gui::MenuWindow _main_options_window;

	private_menu::CharacterWindow _character_window0;
	private_menu::CharacterWindow _character_window1;
	private_menu::CharacterWindow _character_window2;
	private_menu::CharacterWindow _character_window3;
	private_menu::InventoryWindow _inventory_window;
	private_menu::StatusWindow _status_window;
	private_menu::SkillsWindow _skills_window;
	private_menu::EquipWindow _equip_window;
	private_menu::FormationWindow _formation_window;
	MessageWindow *_message_window;

	/** \brief The currently active window
	 **/
	hoa_gui::MenuWindow *_active_window;

//	private_menu::SaveWindow _save_window;

	// FIXME
	//std::vector<private_menu::CharacterWindow> _character_windows;
	//std::vector<private_menu::FormationWindow> _formation_windows;
	//@}

	//! \brief A map of the sounds used while in MenuMode
	std::map<std::string, hoa_audio::SoundDescriptor> _menu_sounds;

	//! The selected character
//	static uint32 _char_selected;

	//! The selected item/skill/equipment
	uint32 _item_selected;

	//! The current option box to display
	uint32 _current_menu_showing;

	//! The current window being drawn
	uint32 _current_window;

	//! A pointer to the current options menu
	hoa_gui::OptionBox *_current_menu;

	//! The top level options in boot mode
	hoa_gui::OptionBox _main_options;

	//! \name Option boxes that are used in the various menu windows
	//@{
	hoa_gui::OptionBox _menu_inventory;
	hoa_gui::OptionBox _menu_skills;
	hoa_gui::OptionBox _menu_status;
	hoa_gui::OptionBox _menu_options;
	hoa_gui::OptionBox _menu_equip;
	hoa_gui::OptionBox _menu_formation;
	hoa_gui::OptionBox _menu_save;
	hoa_gui::OptionBox _char_select;
	//@}

	//! \brief Functions that initialize the numerous option boxes
	//@{
	void _SetupOptionBoxCommonSettings(hoa_gui::OptionBox *ob);
	void _SetupMainOptionBox();
	void _SetupInventoryOptionBox();
	void _SetupSkillsOptionBox();
	void _SetupStatusOptionBox();
	void _SetupOptionsOptionBox();
	void _SetupFormationOptionBox();
	void _SetupSaveOptionBox();
	void _SetupEquipOptionBox();
	//@}

	/** \name Menu Handle Functions
	*** \brief Handler functions to deal with events for all the different menus
	**/
	//@{
	void _HandleMainMenu();
	void _HandleInventoryMenu();
	void _HandleSkillsMenu();
	void _HandleStatusMenu();
	void _HandleOptionsMenu();
	void _HandleFormationMenu();
	void _HandleSaveMenu();
	void _HandleEquipMenu();
	//@}

	/** \name Active Window Functions
	*** \brief Handles finding the next active window
	**/
	//@{
	void _GetNextActiveWindow();
	//@}

	//! \brief Draws the bottom part of the menu mode.
	void _DrawBottomMenu();

	//! \brief Draws the 'Name' and 'Qty' tags for the item list.
	void _DrawItemListHeader();

}; // class MenuMode : public hoa_mode_manager::GameMode

} // namespace hoa_menu

#endif
