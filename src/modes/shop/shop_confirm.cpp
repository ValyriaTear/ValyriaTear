///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_confirm.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for confirm interface of shop mode
***
*** \note The contents of this file are near identical to the contents of
*** shop_leave.cpp. When making any changes to this file, please look to shop_leave.cpp
*** to see if it should have similar changes made.
*** ***************************************************************************/

#include "defs.h"
#include "utils.h"

#include "audio.h"
#include "input.h"
#include "system.h"
#include "video.h"

#include "shop.h"
#include "shop_confirm.h"
#include "shop_buy.h"
#include "shop_sell.h"
#include "shop_trade.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_input;
using namespace hoa_system;
using namespace hoa_video;
using namespace hoa_gui;

namespace hoa_shop {

namespace private_shop {

// *****************************************************************************
// ***** ConfirmInterface class methods
// *****************************************************************************

ConfirmInterface::ConfirmInterface() :
	_state(CONFIRM_STATE_MAIN),
	_active_list(ACTIVE_LIST_BUY),
	_no_transactions(true),
	_buy_count(0),
	_buy_unique(0),
	_sell_count(0),
	_sell_unique(0),
	_trade_count(0),
	_trade_characters(0),
	_buy_list_display(NULL),
	_sell_list_display(NULL)
{
	TextStyle stats_style("text20");

	_buy_header.SetStyle(TextStyle("title22", Color::white));
	_buy_header.SetText(UTranslate("Purchases"));
	_buy_stats.SetStyle(stats_style);

	_sell_header.SetStyle(TextStyle("title22", Color::white));
	_sell_header.SetText(UTranslate("Sales"));
	_sell_stats.SetStyle(stats_style);

	_trade_header.SetStyle(TextStyle("title22", Color::white));
	_trade_header.SetText(UTranslate("Trades"));
	_trade_stats.SetStyle(stats_style);

	_name_header.SetStyle(TextStyle("title24"));
	_name_header.SetText(UTranslate("Name"));

	_properties_header.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());
	_properties_header.SetPosition(480.0f, 390.0f);
	_properties_header.SetDimensions(300.0f, 30.0f, 4, 1, 4, 1);
	_properties_header.SetOptionAlignment(VIDEO_X_RIGHT, VIDEO_Y_CENTER);
	_properties_header.SetTextStyle(TextStyle("title24"));
	_properties_header.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	_properties_header.AddOption(UTranslate("Price"));
	_properties_header.AddOption(UTranslate("Stock"));
	_properties_header.AddOption(UTranslate("Own"));
	_properties_header.AddOption(MakeUnicodeString(""));

	_empty_list_text.SetStyle(TextStyle("text24"));
	_empty_list_text.SetText(UTranslate("No marked transactions."));

	_buy_list_display = new BuyListDisplay();
	_sell_list_display = new SellListDisplay();

	_no_transactions_text.SetStyle(TextStyle("text24"));
	_no_transactions_text.SetText(UTranslate("There are no marked purchases, sales, or trades."));

	_main_prompt.SetStyle(TextStyle("text24"));
	_main_prompt.SetText(UTranslate("Will that be all?"));
	_main_actions.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());
	_main_actions.SetPosition(60.0f, 80.0f);
	_main_actions.SetDimensions(600.0f, 40.0f, 3, 1, 3, 1);
	_main_actions.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_main_actions.SetTextStyle(TextStyle("title24"));
	_main_actions.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	_main_actions.SetCursorOffset(-55.0f, 30.0f);
	_main_actions.SetHorizontalWrapMode(VIDEO_WRAP_MODE_NONE);
	_main_actions.AddOption(UTranslate("Modify Order"));
	_main_actions.AddOption(UTranslate("Clear Order"));
	_main_actions.AddOption(UTranslate("Complete Transaction"));
	_main_actions.SetSelection(0);

