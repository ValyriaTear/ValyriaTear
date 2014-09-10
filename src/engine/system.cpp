////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    system.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Andy Gardner, chopperdave@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for system code management
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "engine/system.h"

#include "engine/script/script.h"

#include "utils/utils_strings.h"
#include "utils/utils_files.h"

#include "common/gui/textbox.h"

#include "mode_manager.h"

using namespace vt_utils;
using namespace vt_script;
using namespace vt_mode_manager;

namespace vt_system
{

SystemEngine *SystemManager = NULL;
bool SYSTEM_DEBUG = false;

// If gettext translations are disabled, let's define a dummy gettext.
#ifdef DISABLE_TRANSLATIONS
const char* gettext(const char *text)
{
    return text;
}
#endif

std::string Translate(const std::string &text)
{
    return std::string(gettext(text.c_str()));
}

ustring UTranslate(const std::string &text)
{
    return MakeUnicodeString(Translate(text));
}

// Use: context|text
std::string CTranslate(const std::string &text)
{
    std::string translation = gettext(text.c_str());

    size_t sep_id = translation.find_first_of('|', 0);

    // No separator found or is the last character
    if (sep_id == std::string::npos || sep_id == translation.size())
        return translation;

    return translation.substr(sep_id + 1);
}

ustring CUTranslate(const std::string &text)
{
    return MakeUnicodeString(CTranslate(text));
}

// Inner templated VTranslate functions
template<typename T> std::string _VTranslate(const std::string &text, const T& arg1)
{
    std::string translation = gettext(text.c_str());

    translation = strprintf(translation.c_str(), arg1);

    return translation;
}

template<typename T> std::string _VTranslate(const std::string &text, const T& arg1, const T& arg2)
{
    std::string translation = gettext(text.c_str());

    translation = strprintf(translation.c_str(), arg1, arg2);

    return translation;
}

std::string VTranslate(const std::string &text, int32 arg1)
{ return _VTranslate(text, arg1); }
std::string VTranslate(const std::string &text, uint32 arg1)
{ return _VTranslate(text, arg1); }
std::string VTranslate(const std::string &text, const std::string& arg1)
{ return _VTranslate(text, arg1.c_str()); }
std::string VTranslate(const std::string &text, float arg1)
{ return _VTranslate(text, arg1); }
std::string VTranslate(const std::string &text, uint32 arg1, uint32 arg2)
{ return _VTranslate(text, arg1, arg2); }
std::string VTranslate(const std::string &text, const std::string &arg1, const std::string &arg2)
{ return _VTranslate(text, arg1.c_str(), arg2.c_str()); }


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

SystemTimer::~SystemTimer()
{
    if(_auto_update == true) {
        SystemManager->RemoveAutoTimer(this);
    }
}

void SystemTimer::Initialize(uint32 duration, int32 number_loops)
{
    _state = SYSTEM_TIMER_INITIAL;
    _duration = duration;
    _number_loops = number_loops;
    _time_expired = 0;
    _times_completed = 0;
}

void SystemTimer::EnableAutoUpdate(GameMode *owner)
{
    if(_auto_update == true) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "timer already had auto update enabled" << std::endl;
        return;
    }

    _auto_update = true;
    _mode_owner = owner;
    SystemManager->AddAutoTimer(this);
}

void SystemTimer::EnableManualUpdate()
{
    if(_auto_update == false) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "timer was already in manual update mode" << std::endl;
        return;
    }

    SystemManager->RemoveAutoTimer(this);
    _auto_update = false;
    _mode_owner = NULL;
}

void SystemTimer::Update()
{
    Update(SystemManager->GetUpdateTime());
}

void SystemTimer::Update(uint32 time)
{
    if(_auto_update == true) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "update failed because timer is in automatic update mode" << std::endl;
        return;
    }
    if(IsRunning() == false) {
        return;
    }

    _UpdateTimer(time);
}



float SystemTimer::PercentComplete() const
{
    switch(_state) {
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

void SystemTimer::SetDuration(uint32 duration)
{
    if(IsInitial() == false) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "function called when the timer was not in the initial state" << std::endl;
        return;
    }

    _duration = duration;
}

void SystemTimer::SetTimeExpired(uint32 time_expired)
{
    if (time_expired <= _duration)
        _time_expired = time_expired;
    else
        _time_expired = _duration;
}

void SystemTimer::SetNumberLoops(int32 loops)
{
    if(IsInitial() == false) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "function called when the timer was not in the initial state" << std::endl;
        return;
    }

    _number_loops = loops;
}

