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

#include "script.h"
#include "video.h"

#include "global_objects.h"
#include "global_effects.h"
#include "global.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_script;
using namespace hoa_video;

using namespace hoa_global::private_global;

namespace hoa_global {

////////////////////////////////////////////////////////////////////////////////
// GlobalObject class
////////////////////////////////////////////////////////////////////////////////

void GlobalObject::_LoadObjectData(hoa_script::ReadScriptDescriptor& script) {
	_name = MakeUnicodeString(script.ReadString("name"));
	_description = MakeUnicodeString(script.ReadString("description"));
	_price = script.ReadUInt("standard_price");
	string icon_file = script.ReadString("icon");
	if (_icon_image.Load(icon_file) == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to load icon image for item: " << _id << endl;
		_InvalidateObject();
	}
}

////////////////////////////////////////////////////////////////////////////////
// GlobalItem class
////////////////////////////////////////////////////////////////////////////////

GlobalItem::GlobalItem(uint32 id, uint32 count) :
	GlobalObject(id, count),
	_target_type(GLOBAL_TARGET_INVALID),
	_battle_use_function(NULL),
	_field_use_function(NULL)
{
	if ((_id == 0) || (_id > MAX_ITEM_ID)) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << endl;
		_InvalidateObject();
		return;
	}

	ReadScriptDescriptor& script_file = GlobalManager->GetItemsScript();
	if (script_file.DoesTableExist(_id) == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "no valid data for item in definition file: " << _id << endl;
		_InvalidateObject();
		return;
	}

	// Load the item data from the script
	script_file.OpenTable(_id);
	_LoadObjectData(script_file);

	_target_type = static_cast<GLOBAL_TARGET>(script_file.ReadInt("target_type"));
	if (script_file.DoesFunctionExist("BattleUse") == true) {
		_battle_use_function = new ScriptObject();
		*_battle_use_function = script_file.ReadFunctionPointer("BattleUse");
	}
	if (script_file.DoesFunctionExist("FieldUse") == true) {
		_field_use_function = new ScriptObject();
		*_field_use_function = script_file.ReadFunctionPointer("FieldUse");
	}


	script_file.CloseTable();
	if (script_file.IsErrorDetected()) {
		if (GLOBAL_DEBUG) {
			PRINT_WARNING << "one or more errors occurred while reading item data - they are listed below" << endl;
			cerr << script_file.GetErrorMessages() << endl;
		}
		_InvalidateObject();
	}
} // void GlobalItem::GlobalItem(uint32 id, uint32 count = 1)



GlobalItem::~GlobalItem() {
	if (_battle_use_function != NULL) {
		delete _battle_use_function;
		_battle_use_function = NULL;
	}
	if (_field_use_function != NULL) {
		delete _field_use_function;
		_field_use_function = NULL;
	}
}



GlobalItem::GlobalItem(const GlobalItem& copy) :
	GlobalObject(copy)
{
	_target_type = copy._target_type;

	// Make copies of valid ScriptObject function pointers
	if (copy._battle_use_function == NULL)
		_battle_use_function = NULL;
	else
		_battle_use_function = new ScriptObject(*copy._battle_use_function);

	if (copy._field_use_function == NULL)
		_field_use_function = NULL;
	else
		_field_use_function = new ScriptObject(*copy._field_use_function);
}



