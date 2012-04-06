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

//! \brief A particle effet ID is an int
typedef int32 ParticleEffectID;

//! \brief -1 represents an invalid effect
const ParticleEffectID VIDEO_INVALID_EFFECT = -1;

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
	 * \param def the new effect to add
	 * \param x x coordinate of where to add the effect
	 * \param y y coordinate of where to add the effect
	 * \return ID corresponding to the effect
	 */
	ParticleEffectID AddParticleEffect(const std::string &filename, float x, float y);

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
	int32 GetNumParticles();

private:
	/*!
	 *  \brief destroys the system. Called by VideoEngine's destructor
	 */
	void _Destroy();

	/*!
	 *  \brief Converts a particle effect id into a ParticleEffect pointer.
	 *         The pointers that this function returns are valid only up until
	 *         the next call to Update(), so they should never be stored. Just use
	 *         them for the current frame and then throw them away.
	 * \param id ID of the effect to get
	 * \return the desired effect
	 */
	ParticleEffect *_GetEffect(ParticleEffectID id);

	/*!
	 *  \brief loads an effect definition from a particle file
	 * \param filename file to load the effect from
	 * \return handle to the effect
	 */
	ParticleEffectDef *_LoadEffect(const std::string &filename);

	/*!
	 *  \brief creates a new instance of an effect at (x,y), given its definition.
	 *         The effect is added to the internal std::map, _effects, and is now
	 *         included in calls to Draw() and Update()
	 * \param def the new effect to add
	 * \param x x coordinate of where to add the effect
	 * \param y y coordinate of where to add the effect
	 * \return ID corresponding to the effect
	 */
	ParticleEffectID _AddEffect(const ParticleEffectDef *def, float x, float y);

	/*!
	*  \brief Helper function to initialize a new ParticleEffect from its definition.
	*	      Used by AddEffect()
	* \param def definition used to create the effect
	* \return the effect created with the specified definition
	*/
	ParticleEffect *_CreateEffect(const ParticleEffectDef *def);

	//! \brief Helper function used to read a color subtable.
	hoa_video::Color _ReadColor(hoa_script::ReadScriptDescriptor& particle_script,
								std::string param_name);

	/** \brief Shows graphical statistics useful for performance tweaking
	*** This includes, for instance, the number of texture switches made during a frame.
	**/
	void _DEBUG_ShowParticleStats();

	//! All the effects currently being managed.
	std::vector<ParticleEffect *> _effects;

	//! Total number of particles among all the active effects. This is updated
	//! during each call to Update(), so that when GetNumParticles() is called,
	//! we can just return this value instead of having to calculate it
	int32 _num_particles;
};

}  // namespace hoa_mode_manager

#endif // !__PARTICLE_MANAGER_HEADER
