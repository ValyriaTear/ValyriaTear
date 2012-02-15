////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    main.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Allacrost initialization code and main game loop.
***
*** The code in this file is the first to execute when the game is started and
*** the last to execute before the game exits. The core engine of Allacrost
*** uses time-based updating, which means that the state of the game is
*** updated based on how much time has expired since the last update.
***
*** The main game loop consists of the following steps.
***
*** -# Render the newly drawn frame to the screen.
*** -# Collect information on new user input events.
*** -# Update the main loop timer.
*** -# Update the game status based on how much time expired from the last update.
*** ***************************************************************************/

#include <iostream>
#include <ctime>
#include <cmath>
#include <string>
#include <ctime>
#ifdef __MACH__
	#include <unistd.h>
	#include <string>
#endif

#include "utils.h"
#include "defs.h"

#include "audio.h"
#include "video.h"
#include "gui.h"
#include "input.h"
#include "script.h"
#include "system.h"

#include "global.h"

#include "mode_manager.h"
#include "boot.h"
#include "map.h"
#include "main_options.h"


using namespace std;
using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_mode_manager;
using namespace hoa_input;
using namespace hoa_system;
using namespace hoa_global;
using namespace hoa_script;
using namespace hoa_boot;
using namespace hoa_map;


/** \brief Frees all data allocated by Allacrost by destroying the singleton classes
***
*** \note <b>Do not attempt to call or otherwise reference this function.</b>
*** It is for use in the application's main() function only.
***
*** Deleteing the singleton class objects will free all of the memory that the game uses.
*** This is because all other classes and data structures in Allacrost are managed
*** by these singletons either directly or in directly. For example, BattleMode is a
*** class object that is managed by the ModeEngine class, and thus the GameModeManager
*** destructor will also invoke the BattleMode destructor (as well as the destructors of any
*** other game modes that exist).
**/
void QuitAllacrost() {
	// NOTE: Even if the singleton objects do not exist when this function is called, invoking the
	// static Destroy() singleton function will do no harm (it checks that the object exists before deleting it).

	// Delete the mode manager first so that all game modes free their resources
	ModeEngine::SingletonDestroy();

	// Delete the global manager second to remove all object references corresponding to other engine subsystems
	GameGlobal::SingletonDestroy();

	// Delete all of the reamining independent engine components
	GUISystem::SingletonDestroy();
	AudioEngine::SingletonDestroy();
	InputEngine::SingletonDestroy();
	ScriptEngine::SingletonDestroy();
	SystemEngine::SingletonDestroy();
	VideoEngine::SingletonDestroy();
} // void QuitAllacrost()

