///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for shop mode interface
***
*** This code provides an interface for the user to purchase wares from a
*** merchant. This mode is usually entered from a map after speaking with a
*** shop keeper.
*** ***************************************************************************/

#ifndef __SHOP_HEADER__
#define __SHOP_HEADER__

#include "defs.h"
#include "utils.h"

#include "mode_manager.h"

#include "global.h"

#include "shop_utils.h"

//! \brief All calls to shop mode are wrapped in this namespace.
namespace hoa_shop {

//! \brief Determines whether the code in the hoa_shop namespace should print debug statements or not.
extern bool SHOP_DEBUG;

//! \brief An internal namespace to be used only within the shop code. Don't use this namespace anywhere else!
namespace private_shop {

/** ****************************************************************************
*** \brief A companion class to ShopMode that holds various multimedia data
***
*** All of the shop mode interfaces share media data in forming their presentations.
*** This class retains all of this common media data and makes it available for
*** the shop interfaces to utilize.
***
*** \note The accessor methods return non-const pointers to the relevant data
*** structures and objects. This is done for reasons of convenience as many places
*** in the shop code wish to make non-const copies of data stored in this class.
*** Take care not to abuse these pointers and modify the state of any of the data
*** members in this class as it would have negative repercussions.
*** ***************************************************************************/
class ShopMedia {
public:
	ShopMedia();

	~ShopMedia();

	/** \brief Finishes preparing media data for use
	*** This function prepares any class members that could not be made ready in the constructor. This
	*** may be the case, for example, where the contents of a data container are dependent on knowing
	*** which object categories the shop deals in (which can not be known until shop mode is initialized
	*** and all wares have been added to the shop).
	**/
	void Initialize();

	std::vector<hoa_utils::ustring>* GetAllCategoryNames()
		{ return &_all_category_names; }

	std::vector<hoa_video::StillImage>* GetAllCategoryIcons()
		{ return &_all_category_icons; }

	std::vector<hoa_utils::ustring>* GetSaleCategoryNames()
		{ return &_sale_category_names; }

	std::vector<hoa_video::StillImage>* GetSaleCategoryIcons()
		{ return &_sale_category_icons; }

	hoa_video::StillImage* GetDrunesIcon()
		{ return &_drunes_icon; }

	hoa_video::StillImage* GetStarIcon()
		{ return &_star_icon; }

	hoa_video::StillImage* GetCheckIcon()
		{ return &_check_icon; }

	hoa_video::StillImage* GetXIcon()
		{ return &_x_icon; }

	hoa_video::StillImage* GetSocketIcon()
		{ return &_socket_icon; }

	hoa_video::StillImage* GetEquipIcon()
		{ return &_equip_icon; }

	std::vector<hoa_video::StillImage>* GetElementalIcons()
		{ return &_elemental_icons; }

	std::vector<hoa_video::StillImage>* GetStatusIcons()
		{ return &_status_icons; }

	std::vector<hoa_video::StillImage>* GetCharacterSprites()
		{ return &_character_sprites; }

	/** \brief Retrieves the category name that represents the specified object type
	*** \param object_type The type of the global object to retrieve the name for
	*** \return A pointer to the ustring holding the category's name. NULL if the argument was invalid.
	*** \note GLOBAL_OBJECT_TOTAL will return the name for "all wares"
	**/
	hoa_utils::ustring* GetCategoryName(hoa_global::GLOBAL_OBJECT object_type);

	/** \brief Retrieves the category icon image that represents the specified object type
	*** \param object_type The type of the global object to retrieve the icon for
	*** \return A pointer to the image holding the category's icon. NULL if the argument was invalid.
	*** \note GLOBAL_OBJECT_TOTAL will return the icon for "all wares"
	**/
	hoa_video::StillImage* GetCategoryIcon(hoa_global::GLOBAL_OBJECT object_type);

	/** \brief Retrieves a specific elemental icon with the proper type and intensity
	*** \param element_type The type of element the user is trying to retrieve the icon for
	*** \param intensity The intensity level of the icon to retrieve
	*** \return The icon representation of the element type and intensity
	**/
	hoa_video::StillImage* GetElementalIcon(hoa_global::GLOBAL_ELEMENTAL element_type, hoa_global::GLOBAL_INTENSITY intensity);

