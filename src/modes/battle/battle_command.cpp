////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_command.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for battle menu windows
*** ***************************************************************************/

#include "audio.h"
#include "input.h"
#include "system.h"
#include "video.h"

#include "battle.h"
#include "battle_actions.h"
#include "battle_actors.h"
#include "battle_command.h"
#include "battle_utils.h"

using namespace std;

using namespace hoa_utils;

using namespace hoa_audio;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_input;
using namespace hoa_system;
using namespace hoa_global;

namespace hoa_battle {

namespace private_battle {

const float HEADER_POSITION_X = 140.0f;
const float HEADER_POSITION_Y = 140.0f;
const float HEADER_SIZE_X = 350.0f;
const float HEADER_SIZE_Y = 30.0f;

const float LIST_POSITION_X = 140.0f;
const float LIST_POSITION_Y = 115.0f;
const float LIST_SIZE_X = 350.0f;
const float LIST_SIZE_Y = 100.0f;

const float TARGET_POSITION_X = 40.0f;
const float TARGET_POSITION_Y = 115.0f;
const float TARGET_SIZE_X = 450.0f;
const float TARGET_SIZE_Y = 100.0f;

// Offset used to properly align the target icons in the skill and item selection lists
const uint32 TARGET_ICON_OFFSET = 288;

////////////////////////////////////////////////////////////////////////////////
// CharacterCommandSettings class
////////////////////////////////////////////////////////////////////////////////

CharacterCommandSettings::CharacterCommandSettings(BattleCharacter* character, MenuWindow& window) :
	_character(character),
	_last_category(CATEGORY_ATTACK),
	_last_item(0),
	_last_self_target(BattleTarget()),
	_last_character_target(BattleTarget()),
	_last_enemy_target(BattleTarget())
{
	_attack_list.SetOwner(&window);
	_attack_list.SetPosition(LIST_POSITION_X, LIST_POSITION_Y);
	_attack_list.SetDimensions(LIST_SIZE_X, LIST_SIZE_Y, 1, 255, 1, 4);
	_attack_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_attack_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_attack_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_attack_list.SetTextStyle(TextStyle("text20"));
	_attack_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	_attack_list.SetCursorOffset(-50.0f, 25.0f);

	_defend_list.SetOwner(&window);
	_defend_list.SetPosition(LIST_POSITION_X, LIST_POSITION_Y);
	_defend_list.SetDimensions(LIST_SIZE_X, LIST_SIZE_Y, 1, 255, 1, 4);
	_defend_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_defend_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_defend_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_defend_list.SetTextStyle(TextStyle("text20"));
	_defend_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	_defend_list.SetCursorOffset(-50.0f, 25.0f);

	_support_list.SetOwner(&window);
	_support_list.SetPosition(LIST_POSITION_X, LIST_POSITION_Y);
	_support_list.SetDimensions(LIST_SIZE_X, LIST_SIZE_Y, 1, 255, 1, 4);
	_support_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_support_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_support_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_support_list.SetTextStyle(TextStyle("text20"));
	_support_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	_support_list.SetCursorOffset(-50.0f, 25.0f);

	if (_character == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL character pointer" << endl;
		return;
	}

	// Construct the attack, defend, and support skill lists for the character
	vector<GlobalSkill*>* skill_list = NULL;

	skill_list = _character->GetGlobalCharacter()->GetAttackSkills();
	for (uint32 i = 0; i < skill_list->size(); i++) {
		_attack_list.AddOption(ustring());
		_attack_list.AddOptionElementText(i, skill_list->at(i)->GetName());
		_attack_list.AddOptionElementPosition(i, TARGET_ICON_OFFSET);
		_attack_list.AddOptionElementImage(i, BattleMode::CurrentInstance()->GetMedia().GetTargetTypeIcon(skill_list->at(i)->GetTargetType()));
		_attack_list.AddOptionElementAlignment(i, VIDEO_OPTION_ELEMENT_RIGHT_ALIGN);
		_attack_list.AddOptionElementText(i, MakeUnicodeString(NumberToString(skill_list->at(i)->GetSPRequired())));
		if (skill_list->at(i)->GetSPRequired() > _character->GetGlobalCharacter()->GetSkillPoints()) {
			_attack_list.EnableOption(i, false);
		}
	}
	if (skill_list->empty() == false)
		_attack_list.SetSelection(0);

	skill_list = _character->GetGlobalCharacter()->GetDefenseSkills();
	for (uint32 i = 0; i < skill_list->size(); i++) {
		_defend_list.AddOption(ustring());
		_defend_list.AddOptionElementText(i, skill_list->at(i)->GetName());
		_defend_list.AddOptionElementPosition(i, TARGET_ICON_OFFSET);
		_defend_list.AddOptionElementImage(i, BattleMode::CurrentInstance()->GetMedia().GetTargetTypeIcon(skill_list->at(i)->GetTargetType()));
		_defend_list.AddOptionElementAlignment(i, VIDEO_OPTION_ELEMENT_RIGHT_ALIGN);
		_defend_list.AddOptionElementText(i, MakeUnicodeString(NumberToString(skill_list->at(i)->GetSPRequired())));
		if (skill_list->at(i)->GetSPRequired() > _character->GetGlobalCharacter()->GetSkillPoints()) {
			_defend_list.EnableOption(i, false);
		}
	}
	if (skill_list->empty() == false)
		_defend_list.SetSelection(0);

	skill_list = _character->GetGlobalCharacter()->GetSupportSkills();
	for (uint32 i = 0; i < skill_list->size(); i++) {
		_support_list.AddOption(ustring());
		_support_list.AddOptionElementText(i, skill_list->at(i)->GetName());
		_support_list.AddOptionElementPosition(i, TARGET_ICON_OFFSET);
		_support_list.AddOptionElementImage(i, BattleMode::CurrentInstance()->GetMedia().GetTargetTypeIcon(skill_list->at(i)->GetTargetType()));
		_support_list.AddOptionElementAlignment(i, VIDEO_OPTION_ELEMENT_RIGHT_ALIGN);
		_support_list.AddOptionElementText(i, MakeUnicodeString(NumberToString(skill_list->at(i)->GetSPRequired())));
		if (skill_list->at(i)->GetSPRequired() > _character->GetGlobalCharacter()->GetSkillPoints()) {
			_support_list.EnableOption(i, false);
		}
	}
	if (skill_list->empty() == false)
		_support_list.SetSelection(0);
} // CharacterCommandSettings::CharacterCommandSettings(BattleCharacter* character, MenuWindow& window)



void CharacterCommandSettings::RefreshLists() {
	uint32 require_sp = 0xFFFFFFFF;
	uint32 current_sp = _character->GetSkillPoints();
	vector<GlobalSkill*>* skill_list = NULL;

	skill_list = _character->GetGlobalCharacter()->GetAttackSkills();
	for (uint32 i = 0; i < skill_list->size(); i++) {
		require_sp = skill_list->at(i)->GetSPRequired();
		if (require_sp > current_sp)
			_attack_list.EnableOption(i, false);
		else
			_attack_list.EnableOption(i, true);
	}

	skill_list = _character->GetGlobalCharacter()->GetDefenseSkills();
	for (uint32 i = 0; i < skill_list->size(); i++) {
		require_sp = skill_list->at(i)->GetSPRequired();
		if (require_sp > current_sp)
			_defend_list.EnableOption(i, false);
		else
			_defend_list.EnableOption(i, true);
	}

	skill_list = _character->GetGlobalCharacter()->GetSupportSkills();
	for (uint32 i = 0; i < skill_list->size(); i++) {
		require_sp = skill_list->at(i)->GetSPRequired();
		if (require_sp > current_sp)
			_support_list.EnableOption(i, false);
		else
			_support_list.EnableOption(i, true);
	}
}



void CharacterCommandSettings::SaveLastTarget(BattleTarget& target) {
	switch (target.GetType()) {
		case GLOBAL_TARGET_SELF_POINT:
		case GLOBAL_TARGET_SELF:
			_last_self_target = target;
			break;
		case GLOBAL_TARGET_ALLY_POINT:
		case GLOBAL_TARGET_ALLY:
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
			IF_PRINT_WARNING(BATTLE_DEBUG) << "target argument was an invalid type: " << target.GetType() << endl;
			break;
	}
}



void CharacterCommandSettings::SetLastSelfTarget(BattleTarget& target) {
	if ((target.GetType() != GLOBAL_TARGET_SELF_POINT) && (target.GetType() != GLOBAL_TARGET_SELF)) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "target argument was an invalid type: " << target.GetType() << endl;
		return;
	}

