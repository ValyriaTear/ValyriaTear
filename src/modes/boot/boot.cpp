///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    boot.cpp
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \brief   Source file for boot mode interface.
*** ***************************************************************************/

#include "modes/boot/boot.h"

#include "engine/audio/audio.h"
#include "engine/script/script_write.h"
#include "engine/input.h"
#include "engine/system.h"

#include "common/global/global.h"

#include "modes/map/map_mode.h"
#include "modes/save/save_mode.h"

#include "modes/mode_help_window.h"

#ifdef DEBUG_FEATURES
// Files below are used for boot mode to do a test launch of other modes
#include "modes/battle/battle.h"
#include "modes/menu/menu.h"
#include "modes/shop/shop.h"
#endif

#include <iostream>
#include <sstream>

using namespace vt_utils;

using namespace vt_audio;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_input;
using namespace vt_mode_manager;
using namespace vt_script;
using namespace vt_system;

using namespace vt_global;

using namespace vt_boot::private_boot;

namespace vt_boot
{

bool BOOT_DEBUG = false;

BootMode *BootMode::_current_instance = NULL;

const std::string _LANGUAGE_FILE = "dat/config/languages.lua";

// ****************************************************************************
// ***** BootMode public methods
// ****************************************************************************

BootMode::BootMode() :
    _boot_state(BOOT_STATE_INTRO),
    _exiting_to_new_game(false),
    _has_modified_settings(false),
    _first_run(false),
    _key_setting_function(NULL),
    _joy_setting_function(NULL),
    _joy_axis_setting_function(NULL),
    _message_window(ustring(), 210.0f, 733.0f),
    _menu_bar_alpha(0.0f),
    _help_text_alpha(0.0f)
{
    // Remove potential previous ambient overlays
    VideoManager->DisableFadeEffect();

    IF_PRINT_DEBUG(BOOT_DEBUG) << "BootMode constructor invoked" << std::endl;
    mode_type = MODE_MANAGER_BOOT_MODE;

    // Note: Not translated on purpose.
    _version_text.SetStyle(TextStyle("text18"));
    std::string version_string = "Development Release - ";
    version_string.append(__DATE__);
    _version_text.SetText(MakeUnicodeString(version_string));

    // Get rid of the old table to make sure no old data is used.
    ScriptManager->DropGlobalTable("boot");

    // Test the existence and validity of the boot script.
    ReadScriptDescriptor boot_script;
    if(!boot_script.OpenFile("dat/config/boot.lua")) {
        PRINT_ERROR << "Failed to load boot data file" << std::endl;
        SystemManager->ExitGame();
        return;
    }

    // Open the boot table spacename
    if(boot_script.OpenTablespace().empty()) {
        PRINT_ERROR << "The boot script file has not set a correct tablespace" << std::endl;
        SystemManager->ExitGame();
        return;
    }
    boot_script.CloseTable(); // The namespace
    boot_script.CloseFile();

    // Trigger the Initialize functions in the scene script component
    GetScriptSupervisor().AddScript("dat/config/boot.lua");
    GetScriptSupervisor().Initialize(this);

    _options_window.Create(300.0f, 550.0f);
    _options_window.SetPosition(360.0f, 188.0f);
    _options_window.SetDisplayMode(VIDEO_MENU_INSTANT);
    _options_window.Hide();

    // Setup all boot menu options and properties
    _SetupMainMenu();
    _SetupOptionsMenu();
    _SetupVideoOptionsMenu();
    _SetupAudioOptionsMenu();
    _SetupLanguageOptionsMenu();
    _SetupKeySettingsMenu();
    _SetupJoySettingsMenu();
    _SetupResolutionMenu();
    _active_menu = &_main_menu;

    // make sure message window is not visible
    _message_window.Hide();

    // Load the menu bar and the help text
    _menu_bar.Load("img/menus/battle_bottom_menu.png", 1024, 128);

    _f1_help_text.SetStyle(TextStyle("text18"));

    // The timer that will be used to display the menu bar and the help text
    _boot_timer.Initialize(14000);
    _boot_timer.EnableManualUpdate();
    _boot_timer.Run();

    // Preload new game sound
    AudioManager->LoadSound("snd/new_game.wav", this);
} // BootMode::BootMode()



BootMode::~BootMode()
{
    _options_window.Destroy();
    _SaveSettingsFile();

    IF_PRINT_WARNING(BOOT_DEBUG)
            << "BOOT: BootMode destructor invoked." << std::endl;

    _key_setting_function = NULL;
    _joy_setting_function = NULL;
    _joy_axis_setting_function = NULL;
}

void BootMode::Reset()
{
    // Set the coordinate system that BootMode uses
    VideoManager->SetStandardCoordSys();
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);

    GlobalManager->ClearAllData(); // Resets the game universe to a NULL state
    _current_instance = this;

    GetScriptSupervisor().Reset();
}