	/** \brief Retrieves a specificstatus icon with the proper type and intensity
	*** \param status_type The type of status the user is trying to retrieve the icon for
	*** \param intensity The intensity level of the icon to retrieve
	*** \return The icon representation of the status type and intensity
	**/
	hoa_video::StillImage* GetStatusIcon(hoa_global::GLOBAL_STATUS status_type, hoa_global::GLOBAL_INTENSITY intensity);

	/** \brief Retrieves a shop sound object
	*** \param identifier The string identifier for the sound to retrieve
	*** \return A pointer to the SoundDescriptor, or NULL if no sound had the identifier name
	**/
	hoa_audio::SoundDescriptor* GetSound(std::string identifier);

private:
	//! \brief Retains text names for all possible object categories, including "all wares"
	std::vector<hoa_utils::ustring> _all_category_names;

	//! \brief Retains icon images for all possible object categories, including "all wares"
	std::vector<hoa_video::StillImage> _all_category_icons;

	//! \brief Retains text names only for those object categories sold by the shop
	std::vector<hoa_utils::ustring> _sale_category_names;

	//! \brief Retains icon images only for those object categories sold by the shop
	std::vector<hoa_video::StillImage> _sale_category_icons;

	//! \brief Image icon representing drunes (currency)
	hoa_video::StillImage _drunes_icon;

	//! \brief Image icon of a single yellow/gold star
	hoa_video::StillImage _star_icon;

	//! \brief Image icon of a green check mark
	hoa_video::StillImage _check_icon;

	//! \brief Image icon of a red x
	hoa_video::StillImage _x_icon;

	//! \brief Image icon representing open sockets available on weapons and armor
	hoa_video::StillImage _socket_icon;

	//! \brief Image icon that represents when a character has a weapon or armor equipped
	hoa_video::StillImage _equip_icon;

	//! \brief Retains all icon images that represent the game's elementals
	std::vector<hoa_video::StillImage> _elemental_icons;

	//! \brief Retains all icon images that represent the game's status effects
	std::vector<hoa_video::StillImage> _status_icons;

	//! \brief Retains sprite image frames for all characters in the active party
	std::vector<hoa_video::StillImage> _character_sprites;

	//! \brief A map of the sounds used in shop mode
	std::map<std::string, hoa_audio::SoundDescriptor*> _sounds;
}; // class ShopMedia


/** ****************************************************************************
*** \brief Manages all data and graphics for showing detailed information about an object
***
*** This class shows summary information about the currently selected object. It displays
*** this information in one of two display modes. The first mode uses only the lower window
*** and shows only a limited amount of information. This is the default view mode that the
*** player sees and this information is visible in tandem with the graphics displayed by the
*** BuyListView class. The second view mode is entered if the player requests to see all the
*** details about the selected object. In this view mode, the BuyListView class' graphics are
*** hidden and this class displays the complete set of information about the selected object
*** using both the middle and lower menu windows. The type of information which is displayed is
*** different depending upon what type of object is selected. The primary purpose of this class
*** is to serve as a helper to the BuyInterface class and to keep the code organized.
***
*** \todo Currently this class treats the display of shards the same as it does for items
*** and key items. This should be changed once shard properties have a more clear definition
*** in the game design.
***
*** \todo Status icons are not currently displayed as there are no status effects implemented
*** in the game yet. Once status effects are functional and graphics are ready to represent
*** them, this code should be updated.
***
*** \todo The character sprites are not yet animated in the display. This should be changed
*** once the appropriate set of sprite frames are available for this menu.
*** ***************************************************************************/
class ShopObjectViewer {
public:
	ShopObjectViewer();

	~ShopObjectViewer()
		{}

	//! \brief Finishes initialization of any data or settings that could not be completed in constructor
	void Initialize();

	//! \brief Updates sprite animations and other graphics as necessary
	void Update();

	//! \brief Draws the contents to the screen
	void Draw();

	/** \brief Changes the selected object to the function argument
	*** \param object A pointer to the shop object to change (NULL-safe)
	*** \note The code using this class should always use this function to change the selected object rather than
	*** changing the public _selected_object member directly. This method updates all of the relevent graphics and
	*** data displays in addition to changing the pointer member.
	**/
	void SetSelectedObject(ShopObject* object);

