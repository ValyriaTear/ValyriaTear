////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_EVENTS_HEADER__
#define __GLOBAL_EVENTS_HEADER__

#include "script/script_read.h"
#include "script/script_write.h"

#include "global_event_group.h"

//! \brief All calls to global code are wrapped inside this namespace.
namespace vt_global
{

//! \brief Handle in-game events dictionary.
class GameEvents
{

public:
    GameEvents();
    ~GameEvents();

    //! \brief Deletes all data.
    void Clear();

    /** \brief Determines if an event of a given name exists within a given group
    *** \param group_name The name of the event group where the event to check is contained
    *** \param event_name The name of the event to check for
    *** \return True if the event was found, or false if the event name or group name was not found
    **/
    bool DoesEventExist(const std::string& group_name, const std::string& event_name) const;

    /** \brief Returns the value of an event inside of a specified group
    *** \param group_name The name of the event group where the event is contained
    *** \param event_name The name of the event whose value should be retrieved
    *** \return The value of the requested event, or 0 if the event was not found
    **/
    int32_t GetEventValue(const std::string& group_name, const std::string& event_name) const;

    /** \brief Set the value of an event inside of a specified group
    *** \param group_name The name of the event group where the event is contained
    *** \param event_name The name of the event whose value should be retrieved
    *** \return The event value.
    *** \note Events and event groups will be created when necessary.
    **/
    void SetEventValue(const std::string& group_name, const std::string& event_name, int32_t event_value);

    /** \brief A helper function to GameGlobal::SaveGame() that writes a group of event data to the saved game file
    *** \param file A reference to the open and valid file where to write the event data
    *** This method will need to be called once for each GlobalEventGroup contained by this class.
    **/
    void SaveEvents(vt_script::WriteScriptDescriptor& file);

    /** \brief A helper function to GameGlobal::LoadGame() that loads a group of game events from a saved game file
    *** \param file A reference to the open and valid file from where to read the event data from
    **/
    void LoadEvents(vt_script::ReadScriptDescriptor& file);

private:
    /** \brief Queries whether or not an event group of a given name exists
    *** \param group_name The name of the event group to check for
    *** \return True if the event group name was found, false if it was not
    **/
    bool _DoesEventGroupExist(const std::string& group_name) const {
        return (_event_groups.find(group_name) != _event_groups.end());
    }

    /** \brief Adds a new event group for the class to manage
    *** \param group_name The name of the new event group to add
    *** \note If an event group  by the given name already exists, the function will abort
    *** and not add the new event group. Otherwise, this class will automatically construct
    *** a new event group of the given name and place it in its map of event groups.
    **/
    void _AddNewEventGroup(const std::string& group_name);

    /** \brief Returns a pointer to an event group of the specified name
    *** \param group_name The name of the event group to retreive
    *** \return A pointer to the GlobalEventGroup that represents the event group, or nullptr if no event group
    *** of the specified name was found.
    **/
    GlobalEventGroup* _GetEventGroup(const std::string& group_name) const;

    /** \brief The container which stores all of the groups of events that have occured in the game
    *** The name of each GlobalEventGroup object serves as its key in this map data structure.
    **/
    std::map<std::string, GlobalEventGroup*> _event_groups;
};

} // namespace vt_global

#endif // __GLOBAL_EVENTS_HEADER__
