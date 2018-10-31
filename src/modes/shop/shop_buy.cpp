///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_buy.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for buy interface of shop mode
***
*** \note The contents of this file are near identical to the contents of
*** shop_sell.cpp. When making any changes to this file, please look to shop_sell.cpp
*** to see if it should have similar changes made.
*** ***************************************************************************/

#include "shop_buy.h"

#include "shop.h"

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/system.h"
#include "engine/video/video.h"

#include "common/global/global.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_input;
using namespace vt_system;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_global;

namespace vt_shop
{

namespace private_shop
{

// *****************************************************************************
// ***** BuyInterface class methods
// *****************************************************************************

BuyInterface::BuyInterface() :
    _view_mode(SHOP_VIEW_MODE_INVALID),
    _selected_object(nullptr),
    _buy_deal_types(0),
    _number_categories(0),
    _current_category(0)
{
    _category_header.SetStyle(TextStyle("title24"));
    _category_header.SetText(UTranslate("Category"));

    _name_header.SetStyle(TextStyle("title24"));
    _name_header.SetText(UTranslate("Name"));

    _properties_header.SetDimensions(300.0f, 30.0f, 3, 1, 3, 1);
    _properties_header.SetOptionAlignment(VIDEO_X_RIGHT, VIDEO_Y_CENTER);
    _properties_header.SetTextStyle(TextStyle("title24"));
    _properties_header.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _properties_header.AddOption(UTranslate("Price"));
    _properties_header.AddOption(UTranslate("Stock"));
    _properties_header.AddOption(UTranslate("Own"));

    _selected_name.SetStyle(TextStyle("text22"));

    _selected_properties.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());
    _selected_properties.SetPosition(480.0f, 70.0f);
    _selected_properties.SetDimensions(300.0f, 30.0f, 3, 1, 3, 1);
    _selected_properties.SetOptionAlignment(VIDEO_X_RIGHT, VIDEO_Y_CENTER);
    _selected_properties.SetTextStyle(TextStyle("text22"));
    _selected_properties.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _selected_properties.AddOption(ustring());
    _selected_properties.AddOption(ustring());
    _selected_properties.AddOption(ustring());
}


BuyInterface::~BuyInterface()
{
    for(uint32_t i = 0; i < _list_displays.size(); i++) {
        delete _list_displays[i];
    }
}


void BuyInterface::_UpdateAvailableBuyDealTypes()
{
    _buy_deal_types = 0;

    // Determine what types of objects the shop deals in based on the managed object list
    std::map<uint32_t, ShopObject *>* buy_objects = ShopMode::CurrentInstance()->GetAvailableBuy();
    for(std::map<uint32_t, ShopObject *>::iterator it = buy_objects->begin(); it != buy_objects->end(); ++it) {
        vt_global::GLOBAL_OBJECT object_type = it->second->GetObject()->GetObjectType();
        switch(object_type) {
        case GLOBAL_OBJECT_ITEM:
            _buy_deal_types |= DEALS_ITEMS;
            break;
        case GLOBAL_OBJECT_WEAPON:
            _buy_deal_types |= DEALS_WEAPONS;
            break;
        case GLOBAL_OBJECT_HEAD_ARMOR:
            _buy_deal_types |= DEALS_HEAD_ARMOR;
            break;
        case GLOBAL_OBJECT_TORSO_ARMOR:
            _buy_deal_types |= DEALS_TORSO_ARMOR;
            break;
        case GLOBAL_OBJECT_ARM_ARMOR:
            _buy_deal_types |= DEALS_ARM_ARMOR;
            break;
        case GLOBAL_OBJECT_LEG_ARMOR:
            _buy_deal_types |= DEALS_LEG_ARMOR;
            break;
        case GLOBAL_OBJECT_SPIRIT:
            _buy_deal_types |= DEALS_SPIRIT;
            break;
        default:
            IF_PRINT_WARNING(SHOP_DEBUG) << "unknown object type sold in shop: " << object_type << std::endl;
            break;
        }
        // Test whether this is a key item
        if (it->second->GetObject()->IsKeyItem())
            _buy_deal_types |= DEALS_KEY_ITEMS;
    }
}


void BuyInterface::_RefreshItemCategories()
{
    // Clear the data
    _category_icons.clear();
    _category_names.clear();
    ShopMedia *shop_media = ShopMode::CurrentInstance()->Media();
    std::vector<ustring>* all_category_names = shop_media->GetAllCategoryNames();
    std::vector<StillImage>* all_category_icons = GlobalManager->Media().GetAllItemCategoryIcons();

    // Determine which categories are used in this shop and populate the true containers with that data
    _UpdateAvailableBuyDealTypes();

    uint8_t bit_x = 0x01; // Used to do a bit-by-bit analysis of the obj_types variable
    for(uint8_t i = 0; i < GLOBAL_OBJECT_TOTAL; i++, bit_x <<= 1) {
        // Check whether the type is available by doing a bit-wise comparison
        if(_buy_deal_types & bit_x) {
            _category_names.push_back(all_category_names->at(i));
            _category_icons.push_back(&all_category_icons->at(i));
        }
    }

    // If here is more than one category, add the text/icon for all wares
    if(_category_names.size() > 1) {
        _category_names.push_back(all_category_names->at(8));
        _category_icons.push_back(&all_category_icons->at(8));
    }

    _number_categories = _category_names.size();
}


void BuyInterface::Reinitialize()
{
    _RefreshItemCategories();

    // Set the initial category to the last category that was added (this is usually "All Wares")
    _current_category = _number_categories > 0 ? _number_categories - 1 : 0;
    // Initialize the category display with the initial category
    if(_number_categories > 0)
        _category_display.ChangeCategory(_category_names[_current_category], _category_icons[_current_category]);

    // Prepare object data containers and determine category index mappings
    // Containers of object data used to populate the display lists
    std::vector<std::vector<ShopObject *> > object_data;

    for(uint32_t i = 0; i < _number_categories; ++i) {
        object_data.push_back(std::vector<ShopObject *>());
    }

    // Holds the index to the _object_data vector where the container for a specific object type is located
    std::vector<uint32_t> type_index(GLOBAL_OBJECT_TOTAL + 1, 0); // + key items
    // Used to set the appropriate data in the type_index vector
    uint32_t next_index = 0;
    // Used to do a bit-by-bit analysis of the deal_types variable
    uint8_t bit_x = 0x01;

    // This loop determines where each type of object should be placed in the object_data container. For example,
    // if the available categories in the shop are items, weapons, spirits, and all wares, the size of object_data
    // will be four. When we go to add an object of one of these types into the object_data container, we need
    // to know the correct index for each type of object. These indeces are stored in the type_index vector. The
    // size of this vector is the number of object types, so it becomes simple to map each object type to its correct
    // location in object_data.
    for(uint8_t i = 0; i < GLOBAL_OBJECT_TOTAL; i++, bit_x <<= 1) {
        // Check if the type is available by doing a bit-wise comparison
        if(_buy_deal_types & bit_x) {
            type_index[i] = next_index++;
        }
    }

    // Populate the object_data containers

    // Pointer to the container of all objects that are bought/sold/traded in the shop
    std::map<uint32_t, ShopObject *>* buy_objects = ShopMode::CurrentInstance()->GetAvailableBuy();

    for(std::map<uint32_t, ShopObject *>::iterator it = buy_objects->begin(); it != buy_objects->end(); ++it) {
        ShopObject* obj = it->second;
        switch(obj->GetObject()->GetObjectType()) {
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
        case GLOBAL_OBJECT_SPIRIT:
            object_data[type_index[6]].push_back(obj);
            break;
        default:
            IF_PRINT_WARNING(SHOP_DEBUG) << "added object of unknown type: " << obj->GetObject()->GetObjectType() << std::endl;
            break;
        }

        // Test whether this is a key item
        if (it->second->GetObject()->IsKeyItem())
            object_data[type_index[7]].push_back(obj);

        // If there is an "All Wares" category, make sure the object gets added there as well
        if(_number_categories > 1) {
            object_data.back().push_back(obj);
        }
    }

    // Create the buy displays using the object data that is now ready
    for(uint32_t i = 0; i < _list_displays.size(); ++i) {
        delete _list_displays[i];
    }
    _list_displays.clear();

    for(uint32_t i = 0; i < object_data.size(); ++i) {
        BuyListDisplay *new_list = new BuyListDisplay();
        new_list->PopulateList(object_data[i]);
        _list_displays.push_back(new_list);
    }

    if(_number_categories > 0)
        _selected_object = _list_displays[_current_category]->GetSelectedObject();
    ChangeViewMode(SHOP_VIEW_MODE_LIST);
}

void BuyInterface::MakeActive()
{
    Reinitialize();

    if(_list_displays.empty()) {
        ShopMode::CurrentInstance()->ChangeState(SHOP_STATE_ROOT);
        return;
    }

    _selected_object = _list_displays[_current_category]->GetSelectedObject();
    ShopMode::CurrentInstance()->ObjectViewer()->ChangeViewMode(_view_mode);
    ShopMode::CurrentInstance()->ObjectViewer()->SetSelectedObject(_selected_object);
    _category_display.ChangeViewMode(_view_mode);
    _category_display.SetSelectedObject(_selected_object);
}

void BuyInterface::TransactionNotification()
{
    Reinitialize();

    for(uint32_t i = 0; i < _list_displays.size(); ++i) {
        _list_displays[i]->ReconstructList();
        _list_displays[i]->ResetSelection();
    }

    _current_category = _number_categories > 0 ? _number_categories - 1 : 0;
    _view_mode = SHOP_VIEW_MODE_LIST;
}

void BuyInterface::Update()
{
    ShopMode* shop = ShopMode::CurrentInstance();

    if(_view_mode == SHOP_VIEW_MODE_LIST && shop->IsInputEnabled()) {
        if(InputManager->ConfirmPress()) {
            ChangeViewMode(SHOP_VIEW_MODE_INFO);
        } else if(InputManager->CancelPress()) {
            shop->ChangeState(SHOP_STATE_ROOT);
            GlobalManager->Media().PlaySound("cancel");
        }

        // Swap cycles through the object categories
        else if(InputManager->MenuPress() && (_number_categories > 1)) {
            if(_ChangeCategory(true))
                shop->ObjectViewer()->SetSelectedObject(_selected_object);
            GlobalManager->Media().PlaySound("confirm");
        }

        // Up/down changes the selected object in the current list
        else if(InputManager->UpPress()) {
            if(_ChangeSelection(false)) {
                shop->ObjectViewer()->SetSelectedObject(_selected_object);
                GlobalManager->Media().PlaySound("bump");
            }
        } else if(InputManager->DownPress()) {
            if(_ChangeSelection(true)) {
                shop->ObjectViewer()->SetSelectedObject(_selected_object);
                GlobalManager->Media().PlaySound("bump");
            }
        }
    } // if (_view_mode == SHOP_VIEW_MODE_LIST)

    else if(_view_mode == SHOP_VIEW_MODE_INFO && shop->IsInputEnabled()) {
        if(InputManager->ConfirmPress()) {
            ChangeViewMode(SHOP_VIEW_MODE_LIST);
            shop->ChangeState(SHOP_STATE_ROOT);
            shop->CompleteTransaction();
            GlobalManager->Media().PlaySound("confirm");
            shop->ClearOrder();
            shop->ChangeState(SHOP_STATE_SELL); //If the entire sell list is emptied, this somehow helps
            shop->ChangeState(SHOP_STATE_BUY);
        } else if(InputManager->CancelPress()) {
            ChangeViewMode(SHOP_VIEW_MODE_LIST);
            while(_list_displays[_current_category]->ChangeBuyQuantity(false)) {}
            GlobalManager->Media().PlaySound("cancel");
            shop->ClearOrder();
        }

        // Left/right change the quantity of the object to buy
        else if(InputManager->LeftPress()) {
            if(_list_displays[_current_category]->ChangeBuyQuantity(false)) {
                shop->ObjectViewer()->UpdateCountText();
                GlobalManager->Media().PlaySound("confirm");
            } else
                GlobalManager->Media().PlaySound("bump");
        } else if(InputManager->RightPress()) {
            if(_list_displays[_current_category]->ChangeBuyQuantity(true)) {
                shop->ObjectViewer()->UpdateCountText();
                GlobalManager->Media().PlaySound("confirm");
            } else
                GlobalManager->Media().PlaySound("bump");
        }
    }

    _category_display.Update();
    _list_displays[_current_category]->Update();
    shop->ObjectViewer()->Update();
}

void BuyInterface::Draw()
{
    if(_view_mode == SHOP_VIEW_MODE_LIST) {
        VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
        VideoManager->Move(200.0f, 210.0f);
        _category_header.Draw();

        VideoManager->SetDrawFlags(VIDEO_X_LEFT, 0);
        VideoManager->MoveRelative(95.0f, 0.0f);
        _name_header.Draw();

        _properties_header.Draw();

        _category_display.Draw();
        _list_displays[_current_category]->Draw();
    } else if(_view_mode == SHOP_VIEW_MODE_INFO) {
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        VideoManager->Move(295.0f, 593.0f);
        _name_header.Draw();
        _properties_header.Draw();

        VideoManager->MoveRelative(0.0f, 50.0f);
        if (!_selected_icon.GetFilename().empty())
            _selected_icon.Draw();
        VideoManager->MoveRelative(30.0f, 0.0f);
        _selected_name.Draw();
        _selected_properties.Draw();

        _category_display.Draw();
    }

    ShopMode::CurrentInstance()->ObjectViewer()->Draw();
}

void BuyInterface::ChangeViewMode(SHOP_VIEW_MODE new_mode)
{
    if(_view_mode == new_mode)
        return;

    if(new_mode == SHOP_VIEW_MODE_LIST) {
        _view_mode = new_mode;
        ShopMode::CurrentInstance()->ObjectViewer()->ChangeViewMode(_view_mode);
        _category_display.ChangeViewMode(_view_mode);

        _properties_header.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());
        _properties_header.SetPosition(480.0f, 10.0f);
    } else if(new_mode == SHOP_VIEW_MODE_INFO) {
        _view_mode = new_mode;
        ShopMode::CurrentInstance()->ObjectViewer()->ChangeViewMode(_view_mode);
        _category_display.ChangeViewMode(_view_mode);
        _category_display.SetSelectedObject(_selected_object);

        _properties_header.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());
        _properties_header.SetPosition(480.0f, 15.0f);

