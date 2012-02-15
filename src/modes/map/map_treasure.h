///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_treasure.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for map mode treasures.
*** *****************************************************************************/

#ifndef __MAP_TREASURE_HEADER__
#define __MAP_TREASURE_HEADER__

// Allacrost utilities
#include "utils.h"
#include "defs.h"

// Allacrost engines
#include "video.h"

// Allacrost common code
#include "gui.h"

// Local map mode headers
#include "map_utils.h"

namespace hoa_map {

namespace private_map {

/** ****************************************************************************
*** \brief A container class for treasures procured by the player
***
*** Treasures may contain multiple quantities and types of items, weapons, armor,
*** or any other type of global object. They may additionally contain any amount
*** of drunes (money). As one would expect, the contents of a treasure should only
*** be retrieved by the player one time. This class holds a member for tracking whether
*** the treasure has been taken or not, but it is not responsible for determining
*** if a treasure was taken in the past (by a previous visit to the map or from the
*** saved game file).
*** ***************************************************************************/
class MapTreasure {
	friend class TreasureSupervisor;

public:
	MapTreasure();

	~MapTreasure();

	/** \brief Adds a number of drunes to be the chest's contents
	*** \note The overflow condition is not checked here: we just assume it will never occur
	**/
	void AddDrunes(uint32 amount)
		{ _drunes += amount; }

	/** \brief Adds an object to the contents of the MapTreasure
	*** \param id The id of the GlobalObject to add
	*** \param quantity The number of the object to add (default == 1)
	*** \return True if the object was added succesfully
	**/
	bool AddObject(uint32 id, uint32 quantity = 1);

	//! \name Class member access methods
	//@{
	bool IsTaken() const
		{ return _taken; }

	void SetTaken(bool taken)
		{ _taken = taken; }
	//@}

private:
	//! \brief Set to true when the contents of the treasure have been added to the player's inventory
	bool _taken;

	//! \brief The number of drunes contained in the chest
	uint32 _drunes;

	//! \brief The list of objects given to the player upon opening the treasure
	std::vector<hoa_global::GlobalObject*> _objects_list;
}; // class MapTreasure : public PhysicalObject


/** ***************************************************************************************
*** \brief Displays the contents of a discovered treasure in a menu window
***
*** Upon opening a treasure chest or other treasure-containing map object, this menu
*** will appear and list the amount of drunes found (if any), a list of the icon and name of
*** each GlobalObject found (items, equipment, etc), and a list of player options.
*** The player may select to view detailed information about a particular entry, go to menu mode,
*** and possibly other actions in the future.
***
*** The treasure menu is composed of three sets of windows. The action window is a small window
*** at the top of the menu that displays the action options in a horizontal list. The list window
*** displays the contents of the treasure in a larger window below the action window. This object
*** list is formatted vertically. The detail window shares the same area as the list window and
*** displays textual and visual detail about an object selected by the user from the list window.
***
*** Proper utilization of this class entails the following steps:
***
*** -# Call the Initialize method to show the menu with the treasure that has been obtained
*** -# Call the Update method to process user input and update the menu's appearance
*** -# Call the Draw method to draw the menu to the screen
*** -# Call the Finish method to hide the menu and add the treasure's contents to the player's
***    inventory
***
*** \todo Allow the player to use or equip selected treasure objects directly from the
*** action menu.
***
*** \todo Add visual scissoring to the list window so that the option list or detail text does
*** not exceed the window boundary when the text or list is exceedingly long.
***
*** \todo Instead of forcing the detail window to share the list window, maybe it would look
*** better if there was a separate detail window which "popped out" of the other two windows
*** and could be placed over them when it was visible? I think this would be much more visually
*** pleasing than the current implementation.
*** **************************************************************************************/
class TreasureSupervisor {
public:
	//! \brief The possible sub-windows that are selected, used for determining how to process user input
	enum SELECTION {
		ACTION_SELECTED = 0, //!< the list of actions a user may take in the treasure menu
		LIST_SELECTED = 1,   //!< active when the user is browsing the list of treasures
		DETAIL_SELECTED = 2  //!< set when the user is viewing details about a particular treasure
	};

	TreasureSupervisor();

	~TreasureSupervisor();

	/** \brief Displays the menu window and initializes it to display the contents of a new treasure
	*** \param map_object A pointer to the object on the map holding the treasure to procure
	**/
	void Initialize(TreasureObject* map_object);

	/** \brief Displays the menu window and initializes it to display the contents of a new treasure
	*** \param treasure A pointer to the treasure to display the contents of
	**/
	void Initialize(MapTreasure* treasure);

	//! \brief Processes input events from the user and updates the showing/hiding progress of the window
	void Update();

	/** \brief Draws the window to the screen
	*** \note If the Initialize method has not been called with a valid treasure pointer beforehand, this
	*** method will print a warning and it will not draw anything to the screen.
	**/
	void Draw();

	//! \brief Hides the window and adds the treasure's contents to the player's inventory
	void Finish();

	//! \brief Returns true if the treasure menu is active
	bool IsActive() const
		{ return (_treasure != NULL); }

private:
	//! \brief A pointer to the treasure object to display the contents of
	MapTreasure* _treasure;

	//! \brief The currently selected sub-window for processing user input
	SELECTION _selection;

	//! \brief A vector containing pointers to objects which should be deleted upon finishing with the current treasure
	std::vector<hoa_global::GlobalObject*> _objects_to_delete;

	//! \brief Contains options for viewing, using, or equipping inventory, or for exiting the menu
	hoa_gui::MenuWindow _action_window;

	//! \brief Lists all of the drunes and inventory objects contained in the treasure
	hoa_gui::MenuWindow _list_window;

	//! \brief The available actions that a user can currently take. Displayed in the _action_window.
	hoa_gui::OptionBox _action_options;

	//! \brief The name + quantity of all drunes and inventory objects earned. Displayed in the _list_window
	hoa_gui::OptionBox _list_options;

	//! \brief A textbox that displays the detailed description about a selected treasure
	hoa_gui::TextBox _detail_textbox;

	//! \brief A rendering of the name for the treasure window
	hoa_video::TextImage _window_title;

	//! \brief The name of the selected list item
	hoa_video::TextImage _selection_name;

	//! \brief A pointer to the image of the selected list item
	hoa_video::StillImage* _selection_icon;

	//! \brief Holds the icon image that represent drunes
	hoa_video::StillImage _drunes_icon;

	// ---------- Private methods

	//! \brief Processes user input when the action sub-window is selected
	void _UpdateAction();

	//! \brief Processes user input when the list sub-window is selected
	void _UpdateList();

	//! \brief Processes user input when the detailed view of a treasure object is selected
	void _UpdateDetail();
}; // class TreasureSupervisor

} // namespace private_map

} // namespace hoa_map

#endif // __MAP_TREASURE_HEADER__
