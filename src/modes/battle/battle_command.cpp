////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_command.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for battle menu windows
*** ***************************************************************************/

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/system.h"
#include "engine/video/video.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actions.h"
#include "modes/battle/battle_actors.h"
#include "modes/battle/battle_command.h"
#include "modes/battle/battle_utils.h"

using namespace vt_utils;

using namespace vt_audio;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_input;
using namespace vt_system;
using namespace vt_global;

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

const float TARGET_POSITION_X = 40.0f;
const float TARGET_POSITION_Y = 15.0f;
const float TARGET_SIZE_X = 450.0f;
const float TARGET_SIZE_Y = 100.0f;

// Offset used to properly align the target icons in the skill and item selection lists
const uint32 TARGET_ICON_OFFSET = 288;

////////////////////////////////////////////////////////////////////////////////
// CharacterCommandSettings class
////////////////////////////////////////////////////////////////////////////////

CharacterCommandSettings::CharacterCommandSettings(BattleCharacter *character, MenuWindow &window) :
    _character(character),
    _last_category(CATEGORY_WEAPON),
    _last_item(0),
    _last_self_target(BattleTarget()),
    _last_character_target(BattleTarget()),
    _last_enemy_target(BattleTarget())
{
    _weapon_skill_list.SetOwner(&window);
    _weapon_skill_list.SetPosition(LIST_POSITION_X, LIST_POSITION_Y);
    _weapon_skill_list.SetDimensions(LIST_SIZE_X, LIST_SIZE_Y, 1, 255, 1, 4);
    _weapon_skill_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _weapon_skill_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _weapon_skill_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _weapon_skill_list.SetTextStyle(TextStyle("text20"));
    _weapon_skill_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _weapon_skill_list.SetCursorOffset(-50.0f, -25.0f);

    _weapon_target_list.SetOwner(&window);
    _weapon_target_list.SetPosition(LIST_POSITION_X + TARGET_ICON_OFFSET, LIST_POSITION_Y);
    _weapon_target_list.SetDimensions(LIST_SIZE_X - TARGET_ICON_OFFSET, LIST_SIZE_Y, 1, 255, 1, 4);
    _weapon_target_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _weapon_target_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _weapon_target_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _weapon_target_list.SetTextStyle(TextStyle("text20"));
    _weapon_target_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _magic_skill_list.SetOwner(&window);
    _magic_skill_list.SetPosition(LIST_POSITION_X, LIST_POSITION_Y);
    _magic_skill_list.SetDimensions(LIST_SIZE_X, LIST_SIZE_Y, 1, 255, 1, 4);
    _magic_skill_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _magic_skill_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _magic_skill_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _magic_skill_list.SetTextStyle(TextStyle("text20"));
    _magic_skill_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _magic_skill_list.SetCursorOffset(-50.0f, -25.0f);

    _magic_target_list.SetOwner(&window);
    _magic_target_list.SetPosition(LIST_POSITION_X + TARGET_ICON_OFFSET, LIST_POSITION_Y);
    _magic_target_list.SetDimensions(LIST_SIZE_X - TARGET_ICON_OFFSET, LIST_SIZE_Y, 1, 255, 1, 4);
    _magic_target_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _magic_target_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _magic_target_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _magic_target_list.SetTextStyle(TextStyle("text20"));
    _magic_target_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _special_skill_list.SetOwner(&window);
    _special_skill_list.SetPosition(LIST_POSITION_X, LIST_POSITION_Y);
    _special_skill_list.SetDimensions(LIST_SIZE_X, LIST_SIZE_Y, 1, 255, 1, 4);
    _special_skill_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _special_skill_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _special_skill_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _special_skill_list.SetTextStyle(TextStyle("text20"));
    _special_skill_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _special_skill_list.SetCursorOffset(-50.0f, -25.0f);

    _special_target_list.SetOwner(&window);
    _special_target_list.SetPosition(LIST_POSITION_X + TARGET_ICON_OFFSET, LIST_POSITION_Y);
    _special_target_list.SetDimensions(LIST_SIZE_X - TARGET_ICON_OFFSET, LIST_SIZE_Y, 1, 255, 1, 4);
    _special_target_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _special_target_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _special_target_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _special_target_list.SetTextStyle(TextStyle("text20"));
    _special_target_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    if(_character == NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL character pointer" << std::endl;
        return;
    }

    // Construct the weapon, magic, and special skill lists for the character
    std::vector<GlobalSkill *>* skill_list = NULL;
    GlobalWeapon* char_wpn = _character->GetGlobalCharacter()->GetWeaponEquipped();

    if (char_wpn)
        skill_list = _character->GetGlobalCharacter()->GetWeaponSkills();
    else
        skill_list = _character->GetGlobalCharacter()->GetBareHandsSkills();
    for(uint32 i = 0; i < skill_list->size(); i++) {
        _weapon_skill_list.AddOption(ustring());
        if (!skill_list->at(i)->GetIconFilename().empty()) {
            _weapon_skill_list.AddOptionElementImage(i, skill_list->at(i)->GetIconFilename());
            _weapon_skill_list.GetEmbeddedImage(i)->SetHeightKeepRatio(25);
            _weapon_skill_list.AddOptionElementPosition(i, 30);
        }
        else {
            // Check for the weapon icon
            std::string wpn_icon_filename;
            if (char_wpn)
                wpn_icon_filename = char_wpn->GetIconImage().GetFilename();
            else
                wpn_icon_filename = "img/icons/weapons/fist-human.png";

            if (!wpn_icon_filename.empty()) {
                _weapon_skill_list.AddOptionElementImage(i, wpn_icon_filename);
                _weapon_skill_list.GetEmbeddedImage(i)->SetHeightKeepRatio(25);
                _weapon_skill_list.AddOptionElementPosition(i, 30);
            }
        }
        _weapon_skill_list.AddOptionElementText(i, skill_list->at(i)->GetName());

        _weapon_target_list.AddOption(ustring());
        _weapon_target_list.AddOptionElementImage(i, BattleMode::CurrentInstance()->GetMedia().GetTargetTypeIcon(skill_list->at(i)->GetTargetType()));
        _weapon_target_list.AddOptionElementPosition(i, 45);
        _weapon_target_list.AddOptionElementText(i, MakeUnicodeString(NumberToString(skill_list->at(i)->GetSPRequired())));
        if(skill_list->at(i)->GetSPRequired() > _character->GetGlobalCharacter()->GetSkillPoints()) {
            _weapon_skill_list.EnableOption(i, false);
            _weapon_target_list.EnableOption(i, false);
        }
    }
    if(!skill_list->empty()) {
        _weapon_skill_list.SetSelection(0);
        _weapon_target_list.SetSelection(0);
    }

    skill_list = _character->GetGlobalCharacter()->GetMagicSkills();
    for(uint32 i = 0; i < skill_list->size(); i++) {
        _magic_skill_list.AddOption(ustring());
        if (!skill_list->at(i)->GetIconFilename().empty()) {
            _magic_skill_list.AddOptionElementImage(i, skill_list->at(i)->GetIconFilename());
            _magic_skill_list.GetEmbeddedImage(i)->SetHeightKeepRatio(25);
            _magic_skill_list.AddOptionElementPosition(i, 30);
        }
        _magic_skill_list.AddOptionElementText(i, skill_list->at(i)->GetName());

        _magic_target_list.AddOption(ustring());
        _magic_target_list.AddOptionElementImage(i, BattleMode::CurrentInstance()->GetMedia().GetTargetTypeIcon(skill_list->at(i)->GetTargetType()));
        _magic_target_list.AddOptionElementPosition(i, 45);
        _magic_target_list.AddOptionElementText(i, MakeUnicodeString(NumberToString(skill_list->at(i)->GetSPRequired())));
        if(skill_list->at(i)->GetSPRequired() > _character->GetGlobalCharacter()->GetSkillPoints()) {
            _magic_skill_list.EnableOption(i, false);
            _magic_target_list.EnableOption(i, false);
        }
    }
    if(!skill_list->empty()) {
        _magic_skill_list.SetSelection(0);
        _magic_target_list.SetSelection(0);
    }

    skill_list = _character->GetGlobalCharacter()->GetSpecialSkills();
    for(uint32 i = 0; i < skill_list->size(); i++) {
        _special_skill_list.AddOption(ustring());
        if (!skill_list->at(i)->GetIconFilename().empty()) {
            _special_skill_list.AddOptionElementImage(i, skill_list->at(i)->GetIconFilename());
            _special_skill_list.GetEmbeddedImage(i)->SetHeightKeepRatio(25);
            _special_skill_list.AddOptionElementPosition(i, 30);
        }
        _special_skill_list.AddOptionElementText(i, skill_list->at(i)->GetName());

        _special_target_list.AddOption(ustring());
        _special_target_list.AddOptionElementImage(i, BattleMode::CurrentInstance()->GetMedia().GetTargetTypeIcon(skill_list->at(i)->GetTargetType()));
        _special_target_list.AddOptionElementPosition(i, 45);
        _special_target_list.AddOptionElementText(i, MakeUnicodeString(NumberToString(skill_list->at(i)->GetSPRequired())));
        if(skill_list->at(i)->GetSPRequired() > _character->GetGlobalCharacter()->GetSkillPoints()) {
            _special_skill_list.EnableOption(i, false);
            _special_target_list.EnableOption(i, false);
        }
    }
    if(!skill_list->empty()) {
        _special_skill_list.SetSelection(0);
        _special_target_list.SetSelection(0);
    }
} // CharacterCommandSettings::CharacterCommandSettings(BattleCharacter* character, MenuWindow& window)



