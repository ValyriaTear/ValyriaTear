////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file   system.h
*** \author Tyler Olsen, roots@allacrost.org
*** \author Andy Gardner, chopperdave@allacrost.org
*** \brief  Header file for system code management
***
*** The system code handles a diverse variety of tasks including timing, threads
*** and translation functions.
***
*** \note This code uses the GNU gettext library for internationalization and
*** localization support.
*** ***************************************************************************/

#ifndef __SYSTEM_HEADER__
#define __SYSTEM_HEADER__

#include <set>
#include <SDL/SDL.h>

#include "utils.h"
#include "defs.h"

#include "mode_manager.h"

#define NO_THREADS 0
#define SDL_THREADS 1

/* Set this to NO_THREADS to disable threads. Set this to SDL_THREADS to use
 * SDL Threads. */
#define THREAD_TYPE SDL_THREADS

#if (THREAD_TYPE == SDL_THREADS)
	#include <SDL/SDL_thread.h>
	#include <SDL/SDL_mutex.h>
	typedef SDL_Thread Thread;
	typedef SDL_sem Semaphore;
#else
	typedef int Thread;
	typedef int Semaphore;
#endif

//! All calls to the system engine are wrapped in this namespace.
namespace hoa_system {

//! \brief The singleton pointer responsible for managing the system during game operation.
extern SystemEngine* SystemManager;

//! \brief Determines whether the code in the hoa_system namespace should print debug statements or not.
extern bool SYSTEM_DEBUG;

/** \brief A constant that represents an "infinite" number of milliseconds that can never be reached
*** \note This value is technically not infinite. It is the maximum value of a 32-bit
*** unsigned integer (2^32 - 1). This value will only be reached after ~49.7 consecutive
*** days of the game running.
**/
const uint32 SYSTEM_INFINITE_TIME = 0xFFFFFFFF;

/** \brief A constant to pass to any "loops" function argument in the TimerSystem class
*** Passing this constant to a TimerSystem object will instruct the timer to run indefinitely
*** and never finish.
**/
const int32 SYSTEM_TIMER_NO_LOOPS = 0;

/** \brief A constant to pass to any "loops" function argument in the TimerSystem class
*** Passing this constant to a TimerSystem object will instruct the timer to run indefinitely
*** and never finish.
**/
const int32 SYSTEM_TIMER_INFINITE_LOOP = -1;

//! \brief All of the possible states which a SystemTimer classs object may be in
enum SYSTEM_TIMER_STATE {
	SYSTEM_TIMER_INVALID  = -1,
	SYSTEM_TIMER_INITIAL  =  0,
	SYSTEM_TIMER_RUNNING  =  1,
	SYSTEM_TIMER_PAUSED   =  2,
	SYSTEM_TIMER_FINISHED =  3,
	SYSTEM_TIMER_TOTAL    =  4
};


/** \brief Returns a standard string translated into the game's current language
*** \param text A const reference to the string that should be translated
*** \return Translated text in the form of a std::string
***
*** If no translation exists in the current language for the requested string, the original string
*** will be returned.
**/
std::string Translate(const std::string& text);


/** \brief Returns a ustring translated into the game's current language
*** \param text A const reference to the string that should be translated
*** \return Translated text in the form of a hoa_utils::ustring
***
*** \note This function is nothing more than a short-cut for typing:
*** MakeUnicodeString(Translate(string));
**/
hoa_utils::ustring UTranslate(const std::string& text);


/** ****************************************************************************
*** \brief A timer assistant useful for monitoring progress and processing event sequences
***
*** This is a light-weight class for a simple timer. This class is designed specifically for
*** use by the various game mode classes, but it is certainly capable of being utilized just
*** as effectively by the engine or or other parts of the code. The operation of this class
*** is also integrated with the SystemEngine class, which routinely updates and manages timers.
*** The features of this timing mechanism include:
***
*** - manual update of timer by a specified amount of time
*** - the ability to enable timers to be updated automatically
*** - allowance to loop for an arbitrary number of times, including an infinte number of loops
*** - declaring a timer to be owned by a game mode and enable the timer to be automatically paused/resumed
***
*** When the timer is in the manual update mode, the timer must have its Update() function invoked manually
*** from whatever code is storing/managing the timer. In auto update mode, the timer will update automatically
*** whenever the SystemEngine updates itself in the main game loop. The default mode for timers is manual update.
***
*** \note The auto pausing mechanism can only be utilized by timers that have auto update enabled and are owned
*** by a valid game mode. The way it works is by detecting when the active game mode (AGM) has changed and pausing
*** all timers which are not owned by the AGM and un-pausing all timers which are owned to the AGM.
*** ***************************************************************************/
class SystemTimer {
	friend class SystemEngine; // For allowing SystemEngine to call the _AutoUpdate() method

public:
	/** The no-arg constructor leaves the timer in the SYSTEM_TIMER_INVALID state.
	*** The Initialize() method must be called before the timer can be used.
	**/
	SystemTimer();

