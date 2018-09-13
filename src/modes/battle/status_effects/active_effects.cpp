////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "active_effects.h"

#include "script/script_read.h"

#include "common/global/global.h"
#include "engine/video/text.h"

using namespace vt_global;
using namespace vt_script;
using namespace vt_video;

namespace vt_battle
{

namespace private_battle
{

ActiveBattleStatusEffect::ActiveBattleStatusEffect():
    GlobalStatusEffect(GLOBAL_STATUS_INVALID, GLOBAL_INTENSITY_NEUTRAL),
    _timer(0),
    _icon_image(nullptr),
    _intensity_changed(false)
{}

ActiveBattleStatusEffect::ActiveBattleStatusEffect(GLOBAL_STATUS type, GLOBAL_INTENSITY intensity, uint32_t duration) :
    GlobalStatusEffect(type, intensity),
    _timer(0),
    _icon_image(nullptr),
    _intensity_changed(false)
{
    // Check that status effect base value are making it actually active
    if (!IsActive())
        return;

    // Make sure that a table entry exists for this status element
    uint32_t table_id = static_cast<uint32_t>(type);
    ReadScriptDescriptor &script_file = GlobalManager->GetStatusEffectsScript();
    if(!script_file.OpenTable(table_id)) {
        PRINT_WARNING << "Lua definition file contained no entry for status effect: " << table_id << std::endl;
        return;
    }

    // Read in the status effect's property data
    std::string name = script_file.ReadString("name");
    _name.SetText(name, TextStyle("text14"));

    // Read the fall back duration when none is given.
    if(duration == 0)
        duration = script_file.ReadUInt("default_duration");
    _timer.SetDuration(duration);

    if(script_file.DoesFunctionExist("BattleApply")) {
        _apply_function = script_file.ReadFunctionPointer("BattleApply");
    } else {
        PRINT_WARNING << "No BattleApply() function found in Lua definition file for status: " << table_id << std::endl;
    }

    if(script_file.DoesFunctionExist("BattleUpdate")) {
        _update_function = script_file.ReadFunctionPointer("BattleUpdate");
    } else {
        PRINT_WARNING << "No BattleUpdate() function found in Lua definition file for status: " << table_id << std::endl;
    }

    if(script_file.DoesFunctionExist("BattleRemove")) {
        _remove_function = script_file.ReadFunctionPointer("BattleRemove");
    } else {
        PRINT_WARNING << "No BattleRemove() function found in Lua definition file for status: " << table_id << std::endl;
    }
    script_file.CloseTable(); // table_id

    if(script_file.IsErrorDetected()) {
        PRINT_WARNING << "one or more errors occurred while reading status effect data - they are listed below"
            << std::endl << script_file.GetErrorMessages() << std::endl;
    }

    // Init the effect timer
    _timer.EnableManualUpdate();
    _timer.Reset();
    _timer.Run();

    _icon_image = GlobalManager->Media().GetStatusIcon(_type, _intensity);
}

void ActiveBattleStatusEffect::SetIntensity(vt_global::GLOBAL_INTENSITY intensity)
{
    if((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
        PRINT_WARNING << "attempted to set status effect to invalid intensity: " << intensity << std::endl;
        return;
    }

    bool no_intensity_change = (_intensity == intensity);
    _intensity = intensity;
    _ProcessIntensityChange(no_intensity_change);
}

bool ActiveBattleStatusEffect::IncrementIntensity(uint8_t amount)
{
    bool change = GlobalStatusEffect::IncrementIntensity(amount);
    _ProcessIntensityChange(!change);
    return change;
}

bool ActiveBattleStatusEffect::DecrementIntensity(uint8_t amount)
{
    bool change = GlobalStatusEffect::DecrementIntensity(amount);
    _ProcessIntensityChange(!change);
    return change;
}

void ActiveBattleStatusEffect::_ProcessIntensityChange(bool reset_timer_only)
{
    _timer.Reset();
    _timer.Run();

    if(reset_timer_only)
        return;

    _intensity_changed = true;
    _icon_image = GlobalManager->Media().GetStatusIcon(_type, _intensity);
}

} // namespace private_battle

} // namespace vt_battle