	_last_self_target = target;
}



void CharacterCommandSettings::SetLastCharacterTarget(BattleTarget& target) {
	if ((target.GetType() != GLOBAL_TARGET_ALLY_POINT) && (target.GetType() != GLOBAL_TARGET_ALLY)) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "target argument was an invalid type: " << target.GetType() << endl;
		return;
	}

	_last_character_target = target;
}



void CharacterCommandSettings::SetLastEnemyTarget(BattleTarget& target) {
	if ((target.GetType() != GLOBAL_TARGET_FOE_POINT) && (target.GetType() != GLOBAL_TARGET_FOE)) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "target argument was an invalid type: " << target.GetType() << endl;
		return;
	}

	_last_enemy_target = target;
}

////////////////////////////////////////////////////////////////////////////////
// ItemCommand class
////////////////////////////////////////////////////////////////////////////////

ItemCommand::ItemCommand(MenuWindow& window) {
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
	_item_list.SetCursorOffset(-50.0f, 25.0f);

	vector<GlobalItem*>* all_items = GlobalManager->GetInventoryItems();
	for (uint32 i = 0; i < all_items->size(); i++) {
		if (all_items->at(i)->IsUsableInBattle() == true) {
			if (all_items->at(i)->GetCount() == 0) {
				IF_PRINT_WARNING(BATTLE_DEBUG) << "discovered item in inventory with a zero count" << endl;
			}

			_items.push_back(BattleItem(GlobalItem(*all_items->at(i))));
		}
	}
	_item_mappings.resize(_items.size(), -1);
}



