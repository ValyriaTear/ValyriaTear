////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_QUESTS_HEADER__
#define __GLOBAL_QUESTS_HEADER__

#include "quest_log_entry.h"
#include "quest_log_info.h"

#include "script/script_read.h"
#include "script/script_write.h"

#include <string>
#include <vector>
#include <map>

namespace vt_global
{

//! \brief Handles quests state
class GameQuests
{

public:
    GameQuests() {}
    ~GameQuests();

    //! \brief (Re)Loads the quest entries into the GlobalManager
    //! \param quest_script_filename The quest log script filename to load
    //! \note this is done in _LoadGlobalScripts().
    bool LoadQuestsScript(const std::string& quests_script_filename);

    //! \brief Deletes all quests data
    void Clear();

    //! \brief Tells whether a quest id is completed, based on the internal quest info
    //! and the current game event values.
    //! \param quest_id the string id into quests table for this quest
    bool IsQuestCompleted(const std::string& quest_id);

    //! \brief Tells whether a quest id is completed, based on the internal quest info
    //! and the current game event values.
    //! \param quest_id the string id into quests table for this quest
    bool IsQuestCompletable(const std::string& quest_id);

    /** \brief adds a new quest log entry into the quest log entries table
    *** \param quest_id the string id into quests table for this quest
    *** \return true if the entry was added. false if the entry already exists
    **/
    bool AddQuestLog(const std::string& quest_id);

    /** \brief gets the number of quest log entries
    *** \return number of log entries
    **/
    uint32_t GetNumberQuestLogEntries() const;

    /** \brief get a list of all the currently active quest log entries
    *** \return a vector of valid quest log entries
    **/
    std::vector<QuestLogEntry *> GetActiveQuestIds() const;

    /** \brief gets a pointer to the description for the quest string id,
    *** \param quest_id the quest id
    *** \return a reference to the given quest log info or an empty quest log info.
    **/
    const QuestLogInfo& GetQuestInfo(const std::string& quest_id) const;

    /** \brief Helper function called by LoadGame() that loads each quest into the quest entry table
    *** based on the quest_entry_keys in the save game file
    *** \param file Reference to open and valid file set for reading the data
    **/
    void LoadQuests(vt_script::ReadScriptDescriptor &file);

    /** \brief Helper function that saves the Quest Log entries. this is called from SaveGame()
    *** \param file Reference to open and valid file set for writting the data
    **/
    void SaveQuests(vt_script::WriteScriptDescriptor& file);

private:
    /** \brief The container which stores the quest log entries in the game. the quest log key
    *** acts as the key for this quest
    *** \note due to a limitation with OptionBoxes, we can only currently only support 255
    *** entries. Please be careful about this limitation
    **/
    std::map<std::string, QuestLogEntry *> _quest_log_entries;

    //! \brief a map of the quest string ids to their info
    std::map<std::string, QuestLogInfo> _quest_log_info;

    /** \brief adds a new quest log entry into the quest log entries table. also updates the quest log number
    *** \param quest_id for the quest
    *** \param the quest entry's log number
    *** \param flag to indicate if this entry is read or not. default is false
    *** \return true if the entry was added. false if the entry already exists
    **/
    bool _AddQuestLog(const std::string& quest_id,
                      uint32_t quest_log_number,
                      bool is_read = false);
};

} // namespace vt_global

#endif // __GLOBAL_QUESTS_HEADER__
