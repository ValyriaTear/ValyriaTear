///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file   effect_supervisor.cpp
*** \author Yohann Ferreira, yohann ferreira orange fre
*** \brief  Source file for managing visual ambient effects
*** **************************************************************************/

#include "utils/utils_pch.h"
#include "effect_supervisor.h"

#include "engine/script/script_read.h"
#include "engine/audio/audio.h"
#include "engine/video/video.h"

#include "utils/utils_random.h"

using namespace vt_video;

namespace vt_mode_manager
{

EffectSupervisor::EffectSupervisor()
{
    // Initialize the overlays
    // Light
    _info.light.active = false;
    _light_overlay_img.Load("", 1.0f, 1.0f);

    // Texture overlay
    _info.overlay.active = false;
    _info.overlay.x_speed = 0;
    _info.overlay.y_speed = 0;
    _info.overlay.x_shift = 0;
    _info.overlay.y_shift = 0;
    _info.overlay.x_parallax = 0.0f;
    _info.overlay.y_parallax = 0.0f;
    _info.overlay.is_parallax = false;

    // Shake members
    _x_shake = 0.0f;
    _y_shake = 0.0f;
}


void EffectSupervisor::EnableAmbientOverlay(const std::string &filename,
        float x_speed, float y_speed,
        bool parallax)
{
    // Note: The StillImage class handles clearing an image
    // when loading another one.
    _ambient_overlay_img.Clear();

    if(_info.overlay.filename != filename
            && _ambient_overlay_img.Load(filename)) {
        _info.overlay.filename = filename;
        _info.overlay.x_speed = x_speed;
        _info.overlay.y_speed = y_speed;
        _info.overlay.active = true;
        _info.overlay.x_parallax = 0.0f;
        _info.overlay.y_parallax = 0.0f;
        _info.overlay.is_parallax = parallax;
    }
}

void EffectSupervisor::DisableAmbientOverlay()
{
    _info.overlay.active = false;
}


void EffectSupervisor::EnableLightingOverlay(const Color &color)
{
    _info.light.color = color;
    _light_overlay_img.SetColor(color);
    _info.light.active = true;
}


void EffectSupervisor::DisableLightingOverlay()
{
    _info.light.active = false;
}

void EffectSupervisor::Update(uint32 frame_time)
{
    _UpdateAmbientOverlay(frame_time);
    _UpdateShake(frame_time);
}

void EffectSupervisor::_UpdateAmbientOverlay(uint32 frame_time)
{
    if(!_info.overlay.active) {
        // Just reset the parallax values in this case
        // As they are also used for other parallax effects.
        _info.overlay.x_parallax = 0.0f;
        _info.overlay.y_parallax = 0.0f;
        return;
    }

    // Update the shifting
    float elapsed_ms = static_cast<float>(frame_time);
    _info.overlay.x_shift += elapsed_ms / 1000 * _info.overlay.x_speed;
    _info.overlay.y_shift += elapsed_ms / 1000 * _info.overlay.y_speed;

    // Add the parallax values to the shifting and reset them for next update.
    if(_info.overlay.is_parallax) {
        _info.overlay.x_shift += _info.overlay.x_parallax;
        _info.overlay.y_shift += _info.overlay.y_parallax;
    }
    _info.overlay.x_parallax = 0.0f;
    _info.overlay.y_parallax = 0.0f;

    float width = _ambient_overlay_img.GetWidth();
    float height = _ambient_overlay_img.GetHeight();

    // Make them negative to draw on the entire screen
    while(_info.overlay.x_shift > 0.0f) {
        _info.overlay.x_shift -= width;
    }
    // handle negative shifting
    if(_info.overlay.x_shift < 2 * -width)
        _info.overlay.x_shift += width;

    while(_info.overlay.y_shift > 0.0f) {
        _info.overlay.y_shift -= height;
    }
    // handle negative shifting
    if(_info.overlay.y_shift < 2 * -height)
        _info.overlay.y_shift += height;
}

void EffectSupervisor::DrawEffects()
{
    // Draw the textured ambient overlay
    if(_info.overlay.active) {
        VideoManager->PushState();
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);
        VideoManager->SetStandardCoordSys();
        float width = _ambient_overlay_img.GetWidth();
        float height = _ambient_overlay_img.GetHeight();
        for(float x = _info.overlay.x_shift; x <= VIDEO_STANDARD_RES_WIDTH; x = x + width) {
            for(float y = _info.overlay.y_shift; y <= VIDEO_STANDARD_RES_HEIGHT; y = y + height) {
                VideoManager->Move(x, y);
                _ambient_overlay_img.Draw();
            }
        }
        VideoManager->PopState();
    }

    // Draw the light overlay
    if(_info.light.active) {
        VideoManager->PushState();
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        // We use a margin to avoid making the shake effects show unlit parts
        VideoManager->SetCoordSys(0.1f, 0.9f, 0.1f, 0.9f);
        VideoManager->Move(0.0f, 0.0f);
        _light_overlay_img.Draw();
        VideoManager->PopState();
    }
}

