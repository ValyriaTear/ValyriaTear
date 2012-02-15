///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    menu_views.h
*** \author  Daniel Steuernol steu@allacrost.org
*** \author  Andy Gardner chopperdave@allacrost.org
*** \brief   Header file for various menu views.
***
*** This code handles the different menu windows that the user will see while the
*** is in menu mode. These windows are used for displaying inventory lists,
*** character statistics, and various other pieces of information.
*** ***************************************************************************/

#ifndef __MENU_VIEWS__
#define __MENU_VIEWS__

#include <string>
#include <vector>

#include "utils.h"
#include "defs.h"

#include "video.h"
#include "gui.h"

#include "global.h"



namespace hoa_menu {

namespace private_menu {

//! \brief The different item categories
enum ITEM_CATEGORY {
	ITEM_ALL = 0,
	ITEM_ITEM = 1,
	ITEM_WEAPONS = 2,
	ITEM_HEAD_ARMOR = 3,
	ITEM_TORSO_ARMOR = 4,
	ITEM_ARM_ARMOR = 5,
	ITEM_LEG_ARMOR = 6,
	ITEM_KEY = 7,
	ITEM_CATEGORY_SIZE = 8
};

//! \brief The different skill types
enum SKILL_CATEGORY {
	SKILL_ALL = 0,
	SKILL_FIELD = 1,
	SKILL_BATTLE = 2,
	SKILL_CATEGORY_SIZE = 3
};

//! \brief The different equipment categories
enum EQUIP_CATEGORY {
	EQUIP_WEAPON = 0,
	EQUIP_HEADGEAR = 1,
	EQUIP_BODYARMOR = 2,
	EQUIP_OFFHAND = 3,
	EQUIP_LEGGINGS = 4,
	EQUIP_CATEGORY_SIZE = 5
};

//! \brief The different option boxes that can be active for items
enum ITEM_ACTIVE_OPTION {
	ITEM_ACTIVE_NONE = 0,
	ITEM_ACTIVE_CATEGORY = 1,
	ITEM_ACTIVE_LIST = 2,
	ITEM_ACTIVE_CHAR = 3,
	ITEM_ACTIVE_SIZE = 4
};

//! \brief The different option boxes that can be active for skills
enum SKILL_ACTIVE_OPTION {
	SKILL_ACTIVE_NONE = 0,
	SKILL_ACTIVE_CHAR = 1,
	SKILL_ACTIVE_CATEGORY = 2,
	SKILL_ACTIVE_LIST = 3,
	SKILL_ACTIVE_CHAR_APPLY = 4,
	SKILL_ACTIVE_SIZE = 5
};

//! \brief The different option boxes that can be active for equipment
enum EQUIP_ACTIVE_OPTION {
	EQUIP_ACTIVE_NONE = 0,
	EQUIP_ACTIVE_CHAR = 1,
	EQUIP_ACTIVE_SELECT = 2,
	EQUIP_ACTIVE_LIST = 3,
	EQUIP_ACTIVE_SIZE = 4
};

//! \brief The different option boxes that can be active for equipment
enum FORM_ACTIVE_OPTION {
	FORM_ACTIVE_NONE = 0,
	FORM_ACTIVE_CHAR = 1,
	FORM_ACTIVE_SECOND = 2,
	FORM_ACTIVE_SIZE = 3
};

//! \brief Possible values from the confirm window
enum CONFIRM_RESULT
{
	CONFIRM_RESULT_YES = 0,
	CONFIRM_RESULT_NO = 1,
	CONFIRM_RESULT_NOTHING = 2,
	CONFIRM_RESULT_CANCEL = 3,
};

/** ****************************************************************************
*** \brief Represents an individual character window
***
*** There should be one of these windows for each character in the game.
*** It will contain all the information of the character and handle its draw
*** placement.
*** ***************************************************************************/
class CharacterWindow : public hoa_gui::MenuWindow {
private:
	//! The name of the character that this window corresponds) to
	uint32 _char_id;

	//! The image of the character
	hoa_video::StillImage _portrait;

public:
	CharacterWindow();

	~CharacterWindow();

	/** \brief Set the character for this window
	*** \param character the character to associate with this window
	**/
	void SetCharacter(hoa_global::GlobalCharacter *character);

	/** \brief render this window to the screen
	*** \return success/failure
	**/
	void Draw();
}; // class CharacterWindow : public hoa_video::MenuWindow



/** ****************************************************************************
*** \brief Represents the inventory window to browse the party's inventory
***
*** This handles item use.  You can also view all items by category.
*** ***************************************************************************/
class InventoryWindow : public hoa_gui::MenuWindow {
	friend class hoa_menu::MenuMode;

public:
	InventoryWindow();

	~InventoryWindow();

