///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    particle_effect.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for particle effects
***
*** Particle effects are basically nothing more than a collection of particle
*** systems. Many effects are just one system. However, for example, if you think
*** about a campfire effect, that might actually consist of fire + smoke + embers.
*** So, that's an example of an effect that consists of 3 systems.
***
*** This file contains two classes: ParticleEffectDef, and ParticleEffect.
***
*** ParticleEffectDef is a "definition" class, which holds a list of
***    ParticleSystemDefs.
***
*** ParticleEffect is an "instance" class, which holds a list of
***    ParticleSystems.
***
***
*** This way, if you have 100 explosions, the properties of the
*** effect are stored only once in a ParticleEffectDef, and the only thing that
*** gets repeated 100 times is the ParticleEffect, which holds instance-specific stuff.
*** **************************************************************************/

#ifndef __PARTICLE_EFFECT_HEADER__
#define __PARTICLE_EFFECT_HEADER__

#include "engine/video/particle_system.h"

#include "utils.h"

namespace vt_script {
class ReadScriptDescriptor;
}

namespace vt_map
{
namespace private_map
{
class ParticleObject;
}
}

namespace vt_mode_manager
{

/*!***************************************************************************
 *  \brief particle effect definition, just consists of each of its subsystems'
 *         definitions.
 *****************************************************************************/

class ParticleEffectDef
{
public:
    ParticleEffectDef():
        effect_collision_width(0.0f),
        effect_collision_height(0.0f)
    {}

    void Clear() {
        effect_collision_width = 0.0f;
        effect_collision_height = 0.0f;
        _systems.clear();
    }

    /** The effect size in pixels, used to know when to display it when it used as
    *** a map object fir instance. It is used to compute the image rectangle.
    *** \note Not used if equal to 0.
    **/
    float effect_collision_width;
    float effect_collision_height;

    //! list of system definitions
    std::vector<ParticleSystemDef> _systems;
};


/*!***************************************************************************
 *  \brief particle effect, basically one coherent "effect" like an explosion,
 *         or snow falling from the sky. Consists of one or more ParticleSystems.
 *         An example of using multiple systems to create one effect would be
 *         a campfire where you have fire + smoke + glowing embers.
 *****************************************************************************/

class ParticleEffect
{
public:
    /*!
     *  \brief Constructor
     */
    ParticleEffect() {
        _Destroy();
    }

    ParticleEffect(const std::string &effect_filename) {
        _Destroy();
        LoadEffect(effect_filename);
    }

    /** Create a particle effect without registering it to the particle manager.
    *** It is useful managing a particle effect as a map object, for instance,
    *** as one can control the drawing order.
    *** \param filename The particle effect filename to load
    *** \return whether the effect is valid.
    **/
    bool LoadEffect(const std::string &effect_filename);

    /*!
     *  \brief moves the effect to the specified position on the screen,
     *         This can be used if you want to move a particle system around
     *         on some flight path, or if you want to attach the system to an
     *         object. (e.g. smoke to a jet)
     * \param x movement of system in x direction
     * \param y movement of system in y direction
     */
    void Move(float x, float y);

    /*!
     *  \brief moves the effect dx and dy units relative to its current position
     * \param dx x offset to move to from current x position
     * \param dy y offset to move to from current y position
     */
    void MoveRelative(float dx, float dy);

    /*!
     *  \brief set the orientation of the effect (including all systems contained
     *         within the effect). This is essentially added to the emitter orientation
     *         for each system. For example, if you want to create a particle system for
     *         smoke coming out of a jet, set the emitter orientation to zero degrees (right)
     *         when you create the effect. Then, at runtime just call SetOrientation() every
     *         frame with the angle the jet is facing.
     * \param angle rotation of particle system
     */
    void SetOrientation(float angle) {
        _orientation = angle;
    }

    /*!
     *  \brief set the position of an "attractor point". Any particle systems which use
     *         radial acceleration and have user-defined attractor points enabled will
     *         have particles move towards this point
     *
     *  \note  a positive radial acceleration will move a particle away from the attractor,
     *         and negative will move it towards it.
     * \param x x coordiante of gravitation point
     * \param y y coordiante of gravitation point
     */
    void SetAttractorPoint(float x, float y);

    /*!
     *  \brief returns true if the system is alive, i.e. the number of active
     *         particles is more than zero. This is used by the particle manager
     *         so it knows when to destroy an effect when registered to it.
     * \return true if system is alive, false if dead
     */
    bool IsAlive() const {
        return _alive;
    }

    /*!
     *  \brief stops this effect
     *
     *  \param kill_immediate If this is true, the effect is immediately killed. If
     *                        it isn't true, then we stop the effect from emitting
     *                        new particles, and allow it to live until all the active
     *                        particles fizzle out.
     *  \note When registered in the particle Manager, the effect will be destroyed
     *        at its end of life.
     */
    void Stop(bool kill_immediate = false);

    /*!
     *  \brief starts this effect
     *  \return whether the effect was started.
     */
    bool Start();

    /*!
     *  \brief return the number of active particles in this effect
     * \return number of particles in the system
     */
    int32 GetNumParticles() const {
        return _num_particles;
    }

    /*!
     *  \brief return the position of the effect into x and y
     * \param x parameter to store x value of system in
     * \param y parameter to store y value of system in
     */
    void GetPosition(float &x, float &y) const;

    /*!
     *  \brief return the age of the system, i.e. how many seconds it has been since
     *         it was created
     * \return age of the system
     */
    float GetAge() const {
        return _age;
    }

    //! \brief Get the overall effect width/height in pixels.
    float GetEffectWidth() const {
        return _effect_def.effect_collision_width;
    }
    float GetEffectHeight() const {
        return _effect_def.effect_collision_height;
    }

    bool IsLoaded() const {
        return _loaded;
    }

    /*!
     * \brief draws the effect.
     * \return success/failure
     */
    bool Draw();

    /*!
     * \brief updates the effect.
     * \param the new frame time
     * \return success/failure
     */
    bool Update(float frame_time);
    bool Update();
private:
    /*!
     * \brief destroys the effect. This is private so that only the ParticleManager class
     *         can destroy effects.
     */
    void _Destroy();

    /*!
     * \brief loads an effect definition from a particle file
     * \param filename file to load the effect from
     * \return Whether the effect def is valid
     */
    bool _LoadEffectDef(const std::string &filename);

    /** Creates the effect based on the particle effect definition.
    *** _LoadEffectDef() must be called before this one.
    **/
    bool _CreateEffect();

    //! \brief Helper function used to read a color subtable.
    vt_video::Color _ReadColor(vt_script::ReadScriptDescriptor &particle_script,
                                const std::string &param_name);

    //! The effect definition
    ParticleEffectDef _effect_def;

    //! list of subsystems that make up the effect. (for example, a fire effect might consist
    //! of a flame + smoke + embers)
    std::vector<ParticleSystem> _systems;

    //! Tells whether the effect definition and systems arewere successfully loaded
    bool _loaded;

    //! position of the effect
    float _x, _y;

    //! position of attractor point
    float _attractor_x, _attractor_y;

    //! orientation of the effect (angle in radians)
    float _orientation;

    //! is the effect is alive or not
    bool  _alive;

    //! age of the effect (seconds since it was created)
    float _age;

    //! number of active particles (this is updated on each call to Update())
    int32 _num_particles;
}; // class ParticleEffect

}  // namespace vt_mode_manager

#endif  //! __PARTICLE_EFFECT_HEADER__
