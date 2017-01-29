////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_event_group.h"

namespace vt_global {

extern bool GLOBAL_DEBUG;

void GlobalEventGroup::AddNewEvent(const std::string &event_name, int32_t event_value)
{
    if(DoesEventExist(event_name)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "an event with the desired name \"" << event_name << "\" already existed in this group: "
                                       << _group_name << std::endl;
        return;
    }
    _events.insert(std::make_pair(event_name, event_value));
}

int32_t GlobalEventGroup::GetEvent(const std::string &event_name)
{
    std::map<std::string, int32_t>::iterator event_iter = _events.find(event_name);
    if(event_iter == _events.end()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "an event with the specified name \"" << event_name << "\" did not exist in this group: "
                                       << _group_name << std::endl;
        return 0;
    }
    return event_iter->second;
}

void GlobalEventGroup::SetEvent(const std::string &event_name, int32_t event_value)
{
    std::map<std::string, int32_t>::iterator event_iter = _events.find(event_name);
    if(event_iter == _events.end()) {
        AddNewEvent(event_name, event_value);
        return;
    }
    event_iter->second = event_value;
}

} // namespace vt_global
