////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_sequence.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for battle sequence manager.
*** ***************************************************************************/

#include "modes/battle/battle_sequence.h"

#include "engine/audio/audio.h"
#include "engine/mode_manager.h"
#include "engine/system.h"
#include "engine/video/video.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actors.h"
#include "modes/battle/battle_utils.h"

#include "common/global/battle_media.h"
#include "common/global/global.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_mode_manager;
using namespace vt_system;
using namespace vt_video;

namespace vt_battle
{

namespace private_battle
{

/** \brief Sequence step constants
***
*** These are used by the SequenceSupervisor methods to progress through each step in a given sequence.
*** These constants should not need to be used in any other area of the battle code.
**/
//@{
static const uint32_t INIT_STEP_BACKGROUND_FADE  =  1;
static const uint32_t INIT_STEP_SPRITE_MOVEMENT  =  2;
static const uint32_t INIT_STEP_GUI_POSITIONING  =  3;

static const uint32_t EXIT_STEP_GUI_POSITIONING  =  1;
static const uint32_t EXIT_STEP_SCREEN_FADE      =  2;
//@}

SequenceSupervisor::SequenceSupervisor(BattleMode *current_instance) :
    _battle(current_instance),
    _one_is_dead(false),
    _sequence_step(0),
    _gui_position_offset(0.0f)
{}

SequenceSupervisor::~SequenceSupervisor()
{}

void SequenceSupervisor::Update()
{
    switch(_battle->_state) {
    case BATTLE_STATE_INITIAL:
        _UpdateInitialSequence();
        break;
    case BATTLE_STATE_EXITING:
        _UpdateExitingSequence();
        break;
    default:
        IF_PRINT_WARNING(BATTLE_DEBUG) << "battle mode was not in a supported sequence state: " << _battle->_state << std::endl;
        _battle->ChangeState(BATTLE_STATE_NORMAL);
        break;
    }
}

void SequenceSupervisor::Draw()
{
    switch(_battle->_state) {
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

void SequenceSupervisor::DrawPostEffects()
{
    switch(_battle->_state) {
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

void SequenceSupervisor::_UpdateInitialSequence()
{
    // Constants that define the time duration of each step in the sequence
    const uint32_t STEP_01_TIME = 500;
    const uint32_t STEP_02_TIME = 500;
    const uint32_t STEP_03_TIME = 500;

    // The furthest position offset we place the GUI objects when bringing them into view
    const float MAX_GUI_OFFSET = 150.0f;
    // The furtherst positions character ane enemy sprites are placed when animating them into view
    const float MAX_CHARACTER_OFFSET = 250.0f;
    const float MAX_ENEMY_OFFSET = 750.0f;

    // Step 0: Initial entry, prepare members for the steps to follow
    if(_sequence_step == 0) {
        // Check whether one character is dead
        _one_is_dead = _battle->isOneCharacterDead();

        _gui_position_offset = MAX_GUI_OFFSET;
        _sequence_timer.Initialize(STEP_01_TIME);
        _sequence_timer.Run();

        // The characters can't run when one of them is dead,
        // as they wouldn't let him behind.
        for(uint32_t i = 0; i < _battle->_character_actors.size(); i++) {
            if(_one_is_dead) {
                _battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin());
                if(_battle->_character_actors[i]->IsAlive())
                    _battle->_character_actors[i]->ChangeSpriteAnimation("idle");
                else
                    _battle->_character_actors[i]->ChangeSpriteAnimation("dead");
            } else {
                _battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin() - MAX_CHARACTER_OFFSET);
                _battle->_character_actors[i]->ChangeSpriteAnimation("run");
            }
        }

        for(uint32_t i = 0; i < _battle->_enemy_actors.size(); i++) {
            _battle->_enemy_actors[i]->SetXLocation(_battle->_enemy_actors[i]->GetXOrigin() + MAX_ENEMY_OFFSET);
        }

        _sequence_step = INIT_STEP_BACKGROUND_FADE;
    }
    // Step 1: Fade in the background graphics
    else if(_sequence_step == INIT_STEP_BACKGROUND_FADE) {
        _sequence_timer.Update();

        if(_sequence_timer.IsFinished()) {
            _sequence_timer.Initialize(STEP_02_TIME);
            _sequence_timer.Run();
            _sequence_step = INIT_STEP_SPRITE_MOVEMENT;
        }
    }
    // Step 2: Move character and enemy sprites from off screen to their positions
    else if(_sequence_step == INIT_STEP_SPRITE_MOVEMENT) {
        _sequence_timer.Update();

        float percent_incomplete = 1.0f - _sequence_timer.PercentComplete();
        if(!_one_is_dead) {
            for(uint32_t i = 0; i < _battle->_character_actors.size(); i++) {
                _battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin() - (MAX_CHARACTER_OFFSET * percent_incomplete));
            }
        }

        for(uint32_t i = 0; i < _battle->_enemy_actors.size(); i++) {
            _battle->_enemy_actors[i]->SetXLocation(_battle->_enemy_actors[i]->GetXOrigin() + (MAX_ENEMY_OFFSET * percent_incomplete));
        }

        if(_sequence_timer.IsFinished()) {
            // Done to ensure that all actors are at their correct locations
            for(uint32_t i = 0; i < _battle->_character_actors.size(); i++) {
                _battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin());
                if(_battle->_character_actors[i]->IsAlive())
                    _battle->_character_actors[i]->ChangeSpriteAnimation("idle");
                else
                    _battle->_character_actors[i]->ChangeSpriteAnimation("dead");
            }
            for(uint32_t i = 0; i < _battle->_enemy_actors.size(); i++) {
                _battle->_enemy_actors[i]->SetXLocation(_battle->_enemy_actors[i]->GetXOrigin());
            }

            _sequence_timer.Initialize(STEP_03_TIME);
            _sequence_timer.Run();
            _sequence_step = INIT_STEP_GUI_POSITIONING;
        }
    }
    // Step 3: Bring in GUI objects from off screen
    else if(_sequence_step == INIT_STEP_GUI_POSITIONING) {
        _sequence_timer.Update();
        _gui_position_offset = MAX_GUI_OFFSET - (_sequence_timer.PercentComplete() * MAX_GUI_OFFSET);

        // Finished with the final step, reset the sequence step counter and begin normal battle state
        if(_sequence_timer.IsFinished()) {
            _sequence_step = 0;
            BattleMode::CurrentInstance()->ChangeState(BATTLE_STATE_NORMAL);
        }
    }
    // If we're in at an unknown step, reset the counter and resume normal battle operation
    else {
        IF_PRINT_DEBUG(BATTLE_DEBUG) << "invalid sequence step counter: " << _sequence_step << std::endl;
        _sequence_step = 0;
        BattleMode::CurrentInstance()->ChangeState(BATTLE_STATE_NORMAL);
    }
}

void SequenceSupervisor::_UpdateExitingSequence()
{
    // Constants that define the time duration of each step in the sequence
    const uint32_t STEP_01_TIME = 500;
    const uint32_t STEP_02_TIME = 1200;

    // The furthest position offset we place the GUI objects when bringing them out of view
    const float GUI_OFFSCREEN_OFFSET = 150.0f;

    // Step 0: Initial entry, prepare members for the steps to follow
    if(_sequence_step == 0) {
        // Check whether one character is dead
        _one_is_dead = _battle->isOneCharacterDead();

        _gui_position_offset = 0.0f;
        _sequence_timer.Initialize(STEP_01_TIME);
        _sequence_timer.Run();

        _sequence_step = EXIT_STEP_GUI_POSITIONING;
    }
    // Step 1: Shift GUI objects off screen
    else if(_sequence_step == EXIT_STEP_GUI_POSITIONING) {
        _sequence_timer.Update();
        _gui_position_offset = GUI_OFFSCREEN_OFFSET * _sequence_timer.PercentComplete();

        if(_sequence_timer.IsFinished()) {
            _sequence_timer.Initialize(STEP_02_TIME);
            _sequence_timer.Run();
            _sequence_step = EXIT_STEP_SCREEN_FADE;

            if(!_one_is_dead) {
                for(uint32_t i = 0; i < _battle->_character_actors.size(); i++) {
                    _battle->_character_actors[i]->ChangeSpriteAnimation("run_after_victory");
                }
            }

            // Trigger a fade out exit state.
            ModeManager->Pop(true, true);
        }
    }
    // Step 2: Run living characters right and fade screen to black
    else if(_sequence_step == EXIT_STEP_SCREEN_FADE) {
        _sequence_timer.Update();

        // Make the character run only if they're all alive.
        if(!_one_is_dead) {
            for(uint32_t i = 0; i < _battle->_character_actors.size(); i++) {
                _battle->_character_actors[i]->SetXLocation(_battle->_character_actors[i]->GetXOrigin() +
                        _sequence_timer.GetTimeExpired());
            }
        }
    }
    // If we're in at an unknown step, restart at sequence zero
    else {
        IF_PRINT_DEBUG(BATTLE_DEBUG) << "invalid sequence step counter: " << _sequence_step << std::endl;
        _sequence_step = 0;
    }
}

void SequenceSupervisor::_DrawInitialSequence()
{
    if(_sequence_step >= INIT_STEP_BACKGROUND_FADE)
        _battle->_DrawBackgroundGraphics();

    if(_one_is_dead || _sequence_step >= INIT_STEP_SPRITE_MOVEMENT)
        _battle->_DrawSprites();

    if(_sequence_step >= INIT_STEP_BACKGROUND_FADE)
        _battle->_DrawForegroundGraphics();
}

void SequenceSupervisor::_DrawInitialSequencePostEffects()
{
    if(_sequence_step >= INIT_STEP_GUI_POSITIONING) {
        _DrawGUI();
    }
}

void SequenceSupervisor::_DrawExitingSequence()
{
    _battle->_DrawBackgroundGraphics();
    _battle->_DrawSprites();
    _battle->_DrawForegroundGraphics();
}

void SequenceSupervisor::_DrawExitingSequencePostEffects()
{
    if(_sequence_step <= EXIT_STEP_GUI_POSITIONING) {
        _DrawGUI();
    }
}

void SequenceSupervisor::_DrawGUI()
{
    vt_global::BattleMedia& battle_media = vt_global::GlobalManager->GetBattleMedia();
    // Draw all images that compose the bottom menu area
    // Draw the static image for the lower menu
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(0.0f, VIDEO_STANDARD_RES_HEIGHT +  _gui_position_offset);
    battle_media.bottom_menu_image.Draw();

    // Draw the stamina bar
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
    VideoManager->Move(STAMINA_BAR_POSITION_X + _gui_position_offset, STAMINA_BAR_POSITION_Y);
    battle_media.stamina_meter.Draw();
}

} // namespace private_battle

} // namespace vt_battle
