////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __QUEST_LOG_INFO_HEADER__
#define __QUEST_LOG_INFO_HEADER__

#include "utils/ustring.h"
#include "engine/video/image.h"

namespace vt_global
{

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

    //! \brief User info about the quest log
    vt_utils::ustring _title;
    vt_utils::ustring _description;

    //! \brief Completion description gets added to the quest description when the quest is considered completed
    vt_utils::ustring _completion_description;

    //! \brief Internal quest info used to know whether the quest is complete.
    std::string _completion_event_group;
    std::string _completion_event_name;

    //! \brief Internal quest info used to know whether the quest is not comppletable anymore.
    std::string _not_completable_event_group;
    std::string _not_completable_event_name;

    //! \brief location information
    vt_video::StillImage _location_image;
    vt_video::StillImage _location_subimage;
    vt_utils::ustring _location_name;
    vt_utils::ustring _location_subname;
};

} // namespace vt_global

#endif
