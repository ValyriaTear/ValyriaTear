///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file   input.cpp
*** \author Tyler Olsen, roots@allacrost.org
*** \brief  Source file for processing user input
*** **************************************************************************/

#include "input.h"
#include "video.h"
#include "script.h"

#include "mode_manager.h"
#include "system.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_video;
using namespace hoa_script;
using namespace hoa_mode_manager;
using namespace hoa_system;
using namespace hoa_input::private_input;

template<> hoa_input::InputEngine* Singleton<hoa_input::InputEngine>::_singleton_reference = NULL;

namespace hoa_input {

InputEngine* InputManager = NULL;
bool INPUT_DEBUG = false;

// Initializes class members
InputEngine::InputEngine() {
	if (INPUT_DEBUG) cout << "INPUT: InputEngine constructor invoked" << endl;
	_any_key_press		    = false;
	_any_key_release	    = false;
	_last_axis_moved      = -1;
	_up_state             = false;
	_up_press             = false;
	_up_release           = false;
	_down_state           = false;
	_down_press           = false;
	_down_release         = false;
	_left_state           = false;
	_left_press           = false;
	_left_release         = false;
	_right_state          = false;
	_right_press          = false;
	_right_release        = false;
	_confirm_state        = false;
	_confirm_press        = false;
	_confirm_release      = false;
	_cancel_state         = false;
	_cancel_press         = false;
	_cancel_release       = false;
	_menu_state           = false;
	_menu_press           = false;
	_menu_release         = false;
	_swap_state           = false;
	_swap_press           = false;
	_swap_release         = false;
	_right_select_state   = false;
	_right_select_press   = false;
	_right_select_release = false;
	_left_select_state    = false;
	_left_select_press    = false;
	_left_select_release  = false;

	_pause_press          = false;
	_quit_press           = false;

	_joyaxis_x_first      = true;
	_joyaxis_y_first      = true;
	_joystick.js          = NULL;
	_joystick.x_axis      = 0;
	_joystick.y_axis      = 1;
	_joystick.threshold   = 8192;
}



InputEngine::~InputEngine() {
	if (INPUT_DEBUG) cout << "INPUT: InputEngine destructor invoked" << endl;

	// If a joystick is open, close it before exiting
	if (_joystick.js != NULL) {
		SDL_JoystickClose(_joystick.js);
	}
}


// Initialize singleton pointers and key/joystick systems.
bool InputEngine::SingletonInitialize() {
	// Initialize the SDL joystick subsystem
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0) {
		cerr << "INPUT ERROR: failed to initailize the SDL joystick subsystem" << endl;
		return false;
	}

	return true;
}


// This is no longer inside SingletonInitialize because we need to load the lua settings
// before initializing the joysticks.
void InputEngine::InitializeJoysticks()
{
	// Attempt to initialize and setup the joystick system
	if (SDL_NumJoysticks() == 0) { // No joysticks found
		SDL_JoystickEventState(SDL_IGNORE);
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	}
	else { // At least one joystick exists
		SDL_JoystickEventState(SDL_ENABLE);
		// TODO: need to allow user to specify which joystick to open, if multiple exist
		_joystick.js = SDL_JoystickOpen(_joystick.joy_index);
	}
}


// Loads the default key settings from the lua file and sets them back
bool InputEngine::RestoreDefaultKeys() {
	// Load the settings file
	string in_filename = GetSettingsFilename();
	ReadScriptDescriptor settings_file;
	if (!settings_file.OpenFile(in_filename)) {
		cerr << "INPUT ERROR: failed to open data file for reading: " << in_filename << endl;
		return false;
	}

	// Load all default keys from the table
	settings_file.OpenTable("settings");
	settings_file.OpenTable("key_defaults");
	_key.up           = static_cast<SDLKey>(settings_file.ReadInt("up"));
	_key.down         = static_cast<SDLKey>(settings_file.ReadInt("down"));
	_key.left         = static_cast<SDLKey>(settings_file.ReadInt("left"));
	_key.right        = static_cast<SDLKey>(settings_file.ReadInt("right"));
	_key.confirm      = static_cast<SDLKey>(settings_file.ReadInt("confirm"));
	_key.cancel       = static_cast<SDLKey>(settings_file.ReadInt("cancel"));
	_key.menu         = static_cast<SDLKey>(settings_file.ReadInt("menu"));
	_key.swap         = static_cast<SDLKey>(settings_file.ReadInt("swap"));
	_key.left_select  = static_cast<SDLKey>(settings_file.ReadInt("left_select"));
	_key.right_select = static_cast<SDLKey>(settings_file.ReadInt("right_select"));
	_key.pause        = static_cast<SDLKey>(settings_file.ReadInt("pause"));
	settings_file.CloseTable();
	settings_file.CloseTable();

	settings_file.CloseFile();

	return true;
}


