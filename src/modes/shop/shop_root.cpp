///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_root.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for root interface of shop mode
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "shop_root.h"

#include "engine/audio/audio.h"
#include "engine/video/video.h"
#include "engine/input.h"
#include "engine/mode_manager.h"
#include "engine/system.h"

#include "common/global/global.h"

#include "shop.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_input;
using namespace vt_mode_manager;
using namespace vt_system;
using namespace vt_global;

namespace vt_shop
{

namespace private_shop
{

// *****************************************************************************
// ***** RootInterface class methods
// *****************************************************************************

RootInterface::RootInterface()
{
    // Initialize text properties and set default text where appropriate
    _shop_name.SetStyle(TextStyle("title28"));
    _shop_name.SetText(UTranslate("The Shop")); // This default name should be overwritten

    _buy_price_text.SetStyle(TextStyle("text22"));
    _buy_price_text.SetText(UTranslate("Buy prices"));
    _sell_price_text.SetStyle(TextStyle("text22"));
    _sell_price_text.SetText(UTranslate("Sell prices"));

    _greeting_text.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());
    _greeting_text.SetPosition(40.0f, 40.0f);
    _greeting_text.SetDimensions(600.0f, 50.0f);
    _greeting_text.SetTextStyle(TextStyle("text22"));
    _greeting_text.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _greeting_text.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _greeting_text.SetDisplayText(UTranslate("\"Welcome! Take a look around.\"")); // Default greeting, should usually be overwritten
}

void RootInterface::Reinitialize()
{
    // ---------- (1): Create the price level graphics
    // Holds the number of stars to display that represent the price levels
    int8 num_buy_stars = 0;
    int8 num_sell_stars = 0;

    // Determine the number of stars based on the price level
    switch(ShopMode::CurrentInstance()->GetBuyPriceLevel()) {
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
        IF_PRINT_WARNING(SHOP_DEBUG) << "invalid buy level argument: " << ShopMode::CurrentInstance()->GetBuyPriceLevel() << std::endl;
        break;
    }

    switch(ShopMode::CurrentInstance()->GetSellPriceLevel()) {
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
        IF_PRINT_WARNING(SHOP_DEBUG) << "invalid sell level argument: " << ShopMode::CurrentInstance()->GetSellPriceLevel() << std::endl;
        break;
    }

    // Star images used to construct the composite star rating (30x30 pixel size)
    StillImage star, gray_star;
    star = *(GlobalManager->Media().GetStarIcon());
    gray_star = star;
    gray_star.EnableGrayScale();

    // Finally, construct the composite images with the correct star rating
    _buy_price_rating.SetDimensions(200.0f, 30.0f);
    _sell_price_rating.SetDimensions(200.0f, 30.0f);

    float offset = 0.0f;
    for(uint8 count = 5; count > 0; count--) {
        if(num_buy_stars > 0) {
            _buy_price_rating.AddImage(star, offset, 0.0f);
        } else {
            _buy_price_rating.AddImage(gray_star, offset, 0.0f);
        }

        num_buy_stars--;
        offset += 40.0f;
    }

    offset = 0.0f;
    for(uint8 count = 5; count > 0; count--, num_sell_stars--) {
        if(num_sell_stars > 0) {
            _sell_price_rating.AddImage(star, offset, 0.0f);
        } else {
            _sell_price_rating.AddImage(gray_star, offset, 0.0f);
        }

        num_buy_stars--;
        offset += 40.0f;
    }
} // void RootInterface::Initialize()

void RootInterface::Update()
{
    _greeting_text.Update();
}

void RootInterface::Draw()
{
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_TOP, 0);

    // Middle window: draw shop's name at the top
    VideoManager->Move(512.0f, 188.0f);
    _shop_name.Draw();

    // Middle window: below the shop name draw the pricing text and rating image
    VideoManager->MoveRelative(-140.0f, 60.0f);
    _buy_price_text.Draw();
    VideoManager->MoveRelative(280.0f, 0.0f);
    _sell_price_text.Draw();

    VideoManager->MoveRelative(-280.0f, 40.0f);
    _buy_price_rating.Draw();
    VideoManager->MoveRelative(280.0f, 0.0f);
    _sell_price_rating.Draw();

    // Bottom window: draw the greeting text
    _greeting_text.Draw();
}

void RootInterface::SetShopName(const vt_utils::ustring& name)
{
    _shop_name.SetText(name);
}

void RootInterface::SetGreetingText(const vt_utils::ustring& greeting)
{
    _greeting_text.SetDisplayText(greeting);
}

} // namespace private_shop

} // namespace vt_shop