	/** \brief Changes the view mode which will effect what content is drawn and where it is drawn
	*** \param new_mode The view mode to change to
	*** If the current view mode is the same as the new mode, no action will be taken. If the new mode is unknown
	*** or otherwise unsupported, a warning message will be printed and no change will take place.
	**/
	void ChangeViewMode(SHOP_VIEW_MODE new_mode);

private:
	//! \brief Holds the current view mode of this class
	SHOP_VIEW_MODE _view_mode;

	//! \brief A pointer to the object who's information should be displayed
	ShopObject* _selected_object;

	//! \brief The type of the selected object
	SHOP_OBJECT _object_type;

	//! \name Data that all object types share
	//@{
	//! \brief The name of the selected object
	hoa_video::TextImage _object_name;

	//! \brief A summary description of the object to display
	hoa_gui::TextBox _description_text;

	//! \brief A more detailed "lore" description about the object's origins and connections with the world
	hoa_gui::TextBox _lore_text;
	//@}

	//! \name Data used only for item object types
	//@{
	//! \brief For items, various header text for identifying specific item properties
	hoa_video::TextImage _field_use_header, _battle_use_header, _target_type_header;

	//! \brief Image copies of a green check mark and a red x mark
	hoa_video::StillImage _check_icon, _x_icon;

	//! \brief Holds rendered text images of possible set of target types ("Self — Point", "Ally", "All Enemies", etc.)
	std::vector<hoa_video::TextImage> _target_type_text;

	//! \brief Booleans that indicate whether the item is usable on maps and/or in battles
	bool _map_usable, _battle_usable;

	//! \brief Index into the _target_type_text vector to get the appropriate descriptor for this item
	uint32 _target_type_index;
	//@}

	//! \name Data used only for weapon and armor object types
	//@{
	//! \brief Header text identifying the physical and metaphysical ratings
	hoa_video::TextImage _phys_header, _meta_header;

	//! \brief A rendering of the physical and metaphysical attack/defense ratings
	hoa_video::TextImage _phys_rating, _meta_rating;

	//! \brief An icon image of a shard socket
	hoa_video::StillImage _socket_icon;

	//! \brief Text indicating how many sockets the selected equipment has available
	hoa_video::TextImage _socket_text;

	//! \brief Icon images representing elemental effects and intensity properties of the selected object
	std::vector<hoa_video::StillImage> _elemental_icons;

	//! \brief Icon images representing status effects and intensity properties of the selected object
	std::vector<hoa_video::StillImage> _status_icons;
	//@}

	//! \name Data used for displaying character sprites and related status
	//@{
	//! \brief Sprite images of all characters currently in the party
	std::vector<hoa_video::StillImage> _character_sprites;

	//! \brief For weapons and armor, icon image that represents when a character already has the object equipped
	hoa_video::StillImage _equip_icon;

	//! \brief For weapons and armor, this member is set to true for each character that has the object equipped
	std::vector<bool> _character_equipped;

	//! \brief For weapons and armor, text to indicate changes in phys/meta stats from current equipment
	std::vector<hoa_video::TextImage> _phys_change_text, _meta_change_text;
	//@}

	/** \brief Updates the data and visuals associated specifically with items for the selected object
	*** This method should only be called if the _selected_object member is an item
	**/
	void _SetItemData();

	/** \brief Updates the visible character sprites, equipped status, and stat text and data for the selected object
	*** This method should only be called if the _selected_object member is a weapon or armor
	**/
	void _SetEquipmentData();

	/** \brief Updates the data and visuals associated specifically with shards for the selected object
	*** This method should only be called if the _selected_object member is a shard
	**/
	void _SetShardData();

	//! \brief Determines the proper window owner, position, dimensions for description text
	void _SetDescriptionText();

	/** \brief Renders the desired physical and metaphysical change text
	*** \param index The index into the _phys_change_text and _meta_change_text containers to re-render
	*** \param phys_diff The physical change amount
	*** \param meta_diff The metaphysical change amount
	**/
	void _SetChangeText(uint32 index, int32 phys_diff, int32 meta_diff);

	/** \brief Sets all elemental icons to the proper image when given a container
	*** \param elemental_effects A const reference to a map of elemental effect types and their associated intensities
	***
	*** The argument is presumed to have an entrity for each type of element. This condition is not checked by the function.
	*** The format of the parameter comes from the global object code, as object classes return a const std::map reference
	*** of this type to indicate their elemental effects.
	**/
	void _SetElementalIcons(const std::map<hoa_global::GLOBAL_ELEMENTAL, hoa_global::GLOBAL_INTENSITY>& elemental_effects);

