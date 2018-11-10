////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_inventory_handler.h"

#include "script/script_read.h"

using namespace vt_script;

namespace vt_global
{

InventoryHandler::~InventoryHandler()
{
    CloseScripts();
}

bool InventoryHandler::LoadScripts()
{
    // Open up the persistent script files
    if(!_items_script.OpenFile("data/inventory/items.lua") || !_items_script.OpenTable("items"))
        return false;

    if(!_weapons_script.OpenFile("data/inventory/weapons.lua") || !_weapons_script.OpenTable("weapons"))
        return false;

    if(!_head_armor_script.OpenFile("data/inventory/head_armor.lua") || !_head_armor_script.OpenTable("armor"))
        return false;

    if(!_torso_armor_script.OpenFile("data/inventory/torso_armor.lua") || !_torso_armor_script.OpenTable("armor"))
        return false;

    if(!_arm_armor_script.OpenFile("data/inventory/arm_armor.lua") || !_arm_armor_script.OpenTable("armor"))
        return false;

    if(!_leg_armor_script.OpenFile("data/inventory/leg_armor.lua") || !_leg_armor_script.OpenTable("armor"))
        return false;

    if(!_spirits_script.OpenFile("data/inventory/spirits.lua") || !_spirits_script.OpenTable("spirits"))
        return false;

    return true;
}

void InventoryHandler::CloseScripts()
{
    // Close all persistent script files
    _items_script.CloseTable();
    _items_script.CloseFile();

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

    _spirits_script.CloseTable();
    _spirits_script.CloseFile();
}

void InventoryHandler::ClearAllData()
{
    _inventory.clear();
    _inventory_items.clear();
    _inventory_weapons.clear();
    _inventory_head_armors.clear();
    _inventory_torso_armors.clear();
    _inventory_arm_armors.clear();
    _inventory_leg_armors.clear();
    _inventory_spirits.clear();
    _inventory_key_items.clear();
}

std::vector<std::shared_ptr<GlobalArmor>>& InventoryHandler::GetInventoryArmors(GLOBAL_OBJECT object_type)
{
    switch(object_type) {
    default:
        PRINT_WARNING << "Invalid object type provided. Returning default container" << std::endl;
        /* Falls through. */
    case GLOBAL_OBJECT_HEAD_ARMOR:
        return _inventory_head_armors;
    case GLOBAL_OBJECT_TORSO_ARMOR:
        return _inventory_torso_armors;
    case GLOBAL_OBJECT_ARM_ARMOR:
        return _inventory_arm_armors;
    case GLOBAL_OBJECT_LEG_ARMOR:
        return _inventory_leg_armors;
    }
}

void InventoryHandler::AddToInventory(uint32_t obj_id, uint32_t obj_count)
{
    // Don't add object instance without at least one actual item.
    if (obj_count == 0)
        return;

    // If the object is already in the inventory, increment the count of the object.
    if (_inventory.find(obj_id) != _inventory.end()) {
        _inventory[obj_id]->IncrementCount(obj_count);
        return;
    }

    // Otherwise, create a new object instance and add it to the inventory.
    std::shared_ptr<GlobalObject> new_object = nullptr;
    if ((obj_id > 0 && obj_id <= MAX_ITEM_ID) ||
        (obj_id > MAX_SPIRIT_ID && obj_id <= MAX_KEY_ITEM_ID)) {
        auto new_item = std::make_shared<GlobalItem>(obj_id, obj_count);
        new_object = new_item;
        _inventory.insert(std::make_pair(obj_id, new_object));
        _inventory_items.push_back(new_item);
    } else if ((obj_id > MAX_ITEM_ID) && (obj_id <= MAX_WEAPON_ID)) {
        auto new_wpn = std::make_shared<GlobalWeapon>(obj_id, obj_count);
        new_object = new_wpn;
        _inventory.insert(std::make_pair(obj_id, new_wpn));
        _inventory_weapons.push_back(new_wpn);
    } else if ((obj_id > MAX_WEAPON_ID) && (obj_id <= MAX_HEAD_ARMOR_ID)) {
        auto new_arm = std::make_shared<GlobalArmor>(obj_id, obj_count);
        new_object = new_arm;
        _inventory.insert(std::make_pair(obj_id, new_arm));
        _inventory_head_armors.push_back(new_arm);
    } else if ((obj_id > MAX_HEAD_ARMOR_ID) && (obj_id <= MAX_TORSO_ARMOR_ID)) {
        auto new_arm = std::make_shared<GlobalArmor>(obj_id, obj_count);
        new_object = new_arm;
        _inventory.insert(std::make_pair(obj_id, new_arm));
        _inventory_torso_armors.push_back(new_arm);
    } else if ((obj_id > MAX_TORSO_ARMOR_ID) && (obj_id <= MAX_ARM_ARMOR_ID)) {
        auto new_arm = std::make_shared<GlobalArmor>(obj_id, obj_count);
        new_object = new_arm;
        _inventory.insert(std::make_pair(obj_id, new_arm));
        _inventory_arm_armors.push_back(new_arm);
    } else if ((obj_id > MAX_ARM_ARMOR_ID) && (obj_id <= MAX_LEG_ARMOR_ID)) {
        auto new_arm = std::make_shared<GlobalArmor>(obj_id, obj_count);
        new_object = new_arm;
        _inventory.insert(std::make_pair(obj_id, new_arm));
        _inventory_leg_armors.push_back(new_arm);
    } else if ((obj_id > MAX_LEG_ARMOR_ID) && (obj_id <= MAX_SPIRIT_ID)) {
        auto new_spirit = std::make_shared<GlobalSpirit>(obj_id, obj_count);
        new_object = new_spirit;
        _inventory.insert(std::make_pair(obj_id, new_spirit));
        _inventory_spirits.push_back(new_spirit);
    } else {
        PRINT_WARNING << "attempted to add invalid object to inventory with id: " << obj_id << std::endl;
    }

    // Update the key items list.
    if (new_object != nullptr &&
        new_object->IsKeyItem()) {
        _inventory_key_items.push_back(new_object);
    }
}

void InventoryHandler::AddToInventory(const std::shared_ptr<GlobalObject>& object)
{
    // Don't process null object addition.
    if (object == nullptr) {
        return;
    }

    uint32_t obj_id = object->GetID();
    uint32_t obj_count = object->GetCount();

    // Don't add object instance without at least one actual item.
    if (obj_count == 0) {
        return;
    }

    // If an instance of the same object is already inside the inventory, just increment the count.
    if (_inventory.find(obj_id) != _inventory.end()) {
        _inventory[obj_id]->IncrementCount(obj_count);
        return;
    }

    // Figure out which type of object this is, cast it to the correct type, and add it to the inventory
    if((obj_id > 0 && obj_id <= MAX_ITEM_ID) ||
       (obj_id > MAX_SPIRIT_ID && obj_id <= MAX_KEY_ITEM_ID)) {
        auto new_obj = std::dynamic_pointer_cast<GlobalItem>(object);
        _inventory.insert(std::make_pair(obj_id, new_obj));
        _inventory_items.push_back(new_obj);
    } else if((obj_id > MAX_ITEM_ID) && (obj_id <= MAX_WEAPON_ID)) {
        auto new_obj = std::dynamic_pointer_cast<GlobalWeapon>(object);
        _inventory.insert(std::make_pair(obj_id, new_obj));
        _inventory_weapons.push_back(new_obj);
    } else if((obj_id > MAX_WEAPON_ID) && (obj_id <= MAX_HEAD_ARMOR_ID)) {
        auto new_obj = std::dynamic_pointer_cast<GlobalArmor>(object);
        _inventory.insert(std::make_pair(obj_id, new_obj));
        _inventory_head_armors.push_back(new_obj);
    } else if((obj_id > MAX_HEAD_ARMOR_ID) && (obj_id <= MAX_TORSO_ARMOR_ID)) {
        auto new_obj = std::dynamic_pointer_cast<GlobalArmor>(object);
        _inventory.insert(std::make_pair(obj_id, new_obj));
        _inventory_torso_armors.push_back(new_obj);
    } else if((obj_id > MAX_TORSO_ARMOR_ID) && (obj_id <= MAX_ARM_ARMOR_ID)) {
        auto new_obj = std::dynamic_pointer_cast<GlobalArmor>(object);
        _inventory.insert(std::make_pair(obj_id, new_obj));
        _inventory_arm_armors.push_back(new_obj);
    } else if((obj_id > MAX_ARM_ARMOR_ID) && (obj_id <= MAX_LEG_ARMOR_ID)) {
        auto new_obj = std::dynamic_pointer_cast<GlobalArmor>(object);
        _inventory.insert(std::make_pair(obj_id, new_obj));
        _inventory_leg_armors.push_back(new_obj);
    } else if((obj_id > MAX_LEG_ARMOR_ID) && (obj_id <= MAX_SPIRIT_ID)) {
        auto new_obj = std::dynamic_pointer_cast<GlobalSpirit>(object);
        _inventory.insert(std::make_pair(obj_id, new_obj));
        _inventory_spirits.push_back(new_obj);
    } else {
        PRINT_WARNING << "attempted to add invalid object to inventory with id: " << obj_id << std::endl;
        return;
    }

    // Updates the key items list.
    if (object->IsKeyItem()) {
        _inventory_key_items.push_back(object);
    }
}

void InventoryHandler::RemoveFromInventory(uint32_t obj_id)
{
    auto it = _inventory.find(obj_id);
    if (it == _inventory.end()) {
        PRINT_WARNING << "attempted to remove an object from inventory that didn't exist with id: " << obj_id << std::endl;
        return;
    }

    // Check whether the item is a key item to remove.
    auto object = it->second;
    if (object != nullptr && object->IsKeyItem()) {
        for (auto it2 = _inventory_key_items.begin(); it2 != _inventory_key_items.end(); ++it2) {

            if ((*it2)->GetID() != obj_id)
                continue;

            _inventory_key_items.erase(it2);
            break;
        }
    }

    // Use the id value to figure out what type of object it is, and remove it from the object vector
    if((obj_id > 0 && obj_id <= MAX_ITEM_ID) ||
       (obj_id > MAX_SPIRIT_ID && obj_id <= MAX_KEY_ITEM_ID)) {
        if(_RemoveFromInventory(obj_id, _inventory_items) == false)
            PRINT_WARNING << "object to remove was not found in inventory items: " << obj_id << std::endl;
    } else if((obj_id > MAX_ITEM_ID) && (obj_id <= MAX_WEAPON_ID)) {
        if(_RemoveFromInventory(obj_id, _inventory_weapons) == false)
            PRINT_WARNING << "object to remove was not found in inventory weapons: " << obj_id << std::endl;
    } else if((obj_id > MAX_WEAPON_ID) && (obj_id <= MAX_HEAD_ARMOR_ID)) {
        if(_RemoveFromInventory(obj_id, _inventory_head_armors) == false)
            PRINT_WARNING << "object to remove was not found in inventory head armor: " << obj_id << std::endl;
    } else if((obj_id > MAX_HEAD_ARMOR_ID) && (obj_id <= MAX_TORSO_ARMOR_ID)) {
        if(_RemoveFromInventory(obj_id, _inventory_torso_armors) == false)
            PRINT_WARNING << "object to remove was not found in inventory torso armor: " << obj_id << std::endl;
    } else if((obj_id > MAX_TORSO_ARMOR_ID) && (obj_id <= MAX_ARM_ARMOR_ID)) {
        if(_RemoveFromInventory(obj_id, _inventory_arm_armors) == false)
            PRINT_WARNING << "object to remove was not found in inventory arm armor: " << obj_id << std::endl;
    } else if((obj_id > MAX_ARM_ARMOR_ID) && (obj_id <= MAX_LEG_ARMOR_ID)) {
        if(_RemoveFromInventory(obj_id, _inventory_leg_armors) == false)
            PRINT_WARNING << "object to remove was not found in inventory leg armor: " << obj_id << std::endl;
    } else if((obj_id > MAX_LEG_ARMOR_ID) && (obj_id <= MAX_SPIRIT_ID)) {
        if(_RemoveFromInventory(obj_id, _inventory_spirits) == false)
            PRINT_WARNING << "object to remove was not found in inventory spirits: " << obj_id << std::endl;
    } else {
        PRINT_WARNING << "attempted to remove an object from inventory with an invalid id: " << obj_id << std::endl;
    }
}

std::shared_ptr<GlobalObject> InventoryHandler::GetGlobalObject(uint32_t obj_id)
{
    if (_inventory.find(obj_id) == _inventory.end()) {
        return nullptr;
    }

    std::shared_ptr<GlobalObject> return_object = nullptr;
    // Use the id value to figure out what type of object it is, and remove it from the object vector
    if((obj_id > 0 && obj_id <= MAX_ITEM_ID) ||
       (obj_id > MAX_SPIRIT_ID && obj_id <= MAX_KEY_ITEM_ID)) {
        return_object = _GetFromInventory(obj_id, _inventory_items);
        if(return_object == nullptr)
            PRINT_WARNING << "object to retrieve was not found in inventory items: " << obj_id << std::endl;
    } else if((obj_id > MAX_ITEM_ID) && (obj_id <= MAX_WEAPON_ID)) {
        return_object = _GetFromInventory(obj_id, _inventory_weapons);
        if(return_object == nullptr)
            PRINT_WARNING << "object to retrieve was not found in inventory weapons: " << obj_id << std::endl;
    } else if((obj_id > MAX_WEAPON_ID) && (obj_id <= MAX_HEAD_ARMOR_ID)) {
        return_object = _GetFromInventory(obj_id, _inventory_head_armors);
        if(return_object == nullptr)
            PRINT_WARNING << "object to retrieve was not found in inventory head armor: " << obj_id << std::endl;
    } else if((obj_id > MAX_HEAD_ARMOR_ID) && (obj_id <= MAX_TORSO_ARMOR_ID)) {
        return_object = _GetFromInventory(obj_id, _inventory_torso_armors);
        if(return_object == nullptr)
            PRINT_WARNING << "object to retrieve was not found in inventory torso armor: " << obj_id << std::endl;
    } else if((obj_id > MAX_TORSO_ARMOR_ID) && (obj_id <= MAX_ARM_ARMOR_ID)) {
        return_object = _GetFromInventory(obj_id, _inventory_arm_armors);
        if(return_object == nullptr)
            PRINT_WARNING << "object to retrieve was not found in inventory arm armor: " << obj_id << std::endl;
    } else if((obj_id > MAX_ARM_ARMOR_ID) && (obj_id <= MAX_LEG_ARMOR_ID)) {
        return_object = _GetFromInventory(obj_id, _inventory_leg_armors);
        if(return_object == nullptr)
            PRINT_WARNING << "object to retrieve was not found in inventory leg armor: " << obj_id << std::endl;
    } else if((obj_id > MAX_LEG_ARMOR_ID) && (obj_id <= MAX_SPIRIT_ID)) {
        return_object = _GetFromInventory(obj_id, _inventory_spirits);
        if(return_object == nullptr)
            PRINT_WARNING << "object to retrieve was not found in inventory spirits: " << obj_id << std::endl;
    } else {
        PRINT_WARNING << "attempted to retrieve an object from inventory with an invalid id: " << obj_id << std::endl;
    }

    return return_object;
}

void InventoryHandler::IncrementItemCount(uint32_t obj_id, uint32_t count)
{
    // Do nothing if the item does not exist in the inventory
    if(_inventory.find(obj_id) == _inventory.end()) {
        PRINT_WARNING << "attempted to increment count for an object that was not present in the inventory: " << obj_id << std::endl;
        return;
    }

    _inventory[obj_id]->IncrementCount(count);
}

void InventoryHandler::DecrementItemCount(uint32_t obj_id, uint32_t count)
{
    // Do nothing if the item does not exist in the inventory
    if(_inventory.find(obj_id) == _inventory.end()) {
        PRINT_WARNING << "attempted to decrement count for an object that was not present in the inventory: " << obj_id << std::endl;
        return;
    }

    // Print a warning if the amount to decrement by exceeds the object's current count
    if(count > _inventory[obj_id]->GetCount()) {
        PRINT_WARNING << "amount to decrement count by exceeded available count: " << obj_id << std::endl;
    }

    // Decrement the number of objects so long as the number to decrement by does not equal or exceed the count
    if(count < _inventory[obj_id]->GetCount())
        _inventory[obj_id]->DecrementCount(count);
    // Otherwise remove the object from the inventory completely
    else
        RemoveFromInventory(obj_id);
}

void InventoryHandler::SaveInventory(vt_script::WriteScriptDescriptor& file)
{
    // Save the inventory (object id + object count pairs)
    // NOTE: This does not save any weapons/armor that are equipped on the characters. That data
    // is stored alongside the character data when it is saved
    _SaveInventory(file, "items", _inventory_items);
    _SaveInventory(file, "weapons", _inventory_weapons);
    _SaveInventory(file, "head_armor", _inventory_head_armors);
    _SaveInventory(file, "torso_armor", _inventory_torso_armors);
    _SaveInventory(file, "arm_armor", _inventory_arm_armors);
    _SaveInventory(file, "leg_armor", _inventory_leg_armors);
    _SaveInventory(file, "spirits", _inventory_spirits);
}

void InventoryHandler::LoadInventory(vt_script::ReadScriptDescriptor& file)
{
    ClearAllData();

    _LoadInventory(file, "items");
    _LoadInventory(file, "weapons");
    _LoadInventory(file, "head_armor");
    _LoadInventory(file, "torso_armor");
    _LoadInventory(file, "arm_armor");
    _LoadInventory(file, "leg_armor");
    _LoadInventory(file, "spirits");
}

void InventoryHandler::_LoadInventory(ReadScriptDescriptor& file, const std::string& category_name)
{
    if(file.IsFileOpen() == false) {
        PRINT_WARNING << "the file provided in the function argument was not open" << std::endl;
        return;
    }

    // The table keys are the inventory object ID numbers. The value of each key is the count of that object
    if (file.OpenTable(category_name)) {
        std::vector<uint32_t> object_ids;
        file.ReadTableKeys(object_ids);
        for(uint32_t i = 0; i < object_ids.size(); i++) {
            AddToInventory(object_ids[i], file.ReadUInt(object_ids[i]));
        }
        file.CloseTable();
    }
}

} // namespace vt_global
