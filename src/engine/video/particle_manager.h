///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
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

#include "utils.h"

namespace vt_mode_manager
{

class ParticleEffect;

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

    ~ParticleManager() {
        _Destroy();
    }

    /*!
     *  \brief Takes control of the effect and moves it at (x,y).
     *         The effect is added to the internal std::map, _effects, and is now
     *         included in calls to Draw() and Update()
     * \param effect the particle effect to register to the particle manager
     * \param x x coordinate of where to add the effect
     * \param y y coordinate of where to add the effect
     * \return whether the effect was added
     */
    bool AddParticleEffect(const std::string &effect_filename, float x, float y);

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
     *  \brief stops all registered effects
     *
     *  \param kill_immediate If this is true, the effects are immediately killed. If
     *                        it isn't true, then we stop the effects from emitting
     *                        new particles, and allow them to live until all the active
     *                        particles fizzle out.
     */
    void StopAll(bool kill_immediate = false);

    /*!
     *  \brief returns the total number of particles among all active registered effects
     * \return number of particles in the effect
     */
    int32 GetNumParticles() {
        return _num_particles;
    }

private:
    /*!
     *  \brief destroys the system. Called by VideoEngine's destructor
     */
    void _Destroy();

    /** \brief Shows graphical statistics useful for performance tweaking
    *** This includes, for instance, the number of texture switches made during a frame.
    **/
    void _DEBUG_ShowParticleStats();

    //! All the effects currently being managed.
    std::vector<ParticleEffect *> _all_effects;

    std::vector<ParticleEffect *> _active_effects;

    //! Total number of particles among all the active effects. This is updated
    //! during each call to Update(), so that when GetNumParticles() is called,
    //! we can just return this value instead of having to calculate it
    int32 _num_particles;
};

}  // namespace vt_mode_manager

#endif // !__PARTICLE_MANAGER_HEADER
