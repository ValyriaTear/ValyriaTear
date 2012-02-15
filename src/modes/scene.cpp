///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
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

#include "audio.h"
#include "video.h"
#include "input.h"
#include "system.h"

#include "scene.h"

using namespace std;
using namespace hoa_mode_manager;
using namespace hoa_input;
using namespace hoa_system;
using namespace hoa_video;
using namespace hoa_pause;
using namespace hoa_scene::private_scene;

namespace hoa_scene {

bool SCENE_DEBUG = false;

SceneMode::SceneMode() {
	if (SCENE_DEBUG) cout << "SCENE: SceneMode constructor invoked" << endl;
	mode_type = MODE_MANAGER_SCENE_MODE;
	

	// setup the scene Image Descriptor

	// VideoManager->LoadImage(scene);
}



// The destructor frees up our scene image
SceneMode::~SceneMode() {
	if (SCENE_DEBUG) cout << "SCENE: SceneMode destructor invoked" << endl;
  // VideoManager->FreeImage(scene);
}


// Resets class members appropriately
void SceneMode::Reset() {
	_scene_timer = 0;
}



// Restores volume or unpauses audio, then pops itself from the game stack
void SceneMode::Update() {
	uint32 time_elapsed = SystemManager->GetUpdateTime();
	_scene_timer += time_elapsed;

	// User must wait 0.75 seconds before they can exit the scene
	if ((InputManager->ConfirmPress() || InputManager->CancelPress()) && _scene_timer < MIN_SCENE_UPDATES) {
		ModeManager->Pop();
	}
}



// Draws the scene
void SceneMode::Draw() {
// 	Draw the scene, maybe with a filter that lets it fade in and out....?
}

} // namespace hoa_scene
