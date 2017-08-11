///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_DIALOGUE_OPTIONS_HEADER__
#define __MAP_DIALOGUE_OPTIONS_HEADER__

#include "common/dialogue.h"

namespace vt_map
{

namespace private_map
{

/** ***************************************************************************************
*** \brief A container class for option sets presented during a map dialogue
***
*** When the player reads a dialogue, they may be presented with a small number of options to
*** select from when coming to a particular line. The selected option determines the next line
*** that will follow. Optionally, each particular option may trigger a different map event when
*** it is selected.
*** **************************************************************************************/
class MapDialogueOptions : public vt_common::DialogueOptions
{
public:
    MapDialogueOptions()
    {}

    virtual ~MapDialogueOptions() override
    {}

    /** \brief Adds a new option to the set of options
    *** \param text The text for the new option
    ***
    *** The following option properties are set when using this call:
    *** - proceed to next sequential line, no event
    **/
    void AddOption(const std::string &text);

    /** \brief Adds a new option to the set of options
    *** \param text The text for the new option
    *** \param next_line An integer index of the next line of dialogue should this option be selected.
    ***
    *** The following option properties are set when using this call:
    *** - no event
    **/
    void AddOption(const std::string &text, int32_t next_line);

    /** \brief Adds a new option to the set of options with the addition of a map event to be executed
    *** \param text The text for the new option
    *** \param event_id The ID of the event to execute should this option be selected
    ***
    *** The following option properties are set when using this call:
    *** - proceed to next sequential line
    **/
    void AddOptionEvent(const std::string &text, const std::string &event_id);

    /** \brief Adds a new option to the set of options with the addition of a map event to be executed
    *** \param text The text for the new option
    *** \param next_line An integer index of the next line of dialogue should this option be selected.
    *** \param event_id The ID of the event to execute should this option be selected
    **/
    void AddOptionEvent(const std::string &text, int32_t next_line, const std::string &event_id);

    //! \name Methods for retrieving properties of a specific line
    std::string GetOptionEvent(uint32_t option) const;

    //! \brief Returns the number of options stored by this class
    uint32_t GetNumberOptions() const {
        return _text.size();
    }

private:
    //! \brief An optional MapEvent that may occur as a result of selecting each option
    std::vector<std::string> _events;
};

} // namespace private_map

} // namespace vt_map

#endif // __MAP_DIALOGUE_OPTIONS_HEADER__
