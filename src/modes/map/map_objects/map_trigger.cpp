///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_objects/map_trigger.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_event_supervisor.h"
#include "modes/map/map_sprites/map_virtual_sprite.h"

#include "common/global/global.h"

namespace vt_map
{

namespace private_map
{

TriggerObject::TriggerObject(const std::string &trigger_name,
                             MapObjectDrawLayer layer,
                             const std::string &off_animation_file,
                             const std::string &on_animation_file,
                             const std::string& off_event_id,
                             const std::string& on_event_id) :
    PhysicalObject(layer),
    _trigger_state(false)
{
    _object_type = TRIGGER_TYPE;

    _trigger_name = trigger_name;

    _off_event = off_event_id;
    _on_event = on_event_id;

    // By default, the player can step on it to toggle its state.
    _triggerable_by_character = true;

    // Dissect the frames and create the closed, opening, and open animations
    vt_video::AnimatedImage off_anim, on_anim;

    off_anim.LoadFromAnimationScript(off_animation_file);
    on_anim.LoadFromAnimationScript(on_animation_file);

    // Set a default collision area making the trigger respond when the character
    // is rather having his/her two feet on it.
    SetCollPixelHalfWidth(off_anim.GetWidth() / 4.0f);
    SetCollPixelHeight(off_anim.GetHeight() * 2.0f / 3.0f);
    SetImgPixelHalfWidth(off_anim.GetWidth() / 2.0f);
    SetImgPixelHeight(off_anim.GetHeight());

    ScaleToMapZoomRatio(off_anim);
    ScaleToMapZoomRatio(on_anim);

    AddAnimation(off_anim);
    AddAnimation(on_anim);


    _LoadState();
}

TriggerObject* TriggerObject::Create(const std::string &trigger_name,
                                     MapObjectDrawLayer layer,
                                     const std::string &off_animation_file,
                                     const std::string &on_animation_file,
                                     const std::string& off_event_id,
                                     const std::string& on_event_id)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new TriggerObject(trigger_name, layer,
                             off_animation_file, on_animation_file,
                             off_event_id, on_event_id);
}

void TriggerObject::Update()
{
    PhysicalObject::Update();

    // The trigger can't be toggle by the character, nothing will happen
    if (!_triggerable_by_character)
        return;

    // TODO: Permit other behaviour
    if (_trigger_state)
        return;

    MapMode *map_mode = MapMode::CurrentInstance();
    if (!map_mode->IsCameraOnVirtualFocus()
            && GetGridCollisionRectangle().IntersectsWith(map_mode->GetCamera()->GetGridCollisionRectangle())) {
        map_mode->GetCamera()->SetMoving(false);
        SetState(true);
    }

}

void TriggerObject::_LoadState()
{
    if(_trigger_name.empty())
        return;

    // If the event value is equal to 1, the trigger has been triggered.
    if(vt_global::GlobalManager->GetGameEvents().GetEventValue("triggers", _trigger_name) == 1) {
        SetCurrentAnimation(TRIGGER_ON_ANIM);
        _trigger_state = true;
    }
    else {
        SetCurrentAnimation(TRIGGER_OFF_ANIM);
        _trigger_state = false;
    }
}

void TriggerObject::SetState(bool state)
{
    if (_trigger_state == state)
        return;

    _trigger_state = state;

    // If the event exists, the treasure has already been opened
    EventSupervisor* event_supervisor = MapMode::CurrentInstance()->GetEventSupervisor();
    if(_trigger_state) {
        SetCurrentAnimation(TRIGGER_ON_ANIM);
        if (!_on_event.empty())
            event_supervisor->StartEvent(_on_event);
        vt_global::GlobalManager->GetGameEvents().SetEventValue("triggers", _trigger_name, 1);
    }
    else {
        SetCurrentAnimation(TRIGGER_OFF_ANIM);
        if (!_off_event.empty())
            event_supervisor->StartEvent(_off_event);
        vt_global::GlobalManager->GetGameEvents().SetEventValue("triggers", _trigger_name, 0);
    }
}

} // namespace private_map

} // namespace vt_map