	_clear_prompt.SetStyle(TextStyle("text24"));
	_clear_prompt.SetText(UTranslate("This will remove all marked purchases, sales, and trades. Are you sure?"));
	_clear_actions.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());
	_clear_actions.SetPosition(60.0f, 80.0f);
	_clear_actions.SetDimensions(400.0f, 40.0f, 2, 1, 2, 1);
	_clear_actions.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_clear_actions.SetTextStyle(TextStyle("title24"));
	_clear_actions.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	_clear_actions.SetCursorOffset(-55.0f, 30.0f);
	_clear_actions.SetHorizontalWrapMode(VIDEO_WRAP_MODE_NONE);
	_clear_actions.AddOption(UTranslate("No"));
	_clear_actions.AddOption(UTranslate("Yes"));
	_clear_actions.SetSelection(0);
}



ConfirmInterface::~ConfirmInterface() {
	if (_buy_list_display != NULL)
		delete _buy_list_display;
	if (_sell_list_display != NULL)
		delete _sell_list_display;

	_buy_list_display = NULL;
	_sell_list_display = NULL;
}



void ConfirmInterface::MakeActive() {
	map<uint32, ShopObject*>* buy_list = ShopMode::CurrentInstance()->GetBuyList();
	map<uint32, ShopObject*>* sell_list = ShopMode::CurrentInstance()->GetSellList();
	// TODO: Get a container of all trades

	// Vector constructs required by the BuyListDisplay/SellListDisplay classes
	vector<ShopObject*> buy_vector;
	vector<ShopObject*> sell_vector;

	buy_vector.reserve(buy_list->size());
	sell_vector.reserve(sell_list->size());

	_buy_count = 0;
	_buy_unique = 0;
	for (map<uint32, ShopObject*>::iterator i = buy_list->begin(); i != buy_list->end(); i++) {
		_buy_unique++;
		_buy_count += i->second->GetBuyCount();
		buy_vector.push_back(i->second);
	}

	_sell_count = 0;
	_sell_unique = 0;
	for (map<uint32, ShopObject*>::iterator i = sell_list->begin(); i != sell_list->end(); i++) {
		_sell_unique++;
		_sell_count += i->second->GetSellCount();
		sell_vector.push_back(i->second);
	}

	_trade_count = 0;
	_trade_characters = 0;
	// TODO: Iterate through the trade container similar to buy and sell containers

	_buy_list_display->PopulateList(buy_vector);
	_sell_list_display->PopulateList(sell_vector);

	// Determine which active list should be initially shown and change properties as needed
	if (_buy_count != 0) {
		_no_transactions = false;
		_active_list = ACTIVE_LIST_BUY;
		_buy_header.SetStyle(TextStyle("title24", Color::yellow));
		_sell_header.SetStyle(TextStyle("title22", Color::white));
		_trade_header.SetStyle(TextStyle("title22", Color::white));
		_properties_header.SetOptionText(3, UTranslate("Buy"));
	}
	else if (_sell_count != 0) {
		_no_transactions = false;
		_active_list = ACTIVE_LIST_SELL;
		_buy_header.SetStyle(TextStyle("title22", Color::white));
		_sell_header.SetStyle(TextStyle("title24", Color::yellow));
		_trade_header.SetStyle(TextStyle("title22", Color::white));
		_properties_header.SetOptionText(3, UTranslate("Sell"));
	}
	else if (_trade_count != 0) {
		_no_transactions = false;
		_active_list = ACTIVE_LIST_TRADE;
		_buy_header.SetStyle(TextStyle("title22", Color::white));
		_sell_header.SetStyle(TextStyle("title22", Color::white));
		_trade_header.SetStyle(TextStyle("title24", Color::yellow));
	}
	else {
		_no_transactions = true;
		_active_list = ACTIVE_LIST_BUY;
		_buy_header.SetStyle(TextStyle("title24", Color::yellow));
		_sell_header.SetStyle(TextStyle("title22", Color::white));
		_trade_header.SetStyle(TextStyle("title22", Color::white));
		_properties_header.SetOptionText(3, UTranslate("Buy"));
	}

	_RenderBuyStats();
	_RenderSellStats();
	_RenderTradeStats();

	_state = CONFIRM_STATE_MAIN;
	_main_actions.SetSelection(0);
	_clear_actions.SetSelection(0);

	_main_actions.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	_clear_actions.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	_buy_list_display->GetIdentifyList().SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	_sell_list_display->GetIdentifyList().SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

	ShopMode::CurrentInstance()->ObjectViewer()->SetSelectedObject(NULL);
	ShopMode::CurrentInstance()->ObjectViewer()->ChangeViewMode(SHOP_VIEW_MODE_INFO);
} // void ConfirmInterface::MakeActive()



