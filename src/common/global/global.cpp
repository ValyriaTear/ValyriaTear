////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for the global game manager
*** ***************************************************************************/

#include "engine/system.h"
#include "global.h"

using namespace hoa_utils;

using namespace hoa_video;
using namespace hoa_script;
using namespace hoa_system;

template<> hoa_global::GameGlobal* Singleton<hoa_global::GameGlobal>::_singleton_reference = NULL;

namespace hoa_global {

using namespace private_global;

GameGlobal* GlobalManager = NULL;
bool GLOBAL_DEBUG = false;

////////////////////////////////////////////////////////////////////////////////
// GlobalEventGroup class
////////////////////////////////////////////////////////////////////////////////

void GlobalEventGroup::AddNewEvent(const std::string& event_name, int32 event_value) {
	if (DoesEventExist(event_name) == true) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "an event with the desired name \"" << event_name << "\" already existed in this group: "
			<< _group_name << std::endl;
		return;
	}
	_events.insert(std::make_pair(event_name, event_value));
}



int32 GlobalEventGroup::GetEvent(const std::string& event_name) {
	std::map<std::string, int32>::iterator event_iter = _events.find(event_name);
	if (event_iter == _events.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "an event with the specified name \"" << event_name << "\" did not exist in this group: "
			<< _group_name << std::endl;
		return GLOBAL_BAD_EVENT;
	}
	return event_iter->second;
}



void GlobalEventGroup::SetEvent(const std::string& event_name, int32 event_value) {
	std::map<std::string, int32>::iterator event_iter = _events.find(event_name);
	if (event_iter == _events.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "the event with the specified name \"" << event_name << "\" did not exist in this group: "
			<< _group_name << std::endl;
		return;
	}
	event_iter->second = event_value;
}

////////////////////////////////////////////////////////////////////////////////
// GameGlobal class - Initialization and Destruction
////////////////////////////////////////////////////////////////////////////////

GameGlobal::GameGlobal() :
	_game_slot_id(0),
	_drunes(0),
	_x_save_map_position(0),
	_y_save_map_position(0),
	_same_map_hud_name_as_previous(false),
	_battle_setting(GLOBAL_BATTLE_INVALID) {
	IF_PRINT_DEBUG(GLOBAL_DEBUG) << "GameGlobal constructor invoked" << std::endl;
}



GameGlobal::~GameGlobal() {
	IF_PRINT_DEBUG(GLOBAL_DEBUG) << "GameGlobal destructor invoked" << std::endl;

	ClearAllData();

	// Close all persistent script files
	_global_script.CloseFile();

	_items_script.CloseTable();
	_items_script.CloseFile();

	_key_items_script.CloseTable();
	_key_items_script.CloseFile();

	_weapons_script.CloseTable();
	_weapons_script.CloseFile();

	_head_armor_script.CloseTable();
	_head_armor_script.CloseFile();

	_torso_armor_script.CloseTable();
	_torso_armor_script.CloseFile();

	_arm_armor_script.CloseTable();
	_arm_armor_script.CloseFile();

	_leg_armor_script.CloseTable();
	_leg_armor_script.CloseFile();

	_attack_skills_script.CloseTable();
	_attack_skills_script.CloseFile();

	_defend_skills_script.CloseTable();
	_defend_skills_script.CloseFile();

	_support_skills_script.CloseTable();
	_support_skills_script.CloseFile();

	_status_effects_script.CloseTable();
	_status_effects_script.CloseFile();

	_map_sprites_script.CloseFile();
	_map_objects_script.CloseFile();
	_map_treasures_script.CloseFile();

	_battle_events_script.CloseTable();
	_battle_events_script.CloseFile();
} // GameGlobal::~GameGlobal()



bool GameGlobal::SingletonInitialize() {
	// Open up the persistent script files
	if (_global_script.OpenFile("dat/global.lua") == false) {
		return false;
	}

	if (!_items_script.OpenFile("dat/objects/items.lua"))
		return false;
	_items_script.OpenTable("items");

	if (!_key_items_script.OpenFile("dat/objects/key_items.lua"))
		return false;
	_key_items_script.OpenTable("key_items");

	if (_weapons_script.OpenFile("dat/objects/weapons.lua") == false) {
		return false;
	}
	_weapons_script.OpenTable("weapons");

	if (_head_armor_script.OpenFile("dat/objects/head_armor.lua") == false) {
		return false;
	}
	_head_armor_script.OpenTable("armor");

	if (_torso_armor_script.OpenFile("dat/objects/torso_armor.lua") == false) {
		return false;
	}
	_torso_armor_script.OpenTable("armor");

	if (_arm_armor_script.OpenFile("dat/objects/arm_armor.lua") == false) {
		return false;
	}
	_arm_armor_script.OpenTable("armor");

	if (_leg_armor_script.OpenFile("dat/objects/leg_armor.lua") == false) {
		return false;
	}
	_leg_armor_script.OpenTable("armor");

	if (_attack_skills_script.OpenFile("dat/skills/attack.lua") == false) {
		return false;
	}
	_attack_skills_script.OpenTable("skills");

	if (_support_skills_script.OpenFile("dat/skills/support.lua") == false) {
		return false;
	}
	_support_skills_script.OpenTable("skills");

	if (_defend_skills_script.OpenFile("dat/skills/defense.lua") == false) {
		return false;
	}
	_defend_skills_script.OpenTable("skills");

	if (_status_effects_script.OpenFile("dat/effects/status.lua") == false) {
		return false;
	}
	_status_effects_script.OpenTable("status_effects");

	if (!_map_sprites_script.OpenFile("dat/actors/map_sprites.lua"))
		return false;

	if (!_map_objects_script.OpenFile("dat/actors/map_objects.lua"))
		return false;

	if (!_map_treasures_script.OpenFile("dat/actors/map_treasures.lua"))
		return false;


	if (_battle_events_script.OpenFile("dat/battle_events.lua") == false) {
		return false;
	}
	_battle_events_script.OpenTable("battle_events");

	return true;
} // bool GameGlobal::SingletonInitialize()