void CharacterCommandSettings::RefreshLists()
{
    uint32 require_sp = 0xFFFFFFFF;
    uint32 current_sp = _character->GetSkillPoints();
    std::vector<GlobalSkill *>* skill_list = NULL;

    skill_list = _character->GetGlobalCharacter()->GetWeaponSkills();
    for(uint32 i = 0; i < skill_list->size(); ++i) {
        require_sp = skill_list->at(i)->GetSPRequired();
        if(require_sp > current_sp) {
            _weapon_skill_list.EnableOption(i, false);
            _weapon_target_list.EnableOption(i, false);
        }
        else {
            _weapon_skill_list.EnableOption(i, true);
            _weapon_target_list.EnableOption(i, true);
        }
    }

    skill_list = _character->GetGlobalCharacter()->GetMagicSkills();
    for(uint32 i = 0; i < skill_list->size(); ++i) {
        require_sp = skill_list->at(i)->GetSPRequired();
        if(require_sp > current_sp) {
            _magic_skill_list.EnableOption(i, false);
            _magic_target_list.EnableOption(i, false);
        }
        else {
            _magic_skill_list.EnableOption(i, true);
            _magic_target_list.EnableOption(i, true);
        }
    }

    skill_list = _character->GetGlobalCharacter()->GetSpecialSkills();
    for(uint32 i = 0; i < skill_list->size(); ++i) {
        require_sp = skill_list->at(i)->GetSPRequired();
        if(require_sp > current_sp) {
            _special_skill_list.EnableOption(i, false);
            _special_target_list.EnableOption(i, false);
        }
        else {
            _special_skill_list.EnableOption(i, true);
            _special_target_list.EnableOption(i, true);
        }
    }
}



void CharacterCommandSettings::SaveLastTarget(BattleTarget &target)
{
    switch(target.GetType()) {
    case GLOBAL_TARGET_SELF_POINT:
    case GLOBAL_TARGET_SELF:
        _last_self_target = target;
        break;
    case GLOBAL_TARGET_ALLY_POINT:
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
        _last_character_target = target;
        break;
    case GLOBAL_TARGET_FOE_POINT:
    case GLOBAL_TARGET_FOE:
        _last_enemy_target = target;
        break;
    case GLOBAL_TARGET_ALL_ALLIES:
    case GLOBAL_TARGET_ALL_FOES:
        break; // Party type targets are not retained
    default:
        IF_PRINT_WARNING(BATTLE_DEBUG) << "target argument was an invalid type: " << target.GetType() << std::endl;
        break;
    }
}



void CharacterCommandSettings::SetLastSelfTarget(BattleTarget &target)
{
    if((target.GetType() != GLOBAL_TARGET_SELF_POINT) && (target.GetType() != GLOBAL_TARGET_SELF)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "target argument was an invalid type: " << target.GetType() << std::endl;
        return;
    }

    _last_self_target = target;
}



void CharacterCommandSettings::SetLastCharacterTarget(BattleTarget &target)
{
    if((target.GetType() != GLOBAL_TARGET_ALLY_POINT) && (target.GetType() != GLOBAL_TARGET_ALLY)
            && (target.GetType() != GLOBAL_TARGET_ALLY_EVEN_DEAD)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "target argument was an invalid type: " << target.GetType() << std::endl;
        return;
    }

    _last_character_target = target;
}



void CharacterCommandSettings::SetLastEnemyTarget(BattleTarget &target)
{
    if((target.GetType() != GLOBAL_TARGET_FOE_POINT) && (target.GetType() != GLOBAL_TARGET_FOE)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "target argument was an invalid type: " << target.GetType() << std::endl;
        return;
    }

    _last_enemy_target = target;
}

////////////////////////////////////////////////////////////////////////////////
// ItemCommand class
////////////////////////////////////////////////////////////////////////////////