void ConfirmInterface::TransactionNotification() {
	_buy_count = 0;
	_buy_unique = 0;
	_sell_count = 0;
	_sell_unique = 0;
	_trade_count = 0;
	_trade_characters = 0;
	_buy_list_display->Clear();
	_sell_list_display->Clear();
	_no_transactions = true;
	_RenderBuyStats();
	_RenderSellStats();
	_RenderTradeStats();
}



void ConfirmInterface::Update() {
	_main_actions.Update();
	_clear_actions.Update();
	_buy_list_display->Update();
	_sell_list_display->Update();

	// A swap press changes the active transaction list being shown. It takes precedence over all other
	// input events. Only the "info" state ignores this command because the transaction list is not visible
	// while in this state
	if ((InputManager->SwapPress()) && (_state != CONFIRM_STATE_INFO)) {
		_CycleActiveTransactionList();
		return;
	}

	// If no transactions are available the user can not modify, clear, or complete them. Perform no other updates
	// or processing of user input except for confirm and cancel presses to leave the interface
	if (_no_transactions == true) {
		if (InputManager->ConfirmPress() || InputManager->CancelPress()) {
			_LeaveInterface();
		}
		return;
	}

	if (_state == CONFIRM_STATE_MAIN) {
		if (InputManager->CancelPress()) {
			_LeaveInterface();
			return;
		}

		else if (InputManager->ConfirmPress()) {
			switch (_main_actions.GetSelection()) {
				case 0: // "Modify Order"
					_ChangeState(CONFIRM_STATE_LIST);
					return;
				case 1: // "Clear Order"
					_ChangeState(CONFIRM_STATE_CLEAR);
					return;
				case 2: // "Complete Transaction"
					// Make sure that at least one purchase/sale/trade is going to take place
					if ((_buy_count != 0) || (_sell_count != 0) || (_trade_count != 0)) {
						ShopMode::CurrentInstance()->Media()->GetSound("coins")->Play();
						// Calling ShopMode::CompleteTransaction() will call the TransactionNotification() method
						// for this interface class. which will reset all transaction counts and list displays.
						ShopMode::CurrentInstance()->CompleteTransaction();
					}
					// Otherwise simply clear the order without a confirmation prompt
					else {
						_ClearOrder();
					}
					return;
				default:
					IF_PRINT_WARNING(SHOP_DEBUG) << "invalid selection in primary action options: "
						<< _main_actions.GetSelection() << endl;
					return;
			}
		}

		else if (InputManager->LeftPress()) {
			_main_actions.InputLeft();
		}
		else if (InputManager->RightPress()) {
			_main_actions.InputRight();
		}
	}

	else if (_state == CONFIRM_STATE_CLEAR) {
		if (InputManager->ConfirmPress()) {
			switch (_clear_actions.GetSelection()) {
				case 0: // "No"
					_ChangeState(CONFIRM_STATE_MAIN);
					return;
				case 1: // "Yes"
					_ClearOrder();
					_ChangeState(CONFIRM_STATE_MAIN);
					return;
				default:
					IF_PRINT_WARNING(SHOP_DEBUG) << "invalid selection in clear action options: "
						<< _clear_actions.GetSelection() << endl;
					return;
			}
		}
		else if (InputManager->ConfirmPress()) {
			_ChangeState(CONFIRM_STATE_MAIN);
		}
		else if (InputManager->LeftPress()) {
			_clear_actions.InputLeft();
		}
		else if (InputManager->RightPress()) {
			_clear_actions.InputRight();
		}
	}

	else if (_state == CONFIRM_STATE_LIST) {
		if (InputManager->CancelPress()) {
			_ChangeState(CONFIRM_STATE_MAIN);
			return;
		}

		switch (_active_list) {
			case ACTIVE_LIST_BUY:
				_UpdateBuyList();
				return;
			case ACTIVE_LIST_SELL:
				_UpdateSellList();
				return;
			case ACTIVE_LIST_TRADE:
				_UpdateTradeList();
				return;
			default:
				IF_PRINT_WARNING(SHOP_DEBUG) << "invalid transaction list was active: "
					<< _active_list << endl;
				return;
		}
	}

	else if (_state == CONFIRM_STATE_INFO) {
		if (InputManager->ConfirmPress() || InputManager->CancelPress()) {
			_ChangeState(CONFIRM_STATE_LIST);
			return;
		}
	}

	else {
		IF_PRINT_WARNING(SHOP_DEBUG) << "invalid confirm state was active: " << _state << endl;
		_state = CONFIRM_STATE_MAIN;
	}
} // void ConfirmInterface::Update()