	/** \brief Creates and places the timer in the SYSTEM_TIMER_INITIAL state
	*** \param duration The duration (in milliseconds) that the timer should count for
	*** \param loops The number of times that the timer should loop for. Default value is set to no looping.
	**/
	SystemTimer(uint32 duration, int32 loops = 0);

	virtual ~SystemTimer();

	/** \brief Initializes the critical members of the system timer class
	*** \param duration The duration (in milliseconds) that the timer should count for
	*** \param loops The number of times that the timer should loop for. Default value is set to no looping.
	***
	*** Invoking this method will instantly halt the timer and reset it to the initial state so use it with care.
	**/
	void Initialize(uint32 duration, int32 loops = 0);

	/** \brief Enables the auto update feature for the timer
	*** \param owner A pointer to the GameMode which owns this class. Default value is set to NULL (no owner).
	**/
	void EnableAutoUpdate(hoa_mode_manager::GameMode* owner = NULL);

	//! \brief Disables the timer auto update feature
	void EnableManualUpdate();

	//! \brief Updates time timer with the standard game update time
	virtual void Update();

	/** \brief Updates the timer by an arbitrary amount
	*** \param time The amount of time to increment the timer by
	**/
	virtual void Update(uint32 time);

	//! \brief Resets the timer to its initial state
	virtual void Reset()
		{ if (_state != SYSTEM_TIMER_INVALID) { _state = SYSTEM_TIMER_INITIAL; _time_expired = 0; _times_completed = 0; } }

	//! \brief Starts the timer from the initial state or resumes it if it is paused
	void Run()
		{ if (IsInitial() || IsPaused()) _state = SYSTEM_TIMER_RUNNING; }

	//! \brief Pauses the timer if it is running
	void Pause()
		{ if (IsRunning()) _state = SYSTEM_TIMER_PAUSED; }

	//! \brief Sets the timer to the finished state
	void Finish()
		{ _state = SYSTEM_TIMER_FINISHED; }

	//! \name Timer State Checking Functions
	//@{
	bool IsInitial() const
		{ return (_state == SYSTEM_TIMER_INITIAL); }

	bool IsRunning() const
		{ return (_state == SYSTEM_TIMER_RUNNING); }

	bool IsPaused() const
		{ return (_state == SYSTEM_TIMER_PAUSED); }

	bool IsFinished() const
		{ return (_state == SYSTEM_TIMER_FINISHED); }
	//@}

	/** \brief Returns the number of the current loop that the timer is on
	*** This will always return a number greater than zero. So if the timer is on the first loop this
	*** function will return 1, and so on.
	**/
	uint32 CurrentLoop() const
		{ return (_times_completed + 1); }

	//! \brief Returns the time remaining for the current loop to end
	uint32 TimeLeft() const
		{ return (_duration - _time_expired); }

	/** \brief Returns a float representing the percent completion for the current loop
	*** \return A float with a value between 0.0f and 1.0f
	*** \note This function is only concered with the percent completion for the current loop.
	*** The number of loops is not taken into account at all.
	***
	*** This method will return 1.0f if the state is SYSTEM_TIMER_FINISHED or 0.0f if the state
	*** is anything other than SYSTEM_TIMER_RUNNING or SYSTEM_TIMER_PAUSED. The number of loops
	**/
	float PercentComplete() const;

	/** \name Member Set Access Functions
	*** \note <b>Only</b> call these methods when the timer is in its initial state. Trying to set
	*** any of these members when in any other state will yield no change and print a warning message.
	**/
	//@{
	void SetDuration(uint32 duration);

	void SetNumberLoops(int32 loops);

	void SetModeOwner(hoa_mode_manager::GameMode* owner);
	//@}

