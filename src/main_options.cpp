///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    main_options.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Implementations for functions that handle command-line arguments.
*** **************************************************************************/

#include "utils.h"
#include "defs.h"

#include "audio.h"
#include "video.h"
#include "script.h"
#include "input.h"
#include "system.h"
#include "mode_manager.h"

#include "global.h"

#include "main_options.h"

using namespace std;
using namespace hoa_utils;

namespace hoa_main {

bool ParseProgramOptions(int32 &return_code, int32 argc, char **argv) {
	// Convert the argument list to a vector of strings for convenience
	vector<string> options(argv, argv + argc);
	return_code = 0;

	for (uint32 i = 1; i < options.size(); i++) {
		if (options[i] == "-c" || options[i] == "--check") {
			if (CheckFiles() == true) {
				return_code = 0;
			}
			else {
				return_code = 1;
			}
			return false;
		}
		else if (options[i] == "-d" || options[i] == "--debug") {
			if ((i + 1) >= options.size()) {
				cerr << "Option " << options[i] << " requires an argument." << endl;
				PrintUsage();
				return_code = 1;
				return false;
			}
			if (EnableDebugging(options[i + 1]) == false) {
				return_code = 1;
				return false;
			}
			i++;
		}
		else if (options[i] == "--disable-audio") {
			hoa_audio::AUDIO_ENABLE = false;
		}
		else if (options[i] == "-h" || options[i] == "--help") {
			PrintUsage();
			return_code = 0;
			return false;
		}
		else if (options[i] == "-i" || options[i] == "--info") {
			if (PrintSystemInformation() == true) {
				return_code = 0;
			}
			else {
				return_code = 1;
			}
			return false;
		}
		else if (options[i] == "-r" || options[i] == "--reset") {
			if (ResetSettings() == true) {
				return_code = 0;
			}
			else {
				return_code = 1;
			}
			return_code = 0;
			return false;
		}
		else {
			cerr << "Unrecognized option: " << options[i] << endl;
			PrintUsage();
			return_code = 1;
			return false;
		}
	}

	return true;
} // bool ParseProgramOptions(int32_t &return_code, int32_t argc, char **argv)



bool ParseSecondaryOptions(string vars, vector<string>& options) {
	uint32 sbegin = 0;

	if (vars.empty()) {
		cerr << "ERROR: debug specifier string is empty" << endl;
		return false;
	}

	// Find the first non-whitespace character
	// TODO: this loop needs to be made more robust to errors
	while (vars[sbegin] == ' ' || vars[sbegin] == '\t') {
		sbegin++;
		if (sbegin >= vars.size()) {
			cerr << "ERROR: no white-space characters in debug specifier string" << endl;
			return false;
		}
	}

	// Parse the vars string on white-space characters and fill the args vector
	// TODO: this loop needs to be made more robust to errors
	for (uint32 i = sbegin; i < vars.size(); i++) {
		if (vars[i] == ' ' || vars[i] == '\t') {
			options.push_back(vars.substr(sbegin, i - sbegin));
			sbegin = i + 1;
		}
	}
	options.push_back(vars.substr(sbegin, vars.size() - sbegin));
	return true;
} // bool ParseSecondaryOptions(string vars, vector<string>& options);


// Prints out the usage options (arguments) for running the program (work in progress)
void PrintUsage() {
	cout << "usage: allacrost [options]" << endl;
	cout << "  --check/-c        :: checks all files for integrity" << endl;
	cout << "  --debug/-d <args> :: enables debug statements in specifed sections of the" << endl;
	cout << "                       program, where <args> can be:" << endl;
	cout << "                       all, audio, battle, boot, data, global, input," << endl;
	cout << "                       map, mode_manager, pause, quit, scene, system" << endl;
	cout << "                       utils, video" << endl;
	cout << "  --disable-audio   :: disables loading and playing audio" << endl;
	cout << "  --help/-h         :: prints this help menu" << endl;
	cout << "  --info/-i         :: prints information about the user's system" << endl;
	cout << "  --reset/-r        :: resets game configuration to use default settings" << endl;
	cout << "  --version/-v      :: checks for newer versions of Allacrost online" << endl;
}



bool PrintSystemInformation() {
	printf("\n===== System Information\n");

	// Initialize SDL and its subsystems and make sure it shutdowns properly on exit
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
		cerr << "ERROR: Unable to initialize SDL: " << SDL_GetError() << endl;
		return false;
	}
	atexit(SDL_Quit);

