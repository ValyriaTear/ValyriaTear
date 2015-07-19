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
*** \file    particle_system.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for particle system
***
*** This file contains two classes: ParticleSystemDef, and ParticleSystem.
***
*** ParticleSystemDef is a "definition" class, meaning that it holds information
*** about a particle system, like its lifetime, the emitter, and other properties.
***
*** ParticleSystem is an "instance" class, meaning that it holds information about
*** a particle system which is currently being drawn on screen.
***
*** This way, if you have 100 explosions for example, the properties of the
*** effect are stored only once, and the only thing that gets repeated 100 times
*** is instance-specific stuff like positions of vertices, etc.
*** **************************************************************************/

#ifndef __PARTICLE_SYSTEM_HEADER__
#define __PARTICLE_SYSTEM_HEADER__

#include "particle.h"
#include "particle_emitter.h"

#include "engine/video/image.h"

namespace vt_mode_manager
{

//! \brief Specifies the stencil operation to use and describes how the stencil buffer is modified
enum VIDEO_STENCIL_OP {
    VIDEO_STENCIL_OP_INVALID = -1,

    //! Set the stencil value to one
    VIDEO_STENCIL_OP_ZERO = 0,

    //! Set the stencil value to zero
    VIDEO_STENCIL_OP_ONE = 1,

    //! Increase the stencil value
    VIDEO_STENCIL_OP_INCREASE = 2,

    //! Decrease the stencil value
    VIDEO_STENCIL_OP_DECREASE = 3,

    VIDEO_STENCIL_OP_TOTAL = 4
};

/*!***************************************************************************
 *  \brief when we change a property of an effect, it affects all of the
 *         systems contained within that effect. So, this structure contains
 *         any relevant parameters that particle systems need to know about.
 *****************************************************************************/

class EffectParameters
{
public:
    EffectParameters():
        orientation(0.0f),
        attractor_x(0.0f),
        attractor_y(0.0f)
    {}

    //! orientation of the effect, called with ParticleEffect::SetOrientation()
    float orientation;

    //! attraction point, particles gravitate towards this
    float attractor_x;
    float attractor_y;
};


class ParticleSystemDef
{
public:
    ParticleSystemDef():
        enabled(false),
        blend_mode(0),
        system_lifetime(0.0f),
        particle_lifetime(0.0f),
        particle_lifetime_variation(0.0f),
        max_particles(0),
        damping(0.0f),
        damping_variation(0.0f),
        acceleration_x(0.0f),
        acceleration_y(0.0f),
        acceleration_variation_x(0.0f),
        acceleration_variation_y(0.0f),
        wind_velocity_x(0.0f),
        wind_velocity_y(0.0f),
        wind_velocity_variation_x(0.0f),
        wind_velocity_variation_y(0.0f),
        wave_motion_used(false),
        wave_length(0.0f),
        wave_length_variation(0.0f),
        wave_amplitude(0.0f),
        wave_amplitude_variation(0.0f),
        tangential_acceleration(0.0f),
        tangential_acceleration_variation(0.0f),
        radial_acceleration(0.0f),
        radial_acceleration_variation(0.0f),
        user_defined_attractor(false),
        attractor_falloff(0.0f),
        rotation_used(false),
        rotate_to_velocity(false),
        speed_scale_used(false),
        speed_scale(0.0f),
        min_speed_scale(0.0f),
        max_speed_scale(0.0f),
        smooth_animation(false),
        modify_stencil(false),
        stencil_op(VIDEO_STENCIL_OP_INVALID),
        use_stencil(false),
        random_initial_angle(false)
    {}

    ~ParticleSystemDef()
    {}

    //! Is this system supposed to be displayed
    bool enabled;

    //! Each system contains 1 emitter, which mainly determines where to shoot particles out from
    //! and how fast to shoot them out
    ParticleEmitter emitter;

    //! Array of keyframes, which specify how particle properties vary over time. This array must
    //! contain at least 1 keyframe (in that case, the properties are all held constant)
    std::vector<ParticleKeyframe> keyframes;

    //! How to blend the particles: VIDEO_NO_BLEND, VIDEO_BLEND, or VIDEO_BLEND_ADD
    //! For most effects, we want VIDEO_BLEND_ADD
    int32 blend_mode;

    //! How many seconds the system should live for before it dies out. This is only
    //! meaningful if our emitter mode is EMITTER_MODE_ONE_SHOT. The other modes
    //! will simply keep playing until the effect is destroyed or stopped, except for
    //! EMITTER_MODE_BURST, which spits out a bunch of particles at the beginning and
    //! then dies as soon as all of those particles die
    float system_lifetime;

    //! How long each particle should live for before it dies
    float particle_lifetime;

