///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/*!****************************************************************************
 * \file    particle_emitter.h
 * \author  Raj Sharma, roos@allacrost.org
 * \brief   Header file for particle emitter
 *
 * This file contains the ParticleEmitter class, and some enumerations for
 * emitter properties.
 *
 * ParticleEmitter stores info about how a system emits particles. This is made
 * up of two things:
 *
 *  1) Where are particles emitted?
 *      -For example, you could create a circle-shaped emitter, and then every
 *         time a particle is emitted, a random spot in that circle is chosen.
 *
 *  2) When are particles emitted?
 *      -For example, you could emit them all at once, or slowly through the
 *         life of the system.
 *****************************************************************************/

#ifndef __PARTICLE_EMITTER_HEADER__
#define __PARTICLE_EMITTER_HEADER__

#include "defs.h"
#include "utils.h"

namespace hoa_video {

/*!***************************************************************************
 *  \brief Specifies whether the orientation for particle rotations should be
 *         counterclockwise, clockwise, or random.
 *****************************************************************************/

enum EMITTER_SPIN
{
	EMITTER_SPIN_INVALID          = -1,
	
	EMITTER_SPIN_CLOCKWISE        = 0,   //! clockwise spin
	EMITTER_SPIN_COUNTERCLOCKWISE = 1,   //! counterclockwise spin
	EMITTER_SPIN_RANDOM           = 2,   //! random spin
	
	EMITTER_SPIN_TOTAL = 3
};


/*!***************************************************************************
 *  \brief Shape of the emitter. A point emitter is the most simple- all
 *         particles come out from a point. Another example is a line emitter.
 *         This may be useful for a snow effect- you place the line at the top
 *         of the screen, and particles will be generated anywhere along that line.
 *****************************************************************************/

enum EMITTER_SHAPE
{
	EMITTER_SHAPE_INVALID          = -1,
	
	EMITTER_SHAPE_POINT            =  0,   //! point (_x, _y)
	EMITTER_SHAPE_LINE             =  1,   //! line from (_x, _y) to (_x2, _y2)
	EMITTER_SHAPE_CIRCLE           =  2,   //! outlined circle with radius of _radius, and transposed by (_x, _y)
	EMITTER_SHAPE_FILLED_CIRCLE    =  3,   //! filled circle with radius of _radius, and transposed by (_x, _y)
	EMITTER_SHAPE_FILLED_RECTANGLE =  4,   //! filled rectangle from (_x, _y) to (_x2, _y2)
	
	EMITTER_SHAPE_TOTAL = 5
};

/*!***************************************************************************
 *  \brief An enumeration of EMITTER modes for particles
 *****************************************************************************/

enum EMITTER_MODE
{
	EMITTER_MODE_INVALID = -1, 
	
	EMITTER_MODE_LOOPING  = 0,   //! particles are emitted continuously
	EMITTER_MODE_ONE_SHOT = 1,   //! particles are emitted for some time, then emitter is disabled
	EMITTER_MODE_BURST    = 2,   //! all particles in the system are emitted at the beginning
	EMITTER_MODE_ALWAYS   = 3,   //! as long as there are free particles, they will be emitted
	
	EMITTER_MODE_TOTAL = 4
};


class ParticleEmitter
{
public:

	//! position of emitter, or in the case of line or rectangle emitters, this is
	//! one point/corner of the emitter
	float _x;	
	float _y;
	
	//! The second point/corner of the emitter, for line or rectangle emitters ONLY
	float _x2;
	float _y2;

	//! position of the emitter's center. In many cases this is just the same thing
	//! as _x and _y, but we store it anyway because then if we have a rectangle
	//! shaped emitter, we don't have to calculate the midpoint every time we want
	//! to know where the center is
	float _center_x;
	float _center_y;

	//! add some variation to the position of each particle
	float _x_variation;
	float _y_variation;

	//! The radius of the emitter, for circular emitters
	float _radius;	
	
	//! shape of the emitter
	EMITTER_SHAPE _shape;

	//! true if emitter should spit out particles in all directions
	bool _omnidirectional;
	
	//! if _omnidirectional is false, then this is the angle at which particles should
	//! be emitted. This angle is the same one as what is used for sinf(), e.g. zero is
	//! right, PI/2 is up, PI is left, 3PI/2 is down, etc.
	float _orientation;
	
	//! outer cone angle, used to create some "spread" in the particle emissions. Set this
	//! to zero if you want all particles to be emitted in exactly the same direction
	float _outer_cone;
	
	//! inner cone angle, used to create some "spread" in the particle emissions. Set this
	//! to zero if you want all particles to be emitted in exactly the same direction
	float _inner_cone;
	
	//! initial speed for particles. The unit is pixels. So, with a coordinate system which
	//! is 1024x768, a particle going from left to right with a speed of 512 pixels/sec could
	//! move across the screen in 2 seconds
	float _initial_speed;
	
	//! variation in the initial speed (it would be boring if all particles had exactly the
	//! same speed)
	float _initial_speed_variation;
		
	//! depending on the emission mode, this specifies how many particles to emit each second
	float _emission_rate;
	
	//! how many seconds to wait until emitting particles. Note that if you want a particle system
	//! to be active for 5 seconds, and start_time is 10 seconds, then you should make the system
	//! lifetime 15 seconds. 
	float _start_time;
	
	//! emitter mode- e.g. burst, always, one shot, looping
	EMITTER_MODE _emitter_mode;
	
	//! emitter spin- whether particles should rotate clockwise, counterclockwise, or mixed (random)
	EMITTER_SPIN _spin;
};

} // namespace hoa_video

#endif  // __PARTICLE_EMITTER_HEADER__
