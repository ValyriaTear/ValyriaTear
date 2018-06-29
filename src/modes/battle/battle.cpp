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
*** \file    battle.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \author  Corey Hoffstein, visage@allacrost.org
*** \author  Andy Gardner, chopperdave@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for battle mode interface.
*** ***************************************************************************/

#include "modes/battle/battle.h"

#include "common/dialogue.h"

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/mode_manager.h"
#include "script/script.h"
#include "engine/video/video.h"

#include "modes/pause.h"

#include "modes/battle/battle_actors.h"
#include "modes/battle/battle_actions.h"
#include "modes/battle/battle_command.h"
#include "modes/battle/finish/battle_defeat.h"
#include "modes/battle/finish/battle_victory.h"
#include "modes/battle/battle_sequence.h"
#include "modes/battle/battle_utils.h"
#include "modes/battle/battle_effects.h"

#include "common/global/actors/global_attack_point.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_video;
using namespace vt_mode_manager;
using namespace vt_input;
using namespace vt_system;
using namespace vt_global;
using namespace vt_script;
using namespace vt_pause;

using namespace vt_battle::private_battle;

namespace vt_battle
{

bool BATTLE_DEBUG = false;

// Initialize static class variable
BattleMode *BattleMode::_current_instance = nullptr;

namespace private_battle
{

/** \brief Factors which adjust the speed of all actors for the various battle types
*** This adjustment is necessary because a single speed will not work well for the various types,
*** as the player needs more time to select an action in the active type, where the battle action
*** never pauses, and less time in the wait type, where the battle will always pause when the
*** player is selecting an action.
**/
//@{
const float BATTLE_WAIT_FACTOR          = 10.0f;
const float BATTLE_SEMI_ACTIVE_FACTOR   = 6.0f;
const float BATTLE_ACTIVE_FACTOR        = 3.0f;
//@}

//! \brief This is the idle state wait time for the fastest actor, used to set idle state timers for all other actors
const uint32_t MIN_IDLE_WAIT_TIME = 10000;

} // namespace private_battle

////////////////////////////////////////////////////////////////////////////////
// BattleMode class -- primary methods
////////////////////////////////////////////////////////////////////////////////

// Fallback positions for enemies when not set by scripts
const float DEFAULT_ENEMY_LOCATIONS[][2] = {
    { 515.0f, 600.0f },
    { 494.0f, 450.0f },
    { 560.0f, 550.0f },
    { 580.0f, 630.0f },
    { 675.0f, 390.0f },
    { 655.0f, 494.0f },
    { 793.0f, 505.0f },
    { 730.0f, 600.0f }
}; // 8 positions are set [0-7]
const uint32_t NUM_DEFAULT_LOCATIONS = 8;

BattleMode::BattleMode() :
    GameMode(MODE_MANAGER_BATTLE_MODE),
    _state(BATTLE_STATE_INVALID),
    _sequence_supervisor(nullptr),
    _command_supervisor(nullptr),
    _dialogue_supervisor(nullptr),
    _battle_finish(nullptr),
    _current_number_swaps(0),
    _last_enemy_dying(false),
    _stamina_icon_alpha(1.0f),
    _actor_state_paused(false),
    _scene_mode(false),
    _battle_type(BATTLE_TYPE_WAIT),
    _highest_stamina(0),
    _battle_type_time_factor(BATTLE_WAIT_FACTOR),
    _is_boss_battle(false),
    _hero_init_boost(false),
    _enemy_init_boost(false)
{
    _current_instance = this;

    _sequence_supervisor = new SequenceSupervisor(this);
    _command_supervisor = new CommandSupervisor();
    _dialogue_supervisor = new vt_common::DialogueSupervisor();
}

BattleMode::~BattleMode()
{
    delete _sequence_supervisor;
    delete _command_supervisor;
    delete _dialogue_supervisor;
    delete _battle_finish;

    // Delete all character and enemy actors
    for(uint32_t i = 0; i < _character_actors.size(); i++) {
        delete _character_actors[i];
    }
    _character_actors.clear();
    _character_party.clear();

    for(uint32_t i = 0; i < _enemy_actors.size(); i++) {
        delete _enemy_actors[i];
    }
    _enemy_actors.clear();
    _enemy_party.clear();

    _ready_queue.clear();
}

void BattleMode::_ResetMusicState()
{
    // Don't change the music when in the middle of winning a battle
    if (_state == BATTLE_STATE_VICTORY)
        return;

    // Reload if necessary
    std::string battle_music = GlobalManager->GetBattleMedia().battle_music_filename;
    AudioManager->LoadMusic(battle_music, this);
    // Retrieve from audio cache
    MusicDescriptor* music = AudioManager->RetrieveMusic(battle_music);
    MusicDescriptor* active_music = AudioManager->GetActiveMusic();

    // Stop the current music if it's not the right one.
    if (active_music != nullptr && music != active_music) {
        active_music->FadeOut(500);
        if (music)
            music->Rewind();
    }

    // If there is no map music
    if (!music)
        return;

    switch(music->GetState()) {
    // If the music is already in the correct state, don't do anything.
    case AUDIO_STATE_FADE_IN:
    case AUDIO_STATE_PLAYING:
        break;
    case AUDIO_STATE_UNLOADED:
    case AUDIO_STATE_FADE_OUT:
    case AUDIO_STATE_PAUSED:
    case AUDIO_STATE_STOPPED:
    default:
        // In case the music volume was modified, we fade it back in smoothly
        if(music->GetVolume() < 1.0f) {
            music->FadeIn(1000);
        }
        else {
            music->Play();
        }
        break;
    }
}

void BattleMode::Reset()
{
    _current_instance = this;

    VideoManager->SetStandardCoordSys();

    _ResetMusicState();

    if(_state == BATTLE_STATE_INVALID)
        _Initialize();

    // Reset potential battle scripts
    GetScriptSupervisor().Reset();
}

void BattleMode::RestartBattle()
{
    // Can't restart a bettle that hasn't started yet.
    if (GetState() == BATTLE_STATE_INVALID)
        return;

    // Restart potential battle scripts
    GetScriptSupervisor().Restart();

    // Removes all enemies and readd only the ones that were present
    // at the beginning of the battle.
    for(uint32_t i = 0; i < _enemy_actors.size(); ++i)
        delete _enemy_actors[i];

    _enemy_actors.clear();
    _enemy_party.clear();
    _ready_queue.clear();

    for(uint32_t i = 0; i < _initial_enemy_actors_info.size(); ++i)
        AddEnemy(_initial_enemy_actors_info[i].enemy_id,
                 _initial_enemy_actors_info[i].position.x,
                 _initial_enemy_actors_info[i].position.y);

    // Reset the state of all characters and enemies
    for(uint32_t i = 0; i < _character_actors.size(); ++i)
        _character_actors[i]->ResetActor();

    for(uint32_t i = 0; i < _enemy_actors.size(); ++i)
        _enemy_actors[i]->ResetActor();

    // Setup the default actor locations when necessary
    _DetermineActorLocations();

    // Reset battle inventory and available actions
    delete _command_supervisor;
    _command_supervisor = new CommandSupervisor();

    MusicDescriptor* music = AudioManager->RetrieveMusic(GlobalManager->GetBattleMedia().battle_music_filename);
    if(music)
    {
        music->Rewind();
        music->Play();
    }

    ChangeState(BATTLE_STATE_INITIAL);
}

//! \brief Compares the Y-coordinates of the battle objects,
//! It is used to sort them before draw calls.
static bool CompareObjectsYCoord(BattleObject *one, BattleObject *other)
{
    return (one->GetYLocation() < other->GetYLocation());
}

void BattleMode::Update()
{
    // Update potential battle animations
    GlobalManager->GetBattleMedia().Update();
    GameMode::Update();

    // Pause/quit requests take priority
    if(InputManager->QuitPress()) {
        ModeManager->Push(new PauseMode(true));
        return;
    }
    if(InputManager->PausePress()) {
        ModeManager->Push(new PauseMode(false));
        return;
    }

    if (InputManager->MenuPress() && !_scene_mode && (_state != BATTLE_STATE_COMMAND ||
            _command_supervisor->GetState() == COMMAND_STATE_CATEGORY)) {
        GlobalManager->Media().PlaySound("bump");
        _battle_menu.Open();
    }

    _battle_menu.Update();

    if(_dialogue_supervisor->IsDialogueActive())
        _dialogue_supervisor->Update();

    // Update all actors animations and y-sorting
    _battle_objects.clear();
    for(uint32_t i = 0; i < _character_actors.size(); ++i) {
        _character_actors[i]->Update();
        _battle_objects.push_back(_character_actors[i]);
    }
    for(uint32_t i = 0; i < _enemy_actors.size(); ++i) {
        _enemy_actors[i]->Update();
        _battle_objects.push_back(_enemy_actors[i]);
    }

    // Add effects (particles and animations)
    for(std::vector<BattleObject *>::iterator it = _battle_effects.begin();
            it != _battle_effects.end();) {
        if((*it)->CanBeRemoved()) {
            delete (*it);
            it = _battle_effects.erase(it);
        } else {
            (*it)->Update();
            _battle_objects.push_back(*it);
            ++it;
        }
    }

    std::sort(_battle_objects.begin(), _battle_objects.end(), CompareObjectsYCoord);

    // If the battle is in scene mode, we only update animation
    if (_scene_mode)
        return;

    // Now checking standard battle conditions

    // Check whether the last enemy is dying
    if(_last_enemy_dying == false && _NumberValidEnemies() == 0)
        _last_enemy_dying = true;

    // If the battle is transitioning to/from a different mode, the sequence supervisor has control
    if(_state == BATTLE_STATE_INITIAL || _state == BATTLE_STATE_EXITING) {
        _sequence_supervisor->Update();
        return;
    }
    // If the battle is in its typical state and player is not selecting a command, check for player input
    else if(_state == BATTLE_STATE_NORMAL) {
        // Check for victory or defeat conditions
        if(_NumberCharactersAlive() == 0) {
            ChangeState(BATTLE_STATE_DEFEAT);
        } else if(_NumberEnemiesAlive() == 0) {
            ChangeState(BATTLE_STATE_VICTORY);
        }

        // Holds a pointer to the character to select an action for
        BattleCharacter *character_selection = nullptr;

        // The four keys below (up/down/left/right) correspond to each character, from top to bottom. Since the character party
        // does not always have four characters, for all but the first key we have to check that a character exists for the
        // corresponding key. If a character does exist, we then have to check whether or not the player is allowed to select a command
        // for it (characters can only have commands selected during certain states). If command selection is permitted, then we begin
        // the command supervisor.

        if (!_battle_menu.IsOpen()) {
            if (InputManager->UpPress()) {
                GlobalManager->Media().PlaySound("bump");
                if (_character_actors.size() >= 1) {   // Should always evaluate to true
                    character_selection = _character_actors[0];
                }
            }

            else if (InputManager->DownPress()) {
                GlobalManager->Media().PlaySound("bump");
                if (_character_actors.size() >= 2) {
                    character_selection = _character_actors[1];
                }
            }

            else if (InputManager->LeftPress()) {
                GlobalManager->Media().PlaySound("bump");
                if (_character_actors.size() >= 3) {
                    character_selection = _character_actors[2];
                }
            }

            else if (InputManager->RightPress()) {
                GlobalManager->Media().PlaySound("bump");
                if (_character_actors.size() >= 4) {
                    character_selection = _character_actors[3];
                }
            }
        }

        if(!_last_enemy_dying && character_selection) {
            OpenCommandMenu(character_selection);
        }
    }
    // If the player is selecting a command for a character, the command supervisor has control
    else if(_state == BATTLE_STATE_COMMAND) {
        // If the last enemy is dying, there is no need to process command further
        if (!_last_enemy_dying) {
            // If auto-battle is active, cancel the current character command.
            if (_battle_menu.IsAutoBattleActive())
                _command_supervisor->CancelCurrentCommand();
            else if (!_battle_menu.IsOpen())
                _command_supervisor->Update();
        }
        else
            ChangeState(BATTLE_STATE_NORMAL);
    }
    // If the battle is in either finish state, the finish supervisor has control
    else if((_state == BATTLE_STATE_VICTORY) || (_state == BATTLE_STATE_DEFEAT)) {
        if (_battle_menu.IsOpen())
            _battle_menu.Close();

        if (_battle_finish)
            _battle_finish->Update();

        // Make the heroes and/or enemies stamina icons fade out
        if(_stamina_icon_alpha > 0.0f) {
            _stamina_icon_alpha -= (float)SystemManager->GetUpdateTime() / 800.0f;

            // Also use it to create a fade to red effect on defeats
            if(_state == BATTLE_STATE_DEFEAT)
                GetEffectSupervisor().EnableLightingOverlay(Color(0.2f, 0.0f, 0.0f, (1.0f - _stamina_icon_alpha) * 0.6f));
        }

        return;
    }

    // If the battle is running in the "wait" setting and a character reaches the command state,
    // we want to open the command menu for that character.
    // The battle will be paused until the player enters a command for all characters
    // that are in command state.
    if(!_last_enemy_dying) {
        for(uint32_t i = 0; i < _character_actors.size(); ++i) {
            if(_character_actors[i]->GetState() == ACTOR_STATE_COMMAND) {
                if (_battle_menu.IsAutoBattleActive()) {
                    _AutoCharacterCommand(_character_actors[i]);
                }
                else if(_state != BATTLE_STATE_COMMAND) {
                    if (_battle_type == BATTLE_TYPE_WAIT || _battle_type == BATTLE_TYPE_SEMI_ACTIVE)
                        OpenCommandMenu(_character_actors[i]);
                }
            }
        }
    }

    // Process the actor ready queue
    if(!_last_enemy_dying && !_ready_queue.empty()) {
        // Only the acting actor is examined in the ready queue. If this actor is in the READY state,
        // that means it has been waiting for BattleMode to allow it to begin its action and thus
        // we set it to the ACTING state. We do nothing while it is in the ACTING state, allowing the
        // actor to completely finish its action. When the actor enters any other state, it is presumed
        // to be finished with the action or otherwise incapacitated and is removed from the queue.
        BattleActor *acting_actor = _ready_queue.front();
        switch(acting_actor->GetState()) {
        case ACTOR_STATE_READY:
            acting_actor->ChangeState(ACTOR_STATE_SHOWNOTICE);
            break;
        case ACTOR_STATE_NOTICEDONE:
            acting_actor->ChangeState(ACTOR_STATE_ACTING);
            break;
        case ACTOR_STATE_SHOWNOTICE:
        case ACTOR_STATE_ACTING:
            break;
        default:
            _ready_queue.pop_front();
            break;
        }
    }
} // void BattleMode::Update()

void BattleMode::Draw()
{
    VideoManager->SetStandardCoordSys();

    if(_state == BATTLE_STATE_INITIAL || _state == BATTLE_STATE_EXITING) {
        _sequence_supervisor->Draw();
        return;
    }

    _DrawBackgroundGraphics();
    _DrawSprites();
    _DrawForegroundGraphics();
}

void BattleMode::DrawPostEffects()
{
    VideoManager->SetStandardCoordSys();

    GetScriptSupervisor().DrawPostEffects();

    if(_state == BATTLE_STATE_INITIAL || _state == BATTLE_STATE_EXITING) {
        _sequence_supervisor->DrawPostEffects();
        return;
    }

    _DrawGUI();
}

////////////////////////////////////////////////////////////////////////////////
// BattleMode class -- secondary methods
////////////////////////////////////////////////////////////////////////////////

void BattleMode::AddEnemy(uint32_t new_enemy_id, float position_x, float position_y)
{
    // Check for the enemy data validity
    if (!vt_global::GlobalManager->DoesEnemyExist(new_enemy_id)) {
        PRINT_WARNING << "Attempted to add a new enemy with an invalid id: "
                      << new_enemy_id << std::endl;
        return;
    }

    BattleEnemy* new_battle_enemy = new BattleEnemy(new_enemy_id);

    // Compute a position when needed.
    if (position_x == 0.0f && position_y == 0.0f) {
        uint32_t default_pos_id = _enemy_actors.size();
        position_x = DEFAULT_ENEMY_LOCATIONS[default_pos_id % NUM_DEFAULT_LOCATIONS][0];
        position_y = DEFAULT_ENEMY_LOCATIONS[default_pos_id % NUM_DEFAULT_LOCATIONS][1];
        // Add an artifical offset when cycling within the default positions.
        // This will permit to still see all the enemies, even when there are more than 8 of them.
        if(default_pos_id > NUM_DEFAULT_LOCATIONS - 1) {
            position_x += default_pos_id * 3;
            position_y += default_pos_id * 3;
        }
    }

    // Set the battleground position
    new_battle_enemy->SetXLocation(position_x);
    new_battle_enemy->SetYLocation(position_y);
    new_battle_enemy->SetXOrigin(position_x);
    new_battle_enemy->SetYOrigin(position_y);

    _enemy_actors.push_back(new_battle_enemy);
    _enemy_party.push_back(new_battle_enemy);

    // Sort the enemies based on their Y location.
    // The player will then be able to target them in that order
    // which is much more straight-forward.
    std::sort(_enemy_party.begin(), _enemy_party.end(), CompareObjectsYCoord);

    if (GetState() == BATTLE_STATE_INVALID) {
        // When the enemy is added before the battle has begun, we can store it
        // in case of a battle restart, as the number of enemies might have changed afterwards
        _initial_enemy_actors_info.push_back(BattleEnemyInfo(new_enemy_id, position_x, position_y));
    }
    else {
        // If the battle has already begun, let's finish the enemy initialization.
        SetActorIdleStateTime(new_battle_enemy);
        new_battle_enemy->ChangeState(ACTOR_STATE_IDLE);
    }
}

void BattleMode::ChangeState(BATTLE_STATE new_state)
{
    if(_state == new_state) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "battle was already in the state to change to: " << _state << std::endl;
        return;
    }

