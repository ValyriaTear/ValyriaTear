////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "passive_effects.h"

#include "common/global/global.h"

#include "script/script_read.h"

using namespace vt_global;
using namespace vt_script;
using namespace vt_video;

namespace vt_battle
{

namespace private_battle
{

PassiveBattleStatusEffect::PassiveBattleStatusEffect(GLOBAL_STATUS type,
                                                     GLOBAL_INTENSITY intensity):
    GlobalStatusEffect(type, intensity),
    _icon_image(nullptr)
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

    if(script_file.DoesFunctionExist("BattleUpdatePassive")) {
        _update_passive_function = script_file.ReadFunctionPointer("BattleUpdatePassive");
    } else {
        PRINT_WARNING << "No BattleUpdatePassive() function found in Lua definition file for status: " << table_id << std::endl;
    }

    script_file.CloseTable(); // table_id

    if(script_file.IsErrorDetected()) {
        PRINT_WARNING << "one or more errors occurred while reading status effect data - they are listed below"
            << std::endl << script_file.GetErrorMessages() << std::endl;
    }

    _icon_image = GlobalManager->Media().GetStatusIcon(type, intensity);
}

} // namespace private_battle

} // namespace vt_battle