ItemCommand::ItemCommand(MenuWindow &window)
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

    _item_target_list.SetOwner(&window);
    _item_target_list.SetPosition(LIST_POSITION_X + TARGET_ICON_OFFSET, LIST_POSITION_Y);
    _item_target_list.SetDimensions(LIST_SIZE_X - TARGET_ICON_OFFSET, LIST_SIZE_Y, 1, 255, 1, 4);
    _item_target_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _item_target_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _item_target_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _item_target_list.SetTextStyle(TextStyle("text20"));
    _item_target_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    ResetItemList();
}

void ItemCommand::ResetItemList()
{
    _items.clear();
    std::vector<GlobalItem *>* all_items = GlobalManager->GetInventoryItems();
    for(uint32 i = 0; i < all_items->size(); i++) {
        if(all_items->at(i)->IsUsableInBattle() == true) {
            if(all_items->at(i)->GetCount() == 0) {
                IF_PRINT_WARNING(BATTLE_DEBUG) << "discovered item in inventory with a zero count" << std::endl;
            }

            _items.push_back(BattleItem(GlobalItem(*all_items->at(i))));
        }
    }
    _item_mappings.resize(_items.size(), -1);
}



void ItemCommand::ConstructList()
{
    _item_list.ClearOptions();
    _item_target_list.ClearOptions();

    uint32 option_index = 0;
    for(uint32 i = 0; i < _items.size(); i++) {
        // Don't add any items with a non-zero count
        if(_items[i].GetBattleCount() == 0) {
            _item_mappings[i] = -1;
            continue;
        }

        _item_list.AddOption();
        if (!_items[i].GetItem().GetIconImage().GetFilename().empty()) {
            _item_list.AddOptionElementImage(option_index, _items[i].GetItem().GetIconImage().GetFilename());
            _item_list.GetEmbeddedImage(option_index)->SetHeightKeepRatio(25);
            _item_list.AddOptionElementPosition(option_index, 30);
        }
        _item_list.AddOptionElementText(option_index, _items[i].GetItem().GetName());

        _item_target_list.AddOption(ustring());
        _item_target_list.AddOptionElementImage(option_index, BattleMode::CurrentInstance()->GetMedia().GetTargetTypeIcon(_items[i].GetTargetType()));
        _item_target_list.AddOptionElementPosition(option_index, 45);
        _item_target_list.AddOptionElementText(option_index, MakeUnicodeString(NumberToString(_items[i].GetBattleCount())));

        _item_mappings[option_index] = i;
        option_index++;
    }

    if(_item_list.GetNumberOptions() == 0) {
        _item_list.SetSelection(-1);
        _item_target_list.SetSelection(-1);
    }
    else {
        _item_list.SetSelection(0);
        _item_target_list.SetSelection(0);
    }
}



void ItemCommand::Initialize(uint32 item_index)
{
    if(item_index >= _items.size()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "item_index argument was out-of-range: " << item_index << std::endl;
        return;
    }

    // If the item is in the list, set the list selection to that item
    if(_item_mappings[item_index] >= 0) {
        _item_list.SetSelection(_item_mappings[item_index]);
        _item_target_list.SetSelection(_item_mappings[item_index]);
        return;
    }

    // Otherwise find the nearest item to the desired item that is in the list
    uint32 next_item_index = 0xFFFFFFFF;
    uint32 prev_item_index = 0xFFFFFFFF;

    for(uint32 i = item_index + 1; i < _items.size(); i++) {
        if(_item_mappings[i] >= 0) {
            next_item_index = i;
            break;
        }
    }
    if(item_index != 0) {
        uint32 i = item_index - 1;

        do {
            if(_item_mappings[i] >= 0) {
                prev_item_index = i;
                break;
            }

            i--;
        } while(i != 0);
    }

    // If this case is true there are no items in the list. This should not happen because the item
    // command should not be used if no items exist
    if((next_item_index == 0xFFFFFFFF) && (prev_item_index == 0xFFFFFFFF)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "no items were in the list" << std::endl;
        return;
    } else if((next_item_index - item_index) <= (item_index - prev_item_index)) {
        _item_list.SetSelection(_item_mappings[next_item_index]);
        _item_target_list.SetSelection(_item_mappings[next_item_index]);
    } else {
        _item_list.SetSelection(_item_mappings[prev_item_index]);
        _item_target_list.SetSelection(_item_mappings[prev_item_index]);
    }
}



BattleItem *ItemCommand::GetSelectedItem()
{
    uint32 index = GetItemIndex();
    if(index == 0xFFFFFFFF)
        return NULL;
    else
        return &(_items[index]);
}



uint32 ItemCommand::GetItemIndex() const
{
    if(_item_list.GetSelection() < 0) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid selection in item list" << std::endl;
        return 0xFFFFFFFF;
    }

    int32 selection = _item_list.GetSelection();
    for(uint32 i = 0; i < _items.size(); i++) {
        if(_item_mappings[selection] == i) {
            return static_cast<uint32>(_item_mappings[selection]);
        }
    }

    // Execution should never reach this line
    IF_PRINT_WARNING(BATTLE_DEBUG) << "could not find index for item list selection: " << _item_list.GetSelection() << std::endl;
    return 0xFFFFFFFF;
}


bool ItemCommand::IsSelectedItemAvailable() const
{
    uint32 index = GetItemIndex();

    if(index == 0xFFFFFFFF)
        return false;
    else
        return (_items[_item_mappings[index]].GetBattleCount() > 0);
}


void ItemCommand::UpdateList()
{
    _item_list.Update();
    _item_target_list.Update();

    if(InputManager->UpPress()) {
        _item_list.InputUp();
        _item_target_list.InputUp();
        GlobalManager->Media().PlaySound("confirm");
    } else if(InputManager->DownPress()) {
        _item_list.InputDown();
        _item_target_list.InputDown();
        GlobalManager->Media().PlaySound("confirm");
    }
}



void ItemCommand::UpdateInformation()
{
    // TODO
}



void ItemCommand::DrawList()
{
    _item_header.Draw();
    _item_list.Draw();
    _item_target_list.Draw();
}



void ItemCommand::DrawInformation()
{
    // TODO
}



void ItemCommand::CommitChangesToInventory()
{
    for(uint32 i = 0; i < _items.size(); ++i) {
        // Get the global item id
        uint32 id = _items[i].GetItem().GetID();

        // Remove slots totally used
        if(_items[i].GetBattleCount() == 0) {
            GlobalManager->RemoveFromInventory(id);
        } else {
            int32 diff = _items[i].GetBattleCount() - _items[i].GetInventoryCount();
            if(diff > 0) {
                // Somehow the character have more than before the battle.
                GlobalManager->IncrementObjectCount(id, diff);
            } else if(diff < 0) {
                // Remove the used items.
                GlobalManager->DecrementObjectCount(id, -diff);
            }
        }
    }
}


