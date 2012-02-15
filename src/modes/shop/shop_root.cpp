///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_root.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for root interface of shop mode
*** ***************************************************************************/

#include <iostream>

#include "utils.h"

#include "audio.h"
#include "video.h"
#include "input.h"
#include "mode_manager.h"
#include "system.h"

#include "global.h"

#include "shop.h"
#include "shop_root.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_input;
using namespace hoa_mode_manager;
using namespace hoa_system;
using namespace hoa_global;

namespace hoa_shop {

namespace private_shop {

// The maximum number of category icons + text that can be displayed in a single row
const uint8 MAX_CATEGORY_ROW_SIZE = 4;

// *****************************************************************************
// ***** CategoryDrawData class methods
// *****************************************************************************

void CategoryDrawData::ComputeCoordinates(uint8 number_categories) {
	if (number_categories > GLOBAL_OBJECT_TOTAL) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "invalid argument value: " << number_categories << endl;
		return;
	}

	// ---------- (1): Determine the number of entries in each category row
	if (number_categories <= MAX_CATEGORY_ROW_SIZE) {
		first_row_num = number_categories;
		second_row_num = 0;
	}
	else {
		first_row_num = MAX_CATEGORY_ROW_SIZE;
		second_row_num = number_categories - first_row_num;
	}

	// ---------- (2): Determine the y position in each category row
	if (second_row_num != 0) { // then there are two rows of categories to draw
		first_row_y = 420.0f;
		second_row_y = 320.0f;
	}
	else {
		first_row_y = 370.0f;
		second_row_y = first_row_y;
	}

	// ---------- (3): Determine the x position of each category row
	switch (first_row_num) {
		case 1:
			first_row_x = 512.0f; // Horizontal center of screen
			break;
		case 2:
			first_row_x = 437.0f; // The delta between icons when drawn is 150.0f, so move the draw cursor by half that amount
			break;
		case 3:
			first_row_x = 362.0f;
			break;
		case 4:
			first_row_x = 287.0f;
			break;
	}
	switch (second_row_num) {
		case 1:
			second_row_x = 512.0f;
			break;
		case 2:
			second_row_x = 437.0f;
			break;
		case 3:
			second_row_x = 362.0f;
			break;
		case 4:
			second_row_x = 287.0f;
			break;
	}
} // void CategoryDrawData::ComputeCoordinates(uint8 number_categories)

// *****************************************************************************
// ***** RootInterface class methods
// *****************************************************************************

RootInterface::RootInterface() {
	// Initialize text properties and set default text where appropriate
	_shop_name.SetStyle(TextStyle("title28"));
	_shop_name.SetText(UTranslate("Shop Name")); // This default name should be overwritten

	_buy_price_text.SetStyle(TextStyle("text22"));
	_buy_price_text.SetText(UTranslate("Buy prices"));
	_sell_price_text.SetStyle(TextStyle("text22"));
	_sell_price_text.SetText(UTranslate("Sell prices"));

	_greeting_text.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());
	_greeting_text.SetPosition(40.0f, 100.0f);
	_greeting_text.SetDimensions(600.0f, 50.0f);
	_greeting_text.SetTextStyle(TextStyle("text22"));
	_greeting_text.SetDisplayMode(VIDEO_TEXT_INSTANT);
	_greeting_text.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_greeting_text.SetDisplayText(UTranslate("\"Welcome! Take a look around.\"")); // Default greeting, should usually be overwritten
}