/** \brief Reads in all of the saved game settings and sets values in the according game manager classes
*** \return True if the settings were loaded successfully
**/
bool LoadSettings()
{
	ReadScriptDescriptor settings;
	if (settings.OpenFile(GetSettingsFilename()) == false)
		return false;

	settings.OpenTable("settings");

	// Load language settings
//	uint32 lang_code = static_cast<uint32>(settings.ReadInt("lang_code"));

//	if (lang_code == 1) SystemManager->SetLanguage("en@quot");
//	else if (lang_code == 2) SystemManager->SetLanguage("fr");
//	else if (lang_code == 3) SystemManager->SetLanguage("pt_BR");
//	else if (lang_code == 4) SystemManager->SetLanguage("es");
//	else if (lang_code == 5) SystemManager->SetLanguage("de");
//	else SystemManager->SetLanguage("en@quot");

	SystemManager->SetLanguage(static_cast<std::string>(settings.ReadString("language")));

	settings.OpenTable("key_settings");
	InputManager->SetUpKey(static_cast<SDLKey>(settings.ReadInt("up")));
	InputManager->SetDownKey(static_cast<SDLKey>(settings.ReadInt("down")));
	InputManager->SetLeftKey(static_cast<SDLKey>(settings.ReadInt("left")));
	InputManager->SetRightKey(static_cast<SDLKey>(settings.ReadInt("right")));
	InputManager->SetConfirmKey(static_cast<SDLKey>(settings.ReadInt("confirm")));
	InputManager->SetCancelKey(static_cast<SDLKey>(settings.ReadInt("cancel")));
	InputManager->SetMenuKey(static_cast<SDLKey>(settings.ReadInt("menu")));
	InputManager->SetSwapKey(static_cast<SDLKey>(settings.ReadInt("swap")));
	InputManager->SetLeftSelectKey(static_cast<SDLKey>(settings.ReadInt("left_select")));
	InputManager->SetRightSelectKey(static_cast<SDLKey>(settings.ReadInt("right_select")));
	InputManager->SetPauseKey(static_cast<SDLKey>(settings.ReadInt("pause")));
	settings.CloseTable();

	if (settings.IsErrorDetected()) {
		cerr << "SETTINGS LOAD ERROR: failure while trying to retrieve key map "
			<< "information from file: " << GetSettingsFilename() << endl;
		cerr << settings.GetErrorMessages() << endl;
		return false;
	}

	settings.OpenTable("joystick_settings");
	// TEMP: this is a hack to disable joystick input to fix a bug with "phantom" joysticks on certain systems.
	// In the future it should call a method of the input engine to disable the joysticks.
	if (settings.DoesBoolExist("input_disabled") && settings.ReadBool("input_disabled") == true) {
		PRINT_DEBUG << "settings file specified to disable joystick input" << endl;
		SDL_JoystickEventState(SDL_IGNORE);
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	}
	InputManager->SetJoyIndex(static_cast<int32>(settings.ReadInt("index")));
	InputManager->SetConfirmJoy(static_cast<uint8>(settings.ReadInt("confirm")));
	InputManager->SetCancelJoy(static_cast<uint8>(settings.ReadInt("cancel")));
	InputManager->SetMenuJoy(static_cast<uint8>(settings.ReadInt("menu")));
	InputManager->SetSwapJoy(static_cast<uint8>(settings.ReadInt("swap")));
	InputManager->SetLeftSelectJoy(static_cast<uint8>(settings.ReadInt("left_select")));
	InputManager->SetRightSelectJoy(static_cast<uint8>(settings.ReadInt("right_select")));
	InputManager->SetPauseJoy(static_cast<uint8>(settings.ReadInt("pause")));

	InputManager->SetQuitJoy(static_cast<uint8>(settings.ReadInt("quit")));
	if (settings.DoesIntExist("x_axis"))
		InputManager->SetXAxisJoy(static_cast<int8>(settings.ReadInt("x_axis")));
	if (settings.DoesIntExist("y_axis"))
		InputManager->SetYAxisJoy(static_cast<int8>(settings.ReadInt("y_axis")));

	// WinterKnight: These are hidden settings. You can change them by editing settings.lua,
	// but they are not available in the options menu at this time.
	if (settings.DoesIntExist("threshold"))
		InputManager->SetThresholdJoy(static_cast<uint16>(settings.ReadInt("threshold")));

	settings.CloseTable();

	// battle_settings.timer_multiplier is also a hidden setting
	if (settings.DoesTableExist("battle_settings")) {
		settings.OpenTable("battle_settings");
		// TEMP: I don't think that we should do this... - Roots
// 		if (settings.DoesFloatExist("timer_multiplier"))
// 			hoa_battle::timer_multiplier = static_cast<float>(settings.ReadFloat("timer_multiplier"));
// 		if (settings.DoesBoolExist("wait"))
// 			hoa_battle::wait = static_cast<bool>(settings.ReadBool("wait"));
		settings.CloseTable();
	}

	if (settings.IsErrorDetected()) {
		cerr << "SETTINGS LOAD ERROR: an error occured while trying to retrieve joystick mapping information "
			<< "from file: " << GetSettingsFilename() << endl;
		cerr << settings.GetErrorMessages() << endl;
		return false;
	}

	// Load video settings
	settings.OpenTable("video_settings");
	bool fullscreen = settings.ReadBool("full_screen");
	int32 resx = settings.ReadInt("screen_resx");
	int32 resy = settings.ReadInt("screen_resy");
	VideoManager->SetInitialResolution(resx, resy);
	VideoManager->SetFullscreen(fullscreen);
	settings.CloseTable();

	if (settings.IsErrorDetected()) {
		cerr << "SETTINGS LOAD ERROR: failure while trying to retrieve video settings "
			<< "information from file: " << GetSettingsFilename() << endl;
		cerr << settings.GetErrorMessages() << endl;
		return false;
	}

	// Load Audio settings
	if (AUDIO_ENABLE) {
		settings.OpenTable("audio_settings");
		AudioManager->SetMusicVolume(static_cast<float>(settings.ReadFloat("music_vol")));
		AudioManager->SetSoundVolume(static_cast<float>(settings.ReadFloat("sound_vol")));
	}
	settings.CloseAllTables();

	if (settings.IsErrorDetected()) {
		cerr << "SETTINGS LOAD ERROR: failure while trying to retrieve audio settings "
			<< "information from file: " << GetSettingsFilename() << endl;
		cerr << settings.GetErrorMessages() << endl;
		return false;
	}

	settings.CloseFile();

	return true;
} // bool LoadSettings()


