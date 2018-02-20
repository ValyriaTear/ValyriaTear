////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_attack_point.h"

#include "global_actor.h"
#include "common/global/objects/global_armor.h"

#include "script/script_read.h"

using namespace vt_utils;
using namespace vt_video;
using namespace vt_script;

namespace vt_global
{

extern bool GLOBAL_DEBUG;

GlobalAttackPoint::GlobalAttackPoint(GlobalActor *owner) :
    _actor_owner(owner),
    _x_position(0),
    _y_position(0),
    _phys_def_modifier(0.0f),
    _mag_def_modifier(0.0f),
    _evade_modifier(0.0f),
    _total_physical_defense(0),
    _total_evade_rating(0)
{
    for (uint32_t i = 0; i < GLOBAL_ELEMENTAL_TOTAL; ++i)
        _total_magical_defense[i] = 0;
}

bool GlobalAttackPoint::LoadData(ReadScriptDescriptor &script)
{
    if(script.IsFileOpen() == false) {
        return false;
    }

    _name = MakeUnicodeString(script.ReadString("name"));
    _x_position = script.ReadInt("x_position");
    _y_position = script.ReadInt("y_position");
    _phys_def_modifier = script.ReadFloat("phys_def_modifier");
    _mag_def_modifier = script.ReadFloat("mag_def_modifier");
    _evade_modifier = script.ReadFloat("evade_modifier");

    // Status effect data is optional so check if a status_effect table exists first
    if(script.DoesTableExist("status_effects")) {
        script.OpenTable("status_effects");

        std::vector<int32_t> table_keys;
        script.ReadTableKeys(table_keys);
        for(uint32_t i = 0; i < table_keys.size(); ++i) {
            float probability = script.ReadFloat(table_keys[i]);
            _status_effects.push_back(std::make_pair(static_cast<GLOBAL_STATUS>(table_keys[i]), probability));
        }

        script.CloseTable();
    }


    if(script.IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading the save game file - they are listed below"
                          << std::endl
                          << script.GetErrorMessages() << std::endl;
        }
        return false;
    }

    return true;
}

void GlobalAttackPoint::CalculateTotalDefense(const std::shared_ptr<GlobalArmor>& equipped_armor)
{
    if(_actor_owner == nullptr) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "attack point has no owning actor" << std::endl;
        return;
    }

    // Calculate defense ratings from owning actor's base stat properties and the attack point modifiers
    if(_phys_def_modifier <= -1.0f)  // If the modifier is less than or equal to -100%, set the total defense to zero
        _total_physical_defense = 0;
    else
        _total_physical_defense = _actor_owner->GetPhysDef() +
                                  static_cast<int32_t>(_actor_owner->GetPhysDef() * _phys_def_modifier);

    // If the modifier is less than or equal to -100%, set the total defense to zero
    uint32_t magical_base = 0;
    if(_mag_def_modifier > -1.0f)
        magical_base = _actor_owner->GetMagDef() + static_cast<int32_t>(_actor_owner->GetMagDef() * _mag_def_modifier);

    // If present, add defense ratings from the armor equipped
    if(equipped_armor) {
        _total_physical_defense += equipped_armor->GetPhysicalDefense();

        for (uint32_t i = 0; i < GLOBAL_ELEMENTAL_TOTAL; ++i) {
            _total_magical_defense[i] = (magical_base + equipped_armor->GetMagicalDefense())
                                        * _actor_owner->GetElementalModifier((GLOBAL_ELEMENTAL) i);
        }
    }
    else {
        for (uint32_t i = 0; i < GLOBAL_ELEMENTAL_TOTAL; ++i)
            _total_magical_defense[i] = magical_base * _actor_owner->GetElementalModifier((GLOBAL_ELEMENTAL) i);
    }
}

void GlobalAttackPoint::CalculateTotalEvade()
{
    if(_actor_owner == nullptr) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "attack point has no owning actor" << std::endl;
        return;
    }

    // Calculate evade ratings from owning actor's base evade stat and the evade modifier
    if(_evade_modifier <= -1.0f)  // If the modifier is less than or equal to -100%, set the total evade to zero
        _total_evade_rating = 0.0f;
    else
        _total_evade_rating = _actor_owner->GetEvade() + (_actor_owner->GetEvade() * _evade_modifier);
}

} // namespace vt_global