void ItemCommand::RefreshEntry(uint32 entry)
{
    if(entry >= _item_list.GetNumberOptions()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "entry argument was out-of-range: " << entry << std::endl;
        return;
    }

    // Determine which item corresponds to the list entry
    int32 item_index = 0;
    for(uint32 i = 0; i < _item_mappings.size(); i++) {
        if(_item_mappings[i] == static_cast<int32>(entry)) {
            item_index = _item_mappings[i];
            break;
        }
    }

    // Clear the option and repopulate its elements
    _item_list.SetOptionText(entry, ustring());
    if (!_items[item_index].GetItem().GetIconImage().GetFilename().empty()) {
        _item_list.AddOptionElementImage(entry, _items[item_index].GetItem().GetIconImage().GetFilename());
        _item_list.GetEmbeddedImage(entry)->SetHeightKeepRatio(25);
        _item_list.AddOptionElementPosition(entry, 30);
    }
    _item_list.AddOptionElementText(entry, _items[item_index].GetItem().GetName());

    _item_target_list.SetOptionText(entry, ustring());
    _item_target_list.AddOptionElementImage(entry, BattleMode::CurrentInstance()->GetMedia().GetTargetTypeIcon(_items[item_index].GetTargetType()));
    _item_target_list.AddOptionElementPosition(entry, 45);
    _item_target_list.AddOptionElementText(entry, MakeUnicodeString(NumberToString(_items[item_index].GetBattleCount())));

    // Gray out the option when there are no items available.
    if(_items[item_index].GetBattleCount() == 0) {
        _item_list.EnableOption(entry, false);
        _item_target_list.EnableOption(entry, false);
    } else {
        // Re-enable it if we come to get an item back for any reasons
        _item_list.EnableOption(entry, true);
        _item_target_list.EnableOption(entry, true);
    }
}

////////////////////////////////////////////////////////////////////////////////
// SkillCommand class
////////////////////////////////////////////////////////////////////////////////

SkillCommand::SkillCommand(MenuWindow &window) :
    _skills(NULL),
    _skill_list(NULL),
    _target_n_cost_list(NULL)
{
    _skill_header.SetOwner(&window);
    _skill_header.SetPosition(HEADER_POSITION_X, HEADER_POSITION_Y);
    _skill_header.SetDimensions(HEADER_SIZE_X, HEADER_SIZE_Y, 1, 1, 1, 1);
    _skill_header.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _skill_header.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _skill_header.SetTextStyle(TextStyle("title22"));
    _skill_header.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _skill_header.AddOption(UTranslate("Skill<R>Type SP"));
}

void SkillCommand::Initialize(std::vector<GlobalSkill *>* skills,
                              OptionBox *skill_list,
                              OptionBox *target_n_cost_list)
{
    if(skills == NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL skills argument" << std::endl;
        return;
    }
    if(skill_list == NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL skill_list argument" << std::endl;
        return;
    }

    _skills = skills;
    _skill_list = skill_list;
    _target_n_cost_list = target_n_cost_list;
}

GlobalSkill *SkillCommand::GetSelectedSkill() const
{
    if((_skills == NULL) || (_skill_list == NULL))
        return NULL;

    uint32 selection = _skill_list->GetSelection();
    // The skills object needs to be returned even if not enabled due to low SP
    // in order to print information of both, enabled and disabled skills.
    return _skills->at(selection);
}



bool SkillCommand::GetSelectedSkillEnabled()
{
    if((_skills == NULL) || (_skill_list == NULL))
        return false;

    uint32 selection = _skill_list->GetSelection();
    return _skill_list->IsOptionEnabled(selection);
}



void SkillCommand::UpdateList()
{
    if(_skill_list == NULL)
        return;

    _skill_list->Update();
    _target_n_cost_list->Update();

    if(InputManager->UpPress()) {
        _skill_list->InputUp();
        _target_n_cost_list->InputUp();
        GlobalManager->Media().PlaySound("confirm");
    } else if(InputManager->DownPress()) {
        _skill_list->InputDown();
        _target_n_cost_list->InputDown();
        GlobalManager->Media().PlaySound("confirm");
    }
}



void SkillCommand::UpdateInformation()
{

}



void SkillCommand::DrawList()
{
    if(_skill_list == NULL)
        return;

    _skill_header.Draw();
    _skill_list->Draw();
    _target_n_cost_list->Draw();
}

////////////////////////////////////////////////////////////////////////////////
// CommandSupervisor class
////////////////////////////////////////////////////////////////////////////////

CommandSupervisor::CommandSupervisor() :
    _state(COMMAND_STATE_INVALID),
    _active_settings(NULL),
    _selected_skill(NULL),
    _selected_item(NULL),
    _item_command(_command_window),
    _skill_command(_command_window)
{
    if(_command_window.Create(512.0f, 128.0f) == false) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "failed to create menu window" << std::endl;
    }
    _command_window.SetPosition(512.0f, 640.0f);
    _command_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _command_window.Show();

    _category_icons.resize(4, StillImage());
    if(_category_icons[0].Load("img/icons/battle/default_weapon.png") == false)
        PRINT_ERROR << "failed to load category icon" << std::endl;
    if(_category_icons[1].Load("img/icons/battle/magic.png") == false)
        PRINT_ERROR << "failed to load category icon" << std::endl;
    if(_category_icons[2].Load("img/icons/battle/default_special.png") == false)
        PRINT_ERROR << "failed to load category icon" << std::endl;
    if(_category_icons[3].Load("img/icons/battle/item.png") == false)
        PRINT_ERROR << "failed to load category icon" << std::endl;

    _category_text.resize(4, TextImage("", TextStyle("title22")));
    _category_text[0].SetText(Translate("Weapon"));
    _category_text[1].SetText(Translate("Magic"));
    _category_text[2].SetText(""); // Default empty text
    _category_text[3].SetText(Translate("Items"));

    std::vector<ustring> option_text;
    option_text.push_back(MakeUnicodeString("<img/icons/battle/default_weapon.png>\n\n") + UTranslate("Weapon"));
    option_text.push_back(MakeUnicodeString("<img/icons/battle/magic.png>\n\n") + UTranslate("Magic"));
    option_text.push_back(MakeUnicodeString("")); // Special
    option_text.push_back(MakeUnicodeString("<img/icons/battle/item.png>\n\n") + UTranslate("Items"));

    _window_header.SetStyle(TextStyle("title22"));
    _window_text.SetStyle(TextStyle("text20"));

    _category_options.SetOwner(&_command_window);
    _category_options.SetPosition(256.0f, 80.0f);
    _category_options.SetDimensions(400.0f, 100.0f, 4, 1, 4, 1);
    _category_options.SetCursorOffset(-20.0f, -25.0f);
    _category_options.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _category_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
    _category_options.SetTextStyle(TextStyle("title22"));
    _category_options.SetSelectMode(VIDEO_SELECT_SINGLE);
    _category_options.SetOptions(option_text);
    _category_options.SetSelection(0);

    // Skip disabled category items
    _category_options.SetSkipDisabled(true);

    _target_options.SetOwner(&_command_window);
    _target_options.SetPosition(TARGET_POSITION_X, TARGET_POSITION_Y);
    _target_options.SetDimensions(TARGET_SIZE_X, TARGET_SIZE_Y, 1, 255, 1, 4);
    _target_options.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _target_options.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _target_options.SetVerticalWrapMode(VIDEO_WRAP_MODE_NONE);
    _target_options.SetTextStyle(TextStyle("text20"));
    _target_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _target_options.SetCursorOffset(-50.0f, -25.0f);
}