void GameGlobal::ClearAllData() {
	// Delete all inventory objects
	for (std::map<uint32, GlobalObject*>::iterator it = _inventory.begin(); it != _inventory.end(); ++it) {
		delete it->second;
	}
	_inventory.clear();
	_inventory_items.clear();
	_inventory_weapons.clear();
	_inventory_head_armor.clear();
	_inventory_torso_armor.clear();
	_inventory_arm_armor.clear();
	_inventory_leg_armor.clear();
	_inventory_shards.clear();
	_inventory_key_items.clear();

	// Delete all characters
	for (std::map<uint32, GlobalCharacter*>::iterator it = _characters.begin(); it != _characters.end(); ++it) {
		delete it->second;
	}
	_characters.clear();
	_ordered_characters.clear();
	_active_party.RemoveAllActors();

	// Delete all event groups
	for (std::map<std::string, GlobalEventGroup*>::iterator it = _event_groups.begin(); it != _event_groups.end(); ++it) {
		delete (it->second);
	}
	_event_groups.clear();
} // void GameGlobal::ClearAllData()

////////////////////////////////////////////////////////////////////////////////
// GameGlobal class - Character Functions
////////////////////////////////////////////////////////////////////////////////

void GameGlobal::AddCharacter(uint32 id) {
	std::map<uint32, GlobalCharacter*>::iterator it = _characters.find(id);
	if (it != _characters.end()) {
		if (it->second->IsEnabled()) {
				IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to add a character that already existed: " << id << std::endl;
		        return;
		}
		else {
			// Re-enable the character in that case
			it->second->Enable(true);
		}
	}

	GlobalCharacter *ch = 0;

	// Add the character if not existing in the main character data
	if (it == _characters.end()) {
		ch = new GlobalCharacter(id);
		_characters.insert(std::make_pair(id, ch));
	}
	else {
		ch = it->second;
	}

	// Add the new character to the active party if the active party contains less than four characters
	if (_ordered_characters.size() < GLOBAL_MAX_PARTY_SIZE)
		_active_party.AddActor(ch);

	_ordered_characters.push_back(ch);
}



void GameGlobal::AddCharacter(GlobalCharacter* ch) {
	if (ch == NULL) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received NULL pointer argument" << std::endl;
		return;
	}

	if (_characters.find(ch->GetID()) != _characters.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to add a character that already existed: " << ch->GetID() << std::endl;
		return;
	}

	_characters.insert(std::make_pair(ch->GetID(), ch));

	// If the charactger is currently disabled, don't add it to the available characters.
	if (!ch->IsEnabled())
		return;

	// Add the new character to the active party if the active party contains less than four characters
	if (_ordered_characters.size() < GLOBAL_MAX_PARTY_SIZE)
		_active_party.AddActor(ch);

	_ordered_characters.push_back(ch);
}



void GameGlobal::RemoveCharacter(uint32 id, bool erase) {
	std::map<uint32, GlobalCharacter*>::iterator it = _characters.find(id);
	if (it == _characters.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to remove a character that did not exist: " << id << std::endl;
		return;
	}

	// Disable the character
	it->second->Enable(false);

	for (std::vector<GlobalCharacter*>::iterator it = _ordered_characters.begin(); it != _ordered_characters.end(); ++it) {
		if ((*it)->GetID() == id) {
			_ordered_characters.erase(it);
			break;
		}
	}

	// Reform the active party in case the removed character was a member of it
	_active_party.RemoveAllActors();
	for (uint32 j = 0; j < _ordered_characters.size() && j < GLOBAL_MAX_PARTY_SIZE; j++) {
		_active_party.AddActor(_ordered_characters[j]);
	}

	// If we were asked to remove the character completely from the game data.
	if (erase) {
		delete(it->second);
		_characters.erase(it);
	}
}


GlobalCharacter* GameGlobal::GetCharacter(uint32 id) {
	std::map<uint32, GlobalCharacter*>::iterator ch = _characters.find(id);
	if (ch == _characters.end())
		return NULL;
	else
		return (ch->second);
}


void GameGlobal::SwapCharactersByIndex(uint32 first_index, uint32 second_index) {
	// Deal with the ordered characters
	if (first_index == second_index) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "first_index and second_index arguments had the same value: " << first_index << std::endl;
		return;
	}
	if (first_index >= _ordered_characters.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "first_index argument exceeded current party size: " << first_index << std::endl;
		return;
	}
	if (second_index >= _ordered_characters.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "second_index argument exceeded current party size: " << second_index << std::endl;
		return;
	}

	// Swap the characters
	GlobalCharacter* tmp = _ordered_characters[first_index];
	_ordered_characters[first_index] = _ordered_characters[second_index];
	_ordered_characters[second_index] = tmp;

	// Do the same for the party member.
	_active_party.SwapActorsByIndex(first_index, second_index);
}

////////////////////////////////////////////////////////////////////////////////
// GameGlobal class - Inventory Functions
////////////////////////////////////////////////////////////////////////////////

void GameGlobal::AddToInventory(uint32 obj_id, uint32 obj_count) {
	// If the object is already in the inventory, increment the count of the object
	if (_inventory.find(obj_id) != _inventory.end()) {
		_inventory[obj_id]->IncrementCount(obj_count);
		return;
	}

	// Otherwise create a new object instance and add it to the inventory
	if ((obj_id > 0) && (obj_id <= MAX_ITEM_ID)) {
		GlobalItem *new_obj = new GlobalItem(obj_id, obj_count);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_items.push_back(new_obj);
	}
	else if ((obj_id > MAX_ITEM_ID) && (obj_id <= MAX_WEAPON_ID)) {
		GlobalWeapon *new_obj = new GlobalWeapon(obj_id, obj_count);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_weapons.push_back(new_obj);
	}
	else if ((obj_id > MAX_WEAPON_ID) && (obj_id <= MAX_HEAD_ARMOR_ID)) {
		GlobalArmor *new_obj = new GlobalArmor(obj_id, obj_count);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_head_armor.push_back(new_obj);
	}
	else if ((obj_id > MAX_HEAD_ARMOR_ID) && (obj_id <= MAX_TORSO_ARMOR_ID)) {
		GlobalArmor *new_obj = new GlobalArmor(obj_id, obj_count);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_torso_armor.push_back(new_obj);
	}
	else if ((obj_id > MAX_TORSO_ARMOR_ID) && (obj_id <= MAX_ARM_ARMOR_ID)) {
		GlobalArmor *new_obj = new GlobalArmor(obj_id, obj_count);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_arm_armor.push_back(new_obj);
	}
	else if ((obj_id > MAX_ARM_ARMOR_ID) && (obj_id <= MAX_LEG_ARMOR_ID)) {
		GlobalArmor *new_obj = new GlobalArmor(obj_id, obj_count);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_leg_armor.push_back(new_obj);
	}
	else if ((obj_id > MAX_LEG_ARMOR_ID) && (obj_id <= MAX_SHARD_ID)) {
// 		GlobalShard *new_obj = new GlobalShard(obj_id, obj_count);
// 		_inventory.insert(std::make_pair(obj_id, new_obj));
// 		_inventory_shards.push_back(new_obj);
	}
	else if ((obj_id > MAX_SHARD_ID) && (obj_id <= MAX_KEY_ITEM_ID)) {
		GlobalKeyItem *new_obj = new GlobalKeyItem(obj_id, obj_count);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_key_items.push_back(new_obj);
	}
	else {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to add invalid object to inventory with id: " << obj_id << std::endl;
	}
} // void GameGlobal::AddToInventory(uint32 obj_id)



