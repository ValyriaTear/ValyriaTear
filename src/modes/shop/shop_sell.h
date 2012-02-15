///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_sell.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for sell interface of shop mode
***
*** \note The contents of this file are near identical to the contents of
*** shop_buy.h. When making any changes to this file, please look to shop_buy.h
*** to see if it should have similar changes made.
*** ***************************************************************************/

#ifndef __SHOP_SELL_HEADER__
#define __SHOP_SELL_HEADER__

#include "defs.h"
#include "utils.h"

#include "video.h"
#include "global.h"

#include "shop_utils.h"

namespace hoa_shop {

namespace private_shop {

/** ****************************************************************************
*** \brief Manages the shop when it is in sell mode and enables the player to sell their inventory
***
*** This interface displays the list of objects that are available for the player to sell.
*** It organizes these objects based on their object type/category (item, weapon, etc.) and
*** allows the player to switch between views of these different categories. This interface
*** also displays information about the currently selected object such as its description,
*** statistical ratings, usable characters, etc.
***
*** \note The player is never allowed to sell key items, even if the shop has key
*** items available for purchase. A shop that only deals in key items means that
*** no categories would be available on the sell interface. As the code stands now, this
*** particular case would most certainly cause shop mode to crash upon entering the sell
*** interface.
*** ***************************************************************************/
class SellInterface : public ShopInterface {
public:
	SellInterface();

	~SellInterface();

	//! \brief Initializes the data conatiners and GUI objects to be used
	void Initialize();

	//! \brief Sets the selected object for the ShopObjectViewer class
	void MakeActive();

	//! \brief Completely reconstructs all display lists from the party's inventory
	void TransactionNotification();

	//! \brief Processes user input and sends appropriate commands to helper class objects
	void Update();

	//! \brief Draws the GUI elements to the screen
	void Draw();

private:
	//! \brief Stores the active view state of the sell interface
	SHOP_VIEW_MODE _view_mode;

	//! \brief A pointer to the currently selected object in the active list display
	ShopObject* _selected_object;

	//! \brief Retains the number of object categories available to sell
	uint32 _number_categories;

	//! \brief Serves as an index to the following containers: _object_data, _category_names, _category_icons, and _list_displays
	uint32 _current_category;

	//! \brief Header text for the category field
	hoa_video::TextImage _category_header;

	//! \brief Header text for the name field
	hoa_video::TextImage _name_header;

	//! \brief Header text for the list of object properties (refer to the SellListDisplay class)
	hoa_gui::OptionBox _properties_header;

	//! \brief String representations of all category names that may
	std::vector<hoa_utils::ustring> _category_names;

	//! \brief A pointer to icon images
	std::vector<hoa_video::StillImage*> _category_icons;

	//! \brief Display manager for the current category of objects selected
	ObjectCategoryDisplay _category_display;

	/** \brief Class objects used to display the object data to the player
	*** The size and contents of this container mimic that which is found in the _object_data container.
	**/
	std::vector<SellListDisplay*> _list_displays;

	//! \brief A copy of the selected object's icon, scaled to 1/4 size
	hoa_video::StillImage _selected_icon;

	//! \brief Text image of the selected object's name
	hoa_video::TextImage _selected_name;

	//! \brief A single row option box containing the selected object's properties
	hoa_gui::OptionBox _selected_properties;

	/** \brief Clears out all list displays and rebuilds them from scratch using the party's inventory
	*** This operation needs to be performed when the interface is initialized and whenever a transaction
	*** occurs. The latter case is necessary because in a transaction, the player may have bought new
	*** objects which should be made available to sell immediately, or the player may have sold off all
	*** counts of objects that they already had and thus these objects should not appear on the sell list
	*** anymore.
	**/
	void _PopulateLists();

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
}; // class SellInterface : public ShopInterface


/** ****************************************************************************
*** \brief A display class that manages and draws lists of objects that may be marked to sell
***
*** The "identify" list contains a 0.25x size icon of the object and the object's name.
*** The "properties" list contains price, shop stock, amount owned by the player, and
*** requested sell quantity. Only sell quantity requires regular refreshing based upon
*** the player's actions while the sell interface is active.
*** ***************************************************************************/
class SellListDisplay : public ObjectListDisplay {
public:
	SellListDisplay()
		{}

	~SellListDisplay()
		{}

	//! \brief Reconstructs all option box entries from the object data
	void ReconstructList();

	/** \brief Will refresh the displayed sell count property for a single list entry
	*** \param index The index of the object data to reconstruct
	***
	*** The reason that only sell quantity is refreshed is that no other property data needs to be
	*** updated while in the sell interface. All other data remains static and require updating
	*** only after a shop transaction is completed.
	**/
	void RefreshEntry(uint32 index);

	/** \brief Changes the sell count of the selected object, refreshes the list entry, and updates financial totals
	*** \param less_or_more False to decrease the quantity, true to increase it
	*** \param amount The amount to decrease/increase the quantity by (default value == 1)
	*** \return False if no quantity change could take place, true if a quantity change did occur
	*** \note Even if the function returns true, there is no guarantee that the requested amount
	*** was fully met. For example, if the function is asked to increase the sell quantity by 8 but
	*** the player only has 3 instances of the selected object in inventory, the function will increase
	*** the quantity by 3 (not 8) and return true.
	**/
	bool ChangeSellQuantity(bool less_or_more, uint32 amount = 1);
}; // class SellListDisplay : public ObjectListDisplay

} // namespace private_shop

} // namespace hoa_shop

#endif // __SHOP_SELL_HEADER__