void ItemCommand::ConstructList() {
	_item_list.ClearOptions();

	uint32 option_index = 0;
	for (uint32 i = 0; i < _items.size(); i++) {
		// Don't add any items with a non-zero count
		if (_items[i].GetAvailableCount() == 0) {
			_item_mappings[i] = -1;
			continue;
		}

		_item_list.AddOption();
		_item_list.AddOptionElementText(option_index, _items[i].GetItem().GetName());
		_item_list.AddOptionElementPosition(option_index, TARGET_ICON_OFFSET);
		_item_list.AddOptionElementImage(option_index, BattleMode::CurrentInstance()->GetMedia().GetTargetTypeIcon(_items[i].GetTargetType()));
		_item_list.AddOptionElementAlignment(option_index, VIDEO_OPTION_ELEMENT_RIGHT_ALIGN);
		_item_list.AddOptionElementText(option_index, MakeUnicodeString(NumberToString(_items[i].GetAvailableCount())));

		_item_mappings[i] = option_index;
		option_index++;
	}

	if (_item_list.GetNumberOptions() == 0)
		_item_list.SetSelection(-1);
	else
		_item_list.SetSelection(0);
}



void ItemCommand::Initialize(uint32 item_index) {
	if (item_index >= _items.size()) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "item_index argument was out-of-range: " << item_index << endl;
		return;
	}

	// If the item is in the list, set the list selection to that item
	if (_item_mappings[item_index] >= 0) {
		_item_list.SetSelection(_item_mappings[item_index]);
		return;
	}

	// Otherwise find the nearest item to the desired item that is in the list
	uint32 next_item_index = 0xFFFFFFFF;
	uint32 prev_item_index = 0xFFFFFFFF;

	for (uint32 i = item_index + 1; i < _items.size(); i++) {
		if (_item_mappings[i] >= 0) {
			next_item_index = i;
			break;
		}
	}
	if (item_index != 0) {
		uint32 i = item_index - 1;

		do {
			if (_item_mappings[i] >= 0) {
				prev_item_index = i;
				break;
			}

			i--;
		} while (i != 0);
	}

	// If this case is true there are no items in the list. This should not happen because the item
	// command should not be used if no items exist
	if ((next_item_index == 0xFFFFFFFF) && (prev_item_index == 0xFFFFFFFF)) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "no items were in the list" << endl;
		return;
	}
	else if ((next_item_index - item_index) <= (item_index - prev_item_index)) {
		_item_list.SetSelection(_item_mappings[next_item_index]);
	}
	else {
		_item_list.SetSelection(_item_mappings[prev_item_index]);
	}
}



BattleItem* ItemCommand::GetSelectedItem() {
	uint32 index = GetItemIndex();
	if (index == 0xFFFFFFFF)
		return NULL;
	else
		return &(_items[index]);
}



uint32 ItemCommand::GetItemIndex() const {
	if (_item_list.GetSelection() < 0) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid selection in item list" << endl;
		return 0xFFFFFFFF;
	}

	int32 selection = _item_list.GetSelection();
	for (uint32 i = 0; i < _items.size(); i++) {
		if (_item_mappings[i] == selection) {
			return static_cast<uint32>(selection);
		}
	}

	// Execution should never reach this line
	IF_PRINT_WARNING(BATTLE_DEBUG) << "could not find index for item list selection: " << _item_list.GetSelection() << endl;
	return 0xFFFFFFFF;
}



void ItemCommand::UpdateList() {
	_item_list.Update();

	if (InputManager->UpPress()) {
		_item_list.InputUp();
		BattleMode::CurrentInstance()->GetMedia().cursor_sound.Play();
	}
	else if (InputManager->DownPress()) {
		_item_list.InputDown();
		BattleMode::CurrentInstance()->GetMedia().cursor_sound.Play();
	}
}



void ItemCommand::UpdateInformation() {
	// TODO
}



void ItemCommand::DrawList() {
	_item_header.Draw();
	_item_list.Draw();
}



void ItemCommand::DrawInformation() {
	// TODO
}



void ItemCommand::CommitInventoryChanges() {
	for (uint32 i = 0; i < _items.size(); i++) {
		if (_items[i].GetAvailableCount() != _items[i].GetCount()) {
			IF_PRINT_WARNING(BATTLE_DEBUG) << "" << endl;
		}

		// TODO
	}
}


void ItemCommand::_RefreshEntry(uint32 entry) {
	if (entry >= _item_list.GetNumberOptions()) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "entry argument was out-of-range: " << entry << endl;
		return;
	}

	// Determine which item corresponds to the list entry
	int32 item_index = 0;
	for (uint32 i = 0; i < _item_mappings.size(); i++) {
		if (_item_mappings[i] == static_cast<int32>(entry)) {
			item_index = _item_mappings[i];
			break;
		}
	}

	// Clear the option and repopulate its elements
	_item_list.SetOptionText(entry, ustring());
	_item_list.AddOptionElementText(entry, _items[item_index].GetItem().GetName());
	_item_list.AddOptionElementPosition(entry, TARGET_ICON_OFFSET);
	_item_list.AddOptionElementImage(entry, BattleMode::CurrentInstance()->GetMedia().GetTargetTypeIcon(_items[item_index].GetTargetType()));
	_item_list.AddOptionElementAlignment(entry, VIDEO_OPTION_ELEMENT_RIGHT_ALIGN);
	_item_list.AddOptionElementText(entry, MakeUnicodeString(NumberToString(_items[item_index].GetAvailableCount())));
}