CommandSupervisor::~CommandSupervisor()
{
    _command_window.Destroy();
}



void CommandSupervisor::ConstructMenus()
{
    _item_command.ConstructList();

    std::deque<BattleCharacter *>& characters = BattleMode::CurrentInstance()->GetCharacterActors();
    for(uint32 i = 0; i < characters.size(); i++)
        _CreateCharacterSettings(characters[i]);
}



void CommandSupervisor::Initialize(BattleCharacter *character)
{
    if(character == NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function recieved NULL pointer argument" << std::endl;
        _state = COMMAND_STATE_INVALID;
        return;
    }

    if(_HasCharacterSettings(character) == false)
        _CreateCharacterSettings(character);

    _ChangeState(COMMAND_STATE_CATEGORY);
    _active_settings = &(_character_settings.find(character)->second);
    // Update _skill_list to check, if some skills need to be deactivated due to low amount of SP
    _active_settings->RefreshLists();
    // Also refresh the item list
    _item_command.ConstructList();

    _category_options.SetSelection(_active_settings->GetLastCategory());

    // Determine which categories should be enabled or disabled

    // Weapon
    // Determine the weapon icon name
    std::string icon_name = "<";

    GlobalWeapon* wpn = character->GetGlobalCharacter()->GetWeaponEquipped();
    if (wpn) {
        if (wpn->GetIconImage().GetFilename().empty())
            icon_name += "img/icons/battle/default_weapon.png";
        else
            icon_name += wpn->GetIconImage().GetFilename();
    }
    else {
        icon_name += "img/icons/weapons/fist-human.png";
    }
    icon_name += ">\n\n";

    // When a character has no basic weapon skill, we add the 'pass turn' skill instead.
    if(wpn) {
        _category_options.SetOptionText(CATEGORY_WEAPON, MakeUnicodeString(icon_name) +  UTranslate("Weapon"));
        _category_text[CATEGORY_WEAPON].SetText(UTranslate("Weapon"));
    }
    else {
        _category_options.SetOptionText(CATEGORY_WEAPON, MakeUnicodeString(icon_name) +  UTranslate("Bare-hand"));
        _category_text[CATEGORY_WEAPON].SetText(UTranslate("Bare-hand"));
    }

    // If there are no skills, disable it, and print a warning
    if (_active_settings->GetWeaponSkillList()->GetNumberOptions() == 0) {
        _category_options.EnableOption(CATEGORY_WEAPON, false);
        PRINT_WARNING << "No weapon or bare hand skills, the battle might get stuck" << std::endl;
    }

    // Magic
    if(_active_settings->GetMagicSkillList()->GetNumberOptions() == 0)
        _category_options.EnableOption(CATEGORY_MAGIC, false);
    else
        _category_options.EnableOption(CATEGORY_MAGIC, true);

    // Special
    if(_active_settings->GetSpecialSkillList()->GetNumberOptions() == 0) {
        _category_options.SetOptionText(CATEGORY_SPECIAL, MakeUnicodeString(""));
        _category_options.EnableOption(CATEGORY_SPECIAL, false);
        _category_text[CATEGORY_SPECIAL].SetText("");
    }
    else {
        // Set icon from character config.
        std::string special_icon = character->GetGlobalCharacter()->GetSpecialCategoryIconFilename();
        if (special_icon.empty())
            special_icon = "img/icons/battle/default_special.png";

        vt_utils::ustring special_name = character->GetGlobalCharacter()->GetSpecialCategoryName();
        _category_options.SetOptionText(CATEGORY_SPECIAL, MakeUnicodeString("<" + special_icon + ">\n\n") + special_name);
        _category_options.EnableOption(CATEGORY_SPECIAL, true);
        // Use special name from character config
        _category_text[CATEGORY_SPECIAL].SetText(special_name);
    }

    // Items
    if(_item_command.GetNumberListOptions() == 0)
        _category_options.EnableOption(CATEGORY_ITEM, false);
    else
        _category_options.EnableOption(CATEGORY_ITEM, true);

    // Warn if there are no enabled options in the category list
    for(uint32 i = 0; i < _category_options.GetNumberOptions(); i++) {
        if(_category_options.IsOptionEnabled(i) == true)
            return;
    }

    PRINT_ERROR << "No category options were enabled. The game might be stuck." << std::endl;
}



void CommandSupervisor::Update()
{
    switch(_state) {
    case COMMAND_STATE_CATEGORY:
        _UpdateCategory();
        break;
    case COMMAND_STATE_ACTION:
        _UpdateAction();
        break;
    case COMMAND_STATE_ACTOR:
        _UpdateActorTarget();
        break;
    case COMMAND_STATE_POINT:
        _UpdateAttackPointTarget();
        break;
    case COMMAND_STATE_INFORMATION:
        _UpdateInformation();
        break;
    default:
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid/unknown command state: " << _state << std::endl;
        _ChangeState(COMMAND_STATE_CATEGORY);
        return;
    }
}



void CommandSupervisor::Draw()
{
    _command_window.Draw();

    switch(_state) {
    case COMMAND_STATE_CATEGORY:
        _DrawCategory();
        break;
    case COMMAND_STATE_ACTION:
        _DrawAction();
        break;
    case COMMAND_STATE_ACTOR:
        _DrawActorTarget();
        break;
    case COMMAND_STATE_POINT:
        _DrawAttackPointTarget();
        break;
    case COMMAND_STATE_INFORMATION:
        _DrawInformation();
        break;
    default:
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid/unknown command state: " << _state << std::endl;
        _ChangeState(COMMAND_STATE_CATEGORY);
        return;
    }
}



