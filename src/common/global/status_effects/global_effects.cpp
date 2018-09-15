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

#include "common/global/global.h"

#include "engine/system.h"
#include "script/script_read.h"

using namespace vt_utils;
using namespace vt_script;
using namespace vt_system;

namespace vt_global
{

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
    if(amount == 0)
        return false;
    if((_intensity <= GLOBAL_INTENSITY_INVALID) || (_intensity >= GLOBAL_INTENSITY_POS_EXTREME))
        return false;

    // This check protects against overflow conditions
    if(amount > (GLOBAL_INTENSITY_TOTAL * 2)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to increment intensity by an excessive amount: " << amount << std::endl;
        if(_intensity == GLOBAL_INTENSITY_POS_EXTREME) {
            return false;
        } else {
            _intensity = GLOBAL_INTENSITY_POS_EXTREME;
            return true;
        }
    }

    _intensity = GLOBAL_INTENSITY(_intensity + amount);
    if(_intensity >= GLOBAL_INTENSITY_TOTAL)
        _intensity = GLOBAL_INTENSITY_POS_EXTREME;
    return true;
}

bool GlobalStatusEffect::DecrementIntensity(uint8_t amount)
{
    if(amount == 0)
        return false;
    if((_intensity <= GLOBAL_INTENSITY_NEG_EXTREME) || (_intensity >= GLOBAL_INTENSITY_TOTAL))
        return false;

    // This check protects against overflow conditions
    if(amount > (GLOBAL_INTENSITY_TOTAL * 2)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to decrement intensity by an excessive amount: " << amount << std::endl;
        _intensity = GLOBAL_INTENSITY_NEG_EXTREME;
        return true;
    }

    _intensity = GLOBAL_INTENSITY(_intensity - amount);
    if(_intensity <= GLOBAL_INTENSITY_INVALID)
        _intensity = GLOBAL_INTENSITY_NEG_EXTREME;
    return true;
}

} // namespace vt_global
