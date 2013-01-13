////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_objects.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for global game objects
*** ***************************************************************************/

#include "global_objects.h"
#include "global.h"

#include "engine/script/script.h"
#include "engine/video/video.h"

using namespace hoa_utils;
using namespace hoa_script;
using namespace hoa_video;
using namespace hoa_global::private_global;

namespace hoa_global
{

////////////////////////////////////////////////////////////////////////////////
// GlobalObject class
////////////////////////////////////////////////////////////////////////////////

void GlobalObject::_LoadObjectData(hoa_script::ReadScriptDescriptor &script)
{
    _name = MakeUnicodeString(script.ReadString("name"));
    _description = MakeUnicodeString(script.ReadString("description"));
    _price = script.ReadUInt("standard_price");
    _LoadTradeConditions(script);
    std::string icon_file = script.ReadString("icon");
    if(_icon_image.Load(icon_file) == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to load icon image for item: " << _id << std::endl;
        _InvalidateObject();
    }
}

void GlobalObject::_LoadElementalEffects(hoa_script::ReadScriptDescriptor &script)
{
    if(!script.DoesTableExist("elemental_effects"))
        return;

    std::vector<int32> elemental_effects;
    script.ReadTableKeys("elemental_effects", elemental_effects);

    if(elemental_effects.empty())
        return;

    script.OpenTable("elemental_effects");

    for(uint32 i = 0; i < elemental_effects.size(); ++i) {

        int32 key = elemental_effects[i];
        if(key <= GLOBAL_ELEMENTAL_INVALID || key >= GLOBAL_ELEMENTAL_TOTAL)
            continue;

        int32 intensity = script.ReadInt(key);
        if(intensity <= GLOBAL_INTENSITY_INVALID || intensity >= GLOBAL_INTENSITY_TOTAL)
            continue;

        _elemental_effects.push_back(std::pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>((GLOBAL_ELEMENTAL)key, (GLOBAL_INTENSITY)intensity));
    }

    script.CloseTable(); // elemental_effects
}

void GlobalObject::_LoadStatusEffects(hoa_script::ReadScriptDescriptor &script)
{
    if(!script.DoesTableExist("status_effects"))
        return;

    std::vector<int32> status_effects;
    script.ReadTableKeys("status_effects", status_effects);

    if(status_effects.empty())
        return;

    script.OpenTable("status_effects");

    for(uint32 i = 0; i < status_effects.size(); ++i) {

        int32 key = status_effects[i];
        if(key <= GLOBAL_STATUS_INVALID || key >= GLOBAL_STATUS_TOTAL)
            continue;

        int32 intensity = script.ReadInt(key);
        // Note: The intensity of a status effect can only be positive
        if(intensity < GLOBAL_INTENSITY_NEUTRAL || intensity >= GLOBAL_INTENSITY_TOTAL)
            continue;

        _status_effects.push_back(std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY>((GLOBAL_STATUS)key, (GLOBAL_INTENSITY)intensity));
    }

    script.CloseTable(); // status_effects
}

void GlobalObject::_LoadTradeConditions(hoa_script::ReadScriptDescriptor &script)
{
    std::vector<uint32> temp;

    if(!script.DoesTableExist("trade_conditions")) {
        return;
    }

    script.ReadTableKeys("trade_conditions", temp);

    if(temp.empty()) {
        return;
    }

    script.OpenTable("trade_conditions");

    for(uint32 i = 0; i < temp.size(); ++i) {
        uint32 key = temp[i];
        uint32 quantity = script.ReadInt(key);
        //_trade_conditions.push_back(key);
        _trade_conditions.push_back(std::pair<uint32, uint32>(key, quantity));
    }

    script.CloseTable(); // status_effects

    return;
}

////////////////////////////////////////////////////////////////////////////////
// GlobalItem class
////////////////////////////////////////////////////////////////////////////////

GlobalItem::GlobalItem(uint32 id, uint32 count) :
    GlobalObject(id, count),
    _target_type(GLOBAL_TARGET_INVALID),
    _warmup_time(0),
    _cooldown_time(0)
{
    if((_id == 0) || (_id > MAX_ITEM_ID)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    ReadScriptDescriptor &script_file = GlobalManager->GetItemsScript();
    if(script_file.DoesTableExist(_id) == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "no valid data for item in definition file: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    // Load the item data from the script
    script_file.OpenTable(_id);
    _LoadObjectData(script_file);

    _target_type = static_cast<GLOBAL_TARGET>(script_file.ReadInt("target_type"));
    _warmup_time = script_file.ReadUInt("warmup_time");
    _cooldown_time = script_file.ReadUInt("cooldown_time");

    _battle_use_function = script_file.ReadFunctionPointer("BattleUse");
    _field_use_function = script_file.ReadFunctionPointer("FieldUse");

    script_file.CloseTable();
    if(script_file.IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading item data - they are listed below"
                          << std::endl << script_file.GetErrorMessages() << std::endl;
        }
        _InvalidateObject();
    }
} // void GlobalItem::GlobalItem(uint32 id, uint32 count = 1)

GlobalItem::GlobalItem(const GlobalItem &copy) :
    GlobalObject(copy)
{
    _target_type = copy._target_type;
    _warmup_time = copy._warmup_time;
    _cooldown_time = copy._cooldown_time;

    // Make copies of valid ScriptObject function pointers
    _battle_use_function = copy._battle_use_function;
    _field_use_function = copy._field_use_function;
}



GlobalItem &GlobalItem::operator=(const GlobalItem &copy)
{
    if(this == &copy)  // Handle self-assignment case
        return *this;

    GlobalObject::operator=(copy);
    _target_type = copy._target_type;
    _warmup_time = copy._warmup_time;
    _cooldown_time = copy._cooldown_time;

    // Make copies of valid ScriptObject function pointers
    _battle_use_function = copy._battle_use_function;
    _field_use_function = copy._field_use_function;

    return *this;
}

////////////////////////////////////////////////////////////////////////////////
// GlobalWeapon class
////////////////////////////////////////////////////////////////////////////////

GlobalWeapon::GlobalWeapon(uint32 id, uint32 count) :
    GlobalObject(id, count)
{
    if((_id <= MAX_ITEM_ID) || (_id > MAX_WEAPON_ID)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    ReadScriptDescriptor &script_file = GlobalManager->GetWeaponsScript();
    if(script_file.DoesTableExist(_id) == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "no valid data for weapon in definition file: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    // Load the weapon data from the script
    script_file.OpenTable(_id);
    _LoadObjectData(script_file);

    _LoadElementalEffects(script_file);
    _LoadStatusEffects(script_file);

    _physical_attack = script_file.ReadUInt("physical_attack");
    _metaphysical_attack = script_file.ReadUInt("metaphysical_attack");
    _usable_by = script_file.ReadUInt("usable_by");

    uint32 shards_number = script_file.ReadUInt("slots");
    _shard_slots.resize(shards_number, NULL);
    // TODO: Load equipped shards data

    // Load the possible battle ammo animated image filename.
    _ammo_image_file = script_file.ReadString("battle_ammo_animation_file");

    script_file.CloseTable();
    if(script_file.IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading weapon data - they are listed below"
                          << std::endl << script_file.GetErrorMessages() << std::endl;
        }
        _InvalidateObject();
    }
} // void GlobalWeapon::GlobalWeapon(uint32 id, uint32 count = 1)

////////////////////////////////////////////////////////////////////////////////
// GlobalArmor class
////////////////////////////////////////////////////////////////////////////////

GlobalArmor::GlobalArmor(uint32 id, uint32 count) :
    GlobalObject(id, count)
{
    if((_id <= MAX_WEAPON_ID) || (_id > MAX_LEG_ARMOR_ID)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    // Figure out the appropriate script reference to grab based on the id value
    ReadScriptDescriptor *script_file;
    switch(GetObjectType()) {
    case GLOBAL_OBJECT_HEAD_ARMOR:
        script_file = &(GlobalManager->GetHeadArmorScript());
        break;
    case GLOBAL_OBJECT_TORSO_ARMOR:
        script_file = &(GlobalManager->GetTorsoArmorScript());
        break;
    case GLOBAL_OBJECT_ARM_ARMOR:
        script_file = &(GlobalManager->GetArmArmorScript());
        break;
    case GLOBAL_OBJECT_LEG_ARMOR:
        script_file = &(GlobalManager->GetLegArmorScript());
        break;
    default:
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "could not determine armor type: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    if(script_file->DoesTableExist(_id) == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "no valid data for armor in definition file: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    // Load the armor data from the script
    script_file->OpenTable(_id);
    _LoadObjectData(*script_file);

    _LoadElementalEffects(*script_file);
    _LoadStatusEffects(*script_file);

    _physical_defense = script_file->ReadUInt("physical_defense");
    _metaphysical_defense = script_file->ReadUInt("metaphysical_defense");
    _usable_by = script_file->ReadUInt("usable_by");

    uint32 shards_number = script_file->ReadUInt("slots");
    _shard_slots.resize(shards_number, NULL);
    // TODO: Load equipped shards data

    script_file->CloseTable();
    if(script_file->IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading armor data - they are listed below"
                          << std::endl << script_file->GetErrorMessages() << std::endl;
        }
        _InvalidateObject();
    }
} // void GlobalArmor::GlobalArmor(uint32 id, uint32 count = 1)



GLOBAL_OBJECT GlobalArmor::GetObjectType() const
{
    if((_id > MAX_WEAPON_ID) && (_id <= MAX_HEAD_ARMOR_ID))
        return GLOBAL_OBJECT_HEAD_ARMOR;
    else if((_id > MAX_HEAD_ARMOR_ID) && (_id <= MAX_TORSO_ARMOR_ID))
        return GLOBAL_OBJECT_TORSO_ARMOR;
    else if((_id > MAX_TORSO_ARMOR_ID) && (_id <= MAX_ARM_ARMOR_ID))
        return GLOBAL_OBJECT_ARM_ARMOR;
    else if((_id > MAX_ARM_ARMOR_ID) && (_id <= MAX_LEG_ARMOR_ID))
        return GLOBAL_OBJECT_LEG_ARMOR;
    else
        return GLOBAL_OBJECT_INVALID;
}

////////////////////////////////////////////////////////////////////////////////
// GlobalShard class
////////////////////////////////////////////////////////////////////////////////

GlobalShard::GlobalShard(uint32 id, uint32 count) :
    GlobalObject(id, count)
{
    if((_id <= MAX_LEG_ARMOR_ID) || (_id > MAX_SHARD_ID)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    // TODO: uncomment the code below when shards scripts are available
// 	ReadScriptDescriptor& script_file = GlobalManager->GetShardsScript();
// 	if (script_file.DoesTableExist(_id) == false) {
// 		IF_PRINT_WARNING(GLOBAL_DEBUG) << "no valid data for shard in definition file: " << _id << std::endl;
// 		_InvalidateObject();
// 		return;
// 	}
//
// 	// Load the shard data from the script
// 	script_file.OpenTable(_id);
// 	_LoadObjectData(script_file);
//
// 	script_file.CloseTable();
// 	if (script_file.IsErrorDetected()) {
// 		if (GLOBAL_DEBUG) {
// 			PRINT_WARNING << "one or more errors occurred while reading shard data - they are listed below" << std::endl;
// 			cerr << script_file.GetErrorMessages() << std::endl;
// 		}
// 		_InvalidateObject();
// 	}
} // void GlobalShard::GlobalShard(uint32 id, uint32 count = 1)

////////////////////////////////////////////////////////////////////////////////
// GlobalKeyItem class
////////////////////////////////////////////////////////////////////////////////

GlobalKeyItem::GlobalKeyItem(uint32 id, uint32 count) :
    GlobalObject(id, count)
{
    if((_id <= MAX_SHARD_ID) || (_id > MAX_KEY_ITEM_ID)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    ReadScriptDescriptor &script_file = GlobalManager->GetKeyItemsScript();
    if(script_file.DoesTableExist(_id) == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "no valid data for key item in definition file: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    // Load the item data from the script
    script_file.OpenTable(_id);
    _LoadObjectData(script_file);

    script_file.CloseTable();
    if(script_file.IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading key item data - they are listed below"
                          << std::endl << script_file.GetErrorMessages() << std::endl;
        }
        _InvalidateObject();
    }
} // void GlobalKeyItem::GlobalKeyItem(uint32 id, uint32 count = 1)

} // namespace hoa_global
