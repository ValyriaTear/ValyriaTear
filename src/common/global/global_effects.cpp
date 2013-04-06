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
*** \file    global_effects.cpp
*** \author  Jacob Rudolph, rujasu@allacrost.org
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
    int32 table_id = static_cast<int32>(type);

    ReadScriptDescriptor &script_file = GlobalManager->GetStatusEffectsScript();
    if(script_file.DoesTableExist(table_id) == true) {
        script_file.OpenTable(table_id);
        if(script_file.DoesStringExist("name") == true) {
            result = script_file.ReadString("name");
        } else {
            IF_PRINT_WARNING(GLOBAL_DEBUG) << "Lua definition file contained an entry but no name for status effect: " << type << std::endl;
        }
        script_file.CloseTable();
    } else {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "Lua definition file contained no entry for status effect: " << type << std::endl;
    }

    if(result == "") {
        result = Translate("Invalid Status");
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// GlobalElementalEffect class
////////////////////////////////////////////////////////////////////////////////

void GlobalElementalEffect::IncrementIntensity(uint8 amount)
{
    vt_global::IncrementIntensity(_intensity, amount);
}



void GlobalElementalEffect::DecrementIntensity(uint8 amount)
{
    vt_global::DecrementIntensity(_intensity, amount);
}

////////////////////////////////////////////////////////////////////////////////
// GlobalStatusEffect class
////////////////////////////////////////////////////////////////////////////////

bool GlobalStatusEffect::IncrementIntensity(uint8 amount)
{
    return vt_global::IncrementIntensity(_intensity, amount);
}



bool GlobalStatusEffect::DecrementIntensity(uint8 amount)
{
    GLOBAL_INTENSITY previous_intensity = _intensity;
    bool intensity_modified = vt_global::DecrementIntensity(_intensity, amount);

    if(intensity_modified == true) {
        if(_intensity < GLOBAL_INTENSITY_NEUTRAL) {
            IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to decrement intensity below neutral level" << std::endl;
            _intensity = GLOBAL_INTENSITY_NEUTRAL;
            if(_intensity == previous_intensity)
                intensity_modified = false;
        }
    }
    return intensity_modified;
}

} // namespace vt_global