    BattleMedia& battle_media = GlobalManager->GetBattleMedia();

    _state = new_state;
    switch(_state) {
    case BATTLE_STATE_INITIAL:
        // Reset logic flags
        _last_enemy_dying = false;
        _actor_state_paused = false;
        // Reset the stamina icons alpha
        _stamina_icon_alpha = 1.0f;

        // Start the music if needed
        _ResetMusicState();

        // Disable potential previous light effects
        VideoManager->DisableFadeEffect();
        GetEffectSupervisor().DisableEffects();

        // Display a message about the stamina bonus related event
        if (_hero_init_boost && _enemy_init_boost)
        {
            GetIndicatorSupervisor().AddShortNotice(UTranslate("Double Rush!"),
						    "data/gui/menus/star.png");
        }
        else if (_hero_init_boost)
        {
            GetIndicatorSupervisor().AddShortNotice(UTranslate("First Strike!"),
						    "data/gui/menus/star.png");
        }
        else if (_enemy_init_boost)
        {
            GetIndicatorSupervisor().AddShortNotice(UTranslate("Ambush!"),
						    "data/gui/battle/exclamation.png");
        }
        break;
    case BATTLE_STATE_NORMAL:
        if(_battle_type == BATTLE_TYPE_WAIT || _battle_type == BATTLE_TYPE_SEMI_ACTIVE) {
            for(uint32_t i = 0; i < _character_actors.size(); i++) {
                if(_character_actors[i]->GetState() == ACTOR_STATE_COMMAND)
                    return;
            }
        }
        // If no other character is waiting for a command (in wait battle modes),
        // restart the battle actors in case they were paused.
        _actor_state_paused = false;
        break;
    case BATTLE_STATE_COMMAND:
        if(_command_supervisor->GetCommandCharacter() == nullptr) {
            IF_PRINT_WARNING(BATTLE_DEBUG) << "no character was selected when changing battle to the command state" << std::endl;
            ChangeState(BATTLE_STATE_NORMAL);
        }
        break;
    case BATTLE_STATE_VICTORY: {
        // Official victory:
        // Cancel all character actions to free possible involved objects
        for(uint32_t i = 0; i < _character_actors.size(); ++i) {
            BattleAction *action = _character_actors[i]->GetAction();
            if(action)
                action->Cancel();
        }

        // Remove the items used in battle from inventory.
        _command_supervisor->CommitChangesToInventory();

        // Attempt to reload the music if it was removed from the cache
        AudioManager->LoadMusic(battle_media.victory_music_filename);
        if (MusicDescriptor* victory_music = AudioManager->RetrieveMusic(battle_media.victory_music_filename)) {
            victory_music->Rewind();
            victory_music->Play();
        }
        _battle_finish = new BattleVictory();
        _battle_finish->Initialize();
        break;
    }
    case BATTLE_STATE_DEFEAT: {
        // Attempt to reload the music if it was removed from the cache
        AudioManager->LoadMusic(battle_media.defeat_music_filename);
        if (MusicDescriptor* defeat_music = AudioManager->RetrieveMusic(battle_media.defeat_music_filename)) {
            defeat_music->Rewind();
            defeat_music->FadeIn(1000.0f);
        }
        _battle_finish = new BattleDefeat();
        _battle_finish->Initialize();
        break;
    }
    default:
        IF_PRINT_WARNING(BATTLE_DEBUG) << "changed to invalid battle state: " << _state << std::endl;
        break;
    }
}



