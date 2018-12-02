////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "quests.h"

#include "common/global/global.h"

#include "utils/ustring.h"

using namespace vt_utils;
using namespace vt_script;

namespace vt_global
{

static const QuestLogInfo _empty_quest_log_info;

GameQuests::~GameQuests()
{
    Clear();
}

bool GameQuests::LoadQuestsScript(const std::string& quests_script_filename)
{
    // First clear the existing quests entries in case of a reloading.
    _quest_log_info.clear();

    vt_script::ReadScriptDescriptor quests_script;
    if(!quests_script.OpenFile(quests_script_filename)) {
        PRINT_ERROR << "Couldn't open quests file: " << quests_script_filename
                    << std::endl;
        return false;
    }

    if(!quests_script.DoesTableExist("quests")) {
        PRINT_ERROR << "No 'quests' table in file: " << quests_script_filename
                    << std::endl;
        quests_script.CloseFile();
        return false;
    }

    std::vector<std::string> quest_ids;
    quests_script.ReadTableKeys("quests", quest_ids);
    if(quest_ids.empty()) {
        PRINT_ERROR << "No quests defined in the 'quests' table of file: "
                    << quests_script_filename << std::endl;
        quests_script.CloseFile();
        return false;
    }

    quests_script.OpenTable("quests");
    for(uint32_t i = 0; i < quest_ids.size(); ++i)
    {
        const std::string& quest_id = quest_ids[i];
        std::vector<std::string> quest_info;

        quests_script.ReadStringVector(quest_id, quest_info);

        // Check for an existing quest entry
        if(_quest_log_info.find(quest_id) != _quest_log_info.end()) {
            PRINT_WARNING << "Duplicate quests defined in the 'quests' table of file: "
                << quests_script_filename << std::endl;
            continue;
        }

        //check whether all fields are there.
        if(quest_info.size() >= 9)
        {
            QuestLogInfo info = QuestLogInfo(MakeUnicodeString(quest_info[0]),
                                     MakeUnicodeString(quest_info[1]),
                                     MakeUnicodeString(quest_info[2]),
                                     quest_info[3], quest_info[4],
                                     MakeUnicodeString(quest_info[5]), quest_info[6],
                                     MakeUnicodeString(quest_info[7]), quest_info[8]);
            // If possible, loads the non-competable event group and name
            if (quest_info.size() == 11) {
                info.SetNotCompletableIf(quest_info[9], quest_info[10]);
            }
            _quest_log_info[quest_id] = info;
        }
        //malformed quest log
        else
        {
            PRINT_ERROR << "malformed quest log for id: " << quest_id << std::endl;
        }
    }

    quests_script.CloseTable();

    quests_script.CloseFile();

    return true;
}

void GameQuests::Clear()
{
    // Clear the quest log
    for(auto itr = _quest_log_entries.begin(); itr != _quest_log_entries.end(); ++itr)
        delete itr->second;
    _quest_log_entries.clear();
}

bool GameQuests::AddQuestLog(const std::string& quest_id)
{
    return _AddQuestLog(quest_id, _quest_log_entries.size());
}

uint32_t GameQuests::GetNumberQuestLogEntries() const
{
    return _quest_log_entries.size();
}

const QuestLogInfo& GameQuests::GetQuestInfo(const std::string& quest_id) const
{
    std::map<std::string, QuestLogInfo>::const_iterator itr = _quest_log_info.find(quest_id);
    if(itr == _quest_log_info.end())
        return _empty_quest_log_info;
    return itr->second;
}

std::vector<QuestLogEntry *> GameQuests::GetActiveQuestIds() const
{
    std::vector<QuestLogEntry *> keys;
    for(std::map<std::string, QuestLogEntry *>::const_iterator itr = _quest_log_entries.begin();
            itr != _quest_log_entries.end(); ++itr) {
        if (itr->second)
            keys.push_back(itr->second);
    }
    return keys;
}

bool GameQuests::IsQuestCompleted(const std::string& quest_id)
{
    std::map<std::string, vt_global::QuestLogInfo>::iterator it = _quest_log_info.find(quest_id);
    if (it == _quest_log_info.end())
        return false;
    const QuestLogInfo& info = it->second;
    if (info._completion_event_group.empty() || info._completion_event_name.empty())
        return true;

    GameEvents &events = GlobalManager->GetGameEvents();
    return (events.GetEventValue(info._completion_event_group, info._completion_event_name) == 1);
}

bool GameQuests::IsQuestCompletable(const std::string& quest_id)
{
    std::map<std::string, vt_global::QuestLogInfo>::iterator it = _quest_log_info.find(quest_id);
    if (it == _quest_log_info.end())
        return true;
    const QuestLogInfo& info = it->second;
    if (info._not_completable_event_group.empty() || info._not_completable_event_name.empty())
        return true;

    GameEvents &events = GlobalManager->GetGameEvents();
    return (events.GetEventValue(info._not_completable_event_group, info._not_completable_event_name) == 0);
}

void GameQuests::LoadQuests(ReadScriptDescriptor& file)
{
    if(file.IsFileOpen() == false) {
        PRINT_WARNING << "The file provided in the function argument was not open" << std::endl;
        return;
    }

    // Load the quest log data
    if (!file.OpenTable("quest_log")) {
        PRINT_WARNING << "'quest_log' table couldn't be open." << std::endl;
        return;
    }

    std::vector<std::string> quest_keys;
    file.ReadTableKeys(quest_keys);
    for(uint32_t i = 0; i < quest_keys.size(); ++i) {
        std::string quest_id =  quest_keys[i];

        std::vector<std::string> quest_info;
        // Read the 4 entries into a new quest entry
        file.ReadStringVector(quest_id, quest_info);
        if(quest_info.size() != 2)
        {
            PRINT_WARNING << "save file has malformed quest log entries" << std::endl;
            return;
        }

        // Conversion of the log number from string int. We need to do thing because ReadStringVector assumes that
        // all items are the same type.
        uint32_t quest_log_number = std::stoi(quest_info[0]);
        // Conversion from string to bool for is_read flag
        bool is_read = quest_info[1].compare("true") == 0;

        if(!_AddQuestLog(quest_id, quest_log_number, is_read))
        {
            PRINT_WARNING << "save file has duplicate quest log id entries" << std::endl;
            return;
        }
    }
    file.CloseTable();
}

void GameQuests::SaveQuests(WriteScriptDescriptor& file)
{
    if(file.IsFileOpen() == false)
    {
        PRINT_WARNING << "the file provided in the function argument was not open" << std::endl;
        return;
    }

    file.WriteLine("quest_log = {");
    for(auto itr = _quest_log_entries.begin(); itr != _quest_log_entries.end(); ++itr) {

        const QuestLogEntry* quest_log_entry = itr->second;

        if(quest_log_entry == nullptr)
        {
            PRINT_WARNING << "SaveQuests function received a nullptr quest log entry pointer argument" << std::endl;
            return;
        }

        // Start writting
        file.WriteLine("\t" + quest_log_entry->GetQuestId() + " = {", false);
        // Write the quest log number. this is written as a string
        // because loading needs a uniform type of data in the array
        file.WriteLine("\"" + NumberToString(quest_log_entry->GetQuestLogNumber()) + "\", ", false);
        // Write the "false" or "true" string if this entry has been read or not
        const std::string is_read(quest_log_entry->IsRead() ? "true" : "false");
        file.WriteLine("\"" + is_read + "\"", false);
        // End writing
        file.WriteLine("},");
    }

    file.WriteLine("},");
    file.InsertNewLine();
}

bool GameQuests::_AddQuestLog(const std::string& quest_id,
                              uint32_t quest_log_number,
                              bool is_read)
{
    if(_quest_log_entries.find(quest_id) != _quest_log_entries.end())
        return false;
    _quest_log_entries[quest_id] = new QuestLogEntry(quest_id,
                                                     quest_log_number,
                                                     is_read);
    return true;
}

} // namespace vt_global
