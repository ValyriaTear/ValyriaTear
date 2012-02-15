///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shake.h
*** \author  Raj Sharma, roos@allacrost.org
*** \brief   Header file for screen shaking code
*** ***************************************************************************/

#ifndef __SHAKE_HEADER__
#define __SHAKE_HEADER__

#include "defs.h"
#include "utils.h"

namespace hoa_video {

//! \brief Screen shake fall-off modes, which control the behavior of a screen shake.
enum ShakeFalloff {
	VIDEO_FALLOFF_INVALID = -1,
	
	//! Shake remains at constant force
	VIDEO_FALLOFF_NONE = 0,
	
	//! Shake starts out small, builds up, then dies down
	VIDEO_FALLOFF_EASE = 1,
	
	//! Shake strength decreases linearly until the end
	VIDEO_FALLOFF_LINEAR = 2,
	
	//! Shake decreases slowly and drops off quickly at the end
	VIDEO_FALLOFF_GRADUAL = 3,
	
	//! Shake suddenly falls off, used for "impacts"
	VIDEO_FALLOFF_SUDDEN = 4,
	
	VIDEO_FALLOFF_TOTAL = 5
};

namespace private_video {

/** ****************************************************************************
*** \brief Represents the force of a screen shake
***
*** The ShakeForce class holds information about a screen shake, and it is used
*** by the video engine to keep track of how to shake the screen.
*** ***************************************************************************/
class ShakeForce {
public:
	//! \brief The initial force of the shake
	float initial_force;
	
	//! \brief Used to interpolate the shaking motion
	Interpolator interpolator;
	
	//! milliseconds that passed since this shake started
	uint32 current_time;
	
	//! milliseconds that this shake was set to last for
	uint32 end_time;
};

}  // namespace private_video

}  // namespace hoa_video

#endif  // __SHAKE_HEADER__