	/** \brief Sets all statusicons to the proper image when given a container
	*** \param status_effects A const reference to a map of status effect types and their associated intensities
	***
	*** The argument is presumed to have an entrity for each type of status. This condition is not checked by the function.
	*** The format of the parameter comes from the global object code, as object classes return a const std::map reference
	*** of this type to indicate their status effects.
	**/
	void _SetStatusIcons(const std::map<hoa_global::GLOBAL_STATUS, hoa_global::GLOBAL_INTENSITY>& status_effects);

	//! \brief Helper function that draws information specific to items
	void _DrawItem();

	//! \brief Helper function that draws information specific to equipment
	void _DrawEquipment();

	//! \brief Helper function that draws information specific to shards
	void _DrawShard();

	//! \brief Helper function that draws information specific to key items
	void _DrawKeyItem();
}; // class ShopObjectViewer

} // namespace private_shop

/** ****************************************************************************
*** \brief Handles the game execution while the player is shopping.
***
*** ShopMode allows the player to purchase items, weapons, armor, and other
*** objects. ShopMode consists of a captured screenshot which forms the
*** background image, upon which a series of menu windows are drawn. The
*** coordinate system used is 1024x768, and a 800x600 arrangement of three
*** menu windows (top, middle, bottom) are drawn on top of that backdrop. These
*** windows are shared beteen all states of shop mode (root, buy, sell, etc.) and
*** the contents of the windows change depending on the active state of shop mode.
***
*** \note The recommended way to create and initialize this class is to call the
*** following methods.
***
*** -# ShopMode constructor
*** -# SetShopName()
*** -# SetGreetingText()
*** -# SetPriceLevels()
*** -# AddObject() for each object to be sold
*** -# Wait for the Reset() method to be automatically called, which will finalize shop initialization
*** ***************************************************************************/
class ShopMode : public hoa_mode_manager::GameMode {
public:
	ShopMode();

	~ShopMode();

	//! \brief Returns a pointer to the active instance of shop mode
	static ShopMode* CurrentInstance()
		{ return _current_instance; }

	//! \brief Provides access to the ShopMedia class members and methods
	private_shop::ShopMedia* Media()
		{ return _shop_media; }

	//! \brief Provides access to the ShopObjectViewer class members and methods
	private_shop::ShopObjectViewer* ObjectViewer()
		{ return _object_viewer; }

	//! \brief Resets appropriate settings and initializes shop if appropriate. Called whenever the ShopMode object is made the active game mode.
	void Reset();

	/** \brief Loads data and prepares shop for initial use
	*** This function is only be called once from the Reset() method. If it is called more than
	*** once it will print a warning and refuse to execute a second time.
	**/
	void Initialize();

	//! \brief Handles user input and updates the shop menu.
	void Update();

	//! \brief Handles the drawing of everything on the shop menu and makes sub-draw function calls as appropriate.
	void Draw();

	/** \brief Used when an object has been selected for purchase by the player
	*** \param object A pointer to the object to add
	*** \note The buy count of the shop object added should be non-zero before this function is called.
	*** Otherwise a warning message will be printed, but the object will still be added.
	**/
	void AddObjectToBuyList(private_shop::ShopObject* object);

	/** \brief Used when the player decides not to purchase an object that was previously marked to be bought
	*** \param object A pointer to the object to remove
	*** \note The buy count of the shop object added should be zero before this function is called.
	*** Otherwise a warning message will be printed, but the object will still be removed.
	**/
	void RemoveObjectFromBuyList(private_shop::ShopObject* object);

	/** \brief Used when an object has been selected to be sold by the player
	*** \param object A pointer to the object to add
	*** \note The sell count of the shop object added should be non-zero before this function is called.
	*** Otherwise a warning message will be printed, but the object will still be added.
	**/
	void AddObjectToSellList(private_shop::ShopObject* object);

	/** \brief Used when the player decides not to sell an object that was previously marked to be sold
	*** \param object A pointer to the object to remove
	*** \note The sell count of the shop object added should be zero before this function is called.
	*** Otherwise a warning message will be printed, but the object will still be removed.
	**/
	void RemoveObjectFromSellList(private_shop::ShopObject* object);

