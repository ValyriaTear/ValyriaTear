////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_EVENT_GROUP_HEADER__
#define __GLOBAL_EVENT_GROUP_HEADER__

#include <string>
#include <map>

namespace vt_global
{

/** ****************************************************************************
*** \brief A container that manages the occurences of several related game events
***
*** Events are nothing more than a string-integer pair. The string
*** represents the name of the event while the integer takes on various meanings
*** about the event. One example of an event could be if the player has already
*** seen a certain piece of dialogue, and the integer would be set to zero or
*** non-zero to emulate a boolean value. Another example could be whether the
*** player previous chose option A, B, C, or D when presented with a list of
*** possible actions to take, in which the integer value would represent the
*** option taken.
***
*** Because we want to continually look-up whether or not an event has occured,
*** it is not efficient to store all game events in a single container (the
*** larger the number of events, the longer the event search time). Instead,
*** this class collectively represents a group of related events. A typical
*** event group could represent all of the events that occured on a particular
*** map, for instance.
***
*** \note Other parts of the code should not have a need to construct objects of
*** this class. The GameGlobal class maintains a container of GlobalEventGroup
*** objects and provides methods to allow the creation, modification, and
*** retrieval of these objects.
*** ***************************************************************************/
class GlobalEventGroup
{
public:
    //! \param group_name The name of the group to create (this can not be changed)
    explicit GlobalEventGroup(const std::string &group_name) :
        _group_name(group_name) {}

    ~GlobalEventGroup() {}

    /** \brief Queries whether or not an event of a given name exists in the group
    *** \param event_name The name of the event to check for
    *** \return True if the event name was found in the group, false if it was not
    **/
    bool DoesEventExist(const std::string &event_name) {
        if(_events.find(event_name) != _events.end()) return true;
        else return false;
    }

    /** \brief Adds a new event to the group
    *** \param event_name The name of the event to add
    *** \param event_value The value of the event to add (default value is zero)
    *** \note If an event by the given name already exists, a warning will be printed and no addition
    *** or modification of any kind will take place
    **/
    void AddNewEvent(const std::string &event_name, int32_t event_value = 0);

    /** \brief Retrieves the value of a specific event in the group
    *** \param event_name The name of the event to retrieve
    *** \return The value of the event, or 0 if there is no event corresponding to
    *** the requested event named
    **/
    int32_t GetEvent(const std::string &event_name);

    /** \brief Sets the value for an existing event
    *** \param event_name The name of the event whose value should be changed
    *** \param event_value The value to set for the event.
    *** \note If the event by the given name is not found, the event group will be created.
    **/
    void SetEvent(const std::string &event_name, int32_t event_value);

    //! \brief Returns a copy of the name of this group
    std::string GetGroupName() const {
        return _group_name;
    }

    //! \brief Returns an immutable reference to the private _events container
    const std::map<std::string, int32_t>& GetEvents() const {
        return _events;
    }

private:
    //! \brief The name given to this group of events
    std::string _group_name;

    /** \brief The map container for all the events in the group
    *** The string is the name of the event, which is unique within the group. The integer value
    *** represents the event's state and can take on multiple meanings depending on the context
    *** of this specific event.
    **/
    std::map<std::string, int32_t> _events;
}; // class GlobalEventGroup

} // namespace vt_global

#endif // __GLOBAL_EVENT_GROUP_HEADER__
