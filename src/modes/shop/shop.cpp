///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for shop mode interface
***
*** This code provides an interface for the user to purchase wares from a
*** merchant. This mode is usually entered from a map after discussing with a
*** shop keeper.
*** ***************************************************************************/

#include <iostream>

#include "defs.h"
#include "utils.h"

#include "audio.h"
#include "video.h"
#include "input.h"
#include "system.h"

#include "global.h"

#include "mode_manager.h"
#include "pause.h"

#include "shop.h"
#include "shop_root.h"
#include "shop_buy.h"
#include "shop_sell.h"
#include "shop_trade.h"
#include "shop_confirm.h"
#include "shop_leave.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_input;
using namespace hoa_system;
using namespace hoa_global;
using namespace hoa_mode_manager;
using namespace hoa_shop::private_shop;
using namespace hoa_pause;

namespace hoa_shop {

bool SHOP_DEBUG = false;
// Initialize static class variable
ShopMode* ShopMode::_current_instance = NULL;

namespace private_shop {

// *****************************************************************************
// ***** ShopMedia class methods
// *****************************************************************************

ShopMedia::ShopMedia() {
	if (_drunes_icon.Load("img/icons/drunes.png") == false)
		IF_PRINT_WARNING(SHOP_DEBUG) << "failed to load drunes icon image" << endl;

	if (_star_icon.Load("img/menus/star.png") == false) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "failed to load star icon image" << endl;
	}

	if (_check_icon.Load("img/menus/green_check.png") == false)
		IF_PRINT_WARNING(SHOP_DEBUG) << "failed to load check icon image" << endl;

	if (_x_icon.Load("img/menus/red_x.png") == false)
		IF_PRINT_WARNING(SHOP_DEBUG) << "failed to load x icon image" << endl;

	if (_socket_icon.Load("img/menus/socket.png") == false)
		IF_PRINT_WARNING(SHOP_DEBUG) << "failed to load socket icon image" << endl;

	if (_socket_icon.Load("img/menus/socket.png") == false)
		IF_PRINT_WARNING(SHOP_DEBUG) << "failed to load socket icon image" << endl;

	if (_equip_icon.Load("img/menus/equip.png") == false)
		IF_PRINT_WARNING(SHOP_DEBUG) << "failed to load equip icon image" << endl;

	if (ImageDescriptor::LoadMultiImageFromElementGrid(_elemental_icons, "img/icons/effects/elemental.png", 8, 9) == false) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "failed to load elemental icon images" << endl;
		return;
	}

	_sounds["confirm"] = new SoundDescriptor();
	_sounds["cancel"] = new SoundDescriptor();
	_sounds["coins"] = new SoundDescriptor();
	_sounds["bump"] = new SoundDescriptor();

	uint32 sound_load_failures = 0;
	if (_sounds["confirm"]->LoadAudio("snd/confirm.wav") == false)
		sound_load_failures++;
	if (_sounds["cancel"]->LoadAudio("snd/cancel.wav") == false)
		sound_load_failures++;
	if (_sounds["coins"]->LoadAudio("snd/coins.wav") == false)
		sound_load_failures++;
	if (_sounds["bump"]->LoadAudio("snd/bump.wav") == false)
		sound_load_failures++;

	if (sound_load_failures > 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "failed to load " << sound_load_failures << " sounds needed by shop mode" << endl;
	}
}



ShopMedia::~ShopMedia() {
	for (map<string, SoundDescriptor*>::iterator i = _sounds.begin(); i != _sounds.end(); i++)
		delete i->second;
	_sounds.clear();
}



void ShopMedia::Initialize() {
	_all_category_names.push_back(UTranslate("Items"));
	_all_category_names.push_back(UTranslate("Weapons"));
	_all_category_names.push_back(UTranslate("Head Armor"));
	_all_category_names.push_back(UTranslate("Torso Armor"));
	_all_category_names.push_back(UTranslate("Arm Armor"));
	_all_category_names.push_back(UTranslate("Leg Armor"));
	_all_category_names.push_back(UTranslate("Shards"));
	_all_category_names.push_back(UTranslate("Key Items"));
	_all_category_names.push_back(UTranslate("All Wares"));

	if (ImageDescriptor::LoadMultiImageFromElementGrid(_all_category_icons, "img/icons/object_category_icons.png", 3, 4) == false) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "failed to load object category icon images" << endl;
		return;
	}
	// The last three images in this multi image are blank, so they are removed
	_all_category_icons.pop_back();
	_all_category_icons.pop_back();
	_all_category_icons.pop_back();

	// Determine which categories are used in this shop and populate the true containers with that data
	uint8 deal_types = ShopMode::CurrentInstance()->GetDealTypes();
	uint8 bit_x = 0x01; // Used to do a bit-by-bit analysis of the obj_types variable
	for (uint8 i = 0; i < GLOBAL_OBJECT_TOTAL; i++, bit_x <<= 1) {
		// Check if the type is available by doing a bit-wise comparison
		if (deal_types & bit_x) {
			_sale_category_names.push_back(_all_category_names[i]);
			_sale_category_icons.push_back(_all_category_icons[i]);
		}
	}

	// If here is more than one category, add the text/icon for all wares
	if (_sale_category_names.size() > 1) {
		_sale_category_names.push_back(_all_category_names[8]);
		_sale_category_icons.push_back(_all_category_icons[8]);
	}

	// Grab the sprite frames for all characters in the active party
	vector<GlobalCharacter*>* characters = GlobalManager->GetCharacterOrder();
	for (uint32 i = 0; i < characters->size(); i++) {
		_character_sprites.push_back(characters->at(i)->GetStandardSpriteFrames()->at(0));
	}
}



ustring* ShopMedia::GetCategoryName(GLOBAL_OBJECT object_type) {
	uint32 index = 0;

	switch (object_type) {
		case GLOBAL_OBJECT_ITEM:
			index = 0;
			break;
		case GLOBAL_OBJECT_WEAPON:
			index = 1;
			break;
		case GLOBAL_OBJECT_HEAD_ARMOR:
			index = 2;
			break;
		case GLOBAL_OBJECT_TORSO_ARMOR:
			index = 3;
			break;
		case GLOBAL_OBJECT_ARM_ARMOR:
			index = 4;
			break;
		case GLOBAL_OBJECT_LEG_ARMOR:
			index = 5;
			break;
		case GLOBAL_OBJECT_SHARD:
			index = 6;
			break;
		case GLOBAL_OBJECT_KEY_ITEM:
			index = 7;
			break;
		case GLOBAL_OBJECT_TOTAL:
			index = 8;
			break;
		default:
			return NULL;
	}

	return &(_all_category_names[index]);
}



StillImage* ShopMedia::GetCategoryIcon(GLOBAL_OBJECT object_type) {
	uint32 index = 0;

	switch (object_type) {
		case GLOBAL_OBJECT_ITEM:
			index = 0;
			break;
		case GLOBAL_OBJECT_WEAPON:
			index = 1;
			break;
		case GLOBAL_OBJECT_HEAD_ARMOR:
			index = 2;
			break;
		case GLOBAL_OBJECT_TORSO_ARMOR:
			index = 3;
			break;
		case GLOBAL_OBJECT_ARM_ARMOR:
			index = 4;
			break;
		case GLOBAL_OBJECT_LEG_ARMOR:
			index = 5;
			break;
		case GLOBAL_OBJECT_SHARD:
			index = 6;
			break;
		case GLOBAL_OBJECT_KEY_ITEM:
			index = 7;
			break;
		case GLOBAL_OBJECT_TOTAL:
			index = 8;
			break;
		default:
			return NULL;
	}

	return &(_all_category_icons[index]);
}



