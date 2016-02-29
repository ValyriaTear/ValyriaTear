///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for shop mode interface
***
*** This code provides an interface for the user to purchase wares from a
*** merchant. This mode is usually entered from a map after speaking with a
*** shop keeper.
*** ***************************************************************************/

#ifndef __SHOP_HEADER__
#define __SHOP_HEADER__

#include "engine/mode_manager.h"

#include "common/global/global.h"

#include "common/gui/menu_window.h"

#include "shop_utils.h"

namespace vt_audio {
class SoundDescriptor;
}

namespace vt_common {
class DialogueSupervisor;
}

//! \brief All calls to shop mode are wrapped in this namespace.
namespace vt_shop
{

//! \brief Determines whether the code in the vt_shop namespace should print debug statements or not.
extern bool SHOP_DEBUG;

//! \brief An internal namespace to be used only within the shop code. Don't use this namespace anywhere else!
namespace private_shop
{

class BuyInterface;
class RootInterface;
class SellInterface;
class TradeInterface;

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
class ShopMedia
{
public:
    ShopMedia();

    ~ShopMedia() {}

    //! \brief Shop specific category names
    std::vector<vt_utils::ustring>* GetAllCategoryNames() {
        return &_all_category_names;
    }

    std::vector<vt_video::AnimatedImage>* GetCharacterSprites() {
        return &_character_sprites;
    }

    /** \brief Retrieves the category name that represents the specified object type
    *** \param object_type The type of the global object to retrieve the name for
    *** \return A pointer to the ustring holding the category's name. nullptr if the argument was invalid.
    *** \note GLOBAL_OBJECT_TOTAL will return the name for "all wares"
    **/
    vt_utils::ustring *GetCategoryName(vt_global::GLOBAL_OBJECT object_type);

private:
    //! \brief Retains text names for all possible object categories, including "all wares"
    std::vector<vt_utils::ustring> _all_category_names;

    //! \brief Retains sprite image frames for all characters in the active party
    std::vector<vt_video::AnimatedImage> _character_sprites;

    //! \brief Initialize the character's data to show
    void _InitializeCharacters();
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
*** \todo Currently this class treats the display of spirits the same as it does for items
*** and key items. This should be changed once spirit properties have a more clear definition
*** in the game design.
***
*** \todo Status icons are not currently displayed as there are no status effects implemented
*** in the game yet. Once status effects are functional and graphics are ready to represent
*** them, this code should be updated.
***
*** \todo The character sprites are not yet animated in the display. This should be changed
*** once the appropriate set of sprite frames are available for this menu.
*** ***************************************************************************/
class ShopObjectViewer
{
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
    *** \param object A pointer to the shop object to change (nullptr-safe)
    *** \note The code using this class should always use this function to change the selected object rather than
    *** changing the public _selected_object member directly. This method updates all of the relevent graphics and
    *** data displays in addition to changing the pointer member.
    **/
    void SetSelectedObject(ShopObject *object);

    /** \brief Changes the view mode which will effect what content is drawn and where it is drawn
    *** \param new_mode The view mode to change to
    *** If the current view mode is the same as the new mode, no action will be taken. If the new mode is unknown
    *** or otherwise unsupported, a warning message will be printed and no change will take place.
    **/
    void ChangeViewMode(SHOP_VIEW_MODE new_mode);

    //! \brief Update the the count text content according to the current shop mode and state.
    void UpdateCountText();

    //! \brief Scroll the trade conditions up or down. Has no effect when not in trade mode.
    void ScrollUpTradeConditions();
    void ScrollDownTradeConditions();

private:
    //! \brief Holds the current view mode of this class
    SHOP_VIEW_MODE _view_mode;

    //! \brief A pointer to the object who's information should be displayed
    ShopObject *_selected_object;

    //! \brief The type of the selected object
    SHOP_OBJECT _object_type;

    //! \brief When the object type is equipment, this tells whether it is a weapon.
    bool _is_weapon;

    //! \name Data that all object types share
    //@{
    //! \brief The name of the selected object
    vt_video::TextImage _object_name;

    //! \brief A summary description of the object to display
    vt_gui::TextBox _description_text;

    //! \brief A hint helping on how to handle commands on the shop
    vt_gui::TextBox _hint_text;

    //! \brief Displays buy count, sell count, or trade count
    vt_gui::TextBox _count_text;

    //! \brief The trade conditions title
    vt_video::TextImage _conditions_title;

    //! \brief Trade conditions displayed in an OptionBox
    vt_gui::OptionBox _conditions_name;
    vt_gui::OptionBox _conditions_number;
    //@}

