////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    pause.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for pause mode interface.
*** ***************************************************************************/

#include "audio.h"
#include "video.h"
#include "input.h"
#include "system.h"
#include "boot.h"
#include "pause.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_mode_manager;
using namespace hoa_input;
using namespace hoa_system;
using namespace hoa_boot;

namespace hoa_pause {

bool PAUSE_DEBUG = false;

/** \name Quit Options Menu Constants
*** These constants reprent the OptionBox selection indeces of the three different options
*** presented to the player while the _quit_state member is active.
**/
//@{
const uint8 QUIT_GAME      = 0;
const uint8 QUIT_TO_BOOT   = 1;
const uint8 QUIT_CANCEL    = 2;
//@}

PauseMode::PauseMode(bool quit_state, bool pause_audio) :
	GameMode(),
	_quit_state(quit_state),
	_audio_paused(pause_audio),
	_dim_color(0.35f, 0.35f, 0.35f, 1.0f) // A grayish opaque color
{
	mode_type = MODE_MANAGER_PAUSE_MODE;

	// Render the paused string in white text
	_paused_text.SetStyle(TextStyle("title28", Color::white, VIDEO_TEXT_SHADOW_BLACK));
	_paused_text.SetText(UTranslate("Paused"));

	// Initialize the quit options box
	_quit_options.SetPosition(512.0f, 384.0f);
	_quit_options.SetDimensions(750.0f, 50.0f, 3, 1, 3, 1);
	_quit_options.SetTextStyle(TextStyle("title24", Color::white, VIDEO_TEXT_SHADOW_BLACK));

	_quit_options.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_quit_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_quit_options.SetSelectMode(VIDEO_SELECT_SINGLE);
	_quit_options.SetCursorOffset(-58.0f, 18.0f);

	_quit_options.AddOption(UTranslate("Quit Game"));
	_quit_options.AddOption(UTranslate("Quit to Main Menu"));
	_quit_options.AddOption(UTranslate("Cancel"));
	_quit_options.SetSelection(QUIT_CANCEL);
}



PauseMode::~PauseMode() {
	if (_audio_paused == true)
		AudioManager->ResumeAudio();
}



void PauseMode::Reset() {
	if (_audio_paused == true)
		AudioManager->PauseAudio();

	// Save a copy of the current screen to use as the backdrop
	try {
		_screen_capture = VideoManager->CaptureScreen();
	}
	catch (Exception e) {
		IF_PRINT_WARNING(PAUSE_DEBUG) << e.ToString() << endl;
	}

	VideoManager->SetCoordSys(0.0f, 1023.0f, 0.0f, 767.0f);
	VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
}



void PauseMode::Update() {
	// Don't eat up 100% of the CPU time while in pause mode
	SDL_Delay(50); // Puts the process to sleep for 50ms

	if (_quit_state == false) {
		if (InputManager->QuitPress() == true) {
			_quit_state = true;
			return;
		}
		else if (InputManager->PausePress() == true) {
			ModeManager->Pop();
			return;
		}
	}

	else { // (_quit_state == true)
		_quit_options.Update();

		if (InputManager->QuitPress() == true) {
			SystemManager->ExitGame();
			return;
		}

		else if (InputManager->ConfirmPress() == true) {
			switch (_quit_options.GetSelection()) {
				case QUIT_CANCEL:
					ModeManager->Pop();
					break;
				case QUIT_TO_BOOT:
					ModeManager->PopAll();
					ModeManager->Push(new BootMode());
					break;
				case QUIT_GAME:
					SystemManager->ExitGame();
					break;
				default:
					IF_PRINT_WARNING(PAUSE_DEBUG) << "unknown quit option selected: " << _quit_options.GetSelection() << endl;
					break;
			}
			return;
		}

		else if (InputManager->CancelPress() == true) {
			ModeManager->Pop();
			return;
		}

		else if (InputManager->LeftPress() == true) {
			_quit_options.InputLeft();
		}

		else if (InputManager->RightPress() == true) {
			_quit_options.InputRight();
		}
	}
} // void PauseMode::Update()



void PauseMode::Draw() {
	// Set the coordinate system for the background and draw
	VideoManager->SetCoordSys(0.0f, _screen_capture.GetWidth(), 0.0f, _screen_capture.GetHeight());
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
	VideoManager->Move(0.0f, 0.0f);
	_screen_capture.Draw(_dim_color);

	// Re-set the coordinate system for everything else
	VideoManager->SetCoordSys(0.0f, 1023.0f, 0.0f, 767.0f);
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
	VideoManager->Move(512.0f, 384.0f);

	if (_quit_state == false) {
		_paused_text.Draw();
	}
	else {
		_quit_options.Draw();
	}
}

} // namespace hoa_pause