////////////////////////////////////////////////////////////////////////////////
// SkillCommand class
////////////////////////////////////////////////////////////////////////////////

SkillCommand::SkillCommand(MenuWindow& window) :
	_skills(NULL),
	_skill_list(NULL)
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



void SkillCommand::Initialize(vector<GlobalSkill*>* skills, OptionBox* skill_list) {
	if (skills == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL skills argument" << endl;
		return;
	}
	if (skill_list == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL skill_list argument" << endl;
		return;
	}

	_skills = skills;
	_skill_list = skill_list;
}



GlobalSkill* SkillCommand::GetSelectedSkill() const {
	if ((_skills == NULL) || (_skill_list == NULL))
		return NULL;

	uint32 selection = _skill_list->GetSelection();
    // _skills needs to be returned even if not enabled due to low SP in order to print information
    // of both, enabled and disabled skills.
//	if (_skill_list->IsOptionEnabled(selection) == false)
//		return NULL;
//	else
    return _skills->at(selection);
}



bool SkillCommand::GetSelectedSkillEnabled() {
	if ((_skills == NULL) || (_skill_list == NULL))
		return false;

	uint32 selection = _skill_list->GetSelection();
    return _skill_list->IsOptionEnabled(selection);
}



void SkillCommand::UpdateList() {
	if (_skill_list == NULL)
		return;

	_skill_list->Update();

	if (InputManager->UpPress()) {
		_skill_list->InputUp();
		BattleMode::CurrentInstance()->GetMedia().cursor_sound.Play();
	}
	else if (InputManager->DownPress()) {
		_skill_list->InputDown();
		BattleMode::CurrentInstance()->GetMedia().cursor_sound.Play();
	}
}



void SkillCommand::UpdateInformation() {

}



void SkillCommand::DrawList() {
	if (_skill_list == NULL)
		return;

	_skill_header.Draw();
	_skill_list->Draw();
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
	if (_command_window.Create(512.0f, 128.0f) == false) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "failed to create menu window" << endl;
	}
	_command_window.SetPosition(512.0f, 128.0f);
	_command_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_command_window.Show();

	_category_icons.resize(4, StillImage());
	if (_category_icons[0].Load("img/icons/battle/attack.png") == false)
		PRINT_ERROR << "failed to load category icon" << endl;
	if (_category_icons[1].Load("img/icons/battle/defend.png") == false)
		PRINT_ERROR << "failed to load category icon" << endl;
	if (_category_icons[2].Load("img/icons/battle/support.png") == false)
		PRINT_ERROR << "failed to load category icon" << endl;
	if (_category_icons[3].Load("img/icons/battle/item.png") == false)
		PRINT_ERROR << "failed to load category icon" << endl;

	_category_text.resize(4, TextImage("", TextStyle("title22")));
	_category_text[0].SetText(Translate("Attack"));
	_category_text[1].SetText(Translate("Defend"));
	_category_text[2].SetText(Translate("Support"));
	_category_text[3].SetText(Translate("Item"));

	vector<ustring> option_text;
	option_text.push_back(MakeUnicodeString("<img/icons/battle/attack.png>\n") + UTranslate("Attack"));
	option_text.push_back(MakeUnicodeString("<img/icons/battle/defend.png>\n") + UTranslate("Defend"));
	option_text.push_back(MakeUnicodeString("<img/icons/battle/support.png>\n") + UTranslate("Support"));
	option_text.push_back(MakeUnicodeString("<img/icons/battle/item.png>\n") + UTranslate("Item"));

	_window_header.SetStyle(TextStyle("title22"));
	_window_text.SetStyle(TextStyle("text20"));

	_category_options.SetOwner(&_command_window);
	_category_options.SetPosition(256.0f, 75.0f);
	_category_options.SetDimensions(400.0f, 80.0f, 4, 1, 4, 1);
	_category_options.SetCursorOffset(-20.0f, 25.0f);
	_category_options.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_category_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_category_options.SetTextStyle(TextStyle("title22"));
	_category_options.SetSelectMode(VIDEO_SELECT_SINGLE);
	_category_options.SetOptions(option_text);
	_category_options.SetSelection(0);

	_target_options.SetOwner(&_command_window);
	_target_options.SetPosition(TARGET_POSITION_X, TARGET_POSITION_Y);
	_target_options.SetDimensions(TARGET_SIZE_X, TARGET_SIZE_Y, 1, 255, 1, 4);
	_target_options.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_target_options.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_target_options.SetVerticalWrapMode(VIDEO_WRAP_MODE_NONE);
	_target_options.SetTextStyle(TextStyle("text20"));
	_target_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	_target_options.SetCursorOffset(-50.0f, 25.0f);
}



CommandSupervisor::~CommandSupervisor() {
	_command_window.Destroy();
}



void CommandSupervisor::ConstructMenus() {
	_item_command.ConstructList();

	deque<BattleCharacter*>& characters = BattleMode::CurrentInstance()->GetCharacterActors();
	for (uint32 i = 0; i < characters.size(); i++)
		_CreateCharacterSettings(characters[i]);
}