bool BattleMode::OpenCommandMenu(BattleCharacter *character)
{
    if(character == nullptr) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received nullptr argument" << std::endl;
        return false;
    }

    if(_state == BATTLE_STATE_COMMAND) {
        return false;
    }

    if(!character->CanSelectCommand())
        return false;

    _command_supervisor->Initialize(character);
    // In case the auto-battle mode was active, we deactivate it.
    _battle_menu.SetAutoBattleActive(false);
    ChangeState(BATTLE_STATE_COMMAND);
    return true;
}


void BattleMode::NotifyCommandCancel()
{
    if(_state != BATTLE_STATE_COMMAND) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "battle was not in command state when function was called" << std::endl;
        return;
    } else if(_command_supervisor->GetCommandCharacter() != nullptr) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "command supervisor still had a character selected when function was called" << std::endl;
        return;
    }

    ChangeState(BATTLE_STATE_NORMAL);
}



void BattleMode::NotifyCharacterCommandComplete(BattleCharacter *character)
{
    if(character == nullptr) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received nullptr argument" << std::endl;
        return;
    }

    // Update the action text to reflect the action and target now set for the character
    character->ChangeActionText();

    // If the character was in the command state when it had its command set, the actor needs to move on the the warmup state to prepare to
    // execute the command. Otherwise if the character was in any other state (likely the idle state), the character should remain in that state.
    if(character->GetState() == ACTOR_STATE_COMMAND) {
        character->ChangeState(ACTOR_STATE_WARM_UP);
    }

    if (_command_supervisor->GetCommandCharacter() == nullptr)
        ChangeState(BATTLE_STATE_NORMAL);
}