	/** \brief Called whenever the player chooses to clear all marked purchases, sales, and trades
	*** This will empty the buy/sell/trade lists and reset the total costs and sales amounts
	**/
	void ClearOrder();

	/** \brief Called whenever the player confirms a transaction
	*** This method processes the transaction, including modifying the party's drune count, adding/removing
	*** objects from the inventory, and auto equipping/un-equipping traded equipment.
	**/
	void CompleteTransaction();

	/** \brief Updates the costs and sales totals
	*** \param costs_amount The amount to change the purchases cost member by
	*** \param sales_amount The amount to change the sales revenue member by
	***
	*** Obviously if one wishes to only update either costs or sales but not both, pass a zero value for the
	*** appropriate argument that should not be changed. This function should only be called when necessary because
	*** it also has to update the finance text. Thus the function does not just modify integer values but in fact
	*** does have a small amount of computational overhead
	**/
	void UpdateFinances(int32 costs_amount, int32 sales_amount);

	/** \brief Changes the active state of shop mode and prepares the interface of the new state
	*** \param new_state The state to change the shop to
	**/
	void ChangeState(private_shop::SHOP_STATE new_state);

	//! \brief Returns true if the user has indicated they wish to buy or sell any items
	bool HasPreparedTransaction() const
		{ return ((_total_costs != 0) || (_total_sales != 0)); }

	//! \brief Returns the number of drunes that the party would be left with after the marked purchases and sales
	uint32 GetTotalRemaining() const
		{ return (hoa_global::GlobalManager->GetDrunes() + _total_sales - _total_costs); }

	/** \name Exported class methods
	*** The methods in this group are avaiable to be called from within Lua. Their intended use is for setting shop settings
	*** and initializing data before the shop is opened.
	**/
	//@{
	/** \brief Sets the name of the store that should be displayed to the player
	*** \param greeting The name of the shop
	*** \note This method will only work if it is called before the shop is initialized. Calling it afterwards will
	*** result in no operation and a warning message
	**/
	void SetShopName(hoa_utils::ustring name);

	/** \brief Sets the greeting message from the shop/merchant
	*** \param greeting The text
	*** \note This method will only work if it is called before the shop is initialized. Calling it afterwards will
	*** result in no operation and a warning message
	**/
	void SetGreetingText(hoa_utils::ustring greeting);

	/** \brief Sets the buy and sell price levels for the shop
	*** \param buy_level The price level to set for wares that the player would buy from the shop
	*** \param sell_level The price level to set for wares that the player would sell to the shop
	*** \note This method will only work if it is called before the shop is initialized. Calling it afterwards will
	*** result in no operation and a warning message
	**/
	void SetPriceLevels(SHOP_PRICE_LEVEL buy_level, SHOP_PRICE_LEVEL sell_level);

	/** \brief Adds a new object for the shop to sell
	*** \param object_id The id number of the object to add
	*** \param stock The amount of the object to make available for sale at the shop
	***
	*** Adding an object after the shop mode instance has already been initialized (by being made the active game state)
	*** this call will add the object but will not be visible to the player.
	**/
	void AddObject(uint32 object_id, uint32 stock);
	//@}

	/** \brief Deletes an object from the shop
	*** \param object_id The id number of the object to remove
	***
	*** This function should be used in only one specific case. This case is when the player owns this object and
	*** chooses to sell all instances of it and additionally the shop does not sell this item. Trying to remove
	*** an object that the shop sells to the player or trying to remove an object that still remains in the party's
	*** inventory will result in a warning message and the object will not be removed.
	**/
	void RemoveObject(uint32 object_id);

	//! \name Class member access functions
	//@{
	bool IsInitialized() const
		{ return _initialized; }

	private_shop::SHOP_STATE GetState() const
		{ return _state; }

	SHOP_PRICE_LEVEL GetBuyPriceLevel() const
		{ return _buy_price_level; }

	SHOP_PRICE_LEVEL GetSellPriceLevel() const
		{ return _sell_price_level; }

	uint8 GetDealTypes() const
		{ return _deal_types; }

	uint32 GetTotalCosts() const
		{ return _total_costs; }

	uint32 GetTotalSales() const
		{ return _total_sales; }

	std::map<uint32, private_shop::ShopObject>* GetShopObjects()
		{ return &_shop_objects; }

