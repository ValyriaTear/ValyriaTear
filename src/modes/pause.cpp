////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    pause.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for pause mode interface.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/pause.h"

#include "engine/audio/audio.h"
#include "engine/video/video.h"
#include "engine/input.h"
#include "engine/system.h"
#include "modes/boot/boot.h"

#include "common/global/global.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_mode_manager;
using namespace vt_input;
using namespace vt_system;
using namespace vt_boot;

namespace vt_pause
{

bool PAUSE_DEBUG = false;

/** \name Quit Options Menu Constants
*** These constants represent the OptionBox selection indeces of the three different options
*** presented to the player while the _quit_state member is active.
**/
//@{
const uint8 QUIT_CANCEL    = 0;
const uint8 QUIT_OPTIONS   = 1;
const uint8 QUIT_TO_BOOT   = 2;
const uint8 QUIT_GAME      = 3;
//@}

PauseMode::PauseMode(bool quit_state, bool pause_audio) :
    GameMode(MODE_MANAGER_PAUSE_MODE),
    _quit_state(quit_state),
    _audio_paused(pause_audio),
    _music_volume(1.0f),
    _dim_color(0.35f, 0.35f, 0.35f, 1.0f), // A grayish opaque color
    _option_selected(false),
    _options_handler(this)
{
    // Save a copy of the current screen to use as the backdrop.
    try {
        _screen_capture = VideoManager->CaptureScreen();
    }
    catch (const Exception &e) {
        IF_PRINT_WARNING(PAUSE_DEBUG) << e.ToString() << std::endl;
    }

    // Render the paused string in white text
    _paused_text.SetStyle(TextStyle("title28", Color::white, VIDEO_TEXT_SHADOW_BLACK));
    _paused_text.SetText(UTranslate("Paused"));

    // Initialize the quit options box
    _quit_options.SetPosition(512.0f, 384.0f);
    _quit_options.SetDimensions(150.0f, 300.0f, 1, 4, 1, 4);
    _quit_options.SetTextStyle(TextStyle("title24", Color::white, VIDEO_TEXT_SHADOW_BLACK));

    _quit_options.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _quit_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _quit_options.SetSelectMode(VIDEO_SELECT_SINGLE);
    _quit_options.SetCursorOffset(-58.0f, -18.0f);

    _SetupOptions();
}

void PauseMode::_SetupOptions()
{
    _quit_options.ClearOptions();
    _quit_options.AddOption(UTranslate("Cancel"));
    _quit_options.AddOption(UTranslate("Options"));
    _quit_options.AddOption(UTranslate("Quit to Main Menu"));
    _quit_options.AddOption(UTranslate("Quit Game"));
    _quit_options.SetSelection(QUIT_CANCEL);
}

void PauseMode::ReloadTranslatedTexts()
{
    _SetupOptions();
}

PauseMode::~PauseMode()
{
    if(_audio_paused)
        AudioManager->ResumeAudio();
    else {
        MusicDescriptor *active_music = AudioManager->GetActiveMusic();
        if (!active_music)
            return;
        else
            active_music->SetVolume(_music_volume);
    }
}

void PauseMode::Reset()
{
    if (_audio_paused) {
        AudioManager->PauseAudio();
    }
    else {
        MusicDescriptor *active_music = AudioManager->GetActiveMusic();
        if (active_music) {
            _music_volume = active_music->GetVolume();
            if (active_music->GetVolume() > 0.3f)
                active_music->SetVolume(0.3f);
        }
        else {
            _music_volume = 0.0f;
        }
    }

    VideoManager->DisableFadeEffect();
}

void PauseMode::Update()
{
    // If an option has been selected, don't handle input until it has finished.
    if(_option_selected)
        return;

    if(!_quit_state) {
        if(InputManager->QuitPress()) {
            _quit_state = true;
        } else if(InputManager->PausePress()) {
            _option_selected = true;
            ModeManager->Pop();
        }

        return;
    }

    // (_quit_state == true)

    // Handles the options menu
    if (_options_handler.IsActive()) {
        _options_handler.Update();
        return;
    }

    vt_global::GlobalMedia& media = vt_global::GlobalManager->Media();

    _quit_options.Update();

    if(InputManager->QuitPress()) {
        _option_selected = true;
        ModeManager->Pop();
        return;
    } else if(InputManager->ConfirmPress()) {
        media.PlaySound("confirm");
        _option_selected = true;
        switch(_quit_options.GetSelection()) {
        case QUIT_CANCEL:
            ModeManager->Pop();
            break;
        case QUIT_TO_BOOT:
            // Disable potential previous effects
            VideoManager->DisableFadeEffect();
            ModeManager->PopAll();

            // This will permit the fade system to start updating again.
            _mode_type = MODE_MANAGER_DUMMY_MODE;

            ModeManager->Push(new BootMode(), true, true);
            break;
        case QUIT_OPTIONS:
            _option_selected = false;
            _options_handler.Activate();
            break;
        case QUIT_GAME:
            SystemManager->ExitGame();
            break;
        default:
            IF_PRINT_WARNING(PAUSE_DEBUG) << "unknown quit option selected: " << _quit_options.GetSelection() << std::endl;
            break;
        }
        return;
    } else if(InputManager->CancelPress()) {
        media.PlaySound("cancel");
        _option_selected = true;
        ModeManager->Pop();
        return;
    } else if(InputManager->UpPress()) {
        media.PlaySound("bump");
        _quit_options.InputUp();
    } else if(InputManager->DownPress()) {
        media.PlaySound("bump");
        _quit_options.InputDown();
    }
}

void PauseMode::DrawPostEffects()
{
    // Draw the background image. Set the system coordinates to the size of the window (same as the screen backdrop)
    VideoManager->SetCoordSys(0.0f, static_cast<float>(VideoManager->GetViewportWidth()),
                              static_cast<float>(VideoManager->GetViewportHeight()), 0.0f);
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);

    vt_video::DrawCapturedBackgroundImage(_screen_capture, 0.0f, 0.0f, _dim_color);

    VideoManager->SetStandardCoordSys();
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, VIDEO_BLEND, 0);
    VideoManager->Move(512.0f, 384.0f);

    if(!_quit_state) {
        _paused_text.Draw();
    } else {
        _quit_options.Draw();

        if (_options_handler.IsActive())
            _options_handler.Draw();
    }
}

} // namespace vt_pause
