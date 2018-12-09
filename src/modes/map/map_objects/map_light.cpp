///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_objects/map_light.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_object_supervisor.h"

#include "engine/video/video.h"

#include "utils/utils_random.h"

using namespace vt_common;

namespace vt_map
{

namespace private_map
{

Light::Light(const std::string& main_flare_filename,
             const std::string& secondary_flare_filename,
             float x, float y,
             const vt_video::Color& main_color,
             const vt_video::Color& secondary_color):
    MapObject(NO_LAYER_OBJECT)
{
    _main_color = main_color;
    _secondary_color = secondary_color;

    _tile_position.x = x;
    _tile_position.y = y;

    _object_type = LIGHT_TYPE;
    _collision_mask = NO_COLLISION;

    _a = _b = 0.0f;
    _distance = 0.0f;

    // For better eye-candy, randomize a bit the secondary flare distances.
    _distance_factor_1 = vt_utils::RandomFloat(8.0f, 12.0f);
    _distance_factor_2 = vt_utils::RandomFloat(17.0f, 23.0f);
    _distance_factor_3 = vt_utils::RandomFloat(12.0f, 18.0f);
    _distance_factor_4 = vt_utils::RandomFloat(5.0f, 9.0f);

    if(_main_animation.LoadFromAnimationScript(main_flare_filename)) {
        // Setup the image collision for the display update
        SetImgPixelHalfWidth(_main_animation.GetWidth() / 3.0f);
        SetImgPixelHeight(_main_animation.GetHeight());

        ScaleToMapZoomRatio(_main_animation);
    }
    if(_secondary_animation.LoadFromAnimationScript(secondary_flare_filename)) {
        ScaleToMapZoomRatio(_secondary_animation);
    }

    // Register the object to the light vector
    MapMode::CurrentInstance()->GetObjectSupervisor()->AddLight(this);
}

Light* Light::Create(const std::string& main_flare_filename,
                     const std::string& secondary_flare_filename,
                     float x, float y,
                     const vt_video::Color& main_color,
                     const vt_video::Color& secondary_color)
{
    // The object auto registers to the object supervisor
    // and will later handle deletion.
    return new Light(main_flare_filename, secondary_flare_filename,
                     x, y, main_color, secondary_color);
}

Rectangle2D Light::GetGridImageRectangle() const
{
    Rectangle2D rect;
    rect.left = _tile_position.x - _img_grid_half_width;
    rect.right = _tile_position.x + _img_grid_half_width;
    // The y coord is also centered in that case
    rect.top = _tile_position.y - (_img_grid_height / 2.0f);
    rect.bottom = _tile_position.y + (_img_grid_height / 2.0f);
    return rect;
}

void Light::_UpdateLightAngle()
{
    MapMode *mm = MapMode::CurrentInstance();
    if(!mm)
        return;
    const MapFrame &frame = mm->GetMapFrame();

    Position2D center;
    center.x = frame.screen_edges.left + (frame.screen_edges.right - frame.screen_edges.left) / 2.0f;
    center.y = frame.screen_edges.top + (frame.screen_edges.bottom - frame.screen_edges.top) / 2.0f;

    // Don't update the distance and angle data in that case.
    if(center.x == _last_center_pos.x && center.y == _last_center_pos.y)
        return;

    _last_center_pos.x = center.x;
    _last_center_pos.y = center.y;

    _distance = sqrtf(_tile_position.GetDistance2(center));

    if(vt_utils::IsFloatEqual(_tile_position.x, center.x, 0.2f))
        _a = 2.5f;
    else
        _a = (_tile_position.y - center.y) / (_tile_position.x - center.x);

    // Prevent angles rough-edges
    if(_a < 0.0f)
        _a = -_a;
    if(_a > 2.5f)
        _a = 2.5f;

    _b = _tile_position.y - _a * _tile_position.x;

    // Update the flare alpha depending on the distance
    float distance = _distance / 5.0f;

    if(distance < 0.0f)
        distance = -distance;
    if(distance < 1.0f)
        distance = 1.0f;

    _main_color_alpha = _main_color;
    _main_color_alpha.SetAlpha(_main_color.GetAlpha() / distance);
    _secondary_color_alpha = _secondary_color;
    _secondary_color_alpha.SetAlpha(_secondary_color.GetAlpha() / distance);
}

void Light::Update()
{
    if(!_updatable)
        return;

    _main_animation.Update();
    _secondary_animation.Update();
    _UpdateLightAngle();
}

void Light::Draw()
{
    if(!MapObject::ShouldDraw() || !_main_animation.GetCurrentFrame())
        return;

    MapMode *mm = MapMode::CurrentInstance();
    if(!mm)
        return;

    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_CENTER,
                                         vt_video::VIDEO_Y_CENTER, 0);

    vt_video::VideoManager->DrawHalo(*_main_animation.GetCurrentFrame(), _main_color_alpha);

    if(!_secondary_animation.GetCurrentFrame()) {
        vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_CENTER,
                                             vt_video::VIDEO_Y_BOTTOM, 0);
        return;
    }

    float next_pos_x = _tile_position.x - _distance / _distance_factor_1;
    float next_pos_y = _a * next_pos_x + _b;

    vt_video::VideoManager->Move(mm->GetScreenXCoordinate(next_pos_x),
                                 mm->GetScreenYCoordinate(next_pos_y));
    vt_video::VideoManager->DrawHalo(*_secondary_animation.GetCurrentFrame(),
                                     _secondary_color_alpha);

    next_pos_x = _tile_position.x - _distance / _distance_factor_2;
    next_pos_y = _a * next_pos_x + _b;
    vt_video::VideoManager->Move(mm->GetScreenXCoordinate(next_pos_x),
                                 mm->GetScreenYCoordinate(next_pos_y));
    vt_video::VideoManager->DrawHalo(*_secondary_animation.GetCurrentFrame(),
                                     _secondary_color_alpha);

    next_pos_x = _tile_position.x + _distance / _distance_factor_3;
    next_pos_y = _a * next_pos_x + _b;
    vt_video::VideoManager->Move(mm->GetScreenXCoordinate(next_pos_x),
                                 mm->GetScreenYCoordinate(next_pos_y));
    vt_video::VideoManager->DrawHalo(*_secondary_animation.GetCurrentFrame(),
                                     _secondary_color_alpha);

    next_pos_x = _tile_position.x + _distance / _distance_factor_4;
    next_pos_y = _a * next_pos_x + _b;
    vt_video::VideoManager->Move(mm->GetScreenXCoordinate(next_pos_x),
                                 mm->GetScreenYCoordinate(next_pos_y));
    vt_video::VideoManager->DrawHalo(*_secondary_animation.GetCurrentFrame(),
                                     _secondary_color_alpha);

    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_CENTER,
                                         vt_video::VIDEO_Y_BOTTOM, 0);
}

} // namespace private_map

} // namespace vt_map
