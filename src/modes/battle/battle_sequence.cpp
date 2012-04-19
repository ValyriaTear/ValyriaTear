////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_sequence.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for battle sequence manager.
*** ***************************************************************************/

#include "engine/audio/audio.h"
#include "engine/mode_manager.h"
#include "engine/system.h"
#include "engine/video/video.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actors.h"
#include "modes/battle/battle_sequence.h"
#include "modes/battle/battle_utils.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_mode_manager;
using namespace hoa_system;
using namespace hoa_video;


namespace hoa_battle {

namespace private_battle {

/** \brief Sequence step constants
***
*** These are used by the SequenceSupervisor methods to progress through each step in a given sequence.
*** These constants should not need to be used in any other area of the battle code.
**/
//@{
static const uint32 INIT_STEP_BACKGROUND_FADE  =  1;
static const uint32 INIT_STEP_SPRITE_MOVEMENT  =  2;
static const uint32 INIT_STEP_GUI_POSITIONING  =  3;

static const uint32 EXIT_STEP_GUI_POSITIONING  =  1;
static const uint32 EXIT_STEP_SCREEN_FADE      =  2;
//@}



SequenceSupervisor::SequenceSupervisor(BattleMode* current_instance) :
	_battle(current_instance),
	_one_is_dead(false),
	_sequence_step(0),
	_gui_position_offset(0.0f)
{}



SequenceSupervisor::~SequenceSupervisor()
{}



void SequenceSupervisor::Update() {
	switch (_battle->_state) {
		case BATTLE_STATE_INITIAL:
			_UpdateInitialSequence();
			break;
		case BATTLE_STATE_EXITING:
			_UpdateExitingSequence();
			break;
		default:
			IF_PRINT_WARNING(BATTLE_DEBUG) << "battle mode was not in a supported sequence state: " << _battle->_state << endl;
			_battle->ChangeState(BATTLE_STATE_NORMAL);
			break;
	}

	// Update the animations of all actors
	for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
		_battle->_character_actors[i]->Update(true);
	}
	for (uint32 i = 0; i < _battle->_enemy_actors.size(); i++) {
		_battle->_enemy_actors[i]->Update(true);
	}
}



void SequenceSupervisor::Draw() {
	switch (_battle->_state) {
		case BATTLE_STATE_INITIAL:
			_DrawInitialSequence();
			break;
		case BATTLE_STATE_EXITING:
			_DrawExitingSequence();
			break;
		default:
			break;
	}
}

void SequenceSupervisor::DrawPostEffects() {
	switch (_battle->_state) {
		case BATTLE_STATE_INITIAL:
			_DrawInitialSequencePostEffects();
			break;
		case BATTLE_STATE_EXITING:
			_DrawExitingSequencePostEffects();
			break;
		default:
			break;
	}
}


void SequenceSupervisor::_UpdateInitialSequence() {
	// Constants that define the time duration of each step in the sequence
	const uint32 STEP_01_TIME = 500;
	const uint32 STEP_02_TIME = 500;
	const uint32 STEP_03_TIME = 500;

	// The furthest position offset we place the GUI objects when bringing them into view
	const float MAX_GUI_OFFSET = 150.0f;
	// The furtherst positions character ane enemy sprites are placed when animating them into view
	const float MAX_CHARACTER_OFFSET = 250.0f;
	const float MAX_ENEMY_OFFSET = 750.0f;

	// Step 0: Initial entry, prepare members for the steps to follow
	if (_sequence_step == 0) {
		// Check whether one character is dead
		_one_is_dead = _battle->isOneCharacterDead();

		_gui_position_offset = MAX_GUI_OFFSET;
		_sequence_timer.Initialize(STEP_01_TIME);
		_sequence_timer.Run();

		// The characters can't run when one of them is dead,
		// as they wouldn't let him behind.
		for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
			if (_one_is_dead) {
				_battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin());
				if (_battle->_character_actors[i]->IsAlive())
					_battle->_character_actors[i]->ChangeSpriteAnimation("idle");
				else
					_battle->_character_actors[i]->ChangeSpriteAnimation("dead");
			}
			else {
				_battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin() - MAX_CHARACTER_OFFSET);
				_battle->_character_actors[i]->ChangeSpriteAnimation("run");
			}
		}

		for (uint32 i = 0; i < _battle->_enemy_actors.size(); i++) {
			_battle->_enemy_actors[i]->SetXLocation(_battle->_enemy_actors[i]->GetXOrigin() + MAX_ENEMY_OFFSET);
		}

		_sequence_step = INIT_STEP_BACKGROUND_FADE;
	}
	// Step 1: Fade in the background graphics
	else if (_sequence_step == INIT_STEP_BACKGROUND_FADE) {
		_sequence_timer.Update();

		if (_sequence_timer.IsFinished() == true) {
			_sequence_timer.Initialize(STEP_02_TIME);
			_sequence_timer.Run();
			_sequence_step = INIT_STEP_SPRITE_MOVEMENT;
		}
	}
	// Step 2: Move character and enemy sprites from off screen to their positions
	else if (_sequence_step == INIT_STEP_SPRITE_MOVEMENT) {
		_sequence_timer.Update();

		float percent_incomplete = 1.0f - _sequence_timer.PercentComplete();
		if (!_one_is_dead) {
			for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
				_battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin() - (MAX_CHARACTER_OFFSET * percent_incomplete));
			}
		}

		for (uint32 i = 0; i < _battle->_enemy_actors.size(); i++) {
			_battle->_enemy_actors[i]->SetXLocation(_battle->_enemy_actors[i]->GetXOrigin() + (MAX_ENEMY_OFFSET * percent_incomplete));
		}

		if (_sequence_timer.IsFinished() == true) {
			// Done to ensure that all actors are at their correct locations
			for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
				_battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin());
				if (_battle->_character_actors[i]->IsAlive())
					_battle->_character_actors[i]->ChangeSpriteAnimation("idle");
				else
					_battle->_character_actors[i]->ChangeSpriteAnimation("dead");
			}
			for (uint32 i = 0; i < _battle->_enemy_actors.size(); i++) {
				_battle->_enemy_actors[i]->SetXLocation(_battle->_enemy_actors[i]->GetXOrigin());
			}

			_sequence_timer.Initialize(STEP_03_TIME);
			_sequence_timer.Run();
			_sequence_step = INIT_STEP_GUI_POSITIONING;
		}
	}
	// Step 3: Bring in GUI objects from off screen
	else if (_sequence_step == INIT_STEP_GUI_POSITIONING) {
		_sequence_timer.Update();
		_gui_position_offset = MAX_GUI_OFFSET - (_sequence_timer.PercentComplete() * MAX_GUI_OFFSET);

		// Finished with the final step, reset the sequence step counter and begin normal battle state
		if (_sequence_timer.IsFinished() == true) {
			_sequence_step = 0;
			BattleMode::CurrentInstance()->ChangeState(BATTLE_STATE_NORMAL);
		}
	}
	// If we're in at an unknown step, reset the counter and resume normal battle operation
	else {
		IF_PRINT_DEBUG(BATTLE_DEBUG) << "invalid sequence step counter: " << _sequence_step << endl;
		_sequence_step = 0;
		BattleMode::CurrentInstance()->ChangeState(BATTLE_STATE_NORMAL);
	}
} // void SequenceSupervisor::_UpdateInitialSequence()