void CommandSupervisor::Initialize(BattleCharacter* character) {
	if (character == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function recieved NULL pointer argument" << endl;
		_state = COMMAND_STATE_INVALID;
		return;
	}

	if (_HasCharacterSettings(character) == false)
		_CreateCharacterSettings(character);

	_ChangeState(COMMAND_STATE_CATEGORY);
	_active_settings = &(_character_settings.find(character)->second);
    // Update _skill_list to check, if some skills need to be deactivated due to low amount of SP
    _active_settings->RefreshLists();
	_category_options.SetSelection(_active_settings->GetLastCategory());

	// Determine which categories should be enabled or disabled
	if (_active_settings->GetAttackList()->GetNumberOptions() == 0)
		_category_options.EnableOption(0, false);
	else
		_category_options.EnableOption(0, true);
	if (_active_settings->GetDefendList()->GetNumberOptions() == 0)
		_category_options.EnableOption(1, false);
	else
		_category_options.EnableOption(1, true);
	if (_active_settings->GetSupportList()->GetNumberOptions() == 0)
		_category_options.EnableOption(2, false);
	else
		_category_options.EnableOption(2, true);
	if (_item_command.GetNumberListOptions() == 0)
		_category_options.EnableOption(3, false);
	else
		_category_options.EnableOption(3, true);

	// Warn if there are no enabled options in the category list
	for (uint32 i = 0; i < _category_options.GetNumberOptions(); i++) {
		if (_category_options.IsOptionEnabled(i) == true)
			return;
	}

	IF_PRINT_WARNING(BATTLE_DEBUG) << "no options in category list were enabled" << endl;
}



void CommandSupervisor::Update() {
	switch (_state) {
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
			IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid/unknown command state: " << _state << endl;
			_ChangeState(COMMAND_STATE_CATEGORY);
			return;
	}
}



void CommandSupervisor::Draw() {
	_command_window.Draw();

	switch (_state) {
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
			IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid/unknown command state: " << _state << endl;
			_ChangeState(COMMAND_STATE_CATEGORY);
			return;
	}
}



void CommandSupervisor::NotifyActorDeath(BattleActor* actor) {
	if (_state == COMMAND_STATE_INVALID) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function called when class was in invalid state" << endl;
		return;
	}

	if (GetCommandCharacter() == actor) {
		_ChangeState(COMMAND_STATE_INVALID);
		return;
	}

	// TODO: update the selected target if the target is the actor who just deceased
	// if (_selected_target.GetActor() == actor)
}



bool CommandSupervisor::_IsSkillCategorySelected() const {
	uint32 category = _category_options.GetSelection();
	if ((category == CATEGORY_ATTACK) || (category == CATEGORY_DEFEND) || (category == CATEGORY_SUPPORT))
		return true;
	else
		return false;
}



bool CommandSupervisor::_IsItemCategorySelected() const {
	uint32 category = _category_options.GetSelection();
	if (category == CATEGORY_ITEM)
		return true;
	else
		return false;
}



GLOBAL_TARGET CommandSupervisor::_ActionTargetType() {
	if (_IsSkillCategorySelected() == true)
		return _skill_command.GetSelectedSkill()->GetTargetType();
	else if (_IsItemCategorySelected() == true)
		return _item_command.GetSelectedItem()->GetTargetType();
	else
		return GLOBAL_TARGET_INVALID;
}



void CommandSupervisor::_SetInitialTarget() {
	BattleActor* user = GetCommandCharacter();
	GLOBAL_TARGET target_type = _ActionTargetType();

	// Party targets are simple because we don't have to restore the last save target, since there is either the
	// ally party or foe party, and both parties are always valid targets (because otherwise the battle would have
	// already ended).
	if (IsTargetParty(target_type) == true) {
		// Party-type targets are always the same, so we don't need to recall the last target in this case
		_selected_target.SetInitialTarget(user, target_type);
		return;
	}

	// Retrieved the last saved target depending on the type (self/ally/foe)
	if (IsTargetSelf(target_type) == true) {
		_selected_target = _active_settings->GetLastSelfTarget();
	}
	else if (IsTargetAlly(target_type) == true) {
		_selected_target = _active_settings->GetLastCharacterTarget();
	}
	else if (IsTargetFoe(target_type) == true) {
		_selected_target = _active_settings->GetLastEnemyTarget();
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "no conditions met for invalid target type: " << target_type << endl;
	}

	// If the target type is invalid that means that there is no previous target so grab the initial target
	if (_selected_target.GetType() == GLOBAL_TARGET_INVALID) {
		_selected_target.SetInitialTarget(user, target_type);
	}
	// Otherwise if the last target is no longer valid, select the next valid target
	else if (_selected_target.IsValid() == false) {
		// Party targets should always be valid and attack points on actors do not disappear, so only the actor
		// must be invalid
		if (_selected_target.SelectNextActor(user) == false) {
			IF_PRINT_WARNING(BATTLE_DEBUG) << "no valid targets found" << endl;
		}
	}

	// This case occurs when our last target was an actor type and we're now using an action with a point targetl,
	// or vice versa. We need to modify the target type while still retaining the original target actor.
	if (_selected_target.GetType() != target_type) {
		if (IsTargetPoint(target_type) == true) {
			_selected_target.SetPointTarget(target_type, 0);
		}
		else { // then IsTargetActor(target_type) == true
			_selected_target.SetActorTarget(target_type, _selected_target.GetActor());
		}
	}
}



