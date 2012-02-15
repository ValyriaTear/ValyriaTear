///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_trade.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for trade interface of shop mode
*** ***************************************************************************/

#ifndef __SHOP_TRADE_HEADER__
#define __SHOP_TRADE_HEADER__

#include "defs.h"
#include "utils.h"

#include "video.h"
#include "global.h"

#include "shop_utils.h"

namespace hoa_shop {

namespace private_shop {

/** ****************************************************************************
*** \brief Manages the shop where the player is allowed to trade current equipment on their characters
***
*** \todo This interface remains incomplete and will be finished at a later time.
*** ***************************************************************************/
class TradeInterface : public ShopInterface {
public:
	TradeInterface();

	~TradeInterface();

	void Initialize();

	void MakeActive();

	void TransactionNotification();

	void Update();

	void Draw();

	//! \brief Temporary text image stating that this interface is unavailable
	hoa_video::TextImage TEMP_feature_unavailable;
}; // class TradeInterface : public ShopInterface

} // namespace private_shop

} // namespace hoa_shop

#endif // __SHOP_TRADE_HEADER__
