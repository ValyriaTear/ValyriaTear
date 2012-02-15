////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file   system.cpp
*** \author Tyler Olsen, roots@allacrost.org
*** \author Andy Gardner, chopperdave@allacrost.org
*** \brief  Source file for system code management
*** ***************************************************************************/

#ifdef _WIN32
	#include <direct.h>
	#include <stdlib.h>          // defines _MAX_PATH constant
	#ifndef PATH_MAX
	#define PATH_MAX _MAX_PATH   // redefine _MAX_PATH to be compatible with Darwin's PATH_MAX
	#endif
#elif defined __MACH__
	#include <unistd.h>
	#include <cstdlib>
#elif defined __linux__
	#include <limits.h>
#endif

// #include "gettext.h"
#include <libintl.h>

#include "system.h"
#include "audio.h"
#include "script.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_script;
using namespace hoa_mode_manager;

template<> hoa_system::SystemEngine* Singleton<hoa_system::SystemEngine>::_singleton_reference = NULL;

namespace hoa_system {

SystemEngine* SystemManager = NULL;
bool SYSTEM_DEBUG = false;



string Translate(const string& text) {
	// gettext is a C library so the gettext() function takes/returns a C-style char* string
	return string(gettext(text.c_str()));
}



ustring UTranslate(const string& text) {
	return MakeUnicodeString(Translate(text));
}

// -----------------------------------------------------------------------------
// SystemTimer Class
// -----------------------------------------------------------------------------

SystemTimer::SystemTimer() :
	_state(SYSTEM_TIMER_INVALID),
	_auto_update(false),
	_duration(0),
	_number_loops(0),
	_mode_owner(NULL),
	_time_expired(0),
	_times_completed(0)
{}



SystemTimer::SystemTimer(uint32 duration, int32 loops) :
	_state(SYSTEM_TIMER_INITIAL),
	_auto_update(false),
	_duration(duration),
	_number_loops(loops),
	_mode_owner(NULL),
	_time_expired(0),
	_times_completed(0)
{}



SystemTimer::~SystemTimer() {
	if (_auto_update == true) {
		SystemManager->RemoveAutoTimer(this);
	}
}



void SystemTimer::Initialize(uint32 duration, int32 number_loops) {
	_state = SYSTEM_TIMER_INITIAL;
	_duration = duration;
	_number_loops = number_loops;
	_time_expired = 0;
	_times_completed = 0;
}



void SystemTimer::EnableAutoUpdate(GameMode* owner) {
	if (_auto_update == true) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "timer already had auto update enabled" << endl;
		return;
	}

	_auto_update = true;
	_mode_owner = owner;
	SystemManager->AddAutoTimer(this);
}



void SystemTimer::EnableManualUpdate() {
	if (_auto_update == false) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "timer was already in manual update mode" << endl;
		return;
	}

	SystemManager->RemoveAutoTimer(this);
	_auto_update = false;
	_mode_owner = NULL;
}



void SystemTimer::Update() {
	Update(SystemManager->GetUpdateTime());
}



void SystemTimer::Update(uint32 time) {
	if (_auto_update == true) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "update failed because timer is in automatic update mode" << endl;
		return;
	}
	if (IsRunning() == false) {
		return;
	}

	_UpdateTimer(time);
}



float SystemTimer::PercentComplete() const {
	switch (_state) {
		case SYSTEM_TIMER_INITIAL:
			return 0.0f;
		case SYSTEM_TIMER_RUNNING:
		case SYSTEM_TIMER_PAUSED:
			return static_cast<float>(_time_expired) / static_cast<float>(_duration);
		case SYSTEM_TIMER_FINISHED:
			return 1.0f;
		default:
			return 0.0f;
	}
}



void SystemTimer::SetDuration(uint32 duration) {
	if (IsInitial() == false) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "function called when the timer was not in the initial state" << endl;
		return;
	}

	_duration = duration;
}



void SystemTimer::SetNumberLoops(int32 loops) {
	if (IsInitial() == false) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "function called when the timer was not in the initial state" << endl;
		return;
	}

	_number_loops = loops;
}



void SystemTimer::SetModeOwner(hoa_mode_manager::GameMode* owner) {
	if (IsInitial() == false) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "function called when the timer was not in the initial state" << endl;
		return;
	}

	_mode_owner = owner;
}



void SystemTimer::_AutoUpdate() {
	if (_auto_update == false) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "tried to automatically update a timer that does not have auto updates enabled" << endl;
		return;
	}
	if (IsRunning() == false) {
		return;
	}

	_UpdateTimer(SystemManager->GetUpdateTime());
}



void SystemTimer::_UpdateTimer(uint32 time) {
	_time_expired += time;

	if (_time_expired >= _duration) {
		_times_completed++;

		// Check if infinite looping is enabled
		if (_number_loops < 0) {
			_time_expired -= _duration;
		}
		// Check if the last loop has been completed
		else if (_times_completed >= static_cast<uint32>(_number_loops)) {
			_time_expired = 0;
			_state = SYSTEM_TIMER_FINISHED;
		}
		// Otherwise there are still additional loops to complete
		else {
			_time_expired -= _duration;
		}
	}
}

// -----------------------------------------------------------------------------
// SystemEngine Class
// -----------------------------------------------------------------------------

SystemEngine::SystemEngine() {
	IF_PRINT_DEBUG(SYSTEM_DEBUG) << "constructor invoked" << endl;

	_not_done = true;
	SetLanguage("en"); //Default language is English
}



SystemEngine::~SystemEngine() {
	IF_PRINT_DEBUG(SYSTEM_DEBUG) << "destructor invoked" << endl;
}