void CommandSupervisor::NotifyActorDeath(BattleActor *actor)
{
    if(_state == COMMAND_STATE_INVALID) {
        IF_PRINT_WARNING(BATTLE_DEBUG)
                << "function called when class was in invalid state" << std::endl;
        return;
    }

    BattleCharacter *character = GetCommandCharacter();

    // If the character currently selecting a command dies, we get out.
    if(character == actor) {
        _ChangeState(COMMAND_STATE_INVALID);
        GlobalManager->Media().PlaySound("cancel");
        return;
    }

    // Update the selected target if the target is the actor who just deceased
    if(_selected_target.GetActor() == actor) {
        GlobalManager->Media().PlaySound("cancel");

        // Try selecting the next actor and fall back to the previous command menu
        // if not possible.
        if(!_selected_target.SelectNextActor(actor)) {
            if(character && (_state == COMMAND_STATE_ACTOR || _state == COMMAND_STATE_POINT))
                _ChangeState(COMMAND_STATE_ACTION);
            return;
        }
    }
}



bool CommandSupervisor::_IsSkillCategorySelected() const
{
    int32 category = _category_options.GetSelection();
    if((category == CATEGORY_WEAPON) || (category == CATEGORY_MAGIC) || (category == CATEGORY_SPECIAL))
        return true;
    else
        return false;
}



bool CommandSupervisor::_IsItemCategorySelected() const
{
    int32 category = _category_options.GetSelection();
    if(category == CATEGORY_ITEM)
        return true;
    else
        return false;
}



GLOBAL_TARGET CommandSupervisor::_ActionTargetType()
{
    if(_IsSkillCategorySelected() == true)
        return _skill_command.GetSelectedSkill()->GetTargetType();
    else if(_IsItemCategorySelected() == true)
        return _item_command.GetSelectedItem()->GetTargetType();
    else
        return GLOBAL_TARGET_INVALID;
}



bool CommandSupervisor::_SetInitialTarget()
{
    BattleActor *user = GetCommandCharacter();
    GLOBAL_TARGET target_type = _ActionTargetType();

    // Party targets are simple because we don't have to restore the last save target, since there is either the
    // ally party or foe party, and both parties are always valid targets (because otherwise the battle would have
    // already ended).
    if(IsTargetParty(target_type) == true) {
        // Party-type targets are always the same, so we don't need to recall the last target in this case
        return _selected_target.SetInitialTarget(user, target_type);
    }

    // Retrieved the last saved target depending on the type (self/ally/foe)
    if(IsTargetSelf(target_type)) {
        _selected_target = _active_settings->GetLastSelfTarget();
    } else if(IsTargetAlly(target_type)) {
        _selected_target = _active_settings->GetLastCharacterTarget();
    } else if(IsTargetFoe(target_type)) {
        _selected_target = _active_settings->GetLastEnemyTarget();
    } else {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "no conditions met for invalid target type: " << target_type << std::endl;
    }

    bool permit_dead_targets = (_selected_target.GetType() == GLOBAL_TARGET_ALLY_EVEN_DEAD);

    // Otherwise if the last target is no longer valid, select the next valid target
    if(!_selected_target.IsValid(permit_dead_targets)) {
        // Party targets should always be valid and attack points on actors do not disappear, so only the actor
        // must be invalid
        if(!_selected_target.SelectNextActor(user, permit_dead_targets)) {
            // No more target of that type, let's go back to the command state
            // Invalidate the target so that one can get a completely new one
            _selected_target.InvalidateTarget();
        }
    }

    // If the target type is invalid that means that there is no previous target so grab the initial target
    if(_selected_target.GetType() == GLOBAL_TARGET_INVALID) {
        if(!_selected_target.SetInitialTarget(user, target_type)) {
            // No more target of that type, let's go back to the command state
            _selected_target.InvalidateTarget();
            GlobalManager->Media().PlaySound("cancel");
            return false;
        }
    }

    // This case occurs when our last target was an actor type and we're now using an action with a point target,
    // or vice versa. We need to modify the target type while still retaining the original target actor.
    if(_selected_target.GetType() != target_type) {
        if(IsTargetPoint(target_type) == true) {
            _selected_target.SetPointTarget(target_type, 0);
        } else { // then IsTargetActor(target_type) == true
            _selected_target.SetActorTarget(target_type, _selected_target.GetActor());
        }
    }
    return true;
}

void CommandSupervisor::_ChangeState(COMMAND_STATE new_state)
{
    if(_state == new_state) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "class was already in state to change to: " << new_state << std::endl;
        return;
    }

    if(new_state == COMMAND_STATE_INVALID) {
        _active_settings = NULL;
        _selected_skill = NULL;
        _selected_item = NULL;
    } else if(new_state == COMMAND_STATE_CATEGORY) {
        // Nothing to do here. The Initialize() function performs all necessary actions when entering this state.
    } else if(new_state == COMMAND_STATE_ACTION) {
        // Construct the appropriate skill or item selection list if we're coming from the category state
        if(_state == COMMAND_STATE_CATEGORY) {
            switch(_category_options.GetSelection()) {
            case CATEGORY_WEAPON:
                if (GetCommandCharacter()->GetGlobalCharacter()->GetWeaponEquipped()) {
                    _skill_command.Initialize(GetCommandCharacter()->GetGlobalCharacter()->GetWeaponSkills(),
                                              _active_settings->GetWeaponSkillList(), _active_settings->GetWeaponTargetList());
                }
                else {
                    _skill_command.Initialize(GetCommandCharacter()->GetGlobalCharacter()->GetBareHandsSkills(),
                                              _active_settings->GetWeaponSkillList(), _active_settings->GetWeaponTargetList());
                }
                break;
            case CATEGORY_MAGIC:
                _skill_command.Initialize(GetCommandCharacter()->GetGlobalCharacter()->GetMagicSkills(),
                                          _active_settings->GetMagicSkillList(), _active_settings->GetMagicTargetList());
                break;
            case CATEGORY_SPECIAL:
                _skill_command.Initialize(GetCommandCharacter()->GetGlobalCharacter()->GetSpecialSkills(),
                                          _active_settings->GetSpecialSkillList(), _active_settings->GetSpecialTargetList());
                break;
            case CATEGORY_ITEM:
                _item_command.Initialize(_active_settings->GetLastItem());
                break;
            default:
                IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid category selection: " << _category_options.GetSelection() << std::endl;
                _category_options.SetSelection(0);
                return;
            }
        }
    } else if(new_state == COMMAND_STATE_ACTOR) {
        // Set the initial target if we're coming from the action selection state
        if(_state == COMMAND_STATE_ACTION) {
            if(!_SetInitialTarget()) {
                GlobalManager->Media().PlaySound("cancel");
                return;
            }
        }

        _CreateActorTargetText();
    } else if(new_state == COMMAND_STATE_POINT) {
        _CreateAttackPointTargetText();
    } else if(new_state == COMMAND_STATE_INFORMATION) {
        _CreateInformationText();
    }

    _state = new_state;
}