	std::map<uint32, private_shop::ShopObject*>* GetBuyList()
		{ return &_buy_list; }

	std::map<uint32, private_shop::ShopObject*>* GetSellList()
		{ return &_sell_list; }

	hoa_gui::MenuWindow* GetTopWindow()
		{ return &_top_window; }

	hoa_gui::MenuWindow* GetMiddleWindow()
		{ return &_middle_window; }

	hoa_gui::MenuWindow* GetBottomWindow()
		{ return &_bottom_window; }
	//@}

private:
	/** \brief A reference to the current instance of ShopMode
	*** This is used by other shop clases to be able to refer to the shop that they exist in. This member
	*** is NULL when no shop is active
	**/
	static ShopMode* _current_instance;

	//! \brief Set to true only after the shop has been initialized and is ready to be used by the player
	bool _initialized;

	//! \brief Keeps track of what windows are open to determine how to handle user input.
	private_shop::SHOP_STATE _state;

	//! \brief A bit vector that represents the types of merchandise that the shop deals in (items, weapons, etc)
	uint8 _deal_types;

	//! \brief The shop's price level of objects that the player buys from the shop
	SHOP_PRICE_LEVEL _buy_price_level;

	//! \brief The shop's price level of objects that the player sells to the shop
	SHOP_PRICE_LEVEL _sell_price_level;

	//! \brief The total cost of all marked purchases.
	uint32 _total_costs;

	//! \brief The total revenue that will be earned from all marked sales.
	uint32 _total_sales;

	/** \brief A container of objects that ShopMode created itself and need to be deleted when finished
	*** These also happen to represent a list of all global objects that the shop may sell to the player
	**/
	std::vector<hoa_global::GlobalObject*> _created_objects;

	/** \brief Holds all objects that can be bought, sold, or traded in the shop
	*** The integer key to this map is the global object ID represented by the ShopObject.
	**/
	std::map<uint32, private_shop::ShopObject> _shop_objects;

	/** \brief Holds pointers to all objects that the player plans to purchase
	*** The integer key to this map is the global object ID represented by the ShopObject.
	**/
	std::map<uint32, private_shop::ShopObject*> _buy_list;

	/** \brief Holds pointers to all objects that the player plans to sell
	*** The integer key to this map is the global object ID represented by the ShopObject.
	**/
	std::map<uint32, private_shop::ShopObject*> _sell_list;

	//! \brief A pointer to the ShopMedia object created to coincide with this instance of ShopMode
	private_shop::ShopMedia* _shop_media;

	/** \brief A pointer to the ShopObjectViewer object created to assist interface classes in visual presentation
	*** ShopMode only creates and initializes this object and makes it available for the shop interfaces to use.
	*** It does not call any standard methods on this class such as Update() or Draw(). That has to be done by
	*** the interface class that uses it.
	**/
	private_shop::ShopObjectViewer* _object_viewer;

	/** \name Shopping interfaces
	*** These are class objects which are responsible for managing each state in shop mode
	**/
	//@{
	private_shop::RootInterface* _root_interface;
	private_shop::BuyInterface* _buy_interface;
	private_shop::SellInterface* _sell_interface;
	private_shop::TradeInterface* _trade_interface;
	private_shop::ConfirmInterface* _confirm_interface;
	private_shop::LeaveInterface* _leave_interface;
	//@}

	//! \brief Holds an image of the screen taken when the ShopMode instance was created
	hoa_video::StillImage _screen_backdrop;

	//! \brief The highest level window that contains the shop actions and finance information
	hoa_gui::MenuWindow _top_window;

	//! \brief The largest window usually used to display lists of objects
	hoa_gui::MenuWindow _middle_window;

	//! \brief The lowest window typically displays detailed information or additional shop options
	hoa_gui::MenuWindow _bottom_window;

	//! \brief The list of options for what the player may do in shop mode
	hoa_gui::OptionBox _action_options;

	//! \brief Separate text images for each action option. Displayed when _action_options are hidden
	std::vector<hoa_video::TextImage> _action_titles;

	//! \brief Table-formatted text containing the financial information about the current purchases and sales
	hoa_gui::OptionBox _finance_table;
}; // class ShopMode : public hoa_mode_manager::GameMode

} // namespace hoa_shop

#endif // __SHOP_HEADER__