void BootMode::Update()
{
    _options_window.Update(SystemManager->GetUpdateTime());

    // Update the game mode generic members.
    GameMode::Update();

    if(_exiting_to_new_game) {
        // When the dae out is done, we start a new game.
        if(!VideoManager->IsFading())
            GlobalManager->NewGame();
        return;
    }

    // The intro is being played
    if(_boot_state == BOOT_STATE_INTRO) {
        if(InputManager->AnyKeyPress()) {
            ChangeState(BOOT_STATE_MENU);
            return;
        } else {
            return; // Otherwise skip rest of the event handling for now
        }
    }

    // Test whether the welcome sequence should be shown once
    static bool language_selection_shown = false;
    if(!language_selection_shown) {
        _ShowLanguageSelectionWindow();
        language_selection_shown = true;
    }

    // On first app run, show the language menu and apply language on any key press.
    if (_first_run && _active_menu == &_language_options_menu) {
        _active_menu->Update();
        if (InputManager->UpPress()) {
            _active_menu->InputUp();
        }
        else if (InputManager->DownPress()) {
            _active_menu->InputDown();
        }
        else if (InputManager->AnyKeyPress()) {
            // Set the language
            _active_menu->InputConfirm();
            // Go directly back to the main menu when first selecting the language.
            _options_window.Hide();
            _active_menu = &_main_menu;
            // And show the help window
            ModeManager->GetHelpWindow()->Show();
            // save the settings (automatically changes the first_start variable to 0)
            _has_modified_settings = true;
            _SaveSettingsFile();
            _first_run = false; // Terminate the first run sequence
        }
        return;
    }

    HelpWindow *help_window = ModeManager->GetHelpWindow();
    if(help_window && help_window->IsActive()) {
        // Any key, except F1
        if(!InputManager->HelpPress() && InputManager->AnyKeyPress()) {
            GlobalManager->Media().PlaySound("confirm");
            help_window->Hide();
        }
        return;
    }

    // Check for waiting keypresses or joystick button presses
    SDL_Event ev = InputManager->GetMostRecentEvent();
    if(_joy_setting_function != NULL) {
        if(InputManager->AnyKeyPress() && ev.type == SDL_JOYBUTTONDOWN) {
            (this->*_joy_setting_function)(InputManager->GetMostRecentEvent().jbutton.button);
            _joy_setting_function = NULL;
            _has_modified_settings = true;
            _RefreshJoySettings();
            _message_window.Hide();
        }
        if(InputManager->CancelPress()) {
            _joy_setting_function = NULL;
            _message_window.Hide();
        }
        return;
    }

    if(_joy_axis_setting_function != NULL) {
        int8 x = InputManager->GetLastAxisMoved();
        if(x != -1) {
            (this->*_joy_axis_setting_function)(x);
            _joy_axis_setting_function = NULL;
            _has_modified_settings = true;
            _RefreshJoySettings();
            _message_window.Hide();
        }
        if(InputManager->CancelPress()) {
            _joy_axis_setting_function = NULL;
            _message_window.Hide();
        }
        return;
    }

    if(_key_setting_function != NULL) {
        if(InputManager->AnyKeyPress() && ev.type == SDL_KEYDOWN) {
            (this->*_key_setting_function)(InputManager->GetMostRecentEvent().key.keysym.sym);
            _key_setting_function = NULL;
            _has_modified_settings = true;
            _RefreshKeySettings();
            _message_window.Hide();
        }
        if(InputManager->CancelPress()) {
            _key_setting_function = NULL;
            _message_window.Hide();
        }
        return;
    }

    _active_menu->Update();

    // Update also the bar and f1 help text alpha
    uint32 time_expired = SystemManager->GetUpdateTime();
    _boot_timer.Update(time_expired);
    if (_boot_timer.GetTimeExpired() >= 4000.0 && _boot_timer.GetTimeExpired() < 12000.0) {
        _help_text_alpha += 0.001f * time_expired;
        if (_help_text_alpha > 1.0f)
            _help_text_alpha = 1.0f;
    }
    else if (_boot_timer.GetTimeExpired() >= 12000.0 && _boot_timer.GetTimeExpired() < 14000.0) {
        _help_text_alpha -= 0.001f * time_expired;
        if (_help_text_alpha < 0.0f)
            _help_text_alpha = 0.0f;
    }

    if (_menu_bar_alpha < 0.6f) {
        _menu_bar_alpha = _menu_bar_alpha + 0.001f * time_expired;
        if (_menu_bar_alpha >= 0.6f)
            _menu_bar_alpha = 0.6f;
    }

    // Only quit when we are at the main menu level
    if(_active_menu == &_main_menu && InputManager->QuitPress()) {
        SystemManager->ExitGame();
        return;
    }

    if(InputManager->ConfirmPress()) {
        // Play 'confirm sound' if the selection isn't grayed out and it has a confirm handler
        if(_active_menu->IsOptionEnabled(_active_menu->GetSelection())) {
            // Don't play the sound on New Games as they have their own sound
            if(_active_menu != &_main_menu && _active_menu->GetSelection() != -1)
                GlobalManager->Media().PlaySound("confirm");
        } else {
            // Otherwise play a different sound
            GlobalManager->Media().PlaySound("bump");
        }

        _active_menu->InputConfirm();

    } else if(InputManager->LeftPress()) {
        _active_menu->InputLeft();
    } else if(InputManager->RightPress()) {
        _active_menu->InputRight();
    } else if(InputManager->UpPress()) {
        _active_menu->InputUp();
    } else if(InputManager->DownPress()) {
        _active_menu->InputDown();
    } else if(InputManager->CancelPress() || InputManager->QuitPress()) {
        if(_active_menu == &_main_menu) {

        } else if(_active_menu == &_options_menu) {
            _options_window.Hide();
            _active_menu = &_main_menu;
        } else if(_active_menu == &_video_options_menu) {
            _active_menu = &_options_menu;
        } else if(_active_menu == &_audio_options_menu) {
            _active_menu = &_options_menu;
        } else if(_active_menu == &_language_options_menu) {
            _active_menu = &_options_menu;
        } else if(_active_menu == &_key_settings_menu) {
            _active_menu = &_options_menu;
        } else if(_active_menu == &_joy_settings_menu) {
            _active_menu = &_options_menu;
        } else if(_active_menu == &_resolution_menu) {
            _active_menu = &_video_options_menu;
        }

        // Play cancel sound
        GlobalManager->Media().PlaySound("cancel");
    }
} // void BootMode::Update()



void BootMode::Draw()
{
    VideoManager->PushState();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
    VideoManager->SetStandardCoordSys();

    GetScriptSupervisor().DrawBackground();
    GetScriptSupervisor().DrawForeground();
    VideoManager->PopState();
}

