////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
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

using namespace vt_utils;
using namespace vt_script;
using namespace vt_video;
using namespace vt_global::private_global;

namespace vt_global
{

////////////////////////////////////////////////////////////////////////////////
// GlobalObject class
////////////////////////////////////////////////////////////////////////////////

void GlobalObject::_LoadObjectData(vt_script::ReadScriptDescriptor &script)
{
    _name = MakeUnicodeString(script.ReadString("name"));
    _description = MakeUnicodeString(script.ReadString("description"));
    _price = script.ReadUInt("standard_price");
    _LoadTradeConditions(script);
    std::string icon_file = script.ReadString("icon");
    if (script.DoesBoolExist("key_item"))
        _is_key_item = script.ReadBool("key_item");
    if(!_icon_image.Load(icon_file)) {
        PRINT_WARNING << "failed to load icon image for item: " << _id << std::endl;

        // try a default icon in that case
        _icon_image.Load("img/icons/battle/default_special.png");
    }
}

void GlobalObject::_LoadElementalEffects(vt_script::ReadScriptDescriptor &script)
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

void GlobalObject::_LoadStatusEffects(vt_script::ReadScriptDescriptor &script)
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

        // Check whether an opposite effect exists.
        bool effect_replaced = false;
        for (uint32 j = 0; j < _status_effects.size(); ++j) {
            GLOBAL_STATUS opposite_effect = GetOppositeStatusEffect((GLOBAL_STATUS) key);
            if (_status_effects[j].first != opposite_effect)
                continue;

            PRINT_WARNING << "The item (id:" << _id << ") has opposing passive status effects." << std::endl;
            effect_replaced = true;
            break;
        }

        if (effect_replaced)
            continue;

        _status_effects.push_back(std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY>((GLOBAL_STATUS)key, (GLOBAL_INTENSITY)intensity));
    }

    script.CloseTable(); // status_effects
}

void GlobalObject::_LoadTradeConditions(vt_script::ReadScriptDescriptor &script)
{
    if(!script.DoesTableExist("trade_conditions"))
        return;

    std::vector<uint32> temp;
    script.ReadTableKeys("trade_conditions", temp);

    if(temp.empty())
        return;

    script.OpenTable("trade_conditions");

    for(uint32 i = 0; i < temp.size(); ++i) {
        uint32 key = temp[i];
        uint32 quantity = script.ReadInt(key);

        // Set the trade price
        if (key == 0)
            _trade_price = quantity;
        else // Or the conditions.
            _trade_conditions.push_back(std::pair<uint32, uint32>(key, quantity));
    }

    script.CloseTable(); // trade_conditions

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
    if(_id == 0 || (_id > MAX_ITEM_ID && (_id <= MAX_SHARD_ID && _id > MAX_KEY_ITEM_ID))) {
        PRINT_WARNING << "invalid id in constructor: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    ReadScriptDescriptor &script_file = GlobalManager->GetItemsScript();
    if(script_file.DoesTableExist(_id) == false) {
        PRINT_WARNING << "no valid data for item in definition file: " << _id << std::endl;
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
        PRINT_WARNING << "one or more errors occurred while reading item data - they are listed below"
                        << std::endl << script_file.GetErrorMessages() << std::endl;
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
    _magical_attack = script_file.ReadUInt("magical_attack");
    _usable_by = script_file.ReadUInt("usable_by");

    uint32 shards_number = script_file.ReadUInt("slots");
    // Only permit a max of 5 shards for equipment
    if (shards_number > 5) {
        shards_number = 5;
        PRINT_WARNING << "More than 5 shards declared in item " << _id << std::endl;
    }
    _shard_slots.resize(shards_number, NULL);

    // Load the possible battle ammo animated image filename.
    _ammo_image_file = script_file.ReadString("battle_ammo_animation_file");

    // Load the weapon battle animation info
    if (script_file.DoesTableExist("battle_animations"))
        _LoadWeaponBattleAnimations(script_file);

    script_file.CloseTable(); // id
    if(script_file.IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading weapon data - they are listed below"
                          << std::endl << script_file.GetErrorMessages() << std::endl;
        }
        _InvalidateObject();
    }
} // void GlobalWeapon::GlobalWeapon(uint32 id, uint32 count = 1)

const std::string& GlobalWeapon::GetWeaponAnimationFile(uint32 character_id, const std::string& animation_alias)
{
    if (_weapon_animations.find(character_id) == _weapon_animations.end())
        return _empty_string;

    const std::map<std::string, std::string>& char_map = _weapon_animations.at(character_id);
    if (char_map.find(animation_alias) == char_map.end())
        return _empty_string;

    return char_map.at(animation_alias);
}

void GlobalWeapon::_LoadWeaponBattleAnimations(ReadScriptDescriptor& script)
{
    //std::map <uint32, std::map<std::string, std::string> > _weapon_animations;
    _weapon_animations.clear();

    // The character id keys
    std::vector<uint32> char_ids;

    script.ReadTableKeys("battle_animations", char_ids);
    if (char_ids.empty())
        return;

    if (!script.OpenTable("battle_animations"))
        return;

    for (uint32 i = 0; i < char_ids.size(); ++i) {
        uint32 char_id = char_ids[i];

        // Read all the animation aliases
        std::vector<std::string> anim_aliases;
        script.ReadTableKeys(char_id, anim_aliases);

        if (anim_aliases.empty())
            continue;

        if (!script.OpenTable(char_id))
            continue;

        for (uint32 j = 0; j < anim_aliases.size(); ++j) {
            std::string anim_alias = anim_aliases[j];
            std::string anim_file = script.ReadString(anim_alias);
            _weapon_animations[char_id].insert(std::make_pair<std::string, std::string>(anim_alias, anim_file));
        }

        script.CloseTable(); // char_id
    }

    script.CloseTable(); // battle_animations
}

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
    _magical_defense = script_file->ReadUInt("magical_defense");
    _usable_by = script_file->ReadUInt("usable_by");

    uint32 shards_number = script_file->ReadUInt("slots");
    // Only permit a max of 5 shards for equipment
    if (shards_number > 5) {
        shards_number = 5;
        PRINT_WARNING << "More than 5 shards declared in item " << _id << std::endl;
    }
    _shard_slots.resize(shards_number, NULL);

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

} // namespace vt_global
