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
 * \file    scene.cpp
 * \author  Tyler Olsen, roots@allacrost.org
 * \brief   Source file for scene mode interface.
 *****************************************************************************/

#include <iostream>

#include "engine/audio/audio.h"
#include "engine/video/video.h"
#include "engine/input.h"
#include "engine/system.h"

#include "scene.h"

using namespace hoa_mode_manager;
using namespace hoa_input;
using namespace hoa_system;
using namespace hoa_video;
using namespace hoa_pause;
using namespace hoa_scene::private_scene;

namespace hoa_scene
{

bool SCENE_DEBUG = false;

SceneMode::SceneMode()
{
    IF_PRINT_WARNING(SCENE_DEBUG) << "SCENE: SceneMode constructor invoked" << std::endl;
    mode_type = MODE_MANAGER_SCENE_MODE;


    // setup the scene Image Descriptor

    // VideoManager->LoadImage(scene);
}



// The destructor frees up our scene image
SceneMode::~SceneMode()
{
    IF_PRINT_WARNING(SCENE_DEBUG) << "SCENE: SceneMode destructor invoked" << std::endl;
    // VideoManager->FreeImage(scene);
}


// Resets class members appropriately
void SceneMode::Reset()
{
    _scene_timer = 0;
}



// Restores volume or unpauses audio, then pops itself from the game stack
void SceneMode::Update()
{
    uint32 time_elapsed = SystemManager->GetUpdateTime();
    _scene_timer += time_elapsed;

    // User must wait 0.75 seconds before they can exit the scene
    if((InputManager->ConfirmPress() || InputManager->CancelPress()) && _scene_timer < MIN_SCENE_UPDATES) {
        ModeManager->Pop();
    }
}



// Draws the scene
void SceneMode::Draw()
{
// 	Draw the scene, maybe with a filter that lets it fade in and out....?
}

} // namespace hoa_scene
