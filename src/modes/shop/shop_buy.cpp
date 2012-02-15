///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_buy.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for buy interface of shop mode
***
*** \note The contents of this file are near identical to the contents of
*** shop_sell.cpp. When making any changes to this file, please look to shop_sell.cpp
*** to see if it should have similar changes made.
*** ***************************************************************************/

#include "defs.h"
#include "utils.h"

#include "audio.h"
#include "input.h"
#include "system.h"
#include "video.h"

#include "global.h"

#include "shop.h"
#include "shop_buy.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_input;
using namespace hoa_system;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_global;

namespace hoa_shop {

namespace private_shop {

// *****************************************************************************
// ***** BuyInterface class methods
// *****************************************************************************

BuyInterface::BuyInterface() :
	_view_mode(SHOP_VIEW_MODE_INVALID),
	_selected_object(NULL),
	_current_category(0)
{
	_category_header.SetStyle(TextStyle("title24"));
	_category_header.SetText(UTranslate("Category"));

	_name_header.SetStyle(TextStyle("title24"));
	_name_header.SetText(UTranslate("Name"));

	_properties_header.SetDimensions(300.0f, 30.0f, 4, 1, 4, 1);
	_properties_header.SetOptionAlignment(VIDEO_X_RIGHT, VIDEO_Y_CENTER);
	_properties_header.SetTextStyle(TextStyle("title24"));
	_properties_header.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	_properties_header.AddOption(UTranslate("Price"));
	_properties_header.AddOption(UTranslate("Stock"));
	_properties_header.AddOption(UTranslate("Own"));
	_properties_header.AddOption(UTranslate("Buy"));

	_selected_name.SetStyle(TextStyle("text22"));

	_selected_properties.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());
	_selected_properties.SetPosition(480.0f, 80.0f);
	_selected_properties.SetDimensions(300.0f, 30.0f, 4, 1, 4, 1);
	_selected_properties.SetOptionAlignment(VIDEO_X_RIGHT, VIDEO_Y_CENTER);
	_selected_properties.SetTextStyle(TextStyle("text22"));
	_selected_properties.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	_selected_properties.AddOption(ustring());
	_selected_properties.AddOption(ustring());
	_selected_properties.AddOption(ustring());
	_selected_properties.AddOption(ustring());
}



BuyInterface::~BuyInterface() {
	for (uint32 i = 0; i < _list_displays.size(); i++) {
		delete _list_displays[i];
	}
}



void BuyInterface::Initialize() {
	// ---------- (1): Load all category names and icon images to be used
	_number_categories = ShopMode::CurrentInstance()->Media()->GetSaleCategoryNames()->size();

	_category_names = *(ShopMode::CurrentInstance()->Media()->GetSaleCategoryNames());
	vector<StillImage>* icons = ShopMode::CurrentInstance()->Media()->GetSaleCategoryIcons();
	for (uint32 i = 0; i < icons->size(); i++) {
		_category_icons.push_back(&(icons->at(i)));
	}

	// Set the initial category to the last category that was added (this is usually "All Wares")
	_current_category = _number_categories - 1;
	// Initialize the category display with the initial category
	_category_display.ChangeCategory(_category_names[_current_category], _category_icons[_current_category]);

	// ---------- (2): Prepare object data containers and determine category index mappings
	// Containers of object data used to populate the display lists
	vector<vector<ShopObject*> > object_data;

	for (uint32 i = 0; i < _number_categories; i++) {
		object_data.push_back(vector<ShopObject*>());
	}

	// Bit-vector that indicates what types of objects are sold in the shop
	uint8 deal_types = ShopMode::CurrentInstance()->GetDealTypes();
	// Holds the index to the _object_data vector where the container for a specific object type is located
	vector<uint32> type_index(GLOBAL_OBJECT_TOTAL, 0);
	// Used to set the appropriate data in the type_index vector
	uint32 next_index = 0;
	// Used to do a bit-by-bit analysis of the deal_types variable
	uint8 bit_x = 0x01;

	// This loop determines where each type of object should be placed in the object_data container. For example,
	// if the available categories in the shop are items, weapons, shards, and all wares, the size of object_data
	// will be four. When we go to add an object of one of these types into the object_data container, we need
	// to know the correct index for each type of object. These indeces are stored in the type_index vector. The
	// size of this vector is the number of object types, so it becomes simple to map each object type to its correct
	// location in object_data.
	for (uint8 i = 0; i < GLOBAL_OBJECT_TOTAL; i++, bit_x <<= 1) {
		// Check if the type is available by doing a bit-wise comparison
		if (deal_types & bit_x) {
			type_index[i] = next_index++;
		}
	}

	// ---------- (3): Populate the object_data containers
	// Used to temporarily hold a pointer to a valid shop object
	ShopObject* obj = NULL;
	// Pointer to the container of all objects that are bought/sold/traded in the ship
	map<uint32, ShopObject>* shop_objects = ShopMode::CurrentInstance()->GetShopObjects();

	for (map<uint32, ShopObject>::iterator i = shop_objects->begin(); i != shop_objects->end(); i++) {
		obj = &(i->second);

		if (obj->IsSoldInShop() == true) {
			switch (obj->GetObject()->GetObjectType()) {
				case GLOBAL_OBJECT_ITEM:
					object_data[type_index[0]].push_back(obj);
					break;
				case GLOBAL_OBJECT_WEAPON:
					object_data[type_index[1]].push_back(obj);
					break;
				case GLOBAL_OBJECT_HEAD_ARMOR:
					object_data[type_index[2]].push_back(obj);
					break;
				case GLOBAL_OBJECT_TORSO_ARMOR:
					object_data[type_index[3]].push_back(obj);
					break;
				case GLOBAL_OBJECT_ARM_ARMOR:
					object_data[type_index[4]].push_back(obj);
					break;
				case GLOBAL_OBJECT_LEG_ARMOR:
					object_data[type_index[5]].push_back(obj);
					break;
				case GLOBAL_OBJECT_SHARD:
					object_data[type_index[6]].push_back(obj);
					break;
				case GLOBAL_OBJECT_KEY_ITEM:
					object_data[type_index[7]].push_back(obj);
					break;
				default:
					IF_PRINT_WARNING(SHOP_DEBUG) << "added object of unknown type: " << obj->GetObject()->GetObjectType() << endl;
					break;
			}

			// If there is an "All Wares" category, make sure the object gets added there as well
			if (_number_categories > 1) {
				object_data.back().push_back(obj);
			}
		}
	}

	// ---------- (4): Create the buy displays using the object data that is now ready
	for (uint32 i = 0; i < object_data.size(); i++) {
		BuyListDisplay* new_list = new BuyListDisplay();
		new_list->PopulateList(object_data[i]);
		_list_displays.push_back(new_list);
	}

	_selected_object = _list_displays[_current_category]->GetSelectedObject();
	_ChangeViewMode(SHOP_VIEW_MODE_LIST);
} // void BuyInterface::Initialize()



void BuyInterface::MakeActive() {
	// Buy counts may have be modified externally so a complete list refresh is necessary
	for (uint32 i = 0; i < _list_displays.size(); i++)
		_list_displays[i]->RefreshAllEntries();

	_selected_object = _list_displays[_current_category]->GetSelectedObject();
	ShopMode::CurrentInstance()->ObjectViewer()->ChangeViewMode(_view_mode);
	ShopMode::CurrentInstance()->ObjectViewer()->SetSelectedObject(_selected_object);
	_category_display.ChangeViewMode(_view_mode);
	_category_display.SetSelectedObject(_selected_object);
}



void BuyInterface::TransactionNotification() {
	for (uint32 i = 0; i < _list_displays.size(); i++) {
		_list_displays[i]->ReconstructList();
		_list_displays[i]->ResetSelection();
	}

	_current_category = _number_categories - 1;
	_view_mode = SHOP_VIEW_MODE_LIST;
}



void BuyInterface::Update() {
	if (_view_mode == SHOP_VIEW_MODE_LIST) {
		if (InputManager->ConfirmPress()) {
			_ChangeViewMode(SHOP_VIEW_MODE_INFO);
		}
		else if (InputManager->CancelPress()) {
			ShopMode::CurrentInstance()->ChangeState(SHOP_STATE_ROOT);
			ShopMode::CurrentInstance()->Media()->GetSound("cancel")->Play();
		}

		// Swap cycles through the object categories
		else if (InputManager->SwapPress() && (_number_categories > 1)) {
			if (_ChangeCategory(true) == true)
				ShopMode::CurrentInstance()->ObjectViewer()->SetSelectedObject(_selected_object);
			ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
		}

		// Up/down changes the selected object in the current list
		else if (InputManager->UpPress()) {
			if (_ChangeSelection(false) == true) {
				ShopMode::CurrentInstance()->ObjectViewer()->SetSelectedObject(_selected_object);
				ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
			}
		}
		else if (InputManager->DownPress()) {
			if (_ChangeSelection(true) == true) {
				ShopMode::CurrentInstance()->ObjectViewer()->SetSelectedObject(_selected_object);
				ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
			}
		}

		// Left/right change the quantity of the object to buy
		else if (InputManager->LeftPress()) {
			if (_list_displays[_current_category]->ChangeBuyQuantity(false) == true)
				ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
			else
				ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
		}
		else if (InputManager->RightPress()) {
			if (_list_displays[_current_category]->ChangeBuyQuantity(true) == true)
				ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
			else
				ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
		}

		// Left select/right select change the quantity of the object to buy by a batch at a time
		else if (InputManager->LeftSelectPress()) {
			if (_list_displays[_current_category]->ChangeBuyQuantity(false, SHOP_BATCH_COUNT) == true)
				ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
			else
				ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
		}
		else if (InputManager->RightSelectPress()) {
			if (_list_displays[_current_category]->ChangeBuyQuantity(true, SHOP_BATCH_COUNT) == true)
				ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
			else
				ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
		}
	} // if (_view_mode == SHOP_VIEW_MODE_LIST)

	else if (_view_mode == SHOP_VIEW_MODE_INFO) {
		if (InputManager->ConfirmPress() || InputManager->CancelPress()) {
			_ChangeViewMode(SHOP_VIEW_MODE_LIST);
		}

		// Left/right change the quantity of the object to buy
		else if (InputManager->LeftPress()) {
			if (_list_displays[_current_category]->ChangeBuyQuantity(false) == true) {
				_RefreshSelectedProperties();
				ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
			}
			else
				ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
		}
		else if (InputManager->RightPress()) {
			if (_list_displays[_current_category]->ChangeBuyQuantity(true) == true) {
				_RefreshSelectedProperties();
				ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
			}
			else
				ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
		}

		// Left select/right select change the quantity of the object to buy by a batch at a time
		else if (InputManager->LeftSelectPress()) {
			if (_list_displays[_current_category]->ChangeBuyQuantity(false, SHOP_BATCH_COUNT) == true) {
				_RefreshSelectedProperties();
				ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
			}
			else
				ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
		}
		else if (InputManager->RightSelectPress()) {
			if (_list_displays[_current_category]->ChangeBuyQuantity(true, SHOP_BATCH_COUNT) == true) {
				_RefreshSelectedProperties();
				ShopMode::CurrentInstance()->Media()->GetSound("confirm")->Play();
			}
			else
				ShopMode::CurrentInstance()->Media()->GetSound("bump")->Play();
		}
	}

	_category_display.Update();
	_list_displays[_current_category]->Update();
	ShopMode::CurrentInstance()->ObjectViewer()->Update();
} // void BuyInterface::Update()



void BuyInterface::Draw() {
	if (_view_mode == SHOP_VIEW_MODE_LIST) {
		VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
		VideoManager->Move(200.0f, 558.0f);
		_category_header.Draw();

		VideoManager->SetDrawFlags(VIDEO_X_LEFT, 0);
		VideoManager->MoveRelative(95.0f, 0.0f);
		_name_header.Draw();

		_properties_header.Draw();

		_category_display.Draw();
		_list_displays[_current_category]->Draw();
	}
	else if (_view_mode == SHOP_VIEW_MODE_INFO) {
		VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
		VideoManager->Move(295.0f, 175.0f);
		_name_header.Draw();
		_properties_header.Draw();

		VideoManager->MoveRelative(0.0f, -50.0f);
		_selected_icon.Draw();
		VideoManager->MoveRelative(30.0f, 0.0f);
		_selected_name.Draw();
		_selected_properties.Draw();

		_category_display.Draw();
	}

	ShopMode::CurrentInstance()->ObjectViewer()->Draw();
}



void BuyInterface::_ChangeViewMode(SHOP_VIEW_MODE new_mode) {
	if (_view_mode == new_mode)
		return;

	if (new_mode == SHOP_VIEW_MODE_LIST) {
		_view_mode = new_mode;
		ShopMode::CurrentInstance()->ObjectViewer()->ChangeViewMode(_view_mode);
		_category_display.ChangeViewMode(_view_mode);

		_properties_header.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());
		_properties_header.SetPosition(480.0f, 390.0f);
	}
	else if (new_mode == SHOP_VIEW_MODE_INFO) {
		_view_mode = new_mode;
		ShopMode::CurrentInstance()->ObjectViewer()->ChangeViewMode(_view_mode);
		_category_display.ChangeViewMode(_view_mode);
		_category_display.SetSelectedObject(_selected_object);

		_properties_header.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());
		_properties_header.SetPosition(480.0f, 130.0f);

