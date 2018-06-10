////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "modes/battle/transition_to_battle.h"

#include "common/global/global.h"

#include "modes/battle/battle.h"
#include "modes/pause.h"

#include "engine/video/video.h"
#include "engine/audio/audio.h"
#include "engine/input.h"
#include "utils/exception.h"
#include "utils/utils_random.h"

using namespace vt_global;
using namespace vt_battle;
using namespace vt_pause;
using namespace vt_mode_manager;
using namespace vt_video;
using namespace vt_audio;
using namespace vt_input;
using namespace vt_system;
using namespace vt_utils;

namespace vt_battle
{

// Available encounter sounds
static const std::string encounter_sound_filenames[] = {
    "data/sounds/battle_encounter_01.ogg",
    "data/sounds/battle_encounter_02.ogg",
    "data/sounds/battle_encounter_03.ogg"
};

// Available encounter sounds
static const std::string boss_encounter_sound_filenames[] = {
    "data/sounds/gong.wav",
    "data/sounds/gong2.wav"
};

TransitionToBattleMode::TransitionToBattleMode(BattleMode* BM, bool is_boss):
    _position(0.0f),
    _is_boss(is_boss),
    _BM(BM)
{
    // Save a copy of the current screen to use as the backdrop.
    try {
        _screen_capture = VideoManager->CaptureScreen();
        _screen_capture.SetDimensions(VIDEO_STANDARD_RES_WIDTH, VIDEO_STANDARD_RES_HEIGHT);
    }
    catch (const Exception &e) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << e.ToString() << std::endl;
    }
}

TransitionToBattleMode::~TransitionToBattleMode() {
    // If the game quits while in pause mode during a transition to battle,
    // The battle mode object needs to be freed.
    if (_BM)
        delete _BM;
}

void TransitionToBattleMode::Update()
{
    // Process quit and pause events
    if(InputManager->QuitPress()) {
        ModeManager->Push(new PauseMode(true));
        return;
    } else if(InputManager->PausePress()) {
        ModeManager->Push(new PauseMode(false));
        return;
    }

    _transition_timer.Update();

    _position += _transition_timer.PercentComplete();

    if(_BM && _transition_timer.IsFinished()) {
        ModeManager->Pop();
        ModeManager->Push(_BM, true, true);
        _BM = nullptr;
    }
}

void TransitionToBattleMode::Draw()
{
    // Draw the battle transition effect.
    int32_t width = VideoManager->GetViewportWidth();
    int32_t height = VideoManager->GetViewportHeight();
    VideoManager->SetCoordSys(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);

    vt_video::DrawCapturedBackgroundImage(_screen_capture, 0.0f, 0.0f);
    vt_video::DrawCapturedBackgroundImage(_screen_capture, _position, _position,
                                          Color(1.0f, 1.0f, 1.0f, 0.3f));
    vt_video::DrawCapturedBackgroundImage(_screen_capture, -_position, _position,
                                          Color(1.0f, 1.0f, 1.0f, 0.3f));
    vt_video::DrawCapturedBackgroundImage(_screen_capture, -_position, -_position,
                                          Color(1.0f, 1.0f, 1.0f, 0.3f));
    vt_video::DrawCapturedBackgroundImage(_screen_capture, _position, -_position,
                                          Color(1.0f, 1.0f, 1.0f, 0.3f));
}

void TransitionToBattleMode::Reset()
{
    // Don't reset a transition in progress
    if (_transition_timer.IsRunning())
        return;

    _position = 0.0f;
    _transition_timer.Initialize(1500, SYSTEM_TIMER_NO_LOOPS);
    _transition_timer.Run();

    // Stop the current map music if it is not the same
    std::string battle_music = GlobalManager->GetBattleMedia().battle_music_filename;
    if (AudioManager->GetActiveMusic() != nullptr &&
        battle_music != AudioManager->GetActiveMusic()->GetFilename())
        AudioManager->GetActiveMusic()->FadeOut(2000);

    // Play a random encounter sound
    if (_is_boss) {
        uint32_t file_id = vt_utils::RandomBoundedInteger(0, 1);
        vt_audio::AudioManager->PlaySound(boss_encounter_sound_filenames[file_id]);
    }
    else {
        uint32_t file_id = vt_utils::RandomBoundedInteger(0, 2);
        vt_audio::AudioManager->PlaySound(encounter_sound_filenames[file_id]);
    }
}

} // namespace vt_battle
