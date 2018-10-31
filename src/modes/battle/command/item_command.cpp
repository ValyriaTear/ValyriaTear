////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "modes/battle/command/item_command.h"

#include "common/global/global.h"

#include "engine/video/video.h"
#include "engine/system.h"
#include "engine/input.h"

using namespace vt_gui;
using namespace vt_global;
using namespace vt_input;
using namespace vt_system;
using namespace vt_utils;
using namespace vt_video;

namespace vt_battle
{

namespace private_battle
{

const float HEADER_POSITION_X = 140.0f;
const float HEADER_POSITION_Y = -12.0f;
const float HEADER_SIZE_X = 350.0f;
const float HEADER_SIZE_Y = 30.0f;

const float LIST_POSITION_X = 140.0f;
const float LIST_POSITION_Y = 15.0f;
const float LIST_SIZE_X = 350.0f;
const float LIST_SIZE_Y = 100.0f;

//! \brief Offset used to properly align the target icons in the skill and item selection lists
const uint32_t TARGET_ICON_OFFSET = 288;

ItemCommand::ItemCommand(MenuWindow& window)
{
    _item_header.SetOwner(&window);
    _item_header.SetPosition(HEADER_POSITION_X, HEADER_POSITION_Y);
    _item_header.SetDimensions(HEADER_SIZE_X, HEADER_SIZE_Y, 1, 1, 1, 1);
    _item_header.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _item_header.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _item_header.SetTextStyle(TextStyle("title22"));
    _item_header.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _item_header.AddOption(UTranslate("Item<R>Type   ×"));

    _item_list.SetOwner(&window);
    _item_list.SetPosition(LIST_POSITION_X, LIST_POSITION_Y);
    _item_list.SetDimensions(LIST_SIZE_X, LIST_SIZE_Y, 1, 255, 1, 4);
    _item_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _item_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _item_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _item_list.SetTextStyle(TextStyle("text20"));
    _item_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _item_list.SetCursorOffset(-50.0f, -25.0f);
    _item_list.AnimateScrolling(false);

    _item_target_list.SetOwner(&window);
    _item_target_list.SetPosition(LIST_POSITION_X + TARGET_ICON_OFFSET, LIST_POSITION_Y);
    _item_target_list.SetDimensions(LIST_SIZE_X - TARGET_ICON_OFFSET, LIST_SIZE_Y, 1, 255, 1, 4);
    _item_target_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _item_target_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _item_target_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _item_target_list.SetTextStyle(TextStyle("text20"));
    _item_target_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _item_target_list.AnimateScrolling(false);

    ResetItemList();
}

void ItemCommand::ResetItemList()
{
    _battle_items.clear();

    auto inv_items = GlobalManager->GetInventoryHandler().GetInventoryItems();
    for (uint32_t i = 0; i < inv_items.size(); ++i) {
        std::shared_ptr<GlobalItem> global_item = inv_items.at(i);

        // Only add non key and valid items as items available at battle start.
        if (global_item->GetCount() == 0)
            continue;
        if (!global_item->IsValid())
            continue;
        if (global_item->IsKeyItem())
            continue;

        if (global_item->IsUsableInBattle()) {
            std::shared_ptr<BattleItem> battle_item = std::make_shared<BattleItem>(*global_item);
            _battle_items.push_back(battle_item);
        }
    }

    // We clear the menu items list as its pointers are now invalid.
    _menu_items.clear();
}

void ItemCommand::ConstructList()
{
    _item_list.ClearOptions();
    _item_target_list.ClearOptions();

    _menu_items.clear();

    uint32_t option_index = 0;
    for(uint32_t i = 0; i < _battle_items.size(); ++i) {
        std::shared_ptr<BattleItem> item = _battle_items[i];
        // Don't add any items with a zero count
        if(item->GetBattleCount() == 0)
            continue;

        // Adds the BattleItem pointer to the main item list
        _menu_items.push_back(item);

        // Adds the item menu option
        _item_list.AddOption();
        const GlobalItem& global_item = item->GetGlobalItem();
        if (!global_item.GetIconImage().GetFilename().empty()) {
            _item_list.AddOptionElementImage(option_index, global_item.GetIconImage().GetFilename());
            _item_list.GetEmbeddedImage(option_index)->SetHeightKeepRatio(25);
            _item_list.AddOptionElementPosition(option_index, 30);
        }
        _item_list.AddOptionElementText(option_index, global_item.GetName());

        _item_target_list.AddOption(ustring());
        _item_target_list.AddOptionElementImage(option_index,
                                                GlobalManager->GetBattleMedia().GetTargetTypeIcon(item->GetTargetType()));
        _item_target_list.AddOptionElementPosition(option_index, 45);
        _item_target_list.AddOptionElementText(option_index, MakeUnicodeString(NumberToString(item->GetBattleCount())));
        ++option_index;
    }

    if(_item_list.GetNumberOptions() == 0) {
        _item_list.SetSelection(-1);
        _item_target_list.SetSelection(-1);
    }
    else {
        _item_list.SetSelection(0);
        _item_target_list.SetSelection(0);
        // Reset scrolling
        _item_list.ResetViewableOption();
        _item_target_list.ResetViewableOption();
    }
}

void ItemCommand::Initialize(uint32_t battle_item_index)
{
    // If there is no more usable items in the inventory,
    // we can set the selection as invalid.
    if(_item_list.GetNumberOptions() == 0 || _menu_items.empty()) {
        _item_list.SetSelection(-1);
        _item_target_list.SetSelection(-1);
        return;
    }

    // In case the selection of the previous item leads to something
    // not relevant anymore, we reset the selection.
    if(battle_item_index >= _battle_items.size() ||
            _battle_items[battle_item_index]->GetBattleCount() == 0) {
        _item_list.SetSelection(0);
        _item_target_list.SetSelection(0);
        return;
    }

    // Find the corresponding item in the menu list and select it.
    uint32_t selection = 0; // First item in the list per default
    std::shared_ptr<BattleItem> selected_item = _battle_items[battle_item_index];
    for (uint32_t i = 0; i < _menu_items.size(); ++i) {
        if (_menu_items[i] == selected_item) {
            selection = i;
            break;
        }
    }

    // If the item is in the list, set the list selection to that item
    _item_list.SetSelection(selection);
    _item_target_list.SetSelection(selection);
}

std::shared_ptr<BattleItem> ItemCommand::GetSelectedItem()
{
    int32_t selection = _item_list.GetSelection();
    if (selection == -1 || selection >= (int32_t)_menu_items.size())
        return nullptr;

    return _menu_items[selection];
}

bool ItemCommand::IsSelectedItemAvailable() const
{
    int32_t selection = _item_list.GetSelection();
    if (selection == -1 || selection >= (int32_t)_menu_items.size())
        return false;

    return (_menu_items[selection]->GetBattleCount() > 0);
}

void ItemCommand::UpdateList()
{
    _item_list.Update();
    _item_target_list.Update();

    if(InputManager->UpPress()) {
        _item_list.InputUp();
        _item_target_list.InputUp();
        GlobalManager->Media().PlaySound("bump");
    } else if(InputManager->DownPress()) {
        _item_list.InputDown();
        _item_target_list.InputDown();
        GlobalManager->Media().PlaySound("bump");
    }
}

void ItemCommand::DrawList()
{
    _item_header.Draw();
    _item_list.Draw();
    _item_target_list.Draw();
}

void ItemCommand::CommitChangesToInventory()
{
    InventoryHandler& inventory_handler = GlobalManager->GetInventoryHandler();

    for(uint32_t i = 0; i < _battle_items.size(); ++i) {
        // Get the global item id
        uint32_t id = _battle_items[i]->GetGlobalItem().GetID();

        // Remove slots totally used
        if (_battle_items[i]->GetBattleCount() == 0) {
            inventory_handler.RemoveFromInventory(id);
        } else {
            int32_t diff = _battle_items[i]->GetBattleCount() - _battle_items[i]->GetInventoryCount();
            if (diff > 0) {
                // Somehow the character have more than before the battle.
                inventory_handler.IncrementItemCount(id, diff);
            } else if (diff < 0) {
                // Remove the used items.
                inventory_handler.DecrementItemCount(id, -diff);
            }
        }
    }
}

} // namespace private_battle

} // namespace vt_battle
