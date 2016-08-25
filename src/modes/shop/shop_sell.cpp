///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_sell.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for sell interface of shop mode
***
*** \note The contents of this file are near identical to the contents of
*** shop_sell.cpp. When making any changes to this file, please look to shop_sell.cpp
*** to see if it should have similar changes made.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "shop_sell.h"

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
// ***** SellInterface class methods
// *****************************************************************************

SellInterface::SellInterface() :
    _view_mode(SHOP_VIEW_MODE_INVALID),
    _selected_object(nullptr),
    _sell_deal_types(0),
    _number_categories(0),
    _current_category(0)
{
    _category_header.SetStyle(TextStyle("title24"));
    _category_header.SetText(UTranslate("Category"));

    _name_header.SetStyle(TextStyle("title24"));
    _name_header.SetText(UTranslate("Name"));

    _properties_header.SetDimensions(300.0f, 30.0f, 2, 1, 2, 1);
    _properties_header.SetOptionAlignment(VIDEO_X_RIGHT, VIDEO_Y_CENTER);
    _properties_header.SetTextStyle(TextStyle("title24"));
    _properties_header.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _properties_header.AddOption(UTranslate("Price"));
    _properties_header.AddOption(UTranslate("Own"));

    _selected_name.SetStyle(TextStyle("text22"));

    _selected_properties.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());
    _selected_properties.SetPosition(480.0f, 70.0f);
    _selected_properties.SetDimensions(300.0f, 30.0f, 2, 1, 2, 1);
    _selected_properties.SetOptionAlignment(VIDEO_X_RIGHT, VIDEO_Y_CENTER);
    _selected_properties.SetTextStyle(TextStyle("text22"));
    _selected_properties.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _selected_properties.AddOption(ustring());
    _selected_properties.AddOption(ustring());
}



SellInterface::~SellInterface()
{
    for(uint32_t i = 0; i < _list_displays.size(); i++) {
        delete _list_displays[i];
    }
}


void SellInterface::_UpdateAvailableSellDealTypes()
{
    _sell_deal_types = 0;

    // Determine what types of objects the shop deals in based on the managed object list
    std::map<uint32_t, ShopObject *>* shop_objects = ShopMode::CurrentInstance()->GetAvailableSell();
    for(auto it = shop_objects->begin(); it != shop_objects->end(); ++it) {
        // Key items can't be sold.
        if (it->second->GetObject()->IsKeyItem())
            continue;

        vt_global::GLOBAL_OBJECT object_type = it->second->GetObject()->GetObjectType();
        switch(object_type) {
        case GLOBAL_OBJECT_ITEM:
            _sell_deal_types |= DEALS_ITEMS;
            break;
        case GLOBAL_OBJECT_WEAPON:
            _sell_deal_types |= DEALS_WEAPONS;
            break;
        case GLOBAL_OBJECT_HEAD_ARMOR:
            _sell_deal_types |= DEALS_HEAD_ARMOR;
            break;
        case GLOBAL_OBJECT_TORSO_ARMOR:
            _sell_deal_types |= DEALS_TORSO_ARMOR;
            break;
        case GLOBAL_OBJECT_ARM_ARMOR:
            _sell_deal_types |= DEALS_ARM_ARMOR;
            break;
        case GLOBAL_OBJECT_LEG_ARMOR:
            _sell_deal_types |= DEALS_LEG_ARMOR;
            break;
        case GLOBAL_OBJECT_SPIRIT:
            _sell_deal_types |= DEALS_SPIRIT;
            break;
        default:
            IF_PRINT_WARNING(SHOP_DEBUG) << "unknown object type sold in shop: " << object_type << std::endl;
            break;
        }
    }
}


