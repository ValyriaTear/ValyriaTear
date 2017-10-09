///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_objects/map_halo.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_object_supervisor.h"

#include "engine/video/video.h"

namespace vt_map
{

namespace private_map
{

Halo::Halo(const std::string& filename,
           float x, float y,
           const vt_video::Color& color):
    MapObject(NO_LAYER_OBJECT) // This is a special object
{
    _color = color;
    _tile_position.x = x;
    _tile_position.y = y;

    _object_type = HALO_TYPE;
    _collision_mask = NO_COLLISION;

    if(!_animation.LoadFromAnimationScript(filename))
        PRINT_WARNING << "Couldn't load the Halo animation "
                      << filename << " properly." << std::endl;

    // Setup the image collision for the display update
    SetImgPixelHalfWidth(_animation.GetWidth() / 2.0f);
    SetImgPixelHeight(_animation.GetHeight());

    MapMode::ScaleToMapZoomRatio(_animation);

    // Auto-registers to the object supervisor for later deletion handling
    MapMode::CurrentInstance()->GetObjectSupervisor()->AddHalo(this);
}

Halo* Halo::Create(const std::string& filename,
                   float x, float y,
                   const vt_video::Color& color)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new Halo(filename, x, y, color);
}

void Halo::Update()
{
    if(_updatable)
        _animation.Update();
}

void Halo::Draw()
{
    if(MapObject::ShouldDraw() && _animation.GetCurrentFrame())
        vt_video::VideoManager->DrawHalo(*_animation.GetCurrentFrame(), _color);
}

} // namespace private_map

} // namespace vt_map