void SequenceSupervisor::_UpdateExitingSequence() {
	// Constants that define the time duration of each step in the sequence
	const uint32 STEP_01_TIME = 500;
	const uint32 STEP_02_TIME = 1200;

	// The furthest position offset we place the GUI objects when bringing them out of view
	const float GUI_OFFSCREEN_OFFSET = 150.0f;

	// Step 0: Initial entry, prepare members for the steps to follow
	if (_sequence_step == 0) {
		// Check whether one character is dead
		_one_is_dead = _battle->isOneCharacterDead();

		_gui_position_offset = 0.0f;
		_sequence_timer.Initialize(STEP_01_TIME);
		_sequence_timer.Run();

		_sequence_step = EXIT_STEP_GUI_POSITIONING;
	}
	// Step 1: Shift GUI objects off screen
	else if (_sequence_step == EXIT_STEP_GUI_POSITIONING) {
		_sequence_timer.Update();
		_gui_position_offset = GUI_OFFSCREEN_OFFSET * _sequence_timer.PercentComplete();

		if (_sequence_timer.IsFinished() == true) {
			_sequence_timer.Initialize(STEP_02_TIME);
			_sequence_timer.Run();
			_sequence_step = EXIT_STEP_SCREEN_FADE;

			if (!_one_is_dead) {
				for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
					_battle->_character_actors[i]->ChangeSpriteAnimation("run");
				}
			}

			// Trigger a fade out exit state.
			ModeManager->Pop(true, true);
		}
	}
	// Step 2: Run living characters right and fade screen to black
	else if (_sequence_step == EXIT_STEP_SCREEN_FADE) {
		_sequence_timer.Update();

		// Make the character run only if they're all alive.
		if (!_one_is_dead) {
			for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
				_battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin() +
					_sequence_timer.GetTimeExpired());
			}
		}
	}
	// If we're in at an unknown step, restart at sequence zero
	else {
		IF_PRINT_DEBUG(BATTLE_DEBUG) << "invalid sequence step counter: " << _sequence_step << endl;
		_sequence_step = 0;
	}
}