void BattleMode::NotifyActorReady(BattleActor *actor)
{
    for(std::list<BattleActor *>::iterator i = _ready_queue.begin(); i != _ready_queue.end(); ++i) {
        if(actor == (*i)) {
            IF_PRINT_WARNING(BATTLE_DEBUG) << "actor was already present in the ready queue" << std::endl;
            return;
        }
    }

    _ready_queue.push_back(actor);
}



void BattleMode::NotifyActorDeath(BattleActor *actor)
{
    if(actor == nullptr) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received nullptr argument" << std::endl;
        return;
    }

    // Remove the actor from the ready queue if it is there
    _ready_queue.remove(actor);

    // Notify the command supervisor about the death event if it is active
    if(_state == BATTLE_STATE_COMMAND) {
        _command_supervisor->NotifyActorDeath(actor);

        // If the actor who died was the character that the player was selecting a command for, this will cause the
        // command supervisor will return to the invalid state.
        if(_command_supervisor->GetState() == COMMAND_STATE_INVALID)
            ChangeState(BATTLE_STATE_NORMAL);
    }

    // Determine if the battle should proceed to the victory or defeat state
    if(IsBattleFinished())
        IF_PRINT_WARNING(BATTLE_DEBUG) << "actor death occurred after battle was finished" << std::endl;
}