void ConfirmInterface::Draw() {
	// Draw the contents of the middle window. Either selected object information or the active transaction list
	if (_state == CONFIRM_STATE_INFO) {
		ShopMode::CurrentInstance()->ObjectViewer()->Draw();
	}
	else {
		// Draw the transaction type headers and stats
		VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
		VideoManager->Move(140.0f, 525.0f);
		_buy_header.Draw();
		VideoManager->SetDrawFlags(VIDEO_Y_TOP, 0);
		_buy_stats.Draw();

		VideoManager->SetDrawFlags(VIDEO_Y_BOTTOM, 0);
		VideoManager->MoveRelative(0.0f, -105.0f);
		_sell_header.Draw();
		VideoManager->SetDrawFlags(VIDEO_Y_TOP, 0);
		_sell_stats.Draw();

		VideoManager->SetDrawFlags(VIDEO_Y_BOTTOM, 0);
		VideoManager->MoveRelative(0.0f, -105.0f);
		_trade_header.Draw();
		VideoManager->SetDrawFlags(VIDEO_Y_TOP, 0);
		_trade_stats.Draw();

		// Draw the list headers and contents
		if ((_active_list == ACTIVE_LIST_BUY) && (_buy_list_display->IsListEmpty() == false)) {
			VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
			VideoManager->Move(295.0f, 558.0f);
			_name_header.Draw();

			_properties_header.Draw();
			_buy_list_display->Draw();
		}
		else if ((_active_list == ACTIVE_LIST_SELL) && (_sell_list_display->IsListEmpty() == false)) {
			VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
			VideoManager->Move(295.0f, 558.0f);
			_name_header.Draw();

			_properties_header.Draw();
			_sell_list_display->Draw();
		}
		else if ((_active_list == ACTIVE_LIST_TRADE) && (_trade_count != 0)) {
			// TODO: once trade support is added, change above condition `(_trade_count != 0)`
			// to a check on the trade list display's empty status
		}
		else { // The active list is empty
			VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
			VideoManager->Move(560.0f, 405.0f);
			_empty_list_text.Draw();
		}
	}

	// Draw the contents of the lower window. Either the clear prompt/actions or the main prompt/actions
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_CENTER, 0);
	VideoManager->Move(160.0f, 180.0f);
	if (_no_transactions == true) {
		_no_transactions_text.Draw();
	}
	else if (_state == CONFIRM_STATE_CLEAR) {
		_clear_prompt.Draw();
		_clear_actions.Draw();
	}
	else {
		_main_prompt.Draw();
		_main_actions.Draw();
	}
} // void ConfirmInterface::Draw()