		_selected_name.SetText(_selected_object->GetObject()->GetName());
		_selected_icon = _selected_object->GetObject()->GetIconImage();
		_selected_icon.SetDimensions(30.0f, 30.0f);
		_selected_properties.SetOptionText(0, MakeUnicodeString(NumberToString(_selected_object->GetBuyPrice())));
		_selected_properties.SetOptionText(1, MakeUnicodeString("×" + NumberToString(_selected_object->GetStockCount())));
		_selected_properties.SetOptionText(2, MakeUnicodeString("×" + NumberToString(_selected_object->GetOwnCount())));
		_selected_properties.SetOptionText(3, MakeUnicodeString("×" + NumberToString(_selected_object->GetBuyCount())));
	}
	else {
		IF_PRINT_WARNING(SHOP_DEBUG) << "tried to change to an invalid/unsupported view mode: " << new_mode << endl;
	}
}



bool BuyInterface::_ChangeCategory(bool left_or_right) {
	if (_number_categories == 1) {
		return false;
	}

	if (left_or_right == false) {
		_current_category = (_current_category == 0) ? (_number_categories - 1) : (_current_category - 1);
	}
	else {
		_current_category = (_current_category >= (_number_categories - 1)) ? 0 : (_current_category + 1);
	}

	_category_display.ChangeCategory(_category_names[_current_category], _category_icons[_current_category]);
	// Refresh all entries in the newly selected list is required because every object is available in two
	// categories, their standard type and the "All Wares" category.
	_list_displays[_current_category]->RefreshAllEntries();

	ShopObject* last_obj = _selected_object;
	_selected_object = _list_displays[_current_category]->GetSelectedObject();
	if (last_obj == _selected_object)
		return false;
	else
		return true;
}



