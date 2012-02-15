///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_utils.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for shop mode utility code.
***
*** This file contains utility code that is shared among the various shop mode
*** classes.
*** ***************************************************************************/

#ifndef __SHOP_UTILS_HEADER__
#define __SHOP_UTILS_HEADER__

#include "defs.h"
#include "utils.h"

#include "system.h"
#include "video.h"
#include "gui.h"

namespace hoa_shop {

//! \brief The five possible price ratings/levels for the shop's buy/sell prices
enum SHOP_PRICE_LEVEL {
	SHOP_PRICE_INVALID   = -1,
	SHOP_PRICE_VERY_GOOD =  0,
	SHOP_PRICE_GOOD      =  1,
	SHOP_PRICE_STANDARD  =  2,
	SHOP_PRICE_POOR      =  3,
	SHOP_PRICE_VERY_POOR =  4,
	SHOP_PRICE_TOTAL     =  5
};

namespace private_shop {

//! \brief Used to indicate what window has control of user input
enum SHOP_STATE {
	SHOP_STATE_INVALID   = -1,
	SHOP_STATE_ROOT      =  0,
	SHOP_STATE_BUY       =  1,
	SHOP_STATE_SELL      =  2,
	SHOP_STATE_TRADE     =  3,
	SHOP_STATE_CONFIRM   =  4,
	SHOP_STATE_LEAVE     =  5,
	SHOP_STATE_TOTAL     =  6
};

//! \brief Used to toggle between different view models in some interfaces
enum SHOP_VIEW_MODE {
	SHOP_VIEW_MODE_INVALID   = -1,
	SHOP_VIEW_MODE_LIST      =  0, //!< Display lists of information, objects, and options
	SHOP_VIEW_MODE_INFO      =  1, //!< Display detailed information about a particular object/selection
	SHOP_VIEW_MODE_TOTAL     =  2
};

/** \brief Type identifiers for representing different categories of shop objects
*** GlobalObject classes already have their own type member, so why create a type for ShopObject classes?
*** The answer is that shop interfaces often present data about certain object types in exactly the same manner
*** and in many cases its convenient to refer to these common types by one name. The ShopObject class,
*** does not have a SHOP_OBJECT member but does have a static function for converting a GLOBAL_OBJECT
*** to a SHOP_OBJECT.
**/
enum SHOP_OBJECT {
	SHOP_OBJECT_INVALID    = -1,
	SHOP_OBJECT_ITEM       =  0,
	SHOP_OBJECT_EQUIPMENT  =  1, //!< This type covers weapons and all types of armor
	SHOP_OBJECT_SHARD      =  2,
	SHOP_OBJECT_KEY_ITEM   =  3,
	SHOP_OBJECT_TOTAL      =  4
};

//! \brief The number to add/subtract from buy/sell count when the player uses the increase/decrease batch commands
const uint32 SHOP_BATCH_COUNT = 10;

//! \name Price multipliers
//! \brief These values are multiplied by an object's standard price to get the price for the desired price level
//@{
const float BUY_PRICE_VERY_GOOD   = 1.2f;
const float BUY_PRICE_GOOD        = 1.4f;
const float BUY_PRICE_STANDARD    = 1.6f;
const float BUY_PRICE_POOR        = 1.8f;
const float BUY_PRICE_VERY_POOR   = 2.0f;

const float SELL_PRICE_VERY_GOOD  = 0.9f;
const float SELL_PRICE_GOOD       = 0.8f;
const float SELL_PRICE_STANDARD   = 0.7f;
const float SELL_PRICE_POOR       = 0.6f;
const float SELL_PRICE_VERY_POOR  = 0.5f;
//@}

//! \name Object deal types
//! \brief Constants used to determine the types of merchandise that the shop deals with
//@{
const uint8 DEALS_ITEMS        = 0x01;
const uint8 DEALS_WEAPONS      = 0x02;
const uint8 DEALS_HEAD_ARMOR   = 0x04;
const uint8 DEALS_TORSO_ARMOR  = 0x08;
const uint8 DEALS_ARM_ARMOR    = 0x10;
const uint8 DEALS_LEG_ARMOR    = 0x20;
const uint8 DEALS_SHARDS       = 0x40;
const uint8 DEALS_KEY_ITEMS    = 0x80;
//@}


/** ***************************************************************************
*** \brief Abstract class for shop interfaces
***
*** Shop interface classes are manager classes for a particular state of shop
*** mode. All interface classes inherit from this abstract class. All interfaces
*** are initialized only once after the ShopMode class finishes its own initialization
*** routine.
*** **************************************************************************/
class ShopInterface {
public:
	ShopInterface()
		{}