void RootInterface::Initialize() {
	// ---------- (1): Create the price level graphics
	// Holds the number of stars to display that represent the price levels
	int8 num_buy_stars = 0;
	int8 num_sell_stars = 0;

	// Determine the number of stars based on the price level
	switch (ShopMode::CurrentInstance()->GetBuyPriceLevel()) {
		case SHOP_PRICE_VERY_GOOD:
			num_buy_stars = 5;
			break;
		case SHOP_PRICE_GOOD:
			num_buy_stars = 4;
			break;
		case SHOP_PRICE_STANDARD:
			num_buy_stars = 3;
			break;
		case SHOP_PRICE_POOR:
			num_buy_stars = 2;
			break;
		case SHOP_PRICE_VERY_POOR:
			num_buy_stars = 1;
			break;
		default:
			IF_PRINT_WARNING(SHOP_DEBUG) << "invalid buy level argument: " << ShopMode::CurrentInstance()->GetBuyPriceLevel() << endl;
			break;
	}

	switch (ShopMode::CurrentInstance()->GetSellPriceLevel()) {
		case SHOP_PRICE_VERY_GOOD:
			num_sell_stars = 5;
			break;
		case SHOP_PRICE_GOOD:
			num_sell_stars = 4;
			break;
		case SHOP_PRICE_STANDARD:
			num_sell_stars = 3;
			break;
		case SHOP_PRICE_POOR:
			num_sell_stars = 2;
			break;
		case SHOP_PRICE_VERY_POOR:
			num_sell_stars = 1;
			break;
		default:
			IF_PRINT_WARNING(SHOP_DEBUG) << "invalid sell level argument: " << ShopMode::CurrentInstance()->GetSellPriceLevel() << endl;
			break;
	}

	// Star images used to construct the composite star rating (30x30 pixel size)
	StillImage star, gray_star;
	star = *(ShopMode::CurrentInstance()->Media()->GetStarIcon());
	gray_star = star;
	gray_star.EnableGrayScale();

	// Finally, construct the composite images with the correct star rating
	_buy_price_rating.SetDimensions(200.0f, 30.0f);
	_sell_price_rating.SetDimensions(200.0f, 30.0f);

	float offset = 0.0f;
	for (uint8 count = 5; count > 0; count--) {
		if (num_buy_stars > 0) {
			_buy_price_rating.AddImage(star, offset, 0.0f);
		}
		else {
			_buy_price_rating.AddImage(gray_star, offset, 0.0f);
		}

		num_buy_stars--;
		offset += 40.0f;
	}

	offset = 0.0f;
	for (uint8 count = 5; count > 0; count--, num_sell_stars--) {
		if (num_sell_stars > 0) {
			_sell_price_rating.AddImage(star, offset, 0.0f);
		}
		else {
			_sell_price_rating.AddImage(gray_star, offset, 0.0f);
		}

		num_buy_stars--;
		offset += 40.0f;
	}

	// ---------- (2): Construct category name text and graphics and determine category draw coordinates
	// Determine the number of names and icons of categories to load
	uint32 number_categories = ShopMode::CurrentInstance()->Media()->GetSaleCategoryNames()->size();
	if (number_categories > 1) // If multiple categories are available, remove one because we don't want to show the "all" category
		number_categories--;

	TextStyle name_style("text22");
	for (uint32 i = 0; i < number_categories; i++) {
		_category_names.push_back(TextImage(ShopMode::CurrentInstance()->Media()->GetSaleCategoryNames()->at(i), name_style));
		_category_icons.push_back(ShopMode::CurrentInstance()->Media()->GetSaleCategoryIcons()->at(i));
	}

	_category_draw_data.ComputeCoordinates(_category_icons.size());
} // void RootInterface::Initialize()



void RootInterface::Update() {
	_greeting_text.Update();
}



void RootInterface::Draw() {
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_TOP, 0);

	// Middle window: draw shop's name at the top
	VideoManager->Move(512.0f, 580.0f);
	_shop_name.Draw();

	// Middle window: below the shop name draw the pricing text and rating image
	VideoManager->MoveRelative(-140.0f, -60.0f);
	_buy_price_text.Draw();
	VideoManager->MoveRelative(280.0f, 0.0f);
	_sell_price_text.Draw();

	VideoManager->MoveRelative(-280.0f, -40.0f);
	_buy_price_rating.Draw();
	VideoManager->MoveRelative(280.0f, 0.0f);
	_sell_price_rating.Draw();

	// Middle window: below the pricing text/image draw the category icons and text in one or two rows
	VideoManager->Move(_category_draw_data.first_row_x, _category_draw_data.first_row_y);
	for (uint8 i = 0; i < _category_draw_data.first_row_num; i++) {
		_category_icons[i].Draw();
		VideoManager->MoveRelative(0.0f, -60.0f);
		_category_names[i].Draw();
		VideoManager->MoveRelative(150.0f, 60.0f);
	}

	VideoManager->Move(_category_draw_data.second_row_x, _category_draw_data.second_row_y);
	for (uint8 i = 0; i < _category_draw_data.second_row_num; i++) {
		_category_icons[i + MAX_CATEGORY_ROW_SIZE].Draw();
		VideoManager->MoveRelative(0.0f, -60.0f);
		_category_names[i +MAX_CATEGORY_ROW_SIZE].Draw();
		VideoManager->MoveRelative(150.0f, 60.0f);
	}

	// Bottom window: draw the greeting text
	_greeting_text.Draw();
}



void RootInterface::SetShopName(hoa_utils::ustring name) {
	_shop_name.SetText(name);
}



void RootInterface::SetGreetingText(hoa_utils::ustring greeting) {
	_greeting_text.SetDisplayText(greeting);
}

} // namespace private_shop

} // namespace hoa_shop