bool BattleMode::isOneCharacterDead() const
{
    for(std::deque<private_battle::BattleCharacter *>::const_iterator it = _character_actors.begin();
            it != _character_actors.end(); ++it) {
        if(!(*it)->IsAlive())
            return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// BattleMode class -- private methods
////////////////////////////////////////////////////////////////////////////////

void BattleMode::_Initialize()
{
    // Unset a possible last enemy dying sequence.
    _last_enemy_dying = false;

    // Construct all character battle actors from the active party, as well as the menus that populate the command supervisor
    GlobalParty *active_party = GlobalManager->GetActiveParty();
    uint32_t party_size = active_party->GetPartySize();
    if(party_size == 0) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "No characters in the active party, exiting battle" << std::endl;
        ModeManager->Pop();
        return;
    }

    for(uint32_t i = 0; i < party_size; ++i) {
        BattleCharacter *new_actor = new BattleCharacter(active_party->GetCharacterAtIndex(i));
        _character_actors.push_back(new_actor);
        _character_party.push_back(new_actor);

    // Sort the characters based on their Y location.
    // The player will then be able to target them in that order
    // which is much more straight-forward.
    std::sort(_character_party.begin(), _character_party.end(), CompareObjectsYCoord);

        // Check whether the character is alive
        if(new_actor->GetHitPoints() == 0)
            new_actor->ChangeState(ACTOR_STATE_DEAD);
    }
    _command_supervisor->ConstructMenus();

    // Determine the origin position for all characters and enemies
    _DetermineActorLocations();

    // Find the actor with the highest stamina rating
    _highest_stamina = 0;
    for(uint32_t i = 0; i < _character_actors.size(); ++i) {
        if(_character_actors[i]->GetStamina() > _highest_stamina)
            _highest_stamina = _character_actors[i]->GetStamina();
    }

    for(uint32_t i = 0; i < _enemy_actors.size(); ++i) {
        if(_enemy_actors[i]->GetStamina() > _highest_stamina)
            _highest_stamina = _enemy_actors[i]->GetStamina();
    }

    if(_highest_stamina == 0) {
        _highest_stamina = 1; // Prevent potential segfault.
        PRINT_WARNING << "The highest stamina found was 0" << std::endl;
    }

    // Adjust each actor's idle state time based on their stamina proportion to the fastest actor
    // If an actor's stamina is half that of the actor with the highest stamina, then they will have an
    // idle state time that is twice that of the slowest actor.

    // We also factor the idle time using the battle type setting
    // ACTIVE BATTLE
    _battle_type_time_factor = BATTLE_ACTIVE_FACTOR;
    // WAIT battle type is always safe, since the character has got all the time
    // he/she wants to think so we can dimish the idle time of character and jump
    // right to the command status.
    if(_battle_type == BATTLE_TYPE_WAIT)
        _battle_type_time_factor = BATTLE_WAIT_FACTOR;
    // SEMI_ACTIVE battle type is a bit more dangerous as if the player is taking
    // too much time to think, the enemies will have slightly more chances to hit.
    // Yet, the semi wait battles are far simpler than active ones, so we
    // can make them relatively faster.
    else if(_battle_type == BATTLE_TYPE_SEMI_ACTIVE)
        _battle_type_time_factor = BATTLE_SEMI_ACTIVE_FACTOR;

    for(uint32_t i = 0; i < _character_actors.size(); ++i) {
        if(_character_actors[i]->IsAlive()) {
            SetActorIdleStateTime(_character_actors[i]);

            // Needed to set up the stamina icon position.
            _character_actors[i]->ChangeState(ACTOR_STATE_IDLE);
        }
    }
    for(uint32_t i = 0; i < _enemy_actors.size(); ++i) {
        SetActorIdleStateTime(_enemy_actors[i]);
        _enemy_actors[i]->ChangeState(ACTOR_STATE_IDLE);
    }

    // Randomize each actor's initial idle state progress to be somewhere in the lower half of their total
    // idle state time. This is performed so that every battle doesn't start will all stamina icons piled on top
    // of one another at the bottom of the stamina bar.
    // Also, depending on who attacked first, the hero or enemy party will receive an stamina boost at battle start.
    for(uint32_t i = 0; i < _character_actors.size(); ++i) {
        if(!_character_actors[i]->IsAlive())
            continue;

        uint32_t max_init_timer = _character_actors[i]->GetIdleStateTime() / 2;
        if (_hero_init_boost)
            _character_actors[i]->GetStateTimer().Update(RandomBoundedInteger(max_init_timer, max_init_timer * 2));
        else
            _character_actors[i]->GetStateTimer().Update(RandomBoundedInteger(0, max_init_timer));
    }
    for(uint32_t i = 0; i < _enemy_actors.size(); ++i) {
        uint32_t max_init_timer = _enemy_actors[i]->GetIdleStateTime() / 2;
        if (_enemy_init_boost)
            _enemy_actors[i]->GetStateTimer().Update(RandomBoundedInteger(max_init_timer, max_init_timer * 2));
        else
            _enemy_actors[i]->GetStateTimer().Update(RandomBoundedInteger(0, max_init_timer));
    }

    // Init the script component.
    GetScriptSupervisor().Initialize(this);

    ChangeState(BATTLE_STATE_INITIAL);
}

void BattleMode::SetActorIdleStateTime(BattleActor* actor)
{
    if(!actor || actor->GetStamina() == 0)
        return;

    if(_highest_stamina == 0 || _battle_type_time_factor == 0.0f)
        return;

    float proportion = static_cast<float>(_highest_stamina)
                       / static_cast<float>(actor->GetStamina() * _battle_type_time_factor);

    actor->SetIdleStateTime(static_cast<uint32_t>(MIN_IDLE_WAIT_TIME * proportion));
}

void BattleMode::TriggerBattleParticleEffect(const std::string &effect_filename, uint32_t x, uint32_t y)
{
    BattleParticleEffect *effect = new BattleParticleEffect(effect_filename);

    effect->SetXLocation(x);
    effect->SetYLocation(y);

    effect->Start();

    _battle_effects.push_back(effect);
}

private_battle::BattleAnimation* BattleMode::CreateBattleAnimation(const std::string& animation_filename)
{
    BattleAnimation* animation = new BattleAnimation(animation_filename);

    // Set it invisible until an event make it usable
    animation->SetVisible(false);

    _battle_effects.push_back(animation);
    return animation;
}

void BattleMode::_DetermineActorLocations()
{
    float position_x, position_y;

    // Determine the default position of the first character in the party,
    // who will be drawn at the top
    switch(_character_actors.size()) {
    case 1:
        position_x = 80.0f;
        position_y = 480.0f;
        break;
    case 2:
        position_x = 118.0f;
        position_y = 425.0f;
        break;
    case 3:
        position_x = 122.0f;
        position_y = 375.0f;
        break;
    case 4:
    default:
        position_x = 160.0f;
        position_y = 320.0f;
        break;
    }

    // Set all characters in their proper positions
    for(uint32_t i = 0; i < _character_actors.size(); i++) {
        _character_actors[i]->SetXOrigin(position_x);
        _character_actors[i]->SetYOrigin(position_y);
        _character_actors[i]->SetXLocation(position_x);
        _character_actors[i]->SetYLocation(position_y);
        position_x -= 32.0f;
        position_y += 105.0f;
    }

    // Assign static locations to enemies
    uint32_t default_pos_id = 0;
    for(uint32_t i = 0; i < _enemy_actors.size(); ++i) {
        // If no position was set for the enemy, pick a default one.
        if(_enemy_actors[i]->GetXLocation() == 0.0f && _enemy_actors[i]->GetYLocation() == 0.0f) {
            position_x = DEFAULT_ENEMY_LOCATIONS[default_pos_id % NUM_DEFAULT_LOCATIONS][0];
            position_y = DEFAULT_ENEMY_LOCATIONS[default_pos_id % NUM_DEFAULT_LOCATIONS][1];
            ++default_pos_id;
            // Add an artifical offset when cycling within the default positions.
            // This will permit to still see all the enemies, even when there are more than 8 of them.
            if(default_pos_id > NUM_DEFAULT_LOCATIONS - 1) {
                position_x += default_pos_id * 3;
                position_y += default_pos_id * 3;
            }
            _enemy_actors[i]->SetXOrigin(position_x);
            _enemy_actors[i]->SetYOrigin(position_y);
            _enemy_actors[i]->SetXLocation(position_x);
            _enemy_actors[i]->SetYLocation(position_y);
        }
    }
} // void BattleMode::_DetermineActorLocations()

void BattleMode::_AutoCharacterCommand(BattleCharacter* character)
{
    if (character == nullptr) {
        PRINT_ERROR << "AutoCharacterCommand was called with a null Character" << std::endl;
        return;
    }

    if (character->IsActionSet() || _command_supervisor->GetCommandCharacter() == character)
        return;

    auto autoTarget = BattleTarget();
    autoTarget.SetTarget(character, GLOBAL_TARGET_FOE);

    GlobalSkill* attackSkill = character->GetSkills()[0];

    auto global_character = character->GetGlobalCharacter();
    if(global_character->GetWeaponEquipped()) {
        auto wpnSkills = global_character->GetWeaponSkills();
        for (auto skill : *wpnSkills) {
            if (skill->GetSPRequired() == 0) {
                attackSkill = skill;
                break;
            }
        }
    } else {
        auto handSkills = global_character->GetBareHandsSkills();
        if (handSkills->size() > 0)
            attackSkill = (*handSkills)[0];
    }

    if (attackSkill == nullptr) {
        PRINT_WARNING << "No valid attack skill was found for character: "
            << vt_utils::MakeStandardString(character->GetGlobalCharacter()->GetName()) << std::endl;
        return;
    }

    BattleAction* new_action = new SkillAction(character, autoTarget, attackSkill);
    character->SetAction(new_action);
    BattleMode::CurrentInstance()->NotifyCharacterCommandComplete(character);

    _actor_state_paused = false;
}

uint32_t BattleMode::_NumberEnemiesAlive() const
{
    uint32_t enemy_count = 0;
    for(uint32_t i = 0; i < _enemy_actors.size(); ++i) {
        if(_enemy_actors[i]->IsAlive()) {
            ++enemy_count;
        }
    }
    return enemy_count;
}

uint32_t BattleMode::_NumberValidEnemies() const
{
    uint32_t enemy_count = 0;
    for(uint32_t i = 0; i < _enemy_actors.size(); ++i) {
        if(_enemy_actors[i]->CanFight())
            ++enemy_count;
    }
    return enemy_count;
}

uint32_t BattleMode::_NumberCharactersAlive() const
{
    uint32_t character_count = 0;
    for(uint32_t i = 0; i < _character_actors.size(); ++i) {
        if(_character_actors[i]->IsAlive())
            ++character_count;
    }
    return character_count;
}

void BattleMode::_DrawBackgroundGraphics()
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_NO_BLEND, 0);
    VideoManager->Move(0.0f, 768.0f);
    GlobalManager->GetBattleMedia().background_image.Draw();

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
    VideoManager->SetStandardCoordSys();

    GetScriptSupervisor().DrawBackground();
}