void CommandSupervisor::_UpdateCategory()
{
    _category_options.Update();

    // Event priority is given to the player requesting to abort the command selection process
    if(InputManager->CancelPress()) {
        BattleMode *BM = BattleMode::CurrentInstance();
        // The only time we do not allow the player to abort the command menu is if they are running the battle with the "wait" setting active and the
        // current character is in the command state. Under these circumstances, the player has to enter a command for this character before the battle
        // is allowed to continue.
        if((BM->GetBattleType() == BATTLE_TYPE_WAIT || BM->GetBattleType() == BATTLE_TYPE_SEMI_ACTIVE)
            && (GetCommandCharacter()->GetState() == ACTOR_STATE_COMMAND)) {
            GlobalManager->Media().PlaySound("cancel");
        } else {
            _ChangeState(COMMAND_STATE_INVALID);
            BM->NotifyCommandCancel();
            GlobalManager->Media().PlaySound("cancel");
        }
    }

    else if(InputManager->ConfirmPress()) {
        if(_category_options.IsOptionEnabled(_category_options.GetSelection())) {
            _active_settings->SetLastCategory(_category_options.GetSelection());
            _ChangeState(COMMAND_STATE_ACTION);
            GlobalManager->Media().PlaySound("confirm");
        } else {
            GlobalManager->Media().PlaySound("cancel");
        }
    }

    else if(InputManager->LeftPress()) {
        _category_options.InputLeft();
        GlobalManager->Media().PlaySound("confirm");
    }

    else if(InputManager->RightPress()) {
        _category_options.InputRight();
        GlobalManager->Media().PlaySound("confirm");
    }
}



void CommandSupervisor::_UpdateAction()
{
    if(InputManager->CancelPress()) {
        _ChangeState(COMMAND_STATE_CATEGORY);
        GlobalManager->Media().PlaySound("cancel");
        return;
    }

    if(_IsSkillCategorySelected() == true) {
        _selected_skill = _skill_command.GetSelectedSkill();

        if(InputManager->ConfirmPress()) {
            bool is_skill_enabled = _skill_command.GetSelectedSkillEnabled();
            if(is_skill_enabled == true) {
                _ChangeState(COMMAND_STATE_ACTOR);
                GlobalManager->Media().PlaySound("confirm");
            } else {
                GlobalManager->Media().PlaySound("cancel");
            }
        }

        else if(InputManager->MenuPress()) {
            _ChangeState(COMMAND_STATE_INFORMATION);
            GlobalManager->Media().PlaySound("confirm");
        }

        else {
            _skill_command.UpdateList();
        }
    } else if(_IsItemCategorySelected() == true) {
        _selected_item = _item_command.GetSelectedItem();

        if(InputManager->ConfirmPress()) {
            // Permit the selection only where are items left.
            if(_selected_item != NULL && _item_command.IsSelectedItemAvailable()) {
                _ChangeState(COMMAND_STATE_ACTOR);
                GlobalManager->Media().PlaySound("confirm");
            } else {
                GlobalManager->Media().PlaySound("cancel");
            }
        }

        else if(InputManager->MenuPress()) {
            _ChangeState(COMMAND_STATE_INFORMATION);
            GlobalManager->Media().PlaySound("confirm");
        }

        else {
            _item_command.UpdateList();
        }
    } else {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid category selection: " << _category_options.GetSelection() << std::endl;
        _state = COMMAND_STATE_CATEGORY;
        _category_options.SetSelection(CATEGORY_WEAPON);
    }
}



void CommandSupervisor::_UpdateActorTarget()
{
    if(InputManager->CancelPress()) {
        _ChangeState(COMMAND_STATE_ACTION);
        GlobalManager->Media().PlaySound("cancel");
    }

    else if(InputManager->ConfirmPress()) {
        if(IsTargetPoint(_selected_target.GetType()) == true) {
            _ChangeState(COMMAND_STATE_POINT);
        } else {
            _FinalizeCommand();
        }
    }

    else if(InputManager->UpPress() || InputManager->DownPress()) {
        if(InputManager->DownPress())
            _target_options.InputDown();
        else
            _target_options.InputUp();

        bool direction = InputManager->DownPress();
        bool permit_dead_targets = (_selected_target.GetType() == GLOBAL_TARGET_ALLY_EVEN_DEAD);

        if((IsTargetActor(_selected_target.GetType()) == true) || (IsTargetPoint(_selected_target.GetType()) == true)) {
            _selected_target.SelectNextActor(GetCommandCharacter(), direction, true, permit_dead_targets);
            _CreateActorTargetText();
            GlobalManager->Media().PlaySound("confirm");
        }
    }
}



void CommandSupervisor::_UpdateAttackPointTarget()
{
    if(InputManager->CancelPress()) {
        _ChangeState(COMMAND_STATE_ACTOR);
        GlobalManager->Media().PlaySound("cancel");
    }

    else if(InputManager->ConfirmPress()
            || _selected_target.GetActor()->GetAttackPoints().size() == 1) {
        _FinalizeCommand();
    }

    else if(InputManager->UpPress() || InputManager->DownPress()) {
        if(InputManager->DownPress())
            _target_options.InputDown();
        else
            _target_options.InputUp();

        _selected_target.SelectNextPoint(GetCommandCharacter(), InputManager->DownPress());
        _CreateAttackPointTargetText();
        GlobalManager->Media().PlaySound("confirm");
    }
}



void CommandSupervisor::_UpdateInformation()
{
    if(InputManager->CancelPress() || InputManager->MenuPress()) {
        _state = COMMAND_STATE_ACTION;
        GlobalManager->Media().PlaySound("cancel");
    }

    else if(InputManager->ConfirmPress()) {
        _ChangeState(COMMAND_STATE_ACTOR);
        GlobalManager->Media().PlaySound("cancel");
    }

    // Change selected skill/item and update the information text
    else if(InputManager->UpPress() || InputManager->DownPress()) {

        if(_IsSkillCategorySelected() == true) {
            _skill_command.UpdateList();
            _selected_skill = _skill_command.GetSelectedSkill();
            GlobalManager->Media().PlaySound("confirm");
        } else if(_IsItemCategorySelected() == true) {
            _item_command.UpdateList();
            _selected_item = _item_command.GetSelectedItem();
            GlobalManager->Media().PlaySound("confirm");
        }

        _CreateInformationText();
    }

    else if(InputManager->RightPress() || InputManager->LeftPress()) {
        // TODO: toggle between description text and detailed stats
    }
}