void GameGlobal::AddToInventory(GlobalObject* object) {
	if (object == NULL) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received NULL pointer argument" << std::endl;
		return;
	}

	uint32 obj_id = object->GetID();
	uint32 obj_count = object->GetCount();

	// If an instance of the same object is already inside the inventory, just increment the count and delete the object
	if (_inventory.find(obj_id) != _inventory.end()) {
		_inventory[obj_id]->IncrementCount(obj_count);
		delete object;
		return;
	}

	// Figure out which type of object this is, cast it to the correct type, and add it to the inventory
	if ((obj_id > 0) && (obj_id <= MAX_ITEM_ID)) {
		GlobalItem *new_obj = dynamic_cast<GlobalItem*>(object);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_items.push_back(new_obj);
	}
	else if ((obj_id > MAX_ITEM_ID) && (obj_id <= MAX_WEAPON_ID)) {
		GlobalWeapon *new_obj = dynamic_cast<GlobalWeapon*>(object);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_weapons.push_back(new_obj);
	}
	else if ((obj_id > MAX_WEAPON_ID) && (obj_id <= MAX_HEAD_ARMOR_ID)) {
		GlobalArmor *new_obj = dynamic_cast<GlobalArmor*>(object);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_head_armor.push_back(new_obj);
	}
	else if ((obj_id > MAX_HEAD_ARMOR_ID) && (obj_id <= MAX_TORSO_ARMOR_ID)) {
		GlobalArmor *new_obj = dynamic_cast<GlobalArmor*>(object);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_torso_armor.push_back(new_obj);
	}
	else if ((obj_id > MAX_TORSO_ARMOR_ID) && (obj_id <= MAX_ARM_ARMOR_ID)) {
		GlobalArmor *new_obj = dynamic_cast<GlobalArmor*>(object);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_arm_armor.push_back(new_obj);
	}
	else if ((obj_id > MAX_ARM_ARMOR_ID) && (obj_id <= MAX_LEG_ARMOR_ID)) {
		GlobalArmor *new_obj = dynamic_cast<GlobalArmor*>(object);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_leg_armor.push_back(new_obj);
	}
	else if ((obj_id > MAX_LEG_ARMOR_ID) && (obj_id <= MAX_SHARD_ID)) {
// 		GlobalShard *new_obj = dynamic_cast<GlobalShard*>(object);
// 		_inventory.insert(std::make_pair(obj_id, new_obj));
// 		_inventory_shards.push_back(new_obj);
	}
	else if ((obj_id > MAX_SHARD_ID) && (obj_id <= MAX_KEY_ITEM_ID)) {
		GlobalKeyItem *new_obj = dynamic_cast<GlobalKeyItem*>(object);
		_inventory.insert(std::make_pair(obj_id, new_obj));
		_inventory_key_items.push_back(new_obj);
	}
	else {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to add invalid object to inventory with id: " << obj_id << std::endl;
		delete object;
	}
} // void GameGlobal::AddToInventory(GlobalObject* object)



void GameGlobal::RemoveFromInventory(uint32 obj_id) {
	if (_inventory.find(obj_id) == _inventory.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to remove an object from inventory that didn't exist with id: " << obj_id << std::endl;
		return;
	}

	// Use the id value to figure out what type of object it is, and remove it from the object vector
	if ((obj_id > 0) && (obj_id <= MAX_ITEM_ID)) {
		if (_RemoveFromInventory(obj_id, _inventory_items) == false)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to remove was not found in inventory items: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_ITEM_ID) && (obj_id <= MAX_WEAPON_ID)) {
		if (_RemoveFromInventory(obj_id, _inventory_weapons) == false)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to remove was not found in inventory weapons: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_WEAPON_ID) && (obj_id <= MAX_HEAD_ARMOR_ID)) {
		if (_RemoveFromInventory(obj_id, _inventory_head_armor) == false)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to remove was not found in inventory head armor: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_HEAD_ARMOR_ID) && (obj_id <= MAX_TORSO_ARMOR_ID)) {
		if (_RemoveFromInventory(obj_id, _inventory_torso_armor) == false)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to remove was not found in inventory torso armor: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_TORSO_ARMOR_ID) && (obj_id <= MAX_ARM_ARMOR_ID)) {
		if (_RemoveFromInventory(obj_id, _inventory_arm_armor) == false)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to remove was not found in inventory arm armor: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_ARM_ARMOR_ID) && (obj_id <= MAX_LEG_ARMOR_ID)) {
		if (_RemoveFromInventory(obj_id, _inventory_leg_armor) == false)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to remove was not found in inventory leg armor: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_LEG_ARMOR_ID) && (obj_id <= MAX_SHARD_ID)) {
		if (_RemoveFromInventory(obj_id, _inventory_shards) == false)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to remove was not found in inventory shards: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_SHARD_ID) && (obj_id <= MAX_KEY_ITEM_ID)) {
		if (_RemoveFromInventory(obj_id, _inventory_key_items) == false)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to remove was not found in inventory key items: " << obj_id << std::endl;
	}
	else {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to remove an object from inventory with an invalid id: " << obj_id << std::endl;
	}
} // void GameGlobal::RemoveFromInventory(uint32 obj_id)