    //! Random variation added to particle lifetime
    float particle_lifetime_variation;

    //! Maximum number of particles this system can have at one time
    int32 max_particles;

    //! A number below 1.0 (but generally pretty close to 1.0). A damp of .99 means that
    //! each second, particle velocity drops by 1%
    float damping;

    //! Random variation added to damping
    float damping_variation;

    //! constant acceleration, good example is gravity. Note that down is in the positive y
    //! direction since we are using screen coordinates
    float acceleration_x;
    float acceleration_y;

    float acceleration_variation_x;
    float acceleration_variation_y;

    //! wind velocity, more generally any velocity which is added to each particle's velocity
    float wind_velocity_x;
    float wind_velocity_y;

    //! wind velocity variation
    float wind_velocity_variation_x;
    float wind_velocity_variation_y;

    //! true if we should use wave motion for this system
    bool wave_motion_used;

    //! wavelength. For example a wavelength of 5 means that it takes 5 seconds to go from
    //! one point on the sinusoidal curve to the next
    float wave_length;

    //! Random variation added to wave length
    float wave_length_variation;

    //! wave amplitude- the distance from the peak to the bottom of the sinusoidal curve
    float wave_amplitude;

    //! Random variation added to wave amplitude
    float wave_amplitude_variation;

    //! tangential acceleration, how particle accelerates tangential to vector from particle
    //! to center of emitter region. Positive is clockwise.
    float tangential_acceleration;

    //! tangential acceleration variation
    float tangential_acceleration_variation;

    //! radial acceleration. Positive means particles accelerate away from the emitter, negative
    //! means they accelerate back towards it
    float radial_acceleration;

    //! radial acceleration variation
    float radial_acceleration_variation;

    //! if true, we use a user defined attractor instead of the emitter position for radial
    //! acceleration. The user defined attractor is set using ParticleEffect::SetAttractorPoint()
    bool user_defined_attractor;

    //! how quickly the "pull" of an attractor falls off as a particle gets further away from it.
    //! For example if falloff is 10^-3, and a particle is 500 pixels away from the attractor,
    //! then the radial acceleration is lessened by (500 * 10^-3) = .5, or 50%
    float attractor_falloff;

    //! True if ANY of the keyframes for the particles contain non-zero rotations
    //! This is used by the Draw() function so it knows whether it needs to factor in
    //! rotations when determining the particle vertices (since this adds a lot of extra
    //! computation)
    bool rotation_used;

    //! True if you want particles to rotate to face the same direction they are going.
    //! So for example a particle going straight up will not be rotated, but a particle
    //! that is going 45 degrees northeast will be rotated 45 degrees clockwise
    bool rotate_to_velocity;

    //! true if speed scaling is used. Note that speed scaling can only be used if
    //! rotate to velocity is also used.
    bool speed_scale_used;

    //! this number is multiplied by a particle's speed to come up with a scale of how much
    //! to stretch a particle in its direction of motion
    float speed_scale;

    //! if you use speed_scale, this can cause particles to become extremely tiny at low speeds
    //! this variable allows you to set a minimum bound on the scaling due to speed
    float min_speed_scale;

    //! if you use speed_scale, this can cause particles to become extremely large at high speeds
    //! this variable allows you to set a maximum bound on the scaling due to speed
    float max_speed_scale;

    //! True if alpha blending should be used to create smooth transitions between animation
    //! frames
    bool smooth_animation;

    //! True if this system should not actually write to the screen, but instead modify the
    //! stencil buffer. Every time a pixel passes the alpha test, the stencil buffer will be
    //! modified, according to _stencil_op
    bool modify_stencil;

    //! if modify stencil is true, then the operation to use when the alpha test passes
    //! can either be VIDEO_STENCIL_OP_INCREASE, VIDEO_STENCIL_OP_ONE, VIDEO_STENCIL_OP_ZERO, or
    //! VIDEO_STENCIL_OP_DECREASE. The stencil test we use is "equal to 1"
    VIDEO_STENCIL_OP stencil_op;

    //! if this is true, then we only draw in areas where the stencil buffer contains a 1.
    //! Note that _use_stencil and _modify_stencil cannot both be 1 at the same time
    bool use_stencil;

    //! true if particles' initial angle should be randomized. If false, then all particles
    //! have an angle of zero when they spawn
    bool random_initial_angle;

    //! Array telling how long each animation should last for
    std::vector<int32> animation_frame_times;

    //! Array of filenames for each frame of animation
    std::vector<std::string> animation_frame_filenames;

}; // class ParticleSystemDef



class ParticleSystem
{
public:
    /*!
     * \brief Constructor
     */
    ParticleSystem(ParticleSystemDef *sys_def) {
        _Destroy();
        _Create(sys_def);
    }

