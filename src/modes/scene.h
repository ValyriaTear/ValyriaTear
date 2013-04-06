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
 * \file    scene.h
 * \author  Tyler Olsen, roots@allacrost.org
 * \brief   Header file for scene mode interface.
 *
 * This code handles the game event processing and frame drawing when the user
 * is in scene mode (when a full-screen art piece is drawn to the screen).
 * The user must wait a minimum amount of time, defined by the MIN_SCENE_UPDATES
 * constant, before the scene can be passed. This is to keep the user from
 * accidentally skipping over one of our beautiful artworks before they have
 * the chance to gawk at it in amazement. :)
 *****************************************************************************/

#ifndef __SCENE_HEADER__
#define __SCENE_HEADER__

#include "utils.h"

#include "engine/mode_manager.h"

#include <string>

//! All calls to scene mode are wrapped in this namespace.
namespace vt_scene
{

//! Determines whether the code in the vt_scene namespace should print debug statements or not.
extern bool SCENE_DEBUG;

//! An internal namespace to be used only within the scene code. Don't use this namespace anywhere else!
namespace private_scene
{

//! How many milliseconds must pass before the user can exit the scene
const uint32 MIN_SCENE_UPDATES = 750;

} // namespace private_scene

/*!****************************************************************************
 * \brief Handles everything that needs to be done when full-screen artwork is displayed.
 *
 * This game mode displays a single full-screen art scene, which are used in
 * various places in the game. The scene can not be exited until the amount
 * of milliseconds defined in MIN_SCENE_UPDATES has expired, to ensure that
 * the user does not accidentally skip the scene and can take the time to
 * appreciate the art.
 *****************************************************************************/
class SceneMode : public vt_mode_manager::GameMode
{
private:
    //! Retains the number of milliseconds that have elapsed since this mode was initialized
    uint32 _scene_timer;

    //vt_video::StillImage scene;
public:
    SceneMode();
    ~SceneMode();

    //! Resets appropriate class members. Called whenever SceneMode is made the active game mode.
    void Reset();
    //! Updates the game state by the amount of time that has elapsed
    void Update();
    //! Draws the next frame to be displayed on the screen
    void Draw();
}; // class SceneMode

} // namespace vt_scene

#endif