void ConfirmInterface::_ChangeState(CONFIRM_STATE new_state) {
	if (_state == new_state) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "new state was the same as the current state: " << _state << endl;
		return;
	}

	// Modify appropriate display properties of former state
	if (_state == CONFIRM_STATE_MAIN) {
		_main_actions.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	}
	else if (_state == CONFIRM_STATE_CLEAR) {
		_clear_actions.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	}
	else if (_state == CONFIRM_STATE_LIST) {
		_buy_list_display->GetIdentifyList().SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
		_sell_list_display->GetIdentifyList().SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	}
	else if (_state == CONFIRM_STATE_INFO) {
		ShopMode::CurrentInstance()->ObjectViewer()->SetSelectedObject(NULL);
	}

	// Modify appropriate display properties of new state
	if (new_state == CONFIRM_STATE_MAIN) {
		_main_actions.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	}
	else if (new_state == CONFIRM_STATE_CLEAR) {
		_clear_actions.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
		_clear_actions.SetSelection(0);
	}
	else if (new_state == CONFIRM_STATE_LIST) {
		_buy_list_display->GetIdentifyList().SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
		_sell_list_display->GetIdentifyList().SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	}
	else if (new_state == CONFIRM_STATE_INFO) {
		ShopObject* selected_object = NULL;
		switch (_active_list) {
			case ACTIVE_LIST_BUY:
				selected_object = _buy_list_display->GetSelectedObject();
				break;
			case ACTIVE_LIST_SELL:
				selected_object = _sell_list_display->GetSelectedObject();
				break;
			case ACTIVE_LIST_TRADE:
				// TODO: implement once trade interface is complete
				break;
			default:
				IF_PRINT_WARNING(SHOP_DEBUG) << "invalid transaction list was active: " << _active_list << endl;
				break;
		}
		ShopMode::CurrentInstance()->ObjectViewer()->SetSelectedObject(selected_object);
	}
	_state = new_state;
} // void ConfirmInterface::_ChangeState(CONFIRM_STATE new_state)



void ConfirmInterface::_CycleActiveTransactionList() {
	TextStyle highlight("title24", Color::yellow);
	TextStyle standard("title22", Color::white);

	if (_active_list == ACTIVE_LIST_BUY) {
		_active_list = ACTIVE_LIST_SELL;
		_buy_header.SetStyle(standard);
		_sell_header.SetStyle(highlight);
		_properties_header.SetOptionText(3, UTranslate("Sell"));
	}
	else if (_active_list == ACTIVE_LIST_SELL) {
		_active_list = ACTIVE_LIST_TRADE;
		_sell_header.SetStyle(standard);
		_trade_header.SetStyle(highlight);
	}
	else if (_active_list == ACTIVE_LIST_TRADE) {
		_active_list = ACTIVE_LIST_BUY;
		_trade_header.SetStyle(standard);
		_buy_header.SetStyle(highlight);
		_properties_header.SetOptionText(3, UTranslate("Buy"));
	}
	else {
		IF_PRINT_WARNING(SHOP_DEBUG) << "invalid transaction list was active: " << _active_list << endl;
	}
}



void ConfirmInterface::_UpdateBuyList() {
	if (_buy_list_display->IsListEmpty() == true)
		return;

	if (InputManager->ConfirmPress()) {
		_ChangeState(CONFIRM_STATE_INFO);
	}
	else if (InputManager->UpPress()) {
		_buy_list_display->InputUp();
	}
	else if (InputManager->DownPress()) {
		_buy_list_display->InputDown();
	}
	else if (InputManager->LeftPress()) {
		if (ChangeBuyQuantity(false) == true)
			ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
		else
			ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
	}
	else if (InputManager->RightPress()) {
		if (ChangeBuyQuantity(true) == true)
			ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
		else
			ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
	}
	else if (InputManager->LeftSelectPress()) {
		if (ChangeBuyQuantity(false, 10) == true)
			ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
		else
			ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
	}
	else if (InputManager->RightSelectPress()) {
		if (ChangeBuyQuantity(true, 10) == true)
			ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
		else
			ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
	}
}