/** \brief Initializes all engine components and makes other preparations for the game to start
*** \return True if the game engine was initialized successfully, false if an unrecoverable error occured
**/
void InitializeEngine() throw (Exception) {
	// Initialize SDL. The video, audio, and joystick subsystems are initialized elsewhere.
	if (SDL_Init(SDL_INIT_TIMER) != 0) {
		throw Exception("MAIN ERROR: Unable to initialize SDL: ", __FILE__, __LINE__, __FUNCTION__);
	}

	// Create and initialize singleton class managers
	AudioManager = AudioEngine::SingletonCreate();
	InputManager = InputEngine::SingletonCreate();
	VideoManager = VideoEngine::SingletonCreate();
	ScriptManager = ScriptEngine::SingletonCreate();
	SystemManager = SystemEngine::SingletonCreate();
	ModeManager = ModeEngine::SingletonCreate();
	GUIManager = GUISystem::SingletonCreate();
	GlobalManager = GameGlobal::SingletonCreate();

	if (VideoManager->SingletonInitialize() == false) {
		throw Exception("ERROR: unable to initialize VideoManager", __FILE__, __LINE__, __FUNCTION__);
	}

	if (AudioManager->SingletonInitialize() == false) {
		throw Exception("ERROR: unable to initialize AudioManager", __FILE__, __LINE__, __FUNCTION__);
	}

	if (ScriptManager->SingletonInitialize() == false) {
		throw Exception("ERROR: unable to initialize ScriptManager", __FILE__, __LINE__, __FUNCTION__);
	}

	hoa_defs::BindEngineCode();
	hoa_defs::BindCommonCode();
	hoa_defs::BindModeCode();

	if (SystemManager->SingletonInitialize() == false) {
		throw Exception("ERROR: unable to initialize SystemManager", __FILE__, __LINE__, __FUNCTION__);
	}
	if (InputManager->SingletonInitialize() == false) {
		throw Exception("ERROR: unable to initialize InputManager", __FILE__, __LINE__, __FUNCTION__);
	}
	if (ModeManager->SingletonInitialize() == false) {
		throw Exception("ERROR: unable to initialize ModeManager", __FILE__, __LINE__, __FUNCTION__);
	}
	if (GlobalManager->SingletonInitialize() == false) {
		throw Exception("ERROR: unable to initialize GlobalManager", __FILE__, __LINE__, __FUNCTION__);
	}

	// Set the window icon
	#ifdef _WIN32
		SDL_WM_SetIcon(SDL_LoadBMP("img/logos/program_icon.bmp"), NULL);
	#else
		// Later, add an icon here for non-Windows systems (which support more than 32x32 .bmp files)
		SDL_WM_SetIcon(SDL_LoadBMP("img/logos/program_icon.bmp"), NULL);
	#endif

	// Load all the settings from lua. This includes some engine configuration settings.
	if (LoadSettings() == false)
		throw Exception("ERROR: Unable to load settings file", __FILE__, __LINE__, __FUNCTION__);

	// Apply engine configuration settings with delayed initialization calls to the managers
	InputManager->InitializeJoysticks();
	if (VideoManager->ApplySettings() == false)
		throw Exception("ERROR: Unable to apply video settings", __FILE__, __LINE__, __FUNCTION__);
	if (VideoManager->FinalizeInitialization() == false)
		throw Exception("ERROR: Unable to apply video settings", __FILE__, __LINE__, __FUNCTION__);
	if (GUIManager->LoadMenuSkin("black_sleet", "img/menus/black_sleet_skin.png", "img/menus/black_sleet_texture.png") == false) {
		throw Exception("Failed to load the 'Black Sleet' MenuSkin images.", __FILE__, __LINE__, __FUNCTION__);
	}
	// NOTE: This function call should have its argument set to false for release builds
	GUIManager->DEBUG_EnableGUIOutlines(false);

	// Load all standard font sets used across the game
	if (VideoManager->Text()->LoadFont("img/fonts/libertine_capitals.ttf", "title20", 20) == false) {
		throw Exception("Failed to load libertine_capitals.ttf font at size 20", __FILE__, __LINE__, __FUNCTION__);
	}
	if (VideoManager->Text()->LoadFont("img/fonts/libertine_capitals.ttf", "title22", 22) == false) {
		throw Exception("Failed to load libertine_capitals.ttf font at size 22", __FILE__, __LINE__, __FUNCTION__);
	}
	if (VideoManager->Text()->LoadFont("img/fonts/libertine_capitals.ttf", "title24", 24) == false) {
		throw Exception("Failed to load libertine_capitals.ttf font at size 24", __FILE__, __LINE__, __FUNCTION__);
	}
	if (VideoManager->Text()->LoadFont("img/fonts/libertine_capitals.ttf", "title28", 28) == false) {
		throw Exception("Failed to load libertine_capitals.ttf font at size 28", __FILE__, __LINE__, __FUNCTION__);
	}

	if (VideoManager->Text()->LoadFont("img/fonts/libertine.ttf", "text18", 18) == false) {
		throw Exception("Failed to load libertine.ttf font at size 18", __FILE__, __LINE__, __FUNCTION__);
	}
	if (VideoManager->Text()->LoadFont("img/fonts/libertine.ttf", "text20", 20) == false) {
		throw Exception("Failed to load libertine.ttf font at size 20", __FILE__, __LINE__, __FUNCTION__);
	}
	if (VideoManager->Text()->LoadFont("img/fonts/libertine.ttf", "text22", 22) == false) {
		throw Exception("Failed to load libertine.ttf font at size 22", __FILE__, __LINE__, __FUNCTION__);
	}
	if (VideoManager->Text()->LoadFont("img/fonts/libertine.ttf", "text24", 24) == false) {
		throw Exception("Failed to load libertine.ttf font at size 24", __FILE__, __LINE__, __FUNCTION__);
	}

	VideoManager->Text()->SetDefaultStyle(TextStyle("text22", Color::white, VIDEO_TEXT_SHADOW_BLACK, 1, -2));

	// Set the window title and icon name
	SDL_WM_SetCaption("Hero of Allacrost", "Hero of Allacrost");

	// Hide the mouse cursor since we don't use or acknowledge mouse input from the user
	SDL_ShowCursor(SDL_DISABLE);

	// Enabled for multilingual keyboard support
	SDL_EnableUNICODE(1);

	// Ignore the events that we don't care about so they never appear in the event queue
	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
	SDL_EventState(SDL_VIDEOEXPOSE, SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

	if (GUIManager->SingletonInitialize() == false) {
		throw Exception("ERROR: unable to initialize GUIManager", __FILE__, __LINE__, __FUNCTION__);
	}

	SystemManager->InitializeTimers();
} // void InitializeEngine()


// Every great game begins with a single function :)
int main(int argc, char *argv[]) {
	// When the program exits, the QuitAllacrost() function will be called first, followed by SDL_Quit()
	atexit(SDL_Quit);
	atexit(QuitAllacrost);

	try {
		// Change to the directory where the Allacrost data is stored
		#ifdef __MACH__
			string path;
			path = argv[0];
			// Remove the binary name
			path.erase(path.find_last_of('/'));
			// Remove the MacOS directory
			path.erase(path.find_last_of('/'));
			// Now the program should be in app/Contents
			path.append ("/Resources/");
			chdir(path.c_str());
		#elif (defined(__linux__) || defined(__FreeBSD__)) && !defined(RELEASE_BUILD)
			// Look for data files in DATADIR only if they are not available in the current directory.
			if (ifstream("dat/config/settings.lua") == NULL) {
				if (chdir(DATADIR) != 0) {
					throw Exception("ERROR: failed to change directory to data location", __FILE__, __LINE__, __FUNCTION__);
				}
			}
		#endif

		// Initialize the random number generator (note: 'unsigned int' is a required usage in this case)
		srand(static_cast<unsigned int>(time(NULL)));

		// This variable will be set by the ParseProgramOptions function
		int32 return_code = EXIT_FAILURE;

		// Parse command lines and exit out of the game if needed
		if (hoa_main::ParseProgramOptions(return_code, static_cast<int32>(argc), argv) == false) {
			return static_cast<int>(return_code);
		}

		// Function call below throws exceptions if any errors occur
		InitializeEngine();

	} catch (Exception& e) {
		#ifdef WIN32
		MessageBox(NULL, e.ToString().c_str(), "Unhandled exception", MB_OK | MB_ICONERROR);
		#else
		cerr << e.ToString() << endl;
		#endif
		return EXIT_FAILURE;
	}

	ModeManager->Push(new BootMode());

	try {
		// This is the main loop for the game. The loop iterates once for every frame drawn to the screen.
		while (SystemManager->NotDone()) {
			// 1) Render the scene
			VideoManager->Clear();
			ModeManager->Draw();
			VideoManager->Display(SystemManager->GetUpdateTime());

			// 2) Process all new events
			InputManager->EventHandler();

			// 3) Update any streaming audio sources
			AudioManager->Update();

			// 4) Update timers for correct time-based movement operation
			SystemManager->UpdateTimers();

			// 5) Update the game status
			ModeManager->Update();

		} // while (SystemManager->NotDone())
	} catch (Exception& e) {
		#ifdef WIN32
		MessageBox(NULL, e.ToString().c_str(), "Unhandled exception", MB_OK | MB_ICONERROR);
		#else
		cerr << e.ToString() << endl;
		#endif
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
} // int main(int argc, char *argv[])