    //! \name Data used only for item object types
    //@{
    //! \brief For items, various header text for identifying specific item properties
    vt_video::TextImage _field_use_header, _battle_use_header, _target_type_header;

    //! \brief Image copies of a green check mark and a red x mark
    vt_video::StillImage *_check_icon;
    vt_video::StillImage *_x_icon;

    //! \brief Holds rendered text images of possible set of target types ("Self — Point", "Ally", "All Enemies", etc.)
    std::vector<vt_video::TextImage> _target_type_text;

    //! \brief Booleans that indicate whether the item is usable on maps and/or in battles
    bool _map_usable, _battle_usable;

    //! \brief Index into the _target_type_text vector to get the appropriate descriptor for this item
    uint32_t _target_type_index;
    //@}

    //! \name Data used only for weapon and armor object types
    //@{
    //! \brief Header text identifying the physical and magical ratings
    vt_video::TextImage _phys_header, _mag_header;

    //! \brief A rendering of the physical and magical attack/defense ratings
    vt_video::TextImage _phys_rating, _mag_rating;

    //! \brief An icon image of a spirit slot
    vt_video::StillImage *_spirit_slot_icon;

    //! \brief The key item icon
    vt_video::StillImage *_key_item_icon;

    //! \brief ATK, DEF, MATK, MDEF icons
    vt_video::StillImage *_atk_icon;
    vt_video::StillImage *_matk_icon;
    vt_video::StillImage *_def_icon;
    vt_video::StillImage *_mdef_icon;

    //! \brief The number of spirit the equipment can support.
    uint32_t _spirit_number;

    //! \brief Icon images representing status effects and intensity properties of the selected object
    std::vector<vt_video::StillImage *> _status_icons;

    //! \brief The skills earned when equipping
    vt_video::TextImage _equip_skills_header;
    std::vector<vt_video::TextImage> _equip_skills;
    std::vector<vt_video::StillImage> _equip_skill_icons;
    //@}

    //! \name Data used for displaying character sprites and related status
    //@{
    //! \brief Sprite images of all characters currently in the party
    std::vector<vt_video::AnimatedImage *> _character_sprites;

    //! \brief For weapons and armor, icon image that represents when a character already has the object equipped
    vt_video::StillImage *_equip_icon;

    //! \brief For weapons and armor, this member is set to true for each character that has the object equipped
    std::vector<bool> _character_equipped;

    //! \brief For weapons and armor, text to indicate changes in phys/mag stats from current equipment
    std::vector<vt_video::TextImage> _phys_change_text, _mag_change_text;
    //@}

    /** \brief Updates the condition list.
    *** Used in trade mode to show the rade conditions in the detailed view.
    **/
    void _UpdateTradeConditions();

    /** \brief Updates the data and visuals associated specifically with items for the selected object
    *** This method should only be called if the _selected_object member is an item
    **/
    void _SetItemData();

    /** \brief Updates the visible character sprites, equipped status, and stat text and data for the selected object
    *** This method should only be called if the _selected_object member is a weapon or armor
    **/
    void _SetEquipmentData();

    /** \brief Updates the data and visuals associated specifically with spirits for the selected object
    *** This method should only be called if the _selected_object member is a spirit
    **/
    void _SetSpiritData();

    //! \brief Determines the proper window owner, position, dimensions and content of the description text
    void _SetDescriptionText();

    //! \brief Determines the hint text content
    void _SetHintText();

    /** \brief Renders the desired physical and magical change text
    *** \param index The index into the _phys_change_text and _mag_change_text containers to re-render
    *** \param phys_diff The physical change amount
    *** \param mag_diff The magical change amount
    **/
    void _SetChangeText(uint32_t index, int32_t phys_diff, int32_t mag_diff);

    /** \brief Sets all statusicons to the proper image when given a container
    *** \param status_effects A const reference to a map of status effect types and their associated intensities
    ***
    *** The argument is presumed to have an entrity for each type of status. This condition is not checked by the function.
    *** The format of the parameter comes from the global object code, as object classes return a const std::map reference
    *** of this type to indicate their status effects.
    **/
    void _SetStatusIcons(const std::vector<std::pair<vt_global::GLOBAL_STATUS, vt_global::GLOBAL_INTENSITY> >& status_effects);

    //! \brief Helper function that draws information specific to items
    void _DrawItem();

    //! \brief Helper function that draws information specific to equipment
    void _DrawEquipment();

    //! \brief Helper function that draws information specific to spirits
    void _DrawSpirit();
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
class ShopMode : public vt_mode_manager::GameMode
{
public:
    ShopMode(const std::string& shop_id);