	/** \brief Toggles the inventory window being in the active context for the player
	*** \param new_status Activates the inventory window when true, de-activates it when false
	**/
	void Activate(bool new_status);

	/** \brief Indicates whether the inventory window is in the active context
	*** \return True if the inventory window is in the active context
	**/
	bool IsActive()
		{ return _active_box; }

	//! If the inventory window is ready to cancel out, or cancel out a sub-window
	//bool CanCancel();

	/*!
	* \brief Updates the inventory window.  Handles key presses, switches window context, etc.
	*/
	void Update();

	/*!
	* \brief Draw the inventory window
	* \return success/failure
	*/
	void Draw();

private:
	//! Used for char portraits in bottom menu
	std::vector<hoa_video::StillImage> _portraits;

	//! Used for the current dungeon
	hoa_video::StillImage _location_graphic;

	//! Flag to specify the active option box
	uint32 _active_box;

	//! OptionBox to display all of the items
	hoa_gui::OptionBox _inventory_items;

	//! OptionBox to choose character
	hoa_gui::OptionBox _char_select;

	//! OptionBox to choose item category
	hoa_gui::OptionBox _item_categories;

	//! TextBox that holds the selected object's description
	hoa_gui::TextBox _description;

	//! Vector of GlobalObjects that corresponds to _inventory_items
	std::vector< hoa_global::GlobalObject* > _item_objects;

	/*!
	* \brief Updates the item text in the inventory items
	*/
	void _UpdateItemText();

	/*!
	* \brief Initializes inventory items option box
	*/
	void _InitInventoryItems();

	/*!
	* \brief Initializes char select option box
	*/
	void _InitCharSelect();

	/*!
	* \brief Initializes item category select option box
	*/
	void _InitCategory();

	template <class T> std::vector<hoa_global::GlobalObject*> _GetItemVector(std::vector<T*>* inv);
}; // class InventoryWindow : public hoa_video::MenuWindow



/** ****************************************************************************
*** \brief Represents the Status window, displaying all the information about the character.
***
*** This window display all the attributes of the character.
*** You can scroll through them all as well, to view all the different characters.
*** ***************************************************************************/
class StatusWindow : public hoa_gui::MenuWindow {
private:
	//! char portraits
	std::vector<hoa_video::StillImage> _full_portraits;

	//! if the window is active or not
	bool _char_select_active;

	//! character selection option box
	hoa_gui::OptionBox _char_select;

	/*!
	* \brief initialize character selection option box
	*/
	void _InitCharSelect();

public:

	StatusWindow();
	~StatusWindow();

	/*!
	* \brief render this window to the screen
	* \return success/failure
	*/
	void Draw();

	/*!
	* \brief update function handles input to the window
	*/
	void Update();

	/*!
	* \brief Check if status window is active
	* \return true if the window is active, false if it's not
	*/
	inline bool IsActive() { return _char_select_active; }

	/*!
	* \brief Active this window
	* \param new_value true to activate window, false to deactivate window
	*/
	void Activate(bool new_value);

}; // class StatusWindow : public hoa_video::MenuWindow



/** ****************************************************************************
*** \brief Represents the Skills window, displaying all the skills for the character.
***
*** This window display all the skills for a particular character.
*** You can scroll through them all, filter by category, choose one, and apply it
*** to a character.
*** ***************************************************************************/
class SkillsWindow : public hoa_gui::MenuWindow {
	friend class hoa_menu::MenuMode;

public:
	SkillsWindow();

	~SkillsWindow()
		{}

	/*!
	* \brief Updates key presses and window states
	*/
	void Update();

	/*!
	* \brief Draws the windows and option boxes
	* \return success/failure
	*/
	void Draw();

	/*!
	* \brief Activates the window
	* \param new_value true to activate window, false to deactivate window
	*/
	void Activate(bool new_status);

	/*!
	* \brief Checks to see if the skills window is active
	* \return true if the window is active, false if it's not
	*/
	bool IsActive()
		{ return _active_box; }

private:
	//! Flag to specify the active option box
	uint32 _active_box;

	//! The character select option box
	hoa_gui::OptionBox _char_select;

	//! The skills categories option box
	hoa_gui::OptionBox _skills_categories;

	//! The skills list option box
	hoa_gui::OptionBox _skills_list;

	//! The skill SP cost option box
	hoa_gui::OptionBox _skill_cost_list;

	//! TextBox that holds the selected skill's description
	hoa_gui::TextBox _description;

	//! Track which character's skillset was chosen
	int32 _char_skillset;

	/*!
	* \brief Initializes the skills category chooser
	*/
	void _InitSkillsCategories();

	/*!
	* \brief Initializes the skills chooser
	*/
	void _InitSkillsList();

	/*!
	* \brief Initializes the character selector
	*/
	void _InitCharSelect();