	virtual ~ShopInterface()
		{}

	//! \brief Performs any initialization that could not be done when the class was constructed
	virtual void Initialize() = 0;

	//! \brief Invoked to notify when the shop state has changed and the interface has become active
	virtual void MakeActive() = 0;

	//! \brief Invoked whenever a transaction is made to allow the interface to update itself appropriately
	virtual void TransactionNotification() = 0;

	//! \brief Updates the state of the interface and operates on user input
	virtual void Update() = 0;

	//! \brief Draws the interface's contents to the screen
	virtual void Draw() = 0;
}; // class ShopInterface


/** ***************************************************************************
*** \brief Represents objects that are bought, sold, and traded within the shop
***
*** This class wraps around a GlobalObject and uses additional members that are
*** properties of the object specific to shopping. The ShopMode class maintains
*** containers of these objects and the various interfaces perform modifications
*** to their properties.
***
*** \note Be careful with assigning the GlobalObject pointer in the class constructor.
*** The object pointed to, if it exists in the global party inventory, will be deleted
*** if all counts to this member are removed from the inventory. Therefore never use
*** a GlobalObject inventory pointer if you don't have to (use the ones that ShopMode
*** creates for all objects being sold in the shop) and if a sell count goes to zero,
*** delete the corresponding ShopObject.
***
*** \note The data in this class is used to determine if this object should be visible
*** in buy and/or sell lists.
*** **************************************************************************/
class ShopObject {
public:
	/** \param object A pointer to a valid GlobalObject instance that the shop object will represent
	*** \param sold_by_shop True if this object is offered for sale by the shop
	**/
	ShopObject(hoa_global::GlobalObject* object, bool sold_by_shop);

	~ShopObject()
		{}

	/** \brief Determines the appropriate SHOP_OBJECT that corresponds to a GLOBAL_OBJECT
	*** \param global_type The global object type to find the equivalent shop object type for
	**/
	static SHOP_OBJECT DetermineShopObjectType(hoa_global::GLOBAL_OBJECT global_type);

	//! \brief Returns the shop object type of this object
	SHOP_OBJECT DetermineShopObjectType();

	/** \brief Sets the buy and sell prices for the object
	*** \param buy_level The buy level of the shop that will determine its buy price
	*** \param sell_level The sell level of the shop that will determine its sell price
	**/
	void SetPricing(SHOP_PRICE_LEVEL buy_level, SHOP_PRICE_LEVEL sell_level);

	//! \name Class member accessor functions
	//@{
	hoa_global::GlobalObject* GetObject() const
		{ return _object; }

	bool IsSoldInShop() const
		{ return _sold_in_shop; }

	uint32 GetBuyPrice() const
		{ return _buy_price; }

	uint32 GetSellPrice() const
		{ return _sell_price; }

	uint32 GetOwnCount() const
		{ return _own_count; }

	uint32 GetStockCount() const
		{ return _stock_count; }

	uint32 GetBuyCount() const
		{ return _buy_count; }

	uint32 GetSellCount() const
		{ return _sell_count; }

	void ResetBuyCount()
		{ _buy_count = 0; }

	void ResetSellCount()
		{ _sell_count = 0; }
	//@}

