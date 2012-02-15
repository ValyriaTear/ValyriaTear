///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_utils.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for shop mode utility code.
***
*** This file contains utility code that is shared among the various shop mode
*** classes.
*** ***************************************************************************/

#include "video.h"

#include "global.h"

#include "shop_utils.h"
#include "shop.h"

using namespace std;

using namespace hoa_utils;

using namespace hoa_system;
using namespace hoa_video;
using namespace hoa_gui;

using namespace hoa_global;

namespace hoa_shop {

namespace private_shop {

// *****************************************************************************
// ***** ShopObject class methods
// *****************************************************************************

ShopObject::ShopObject(GlobalObject* object, bool sold_by_shop) :
	_object(object),
	_sold_in_shop(sold_by_shop),
	_buy_price(0),
	_sell_price(0),
	_own_count(0),
	_stock_count(0),
	_buy_count(0),
	_sell_count(0)
{
	assert(_object != NULL);
}



SHOP_OBJECT ShopObject::DetermineShopObjectType(GLOBAL_OBJECT global_type) {
	SHOP_OBJECT shop_type;

	switch (global_type) {
		case GLOBAL_OBJECT_ITEM:
			shop_type = SHOP_OBJECT_ITEM;
			break;
		case GLOBAL_OBJECT_WEAPON:
		case GLOBAL_OBJECT_HEAD_ARMOR:
		case GLOBAL_OBJECT_TORSO_ARMOR:
		case GLOBAL_OBJECT_ARM_ARMOR:
		case GLOBAL_OBJECT_LEG_ARMOR:
			shop_type = SHOP_OBJECT_EQUIPMENT;
			break;
		case GLOBAL_OBJECT_SHARD:
			shop_type = SHOP_OBJECT_SHARD;
			break;
		case GLOBAL_OBJECT_KEY_ITEM:
			shop_type = SHOP_OBJECT_KEY_ITEM;
			break;
		case GLOBAL_OBJECT_INVALID:
		case GLOBAL_OBJECT_TOTAL:
		default:
			IF_PRINT_WARNING(SHOP_DEBUG) << "no conversion type existed for global object: " << global_type << endl;
			shop_type = SHOP_OBJECT_INVALID;
			break;
	}

	return shop_type;
}



SHOP_OBJECT ShopObject::DetermineShopObjectType() {
	return DetermineShopObjectType(GetObject()->GetObjectType());
}



void ShopObject::SetPricing(SHOP_PRICE_LEVEL buy_level, SHOP_PRICE_LEVEL sell_level) {
	_buy_price = _object->GetPrice();
	_sell_price = _object->GetPrice();

	switch (buy_level) {
		case SHOP_PRICE_VERY_GOOD:
			_buy_price *= BUY_PRICE_VERY_GOOD;
			break;
		case SHOP_PRICE_GOOD:
			_buy_price *= BUY_PRICE_GOOD;
			break;
		case SHOP_PRICE_STANDARD:
			_buy_price *= BUY_PRICE_STANDARD;
			break;
		case SHOP_PRICE_POOR:
			_buy_price *= BUY_PRICE_POOR;
			break;
		case SHOP_PRICE_VERY_POOR:
			_buy_price *= BUY_PRICE_VERY_POOR;
			break;
		default:
			IF_PRINT_WARNING(SHOP_DEBUG) << "unknown buy level: " << buy_level << endl;
	}

	switch (sell_level) {
		case SHOP_PRICE_VERY_GOOD:
			_sell_price *= SELL_PRICE_VERY_GOOD;
			break;
		case SHOP_PRICE_GOOD:
			_sell_price *= SELL_PRICE_GOOD;
			break;
		case SHOP_PRICE_STANDARD:
			_sell_price *= SELL_PRICE_STANDARD;
			break;
		case SHOP_PRICE_POOR:
			_sell_price *= SELL_PRICE_POOR;
			break;
		case SHOP_PRICE_VERY_POOR:
			_sell_price *= SELL_PRICE_VERY_POOR;
			break;
		default:
			IF_PRINT_WARNING(SHOP_DEBUG) << "unknown sell level: " << sell_level << endl;
	}
}



void ShopObject::IncrementOwnCount(uint32 inc) {
	_own_count += inc;
}



void ShopObject::IncrementStockCount(uint32 inc) {
	_stock_count += inc;
}



void ShopObject::IncrementBuyCount(uint32 inc) {
	uint32 old_count = _buy_count;
	if (inc == 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function received an argument with a value of zero" << endl;
		return;
	}

	_buy_count += inc;
	if (_stock_count < _buy_count) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "incremented buy count beyond the amount available in stock" << endl;
		_buy_count = old_count;
		return;
	}
	if (old_count == 0) {
		ShopMode::CurrentInstance()->AddObjectToBuyList(this);
	}
}



void ShopObject::IncrementSellCount(uint32 inc) {
	uint32 old_count = _sell_count;
	if (inc == 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function received an argument with a value of zero" << endl;
		return;
	}

	_sell_count += inc;
	if (_sell_count > _own_count) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "incremented sell count beyond the amount available to be sold" << endl;
		_sell_count -= inc;
		return;
	}
	if (old_count == 0) {
		ShopMode::CurrentInstance()->AddObjectToSellList(this);
	}
}



