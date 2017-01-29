////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __QUEST_LOG_HEADER__
#define __QUEST_LOG_HEADER__

#include "utils/ustring.h"
#include "engine/video/image.h"

namespace vt_global
{

/** ****************************************************************************
*** \brief An Entry for the quest log
***
*** Similar to Event Groups, QuestLogEntry is a string to multiple-string
*** relationship. Due to lookup speed being a concern, each quest log entry
*** is its own object, with the namespace of the Quest being the key
***
*** A Quest Entry is defined more formally as a Quest name and a "tuple" of
*** the mother dependency event, the completion event, the displaying log entry
*** name and finally the description string
***
*** There should be no need for this to be created outside of the global
*** manager, as the setting and toggling of quests will be done from the game
*** manager and through scrip entries
*** ***************************************************************************/
struct QuestLogEntry
{
public:
    //! ctor to create the QuestLogEntry. the key, event states and description
    //! cannot be changed
    //! \param quest_id for quest entry in config/quest.lua quest table
    //! \param the quest log counter, indicating the order in which the quest was added
    //! \param set whether or not the log entry is read or not. Defaults to false.
    //! this flag is manipulated by the game internals as opposed to the scripting side
    QuestLogEntry(const std::string& quest_id,
                  uint32_t quest_number,
                  bool is_read = false):
        _quest_id(quest_id),
        _quest_log_number(quest_number),
        _is_read(is_read)
    {}

    void SetRead()
    { _is_read = true; }

    bool IsRead() const
    { return _is_read; }

    const std::string& GetQuestId() const
    { return _quest_id; }

    uint32_t GetQuestLogNumber() const
    { return _quest_log_number; }

private:
    //! the string id for the title and description of this quest
    const std::string _quest_id;

    //! the quest log number for this quest
    const uint32_t _quest_log_number;

    //! flag to indicate whether or not this entry has been read
    bool _is_read;
};

//! \brief A simple structure used to store quest log system info.
class QuestLogInfo {

public:
    QuestLogInfo(const vt_utils::ustring& title,
                 const vt_utils::ustring& description,
                 const vt_utils::ustring& completion_description,
                 const std::string& completion_event_group,
                 const std::string& completion_event_name,
                 const vt_utils::ustring& location_name,
                 const std::string& location_banner_filename,
                 const vt_utils::ustring& location_subname,
                 const std::string& location_subimage_filename);

    QuestLogInfo()
    {}

    void SetNotCompletableIf(const std::string& not_completable_event_group,
                             const std::string& not_completable_event_name) {
        _not_completable_event_group = not_completable_event_group;
        _not_completable_event_name = not_completable_event_name;
    }

    // User info about the quest log
    vt_utils::ustring _title;
    vt_utils::ustring _description;
    // Completion description gets added to the quest description when the quest is considered completed
    vt_utils::ustring _completion_description;

    // Internal quest info used to know whether the quest is complete.
    std::string _completion_event_group;
    std::string _completion_event_name;
    // Internal quest info used to know whether the quest is not comppletable anymore.
    std::string _not_completable_event_group;
    std::string _not_completable_event_name;

    // location information
    vt_video::StillImage _location_image;
    vt_video::StillImage _location_subimage;
    vt_utils::ustring _location_name;
    vt_utils::ustring _location_subname;
};

} // namespace vt_global

#endif