void SystemTimer::SetModeOwner(vt_mode_manager::GameMode *owner)
{
    if(IsInitial() == false) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "function called when the timer was not in the initial state" << std::endl;
        return;
    }

    _mode_owner = owner;
}

void SystemTimer::_AutoUpdate()
{
    if(_auto_update == false) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "tried to automatically update a timer that does not have auto updates enabled" << std::endl;
        return;
    }
    if(IsRunning() == false) {
        return;
    }

    _UpdateTimer(SystemManager->GetUpdateTime());
}

void SystemTimer::_UpdateTimer(uint32 time)
{
    _time_expired += time;

    if(_time_expired >= _duration) {
        _times_completed++;

        // Check if infinite looping is enabled
        if(_number_loops < 0) {
            _time_expired -= _duration;
        }
        // Check if the last loop has been completed
        else if(_times_completed >= static_cast<uint32>(_number_loops)) {
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

SystemEngine::SystemEngine():
    _last_update(0),
    _update_time(1), // Set to 1 to avoid hanging the system.
    _hours_played(0),
    _minutes_played(0),
    _seconds_played(0),
    _milliseconds_played(0),
    _not_done(true),
    _message_speed(vt_gui::DEFAULT_MESSAGE_SPEED)
{
    IF_PRINT_DEBUG(SYSTEM_DEBUG) << "constructor invoked" << std::endl;

    SetLanguage("en@quot"); // Default language is English
    _language = "en@quot"; // In case no files were found.
}



SystemEngine::~SystemEngine()
{
    IF_PRINT_DEBUG(SYSTEM_DEBUG) << "destructor invoked" << std::endl;
}


std::string _Reinitl10n()
{
    // Initialize the gettext library
    setlocale(LC_ALL, "");
    setlocale(LC_NUMERIC, "C");

    std::string bind_text_domain_path;

#if defined(_WIN32) || defined(__APPLE__)
    char buffer[PATH_MAX];
    // Get the current working directory.
    bind_text_domain_path = getcwd(buffer, PATH_MAX);
    bind_text_domain_path.append("/po/");

#elif (defined(__linux__) || defined(__FreeBSD__)) && !defined(RELEASE_BUILD)
    // Look for translation files in LOCALEDIR only if they are not available in the
    // current directory.
    if(!vt_utils::DoesFileExist("po/en@quot/LC_MESSAGES/"APPSHORTNAME".mo")) {
        bind_text_domain_path = LOCALEDIR;
    } else {
        char buffer[PATH_MAX];
        // Get the current working directory.
        bind_text_domain_path = getcwd(buffer, PATH_MAX);
        bind_text_domain_path.append("/po/");
    }
#else
    bind_text_domain_path = LOCALEDIR;
#endif
#ifndef DISABLE_TRANSLATIONS
    bindtextdomain(APPSHORTNAME, bind_text_domain_path.c_str());
    bind_textdomain_codeset(APPSHORTNAME, "UTF-8");
    textdomain(APPSHORTNAME);
#endif
    return bind_text_domain_path;
}

bool SystemEngine::IsLanguageAvailable(const std::string& lang)
{
    // Construct the corresponding mo filename path.
    std::string mo_filename = _Reinitl10n();
    mo_filename.append("/");
    mo_filename.append(lang);
    mo_filename.append("/LC_MESSAGES/"APPSHORTNAME".mo");

    // Note: English is always available as it's the default language
    if (lang == "en@quot")
        return true;

    // Test whether the file is existing.
    if (!vt_utils::DoesFileExist(mo_filename))
        return false;

    return true;
}

bool SystemEngine::SetLanguage(const std::string& lang)
{
    // Test whether the file is existing.
    // The function called also reinit the i10n paths
    // so we don't have to do it here.
    if (!IsLanguageAvailable(lang))
        return false;

    _language = lang;
    setlocale(LC_MESSAGES, _language.c_str());
    setlocale(LC_ALL, "");

#ifdef _WIN32
    std::string lang_var = "LANGUAGE=" + _language;
    putenv(lang_var.c_str());
    SetEnvironmentVariableA("LANGUAGE", _language.c_str());
    SetEnvironmentVariableA("LANG", _language.c_str());
#else
    setenv("LANGUAGE", _language.c_str(), 1);
    setenv("LANG", _language.c_str(), 1);
#endif
    return true;
}


bool SystemEngine::SingletonInitialize()
{
    return true;
}


void SystemEngine::InitializeTimers()
{
    _last_update = SDL_GetTicks();
    _update_time = 1; // Set to non-zero, otherwise bad things may happen...
    _hours_played = 0;
    _minutes_played = 0;
    _seconds_played = 0;
    _milliseconds_played = 0;
    _auto_system_timers.clear();
}



void SystemEngine::AddAutoTimer(SystemTimer *timer)
{
    if(timer == NULL) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "function received NULL argument" << std::endl;
        return;
    }
    if(timer->IsAutoUpdate() == false) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "timer did not have auto update feature enabled" << std::endl;
        return;
    }

// 	pair<set<SystemTimer*>::iterator, bool> return_value;
    if(_auto_system_timers.insert(timer).second == false) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "timer already existed in auto system timer container" << std::endl;
    }
}