GlobalObject* GameGlobal::RetrieveFromInventory(uint32 obj_id, bool all_counts) {
	if (_inventory.find(obj_id) == _inventory.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to retrieve an object from inventory that didn't exist with id: " << obj_id << std::endl;
		return NULL;
	}

	GlobalObject* return_object = NULL;
	// Use the id value to figure out what type of object it is, and remove it from the object vector
	if ((obj_id > 0) && (obj_id <= MAX_ITEM_ID)) {
		return_object = _RetrieveFromInventory(obj_id, _inventory_items, all_counts);
		if (return_object == NULL)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to retrieve was not found in inventory items: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_ITEM_ID) && (obj_id <= MAX_WEAPON_ID)) {
		return_object = _RetrieveFromInventory(obj_id, _inventory_weapons, all_counts);
		if (return_object == NULL)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to retrieve was not found in inventory weapons: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_WEAPON_ID) && (obj_id <= MAX_HEAD_ARMOR_ID)) {
		return_object = _RetrieveFromInventory(obj_id, _inventory_head_armor, all_counts);
		if (return_object == NULL)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to retrieve was not found in inventory head armor: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_HEAD_ARMOR_ID) && (obj_id <= MAX_TORSO_ARMOR_ID)) {
		return_object = _RetrieveFromInventory(obj_id, _inventory_torso_armor, all_counts);
		if (return_object == NULL)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to retrieve was not found in inventory torso armor: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_TORSO_ARMOR_ID) && (obj_id <= MAX_ARM_ARMOR_ID)) {
		return_object = _RetrieveFromInventory(obj_id, _inventory_arm_armor, all_counts);
		if (return_object == NULL)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to retrieve was not found in inventory arm armor: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_ARM_ARMOR_ID) && (obj_id <= MAX_LEG_ARMOR_ID)) {
		return_object = _RetrieveFromInventory(obj_id, _inventory_leg_armor, all_counts);
		if (return_object == NULL)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to retrieve was not found in inventory leg armor: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_LEG_ARMOR_ID) && (obj_id <= MAX_SHARD_ID)) {
		return_object = _RetrieveFromInventory(obj_id, _inventory_shards, all_counts);
		if (return_object == NULL)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to retrieve was not found in inventory shards: " << obj_id << std::endl;
	}
	else if ((obj_id > MAX_SHARD_ID) && (obj_id <= MAX_KEY_ITEM_ID)) {
		return_object = _RetrieveFromInventory(obj_id, _inventory_key_items, all_counts);
		if (return_object == NULL)
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "object to retrieve was not found in inventory key items: " << obj_id << std::endl;
	}
	else {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to retrieve an object from inventory with an invalid id: " << obj_id << std::endl;
	}

	return return_object;
} // GlobalObject* GameGlobal::RetrieveFromInventory(uint32 obj_id, bool all_counts)



void GameGlobal::IncrementObjectCount(uint32 obj_id, uint32 count) {
	// Do nothing if the item does not exist in the inventory
	if (_inventory.find(obj_id) == _inventory.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to increment count for an object that was not present in the inventory: " << obj_id << std::endl;
		return;
	}

	_inventory[obj_id]->IncrementCount(count);
}



void GameGlobal::DecrementObjectCount(uint32 obj_id, uint32 count) {
	// Do nothing if the item does not exist in the inventory
	if (_inventory.find(obj_id) == _inventory.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to decrement count for an object that was not present in the inventory: " << obj_id << std::endl;
		return;
	}

	// Print a warning if the amount to decrement by exceeds the object's current count
	if (count > _inventory[obj_id]->GetCount()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "amount to decrement count by exceeded available count: " << obj_id << std::endl;
	}

	// Decrement the number of objects so long as the number to decrement by does not equal or exceed the count
	if (count < _inventory[obj_id]->GetCount())
		_inventory[obj_id]->DecrementCount(count);
	// Otherwise remove the object from the inventory completely
	else
		RemoveFromInventory(obj_id);
}

////////////////////////////////////////////////////////////////////////////////
// GameGlobal class - Event Group Functions
////////////////////////////////////////////////////////////////////////////////

bool GameGlobal::DoesEventExist(const std::string& group_name, const std::string& event_name) const {
	std::map<std::string, GlobalEventGroup*>::const_iterator group_iter = _event_groups.find(group_name);
	if (group_iter == _event_groups.end())
		return false;

	std::map<std::string, int32>::const_iterator event_iter = group_iter->second->GetEvents().find(event_name);
	if (event_iter == group_iter->second->GetEvents().end())
		return false;

	return true;
}



void GameGlobal::AddNewEventGroup(const std::string& group_name) {
	if (DoesEventGroupExist(group_name) == true) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed because there was already an event group that existed for "
			<< "the requested group name: " << group_name << std::endl;
		return;
	}

	GlobalEventGroup* geg = new GlobalEventGroup(group_name);
	_event_groups.insert(std::make_pair(group_name, geg));
}



GlobalEventGroup* GameGlobal::GetEventGroup(const std::string& group_name) const {
	std::map<std::string, GlobalEventGroup*>::const_iterator group_iter = _event_groups.find(group_name);
	if (group_iter == _event_groups.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "could not find any event group by the requested name: " << group_name << std::endl;
		return NULL;
	}
	return (group_iter->second);
}



int32 GameGlobal::GetEventValue(const std::string& group_name, const std::string& event_name) const {
	std::map<std::string, GlobalEventGroup*>::const_iterator group_iter = _event_groups.find(group_name);
	if (group_iter == _event_groups.end())
		return GLOBAL_BAD_EVENT;

	std::map<std::string, int32>::const_iterator event_iter = group_iter->second->GetEvents().find(event_name);
	if (event_iter == group_iter->second->GetEvents().end())
		return GLOBAL_BAD_EVENT;

	return event_iter->second;
}



uint32 GameGlobal::GetNumberEvents(const std::string& group_name) const {
	std::map<std::string, GlobalEventGroup*>::const_iterator group_iter = _event_groups.find(group_name);
	if (group_iter == _event_groups.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "could not find any event group by the requested name: " << group_name << std::endl;
		return 0;
	}
	return group_iter->second->GetNumberEvents();
}

