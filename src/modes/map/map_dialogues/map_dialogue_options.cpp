///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_dialogues/map_dialogue_options.h"

namespace vt_map
{

namespace private_map
{

void MapDialogueOptions::AddOption(const std::string &text)
{
    AddOptionEvent(text, vt_common::DIALOGUE_NEXT_LINE, std::string());
}

void MapDialogueOptions::AddOption(const std::string &text, int32_t next_line)
{
    AddOptionEvent(text, next_line, std::string());
}

void MapDialogueOptions::AddOptionEvent(const std::string &text, const std::string &event_id)
{
    AddOptionEvent(text, vt_common::DIALOGUE_NEXT_LINE, event_id);
}

void MapDialogueOptions::AddOptionEvent(const std::string &text, int32_t next_line, const std::string &event_id)
{
    DialogueOptions::AddOption(text, next_line);
    _events.push_back(event_id);
}

std::string MapDialogueOptions::GetOptionEvent(uint32_t option) const
{
    if (option >= GetNumberOptions())
        return std::string();
    return _events[option];
}

} // namespace private_map

} // namespace vt_map
