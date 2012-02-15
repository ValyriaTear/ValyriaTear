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

#include "audio.h"
#include "mode_manager.h"
#include "system.h"
#include "video.h"

#include "battle.h"
#include "battle_actors.h"
#include "battle_sequence.h"
#include "battle_utils.h"

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
	_sequence_step(0),
	_background_fade(1.0f, 1.0f, 1.0f, 0.0f),
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
		_background_fade.SetAlpha(0.0f);
		_gui_position_offset = MAX_GUI_OFFSET;
		_sequence_timer.Initialize(STEP_01_TIME);
		_sequence_timer.Run();

		for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
			_battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin() - MAX_CHARACTER_OFFSET);
			_battle->_character_actors[i]->ChangeSpriteAnimation("run");
		}

		for (uint32 i = 0; i < _battle->_enemy_actors.size(); i++) {
			_battle->_enemy_actors[i]->SetXLocation(_battle->_enemy_actors[i]->GetXOrigin() + MAX_ENEMY_OFFSET);
		}

		_sequence_step = INIT_STEP_BACKGROUND_FADE;
	}
	// Step 1: Fade in the background graphics
	else if (_sequence_step == INIT_STEP_BACKGROUND_FADE) {
		_sequence_timer.Update();
		_background_fade.SetAlpha(_sequence_timer.PercentComplete());

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
		for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
			_battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin() - (MAX_CHARACTER_OFFSET * percent_incomplete));
		}

		for (uint32 i = 0; i < _battle->_enemy_actors.size(); i++) {
			_battle->_enemy_actors[i]->SetXLocation(_battle->_enemy_actors[i]->GetXOrigin() + (MAX_ENEMY_OFFSET * percent_incomplete));
		}

		if (_sequence_timer.IsFinished() == true) {
			// Done to ensure that all actors are at their correct locations
			for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
				_battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin());
				_battle->_character_actors[i]->ChangeSpriteAnimation("idle");
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
	const uint32 STEP_02_TIME = 750;

	// The furthest position offset we place the GUI objects when bringing them out of view
	const float GUI_OFFSCREEN_OFFSET = 150.0f;

	// Step 0: Initial entry, prepare members for the steps to follow
	if (_sequence_step == 0) {
		_background_fade.SetAlpha(1.0f);
		_gui_position_offset = 0.0f;
		_sequence_timer.Initialize(STEP_01_TIME);
		_sequence_timer.Run();

		// TEMP: move all enemy sprites off screen so they are not drawn
		for (uint32 i = 0; i < _battle->_enemy_actors.size(); i++) {
			_battle->_enemy_actors[i]->SetXLocation(1500.0f);
		}

		// Restore characters to their idle animations
		for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
			_battle->_character_actors[i]->ChangeSpriteAnimation("idle");
		}

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

			for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
				_battle->_character_actors[i]->ChangeSpriteAnimation("run");
			}
		}
	}
	// Step 2: Run living characters right and fade screen to black
	else if (_sequence_step == EXIT_STEP_SCREEN_FADE) {
		_sequence_timer.Update();
		// TODO: Screen fade doesn't work well right now (its instant instead of gradual). Add fade back in when its functional.
// 		VideoManager->FadeScreen(Color::black, STEP_02_TIME / 2);

		for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
			if (_battle->_character_actors[i]->IsAlive() == true) {
				_battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin() +
					_sequence_timer.GetTimeExpired());
			}
		}

		// Finished with the final step, reset the sequence step counter and exit battle mode
		if (_sequence_timer.IsFinished() == true) {
			_sequence_step = 0;
			ModeManager->Pop();
		}
	}
	// If we're in at an unknown step, restart at sequence zero
	else {
		IF_PRINT_DEBUG(BATTLE_DEBUG) << "invalid sequence step counter: " << _sequence_step << endl;
		_sequence_step = 0;
	}
}



void SequenceSupervisor::_DrawInitialSequence() {
	if (_sequence_step >= INIT_STEP_BACKGROUND_FADE) {
		_DrawBackgroundGraphics();
	}
	if (_sequence_step >= INIT_STEP_SPRITE_MOVEMENT) {
		_DrawSprites();
	}
	if (_sequence_step >= INIT_STEP_GUI_POSITIONING) {
		_DrawGUI();
	}
}



void SequenceSupervisor::_DrawExitingSequence() {
	_DrawBackgroundGraphics();
	_DrawSprites();
	if (_sequence_step <= EXIT_STEP_GUI_POSITIONING) {
		_DrawGUI();
	}
}



void SequenceSupervisor::_DrawBackgroundGraphics() {
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
	VideoManager->Move(0.0f, 0.0f);
	_battle->GetMedia().background_image.Draw(_background_fade);


	// TODO: Draw other background objects and animations
}



void SequenceSupervisor::_DrawSprites() {
	// TODO: Draw sprites in order based on their x and y coordinates on the screen (top to bottom, then left to right)

	// Draw all character sprites
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
	for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
		_battle->_character_actors[i]->DrawSprite();
	}

	// Draw all enemy sprites
	for (uint32 i = 0; i < _battle->_enemy_actors.size(); i++) {
		_battle->_enemy_actors[i]->DrawSprite();
	}
}



void SequenceSupervisor::_DrawGUI() {
	// ----- (1): Draw all images that compose the bottom menu area
	// Draw the static image for the lower menu
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
	VideoManager->Move(0.0f, 0.0f -  _gui_position_offset);
	_battle->GetMedia().bottom_menu_image.Draw();

	// Draw the swap icon
	VideoManager->MoveRelative(6.0f, 16.0f);
	_battle->GetMedia().swap_icon.Draw(Color::gray);

	// TODO: Decide if we want to draw this information during initial sequence or not
// 	// Draw the status information of all character actors
// 	for (uint32 i = 0; i < _battle->_character_actors.size(); i++) {
// 		_battle->_character_actors[i]->DrawStatus(i);
// 	}

	// ----- (2): Determine the draw order of stamina icons for all living actors
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

	//std::vector<bool> selected(live_actors.size(), false);

	vector<float> draw_positions(live_actors.size(), 0.0f);
	for (uint32 i = 0; i < live_actors.size(); i++) {
		switch (live_actors[i]->GetState()) {
			case ACTOR_STATE_IDLE:
				draw_positions[i] = STAMINA_LOCATION_BOTTOM + (STAMINA_LOCATION_COMMAND - STAMINA_LOCATION_BOTTOM) *
					live_actors[i]->GetStateTimer().PercentComplete();
				break;
			default:
				// All other cases are invalid. Instead of printing a debug message that will get echoed every
				// loop, draw the icon at a clearly invalid position well away from the stamina bar
				draw_positions[i] = STAMINA_LOCATION_BOTTOM - 50.0f;
				break;
		}
	}

	// TODO: sort the draw positions container and correspond that to live_actors
// 	sort(draw_positions.begin(), draw_positions.end());

	// ----- (3): Draw the stamina bar
	const float STAMINA_BAR_POSITION_X = 970.0f, STAMINA_BAR_POSITION_Y = 128.0f; // The X and Y position of the stamina bar
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
	VideoManager->Move(STAMINA_BAR_POSITION_X + _gui_position_offset, STAMINA_BAR_POSITION_Y);
	_battle->GetMedia().stamina_meter.Draw();

	// ----- (4): Draw all stamina icons in order
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