	//! \name Class Member Accessor Methods
	//@{
	SYSTEM_TIMER_STATE GetState() const
		{ return _state; }

	uint32 GetDuration() const
		{ return _duration; }

	int32 GetNumberLoops() const
		{ return _number_loops; }

	bool IsAutoUpdate() const
		{ return _auto_update; }

	hoa_mode_manager::GameMode* GetModeOwner() const
		{ return _mode_owner; }

	uint32 GetTimeExpired() const
		{ return _time_expired; }

	uint32 GetTimesCompleted() const
		{ return _times_completed; }
	//@}

protected:
	//! \brief Maintains the current state of the timer (initial, running, paused, or finished)
	SYSTEM_TIMER_STATE _state;

	//! \brief When true the timer will automatically update itself
	bool _auto_update;

	//! \brief The duration (in milliseconds) that the timer should run for
	uint32 _duration;

	//! \brief The number of loops the timer should run for. -1 indicates infinite looping.
	int32 _number_loops;

	//! \brief A pointer to the game mode object which owns this timer, or NULL if it is unowned
	hoa_mode_manager::GameMode* _mode_owner;

	//! \brief The amount of time that has expired on the current timer loop (counts up from 0 to _duration)
	uint32 _time_expired;

	//! \brief Incremented by one each time the timer reaches the finished state
	uint32 _times_completed;

	/** \brief Updates the timer if it is running and has auto updating enabled
	*** This method can only be invoked by the SystemEngine class.
	**/
	virtual void _AutoUpdate();

	/** \brief Performs the actual update of the class members
	*** \param amount The amount of time to update the timer by
	***
	*** The function contains the core logic of performing the update for the _time_expired and
	*** _times_completed members as well as setting the _state member to SYSTEM_TIMER_FINISHED
	*** when the timer has completed all of its loops. This is a helper function to the Update()
	*** and _AutoUpdate() methods, who should perform all appropriate checking of timer state
	*** before calling this method. The method intentionally does not do any state or error-checking
	*** by itself; It simply updates the timer without complaint.
	**/
	void _UpdateTimer(uint32 amount);
}; // class SystemTimer


/** ****************************************************************************
*** \brief Engine class that manages system information and functions
***
*** This is somewhat of a "miscellaneous" game engine class that manages constructs
*** that don't really fit in with any other engine component. Perhaps the most
*** important task that this engine component handles is that of timing.
***
*** \note This class is a singleton.
*** ***************************************************************************/
class SystemEngine : public hoa_utils::Singleton<SystemEngine> {
	friend class hoa_utils::Singleton<SystemEngine>;

public:
	~SystemEngine();

	bool SingletonInitialize();

	/** \brief Initializes the timers used in the game
	*** This function should only be called <b>once</b> in main.cpp, just before the main game loop begins.
	**/
	void InitializeTimers();

	/** \brief Initializes the game update timer
	*** This function should typically only be called when the active game mode is changed. This ensures that
	*** the active game mode's execution begins with only 1 millisecond of time expired instead of several.
	**/
	void InitializeUpdateTimer()
		{ _last_update = SDL_GetTicks(); _update_time = 1; }

	/** \brief Adds a timer to the set system timers for auto updating
	*** \param timer A pointer to the timer to add
	***
	*** If the timer object does not have the auto update feature enabled, a warning will be printed and the
	*** timer will not be added.
	**/
	void AddAutoTimer(SystemTimer* timer);

	/** \brief Removes a timer to the set system timers for auto updating
	*** \param timer A pointer to the timer to add
	***
	*** If the timer object does not have the auto update feature enabled, a warning will be printed but it
	*** will still attempt to remove the timer.
	**/
	void RemoveAutoTimer(SystemTimer* timer);

	/** \brief Updates the game timer variables.
	*** This function should only be called <b>once</b> for each cycle through the main game loop. Since
	*** it is called inside the loop in main.cpp, you should have no reason to call this function anywhere
	*** else.
	**/
	void UpdateTimers();

	/** \brief Checks all system timers for whether they should be paused or resumed
	*** This function is typically called whenever the ModeEngine class has changed the active game mode.
	*** When this is done, all system timers that are owned by the active game mode are resumed, all timers with
	*** a different owner are paused, and all timers with no owner are ignored.
	**/
	void ExamineSystemTimers();

