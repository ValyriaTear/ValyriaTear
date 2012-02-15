///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/*!****************************************************************************
 * \file    particle_keyframe.h
 * \author  Raj Sharma, roos@allacrost.org
 * \brief   Header file for particle keyframes
 *
 * Particle properties are keyframed- for example, you can vary the size of a
 * particle along its lifetime, to create some interesting effects. The
 * ParticleKeyframe class contains all of the keyframed properties for a given
 * snapshot in time (time ranges from 0.0 to 1.0), and these keyframes are stored
 * in the ParticleSystemDef class.
 *****************************************************************************/

#ifndef __PARTICLE_KEYFRAME_HEADER__
#define __PARTICLE_KEYFRAME_HEADER__

#include "defs.h"
#include "utils.h"
#include "color.h"

namespace hoa_video
{

namespace private_video
{


/*!***************************************************************************
 *  \brief Keyframes, consist of a _time, plus various properties. These are
 *         used to specify how the properties of a particle vary over time.
 *         For example, in most systems, we'll want particles to fade out over
 *         their lifetime, so this could be done by creating 2 keyframes, where
 *         the alpha component of the 2nd keyframe's _color is zero.
 *****************************************************************************/

class ParticleKeyframe
{
public:

	ParticleKeyframe()
	: size_x(0.0f),
	  size_y(0.0f),
	  color(0.0f, 0.0f, 0.0f, 0.0f),
	  rotation_speed(0.0f),
	  size_variation_x(0.0f),
	  size_variation_y(0.0f),
	  rotation_speed_variation(0.0f),
	  color_variation(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	//! width and height scale. 1.0 means to use the normal height
	float size_x;
	float size_y;
	
	//! color (includes alpha)
	Color color;
	
	//! rotation speed, radians per second clockwise
	float rotation_speed;

	//! random variation added to size
	float size_variation_x;
	float size_variation_y;
	
	//! random variation added to rotation speed
	float rotation_speed_variation;
	
	//! random variation added to color (each channel contains
	//! the variation for that channel)
	Color color_variation;

	float time;	
};


}  // namespace private_video
}  // namespace hoa_video

#endif  //! __PARTICLE_KEYFRAME_HEADER__