void SystemEngine::RemoveAutoTimer(SystemTimer *timer)
{
    if(timer == NULL) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "function received NULL argument" << std::endl;
        return;
    }
    if(timer->IsAutoUpdate() == false) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "timer did not have auto update feature enabled" << std::endl;
    }

    if(_auto_system_timers.erase(timer) == 0) {
        IF_PRINT_WARNING(SYSTEM_DEBUG) << "timer was not found in auto system timer container" << std::endl;
    }
}



void SystemEngine::UpdateTimers()
{
    // Update the update game timer
    uint32 tmp = _last_update;
    _last_update = SDL_GetTicks();
    _update_time = _last_update - tmp;

    // Update the game play timer
    _milliseconds_played += _update_time;
    if(_milliseconds_played >= 1000) {
        _seconds_played += _milliseconds_played / 1000;
        _milliseconds_played = _milliseconds_played % 1000;
        if(_seconds_played >= 60) {
            _minutes_played += _seconds_played / 60;
            _seconds_played = _seconds_played % 60;
            if(_minutes_played >= 60) {
                _hours_played += _minutes_played / 60;
                _minutes_played = _minutes_played % 60;
            }
        }
    }

    // Update all SystemTimer objects
    for(std::set<SystemTimer *>::iterator i = _auto_system_timers.begin(); i != _auto_system_timers.end(); ++i)
        (*i)->_AutoUpdate();
}

// Avoid a useless dependency on the mode manager for the editor build
void SystemEngine::ExamineSystemTimers()
{
    GameMode* active_mode = ModeManager->GetTop();

    for(std::set<SystemTimer *>::iterator i = _auto_system_timers.begin(); i != _auto_system_timers.end(); ++i) {
        GameMode* timer_mode = (*i)->GetModeOwner();
        if(timer_mode == NULL)
            continue;

        if(timer_mode == active_mode)
            (*i)->Run();
        else
            (*i)->Pause();
    }
}

void SystemEngine::WaitForThread(Thread *thread)
{
#if (THREAD_TYPE == SDL_THREADS)
    SDL_WaitThread(thread, NULL);
#endif
}


Semaphore *SystemEngine::CreateSemaphore(int max)
{
#if (THREAD_TYPE == SDL_THREADS)
    return SDL_CreateSemaphore(max);
#endif
}



void SystemEngine::DestroySemaphore(Semaphore *s)
{
#if (THREAD_TYPE == SDL_THREADS)
    SDL_DestroySemaphore(s);
#endif
}



void SystemEngine::LockThread(Semaphore *s)
{
#if (THREAD_TYPE == SDL_THREADS)
    SDL_SemWait(s);
#endif
}



void SystemEngine::UnlockThread(Semaphore *s)
{
#if (THREAD_TYPE == SDL_THREADS)
    SDL_SemPost(s);
#endif
}

template <class T> struct generic_class_func_info {
    static int SpawnThread_Intermediate(void *vptr) {
        ((((generic_class_func_info <T> *) vptr)->myclass)->*(((generic_class_func_info <T> *) vptr)->func))();
        return 0;
    }

    T *myclass;
    void (T::*func)();
};



template <class T> Thread *SystemEngine::SpawnThread(void (T::*func)(), T *myclass)
{
#if (THREAD_TYPE == SDL_THREADS)
    Thread *thread;
    static generic_class_func_info <T> gen;
    gen.func = func;
    gen.myclass = myclass;

    // Winter Knight: There is a potential, but unlikely race condition here.
    // gen may be overwritten prematurely if this function, SpawnThread, gets
    // called a second time before SpawnThread_Intermediate calls myclass->*func
    // This will result in a segfault.
    thread = SDL_CreateThread(gen.SpawnThread_Intermediate, &gen);
    if(thread == NULL) {
        PRINT_ERROR << "Unable to create thread: " << SDL_GetError() << std::endl;
        return NULL;
    }
    return thread;
#elif (THREAD_TYPE == NO_THREADS)
    (myclass->*func)();
    return 1;
#else
    PRINT_ERROR << "Invalid THREAD_TYPE." << std::endl;
    return 0;
#endif
}

} // namespace vt_system