void CommandSupervisor::_ChangeState(COMMAND_STATE new_state) {
	if (_state == new_state) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "class was already in state to change to: " << new_state << endl;
		return;
	}

	if (new_state == COMMAND_STATE_INVALID) {
		_active_settings = NULL;
		_selected_skill = NULL;
		_selected_item = NULL;
	}
	else if (new_state == COMMAND_STATE_CATEGORY) {
		// Nothing to do here. The Initialize() function performs all necessary actions when entering this state.
	}
	else if (new_state == COMMAND_STATE_ACTION) {
		// Construct the appropriate skill or item selection list if we're coming from the category state
		if (_state == COMMAND_STATE_CATEGORY) {
			switch (_category_options.GetSelection()) {
				case CATEGORY_ATTACK:
					_skill_command.Initialize(GetCommandCharacter()->GetGlobalCharacter()->GetAttackSkills(), _active_settings->GetAttackList());
					break;
				case CATEGORY_DEFEND:
					_skill_command.Initialize(GetCommandCharacter()->GetGlobalCharacter()->GetDefenseSkills(), _active_settings->GetDefendList());
					break;
				case CATEGORY_SUPPORT:
					_skill_command.Initialize(GetCommandCharacter()->GetGlobalCharacter()->GetSupportSkills(), _active_settings->GetSupportList());
					break;
				case CATEGORY_ITEM:
					_item_command.Initialize(_active_settings->GetLastItem());
					break;
				default:
					IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid category selection: " << _category_options.GetSelection() << endl;
					_category_options.SetSelection(0);
					return;
			}
		}
	}
	else if (new_state == COMMAND_STATE_ACTOR) {
		// Set the initial target if we're coming from the action selection state
		if (_state == COMMAND_STATE_ACTION) {
			_SetInitialTarget();
		}

		_CreateActorTargetText();
	}
	else if (new_state == COMMAND_STATE_POINT) {
		_CreateAttackPointTargetText();
	}
	else if (new_state == COMMAND_STATE_INFORMATION) {
		_CreateInformationText();
	}

	_state = new_state;
}



void CommandSupervisor::_UpdateCategory() {
	_category_options.Update();

	// Event priority is given to the player requesting to abort the command selection process
	if (InputManager->CancelPress()) {
		// The only time we do not allow the player to abort the command menu is if they are running the battle with the "wait" setting active and the
		// current character is in the command state. Under these circumstances, the player has to enter a command for this character before the battle
		// is allowed to continue.
		if ((GlobalManager->GetBattleSetting() == GLOBAL_BATTLE_WAIT) && (GetCommandCharacter()->GetState() == ACTOR_STATE_COMMAND)) {
			// TODO: Play an approriate "invalid" type sound here
		}
		else {
			_ChangeState(COMMAND_STATE_INVALID);
			BattleMode::CurrentInstance()->NotifyCommandCancel();
			BattleMode::CurrentInstance()->GetMedia().cancel_sound.Play();
		}
	}

	else if (InputManager->ConfirmPress()) {
		if (_category_options.IsOptionEnabled(_category_options.GetSelection()) == true) {
			_active_settings->SetLastCategory(_category_options.GetSelection());
			_ChangeState(COMMAND_STATE_ACTION);
			BattleMode::CurrentInstance()->GetMedia().confirm_sound.Play();
		}
		else {
			BattleMode::CurrentInstance()->GetMedia().invalid_sound.Play();
		}
	}

	else if (InputManager->LeftPress()) {
		_category_options.InputLeft();
		BattleMode::CurrentInstance()->GetMedia().cursor_sound.Play();
	}

	else if (InputManager->RightPress()) {
		_category_options.InputRight();
		BattleMode::CurrentInstance()->GetMedia().cursor_sound.Play();
	}
}



void CommandSupervisor::_UpdateAction() {
	if (InputManager->CancelPress()) {
		_ChangeState(COMMAND_STATE_CATEGORY);
		BattleMode::CurrentInstance()->GetMedia().cancel_sound.Play();
		return;
	}

	if (_IsSkillCategorySelected() == true) {
		_selected_skill = _skill_command.GetSelectedSkill();

		if (InputManager->ConfirmPress()) {
            bool is_skill_enabled = _skill_command.GetSelectedSkillEnabled();
			if (is_skill_enabled == true) {
				_ChangeState(COMMAND_STATE_ACTOR);
				BattleMode::CurrentInstance()->GetMedia().confirm_sound.Play();
			}
			else {
				BattleMode::CurrentInstance()->GetMedia().invalid_sound.Play();
			}
		}

		else if (InputManager->MenuPress()) {
			_ChangeState(COMMAND_STATE_INFORMATION);
			BattleMode::CurrentInstance()->GetMedia().confirm_sound.Play();
		}

		else {
			_skill_command.UpdateList();
		}
	}
	else if (_IsItemCategorySelected() == true) {
		_selected_item = _item_command.GetSelectedItem();

		if (InputManager->ConfirmPress()) {
			if (_selected_item != NULL) {
				_ChangeState(COMMAND_STATE_ACTOR);
				BattleMode::CurrentInstance()->GetMedia().confirm_sound.Play();
			}
			else {
				BattleMode::CurrentInstance()->GetMedia().invalid_sound.Play();
			}
		}

		else if (InputManager->MenuPress()) {
			_ChangeState(COMMAND_STATE_INFORMATION);
			BattleMode::CurrentInstance()->GetMedia().confirm_sound.Play();
		}

		else {
			_item_command.UpdateList();
		}
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid category selection: " << _category_options.GetSelection() << endl;
		_state = COMMAND_STATE_CATEGORY;
		_category_options.SetSelection(0);
	}
}