void BootMode::DrawPostEffects()
{
    VideoManager->PushState();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
    VideoManager->SetStandardCoordSys();

    GetScriptSupervisor().DrawPostEffects();

    if(_boot_state == BOOT_STATE_MENU) {
        if (!_first_run) {
            VideoManager->Move(0.0f, 640.0f);
            _menu_bar.Draw(Color(1.0f, 1.0f, 1.0f, _menu_bar_alpha));
        }

        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        VideoManager->Move(322.0f, 745.0f);
        _f1_help_text.Draw(Color(1.0f, 1.0f, 1.0f, _help_text_alpha));

        VideoManager->Move(10.0f, 758.0f);
        _version_text.Draw();

        _options_window.Draw();
        if(_active_menu)
            _active_menu->Draw();

        VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, 0);
        VideoManager->Move(0.0f, 0.0f);
        _message_window.Draw();
    }
    VideoManager->PopState();
}

// ****************************************************************************
// ***** BootMode menu setup and refresh methods
// ****************************************************************************
bool BootMode::_SavesAvailable(int32 maxId)
{
    assert(maxId > 0);
    int32 savesAvailable = 0;
    std::string data_path = GetUserDataPath();
    for(int id = 0; id < maxId; ++id) {
        std::ostringstream f;
        f << data_path + "saved_game_" << id << ".lua";
        const std::string filename = f.str();

        if(DoesFileExist(filename)) {
            ++savesAvailable;
        }
    }
    return (savesAvailable > 0);
}


void BootMode::_ReloadTranslatableMenus()
{
    _SetupMainMenu();
    _SetupOptionsMenu();
    _SetupVideoOptionsMenu();
    _SetupAudioOptionsMenu();
    _SetupKeySettingsMenu();
    _SetupJoySettingsMenu();
    _SetupResolutionMenu();
}


void BootMode::_SetupMainMenu()
{
    _main_menu.ClearOptions();
    _main_menu.SetPosition(512.0f, 688.0f);
    _main_menu.SetTextStyle(TextStyle("title24"));
    _main_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _main_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _main_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _main_menu.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _main_menu.SetCursorOffset(-50.0f, -28.0f);
    _main_menu.SetSkipDisabled(true);

    _main_menu.AddOption(UTranslate("New Game"), &BootMode::_OnNewGame);
    _main_menu.AddOption(UTranslate("Load Game"), &BootMode::_OnLoadGame);
    _main_menu.AddOption(UTranslate("Options"), &BootMode::_OnOptions);

    // Insert the debug options
#ifdef DEBUG_FEATURES
    _main_menu.SetDimensions(1000.0f, 50.0f, 7, 1, 7, 1);
    _main_menu.AddOption(UTranslate("Battle"), &BootMode::_DEBUG_OnBattle);
    _main_menu.AddOption(UTranslate("Menu"), &BootMode::_DEBUG_OnMenu);
    _main_menu.AddOption(UTranslate("Shop"), &BootMode::_DEBUG_OnShop);
#else
    _main_menu.SetDimensions(800.0f, 50.0f, 4, 1, 4, 1);
#endif
    _main_menu.AddOption(UTranslate("Quit"), &BootMode::_OnQuit);


    if(!_SavesAvailable()) {
        _main_menu.EnableOption(1, false);
        _main_menu.SetSelection(0);
    } else {
        _main_menu.SetSelection(1);
    }

    _f1_help_text.SetText(VTranslate("Press '%s' to get to know about the game keys.",
                                     InputManager->GetHelpKeyName()));
}


void BootMode::_SetupOptionsMenu()
{
    _options_menu.ClearOptions();
    _options_menu.SetPosition(512.0f, 468.0f);
    _options_menu.SetDimensions(300.0f, 600.0f, 1, 5, 1, 5);
    _options_menu.SetTextStyle(TextStyle("title22"));
    _options_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _options_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _options_menu.SetCursorOffset(-50.0f, -28.0f);
    _options_menu.SetSkipDisabled(true);

    _options_menu.AddOption(UTranslate("Video"), &BootMode::_OnVideoOptions);
    _options_menu.AddOption(UTranslate("Audio"), &BootMode::_OnAudioOptions);
    _options_menu.AddOption(UTranslate("Language"), &BootMode::_OnLanguageOptions);
    _options_menu.AddOption(UTranslate("Key Settings"), &BootMode::_OnKeySettings);
    _options_menu.AddOption(UTranslate("Joystick Settings"), &BootMode::_OnJoySettings);

    _options_menu.SetSelection(0);
}


void BootMode::_SetupVideoOptionsMenu()
{
    _video_options_menu.ClearOptions();
    _video_options_menu.SetPosition(512.0f, 468.0f);
    _video_options_menu.SetDimensions(300.0f, 400.0f, 1, 4, 1, 4);
    _video_options_menu.SetTextStyle(TextStyle("title22"));
    _video_options_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _video_options_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _video_options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _video_options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _video_options_menu.SetCursorOffset(-50.0f, -28.0f);
    _video_options_menu.SetSkipDisabled(true);

    _video_options_menu.AddOption(UTranslate("Resolution:"), &BootMode::_OnResolution);
    // Left & right will change window mode as well as confirm
    _video_options_menu.AddOption(UTranslate("Window mode:"), &BootMode::_OnToggleFullscreen, NULL, NULL, &BootMode::_OnToggleFullscreen, &BootMode::_OnToggleFullscreen);
    _video_options_menu.AddOption(UTranslate("Brightness:"), NULL, NULL, NULL, &BootMode::_OnBrightnessLeft, &BootMode::_OnBrightnessRight);
    _video_options_menu.AddOption(UTranslate("Map tiles: "), &BootMode::_OnTogglePixelArtSmoothed, NULL, NULL, &BootMode::_OnTogglePixelArtSmoothed, &BootMode::_OnTogglePixelArtSmoothed);

    _video_options_menu.SetSelection(0);
}


