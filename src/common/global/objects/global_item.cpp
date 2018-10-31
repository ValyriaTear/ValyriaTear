////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_item.h"

#include "common/global/global.h"

using namespace vt_script;

namespace vt_global
{

GlobalItem::GlobalItem(uint32_t id, uint32_t count) :
    GlobalObject(id, count),
    _target_type(GLOBAL_TARGET_INVALID),
    _warmup_time(0),
    _cooldown_time(0)
{
    if(_id == 0 || (_id > MAX_ITEM_ID && (_id <= MAX_SPIRIT_ID && _id > MAX_KEY_ITEM_ID))) {
        PRINT_WARNING << "invalid id in constructor: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    ReadScriptDescriptor& script_file = GlobalManager->GetInventoryHandler().GetItemsScript();
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

    // Read all the battle animation scripts linked to this item, if any.
    if(script_file.DoesTableExist("animation_scripts")) {
        std::vector<uint32_t> characters_ids;
        _animation_scripts.clear();
        script_file.ReadTableKeys("animation_scripts", characters_ids);
        script_file.OpenTable("animation_scripts");
        for(uint32_t i = 0; i < characters_ids.size(); ++i) {
            _animation_scripts[characters_ids[i]] = script_file.ReadString(characters_ids[i]);
        }
        script_file.CloseTable(); // animation_scripts table
    }

    script_file.CloseTable(); // id

    if(script_file.IsErrorDetected()) {
        PRINT_WARNING << "one or more errors occurred while reading item data - they are listed below"
                        << std::endl << script_file.GetErrorMessages() << std::endl;
        _InvalidateObject();
    }
}

GlobalItem::GlobalItem(const GlobalItem &copy) :
    GlobalObject(copy),
    _animation_scripts(copy._animation_scripts)
{
    _target_type = copy._target_type;
    _warmup_time = copy._warmup_time;
    _cooldown_time = copy._cooldown_time;

    // Make copies of valid luabind::object function pointers
    _battle_use_function = copy._battle_use_function;
    _field_use_function = copy._field_use_function;
}

std::string GlobalItem::GetAnimationScript(uint32_t character_id) const
{
    std::string script_file; // Empty by default

    std::map<uint32_t, std::string>::const_iterator it = _animation_scripts.find(character_id);
    if(it != _animation_scripts.end())
        script_file = it->second;
    return script_file;
}

GlobalItem& GlobalItem::operator=(const GlobalItem& copy)
{
    if(this == &copy)  // Handle self-assignment case
        return *this;

    GlobalObject::operator=(copy);
    _target_type = copy._target_type;
    _warmup_time = copy._warmup_time;
    _cooldown_time = copy._cooldown_time;

    // Make copies of valid luabind::object function pointers
    _battle_use_function = copy._battle_use_function;
    _field_use_function = copy._field_use_function;
    _animation_scripts = copy._animation_scripts;

    return *this;
}

} // namespace vt_global