// Loads the default joystick settings from the lua file and sets them back
bool InputEngine::RestoreDefaultJoyButtons()
{
	// Load the settings file
	string in_filename = GetSettingsFilename();
	ReadScriptDescriptor settings_file;
	if (settings_file.OpenFile(in_filename) == false) {
		cerr << "INPUT ERROR: failed to open data file for reading: " << in_filename << endl;
		return false;
	}

	// Load all default buttons from the table
	settings_file.OpenTable("settings");
	settings_file.OpenTable("joystick_defaults");
	_joystick.confirm      = static_cast<uint8>(settings_file.ReadInt("confirm"));
	_joystick.cancel       = static_cast<uint8>(settings_file.ReadInt("cancel"));
	_joystick.menu         = static_cast<uint8>(settings_file.ReadInt("menu"));
	_joystick.swap         = static_cast<uint8>(settings_file.ReadInt("swap"));
	_joystick.left_select  = static_cast<uint8>(settings_file.ReadInt("left_select"));
	_joystick.right_select = static_cast<uint8>(settings_file.ReadInt("right_select"));
	_joystick.pause        = static_cast<uint8>(settings_file.ReadInt("pause"));
	_joystick.quit		   = static_cast<uint8>(settings_file.ReadInt("quit"));
	settings_file.CloseTable();
	settings_file.CloseTable();

	settings_file.CloseFile();

	return true;
}


// Checks if any keyboard key or joystick button is pressed
bool InputEngine::AnyKeyPress() {
	return _any_key_press;
}


// Checks if any keyboard key or joystick button is released
bool InputEngine::AnyKeyRelease() {
	return _any_key_release;
}


// Handles all of the event processing for the game.
void InputEngine::EventHandler() {
	SDL_Event event; // Holds the game event
	
	// Reset all of the press and release flags so that they don't get detected twice.
	_any_key_press   = false;
	_any_key_release = false;

	_up_press             = false;
	_up_release           = false;
	_down_press           = false;
	_down_release         = false;
	_left_press           = false;
	_left_release         = false;
	_right_press          = false;
	_right_release        = false;
	_confirm_press        = false;
	_confirm_release      = false;
	_cancel_press         = false;
	_cancel_release       = false;
	_menu_press           = false;
	_menu_release         = false;
	_swap_press           = false;
	_swap_release         = false;
	_right_select_press   = false;
	_right_select_release = false;
	_left_select_press    = false;
	_left_select_release  = false;

	_pause_press = false;
	_quit_press = false;

	// Loops until there are no remaining events to process
	while (SDL_PollEvent(&event)) {
		_event = event;
		if (event.type == SDL_QUIT) {
			_quit_press = true;
			break;
		}
		// Check if the window was iconified/minimized or restored
		else if (event.type == SDL_ACTIVEEVENT) {
			// TEMP: pausing the game on a context switch between another application proved to
			// be rather annoying. The code which did this is commented out below. I think it would
			// be better if instead the application yielded for a certain amount of time when the
			// application looses context.

// 			if (event.active.state & SDL_APPACTIVE) {
// 				if (event.active.gain == 0) { // Window was iconified/minimized
// 					// Check if the game is in pause mode. Otherwise the player might put pause on,
// 					// minimize the window and then the pause is off.
// 					if (ModeManager->GetGameType() != MODE_MANAGER_PAUSE_MODE) {
// 						TogglePause();
// 					}
// 				}
// 				else if (ModeManager->GetGameType() == MODE_MANAGER_PAUSE_MODE) { // Window was restored
// 					TogglePause();
// 				}
// 			}
// 			else if (event.active.state & SDL_APPINPUTFOCUS) {
// 				if (event.active.gain == 0) { // Window lost keyboard focus (another application was made active)
// 					// Check if the game is in pause mode. Otherwise the player might put pause on,
// 					// minimize the window and then the pause is off.
// 					if (ModeManager->GetGameType() != MODE_MANAGER_PAUSE_MODE) {
// 						TogglePause();
// 					}
// 				}
// 				else if (ModeManager->GetGameType() == MODE_MANAGER_PAUSE_MODE) { // Window gain keyboard focus (not sure)
// 					TogglePause();
// 				}
// 			}
			break;
		}
		else if (event.type == SDL_KEYUP || event.type == SDL_KEYDOWN) {
			_KeyEventHandler(event.key);
		}
		else {
			_JoystickEventHandler(event);
		}
	} // while (SDL_PollEvent(&event)
} // void InputEngine::EventHandler()



