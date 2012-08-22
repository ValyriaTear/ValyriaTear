///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/*!****************************************************************************
 * \file    particle_manager.h
 * \author  Raj Sharma, roos@allacrost.org
 * \brief   Header file for particle manager
 *
 * The particle manager is very simple. Every time you want to draw an effect,
 * you call AddEffect() with a pointer to the effect definition structure.
 * Then every frame, call Update() and Draw() to draw all the effects.
 *****************************************************************************/

#ifndef __PARTICLE_MANAGER_HEADER__
#define __PARTICLE_MANAGER_HEADER__

#include "defs.h"
#include "utils.h"

namespace hoa_mode_manager
{

/*!***************************************************************************
 *  \brief ParticleManager, used internally by video engine to store/update/draw
 *         all particle effects.
 *****************************************************************************/

class ParticleManager
{
public:

	/*!
	 *  \brief Constructor
	 */
	ParticleManager() {}

	~ParticleManager()
		{ _Destroy(); }

	/*!
	 *  \brief creates a new instance of an effect at (x,y), given its definition file.
	 *         The effect is added to the internal std::map, _effects, and is now
	 *         included in calls to Draw() and Update()
	 * \param filename the new effect filename to add
	 * \param x x coordinate of where to add the effect
	 * \param y y coordinate of where to add the effect
	 * \return The effect pointer
	 */
	ParticleEffect* AddParticleEffect(const std::string& filename, float x, float y);

	/*!
	 *  \brief Restart the given particle effect
	 *  \return Whether the effect successfully restarted.
	 */
	bool RestartParticleEffect(ParticleEffect *effect);

	/*!
	 *  \brief draws all active effects
	 * \return success/failure
	 */
	bool Draw();

	/*!
	 *  \brief updates all active effects
	 * \param the new time
	 * \return success/failure
	 */
	bool Update(int32 frame_time);

	/*!
	 *  \brief stops all effects
	 *
	 *  \param kill_immediate If this is true, the effects are immediately killed. If
	 *                        it isn't true, then we stop the effects from emitting
	 *                        new particles, and allow them to live until all the active
	 *                        particles fizzle out.
	 */
	void StopAll(bool kill_immediate = false);

	/*!
	 *  \brief returns the total number of particles among all active effects
	 * \return number of particles in the effect
	 */
	int32 GetNumParticles()
	{ return _num_particles; }

	/** Create a particle effect without registering it to the particle manager.
	*** It is useful managing a particle effect as a map object, for instance,
	*** as one can control the drawing order.
	*** \param filename The particle effect filename to load
	*** \return ParticleEffect The particle effect object or NULL is invalid.
	**/
	static ParticleEffect* CreateEffect(const std::string& filename);

private:
	/*!
	 *  \brief destroys the system. Called by VideoEngine's destructor
	 */
	void _Destroy();

	/*!
	 *  \brief loads an effect definition from a particle file
	 * \param filename file to load the effect from
	 * \return handle to the effect
	 */
	static ParticleEffectDef* _LoadEffect(const std::string& filename);

	/*!
	 *  \brief creates a new instance of an effect at (x,y), given its definition.
	 *         The effect is added to the internal std::map, _effects, and is now
	 *         included in calls to Draw() and Update()
	 * \param def the new effect to add
	 * \param x x coordinate of where to add the effect
	 * \param y y coordinate of where to add the effect
	 * \return The effect pointer
	 */
	ParticleEffect* _AddEffect(const ParticleEffectDef *def, float x, float y);

	/*!
	*  \brief Helper function to initialize a new ParticleEffect from its definition.
	*	      Used by AddEffect()
	* \param def definition used to create the effect
	* \return the effect created with the specified definition
	*/
	static ParticleEffect *_CreateEffect(const ParticleEffectDef *def);

	//! \brief Helper function used to read a color subtable.
	static hoa_video::Color _ReadColor(hoa_script::ReadScriptDescriptor& particle_script,
								        const std::string& param_name);

	/** \brief Shows graphical statistics useful for performance tweaking
	*** This includes, for instance, the number of texture switches made during a frame.
	**/
	void _DEBUG_ShowParticleStats();

	//! All the effects currently being managed.
	std::vector<ParticleEffect*> _all_effects;

	std::vector<ParticleEffect*> _active_effects;

	//! Total number of particles among all the active effects. This is updated
	//! during each call to Update(), so that when GetNumParticles() is called,
	//! we can just return this value instead of having to calculate it
	int32 _num_particles;
};

}  // namespace hoa_mode_manager

#endif // !__PARTICLE_MANAGER_HEADER