void ShopObject::DecrementOwnCount(uint32 dec) {
	if (dec > _own_count) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "attempted to decrement own count below zero" << endl;
		return;
	}

	_own_count -= dec;

	if (_own_count < _sell_count) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "decremented own count below that of the sell count" << endl;
		_own_count += dec;
	}
}



void ShopObject::DecrementStockCount(uint32 dec) {
	if (dec > _stock_count) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "attempted to decrement stock count below zero" << endl;
		return;
	}

	_stock_count -= dec;

	if (_stock_count < _buy_count) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "decremented stock count below that of the buy count" << endl;
		_stock_count += dec;
	}
}



void ShopObject::DecrementBuyCount(uint32 dec) {
	if (dec == 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function received an argument with a value of zero" << endl;
		return;
	}

	if (dec > _buy_count) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "attempted to decrement buy count below zero" << endl;
		return;
	}

	_buy_count -= dec;
	if (_buy_count == 0) {
		ShopMode::CurrentInstance()->RemoveObjectFromBuyList(this);
	}
}



void ShopObject::DecrementSellCount(uint32 dec) {
	if (dec == 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function received an argument with a value of zero" << endl;
		return;
	}

	if (dec > _sell_count) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "attempted to decrement sell count below zero" << endl;
		return;
	}

	_sell_count -= dec;
	if (_sell_count == 0) {
		ShopMode::CurrentInstance()->RemoveObjectFromSellList(this);
	}
}

// *****************************************************************************
// ***** ObjectCategoryDisplay class methods
// *****************************************************************************

// The time it takes to transition graphics to a new category (in milliseconds)
const uint32 TRANSITION_TIME_ICON = 500;
// Represents the display speed when transitioning to new category text
const float TRANSITION_TIME_TEXT = 25.0f;

ObjectCategoryDisplay::ObjectCategoryDisplay() :
	_view_mode(SHOP_VIEW_MODE_LIST),
	_selected_object(NULL),
	_current_icon(NULL),
	_last_icon(NULL),
	_object_icon(NULL)
{
	_name_text.SetStyle(TextStyle("text22"));

	_name_textbox.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());
	_name_textbox.SetPosition(25.0f, 175.0f);
	_name_textbox.SetDimensions(125.0f, 30.0f);
	_name_textbox.SetTextStyle(TextStyle("text22"));
	_name_textbox.SetDisplayMode(VIDEO_TEXT_FADECHAR);
	_name_textbox.SetDisplaySpeed(TRANSITION_TIME_TEXT);
	_name_textbox.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	// TODO: Alignment should be VIDEO_X_CENTER, but a bug is preventing it from working correctly right now
	_name_textbox.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);

	_transition_timer.Initialize(TRANSITION_TIME_ICON, SYSTEM_TIMER_NO_LOOPS);
	_transition_timer.EnableAutoUpdate(ShopMode::CurrentInstance());
}



ObjectCategoryDisplay::~ObjectCategoryDisplay()
{
	_selected_object = NULL;
	_current_icon = NULL;
	_last_icon = NULL;
	_object_icon = NULL;
}



void ObjectCategoryDisplay::Update() {
	_name_textbox.Update();
}



void ObjectCategoryDisplay::Draw() {
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);

	if (_view_mode == SHOP_VIEW_MODE_LIST) {
		VideoManager->Move(200.0f, 410.0f);

		if (_transition_timer.IsRunning() == true) {
			// Alpha ranges from 0.0f at timer start to 1.0f at end
			float alpha = static_cast<float>(_transition_timer.GetTimeExpired()) / static_cast<float>(TRANSITION_TIME_ICON);

			if (_last_icon != NULL)
				_last_icon->Draw(Color(1.0f, 1.0f, 1.0f, 1.0f - alpha));
			if (_current_icon != NULL)
				_current_icon->Draw(Color(1.0f, 1.0f, 1.0f, alpha));
		}
		else if (_current_icon != NULL) {
			_current_icon->Draw();
		}
		_name_textbox.Draw();
	}
	else if ((_view_mode == SHOP_VIEW_MODE_INFO) && (_selected_object != NULL)) {
		VideoManager->Move(200.0f, 165.0f);
		_object_icon->Draw();
		VideoManager->MoveRelative(0.0f, -45.0f);
		_name_text.Draw();
	}
}