void BootMode::_SetupAudioOptionsMenu()
{
    _audio_options_menu.ClearOptions();
    _audio_options_menu.SetPosition(512.0f, 468.0f);
    _audio_options_menu.SetDimensions(300.0f, 200.0f, 1, 2, 1, 2);
    _audio_options_menu.SetTextStyle(TextStyle("title22"));
    _audio_options_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _audio_options_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _audio_options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _audio_options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _audio_options_menu.SetCursorOffset(-50.0f, -28.0f);
    _audio_options_menu.SetSkipDisabled(true);

    _audio_options_menu.AddOption(UTranslate("Sound Volume: "), NULL, NULL, NULL, &BootMode::_OnSoundLeft, &BootMode::_OnSoundRight);
    _audio_options_menu.AddOption(UTranslate("Music Volume: "), NULL, NULL, NULL, &BootMode::_OnMusicLeft, &BootMode::_OnMusicRight);

    _audio_options_menu.SetSelection(0);
}


void BootMode::_SetupLanguageOptionsMenu()
{
    _language_options_menu.SetPosition(512.0f, 468.0f);
    _language_options_menu.SetTextStyle(TextStyle("title22"));
    _language_options_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _language_options_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _language_options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _language_options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _language_options_menu.SetCursorOffset(-50.0f, -28.0f);
    _language_options_menu.SetSkipDisabled(true);


    // Get the list of languages from the Lua file.
    ReadScriptDescriptor read_data;
    if(!read_data.OpenFile(_LANGUAGE_FILE)) {
        PRINT_ERROR << "Failed to load language file: " << _LANGUAGE_FILE << std::endl;
        PRINT_ERROR << "The language list will be empty." << std::endl;
        return;
    }

    read_data.OpenTable("languages");
    uint32 table_size = read_data.GetTableSize();

    // Set up the dimensions of the window according to how many languages are available.
    _language_options_menu.SetDimensions(300.0f, 200.0f, 1, table_size, 1, table_size);

    _po_files.clear();
    for(uint32 i = 1; i <= table_size; i++) {
        read_data.OpenTable(i);
        _po_files.push_back(read_data.ReadString(2));
        _language_options_menu.AddOption(MakeUnicodeString(read_data.ReadString(1)),
                                         &BootMode::_OnLanguageSelect);
        read_data.CloseTable();
    }

    read_data.CloseTable();
    if(read_data.IsErrorDetected())
        PRINT_ERROR << "Error occurred while loading language list: " << read_data.GetErrorMessages() << std::endl;
    read_data.CloseFile();
}


void BootMode::_SetupKeySettingsMenu()
{
    _key_settings_menu.ClearOptions();
    _key_settings_menu.SetPosition(512.0f, 468.0f);
    _key_settings_menu.SetDimensions(250.0f, 500.0f, 1, 8, 1, 8);
    _key_settings_menu.SetTextStyle(TextStyle("title22"));
    _key_settings_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _key_settings_menu.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _key_settings_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _key_settings_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _key_settings_menu.SetCursorOffset(-50.0f, -28.0f);
    _key_settings_menu.SetSkipDisabled(true);

    _key_settings_menu.AddOption(UTranslate("Up: "), &BootMode::_RedefineUpKey);
    _key_settings_menu.AddOption(UTranslate("Down: "), &BootMode::_RedefineDownKey);
    _key_settings_menu.AddOption(UTranslate("Left: "), &BootMode::_RedefineLeftKey);
    _key_settings_menu.AddOption(UTranslate("Right: "), &BootMode::_RedefineRightKey);
    _key_settings_menu.AddOption(UTranslate("Confirm: "), &BootMode::_RedefineConfirmKey);
    _key_settings_menu.AddOption(UTranslate("Cancel: "), &BootMode::_RedefineCancelKey);
    _key_settings_menu.AddOption(UTranslate("Menu: "), &BootMode::_RedefineMenuKey);
    _key_settings_menu.AddOption(UTranslate("Pause: "), &BootMode::_RedefinePauseKey);
    _key_settings_menu.AddOption(UTranslate("Restore defaults"), &BootMode::_OnRestoreDefaultKeys);
}


void BootMode::_SetupJoySettingsMenu()
{
    _joy_settings_menu.ClearOptions();
    _joy_settings_menu.SetPosition(512.0f, 468.0f);
    _joy_settings_menu.SetDimensions(250.0f, 500.0f, 1, 9, 1, 9);
    _joy_settings_menu.SetTextStyle(TextStyle("title22"));
    _joy_settings_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _joy_settings_menu.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _joy_settings_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _joy_settings_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _joy_settings_menu.SetCursorOffset(-50.0f, -28.0f);
    _joy_settings_menu.SetSkipDisabled(true);

    ustring dummy;
    _joy_settings_menu.AddOption(dummy, &BootMode::_OnToggleJoystickEnabled, NULL, NULL,
                                 &BootMode::_OnToggleJoystickEnabled, &BootMode::_OnToggleJoystickEnabled);
    _joy_settings_menu.AddOption(dummy, &BootMode::_RedefineXAxisJoy);
    _joy_settings_menu.AddOption(dummy, &BootMode::_RedefineYAxisJoy);
    _joy_settings_menu.AddOption(dummy, NULL, NULL, NULL, &BootMode::_OnThresholdJoyLeft, &BootMode::_OnThresholdJoyRight);

    _joy_settings_menu.AddOption(dummy, &BootMode::_RedefineConfirmJoy);
    _joy_settings_menu.AddOption(dummy, &BootMode::_RedefineCancelJoy);
    _joy_settings_menu.AddOption(dummy, &BootMode::_RedefineMenuJoy);
    _joy_settings_menu.AddOption(dummy, &BootMode::_RedefinePauseJoy);
    _joy_settings_menu.AddOption(dummy, &BootMode::_RedefineQuitJoy);

    _joy_settings_menu.AddOption(UTranslate("Restore defaults"), &BootMode::_OnRestoreDefaultJoyButtons);
}