StillImage* ShopMedia::GetElementalIcon(GLOBAL_ELEMENTAL element_type, GLOBAL_INTENSITY intensity) {
	const uint32 NUMBER_INTENSTIY_LEVELS = 9;

	// Row/col coordinates for where the specific icon can be found in the multi image array
	uint32 row = 0, col = 0;

	// Elemental type determines the icon's row
	switch (element_type) {
		case GLOBAL_ELEMENTAL_FIRE:
			row = 0;
			break;
		case GLOBAL_ELEMENTAL_WATER:
			row = 1;
			break;
		case GLOBAL_ELEMENTAL_VOLT:
			row = 2;
			break;
		case GLOBAL_ELEMENTAL_EARTH:
			row = 3;
			break;
		case GLOBAL_ELEMENTAL_SLICING:
			row = 4;
			break;
		case GLOBAL_ELEMENTAL_SMASHING:
			row = 5;
			break;
		case GLOBAL_ELEMENTAL_MAULING:
			row = 6;
			break;
		case GLOBAL_ELEMENTAL_PIERCING:
			row = 7;
			break;
		default:
			IF_PRINT_WARNING(SHOP_DEBUG) << "invalid elemental type: " << element_type << endl;
			return NULL;
	}

	// Intensity determines the icon's column
	switch (intensity) {
		case GLOBAL_INTENSITY_POS_EXTREME:
			col = 0;
			break;
		case GLOBAL_INTENSITY_POS_GREATER:
			col = 1;
			break;
		case GLOBAL_INTENSITY_POS_MODERATE:
			col = 2;
			break;
		case GLOBAL_INTENSITY_POS_LESSER:
			col = 3;
			break;
		case GLOBAL_INTENSITY_NEUTRAL:
			col = 4;
			break;
		case GLOBAL_INTENSITY_NEG_LESSER:
			col = 5;
			break;
		case GLOBAL_INTENSITY_NEG_MODERATE:
			col = 6;
			break;
		case GLOBAL_INTENSITY_NEG_GREATER:
			col = 7;
			break;
		case GLOBAL_INTENSITY_NEG_EXTREME:
			col = 8;
			break;
		default:
			IF_PRINT_WARNING(SHOP_DEBUG) << "invalid intensity level: " << intensity << endl;
			return NULL;
	}

	return &(_elemental_icons[(row * NUMBER_INTENSTIY_LEVELS) + col]);
}



StillImage* GetStatusIcon(GLOBAL_STATUS status_type, GLOBAL_INTENSITY intensity) {
	// TODO: implement this function once status effects are ready
	return NULL;
}



SoundDescriptor* ShopMedia::GetSound(string identifier) {
	map<string, SoundDescriptor*>::iterator sound = _sounds.find(identifier);
	if (sound != _sounds.end()) {
		return sound->second;
	}
	else {
		return NULL;
	}
}

// *****************************************************************************
// ***** ShopObjectViewer class methods
// *****************************************************************************

ShopObjectViewer::ShopObjectViewer() :
	_view_mode(SHOP_VIEW_MODE_LIST),
	_selected_object(NULL),
	_object_type(SHOP_OBJECT_INVALID),
	_map_usable(false),
	_battle_usable(false),
	_target_type_index(0)
{
	// Initialize all properties of class members that we can
	_object_name.SetStyle(TextStyle("title24"));

	// Position and dimensions for _description_text are set by _SetDescriptionText()
	_description_text.SetTextStyle(TextStyle("text20"));
	_description_text.SetDisplayMode(VIDEO_TEXT_INSTANT);
	_description_text.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_description_text.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_SetDescriptionText(); // Will set the position and dimensions of _description_text

	_lore_text.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());
	_lore_text.SetPosition(25.0f, 100.0f);
	_lore_text.SetDimensions(760.0f, 80.0f);
	_lore_text.SetTextStyle(TextStyle("text20"));
	_lore_text.SetDisplayMode(VIDEO_TEXT_INSTANT);
	_lore_text.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_lore_text.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);

	_field_use_header.SetStyle(TextStyle("text22"));
	_field_use_header.SetText(UTranslate("Field Use:"));
	_battle_use_header.SetStyle(TextStyle("text22"));
	_battle_use_header.SetText(UTranslate("Battle Use:"));
	_target_type_header.SetStyle(TextStyle("text22"));
	_target_type_header.SetText(UTranslate("Target:"));

	_target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_SELF_POINT), TextStyle("text22")));
	_target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_ALLY_POINT), TextStyle("text22")));
	_target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_FOE_POINT), TextStyle("text22")));
	_target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_SELF), TextStyle("text22")));
	_target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_ALLY), TextStyle("text22")));
	_target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_FOE), TextStyle("text22")));
	_target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_ALL_ALLIES), TextStyle("text22")));
	_target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_ALL_FOES), TextStyle("text22")));

	_phys_header.SetStyle(TextStyle("text22"));
	_phys_header.SetText(UTranslate("Phys:"));
	_meta_header.SetStyle(TextStyle("text22"));
	_meta_header.SetText(UTranslate("Meta:"));

	_phys_rating.SetStyle(TextStyle("text22"));
	_meta_rating.SetStyle(TextStyle("text22"));
	_socket_text.SetStyle(TextStyle("text22"));

	// Size elemental and status icon containers to the total number of avaiable elementals/status effects
	_elemental_icons.resize(GLOBAL_ELEMENTAL_TOTAL, StillImage());
	// TODO
// 	_status_icons.resize(GLOBAL_STATUS_TOTAL, StillImage());
}



void ShopObjectViewer::Initialize() {
	_description_text.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());

	_check_icon = *(ShopMode::CurrentInstance()->Media()->GetCheckIcon());
	_x_icon = *(ShopMode::CurrentInstance()->Media()->GetXIcon());
	_socket_icon = *(ShopMode::CurrentInstance()->Media()->GetSocketIcon());
	_equip_icon = *(ShopMode::CurrentInstance()->Media()->GetEquipIcon());

	uint32 number_character = ShopMode::CurrentInstance()->Media()->GetCharacterSprites()->size();

	for (uint32 i = 0; i < number_character; i++) {
		_character_sprites.push_back(ShopMode::CurrentInstance()->Media()->GetCharacterSprites()->at(i));
		_character_equipped.push_back(false);
		_phys_change_text.push_back(TextImage());
		_meta_change_text.push_back(TextImage());
	}
}



void ShopObjectViewer::Update() {
	_description_text.Update();
	_lore_text.Update();
}