    ~ShopMode();

    //! \brief Returns a pointer to the active instance of shop mode
    static ShopMode *CurrentInstance() {
        return _current_instance;
    }

    //! \brief Provides access to the ShopMedia class members and methods
    private_shop::ShopMedia *Media() {
        return _shop_media;
    }

    //! \brief Provides access to the ShopObjectViewer class members and methods
    private_shop::ShopObjectViewer *ObjectViewer() {
        return _object_viewer;
    }

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
    void AddObjectToBuyList(private_shop::ShopObject *object);

    /** \brief Used when the player decides not to purchase an object that was previously marked to be bought
    *** \param object A pointer to the object to remove
    *** \note The buy count of the shop object added should be zero before this function is called.
    *** Otherwise a warning message will be printed, but the object will still be removed.
    **/
    void RemoveObjectFromBuyList(private_shop::ShopObject *object);

    /** \brief Used when an object has been selected to be sold by the player
    *** \param object A pointer to the object to add
    *** \note The sell count of the shop object added should be non-zero before this function is called.
    *** Otherwise a warning message will be printed, but the object will still be added.
    **/
    void AddObjectToSellList(private_shop::ShopObject *object);

    /** \brief Used when the player decides not to sell an object that was previously marked to be sold
    *** \param object A pointer to the object to remove
    *** \note The sell count of the shop object added should be zero before this function is called.
    *** Otherwise a warning message will be printed, but the object will still be removed.
    **/
    void RemoveObjectFromSellList(private_shop::ShopObject *object);

    /** \brief Used when an object has been selected to be sold by the player
    *** \param object A pointer to the object to add
    *** \note The sell count of the shop object added should be non-zero before this function is called.
    *** Otherwise a warning message will be printed, but the object will still be added.
    **/
    void AddObjectToTradeList(private_shop::ShopObject *object);

    /** \brief Used when the player decides not to sell an object that was previously marked to be sold
    *** \param object A pointer to the object to remove
    *** \note The sell count of the shop object added should be zero before this function is called.
    *** Otherwise a warning message will be printed, but the object will still be removed.
    **/
    void RemoveObjectFromTradeList(private_shop::ShopObject *object);

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
    void UpdateFinances(int32_t change_amount);

    /** \brief Changes the active state of shop mode and prepares the interface of the new state
    *** \param new_state The state to change the shop to
    **/
    void ChangeState(SHOP_STATE new_state);

    /** \brief Changes the view mode of the active shop interface.
    *** \param new_mode The view mode needed to be shown.
    **/
    void ChangeViewMode(SHOP_VIEW_MODE new_mode);

    //! \brief Returns true if the user has indicated they wish to buy or sell any items
    bool HasPreparedTransaction() const {
        return ((_total_costs != 0) || (_total_sales != 0));
    }

    //! \brief Returns the number of drunes that the party would be left with after the marked purchases and sales
    uint32_t GetTotalRemaining() const {
        return (vt_global::GlobalManager->GetDrunes() + _total_change_amount);
    }

    /** \name Exported class methods
    *** The methods in this group are avaiable to be called from within Lua. Their intended use is for setting shop settings
    *** and initializing data before the shop is opened.
    **/
    //@{
    /** \brief Sets the name of the store that should be displayed to the player
    *** \param name The name of the shop
    *** \note This method will only work if it is called before the shop is initialized. Calling it afterwards will
    *** result in no operation and a warning message
    **/
    void SetShopName(const vt_utils::ustring& name);

    /** \brief Sets the greeting message from the shop/merchant
    *** \param greeting The text
    *** \note This method will only work if it is called before the shop is initialized. Calling it afterwards will
    *** result in no operation and a warning message
    **/
    void SetGreetingText(const vt_utils::ustring& greeting);

    //! \brief Sets whether the player can sell item in this shop.
    void SetSellModeEnabled(bool enable_sell_mode) {
        _sell_mode_enabled = enable_sell_mode;
    }

    /** \brief Sets the buy and sell price levels for the shop
    *** \param buy_level The price level to set for wares that the player would buy from the shop
    *** \param sell_level The price level to set for wares that the player would sell to the shop
    *** \note This method will only work if it is called before the shop is initialized. Calling it afterwards will
    *** result in no operation and a warning message
    **/
    void SetPriceLevels(SHOP_PRICE_LEVEL buy_level, SHOP_PRICE_LEVEL sell_level);