bool BuyInterface::_ChangeSelection(bool up_or_down) {
	BuyListDisplay* selected_list = _list_displays[_current_category];

	if (up_or_down == false)
		selected_list->InputUp();
	else
		selected_list->InputDown();

	ShopObject* last_obj = _selected_object;
	_selected_object = _list_displays[_current_category]->GetSelectedObject();
	if (last_obj == _selected_object)
		return false;
	else
		return true;
}



void BuyInterface::_RefreshSelectedProperties() {
	if (_selected_object == NULL)
		return;

	// The only property that really needs to be refreshed is the buy quantity. Other properties will remain static.
	_selected_properties.SetOptionText(_selected_properties.GetNumberColumns() - 1,
		MakeUnicodeString("×" + NumberToString(_selected_object->GetBuyCount())));
}

// *****************************************************************************
// ***** BuyListDisplay class methods
// *****************************************************************************

void BuyListDisplay::ReconstructList() {
	_identify_list.ClearOptions();
	_property_list.ClearOptions();

	ShopObject* obj = NULL;
	for (uint32 i = 0; i < _objects.size(); i++) {
		obj = _objects[i];
		// Add an entry with the icon image of the object (scaled down by 4x to 30x30 pixels) followed by the object name
		_identify_list.AddOption(MakeUnicodeString("<" + obj->GetObject()->GetIconImage().GetFilename() + "><30>")
			+ obj->GetObject()->GetName());
		_identify_list.GetEmbeddedImage(i)->SetDimensions(30.0f, 30.0f);

		// Add an option for each object property in the order of: price, stock, number owned, and amount to buy
		_property_list.AddOption(MakeUnicodeString(NumberToString(obj->GetBuyPrice())));
		_property_list.AddOption(MakeUnicodeString("×" + NumberToString(obj->GetStockCount())));
		_property_list.AddOption(MakeUnicodeString("×" + NumberToString(obj->GetOwnCount())));
		_property_list.AddOption(MakeUnicodeString("×" + NumberToString(obj->GetBuyCount())));
	}

	if (_objects.empty() == false) {
		_identify_list.SetSelection(0);
		_property_list.SetSelection(0);
	}
}



