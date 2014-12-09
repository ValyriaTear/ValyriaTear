///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file   mode_manager.cpp
*** \author Tyler Olsen, roots@allacrost.org
*** \author Yohann Ferreira, yohann ferreira orange fr
*** \brief  Source file for managing user settings
*** **************************************************************************/

#include "utils/utils_pch.h"
#include "mode_manager.h"

#include "system.h"

#include "engine/video/video.h"
#include "engine/audio/audio.h"

#include "modes/mode_help_window.h"

using namespace vt_utils;
using namespace vt_system;
using namespace vt_video;
using namespace vt_audio;

namespace vt_mode_manager
{

ModeEngine *ModeManager = NULL;
bool MODE_MANAGER_DEBUG = false;

const uint32 FADE_IN_OUT_TIME = 800;

// ****************************************************************************
// ***** GameMode class
// ****************************************************************************

GameMode::GameMode()
{
    IF_PRINT_WARNING(MODE_MANAGER_DEBUG)
            << "MODE MANAGER: GameMode constructor invoked" << std::endl;

    // The value of this member should later be replaced by the child class
    _mode_type = MODE_MANAGER_DUMMY_MODE;
}


GameMode::GameMode(uint8 mt)
{
    IF_PRINT_WARNING(MODE_MANAGER_DEBUG)
            << "MODE MANAGER: GameMode constructor invoked" << std::endl;
    _mode_type = mt;
}


GameMode::~GameMode()
{
    IF_PRINT_WARNING(MODE_MANAGER_DEBUG)
            << "MODE MANAGER: GameMode destructor invoked" << std::endl;

    // Tells the audio manager that the mode is ending to permit freeing self-managed audio files.
    AudioManager->RemoveGameModeOwner(this);
}


void GameMode::Update()
{
    uint32 frame_time = vt_system::SystemManager->GetUpdateTime();

    _script_supervisor.Update();
    _effect_supervisor.Update(frame_time);
    _particle_manager.Update(frame_time);
    _indicator_supervisor.Update();
}


void GameMode::DrawEffects()
{
    _particle_manager.Draw();
    _effect_supervisor.DrawEffects();
}

void GameMode::DrawPostEffects()
{
}

void GameMode::Deactivate()
{
}

EffectSupervisor& GameMode::GetEffectSupervisor()
{
    return _effect_supervisor;
}

ParticleManager& GameMode::GetParticleManager()
{
    return _particle_manager;
}

ScriptSupervisor& GameMode::GetScriptSupervisor()
{
    return _script_supervisor;
}

IndicatorSupervisor& GameMode::GetIndicatorSupervisor()
{
    return _indicator_supervisor;
}

// ****************************************************************************
// ***** ModeEngine class
// ****************************************************************************

// This constructor must be defined for the singleton macro
ModeEngine::ModeEngine()
{
    IF_PRINT_WARNING(MODE_MANAGER_DEBUG)
            << "MODE MANAGER: ModeEngine constructor invoked" << std::endl;
    _pop_count = 0;
    _state_change = false;
    _fade_out = false;
    _fade_out_finished = false;

    // Initialized in Push()
    _help_window = 0;
}


// The destructor frees all the modes still on the stack
ModeEngine::~ModeEngine()
{
    IF_PRINT_WARNING(MODE_MANAGER_DEBUG)
            << "MODE MANAGER: ModeEngine destructor invoked" << std::endl;
    // Delete any game modes on the stack
    while(!_game_stack.empty()) {
        delete _game_stack.back();
        _game_stack.pop_back();
    }

    // Delete any game modes on the push stack
    while(!_push_stack.empty()) {
        delete _push_stack.back();
        _push_stack.pop_back();
    }

    delete _help_window;
}


// Deletes any game modes on the stack or the push stack and resets all counters
bool ModeEngine::SingletonInitialize()
{
    // Delete any game modes on the stack
    while(!_game_stack.empty()) {
        delete _game_stack.back();
        _game_stack.pop_back();
    }

    // Delete any game modes on the push stack
    while(!_push_stack.empty()) {
        delete _push_stack.back();
        _push_stack.pop_back();
    }

    // Reset the pop counter
    _pop_count = 0;

    return true;
}


// Free the top mode on the stack and pop it off
void ModeEngine::Pop(bool fade_out, bool fade_in)
{
    _fade_in = fade_in;

    _pop_count++;
    _state_change = true;

    if(fade_out) {
        // Start a fade out between the two game modes.
        VideoManager->_StartTransitionFadeOut(Color::black, FADE_IN_OUT_TIME);
        _fade_out = true;
        _fade_out_finished = false;
    } else {
        _fade_out = false;
        _fade_out_finished = true;
    }
}


// Pop off all game modes
void ModeEngine::PopAll()
{
    _pop_count = static_cast<uint32>(_game_stack.size());
}


// Push a new game mode onto the stack
void ModeEngine::Push(GameMode *gm, bool fade_out, bool fade_in)
{
    _push_stack.push_back(gm);

    _state_change = true;

    _fade_in = fade_in;

    if(fade_out) {
        VideoManager->_StartTransitionFadeOut(Color::black, FADE_IN_OUT_TIME);
        _fade_out = true;
        _fade_out_finished = false;
    } else {
        _fade_out = false;
        _fade_out_finished = true;
    }

    // We init the help window only at first game mode push, so give time to
    // every game sub-engine to init properly.
    if(!_help_window)
        _help_window = new HelpWindow();
}


// Returns the mode type of the game mode on the top of the stack
uint8 ModeEngine::GetGameType()
{
    if(_game_stack.empty())
        return MODE_MANAGER_DUMMY_MODE;
    else
        return _game_stack.back()->GetGameType();
}


// Returns the mode type of a game mode on the stack
uint8 ModeEngine::GetGameType(uint32 index)
{
    if(_game_stack.size() < index)
        return MODE_MANAGER_DUMMY_MODE;
    else
        return _game_stack.at(_game_stack.size() - index)->GetGameType();
}


// Returns a pointer to the game mode that's currently on the top of the stack
GameMode *ModeEngine::GetTop()
{
    if(_game_stack.empty())
        return NULL;
    else
        return _game_stack.back();
}


// Returns a pointer to a game mode on the stack
GameMode *ModeEngine::Get(uint32 index)
{
    if(_game_stack.size() < index)
        return NULL;
    else
        return _game_stack.at(_game_stack.size() - index);
}


// Checks if any game modes need to be pushed or popped off the stack, then updates the top stack mode.
void ModeEngine::Update()
{
    // Check whether the fade out is done.
    if(_fade_out && VideoManager->IsLastFadeTransitional() && !VideoManager->IsFading()) {
        _fade_out = false;
        _fade_out_finished = true;
    }

    // If a Push() or Pop() function was called, we need to adjust the state of the game stack.
    if(_fade_out_finished && _state_change) {
        // Pop however many game modes we need to from the top of the stack
        while(_pop_count != 0) {
            if(_game_stack.empty()) {
                PRINT_WARNING << "Tried to pop off more game modes than were on the stack!" << std::endl;
                _pop_count = 0;
                break; // Exit the loop
            }
            delete _game_stack.back();
            _game_stack.pop_back();
            _pop_count--;
        }

        // Push any new game modes onto the true game stack.
        while(!_push_stack.empty()) {
            // Tell the previous game mode about being deactivated.
            if(!_game_stack.empty() && _game_stack.back())
                _game_stack.back()->Deactivate();

            _game_stack.push_back(_push_stack.back());
            _push_stack.pop_back();
        }

        // Make sure there is a game mode on the stack, otherwise we'll get a segmentation fault.
        if(_game_stack.empty()) {
            PRINT_WARNING << "game stack is empty, exiting application" << std::endl;
            SystemManager->ExitGame();
        }

        // Call the newly active game mode's Reset() function to re-initialize the game mode
        _game_stack.back()->Reset();

        // Reset the state change variable
        _state_change = false;

        // Reset the fade out member
        _fade_out_finished = false;

        // We can now fade in, or not
        VideoManager->_TransitionalFadeIn(_fade_in ? FADE_IN_OUT_TIME : 0);

        // Call the system manager and tell it that the active game mode changed so it can update timers accordingly
        SystemManager->ExamineSystemTimers();

        // Re-initialize the game update timer so that the new active game mode does not begin with any update time to process
        SystemManager->InitializeUpdateTimer();
    } // if (_state_change == true)

    // Call the Update function on the top stack mode (the active game mode)
    if(!_game_stack.empty())
        _game_stack.back()->Update();
}


void ModeEngine::Draw()
{
    if(_game_stack.empty())
        return;

    _game_stack.back()->Draw();
}

void ModeEngine::DrawEffects()
{
    if(_game_stack.empty())
        return;

    _game_stack.back()->DrawEffects();
}

void ModeEngine::DrawPostEffects()
{
    if(_game_stack.empty())
        return;

    _game_stack.back()->DrawPostEffects();

    if(_help_window && _help_window->IsActive())
        _help_window->Draw();
}


// Used for debugging purposes ONLY. Prints the contents of the game mode stack.
void ModeEngine::DEBUG_PrintStack()
{
    PRINT_WARNING << "MODE MANAGER DEBUG: Printing Game Stack" << std::endl;
    if(_game_stack.empty()) {
        PRINT_WARNING << "***Game stack is empty!" << std::endl;
        return;
    }

    PRINT_WARNING << "***top of stack***" << std::endl;
    for(int32 i = static_cast<int32>(_game_stack.size()) - 1; i >= 0; i--)
        PRINT_WARNING << " index: " << i << " type: " << _game_stack[i]->GetGameType() << std::endl;
    PRINT_WARNING << "***bottom of stack***" << std::endl;
}

} // namespace vt_mode_manager
