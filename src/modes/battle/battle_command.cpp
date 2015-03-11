////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
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

#include "utils/utils_pch.h"
#include "modes/battle/battle_command.h"

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/system.h"
#include "engine/video/video.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actions.h"
#include "modes/battle/battle_actors.h"
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
    _weapon_skill_list.AnimateScrolling(false);

    _weapon_target_list.SetOwner(&window);
    _weapon_target_list.SetPosition(LIST_POSITION_X + TARGET_ICON_OFFSET, LIST_POSITION_Y);
    _weapon_target_list.SetDimensions(LIST_SIZE_X - TARGET_ICON_OFFSET, LIST_SIZE_Y, 1, 255, 1, 4);
    _weapon_target_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _weapon_target_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _weapon_target_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _weapon_target_list.SetTextStyle(TextStyle("text20"));
    _weapon_target_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _weapon_target_list.AnimateScrolling(false);

    _magic_skill_list.SetOwner(&window);
    _magic_skill_list.SetPosition(LIST_POSITION_X, LIST_POSITION_Y);
    _magic_skill_list.SetDimensions(LIST_SIZE_X, LIST_SIZE_Y, 1, 255, 1, 4);
    _magic_skill_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _magic_skill_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _magic_skill_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _magic_skill_list.SetTextStyle(TextStyle("text20"));
    _magic_skill_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _magic_skill_list.SetCursorOffset(-50.0f, -25.0f);
    _magic_skill_list.AnimateScrolling(false);

    _magic_target_list.SetOwner(&window);
    _magic_target_list.SetPosition(LIST_POSITION_X + TARGET_ICON_OFFSET, LIST_POSITION_Y);
    _magic_target_list.SetDimensions(LIST_SIZE_X - TARGET_ICON_OFFSET, LIST_SIZE_Y, 1, 255, 1, 4);
    _magic_target_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _magic_target_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _magic_target_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _magic_target_list.SetTextStyle(TextStyle("text20"));
    _magic_target_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _magic_target_list.AnimateScrolling(false);

    _special_skill_list.SetOwner(&window);
    _special_skill_list.SetPosition(LIST_POSITION_X, LIST_POSITION_Y);
    _special_skill_list.SetDimensions(LIST_SIZE_X, LIST_SIZE_Y, 1, 255, 1, 4);
    _special_skill_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _special_skill_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _special_skill_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _special_skill_list.SetTextStyle(TextStyle("text20"));
    _special_skill_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _special_skill_list.SetCursorOffset(-50.0f, -25.0f);
    _special_skill_list.AnimateScrolling(false);

    _special_target_list.SetOwner(&window);
    _special_target_list.SetPosition(LIST_POSITION_X + TARGET_ICON_OFFSET, LIST_POSITION_Y);
    _special_target_list.SetDimensions(LIST_SIZE_X - TARGET_ICON_OFFSET, LIST_SIZE_Y, 1, 255, 1, 4);
    _special_target_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _special_target_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _special_target_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _special_target_list.SetTextStyle(TextStyle("text20"));
    _special_target_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _special_target_list.AnimateScrolling(false);

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
                wpn_icon_filename = "data/inventory/weapons/fist-human.png";

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
    case GLOBAL_TARGET_DEAD_ALLY:
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
    std::vector<GlobalItem *>* inv_items = GlobalManager->GetInventoryItems();
    for(uint32 i = 0; i < inv_items->size(); ++i) {
        // Only add non key and valid items as items available at battle start.
        if(inv_items->at(i)->GetCount() == 0)
            continue;
        if (!inv_items->at(i)->IsValid())
            continue;
        if (inv_items->at(i)->IsKeyItem())
            continue;

        if(inv_items->at(i)->IsUsableInBattle())
            _battle_items.push_back(BattleItem(GlobalItem(*inv_items->at(i))));
    }
    // We clear the menu items list as its pointers are now invalid.
    _menu_items.clear();
}

