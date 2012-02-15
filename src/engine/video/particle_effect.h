///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/*!****************************************************************************
 * \file    particle_effect.h
 * \author  Raj Sharma, roos@allacrost.org
 * \brief   Header file for particle effects
 *
 * Particle effects are basically nothing more than a collection of particle
 * systems. Many effects are just one system. However, for example, if you think
 * about a campfire effect, that might actually consist of fire + smoke + embers.
 * So, that's an example of an effect that consists of 3 systems.
 *
 * This file contains two classes: ParticleEffectDef, and ParticleEffect.
 *
 * ParticleEffectDef is a "definition" class, which holds a list of
 *    ParticleSystemDefs.
 *
 * ParticleEffect is an "instance" class, which holds a list of
 *    ParticleSystems.
 *
 *
 * This way, if you have 100 explosions, the properties of the
 * effect are stored only once in a ParticleEffectDef, and the only thing that 
 * gets repeated 100 times is the ParticleEffect, which holds instance-specific stuff.
 *****************************************************************************/

#ifndef __PARTICLE_EFFECT_HEADER__
#define __PARTICLE_EFFECT_HEADER__

#include "defs.h"
#include "utils.h"

namespace hoa_video
{

using private_video::ParticleSystem;
using private_video::ParticleSystemDef;

/*!***************************************************************************
 *  \brief particle effect definition, just consists of each of its subsystems'
 *         definitions. This is basically just a struct, except it has a
 *         function to load the structure from a particle file (.lua/.hoa)
 *****************************************************************************/

class ParticleEffectDef
{
public:
	
	//! list of system definitions
	std::list<ParticleSystemDef *> _systems;
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
	ParticleEffect();
	

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
	void SetOrientation(float angle);
	
	
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
	 *         so it knows when to destroy an effect.
	 * \return true if system is alive, false if dead
	 */			
	bool IsAlive();
	
	
	/*!
	 *  \brief stops this effect
	 *
	 *  \param kill_immediate If this is true, the effect is immediately killed. If
	 *                        it isn't true, then we stop the effect from emitting
	 *                        new particles, and allow it to live until all the active
	 *                        particles fizzle out.
	 */		
	void Stop(bool kill_immediate = false);


	/*!
	 *  \brief return the number of active particles in this effect
	 * \return number of particles in the system
	 */				
	int32 GetNumParticles() const;
	

	/*!
	 *  \brief return the position of the effect into x and y
	 * \param x parameter to store x value of system in
	 * \param y parameter to store y value of system in
	 */				
	void  GetPosition(float &x, float &y) const;


	/*!
	 *  \brief return the age of the system, i.e. how many seconds it has been since
	 *         it was created
	 * \return age of the system
	 */			
	float GetAge() const;

private:

	/*!
	 *  \brief draws the effect. This is private so that only the ParticleManager class
	 *         can draw effects.
	 * \return success/failure
	 */			
	bool _Draw();


	/*!
	 *  \brief updates the effect. This is private so that only the ParticleManager class
	 *         can update effects.
	 * \param the new frame time
	 * \return success/failure
	 */			
	bool _Update(float frame_time);


	/*!
	 *  \brief destroys the effect. This is private so that only the ParticleManager class
	 *         can destroy effects.
	 */			
	void _Destroy();
	
	
	//! pointer to the effect definition
	const ParticleEffectDef *_effect_def;	
	
	//! list of subsystems that make up the effect. (for example, a fire effect might consist
	//! of a flame + smoke + embers)
	std::list <ParticleSystem *> _systems;
	
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
	
	friend class private_video::ParticleManager;
	
}; // class ParticleEffect

}  // namespace hoa_video

#endif  //! __PARTICLE_EFFECT_HEADER__