////////////////////////////////////////////////////////////////////////////////
// GameGlobal class - Other Functions
////////////////////////////////////////////////////////////////////////////////

void GameGlobal::SetMap(const std::string& map_filename, const std::string& map_image_filename,
						const hoa_utils::ustring& map_hud_name) {
	_map_filename = map_filename;

	if (!_map_image.Load(map_image_filename))
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to load map image: " << map_image_filename << std::endl;

	// Updates the map hud names info.
	_previous_map_hud_name = _map_hud_name;
	_map_hud_name = map_hud_name;
	_same_map_hud_name_as_previous = (MakeStandardString(_previous_map_hud_name) == MakeStandardString(_map_hud_name));
}



bool GameGlobal::SaveGame(const std::string& filename, uint32 slot_id, uint32 x_position, uint32 y_position) {
	WriteScriptDescriptor file;
	if (file.OpenFile(filename) == false) {
		return false;
	}

	// Write out namespace information
	file.WriteNamespace("save_game1");

	// Save play settings
	file.InsertNewLine();
	file.WriteInt("battle_setting", _battle_setting);

	// Save simple play data
	file.InsertNewLine();
	file.WriteString("map_filename", _map_filename);
	//! \note Coords are in map tiles
	file.WriteUInt("location_x", x_position);
	file.WriteUInt("location_y", y_position);
	file.WriteUInt("play_hours", SystemManager->GetPlayHours());
	file.WriteUInt("play_minutes", SystemManager->GetPlayMinutes());
	file.WriteUInt("play_seconds", SystemManager->GetPlaySeconds());
	file.WriteUInt("drunes", _drunes);

	// Save the inventory (object id + object count pairs)
	// NOTE: This does not save any weapons/armor that are equipped on the characters. That data
	// is stored alongside the character data when it is saved
	_SaveInventory(file, "items", _inventory_items);
	_SaveInventory(file, "weapons", _inventory_weapons);
	_SaveInventory(file, "head_armor", _inventory_head_armor);
	_SaveInventory(file, "torso_armor", _inventory_torso_armor);
	_SaveInventory(file, "arm_armor", _inventory_arm_armor);
	_SaveInventory(file, "leg_armor", _inventory_leg_armor);
	_SaveInventory(file, "shards", _inventory_shards);
	_SaveInventory(file, "key_items", _inventory_key_items);

	// ----- (5) Save character data
	file.InsertNewLine();
	file.WriteLine("characters = {");
	// First save the order of the characters in the party
	file.WriteLine("\t[\"order\"] = {");
	for (uint32 i = 0; i < _ordered_characters.size(); ++i) {
		if (i == 0)
			file.WriteLine("\t\t" + NumberToString(_ordered_characters[i]->GetID()), false);
		else
			file.WriteLine(", " + NumberToString(_ordered_characters[i]->GetID()), false);
	}
	file.WriteLine("\n\t},");

	// Now save each individual character's data
	for (uint32 i = 0; i < _ordered_characters.size(); i++) {
		if ((i + 1) == _ordered_characters.size())
			_SaveCharacter(file, _ordered_characters[i], true);
		else
			_SaveCharacter(file, _ordered_characters[i], false);
	}
	file.WriteLine("}");

	// ----- (6) Save event data
	file.InsertNewLine();
	file.WriteLine("event_groups = {");
	for (std::map<std::string, GlobalEventGroup*>::iterator it = _event_groups.begin(); it != _event_groups.end(); ++it) {
		_SaveEvents(file, it->second);
	}
	file.WriteLine("}");

	file.InsertNewLine();

	// ----- (7) Report any errors detected from the previous write operations
	if (file.IsErrorDetected()) {
		if (GLOBAL_DEBUG) {
			PRINT_WARNING << "one or more errors occurred while writing the save game file - they are listed below" << std::endl;
			std::cerr << file.GetErrorMessages() << std::endl;
			file.ClearErrors();
		}
	}

	file.CloseFile();

	// Store the game slot the game is coming from.
	_game_slot_id = slot_id;

	return true;
} // bool GameGlobal::SaveGame(string& filename)



bool GameGlobal::LoadGame(const std::string& filename, uint32 slot_id) {
	ReadScriptDescriptor file;
	if (file.OpenFile(filename, true) == false) {
		return false;
	}

	ClearAllData();

	// open the namespace that the save game is encapsulated in.
	file.OpenTable("save_game1");

	// Load play settings
	_battle_setting = static_cast<GLOBAL_BATTLE_SETTING>(file.ReadInt("battle_setting"));

	// Load play data
	_map_filename = file.ReadString("map_filename");
	// Load a potential saved position
	_x_save_map_position = file.ReadUInt("location_x");
	_y_save_map_position = file.ReadUInt("location_y");
	uint8 hours, minutes, seconds;
	hours = file.ReadUInt("play_hours");
	minutes = file.ReadUInt("play_minutes");
	seconds = file.ReadUInt("play_seconds");
	SystemManager->SetPlayTime(hours, minutes, seconds);
	_drunes = file.ReadUInt("drunes");

	// Load inventory
	_LoadInventory(file, "items");
	_LoadInventory(file, "weapons");
	_LoadInventory(file, "head_armor");
	_LoadInventory(file, "torso_armor");
	_LoadInventory(file, "arm_armor");
	_LoadInventory(file, "leg_armor");
	_LoadInventory(file, "shards");
	_LoadInventory(file, "key_items");

	// Load characters into the party in the correct order
	file.OpenTable("characters");
	std::vector<uint32> char_ids;
	file.ReadUIntVector("order", char_ids);
	for (uint32 i = 0; i < char_ids.size(); i++) {
		_LoadCharacter(file, char_ids[i]);
	}
	file.CloseTable();

	// Load event data
	std::vector<std::string> group_names;
	file.OpenTable("event_groups");
	file.ReadTableKeys(group_names);
	for (uint32 i = 0; i < group_names.size(); i++)
		_LoadEvents(file, group_names[i]);
	file.CloseTable();

	// Report any errors detected from the previous read operations
	if (file.IsErrorDetected()) {
		if (GLOBAL_DEBUG) {
			PRINT_WARNING << "one or more errors occurred while reading the save game file - they are listed below" << std::endl;
			std::cerr << file.GetErrorMessages() << std::endl;
			file.ClearErrors();
		}
	}

	file.CloseFile();

	// Store the game slot the game is coming from.
	_game_slot_id = slot_id;

	return true;
} // bool GameGlobal::LoadGame(string& filename)

