////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    main.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   initialization code and main game loop.
***
*** The code in this file is the first to execute when the game is started and
*** the last to execute before the game exits. The core engine
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

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/mode_manager.h"
#include "engine/video/video.h"
#include "engine/system.h"

#include "common/global/global.h"
#include "common/gui/gui.h"

#include "modes/boot/boot.h"
#include "main_options.h"

#ifdef __MACH__
#include <unistd.h>
#endif

#include <SDL_image.h>
#include <time.h>

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

//! \brief Namespace which contains all binding functions
namespace hoa_defs
{

/** \brief Contains the binding code which makes the C++ engine available to Lua
*** This method should <b>only be called once</b>. It must be called after the
*** ScriptEngine is initialized, otherwise the application will crash.
**/

void BindEngineCode();
void BindCommonCode();
void BindModeCode();

} // namespace hoa_defs

/** \brief Frees all data allocated by the game by destroying the singleton classes
***
*** \note <b>Do not attempt to call or otherwise reference this function.</b>
*** It is for use in the application's main() function only.
***
*** Deleteing the singleton class objects will free all of the memory that the game uses.
*** This is because all other classes and data structures in the game are managed
*** by these singletons either directly or in directly. For example, BattleMode is a
*** class object that is managed by the ModeEngine class, and thus the GameModeManager
*** destructor will also invoke the BattleMode destructor (as well as the destructors of any
*** other game modes that exist).
**/
void QuitApp()
{
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
    SystemEngine::SingletonDestroy();
    VideoEngine::SingletonDestroy();
    // Do it last since all luabind objects must be freed before closing the lua state.
    ScriptEngine::SingletonDestroy();
} // void QuitApp()

/** \brief Reads in all of the saved game settings and sets values in the according game manager classes
*** \return True if the settings were loaded successfully
**/
bool LoadSettings()
{
    ReadScriptDescriptor settings;
    if(!settings.OpenFile(GetSettingsFilename()))
        return false;

    if (!settings.OpenTable("settings")) {
        PRINT_ERROR << "Couldn't open the 'settings' table in: "
            << settings.GetFilename() << std::endl
            << settings.GetErrorMessages() << std::endl;
        settings.CloseFile();
        return false;
    }

    // Load language settings
    SystemManager->SetLanguage(static_cast<std::string>(settings.ReadString("language")));

    if (!settings.OpenTable("key_settings")) {
        PRINT_ERROR << "Couldn't open the 'key_settings' table in: "
            << settings.GetFilename() << std::endl
            << settings.GetErrorMessages() << std::endl;
            settings.CloseFile();
        return false;
    }

    InputManager->SetUpKey(static_cast<SDLKey>(settings.ReadInt("up")));
    InputManager->SetDownKey(static_cast<SDLKey>(settings.ReadInt("down")));
    InputManager->SetLeftKey(static_cast<SDLKey>(settings.ReadInt("left")));
    InputManager->SetRightKey(static_cast<SDLKey>(settings.ReadInt("right")));
    InputManager->SetConfirmKey(static_cast<SDLKey>(settings.ReadInt("confirm")));
    InputManager->SetCancelKey(static_cast<SDLKey>(settings.ReadInt("cancel")));
    InputManager->SetMenuKey(static_cast<SDLKey>(settings.ReadInt("menu")));
    InputManager->SetPauseKey(static_cast<SDLKey>(settings.ReadInt("pause")));
    settings.CloseTable(); // key_settings

    if (!settings.OpenTable("joystick_settings")) {
        PRINT_ERROR << "Couldn't open the 'joystick_settings' table in: "
            << settings.GetFilename() << std::endl
            << settings.GetErrorMessages() << std::endl;
            settings.CloseFile();
        return false;
    }

    InputManager->SetJoysticksEnabled(!settings.ReadBool("input_disabled"));
    InputManager->SetJoyIndex(static_cast<int32>(settings.ReadInt("index")));
    InputManager->SetConfirmJoy(static_cast<uint8>(settings.ReadInt("confirm")));
    InputManager->SetCancelJoy(static_cast<uint8>(settings.ReadInt("cancel")));
    InputManager->SetMenuJoy(static_cast<uint8>(settings.ReadInt("menu")));
    InputManager->SetPauseJoy(static_cast<uint8>(settings.ReadInt("pause")));

    InputManager->SetQuitJoy(static_cast<uint8>(settings.ReadInt("quit")));
    if(settings.DoesIntExist("x_axis"))
        InputManager->SetXAxisJoy(static_cast<int8>(settings.ReadInt("x_axis")));
    if(settings.DoesIntExist("y_axis"))
        InputManager->SetYAxisJoy(static_cast<int8>(settings.ReadInt("y_axis")));

    if(settings.DoesIntExist("threshold"))
        InputManager->SetThresholdJoy(static_cast<uint16>(settings.ReadInt("threshold")));

    settings.CloseTable(); // joystick_settings

    if (!settings.OpenTable("video_settings")) {
        PRINT_ERROR << "Couldn't open the 'video_settings' table in: "
            << settings.GetFilename() << std::endl
            << settings.GetErrorMessages() << std::endl;
            settings.CloseFile();
        return false;
    }

    // Load video settings
    int32 resx = settings.ReadInt("screen_resx");
    int32 resy = settings.ReadInt("screen_resy");
    VideoManager->SetInitialResolution(resx, resy);
    VideoManager->SetFullscreen(settings.ReadBool("full_screen"));
    // Enforce smooth tiles graphics at first run
    if (settings.DoesBoolExist("smooth_graphics"))
        VideoManager->SetPixelArtSmoothed(settings.ReadBool("smooth_graphics"));
    else
        VideoManager->SetPixelArtSmoothed(true);
    settings.CloseTable(); // video_settings

    // Load Audio settings
    if(AUDIO_ENABLE) {
        if (!settings.OpenTable("audio_settings")) {
            PRINT_ERROR << "Couldn't open the 'audio_settings' table in: "
                << settings.GetFilename() << std::endl
                << settings.GetErrorMessages() << std::endl;
                settings.CloseFile();
            return false;
        }

        AudioManager->SetMusicVolume(static_cast<float>(settings.ReadFloat("music_vol")));
        AudioManager->SetSoundVolume(static_cast<float>(settings.ReadFloat("sound_vol")));

        settings.CloseTable(); // audio_settings
    }
    settings.CloseTable(); // settings

    if(settings.IsErrorDetected()) {
        PRINT_ERROR << "Errors while attempting to load the setting file: "
            << settings.GetFilename() << std::endl
            << settings.GetErrorMessages() << std::endl;
        return false;
    }

    settings.CloseFile();

    return true;
} // bool LoadSettings()

