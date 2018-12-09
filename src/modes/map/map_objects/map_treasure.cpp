///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_objects/map_treasure.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_event_supervisor.h"

#include "common/global/global.h"

namespace vt_map
{

namespace private_map
{

TreasureObject::TreasureObject(const std::string& treasure_name,
                               MapObjectDrawLayer layer,
                               const std::string& closed_animation_file,
                               const std::string& opening_animation_file,
                               const std::string& open_animation_file) :
    PhysicalObject(layer)
{
    _object_type = TREASURE_TYPE;
    _events_triggered = false;
    _is_opening = false;

    _treasure_name = treasure_name;
    if(treasure_name.empty())
        PRINT_WARNING << "Empty treasure name found. The treasure won't function normally." << std::endl;

    _treasure = new vt_map::private_map::MapTreasureContent();

    // Dissect the frames and create the closed, opening, and open animations
    vt_video::AnimatedImage closed_anim, opening_anim, open_anim;

    closed_anim.LoadFromAnimationScript(closed_animation_file);
    if(!opening_animation_file.empty())
        opening_anim.LoadFromAnimationScript(opening_animation_file);
    open_anim.LoadFromAnimationScript(open_animation_file);

    // Set the collision rectangle according to the dimensions of the first frame
    SetCollPixelHalfWidth(closed_anim.GetWidth() / 2.0f);
    SetCollPixelHeight(closed_anim.GetHeight());

    // Apply the zoom ratio on the animations.
    ScaleToMapZoomRatio(closed_anim);
    ScaleToMapZoomRatio(opening_anim);
    ScaleToMapZoomRatio(open_anim);

    AddAnimation(closed_anim);
    AddAnimation(opening_anim);
    AddAnimation(open_anim);

    _LoadState();
}

TreasureObject* TreasureObject::Create(const std::string &treasure_name,
                                       MapObjectDrawLayer layer,
                                       const std::string &closed_animation_file,
                                       const std::string &opening_animation_file,
                                       const std::string &open_animation_file)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new TreasureObject(treasure_name, layer,
                              closed_animation_file,
                              opening_animation_file,
                              open_animation_file);
}

void TreasureObject::_LoadState()
{
    if(!_treasure)
        return;

    // If the event exists, the treasure has already been opened
    if(vt_global::GlobalManager->GetGameEvents().DoesEventExist("treasures", _treasure_name)) {
        SetCurrentAnimation(TREASURE_OPEN_ANIM);
        _treasure->SetTaken(true);
    }
}

void TreasureObject::Open()
{
    if(!_treasure) {
        PRINT_ERROR << "Can't open treasure with invalid treasure content." << std::endl;
        return;
    }

    if(_treasure->IsTaken()) {
        IF_PRINT_WARNING(MAP_DEBUG) << "attempted to retrieve an already taken treasure: " << _object_id << std::endl;
        return;
    }

    // Test whether events should be triggered
    if (_events.empty())
        _events_triggered = true;

    SetCurrentAnimation(TREASURE_OPENING_ANIM);
    _is_opening = true;
}

void TreasureObject::Update()
{
    PhysicalObject::Update();

    if ((GetCurrentAnimationId() == TREASURE_OPENING_ANIM) && (_animations[TREASURE_OPENING_ANIM].IsAnimationFinished()))
        SetCurrentAnimation(TREASURE_OPEN_ANIM);

    if (!_is_opening || GetCurrentAnimationId() != TREASURE_OPEN_ANIM)
        return;

    MapMode* mm = MapMode::CurrentInstance();

    // Once opened, we handle potential events and the display of the treasure supervisor
    EventSupervisor* event_manager = mm->GetEventSupervisor();

    if (!_events_triggered) {
        // Trigger potential events after opening
        for (uint32_t i = 0; i < _events.size(); ++i) {
            if (!event_manager->IsEventActive(_events[i]))
                 event_manager->StartEvent(_events[i]);
        }
        _events_triggered = true;
    }
    else if (!_events.empty()) {
        // Test whether the events have finished
        std::vector<std::string>::iterator it = _events.begin();
        for (; it != _events.end();) {
            // Once the event has finished, we forget it
            if (!event_manager->IsEventActive(*it))
                it = _events.erase(it);
            else
                ++it;
        }
    }
    else {
        // Once all events are finished, we can open the treasure supervisor
        mm->GetTreasureSupervisor()->Initialize(this);
        // Add an event to the treasures group indicating that the treasure has now been opened
        vt_global::GlobalManager->GetGameEvents().SetEventValue("treasures", _treasure_name, 1);
        // End the opening sequence
        _is_opening = false;
    }
}

bool TreasureObject::AddItem(uint32_t id, uint32_t quantity)
{
    if(!_treasure)
        return false;
    return _treasure->AddItem(id, quantity);
}

void TreasureObject::AddEvent(const std::string& event_id)
{
    if (!event_id.empty())
        _events.push_back(event_id);
}

} // namespace private_map

} // namespace vt_map