void CommandSupervisor::_UpdateActorTarget() {
	if (InputManager->CancelPress()) {
		_ChangeState(COMMAND_STATE_ACTION);
		BattleMode::CurrentInstance()->GetMedia().cancel_sound.Play();
	}

	else if (InputManager->ConfirmPress()) {
		if (IsTargetPoint(_selected_target.GetType()) == true) {
			_ChangeState(COMMAND_STATE_POINT);
		}
		else {
			_FinalizeCommand();
		}
	}

	else if (InputManager->UpPress() || InputManager->DownPress()) {
		if (InputManager->DownPress())
			_target_options.InputDown();
		else
			_target_options.InputUp();

		if ((IsTargetActor(_selected_target.GetType()) == true) || (IsTargetPoint(_selected_target.GetType()) == true)) {
			_selected_target.SelectNextActor(GetCommandCharacter(), InputManager->UpPress());
			_CreateActorTargetText();
			BattleMode::CurrentInstance()->GetMedia().cursor_sound.Play();
		}
	}
}



void CommandSupervisor::_UpdateAttackPointTarget() {
	if (InputManager->CancelPress()) {
		_ChangeState(COMMAND_STATE_ACTOR);
		BattleMode::CurrentInstance()->GetMedia().cancel_sound.Play();
	}

	else if (InputManager->ConfirmPress()) {
		_FinalizeCommand();
	}

	else if (InputManager->UpPress() || InputManager->DownPress()) {
		if (InputManager->DownPress())
			_target_options.InputDown();
		else
			_target_options.InputUp();

		_selected_target.SelectNextPoint(GetCommandCharacter(), InputManager->DownPress());
		_CreateAttackPointTargetText();
		BattleMode::CurrentInstance()->GetMedia().cursor_sound.Play();
	}
}



void CommandSupervisor::_UpdateInformation() {
	if (InputManager->CancelPress() || InputManager->MenuPress()) {
		_state = COMMAND_STATE_ACTION;
		BattleMode::CurrentInstance()->GetMedia().cancel_sound.Play();
	}

	else if (InputManager->ConfirmPress()) {
		_ChangeState(COMMAND_STATE_ACTOR);
		BattleMode::CurrentInstance()->GetMedia().cancel_sound.Play();
	}

	// Change selected skill/item and update the information text
	else if (InputManager->UpPress() || InputManager->DownPress()) {

		if (_IsSkillCategorySelected() == true) {
			_skill_command.UpdateList();
			_selected_skill = _skill_command.GetSelectedSkill();
			BattleMode::CurrentInstance()->GetMedia().cursor_sound.Play();
		}
		else if (_IsItemCategorySelected() == true) {
			_item_command.UpdateList();
			_selected_item = _item_command.GetSelectedItem();
			BattleMode::CurrentInstance()->GetMedia().cursor_sound.Play();
		}

		_CreateInformationText();
	}

	else if (InputManager->RightPress() || InputManager->LeftPress()) {
		// TODO: toggle between description text and detailed stats
	}
}



void CommandSupervisor::_DrawCategory() {
	_category_options.Draw();
}



void CommandSupervisor::_DrawAction() {
	uint32 category_index = _category_options.GetSelection();

	// Draw the corresponding category icon and text to the left side of the window
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
	VideoManager->Move(570.0f, 75.0);
	_category_icons[category_index].Draw();
	VideoManager->MoveRelative(0.0f, -35.0f);
	_category_text[category_index].Draw();

	// Draw the header and list for either the skills or items to the right side of the window
	if (_IsSkillCategorySelected() == true) {
		_skill_command.DrawList();
	}
	else if (_IsItemCategorySelected() == true) {
		_item_command.DrawList();
	}
}



void CommandSupervisor::_DrawActorTarget() {
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
	VideoManager->Move(560.0f, 110.0f);
	_window_header.Draw();
	VideoManager->Move(560.0f, 85.0f);
	_window_text.Draw();

// 	_target_options.Draw();
	// TODO: draw relevant status/elemental icons
}



void CommandSupervisor::_DrawAttackPointTarget() {
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
	VideoManager->Move(560.0f, 110.0f);
	_window_header.Draw();
	VideoManager->Move(560.0f, 85.0f);
// 	_window_text.Draw();

	_target_options.Draw();
	// TODO: draw relevant status/elemental icons
}