    /** \brief Adds a new item for the shop to sell
    *** \param object_id The id number of the object to add
    *** \param stock The amount of the object to make available for sale at the shop.
    *** If set to 0, the number of objects to buy is infinite.
    ***
    *** Adding an object after the shop mode instance has already been initialized (by being made the active game state)
    *** this call will add the object but will not be visible to the player.
    **/
    void AddItem(uint32_t object_id, uint32_t stock);
    //@}

    /** \brief Adds a new trade for the shop to sell
    *** \param object_id The id number of the object to add
    *** \param stock The amount of the object to make available for sale at the shop.
    *** If set to 0, the number of objects is infinite.
    ***
    *** Adding an object after the shop mode instance has already been initialized (by being made the active game state)
    *** this call will add the object but will not be visible to the player.
    **/
    void AddTrade(uint32_t object_id, uint32_t stock);
    //@}

    /** \brief Deletes an object from the shop sell list
    *** \param object_id The id number of the object to remove
    ***
    *** This function should be used in only one specific case. This case is when the player buys all instances
    *** of one object type.
    *** Trying to remove an object that the shop sells to the player or trying to remove an object
    *** that still remains in the shop's inventory will result in a warning message and the object will not be removed.
    **/
    void RemoveObjectToBuy(uint32_t object_id);

    /** \brief Deletes an object from the shop sell list
    *** \param object_id The id number of the object to remove
    ***
    *** This function should be used in only one specific case. This case is when the player owns this object and
    *** chooses to sell all instances of it and additionally the shop does not sell this item. Trying to remove
    *** an object that the shop sells to the player or trying to remove an object that still remains in the party's
    *** inventory will result in a warning message and the object will not be removed.
    **/
    void RemoveObjectToSell(uint32_t object_id);

    /** \brief Deletes an object from the shop
    *** \param object_id The id number of the object to remove
    ***
    *** This function should be used in only one specific case. This case is when the player owns this object and
    *** chooses to sell all instances of it and additionally the shop does not sell this item. Trying to remove
    *** an object that the shop sells to the player or trying to remove an object that still remains in the party's
    *** inventory will result in a warning message and the object will not be removed.
    **/
    void RemoveObjectToTrade(uint32_t object_id);

    //! \name Class member access functions
    //@{
    bool IsInitialized() const {
        return _initialized;
    }

    SHOP_STATE GetState() const {
        return _state;
    }

    SHOP_PRICE_LEVEL GetBuyPriceLevel() const {
        return _buy_price_level;
    }

    SHOP_PRICE_LEVEL GetSellPriceLevel() const {
        return _sell_price_level;
    }

    uint32_t GetTotalCosts() const {
        return _total_costs;
    }

    uint32_t GetTotalSales() const {
        return _total_sales;
    }

    //! Returns the available list of item for sale.
    std::map<uint32_t, private_shop::ShopObject *>* GetAvailableBuy() {
        return &_available_buy;
    }

    //! Returns the available list of item that can be sold by the character
    std::map<uint32_t, private_shop::ShopObject *>* GetAvailableSell() {
        return &_available_sell;
    }

    //! Returns the available list of item that can be traded for
    std::map<uint32_t, private_shop::ShopObject *>* GetAvailableTrade() {
        return &_available_trade;
    }

    //!  Returns the list of items the player has currently reserved for acquisition.
    std::map<uint32_t, private_shop::ShopObject *>* GetBuyList() {
        return &_buy_list;
    }

    //! Returns the list of items the player is currently willing to sell.
    std::map<uint32_t, private_shop::ShopObject *>* GetSellList() {
        return &_sell_list;
    }

    //! Returns the list of items the player is currently willing to sell.
    std::map<uint32_t, private_shop::ShopObject *>* GetTradeList() {
        return &_trade_list;
    }

    vt_gui::MenuWindow *GetTopWindow() {
        return &_top_window;
    }

    vt_gui::MenuWindow *GetMiddleWindow() {
        return &_middle_window;
    }

    vt_gui::MenuWindow *GetBottomWindow() {
        return &_bottom_window;
    }
    //@}

    vt_common::DialogueSupervisor* GetDialogueSupervisor() {
        return _dialogue_supervisor;
    }

    //! \brief Get/Set whether the shop user input is enabled.
    bool IsInputEnabled() const {
        return _input_enabled;
    }
    void SetInputEnabled(bool input_enabled) {
        _input_enabled = input_enabled;
    }

    //! \brief Tells whether input in dialogues is enabled.
    //! By Default, input in dialogues is enabled when the input
    //! is disabled for the shop.
    bool AcceptUserInputInDialogues() const {
        return !_input_enabled;
    }

private:
    //! \brief update (enable, disable) the available shop options (buy, sell, ...)
    void _UpdateAvailableShopOptions();

