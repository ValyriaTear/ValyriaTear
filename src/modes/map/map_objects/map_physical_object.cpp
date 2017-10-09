///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_objects/map_physical_object.h"

#include "engine/video/video.h"

using namespace vt_common;

namespace vt_map
{

namespace private_map
{

PhysicalObject::PhysicalObject(MapObjectDrawLayer layer) :
    MapObject(layer),
    _current_animation_id(0)
{
    _object_type = PHYSICAL_TYPE;
}

PhysicalObject::~PhysicalObject()
{
    _animations.clear();
}

PhysicalObject* PhysicalObject::Create(MapObjectDrawLayer layer)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new PhysicalObject(layer);
}

void PhysicalObject::Update()
{
    MapObject::Update();
    if(!_animations.empty() && _updatable)
        _animations[_current_animation_id].Update();
}

void PhysicalObject::Draw()
{
    if(_animations.empty() || !MapObject::ShouldDraw())
        return;

    _animations[_current_animation_id].Draw();

    // Draw collision rectangle if the debug view is on.
    if(!vt_video::VideoManager->DebugInfoOn())
        return;

    Position2D position = vt_video::VideoManager->GetDrawPosition();
    Rectangle2D rect = GetScreenCollisionRectangle(position.x, position.y);
    vt_video::VideoManager->DrawRectangle(rect.right - rect.left,
                                          rect.bottom - rect.top,
                                          vt_video::Color(0.0f, 1.0f, 0.0f,
                                                          0.6f));
}

int32_t PhysicalObject::AddAnimation(const std::string& animation_filename)
{
    vt_video::AnimatedImage new_animation;
    if(!new_animation.LoadFromAnimationScript(animation_filename)) {
        PRINT_WARNING << "Could not add animation because the animation filename was invalid: "
                      << animation_filename << std::endl;
        return -1;
    }
    new_animation.SetDimensions(_img_screen_half_width * 2, _img_screen_height);

    _animations.push_back(new_animation);
    return (int32_t)_animations.size() - 1;
}

int32_t PhysicalObject::AddStillFrame(const std::string& image_filename)
{
    vt_video::AnimatedImage new_animation;
    // Adds a frame with a zero length: Making it last forever
    if (!new_animation.AddFrame(image_filename, 0)) {
        PRINT_WARNING << "Could not add a still frame because the image filename was invalid: "
                      << image_filename << std::endl;
        return -1;
    }
    new_animation.SetDimensions(_img_screen_half_width * 2, _img_screen_height);

    _animations.push_back(new_animation);
    return (int32_t)_animations.size() - 1;
}

void PhysicalObject::SetCurrentAnimation(uint32_t animation_id)
{
    if(animation_id < _animations.size()) {
        _animations[_current_animation_id].SetTimeProgress(0);
        _current_animation_id = animation_id;
    }
}

} // namespace private_map

} // namespace vt_map