	printf("SDL version (compiled):  %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
	printf("SDL version (linked):    %d.%d.%d\n", SDL_Linked_Version()->major, SDL_Linked_Version()->minor, SDL_Linked_Version()->patch);

	SDL_Joystick *js_test;
	int32 js_num = SDL_NumJoysticks();
	printf("Number of joysticks found:  %d\n", js_num);

	// Print out information about each joystick
	for (int32 i = 0; i < js_num; i++) {
		printf("  Joystick #%d\n", i);
		printf("    Joystick Name: %s\n", SDL_JoystickName(i));
		js_test = SDL_JoystickOpen(i);
		if (js_test == NULL)
			printf("    ERROR: SDL was unable to open joystick #%d!\n", i);
		else {
			printf("    Number Axes: %d\n", SDL_JoystickNumAxes(js_test));
			printf("    Number Buttons: %d\n", SDL_JoystickNumButtons(js_test));
			printf("    Number Trackballs: %d\n", SDL_JoystickNumBalls(js_test));
			printf("    Number Hat Switches: %d\n", SDL_JoystickNumHats(js_test));
			SDL_JoystickClose(js_test);
		}
	}

	printf("\n===== Video Information\n");

	// TODO: This code should be re-located to a function (DEBUG_PrintInfo()) in the video engine
// 	hoa_video::VideoManager = hoa_video::VideoEngine::SingletonCreate();
// 	if (hoa_video::VideoManager->SingletonInitialize() == false) {
// 		cerr << "ERROR: unable to initialize the VideoManager" << endl;
// 		return false;
// 	}
// 	else {
// 		hoa_video::VideoManager->DEBUG_PrintInfo();
// 	}
// 	hoa_video::VideoEngine::SingletonDestroy();

	// TODO: print the OpenGL version number here

	printf("SDL_ttf version (compiled): %d.%d.%d\n", SDL_TTF_MAJOR_VERSION, SDL_TTF_MINOR_VERSION, SDL_TTF_PATCHLEVEL);
	// printf("SDL_ttf version (linked):   %d.%d.%d\n", Ttf_Linked_Version()->major, Ttf_Linked_Version()->minor, Ttf_Linked_Version()->patch);

	char video_driver[80];
	SDL_VideoDriverName(video_driver, 80);
	printf("Name of video driver: %s\n", video_driver);

	const SDL_VideoInfo *user_video;
	user_video = SDL_GetVideoInfo(); // Get information about the user's video system
	cout << "  Best available video mode" << endl;
	cout << "    Creates hardware surfaces: ";
	if (user_video->hw_available == 1)
		cout << "yes\n";
	else
		cout << "no\n";
	cout << "    Has window manager available: ";
	if (user_video->wm_available == 1)
		cout << "yes\n";
	else
		cout << "no\n";
	cout << "    Hardware to hardware blits accelerated: ";
	if (user_video->blit_hw == 1)
		cout << "yes\n";
	else
		cout << "no\n";
	cout << "    Hardware to hardware colorkey blits accelerated: ";
	if (user_video->blit_hw_CC == 1)
		cout << "yes\n";
	else
		cout << "no\n";
	cout << "    Hardware to hardware alpha blits accelerated: ";
	if (user_video->blit_hw_A == 1)
		cout << "yes\n";
	else
		cout << "no\n";
	cout << "    Software to hardware blits acceleerated: ";
	if (user_video->blit_sw == 1)
		cout << "yes\n";
	else
		cout << "no\n";
	cout << "    Software to hardware colorkey blits accelerated: ";
	if (user_video->blit_sw_CC == 1)
		cout << "yes\n";
	else
		cout << "no\n";
	cout << "    Software to hardware alpha blits accelerated: ";
	if (user_video->blit_sw_A == 1)
		cout << "yes\n";
	else
		cout << "no\n";
	cout << "    Color fills accelerated: ";
	if (user_video->blit_fill == 1)
		cout << "yes\n";
	else
		cout << "no\n";
	cout << "    Total video memory: " << user_video->video_mem << " kilobytes" << endl;
	// cout << "    Best pixel format: " << user_video->vfmt << endl;

	printf("\n===== Audio Information\n");

	hoa_audio::AudioManager = hoa_audio::AudioEngine::SingletonCreate();
	if (hoa_audio::AudioManager->SingletonInitialize() == false) {
		cerr << "ERROR: unable to initialize the AudioManager" << endl;
		return false;
	}
	else {
		hoa_audio::AudioManager->DEBUG_PrintInfo();
	}
	hoa_audio::AudioEngine::SingletonDestroy();

	printf("\n");

	return true;
} // bool PrintSystemInformation()



bool ResetSettings() {
	cout << "This option is not yet implemented." << endl;
	return false;
} // bool ResetSettings()



bool CheckFiles() {
	cout << "This option is not yet implemented." << endl;
	return false;
} // bool CheckFiles()



bool EnableDebugging(string vars) {
	// A vector of all the debug arguments
	vector<string> args;
	ParseSecondaryOptions(vars, args);

	// Enable all specified debug variables
	for (uint32 i = 0; i < args.size(); i++) {
		if (args[i] == "all") {
			// This causes every call to SDL_SetError to also print an error message on stderr.
			// NOTE: commented out because apparently SDL_putenv is not yet an available function on some systems
			// SDL_putenv("SDL_DEBUG=1");

			hoa_audio::AUDIO_DEBUG                  = true;
			hoa_battle::BATTLE_DEBUG                = true;
			hoa_boot::BOOT_DEBUG                    = true;
			hoa_common::COMMON_DEBUG                = true;
			hoa_script::SCRIPT_DEBUG                = true;
			hoa_mode_manager::MODE_MANAGER_DEBUG    = true;
			hoa_input::INPUT_DEBUG                  = true;
			hoa_system::SYSTEM_DEBUG                = true;
			hoa_global::GLOBAL_DEBUG                = true;
			hoa_map::MAP_DEBUG                      = true;
			hoa_menu::MENU_DEBUG                    = true;
			hoa_pause::PAUSE_DEBUG                  = true;
			hoa_shop::SHOP_DEBUG                    = true;
			hoa_scene::SCENE_DEBUG                  = true;
			hoa_utils::UTILS_DEBUG                  = true;
			hoa_video::VIDEO_DEBUG                  = true;
		}
		else if (args[i] == "audio") {
			hoa_audio::AUDIO_DEBUG = true;
		}
		else if (args[i] == "battle") {
			hoa_battle::BATTLE_DEBUG = true;
		}
		else if (args[i] == "boot") {
			hoa_boot::BOOT_DEBUG = true;
		}
		else if (args[i] == "common") {
			hoa_common::COMMON_DEBUG = true;
		}
		else if (args[i] == "data") {
			hoa_script::SCRIPT_DEBUG = true;
		}
		else if (args[i] == "mode_manager") {
			hoa_mode_manager::MODE_MANAGER_DEBUG = true;
		}
		else if (args[i] == "input") {
			hoa_input::INPUT_DEBUG = true;
		}
		else if (args[i] == "system") {
			hoa_system::SYSTEM_DEBUG = true;
		}
		else if (args[i] == "global") {
			hoa_global::GLOBAL_DEBUG = true;
		}
		else if (args[i] == "map") {
			hoa_map::MAP_DEBUG = true;
		}
		else if (args[i] == "menu") {
			hoa_menu::MENU_DEBUG = true;
		}
		else if (args[i] == "pause") {
			hoa_pause::PAUSE_DEBUG = true;
		}
		else if (args[i] == "scene") {
			hoa_scene::SCENE_DEBUG = true;
		}
		else if (args[i] == "shop") {
			hoa_shop::SHOP_DEBUG = true;
		}
		else if (args[i] == "utils") {
			hoa_utils::UTILS_DEBUG = true;
		}
		else if (args[i] == "video") {
			hoa_video::VIDEO_DEBUG = true;
		}
		else {
			cerr << "ERROR: invalid debug argument: " << args[i] << endl;
			return false;
		}
	} // for (uint32 i = 0; i < args.size(); i++)

	return true;
} // bool EnableDebugging(string vars)

} // namespace hoa_main