void CommandSupervisor::_DrawInformation() {
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
	VideoManager->Move(560.0f, 110.0f);
	_window_header.Draw();
	VideoManager->Move(560.0f, 85.0f);
	_window_text.Draw();

	// TODO: draw relevant status/elemental icons
}



void CommandSupervisor::_CreateActorTargetText() {
	_window_header.SetText(UTranslate("Select Target"));

	_target_options.ClearOptions();
	if (IsTargetParty(_selected_target.GetType()) == true) {
		if (_selected_target.GetType() == GLOBAL_TARGET_ALL_ALLIES) {
			_target_options.AddOption(UTranslate("All Allies"));
		}
		else {
			_target_options.AddOption(UTranslate("All Enemies"));
		}
	}
	else if (IsTargetSelf(_selected_target.GetType()) == true) {
		_target_options.AddOption(_selected_target.GetActor()->GetName());
	}
	else if (IsTargetAlly(_selected_target.GetType()) == true) {
		for (uint32 i = 0; i < BattleMode::CurrentInstance()->GetCharacterActors().size(); i++) {
			_target_options.AddOption(BattleMode::CurrentInstance()->GetCharacterActors().at(i)->GetName());
			if (BattleMode::CurrentInstance()->GetCharacterActors().at(i)->IsAlive() == false) {
				_target_options.EnableOption(i, false);
			}
		}
	}
	else if (IsTargetFoe(_selected_target.GetType()) == true) {
		for (uint32 i = 0; i < BattleMode::CurrentInstance()->GetEnemyActors().size(); i++) {
			_target_options.AddOption(BattleMode::CurrentInstance()->GetEnemyActors().at(i)->GetName());
			if (BattleMode::CurrentInstance()->GetEnemyActors().at(i)->IsAlive() == false) {
				_target_options.EnableOption(i, false);
			}
		}
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << _selected_target.GetType() << endl;
	}

	// TEMP: remove once _target_options box works properly
	if (IsTargetParty(_selected_target.GetType()) == true) {
		if (_selected_target.GetType() == GLOBAL_TARGET_ALL_ALLIES) {
			_window_text.SetText(UTranslate("All Allies"));
		}
		else {
			_window_text.SetText(UTranslate("All Enemies"));
		}
	}
	else {
		_window_text.SetText(_selected_target.GetActor()->GetName());
	}

}



void CommandSupervisor::_CreateAttackPointTargetText() {
	_window_header.SetText(UTranslate("Select Attack Point"));

	BattleActor* actor = _selected_target.GetActor();
	uint32 selected_point = _selected_target.GetPoint();

	_target_options.ClearOptions();
	for (uint32 i = 0; i < actor->GetAttackPoints().size(); i++) {
		_target_options.AddOption(actor->GetAttackPoints().at(i)->GetName());
	}

	_target_options.SetSelection(selected_point);
}



void CommandSupervisor::_CreateInformationText() {
	ustring info_text;

	if (_IsSkillCategorySelected() == true) {
		_window_header.SetText(_selected_skill->GetName());

		info_text = UTranslate("Skill Points: " + NumberToString(_selected_skill->GetSPRequired())) + MakeUnicodeString("\n");
		info_text += UTranslate("Target Type: ") + MakeUnicodeString(GetTargetText(_selected_skill->GetTargetType())) + MakeUnicodeString("\n");
		info_text += UTranslate("Prep Time: ") + MakeUnicodeString(NumberToString(_selected_skill->GetWarmupTime())) + MakeUnicodeString("\n");
		info_text += UTranslate("Cool Time: ") + MakeUnicodeString(NumberToString(_selected_skill->GetCooldownTime())) + MakeUnicodeString("\n");
	}
	else if (_IsItemCategorySelected() == true) {
		_window_header.SetText(_selected_item->GetItem().GetName());

		info_text = UTranslate("Quantity: " + NumberToString(_selected_item->GetCount())) + MakeUnicodeString("\n");
		info_text += UTranslate("Target Type: ") + MakeUnicodeString(GetTargetText(_selected_item->GetItem().GetTargetType())) + MakeUnicodeString("\n");
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "unknown category selected: " << _category_options.GetSelection() << endl;
	}

	_window_text.SetText(info_text);
}



void CommandSupervisor::_FinalizeCommand() {
	BattleAction* new_action = NULL;
	BattleCharacter* character = GetCommandCharacter();

	_active_settings->SaveLastTarget(_selected_target);

	if (_IsSkillCategorySelected() == true) {
		new_action = new SkillAction(character, _selected_target, _selected_skill);
	}
	else if (_IsItemCategorySelected() == true) {
		new_action = new ItemAction(character, _selected_target, _selected_item);
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "did not create action for character, unknown category selected: " << _category_options.GetSelection() << endl;
	}
	character->SetAction(new_action);

	_ChangeState(COMMAND_STATE_INVALID);
	BattleMode::CurrentInstance()->NotifyCharacterCommandComplete(character);
	BattleMode::CurrentInstance()->GetMedia().finish_sound.Play();
}

} // namespace private_battle

} // namespace hoa_battle