bool SystemEngine::SingletonInitialize() {
	// Initialize the gettext library
	setlocale(LC_ALL, "");
	setlocale(LC_NUMERIC, "C");

	#if defined(_WIN32) || defined(__MACH__)
		char buffer[PATH_MAX];
		// Get the current working directory.
		string cwd(getcwd(buffer, PATH_MAX));
		cwd.append("/translations/");
		bindtextdomain("allacrost", cwd.c_str());
		bind_textdomain_codeset("allacrost", "UTF-8");
		textdomain("allacrost");
	#elif (defined(__linux__) || defined(__FreeBSD__)) && !defined(RELEASE_BUILD)
		// Look for translation files in LOCALEDIR only if they are not available in the
		// current directory.
		if (ifstream("dat/config/settings.lua") == NULL) {
			bindtextdomain(PACKAGE, LOCALEDIR);
			bind_textdomain_codeset(PACKAGE, "UTF-8");
			textdomain(PACKAGE);
		}
		else {
			char buffer[PATH_MAX];
			// Get the current working directory.
			string cwd(getcwd(buffer, PATH_MAX));
			cwd.append("/txt/");
			bindtextdomain(PACKAGE, cwd.c_str());
			bind_textdomain_codeset(PACKAGE, "UTF-8");
			textdomain(PACKAGE);
		}
	#else
		bindtextdomain(PACKAGE, LOCALEDIR);
		bind_textdomain_codeset(PACKAGE, "UTF-8");
		textdomain(PACKAGE);
	#endif

	// Called here to set the default English language to use nice quote characters.
	SetLanguage("en@quot");

	return true;
}



void SystemEngine::InitializeTimers() {
	_last_update = SDL_GetTicks();
	_update_time = 1; // Set to non-zero, otherwise bad things may happen...
	_hours_played = 0;
	_minutes_played = 0;
	_seconds_played = 0;
	_milliseconds_played = 0;
	_auto_system_timers.clear();
}



void SystemEngine::AddAutoTimer(SystemTimer* timer) {
	if (timer == NULL) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "function received NULL argument" << endl;
		return;
	}
	if (timer->IsAutoUpdate() == false) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "timer did not have auto update feature enabled" << endl;
		return;
	}

// 	pair<set<SystemTimer*>::iterator, bool> return_value;
	if (_auto_system_timers.insert(timer).second == false) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "timer already existed in auto system timer container" << endl;
	}
}



void SystemEngine::RemoveAutoTimer(SystemTimer* timer) {
	if (timer == NULL) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "function received NULL argument" << endl;
		return;
	}
	if (timer->IsAutoUpdate() == false) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "timer did not have auto update feature enabled" << endl;
	}

	if (_auto_system_timers.erase(timer) == 0) {
		IF_PRINT_WARNING(SYSTEM_DEBUG) << "timer was not found in auto system timer container" << endl;
	}
}



void SystemEngine::UpdateTimers() {
	// ----- (1): Update the update game timer
	uint32 tmp = _last_update;
	_last_update = SDL_GetTicks();
	_update_time = _last_update - tmp;

	// ----- (2): Update the game play timer
	_milliseconds_played += _update_time;
	if (_milliseconds_played >= 1000) {
		_seconds_played += _milliseconds_played / 1000;
		_milliseconds_played = _milliseconds_played % 1000;
		if (_seconds_played >= 60) {
			_minutes_played += _seconds_played / 60;
			_seconds_played = _seconds_played % 60;
			if (_minutes_played >= 60) {
				_hours_played += _minutes_played / 60;
				_minutes_played = _minutes_played % 60;
			}
		}
	}

	// ----- (3): Update all SystemTimer objects
	for (set<SystemTimer*>::iterator i = _auto_system_timers.begin(); i != _auto_system_timers.end(); i++)
		(*i)->_AutoUpdate();
}



void SystemEngine::ExamineSystemTimers() {
	GameMode* active_mode = ModeManager->GetTop();
	GameMode* timer_mode = NULL;

	for (set<SystemTimer*>::iterator i = _auto_system_timers.begin(); i != _auto_system_timers.end(); i++) {
		timer_mode = (*i)->GetModeOwner();
		if (timer_mode == NULL)
			continue;

		if (timer_mode == active_mode)
			(*i)->Run();
		else
			(*i)->Pause();
	}
}



void SystemEngine::SetLanguage(std::string lang) {
	_language = lang;

	/// @TODO, implement a cross-platform wrapper for setenv in utils code
	#ifdef _WIN32
		SetEnvironmentVariable("LANGUAGE", _language.c_str());
	#else
		setenv("LANGUAGE", _language.c_str(), 1);
	#endif
}



void SystemEngine::WaitForThread(Thread * thread) {
#if (THREAD_TYPE == SDL_THREADS)
	SDL_WaitThread(thread, NULL);
#endif
}



Semaphore * SystemEngine::CreateSemaphore(int max) {
#if (THREAD_TYPE == SDL_THREADS)
	return SDL_CreateSemaphore(max);
#endif
}



void SystemEngine::DestroySemaphore(Semaphore * s) {
#if (THREAD_TYPE == SDL_THREADS)
	SDL_DestroySemaphore(s);
#endif
}



void SystemEngine::LockThread(Semaphore * s) {
#if (THREAD_TYPE == SDL_THREADS)
	SDL_SemWait(s);
#endif
}



void SystemEngine::UnlockThread(Semaphore * s) {
#if (THREAD_TYPE == SDL_THREADS)
	SDL_SemPost(s);
#endif
}

} // namespace hoa_system
