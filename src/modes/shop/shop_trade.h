///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_trade.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Josh Niehenke, jnskeer@gmail.com
*** \brief   Header file for sell interface of shop mode
***
*** \note The contents of this file are near identical to the contents of
*** shop_buy.h. When making any changes to this file, please look to shop_buy.h
*** to see if it should have similar changes made.
*** ***************************************************************************/

#ifndef __SHOP_TRADE_HEADER__
#define __SHOP_TRADE_HEADER__

#include "common/global/global.h"

#include "shop_utils.h"

namespace hoa_shop
{

namespace private_shop
{

class TradeListDisplay;

/** ****************************************************************************
*** \brief Manages the shop when it is in buy mode and enables the player to view and purchase wares
***
*** This interface displays the list of objects that are available for sale. It organizes
*** these objects based on their object type/category (item, weapon, etc.) and allows the
*** player to switch between views of these different categories. This interface also displays
*** information about the currently selected object such as its description, statistical
*** ratings, usable characters, etc.
*** ***************************************************************************/
class TradeInterface : public ShopInterface
{
public:
    TradeInterface();

    ~TradeInterface();

    //! \brief (Re)initializes the data conatiners and GUI objects to be used
    void Reinitialize();

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
    ShopObject *_selected_object;

    //! \brief A bit vector that represents the types of merchandise that the shop deals in (items, weapons, etc)
    uint8 _trade_deal_types;

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
    std::vector<hoa_video::StillImage *> _category_icons;

    //! \brief Display manager for the current category of objects selected
    ObjectCategoryDisplay _category_display;

    //! \brief Class objects used to display the object data to the player
    std::vector<TradeListDisplay *> _list_displays;

    //! \brief A copy of the selected object's icon, scaled to 1/4 size
    hoa_video::StillImage _selected_icon;

    //! \brief Text image of the selected object's name
    hoa_video::TextImage _selected_name;

    //! \brief A single row option box containing the selected object's properties
    hoa_gui::OptionBox _selected_properties;

    /**
    ***  Update the available deal types. Called only in _RefreshItemCategories().
    */
    void _UpdateAvailableTradeDealTypes();

    /**
    *** Refresh the available item categories.
    **/
    void _RefreshItemCategories();

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
}; // class TradeInterface : public ShopInterface


/** ****************************************************************************
*** \brief A display class that manages and draws lists of objects that may be marked to buy
***
*** The "identify" list contains a 0.25x size icon of the object and the object's name.
*** The "properties" list contains price, shop stock, amount owned by the player, and
*** requested buy quantity. Only buy quantity requires regular refreshing based upon
*** the player's actions while the buy interface is active.
*** ***************************************************************************/
class TradeListDisplay : public ObjectListDisplay
{
public:
    TradeListDisplay()
    {}

    ~TradeListDisplay()
    {}

    //! \brief Reconstructs all option box entries from the object data
    void ReconstructList();

    /** \brief Changes the buy count of the selected object, refreshes the list entry, and updates financial totals
    *** \param less_or_more False to decrease the quantity, true to increase it
    *** \param amount The amount to decrease/increase the quantity by (default value == 1)
    *** \return False if no quantity change could take place, true if a quantity change did occur
    *** \note Even if the function returns true, there is no guarantee that the requested amount
    *** was fully met. For example, if the function is asked to increase the buy quantity by 10 but
    *** the shop only has 6 instances of the selected object in stock, the function will increase
    *** the quantity by 6 (not 10) and return true.
    **/
    bool ChangeTradeQuantity(bool less_or_more, uint32 amount = 1);
}; // class TradeListDisplay : public ObjectListDisplay

} // namespace private_shop

} // namespace hoa_shop

#endif // __SHOP_TRADE_HEADER__
