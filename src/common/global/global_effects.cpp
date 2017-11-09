////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_effects.cpp
*** \author  Jacob Rudolph, rujasu@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for global game effects
*** ***************************************************************************/

#include "global_effects.h"

#include "global.h"

#include "engine/system.h"

using namespace vt_utils;

using namespace vt_script;
using namespace vt_system;

namespace vt_global
{

std::string GetElementName(GLOBAL_ELEMENTAL type)
{
    switch(type) {
    case GLOBAL_ELEMENTAL_FIRE:
        return Translate("Fire");
    case GLOBAL_ELEMENTAL_WATER:
        return Translate("Water");
    case GLOBAL_ELEMENTAL_VOLT:
        return Translate("Volt");
    case GLOBAL_ELEMENTAL_EARTH:
        return Translate("Earth");
    case GLOBAL_ELEMENTAL_LIFE:
        return Translate("Life");
    case GLOBAL_ELEMENTAL_DEATH:
        return Translate("Death");
    case GLOBAL_ELEMENTAL_NEUTRAL:
        return Translate("Neutral");
    default:
        return Translate("Invalid Element");
    }
}

std::string GetStatusName(GLOBAL_STATUS type)
{
    std::string result;
    int32_t table_id = static_cast<int32_t>(type);

    ReadScriptDescriptor &script_file = GlobalManager->GetStatusEffectsScript();
    if(script_file.DoesTableExist(table_id)) {
        script_file.OpenTable(table_id);
        if(script_file.DoesStringExist("name")) {
            result = script_file.ReadString("name");
        } else {
            IF_PRINT_WARNING(GLOBAL_DEBUG) << "Lua definition file contained an entry but no name for status effect: " << type << std::endl;
        }
        script_file.CloseTable();
    } else {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "Lua definition file contained no entry for status effect: " << type << std::endl;
    }

    if(result.empty()) {
        result = Translate("Invalid Status");
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// GlobalStatusEffect class
////////////////////////////////////////////////////////////////////////////////

GlobalStatusEffect::GlobalStatusEffect(GLOBAL_STATUS type, GLOBAL_INTENSITY intensity) :
        _type(type),
        _intensity(intensity),
        _update_timer(0),
        _use_update_timer(false)
{
    // Check that the constructor arguments are valid
    if((type <= GLOBAL_STATUS_INVALID) || (type >= GLOBAL_STATUS_TOTAL)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "constructor received an invalid type argument: " << type << std::endl;
        return;
    }
    if((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "constructor received an invalid intensity argument: " << intensity << std::endl;
        return;
    }

    // Make sure that a table entry exists for this status element
    uint32_t table_id = static_cast<uint32_t>(type);
    ReadScriptDescriptor &script_file = GlobalManager->GetStatusEffectsScript();
    if(!script_file.OpenTable(table_id)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "Lua definition file contained no entry for status effect: " << table_id << std::endl;
        return;
    }

    uint32_t update_every = script_file.ReadUInt("update_every");
    if (update_every > 0)
        _update_timer.SetDuration(update_every);

    script_file.CloseTable(); // table_id

    // Init the update effect timer
    if (update_every > 0) {
        _update_timer.EnableManualUpdate();
        _update_timer.Reset();
        _update_timer.Run();
        _use_update_timer = true;
    }
}

bool GlobalStatusEffect::IncrementIntensity(uint8_t amount)
{
    return vt_global::IncrementIntensity(_intensity, amount);
}

bool GlobalStatusEffect::DecrementIntensity(uint8_t amount)
{
    return vt_global::DecrementIntensity(_intensity, amount);
}

} // namespace vt_global