void BattleMode::_DrawForegroundGraphics()
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
    VideoManager->SetStandardCoordSys();

    GetScriptSupervisor().DrawForeground();
}

void BattleMode::_DrawSprites()
{
    BattleMedia& battle_media = GlobalManager->GetBattleMedia();

    // Booleans used to determine whether or not the actor selector and attack point selector graphics should be drawn
    bool draw_actor_selection = false;
    bool draw_point_selection = false;

    BattleTarget target = _command_supervisor->GetSelectedTarget(); // The target that the player has selected
    BattleActor *actor_target = target.GetActor(); // A pointer to an actor being targeted.

    // Determine if selector graphics should be drawn
    if((_state == BATTLE_STATE_COMMAND)
            && ((_command_supervisor->GetState() == COMMAND_STATE_ACTOR) || (_command_supervisor->GetState() == COMMAND_STATE_POINT))) {
        draw_actor_selection = true;
        if((_command_supervisor->GetState() == COMMAND_STATE_POINT) && (IsTargetPoint(target.GetType())))
            draw_point_selection = true;
    }

    // Draw the actor selector graphic
    if(draw_actor_selection) {
        VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
        if(IsTargetParty(target.GetType())) {
            const std::deque<BattleActor *>& party_target = target.GetPartyTarget();
            for(uint32_t i = 0; i < party_target.size(); i++) {
                VideoManager->Move(party_target[i]->GetXLocation(),  party_target[i]->GetYLocation());
                VideoManager->MoveRelative(0.0f, 20.0f);
                battle_media.actor_selection_image.Draw();
            }
        }
        else if(actor_target != nullptr) {
            VideoManager->Move(actor_target->GetXLocation(), actor_target->GetYLocation());
            VideoManager->MoveRelative(0.0f, 20.0f);
            battle_media.actor_selection_image.Draw();
        }
        // Else this target is invalid so don't draw anything
    }

    // Draw sprites in order based on their x and y coordinates on the screen (bottom to top)
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    for(uint32_t i = 0; i < _battle_objects.size(); ++i)
        _battle_objects[i]->DrawSprite();

    // Draw the attack point selector graphic
    if(draw_point_selection) {
        uint32_t point = target.GetAttackPoint();

        VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, VIDEO_BLEND, 0);
        VideoManager->Move(actor_target->GetXLocation(), actor_target->GetYLocation());
        VideoManager->MoveRelative(actor_target->GetAttackPoint(point)->GetXPosition(), -actor_target->GetAttackPoint(point)->GetYPosition());
        battle_media.attack_point_indicator.Draw();
    }
}