	//! \brief Returns the currently selected skill
	hoa_global::GlobalSkill *_GetCurrentSkill();

	/*!
	* \brief Sets up the skills that comprise the different categories
	*/
	void _UpdateSkillList();

	hoa_utils::ustring _BuildSkillListText(const hoa_global::GlobalSkill * skill);

	//! \brief parses the 3 skill lists of the global character and sorts them according to use (menu/battle)
	void _BuildMenuBattleSkillLists(std::vector<hoa_global::GlobalSkill *> *skill_list,
		std::vector<hoa_global::GlobalSkill *> *field, std::vector<hoa_global::GlobalSkill *> *battle,
		std::vector<hoa_global::GlobalSkill *> *all);

}; //class SkillsWindow : public hoa_video::MenuWindow


/** ****************************************************************************
*** \brief Represents the Equipment window, allowing the player to change equipment.
***
*** This window changes a character's equipment.
*** You can choose a piece of equipment and replace with an item from the given list.
*** ***************************************************************************/
class EquipWindow : public hoa_gui::MenuWindow {
	friend class hoa_menu::MenuMode;

public:
	EquipWindow();
	~EquipWindow();

	/*!
	* \brief Draws window
	* \return success/failure
	*/
	void Draw();

	/*!
	* \brief Performs updates
	*/
	void Update();

	/*!
	* \brief Checks to see if the equipment window is active
	* \return true if the window is active, false if it's not
	*/
	bool IsActive()
		{ return _active_box; }

	/*!
	* \brief Activates the window
	* \param new_value true to activate window, false to deactivate window
	*/
	void Activate(bool new_status);

private:

	//! Character selector
	hoa_gui::OptionBox _char_select;

	//! Equipment selector
	hoa_gui::OptionBox _equip_select;

	//! Replacement selector
	hoa_gui::OptionBox _equip_list;

	//! Flag to specify the active option box
	uint32 _active_box;

	//! equipment images
	std::vector<hoa_video::StillImage> _equip_images;

	/*!
	* \brief Set up char selector
	*/
	void _InitCharSelect();

	/*!
	* \brief Set up equipment selector
	*/
	void _InitEquipmentSelect();

	/*!
	* \brief Set up replacement selector
	*/
	void _InitEquipmentList();

	/*!
	* \brief Updates the equipment list
	*/
	void _UpdateEquipList();

}; // class EquipWindow : public hoa_video::MenuWindow



/** ****************************************************************************
*** \brief Represents the Formation window, allowing the party to change order.
***
*** This window changes party order.
*** ***************************************************************************/
class FormationWindow : public hoa_gui::MenuWindow {
	friend class hoa_menu::MenuMode;

public:
	FormationWindow();
	~FormationWindow();
	void Update();
	void Draw();

	/*!
	* \brief Activates the window
	* \param new_value true to activate window, false to deactivate window
	*/
	void Activate(bool new_status);

	/*!
	* \brief Checks to see if the skills window is active
	* \return true if the window is active, false if it's not
	*/
	bool IsActive()
		{ return _active_box; }

private:
	//! Flag to specify the active option box
	uint32 _active_box;

	//! The character select option box
	hoa_gui::OptionBox _char_select;

	//! The character select option box once first character has been selected
	hoa_gui::OptionBox _second_char_select;

	/*!
	* \brief initialize character selection option box
	*/
	void _InitCharSelect();

}; // class FormationWindow : public hoa_video::MenuWindow

/*!
* \brief Converts a vector of GlobalItem*, etc. to a vector of GlobalObjects*
* \return the same vector, with elements of type GlobalObject*
*/
template <class T> std::vector<hoa_global::GlobalObject*> InventoryWindow::_GetItemVector(std::vector<T*>* inv) {
	std::vector<hoa_global::GlobalObject*> obj_vector;

	for (typename std::vector<T*>::iterator i = inv->begin(); i != inv->end(); i++) {
		obj_vector.push_back( *i );
	}

	return obj_vector;
}

} // namespace private_menu

/** **************************************************************************
*** \brief A window to display a message to the player
*** Displays a message to the user in the center of the screen
*** This class is not private because it's a handy message box and
*** it could be used else where.
*** **************************************************************************/
class MessageWindow : public hoa_gui::MenuWindow
{
public:
	MessageWindow(const hoa_utils::ustring &message, float w, float h);
	~MessageWindow();

	//! \brief Set the text to display in the window
	void SetText(const hoa_utils::ustring &message)
	{ _message = message; _textbox.SetDisplayText(message); }

	//! \brief Standard Window Functions
	//@{
	void Draw();
	//@}

private:
	//! \brief the message to display
	hoa_utils::ustring _message;

	//! \brief used to display the message
	hoa_gui::TextBox _textbox;
}; // class MessageWindow

} // namespace hoa_menu

#endif
