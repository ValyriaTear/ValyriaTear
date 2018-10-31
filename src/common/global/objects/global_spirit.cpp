////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_spirit.h"

#include "common/global/global.h"

using namespace vt_utils;
using namespace vt_script;

namespace vt_global
{

GlobalSpirit::GlobalSpirit(uint32_t id, uint32_t count) :
    GlobalObject(id, count)
{
    if((_id <= MAX_LEG_ARMOR_ID) || (_id > MAX_SPIRIT_ID)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "invalid id in constructor: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    ReadScriptDescriptor& script_file = GlobalManager->GetInventoryHandler().GetSpiritsScript();
    if (script_file.DoesTableExist(_id) == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "No valid data for spirit id: " << _id << std::endl;
        _InvalidateObject();
        return;
    }

    // Load the spirit data from the script
    script_file.OpenTable(_id);
    _LoadObjectData(script_file);

    script_file.CloseTable();
    if (script_file.IsErrorDetected()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "one or more errors occurred while reading spirit data - they are listed below" << std::endl
            << script_file.GetErrorMessages() << std::endl;

        _InvalidateObject();
    }
}

} // namespace vt_global
