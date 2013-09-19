///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    main_options.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Implementations for functions that handle command-line arguments.
*** **************************************************************************/

#include "utils/utils_pch.h"
#include "main_options.h"

#include "engine/audio/audio.h"
#include "engine/video/video.h"
#include "engine/script/script.h"
#include "engine/input.h"
#include "engine/system.h"
#include "engine/mode_manager.h"

#include "common/global/global.h"

namespace vt_battle {
extern bool BATTLE_DEBUG;
}
namespace vt_boot {
extern bool BOOT_DEBUG;
}
namespace vt_common {
extern bool COMMON_DEBUG;
}
namespace vt_menu {
extern bool MENU_DEBUG;
}
namespace vt_pause {
extern bool PAUSE_DEBUG;
}
namespace vt_shop {
extern bool SHOP_DEBUG;
}

using namespace vt_utils;

namespace vt_main
{

bool ParseProgramOptions(int32 &return_code, int32 argc, char **argv)
{
    // Convert the argument list to a vector of strings for convenience
    std::vector<std::string> options(argv, argv + argc);
    return_code = 0;

    for(uint32 i = 1; i < options.size(); i++) {
        if(options[i] == "-c" || options[i] == "--check") {
            if(CheckFiles() == true) {
                return_code = 0;
            } else {
                return_code = 1;
            }
            return false;
        } else if(options[i] == "-d" || options[i] == "--debug") {
            if((i + 1) >= options.size()) {
                std::cerr << "Option " << options[i] << " requires an argument." << std::endl;
                PrintUsage();
                return_code = 1;
                return false;
            }
            if(EnableDebugging(options[i + 1]) == false) {
                return_code = 1;
                return false;
            }
            i++;
        } else if(options[i] == "--disable-audio") {
            vt_audio::AUDIO_ENABLE = false;
        } else if(options[i] == "-h" || options[i] == "--help") {
            PrintUsage();
            return_code = 0;
            return false;
        } else if(options[i] == "-i" || options[i] == "--info") {
            if(PrintSystemInformation() == true) {
                return_code = 0;
            } else {
                return_code = 1;
            }
            return false;
        } else if(options[i] == "-r" || options[i] == "--reset") {
            if(ResetSettings() == true) {
                return_code = 0;
            } else {
                return_code = 1;
            }
            return_code = 0;
            return false;
        } else {
            std::cerr << "Unrecognized option: " << options[i] << std::endl;
            PrintUsage();
            return_code = 1;
            return false;
        }
    }

    return true;
} // bool ParseProgramOptions(int32_t &return_code, int32_t argc, char **argv)



bool ParseSecondaryOptions(const std::string &vars, std::vector<std::string>& options)
{
    uint32 sbegin = 0;

    if(vars.empty()) {
        std::cerr << "ERROR: debug specifier string is empty" << std::endl;
        return false;
    }

    // Find the first non-whitespace character
    // TODO: this loop needs to be made more robust to errors
    while(vars[sbegin] == ' ' || vars[sbegin] == '\t') {
        sbegin++;
        if(sbegin >= vars.size()) {
            std::cerr << "ERROR: no white-space characters in debug specifier string" << std::endl;
            return false;
        }
    }

    // Parse the vars string on white-space characters and fill the args vector
    // TODO: this loop needs to be made more robust to errors
    for(uint32 i = sbegin; i < vars.size(); i++) {
        if(vars[i] == ' ' || vars[i] == '\t') {
            options.push_back(vars.substr(sbegin, i - sbegin));
            sbegin = i + 1;
        }
    }
    options.push_back(vars.substr(sbegin, vars.size() - sbegin));
    return true;
} // bool ParseSecondaryOptions(string vars, vector<string>& options);


// Prints out the usage options (arguments) for running the program (work in progress)
void PrintUsage()
{
    std::cout
            << "usage: "APPSHORTNAME" [options]" << std::endl
            << "  --check/-c        :: checks all files for integrity" << std::endl
            << "  --debug/-d <args> :: enables debug statements in specified sections of the" << std::endl
            << "                       program, where <args> can be:" << std::endl
            << "                       all, audio, battle, boot, data, global, input," << std::endl
            << "                       map, mode_manager, pause, quit, scene, system" << std::endl
            << "                       utils, video" << std::endl
            << "  --disable-audio   :: disables loading and playing audio" << std::endl
            << "  --help/-h         :: prints this help menu" << std::endl
            << "  --info/-i         :: prints information about the user's system" << std::endl
            << "  --reset/-r        :: resets game configuration to use default settings" << std::endl;
}



bool PrintSystemInformation()
{
    printf("\n===== System Information\n");

    // Initialize SDL and its subsystems and make sure it shutdowns properly on exit
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        std::cerr << "ERROR: Unable to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    atexit(SDL_Quit);

    SDL_version sdl_linked_ver;
    SDL_GetVersion(&sdl_linked_ver);

    printf("SDL version (compiled):  %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
    printf("SDL version (linked):    %d.%d.%d\n", sdl_linked_ver.major, sdl_linked_ver.minor, sdl_linked_ver.patch);

    int32 js_num = SDL_NumJoysticks();
    printf("Number of joysticks found:  %d\n", js_num);

    // Print out information about each joystick
    for(int32 i = 0; i < js_num; i++) {
        printf("  Joystick #%d\n", i);
        //printf("    Joystick Name: %s\n", SDL_GameControllerNameForIndex(i));
        SDL_Joystick* js_test = SDL_JoystickOpen(i);
        if(js_test == NULL)
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
// 	vt_video::VideoManager = vt_video::VideoEngine::SingletonCreate();
// 	if (vt_video::VideoManager->SingletonInitialize() == false) {
// 		cerr << "ERROR: unable to initialize the VideoManager" << std::endl;
// 		return false;
// 	}
// 	else {
// 		vt_video::VideoManager->DEBUG_PrintInfo();
// 	}
// 	vt_video::VideoEngine::SingletonDestroy();

    // TODO: print the OpenGL version number here

    printf("SDL_ttf version (compiled): %d.%d.%d\n", SDL_TTF_MAJOR_VERSION, SDL_TTF_MINOR_VERSION, SDL_TTF_PATCHLEVEL);
/*
    char video_driver[80];
    SDL_VideoDriverName(video_driver, 80);
    printf("Name of video driver: %s\n", video_driver);

    const SDL_VideoInfo *user_video;
    user_video = SDL_GetVideoInfo(); // Get information about the user's video system
    std::cout << "  Best available video mode" << std::endl
              << "    Creates hardware surfaces: ";
    if(user_video->hw_available == 1)
        std::cout << "yes\n";
    else
        std::cout << "no\n";
    std::cout << "    Has window manager available: ";
    if(user_video->wm_available == 1)
        std::cout << "yes\n";
    else
        std::cout << "no\n";
    std::cout << "    Hardware to hardware blits accelerated: ";
    if(user_video->blit_hw == 1)
        std::cout << "yes\n";
    else
        std::cout << "no\n";
    std::cout << "    Hardware to hardware colorkey blits accelerated: ";
    if(user_video->blit_hw_CC == 1)
        std::cout << "yes\n";
    else
        std::cout << "no\n";
    std::cout << "    Hardware to hardware alpha blits accelerated: ";
    if(user_video->blit_hw_A == 1)
        std::cout << "yes\n";
    else
        std::cout << "no\n";
    std::cout << "    Software to hardware blits acceleerated: ";
    if(user_video->blit_sw == 1)
        std::cout << "yes\n";
    else
        std::cout << "no\n";
    std::cout << "    Software to hardware colorkey blits accelerated: ";
    if(user_video->blit_sw_CC == 1)
        std::cout << "yes\n";
    else
        std::cout << "no\n";
    std::cout << "    Software to hardware alpha blits accelerated: ";
    if(user_video->blit_sw_A == 1)
        std::cout << "yes\n";
    else
        std::cout << "no\n";
    std::cout << "    Color fills accelerated: ";
    if(user_video->blit_fill == 1)
        std::cout << "yes\n";
    else
        std::cout << "no\n";
    std::cout << "    Total video memory: " << user_video->video_mem << " kilobytes" << std::endl;
    // std::cout << "    Best pixel format: " << user_video->vfmt << std::endl;
*/
    printf("\n===== Audio Information\n");

    vt_audio::AudioManager = vt_audio::AudioEngine::SingletonCreate();
    if(vt_audio::AudioManager->SingletonInitialize() == false) {
        std::cerr << "ERROR: unable to initialize the AudioManager" << std::endl;
        return false;
    } else {
        vt_audio::AudioManager->DEBUG_PrintInfo();
    }
    vt_audio::AudioEngine::SingletonDestroy();

    printf("\n");

    return true;
} // bool PrintSystemInformation()



bool ResetSettings()
{
    std::cout << "This option is not yet implemented." << std::endl;
    return false;
} // bool ResetSettings()



bool CheckFiles()
{
    std::cout << "This option is not yet implemented." << std::endl;
    return false;
} // bool CheckFiles()



bool EnableDebugging(const std::string &vars)
{
    // A vector of all the debug arguments
    std::vector<std::string> args;
    ParseSecondaryOptions(vars, args);

    // Enable all specified debug variables
    for(uint32 i = 0; i < args.size(); i++) {
        if(args[i] == "all") {
            // This causes every call to SDL_SetError to also print an error message on stderr.
            // NOTE: commented out because apparently SDL_putenv is not yet an available function on some systems
            // SDL_putenv("SDL_DEBUG=1");

            vt_audio::AUDIO_DEBUG                  = true;
            vt_battle::BATTLE_DEBUG                = true;
            vt_boot::BOOT_DEBUG                    = true;
            vt_common::COMMON_DEBUG                = true;
            vt_script::SCRIPT_DEBUG                = true;
            vt_mode_manager::MODE_MANAGER_DEBUG    = true;
            vt_input::INPUT_DEBUG                  = true;
            vt_system::SYSTEM_DEBUG                = true;
            vt_global::GLOBAL_DEBUG                = true;
            vt_map::MAP_DEBUG                      = true;
            vt_menu::MENU_DEBUG                    = true;
            vt_pause::PAUSE_DEBUG                  = true;
            vt_shop::SHOP_DEBUG                    = true;
            vt_utils::UTILS_DEBUG                  = true;
            vt_video::VIDEO_DEBUG                  = true;
        } else if(args[i] == "audio") {
            vt_audio::AUDIO_DEBUG = true;
        } else if(args[i] == "battle") {
            vt_battle::BATTLE_DEBUG = true;
        } else if(args[i] == "boot") {
            vt_boot::BOOT_DEBUG = true;
        } else if(args[i] == "common") {
            vt_common::COMMON_DEBUG = true;
        } else if(args[i] == "data") {
            vt_script::SCRIPT_DEBUG = true;
        } else if(args[i] == "mode_manager") {
            vt_mode_manager::MODE_MANAGER_DEBUG = true;
        } else if(args[i] == "input") {
            vt_input::INPUT_DEBUG = true;
        } else if(args[i] == "system") {
            vt_system::SYSTEM_DEBUG = true;
        } else if(args[i] == "global") {
            vt_global::GLOBAL_DEBUG = true;
        } else if(args[i] == "map") {
            vt_map::MAP_DEBUG = true;
        } else if(args[i] == "menu") {
            vt_menu::MENU_DEBUG = true;
        } else if(args[i] == "pause") {
            vt_pause::PAUSE_DEBUG = true;
        } else if(args[i] == "shop") {
            vt_shop::SHOP_DEBUG = true;
        } else if(args[i] == "utils") {
            vt_utils::UTILS_DEBUG = true;
        } else if(args[i] == "video") {
            vt_video::VIDEO_DEBUG = true;
        } else {
            std::cerr << "ERROR: invalid debug argument: " << args[i] << std::endl;
            return false;
        }
    } // for (uint32 i = 0; i < args.size(); i++)

    return true;
} // bool EnableDebugging(string vars)

} // namespace vt_main
