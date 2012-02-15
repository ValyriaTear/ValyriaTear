///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    interpolator.h
*** \author  Raj Sharma, roos@allacrost.org
*** \brief   Source file for Interpolator class
*** ***************************************************************************/

#include <cassert>
#include <cstdarg>
#include <math.h>

#include "video.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_video::private_video;

namespace hoa_video {

// Controls how slow the slow transform is. The greater the number, the "slower" it is. Should be greater than 1.0f
const float VIDEO_SLOW_TRANSFORM_POWER = 2.0f;

// Controls how fast the fast transform is. The smaller the number, the "faster" it is. Should be between 0.0f and 1.0f
const float VIDEO_FAST_TRANSFORM_POWER = 0.3f;



Interpolator::Interpolator() :
	_method(VIDEO_INTERPOLATE_LINEAR),
	_a(0.0f),
	_b(0.0f),
	_current_time(0),
	_end_time(0),
	_current_value(0.0f),
	_finished(true) // no interpolation is in progress
{}



void Interpolator::Start(float a, float b, uint32 milliseconds) {
	if (_ValidMethod() == false) {
		if (VIDEO_DEBUG)
			cerr << "VIDEO WARNING: " << __FUNCTION__ << " was called when an invalid interpolation method was set" << endl;
		return;
	}

	_a = a;
	_b = b;

	_current_time = 0;
	_end_time = milliseconds;
	_finished = false;

	Update(0);  // Do an initial update so that we have a valid value for GetValue()
}



void Interpolator::SetMethod(InterpolationMethod method) {
	if (_finished == false) {
		if (VIDEO_DEBUG)
			cerr << "VIDEO WARNING: " << __FUNCTION__ << " was called when an interpolation was still in progress" << endl;
		return;
	}

	_method = method;
	if (_ValidMethod() == false) {
		if (VIDEO_DEBUG)
			cerr << "VIDEO WARNING: " << __FUNCTION__ << " was passed an invalid method argument" << endl;
	}
}



void Interpolator::Update(uint32 frame_time) {
	if (_ValidMethod() == false) {
		if (VIDEO_DEBUG)
			cerr << "VIDEO WARNING: " << __FUNCTION__ << " was called when an invalid method was set" << endl;
		return;
	}

	// update current time
	_current_time += frame_time;

	if (_current_time > _end_time) {
		_current_time = _end_time;
		_finished = true;
	}

	// Calculate a value from 0.0f to 1.0f that tells how far we are in the interpolation
	float progress;

	if (_end_time == 0) {
		progress = 1.0f;
	}
	else {
		progress = static_cast<float>(_current_time) / static_cast<float>(_end_time);
	}

	if (progress > 1.0f) {
		if (VIDEO_DEBUG)
			cerr << "VIDEO WARNING: " << __FUNCTION__ << " calculated a progress value greater than 1.0" << endl;
		progress = 1.0f;
	}

	// Apply a transformation based on the interpolation method
	switch(_method) {
		case VIDEO_INTERPOLATE_EASE:
			progress = _EaseTransform(progress);
			break;
		case VIDEO_INTERPOLATE_SRCA:
			progress = 0.0f;
			break;
		case VIDEO_INTERPOLATE_SRCB:
			progress = 1.0f;
			break;
		case VIDEO_INTERPOLATE_FAST:
			progress = _FastTransform(progress);
			break;
		case VIDEO_INTERPOLATE_SLOW:
			progress = _SlowTransform(progress);
			break;
		case VIDEO_INTERPOLATE_LINEAR:
			// Nothing to do, just use progress value as it is
			break;
		default:
			if (VIDEO_DEBUG)
				cerr << "VIDEO WARNING: " << __FUNCTION__ << " the current method did not match any supported methods" << endl;
			return;
	};

	_current_value = Lerp(progress, _a, _b);
} // void Interpolator::Update(uint32 frame_time)



float Interpolator::_FastTransform(float initial_value) {
	return pow(initial_value, VIDEO_FAST_TRANSFORM_POWER);
}



float Interpolator::_SlowTransform(float initial_value) {
	return pow(initial_value, VIDEO_SLOW_TRANSFORM_POWER);
}



float Interpolator::_EaseTransform(float initial_value) {
	return 0.5f * (1.0f + sinf(UTILS_2PI * (initial_value - 0.25f)));
}

}  // namespace hoa_video
