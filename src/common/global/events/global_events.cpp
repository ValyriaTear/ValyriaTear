////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_events.h"

using namespace vt_utils;
using namespace vt_script;

namespace vt_global
{

GameEvents::GameEvents()
{
}

GameEvents::~GameEvents()
{
    Clear();
}

void GameEvents::Clear()
{
    // Delete all event groups
    for(std::map<std::string, GlobalEventGroup *>::iterator it = _event_groups.begin(); it != _event_groups.end(); ++it) {
        delete(it->second);
    }
    _event_groups.clear();
}

bool GameEvents::DoesEventExist(const std::string& group_name, const std::string& event_name) const
{
    std::map<std::string, GlobalEventGroup *>::const_iterator group_iter = _event_groups.find(group_name);
    if(group_iter == _event_groups.end())
        return false;

    std::map<std::string, int32_t>::const_iterator event_iter = group_iter->second->GetEvents().find(event_name);
    if(event_iter == group_iter->second->GetEvents().end())
        return false;

    return true;
}

int32_t GameEvents::GetEventValue(const std::string& group_name, const std::string& event_name) const
{
    std::map<std::string, GlobalEventGroup *>::const_iterator group_iter = _event_groups.find(group_name);
    if(group_iter == _event_groups.end())
        return 0;

    std::map<std::string, int32_t>::const_iterator event_iter = group_iter->second->GetEvents().find(event_name);
    if(event_iter == group_iter->second->GetEvents().end())
        return 0;

    return event_iter->second;
}

void GameEvents::SetEventValue(const std::string& group_name,
                               const std::string& event_name,
                               int32_t event_value)
{
    GlobalEventGroup* geg = nullptr;
    std::map<std::string, GlobalEventGroup *>::const_iterator group_iter = _event_groups.find(group_name);
    if(group_iter == _event_groups.end()) {
        geg = new GlobalEventGroup(group_name);
        _event_groups.insert(std::make_pair(group_name, geg));
    } else {
        geg = group_iter->second;
    }

    geg->SetEvent(event_name, event_value);
}

void GameEvents::SaveEvents(WriteScriptDescriptor& file)
{
    if(file.IsFileOpen() == false) {
        PRINT_WARNING << "the file provided in the function argument was not open" << std::endl;
        return;
    }

    file.InsertNewLine();
    file.WriteLine("event_groups = {");
    for(auto it = _event_groups.begin(); it != _event_groups.end(); ++it) {
        GlobalEventGroup* event_group = it->second;

        file.WriteLine("\t" + event_group->GetGroupName() + " = {");

        uint32_t i = 0;
        for(auto it = event_group->GetEvents().begin(); it != event_group->GetEvents().end(); ++it) {
            if(it == event_group->GetEvents().begin())
                file.WriteLine("\t\t", false);
            else
                file.WriteLine(", ", false);

            // Add a new line every 4 entries for better readability and debugging
            if ((i > 0) && !(i % 4)) {
                file.InsertNewLine();
                file.WriteLine("\t\t", false);
            }

            file.WriteLine("[\"" + it->first + "\"] = " + NumberToString(it->second), false);

            ++i;
        }
        file.WriteLine("\n\t},");

    }

    file.WriteLine("},"); // event_groups
    file.InsertNewLine();

}

void GameEvents::LoadEvents(ReadScriptDescriptor& file)
{
    if(file.IsFileOpen() == false) {
        PRINT_WARNING << "The file provided in the function argument was not open" << std::endl;
        return;
    }

    std::vector<std::string> group_names;
    if (!file.OpenTable("event_groups"))
        return;

    file.ReadTableKeys(group_names);
    for(uint32_t i = 0; i < group_names.size(); i++) {
        std::string group_name = group_names[i];
        _AddNewEventGroup(group_name);
        // new_group is guaranteed not to be nullptr
        GlobalEventGroup* new_group = _GetEventGroup(group_name);

        std::vector<std::string> event_names;

        if (file.OpenTable(group_name)) {
            file.ReadTableKeys(event_names);
            for(uint32_t i = 0; i < event_names.size(); i++) {
                new_group->AddNewEvent(event_names[i], file.ReadInt(event_names[i]));
            }
            file.CloseTable();
        }
        else {
            PRINT_ERROR << "Invalid event group name '" << group_name << " in save file "
                        << file.GetFilename() << std::endl;
        }
    }
    file.CloseTable(); // event_groups
}

void GameEvents::_AddNewEventGroup(const std::string& group_name)
{
    if(_DoesEventGroupExist(group_name)) {
        PRINT_WARNING << "failed because there was already an event group that existed for "
                      << "the requested group name: " << group_name << std::endl;
        return;
    }

    GlobalEventGroup* geg = new GlobalEventGroup(group_name);
    _event_groups.insert(std::make_pair(group_name, geg));
}

GlobalEventGroup* GameEvents::_GetEventGroup(const std::string& group_name) const
{
    std::map<std::string, GlobalEventGroup *>::const_iterator group_iter = _event_groups.find(group_name);
    if(group_iter == _event_groups.end()) {
        PRINT_WARNING << "could not find any event group by the requested name: " << group_name << std::endl;
        return nullptr;
    }
    return (group_iter->second);
}

} // namespace vt_global
