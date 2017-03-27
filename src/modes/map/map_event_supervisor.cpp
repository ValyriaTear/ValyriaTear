///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_event_supervisor.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_sprites.h"

#include "engine/system.h"

namespace vt_map
{

namespace private_map
{

EventSupervisor::~EventSupervisor()
{
    _active_events.clear();
    _paused_events.clear();
    _active_delayed_events.clear();
    _paused_delayed_events.clear();

    for(std::map<std::string, MapEvent *>::iterator it = _all_events.begin(); it != _all_events.end(); ++it) {
        delete it->second;
    }
    _all_events.clear();
}

void EventSupervisor::StartEvent(const std::string &event_id)
{
    MapEvent *event = GetEvent(event_id);
    if(event == nullptr) {
        PRINT_WARNING << "No event with this ID existed: '" << event_id
            << "' in map script: "
            << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    StartEvent(event);
}

void EventSupervisor::StartEvent(const std::string &event_id, uint32_t launch_time)
{
    MapEvent *event = GetEvent(event_id);
    if(event == nullptr) {
        PRINT_WARNING << "No event with this ID existed: '" << event_id
            << "' in map script: "
            << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    if(launch_time == 0)
        StartEvent(event);
    else
        _active_delayed_events.push_back(std::make_pair(static_cast<int32_t>(launch_time), event));
}

void EventSupervisor::StartEvent(MapEvent *event, uint32_t launch_time)
{
    if(event == nullptr) {
        IF_PRINT_WARNING(MAP_DEBUG) << "nullptr argument passed to function"
                                    << std::endl;
        return;
    }

    if(launch_time == 0)
        StartEvent(event);
    else
        _active_delayed_events.push_back(std::make_pair(static_cast<int32_t>(launch_time), event));
}

void EventSupervisor::StartEvent(MapEvent *event)
{
    if(!event) {
        PRINT_WARNING << "nullptr argument passed to function" << std::endl;
        return;
    }

    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to start the event: '" << event->GetEventID()
                      << "' within an update function. The StartEvent() call will be ignored. "
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    for(std::vector<MapEvent *>::iterator it = _active_events.begin(); it != _active_events.end(); ++it) {
        if((*it) == event) {
            PRINT_WARNING << "The event: '" << event->GetEventID()
                          << "' is already active and can be active only once at a time. "
                          << "The StartEvent() call will be ignored."
                          << std::endl << " You should fix the map script: "
                          << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
            return;
        }
    }

    _active_events.push_back(event);
    event->_Start();
    _ExamineEventLinks(event, true);
}

void EventSupervisor::PauseEvent(const std::string &event_id)
{
    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to pause the event: '" << event_id
                      << "' within an update function. The PauseEvent() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    // Search for active ones
    for(std::vector<MapEvent *>::iterator it = _active_events.begin(); it != _active_events.end();) {
        if((*it)->_event_id == event_id) {
            _paused_events.push_back(*it);
            it = _active_events.erase(it);
        } else {
            ++it;
        }
    }

    // and for the delayed ones
    for(std::vector<std::pair<int32_t, MapEvent *> >::iterator it = _active_delayed_events.begin();
            it != _active_delayed_events.end();) {
        if((*it).second->_event_id == event_id) {
            _paused_delayed_events.push_back(*it);
            it = _active_delayed_events.erase(it);
        } else {
            ++it;
        }
    }
}

void EventSupervisor::PauseAllEvents(VirtualSprite *sprite)
{
    if(!sprite)
        return;
    // Examine all potential active (now or later) events

    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to pause all events for sprite: " << sprite->GetObjectID()
                      << " within an update function. The PauseAllEvents() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    // Starting by active ones.
    for(std::vector<MapEvent *>::iterator it = _active_events.begin(); it != _active_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>(*it);
        if(event && event->GetSprite() == sprite) {
            _paused_events.push_back(*it);
            it = _active_events.erase(it);
        } else {
            ++it;
        }
    }

    // Looking at incoming ones.
    for(std::vector<std::pair<int32_t, MapEvent *> >::iterator it = _active_delayed_events.begin();
            it != _active_delayed_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>((*it).second);
        if(event && event->GetSprite() == sprite) {
            _paused_delayed_events.push_back(*it);
            it = _active_delayed_events.erase(it);
        } else {
            ++it;
        }
    }
}

void EventSupervisor::ResumeEvent(const std::string &event_id)
{
    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to resume event: '" << event_id
                      << "' within an update function. The ResumeEvent() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    for(std::vector<MapEvent *>::iterator it = _paused_events.begin();
            it != _paused_events.end();) {
        if((*it)->_event_id == event_id) {
            _active_events.push_back(*it);
            it = _paused_events.erase(it);
        } else {
            ++it;
        }
    }

    // and the delayed ones
    for(std::vector<std::pair<int32_t, MapEvent *> >::iterator it = _paused_delayed_events.begin();
            it != _paused_delayed_events.end();) {
        if((*it).second->_event_id == event_id) {
            _active_delayed_events.push_back(*it);
            it = _paused_delayed_events.erase(it);
        } else {
            ++it;
        }
    }
}

void EventSupervisor::ResumeAllEvents(VirtualSprite *sprite)
{
    if(!sprite)
        return;
    // Examine all potential active (now or later) events

    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to resume all events for sprite: " << sprite->GetObjectID()
                      << " within an update function. The EndAllEvents() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    // Starting by active ones.
    for(std::vector<MapEvent *>::iterator it = _paused_events.begin(); it != _paused_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>(*it);
        if(event && event->GetSprite() == sprite) {
            _active_events.push_back(*it);
            it = _paused_events.erase(it);
        } else {
            ++it;
        }
    }

    // Looking at incoming ones.
    for(std::vector<std::pair<int32_t, MapEvent *> >::iterator it = _paused_delayed_events.begin();
            it != _paused_delayed_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>((*it).second);
        if(event && event->GetSprite() == sprite) {
            _active_delayed_events.push_back(*it);
            it = _paused_delayed_events.erase(it);
        } else {
            ++it;
        }
    }
}

void EventSupervisor::EndEvent(const std::string &event_id, bool trigger_event_links)
{
    // Examine all potential active (now or later) events

    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to terminate the event: '" << event_id
                      << "' within an update function. The EndEvent() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    // Starting by active ones.
    for(std::vector<MapEvent *>::iterator it = _active_events.begin(); it != _active_events.end();) {
        if((*it)->_event_id == event_id) {
            SpriteEvent *sprite_event = dynamic_cast<SpriteEvent *>(*it);
            // Terminated sprite events need to release their owned sprite.
            if(sprite_event)
                sprite_event->Terminate();

            MapEvent *terminated_event = *it;
            it = _active_events.erase(it);
            // We examine the event links only after the event has been removed from the active list
            if(trigger_event_links)
                _ExamineEventLinks(terminated_event, false);
        } else {
            ++it;
        }
    }

    // Looking at incoming ones.
    for(std::vector<std::pair<int32_t, MapEvent *> >::iterator it = _active_delayed_events.begin();
            it != _active_delayed_events.end();) {
        if((*it).second->_event_id == event_id) {
            MapEvent *terminated_event = (*it).second;
            it = _active_delayed_events.erase(it);

            // We examine the event links only after the event has been removed from the active list
            if(trigger_event_links)
                _ExamineEventLinks(terminated_event, false);
        } else {
            ++it;
        }
    }

    // And paused ones
    for(std::vector<MapEvent *>::iterator it = _paused_events.begin(); it != _paused_events.end();) {
        if((*it)->_event_id == event_id) {
            SpriteEvent *sprite_event = dynamic_cast<SpriteEvent *>(*it);
            // Paused sprite events need to release their owned sprite as they have been previously started.
            if(sprite_event)
                sprite_event->Terminate();

            MapEvent *terminated_event = *it;
            it = _paused_events.erase(it);
            // We examine the event links only after the event has been removed from the list
            if(trigger_event_links)
                _ExamineEventLinks(terminated_event, false);
        } else {
            ++it;
        }
    }

    for(std::vector<std::pair<int32_t, MapEvent *> >::iterator it = _paused_delayed_events.begin();
            it != _paused_delayed_events.end();) {
        if((*it).second->_event_id == event_id) {
            MapEvent *terminated_event = (*it).second;
            it = _paused_delayed_events.erase(it);

            // We examine the event links only after the event has been removed from the list
            if(trigger_event_links)
                _ExamineEventLinks(terminated_event, false);
        } else {
            ++it;
        }
    }
}

void EventSupervisor::EndEvent(MapEvent *event, bool trigger_event_links)
{
    if(!event) {
        PRINT_ERROR << "Couldn't terminate nullptr event" << std::endl;
        return;
    }

    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to terminate the event: '" << event->GetEventID()
                      << "' within an update function. The EndEvent() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    EndEvent(event->GetEventID(), trigger_event_links);
}

void EventSupervisor::EndAllEvents(VirtualSprite *sprite)
{
    if(!sprite)
        return;
    // Examine all potential active (now or later) events

    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to terminate all events for sprite: " << sprite->GetObjectID()
                      << " within an update function. The EndAllEvents() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    // Starting by active ones.
    for(std::vector<MapEvent *>::iterator it = _active_events.begin(); it != _active_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>(*it);
        if(event && event->GetSprite() == sprite) {
            // Active events need to release their owned sprite upon termination.
            event->Terminate();

            it = _active_events.erase(it);
        } else {
            ++it;
        }
    }

    // Looking at incoming ones.
    for(std::vector<std::pair<int32_t, MapEvent *> >::iterator it = _active_delayed_events.begin();
            it != _active_delayed_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>((*it).second);
        if(event && event->GetSprite() == sprite)
            it = _active_delayed_events.erase(it);
        else
            ++it;
    }

    // And paused ones
    for(std::vector<MapEvent *>::iterator it = _paused_events.begin(); it != _paused_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>(*it);
        if(event && event->GetSprite() == sprite) {
            // Paused events have been started, so they might need to release their owned sprite.
            event->Terminate();

            it = _paused_events.erase(it);
        } else {
            ++it;
        }
    }

    for(std::vector<std::pair<int32_t, MapEvent *> >::iterator it = _paused_delayed_events.begin();
            it != _paused_delayed_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>((*it).second);
        if(event && event->GetSprite() == sprite)
            it = _paused_delayed_events.erase(it);
        else
            ++it;
    }
}

void EventSupervisor::Update()
{
    // Store the events that became active in the delayed event loop.
    std::vector<MapEvent *> events_to_start;

    // Update all launch event timers and start all events whose timers have finished
    for(std::vector<std::pair<int32_t, MapEvent *> >::iterator it = _active_delayed_events.begin();
            it != _active_delayed_events.end();) {
        it->first -= vt_system::SystemManager->GetUpdateTime();

        if(it->first <= 0) {  // Timer has expired
            MapEvent *start_event = it->second;
            it = _active_delayed_events.erase(it);

            // We add the event ready to start i a vector, waiting for the loop to end
            // before starting it.
            events_to_start.push_back(start_event);
        } else {
            ++it;
        }
    }

    // Starts the events that became active.
    for(std::vector<MapEvent *>::iterator it = events_to_start.begin(); it != events_to_start.end(); ++it)
        StartEvent(*it);

    // Store the events that ended within the update loop.
    std::vector<MapEvent *> finished_events;

    // Make the engine aware that the event supervisor is entering the event update loop
    _is_updating = true;

    // Check for active events which have finished
    for(std::vector<MapEvent *>::iterator it = _active_events.begin(); it != _active_events.end();) {
        if((*it)->_Update()) {
            // Add it ot the finished events list
            finished_events.push_back(*it);

            // Remove the finished event from the active queue.
            it = _active_events.erase(it);
        } else {
            ++it;
        }
    }

    _is_updating = false;

    // We examine the event links only after the events has been removed from the active list
    // and the active list has finished parsing, to avoid a crash when adding a new event within the update loop.
    for(std::vector<MapEvent *>::iterator it = finished_events.begin(); it != finished_events.end(); ++it) {
        _ExamineEventLinks(*it, false);
    }
}

bool EventSupervisor::IsEventActive(const std::string &event_id) const
{
    for(std::vector<MapEvent *>::const_iterator it = _active_events.begin(); it != _active_events.end(); ++it) {
        if((*it)->_event_id == event_id) {
            return true;
        }
    }
    return false;
}

MapEvent *EventSupervisor::GetEvent(const std::string &event_id) const
{
    std::map<std::string, MapEvent *>::const_iterator it = _all_events.find(event_id);

    if(it == _all_events.end())
        return nullptr;
    else
        return it->second;
}

bool EventSupervisor::_RegisterEvent(MapEvent* new_event)
{
    if(new_event == nullptr) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function argument was nullptr" << std::endl;
        return false;
    }

    if(GetEvent(new_event->_event_id) != nullptr) {
        PRINT_WARNING << "The event with this ID already existed: '"
                      << new_event->_event_id
                      << "' in map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return false;
    }

    _all_events.insert(std::make_pair(new_event->_event_id, new_event));
    return true;
}

void EventSupervisor::_ExamineEventLinks(MapEvent *parent_event, bool event_start)
{
    for(uint32_t i = 0; i < parent_event->_event_links.size(); ++i) {
        EventLink &link = parent_event->_event_links[i];

        // Case 1: Start/finish launch member is not equal to the start/finish status of the parent event, so ignore this link
        if(link.launch_at_start != event_start) {
            continue;
        }
        // Case 2: The child event is to be launched immediately
        else if(link.launch_timer == 0) {
            StartEvent(link.child_event_id);
        }
        // Case 3: The child event has a timer associated with it and needs to be placed in the event launch container
        else {
            MapEvent *child = GetEvent(link.child_event_id);
            if(child == nullptr) {
                PRINT_WARNING << "Couldn't launch child event, no event with this ID existed: '"
                              << link.child_event_id << "' from parent event ID: '"
                              << parent_event->GetEventID()
                              << "' in map script: "
                              << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
                continue;
            } else {
                _active_delayed_events.push_back(std::make_pair(static_cast<int32_t>(link.launch_timer), child));
            }
        }
    }
}

} // namespace private_map

} // namespace vt_map