	/** \brief Retrieves the amount of time that the game should be updated by for time-based movement.
	*** \return The number of milliseconds that have transpired since the last update.
	***
	*** \note There's a chance we could get errors in other parts of the program code if the
	*** value returned by this function is zero. We can prevent this if we always make sure the
	*** function returns at least one, but I'm not sure there exists a computer fast enough
	*** that we have to worry about it.
	**/
	uint32 GetUpdateTime() const
		{ return _update_time; }

	/** \brief Sets the play time of a game instance
	*** \param h The amount of hours to set.
	*** \param m The amount of minutes to set.
	*** \param s The amount of seconds to set.
	***
	*** This function is meant to be called whenever the user loads a saved game.
	**/
	void SetPlayTime(const uint8 h, const uint8 m, const uint8 s)
		{ _hours_played = h; _minutes_played = m; _seconds_played = s; _milliseconds_played = 0; }

	/** \brief Functions for retrieving the play time.
	*** \return The number of hours, minutes, or seconds of play time.
	**/
	//@{
	uint8 GetPlayHours() const
		{ return _hours_played; }

	uint8 GetPlayMinutes() const
		{ return _minutes_played; }

	uint8 GetPlaySeconds() const
		{ return _seconds_played; }
	//@}

	/** \brief Used to determine what language the game is running in.
	*** \return The language that the game is running in.
	**/
	std::string GetLanguage() const
		{ return _language; }

	/** \brief Sets the language that the game should use.
	*** \param lang A two-character string representing the language to execute the game in
	**/
	void SetLanguage(std::string lang);

	/** \brief Determines whether the user is done with the game.
	*** \return False if the user would like to exit the game.
	**/
	bool NotDone() const
		{ return _not_done; }

	/** \brief The function to call to initialize the exit process of the game.
	*** \note The game will exit the main loop once it reaches the end of its current iteration
	**/
	void ExitGame()
		{ _not_done = false; }

	//! Threading classes
	template <class T> Thread* SpawnThread(void (T::*)(), T *);
	void WaitForThread(Thread* thread);

	void LockThread(Semaphore *);
	void UnlockThread(Semaphore *);
	Semaphore * CreateSemaphore(int max);
	void DestroySemaphore(Semaphore *);


private:
	SystemEngine();

	//! \brief The last time that the UpdateTimers function was called, in milliseconds.
	uint32 _last_update;

	//! \brief The number of milliseconds that have transpired on the last timer update.
	uint32 _update_time;

	/** \name Play time members
	*** \brief Timers that retain the total amount of time that the user has been playing
	*** When the player starts a new game or loads an existing game, these timers are reset.
	**/
	//@{
	uint8 _hours_played;
	uint8 _minutes_played;
	uint8 _seconds_played;
	uint16 _milliseconds_played; //!< \note Milliseconds are not retained when saving or loading a saved game file.
	//@}

	//! \brief When this member is set to false, the program will exit.
	bool _not_done;

	//! \brief The identification string that determines what language the game is running in
	std::string _language;

	/** \brief A set container for all SystemTimer objects that have automatic updating enabled
	*** The timers in this container are updated on each call to UpdateTimers().
	**/
	std::set<SystemTimer*> _auto_system_timers;
}; // class SystemEngine : public hoa_utils::Singleton<SystemEngine>



template <class T> struct generic_class_func_info
{
	static int SpawnThread_Intermediate(void* vptr) {
		((((generic_class_func_info <T> *) vptr)->myclass)->*(((generic_class_func_info <T> *) vptr)->func))();
		return 0;
	}

	T* myclass;
	void (T::*func)();
};



template <class T> Thread* SystemEngine::SpawnThread(void (T::*func)(), T* myclass) {
#if (THREAD_TYPE == SDL_THREADS)
	Thread * thread;
	static generic_class_func_info <T> gen;
	gen.func = func;
	gen.myclass = myclass;

	// Winter Knight: There is a potential, but unlikely race condition here.
	// gen may be overwritten prematurely if this function, SpawnThread, gets
	// called a second time before SpawnThread_Intermediate calls myclass->*func
	// This will result in a segfault.
	thread = SDL_CreateThread(gen.SpawnThread_Intermediate, &gen);
	if (thread == NULL) {
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

} // namepsace hoa_system

#endif // __SYSTEM_HEADER__
