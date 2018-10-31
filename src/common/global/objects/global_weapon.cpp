////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_weapon.h"

#include "common/global/global.h"

#include "utils/utils_strings.h"

using namespace vt_script;
using namespace vt_utils;

namespace vt_global
{

GlobalWeapon::GlobalWeapon(uint32_t id, uint32_t count) :
    GlobalObject(id, count)
{
    if((_id <= MAX_ITEM_ID) || (_id > MAX_WEAPON_ID)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    ReadScriptDescriptor& script_file = GlobalManager->GetInventoryHandler().GetWeaponsScript();
    if(script_file.DoesTableExist(_id) == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "no valid data for weapon in definition file: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    // Load the weapon data from the script
    script_file.OpenTable(_id);
    _LoadObjectData(script_file);

    _LoadStatusEffects(script_file);
    _LoadEquipmentSkills(script_file);

    _physical_attack = script_file.ReadUInt("physical_attack");
    _magical_attack = script_file.ReadUInt("magical_attack");

    _usable_by = script_file.ReadUInt("usable_by");

    uint32_t spirits_number = script_file.ReadUInt("slots");
    // Only permit a max of 5 spirits for equipment
    if (spirits_number > 5) {
        spirits_number = 5;
        PRINT_WARNING << "More than 5 spirit slots declared in item " << _id << std::endl;
    }
    _spirit_slots.resize(spirits_number, nullptr);

    // Load the possible battle ammo animated image filename.
    _ammo_animation_file = script_file.ReadString("battle_ammo_animation_file");

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
}

const std::string& GlobalWeapon::GetWeaponAnimationFile(uint32_t character_id, const std::string& animation_alias)
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
    //std::map <uint32_t, std::map<std::string, std::string> > _weapon_animations;
    _weapon_animations.clear();

    // The character id keys
    std::vector<uint32_t> char_ids;

    script.ReadTableKeys("battle_animations", char_ids);
    if (char_ids.empty())
        return;

    if (!script.OpenTable("battle_animations"))
        return;

    for (uint32_t i = 0; i < char_ids.size(); ++i) {
        uint32_t char_id = char_ids[i];

        // Read all the animation aliases
        std::vector<std::string> anim_aliases;
        script.ReadTableKeys(char_id, anim_aliases);

        if (anim_aliases.empty())
            continue;

        if (!script.OpenTable(char_id))
            continue;

        for (uint32_t j = 0; j < anim_aliases.size(); ++j) {
            std::string anim_alias = anim_aliases[j];
            std::string anim_file = script.ReadString(anim_alias);
            _weapon_animations[char_id].insert(std::make_pair(anim_alias, anim_file));
        }

        script.CloseTable(); // char_id
    }

    script.CloseTable(); // battle_animations
}

} // namespace vt_global
