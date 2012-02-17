///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_trade.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for trade interface of shop mode
*** ***************************************************************************/

#include "defs.h"
#include "utils.h"

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/system.h"
#include "engine/video/video.h"

#include "common/global/global.h"

#include "shop.h"
#include "shop_trade.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_input;
using namespace hoa_system;
using namespace hoa_video;
using namespace hoa_global;

namespace hoa_shop {

namespace private_shop {

// *****************************************************************************
// ***** TradeInterface class methods
// *****************************************************************************

TradeInterface::TradeInterface() {
	TEMP_feature_unavailable.SetStyle(TextStyle("text24"));
	TEMP_feature_unavailable.SetText(UTranslate("This feature is not yet available."));
}



TradeInterface::~TradeInterface() {

}



void TradeInterface::Initialize() {

}



void TradeInterface::MakeActive() {

}



void TradeInterface::TransactionNotification() {

}



void TradeInterface::Update() {
	if (InputManager->ConfirmPress() || InputManager->CancelPress()) {
		ShopMode::CurrentInstance()->ChangeState(SHOP_STATE_ROOT);
	}
}



void TradeInterface::Draw() {
	VideoManager->Move(512.0f, 405.0f);
	TEMP_feature_unavailable.Draw();
}

} // namespace private_shop

} // namespace hoa_shop