//! Loads all the fonts available in the game.
//! And sets a default one
//! The function will exit the game if no valid font were loaded
//! or if the default font is invalid.
static void LoadFonts(const std::string &font_script_filename)
{
    hoa_script::ReadScriptDescriptor font_script;

    //Checking the file existence and validity.
    if(!font_script.OpenFile(font_script_filename)) {
        PRINT_ERROR << "Couldn't open font file: " << font_script_filename
                    << std::endl;
        exit(EXIT_FAILURE);
    }

    if(!font_script.DoesTableExist("fonts")) {
        PRINT_ERROR << "No 'fonts' table in file: " << font_script_filename
                    << std::endl;
        font_script.CloseFile();
        exit(EXIT_FAILURE);
    }

    std::string font_default = font_script.ReadString("font_default");
    if(font_default.empty()) {
        PRINT_ERROR << "No default font defined in: " << font_script_filename
                    << std::endl;
        font_script.CloseFile();
        exit(EXIT_FAILURE);
    }

    std::vector<std::string> style_names;
    font_script.ReadTableKeys("fonts", style_names);
    if(style_names.empty()) {
        PRINT_ERROR << "No text styles defined in the 'fonts' table of file: "
                    << font_script_filename << std::endl;
        font_script.CloseFile();
        exit(EXIT_FAILURE);
    }

    font_script.OpenTable("fonts");
    for(uint32 i = 0; i < style_names.size(); ++i) {
        font_script.OpenTable(style_names[i]); // Text style

        std::string font_file = font_script.ReadString("font");
        uint32 font_size = font_script.ReadInt("size");

        if(!VideoManager->Text()->LoadFont(font_file, style_names[i], font_size)) {
            // Check whether the default font is invalid
            if(font_default == style_names[i]) {
                font_script.CloseAllTables();
                font_script.CloseFile();
                PRINT_ERROR << "The default font '" << font_default
                            << "' couldn't be loaded in file: " << font_script_filename
                            << std::endl;
                exit(EXIT_FAILURE);
                return; // Superfluous but for readability.
            }
        }

        font_script.CloseTable(); // Text style
    }
    font_script.CloseTable(); // fonts

    font_script.CloseFile();

    // Setup the default font
    VideoManager->Text()->SetDefaultStyle(TextStyle(font_default, Color::white,
                                          VIDEO_TEXT_SHADOW_BLACK, 1, -2));
}