void BattleMode::_DrawGUI()
{
    _DrawBottomMenu();
    _DrawStaminaBar();

    if (_battle_menu.IsAutoBattleActive()) {
        BattleMedia& battle_media = GlobalManager->GetBattleMedia();
        VideoManager->Move(800.0f, 50.0f);
        battle_media.GetAutoBattleIcon().Draw();
        VideoManager->MoveRelative(80.0f, 0.0f);
        battle_media.GetAutoBattleActiveText()->Draw();
    }

    // Don't draw battle actor indicators at battle ends
    if(_state != BATTLE_STATE_VICTORY && _state != BATTLE_STATE_DEFEAT)
        GetIndicatorSupervisor().Draw();

    if(_command_supervisor->GetState() != COMMAND_STATE_INVALID) {
        // Do not draw the command selection GUI if the battle is in scene mode
        if(!IsInSceneMode() && !_last_enemy_dying)
            _command_supervisor->Draw();
    }

    if (_battle_menu.IsOpen())
        _battle_menu.Draw();

    if(_dialogue_supervisor->IsDialogueActive())
        _dialogue_supervisor->Draw();

    if((_state == BATTLE_STATE_VICTORY || _state == BATTLE_STATE_DEFEAT)) {
        if (_battle_finish)
            _battle_finish->Draw();
    }
}