void ShopObjectViewer::Draw() {
	if (_selected_object == NULL) {
		return;
	}

	// Set the initial draw cursor position to the top left corner of the proper window
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_CENTER, 0);
	if (_view_mode == SHOP_VIEW_MODE_LIST) {
		VideoManager->Move(135.0f, 188.0f);
	}
	else if (_view_mode == SHOP_VIEW_MODE_INFO) {
		VideoManager->Move(135.0f, 568.0f);
	}
	else { // An unknown/unsupported view mode is active, so draw nothing
		return;
	}

	// Object's name and icon are drawn in the same position for all objects
	_object_name.Draw();
	VideoManager->MoveRelative(0.0f, -55.0f);
	_selected_object->GetObject()->GetIconImage().Draw();

	switch (_object_type) {
		case SHOP_OBJECT_ITEM:
			_DrawItem();
			break;
		case SHOP_OBJECT_EQUIPMENT:
			_DrawEquipment();
			break;
		case SHOP_OBJECT_SHARD:
			_DrawShard();
			break;
		case SHOP_OBJECT_KEY_ITEM:
			_DrawKeyItem();
			break;
		default: // unknown/unsupported object type, draw no further information
			break;
	}

	// In the info view mode, description text and lore text is always drawn near the bottom of the middle window
	if (_view_mode == SHOP_VIEW_MODE_INFO) {
		_description_text.Draw();
		_lore_text.Draw();
	}
}



void ShopObjectViewer::SetSelectedObject(ShopObject* object) {
	if (object == NULL) {
		_selected_object = NULL;
		return;
	}

	if (_selected_object == object) {
		return;
	}

	_selected_object = object;
	_object_type = _selected_object->DetermineShopObjectType();

	// Get a pointer to the global object type of the new object selection
	switch (_object_type) {
		case SHOP_OBJECT_ITEM:
			_SetItemData();
			break;
		case SHOP_OBJECT_EQUIPMENT:
			_SetEquipmentData();
			break;
		case SHOP_OBJECT_SHARD:
			_SetShardData();
			break;
		case SHOP_OBJECT_KEY_ITEM:
			_SetDescriptionText();
			break;
		default:
			IF_PRINT_WARNING(SHOP_DEBUG) << "invalid object type: " << _object_type << endl;
			break;
	}

	_object_name.SetText(_selected_object->GetObject()->GetName());
	_description_text.SetDisplayText(_selected_object->GetObject()->GetDescription());
	// TODO: this data is not yet available in the global code
// 	_lore_text.SetDisplayText(_selected_object->GetObject()->GetLore());
} // void ShopObjectViewer::SetSelectedObject(ShopObject* object)



void ShopObjectViewer::ChangeViewMode(SHOP_VIEW_MODE new_mode) {
	if (_view_mode == new_mode) {
		return;
	}

	if (new_mode == SHOP_VIEW_MODE_LIST) {
		_view_mode = new_mode;
	}
	else if (new_mode == SHOP_VIEW_MODE_INFO) {
		_view_mode = new_mode;
	}
	else {
		IF_PRINT_WARNING(SHOP_DEBUG) << "unknown/unsupported view mode passed in function argument: " << new_mode << endl;
	}
	_SetDescriptionText(); // Necessary because description text must change its owner window
}



void ShopObjectViewer::_SetItemData() {
	if (_object_type != SHOP_OBJECT_ITEM) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function invoked when selected object was not an item: " << _object_type << endl;
		return;
	}

	// Ensure that the position of description text is correct
	_SetDescriptionText();

	// Set map/battle usability status
	GlobalItem* item = dynamic_cast<GlobalItem*>(_selected_object->GetObject());
	_map_usable = item->IsUsableInField();
	_battle_usable = item->IsUsableInBattle();

	// Determine the target type text to display for this item
	GLOBAL_TARGET target_type = item->GetTargetType();
	_target_type_index = static_cast<uint32>(target_type);
	if (_target_type_index >= _target_type_text.size()) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "unknown/invalid target type, defaulting to 'Self': " << target_type << endl;
		_target_type_index = 0;
	}
}