void BootMode::_SetupResolutionMenu()
{
    _resolution_menu.SetPosition(512.0f, 468.0f);
    _resolution_menu.SetDimensions(300.0f, 200.0f, 1, 4, 1, 4);
    _resolution_menu.SetTextStyle(TextStyle("title22"));
    _resolution_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _resolution_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _resolution_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _resolution_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _resolution_menu.SetCursorOffset(-50.0f, -28.0f);
    _resolution_menu.SetSkipDisabled(true);

    _resolution_menu.AddOption(MakeUnicodeString("640 x 480"), &BootMode::_OnResolution640x480);
    _resolution_menu.AddOption(MakeUnicodeString("800 x 600"), &BootMode::_OnResolution800x600);
    _resolution_menu.AddOption(MakeUnicodeString("1024 x 768"), &BootMode::_OnResolution1024x768);
    _resolution_menu.AddOption(MakeUnicodeString("1280 x 1024"), &BootMode::_OnResolution1280x1024);

    if(VideoManager->GetScreenWidth() == 640)
        _resolution_menu.SetSelection(0);
    else if(VideoManager->GetScreenWidth() == 800)
        _resolution_menu.SetSelection(1);
    else if(VideoManager->GetScreenWidth() == 1024)
        _resolution_menu.SetSelection(2);
    else if(VideoManager->GetScreenWidth() == 1280)
        _resolution_menu.SetSelection(3);
}

void BootMode::_RefreshVideoOptions()
{
    // Update resolution text
    std::ostringstream resolution("");
    resolution << VideoManager->GetScreenWidth() << " x " << VideoManager->GetScreenHeight();
    _video_options_menu.SetOptionText(0, UTranslate("Resolution: ") + MakeUnicodeString(resolution.str()));

    // Update text on current video mode
    if(VideoManager->IsFullscreen())
        _video_options_menu.SetOptionText(1, UTranslate("Window mode: ") + UTranslate("Fullscreen"));
    else
        _video_options_menu.SetOptionText(1, UTranslate("Window mode: ") + UTranslate("Windowed"));

    // Update brightness
    _video_options_menu.SetOptionText(2, UTranslate("Brightness: ") + MakeUnicodeString(NumberToString(VideoManager->GetGamma() * 50.0f + 0.5f) + " %"));

    // Update the image quality text
    if(VideoManager->ShouldSmoothPixelArt())
        _video_options_menu.SetOptionText(3, UTranslate("Map tiles: ") + UTranslate("Smoothed"));
    else
        _video_options_menu.SetOptionText(3, UTranslate("Map tiles: ") + UTranslate("Normal"));
}



void BootMode::_RefreshAudioOptions()
{
    _audio_options_menu.SetOptionText(0, UTranslate("Sound Volume: ") + MakeUnicodeString(NumberToString(static_cast<int32>(AudioManager->GetSoundVolume() * 100.0f + 0.5f)) + " %"));
    _audio_options_menu.SetOptionText(1, UTranslate("Music Volume: ") + MakeUnicodeString(NumberToString(static_cast<int32>(AudioManager->GetMusicVolume() * 100.0f + 0.5f)) + " %"));
}



void BootMode::_RefreshKeySettings()
{
    // Update key names
    _key_settings_menu.SetOptionText(0, UTranslate("Move Up") + MakeUnicodeString("<r>" + InputManager->GetUpKeyName()));
    _key_settings_menu.SetOptionText(1, UTranslate("Move Down") + MakeUnicodeString("<r>" + InputManager->GetDownKeyName()));
    _key_settings_menu.SetOptionText(2, UTranslate("Move Left") + MakeUnicodeString("<r>" + InputManager->GetLeftKeyName()));
    _key_settings_menu.SetOptionText(3, UTranslate("Move Right") + MakeUnicodeString("<r>" + InputManager->GetRightKeyName()));
    _key_settings_menu.SetOptionText(4, UTranslate("Confirm") + MakeUnicodeString("<r>" + InputManager->GetConfirmKeyName()));
    _key_settings_menu.SetOptionText(5, UTranslate("Cancel") + MakeUnicodeString("<r>" + InputManager->GetCancelKeyName()));
    _key_settings_menu.SetOptionText(6, UTranslate("Menu") + MakeUnicodeString("<r>" + InputManager->GetMenuKeyName()));
    _key_settings_menu.SetOptionText(7, UTranslate("Pause") + MakeUnicodeString("<r>" + InputManager->GetPauseKeyName()));
}



