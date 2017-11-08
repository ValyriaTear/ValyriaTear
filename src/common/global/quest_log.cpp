////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "quest_log.h"

#include "utils/utils_common.h"

namespace vt_global
{

QuestLogInfo::QuestLogInfo(const vt_utils::ustring &title,
             const vt_utils::ustring &description,
             const vt_utils::ustring &completion_description,
             const std::string &completion_event_group,
             const std::string &completion_event_name,
             const vt_utils::ustring &location_name,
             const std::string &location_banner_filename,
             const vt_utils::ustring &location_subname,
             const std::string &location_subimage_filename) :
    _title(title),
    _description(description),
    _completion_description(completion_description),
    _completion_event_group(completion_event_group),
    _completion_event_name(completion_event_name),
    _location_name(location_name),
    _location_subname(location_subname)
{
    if(!_location_image.Load(location_banner_filename))
    {
        PRINT_ERROR << "image: " << location_banner_filename << " not able to load" << std::endl;
        return;
    }
    //rescale such that the height is no bigger than 90 pixels. we give ourselves a bit of wiggle room
    //by actually setting it to 90px, 5 pixel buffer top and bottom, so that we can utilize a potential 100px
    if(_location_image.GetHeight() > 90.0f)
        _location_image.SetHeightKeepRatio(90.0f);

    if(!_location_subimage.Load(location_subimage_filename))
    {
        PRINT_ERROR << "image: " << location_subimage_filename << " not able to load" << std::endl;
        return;
    }
    //rescale such that the height is no bigger than 90 pixels. we give ourselves a bit of wiggle room
    //by actually setting it to 90px, 5 pixel buffer top and bottom, so that we can utilize a potential 100px
    if(_location_subimage.GetHeight() > 90.0f)
        _location_subimage.SetHeightKeepRatio(90.0f);
}

} // namespace vt_global