////////////////////////////////////////////////////////////////////////////////
// GameGlobal class - Private Methods
////////////////////////////////////////////////////////////////////////////////

void GameGlobal::_SaveCharacter(WriteScriptDescriptor& file, GlobalCharacter* character, bool last) {
	if (file.IsFileOpen() == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "the file provided in the function argument was not open" << std::endl;
		return;
	}
	if (character == NULL) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a NULL character pointer argument" << std::endl;
		return;
	}

	file.WriteLine("\t[" + NumberToString(character->GetID()) + "] = {");

	// Store whether the character is available
	file.WriteLine("\t\tenabled = " + std::string(character->IsEnabled() ? "true" : "false") + ",");

	// ----- (1): Write out the character's stats
	file.WriteLine("\t\texperience_level = " + NumberToString(character->GetExperienceLevel()) + ",");
	file.WriteLine("\t\texperience_points = " + NumberToString(character->GetExperiencePoints()) + ",");
	file.WriteLine("\t\texperience_points_next = " + NumberToString(character->GetExperienceForNextLevel()) + ", ");

	file.WriteLine("\t\tmax_hit_points = " + NumberToString(character->GetMaxHitPoints()) + ",");
	file.WriteLine("\t\thit_points = " + NumberToString(character->GetHitPoints()) + ",");
	file.WriteLine("\t\tmax_skill_points = " + NumberToString(character->GetMaxSkillPoints()) + ",");
	file.WriteLine("\t\tskill_points = " + NumberToString(character->GetSkillPoints()) + ",");

	file.WriteLine("\t\tstrength = " + NumberToString(character->GetStrength()) + ",");
	file.WriteLine("\t\tvigor = " + NumberToString(character->GetVigor()) + ",");
	file.WriteLine("\t\tfortitude = " + NumberToString(character->GetFortitude()) + ",");
	file.WriteLine("\t\tprotection = " + NumberToString(character->GetProtection()) + ",");
	file.WriteLine("\t\tagility = " + NumberToString(character->GetAgility()) + ",");
	file.WriteLine("\t\tevade = " + NumberToString(character->GetEvade()) + ",");

	// ----- (2): Write out the character's equipment
	uint32 weapon_id = 0;
	uint32 head_id = 0;
	uint32 torso_id = 0;
	uint32 arm_id = 0;
	uint32 leg_id = 0;
	GlobalObject *obj_tmp = NULL;

	obj_tmp = character->GetWeaponEquipped();
	if (obj_tmp != NULL)
		weapon_id = obj_tmp->GetID();

	obj_tmp = character->GetHeadArmorEquipped();
	if (obj_tmp != NULL)
		head_id = obj_tmp->GetID();

	obj_tmp = character->GetTorsoArmorEquipped();
	if (obj_tmp != NULL)
		torso_id = obj_tmp->GetID();

	obj_tmp = character->GetArmArmorEquipped();
	if (obj_tmp != NULL)
		arm_id = obj_tmp->GetID();

	obj_tmp = character->GetLegArmorEquipped();
	if (obj_tmp != NULL)
		leg_id = obj_tmp->GetID();

	file.InsertNewLine();
	file.WriteLine("\t\tequipment = {");
	file.WriteLine("\t\t\tweapon = " + NumberToString(weapon_id) + ",");
	file.WriteLine("\t\t\thead_armor = " + NumberToString(head_id) + ",");
	file.WriteLine("\t\t\ttorso_armor = " + NumberToString(torso_id) + ",");
	file.WriteLine("\t\t\tarm_armor = " + NumberToString(arm_id) + ",");
	file.WriteLine("\t\t\tleg_armor = " + NumberToString(leg_id));
	file.WriteLine("\t\t},");

	// ----- (3): Write out the character's skills
	std::vector<GlobalSkill*>* skill_vector;

	file.InsertNewLine();
	file.WriteLine("\t\tattack_skills = {");
	skill_vector = character->GetAttackSkills();
	for (uint32 i = 0; i < skill_vector->size(); i++) {
		if (i == 0)
			file.WriteLine("\t\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine(NumberToString(skill_vector->at(i)->GetID()), false);
	}
	file.WriteLine("\n\t\t},");

	file.InsertNewLine();
	file.WriteLine("\t\tdefense_skills = {");
	skill_vector = character->GetDefenseSkills();
	for (uint32 i = 0; i < skill_vector->size(); i++) {
		if (i == 0)
			file.WriteLine("\t\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine(NumberToString(skill_vector->at(i)->GetID()), false);
	}
	file.WriteLine("\n\t\t},");

	file.InsertNewLine();
	file.WriteLine("\t\tsupport_skills = {");
	skill_vector = character->GetSupportSkills();
	for (uint32 i = 0; i < skill_vector->size(); i++) {
		if (i == 0)
			file.WriteLine("\t\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine(NumberToString(skill_vector->at(i)->GetID()), false);
	}
	file.WriteLine("\n\t\t},");

	// ----- (4): Write out the character's growth data
	GlobalCharacterGrowth* growth = character->GetGrowth();
	if (growth->IsGrowthDetected()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "discovered unacknowledged character growth while saving game file" << std::endl;
	}

	file.InsertNewLine();
	file.WriteLine("\t\tgrowth = {");
	file.WriteLine("\t\t\texperience_for_last_level = " + NumberToString(growth->_experience_for_last_level) + ",");
	file.WriteLine("\t\t\texperience_for_next_level = " + NumberToString(growth->_experience_for_next_level) + ",");

	file.WriteLine("\t\t\thit_points = { ");
	for (uint32 i = 0; i < growth->_hit_points_periodic_growth.size(); i++) {
		if (i == 0)
			file.WriteLine("\t\t\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine("[" + NumberToString(growth->_hit_points_periodic_growth[i].first) + "] = "
			+ NumberToString(growth->_hit_points_periodic_growth[i].second), false);
	}
	file.WriteLine("\n\t\t\t},");

	file.WriteLine("\t\t\tskill_points = { ");
	for (uint32 i = 0; i < growth->_skill_points_periodic_growth.size(); i++) {
		if (i == 0)
			file.WriteLine("\t\t\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine("[" + NumberToString(growth->_skill_points_periodic_growth[i].first) + "] = "
			+ NumberToString(growth->_skill_points_periodic_growth[i].second), false);
	}
	file.WriteLine("\n\t\t\t},");

	file.WriteLine("\t\t\tstrength = { ");
	for (uint32 i = 0; i < growth->_strength_periodic_growth.size(); i++) {
		if (i == 0)
			file.WriteLine("\t\t\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine("[" + NumberToString(growth->_strength_periodic_growth[i].first) + "] = "
			+ NumberToString(growth->_strength_periodic_growth[i].second), false);
	}
	file.WriteLine("\n\t\t\t},");

	file.WriteLine("\t\t\tvigor = { ");
	for (uint32 i = 0; i < growth->_vigor_periodic_growth.size(); i++) {
		if (i == 0)
			file.WriteLine("\t\t\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine("[" + NumberToString(growth->_vigor_periodic_growth[i].first) + "] = "
			+ NumberToString(growth->_vigor_periodic_growth[i].second), false);
	}
	file.WriteLine("\n\t\t\t},");

	file.WriteLine("\t\t\tfortitude = { ");
	for (uint32 i = 0; i < growth->_fortitude_periodic_growth.size(); i++) {
		if (i == 0)
			file.WriteLine("\t\t\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine("[" + NumberToString(growth->_fortitude_periodic_growth[i].first) + "] = "
			+ NumberToString(growth->_fortitude_periodic_growth[i].second), false);
	}
	file.WriteLine("\n\t\t\t},");

	file.WriteLine("\t\t\tprotection = { ");
	for (uint32 i = 0; i < growth->_protection_periodic_growth.size(); i++) {
		if (i == 0)
			file.WriteLine("\t\t\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine("[" + NumberToString(growth->_protection_periodic_growth[i].first) + "] = "
			+ NumberToString(growth->_protection_periodic_growth[i].second), false);
	}
	file.WriteLine("\n\t\t\t},");

	file.WriteLine("\t\t\tagility = { ");
	for (uint32 i = 0; i < growth->_agility_periodic_growth.size(); i++) {
		if (i == 0)
			file.WriteLine("\t\t\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine("[" + NumberToString(growth->_agility_periodic_growth[i].first) + "] = "
			+ NumberToString(growth->_agility_periodic_growth[i].second), false);
	}
	file.WriteLine("\n\t\t\t},");

	file.WriteLine("\t\t\tevade = { ");
	for (uint32 i = 0; i < growth->_evade_periodic_growth.size(); i++) {
		if (i == 0)
			file.WriteLine("\t\t\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine("[" + NumberToString(growth->_evade_periodic_growth[i].first) + "] = "
			+ NumberToString(growth->_evade_periodic_growth[i].second), false);
	}
	file.WriteLine("\n\t\t\t},");

	file.WriteLine("\t\t\tskills_learned = { ");
	for (std::vector<GlobalSkill*>::iterator it = growth->_skills_learned.begin(); it != growth->_skills_learned.end(); ++it) {
		if (it == growth->_skills_learned.begin())
			file.WriteLine("\t\t\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine(NumberToString((*it)->GetID()), false);
	}
	file.WriteLine("\n\t\t\t}");
	file.WriteLine("\t\t}");

	if (last)
		file.WriteLine("\t}");
	else
		file.WriteLine("\t},");
} // void GameGlobal::_SaveCharacter(WriteScriptDescriptor& file, GlobalCharacter* character, bool last)



void GameGlobal::_SaveEvents(WriteScriptDescriptor& file, GlobalEventGroup* event_group) {
	if (file.IsFileOpen() == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "the file provided in the function argument was not open" << std::endl;
		return;
	}
	if (event_group == NULL) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a NULL event group pointer argument" << std::endl;
		return;
	}

	file.WriteLine("\t" + event_group->GetGroupName() + " = {");

	for (std::map<std::string, int32>::const_iterator it = event_group->GetEvents().begin(); it != event_group->GetEvents().end(); ++it) {
		if (it == event_group->GetEvents().begin())
			file.WriteLine("\t\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine("[\"" + it->first + "\"] = " + NumberToString(it->second), false);
	}
	file.WriteLine("\t},");
}



void GameGlobal::_LoadInventory(ReadScriptDescriptor& file, const std::string& category_name) {
	if (file.IsFileOpen() == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "the file provided in the function argument was not open" << std::endl;
		return;
	}

	std::vector<uint32> object_ids;

	// The table keys are the inventory object ID numbers. The value of each key is the count of that object
	file.OpenTable(category_name);
	file.ReadTableKeys(object_ids);
	for (uint32 i = 0; i < object_ids.size(); i++) {
		AddToInventory(object_ids[i], file.ReadUInt(object_ids[i]));
	}
	file.CloseTable();
}



void GameGlobal::_LoadCharacter(ReadScriptDescriptor& file, uint32 id) {
	if (file.IsFileOpen() == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "the file provided in the function argument was not open" << std::endl;
		return;
	}

	// ----- (1): Create a new GlobalCharacter object using the provided id
	// This loads all of the character's "static" data, such as their name, etc.
	GlobalCharacter* character = new GlobalCharacter(id, false);

	// This function assumes that the characters table in the saved game file is already open.
	// So all we need to open is the character's table
	file.OpenTable(id);

    // Gets whether the character is currently enabled
	if (file.DoesBoolExist("enabled"))
		character->Enable(file.ReadBool("enabled"));
	else // old format
		character->Enable(true);

	// ----- (2): Read in all of the character's stats data
	character->SetExperienceLevel(file.ReadUInt("experience_level"));
	character->SetExperiencePoints(file.ReadUInt("experience_points"));

	character->SetMaxHitPoints(file.ReadUInt("max_hit_points"));
	character->SetHitPoints(file.ReadUInt("hit_points"));
	character->SetMaxSkillPoints(file.ReadUInt("max_skill_points"));
	character->SetSkillPoints(file.ReadUInt("skill_points"));

	character->SetStrength(file.ReadUInt("strength"));
	character->SetVigor(file.ReadUInt("vigor"));
	character->SetFortitude(file.ReadUInt("fortitude"));
	character->SetProtection(file.ReadUInt("protection"));
	character->SetAgility(file.ReadUInt("agility"));
	character->SetEvade(file.ReadFloat("evade"));

	// ----- (3): Read the character's equipment and load it onto the character
	file.OpenTable("equipment");
	uint32 equip_id;

	// Equip the objects on the character as long as valid equipment IDs were read
	equip_id = file.ReadUInt("weapon");
	if (equip_id != 0) {
		character->EquipWeapon(new GlobalWeapon(equip_id));
	}

	equip_id = file.ReadUInt("head_armor");
	if (equip_id != 0) {
		character->EquipHeadArmor(new GlobalArmor(equip_id));
	}

	equip_id = file.ReadUInt("torso_armor");
	if (equip_id != 0) {
		character->EquipTorsoArmor(new GlobalArmor(equip_id));
	}

	equip_id = file.ReadUInt("arm_armor");
	if (equip_id != 0) {
		character->EquipArmArmor(new GlobalArmor(equip_id));
	}

	equip_id = file.ReadUInt("leg_armor");
	if (equip_id != 0) {
		character->EquipLegArmor(new GlobalArmor(equip_id));
	}

	file.CloseTable();

	// ----- (4): Read the character's skills and pass those onto the character object
	std::vector<uint32> skill_ids;

	skill_ids.clear();
	file.ReadUIntVector("attack_skills", skill_ids);
	for (uint32 i = 0; i < skill_ids.size(); i++) {
		character->AddSkill(skill_ids[i]);
	}

	skill_ids.clear();
	file.ReadUIntVector("defense_skills", skill_ids);
	for (uint32 i = 0; i < skill_ids.size(); i++) {
		character->AddSkill(skill_ids[i]);
	}

	skill_ids.clear();
	file.ReadUIntVector("support_skills", skill_ids);
	for (uint32 i = 0; i < skill_ids.size(); i++) {
		character->AddSkill(skill_ids[i]);
	}

	// ----- (5): Reset the character's growth from the saved data
	GlobalCharacterGrowth* growth = character->GetGrowth();
	std::vector<uint32> growth_keys;

	file.OpenTable("growth");

	growth->_experience_for_last_level = file.ReadUInt("experience_for_last_level");
	growth->_experience_for_next_level = file.ReadUInt("experience_for_next_level");

	growth_keys.clear();
	file.OpenTable("hit_points");
	file.ReadTableKeys(growth_keys);
	for (uint32 i = 0; i < growth_keys.size(); i++) {
		growth->_hit_points_periodic_growth.push_back(std::make_pair(growth_keys[i], file.ReadUInt(growth_keys[i])));
	}
	file.CloseTable();

	growth_keys.clear();
	file.OpenTable("skill_points");
	file.ReadTableKeys(growth_keys);
	for (uint32 i = 0; i < growth_keys.size(); i++) {
		growth->_skill_points_periodic_growth.push_back(std::make_pair(growth_keys[i], file.ReadUInt(growth_keys[i])));
	}
	file.CloseTable();

	growth_keys.clear();
	file.OpenTable("strength");
	file.ReadTableKeys(growth_keys);
	for (uint32 i = 0; i < growth_keys.size(); i++) {
		growth->_strength_periodic_growth.push_back(std::make_pair(growth_keys[i], file.ReadUInt(growth_keys[i])));
	}
	file.CloseTable();

	growth_keys.clear();
	file.OpenTable("vigor");
	file.ReadTableKeys(growth_keys);
	for (uint32 i = 0; i < growth_keys.size(); i++) {
		growth->_vigor_periodic_growth.push_back(std::make_pair(growth_keys[i], file.ReadUInt(growth_keys[i])));
	}
	file.CloseTable();

	growth_keys.clear();
	file.OpenTable("fortitude");
	file.ReadTableKeys(growth_keys);
	for (uint32 i = 0; i < growth_keys.size(); i++) {
		growth->_fortitude_periodic_growth.push_back(std::make_pair(growth_keys[i], file.ReadUInt(growth_keys[i])));
	}
	file.CloseTable();

	growth_keys.clear();
	file.OpenTable("protection");
	file.ReadTableKeys(growth_keys);
	for (uint32 i = 0; i < growth_keys.size(); i++) {
		growth->_protection_periodic_growth.push_back(std::make_pair(growth_keys[i], file.ReadUInt(growth_keys[i])));
	}
	file.CloseTable();

	growth_keys.clear();
	file.OpenTable("agility");
	file.ReadTableKeys(growth_keys);
	for (uint32 i = 0; i < growth_keys.size(); i++) {
		growth->_agility_periodic_growth.push_back(std::make_pair(growth_keys[i], file.ReadUInt(growth_keys[i])));
	}
	file.CloseTable();

	growth_keys.clear();
	file.OpenTable("evade");
	file.ReadTableKeys(growth_keys);
	for (uint32 i = 0; i < growth_keys.size(); i++) {
		growth->_evade_periodic_growth.push_back(std::make_pair(growth_keys[i], file.ReadFloat(growth_keys[i])));
	}
	file.CloseTable();

	growth_keys.clear();
	file.ReadUIntVector("skills_learned", growth_keys);
	for (uint32 i = 0; i < growth_keys.size(); i++) {
		growth->_skills_learned.push_back(new GlobalSkill(growth_keys[i]));
	}

	file.CloseTable();
	file.CloseTable();

	AddCharacter(character);
} // void GameGlobal::_LoadCharacter(ReadScriptDescriptor& file, uint32 id);



void GameGlobal::_LoadEvents(ReadScriptDescriptor& file, const std::string& group_name) {
	if (file.IsFileOpen() == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "the file provided in the function argument was not open" << std::endl;
		return;
	}

	AddNewEventGroup(group_name);
	GlobalEventGroup* new_group = GetEventGroup(group_name); // new_group is guaranteed not to be NULL

	std::vector<std::string> event_names;

	file.OpenTable(group_name);
	file.ReadTableKeys(event_names);
	for (uint32 i = 0; i < event_names.size(); i++) {
		new_group->AddNewEvent(event_names[i], file.ReadInt(event_names[i]));
	}
	file.CloseTable();
}

} // namespace hoa_global