void EffectSupervisor::DisableEffects()
{
    DisableAmbientOverlay();
    DisableLightingOverlay();
    StopShaking();
}

void EffectSupervisor::ShakeScreen(float force, uint32 falloff_time, ShakeFalloff falloff_method)
{
    if(force < 0.0f) {
        IF_PRINT_WARNING(VIDEO_DEBUG)
                << " was passed a negative force value" << std::endl;
        return;
    }

    if(falloff_method <= SHAKE_FALLOFF_INVALID || falloff_method >= SHAKE_FALLOFF_TOTAL) {
        IF_PRINT_WARNING(VIDEO_DEBUG)
                << " was passed an invalid falloff method" << std::endl;
        return;
    }

    if(falloff_time == 0 && falloff_method != SHAKE_FALLOFF_NONE) {
        IF_PRINT_WARNING(VIDEO_DEBUG)
                << " was called with infinite falloff_time (0), but falloff_method was not VIDEO_FALLOFF_NONE" << std::endl;
        return;
    }

    // Create a new ShakeForce object to represent the shake
    ShakeForce shake;
    shake.current_time  = 0;
    shake.end_time = falloff_time;
    shake.initial_force = force;

    // set up the interpolation
    switch(falloff_method) {
    case SHAKE_FALLOFF_NONE:
        shake.interpolator.SetMethod(INTERPOLATE_SRCA);
        shake.interpolator.Start(force, 0.0f, falloff_time);
        break;

    case SHAKE_FALLOFF_EASE:
        shake.interpolator.SetMethod(INTERPOLATE_EASE);
        shake.interpolator.Start(0.0f, force, falloff_time);
        break;

    case SHAKE_FALLOFF_LINEAR:
        shake.interpolator.SetMethod(INTERPOLATE_LINEAR);
        shake.interpolator.Start(force, 0.0f, falloff_time);
        break;

    case SHAKE_FALLOFF_GRADUAL:
        shake.interpolator.SetMethod(INTERPOLATE_SLOW);
        shake.interpolator.Start(force, 0.0f, falloff_time);
        break;

    case SHAKE_FALLOFF_SUDDEN:
        shake.interpolator.SetMethod(INTERPOLATE_FAST);
        shake.interpolator.Start(force, 0.0f, falloff_time);
        break;

    default:
        IF_PRINT_WARNING(VIDEO_DEBUG)
                << "VIDEO ERROR: falloff method passed to ShakeScreen() was not supported!" << std::endl;
        return;
    };

    // Add the shake force to the list of shakes
    _shake_forces.push_front(shake);
}

/** \brief Rounds a force value to the nearest integer based on probability.
*** \param force  The force to round
*** \return the rounded force value
*** \note For example, a force value of 2.85 has an 85% chance of rounding to 3 and a 15% chance of rounding to 2. This rounding
*** methodology is necessary because for force values less than 1 (e.g. 0.5f), the shake force would always round down to zero
*** even though there is positive force.
**/
static float _RoundForce(float force)
{
    int32 fraction_percent = static_cast<int32>(force * 100.0f) - (static_cast<int32>(force) * 100);

    int32 random_percent = rand() % 100;
    if(fraction_percent > random_percent)
        force = ceilf(force);
    else
        force = floorf(force);

    return force;
}

void EffectSupervisor::_UpdateShake(uint32 frame_time)
{
    const uint32 TIME_BETWEEN_SHAKE_UPDATES = 50;

    if(_shake_forces.empty()) {
        _x_shake = 0.0f;
        _y_shake = 0.0f;
        return;
    }

    static uint32 time_til_next_update = 0; // Used to cap the maximum update frequency
    time_til_next_update += frame_time;

    // Return if not enough time has expired to do a shake update
    if(time_til_next_update < TIME_BETWEEN_SHAKE_UPDATES)
        return;

    // First, update all the shake effects based on the time expired.
    // Then calculate the net force, i.e. the sum of forces for all the shakes
    float net_force = 0.0f;

    // NOTE: time_til_next_update now holds the total update time for ShakeForce and Interpolator to use

    for(std::deque<ShakeForce>::iterator it = _shake_forces.begin(); it != _shake_forces.end();) {
        ShakeForce &shake = *it;
        shake.current_time += time_til_next_update;

        if(shake.end_time != 0 && shake.current_time >= shake.end_time) {
            it = _shake_forces.erase(it);
        } else {
            shake.interpolator.Update(time_til_next_update);
            net_force += shake.interpolator.GetValue();
            ++it;
        }
    }

    time_til_next_update -= TIME_BETWEEN_SHAKE_UPDATES;

    // Calculate random shake offsets using the negative and positive net force values
    // Note that this doesn't produce a radially symmetric distribution of offsets
    _x_shake = _RoundForce(vt_utils::RandomFloat(-net_force, net_force));
    _y_shake = _RoundForce(vt_utils::RandomFloat(-net_force, net_force));
}

} // namespace vt_mode_manager