void ConfirmInterface::_UpdateSellList() {
	if (_sell_list_display->IsListEmpty() == true)
		return;

	if (InputManager->ConfirmPress()) {
		_ChangeState(CONFIRM_STATE_INFO);
	}
	else if (InputManager->UpPress()) {
		_sell_list_display->InputUp();
	}
	else if (InputManager->DownPress()) {
		_sell_list_display->InputDown();
	}
	else if (InputManager->LeftPress()) {
		if (ChangeSellQuantity(false) == true)
			ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
		else
			ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
	}
	else if (InputManager->RightPress()) {
		if (ChangeSellQuantity(true) == true)
			ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
		else
			ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
	}
	else if (InputManager->LeftSelectPress()) {
		if (ChangeSellQuantity(false, SHOP_BATCH_COUNT) == true)
			ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
		else
			ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
	}
	else if (InputManager->RightSelectPress()) {
		if (ChangeSellQuantity(true, SHOP_BATCH_COUNT) == true)
			ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
		else
			ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
	}
}



void ConfirmInterface::_UpdateTradeList() {
	// TODO: implement this method when trade interface is complete
}



bool ConfirmInterface::ChangeBuyQuantity(bool less_or_more, uint32 amount) {
	ShopObject* obj = _buy_list_display->GetSelectedObject();
	int32 old_count = obj->GetBuyCount();

	if (_buy_list_display->ChangeBuyQuantity(less_or_more, amount) == false) {
		return false;
	}

	// new_count should never equal old_count if BuyListDisplay::ChangeBuyQuantity() returned true
	int32 new_count = obj->GetBuyCount();

	// Update the unique and count buy members and re-render the stats
	if (old_count == 0)
		_buy_unique++;
	if (new_count == 0)
		_buy_unique--;
	_buy_count += (new_count - old_count);
	_RenderBuyStats();
	return true;
}



bool ConfirmInterface::ChangeSellQuantity(bool less_or_more, uint32 amount) {
	ShopObject* obj = _sell_list_display->GetSelectedObject();
	int32 old_count = obj->GetSellCount();

	if (_sell_list_display->ChangeSellQuantity(less_or_more, amount) == false) {
		return false;
	}

	// new_count should never equal old_count if SellListDisplay::ChangeSellQuantity() returned true
	int32 new_count = obj->GetSellCount();

	// Update the unique and count sell members and re-render the stats
	if (old_count == 0)
		_sell_unique++;
	if (new_count == 0)
		_sell_unique--;
	_sell_count += (new_count - old_count);
	_RenderSellStats();
	return true;
}



void ConfirmInterface::_RenderBuyStats() {
	_buy_stats.SetText(NumberToString(_buy_count) + Translate(" count") + "\n" +
		NumberToString(_buy_unique) + Translate(" unique"));
}



void ConfirmInterface::_RenderSellStats() {
	_sell_stats.SetText(NumberToString(_sell_count) + Translate(" count") + "\n" +
		NumberToString(_sell_unique) + Translate(" unique"));
}



void ConfirmInterface::_RenderTradeStats() {
	_trade_stats.SetText(NumberToString(_trade_count) + Translate(" count") + "\n" +
		NumberToString(_trade_characters) + Translate(" characters"));
}



void ConfirmInterface::_ClearOrder() {
	_buy_count = 0;
	_buy_unique = 0;
	_sell_count = 0;
	_sell_unique = 0;
	_trade_count = 0;
	_trade_characters = 0;
	_buy_list_display->Clear();
	_sell_list_display->Clear();
	_no_transactions = true;
	ShopMode::CurrentInstance()->ClearOrder();
	// TODO: play appropriate sound
}



void ConfirmInterface::_LeaveInterface() {
	map<uint32, ShopObject*>* buy_list = ShopMode::CurrentInstance()->GetBuyList();
	map<uint32, ShopObject*>* sell_list = ShopMode::CurrentInstance()->GetSellList();

	// Go through the lists and remove any entries that have had their counts set to zero
	for (map<uint32, ShopObject*>::iterator i = buy_list->begin(); i != buy_list->end();) {
		if (i->second->GetBuyCount() == 0)
			buy_list->erase(i++);
		else
			i++;
	}
	for (map<uint32, ShopObject*>::iterator i = sell_list->begin(); i != sell_list->end();) {
		if (i->second->GetSellCount() == 0)
			sell_list->erase(i++);
		else
			i++;
	}

	ShopMode::CurrentInstance()->ChangeState(SHOP_STATE_ROOT);
}

} // namespace private_shop

} // namespace hoa_shop