    ~ParticleSystem() {
        _Destroy();
    }

    //! \brief draws the system
    void Draw();

    /*!
     * \brief updates the system
     * \param frame_time the current frame time
     * \param params the effect parameters to use for this update (orientation and attractor point)
     */
    void Update(float frame_time, const EffectParameters &params);

    /*!
     * \brief returns true if system is still alive
     * \return true if alive, false if dead
     */
    bool IsAlive() const {
        return _alive && _system_def->enabled;
    }

    /*!
     * \brief returns true if system has been stopped by a call to Stop()
     * \return true if stopped, false if still going
     */
    bool IsStopped() const {
        return _stopped;
    }

    /*!
     *  \brief stops the system, i.e. makes it stop emitting new particles
     */
    void Stop() {
        _stopped = true;
    }

    /*!
     *  \brief returns how many particles are alive in this system
     * \return the number of particles in this system
     */
    int32 GetNumParticles() const {
        return _num_particles;
    }

    /*!
     *  \brief returns the number of seconds since this system was created
     * \return the age of the system
     */
    float GetAge() const {
        return _age;
    }

private:
    /*!
     *  \brief initializes this particle system as an instance of the
     *         type of particle system specified by the ParticleSystemDef
     * \param sys_def particle definition to base the system off of
     * \return success/failure
     */
    bool _Create(ParticleSystemDef *sys_def);

    /*!
     *  \brief destroys the system
     */
    void _Destroy();

    /*!
     *  \brief helper function to update properties of particles
     * \param t the current frame time
     * \param params the effect parameters to use for this update (orientation and attractor point)
     */
    void _UpdateParticles(float t, const EffectParameters &params);

    /*!
     *  \brief helper function to kill off any particles that have died
     *
     *  \param num_particles this is an optimization that lets us avoid
     *         killing particles. Killing particles is expensive, because
     *         it leaves holes in our array, so we have to shuffle data
     *         around to fill those holes. So instead, say we have 10
     *         particles to kill this frame, and we also want to emit 8
     *         particles. Then, instead of killing 10 particles, we only
     *         kill 2, and for the other 8, we respawn them immediately.
     * \param params the effect parameters to use for this update (orientation and attractor point)
     */
    void _KillParticles(int32 &num_particles, const EffectParameters &params);

    /*!
     *  \brief helper function that emits whatever particles still need to be
     *         emitted after calling _KillParticles
     * \param num_particles the number of particles that need to be emitted
     * \param params the effect parameters to use for this update (orientation and attractor point)
     */
    void _EmitParticles(int32 num_particles, const EffectParameters &params);

    /*!
     *  \brief helper function to move a particle from element src to element dest
     *         in the array. This is required any time we kill a particle, because
     *         killing particles leaves a hole in the array
     * \param src where to move the particle from
     * \param dest where to move the particle to
     */
    void _MoveParticle(int32 src, int32 dest);

    /*!
     *  \brief creates a new particle at element i in the particle array
     * \param i index of the particle to respawn
     * \param params the effect parameters to use for this update (orientation and attractor point)
     */
    void _RespawnParticle(int32 i, const EffectParameters &params);

    //! The system definition, contains information like the emitter properties, lifetime of
    //! particles, particle keyframes, etc. Basically everything which isn't instance-specific
    //! Note that this pointer shouldn't be deleted by the particle system, since it's handled by
    //! the corresponding ParticleEffectDef instance.
    ParticleSystemDef *_system_def;

    //! Animation for each particle. If it's non-animated, it just has 1 frame
    vt_video::AnimatedImage _animation;

    //! Number of active particles in this system. (The size of the vectors may be larger, since
    //! we might set a particle quota for the system which is higher than what's actually there.)
    int32 _num_particles;

    //! The array of particle vertices. Note that this array contains FOUR vertices per particle.
    //! This is used for rendering the particles with OpenGL
    std::vector<ParticleVertex> _particle_vertices;
    std::vector<vt_video::Color> _particle_colors;
    std::vector<ParticleTexCoord> _particle_texcoords;

    //! This array holds everything except positions and colors. The reason we keep positions and
    //! colors separate is so that they can be efficiently fed to OpenGL for rendering.
    std::vector<Particle> _particles;

    //! if stopped is true, no new particles should be emitted
    bool _stopped;

    //! alive gets set to false when the number of active particles drops to zero
    bool _alive;

    //! age of the system, since it was created
    float _age;

    //! last time the system was updated (based on the system's age)
    float _last_update_time;

}; // class ParticleSystem

}  // namespace vt_mode_manager

#endif
