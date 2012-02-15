///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////


#include <cassert>
#include <cstdarg>
#include <math.h>

#include "utils.h"
#include "video.h"

using namespace std;
using namespace hoa_video::private_video;
using namespace hoa_utils;

namespace hoa_video  {

// The time between screen shake updates in milliseconds
const uint32 VIDEO_TIME_BETWEEN_SHAKE_UPDATES = 50;



void VideoEngine::ShakeScreen(float force, uint32 falloff_time, ShakeFalloff falloff_method) {
	if (force < 0.0f) {
		if (VIDEO_DEBUG)
			cerr << "VIDEO WARNING: " << __FUNCTION__ << " was passed a negative force value" << endl;
		return;
	}

	if (falloff_method <= VIDEO_FALLOFF_INVALID || falloff_method >= VIDEO_FALLOFF_TOTAL) {
		if (VIDEO_DEBUG)
			cerr << "VIDEO WARNING: " << __FUNCTION__ << " was passed an invalid falloff method" << endl;
		return;
	}
	
	if (falloff_time == 0 && falloff_method != VIDEO_FALLOFF_NONE) {
		if (VIDEO_DEBUG)
			cerr << "VIDEO WARNING: " << __FUNCTION__ << " was called with infinite falloff_time (0), but falloff_method was not VIDEO_FALLOFF_NONE" << endl;
		return;
	}
		
	// Create a new ShakeForce object to represent the shake
	ShakeForce shake;
	shake.current_time  = 0;
	shake.end_time = falloff_time;
	shake.initial_force = force;
	
	// set up the interpolation
	switch (falloff_method) {
		case VIDEO_FALLOFF_NONE:
			shake.interpolator.SetMethod(VIDEO_INTERPOLATE_SRCA);
			shake.interpolator.Start(force, 0.0f, falloff_time);
			break;
		
		case VIDEO_FALLOFF_EASE:
			shake.interpolator.SetMethod(VIDEO_INTERPOLATE_EASE);
			shake.interpolator.Start(0.0f, force, falloff_time);
			break;
		
		case VIDEO_FALLOFF_LINEAR:
			shake.interpolator.SetMethod(VIDEO_INTERPOLATE_LINEAR);
			shake.interpolator.Start(force, 0.0f, falloff_time);
			break;
			
		case VIDEO_FALLOFF_GRADUAL:
			shake.interpolator.SetMethod(VIDEO_INTERPOLATE_SLOW);
			shake.interpolator.Start(force, 0.0f, falloff_time);
			break;
			
		case VIDEO_FALLOFF_SUDDEN:
			shake.interpolator.SetMethod(VIDEO_INTERPOLATE_FAST);
			shake.interpolator.Start(force, 0.0f, falloff_time);
			break;
		
		default:
			if (VIDEO_DEBUG)
				cerr << "VIDEO ERROR: falloff method passed to ShakeScreen() was not supported!" << endl;
			return;
	};
	
	// Add the shake force to the list of shakes
	_shake_forces.push_front(shake);
} // bool VideoEngine::ShakeScreen(float force, uint32 falloff_time, ShakeFalloff falloff_method)



void VideoEngine::_UpdateShake(uint32 frame_time) {
	if (_shake_forces.empty()) {
		_x_shake = 0;
		_y_shake = 0;
		return;
	}

	static uint32 time_til_next_update = 0; // Used to cap the maximum update frequency
	time_til_next_update += frame_time;

	// Return if not enough time has expired to do a shake update
	if (time_til_next_update < VIDEO_TIME_BETWEEN_SHAKE_UPDATES)
		return;

	// First, update all the shake effects based on the time expired.
	// Then calculate the net force, i.e. the sum of forces for all the shakes
	float net_force = 0.0f;

	// NOTE: time_til_next_update now holds the total update time for ShakeForce and Interpolator to use

	for (list<ShakeForce>::iterator i = _shake_forces.begin(); i != _shake_forces.end();) {
		ShakeForce &shake = *i;
		shake.current_time += time_til_next_update;

		if (shake.end_time != 0 && shake.current_time >= shake.end_time) {
			i = _shake_forces.erase(i);
		}
		else {
			shake.interpolator.Update(time_til_next_update);
			net_force += shake.interpolator.GetValue();
			i++;
		}
	}

	time_til_next_update -= VIDEO_TIME_BETWEEN_SHAKE_UPDATES;

	// Calculate random shake offsets using the negative and positive net force values
	// Note that this doesn't produce a radially symmetric distribution of offsets
	_x_shake = _RoundForce(RandomFloat(-net_force, net_force));
	_y_shake = _RoundForce(RandomFloat(-net_force, net_force));	
} // void VideoEngine::_UpdateShake(uint32 frame_time)



float VideoEngine::_RoundForce(float force) {
	int32 fraction_percent = static_cast<int32>(force * 100.0f) - (static_cast<int32>(force) * 100);
	
	int32 random_percent = rand() % 100;
	if (fraction_percent > random_percent)
		force = ceilf(force);
	else
		force = floorf(force);
		
	return force;
}

}  // namespace hoa_video
