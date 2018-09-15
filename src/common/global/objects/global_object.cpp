////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_object.h"

#include "global_item.h"
#include "global_weapon.h"
#include "global_armor.h"
#include "global_spirit.h"

#include "script/script_read.h"

namespace vt_global
{

std::shared_ptr<GlobalObject> GlobalCreateNewObject(uint32_t id, uint32_t count)
{
    std::shared_ptr<GlobalObject> new_object = nullptr;

    if ((id > 0 && id <= MAX_ITEM_ID) ||
        (id > MAX_SPIRIT_ID && id <= MAX_KEY_ITEM_ID))
        new_object = std::make_shared<GlobalItem>(id, count);
    else if ((id > MAX_ITEM_ID) && (id <= MAX_WEAPON_ID))
        new_object = std::make_shared<GlobalWeapon>(id, count);
    else if ((id > MAX_WEAPON_ID) && (id <= MAX_LEG_ARMOR_ID))
        new_object = std::make_shared<GlobalArmor>(id, count);
    else if ((id > MAX_LEG_ARMOR_ID) && (id <= MAX_SPIRIT_ID))
        new_object = std::make_shared<GlobalSpirit>(id, count);
    else
        PRINT_WARNING << "function received an invalid id argument: " << id << std::endl;

    // If an object was created but its ID was set to nullptr, this indicates that the object is invalid
    if ((new_object != nullptr) &&
        (new_object->GetID() == 0)) {
        new_object = nullptr;
    }

    return new_object;
}

void GlobalObject::_LoadObjectData(vt_script::ReadScriptDescriptor &script)
{
    _name = vt_utils::MakeUnicodeString(script.ReadString("name"));
    _description = vt_utils::MakeUnicodeString(script.ReadString("description"));
    _price = script.ReadUInt("standard_price");
    _LoadTradeConditions(script);
    std::string icon_file = script.ReadString("icon");
    if (script.DoesBoolExist("key_item"))
        _is_key_item = script.ReadBool("key_item");
    if(!_icon_image.Load(icon_file)) {
        PRINT_WARNING << "failed to load icon image for item: " << _id << std::endl;

        // try a default icon in that case
        _icon_image.Load("data/gui/battle/default_special.png");
    }
}

//! \brief Compares the status effect id, used to sort them.
static bool CompareStatusEffects(std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY> one, std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY> other)
{
    uint32_t status1 = one.first;
    uint32_t status2 = other.first;
    return (status1 < status2);
}

void GlobalObject::_LoadStatusEffects(vt_script::ReadScriptDescriptor &script)
{
    if(!script.DoesTableExist("status_effects"))
        return;

    std::vector<int32_t> status_effects;
    script.ReadTableKeys("status_effects", status_effects);

    if(status_effects.empty())
        return;

    script.OpenTable("status_effects");

    for(uint32_t i = 0; i < status_effects.size(); ++i) {

        int32_t key = status_effects[i];
        if(key <= GLOBAL_STATUS_INVALID || key >= GLOBAL_STATUS_TOTAL)
            continue;

        int32_t intensity = script.ReadInt(key);
        // Note: The intensity of a status effect can only be positive
        if(intensity <= GLOBAL_INTENSITY_INVALID || intensity >= GLOBAL_INTENSITY_TOTAL)
            continue;

        _status_effects.push_back(std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY>((GLOBAL_STATUS)key, (GLOBAL_INTENSITY)intensity));
    }
    // Make the effects be always presented in the same order.
    std::sort(_status_effects.begin(), _status_effects.end(), CompareStatusEffects);

    script.CloseTable(); // status_effects
}

void GlobalObject::_LoadTradeConditions(vt_script::ReadScriptDescriptor &script)
{
    if(!script.DoesTableExist("trade_conditions"))
        return;

    std::vector<uint32_t> temp;
    script.ReadTableKeys("trade_conditions", temp);

    if(temp.empty())
        return;

    script.OpenTable("trade_conditions");

    for(uint32_t i = 0; i < temp.size(); ++i) {
        uint32_t key = temp[i];
        uint32_t quantity = script.ReadInt(key);

        // Set the trade price
        if (key == 0)
            _trade_price = quantity;
        else // Or the conditions.
            _trade_conditions.push_back(std::pair<uint32_t, uint32_t>(key, quantity));
    }

    script.CloseTable(); // trade_conditions
}

void GlobalObject::_LoadEquipmentSkills(vt_script::ReadScriptDescriptor &script)
{
    _equipment_skills.clear();
    if(!script.DoesTableExist("equipment_skills"))
        return;

    script.ReadUIntVector("equipment_skills", _equipment_skills);
}

} // namespace vt_global
