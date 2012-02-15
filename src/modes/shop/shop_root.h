///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
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

#include "defs.h"
#include "utils.h"

#include "video.h"
#include "global.h"

#include "shop_utils.h"

namespace hoa_shop {

namespace private_shop {

/** ****************************************************************************
*** \brief Assists the RootInterface in proper draw orientation of object category icons and text
***
*** Shops have anywhere between one and eight different categories of objects that they may deal in.
*** One job of the root interface is to display the icon and name of all categories which the shop
*** deals in. This information is displayed in rows of four. To keep the interface looking nice and consistent
*** with all the different possible combinations of category numbers, the y draw position for the rows is altered
*** depending on whether one or two rows are to be drawn. The x draw position of the category icons and text is
*** also variable depending on the number of categories to be drawn in that row.
*** ***************************************************************************/
class CategoryDrawData {
public:
	/** \brief Determines the appropriate values to set for each class member
	*** \param number_categories The total number of object categories to draw. Valid range: 1-8
	**/
	void ComputeCoordinates(uint8 number_categories);

	/** \brief The number of object categories in the first and second rows
	*** \note The first row may have 1-4 categories and the second row may have 0-4 categories
	**/
	uint8 first_row_num, second_row_num;

	//! \brief Starting draw positions for the first and second rows of object categories
	float first_row_x, first_row_y, second_row_x, second_row_y;
}; // class CategoryDrawData


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
class RootInterface : public ShopInterface {
public:
	RootInterface();

	~RootInterface() {}

	//! \brief Initializes various textual and image data based on the shop properties
	void Initialize();

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
	void SetShopName(hoa_utils::ustring name);

	/** \brief Creates the shop keeper's greeting message text
	*** \param greeting The textual greeting
	**/
	void SetGreetingText(hoa_utils::ustring greeting);

private:
	//! \brief The rendered text image of the shop name
	hoa_video::TextImage _shop_name;

	//! \brief Text for displaying price levels
	hoa_video::TextImage _buy_price_text, _sell_price_text;

	//! \brief A composite image for the star rating of the buy and sell price levels
	hoa_video::CompositeImage _buy_price_rating, _sell_price_rating;

	/** \brief Container for the text object representing each object category's name
	*** Categories which the shop does not deal in will have their text rendered in grayscale
	**/
	std::vector<hoa_video::TextImage> _category_names;

	/** \brief Container for icon images that represent each object category
	*** Categories which the shop does not deal in will have their icon rendered in grayscale
	**/
	std::vector<hoa_video::StillImage> _category_icons;

	//! \brief A textbox displaying a short greeting or message from the shop keeper
	hoa_gui::TextBox _greeting_text;

	//! \brief A container holding the correct draw coordinates and information for object categories
	CategoryDrawData _category_draw_data;
}; // class RootInterface : public ShopInterface

} // namespace private_shop

} // namespace hoa_shop

#endif // __SHOP_ROOT_HEADER__
