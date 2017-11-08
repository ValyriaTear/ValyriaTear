///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    interpolator.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for Interpolator class
*** ***************************************************************************/

#include "common/include_pch.h"
#include "interpolator.h"

#include "utils/utils_numeric.h"

namespace vt_mode_manager
{

extern bool MODE_MANAGER_DEBUG;

// Controls how slow the slow transform is. The greater the number, the "slower" it is. Should be greater than 1.0f
const float SLOW_TRANSFORM_POWER = 2.0f;

// Controls how fast the fast transform is. The smaller the number, the "faster" it is. Should be between 0.0f and 1.0f
const float FAST_TRANSFORM_POWER = 0.3f;

/** \brief Interpolates logarithmically. Increases quickly and then levels off.
*** \param initial_value The initial value to interpolate. Should be between 0.0f and 1.0f (inclusive)
*** \return The interpolated value
**/
static float _FastTransform(float initial_value)
{
    return pow(initial_value, FAST_TRANSFORM_POWER);
}

/** \brief Interpolates exponentially. Increases slowly and then sky rockets.
*** \param initial_value The initial value to interpolate. Should be between 0.0f and 1.0f (inclusive)
*** \return The interpolated value
**/
static float _SlowTransform(float initial_value)
{
    return pow(initial_value, SLOW_TRANSFORM_POWER);
}

/** \brief Interpolates periodically. Increases slowly to 1.0f then back down to 0.0f via a sine function.
*** \param initial_value The initial value to interpolate. Should be between 0.0f and 1.0f (inclusive)
*** \return The interpolated value
**/
static float _EaseTransform(float initial_value)
{
    return 0.5f * (1.0f + sinf(vt_utils::UTILS_2PI * (initial_value - 0.25f)));
}

Interpolator::Interpolator() :
    _method(INTERPOLATE_LINEAR),
    _a(0.0f),
    _b(0.0f),
    _current_time(0),
    _end_time(0),
    _current_value(0.0f),
    _finished(true) // no interpolation is in progress
{}



void Interpolator::Start(float a, float b, uint32_t milliseconds)
{
    if(_ValidMethod() == false) {
        IF_PRINT_WARNING(MODE_MANAGER_DEBUG)
                << " was called when an invalid interpolation method was set" << std::endl;
        return;
    }

    _a = a;
    _b = b;

    _current_time = 0;
    _end_time = milliseconds;
    _finished = false;

    Update(0);  // Do an initial update so that we have a valid value for GetValue()
}



void Interpolator::SetMethod(InterpolationMethod method)
{
    if(_finished == false) {
        IF_PRINT_WARNING(MODE_MANAGER_DEBUG)
                << " was called when an interpolation was still in progress" << std::endl;
        return;
    }

    _method = method;
    if(_ValidMethod() == false) {
        IF_PRINT_WARNING(MODE_MANAGER_DEBUG)
                << " was passed an invalid method argument" << std::endl;
    }
}



void Interpolator::Update(uint32_t frame_time)
{
    if(_ValidMethod() == false) {
        IF_PRINT_WARNING(MODE_MANAGER_DEBUG)
                << " was called when an invalid method was set" << std::endl;
        return;
    }

    // update current time
    _current_time += frame_time;

    if(_current_time > _end_time) {
        _current_time = _end_time;
        _finished = true;
    }

    // Calculate a value from 0.0f to 1.0f that tells how far we are in the interpolation
    float progress;

    if(_end_time == 0) {
        progress = 1.0f;
    } else {
        progress = static_cast<float>(_current_time) / static_cast<float>(_end_time);
    }

    if(progress > 1.0f) {
        IF_PRINT_WARNING(MODE_MANAGER_DEBUG)
                << " calculated a progress value greater than 1.0" << std::endl;
        progress = 1.0f;
    }

    // Apply a transformation based on the interpolation method
    switch(_method) {
    case INTERPOLATE_EASE:
        progress = _EaseTransform(progress);
        break;
    case INTERPOLATE_SRCA:
        progress = 0.0f;
        break;
    case INTERPOLATE_SRCB:
        progress = 1.0f;
        break;
    case INTERPOLATE_FAST:
        progress = _FastTransform(progress);
        break;
    case INTERPOLATE_SLOW:
        progress = _SlowTransform(progress);
        break;
    case INTERPOLATE_LINEAR:
        // Nothing to do, just use progress value as it is
        break;
    default:
        IF_PRINT_WARNING(MODE_MANAGER_DEBUG)
                << " the current method did not match any supported methods" << std::endl;
        return;
    };

    _current_value = vt_utils::Lerp(progress, _a, _b);
} // void Interpolator::Update(uint32_t frame_time)

}  // namespace vt_video