// Handles all keyboard events for the game
void InputEngine::_KeyEventHandler(SDL_KeyboardEvent& key_event) {
	if (key_event.type == SDL_KEYDOWN) { // Key was pressed

		_any_key_press = true;

		if (key_event.keysym.mod & KMOD_CTRL || key_event.keysym.sym == SDLK_LCTRL || key_event.keysym.sym == SDLK_RCTRL) { // CTRL key was held down

			_any_key_press = false; // CTRL isn't "any key"! :)

			if (key_event.keysym.sym == SDLK_a) {
				// Toggle the display of advanced video engine information
				VideoManager->ToggleAdvancedDisplay();
			}
			else if (key_event.keysym.sym == SDLK_f) {
				// Toggle between full-screen and windowed mode
				VideoManager->ToggleFullscreen();
				VideoManager->ApplySettings();
				return;
			}
			else if (key_event.keysym.sym == SDLK_q) {
				_quit_press = true;
			}
			else if (key_event.keysym.sym == SDLK_r) {
				VideoManager->ToggleFPS();
				return;
			}
			else if (key_event.keysym.sym == SDLK_s) {
				// Take a screenshot of the current game
				static uint32 i = 1;
				string path = "";
				while (true)
				{
					path = hoa_utils::GetUserDataPath(true) + "screenshot_" + NumberToString<uint32>(i) + ".jpg";
					if (!DoesFileExist(path))
						break;
					i++;
				}
				VideoManager->MakeScreenshot(path);
				return;
			}
			else if (key_event.keysym.sym == SDLK_t) {
				// Display and cycle through the texture sheets
				VideoManager->Textures()->DEBUG_NextTexSheet();
				return;
			}

			//return;
		} // endif CTRL pressed

		// Note: a switch-case statement won't work here because Key.up is not an
		// integer value the compiler will whine and cry about it ;_;
		if (key_event.keysym.sym == SDLK_ESCAPE) {
			_quit_press = true;
			return;
		}
		else if (key_event.keysym.sym == _key.up) {
			_up_state = true;
			_up_press = true;
			return;
		}
		else if (key_event.keysym.sym == _key.down) {
			_down_state = true;
			_down_press = true;
			return;
		}
		else if (key_event.keysym.sym == _key.left) {
			_left_state = true;
			_left_press = true;
			return;
		}
		else if (key_event.keysym.sym == _key.right) {
			_right_state = true;
			_right_press = true;
			return;
		}
		else if (key_event.keysym.sym == _key.confirm) {
			_confirm_state = true;
			_confirm_press = true;
			return;
		}
		else if (key_event.keysym.sym == _key.cancel) {
			_cancel_state = true;
			_cancel_press = true;
			return;
		}
		else if (key_event.keysym.sym == _key.menu) {
			_menu_state = true;
			_menu_press = true;
			return;
		}
		else if (key_event.keysym.sym == _key.swap) {
			_swap_state = true;
			_swap_press = true;
			return;
		}
		else if (key_event.keysym.sym == _key.left_select) {
			_left_select_state = true;
			_left_select_press = true;
			return;
		}
		else if (key_event.keysym.sym == _key.right_select) {
			_right_select_state = true;
			_right_select_press = true;
			return;
		}
		else if (key_event.keysym.sym == _key.pause) {
			_pause_press = true;
			return;
		}
	}

	else { // Key was released

		_any_key_press = false;
		_any_key_release = true;

		if (key_event.keysym.sym == _key.up) {
			_up_state = false;
			_up_release = true;
			return;
		}
		else if (key_event.keysym.sym == _key.down) {
			_down_state = false;
			_down_release = true;
			return;
		}
		else if (key_event.keysym.sym == _key.left) {
			_left_state = false;
			_left_release = true;
			return;
		}
		else if (key_event.keysym.sym == _key.right) {
			_right_state = false;
			_right_release = true;
			return;
		}
		else if (key_event.keysym.sym == _key.confirm) {
			_confirm_state = false;
			_confirm_release = true;
			return;
		}
		else if (key_event.keysym.sym == _key.cancel) {
			_cancel_state = false;
			_cancel_release = true;
			return;
		}
		else if (key_event.keysym.sym == _key.menu) {
			_menu_state = false;
			_menu_release = true;
			return;
		}
		else if (key_event.keysym.sym == _key.swap) {
			_swap_state = false;
			_swap_release = true;
			return;
		}
		else if (key_event.keysym.sym == _key.left_select) {
			_left_select_state = false;
			_left_select_release = true;
			return;
		}
		else if (key_event.keysym.sym == _key.right_select) {
			_right_select_state = false;
			_right_select_release = true;
			return;
		}
	}
} // void InputEngine::_KeyEventHandler(SDL_KeyboardEvent& key_event)