        _selected_name.SetText(_selected_object->GetObject()->GetName());
        _selected_icon = _selected_object->GetObject()->GetIconImage();
        _selected_icon.SetDimensions(30.0f, 30.0f);
        _selected_properties.SetOptionText(0, MakeUnicodeString(NumberToString(_selected_object->GetBuyPrice())));
        if (_selected_object->IsInfiniteAmount())
            _selected_properties.SetOptionText(1, MakeUnicodeString("∞"));
        else
            _selected_properties.SetOptionText(1, MakeUnicodeString("×" + NumberToString(_selected_object->GetStockCount())));
        uint32_t own_count = GlobalManager->GetInventoryHandler().HowManyObjectsInInventory(_selected_object->GetObject()->GetID());
        _selected_properties.SetOptionText(2, MakeUnicodeString("×" + NumberToString(own_count)));

        // Set the buy count to one, permitting quick one-buy sequences when affordable.
        if (_list_displays[_current_category]->ChangeBuyQuantity(true, 1))
            ShopMode::CurrentInstance()->ObjectViewer()->UpdateCountText();
    } else {
        IF_PRINT_WARNING(SHOP_DEBUG) << "tried to change to an invalid/unsupported view mode: " << new_mode << std::endl;
    }
}



bool BuyInterface::_ChangeCategory(bool left_or_right)
{
    if(_number_categories <= 1)
        return false;

    if(left_or_right == false) {
        _current_category = (_current_category == 0) ? (_number_categories - 1) : (_current_category - 1);
    } else {
        _current_category = (_current_category >= (_number_categories - 1)) ? 0 : (_current_category + 1);
    }

    _category_display.ChangeCategory(_category_names[_current_category], _category_icons[_current_category]);

    ShopObject *last_obj = _selected_object;
    _selected_object = _list_displays[_current_category]->GetSelectedObject();
    if(last_obj == _selected_object)
        return false;
    else
        return true;
}