void CommandSupervisor::_DrawCategory()
{
    _category_options.Draw();
}



void CommandSupervisor::_DrawAction()
{
    uint32 category_index = _category_options.GetSelection();

    // Draw the corresponding category icon and text to the left side of the window
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
    VideoManager->Move(570.0f, 693.0);
    if (_category_options.GetEmbeddedImage(category_index))
        _category_options.GetEmbeddedImage(category_index)->Draw();
    else
        _category_icons[category_index].Draw();
    VideoManager->MoveRelative(0.0f, 35.0f);
    _category_text[category_index].Draw();

    // Draw the header and list for either the skills or items to the right side of the window
    if(_IsSkillCategorySelected()) {
        _skill_command.DrawList();
    } else if(_IsItemCategorySelected()) {
        _item_command.DrawList();
    }
}



void CommandSupervisor::_DrawActorTarget()
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(560.0f, 658.0f);
    _window_header.Draw();
    VideoManager->Move(560.0f, 683.0f);
    _window_text.Draw();

// 	_target_options.Draw();
    // TODO: draw relevant status/elemental icons
}



void CommandSupervisor::_DrawAttackPointTarget()
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(560.0f, 658.0f);
    _window_header.Draw();
    VideoManager->Move(560.0f, 683.0f);
// 	_window_text.Draw();

    _target_options.Draw();
    // TODO: draw relevant status/elemental icons
}



void CommandSupervisor::_DrawInformation()
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(560.0f, 658.0f);
    _window_header.Draw();
    VideoManager->Move(560.0f, 683.0f);
    _window_text.Draw();

    // TODO: draw relevant status/elemental icons
}



void CommandSupervisor::_CreateActorTargetText()
{
    _window_header.SetText(UTranslate("Select Target"));

    _target_options.ClearOptions();
    if(IsTargetParty(_selected_target.GetType()) == true) {
        if(_selected_target.GetType() == GLOBAL_TARGET_ALL_ALLIES) {
            _target_options.AddOption(UTranslate("All Allies"));
        } else {
            _target_options.AddOption(UTranslate("All Enemies"));
        }
    } else if(IsTargetSelf(_selected_target.GetType()) == true) {
        _target_options.AddOption(_selected_target.GetActor()->GetName());
    } else if(IsTargetAlly(_selected_target.GetType()) == true) {
        for(uint32 i = 0; i < BattleMode::CurrentInstance()->GetCharacterActors().size(); i++) {
            _target_options.AddOption(BattleMode::CurrentInstance()->GetCharacterActors().at(i)->GetName());
            if(_selected_target.GetType() != GLOBAL_TARGET_ALLY_EVEN_DEAD
                    && !BattleMode::CurrentInstance()->GetCharacterActors().at(i)->IsAlive()) {
                _target_options.EnableOption(i, false);
            }
        }
    } else if(IsTargetFoe(_selected_target.GetType()) == true) {
        for(uint32 i = 0; i < BattleMode::CurrentInstance()->GetEnemyActors().size(); i++) {
            _target_options.AddOption(BattleMode::CurrentInstance()->GetEnemyActors().at(i)->GetName());
            if(BattleMode::CurrentInstance()->GetEnemyActors().at(i)->IsAlive() == false) {
                _target_options.EnableOption(i, false);
            }
        }
    } else {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << _selected_target.GetType() << std::endl;
    }

    // TEMP: remove once _target_options box works properly
    if(IsTargetParty(_selected_target.GetType()) == true) {
        if(_selected_target.GetType() == GLOBAL_TARGET_ALL_ALLIES) {
            _window_text.SetText(UTranslate("All Allies"));
        } else {
            _window_text.SetText(UTranslate("All Enemies"));
        }
    } else {
        _window_text.SetText(_selected_target.GetActor()->GetName());
    }

}



void CommandSupervisor::_CreateAttackPointTargetText()
{
    _window_header.SetText(UTranslate("Select Attack Point"));

    BattleActor *actor = _selected_target.GetActor();
    uint32 selected_point = _selected_target.GetPoint();

    _target_options.ClearOptions();
    for(uint32 i = 0; i < actor->GetAttackPoints().size(); i++) {
        _target_options.AddOption(actor->GetAttackPoints().at(i)->GetName());
    }

    _target_options.SetSelection(selected_point);
}



void CommandSupervisor::_CreateInformationText()
{
    ustring info_text;

    if(_IsSkillCategorySelected() == true) {
        _window_header.SetText(_selected_skill->GetName());

        info_text = UTranslate("Skill Points: " + NumberToString(_selected_skill->GetSPRequired())) + MakeUnicodeString("\n");
        info_text += UTranslate("Target Type: ") + MakeUnicodeString(GetTargetText(_selected_skill->GetTargetType())) + MakeUnicodeString("\n");
        info_text += UTranslate("Prep Time: ") + MakeUnicodeString(NumberToString(_selected_skill->GetWarmupTime())) + MakeUnicodeString("\n");
        info_text += UTranslate("Cool Time: ") + MakeUnicodeString(NumberToString(_selected_skill->GetCooldownTime())) + MakeUnicodeString("\n");
    } else if(_IsItemCategorySelected() == true) {
        _window_header.SetText(_selected_item->GetItem().GetName());

        info_text = UTranslate("Quantity: " + NumberToString(_selected_item->GetBattleCount())) + MakeUnicodeString("\n");
        info_text += UTranslate("Target Type: ") + MakeUnicodeString(GetTargetText(_selected_item->GetItem().GetTargetType())) + MakeUnicodeString("\n");
    } else {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "unknown category selected: " << _category_options.GetSelection() << std::endl;
    }

    _window_text.SetText(info_text);
}



void CommandSupervisor::_FinalizeCommand()
{
    BattleAction *new_action = NULL;
    BattleCharacter *character = GetCommandCharacter();

    _active_settings->SaveLastTarget(_selected_target);

    if(_IsSkillCategorySelected() == true) {
        new_action = new SkillAction(character, _selected_target, _selected_skill);
    } else if(_IsItemCategorySelected() == true) {
        new_action = new ItemAction(character, _selected_target, _selected_item);

        // Reserve the item for use by the character.
        _selected_item->DecrementBattleCount();
        _item_command.RefreshEntry(_item_command.GetItemIndex());
    } else {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "did not create action for character, unknown category selected: " << _category_options.GetSelection() << std::endl;
    }
    character->SetAction(new_action);

    _ChangeState(COMMAND_STATE_INVALID);
    BattleMode::CurrentInstance()->NotifyCharacterCommandComplete(character);
    GlobalManager->Media().PlaySound("confirm");
}

} // namespace private_battle

} // namespace vt_battle
