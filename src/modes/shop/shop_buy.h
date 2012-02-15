///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_buy.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for buy interface of shop mode
***
*** \note The contents of this file are near identical to the contents of
*** shop_sell.h. When making any changes to this file, please look to shop_sell.h
*** to see if it should have similar changes made.
*** ***************************************************************************/

#ifndef __SHOP_BUY_HEADER__
#define __SHOP_BUY_HEADER__

#include "defs.h"
#include "utils.h"

#include "video.h"
#include "global.h"

#include "shop_utils.h"

namespace hoa_shop {

namespace private_shop {

/** ****************************************************************************
*** \brief Manages the shop when it is in buy mode and enables the player to view and purchase wares
***
*** This interface displays the list of objects that are available for sale. It organizes
*** these objects based on their object type/category (item, weapon, etc.) and allows the
*** player to switch between views of these different categories. This interface also displays
*** information about the currently selected object such as its description, statistical
*** ratings, usable characters, etc.
*** ***************************************************************************/
class BuyInterface : public ShopInterface {
public:
	BuyInterface();

	~BuyInterface();

	//! \brief Initializes the data conatiners and GUI objects to be used
	void Initialize();

	//! \brief Sets the selected object for the ShopObjectViewer class
	void MakeActive();

	//! \brief Reconstructs all buy display lists and resets the current category
	void TransactionNotification();

	//! \brief Processes user input and sends appropriate commands to helper class objects
	void Update();

	//! \brief Draws the GUI elements to the screen
	void Draw();

private:
	//! \brief Stores the active view state of the buy interface
	SHOP_VIEW_MODE _view_mode;

	//! \brief A pointer to the currently selected object in the active list display
	ShopObject* _selected_object;

	//! \brief Retains the number of object categories for sale
	uint32 _number_categories;

	//! \brief Serves as an index to the following containers: _category_names, _category_icons, and _list_displays
	uint32 _current_category;

	//! \brief Header text for the category field
	hoa_video::TextImage _category_header;

	//! \brief Header text for the name field
	hoa_video::TextImage _name_header;

	//! \brief Header text for the list of object properties (refer to the BuyListDisplay class)
	hoa_gui::OptionBox _properties_header;

	//! \brief String representations of all object categories where purchases are available
	std::vector<hoa_utils::ustring> _category_names;

	//! \brief A pointer to the icon image for each purchasable object category
	std::vector<hoa_video::StillImage*> _category_icons;

	//! \brief Display manager for the current category of objects selected
	ObjectCategoryDisplay _category_display;

	//! \brief Class objects used to display the object data to the player
	std::vector<BuyListDisplay*> _list_displays;

	//! \brief A copy of the selected object's icon, scaled to 1/4 size
	hoa_video::StillImage _selected_icon;

	//! \brief Text image of the selected object's name
	hoa_video::TextImage _selected_name;

	//! \brief A single row option box containing the selected object's properties
	hoa_gui::OptionBox _selected_properties;

	/** \brief Takes all necessary action for when the active view mode is to be altered
	*** \param new_mode The new view mode to set
	**/
	void _ChangeViewMode(SHOP_VIEW_MODE new_mode);

	/** \brief Changes the current category and object list that is being displayed
	*** \param left_or_right False to move the category to the left, or true for the right
	*** \return True if the _selected_object member has changed
	**/
	bool _ChangeCategory(bool left_or_right);

	/** \brief Changes the current selection in the object list
	*** \param up_or_down False to move the selection cursor up, or true to move it down
	*** \return True if the _selected_object member has changed
	**/
	bool _ChangeSelection(bool up_or_down);

	/** \brief Refreshes the text in the _selected_properties OptionBox
	*** This method only needs to be called when the properties (likely quantity) change
	*** in the "info" view mode. Calling it while in "list" view is wasted effort
	**/
	void _RefreshSelectedProperties();
}; // class BuyInterface : public ShopInterface


/** ****************************************************************************
*** \brief A display class that manages and draws lists of objects that may be marked to buy
***
*** The "identify" list contains a 0.25x size icon of the object and the object's name.
*** The "properties" list contains price, shop stock, amount owned by the player, and
*** requested buy quantity. Only buy quantity requires regular refreshing based upon
*** the player's actions while the buy interface is active.
*** ***************************************************************************/
class BuyListDisplay : public ObjectListDisplay {
public:
	BuyListDisplay()
		{}

	~BuyListDisplay()
		{}

	//! \brief Reconstructs all option box entries from the object data
	void ReconstructList();

	/** \brief Will refresh the displayed buy count property for a single list entry
	*** \param index The index of the object data to reconstruct
	***
	*** The reason that only buy quantity is refreshed is that no other property data needs to be
	*** updated while in the buy interface. All other data remains static and require updating
	*** only after a shop transaction is completed.
	**/
	void RefreshEntry(uint32 index);

	/** \brief Changes the buy count of the selected object, refreshes the list entry, and updates financial totals
	*** \param less_or_more False to decrease the quantity, true to increase it
	*** \param amount The amount to decrease/increase the quantity by (default value == 1)
	*** \return False if no quantity change could take place, true if a quantity change did occur
	*** \note Even if the function returns true, there is no guarantee that the requested amount
	*** was fully met. For example, if the function is asked to increase the buy quantity by 10 but
	*** the shop only has 6 instances of the selected object in stock, the function will increase
	*** the quantity by 6 (not 10) and return true.
	**/
	bool ChangeBuyQuantity(bool less_or_more, uint32 amount = 1);
}; // class BuyListDisplay : public ObjectListDisplay

} // namespace private_shop

} // namespace hoa_shop

#endif // __SHOP_BUY_HEADER__