bool BuyInterface::_ChangeSelection(bool up_or_down)
{
    if(_current_category >= _list_displays.size())
        return false;

    BuyListDisplay *selected_list = _list_displays[_current_category];

    if(!selected_list)
        return false;

    if(up_or_down == false)
        selected_list->InputUp();
    else
        selected_list->InputDown();

    ShopObject *last_obj = _selected_object;
    _selected_object = _list_displays[_current_category]->GetSelectedObject();
    if(last_obj == _selected_object)
        return false;
    else
        return true;
}

// *****************************************************************************
// ***** BuyListDisplay class methods
// *****************************************************************************

void BuyListDisplay::ReconstructList()
{
    _identify_list.ClearOptions();
    _property_list.ClearOptions();

    for(uint32_t i = 0; i < _objects.size(); i++) {
        ShopObject* obj = _objects[i];
        // Add an entry with the icon image of the object (scaled down by 4x to 30x30 pixels) followed by the object name
        if (obj->GetObject()->GetIconImage().GetFilename().empty()) {
            _identify_list.AddOption(MakeUnicodeString("<30>") + obj->GetObject()->GetName());
        }
        else {
            _identify_list.AddOption(MakeUnicodeString("<" + obj->GetObject()->GetIconImage().GetFilename() + "><30>")
                                     + obj->GetObject()->GetName());
            _identify_list.GetEmbeddedImage(i)->SetDimensions(30.0f, 30.0f);
        }

        // Add an option for each object property in the order of: price, stock, and number owned.
        _property_list.AddOption(MakeUnicodeString(NumberToString(obj->GetBuyPrice())));
        if (obj->IsInfiniteAmount())
            _property_list.AddOption(MakeUnicodeString("∞"));
        else
            _property_list.AddOption(MakeUnicodeString("×" + NumberToString(obj->GetStockCount())));
        uint32_t own_count = GlobalManager->GetInventoryHandler().HowManyObjectsInInventory(obj->GetObject()->GetID());
        _property_list.AddOption(MakeUnicodeString("×" + NumberToString(own_count)));
    }

    if(_objects.empty() == false) {
        _identify_list.SetSelection(0);
        _property_list.SetSelection(0);
    }
}