void SellInterface::_RefreshItemCategories()
{
    // Clear the data
    _category_icons.clear();
    _category_names.clear();
    ShopMedia *shop_media = ShopMode::CurrentInstance()->Media();
    std::vector<ustring>* all_category_names = shop_media->GetAllCategoryNames();
    std::vector<StillImage>* all_category_icons = GlobalManager->Media().GetAllItemCategoryIcons();

    // Determine which categories are used in this shop and populate the true containers with that data
    _UpdateAvailableSellDealTypes();

    uint8_t bit_x = 0x01; // Used to do a bit-by-bit analysis of the obj_types variable
    for(uint8_t i = 0; i < GLOBAL_OBJECT_TOTAL; i++, bit_x <<= 1) {
        // Check whether the type is available by doing a bit-wise comparison
        if(_sell_deal_types & bit_x) {
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


void SellInterface::_PopulateLists()
{
    // ---------- (1): Prepare object data containers and determine category index mappings
    // Containers of object data used to populate the display lists
    std::vector<std::vector<ShopObject *> > object_data;

    for(uint32_t i = 0; i < _number_categories; i++) {
        object_data.push_back(std::vector<ShopObject *>());
    }

    // Holds the index to the object_data vector where the container for a specific object type is located
    std::vector<uint32_t> type_index(GLOBAL_OBJECT_TOTAL, 0);
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
        if(_sell_deal_types & bit_x) {
            type_index[i] = next_index++;
        }
    }

    // Populate the object_data containers

    // Pointer to the container of all objects that are bought/sold/traded in the ship
    std::map<uint32_t, ShopObject *>* shop_objects = ShopMode::CurrentInstance()->GetAvailableSell();

    for(auto it = shop_objects->begin(); it != shop_objects->end(); ++it) {
        ShopObject* obj = it->second;

        // Key items are not permitted to be sold
        if(!obj || obj->GetObject()->IsKeyItem())
            continue;

        if(obj->GetOwnCount() > 0) {
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
                IF_PRINT_WARNING(SHOP_DEBUG) << "added object of unknown type: "
                                             << obj->GetObject()->GetObjectType()
                                             << std::endl;
                break;
            }

            // If there is an "All Wares" category, make sure the object gets added there as well
            if(_number_categories > 1) {
                object_data.back().push_back(obj);
            }
        }
    }

    // ---------- (3): Create the sell displays using the object data that is now ready
    for(uint32_t i = 0; i < object_data.size(); i++) {
        _list_displays[i]->PopulateList(object_data[i]);
    }
} // void SellInterface::_PopulateLists()


void SellInterface::Reinitialize()
{
    _RefreshItemCategories();

    // (Re)create the sell displays and populate them with the object data
    for(uint32_t i = 0; i < _list_displays.size(); ++i)
        delete _list_displays[i];
    _list_displays.clear();

    for(uint32_t i = 0; i < _number_categories; ++i)
        _list_displays.push_back(new SellListDisplay());

    _PopulateLists();

    // Initialize other class members and states appropriately
    // Set the initial category to the last category that was added (this is usually "All Wares")
    _current_category = _number_categories > 0 ? _number_categories - 1 : 0;

    // Initialize the category display with the initial category
    if(_number_categories > 0) {
        _category_display.ChangeCategory(_category_names[_current_category], _category_icons[_current_category]);
        _selected_object = _list_displays[_current_category]->GetSelectedObject();
    }
    ChangeViewMode(SHOP_VIEW_MODE_LIST);
}

void SellInterface::MakeActive()
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

void SellInterface::TransactionNotification()
{
    Reinitialize();
}

void SellInterface::Update()
{
    ShopMode* shop = ShopMode::CurrentInstance();

    if(_view_mode == SHOP_VIEW_MODE_LIST && shop->IsInputEnabled()) {
        if((InputManager->ConfirmPress()) && (_selected_object != nullptr)) {
            GlobalManager->Media().PlaySound("confirm");
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
        else if(InputManager->UpPress() && (_selected_object != nullptr)) {
            if(_ChangeSelection(false)) {
                shop->ObjectViewer()->SetSelectedObject(_selected_object);
                GlobalManager->Media().PlaySound("bump");
            }
        } else if(InputManager->DownPress() && (_selected_object != nullptr)) {
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
            shop->ChangeState(SHOP_STATE_SELL);
        } else if(InputManager->CancelPress()) {
            ChangeViewMode(SHOP_VIEW_MODE_LIST);

            while(_list_displays[_current_category]->ChangeSellQuantity(false))
            {}

            GlobalManager->Media().PlaySound("cancel");
            shop->ClearOrder();
        }

        // Left/right change the quantity of the object to sell
        else if(InputManager->LeftPress()) {
            if(_list_displays[_current_category]->ChangeSellQuantity(false)) {
                shop->ObjectViewer()->UpdateCountText();
                GlobalManager->Media().PlaySound("confirm");
            } else
                GlobalManager->Media().PlaySound("bump");
        } else if(InputManager->RightPress()) {
            if(_list_displays[_current_category]->ChangeSellQuantity(true)) {
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

void SellInterface::Draw()
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

void SellInterface::ChangeViewMode(SHOP_VIEW_MODE new_mode)
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
        _selected_properties.SetOptionText(0, MakeUnicodeString(NumberToString(_selected_object->GetSellPrice())));
        _selected_properties.SetOptionText(1, MakeUnicodeString("×" + NumberToString(_selected_object->GetOwnCount())));
    } else {
        IF_PRINT_WARNING(SHOP_DEBUG) << "tried to change to an invalid/unsupported view mode: "
                                     << new_mode << std::endl;
    }
}


bool SellInterface::_ChangeCategory(bool left_or_right)
{
    if(_number_categories <= 1)
        return false;

    if(left_or_right == false) {
        _current_category = (_current_category == 0) ?
            (_number_categories - 1) :
            (_current_category - 1);
    } else {
        _current_category = (_current_category >= (_number_categories - 1)) ?
            0 :
            (_current_category + 1);
    }

    _category_display.ChangeCategory(_category_names[_current_category], _category_icons[_current_category]);

    ShopObject *last_obj = _selected_object;
    _selected_object = _list_displays[_current_category]->GetSelectedObject();
    return last_obj != _selected_object;
}



bool SellInterface::_ChangeSelection(bool down)
{
    if(_current_category >= _list_displays.size())
        return false;

    SellListDisplay *selected_list = _list_displays[_current_category];

    if(!selected_list)
        return false;

    if(!down)
        selected_list->InputUp();
    else
        selected_list->InputDown();

    ShopObject *last_obj = _selected_object;
    _selected_object = _list_displays[_current_category]->GetSelectedObject();
    return last_obj != _selected_object;
}

// *****************************************************************************
// ***** SellListDisplay class methods
// *****************************************************************************

void SellListDisplay::ReconstructList()
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

        // Add an option for each object property in the order of: price, and number owned.
        _property_list.AddOption(MakeUnicodeString(NumberToString(obj->GetSellPrice())));
        _property_list.AddOption(MakeUnicodeString("×" + NumberToString(obj->GetOwnCount())));
    }

    if(_objects.empty() == false) {
        _identify_list.SetSelection(0);
        _property_list.SetSelection(0);
    }
}


bool SellListDisplay::ChangeSellQuantity(bool more, uint32_t amount)
{
    ShopObject *obj = GetSelectedObject();
    if(!obj) {
        PRINT_WARNING << "function could not perform operation because list was empty"
                      << std::endl;
        return false;
    }

    // Holds the amount that the quantity will actually increase or decrease by. May be less than the
    // amount requested if there is an limitation such as shop stock or available funds
    uint32_t change_amount = amount;

    if(!more) {
        // Make sure that there is at least one more count to sell and that the player has enough funds to return it
        if(obj->GetSellCount() == 0 ||
                obj->GetSellPrice() > ShopMode::CurrentInstance()->GetTotalRemaining()) {
            return false;
        }

        // Determine if there's a sufficient count selected to decrement by the desire amount. If not, return as many as possible
        if(amount > obj->GetSellCount()) {
            change_amount = obj->GetSellCount();
        }

        // Determine how many of the possible amount to sell the player can actually perform. This is necessary to check
        // because by reducing sales revenue, its possible that marked costs exceed what the player can afford with this
        // lost revenue.
        int32_t total_lost_revenue = change_amount * obj->GetSellPrice();
        int32_t total_remaining = static_cast<int32_t>(ShopMode::CurrentInstance()->GetTotalRemaining());
        while(total_lost_revenue > total_remaining) {
            change_amount--;
            total_lost_revenue -= obj->GetSellPrice();
        }

        obj->DecrementSellCount(change_amount);
        ShopMode::CurrentInstance()->UpdateFinances(-obj->GetSellPrice() *
                                                    change_amount);
        return true;
    }

    // more
    // Make sure that there is at least one more object available to sell in the player's inventory
    if(obj->GetSellCount() >= obj->GetOwnCount()) {
        return false;
    }

    // Determine if there's enough of the object in stock to sell.
    // If not, sell as many left as possible
    if((obj->GetOwnCount() - obj->GetSellCount()) < change_amount) {
        change_amount = obj->GetOwnCount() - obj->GetSellCount();
    }

    obj->IncrementSellCount(change_amount);

    ShopMode::CurrentInstance()->UpdateFinances(obj->GetSellPrice() *
                                                change_amount);
    return true;
} // bool SellListDisplay::ChangeSellQuantity(bool more, uint32_t amount)

} // namespace private_shop

} // namespace vt_shop
