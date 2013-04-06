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
*** \file    pause.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for pause mode interface.
*** ***************************************************************************/

#include "engine/audio/audio.h"
#include "engine/video/video.h"
#include "engine/input.h"
#include "engine/system.h"
#include "modes/boot/boot.h"
#include "modes/pause.h"

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
    _music_volume(1.0f),
    _dim_color(0.35f, 0.35f, 0.35f, 1.0f), // A grayish opaque color
    _option_selected(false)
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
    _quit_options.SetCursorOffset(-58.0f, -18.0f);

    _quit_options.AddOption(UTranslate("Quit Game"));
    _quit_options.AddOption(UTranslate("Quit to Main Menu"));
    _quit_options.AddOption(UTranslate("Cancel"));
    _quit_options.SetSelection(QUIT_CANCEL);
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
    if(_audio_paused)
        AudioManager->PauseAudio();
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

    // Save a copy of the current screen to use as the backdrop
    try {
        _screen_capture = VideoManager->CaptureScreen();
    } catch(const Exception &e) {
        IF_PRINT_WARNING(PAUSE_DEBUG) << e.ToString() << std::endl;
    }

    _screen_capture.SetWidthKeepRatio(VIDEO_STANDARD_RES_WIDTH);

    VideoManager->SetStandardCoordSys();
    VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
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
            return;
        } else if(InputManager->PausePress()) {
            _option_selected = true;
            ModeManager->Pop();
            return;
        }
    } else { // (_quit_state == true)
        _quit_options.Update();

        if(InputManager->QuitPress()) {
            _option_selected = true;
            ModeManager->Pop();
            return;
        } else if(InputManager->ConfirmPress()) {
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
                mode_type = MODE_MANAGER_DUMMY_MODE;

                ModeManager->Push(new BootMode(), true, true);
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
            _option_selected = true;
            ModeManager->Pop();
            return;
        } else if(InputManager->LeftPress()) {
            _quit_options.InputLeft();
        } else if(InputManager->RightPress()) {
            _quit_options.InputRight();
        }
    }
} // void PauseMode::Update()

void PauseMode::DrawPostEffects()
{
    // Set the coordinate system for the background and draw
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
    VideoManager->Move(0.0f, VIDEO_STANDARD_RES_HEIGHT);
    _screen_capture.Draw(_dim_color);

    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
    VideoManager->Move(512.0f, 384.0f);

    if(!_quit_state) {
        _paused_text.Draw();
    } else {
        _quit_options.Draw();
    }
}

} // namespace vt_pause