	/** \name Increment and Decrement Functions
	*** \brief Increments or decrements the value of the various count members
	*** \param inc/dec The amount to decrement the count by (default value == 1)
	***
	*** These functions increment or decrement the respective count members. Checks are performed
	*** to prevent error conditions from occurring. For example, the buy count can not be greater
	*** than the stock count and the sell count can not be greater than the own count. None of the
	*** count members will be allowed to decrement below zero. Overflow conditions however are not
	*** checked. Should any error condition occur, a warning message will be printed and the value
	*** of the count member will not be modified.
	**/
	//@{
	void IncrementOwnCount(uint32 inc = 1);
	void IncrementStockCount(uint32 inc = 1);
	void IncrementBuyCount(uint32 inc = 1);
	void IncrementSellCount(uint32 inc = 1);
	void DecrementOwnCount(uint32 dec = 1);
	void DecrementStockCount(uint32 dec = 1);
	void DecrementBuyCount(uint32 dec = 1);
	void DecrementSellCount(uint32 dec = 1);
	//@}

private:
	//! \brief A pointer to the global object represented by this
	hoa_global::GlobalObject* _object;

	//! \brief Set to true if the player is able to buy this object from the shop
	bool _sold_in_shop;

	//! \brief The price that the player must pay to buy this object from the shop
	uint32 _buy_price;

	//! \brief The return that the player will receive for selling this object to the shop
	uint32 _sell_price;

	//! \brief The number of this object that the player's party currently owns
	uint32 _own_count;

	//! \brief The stock of this object that the shop
	uint32 _stock_count;

	//! \brief The amount of this object that the player plans to purchase
	uint32 _buy_count;

	//! \brief The amount of this object that the player plans to sell
	uint32 _sell_count;
}; // class ShopObject


/** ****************************************************************************
*** \brief Displays text and an icon image to represent an object category
***
*** This class is used to display an icon image and name representing a category.
*** It is designed to support the needs of both the buy and sell interfaces of
*** shop mode and behaves slightly differently based on the current view mode.
*** When in the "list" view mode, the name + icon are drawn to the left side of
*** the middle window. Cycling through categories while in this view mode will
*** transition the text and icon graphics smoothly by fading the icon from one
*** to the next and visibly refreshing the line of text.
***
*** While in the "info" view mode, the display of the category and name text is
*** done instantly. This view mode will draw the contents to the left side of the
*** bottom window. The icon and name can also be different than what is show in
*** list mode, because the category name/icon drawn in info mode depends entirely
*** upon the type of object selected. For example, while the list view mode may
*** show a text/icon representing "all wares", in info view mode all wares is
*** never displayed because it does not represent a particular type of object.
*** ***************************************************************************/
class ObjectCategoryDisplay {
public:
	ObjectCategoryDisplay();

	~ObjectCategoryDisplay();

	//! \brief Must be called so that the TextBox can proceed
	void Update();

	//! \brief Draws the graphics and text to the screen
	void Draw();

	/** \brief Changes the viewing mode for the display and updates display state appropriately
	*** \param new_mode The mode to change the view to
	**/
	void ChangeViewMode(SHOP_VIEW_MODE new_mode);

	/** \brief Sets the selected object and may also render a new text image
	*** \param shop_object A pointer to the new object. A NULL pointer is acceptable
	**/
	void SetSelectedObject(ShopObject* shop_object);

	/** \brief Sets the category text and icon for diplay in "list" view mode
	*** \param name The text to display that represents the category's name
	*** \param icon A pointer to the new image to represent the category's icon
	*** \note This function does not check whether or not the new name/icon are not
	*** the same as the previous name/icon
	*** \note It is safe to pass a NULL pointer for the icon argument
	**/
	void ChangeCategory(hoa_utils::ustring& name, const hoa_video::StillImage* icon);

protected:
	//! \brief Determines where and how the category display should draw its images
	SHOP_VIEW_MODE _view_mode;

	//! \brief A pointer to the selected object whose category should be represented
	ShopObject* _selected_object;

	//! \brief A pointer to an icon image representing the current category
	const hoa_video::StillImage* _current_icon;

	//! \brief A pointer to the icon image that represents the previous category
	const hoa_video::StillImage* _last_icon;

	//! \brief The category image for the selected object
	const hoa_video::StillImage* _object_icon;

	//! \brief The name of the current category for "list" view mode
	hoa_gui::TextBox _name_textbox;