void ShopObjectViewer::_SetEquipmentData() {
	if (_object_type != SHOP_OBJECT_EQUIPMENT) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function invoked when selected object was not a piece of equipment: " << _object_type << endl;
		return;
	}

	// ---------- (1): Determine whether the selected object is a weapon or piece of armor
	GlobalWeapon* selected_weapon = NULL;
	GlobalArmor* selected_armor = NULL;
	uint32 usable_status = 0; // This is a bit mask that will hold the selected object's usablility information
	uint32 armor_index = 0; // Will hold the correct index into a GlobalCharacter object's equipped armor container

	if (_selected_object->GetObject()->GetObjectType() == GLOBAL_OBJECT_WEAPON) {
		selected_weapon = dynamic_cast<GlobalWeapon*>(_selected_object->GetObject());
		usable_status = selected_weapon->GetUsableBy();
	}
	else {
		selected_armor = dynamic_cast<GlobalArmor*>(_selected_object->GetObject());
		usable_status = selected_armor->GetUsableBy();

		// Armor on GlobalCharacter objects are stored in 4-element vectors. The different armor type maps to one of these four elements
		switch (selected_armor->GetObjectType()) {
			case GLOBAL_OBJECT_HEAD_ARMOR:
				armor_index = 0;
				break;
			case GLOBAL_OBJECT_TORSO_ARMOR:
				armor_index = 1;
				break;
			case GLOBAL_OBJECT_ARM_ARMOR:
				armor_index = 2;
				break;
			case GLOBAL_OBJECT_LEG_ARMOR:
				armor_index = 3;
				break;
			default:
				IF_PRINT_WARNING(SHOP_DEBUG) << "object type was not armor: " << _selected_object->GetObject()->GetObjectType() << endl;
				return;
		}
	}

	// ---------- (2): Determine equipment's rating, socket, elemental effects, and status effects to report

	if (selected_weapon != NULL) {
		_phys_rating.SetText(NumberToString(selected_weapon->GetPhysicalAttack()));
		_meta_rating.SetText(NumberToString(selected_weapon->GetMetaphysicalAttack()));
		_socket_text.SetText("x" + NumberToString(selected_weapon->GetSockets().size()));
		_SetElementalIcons(selected_weapon->GetElementalEffects());
		// TODO
		//_SetStatusIcons(selected_weapon->GetStatusEffects());
	}
	else if (selected_armor != NULL) {
		_phys_rating.SetText(NumberToString(selected_armor->GetPhysicalDefense()));
		_meta_rating.SetText(NumberToString(selected_armor->GetMetaphysicalDefense()));
		_socket_text.SetText("x" + NumberToString(selected_armor->GetSockets().size()));
		_SetElementalIcons(selected_armor->GetElementalEffects());
		// TODO
		//_SetStatusIcons(selected_weapon->GetStatusEffects());
	}

	// ---------- (3): For each character, determine if they already have the selection equipped or determine the change in pricing
	vector<GlobalCharacter*>* party = GlobalManager->GetCharacterOrder();
	GlobalCharacter* character = NULL;
	GlobalWeapon* equipped_weapon = NULL;
	GlobalArmor* equipped_armor = NULL;
	int32 phys_diff = 0, meta_diff = 0; // Holds the difference in attack power from equipped weapon/armor to selected weapon/armor

	// NOTE: In this block of code, entries to the _phys_change_text and _meta_change_text members are only modified if that information is to be
	// displayed for the character (meaning that the character can use the weapon/armor and does not already have it equipped). This means
	// that these two container members may contain stale data from previous objects. This is acceptable, however, as the stale data should
	// never be drawn. The stale data is allowed to remain so that we do not waste time re-rendering text for which we will not display.
	if (selected_weapon != NULL) {
		for (uint32 i = 0; i < party->size(); i++) {
			character = party->at(i);
			equipped_weapon = character->GetWeaponEquipped();

			// Initially assume that the character does not have this weapon equipped
			_character_equipped[i] = false;

			// Case 1: determine if the character can use the weapon and if not, move on to the next character
			// Toggle grayscale mode appropriately to indicate whether or not the character can equip this
			if (usable_status & (character->GetID())) {
				if (_character_sprites[i].IsGrayScale() == true)
					_character_sprites[i].DisableGrayScale();
			}
			else {
				if (_character_sprites[i].IsGrayScale() == false)
					_character_sprites[i].EnableGrayScale();
				continue;
			}
			// Case 2: if the player does not have any weapon equipped, the stat diff is equal to the selected weapon's ratings
			if (equipped_weapon == NULL) {
				phys_diff = static_cast<int32>(selected_weapon->GetPhysicalAttack());
				meta_diff = static_cast<int32>(selected_weapon->GetMetaphysicalAttack());
			}
			// Case 3: if the player already has this weapon equipped, indicate thus and move on to the next character
			if (selected_weapon->GetID() == equipped_weapon->GetID()) {
				_character_equipped[i] = true;
				continue;
			}
			// Case 4: the player can use this weapon and does not already have it equipped
			else {
				phys_diff = static_cast<int32>(selected_weapon->GetPhysicalAttack()) -
					static_cast<int32>(equipped_weapon->GetPhysicalAttack());
				meta_diff = static_cast<int32>(selected_weapon->GetMetaphysicalAttack()) -
					static_cast<int32>(equipped_weapon->GetMetaphysicalAttack());
			}

			// If this line has been reached, either case (2) or case (4) were evaluated as true. Render the phys/meta stat variation text
			_SetChangeText(i, phys_diff, meta_diff);
		}
	}
	else { // (selected_armor != NULL)
		for (uint32 i = 0; i < party->size(); i++) {
			character = party->at(i);
			equipped_armor = character->GetArmorEquipped().at(armor_index);

			// Initially assume that the character does not have this armor equipped
			_character_equipped[i] = false;

			// Case 1: determine if the character can use the armor and if not, move on to the next character
			// Toggle grayscale mode appropriately to indicate whether or not the character can equip this
			if (usable_status & (character->GetID())) {
				if (_character_sprites[i].IsGrayScale() == true)
					_character_sprites[i].DisableGrayScale();
			}
			else {
				if (_character_sprites[i].IsGrayScale() == false)
					_character_sprites[i].EnableGrayScale();
				continue;
			}
			// Case 2: if the player does not have any armor equipped, the stat diff is equal to the selected armor's ratings
			if (equipped_armor == NULL) {
				phys_diff = static_cast<int32>(selected_armor->GetPhysicalDefense());
				meta_diff = static_cast<int32>(selected_armor->GetMetaphysicalDefense());
			}
			// Case 3: if the player already has this armor equipped, indicate thus and move on to the next character
			if (selected_armor->GetID() == equipped_armor->GetID()) {
				_character_equipped[i] = true;
				continue;
			}
			// Case 4: the player can use this armor and does not already have it equipped
			else {
				phys_diff = static_cast<int32>(selected_armor->GetPhysicalDefense()) -
					static_cast<int32>(equipped_armor->GetPhysicalDefense());
				meta_diff = static_cast<int32>(selected_armor->GetMetaphysicalDefense()) -
					static_cast<int32>(equipped_armor->GetMetaphysicalDefense());
			}

			// If this line has been reached, either case (2) or case (4) were evaluated as true. Render the phys/meta stat variation text
			_SetChangeText(i, phys_diff, meta_diff);
		}
	}
} // void ShopObjectViewer::_SetEquipmentData()



void ShopObjectViewer::_SetShardData() {
	// TODO: implement when GlobalShard class is ready for use
}



void ShopObjectViewer::_SetDescriptionText() {
	if (_view_mode == SHOP_VIEW_MODE_LIST) {
		_description_text.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());
		// For key items, draw position is a little higher than other cases to center it in the blank area
		if (_object_type == SHOP_OBJECT_KEY_ITEM) {
			_description_text.SetPosition(102.0f, 76.0f);
		}
		else {
			_description_text.SetPosition(102.0f, 56.0f);
		}
		_description_text.SetDimensions(675.0f, 50.0f);
	}

	else if (_view_mode == SHOP_VIEW_MODE_INFO) {
		_description_text.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());
		_description_text.SetPosition(25.0f, 140.0f);
		_description_text.SetDimensions(750.0f, 50.0f);
	}

	else {
		IF_PRINT_WARNING(SHOP_DEBUG) << "unknown/unsupported view mode was active: " << _view_mode << endl;
	}
}



void ShopObjectViewer::_SetChangeText(uint32 index, int32 phys_diff, int32 meta_diff) {
	if (index >= _character_sprites.size()) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "index argument was out of bounds: " << index << endl;
		return;
	}

	_phys_change_text[index].Clear();
	if (phys_diff > 0) {
		_phys_change_text[index].SetStyle(TextStyle("text18", Color::green));
		_phys_change_text[index].SetText("+" + NumberToString(phys_diff));
	}
	else if (phys_diff < 0) {
		_phys_change_text[index].SetStyle(TextStyle("text18", Color::red));
		_phys_change_text[index].SetText(NumberToString(phys_diff));
	}
	else { // (phys_diff == 0)
		_phys_change_text[index].SetStyle(TextStyle("text18", Color::white));
		_phys_change_text[index].SetText(NumberToString(phys_diff));
	}

	_meta_change_text[index].Clear();
	if (meta_diff > 0) {
		_meta_change_text[index].SetStyle(TextStyle("text18", Color::green));
		_meta_change_text[index].SetText("+" + NumberToString(meta_diff));
	}
	else if (meta_diff < 0) {
		_meta_change_text[index].SetStyle(TextStyle("text18", Color::red));
		_meta_change_text[index].SetText(NumberToString(meta_diff));
	}
	else { // (meta_diff == 0)
		_meta_change_text[index].SetStyle(TextStyle("text18", Color::white));
		_meta_change_text[index].SetText(NumberToString(meta_diff));
	}
}



void ShopObjectViewer::_SetElementalIcons(const map<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>& elemental_effects) {
	uint32 index = 0;

	for (map<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>::const_iterator i = elemental_effects.begin(); i != elemental_effects.end(); i++) {
		switch (i->first) {
			case GLOBAL_ELEMENTAL_FIRE:
				index = 0;
				break;
			case GLOBAL_ELEMENTAL_WATER:
				index = 1;
				break;
			case GLOBAL_ELEMENTAL_VOLT:
				index = 2;
				break;
			case GLOBAL_ELEMENTAL_EARTH:
				index = 3;
				break;
			case GLOBAL_ELEMENTAL_SLICING:
				index = 4;
				break;
			case GLOBAL_ELEMENTAL_SMASHING:
				index = 5;
				break;
			case GLOBAL_ELEMENTAL_MAULING:
				index = 6;
				break;
			case GLOBAL_ELEMENTAL_PIERCING:
				index = 7;
				break;
			default:
				IF_PRINT_WARNING(SHOP_DEBUG) << "invalid elemental type: " << i->first << endl;
				break;
		}

		_elemental_icons[index] = *(ShopMode::CurrentInstance()->Media()->GetElementalIcon(i->first, i->second));
		if (i->second == GLOBAL_INTENSITY_NEUTRAL) {
			_elemental_icons[index].EnableGrayScale();
		}
	}
}



