///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    options_handler.cpp
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for the options menus
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "options_handler.h"

#include "engine/system.h"
#include "engine/input.h"
#include "engine/audio/audio.h"
#include "engine/script/script_write.h"

#include "engine/mode_manager.h"
#include "modes/mode_help_window.h"

#include "utils/utils_files.h"

#include "common/global/global.h"

using namespace vt_utils;
using namespace vt_video;
using namespace vt_input;
using namespace vt_global;
using namespace vt_system;
using namespace vt_script;
using namespace vt_audio;

namespace vt_gui
{

namespace private_gui
{

void OptionMenu::AddOption(const ustring &text, GameOptionsMenuHandler* handler,
                         void (GameOptionsMenuHandler::*confirm_function)(),
                         void (GameOptionsMenuHandler::*up_function)(), void (GameOptionsMenuHandler::*down_function)(),
                         void (GameOptionsMenuHandler::*left_function)(), void (GameOptionsMenuHandler::*right_function)())
{
    OptionBox::AddOption(text);

    _handler = handler;

    if (_handler == nullptr) {
        PRINT_WARNING << "Invalid option menu handler given: No option menu functions will be working..." << std::endl;
        _confirm_handlers.push_back(nullptr);
        _up_handlers.push_back(nullptr);
        _down_handlers.push_back(nullptr);
        _left_handlers.push_back(nullptr);
        _right_handlers.push_back(nullptr);
        return;
    }

    _confirm_handlers.push_back(confirm_function);
    _up_handlers.push_back(up_function);
    _down_handlers.push_back(down_function);
    _left_handlers.push_back(left_function);
    _right_handlers.push_back(right_function);
}

void OptionMenu::InputConfirm()
{
    OptionBox::InputConfirm();

    int32_t selection = OptionBox::GetSelection();
    if((selection != -1) && (_confirm_handlers.empty() == false)) {
        void (GameOptionsMenuHandler::*confirm_function)() = _confirm_handlers.at(selection);
        if(confirm_function != nullptr)
            (_handler->*confirm_function)();
    }
}

void OptionMenu::InputUp()
{
    OptionBox::InputUp();

    int32_t selection = OptionBox::GetSelection();
    if((selection != -1) && (_up_handlers.empty() == false)) {
        void (GameOptionsMenuHandler::*up_function)() = _up_handlers.at(selection);
        if(up_function != nullptr)
            (_handler->*up_function)();
    }
}

void OptionMenu::InputDown()
{
    OptionBox::InputDown();

    int32_t selection = OptionBox::GetSelection();
    if((selection != -1) && (_down_handlers.empty() == false)) {
        void (GameOptionsMenuHandler::*down_function)() = _down_handlers.at(selection);
        if(down_function != nullptr)
            (_handler->*down_function)();
    }
}

void OptionMenu::InputLeft()
{
    OptionBox::InputLeft();

    int32_t selection = OptionBox::GetSelection();
    if((selection != -1) && (_left_handlers.empty() == false)) {
        void (GameOptionsMenuHandler::*left_function)() = _left_handlers.at(selection);
        if(left_function != nullptr)
            (_handler->*left_function)();
    }
}

void OptionMenu::InputRight()
{
    OptionBox::InputRight();

    int32_t selection = OptionBox::GetSelection();
    if((selection != -1) && (_right_handlers.empty() == false)) {
        void (GameOptionsMenuHandler::*right_function)() = _right_handlers.at(selection);
        if(right_function != nullptr)
            (_handler->*right_function)();
    }
}

const std::string _LANGUAGE_FILE = "data/config/languages.lua";
const uint16_t SKIN_MENU_INDEX = 5;

GameOptionsMenuHandler::GameOptionsMenuHandler(vt_mode_manager::GameMode* parent_mode):
    _first_run(false),
    _has_modified_settings(false),
    _active_menu(nullptr),
    _key_setting_function(nullptr),
    _joy_setting_function(nullptr),
    _joy_axis_setting_function(nullptr),
    _message_window(ustring(), -1.0f, -1.0f, 410.0f, 133.0f),
    _explanation_window(ustring(), -1.0f, 650.0f, 510.0f, 100.0f),
    _parent_mode(parent_mode)
{
    // Create the option window used as background
    _options_window.Create(400.0f, 550.0f);
    _options_window.SetPosition(310.0f, 58.0f);
    _options_window.Hide();

    // Setup all menu options and properties
    _SetupOptionsMenu();
    _SetupVideoOptionsMenu();
    _SetupAudioOptionsMenu();
    _SetupGameOptions();
    _SetupLanguageOptionsMenu();
    _SetupKeySettingsMenu();
    _SetupJoySettingsMenu();
    _SetupResolutionMenu();

    // make sure message window is not visible
    _message_window.Hide();
}

GameOptionsMenuHandler::~GameOptionsMenuHandler()
{
    _options_window.Destroy();
    _SaveSettingsFile();

    _key_setting_function = nullptr;
    _joy_setting_function = nullptr;
    _joy_axis_setting_function = nullptr;
}

void GameOptionsMenuHandler::Activate()
{
    _active_menu = &_options_menu;
    _options_window.Show();
    _UpdateExplanationText();
}

void GameOptionsMenuHandler::ShowFirstRunLanguageSelection()
{
    _first_run = true;
    _options_window.Show();
    _active_menu = &_language_options_menu;
}

void GameOptionsMenuHandler::Update()
{
    _options_window.Update(vt_system::SystemManager->GetUpdateTime());
    _message_window.Update();
    _explanation_window.Update();

    // On first app run, show the language menu and apply language on any key press.
    if (_first_run && _active_menu == &_language_options_menu) {
        _active_menu->Update();
        if (InputManager->UpPress()) {
            _active_menu->InputUp();
        }
        else if (InputManager->DownPress()) {
            _active_menu->InputDown();
        }
        else if (InputManager->LeftPress() || InputManager->RightPress()) {
            // Do nothing in this case
        }
        else if (InputManager->AnyKeyboardKeyPress()
                || InputManager->AnyJoystickKeyPress()
                || InputManager->ConfirmPress()) {
            // Set the language
            _active_menu->InputConfirm();
            // Go directly back to the main menu when first selecting the language.
            _options_window.Hide();
            _active_menu = nullptr;
            // And show the help window
            vt_mode_manager::ModeManager->GetHelpWindow()->Show();
            // save the settings (automatically changes the first_start variable to 0)
            _has_modified_settings = true;
            _SaveSettingsFile();
            _first_run = false;
        }
        return;
    }

    // Updates the current menu or do nothing
    if (_active_menu)
        _active_menu->Update();
    else
        return;

    // Check for waiting keypresses or joystick button presses
    if(_joy_setting_function != nullptr) {
        if(InputManager->AnyJoystickKeyPress()) {
            (this->*_joy_setting_function)(InputManager->GetMostRecentEvent().jbutton.button);
            _joy_setting_function = nullptr;
            _has_modified_settings = true;
            _RefreshJoySettings();
            _message_window.Hide();
        }
        if(InputManager->CancelPress()) {
            _joy_setting_function = nullptr;
            _message_window.Hide();
        }
        return;
    }

    if(_joy_axis_setting_function != nullptr) {
        int8_t x = InputManager->GetLastAxisMoved();
        if(x != -1) {
            (this->*_joy_axis_setting_function)(x);
            _joy_axis_setting_function = nullptr;
            _has_modified_settings = true;
            _RefreshJoySettings();
            _message_window.Hide();
        }
        if(InputManager->CancelPress()) {
            _joy_axis_setting_function = nullptr;
            _message_window.Hide();
        }
        return;
    }

    if(_key_setting_function != nullptr) {
        if(InputManager->AnyKeyboardKeyPress()) {
            (this->*_key_setting_function)(InputManager->GetMostRecentEvent().key.keysym.sym);
            _key_setting_function = nullptr;
            _has_modified_settings = true;
            _RefreshKeySettings();
            _message_window.Hide();
        }
        if(InputManager->CancelPress()) {
            _key_setting_function = nullptr;
            _message_window.Hide();
        }
        return;
    }

    if(InputManager->ConfirmPress()) {
        // Play 'confirm sound' if the selection isn't grayed out and it has a confirm handler
        if(_active_menu->IsOptionEnabled(_active_menu->GetSelection())) {
            // Don't play the sound on New Games as they have their own sound
            if(_active_menu->GetSelection() != -1)
                GlobalManager->Media().PlaySound("confirm");
        } else {
            // Otherwise play a different sound
            GlobalManager->Media().PlaySound("bump");
        }

        _active_menu->InputConfirm();

    } else if(InputManager->LeftPress()) {
        GlobalManager->Media().PlaySound("bump");
        _active_menu->InputLeft();
    } else if(InputManager->RightPress()) {
        GlobalManager->Media().PlaySound("bump");
        _active_menu->InputRight();
    } else if(InputManager->UpPress()) {
        GlobalManager->Media().PlaySound("bump");
        _active_menu->InputUp();
    } else if(InputManager->DownPress()) {
        GlobalManager->Media().PlaySound("bump");
        _active_menu->InputDown();
    } else if(InputManager->CancelPress() || InputManager->QuitPress()) {
        if(_active_menu == &_options_menu) {
            _options_window.Hide();
            _active_menu = nullptr;
        } else if(_active_menu == &_video_options_menu) {
            _active_menu = &_options_menu;
        } else if(_active_menu == &_audio_options_menu) {
            _active_menu = &_options_menu;
        } else if(_active_menu == &_game_options_menu) {
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

    // Updates the explanation text when it's relevant.
    if (InputManager->AnyRegisteredKeyPress())
        _UpdateExplanationText();
}

void GameOptionsMenuHandler::Draw()
{
    VideoManager->PushState();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
    VideoManager->SetStandardCoordSys();

    _options_window.Draw();

    if(_active_menu)
        _active_menu->Draw();

    VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, 0);
    VideoManager->Move(0.0f, 0.0f);
    _message_window.Draw();
    _explanation_window.Draw();

    VideoManager->PopState();
}

void GameOptionsMenuHandler::ReloadTranslatableMenus()
{
    _SetupOptionsMenu();
    _SetupVideoOptionsMenu();
    _SetupAudioOptionsMenu();
    _SetupGameOptions();
    _SetupKeySettingsMenu();
    _SetupJoySettingsMenu();
    _SetupResolutionMenu();

    // Make the parent game mode reload its translated text
    if (_parent_mode)
        _parent_mode->ReloadTranslatedTexts();

    // Reset the window title
    SDL_Window* sdl_window = VideoManager->GetWindowHandle();
    if (sdl_window) {
        /// Translators: The window title only supports UTF-8 characters in SDL2.
        std::string AppFullName = vt_system::Translate("Valyria Tear");
        SDL_SetWindowTitle(sdl_window, AppFullName.c_str());
    }
}

void GameOptionsMenuHandler::_SetupOptionsMenu()
{
    _options_menu.ClearOptions();
    _options_menu.SetPosition(512.0f, 338.0f);
    _options_menu.SetDimensions(300.0f, 600.0f, 1, 6, 1, 6);
    _options_menu.SetTextStyle(TextStyle("title22"));
    _options_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _options_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _options_menu.SetCursorOffset(-50.0f, -28.0f);
    _options_menu.SetSkipDisabled(true);

    _options_menu.AddOption(UTranslate("Video"), this, &GameOptionsMenuHandler::_OnVideoOptions);
    _options_menu.AddOption(UTranslate("Audio"), this, &GameOptionsMenuHandler::_OnAudioOptions);
    _options_menu.AddOption(UTranslate("Game"), this, &GameOptionsMenuHandler::_OnGameOptions);
    _options_menu.AddOption(UTranslate("Language"), this, &GameOptionsMenuHandler::_OnLanguageOptions);
    _options_menu.AddOption(UTranslate("Key Settings"), this, &GameOptionsMenuHandler::_OnKeySettings);
    _options_menu.AddOption(UTranslate("Joystick Settings"), this, &GameOptionsMenuHandler::_OnJoySettings);

    _options_menu.SetSelection(0);

    // Disable the language menu when not in the boot menu.
    // Otherwise, the game language changes aren't handled correctly.
    if (_parent_mode && _parent_mode->GetGameType() != vt_mode_manager::MODE_MANAGER_BOOT_MODE)
        _options_menu.EnableOption(3, false);
}

void GameOptionsMenuHandler::_SetupVideoOptionsMenu()
{
    _video_options_menu.ClearOptions();
    _video_options_menu.SetPosition(512.0f, 338.0f);
    _video_options_menu.SetDimensions(300.0f, 400.0f, 1, 6, 1, 6);
    _video_options_menu.SetTextStyle(TextStyle("title22"));
    _video_options_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _video_options_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _video_options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _video_options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _video_options_menu.SetCursorOffset(-50.0f, -28.0f);
    _video_options_menu.SetSkipDisabled(true);

    _video_options_menu.AddOption(UTranslate("Resolution: "), this, &GameOptionsMenuHandler::_OnResolution);
    // Left & right will change window mode as well as confirm
    _video_options_menu.AddOption(UTranslate("Window mode: "), this, &GameOptionsMenuHandler::_OnToggleFullscreen, nullptr, nullptr,
                                  &GameOptionsMenuHandler::_OnToggleFullscreen, &GameOptionsMenuHandler::_OnToggleFullscreen);
    _video_options_menu.AddOption(UTranslate("Brightness: "), this, nullptr, nullptr, nullptr, &GameOptionsMenuHandler::_OnBrightnessLeft,
                                  &GameOptionsMenuHandler::_OnBrightnessRight);
    _video_options_menu.AddOption(UTranslate("VSync: "), this, nullptr, nullptr, nullptr,
                                  &GameOptionsMenuHandler::_OnChangeVSyncLeft,
                                  &GameOptionsMenuHandler::_OnChangeVSyncRight);
    _video_options_menu.AddOption(UTranslate("Update method: "), this, &GameOptionsMenuHandler::_OnChangeGameUpdateMode,
                                  nullptr, nullptr, nullptr, nullptr);
    _video_options_menu.AddOption(UTranslate("UI Theme: "), this, &GameOptionsMenuHandler::_OnUIThemeRight, nullptr, nullptr,
                                  &GameOptionsMenuHandler::_OnUIThemeLeft, &GameOptionsMenuHandler::_OnUIThemeRight);

    _video_options_menu.SetSelection(0);
}

void GameOptionsMenuHandler::_SetupAudioOptionsMenu()
{
    _audio_options_menu.ClearOptions();
    _audio_options_menu.SetPosition(512.0f, 338.0f);
    _audio_options_menu.SetDimensions(300.0f, 200.0f, 1, 2, 1, 2);
    _audio_options_menu.SetTextStyle(TextStyle("title22"));
    _audio_options_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _audio_options_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _audio_options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _audio_options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _audio_options_menu.SetCursorOffset(-50.0f, -28.0f);
    _audio_options_menu.SetSkipDisabled(true);

    _audio_options_menu.AddOption(UTranslate("Sound Volume: "), this, nullptr, nullptr, nullptr,
                                  &GameOptionsMenuHandler::_OnSoundLeft,
                                  &GameOptionsMenuHandler::_OnSoundRight);
    _audio_options_menu.AddOption(UTranslate("Music Volume: "), this, nullptr, nullptr, nullptr,
                                  &GameOptionsMenuHandler::_OnMusicLeft,
                                  &GameOptionsMenuHandler::_OnMusicRight);

    _audio_options_menu.SetSelection(0);
}

void GameOptionsMenuHandler::_SetupGameOptions()
{
    _game_options_menu.ClearOptions();
    _game_options_menu.SetPosition(512.0f, 338.0f);
    _game_options_menu.SetDimensions(300.0f, 200.0f, 1, 3, 1, 3);
    _game_options_menu.SetTextStyle(TextStyle("title22"));
    _game_options_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _game_options_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _game_options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _game_options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _game_options_menu.SetCursorOffset(-50.0f, -28.0f);

    std::string difficulty_text;
    switch (SystemManager->GetGameDifficulty()) {
    case 1:
        difficulty_text = Translate("Easy");
        break;
    default:
        SystemManager->SetGameDifficulty(2);
    case 2:
        difficulty_text = Translate("Normal");
        break;
    case 3:
        difficulty_text = Translate("Hard");
        break;
    }

    ustring text = MakeUnicodeString(VTranslate("Game Difficulty: %s", difficulty_text));
    _game_options_menu.AddOption(text, this, &GameOptionsMenuHandler::_OnGameDifficultyConfirm,
                                 nullptr, nullptr,
                                 &GameOptionsMenuHandler::_OnGameDifficultyConfirm,
                                 &GameOptionsMenuHandler::_OnGameDifficultyConfirm);

    text = MakeUnicodeString(VTranslate("Dialogue text speed: %i", static_cast<int32_t>(SystemManager->GetMessageSpeed())));
    _game_options_menu.AddOption(text, this, nullptr, nullptr, nullptr,
                                 &GameOptionsMenuHandler::_OnDialogueSpeedLeft,
                                 &GameOptionsMenuHandler::_OnDialogueSpeedRight);
    text = MakeUnicodeString(VTranslate("Battle target cursor memory: %s", (SystemManager->GetBattleTargetMemory() ? Translate("Yes") : Translate("No"))));
    _game_options_menu.AddOption(text, this, &GameOptionsMenuHandler::_OnBattleTargetCursorMemoryConfirm);
}

void GameOptionsMenuHandler::_SetupLanguageOptionsMenu()
{
    _language_options_menu.SetPosition(402.0f, 338.0f);
    _language_options_menu.SetTextStyle(TextStyle("title22"));
    _language_options_menu.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _language_options_menu.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _language_options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _language_options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _language_options_menu.SetCursorOffset(-50.0f, -28.0f);
    _language_options_menu.SetSkipDisabled(true);

    _RefreshLanguageOptions();
}

void GameOptionsMenuHandler::_SetupKeySettingsMenu()
{
    _key_settings_menu.ClearOptions();
    _key_settings_menu.SetPosition(512.0f, 338.0f);
    _key_settings_menu.SetDimensions(250.0f, 500.0f, 1, 10, 1, 10);
    _key_settings_menu.SetTextStyle(TextStyle("title20"));
    _key_settings_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _key_settings_menu.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _key_settings_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _key_settings_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _key_settings_menu.SetCursorOffset(-50.0f, -28.0f);
    _key_settings_menu.SetSkipDisabled(true);

    _key_settings_menu.AddOption(UTranslate("Up: "), this, &GameOptionsMenuHandler::_RedefineUpKey);
    _key_settings_menu.AddOption(UTranslate("Down: "), this, &GameOptionsMenuHandler::_RedefineDownKey);
    _key_settings_menu.AddOption(UTranslate("Left: "), this, &GameOptionsMenuHandler::_RedefineLeftKey);
    _key_settings_menu.AddOption(UTranslate("Right: "), this, &GameOptionsMenuHandler::_RedefineRightKey);
    _key_settings_menu.AddOption(UTranslate("Confirm: "), this, &GameOptionsMenuHandler::_RedefineConfirmKey);
    _key_settings_menu.AddOption(UTranslate("Cancel: "), this, &GameOptionsMenuHandler::_RedefineCancelKey);
    _key_settings_menu.AddOption(UTranslate("Menu: "), this, &GameOptionsMenuHandler::_RedefineMenuKey);
    _key_settings_menu.AddOption(UTranslate("Toggle Map: "), this, &GameOptionsMenuHandler::_RedefineMinimapKey);
    _key_settings_menu.AddOption(UTranslate("Pause: "), this, &GameOptionsMenuHandler::_RedefinePauseKey);
    _key_settings_menu.AddOption(UTranslate("Restore defaults"), this, &GameOptionsMenuHandler::_OnRestoreDefaultKeys);
}

void GameOptionsMenuHandler::_SetupJoySettingsMenu()
{
    _joy_settings_menu.ClearOptions();
    _joy_settings_menu.SetPosition(512.0f, 338.0f);
    _joy_settings_menu.SetDimensions(250.0f, 500.0f, 1, 12, 1, 12);
    _joy_settings_menu.SetTextStyle(TextStyle("title20"));
    _joy_settings_menu.SetTextStyle(TextStyle("title22"));
    _joy_settings_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _joy_settings_menu.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _joy_settings_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _joy_settings_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _joy_settings_menu.SetCursorOffset(-50.0f, -28.0f);
    _joy_settings_menu.SetSkipDisabled(true);

    ustring dummy;
    _joy_settings_menu.AddOption(dummy, this, &GameOptionsMenuHandler::_OnToggleJoystickEnabled, nullptr, nullptr,
                                 &GameOptionsMenuHandler::_OnToggleJoystickEnabled,
                                 &GameOptionsMenuHandler::_OnToggleJoystickEnabled);
    _joy_settings_menu.AddOption(dummy, this, &GameOptionsMenuHandler::_RedefineXAxisJoy);
    _joy_settings_menu.AddOption(dummy, this, &GameOptionsMenuHandler::_RedefineYAxisJoy);
    _joy_settings_menu.AddOption(dummy, this, nullptr, nullptr, nullptr, &GameOptionsMenuHandler::_OnThresholdJoyLeft,
                                 &GameOptionsMenuHandler::_OnThresholdJoyRight);

    _joy_settings_menu.AddOption(dummy, this, &GameOptionsMenuHandler::_RedefineConfirmJoy);
    _joy_settings_menu.AddOption(dummy, this, &GameOptionsMenuHandler::_RedefineCancelJoy);
    _joy_settings_menu.AddOption(dummy, this, &GameOptionsMenuHandler::_RedefineMenuJoy);
    _joy_settings_menu.AddOption(dummy, this, &GameOptionsMenuHandler::_RedefineMinimapJoy);
    _joy_settings_menu.AddOption(dummy, this, &GameOptionsMenuHandler::_RedefinePauseJoy);
    _joy_settings_menu.AddOption(dummy, this, &GameOptionsMenuHandler::_RedefineHelpJoy);
    _joy_settings_menu.AddOption(dummy, this, &GameOptionsMenuHandler::_RedefineQuitJoy);

    _joy_settings_menu.AddOption(UTranslate("Restore defaults"), this, &GameOptionsMenuHandler::_OnRestoreDefaultJoyButtons);
}

void GameOptionsMenuHandler::_SetupResolutionMenu()
{
    _resolution_menu.SetPosition(442.0f, 338.0f);
    _resolution_menu.SetTextStyle(TextStyle("title22"));
    _resolution_menu.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _resolution_menu.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _resolution_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _resolution_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _resolution_menu.SetCursorOffset(-50.0f, -28.0f);
    _resolution_menu.SetSkipDisabled(true);

    // Get the different resolution modes
    int32_t display_count = SDL_GetNumVideoDisplays();
    //std::cout << "Display count (Screens available): " << display_count << std::endl;
    int32_t mode_count = SDL_GetNumDisplayModes(0);

    _resolution_list.clear();

    SDL_DisplayMode mode = {SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};
    if (display_count > 0 && mode_count > 0) {
        // Reverse iteration of available resolutions since it's the highest to smallest.
        for (int32_t mode_index = mode_count - 1; mode_index != 0; --mode_index) {

            // Don't list failing modes
            if (SDL_GetDisplayMode(0, mode_index, &mode) != 0)
                continue;

            // debug: List Mode
            //std::cout << "Display mode: " << SDL_BITSPERPIXEL(mode.format) << " bpp, "
            //    << mode.w << " x " << mode.h << std::endl;

           // Only accept modes with 24 or higher bpp
           if (SDL_BITSPERPIXEL(mode.format) < 24)
               continue;

           // dups check
           bool duplicate_found = false;
           for (uint32_t i = 0; i < _resolution_list.size(); ++i) {
               if (_resolution_list[i].w == mode.w && _resolution_list[i].h == mode.h) {
                   duplicate_found = true;
                   break;
               }
           }
           if (duplicate_found)
               continue;

           _resolution_list.push_back(mode);
        }
    }
    else {
        // There is a problem to get info from the display, let's fall back to hard-coded mode
        SDL_DisplayMode hardcodedmode = {SDL_PIXELFORMAT_UNKNOWN, 640, 480, 0, 0};
        _resolution_list.push_back(hardcodedmode);
        hardcodedmode.w = 800; hardcodedmode.h = 600;
        _resolution_list.push_back(hardcodedmode);
        hardcodedmode.w = 1024; hardcodedmode.h = 768;
        _resolution_list.push_back(hardcodedmode);
        hardcodedmode.w = 1280; hardcodedmode.h = 1024;
        _resolution_list.push_back(hardcodedmode);
        hardcodedmode.w = 1366; hardcodedmode.h = 768;
        _resolution_list.push_back(hardcodedmode);
        hardcodedmode.w = 1440; hardcodedmode.h = 900;
        _resolution_list.push_back(hardcodedmode);
        hardcodedmode.w = 1600; hardcodedmode.h = 900;
        _resolution_list.push_back(hardcodedmode);
        hardcodedmode.w = 3200; hardcodedmode.h = 1800;
        _resolution_list.push_back(hardcodedmode);
    }

    // We set up the size display list.
    _resolution_menu.SetDimensions(250.0f, 500.0f, 1, _resolution_list.size(), 1,
                                   _resolution_list.size() > 15 ? 15 : _resolution_list.size());

    // Get current resolution
    if (SDL_GetWindowDisplayMode(VideoManager->GetWindowHandle(), &mode) != 0) {
        // If we can't, fall back to a reasonably small one.
        mode.w = 800;
        mode.h = 600;
    }
    //else {
    //    std::cout << "Current Display mode: " << SDL_BITSPERPIXEL(mode.format) << " bpp, "
    //        << mode.w << " x " << mode.h << std::endl;
    //}

    _resolution_menu.ClearOptions();

    for (uint32_t i = 0; i < _resolution_list.size(); ++i) {
        _resolution_menu.AddOption(ustring(), this, &GameOptionsMenuHandler::_OnResolutionConfirm);
        // Show the current selection
        if (mode.w == _resolution_list[i].w && mode.h == _resolution_list[i].h) {
            _resolution_menu.AddOptionElementImage(i, "data/gui/menus/star.png");
            _resolution_menu.SetSelection(i);
        }
        _resolution_menu.AddOptionElementPosition(i, 32);
        std::stringstream resStr("");
        resStr << _resolution_list[i].w << " x " << _resolution_list[i].h;
        _resolution_menu.AddOptionElementText(i, MakeUnicodeString(resStr.str()));
    }
}

void GameOptionsMenuHandler::_RefreshVideoOptions()
{
    // Update resolution text
    std::ostringstream resolution("");
    resolution << VideoManager->GetScreenWidth() << " x " << VideoManager->GetScreenHeight();
    _video_options_menu.SetOptionText(0, UTranslate("Resolution: ") + MakeUnicodeString(resolution.str()));

    // Update text on current video mode
    if(VideoManager->IsFullscreen()) {
        _video_options_menu.EnableOption(0, false);
        _video_options_menu.SetOptionText(1, UTranslate("Window mode: ") + UTranslate("Fullscreen"));
    }
    else {
        _video_options_menu.EnableOption(0, true);
        _video_options_menu.SetOptionText(1, UTranslate("Window mode: ") + UTranslate("Windowed"));
    }

    // Update brightness
    float curr_brightness = VideoManager->GetBrightness();
    uint32_t brightness = IsFloatEqual(curr_brightness * 50.0f, 0.0f) ? 0 : (uint32_t)(curr_brightness * 50.0f + 0.5f);
    _video_options_menu.SetOptionText(2, UTranslate("Brightness: ") + MakeUnicodeString(NumberToString(brightness) + " %"));

    // Update the Vsync mode
    std::string vsync_str;
    bool update_mode_enabled = true;
    /// Translators: Do not translate the part before the '|'.
    /// It is used for contextual translation support.
    switch(VideoManager->GetVSyncMode()) {
    default:
    case 0:
        vsync_str = CTranslate("VSync_mode|Off");
        break;
    case 1:
        vsync_str = CTranslate("VSync_mode|On");
        update_mode_enabled = false;
        break;
    case 2:
        vsync_str = CTranslate("VSync_mode|Swap Tearing");
        update_mode_enabled = false;
        break;
    }
    _video_options_menu.SetOptionText(3, UTranslate("VSync: ") + MakeUnicodeString(vsync_str));

    // With VSync support, there is no need to determine the update method, it can ve set to performance.
    // The VSync method will auto limit the framerate.
    if (update_mode_enabled) {
        std::string update_mode_str;
        /// Translators: Do not translate the part before the '|'.
        /// It is used for contextual translation support.
        if (VideoManager->GetGameUpdateMode())
            update_mode_str = CTranslate("UpdateMode|Performance");
        else
            update_mode_str = CTranslate("UpdateMode|Gentle");
        _video_options_menu.SetOptionText(4, UTranslate("Update method: ") + MakeUnicodeString(update_mode_str));
        _video_options_menu.EnableOption(4, true);
    }
    else {
        _video_options_menu.SetOptionText(4, UTranslate("Update method: ") + UTranslate("Performance"));
        _video_options_menu.EnableOption(4, false);
    }

    // Update the UI theme.
    _video_options_menu.SetOptionText(SKIN_MENU_INDEX, UTranslate("UI Theme: ") + GUIManager->GetDefaultMenuSkinName());
}

void GameOptionsMenuHandler::_RefreshLanguageOptions()
{
    // Get the list of languages from the Lua file.
    ReadScriptDescriptor read_data;
    if(!read_data.OpenFile(_LANGUAGE_FILE) || !read_data.OpenTable("languages")) {
        PRINT_ERROR << "Failed to load language file: " << _LANGUAGE_FILE << std::endl
                    << "The language list will be empty." << std::endl;
        read_data.CloseFile();
        return;
    }

    uint32_t table_size = read_data.GetTableSize();

    // Set up the dimensions of the window according to how many languages are available.
    _language_options_menu.ClearOptions();
    _language_options_menu.SetDimensions(300.0f, 500.0f, 1, table_size, 1, (table_size > 12 ? 12 : table_size));

    // Used to warn about missing po files, but only once at start.
    static bool warnAboutMissingFiles = true;

    _po_files.clear();
    std::string current_language = vt_system::SystemManager->GetLanguage();
    for(uint32_t i = 1; i <= table_size; ++i) {
        read_data.OpenTable(i);
        _po_files.push_back(read_data.ReadString(2));

        std::string lang = _po_files[i - 1];
        _language_options_menu.AddOption(ustring(), this,  &GameOptionsMenuHandler::_OnLanguageSelect);
        if (lang == current_language) {
            _language_options_menu.AddOptionElementImage(i - 1, "data/gui/menus/star.png");
            _language_options_menu.SetSelection(i - 1);
        }
        _language_options_menu.AddOptionElementPosition(i - 1, 32);
        _language_options_menu.AddOptionElementText(i - 1, MakeUnicodeString(read_data.ReadString(1)));

        // Test the current language availability
        if (!vt_system::SystemManager->IsLanguageAvailable(lang)) {
            // NOTE: English is always available.
            if (i > 1)
                _language_options_menu.EnableOption(i - 1, false);
            // We also reset the current selection when the current language is unavailable.
            if (lang == current_language)
                _language_options_menu.SetSelection(0);

            if (warnAboutMissingFiles) {
                std::string mo_filename = lang + "/LC_MESSAGES/" APPSHORTNAME ".mo";
                PRINT_WARNING << "Couldn't locate gettext .mo file: '" << mo_filename << "'." << std::endl
                    << "The " << lang << " translation will be disabled." << std::endl;
            }
        }

#ifdef DISABLE_TRANSLATIONS
        // If translations are disabled, only admit the first entry (English)
        if (i > 1)
            _language_options_menu.EnableOption(i - 1, false);
        _language_options_menu.SetSelection(0);
#endif
        read_data.CloseTable();
    }

    // Only warn once about missing language files.
    warnAboutMissingFiles = false;

    read_data.CloseTable();
    if(read_data.IsErrorDetected())
        PRINT_ERROR << "Error occurred while loading language list: " << read_data.GetErrorMessages() << std::endl;
    read_data.CloseFile();
}

void GameOptionsMenuHandler::_RefreshAudioOptions()
{
    _audio_options_menu.SetOptionText(0, UTranslate("Sound Volume: ") + MakeUnicodeString(NumberToString(static_cast<int32_t>(AudioManager->GetSoundVolume() * 100.0f + 0.5f)) + " %"));
    _audio_options_menu.SetOptionText(1, UTranslate("Music Volume: ") + MakeUnicodeString(NumberToString(static_cast<int32_t>(AudioManager->GetMusicVolume() * 100.0f + 0.5f)) + " %"));
}

void GameOptionsMenuHandler::_RefreshGameOptions()
{
    std::string difficulty_text;
    switch (SystemManager->GetGameDifficulty()) {
    case 1:
        difficulty_text = Translate("Easy");
        break;
    default:
        SystemManager->SetGameDifficulty(2);
    case 2:
        difficulty_text = Translate("Normal");
        break;
    case 3:
        difficulty_text = Translate("Hard");
        break;
    }

    ustring text = MakeUnicodeString(VTranslate("Game Difficulty: %s", difficulty_text));
    _game_options_menu.SetOptionText(0, text);

    text = MakeUnicodeString(VTranslate("Dialogue text speed: %i", static_cast<int32_t>(SystemManager->GetMessageSpeed())));
    _game_options_menu.SetOptionText(1, text);

    text = MakeUnicodeString(VTranslate("Battle target cursor memory: %s", (SystemManager->GetBattleTargetMemory() ? Translate("Yes") : Translate("No"))));
    _game_options_menu.SetOptionText(2, text);
}

void GameOptionsMenuHandler::_RefreshKeySettings()
{
    // Update key names
    _key_settings_menu.SetOptionText(0, UTranslate("Move Up") + MakeUnicodeString("<r>") + UTranslate(InputManager->GetUpKeyName()));
    _key_settings_menu.SetOptionText(1, UTranslate("Move Down") + MakeUnicodeString("<r>") + UTranslate(InputManager->GetDownKeyName()));
    _key_settings_menu.SetOptionText(2, UTranslate("Move Left") + MakeUnicodeString("<r>") + UTranslate(InputManager->GetLeftKeyName()));
    _key_settings_menu.SetOptionText(3, UTranslate("Move Right") + MakeUnicodeString("<r>") + UTranslate(InputManager->GetRightKeyName()));
    _key_settings_menu.SetOptionText(4, UTranslate("Confirm") + MakeUnicodeString("<r>") + UTranslate(InputManager->GetConfirmKeyName()));
    _key_settings_menu.SetOptionText(5, UTranslate("Cancel") + MakeUnicodeString("<r>") + UTranslate(InputManager->GetCancelKeyName()));
    _key_settings_menu.SetOptionText(6, UTranslate("Menu") + MakeUnicodeString("<r>") + UTranslate(InputManager->GetMenuKeyName()));
    _key_settings_menu.SetOptionText(7, UTranslate("Toggle Map") + MakeUnicodeString("<r>") + UTranslate(InputManager->GetMinimapKeyName()));
    _key_settings_menu.SetOptionText(8, UTranslate("Pause") + MakeUnicodeString("<r>") + UTranslate(InputManager->GetPauseKeyName()));
}

void GameOptionsMenuHandler::_RefreshJoySettings()
{
    int32_t i = 0;
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
    _joy_settings_menu.SetOptionText(i++, UTranslate("Map: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetMinimapJoy())));
    _joy_settings_menu.SetOptionText(i++, UTranslate("Pause: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetPauseJoy())));
    _joy_settings_menu.SetOptionText(i++, UTranslate("Help: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetHelpJoy())));
    _joy_settings_menu.SetOptionText(i++, UTranslate("Quit: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetQuitJoy())));
}

void GameOptionsMenuHandler::_OnVideoOptions()
{
    _active_menu = &_video_options_menu;
    _RefreshVideoOptions();
}

void GameOptionsMenuHandler::_OnAudioOptions()
{
    // Switch the current menu
    _active_menu = &_audio_options_menu;
    _RefreshAudioOptions();
}

void GameOptionsMenuHandler::_OnGameOptions()
{
    // Switch the current menu
    _active_menu = &_game_options_menu;
    _RefreshGameOptions();
}

void GameOptionsMenuHandler::_OnLanguageOptions()
{
    // Switch the current menu
    _active_menu = &_language_options_menu;
    _RefreshLanguageOptions();
}

void GameOptionsMenuHandler::_OnKeySettings()
{
    _active_menu = &_key_settings_menu;
    _RefreshKeySettings();
}

void GameOptionsMenuHandler::_OnJoySettings()
{
    _active_menu = &_joy_settings_menu;
    _RefreshJoySettings();
}

void GameOptionsMenuHandler::_OnToggleFullscreen()
{
    // Toggle fullscreen / windowed
    VideoManager->ToggleFullscreen();
    VideoManager->ApplySettings();
    _RefreshVideoOptions();
    _SetupResolutionMenu();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnResolution()
{
    _active_menu = &_resolution_menu;
}

void GameOptionsMenuHandler::_OnResolutionConfirm()
{
    if (_resolution_list.empty())
        return;

    uint32_t index = static_cast<uint32_t>(_resolution_menu.GetSelection());

    if (index >= _resolution_list.size())
        index = 0;

    _ChangeResolution(_resolution_list[index].w, _resolution_list[index].h);
}

void GameOptionsMenuHandler::_OnBrightnessLeft()
{
    VideoManager->SetBrightness(VideoManager->GetBrightness() - 0.1f);
    _RefreshVideoOptions();
}

void GameOptionsMenuHandler::_OnBrightnessRight()
{
    VideoManager->SetBrightness(VideoManager->GetBrightness() + 0.1f);
    _RefreshVideoOptions();
}

void GameOptionsMenuHandler::_OnChangeVSyncLeft()
{
    uint32_t vsync_mode = VideoManager->GetVSyncMode();
    if (vsync_mode == 0)
        vsync_mode = 2;
    else
        --vsync_mode;
    VideoManager->SetVSyncMode(vsync_mode);
    VideoManager->ApplySettings();
    _RefreshVideoOptions();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnChangeVSyncRight()
{
    uint32_t vsync_mode = VideoManager->GetVSyncMode();
    if (vsync_mode == 2)
        vsync_mode = 0;
    else
        ++vsync_mode;
    VideoManager->SetVSyncMode(vsync_mode);
    VideoManager->ApplySettings();
    _RefreshVideoOptions();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnChangeGameUpdateMode()
{
    VideoManager->SetGameUpdateMode(!VideoManager->GetGameUpdateMode());
    VideoManager->ApplySettings();
    _RefreshVideoOptions();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnUIThemeLeft()
{
    GUIManager->SetPreviousDefaultMenuSkin();
    _ReloadGUIDefaultSkin();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnUIThemeRight()
{
    GUIManager->SetNextDefaultMenuSkin();
    _ReloadGUIDefaultSkin();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnSoundLeft()
{
    AudioManager->SetSoundVolume(AudioManager->GetSoundVolume() - 0.1f);
    _RefreshAudioOptions();
    // Play a sound for user to hear new volume level.
    GlobalManager->Media().PlaySound("volume_test");
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnSoundRight()
{
    AudioManager->SetSoundVolume(AudioManager->GetSoundVolume() + 0.1f);
    _RefreshAudioOptions();
    // Play a sound for user to hear new volume level.
    GlobalManager->Media().PlaySound("volume_test");
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnMusicLeft()
{
    AudioManager->SetMusicVolume(AudioManager->GetMusicVolume() - 0.1f);
    _RefreshAudioOptions();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnMusicRight()
{
    AudioManager->SetMusicVolume(AudioManager->GetMusicVolume() + 0.1f);
    _RefreshAudioOptions();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnLanguageSelect()
{
    std::string language = _po_files[_language_options_menu.GetSelection()];
    // Reset the language in case changing failed.
    if (!SystemManager->SetLanguage(language)) {
        _RefreshLanguageOptions();
        return;
    }

    // Reload the font according to the newly selected language.
    TextManager->LoadFonts(language);

    _has_modified_settings = true;

    // Reloads the theme names before the menus
    GUIManager->ReloadSkinNames("data/config/themes.lua");

    // Reload all the translatable text in the menus.
    ReloadTranslatableMenus();

    // Reloads the global scripts to update their inner translatable strings
    GlobalManager->ReloadGlobalScripts();

    _RefreshLanguageOptions();
}

void GameOptionsMenuHandler::_OnGameDifficultyConfirm()
{
    uint32_t difficulty = SystemManager->GetGameDifficulty();
    if (difficulty == 0) // Invalid
        difficulty = 2; // Normal
    if (difficulty >= 3) // Cycle through values.
        difficulty = 1;
    else
        ++difficulty;

    SystemManager->SetGameDifficulty(difficulty);
    _RefreshGameOptions();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnDialogueSpeedLeft()
{
    SystemManager->SetMessageSpeed(SystemManager->GetMessageSpeed() - 10.0f);
    _RefreshGameOptions();
    _has_modified_settings = true;

    // Used to let the user visually see the difference with the text scrolling speed.
    _explanation_window.SetDisplaySpeed(SystemManager->GetMessageSpeed());
    _explanation_window.SetText(ustring());
    _UpdateExplanationText();
}

void GameOptionsMenuHandler::_OnDialogueSpeedRight()
{
    SystemManager->SetMessageSpeed(SystemManager->GetMessageSpeed() + 10.0f);
    _RefreshGameOptions();
    _has_modified_settings = true;

    // Used to let the user visually see the difference with the text scrolling speed.
    _explanation_window.SetDisplaySpeed(SystemManager->GetMessageSpeed());
    _explanation_window.SetText(ustring());
    _UpdateExplanationText();
}

void GameOptionsMenuHandler::_OnBattleTargetCursorMemoryConfirm()
{
    SystemManager->SetBattleTargetMemory(!SystemManager->GetBattleTargetMemory());
    _RefreshGameOptions();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnRestoreDefaultKeys()
{
    InputManager->RestoreDefaultKeys();
    _RefreshKeySettings();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnToggleJoystickEnabled()
{
    InputManager->SetJoysticksEnabled(!InputManager->GetJoysticksEnabled());
    if (InputManager->GetJoysticksEnabled())
        InputManager->InitializeJoysticks();
    else
        InputManager->DeinitializeJoysticks();

    _RefreshJoySettings();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnThresholdJoyLeft()
{
    InputManager->SetThresholdJoy(InputManager->GetThresholdJoy() - 100);
    _RefreshJoySettings();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnThresholdJoyRight()
{
    InputManager->SetThresholdJoy(InputManager->GetThresholdJoy() + 100);
    _RefreshJoySettings();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_OnRestoreDefaultJoyButtons()
{
    InputManager->RestoreDefaultJoyButtons();
    _RefreshJoySettings();
    _has_modified_settings = true;
}

void GameOptionsMenuHandler::_ShowMessageWindow(bool joystick)
{
    if(joystick)
        _ShowMessageWindow(WAIT_JOY_BUTTON);
    else
        _ShowMessageWindow(WAIT_KEY);
}

void GameOptionsMenuHandler::_ShowMessageWindow(WAIT_FOR wait)
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

void GameOptionsMenuHandler::_UpdateExplanationText()
{
    // We show the window per default...
    _explanation_window.Show();

    // The default menu
    if (_active_menu == &_options_menu) {
        switch(_options_menu.GetSelection()) {
        case 0:
            _explanation_window.SetText(UTranslate("Here you can set up the video options such as the resolution, whether to play in full screen, the GUI theme, ..."));
            break;
        case 1:
            _explanation_window.SetText(UTranslate("Here you can setup the music and SFX volumes."));
            break;
        case 2:
            _explanation_window.SetText(UTranslate("This menu permits to setup game specific options, such as the text scrolling speed, the game difficulty, ..."));
            break;
        case 3:
            _explanation_window.SetText(UTranslate("This menu will show the available languages. Only available from the boot menu..."));
            break;
        case 4:
            _explanation_window.SetText(UTranslate("Here you change the keys used when playing with the keyboard..."));
            break;
        case 5:
            _explanation_window.SetText(UTranslate("Here you change the keys used when playing with a joystick..."));
            break;
        default:
            _explanation_window.Hide();
            break;
        }
    }
    else if (_active_menu == &_video_options_menu) {
        switch(_video_options_menu.GetSelection()) {
        case 0:
            _explanation_window.SetText(UTranslate("This permits to select the game's resolution..."));
            break;
        case 1:
            _explanation_window.SetText(UTranslate("Here you can set whether you want to play in windowed mode or fullscreen."));
            break;
        case 2:
            _explanation_window.SetText(UTranslate("Sets the game brightness percentage. Default: 50%"));
            break;
        case 3:
            _explanation_window.SetText(UTranslate("Permits to change the Screen Vertical Synchronization mode. (Use the left and right arrow keys.)"));
            break;
        case 4:
            _explanation_window.SetText(UTranslate("Permits to change the Game Update mode. Use 'Performance' for more FPS, and 'Gentle' to be kind with your CPU. Disabled when VSync is on."));
            break;
        case 5:
            _explanation_window.SetText(UTranslate("Permits to change the in-game GUI theme."));
            break;
        default:
            _explanation_window.Hide();
            break;
        }
    }
    else if (_active_menu == &_audio_options_menu) {
        switch(_audio_options_menu.GetSelection()) {
        case 0:
            _explanation_window.SetText(UTranslate("Permits to set the general music volume."));
            break;
        case 1:
            _explanation_window.SetText(UTranslate("Permits to set the general SFX volume."));
            break;
        default:
            _explanation_window.Hide();
            break;
        }
    }
    else if (_active_menu == &_game_options_menu) {
        switch(_game_options_menu.GetSelection()) {
        case 0:
            _explanation_window.SetText(UTranslate("Sets the game difficulty, can change how hard battles and certain events are in game..."));
            break;
        case 1:
            _explanation_window.SetText(UTranslate("Permits to set up the dialogue text scrolling speed (in character per seconds)..."));
            break;
        case 2:
            _explanation_window.SetText(UTranslate("Sets whether the battle target should be kept in memory between two actions for a given character."));
            break;
        default:
            _explanation_window.Hide();
            break;
        }
    }
    else {
        _explanation_window.Hide();
    }
}

bool GameOptionsMenuHandler::_ChangeResolution(int32_t width, int32_t height)
{
    if (VideoManager->GetScreenWidth() == width &&
            VideoManager->GetScreenHeight() == height)
        return false;

    VideoManager->SetResolution(width, height);

    bool ret_value = VideoManager->ApplySettings();
    if (ret_value) {
        _has_modified_settings = true;
    }

    _RefreshVideoOptions();
    _SetupResolutionMenu();

    return ret_value;
}

void GameOptionsMenuHandler::_ReloadGUIDefaultSkin()
{
    _options_window.Destroy();
    _options_window.Create(400.0f, 550.0f);
    _options_window.SetPosition(310.0f, 58.0f);
    _options_window.Show();

    // Setup all menu options and properties
    _SetupOptionsMenu();
    _SetupVideoOptionsMenu();
    _SetupAudioOptionsMenu();
    _SetupLanguageOptionsMenu();
    _SetupKeySettingsMenu();
    _SetupJoySettingsMenu();
    _SetupResolutionMenu();

    _active_menu = &_video_options_menu;

    // Set the option back to the UI theme.
    _video_options_menu.SetSelection(SKIN_MENU_INDEX);

    _RefreshVideoOptions();

    // Reload the explanation and change key windows.
    _message_window.Destroy();
    _message_window.CreateMessageWindow(-1.0f, -1.0f, 410.0f, 133.0f);
    _message_window.Hide();

    _explanation_window.Destroy();
    _explanation_window.CreateMessageWindow(-1.0f, 650.0f, 510.0f, 100.0f);
    _UpdateExplanationText();
}

bool GameOptionsMenuHandler::_SaveSettingsFile(const std::string& filename)
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
        CopyFile(std::string("data/config/settings.lua"), file);

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
    settings_lua.WriteComment("Get the desired VSync mode. 0: No VSync, 1: VSync, 2: Swap Tearing");
    settings_lua.WriteUInt("vsync_mode", VideoManager->GetVSyncMode());
    settings_lua.WriteComment("The game update loop mode. 'false' for a more gentle update loop, 'true' for performance.");
    settings_lua.WriteBool("game_update_mode", VideoManager->GetGameUpdateMode());
    settings_lua.WriteComment("The UI Theme to load.");
    settings_lua.WriteString("ui_theme", GUIManager->GetDefaultMenuSkinId());
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
    settings_lua.WriteInt("minimap", InputManager->GetMinimapKey());
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
    settings_lua.WriteInt("minimap", InputManager->GetMinimapJoy());
    settings_lua.WriteInt("pause", InputManager->GetPauseJoy());
    settings_lua.WriteInt("help", InputManager->GetHelpJoy());
    settings_lua.WriteInt("quit", InputManager->GetQuitJoy());
    settings_lua.EndTable(); // joystick_settings

    // game
    settings_lua.InsertNewLine();
    settings_lua.WriteComment("--Game settings--");
    settings_lua.BeginTable("game_options");
    settings_lua.WriteComment("The game difficulty: 1: Easy, 2: Normal, 3: Hard. (Default: Normal)");
    settings_lua.WriteComment("The difficulty will change how much XP you win and will taint the enemies stats.");
    settings_lua.WriteComment("Certain scripted events may also change according to the current difficulty when entering a new map/battle.");
    settings_lua.WriteUInt("game_difficulty", SystemManager->GetGameDifficulty());

    std::stringstream speed_text("");
    speed_text << "Speed of text displayed in dialogues (in characters per seconds) [1-N] (Default: "
               << vt_gui::DEFAULT_MESSAGE_SPEED << ")";
    settings_lua.WriteComment(speed_text.str());
    settings_lua.WriteInt("message_speed", SystemManager->GetMessageSpeed());
    settings_lua.WriteComment("Whether the latest battle target should be kept in memory between two actions for each characters. (Default: 'true')");
    settings_lua.WriteBool("battle_target_cursor_memory", SystemManager->GetBattleTargetMemory());
    settings_lua.EndTable(); // game_options

    settings_lua.EndTable(); // settings

    // and save it!
    settings_lua.SaveFile();
    settings_lua.CloseFile();

    _has_modified_settings = false;

    return true;
} // bool GameOptionsMenuHandler::_SaveSettingsFile(const std::string& filename)

// ****************************************************************************
// ***** Handler input configuration methods
// ****************************************************************************

void GameOptionsMenuHandler::_RedefineUpKey()
{
    _key_setting_function = &GameOptionsMenuHandler::_SetUpKey;
    _ShowMessageWindow(false);
}

void GameOptionsMenuHandler::_RedefineDownKey()
{
    _key_setting_function = &GameOptionsMenuHandler::_SetDownKey;
    _ShowMessageWindow(false);
}

void GameOptionsMenuHandler::_RedefineLeftKey()
{
    _key_setting_function = &GameOptionsMenuHandler::_SetLeftKey;
    _ShowMessageWindow(false);
}

void GameOptionsMenuHandler::_RedefineRightKey()
{
    _key_setting_function = &GameOptionsMenuHandler::_SetRightKey;
    _ShowMessageWindow(false);
}

void GameOptionsMenuHandler::_RedefineConfirmKey()
{
    _key_setting_function = &GameOptionsMenuHandler::_SetConfirmKey;
    _ShowMessageWindow(false);
}

void GameOptionsMenuHandler::_RedefineCancelKey()
{
    _key_setting_function = &GameOptionsMenuHandler::_SetCancelKey;
    _ShowMessageWindow(false);
}

void GameOptionsMenuHandler::_RedefineMenuKey()
{
    _key_setting_function = &GameOptionsMenuHandler::_SetMenuKey;
    _ShowMessageWindow(false);
}

void GameOptionsMenuHandler::_RedefineMinimapKey()
{
    _key_setting_function = &GameOptionsMenuHandler::_SetMinimapKey;
    _ShowMessageWindow(false);
}

void GameOptionsMenuHandler::_RedefinePauseKey()
{
    _key_setting_function = &GameOptionsMenuHandler::_SetPauseKey;
    _ShowMessageWindow(false);
}

void GameOptionsMenuHandler::_SetUpKey(const SDL_Keycode &key)
{
    InputManager->SetUpKey(key);
}

void GameOptionsMenuHandler::_SetDownKey(const SDL_Keycode &key)
{
    InputManager->SetDownKey(key);
}

void GameOptionsMenuHandler::_SetLeftKey(const SDL_Keycode &key)
{
    InputManager->SetLeftKey(key);
}

void GameOptionsMenuHandler::_SetRightKey(const SDL_Keycode &key)
{
    InputManager->SetRightKey(key);
}

void GameOptionsMenuHandler::_SetConfirmKey(const SDL_Keycode &key)
{
    InputManager->SetConfirmKey(key);
}

void GameOptionsMenuHandler::_SetCancelKey(const SDL_Keycode &key)
{
    InputManager->SetCancelKey(key);
}

void GameOptionsMenuHandler::_SetMenuKey(const SDL_Keycode &key)
{
    InputManager->SetMenuKey(key);
}

void GameOptionsMenuHandler::_SetMinimapKey(const SDL_Keycode &key)
{
    InputManager->SetMinimapKey(key);
}

void GameOptionsMenuHandler::_SetPauseKey(const SDL_Keycode &key)
{
    InputManager->SetPauseKey(key);
}

void GameOptionsMenuHandler::_RedefineXAxisJoy()
{
    _joy_axis_setting_function = &GameOptionsMenuHandler::_SetXAxisJoy;
    _ShowMessageWindow(WAIT_JOY_AXIS);
    InputManager->ResetLastAxisMoved();
}

void GameOptionsMenuHandler::_RedefineYAxisJoy()
{
    _joy_axis_setting_function = &GameOptionsMenuHandler::_SetYAxisJoy;
    _ShowMessageWindow(WAIT_JOY_AXIS);
    InputManager->ResetLastAxisMoved();
}

void GameOptionsMenuHandler::_RedefineConfirmJoy()
{
    _joy_setting_function = &GameOptionsMenuHandler::_SetConfirmJoy;
    _ShowMessageWindow(true);
}

void GameOptionsMenuHandler::_RedefineCancelJoy()
{
    _joy_setting_function = &GameOptionsMenuHandler::_SetCancelJoy;
    _ShowMessageWindow(true);
}

void GameOptionsMenuHandler::_RedefineMenuJoy()
{
    _joy_setting_function = &GameOptionsMenuHandler::_SetMenuJoy;
    _ShowMessageWindow(true);
}

void GameOptionsMenuHandler::_RedefineMinimapJoy()
{
    _joy_setting_function = &GameOptionsMenuHandler::_SetMinimapJoy;
    _ShowMessageWindow(true);
}

void GameOptionsMenuHandler::_RedefinePauseJoy()
{
    _joy_setting_function = &GameOptionsMenuHandler::_SetPauseJoy;
    _ShowMessageWindow(true);
}

void GameOptionsMenuHandler::_RedefineHelpJoy()
{
    _joy_setting_function = &GameOptionsMenuHandler::_SetHelpJoy;
    _ShowMessageWindow(true);
}

void GameOptionsMenuHandler::_RedefineQuitJoy()
{
    _joy_setting_function = &GameOptionsMenuHandler::_SetQuitJoy;
    _ShowMessageWindow(true);
}

void GameOptionsMenuHandler::_SetXAxisJoy(int8_t axis)
{
    InputManager->SetXAxisJoy(axis);
}

void GameOptionsMenuHandler::_SetYAxisJoy(int8_t axis)
{
    InputManager->SetYAxisJoy(axis);
}

void GameOptionsMenuHandler::_SetConfirmJoy(uint8_t button)
{
    InputManager->SetConfirmJoy(button);
}

void GameOptionsMenuHandler::_SetCancelJoy(uint8_t button)
{
    InputManager->SetCancelJoy(button);
}

void GameOptionsMenuHandler::_SetMenuJoy(uint8_t button)
{
    InputManager->SetMenuJoy(button);
}

void GameOptionsMenuHandler::_SetMinimapJoy(uint8_t button)
{
    InputManager->SetMinimapJoy(button);
}

void GameOptionsMenuHandler::_SetPauseJoy(uint8_t button)
{
    InputManager->SetPauseJoy(button);
}

void GameOptionsMenuHandler::_SetHelpJoy(uint8_t button)
{
    InputManager->SetHelpJoy(button);
}

void GameOptionsMenuHandler::_SetQuitJoy(uint8_t button)
{
    InputManager->SetQuitJoy(button);
}

} // namespace private_gui

} // namespace vt_gui