void BootMode::_RefreshJoySettings()
{
    int32 i = 0;
    if(InputManager->GetJoysticksEnabled())
        _joy_settings_menu.SetOptionText(i++, UTranslate("Input enabled: ") + MakeUnicodeString("<r>") +  UTranslate("Yes"));
    else
        _joy_settings_menu.SetOptionText(i++, UTranslate("Input enabled: ") + MakeUnicodeString("<r>") +  UTranslate("No"));

    _joy_settings_menu.SetOptionText(i++, UTranslate("X Axis") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetXAxisJoy())));
    _joy_settings_menu.SetOptionText(i++, UTranslate("Y Axis") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetYAxisJoy())));
    _joy_settings_menu.SetOptionText(i++, UTranslate("Threshold") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetThresholdJoy())));
    _joy_settings_menu.SetOptionText(i++, UTranslate("Confirm: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetConfirmJoy())));
    _joy_settings_menu.SetOptionText(i++, UTranslate("Cancel: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetCancelJoy())));
    _joy_settings_menu.SetOptionText(i++, UTranslate("Menu: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetMenuJoy())));
    _joy_settings_menu.SetOptionText(i++, UTranslate("Pause: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetPauseJoy())));
    _joy_settings_menu.SetOptionText(i++, UTranslate("Quit: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetQuitJoy())));
}

// ****************************************************************************
// ***** BootMode menu handler methods
// ****************************************************************************

void BootMode::_OnNewGame()
{
    AudioManager->StopAllMusic();
    VideoManager->FadeScreen(Color::black, 2000);

    AudioManager->PlaySound("snd/new_game.wav");
    _exiting_to_new_game = true;
}



void BootMode::_OnLoadGame()
{
    vt_save::SaveMode *SVM = new vt_save::SaveMode(false);
    ModeManager->Push(SVM);
}



void BootMode::_OnOptions()
{
    _active_menu = &_options_menu;
    _options_window.Show();
}


void BootMode::_OnQuit()
{
    SystemManager->ExitGame();
}


#ifdef DEBUG_FEATURES
void BootMode::_DEBUG_OnBattle()
{
    ReadScriptDescriptor read_data;
    read_data.RunScriptFunction("dat/debug/debug_battle.lua",
                                "BootBattleTest", true);
}



void BootMode::_DEBUG_OnMenu()
{
    ReadScriptDescriptor read_data;
    read_data.RunScriptFunction("dat/debug/debug_menu.lua",
                                "BootMenuTest", true);
}



void BootMode::_DEBUG_OnShop()
{
    ReadScriptDescriptor read_data;
    read_data.RunScriptFunction("dat/debug/debug_shop.lua",
                                "BootShopTest", true);
}
#endif // #ifdef DEBUG_FEATURES


void BootMode::_OnVideoOptions()
{
    _active_menu = &_video_options_menu;
    _RefreshVideoOptions();
}



void BootMode::_OnAudioOptions()
{
    // Switch the current menu
    _active_menu = &_audio_options_menu;
    _RefreshAudioOptions();
}



void BootMode::_OnLanguageOptions()
{
    // Switch the current menu
    _active_menu = &_language_options_menu;
}



void BootMode::_OnKeySettings()
{
    _active_menu = &_key_settings_menu;
    _RefreshKeySettings();
}



void BootMode::_OnJoySettings()
{
    _active_menu = &_joy_settings_menu;
    _RefreshJoySettings();
}

void BootMode::_OnToggleFullscreen()
{
    // Toggle fullscreen / windowed
    VideoManager->ToggleFullscreen();
    VideoManager->ApplySettings();
    _RefreshVideoOptions();
    _has_modified_settings = true;
}

void BootMode::_OnTogglePixelArtSmoothed()
{
    // Toggle smooth texturing
    VideoManager->SetPixelArtSmoothed(!VideoManager->ShouldSmoothPixelArt());
    VideoManager->ApplySettings();
    _RefreshVideoOptions();
    _has_modified_settings = true;
}


void BootMode::_OnResolution()
{
    _active_menu = &_resolution_menu;
}



void BootMode::_OnResolution640x480()
{
    if(VideoManager->GetScreenWidth() != 640 && VideoManager->GetScreenHeight() != 480)
        _ChangeResolution(640, 480);
}



void BootMode::_OnResolution800x600()
{
    if(VideoManager->GetScreenWidth() != 800 && VideoManager->GetScreenHeight() != 600)
        _ChangeResolution(800, 600);
}



void BootMode::_OnResolution1024x768()
{
    if(VideoManager->GetScreenWidth() != 1024 && VideoManager->GetScreenHeight() != 768)
        _ChangeResolution(1024, 768);
}



void BootMode::_OnResolution1280x1024()
{
    if(VideoManager->GetScreenWidth() != 1280 && VideoManager->GetScreenHeight() != 1024)
        _ChangeResolution(1280, 1024);
}



void BootMode::_OnBrightnessLeft()
{
    VideoManager->SetGamma(VideoManager->GetGamma() - 0.1f);
    _RefreshVideoOptions();
}



void BootMode::_OnBrightnessRight()
{
    VideoManager->SetGamma(VideoManager->GetGamma() + 0.1f);
    _RefreshVideoOptions();
}



void BootMode::_OnSoundLeft()
{
    AudioManager->SetSoundVolume(AudioManager->GetSoundVolume() - 0.1f);
    _RefreshAudioOptions();
    // Play a sound for user to hear new volume level.
    GlobalManager->Media().PlaySound("volume_test");
    _has_modified_settings = true;
}



void BootMode::_OnSoundRight()
{
    AudioManager->SetSoundVolume(AudioManager->GetSoundVolume() + 0.1f);
    _RefreshAudioOptions();
    // Play a sound for user to hear new volume level.
    GlobalManager->Media().PlaySound("volume_test");
    _has_modified_settings = true;
}



void BootMode::_OnMusicLeft()
{
    AudioManager->SetMusicVolume(AudioManager->GetMusicVolume() - 0.1f);
    _RefreshAudioOptions();
    _has_modified_settings = true;
}



void BootMode::_OnMusicRight()
{
    AudioManager->SetMusicVolume(AudioManager->GetMusicVolume() + 0.1f);
    _RefreshAudioOptions();
    _has_modified_settings = true;
}



void BootMode::_OnLanguageSelect()
{
    SystemManager->SetLanguage(_po_files[_language_options_menu.GetSelection()]);
    _has_modified_settings = true;

    // Reload all the translatable text in the boot menus.
    _ReloadTranslatableMenus();

    // Reloads the global scripts to update their inner translatable strings
    GlobalManager->ReloadGlobalScripts();
}



void BootMode::_OnRestoreDefaultKeys()
{
    InputManager->RestoreDefaultKeys();
    _RefreshKeySettings();
    _has_modified_settings = true;
}

void BootMode::_OnToggleJoystickEnabled()
{
    InputManager->SetJoysticksEnabled(!InputManager->GetJoysticksEnabled());
    if (InputManager->GetJoysticksEnabled())
        InputManager->InitializeJoysticks();
    else
        InputManager->DeinitializeJoysticks();

    _RefreshJoySettings();
    _has_modified_settings = true;
}

void BootMode::_OnThresholdJoyLeft()
{
    InputManager->SetThresholdJoy(InputManager->GetThresholdJoy() - 100);
    _RefreshJoySettings();
    _has_modified_settings = true;
}

void BootMode::_OnThresholdJoyRight()
{
    InputManager->SetThresholdJoy(InputManager->GetThresholdJoy() + 100);
    _RefreshJoySettings();
    _has_modified_settings = true;
}

void BootMode::_OnRestoreDefaultJoyButtons()
{
    InputManager->RestoreDefaultJoyButtons();
    _RefreshJoySettings();
    _has_modified_settings = true;
}

// ****************************************************************************
// ***** BootMode helper methods
// ****************************************************************************

void BootMode::_ShowLanguageSelectionWindow()
{
    // Load the settings file for reading in the welcome variable
    ReadScriptDescriptor settings_lua;
    std::string file = GetSettingsFilename();
    if(!settings_lua.OpenFile(file)) {
        PRINT_WARNING << "failed to load the settings file" << std::endl;
        return;
    }

    settings_lua.OpenTable("settings");
    if(settings_lua.ReadInt("first_start") == 1) {
        _first_run = true;
        _options_window.Show();
        _active_menu = &_language_options_menu;
    }
    settings_lua.CloseTable();
    settings_lua.CloseFile();
}

void BootMode::_ShowMessageWindow(bool joystick)
{
    if(joystick)
        _ShowMessageWindow(WAIT_JOY_BUTTON);
    else
        _ShowMessageWindow(WAIT_KEY);
}

void BootMode::_ShowMessageWindow(WAIT_FOR wait)
{
    if(wait == WAIT_JOY_BUTTON)
        _message_window.SetText(UTranslate("Please press a new joystick button."));
    else if(wait == WAIT_KEY)
        _message_window.SetText(UTranslate("Please press a new key."));
    else if(wait == WAIT_JOY_AXIS)
        _message_window.SetText(UTranslate("Please move an axis."));
    else {
        PRINT_WARNING << "Undefined wait value." << std::endl;
        return;
    }

    _message_window.Show();
}

void BootMode::_ChangeResolution(int32 width, int32 height)
{
    VideoManager->SetResolution(width, height);
    VideoManager->ApplySettings();
    _RefreshVideoOptions();
    _has_modified_settings = true;
}

bool BootMode::_SaveSettingsFile(const std::string &filename)
{

    // No need to save the settings if we haven't edited anything!
    if(!_has_modified_settings)
        return false;

    std::string file;
    std::string fileTemp;

    // Load the settings file for reading in the original data
    fileTemp = GetUserConfigPath() + "/settings.lua";

    if(filename.empty())
        file = fileTemp;
    else
        file = GetUserConfigPath() + "/" + filename;

    //copy the default file so we have an already set up lua file and then we can modify its settings
    if(!DoesFileExist(file))
        CopyFile(std::string("dat/config/settings.lua"), file);

    WriteScriptDescriptor settings_lua;
    if(!settings_lua.OpenFile(file)) {
        PRINT_ERROR << "Failed to open settings file: " <<
            file << std::endl;
        return false;
    }

    settings_lua.WriteComment("--General settings--");
    settings_lua.BeginTable("settings");

    // Write the current settings into the .lua file
    settings_lua.WriteComment("Show the first time help window");
    settings_lua.WriteInt("first_start", 0);


    //Save language settings
    settings_lua.WriteComment("The GUI and in game dialogues language used");
    settings_lua.WriteString("language", SystemManager->GetLanguage());

    // video
    settings_lua.InsertNewLine();
    settings_lua.WriteComment("--Video settings--");
    settings_lua.BeginTable("video_settings");
    settings_lua.WriteComment("Screen resolution");
    settings_lua.WriteInt("screen_resx", VideoManager->GetScreenWidth());
    settings_lua.WriteInt("screen_resy", VideoManager->GetScreenHeight());
    settings_lua.WriteComment("Run the screen fullscreen/in a window");
    settings_lua.WriteBool("full_screen", VideoManager->IsFullscreen());
    settings_lua.WriteComment("Used smoothed tile sprites when playing");
    settings_lua.WriteBool("smooth_graphics", VideoManager->ShouldSmoothPixelArt());
    settings_lua.EndTable(); // video_settings

    // audio
    settings_lua.InsertNewLine();
    settings_lua.WriteComment("--Audio settings--");
    settings_lua.BeginTable("audio_settings");
    settings_lua.WriteComment("Music and sounds volumes: [0.0 - 1.0]");
    settings_lua.WriteFloat("music_vol", AudioManager->GetMusicVolume());
    settings_lua.WriteFloat("sound_vol", AudioManager->GetSoundVolume());
    settings_lua.EndTable(); // audio_settings

    // input
    settings_lua.InsertNewLine();
    settings_lua.WriteComment("--Keyboard settings--");
    settings_lua.BeginTable("key_settings");
    settings_lua.WriteComment("Keyboard key SDL values.");
    settings_lua.WriteInt("up", InputManager->GetUpKey());
    settings_lua.WriteInt("down", InputManager->GetDownKey());
    settings_lua.WriteInt("left", InputManager->GetLeftKey());
    settings_lua.WriteInt("right", InputManager->GetRightKey());
    settings_lua.WriteInt("confirm", InputManager->GetConfirmKey());
    settings_lua.WriteInt("cancel", InputManager->GetCancelKey());
    settings_lua.WriteInt("menu", InputManager->GetMenuKey());
    settings_lua.WriteInt("pause", InputManager->GetPauseKey());
    settings_lua.EndTable(); // key_settings

    settings_lua.InsertNewLine();
    settings_lua.WriteComment("--Joystick settings--");
    settings_lua.BeginTable("joystick_settings");
    settings_lua.WriteComment("Tells whether joysticks input should be taken in account");
    settings_lua.WriteBool("input_disabled", (!InputManager->GetJoysticksEnabled()));
    settings_lua.WriteComment("The axis index number to be used as x/y axis");
    settings_lua.WriteInt("x_axis", InputManager->GetXAxisJoy());
    settings_lua.WriteInt("y_axis", InputManager->GetYAxisJoy());
    settings_lua.WriteComment("The joystick x/y axis dead zone [0-N] (Default: 8192)");
    settings_lua.WriteInt("threshold", InputManager->GetThresholdJoy());
    settings_lua.WriteComment("Joystick keys index number [0-N] (0 is the first button)");
    settings_lua.WriteInt("confirm", InputManager->GetConfirmJoy());
    settings_lua.WriteInt("cancel", InputManager->GetCancelJoy());
    settings_lua.WriteInt("menu", InputManager->GetMenuJoy());
    settings_lua.WriteInt("pause", InputManager->GetPauseJoy());
    settings_lua.WriteInt("quit", InputManager->GetQuitJoy());
    settings_lua.EndTable(); // joystick_settings

    settings_lua.EndTable(); // settings

    // and save it!
    settings_lua.SaveFile();
    settings_lua.CloseFile();

    _has_modified_settings = false;

    return true;
} // bool BootMode::_SaveSettingsFile(const std::string& filename)

// ****************************************************************************
// ***** BootMode input configuration methods
// ****************************************************************************

SDLKey BootMode::_WaitKeyPress()
{
    SDL_Event event;
    while(SDL_WaitEvent(&event)) {
        if(event.type == SDL_KEYDOWN)
            break;
    }

    return event.key.keysym.sym;
}

uint8 BootMode::_WaitJoyPress()
{
    SDL_Event event;
    while(SDL_WaitEvent(&event)) {
        if(event.type == SDL_JOYBUTTONDOWN)
            break;
    }

    return event.jbutton.button;
}

void BootMode::_RedefineUpKey()
{
    _key_setting_function = &BootMode::_SetUpKey;
    _ShowMessageWindow(false);
}

void BootMode::_RedefineDownKey()
{
    _key_setting_function = &BootMode::_SetDownKey;
    _ShowMessageWindow(false);
}

void BootMode::_RedefineLeftKey()
{
    _key_setting_function = &BootMode::_SetLeftKey;
    _ShowMessageWindow(false);
}

void BootMode::_RedefineRightKey()
{
    _key_setting_function = &BootMode::_SetRightKey;
    _ShowMessageWindow(false);
}

void BootMode::_RedefineConfirmKey()
{
    _key_setting_function = &BootMode::_SetConfirmKey;
    _ShowMessageWindow(false);
}

void BootMode::_RedefineCancelKey()
{
    _key_setting_function = &BootMode::_SetCancelKey;
    _ShowMessageWindow(false);
}

void BootMode::_RedefineMenuKey()
{
    _key_setting_function = &BootMode::_SetMenuKey;
    _ShowMessageWindow(false);
}

void BootMode::_RedefinePauseKey()
{
    _key_setting_function = &BootMode::_SetPauseKey;
    _ShowMessageWindow(false);
}

void BootMode::_SetUpKey(const SDLKey &key)
{
    InputManager->SetUpKey(key);
}

void BootMode::_SetDownKey(const SDLKey &key)
{
    InputManager->SetDownKey(key);
}

void BootMode::_SetLeftKey(const SDLKey &key)
{
    InputManager->SetLeftKey(key);
}

void BootMode::_SetRightKey(const SDLKey &key)
{
    InputManager->SetRightKey(key);
}

void BootMode::_SetConfirmKey(const SDLKey &key)
{
    InputManager->SetConfirmKey(key);
}

void BootMode::_SetCancelKey(const SDLKey &key)
{
    InputManager->SetCancelKey(key);
}

void BootMode::_SetMenuKey(const SDLKey &key)
{
    InputManager->SetMenuKey(key);
}

void BootMode::_SetPauseKey(const SDLKey &key)
{
    InputManager->SetPauseKey(key);
}

void BootMode::_RedefineXAxisJoy()
{
    _joy_axis_setting_function = &BootMode::_SetXAxisJoy;
    _ShowMessageWindow(WAIT_JOY_AXIS);
    InputManager->ResetLastAxisMoved();
}

void BootMode::_RedefineYAxisJoy()
{
    _joy_axis_setting_function = &BootMode::_SetYAxisJoy;
    _ShowMessageWindow(WAIT_JOY_AXIS);
    InputManager->ResetLastAxisMoved();
}

void BootMode::_RedefineConfirmJoy()
{
    _joy_setting_function = &BootMode::_SetConfirmJoy;
    _ShowMessageWindow(true);
}

void BootMode::_RedefineCancelJoy()
{
    _joy_setting_function = &BootMode::_SetCancelJoy;
    _ShowMessageWindow(true);
}

void BootMode::_RedefineMenuJoy()
{
    _joy_setting_function = &BootMode::_SetMenuJoy;
    _ShowMessageWindow(true);
}

void BootMode::_RedefinePauseJoy()
{
    _joy_setting_function = &BootMode::_SetPauseJoy;
    _ShowMessageWindow(true);
}

void BootMode::_RedefineQuitJoy()
{
    _joy_setting_function = &BootMode::_SetQuitJoy;
    _ShowMessageWindow(true);
}

void BootMode::_SetXAxisJoy(int8 axis)
{
    InputManager->SetXAxisJoy(axis);
}

void BootMode::_SetYAxisJoy(int8 axis)
{
    InputManager->SetYAxisJoy(axis);
}

void BootMode::_SetConfirmJoy(uint8 button)
{
    InputManager->SetConfirmJoy(button);
}

void BootMode::_SetCancelJoy(uint8 button)
{
    InputManager->SetCancelJoy(button);
}

void BootMode::_SetMenuJoy(uint8 button)
{
    InputManager->SetMenuJoy(button);
}

void BootMode::_SetPauseJoy(uint8 button)
{
    InputManager->SetPauseJoy(button);
}

void BootMode::_SetQuitJoy(uint8 button)
{
    InputManager->SetQuitJoy(button);
}

} // namespace vt_boot