void BuyListDisplay::RefreshEntry(uint32 index) {
	if (_objects.empty() == true) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "no object data is available" << endl;
		return;
	}
	if (index >= _objects.size()) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "index argument was out of range: " << index << endl;
		return;
	}

	_property_list.SetOptionText((index * _property_list.GetNumberColumns()) + (_property_list.GetNumberColumns() - 1),
		MakeUnicodeString("×" + NumberToString(_objects[index]->GetBuyCount())));
}



bool BuyListDisplay::ChangeBuyQuantity(bool less_or_more, uint32 amount) {
	ShopObject* obj = GetSelectedObject();
	if (obj == NULL) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function could not perform operation because list was empty" << endl;
		return false;
	}

	// Holds the amount that the quantity will actually increase or decrease by. May be less than the
	// amount requested if there is an limitation such as shop stock or available funds
	uint32 change_amount = amount;

	if (less_or_more == false) {
		// Ensure that at least one count of this object is already marked for purchase
		if (obj->GetBuyCount() == 0) {
			return false;
		}

		// Determine if there's a sufficient count selected to decrement by the desire amount. If not, return as many as possible
		if (amount > obj->GetBuyCount()) {
			change_amount = obj->GetBuyCount();
		}

		obj->DecrementBuyCount(change_amount);
		ShopMode::CurrentInstance()->UpdateFinances(-(obj->GetBuyPrice() * change_amount), 0);
		RefreshEntry(GetCurrentSelection());
		return true;
	}
	else {
		// Make sure that there is at least one more object in stock and the player has enough funds to purchase it
		if ((obj->GetBuyCount() >= obj->GetStockCount()) ||
			(obj->GetBuyPrice() > ShopMode::CurrentInstance()->GetTotalRemaining()))
		{
			return false;
		}

		// Determine if there's enough of the object in stock to buy. If not, buy as many left as possible
		if ((obj->GetStockCount() - obj->GetBuyCount()) < change_amount) {
			change_amount = obj->GetStockCount() - obj->GetBuyCount();
		}

		// Determine how many of the possible amount to buy the player can actually afford
		int32 total_cost = change_amount * obj->GetBuyPrice();
		int32 total_remaining = static_cast<int32>(ShopMode::CurrentInstance()->GetTotalRemaining());
		while (total_cost > total_remaining) {
			change_amount--;
			total_cost -= obj->GetBuyPrice();
		}

		obj->IncrementBuyCount(change_amount);
		ShopMode::CurrentInstance()->UpdateFinances((obj->GetBuyPrice() * change_amount), 0);
		RefreshEntry(GetCurrentSelection());
		return true;
	}
} // bool BuyListDisplay::ChangeBuyQuantity(bool less_or_more, uint32 amount)

} // namespace private_shop

} // namespace hoa_shop