// Handles all joystick events for the game
void InputEngine::_JoystickEventHandler(SDL_Event& js_event) {
	if (js_event.type == SDL_JOYAXISMOTION) {
		if (js_event.jaxis.axis == _joystick.x_axis) {
			if (js_event.jaxis.value < -_joystick.threshold) {
				if (!_left_state) {
					_left_state = true;
					_left_press = true;
				}
			}
			else {
				_left_state = false;
			}

			if (js_event.jaxis.value > _joystick.threshold) {
				if (!_right_state) {
					_right_state = true;
					_right_press = true;
				}
			}
			else {
				_right_state = false;
			}
		}
		else if (js_event.jaxis.axis == _joystick.y_axis) {
			if (js_event.jaxis.value < -_joystick.threshold) {
				if (!_up_state) {
					_up_state = true;
					_up_press = true;
				}
			}
			else {
				_up_state = false;
			}

			if (js_event.jaxis.value > _joystick.threshold) {
				if (!_down_state) {
					_down_state = true;
					_down_press = true;
				}
			}
			else {
				_down_state = false;
			}
		}

		if (js_event.jaxis.value > _joystick.threshold
			|| js_event.jaxis.value < -_joystick.threshold)
			_last_axis_moved = js_event.jaxis.axis;
	} // if (js_event.type == SDL_JOYAXISMOTION)

	else if (js_event.type == SDL_JOYBUTTONDOWN) {

		_any_key_press = true;

		if (js_event.jbutton.button == _joystick.confirm) {
			_confirm_state = true;
			_confirm_press = true;
			return;
		}
		else if (js_event.jbutton.button == _joystick.cancel) {
			_cancel_state = true;
			_cancel_press = true;
			return;
		}
		else if (js_event.jbutton.button == _joystick.menu) {
			_menu_state = true;
			_menu_press = true;
			return;
		}
		else if (js_event.jbutton.button == _joystick.swap) {
			_swap_state = true;
			_swap_press = true;
			return;
		}
		else if (js_event.jbutton.button == _joystick.left_select) {
			_left_select_state = true;
			_left_select_press = true;
			return;
		}
		else if (js_event.jbutton.button == _joystick.right_select) {
			_right_select_state = true;
			_right_select_press = true;
			return;
		}
		else if (js_event.jbutton.button == _joystick.pause) {
			_pause_press = true;
			return;
		}
		else if (js_event.jbutton.button == _joystick.quit) {
			_quit_press = true;
			return;
		}
	} // else if (js_event.type == JOYBUTTONDOWN)

	else if (js_event.type == SDL_JOYBUTTONUP) {
		_any_key_press = false;
		_any_key_release = true;

		if (js_event.jbutton.button == _joystick.confirm) {
			_confirm_state = false;
			_confirm_release = true;
			return;
		}
		else if (js_event.jbutton.button == _joystick.cancel) {
			_cancel_state = false;
			_cancel_release = true;
			return;
		}
		else if (js_event.jbutton.button == _joystick.menu) {
			_menu_state = false;
			_menu_release = true;
			return;
		}
		else if (js_event.jbutton.button == _joystick.swap) {
			_swap_state = false;
			_swap_release = true;
			return;
		}
		else if (js_event.jbutton.button == _joystick.left_select) {
			_left_select_state = false;
			_left_select_release = true;
			return;
		}
		else if (js_event.jbutton.button == _joystick.right_select) {
			_right_select_state = false;
			_right_select_release = true;
			return;
		}
	} // else if (js_event.type == JOYBUTTONUP)

	// NOTE: SDL_JOYBALLMOTION and SDL_JOYHATMOTION are ignored for now. Should we process them?
} // void InputEngine::_JoystickEventHandler(SDL_Event& js_event)


