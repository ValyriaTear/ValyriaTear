////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for battle sequence manager.
***
*** Battle sequences are the brief scenes that the player sees when the battle
*** first begins, transitions to the finish menu, or exits. Other types of
*** sequences may also be executed. During a sequence, the player has no control
*** over the events unfolding on the screen.
*** ***************************************************************************/

#ifndef __BATTLE_SEQUENCE_HEADER__
#define __BATTLE_SEQUENCE_HEADER__

#include "defs.h"
#include "utils.h"

#include "audio.h"
#include "mode_manager.h"
#include "script.h"
#include "system.h"
#include "video.h"

#include "global.h"

#include "battle_utils.h"

namespace hoa_battle {

extern bool BATTLE_DEBUG;

//! \brief An internal namespace to be used only within the battle code. Don't use this namespace anywhere else!
namespace private_battle {

/** ****************************************************************************
*** \brief Controls audio and visual elements to produce various sequences
***
*** Certain sequences such as when a battle first begins require a lot of operations that are
*** not required for the remainder of the time that a battle is active. The purpose of this class
*** is to enable those custom operations and prevent the BattleMode class from being inundated
*** with members and methods that are rarely used. This class supports multiple different sequences
*** and thus some members and methods may only pertain to one sequence or another, Other members and methods
*** may be shared among sequences.
***
*** Currently this class supports the following sequences:
***   - Battle initialization
***   - TODO: Transition to finish screen
***   - Battle exit
***
*** \note BattleMode declares this class as a friend, and hence this class has access to all private data and methods of
*** the BattleMode class.
*** ***************************************************************************/
class SequenceSupervisor {
public:
	SequenceSupervisor(BattleMode* current_instance);

	~SequenceSupervisor();

	//! \brief Main function which processes the sequence from its start to its completion
	void Update();

	//! \brief Draws all contents of the battle to the screen appropriately
	void Draw();

private:
	//! \brief A pointer to the active battle mode instance, retained locally in this class only for convience
	BattleMode* _battle;

	//! \brief Used to represent the state of which "step" we are on in producing a particular sequence
	uint32 _sequence_step;

	//! \brief A timer utilized for many different purposes when playing out a sequence
	hoa_system::SystemTimer _sequence_timer;

	//! \brief Color whose alpha value is used for fading background graphics in and out
	hoa_video::Color _background_fade;

	//! \brief A position offset used to move GUI objects from off screen to their permanent positions
	float _gui_position_offset;

	//! \brief Updates state when the battle is in its initial sequence
	void _UpdateInitialSequence();

	//! \brief Updates state when the battle is in its exiting sequence
	void _UpdateExitingSequence();

	//! \brief Main draw function for the initial sequence
	void _DrawInitialSequence();

	//! \brief Main draw function for the exiting sequence
	void _DrawExitingSequence();

	//! \brief Draws the battle background and other environmental graphics
	void _DrawBackgroundGraphics();

	//! \brief Draws all sprites to the screen
	void _DrawSprites();

	//! \brief Draws the bottom battle menu, stamina bar, and other GUI objects
	void _DrawGUI();
}; // class SequenceSupervisor

} // namespace private_battle

} // namespace hoa_battle

#endif // __BATTLE_SEQUENCE_HEADER__
