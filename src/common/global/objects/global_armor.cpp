////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_armor.h"

#include "common/global/global.h"

using namespace vt_utils;
using namespace vt_script;

namespace vt_global
{

GlobalArmor::GlobalArmor(uint32_t id, uint32_t count) :
    GlobalObject(id, count)
{
    if((_id <= MAX_WEAPON_ID) || (_id > MAX_LEG_ARMOR_ID)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    // Figure out the appropriate script reference to grab based on the id value
    ReadScriptDescriptor* script_file = nullptr;
    InventoryHandler& inventory = GlobalManager->GetInventoryHandler();
    switch(GetObjectType()) {
    case GLOBAL_OBJECT_HEAD_ARMOR:
        script_file = &(inventory.GetHeadArmorScript());
        break;
    case GLOBAL_OBJECT_TORSO_ARMOR:
        script_file = &(inventory.GetTorsoArmorScript());
        break;
    case GLOBAL_OBJECT_ARM_ARMOR:
        script_file = &(inventory.GetArmArmorScript());
        break;
    case GLOBAL_OBJECT_LEG_ARMOR:
        script_file = &(inventory.GetLegArmorScript());
        break;
    default:
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "could not determine armor type: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    if(script_file->DoesTableExist(_id) == false) {
        PRINT_WARNING << "no valid data for armor in definition file: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    // Load the armor data from the script
    script_file->OpenTable(_id);
    _LoadObjectData(*script_file);

    _LoadStatusEffects(*script_file);
    _LoadEquipmentSkills(*script_file);

    _physical_defense = script_file->ReadUInt("physical_defense");
    _magical_defense = script_file->ReadUInt("magical_defense");

    _usable_by = script_file->ReadUInt("usable_by");

    uint32_t spirits_number = script_file->ReadUInt("slots");
    // Only permit a max of 5 spirits for equipment
    if (spirits_number > 5) {
        spirits_number = 5;
        PRINT_WARNING << "More than 5 spirit slots declared in item " << _id << std::endl;
    }
    _spirit_slots.resize(spirits_number, nullptr);

    script_file->CloseTable();
    if(script_file->IsErrorDetected()) {
        PRINT_WARNING << "one or more errors occurred while reading armor data - they are listed below"
                      << std::endl << script_file->GetErrorMessages() << std::endl;
        _InvalidateObject();
    }
}

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

} // namespace vt_global