void BattleMode::_DrawBottomMenu()
{
    // Draw the static image for the lower menu
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(0.0f, 768.0f);
    BattleMedia& battle_media = GlobalManager->GetBattleMedia();
    battle_media.bottom_menu_image.Draw();

    if(_state != BATTLE_STATE_DEFEAT && _state != BATTLE_STATE_VICTORY) {
        // If the player is selecting a command for a particular character,
        // draw that character's portrait
        if(_command_supervisor->GetCommandCharacter())
            _command_supervisor->GetCommandCharacter()->DrawPortrait();

        // Draw the highlight images for the character that a command
        // is being selected for (if any) and/or any characters.
        // that are in the "command" state. The latter indicates that
        // these characters needs a command selected as soon as possible
        for(uint32_t i = 0; i < _character_actors.size(); ++i) {
            if(_character_actors[i] == _command_supervisor->GetCommandCharacter()) {
                VideoManager->Move(148.0f, 683.0f + (25.0f * i));
                battle_media.character_selected_highlight.Draw();
            } else if(_character_actors[i]->GetState() == ACTOR_STATE_COMMAND) {
                VideoManager->Move(148.0f, 683.0f + (25.0f * i));
                battle_media.character_command_highlight.Draw();
            }
        }
    }

    // Draw the status information of all character actors
    for(uint32_t i = 0; i < _character_actors.size(); i++) {
        _character_actors[i]->DrawStatus(i, _command_supervisor->GetCommandCharacter());
    }
}

void BattleMode::_DrawStaminaBar()
{
    // Used to determine whether or not an icon selector graphic needs to be drawn
    bool draw_icon_selection = false;
    // If true, an entire party of actors is selected
    bool is_party_selected = false;
    // If true, the selected party is the enemy party
    bool is_party_enemy = false;

    BattleActor *selected_actor = nullptr; // A pointer to the selected actor

    // Determine whether the selector graphics should be drawn
    if((_state == BATTLE_STATE_COMMAND)
            && ((_command_supervisor->GetState() == COMMAND_STATE_ACTOR) || (_command_supervisor->GetState() == COMMAND_STATE_POINT))) {
        BattleTarget target = _command_supervisor->GetSelectedTarget();

        draw_icon_selection = true;
        selected_actor = target.GetActor(); // Will remain nullptr if the target type is a party

        if(target.GetType() == GLOBAL_TARGET_ALL_ALLIES) {
            is_party_selected = true;
            is_party_enemy = false;
        } else if(target.GetType() == GLOBAL_TARGET_ALL_FOES) {
            is_party_selected = true;
            is_party_enemy = true;
        }
    }

    // TODO: sort the draw positions

    // Draw the stamina bar
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
    VideoManager->Move(STAMINA_BAR_POSITION_X, STAMINA_BAR_POSITION_Y); // 1010
    BattleMedia& battle_media = GlobalManager->GetBattleMedia();
    battle_media.stamina_meter.Draw();

    // Draw all stamina icons in order along with the selector graphic
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);

    for(uint32_t i = 0; i < _character_actors.size(); ++i) {
        if(!_character_actors[i]->IsAlive())
            continue;

        _character_actors[i]->DrawStaminaIcon(Color(1.0f, 1.0f, 1.0f, _stamina_icon_alpha));

        if(!draw_icon_selection)
            continue;

        // Draw selections
        if((is_party_selected && !is_party_enemy) || _character_actors[i] == selected_actor)
            battle_media.stamina_icon_selected.Draw();
    }

    for(uint32_t i = 0; i < _enemy_actors.size(); ++i) {
        if(!_enemy_actors[i]->IsAlive())
            continue;

        _enemy_actors[i]->DrawStaminaIcon(Color(1.0f, 1.0f, 1.0f, _stamina_icon_alpha));

        if(!draw_icon_selection)
            continue;

        // Draw selections
        if((is_party_selected && is_party_enemy) || _enemy_actors[i] == selected_actor)
            battle_media.stamina_icon_selected.Draw();
    }
}

} // namespace vt_battle