	//! \brief The name of the current category for "info" view mode
	hoa_video::TextImage _name_text;

	//! \brief A timer used to track the progress of category transitions
	hoa_system::SystemTimer _transition_timer;
}; // class ObjectCategoryDisplay


/** ****************************************************************************
*** \brief An abstract class for displaying a list of shop objects
***
*** This class is used to display a list of shop objects to the user along with
*** certain properties. It uses two OptionBox objects to achieve this, which are
*** placed side by side. The left list, "identify", contains indentifying information
*** of the shop object, usually its name and image icon. The right list, "properties",
*** contains several properties of the object. Both lists have the same number of rows,
*** where a row represents a single object, the identify list has only a single column
*** while the property list has four columns.
***
*** The deriving class determines what data is placed in both the identify and property
*** lists. It will define the ReconstructList() and RefreshEntry() methods to achieve
*** this. The lists are always drawn on the right side of the middle shop window and
*** display no more than eight entries at a time. If desired, a deriving class can
*** retrieve references to the OptionBox objects representing both lists in this class
*** and change their default properties to fit one's needs.
***
*** \note Because the data used to construct this list are a series of ShopObject pointers,
*** this means that the object data can and often is modified outside of this class. It is
*** up to the user of this class to determine when the object data has been modified and
*** to refresh the list data as appropriate.
*** ***************************************************************************/
class ObjectListDisplay {
public:
	ObjectListDisplay();

	virtual ~ObjectListDisplay()
		{ Clear(); }

	/** \brief Removes all entries from the option boxes
	*** \note This will also clear all entries from the _objects member, so usually calling this
	*** function should be followed by invoking PopulateList() to refill the class with valid data.
	**/
	void Clear();

	/** \brief Clears the lists and then reconstructs them using the option box data given
	*** \param objects A reference to a data vector containing the objects to populate the list with
	**/
	void PopulateList(std::vector<ShopObject*>& objects);

	//! \brief Reconstructs all option box entries from the object data
	virtual void ReconstructList() = 0;

	/** \brief Refreshes the desired properties of a single object
	*** \param index The row index of the object data to reconstruct
	**/
	virtual void RefreshEntry(uint32 index) = 0;

	/** \brief Refreshes the desired properties of all objects in the list
	*** The difference between this method and the ReconstructList() method is that this method only
	*** operates on the object's desired properties by calling RefreshEntry(), whereas ReconstructList()
	*** clears and rebuilds the entire list from scratch. Using this method is much less costly than
	*** reconstructing the entire list.
	**/
	virtual void RefreshAllEntries();

	/** \brief Returns a pointer to the currently selected shop object
	*** This method may return NULL if the current selection is invalid or the _objects container is empty
	**/
	ShopObject* GetSelectedObject();

	//! \brief Resets list selection to the first entry
	void ResetSelection();

	/** \brief Returns the index of the currently selected object in the list
	*** \return The selected index. Will return 0 if there is no valid selection (if the list is empty)
	**/
	uint32 GetCurrentSelection();

	//! \brief Inputs an up command to change the current selection
	void InputUp();

	//! \brief Inputs a down command to change the current selection
	void InputDown();

	//! \brief Returns true if the list contains no entries
	bool IsListEmpty() const
		{ return _objects.empty(); }

	//! \brief Updates the option boxes
	void Update();

	//! \brief Draws the option boxes
	void Draw();

	//! \name Class member access methods
	//@{
	hoa_gui::OptionBox& GetIdentifyList()
		{ return _identify_list; }

	hoa_gui::OptionBox& GetPropertyList()
		{ return _property_list; }
	//@}

protected:
	//! \brief The vector of object data that the class is to display
	std::vector<ShopObject*> _objects;

	//! \brief Contains identification information about each objects such as graphical icon and name
	hoa_gui::OptionBox _identify_list;

	//! \brief Contains properties about the object such as price, stock, amount owned, or amount to buy/sell
	hoa_gui::OptionBox _property_list;
}; // class ObjectListDisplay

} // namespace private_shop

} // namespace hoa_shop

#endif // __SHOP_UTILS_HEADER__
