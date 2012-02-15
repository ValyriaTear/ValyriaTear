///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file   mode_manager.cpp
*** \author Tyler Olsen, roots@allacrost.org
*** \brief  Source file for managing user settings
*** **************************************************************************/

#include "mode_manager.h"
#include "system.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_system;
using namespace hoa_boot;


template<> hoa_mode_manager::ModeEngine* Singleton<hoa_mode_manager::ModeEngine>::_singleton_reference = NULL;

namespace hoa_mode_manager {

ModeEngine* ModeManager = NULL;
bool MODE_MANAGER_DEBUG = false;

// ****************************************************************************
// ***** GameMode class
// ****************************************************************************

GameMode::GameMode() {
	if (MODE_MANAGER_DEBUG) cout << "MODE MANAGER: GameMode constructor invoked" << endl;

	// The value of this member should later be replaced by the child class
	mode_type = MODE_MANAGER_DUMMY_MODE;
}

GameMode::GameMode(uint8 mt) {
	if (MODE_MANAGER_DEBUG) cout << "MODE MANAGER: GameMode constructor invoked" << endl;
	mode_type = mt;
}

GameMode::~GameMode() {
	if (MODE_MANAGER_DEBUG) cout << "MODE MANAGER: GameMode destructor invoked" << endl;
}

// ****************************************************************************
// ***** ModeEngine class
// ****************************************************************************

// This constructor must be defined for the singleton macro
ModeEngine::ModeEngine() {
	if (MODE_MANAGER_DEBUG) cout << "MODE MANAGER: ModeEngine constructor invoked" << endl;
	_pop_count = 0;
	_state_change = false;
}


// The destructor frees all the modes still on the stack
ModeEngine::~ModeEngine() {
	if (MODE_MANAGER_DEBUG) cout << "MODE MANAGER: ModeEngine destructor invoked" << endl;
	// Delete any game modes on the stack
	while (_game_stack.size() != 0) {
		delete _game_stack.back();
		_game_stack.pop_back();
	}

	// Delete any game modes on the push stack
	while (_push_stack.size() != 0) {
		delete _push_stack.back();
		_push_stack.pop_back();
	}
}


// Deletes any game modes on the stack or the push stack and resets all counters
bool ModeEngine::SingletonInitialize() {
	// Delete any game modes on the stack
	while (_game_stack.size() != 0) {
		delete _game_stack.back();
		_game_stack.pop_back();
	}

	// Delete any game modes on the push stack
	while (_push_stack.size() != 0) {
		delete _push_stack.back();
		_push_stack.pop_back();
	}

	// Reset the pop counter
	_pop_count = 0;

	return true;
}


// Free the top mode on the stack and pop it off
void ModeEngine::Pop() {
	_pop_count++;
	_state_change = true;
}


// Pop off all game modes
void ModeEngine::PopAll() {
	_pop_count = static_cast<uint32>(_game_stack.size());
}


// Push a new game mode onto the stack
void ModeEngine::Push(GameMode* gm) {
	_push_stack.push_back(gm);
	_state_change = true;
}


// Returns the mode type of the game mode on the top of the stack
uint8 ModeEngine::GetGameType() {
	if (_game_stack.empty())
		return MODE_MANAGER_DUMMY_MODE;
	else
		return _game_stack.back()->mode_type;
}


// Returns the mode type of a game mode on the stack
uint8 ModeEngine::GetGameType(uint32 index) {
	if (_game_stack.size() < index)
		return MODE_MANAGER_DUMMY_MODE;
	else
		return _game_stack.at(_game_stack.size() - index)->mode_type;
}


// Returns a pointer to the game mode that's currently on the top of the stack
GameMode* ModeEngine::GetTop() {
	if (_game_stack.empty())
		return NULL;
	else
		return _game_stack.back();
}


// Returns a pointer to a game mode on the stack
GameMode* ModeEngine::Get(uint32 index) {
	if (_game_stack.size() < index)
		return NULL;
	else
		return _game_stack.at(_game_stack.size() - index);
}


// Checks if any game modes need to be pushed or popped off the stack, then updates the top stack mode.
void ModeEngine::Update() {
	// If a Push() or Pop() function was called, we need to adjust the state of the game stack.
	if (_state_change == true) {
		// Pop however many game modes we need to from the top of the stack
		while (_pop_count != 0) {
			if (_game_stack.empty()) {
				if (MODE_MANAGER_DEBUG) {
					cerr << "MODE MANAGER WARNING: Tried to pop off more game modes than were on the stack!" << endl;
				}
				break; // Exit the loop
			}
			delete _game_stack.back();
			_game_stack.pop_back();
			_pop_count--;
		}

		// Push any new game modes onto the true game stack.
		while (_push_stack.size() != 0) {
			_game_stack.push_back(_push_stack.back());
			_push_stack.pop_back();
		}

		// Make sure there is a game mode on the stack, otherwise we'll get a segementation fault.
		if (_game_stack.empty()) {
			PRINT_WARNING << "game stack is empty, exiting application" << endl;
			SystemManager->ExitGame();
		}

		// Call the newly active game mode's Reset() function to re-initialize the game mode
		_game_stack.back()->Reset();

		// Reset the state change variable
		_state_change = false;

		// Call the system manager and tell it that the active game mode changed so it can update timers accordingly
		SystemManager->ExamineSystemTimers();

		// Re-initialize the game update timer so that the new active game mode does not begin with any update time to process
		SystemManager->InitializeUpdateTimer();
	} // if (_state_change == true)

	// Call the Update function on the top stack mode (the active game mode)
	_game_stack.back()->Update();
}


// Checks if any game modes need to be pushed or popped off the stack, then updates the top stack mode.
void ModeEngine::Draw() {
	if (_game_stack.size() == 0) {
		return;
	}

	_game_stack.back()->Draw();
}


// Used for debugging purposes ONLY. Prints the contents of the game mode stack.
void ModeEngine::DEBUG_PrintStack() {
	cout << "MODE MANAGER DEBUG: Printing Game Stack" << endl;
	if (_game_stack.size() == 0) {
		cout << "***Game stack is empty!" << endl;
		return;
	}

	cout << "***top of stack***" << endl;
	for (int32 i = static_cast<int32>(_game_stack.size()) - 1; i >= 0; i--)
		cout << " index: " << i << " type: " << _game_stack[i]->mode_type << endl;
	cout << "***bottom of stack***" << endl;
}

} // namespace hoa_mode_manager