void ObjectCategoryDisplay::ChangeViewMode(SHOP_VIEW_MODE new_mode) {
	if (_view_mode == new_mode) {
		return;
	}

	if (new_mode == SHOP_VIEW_MODE_LIST) {
		_view_mode = new_mode;
		_transition_timer.Finish();
	}
	else if (new_mode == SHOP_VIEW_MODE_INFO) {
		_view_mode = new_mode;
	}
	else {
		IF_PRINT_WARNING(SHOP_DEBUG) << "invalid/unknown view mode requested: " << new_mode << endl;
		return;
	}
}



void ObjectCategoryDisplay::SetSelectedObject(ShopObject* shop_object) {
	if (_selected_object == shop_object) {
		return;
	}

	_selected_object = shop_object;

	if (_selected_object == NULL) {
		_object_icon = NULL;
		_name_text.SetText("");
		return;
	}
	else {
		GLOBAL_OBJECT object_type = _selected_object->GetObject()->GetObjectType();
		_name_text.SetText(*(ShopMode::CurrentInstance()->Media()->GetCategoryName(object_type)));
		_object_icon = ShopMode::CurrentInstance()->Media()->GetCategoryIcon(object_type);
	}
}



void ObjectCategoryDisplay::ChangeCategory(ustring& name, const StillImage* icon) {
	if (icon == NULL) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function was passed a NULL pointer argument" << endl;
	}

	_name_textbox.SetDisplayText(name);

	_last_icon = _current_icon;
	_current_icon = icon;

	if (_view_mode == SHOP_VIEW_MODE_LIST) {
		_transition_timer.Reset();
		_transition_timer.Run();
	}
}

// *****************************************************************************
// ***** ObjectListDisplay class methods
// *****************************************************************************

ObjectListDisplay::ObjectListDisplay() {
	_identify_list.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());
	_identify_list.SetPosition(180.0f, 330.0f);
	_identify_list.SetDimensions(300.0f, 300.0f, 1, 255, 1, 8);
	_identify_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_identify_list.SetTextStyle(TextStyle("text22"));
	_identify_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	_identify_list.SetSelectMode(VIDEO_SELECT_SINGLE);
	_identify_list.SetCursorOffset(-50.0f, 20.0f);
	_identify_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_NONE);
	_identify_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);

	_property_list.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());
	_property_list.SetPosition(480.0f, 330.0f);
	_property_list.SetDimensions(300.0f, 300.0f, 4, 255, 4, 8);
	_property_list.SetOptionAlignment(VIDEO_X_RIGHT, VIDEO_Y_CENTER);
	_property_list.SetTextStyle(TextStyle("text22"));
	_property_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	_property_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_NONE);
	_property_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
}



void ObjectListDisplay::Clear() {
	_objects.clear();
	_identify_list.ClearOptions();
	_property_list.ClearOptions();
}



void ObjectListDisplay::PopulateList(vector<ShopObject*>& objects) {
	_objects = objects;
	ReconstructList();
}



void ObjectListDisplay::RefreshAllEntries() {
	if (IsListEmpty() == true)
		return;

	for (uint32 i = 0; i < _objects.size(); i++)
		RefreshEntry(i);
}



ShopObject* ObjectListDisplay::GetSelectedObject() {
	if (IsListEmpty() == true)
		return NULL;

	if (static_cast<uint32>(_identify_list.GetSelection()) >= _objects.size()) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "current selection index exceeds available objects: " << _identify_list.GetSelection() << endl;
		return NULL;
	}

	return _objects[_identify_list.GetSelection()];
}



void ObjectListDisplay::ResetSelection() {
	if (IsListEmpty() == false) {
		_identify_list.SetSelection(0);
		_property_list.SetSelection(0);
	}
}



uint32 ObjectListDisplay::GetCurrentSelection() {
	if (IsListEmpty() == true)
		return 0;
	else
		return static_cast<uint32>(_identify_list.GetSelection());
}



void ObjectListDisplay::InputUp() {
	_identify_list.InputUp();
	_property_list.InputUp();
}



void ObjectListDisplay::InputDown() {
	_identify_list.InputDown();
	_property_list.InputDown();
}



void ObjectListDisplay::Update() {
	_identify_list.Update();
	_property_list.Update();
}



void ObjectListDisplay::Draw() {
	_identify_list.Draw();
	_property_list.Draw();
}

} // namespace private_shop

} // namespace hoa_shop