bool BuyListDisplay::ChangeBuyQuantity(bool more, uint32_t amount)
{
    ShopObject* obj = GetSelectedObject();
    if(obj == nullptr) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "function could not perform operation because list was empty" << std::endl;
        return false;
    }

    // Holds the amount that the quantity will actually increase or decrease by. May be less than the
    // amount requested if there is an limitation such as shop stock or available funds
    uint32_t change_amount = amount;

    if(more == false) { // Decrement
        // Ensure that at least one count of this object is already marked for purchase
        if(obj->GetBuyCount() == 0) {
            return false;
        }

        // Determine if there's a sufficient count selected to decrement by the desire amount. If not, return as many as possible
        if(amount > obj->GetBuyCount()) {
            change_amount = obj->GetBuyCount();
        }

        obj->DecrementBuyCount(change_amount);
        ShopMode::CurrentInstance()->UpdateFinances(obj->GetBuyPrice() * change_amount);
        return true;
    }

    // increment

    // Make sure that there is at least one more object in stock and the player has enough funds to purchase it
    if((!obj->IsInfiniteAmount() && obj->GetBuyCount() >= obj->GetStockCount()) ||
            (obj->GetBuyPrice() > ShopMode::CurrentInstance()->GetTotalRemaining())) {
        return false;
    }

    // Determine if there's enough of the object in stock to buy. If not, buy as many left as possible
    if(!obj->IsInfiniteAmount() && (obj->GetStockCount() - obj->GetBuyCount()) < change_amount) {
        change_amount = obj->GetStockCount() - obj->GetBuyCount();
    }

    // Determine how many of the possible amount to buy the player can actually afford
    int32_t total_cost = change_amount * obj->GetBuyPrice();
    int32_t total_remaining = static_cast<int32_t>(ShopMode::CurrentInstance()->GetTotalRemaining());
    while(total_cost > total_remaining) {
        --change_amount;
        total_cost -= obj->GetBuyPrice();
    }

    obj->IncrementBuyCount(change_amount);
    ShopMode::CurrentInstance()->UpdateFinances(-obj->GetBuyPrice() * change_amount);
    return true;
} // bool BuyListDisplay::ChangeBuyQuantity(bool more, uint32_t amount)

} // namespace private_shop

} // namespace vt_shop