void ItemCommand::ConstructList()
{
    _item_list.ClearOptions();
    _item_target_list.ClearOptions();

    _menu_items.clear();

    uint32 option_index = 0;
    for(uint32 i = 0; i < _battle_items.size(); ++i) {
        BattleItem* item = &_battle_items[i];
        // Don't add any items with a zero count
        if(item->GetBattleCount() == 0)
            continue;

        // Adds the BattleItem pointer to the main item list
        _menu_items.push_back(item);

        // Adds the item menu option
        _item_list.AddOption();
        if (!item->GetItem().GetIconImage().GetFilename().empty()) {
            _item_list.AddOptionElementImage(option_index, item->GetItem().GetIconImage().GetFilename());
            _item_list.GetEmbeddedImage(option_index)->SetHeightKeepRatio(25);
            _item_list.AddOptionElementPosition(option_index, 30);
        }
        _item_list.AddOptionElementText(option_index, item->GetItem().GetName());

        _item_target_list.AddOption(ustring());
        _item_target_list.AddOptionElementImage(option_index,
                                                BattleMode::CurrentInstance()->GetMedia().GetTargetTypeIcon(item->GetTargetType()));
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



void ItemCommand::Initialize(uint32 battle_item_index)
{
    // If there is no more usable items in the inventory,
    // we can set the selection as invalid.
    if(_item_list.GetNumberOptions() == 0 || _menu_items.size() == 0) {
        _item_list.SetSelection(-1);
        _item_target_list.SetSelection(-1);
        return;
    }

    // In case the selection of the previous item leads to something
    // not relevant anymore, we reset the selection.
    if(battle_item_index >= _battle_items.size() ||
            _battle_items[battle_item_index].GetBattleCount() == 0) {
        _item_list.SetSelection(0);
        _item_target_list.SetSelection(0);
        return;
    }

    // Find the corresponding item in the menu list and select it.
    uint32 selection = 0; // First item in the list per default
    BattleItem* selected_item = &_battle_items[battle_item_index];
    for (uint32 i = 0; i < _menu_items.size(); ++i) {
        if (_menu_items[i] == selected_item) {
            selection = i;
            break;
        }
    }

    // If the item is in the list, set the list selection to that item
    _item_list.SetSelection(selection);
    _item_target_list.SetSelection(selection);
}



BattleItem *ItemCommand::GetSelectedItem()
{
    int32 selection = _item_list.GetSelection();
    if (selection == -1 || selection >= (int32)_menu_items.size())
        return NULL;

    return _menu_items[selection];
}

bool ItemCommand::IsSelectedItemAvailable() const
{
    int32 selection = _item_list.GetSelection();
    if (selection == -1 || selection >= (int32)_menu_items.size())
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
    for(uint32 i = 0; i < _battle_items.size(); ++i) {
        // Get the global item id
        uint32 id = _battle_items[i].GetItem().GetID();

        // Remove slots totally used
        if(_battle_items[i].GetBattleCount() == 0) {
            GlobalManager->RemoveFromInventory(id);
        } else {
            int32 diff = _battle_items[i].GetBattleCount() - _battle_items[i].GetInventoryCount();
            if(diff > 0) {
                // Somehow the character have more than before the battle.
                GlobalManager->IncrementItemCount(id, diff);
            } else if(diff < 0) {
                // Remove the used items.
                GlobalManager->DecrementItemCount(id, -diff);
            }
        }
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
        GlobalManager->Media().PlaySound("bump");
    } else if(InputManager->DownPress()) {
        _skill_list->InputDown();
        _target_n_cost_list->InputDown();
        GlobalManager->Media().PlaySound("bump");
    }
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
    _skill_command(_command_window),
    _show_information(false)
{
    if(_command_window.Create(512.0f, 128.0f) == false) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "failed to create menu window" << std::endl;
    }
    _command_window.SetPosition(512.0f, 640.0f);
    _command_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _command_window.Show();

    _info_window.Create(512.0f, 150.0f);
    _info_window.SetPosition(512.0f, 470.0f);
    _info_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _info_window.Show();

    _category_icons.resize(4, StillImage());
    if(_category_icons[0].Load("data/battles/menu_icons/default_weapon.png") == false)
        PRINT_ERROR << "failed to load category icon" << std::endl;
    if(_category_icons[1].Load("data/battles/menu_icons/magic.png") == false)
        PRINT_ERROR << "failed to load category icon" << std::endl;
    if(_category_icons[2].Load("data/battles/menu_icons/default_special.png") == false)
        PRINT_ERROR << "failed to load category icon" << std::endl;
    if(_category_icons[3].Load("data/battles/menu_icons/item.png") == false)
        PRINT_ERROR << "failed to load category icon" << std::endl;

    _category_text.resize(4, TextImage("", TextStyle("title22")));
    _category_text[0].SetText(Translate("Weapon"));
    _category_text[1].SetText(Translate("Magic"));
    _category_text[2].SetText(""); // Default empty text
    _category_text[3].SetText(Translate("Items"));

    std::vector<ustring> option_text;
    option_text.push_back(MakeUnicodeString("<data/battles/menu_icons/default_weapon.png>\n\n") + UTranslate("Weapon"));
    option_text.push_back(MakeUnicodeString("<data/battles/menu_icons/magic.png>\n\n") + UTranslate("Magic"));
    option_text.push_back(MakeUnicodeString("")); // Special
    option_text.push_back(MakeUnicodeString("<data/battles/menu_icons/item.png>\n\n") + UTranslate("Items"));

    _window_header.SetStyle(TextStyle("title22"));
    _selected_target_name.SetStyle(TextStyle("text20"));
    _info_header.SetStyle(TextStyle("title22"));
    _info_text.SetStyle(TextStyle("text20"));
    _info_text.SetWordWrapWidth(475);

    _category_options.SetOwner(&_command_window);
    _category_options.SetPosition(256.0f, 80.0f);
    _category_options.SetDimensions(400.0f, 100.0f, 4, 1, 4, 1);
    _category_options.SetCursorOffset(-20.0f, -25.0f);
    _category_options.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _category_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
    _category_options.SetTextStyle(TextStyle("title22"));
    _category_options.SetSelectMode(VIDEO_SELECT_SINGLE);
    _category_options.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _category_options.SetOptions(option_text);
    _category_options.SetSelection(0);

    // Skip disabled category items
    _category_options.SetSkipDisabled(true);

    _target_options.SetOwner(&_command_window);
    _target_options.SetPosition(TARGET_POSITION_X, TARGET_POSITION_Y);
    _target_options.SetDimensions(TARGET_SIZE_X, TARGET_SIZE_Y, 1, 255, 1, 4);
    _target_options.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _target_options.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _target_options.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _target_options.SetTextStyle(TextStyle("text20"));
    _target_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _target_options.SetCursorOffset(-50.0f, -25.0f);
}

CommandSupervisor::~CommandSupervisor()
{
    _command_window.Destroy();
    _info_window.Destroy();
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
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL pointer argument" << std::endl;
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
            icon_name += "data/battles/menu_icons/default_weapon.png";
        else
            icon_name += wpn->GetIconImage().GetFilename();
    }
    else {
        icon_name += "data/inventory/weapons/fist-human.png";
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
            special_icon = "data/battles/menu_icons/default_special.png";

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
        // Show potential info about the current selection
        if (_show_information)
            _DrawActionInformation();

        _DrawAction();
        break;
    case COMMAND_STATE_ACTOR:
        _DrawActorTarget();
        break;
    case COMMAND_STATE_POINT:
        // Show potential info about the currently selected attack point
        if (_show_information)
            _DrawActionInformation();

        _DrawAttackPointTarget();
        break;
    default:
        break;
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

    // If we don't memorize the last target, simply select the first one again.
    if(!SystemManager->GetBattleTargetMemory()) {
        // If the target type is invalid that means that there is no previous target so grab the initial target
        if(!_selected_target.SetInitialTarget(user, target_type)) {
            // No more target of that type, let's go back to the command state
            _selected_target.InvalidateTarget();
            GlobalManager->Media().PlaySound("cancel");
            return false;
        }
        return true;
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

    bool permit_dead_targets = ((_selected_target.GetType() == GLOBAL_TARGET_ALLY_EVEN_DEAD)
                                || (_selected_target.GetType() == GLOBAL_TARGET_DEAD_ALLY));

    // Otherwise if the last target is no longer valid, select the next valid target
    if(!_selected_target.IsValid(permit_dead_targets)) {
        // Party targets should always be valid and attack points on actors do not disappear, so only the actor
        // must be invalid
        if(!_selected_target.SelectNextActor(user, true, true, permit_dead_targets)) {
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
        GlobalManager->Media().PlaySound("bump");
    }

    else if(InputManager->RightPress()) {
        _category_options.InputRight();
        GlobalManager->Media().PlaySound("bump");
    }
}



void CommandSupervisor::_UpdateAction()
{
    if(InputManager->CancelPress()) {
        _ChangeState(COMMAND_STATE_CATEGORY);
        GlobalManager->Media().PlaySound("cancel");
        return;
    }

    // Handles showing skills/item info on menu key pressed state
    if(InputManager->MenuState()) {
        _show_information = true;
        _UpdateActionInformation();
    }
    else {
        _show_information = false;
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
        else {
            _skill_command.UpdateList();
            _UpdateActionInformation();
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
        else {
            _item_command.UpdateList();
            _UpdateActionInformation();
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
        bool permit_dead_targets = ((_selected_target.GetType() == GLOBAL_TARGET_ALLY_EVEN_DEAD)
                                || (_selected_target.GetType() == GLOBAL_TARGET_DEAD_ALLY));

        if((IsTargetActor(_selected_target.GetType()) == true) || (IsTargetPoint(_selected_target.GetType()) == true)) {
            // Since we're changing the target, we reinit the attack point to the first one,
            // as the new target may have less attack points than the latest one.
            _selected_target.ReinitAttackPoint();
            _selected_target.SelectNextActor(GetCommandCharacter(), direction, true, permit_dead_targets);
            _CreateActorTargetText();
            GlobalManager->Media().PlaySound("bump");
        }
    }
    _target_options.Update();
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
        GlobalManager->Media().PlaySound("confirm");
    }

    else if(InputManager->UpPress() || InputManager->DownPress()) {
        if(InputManager->DownPress())
            _target_options.InputDown();
        else
            _target_options.InputUp();

        _selected_target.SelectNextPoint(GetCommandCharacter(), InputManager->DownPress());
        _CreateAttackPointTargetText();
        GlobalManager->Media().PlaySound("bump");
    }

    // Handles showing target points info
    if(InputManager->MenuState()) {
        _show_information = true;
        _UpdateActionInformation();
    }
    else {
        _show_information = false;
    }
}

std::string _TurnIntoSeconds(uint32 milliseconds)
{
    uint32 seconds = milliseconds / 1000;
    uint32 dec = (milliseconds / 100) - (seconds * 10);
    std::string formatted_seconds = NumberToString(seconds) + "." + NumberToString(dec);
    /// TRANSLATORS: this is about displaying a time: eg 4.2s
    formatted_seconds = VTranslate("%ss", formatted_seconds);
    return formatted_seconds;
}

void CommandSupervisor::_UpdateActionInformation()
{
    ustring info_text;

    if (_state == COMMAND_STATE_POINT) {
        // Show the target points information.
        BattleActor* actor = _selected_target.GetActor();
        uint32 selected_point = _selected_target.GetPoint();
        GlobalAttackPoint* attack_point = actor->GetAttackPoint(selected_point);

        _info_header.SetText(attack_point->GetName());

        // Set the text
        // Evade
        float evade_modifier = attack_point->GetEvadeModifier();
        if (evade_modifier != 0.0f) {
            std::string evade_str;
            if (evade_modifier > 0.0f)
                evade_str = "+" + NumberToString(evade_modifier);
            else
                evade_str = NumberToString(evade_modifier);
            info_text += MakeUnicodeString(VTranslate("Evade: %s", evade_str) + "%    ");
        }
        // Fortitude (Physical defense.)
        float fortitude_modifier = attack_point->GetFortitudeModifier();
        if (fortitude_modifier != 0.0f) {
            std::string fort_str;
            if (fortitude_modifier > 0.0f)
                fort_str = "+" + NumberToString(fortitude_modifier);
            else
                fort_str = NumberToString(fortitude_modifier);
            info_text += MakeUnicodeString(VTranslate("Fortitude: %s", fort_str) + "    ");
        }
        // Protection (Magical defense.)
        float protection_modifier = attack_point->GetProtectionModifier();
        if (protection_modifier != 0.0f) {
            std::string prot_str;
            if (protection_modifier > 0.0f)
                prot_str = "+" + NumberToString(protection_modifier);
            else
                prot_str = NumberToString(protection_modifier);
            info_text += MakeUnicodeString(VTranslate("Protection: %s", prot_str));
        }
        info_text += MakeUnicodeString("\n\nEffects: ");

    } else if(_IsSkillCategorySelected() == true) {
        _info_header.SetText(_selected_skill->GetName()
                             + MakeUnicodeString(" - "
                             + VTranslate("%s SP", NumberToString(_selected_skill->GetSPRequired()))));

        info_text = MakeUnicodeString(VTranslate("Target Type: %s", GetTargetText(_selected_skill->GetTargetType())) + "\n");
        info_text += MakeUnicodeString(VTranslate("Prep Time: %s", _TurnIntoSeconds(_selected_skill->GetWarmupTime())) + " - ");
        info_text += MakeUnicodeString(VTranslate("Cool Time: %s", _TurnIntoSeconds(_selected_skill->GetCooldownTime())) + "\n\n");
        info_text += _selected_skill->GetDescription();
    } else if(_IsItemCategorySelected() == true) {
        _info_header.SetText(_selected_item->GetItem().GetName()
                             + MakeUnicodeString(" x " + NumberToString(_selected_item->GetBattleCount())));
        info_text = MakeUnicodeString(VTranslate("Target Type: %s", GetTargetText(_selected_item->GetTargetType())) + "\n\n");
        info_text += _selected_item->GetItem().GetDescription();
    } else {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "unknown category selected: " << _category_options.GetSelection() << std::endl;
    }

    _info_text.SetText(info_text);
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
    _selected_target_name.Draw();
    // We don't draw the target option as it may desync when enemies spawn/die.
    //_target_options.Draw();

    // Draw relevant active status effect icons
    float width = _selected_target_name.GetWidth() < 130.0f ? 130.0f : _selected_target_name.GetWidth() + 10.0f;
    VideoManager->MoveRelative(width, 0.0f);
    for (uint32 i = 0; i < _selected_target_status_effects.size(); ++i) {
        _selected_target_status_effects[i]->Draw();
        VideoManager->MoveRelative(25.0f, 0.0f);
    }
}

void CommandSupervisor::_DrawAttackPointTarget()
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(560.0f, 658.0f);
    _window_header.Draw();
    VideoManager->Move(560.0f, 683.0f);
    _target_options.Draw();
}

void CommandSupervisor::_DrawActionInformation()
{
    _info_window.Draw();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(560.0f, 488.0f);
    _info_header.Draw();
    VideoManager->Move(530.0f, 510.0f);
    _info_text.Draw();
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

    // Clear the shown effects first.
    _selected_target_status_effects.clear();

    if(IsTargetParty(_selected_target.GetType()) == true) {
        if(_selected_target.GetType() == GLOBAL_TARGET_ALL_ALLIES) {
            _selected_target_name.SetText(UTranslate("All Allies"));
        } else {
            _selected_target_name.SetText(UTranslate("All Enemies"));
        }
    } else {
        _selected_target_name.SetText(_selected_target.GetActor()->GetName());

        // Get every non neutral status effects.
        for (uint32 i = 0; i < GLOBAL_STATUS_TOTAL; ++i) {
            GLOBAL_STATUS status = static_cast<GLOBAL_STATUS>(i);
            GLOBAL_INTENSITY intensity = _selected_target.GetActor()->GetActiveStatusEffectIntensity(status);
            if (intensity == GLOBAL_INTENSITY_NEUTRAL ||
                    intensity <= GLOBAL_INTENSITY_INVALID ||
                    intensity >= GLOBAL_INTENSITY_TOTAL) {
                continue;
            }

            _selected_target_status_effects.push_back(vt_global::GlobalManager->Media().GetStatusIcon(status, intensity));
        }
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
    } else {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "did not create action for character, unknown category selected: " << _category_options.GetSelection() << std::endl;
    }
    character->SetAction(new_action);

    _ChangeState(COMMAND_STATE_INVALID);
    BattleMode::CurrentInstance()->NotifyCharacterCommandComplete(character);
}

} // namespace private_battle

} // namespace vt_battle