void ShopObjectViewer::_SetStatusIcons(const map<GLOBAL_STATUS, GLOBAL_INTENSITY>& status_effects) {
	// TODO: Implement this method when status effects are available.
	// It should work very much the same way as _SetElementalIcons()
}



void ShopObjectViewer::_DrawItem() {
	float move_offset = 0.0f; // Used to save image widths in determining relative movement
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_CENTER, 0);

	VideoManager->MoveRelative(80.0f, 15.0f);
	_field_use_header.Draw();
	move_offset = _field_use_header.GetWidth() + 5.0f; // 5.0f is a small buffer space between text and graphic
	VideoManager->MoveRelative(move_offset, 0.0f);
	if (_map_usable == true) {
		_check_icon.Draw();
	}
	else {
		_x_icon.Draw();
	}

	VideoManager->MoveRelative(175.0f - move_offset, 0.0f);
	_battle_use_header.Draw();
	move_offset = _battle_use_header.GetWidth() + 5.0f;
	VideoManager->MoveRelative(move_offset, 0.0f);
	if (_battle_usable == true) {
		_check_icon.Draw();
	}
	else {
		_x_icon.Draw();
	}

	VideoManager->MoveRelative(175.0f - move_offset, 0.0f);
	_target_type_header.Draw();
	move_offset = _target_type_header.GetWidth() + 5.0f;
	VideoManager->MoveRelative(move_offset, 0.0f);
	_target_type_text[_target_type_index].Draw();

	_description_text.Draw();
}



void ShopObjectViewer::_DrawEquipment() {
	VideoManager->MoveRelative(80.0f, 15.0f);
	_phys_header.Draw();
	VideoManager->MoveRelative(0.0f, -30.0f);
	_meta_header.Draw();

	VideoManager->SetDrawFlags(VIDEO_X_RIGHT, 0);
	VideoManager->MoveRelative(90.0f, 30.0f);
	_phys_rating.Draw();
	VideoManager->MoveRelative(0.0f, -30.0f);
		_meta_rating.Draw();

	VideoManager->SetDrawFlags(VIDEO_X_LEFT, 0);
	VideoManager->MoveRelative(20.0f, 15.0f);
	_socket_icon.Draw();
	VideoManager->MoveRelative(20.0f, 0.0f);
	_socket_text.Draw();

	VideoManager->SetDrawFlags(VIDEO_X_CENTER, 0);
	VideoManager->MoveRelative(50.0f, 55.0f);
	for (uint32 i = 0; i < GLOBAL_ELEMENTAL_TOTAL / 2; i++) {
		_elemental_icons[i].Draw();
		VideoManager->MoveRelative(0.0f, -25.0f);
	}
	VideoManager->MoveRelative(40.0f, 100.0f);
	for (uint32 i = GLOBAL_ELEMENTAL_TOTAL / 2; i < GLOBAL_ELEMENTAL_TOTAL; i++) {
		_elemental_icons[i].Draw();
		VideoManager->MoveRelative(0.0f, -25.0f);
	}

	// TODO: Draw two columns of status icons
	VideoManager->MoveRelative(80.0f, 0.0f); // TEMP: remove once commented code block below is added
// 		VideoManager->MoveRelative(40.0f, 100.0f);
// 		for (uint32 i = 0; i < 4; i++) {
// 			_status_icons[i].Draw();
// 			VideoManager->MoveRelative(0.0f, -25.0f);
// 		}
// 		VideoManager->MoveRelative(40.0f, 100.0f);
// 		for (uint32 i = 4; i < 8; i++) {
// 			_status_icons[i].Draw();
// 			VideoManager->MoveRelative(0.0f, -25.0f);
// 		}

	VideoManager->SetDrawFlags(VIDEO_Y_TOP, 0);
	if (_view_mode == SHOP_VIEW_MODE_LIST) {
		// In list view mode, draw the sprites to the right of the icons
		VideoManager->MoveRelative(60.0f, 115.0f);
	}
	else { // (_view_mode == SHOP_VIEW_MODE_INFO)
		// In info view mode, draw the spites centered on the screen in a row below the other equipment data
		VideoManager->Move(512.0f, 475.0f);
		float x_offset = -20.0f * _character_sprites.size();
		VideoManager->MoveRelative(x_offset, 0.0f);
	}
	for (uint32 i = 0; i < _character_sprites.size(); i++) {
		// In list mode, there's only enough room to show 8 sprites
		if ((_view_mode == SHOP_VIEW_MODE_LIST) && (i >= 8)) {
			break;
		}

		_character_sprites[i].Draw();

		// Case 1: Draw the equip icon below the character sprite
		if (_character_equipped[i] == true) {
			VideoManager->MoveRelative(0.0f, -78.0f);
			_equip_icon.Draw();
			VideoManager->MoveRelative(0.0f, 78.0f);
		}
		// Case 2: Draw the phys/meta change text below the sprite
		else if (_character_sprites[i].IsGrayScale() == false) {
			VideoManager->MoveRelative(0.0f, -65.0f);
			_phys_change_text[i].Draw();
			VideoManager->MoveRelative(0.0f, -20.0f);
			_meta_change_text[i].Draw();
			VideoManager->MoveRelative(0.0f, 85.0f);
		}
		// Case 3: Nothing needs to be drawn below the sprite
		VideoManager->MoveRelative(40.0f, 0.0f);
	}
} // void ShopObjectViewer::_DrawEquipment()



void ShopObjectViewer::_DrawShard() {
	// TODO: implement when GlobalShard class is ready for use
}



void ShopObjectViewer::_DrawKeyItem() {
	_description_text.Draw();
}


} // namespace private_shop

// *****************************************************************************
// ***** ShopMode class methods
// *****************************************************************************