GlobalItem& GlobalItem::operator=(const GlobalItem& copy) {
	if (this == &copy) // Handle self-assignment case
		return *this;

	GlobalObject::operator=(copy);
	_target_type = copy._target_type;

	// Make copies of valid ScriptObject function pointers
	if (copy._battle_use_function == NULL)
		_battle_use_function = NULL;
	else
		_battle_use_function = new ScriptObject(*copy._battle_use_function);

	if (copy._field_use_function == NULL)
		_field_use_function = NULL;
	else
		_field_use_function = new ScriptObject(*copy._field_use_function);

	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// GlobalWeapon class
////////////////////////////////////////////////////////////////////////////////

GlobalWeapon::GlobalWeapon(uint32 id, uint32 count) :
	GlobalObject(id, count)
{
	// Initialize all elemental effects as neutral
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_FIRE, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_WATER, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_VOLT, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_EARTH, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_SLICING, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_SMASHING, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_MAULING, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_PIERCING, GLOBAL_INTENSITY_NEUTRAL));

	if ((_id <= MAX_ITEM_ID) || (_id > MAX_WEAPON_ID)) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << endl;
		_InvalidateObject();
		return;
	}

	ReadScriptDescriptor& script_file = GlobalManager->GetWeaponsScript();
	if (script_file.DoesTableExist(_id) == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "no valid data for weapon in definition file: " << _id << endl;
		_InvalidateObject();
		return;
	}

	// Load the weapon data from the script
	script_file.OpenTable(_id);
	_LoadObjectData(script_file);

	_physical_attack = script_file.ReadUInt("physical_attack");
	_metaphysical_attack = script_file.ReadUInt("metaphysical_attack");
	_usable_by = script_file.ReadUInt("usable_by");

	script_file.CloseTable();
	if (script_file.IsErrorDetected()) {
		if (GLOBAL_DEBUG) {
			PRINT_WARNING << "one or more errors occurred while reading weapon data - they are listed below" << endl;
			cerr << script_file.GetErrorMessages() << endl;
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
	// Initialize all elemental effects as neutral
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_FIRE, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_WATER, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_VOLT, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_EARTH, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_SLICING, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_SMASHING, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_MAULING, GLOBAL_INTENSITY_NEUTRAL));
	_elemental_effects.insert(pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>(GLOBAL_ELEMENTAL_PIERCING, GLOBAL_INTENSITY_NEUTRAL));

	if ((_id <= MAX_WEAPON_ID) || (_id > MAX_LEG_ARMOR_ID)) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << endl;
		_InvalidateObject();
		return;
	}

	// Figure out the appropriate script reference to grab based on the id value
	ReadScriptDescriptor* script_file;
	switch (GetObjectType()) {
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
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "could not determine armor type: " << _id << endl;
			_InvalidateObject();
			return;
	}

	if (script_file->DoesTableExist(_id) == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "no valid data for armor in definition file: " << _id << endl;
		_InvalidateObject();
		return;
	}

	// Load the armor data from the script
	script_file->OpenTable(_id);
	_LoadObjectData(*script_file);

	_physical_defense = script_file->ReadUInt("physical_defense");
	_metaphysical_defense = script_file->ReadUInt("metaphysical_defense");
	_usable_by = script_file->ReadUInt("usable_by");

	script_file->CloseTable();
	if (script_file->IsErrorDetected()) {
		if (GLOBAL_DEBUG) {
			PRINT_WARNING << "one or more errors occurred while reading armor data - they are listed below" << endl;
			cerr << script_file->GetErrorMessages() << endl;
		}
		_InvalidateObject();
	}
} // void GlobalArmor::GlobalArmor(uint32 id, uint32 count = 1)



GLOBAL_OBJECT GlobalArmor::GetObjectType() const {
	if ((_id > MAX_WEAPON_ID) && (_id <= MAX_HEAD_ARMOR_ID))
		return GLOBAL_OBJECT_HEAD_ARMOR;
	else if ((_id > MAX_HEAD_ARMOR_ID) && (_id <= MAX_TORSO_ARMOR_ID))
		return GLOBAL_OBJECT_TORSO_ARMOR;
	else if ((_id > MAX_TORSO_ARMOR_ID) && (_id <= MAX_ARM_ARMOR_ID))
		return GLOBAL_OBJECT_ARM_ARMOR;
	else if ((_id > MAX_ARM_ARMOR_ID) && (_id <= MAX_LEG_ARMOR_ID))
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
	if ((_id <= MAX_LEG_ARMOR_ID) || (_id > MAX_SHARD_ID)) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << endl;
		_InvalidateObject();
		return;
	}

	// TODO: uncomment the code below when shards scripts are available
// 	ReadScriptDescriptor& script_file = GlobalManager->GetShardsScript();
// 	if (script_file.DoesTableExist(_id) == false) {
// 		IF_PRINT_WARNING(GLOBAL_DEBUG) << "no valid data for shard in definition file: " << _id << endl;
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
// 			PRINT_WARNING << "one or more errors occurred while reading shard data - they are listed below" << endl;
// 			cerr << script_file.GetErrorMessages() << endl;
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
	if ((_id <= MAX_SHARD_ID) || (_id > MAX_KEY_ITEM_ID)) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << endl;
		_InvalidateObject();
		return;
	}

	// TODO: uncomment the code below when key item scripts are available
// 	ReadScriptDescriptor& script_file = GlobalManager->GetKeyItemsScript();
// 	if (script_file.DoesTableExist(_id) == false) {
// 		IF_PRINT_WARNING(GLOBAL_DEBUG) << "no valid data for key item in definition file: " << _id << endl;
// 		_InvalidateObject();
// 		return;
// 	}
//
// 	// Load the key item data from the script
// 	script_file.OpenTable(_id);
// 	_LoadObjectData(script_file);
//
// 	script_file.CloseTable();
// 	if (script_file.IsErrorDetected()) {
// 		if (GLOBAL_DEBUG) {
// 			PRINT_WARNING << "one or more errors occurred while reading key item data - they are listed below" << endl;
// 			cerr << script_file.GetErrorMessages() << endl;
// 		}
// 		_InvalidateObject();
// 	}
} // void GlobalKeyItem::GlobalKeyItem(uint32 id, uint32 count = 1)

} // namespace hoa_global
