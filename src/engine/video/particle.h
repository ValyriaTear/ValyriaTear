///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    particle.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for particle data
***
*** This file contains structure(s) for representing a single particle. In theory,
*** we should be able to just have one structure which holds all properties for
*** a particle, but in fact we have two: one for the position, and one for everything
*** else. The reason is that having the positions separated is more efficient
*** for rendering.
*** **************************************************************************/

#ifndef __PARTICLE_HEADER__
#define __PARTICLE_HEADER__

#include "particle_keyframe.h"

namespace vt_mode_manager
{

/*!***************************************************************************
 *  \brief Every time the particle system is rendered, we need to iterate through
 *         all the particles in the system, and use the position, size, and
 *         rotation to generate 4 ParticleVertex's. This is pretty expensive, but
 *         unfortunately it's necessary since the positions change every frame.
 *****************************************************************************/

class ParticleVertex
{
public:
    ParticleVertex() :
        _x(0.0f),
        _y(0.0f),
        _z(0.0f)
    {
    }

    //! The position of a vertex of the particle's quad.
    float _x;
    float _y;
    float _z;
};

/*!***************************************************************************
 *  \brief this is used in texture coordinate array for DrawArrays()
 *         Unless animated particles are used, this can be generated just once
 *****************************************************************************/

class ParticleTexCoord
{
public:
    ParticleTexCoord():
        _t0(0.0f),
        _t1(0.0f)
    {
    }

    //! The texture coordinates of a vertex of the particle's quad.
    float _t0;
    float _t1;
};

/*!***************************************************************************
 *  \brief this is the structure we use to represent a particle
 *****************************************************************************/

class Particle
{
public:
    Particle():
        x(0.0f),
        y(0.0f),
        size_x(0.0f),
        size_y(0.0f),
        velocity_x(0.0f),
        velocity_y(0.0f),
        combined_velocity_x(0.0f),
        combined_velocity_y(0.0f),
        rotation_angle(0.0f),
        rotation_speed(0.0f),
        time(0.0f),
        lifetime(0.0f),
        wave_length_coefficient(0.0f),
        wave_half_amplitude(0.0f),
        acceleration_x(0.0f),
        acceleration_y(0.0f),
        tangential_acceleration(0.0f),
        radial_acceleration(0.0f),
        wind_velocity_x(0.0f),
        wind_velocity_y(0.0f),
        damping(0.0f),
        rotation_direction(0.0f),
        current_size_variation_x(0.0f),
        current_size_variation_y(0.0f),
        next_size_variation_x(0.0f),
        next_size_variation_y(0.0f),
        current_rotation_speed_variation(0.0f),
        next_rotation_speed_variation(0.0f),
        current_keyframe(NULL),
        next_keyframe(NULL)
    {}

    //! position
    float x;
    float y;

    //! size
    float size_x;
    float size_y;

    //! velocity
    float velocity_x;
    float velocity_y;

    //! store the combined velocity (particle + wind + wave) so we only have
    //! to calculate it once
    float combined_velocity_x;
    float combined_velocity_y;

    //! color
    vt_video::Color color;

    //! current rotation angle
    float rotation_angle;

    //! rotation speed
    float rotation_speed;

    //! seconds since particle was spawned
    float time;

    //! lifetime (when the particle is supposed to die)
    float lifetime;

    //! this is 2 * pi / wavelength. The reason we store this weird
    //! number instead of the wavelength is because that's what we
    //! will ultimately plug into the sin function
    float wave_length_coefficient;

    //! half the amplitude of the wave. We store half the amplitude
    //! instead of the whole amplitude because that's what gets multiplied
    //! with the sin function
    float wave_half_amplitude;

    //! acceleration, i.e. change in velocity per second. The most common use
    //! for this is for simulating gravity. If you have multiple constant
    //! forces acting on particles, then this vector should be the sum of
    //! those forces.
    float acceleration_x;
    float acceleration_y;

    //! tangential acceleration- just like normal acceleration, except it
    //! is applied in the tangent direction. positive = clockwise.
    float tangential_acceleration;

    //! radial acceleration- acceleration towards (negative) or away (positive)
    //! from an attractor. Note that the default attractor is the emitter position.
    //! The client can set an attractor for the entire effect by calling
    //! ParticleEffect::SetAttractor(x,y)
    float radial_acceleration;

    //! wind velocity. this gets added to the particle's velocity each frame.
    //! note that different particles might also have a slightly different wind
    //! velocity, if the system has some wind velocity variation
    float wind_velocity_x;
    float wind_velocity_y;

    //! damping- the particle's velocity gets multiplied by this value each second.
    //! So for example, a damping of .6 means that a particle slows down by 40% each
    //! second.
    float damping;

    //! when a particle is created, it is given a rotation direction: either
    //! 1 (clockwise) or -1 (counterclockwise)
    float rotation_direction;

    //! property variations
    float current_size_variation_x;
    float current_size_variation_y;
    float next_size_variation_x;
    float next_size_variation_y;
    float current_rotation_speed_variation;
    float next_rotation_speed_variation;
    vt_video::Color current_color_variation;
    vt_video::Color next_color_variation;

    //! keep track of current and next keyframes
    ParticleKeyframe *current_keyframe;
    ParticleKeyframe *next_keyframe;
};

} // vt_mode_manager

#endif  //! __PARTICLE_HEADER__