ShopMode::ShopMode() :
	_initialized(false),
	_state(SHOP_STATE_ROOT),
	_deal_types(0),
	_buy_price_level(SHOP_PRICE_STANDARD),
	_sell_price_level(SHOP_PRICE_STANDARD),
	_total_costs(0),
	_total_sales(0),
	_shop_media(NULL),
	_object_viewer(NULL),
	_root_interface(NULL),
	_buy_interface(NULL),
	_sell_interface(NULL),
	_trade_interface(NULL),
	_confirm_interface(NULL)
{
	mode_type = MODE_MANAGER_SHOP_MODE;
	_current_instance = this;

	// ---------- (1): Create the menu windows and set their properties
	_top_window.Create(800.0f, 96.0f, ~VIDEO_MENU_EDGE_BOTTOM);
	_top_window.SetPosition(112.0f, 684.0f);
	_top_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_top_window.SetDisplayMode(VIDEO_MENU_INSTANT);
	_top_window.Show();

	_middle_window.Create(800.0f, 400.0f, VIDEO_MENU_EDGE_ALL, VIDEO_MENU_EDGE_TOP | VIDEO_MENU_EDGE_BOTTOM);
	_middle_window.SetPosition(112.0f, 604.0f);
	_middle_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_middle_window.SetDisplayMode(VIDEO_MENU_INSTANT);
	_middle_window.Show();

	_bottom_window.Create(800.0f, 140.0f, ~VIDEO_MENU_EDGE_TOP);
	_bottom_window.SetPosition(112.0f, 224.0f);
	_bottom_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_bottom_window.SetDisplayMode(VIDEO_MENU_INSTANT);
	_bottom_window.Show();

	// (2) Create the list of shop actions
	_action_options.SetOwner(&_top_window);
	_action_options.SetPosition(80.0f, 90.0f);
	_action_options.SetDimensions(640.0f, 30.0f, 4, 1, 4, 1);
	_action_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
	_action_options.SetTextStyle(TextStyle("title28"));
	_action_options.SetSelectMode(VIDEO_SELECT_SINGLE);
	_action_options.SetCursorOffset(-55.0f, 30.0f);
	_action_options.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);

	vector<ustring> option_text;
	option_text.push_back(UTranslate("Buy"));
	option_text.push_back(UTranslate("Sell"));
	option_text.push_back(UTranslate("Trade"));
	option_text.push_back(UTranslate("Confirm"));
	_action_options.SetOptions(option_text);
	_action_options.SetSelection(0);

	_action_titles.push_back(TextImage(option_text[0], TextStyle("title28")));
	_action_titles.push_back(TextImage(option_text[1], TextStyle("title28")));
	_action_titles.push_back(TextImage(option_text[2], TextStyle("title28")));
	_action_titles.push_back(TextImage(option_text[3], TextStyle("title28")));
	_action_titles.push_back(TextImage(UTranslate("Leave"), TextStyle("title28")));

	// (3) Create the financial table text
	_finance_table.SetOwner(&_top_window);
	_finance_table.SetPosition(80.0f, 45.0f);
	_finance_table.SetDimensions(640.0f, 20.0f, 4, 1, 4, 1);
	_finance_table.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_finance_table.SetTextStyle(TextStyle("text22"));
	_finance_table.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	// Initialize all four options with an empty string that will be overwritten by the following method call
	for (uint32 i = 0; i < 4; i++)
		_finance_table.AddOption(ustring());
	UpdateFinances(0, 0);

	_shop_media = new ShopMedia();
	_object_viewer = new ShopObjectViewer();
	_root_interface = new RootInterface();
	_buy_interface = new BuyInterface();
	_sell_interface = new SellInterface();
	_trade_interface = new TradeInterface();
	_confirm_interface = new ConfirmInterface();
	_leave_interface = new LeaveInterface();

	try {
		_screen_backdrop = VideoManager->CaptureScreen();
	}
	catch (Exception e) {
		IF_PRINT_WARNING(SHOP_DEBUG) << e.ToString() << endl;
	}
} // ShopMode::ShopMode()



ShopMode::~ShopMode() {
	for (uint32 i = 0; i < _created_objects.size(); i++) {
		delete(_created_objects[i]);
	}
	_created_objects.clear();

	delete _shop_media;
	delete _object_viewer;
	delete _root_interface;
	delete _buy_interface;
	delete _sell_interface;
	delete _trade_interface;
	delete _confirm_interface;
	delete _leave_interface;

	_top_window.Destroy();
	_middle_window.Destroy();
	_bottom_window.Destroy();

	if (_current_instance == this) {
		_current_instance = NULL;
	}
}



void ShopMode::Reset() {
	VideoManager->SetCoordSys(0.0f, 1023.0f, 0.0f, 767.0f);
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);

	_current_instance = this;
	if (IsInitialized() == false)
		Initialize();
}



void ShopMode::Initialize() {
	if (IsInitialized() == true) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "shop was already initialized previously" << endl;
		return;
	}

	_initialized = true;

	// ---------- (1): Determine what types of objects the shop deals in based on the managed object list
	for (uint32 i = 0; i < _created_objects.size(); i++) {
		switch (_created_objects[i]->GetObjectType()) {
			case GLOBAL_OBJECT_ITEM:
				_deal_types |= DEALS_ITEMS;
				break;
			case GLOBAL_OBJECT_WEAPON:
				_deal_types |= DEALS_WEAPONS;
				break;
			case GLOBAL_OBJECT_HEAD_ARMOR:
				_deal_types |= DEALS_HEAD_ARMOR;
				break;
			case GLOBAL_OBJECT_TORSO_ARMOR:
				_deal_types |= DEALS_TORSO_ARMOR;
				break;
			case GLOBAL_OBJECT_ARM_ARMOR:
				_deal_types |= DEALS_ARM_ARMOR;
				break;
			case GLOBAL_OBJECT_LEG_ARMOR:
				_deal_types |= DEALS_LEG_ARMOR;
				break;
			case GLOBAL_OBJECT_SHARD:
				_deal_types |= DEALS_SHARDS;
				break;
			case GLOBAL_OBJECT_KEY_ITEM:
				_deal_types |= DEALS_KEY_ITEMS;
				break;
			default:
				IF_PRINT_WARNING(SHOP_DEBUG) << "unknown object type sold in shop: " << _created_objects[i]->GetObjectType() << endl;
				break;
		}
	}

	// ---------- (2): Add objects from the player's inventory to the list of shop objects
	map<uint32, GlobalObject*>* inventory = GlobalManager->GetInventory();
	for (map<uint32, GlobalObject*>::iterator i = inventory->begin(); i != inventory->end(); i++) {
		// Check if the object already exists in the shop list and if so, set its ownership count
		map<uint32, ShopObject>::iterator shop_obj_iter = _shop_objects.find(i->second->GetID());
		if (shop_obj_iter != _shop_objects.end()) {
			shop_obj_iter->second.IncrementOwnCount(i->second->GetCount());
		}
		// Otherwise, add the shop object to the list
		else {
			ShopObject new_shop_object(i->second, false);
			new_shop_object.IncrementOwnCount(i->second->GetCount());
			_shop_objects.insert(make_pair(i->second->GetID(), new_shop_object));
		}
	}

	// ---------- (3): Initialize pricing for all shop objects
	for (map<uint32, ShopObject>::iterator i = _shop_objects.begin(); i != _shop_objects.end(); i++) {
		i->second.SetPricing(_buy_price_level, _sell_price_level);
	}

	// ---------- (4): Initialize multimedia data and viewer
	_shop_media->Initialize();
	_object_viewer->Initialize();

	// ---------- (5): Initialize all shop interfaces
	_root_interface->Initialize();
	_buy_interface->Initialize();
	_sell_interface->Initialize();
	_trade_interface->Initialize();
	_confirm_interface->Initialize();
	_leave_interface->Initialize();
} // void ShopMode::Initialize()