void SequenceSupervisor::_DrawInitialSequence() {
	if (_sequence_step >= INIT_STEP_BACKGROUND_FADE)
		_battle->_DrawBackgroundGraphics();

	if (_one_is_dead || _sequence_step >= INIT_STEP_SPRITE_MOVEMENT)
		_battle->_DrawSprites();

	if (_sequence_step >= INIT_STEP_BACKGROUND_FADE)
		_battle->_DrawForegroundGraphics();
}

void SequenceSupervisor::_DrawInitialSequencePostEffects() {
	if (_sequence_step >= INIT_STEP_GUI_POSITIONING) {
		_DrawGUI();
	}
}

void SequenceSupervisor::_DrawExitingSequence() {
	_battle->_DrawBackgroundGraphics();
	_battle->_DrawSprites();
	_battle->_DrawForegroundGraphics();
}

void SequenceSupervisor::_DrawExitingSequencePostEffects() {
	if (_sequence_step <= EXIT_STEP_GUI_POSITIONING) {
		_DrawGUI();
	}
}


void SequenceSupervisor::_DrawGUI() {
	// Draw all images that compose the bottom menu area
	// Draw the static image for the lower menu
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
	VideoManager->Move(0.0f, 0.0f -  _gui_position_offset);
	_battle->GetMedia().bottom_menu_image.Draw();

	// Draw the swap icon
	VideoManager->MoveRelative(6.0f, 16.0f);
	_battle->GetMedia().swap_icon.Draw(Color::gray);

	// Determine the draw order of stamina icons for all living actors
	// A container to hold all actors that should have their stamina icons drawn
	vector<BattleActor*> live_actors;

	for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
		if (_battle->_character_actors[i]->IsAlive())
			live_actors.push_back(_battle->_character_actors[i]);
	}
	for (uint32 i = 0; i < _battle->_enemy_actors.size(); i++) {
		if (_battle->_enemy_actors[i]->IsAlive())
			live_actors.push_back(_battle->_enemy_actors[i]);
	}

	vector<float> draw_positions(live_actors.size(), 0.0f);
	for (uint32 i = 0; i < live_actors.size(); i++) {
		switch (live_actors[i]->GetState()) {
			case ACTOR_STATE_IDLE:
				draw_positions[i] = STAMINA_LOCATION_BOTTOM + (STAMINA_LOCATION_COMMAND - STAMINA_LOCATION_BOTTOM) *
					live_actors[i]->GetStateTimer().PercentComplete();
				break;
			default:
				draw_positions[i] = STAMINA_LOCATION_BOTTOM - 50.0f;
				break;
		}
	}

	// TODO: sort the draw positions container

	// Draw the stamina bar
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
	VideoManager->Move(STAMINA_BAR_POSITION_X + _gui_position_offset, STAMINA_BAR_POSITION_Y);
	_battle->GetMedia().stamina_meter.Draw();

	// Draw all stamina icons in order
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
	for (uint32 i = 0; i < live_actors.size(); i++) {
		if (live_actors[i]->IsEnemy() == false)
			VideoManager->Move(STAMINA_BAR_POSITION_X - 25.0f + _gui_position_offset, draw_positions[i]);
		else
			VideoManager->Move(STAMINA_BAR_POSITION_X + 25.0f + _gui_position_offset, draw_positions[i]);
		live_actors[i]->GetStaminaIcon().Draw();
	}
} // void SequenceSupervisor::_DrawGUI()

} // namespace private_battle

} // namespace hoa_battle