//! Loads the default window GUI theme for the game.
//! TODO: Make this changeable from the boot menu
//! and handle keeping the them in memory through config
static void LoadGUIThemes(const std::string& theme_script_filename)
{
    hoa_script::ReadScriptDescriptor theme_script;

    // Checking the file existence and validity.
    if(!theme_script.OpenFile(theme_script_filename)) {
        PRINT_ERROR << "Couldn't open theme file: " << theme_script_filename
                    << std::endl;
        exit(EXIT_FAILURE);
    }

    if(!theme_script.DoesTableExist("themes")) {
        PRINT_ERROR << "No 'themes' table in file: " << theme_script_filename
                    << std::endl;
        theme_script.CloseFile();
        exit(EXIT_FAILURE);
    }

    std::string default_theme = theme_script.ReadString("default_theme");
    if (default_theme.empty()) {
        PRINT_ERROR << "No default theme defined in: " << theme_script_filename
                    << std::endl;
        theme_script.CloseFile();
        exit(EXIT_FAILURE);
    }

    std::vector<std::string> theme_names;
    theme_script.ReadTableKeys("themes", theme_names);
    if (theme_names.empty()) {
        PRINT_ERROR << "No themes defined in the 'themes' table of file: "
                    << theme_script_filename << std::endl;
        theme_script.CloseFile();
        exit(EXIT_FAILURE);
    }

    theme_script.OpenTable("themes");

    bool default_theme_found = false;

    for(uint32 i = 0; i < theme_names.size(); ++i) {
        theme_script.OpenTable(theme_names[i]); // Theme name

        std::string win_border_file = theme_script.ReadString("win_border_file");
        std::string win_background_file = theme_script.ReadString("win_background_file");
        std::string cursor_file = theme_script.ReadString("cursor_file");

        if(!GUIManager->LoadMenuSkin(theme_names[i], win_border_file, win_background_file)) {
            // Check whether the default font is invalid
            if(default_theme == theme_names[i]) {
                theme_script.CloseAllTables();
                theme_script.CloseFile();
                PRINT_ERROR << "The default theme '" << default_theme
                            << "' couldn't be loaded in file: '" << theme_script_filename
                            << "'. Exitting." << std::endl;
                exit(EXIT_FAILURE);
                return; // Superfluous but for readability.
            }
        }

        if(default_theme == theme_names[i]) {
            default_theme_found = true;
            if(!VideoManager->SetDefaultCursor(cursor_file)) {
                theme_script.CloseAllTables();
                theme_script.CloseFile();
                PRINT_ERROR << "Couldn't load the GUI cursor file: '" << cursor_file
                    << "'. Exitting." << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        theme_script.CloseTable(); // Theme name
    }

    theme_script.CloseFile();

    if (!default_theme_found) {
        PRINT_ERROR << "Couldn't find the default theme: '" << default_theme
            << "' in file: '" << theme_script_filename << "'. Exitting." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Activate the default theme
    // TODO: Obtain it from config
    GUIManager->SetDefaultMenuSkin(default_theme);
}

/** \brief Initializes all engine components and makes other preparations for the game to start
*** \return True if the game engine was initialized successfully, false if an unrecoverable error occured
**/
void InitializeEngine() throw(Exception)
{
    // Initialize SDL. The video, audio, and joystick subsystems are initialized elsewhere.
    if(SDL_Init(SDL_INIT_TIMER) != 0) {
        throw Exception("MAIN ERROR: Unable to initialize SDL: ", __FILE__, __LINE__, __FUNCTION__);
    }

    // Create and initialize singleton class managers
    AudioManager = AudioEngine::SingletonCreate();
    InputManager = InputEngine::SingletonCreate();
    ScriptManager = ScriptEngine::SingletonCreate();
    VideoManager = VideoEngine::SingletonCreate();
    SystemManager = SystemEngine::SingletonCreate();
    ModeManager = ModeEngine::SingletonCreate();
    GUIManager = GUISystem::SingletonCreate();
    GlobalManager = GameGlobal::SingletonCreate();

    if(VideoManager->SingletonInitialize() == false) {
        throw Exception("ERROR: unable to initialize VideoManager", __FILE__, __LINE__, __FUNCTION__);
    }

    if(AudioManager->SingletonInitialize() == false) {
        throw Exception("ERROR: unable to initialize AudioManager", __FILE__, __LINE__, __FUNCTION__);
    }

    if(ScriptManager->SingletonInitialize() == false) {
        throw Exception("ERROR: unable to initialize ScriptManager", __FILE__, __LINE__, __FUNCTION__);
    }

    hoa_defs::BindEngineCode();
    hoa_defs::BindCommonCode();
    hoa_defs::BindModeCode();

    if(SystemManager->SingletonInitialize() == false) {
        throw Exception("ERROR: unable to initialize SystemManager", __FILE__, __LINE__, __FUNCTION__);
    }
    if(InputManager->SingletonInitialize() == false) {
        throw Exception("ERROR: unable to initialize InputManager", __FILE__, __LINE__, __FUNCTION__);
    }
    if(ModeManager->SingletonInitialize() == false) {
        throw Exception("ERROR: unable to initialize ModeManager", __FILE__, __LINE__, __FUNCTION__);
    }

    // Set the window icon
    // NOTE: Seems to be working only under WinXP for now.
    SDL_WM_SetIcon(IMG_Load("img/logos/program_icon.png"), NULL);

    // Load all the settings from lua. This includes some engine configuration settings.
    if(!LoadSettings())
        throw Exception("ERROR: Unable to load settings file", __FILE__, __LINE__, __FUNCTION__);

    // Apply engine configuration settings with delayed initialization calls to the managers
    InputManager->InitializeJoysticks();

    if(VideoManager->ApplySettings() == false)
        throw Exception("ERROR: Unable to apply video settings", __FILE__, __LINE__, __FUNCTION__);
    if(VideoManager->FinalizeInitialization() == false)
        throw Exception("ERROR: Unable to apply video settings", __FILE__, __LINE__, __FUNCTION__);

    // Loads the default GUI skin
    LoadGUIThemes("dat/config/themes.lua");

    // NOTE: This function call should have its argument set to false for release builds
    GUIManager->DEBUG_EnableGUIOutlines(false);

    // Loads all game fonts
    LoadFonts("dat/config/fonts.lua");

    // Loads potential emotes
    GlobalManager->LoadEmotes("dat/effects/emotes.lua");

    // Set the window title and icon name
    SDL_WM_SetCaption(APPFULLNAME, APPFULLNAME);

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

    if(GUIManager->SingletonInitialize() == false) {
        throw Exception("ERROR: unable to initialize GUIManager", __FILE__, __LINE__, __FUNCTION__);
    }

    // This loads the game global script, once everything is ready,
    // and will permit to load skills, items and other translatable strings
    // using the correct settings language.
    if(!GlobalManager->SingletonInitialize())
        throw Exception("ERROR: unable to initialize GlobalManager", __FILE__, __LINE__, __FUNCTION__);

    SystemManager->InitializeTimers();
} // void InitializeEngine()


// Every great game begins with a single function :)
int main(int argc, char *argv[])
{
    // When the program exits, the QuitApp() function will be called first, followed by SDL_Quit()
    atexit(SDL_Quit);
    atexit(QuitApp);

    try {
        // Change to the directory where the game data is stored
#ifdef __MACH__
        string path;
        path = argv[0];
        // Remove the binary name
        path.erase(path.find_last_of('/'));
        // Remove the MacOS directory
        path.erase(path.find_last_of('/'));
        // Now the program should be in app/Contents
        path.append("/Resources/");
        chdir(path.c_str());
#elif (defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)) && !defined(RELEASE_BUILD)
        // Look for data files in DATADIR only if they are not available in the current directory.
        if(std::ifstream("dat/config/settings.lua") == NULL) {
            if(chdir(PKG_DATADIR) != 0) {
                throw Exception("ERROR: failed to change directory to data location", __FILE__, __LINE__, __FUNCTION__);
            }
        }
#endif

        // Initialize the random number generator (note: 'unsigned int' is a required usage in this case)
        srand(static_cast<unsigned int>(time(NULL)));

        // This variable will be set by the ParseProgramOptions function
        int32 return_code = EXIT_FAILURE;

        // Parse command lines and exit out of the game if needed
        if(hoa_main::ParseProgramOptions(return_code, static_cast<int32>(argc), argv) == false) {
            return static_cast<int>(return_code);
        }

        // Function call below throws exceptions if any errors occur
        InitializeEngine();

    } catch(const Exception &e) {
#ifdef WIN32
        MessageBox(NULL, e.ToString().c_str(), "Unhandled exception", MB_OK | MB_ICONERROR);
#else
        std::cerr << e.ToString() << std::endl;
#endif
        return EXIT_FAILURE;
    }

    ModeManager->Push(new BootMode(), false, true);

    try {
        // This is the main loop for the game. The loop iterates once for every frame drawn to the screen.
        while(SystemManager->NotDone()) {
            // Update only every 10 milliseconds.
            SDL_Delay(10);

            // 1) Render the scene
            VideoManager->Clear();
            ModeManager->Draw();
            VideoManager->Draw();
            ModeManager->DrawEffects();
            ModeManager->DrawPostEffects();
            VideoManager->DrawFadeEffect();
            // Swap the buffers once the draw operations are done.
            SDL_GL_SwapBuffers();

            // Update timers for correct time-based movement operation
            SystemManager->UpdateTimers();

            // Process all new events
            InputManager->EventHandler();

            // Update video
            VideoManager->Update();

            // Update any streaming audio sources
            AudioManager->Update();

            // Update the game status
            ModeManager->Update();

        } // while (SystemManager->NotDone())
    } catch(const Exception &e) {
#ifdef WIN32
        MessageBox(NULL, e.ToString().c_str(), "Unhandled exception", MB_OK | MB_ICONERROR);
#else
        std::cerr << e.ToString() << std::endl;
#endif
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
} // int main(int argc, char *argv[])