void ShopMode::Update() {
	// Pause and quit events have highest priority. If either type of event is detected, no other update processing will be done
	if (InputManager->QuitPress() == true) {
		ModeManager->Push(new PauseMode(true));
		return;
	}
	else if (InputManager->PausePress() == true) {
		ModeManager->Push(new PauseMode(false));
		return;
	}

	// When the state is at the root interface ,ShopMode needs to process user input and possibly change state
	if (_state == SHOP_STATE_ROOT) {
		SoundDescriptor* sound = NULL; // Used to hold pointers of sound objects to play

		if (InputManager->ConfirmPress()) {
			if (_action_options.GetSelection() < 0 || _action_options.GetSelection() > 4) {
				IF_PRINT_WARNING(SHOP_DEBUG) << "invalid selection in action window: " << _action_options.GetSelection() << endl;
				_action_options.SetSelection(0);
				return;
			}

			_action_options.InputConfirm();
			sound = ShopMode::CurrentInstance()->Media()->GetSound("confirm");
			assert(sound != NULL);
			sound->Play();

			if (_action_options.GetSelection() == 0) { // Buy
				ChangeState(SHOP_STATE_BUY);
			}
			else if (_action_options.GetSelection() == 1) { // Sell
				ChangeState(SHOP_STATE_SELL);
			}
			else if (_action_options.GetSelection() == 2) { // Trade
				ChangeState(SHOP_STATE_TRADE);
			}
			else if (_action_options.GetSelection() == 3) { // Confirm
				ChangeState(SHOP_STATE_CONFIRM);
			}
		}
		else if (InputManager->CancelPress()) {
			ChangeState(SHOP_STATE_LEAVE);
		}
		else if (InputManager->LeftPress()) {
			_action_options.InputLeft();
		}
		else if (InputManager->RightPress()) {
			_action_options.InputRight();
		}
		_action_options.Update();

		_root_interface->Update();
	} // if (_state == SHOP_STATE_ROOT)
	else {
		// Update the active interface
		switch (_state) {
			case SHOP_STATE_BUY:
				_buy_interface->Update();
				break;
			case SHOP_STATE_SELL:
				_sell_interface->Update();
				break;
			case SHOP_STATE_TRADE:
				_trade_interface->Update();
				break;
			case SHOP_STATE_CONFIRM:
				_confirm_interface->Update();
				break;
			case SHOP_STATE_LEAVE:
				_leave_interface->Update();
				break;
			default:
				IF_PRINT_WARNING(SHOP_DEBUG) << "invalid shop state: " << _state << ", reseting to root state" << endl;
				_state = SHOP_STATE_ROOT;
				break;
		} // switch (_state)
	}
} // void ShopMode::Update()



void ShopMode::Draw() {
	// ---------- (1): Draw the background image. Set the system coordinates to the size of the window (same as the screen backdrop)
	VideoManager->SetCoordSys(0.0f, static_cast<float>(VideoManager->GetScreenWidth()), 0.0f, static_cast<float>(VideoManager->GetScreenHeight()));
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
	VideoManager->Move(0.0f, 0.0f);
	_screen_backdrop.Draw();

	// ---------- (2): Draw all menu windows
	VideoManager->SetCoordSys(0.0f, 1024.0f, 0.0f, 768.0f); // Restore the standard shop coordinate system before drawing the shop windows
	_top_window.Draw();
	_bottom_window.Draw();
	_middle_window.Draw(); // Drawn last because the middle window has the middle upper and lower window borders attached

	// ---------- (3): Draw the contents of the top window
	VideoManager->Move(130.0f, 605.0f);
	ShopMode::CurrentInstance()->Media()->GetDrunesIcon()->Draw();
	VideoManager->MoveRelative(705.0f, 0.0f);
	ShopMode::CurrentInstance()->Media()->GetDrunesIcon()->Draw();

	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
	VideoManager->Move(512.0f, 657.0f);
	switch (_state) {
		case SHOP_STATE_ROOT:
			_action_options.Draw();
			break;
		case SHOP_STATE_BUY:
			_action_titles[0].Draw();
			break;
		case SHOP_STATE_SELL:
			_action_titles[1].Draw();
			break;
		case SHOP_STATE_TRADE:
			_action_titles[2].Draw();
			break;
		case SHOP_STATE_CONFIRM:
			_action_titles[3].Draw();
			break;
		case SHOP_STATE_LEAVE:
			_action_titles[4].Draw();
			break;
		default:
			IF_PRINT_WARNING(SHOP_DEBUG) << "invalid shop state: " << _state << endl;
			break;
	}

	// TODO: This method isn't working correctly (know idea why these coordinates work). When the call is fixed, the args should be updated
	VideoManager->DrawLine(-315.0f, -20.0f, 315.0f, -20.0f, 1.0f, Color::white);

	_finance_table.Draw();

	// ---------- (4): Call the draw function on the active interface to fill the contents of the other two windows
	switch (_state) {
		case SHOP_STATE_ROOT:
			_root_interface->Draw();
			break;
		case SHOP_STATE_BUY:
			_buy_interface->Draw();
			break;
		case SHOP_STATE_SELL:
			_sell_interface->Draw();
			break;
		case SHOP_STATE_TRADE:
			_trade_interface->Draw();
			break;
		case SHOP_STATE_CONFIRM:
			_confirm_interface->Draw();
			break;
		case SHOP_STATE_LEAVE:
			_leave_interface->Draw();
			break;
		default:
			IF_PRINT_WARNING(SHOP_DEBUG) << "invalid shop state: " << _state << endl;
			break;
	}
} // void ShopMode::Draw()



void ShopMode::AddObjectToBuyList(ShopObject* object) {
	if (object == NULL) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function was passed a NULL argument" << endl;
		return;
	}

	if (object->GetBuyCount() == 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "object to be added had a buy count of zero" << endl;
	}

	uint32 object_id = object->GetObject()->GetID();
	pair<map<uint32, ShopObject*>::iterator, bool> ret_val;
	ret_val = _buy_list.insert(make_pair(object_id, object));
	if (ret_val.second == false) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "object to be added already existed in buy list" << endl;
	}
}



void ShopMode::RemoveObjectFromBuyList(ShopObject* object) {
	if (object == NULL) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function was passed a NULL argument" << endl;
		return;
	}

	if (object->GetBuyCount() > 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "object to be removed had a buy count that was non-zero" << endl;
	}

	uint32 object_id = object->GetObject()->GetID();
	map<uint32, ShopObject*>::iterator object_entry = _buy_list.find(object_id);
	if (object_entry == _buy_list.end()) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "object to be removed did not exist on the buy list" << endl;
	}
	else {
		_buy_list.erase(object_entry);
	}
}



void ShopMode::AddObjectToSellList(ShopObject* object) {
	if (object == NULL) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function was passed a NULL argument" << endl;
		return;
	}

	if (object->GetSellCount() == 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "object to be added had a sell count of zero" << endl;
	}

	uint32 object_id = object->GetObject()->GetID();
	pair<map<uint32, ShopObject*>::iterator, bool> ret_val;
	ret_val = _sell_list.insert(make_pair(object_id, object));
	if (ret_val.second == false) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "object to be added already existed in sell list" << endl;
	}
}



void ShopMode::RemoveObjectFromSellList(ShopObject* object) {
	if (object == NULL) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function was passed a NULL argument" << endl;
		return;
	}

	if (object->GetSellCount() > 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "object to be removed had a sell count that was non-zero" << endl;
	}

	uint32 object_id = object->GetObject()->GetID();
	map<uint32, ShopObject*>::iterator object_entry = _sell_list.find(object_id);
	if (object_entry == _sell_list.end()) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "object to be removed did not exist on the sell list" << endl;
	}
	else {
		_sell_list.erase(object_entry);
	}
}



