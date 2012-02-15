///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file   mode_manager.h
*** \author Tyler Olsen, roots@allacrost.org
*** \brief  Header file for game mode processingd
*** **************************************************************************/

#ifndef __MODE_MANAGER_HEADER__
#define __MODE_MANAGER_HEADER__

#include "utils.h"
#include "defs.h"

//! All calls to the mode management code are wrapped inside this namespace
namespace hoa_mode_manager {

//! The singleton pointer responsible for maintaining and updating the game mode state.
extern ModeEngine* ModeManager;

//! Determines whether the code in the hoa_mode_manager namespace should print debug statements or not.
extern bool MODE_MANAGER_DEBUG;

//! \name Game States/Modes
//@{
//! \brief Different modes of operation that the game can be in.
const uint8 MODE_MANAGER_DUMMY_MODE  = 0;
const uint8 MODE_MANAGER_BOOT_MODE   = 1;
const uint8 MODE_MANAGER_MAP_MODE    = 2;
const uint8 MODE_MANAGER_BATTLE_MODE = 3;
const uint8 MODE_MANAGER_MENU_MODE   = 4;
const uint8 MODE_MANAGER_SHOP_MODE   = 5;
const uint8 MODE_MANAGER_PAUSE_MODE  = 6;
const uint8 MODE_MANAGER_SCENE_MODE  = 7;
const uint8 MODE_MANAGER_WORLD_MODE  = 8;
const uint8 MODE_MANAGER_SAVE_MODE   = 9;
//@}


/** ***************************************************************************
*** \brief An abstract class that all game mode classes inherit from.
***
*** The GameMode class is the starting base for developing a new mode of operation
*** for the game. The ModeEngine class handles all of the GameMode class
*** objects. One should learn to understand the interaction between these two
*** classes.
***
*** \note THIS IS VERY IMPORTANT. Never, under any circumstances should
*** you ever invoke the delete function on a pointer to this object or its related
*** subclasses. The reason is that all of the memory reference handling is done
*** by the ModeEngine class. If you attempt to ignore this warning you \b will
*** generate a segmentation fault.
*** **************************************************************************/
class GameMode {
	friend class ModeEngine;

protected:
	//! Indicates what 'mode' this object is in (what type of inherited class).
	uint8 mode_type;

private:
	//! Copy constructor is private, because making a copy of a game mode object is a \b bad idea.
	GameMode(const GameMode& other);
	//! Copy assignment operator is private, because making a copy of a game mode object is a \b bad idea.
	GameMode& operator=(const GameMode& other);
	// Note: Should I make the delete and delete[] operators private too?
public:
	GameMode();
	//! \param mt The mode_type to set the new GameMode object to.
	GameMode(uint8 mt);
	//! Destructor is virutal, since the inherited class holds all the important data.
	virtual ~GameMode();

	//! Updates the state of the game mode.
	virtual void Update() = 0;
	//! Draws the next screen frame for the game mode.
	virtual void Draw() = 0;
	/** \brief Resets the state of the class.
	***
	*** This function is called whenever the game mode is made active (ie, it is made the new active game mode
	*** on the top of the game modestack). This includes when the game mode is first created and pushed onto the
	*** game stack, so in that manner it can also be viewed as a helper function to the constructor.
	**/
	virtual void Reset() = 0;
}; // class GameMode


/** ***************************************************************************
*** \brief Manages and maintains all of the living game mode objects.
***
*** The ModeEngine class keeps a stack of GameMode objects, where the object
*** on the top of the stack is the active GameMode (there can only be one active
*** game mode at any time). The Update() and Draw() functions for this class are
*** wrapper calls to the GameMode functions of the same name, and act on the
*** active game mode.
***
*** When a condition is encountered in which a game mode wishes to destroy itself
*** and/or push a new mode onto the stack, this does not occur until the next
*** call to the ModeEngine#Update() function. The GameModeManager#push_stack
*** retains all the game modes we wish to push onto the stack on the next call to
*** ModeEngine#Update(), and the GameModeManager#pop_count member retains
*** how many modes to delete and pop off the ModeEngine#game_stack. Pop
*** operations are \b always performed before push operations.
***
*** \note 1) This class is a singleton.
***
*** \note 2) You might be wondering why the game stack uses a vector container
*** rather than a stack container. There are two reasons: the first being that
*** we can't do a debug printout of the game_stack without removing elements *if*
*** a stack is used. The second reason is "just in case" we need to access a stack
*** element that is not on the top of the stack.
*** **************************************************************************/
class ModeEngine : public hoa_utils::Singleton<ModeEngine> {
	friend class hoa_utils::Singleton<ModeEngine>;

private:
	ModeEngine();

	/** \brief A stack containing all the live game modes.
	*** \note The back/last element of the vector is the top of the stack.
	**/
	std::vector<GameMode*> _game_stack;

	//! A vector of game modes to push to the stack on the next call to ModeEngine#Update().
	std::vector<GameMode*> _push_stack;

	//! True if a state change occured and we need to change the active game mode.
	bool _state_change;

	//! The number of game modes to pop from the back of the stack on the next call to ModeEngine#Update().
	uint32 _pop_count;

public:
	~ModeEngine();

	bool SingletonInitialize();

	//! \brief Increments by one the number of game modes to pop off the stack
	void Pop();

	/** \brief Removes all game modes from the stack on the next call to ModeEngine#Update().
	***
	*** This function sets the ModeEngine#pop_count member to the size of GameModeManager#game_stack.
	*** If there is no game mode in ModeEngine#push_stack before the next call to GameModeManager#Update(),
	*** The game will encounter a segmentation fault and die. Therefore, be careful with this function.
	***
	*** \note Typically this function is only used when the game exits, or when a programmer is smoking crack.
	**/
	void PopAll();

	/** \brief Pushes a new GameMode object on top of the stack.
	*** \param gm The new GameMode object that will go to the top of the stack.
	*** \note This should be obvious, but once you push a new object on the stack
	*** top, it will automatically become the new active game state.
	**/
	void Push(GameMode* gm);

	/**  \brief  Gets the type of the currently active game mode.
	***  \return The value of the mode_type member of the GameMode object on the top of the stack.
	**/
	uint8 GetGameType();

	/**  \brief  Gets the type of a game mode in the stack.
	***  \return The value of the mode_type member of the GameMode object on the top of the stack.
	**/
	uint8 GetGameType(uint32 index);

	/** \brief Gets a pointer to the top game stack object.
	*** \return A pointer to the GameMode object on the top of the stack.
	**/
	GameMode* GetTop();

	/** \brief Gets a pointer to a game stack object.
	*** \return A pointer to the GameMode object at (index) from the top.
	**/
	GameMode* Get(uint32 index);

	//! \brief Checks if the game stack needs modes pushed or popped, then calls Update on the active game mode.
	void Update();

	//! \brief Calls the Draw() function on the active game mode.
	void Draw();

	//! \brief Prints the contents of the game_stack member to standard output.
	void DEBUG_PrintStack();
}; // class ModeEngine : public hoa_utils::Singleton<ModeEngine>

} // namespace hoa_mode_manager

#endif