// Sets a new key over an older one. If the same key is used elsewhere, the older one is removed
void InputEngine::_SetNewKey(SDLKey & old_key, SDLKey new_key) {
	if (_key.up == new_key) { // up key used already
		_key.up = old_key;
		old_key = new_key;
		return;
	}
	if (_key.down == new_key) { // down key used already
		_key.down = old_key;
		old_key = new_key;
		return;
	}
	if (_key.left == new_key) { // left key used already
		_key.left = old_key;
		old_key = new_key;
		return;
	}
	if (_key.right == new_key) { // right key used already
		_key.right = old_key;
		old_key = new_key;
		return;
	}
	if (_key.confirm == new_key) { // confirm key used already
		_key.confirm = old_key;
		old_key = new_key;
		return;
	}
	if (_key.cancel == new_key) { // cancel key used already
		_key.cancel = old_key;
		old_key = new_key;
		return;
	}
	if (_key.menu == new_key) { // menu key used already
		_key.menu = old_key;
		old_key = new_key;
		return;
	}
	if (_key.swap == new_key) { // swap key used already
		_key.swap = old_key;
		old_key = new_key;
		return;
	}
	if (_key.left_select == new_key) { // left_select key used already
		_key.left_select = old_key;
		old_key = new_key;
		return;
	}
	if (_key.right_select == new_key) { // right_select key used already
		_key.right_select = old_key;
		old_key = new_key;
		return;
	}
	if (_key.pause == new_key) { // pause key used already
		_key.pause = old_key;
		old_key = new_key;
		return;
	}

	old_key = new_key; // Otherwise simply overwrite the old value
} // end InputEngine::_SetNewKey(SDLKey & old_key, SDLKey new_key)


// Sets a new joystick button over an older one. If the same button is used elsewhere, the older one is removed
void InputEngine::_SetNewJoyButton(uint8 & old_button, uint8 new_button) {
	if (_joystick.confirm == new_button) { // confirm button used already
		_joystick.confirm = old_button;
		old_button = new_button;
		return;
	}
	if (_joystick.cancel == new_button) { // cancel button used already
		_joystick.cancel = old_button;
		old_button = new_button;
		return;
	}
	if (_joystick.menu == new_button) { // menu button used already
		_joystick.menu = old_button;
		old_button = new_button;
		return;
	}
	if (_joystick.swap == new_button) { // swap button used already
		_joystick.swap = old_button;
		old_button = new_button;
		return;
	}
	if (_joystick.left_select == new_button) { // left_select button used already
		_joystick.left_select = old_button;
		old_button = new_button;
		return;
	}
	if (_joystick.right_select == new_button) { // right_select button used already
		_joystick.right_select = old_button;
		old_button = new_button;
		return;
	}
	if (_joystick.pause == new_button) { // pause button used already
		_joystick.pause = old_button;
		old_button = new_button;
		return;
	}

	old_button = new_button; // Otherwise simply overwrite the old value
} // end InputEngine::_SetNewJoyButton(uint8 & old_button, uint8 new_button)


} // namespace hoa_input
