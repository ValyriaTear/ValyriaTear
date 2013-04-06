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
*** \file    shop_root.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for root interface of shop mode
*** ***************************************************************************/

#ifndef __SHOP_ROOT_HEADER__
#define __SHOP_ROOT_HEADER__

#include "engine/video/video.h"
#include "common/global/global.h"

#include "shop_utils.h"

namespace vt_shop
{

namespace private_shop
{

/** ****************************************************************************
*** \brief The highest level shopping interface which displays information about the shop
***
*** This interface is the first information that the player sees when entering the shop.
*** Its function serves two purposes. The first is to allow the user to select between the
*** primary shop actions which are: buy, sell, trade, confirm, and leave. Its second purpose
*** is to display statistical information about the shop, including its name, price levels,
*** the type of wares it deals in, and a short greeting message from the shop keeper.
***
*** The top window contains the shop actions which the user may select from. Take note that
*** the processing and display code for this segment is managed by the ShopMode class and not
*** here. This is so because the top window is mostly static throughout the different interface
*** classes, so having ShopMode manage it eliminates redundant code.
***
*** The middle window contains the name of the shop at the top. Below that are star indicators
*** that inform the user how good the buy and sell prices for the shop are. The more stars, the
*** better it is to buy or sell equipment at the shop. Below the price information are a series
*** of icon images and labels that represent each type of object which may be bought or sold.
*** Not all stores deal in all types of objects, so those object types which the store does not
*** deal in are displayed in grayscale text.
***
*** Finally, the bottom window contains nothing more than a short message or greeting from the
*** shop keeper.
*** ***************************************************************************/
class RootInterface : public ShopInterface
{
public:
    RootInterface();

    ~RootInterface() {}

    //! \brief (Re)initializes various textual and image data based on the shop properties
    void Reinitialize();

    //! \brief No actions need to take place when this interface becomes activated
    void MakeActive()
    {}

    //! \brief No actions need to take place when a transaction occurs
    void TransactionNotification()
    {}

    //! \brief Updates the state of GUI objects and may also process user input
    void Update();

    //! \brief Draws the root window and, if shop mode is in the correct state, the greeting window
    void Draw();

    /** \brief Create's the shop name's text
    *** \param name The name of the shop
    **/
    void SetShopName(const vt_utils::ustring& name);

    /** \brief Creates the shop keeper's greeting message text
    *** \param greeting The textual greeting
    **/
    void SetGreetingText(const vt_utils::ustring& greeting);

private:
    //! \brief The rendered text image of the shop name
    vt_video::TextImage _shop_name;

    //! \brief Text for displaying price levels
    vt_video::TextImage _buy_price_text, _sell_price_text;

    //! \brief A composite image for the star rating of the buy and sell price levels
    vt_video::CompositeImage _buy_price_rating, _sell_price_rating;

    //! \brief A textbox displaying a short greeting or message from the shop keeper
    vt_gui::TextBox _greeting_text;
}; // class RootInterface : public ShopInterface

} // namespace private_shop

} // namespace vt_shop

#endif // __SHOP_ROOT_HEADER__
