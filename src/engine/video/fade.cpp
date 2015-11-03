///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    fade.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for ScreenFader class.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "fade.h"

#include "video.h"

#include "engine/mode_manager.h"

using namespace vt_utils;
using namespace vt_mode_manager;

namespace vt_video
{

namespace private_video
{

ScreenFader::ScreenFader() :
    _current_color(0.0f, 0.0f, 0.0f, 0.0f),
    _initial_color(0.0f, 0.0f, 0.0f, 0.0f),
    _final_color(0.0f, 0.0f, 0.0f, 0.0f),
    _current_time(0),
    _end_time(0),
    _is_fading(false),
    _interpolate_rgb_values(false),
    _transitional_fading(false)
{
    // Fading overlay image
    _fade_overlay_img.Load("", 1.0f, 1.0f);
}


void ScreenFader::BeginFade(const Color &final_color, uint32_t duration, bool transitional)
{
    // If last fade is made by the system, don't permit to fade:
    if(!transitional && _is_fading && _transitional_fading)
        return;

    _transitional_fading = transitional;
    _is_fading = true;

    _end_time = duration;

    _initial_color = _current_color;
    _final_color = final_color;
    _current_time = 0;

    // If we are fading to or from transparent, then the RGB values do not need to be interpolated
    if(IsFloatEqual(_final_color[3], 0.0f)) {
        _interpolate_rgb_values = true;
        _current_color[0] = _initial_color[0];
        _current_color[1] = _initial_color[1];
        _current_color[2] = _initial_color[2];
    } else if(IsFloatEqual(_initial_color[3], 0.0f)) {
        _interpolate_rgb_values = true;
        _current_color[0] = _final_color[0];
        _current_color[1] = _final_color[1];
        _current_color[2] = _final_color[2];
    } else {
        _interpolate_rgb_values = false;
    }

    Update(0); // Do an initial update
} // void ScreenFader::BeginFade(const Color &final, uint32_t time)



void ScreenFader::Update(uint32_t time)
{
    if(!_is_fading)
        return;

    // Don't update fading while in pause
    if(ModeManager->GetGameType() == MODE_MANAGER_PAUSE_MODE)
        return;

    // Check for fading finish condition
    if(_current_time >= _end_time) {
        _current_color = _final_color;
        _fade_overlay_img.SetColor(_current_color);
        _is_fading = false;
        return;
    }

    // Calculate the new interpolated color
    float percent_complete = static_cast<float>(_current_time) / static_cast<float>(_end_time);

    if(_interpolate_rgb_values) {
        _current_color[0] = Lerp(percent_complete, _initial_color[0], _final_color[0]);
        _current_color[1] = Lerp(percent_complete, _initial_color[1], _final_color[1]);
        _current_color[2] = Lerp(percent_complete, _initial_color[2], _final_color[2]);
    }
    _current_color[3] = Lerp(percent_complete, _initial_color[3], _final_color[3]);

    _fade_overlay_img.SetColor(_current_color);

    _current_time += time;
}

void ScreenFader::Draw()
{
    if (_current_color == Color::clear)
        return;

    VideoManager->PushState();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);
    // We add a margin in case of screen shaking to avoid unlit parts.
    VideoManager->SetCoordSys(0.1f, 0.9f, 0.9f, 0.1f);
    VideoManager->Move(0.0f, 0.0f);
    _fade_overlay_img.Draw();
    VideoManager->PopState();
}

} // namespace private_video

}  // namespace vt_video