void ShopMode::ClearOrder() {
	for (map<uint32, ShopObject*>::iterator i = _buy_list.begin(); i != _buy_list.end(); i++)
		i->second->ResetBuyCount();
	for (map<uint32, ShopObject*>::iterator i = _sell_list.begin(); i != _sell_list.end(); i++)
		i->second->ResetSellCount();

	_buy_list.clear();
	_sell_list.clear();

	_total_costs = 0;
	_total_sales = 0;
	UpdateFinances(0, 0);
}



void ShopMode::CompleteTransaction() {
	uint32 count = 0;
	uint32 id = 0;

	// Add all objects on the buy list to inventory and update shop object status
	for (map<uint32, ShopObject*>::iterator i = _buy_list.begin(); i != _buy_list.end(); i++) {
		count = i->second->GetBuyCount();
		id = i->second->GetObject()->GetID();

		// The player may have reduced the buy count to zero in the confirm interface before completing the transaction
		// We simply ignore any objects on the buy list with this condition
		if (count == 0)
			continue;

		i->second->ResetBuyCount();
		i->second->IncrementOwnCount(count);
		i->second->DecrementStockCount(count);
		GlobalManager->AddToInventory(id, count);
	}
	_buy_list.clear();

	// Remove all objects on the sell list from the inventory and update shop object status
	for (map<uint32, ShopObject*>::iterator i = _sell_list.begin(); i != _sell_list.end(); i++) {
		count = i->second->GetSellCount();
		id = i->second->GetObject()->GetID();

		if (count == 0)
			continue;

		i->second->ResetSellCount();
		i->second->DecrementOwnCount(count);
		GlobalManager->DecrementObjectCount(id, count);

		// When all owned instances of this object have been sold off, the object is automatically removed
		// from the player's inventory. If the object is not sold in the shop, this means it must be removed
		// from all shop object containers as the object data (GlobalObject pointer) is now invalid.
		if ((i->second->GetOwnCount() == 0) && (i->second->IsSoldInShop() == false)) {
			RemoveObject(id);
		}
	}
	_sell_list.clear();

	// Update the player's drune count by subtracting costs and adding revenue and update the shop's financial display
	GlobalManager->AddDrunes(_total_sales);
	GlobalManager->SubtractDrunes(_total_costs);
	_total_costs = 0;
	_total_sales = 0;
	UpdateFinances(0, 0);

	// Notify all interfaces that a transaction has just been completed
	_root_interface->TransactionNotification();
	_buy_interface->TransactionNotification();
	_sell_interface->TransactionNotification();
	_trade_interface->TransactionNotification();
	_confirm_interface->TransactionNotification();
	_leave_interface->TransactionNotification();
} // void ShopMode::CompleteTransaction()



void ShopMode::UpdateFinances(int32 costs_amount, int32 sales_amount) {
	int32 updated_costs = _total_costs + costs_amount;
	int32 updated_sales = _total_sales + sales_amount;

	if (updated_costs < 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "updated amount causes costs to become negative: " << costs_amount << endl;
		return;
	}
	if (updated_sales < 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "updated amount causes sales to become negative: " << sales_amount << endl;
		return;
	}
	if ((static_cast<int32>(GlobalManager->GetDrunes()) + updated_sales - updated_costs) < 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "updated costs and sales values cause negative balance: " << costs_amount << ", " << sales_amount << endl;
		return;
	}

	_total_costs = static_cast<uint32>(updated_costs);
	_total_sales = static_cast<uint32>(updated_sales);

	_finance_table.SetOptionText(0, UTranslate("Funds: ") + MakeUnicodeString(NumberToString(GlobalManager->GetDrunes())));
	_finance_table.SetOptionText(1, UTranslate("Purchases: -") + MakeUnicodeString(NumberToString(_total_costs)));
	_finance_table.SetOptionText(2, UTranslate("Sales: +") + MakeUnicodeString(NumberToString(_total_sales)));
	_finance_table.SetOptionText(3, UTranslate("Total: ") + MakeUnicodeString(NumberToString(GetTotalRemaining())));
}



void ShopMode::ChangeState(SHOP_STATE new_state) {
	if (_state == new_state) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "shop was already in the state to change to: " << _state << endl;
		return;
	}

	_state = new_state;

	// When state changes to the leave state, leave immediately if there are no marked purchases, sales, or trades
	if (_state == SHOP_STATE_LEAVE) {
		if ((GetTotalCosts() == 0) && (GetTotalSales() == 0)) {
			ModeManager->Pop();
			return;
		}
	}

	switch (_state) {
		case SHOP_STATE_ROOT:
			_root_interface->MakeActive();
			break;
		case SHOP_STATE_BUY:
			_buy_interface->MakeActive();
			break;
		case SHOP_STATE_SELL:
			_sell_interface->MakeActive();
			break;
		case SHOP_STATE_TRADE:
			_trade_interface->MakeActive();
			break;
		case SHOP_STATE_CONFIRM:
			_confirm_interface->MakeActive();
			break;
		case SHOP_STATE_LEAVE:
			_leave_interface->MakeActive();
			break;
		default:
			IF_PRINT_WARNING(SHOP_DEBUG) << "invalid shop state: " << _state << endl;
			break;
	}
}



void ShopMode::SetShopName(ustring name) {
	if (IsInitialized() == true) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function called after shop was already initialized" << endl;
		return;
	}

	_root_interface->SetShopName(name);
}



void ShopMode::SetGreetingText(ustring greeting) {
	if (IsInitialized() == true) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function called after shop was already initialized" << endl;
		return;
	}

	_root_interface->SetGreetingText(greeting);
}



void ShopMode::SetPriceLevels(SHOP_PRICE_LEVEL buy_level, SHOP_PRICE_LEVEL sell_level) {
	if (IsInitialized() == true) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function called after shop was already initialized" << endl;
		return;
	}

	_buy_price_level = buy_level;
	_sell_price_level = sell_level;
}



void ShopMode::AddObject(uint32 object_id, uint32 stock) {
	if (IsInitialized() == true) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "function called after shop was already initialized" << endl;
		return;
	}

	if (object_id == private_global::OBJECT_ID_INVALID || object_id >= private_global::OBJECT_ID_EXCEEDS) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "attempted to add object with invalid id: " << object_id << endl;
		return;
	}

	if (_shop_objects.find(object_id) != _shop_objects.end()) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "attempted to add object that already existed: " << object_id << endl;
		return;
	}

	GlobalObject* new_object = GlobalCreateNewObject(object_id, 1);
	if (new_object != NULL)
	{
		_created_objects.push_back(new_object);
		ShopObject new_shop_object(new_object, true);
		new_shop_object.IncrementStockCount(stock);
		_shop_objects.insert(make_pair(object_id, new_shop_object));
	}
}



void ShopMode::RemoveObject(uint32 object_id) {
	map<uint32, ShopObject>::iterator shop_iter = _shop_objects.find(object_id);
	if (shop_iter == _shop_objects.end()) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "attempted to remove object that did not exist: " << object_id << endl;
		return;
	}

	if (shop_iter->second.IsSoldInShop() == true) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "tried to remove object that is sold in shop: " << object_id << endl;
		return;
	}

	if (shop_iter->second.GetOwnCount() != 0) {
		IF_PRINT_WARNING(SHOP_DEBUG) << "object's ownership count was non-zero: " << object_id << endl;
		return;
	}

	_shop_objects.erase(shop_iter);
}

} // namespace hoa_shop
