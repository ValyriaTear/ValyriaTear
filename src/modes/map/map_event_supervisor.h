///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_EVENT_SUPERVISOR_HEADER__
#define __MAP_EVENT_SUPERVISOR_HEADER__

#include "modes/map/map_events.h"

namespace vt_map
{

namespace private_map
{

/** ****************************************************************************
*** \brief Manages, processes, and launches map events
***
*** The EventSupervisor serves as an assistant to the MapMode class, much like the
*** other map supervisor classes. As such, this class is only created as a member
*** of the MapMode class. The first responsibility of the EventSupervisor is to
*** retain all of the MapEvent objects that have been created. The second responsibility
*** of this class is to initialize and begin the first event in a n-length chain
*** of events, where n can be equal to one or any higher interger value.
***
*** When an event chain begins, the first (base) event of the chain is started.
*** Immediately after starting the first event, the supervisor will examine its event
*** links to determine which, if any, children events begin relative to the start of
*** the base event. If they are to start a certain time after the start of the parent
*** event, they are placed in a container and their countdown timers are initialized.
*** These timers will count down on every update call to the event manager and after
*** the timers expire, these events will be launched. When an active event ends, again
*** its event links are examined to determine if any children events exist that start
*** relative to the end of the parent event.
*** ***************************************************************************/
class EventSupervisor
{
    friend class MapEvent;
public:
    EventSupervisor():
        _is_updating(false)
    {}

    ~EventSupervisor();

    /** \brief Marks a specified event as active and starts the event
    *** \param event_id The ID of the event to activate
    *** \param launch_time The time to wait before launching the event.
    *** The specified event to start may be linked to several children, grandchildren, etc. events.
    *** If the event has no children, it will activate only the single event requested. Otherwise
    *** all events in the chain will become activated at the appropriate time.
    **/
    void StartEvent(const std::string& event_id);
    void StartEvent(const std::string& event_id, uint32_t launch_time);

    /** \brief Marks a specified event as active and starts the event
    *** \param event A pointer to the event to begin
    *** \param launch_time The time to wait before launching the event.
    *** The specified event to start may be linked to several children, grandchildren, etc. events.
    *** If the event has no children, it will activate only the single event requested. Otherwise
    *** all events in the chain will become activated at the appropriate time.
    **/
    void StartEvent(MapEvent* event);
    void StartEvent(MapEvent* event, uint32_t launch_time);

    /** \brief Pauses the active events by preventing them from updating
    *** \param event_id The ID of the active event(s) to pause
    *** If the event corresponding to the ID is not active, a warning will be issued and no change
    *** will occur.
    **/
    void PauseEvent(const std::string& event_id);

    /** \brief Pauses the given sprite events
    *** \param sprite The sprite to pause the sprite events from
    **/
    void PauseAllEvents(VirtualSprite* sprite);

    /** \brief Resumes a paused event
    *** \param event_id The ID of the active event(s) to resume
    *** If the event corresponding to the ID is not paused, a warning will be issued and no change
    *** will occur.
    **/
    void ResumeEvent(const std::string& event_id);

    /** \brief Resumes the given sprite events
    *** \param sprite The sprite to resume the sprite events from
    **/
    void ResumeAllEvents(VirtualSprite* sprite);

    /** \brief Terminates an event if it is active
    *** \param event_id The ID of the event(s) to terminate
    *** \param event Mapevent(s) to terminate
    *** \param trigger_event_links Tells whether the launching of any of the events' children should occur, true by default.
    *** \note If there is no active event that corresponds to the event ID, the function will do nothing.
    **/
    void EndEvent(const std::string& event_id, bool trigger_event_links = true);
    void EndEvent(MapEvent* event, bool trigger_event_links = true);

    /** \brief Terminates all the SpriteEvents (active, paused, or incoming) for the given sprite.
    ***
    *** This is very useful when wanting to break an active event chain controlling a sprite
    *** and liberate it for something else.
    *** Note that you should start the new sprite event chain *after* this call.
    **/
    void EndAllEvents(VirtualSprite* sprite);

    //! \brief Updates the state of all active and launch events
    void Update();

    /** \brief Determines if a chosen event is active
    *** \param event_id The ID of the event to check
    *** \return True if the event is active, false if it is not or the event could not be found
    **/
    bool IsEventActive(const std::string& event_id) const;

    //! \brief Returns true if any events are active
    bool HasActiveEvent() const {
        return !_active_events.empty();
    }

    //! \brief Returns true if any events are being prepared to be launched after their timers expire
    bool HasActiveDelayedEvent() const {
        return !_active_delayed_events.empty();
    }

    /** \brief Returns a pointer to a specified event stored by this class
    *** \param event_id The ID of the event to retrieve
    *** \return A MapEvent pointer (which may need to be casted to the proper event type), or nullptr if no event was found
    **/
    MapEvent* GetEvent(const std::string& event_id) const;

    bool DoesEventExist(const std::string& event_id) const
    { return !(GetEvent(event_id) == nullptr); }

private:
    //! \brief A container for all map events, where the event's ID serves as the key to the std::map
    std::map<std::string, MapEvent*> _all_events;

    //! \brief A list of all events which have started but are not yet finished
    std::vector<MapEvent*> _active_events;

    //! \brief A list of all events which have been paused
    std::vector<MapEvent*> _paused_events;

    /** \brief A list of all events that are waiting on their launch timers to expire before being started
    *** The interger part of this std::pair is the countdown timer for this event to be launched
    **/
    std::vector<std::pair<int32_t, MapEvent*> > _active_delayed_events;

    /** \brief A list of all events that are waiting on their launch timers to expire before being started
    *** The interger part of this std::pair is the countdown timer for this event to be launched
    *** Those ones are put on hold by PauseAllEvents() and PauseEvent();
    **/
    std::vector<std::pair<int32_t, MapEvent*> > _paused_delayed_events;

    /** States whether the event supervisor is parsing the active events queue, thus any modifications
    *** there on active events should be avoided.
    **/
    volatile bool _is_updating;

    /** \brief A function that is called whenever an event starts or finishes to examine that event's links
    *** \param parent_event The event that has just started or finished
    *** \param event_start The event has just started if this member is true, or if it just finished it will be false
    **/
    void _ExamineEventLinks(MapEvent* parent_event, bool event_start);

    /** \brief Registers a map event object with the event supervisor
    *** \param new_event A pointer to the new event
    *** \return whether the event was successfully registered.
    *** \note This function should be called by the MapEvent constructor only
    **/
    bool _RegisterEvent(MapEvent* new_event);
};

} // namespace private_map

} // namespace vt_map

#endif // __MAP_EVENT_SUPERVISOR_HEADER__
