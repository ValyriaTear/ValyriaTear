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
*** \file    boot.cpp
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for boot mode interface.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/boot/boot.h"

#include "common/global/global.h"

#include "engine/input.h"
#include "engine/audio/audio.h"

#include "modes/map/map_mode.h"
#include "modes/mode_help_window.h"
#include "modes/save/save_mode.h"

#include "utils/utils_files.h"

using namespace vt_utils;

using namespace vt_audio;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_input;
using namespace vt_mode_manager;
using namespace vt_script;
using namespace vt_system;

using namespace vt_global;

namespace vt_boot
{

bool BOOT_DEBUG = false;

BootMode* BootMode::_current_instance = nullptr;

// ****************************************************************************
// ***** BootMode public methods
// ****************************************************************************

BootMode::BootMode() :
    GameMode(MODE_MANAGER_BOOT_MODE),
    _boot_state(BOOT_STATE_INTRO),
    _exiting_to_new_game(false),
    _new_game_called(false),
    _menu_handler(this),
    _menu_bar_alpha(0.0f),
    _help_text_alpha(0.0f)
{
    _current_instance = this;

#ifdef DEBUG_FEATURES
    _debug_script_menu_open = false;

    // List the debug scripts
    _debug_scripts = vt_utils::ListDirectory("./data/debug", ".lua");

    _debug_script_menu.SetPosition(512.0f, 164.0f);
    _debug_script_menu.SetTextStyle(TextStyle("text24"));
    _debug_script_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _debug_script_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _debug_script_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
    _debug_script_menu.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _debug_script_menu.SetCursorOffset(-50.0f, -28.0f);
    _debug_script_menu.SetSkipDisabled(true);

    _debug_script_menu.ClearOptions();
    float debug_win_height = 28.0f * static_cast<float>(_debug_scripts.size());
    _debug_script_menu.SetDimensions(380.0f, debug_win_height > 600.0f ? 600.0f : debug_win_height,
                                     1, 255, 1, _debug_scripts.size());
    for (std::string file : _debug_scripts)
        _debug_script_menu.AddOption(vt_utils::MakeUnicodeString(file));

    // Create the debug window used as background
    _debug_scripts_window.Create(400.0f, 550.0f);
    _debug_scripts_window.SetPosition(310.0f, 58.0f);
    _debug_scripts_window.Hide();
#endif

    // Remove potential previous ambient overlays
    VideoManager->DisableFadeEffect();

    // Note: Not translated on purpose.
    _version_text.SetStyle(TextStyle("text18"));
    std::string version_string = "Version 1.0.0 - Development version";

    // NOTE: Only leave the " - " part for releases.
    version_string.append(" - ");
    version_string.append(__DATE__);
    _version_text.SetText(MakeUnicodeString(version_string));

    // Get rid of the old table to make sure no old data is used.
    ScriptManager->DropGlobalTable("boot");

    // Test the existence and validity of the boot script.
    ReadScriptDescriptor boot_script;
    if(!boot_script.OpenFile("data/boot_menu/boot.lua")) {
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
    GetScriptSupervisor().AddScript("data/boot_menu/boot.lua");
    GetScriptSupervisor().Initialize(this);

    // Load the menu bar and the help text
    _menu_bar.Load("data/gui/battle/battle_bottom_menu.png", 1024, 128);

    _SetupMainMenu();

    _f1_help_text.SetStyle(TextStyle("text18"));

    // The timer that will be used to display the menu bar and the help text
    _boot_timer.Initialize(14000);
    _boot_timer.EnableManualUpdate();
    _boot_timer.Run();

    // Preload new game sound
    AudioManager->LoadSound("data/sounds/new_game.wav", this);
}

BootMode::~BootMode()
{
    IF_PRINT_WARNING(BOOT_DEBUG)
            << "BOOT: BootMode destructor invoked." << std::endl;
}

void BootMode::Reset()
{
    _current_instance = this;

    // Set the coordinate system that BootMode uses
    VideoManager->SetStandardCoordSys();
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);

    GlobalManager->ClearAllData(); // Resets the game universe to a nullptr state

    GetScriptSupervisor().Reset();
}

void BootMode::Update()
{
    // Update the game mode generic members.
    GameMode::Update();

    if(_exiting_to_new_game) {
        // When the fade out is done, we start a new game.
        if (!VideoManager->IsFading() && _new_game_called == false) {
            GlobalManager->NewGame();
            _new_game_called = true;
        }
        return;
    }

    // The intro is being played
    if(_boot_state == BOOT_STATE_INTRO) {
        if(InputManager->AnyRegisteredKeyPress()) {
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

    HelpWindow *help_window = ModeManager->GetHelpWindow();
    if(help_window && help_window->IsActive()) {
        // Any key, except F1
        if(!InputManager->HelpPress()
                && (InputManager->AnyKeyboardKeyPress() || InputManager->AnyJoystickKeyPress())) {
            GlobalManager->Media().PlaySound("confirm");
            help_window->Hide();
        }
        return;
    }

    // Updates the main menu when the option menu handler isn't active.
    bool is_menu_active = _menu_handler.IsActive();
    if(is_menu_active) {
        bool was_showing_first_run_dlg = _menu_handler.IsShowingFirstRunLanguageMenu();
        _menu_handler.Update();

        // We do this to prevent unwanted input going to the main menu
        // after the first app run sequence.
        if (was_showing_first_run_dlg)
            return;
    }
    else {
        _main_menu.Update();
    }

    // Update also the bar and f1 help text alpha
    uint32_t time_expired = SystemManager->GetUpdateTime();
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

    if (is_menu_active)
        return;

#ifdef DEBUG_FEATURES
    if (_debug_script_menu_open) {
        _debug_script_menu.Update();

        // Check whether we can quit the debug menu.
        if (InputManager->UpPress()) {
            _debug_script_menu.InputUp();
        }
        else if (InputManager->DownPress()) {
            _debug_script_menu.InputDown();
        }
        else if (InputManager->CancelPress()) {
            _debug_script_menu_open = false;
            _debug_scripts_window.Hide();
        }
        else if (InputManager->ConfirmPress()) {
            _DEBUG_OnDebugScriptRun();
        }
        return;
    }
#endif

    // Handles the main menu input.

    // Only quit when we are at the main menu level.
    if(InputManager->QuitPress()) {
        // Don't quit the game when using the joystick,
        // as it is confusing for the user.
        SDL_Event ev = InputManager->GetMostRecentEvent();
        if (ev.type == SDL_KEYDOWN)
            SystemManager->ExitGame();
    }

    if (InputManager->LeftPress()) {
        GlobalManager->Media().PlaySound("bump");
        _main_menu.InputLeft();
        return;
    }
    else if (InputManager->RightPress()) {
        GlobalManager->Media().PlaySound("bump");
        _main_menu.InputRight();
        return;
    }
    else if (!InputManager->ConfirmPress())
        return;

    // Confirm press
    int32_t selection = _main_menu.GetSelection();
    switch(selection) {
    default:
        break;
    case 0: // New Game
        _OnNewGame();
        break;
    case 1:
        _OnLoadGame();
        break;
    case 2:
        _OnOptions();
        break;
#ifdef DEBUG_FEATURES
    // Insert the debug option.
    case 3:
        _DEBUG_OnDebugScriptList();
        break;
    case 4:
        _OnQuit();
        break;
#else
    case 3:
        _OnQuit();
        break;
#endif
    }
}

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
        if (!_menu_handler.IsShowingFirstRunLanguageMenu()) {
            VideoManager->Move(0.0f, 640.0f);
            _menu_bar.Draw(Color(1.0f, 1.0f, 1.0f, _menu_bar_alpha));
        }

        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        VideoManager->Move(322.0f, 730.0f);
        _f1_help_text.Draw(Color(1.0f, 1.0f, 1.0f, _help_text_alpha));

        VideoManager->Move(10.0f, 758.0f);
        _version_text.Draw();

        _main_menu.Draw();

#ifdef DEBUG_FEATURES
        if (_debug_script_menu_open){
            _debug_scripts_window.Draw();
            _debug_script_menu.Draw();
        }
#endif

        if (_menu_handler.IsActive())
            _menu_handler.Draw();
    }
    VideoManager->PopState();
}

// ****************************************************************************
// ***** BootMode menu setup and refresh methods
// ****************************************************************************
bool BootMode::_SavesAvailable()
{
    int32_t savesAvailable = 0;
    std::string data_path = GetUserDataPath();
    uint32_t max_slot_id = SystemManager->GetGameSaveSlots();
    for(uint32_t id = 0; id < max_slot_id; ++id) {
        std::ostringstream f;
        f << data_path + "saved_game_" << id << ".lua";
        const std::string filename = f.str();

        if(DoesFileExist(filename)) {
            ++savesAvailable;
        }
    }
    return (savesAvailable > 0);
}

void BootMode::ReloadTranslatedTexts()
{
    _SetupMainMenu();
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

    _main_menu.AddOption(UTranslate("New Game"));
    _main_menu.AddOption(UTranslate("Load Game"));
    _main_menu.AddOption(UTranslate("Options"));
    // Insert the debug options
#ifdef DEBUG_FEATURES
    _main_menu.SetDimensions(1000.0f, 50.0f, 5, 1, 5, 1);
    _main_menu.AddOption(vt_utils::MakeUnicodeString("Debug scripts"));
    if (_debug_scripts.empty())
        _main_menu.EnableOption(3, false);
#else
    _main_menu.SetDimensions(800.0f, 50.0f, 4, 1, 4, 1);
#endif
    _main_menu.AddOption(UTranslate("Quit"));


    if(!_SavesAvailable()) {
        _main_menu.EnableOption(1, false);
        _main_menu.SetSelection(0);
    } else {
        _main_menu.SetSelection(1);
    }

    _f1_help_text.SetText(VTranslate("Press '%s' to get to know about the game keys.",
                                     InputManager->GetHelpKeyName()));
}

// ****************************************************************************
// ***** BootMode menu handler methods
// ****************************************************************************

void BootMode::_OnNewGame()
{
    AudioManager->StopActiveMusic();
    VideoManager->FadeScreen(Color::black, 2000);

    AudioManager->PlaySound("data/sounds/new_game.wav");
    _exiting_to_new_game = true;
}

void BootMode::_OnLoadGame()
{
    vt_save::SaveMode *SVM = new vt_save::SaveMode(false);
    ModeManager->Push(SVM);
}

void BootMode::_OnOptions()
{
    _menu_handler.Activate();
}

void BootMode::_OnQuit()
{
    SystemManager->ExitGame();
}

#ifdef DEBUG_FEATURES

void BootMode::_DEBUG_OnDebugScriptList()
{
    _debug_script_menu_open = true;
    _debug_scripts_window.Show();
}

void BootMode::_DEBUG_OnDebugScriptRun()
{
    if (_debug_script_menu.GetNumberOptions() == 0)
        return;

    std::string debug_script = "./data/debug/" + _debug_scripts[_debug_script_menu.GetSelection()];
    ReadScriptDescriptor read_data;
    read_data.RunScriptFunction(debug_script, "TestFunction", true);
}

#endif // #ifdef DEBUG_FEATURES

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
    if(settings_lua.ReadInt("first_start") == 1)
        _menu_handler.ShowFirstRunLanguageSelection();

    settings_lua.CloseTable();
    settings_lua.CloseFile();
}

} // namespace vt_boot