    //! \brief updates the available items the user can sell
    void _UpdateAvailableObjectsToSell();

    //! \brief Handles any key input made on the shop root interface.
    void _HandleRootInterfaceInput();

    //! \brief Save the current shop data to global info.
    void _SaveShopData();

    /** \brief A reference to the current instance of ShopMode
    *** This is used by other shop classes to be able to refer to the shop that they exist in. This member
    *** is nullptr when no shop is active
    **/
    static ShopMode* _current_instance;

    //! \brief The shop unique id. Used to store bought/sold items in the save game.
    std::string _shop_id;

    //! \brief Tells whether the sell mode is enabled in this shop, thus whether the player can sell items.
    bool _sell_mode_enabled;

    //! \brief Set to true only after the shop has been initialized and is ready to be used by the player
    bool _initialized;

    //! \brief Keeps track of what windows are open to determine how to handle user input.
    SHOP_STATE _state;

    //! \brief The shop's price level of objects that the player buys from the shop
    SHOP_PRICE_LEVEL _buy_price_level;

    //! \brief The shop's price level of objects that the player sells to the shop
    SHOP_PRICE_LEVEL _sell_price_level;

    //! \brief The total cost of all marked purchases.
    uint32_t _total_costs;

    //! \brief The total revenue that will be earned from all marked sales.
    uint32_t _total_sales;

    //! \brief The total revenue that will be earned from all marked sales.
    int32_t _total_change_amount;

    /** \brief Holds all objects that can be bought, or sold in the shop
    *** The integer key to this map is the global object ID represented by the ShopObject.
    **/
    std::map<uint32_t, private_shop::ShopObject *> _available_buy;
    std::map<uint32_t, private_shop::ShopObject *> _available_sell;
    std::map<uint32_t, private_shop::ShopObject *> _available_trade;

    /** \brief Holds pointers to all objects that the player plans to purchase
    *** The integer key to this map is the global object ID represented by the ShopObject.
    **/
    std::map<uint32_t, private_shop::ShopObject *> _buy_list;

    /** \brief Holds pointers to all objects that the player plans to sell
    *** The integer key to this map is the global object ID represented by the ShopObject.
    **/
    std::map<uint32_t, private_shop::ShopObject *> _sell_list;

    /** \brief Holds pointers to all objects that the player plans to sell
    *** The integer key to this map is the global object ID represented by the ShopObject.
    **/
    std::map<uint32_t, private_shop::ShopObject *> _trade_list;

    //! \brief A pointer to the ShopMedia object created to coincide with this instance of ShopMode
    private_shop::ShopMedia *_shop_media;

    /** \brief A pointer to the ShopObjectViewer object created to assist interface classes in visual presentation
    *** ShopMode only creates and initializes this object and makes it available for the shop interfaces to use.
    *** It does not call any standard methods on this class such as Update() or Draw(). That has to be done by
    *** the interface class that uses it.
    **/
    private_shop::ShopObjectViewer *_object_viewer;

    /** \name Shopping interfaces
    *** These are class objects which are responsible for managing each state in shop mode
    **/
    //@{
    private_shop::RootInterface *_root_interface;
    private_shop::BuyInterface *_buy_interface;
    private_shop::SellInterface *_sell_interface;
    private_shop::TradeInterface *_trade_interface;
    //@}

    //! \brief Holds an image of the screen taken when the ShopMode instance was created
    vt_video::StillImage _screen_backdrop;

    //! \brief The highest level window that contains the shop actions and finance information
    vt_gui::MenuWindow _top_window;

    //! \brief The largest window usually used to display lists of objects
    vt_gui::MenuWindow _middle_window;

    //! \brief The lowest window typically displays detailed information or additional shop options
    vt_gui::MenuWindow _bottom_window;

    //! \brief The list of options for what the player may do in shop mode
    vt_gui::OptionBox _action_options;

    //! \brief Separate text images for each action option. Displayed when _action_options are hidden
    std::vector<vt_video::TextImage> _action_titles;

    //! \brief Table-formatted text containing the financial information about the current purchases and sales
    vt_gui::OptionBox _finance_table;

    //! \brief Stores and processes any dialogue that is to occur within the shop mode.
    vt_common::DialogueSupervisor* _dialogue_supervisor;

    //! \brief Tells whether input should be processed.
    //! \note Useful for certain dialogues and other events such as tutorial
    bool _input_enabled;
}; // class ShopMode : public vt_mode_manager::GameMode

} // namespace vt_shop

#endif // __SHOP_HEADER__
