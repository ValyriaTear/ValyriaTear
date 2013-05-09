////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
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

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/mode_manager.h"
#include "engine/script/script.h"
#include "engine/video/video.h"

#include "modes/pause.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actors.h"
#include "modes/battle/battle_actions.h"
#include "modes/battle/battle_command.h"
#include "modes/battle/battle_dialogue.h"
#include "modes/battle/battle_finish.h"
#include "modes/battle/battle_sequence.h"
#include "modes/battle/battle_utils.h"
#include "modes/battle/battle_effects.h"

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
BattleMode *BattleMode::_current_instance = NULL;

namespace private_battle
{

////////////////////////////////////////////////////////////////////////////////
// BattleMedia class
////////////////////////////////////////////////////////////////////////////////

// Filenames of the default music that is played when no specific music is requested
//@{
const char *DEFAULT_BATTLE_MUSIC   = "mus/heroism-OGA-Edward-J-Blakeley.ogg";
const char *DEFAULT_VICTORY_MUSIC  = "mus/Fanfare.ogg";
const char *DEFAULT_DEFEAT_MUSIC   = "mus/Battle_lost-OGA-Mumu.ogg";
//@}

BattleMedia::BattleMedia()
{
    if(!background_image.Load("img/backdrops/battle/desert_cave/desert_cave.png"))
        PRINT_ERROR << "failed to load default background image" << std::endl;

    if(stamina_icon_selected.Load("img/menus/stamina_icon_selected.png") == false)
        PRINT_ERROR << "failed to load stamina icon selected image" << std::endl;

    attack_point_indicator.SetDimensions(16.0f, 16.0f);
    if(attack_point_indicator.LoadFromFrameGrid("img/icons/battle/attack_point_target.png",
            std::vector<uint32>(4, 100), 1, 4) == false)
        PRINT_ERROR << "failed to load attack point indicator." << std::endl;

    if(stamina_meter.Load("img/menus/stamina_bar.png") == false)
        PRINT_ERROR << "failed to load time meter." << std::endl;

    if(actor_selection_image.Load("img/icons/battle/character_selector.png") == false)
        PRINT_ERROR << "unable to load player selector image" << std::endl;

    if(character_selected_highlight.Load("img/menus/battle_character_selection.png") == false)
        PRINT_ERROR << "failed to load character selection highlight image" << std::endl;

    if(character_command_highlight.Load("img/menus/battle_character_command.png") == false)
        PRINT_ERROR << "failed to load character command highlight image" << std::endl;

    if(bottom_menu_image.Load("img/menus/battle_bottom_menu.png") == false)
        PRINT_ERROR << "failed to load bottom menu image" << std::endl;

    if(ImageDescriptor::LoadMultiImageFromElementGrid(character_action_buttons, "img/menus/battle_command_buttons.png", 2, 5) == false)
        PRINT_ERROR << "failed to load character action buttons" << std::endl;

    if(ImageDescriptor::LoadMultiImageFromElementGrid(_target_type_icons, "img/icons/effects/targets.png", 1, 8) == false)
        PRINT_ERROR << "failed to load character action buttons" << std::endl;

    character_HP_text.SetStyle(TextStyle("text18", Color::white));
    character_HP_text.SetText(Translate("HP"));
    character_SP_text.SetStyle(TextStyle("text18", Color::white));
    character_SP_text.SetText(Translate("SP"));

    if(victory_music.LoadAudio(DEFAULT_VICTORY_MUSIC) == false)
        IF_PRINT_WARNING(BATTLE_DEBUG) << "failed to load victory music file: " << DEFAULT_VICTORY_MUSIC << std::endl;

    if(defeat_music.LoadAudio(DEFAULT_DEFEAT_MUSIC) == false)
        IF_PRINT_WARNING(BATTLE_DEBUG) << "failed to load defeat music file: " << DEFAULT_DEFEAT_MUSIC << std::endl;

    // Load the stunned icon
    if(!_stunned_icon.Load("img/icons/effects/zzz.png"))
        IF_PRINT_WARNING(BATTLE_DEBUG) << "failed to load stunned icon" << std::endl;
}


void BattleMedia::Update()
{
    attack_point_indicator.Update();
}


void BattleMedia::SetBackgroundImage(const std::string &filename)
{
    if(background_image.Load(filename) == false) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "failed to load background image: " << filename << std::endl;
    }
}


void BattleMedia::SetBattleMusic(const std::string &filename)
{
    if(battle_music.LoadAudio(filename) == false) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "failed to load music file: " << filename << std::endl;
    }
}


StillImage *BattleMedia::GetCharacterActionButton(uint32 index)
{
    if(index >= character_action_buttons.size()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received invalid index argument: " << index << std::endl;
        return NULL;
    }

    return &(character_action_buttons[index]);
}


StillImage *BattleMedia::GetTargetTypeIcon(vt_global::GLOBAL_TARGET target_type)
{
    switch(target_type) {
    case GLOBAL_TARGET_SELF_POINT:
        return &_target_type_icons[0];
    case GLOBAL_TARGET_ALLY_POINT:
        return &_target_type_icons[1];
    case GLOBAL_TARGET_FOE_POINT:
        return &_target_type_icons[2];
    case GLOBAL_TARGET_SELF:
        return &_target_type_icons[3];
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
        return &_target_type_icons[4];
    case GLOBAL_TARGET_FOE:
        return &_target_type_icons[5];
    case GLOBAL_TARGET_ALL_ALLIES:
        return &_target_type_icons[6];
    case GLOBAL_TARGET_ALL_FOES:
        return &_target_type_icons[7];
    default:
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received invalid target type argument: " << target_type << std::endl;
        return NULL;
    }
}

} // namespace private_battle

////////////////////////////////////////////////////////////////////////////////
// BattleMode class -- primary methods
////////////////////////////////////////////////////////////////////////////////

BattleMode::BattleMode() :
    _state(BATTLE_STATE_INVALID),
    _sequence_supervisor(NULL),
    _command_supervisor(NULL),
    _dialogue_supervisor(NULL),
    _finish_supervisor(NULL),
    _current_number_swaps(0),
    _last_enemy_dying(false),
    _stamina_icon_alpha(1.0f),
    _actor_state_paused(false),
    _scene_mode(false),
    _battle_type(BATTLE_TYPE_WAIT),
    _highest_agility(0),
    _battle_type_time_factor(BATTLE_WAIT_FACTOR)
{
    IF_PRINT_DEBUG(BATTLE_DEBUG) << "constructor invoked" << std::endl;

    mode_type = MODE_MANAGER_BATTLE_MODE;

    _current_instance = this;

    _sequence_supervisor = new SequenceSupervisor(this);
    _command_supervisor = new CommandSupervisor();
    _dialogue_supervisor = new DialogueSupervisor();
    _finish_supervisor = new FinishSupervisor();
} // BattleMode::BattleMode()



BattleMode::~BattleMode()
{
    delete _sequence_supervisor;
    delete _command_supervisor;
    delete _dialogue_supervisor;
    delete _finish_supervisor;

    // Delete all character and enemy actors
    for(uint32 i = 0; i < _character_actors.size(); i++) {
        delete _character_actors[i];
    }
    _character_actors.clear();
    _character_party.clear();

    for(uint32 i = 0; i < _enemy_actors.size(); i++) {
        delete _enemy_actors[i];
    }
    _enemy_actors.clear();
    _enemy_party.clear();

    _ready_queue.clear();

    if(_current_instance == this) {
        _current_instance = NULL;
    }
} // BattleMode::~BattleMode()

void BattleMode::Reset()
{
    VideoManager->SetStandardCoordSys();

    // Load the default battle music track if no other music has been added
    if(_battle_media.battle_music.GetState() == AUDIO_STATE_UNLOADED) {
        if(!_battle_media.battle_music.LoadAudio(DEFAULT_BATTLE_MUSIC)) {
            IF_PRINT_WARNING(BATTLE_DEBUG) << "failed to load default battle music: " << DEFAULT_BATTLE_MUSIC << std::endl;
        }
    }

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

    // Reset potential battle scripts
    GetScriptSupervisor().Reset();

    // Removes all enemies and readd only the ones that were present
    // at the beginning of the battle.
    for(uint32 i = 0; i < _enemy_actors.size(); ++i)
        delete _enemy_actors[i];

    _enemy_actors.clear();
    _enemy_party.clear();

    for(uint32 i = 0; i < _initial_enemy_actors_info.size(); ++i)
        AddEnemy(_initial_enemy_actors_info[i].id, _initial_enemy_actors_info[i].pos_x, _initial_enemy_actors_info[i].pos_y);

    // Reset the state of all characters and enemies
    for(uint32 i = 0; i < _character_actors.size(); ++i)
    {
        _character_actors[i]->ResetActor();
        _ResetPassiveStatusEffects(*(_character_actors[i]));
    }

    for(uint32 i = 0; i < _enemy_actors.size(); ++i)
        _enemy_actors[i]->ResetActor();

    // Reset battle inventory and available actions
    delete _command_supervisor;
    _command_supervisor = new CommandSupervisor();

    _battle_media.battle_music.Rewind();
    _battle_media.battle_music.Play();

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
    _battle_media.Update();
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

    if(_dialogue_supervisor->IsDialogueActive())
        _dialogue_supervisor->Update();

    // Update all actors animations and y-sorting
    _battle_objects.clear();
    for(uint32 i = 0; i < _character_actors.size(); ++i) {
        _character_actors[i]->Update();
        _battle_objects.push_back(_character_actors[i]);
        // Add also potential ammo objects
        if(_character_actors[i]->GetAmmo().IsAmmoShown())
            _battle_objects.push_back(&(_character_actors[i]->GetAmmo()));
    }
    for(uint32 i = 0; i < _enemy_actors.size(); ++i) {
        _enemy_actors[i]->Update();
        _battle_objects.push_back(_enemy_actors[i]);
    }

    // Add particle effects
    for(std::vector<BattleParticleEffect *>::iterator it = _battle_particle_effects.begin();
            it != _battle_particle_effects.end();) {
        if((*it)->IsAlive()) {
            (*it)->Update();
            _battle_objects.push_back(*it);
            ++it;
        } else {
            delete (*it);
            it = _battle_particle_effects.erase(it);
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
        BattleCharacter *character_selection = NULL;

        // The four keys below (up/down/left/right) correspond to each character, from top to bottom. Since the character party
        // does not always have four characters, for all but the first key we have to check that a character exists for the
        // corresponding key. If a character does exist, we then have to check whether or not the player is allowed to select a command
        // for it (characters can only have commands selected during certain states). If command selection is permitted, then we begin
        // the command supervisor.

        if(InputManager->UpPress()) {
            if(_character_actors.size() >= 1) {   // Should always evaluate to true
                character_selection = _character_actors[0];
            }
        }

        else if(InputManager->DownPress()) {
            if(_character_actors.size() >= 2) {
                character_selection = _character_actors[1];
            }
        }

        else if(InputManager->LeftPress()) {
            if(_character_actors.size() >= 3) {
                character_selection = _character_actors[2];
            }
        }

        else if(InputManager->RightPress()) {
            if(_character_actors.size() >= 4) {
                character_selection = _character_actors[3];
            }
        }

        if(!_last_enemy_dying && character_selection) {
            OpenCommandMenu(character_selection);
        }
    }
    // If the player is selecting a command for a character, the command supervisor has control
    else if(_state == BATTLE_STATE_COMMAND) {
        // If the last enemy is dying, there is no need to process command further
        if(!_last_enemy_dying)
            _command_supervisor->Update();
        else
            ChangeState(BATTLE_STATE_NORMAL);
    }
    // If the battle is in either finish state, the finish supervisor has control
    else if((_state == BATTLE_STATE_VICTORY) || (_state == BATTLE_STATE_DEFEAT)) {
        _finish_supervisor->Update();

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
    if(!_last_enemy_dying
        && (_battle_type == BATTLE_TYPE_WAIT || _battle_type == BATTLE_TYPE_SEMI_ACTIVE)) {
        for(uint32 i = 0; i < _character_actors.size(); i++) {
            if(_character_actors[i]->GetState() == ACTOR_STATE_COMMAND) {
                if(_state != BATTLE_STATE_COMMAND) {
                    OpenCommandMenu(_character_actors[i]);
                }
                return;
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
            acting_actor->ChangeState(ACTOR_STATE_ACTING);
            break;
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

void BattleMode::AddEnemy(uint32 new_enemy_id, float position_x, float position_y)
{
    GlobalEnemy *new_enemy = new vt_global::GlobalEnemy(new_enemy_id);

    // Don't add the enemy if its id was invalidated
    if(new_enemy->GetID() == 0) {
        PRINT_WARNING << "attempted to add a new enemy with an invalid id: "
            << new_enemy->GetID() << std::endl;
        delete new_enemy;
        return;
    }

    new_enemy->Initialize();
    BattleEnemy *new_battle_enemy = new BattleEnemy(new_enemy);
    // Set the battleground position
    new_battle_enemy->SetXLocation(position_x);
    new_battle_enemy->SetYLocation(position_y);
    new_battle_enemy->SetXOrigin(position_x);
    new_battle_enemy->SetYOrigin(position_y);

    _enemy_actors.push_back(new_battle_enemy);
    _enemy_party.push_back(new_battle_enemy);

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

    _state = new_state;
    switch(_state) {
    case BATTLE_STATE_INITIAL:
        // Reset logic flags
        _last_enemy_dying = false;
        _actor_state_paused = false;
        // Reset the stamina icons alpha
        _stamina_icon_alpha = 1.0f;
        // Start the music
        _battle_media.battle_music.FadeIn(1000);

        // Disable potential previous light effects
        VideoManager->DisableFadeEffect();
        GetEffectSupervisor().DisableEffects();
        break;
    case BATTLE_STATE_NORMAL:
        // Restart the battle actors in case they were paused
        _actor_state_paused = false;
        break;
    case BATTLE_STATE_COMMAND:
        if(_command_supervisor->GetCommandCharacter() == NULL) {
            IF_PRINT_WARNING(BATTLE_DEBUG) << "no character was selected when changing battle to the command state" << std::endl;
            ChangeState(BATTLE_STATE_NORMAL);
        }
        break;
    case BATTLE_STATE_VICTORY:
        // Official victory:
        // Cancel all character actions to free possible involved objects
        for(uint32 i = 0; i < _character_actors.size(); ++i) {
            BattleAction *action = _character_actors[i]->GetAction();
            if(action)
                action->Cancel();
        }

        // Remove the items used in battle from inventory.
        _command_supervisor->CommitChangesToInventory();

        _battle_media.victory_music.Rewind();
        _battle_media.victory_music.Play();
        _finish_supervisor->Initialize(true);
        break;
    case BATTLE_STATE_DEFEAT:
        _battle_media.defeat_music.Rewind();
        _battle_media.defeat_music.FadeIn(1000);
        _finish_supervisor->Initialize(false);
        break;
    default:
        IF_PRINT_WARNING(BATTLE_DEBUG) << "changed to invalid battle state: " << _state << std::endl;
        break;
    }
}



bool BattleMode::OpenCommandMenu(BattleCharacter *character)
{
    if(character == NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL argument" << std::endl;
        return false;
    }
    if(_state == BATTLE_STATE_COMMAND) {
        return false;
    }

    if(character->CanSelectCommand() == true) {
        _command_supervisor->Initialize(character);
        ChangeState(BATTLE_STATE_COMMAND);
        return true;
    }

    return false;
}


void BattleMode::NotifyCommandCancel()
{
    if(_state != BATTLE_STATE_COMMAND) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "battle was not in command state when function was called" << std::endl;
        return;
    } else if(_command_supervisor->GetCommandCharacter() != NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "command supervisor still had a character selected when function was called" << std::endl;
        return;
    }

    ChangeState(BATTLE_STATE_NORMAL);
}



void BattleMode::NotifyCharacterCommandComplete(BattleCharacter *character)
{
    if(character == NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL argument" << std::endl;
        return;
    }

    // Update the action text to reflect the action and target now set for the character
    character->ChangeActionText();

    // If the character was in the command state when it had its command set, the actor needs to move on the the warmup state to prepare to
    // execute the command. Otherwise if the character was in any other state (likely the idle state), the character should remain in that state.
    if(character->GetState() == ACTOR_STATE_COMMAND) {
        character->ChangeState(ACTOR_STATE_WARM_UP);
    }

    ChangeState(BATTLE_STATE_NORMAL);
}



void BattleMode::NotifyActorReady(BattleActor *actor)
{
    for(std::list<BattleActor *>::iterator i = _ready_queue.begin(); i != _ready_queue.end(); i++) {
        if(actor == (*i)) {
            IF_PRINT_WARNING(BATTLE_DEBUG) << "actor was already present in the ready queue" << std::endl;
            return;
        }
    }

    _ready_queue.push_back(actor);
}



void BattleMode::NotifyActorDeath(BattleActor *actor)
{
    if(actor == NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL argument" << std::endl;
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
    if(active_party->GetPartySize() == 0) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "no characters in the active party, exiting battle" << std::endl;
        ModeManager->Pop();
        return;
    }

    for(uint32 i = 0; i < active_party->GetPartySize(); i++) {
        BattleCharacter *new_actor = new BattleCharacter(dynamic_cast<GlobalCharacter *>(active_party->GetActorAtIndex(i)));
        _character_actors.push_back(new_actor);
        _character_party.push_back(new_actor);
        _ResetPassiveStatusEffects(*new_actor);
        // Check whether the character is alive
        if(new_actor->GetHitPoints() == 0)
            new_actor->ChangeState(ACTOR_STATE_DEAD);
    }
    _command_supervisor->ConstructMenus();

    // Determine the origin position for all characters and enemies
    _DetermineActorLocations();

    // Find the actor with the highext agility rating
    _highest_agility = 0;
    for(uint32 i = 0; i < _character_actors.size(); ++i) {
        if(_character_actors[i]->GetAgility() > _highest_agility)
            _highest_agility = _character_actors[i]->GetAgility();
    }

    for(uint32 i = 0; i < _enemy_actors.size(); ++i) {
        if(_enemy_actors[i]->GetAgility() > _highest_agility)
            _highest_agility = _enemy_actors[i]->GetAgility();
    }

    if(_highest_agility == 0) {
        _highest_agility = 1; // Prevent potential segfault.
        PRINT_WARNING << "The highest agility found was 0" << std::endl;
    }

    // Adjust each actor's idle state time based on their agility proportion to the fastest actor
    // If an actor's agility is half that of the actor with the highest agility, then they will have an
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

    for(uint32 i = 0; i < _character_actors.size(); i++) {
        if(_character_actors[i]->IsAlive()) {
            SetActorIdleStateTime(_character_actors[i]);

            // Needed to set up the stamina icon position.
            _character_actors[i]->ChangeState(ACTOR_STATE_IDLE);
        }
    }
    for(uint32 i = 0; i < _enemy_actors.size(); i++) {
        SetActorIdleStateTime(_enemy_actors[i]);
        _enemy_actors[i]->ChangeState(ACTOR_STATE_IDLE);
    }

    // Randomize each actor's initial idle state progress to be somewhere in the lower half of their total
    // idle state time. This is performed so that every battle doesn't start will all stamina icons piled on top
    // of one another at the bottom of the stamina bar
    for(uint32 i = 0; i < _character_actors.size(); i++) {
        if(_character_actors[i]->IsAlive()) {
            uint32 max_init_timer = _character_actors[i]->GetIdleStateTime() / 2;
            _character_actors[i]->GetStateTimer().Update(RandomBoundedInteger(0, max_init_timer));
        }
    }
    for(uint32 i = 0; i < _enemy_actors.size(); i++) {
        uint32 max_init_timer = _enemy_actors[i]->GetIdleStateTime() / 2;
        _enemy_actors[i]->GetStateTimer().Update(RandomBoundedInteger(0, max_init_timer));
    }

    // Init the script component.
    GetScriptSupervisor().Initialize(this);

    ChangeState(BATTLE_STATE_INITIAL);
} // void BattleMode::_Initialize()

void BattleMode::_ResetAttributesFromGlobalActor(private_battle::BattleActor &character)
{
    character.ResetAgility();
    character.ResetEvade();
    character.ResetFortitude();
    character.ResetProtection();
    character.ResetStrength();
    character.ResetVigor();
}

//! \brief sets the max_effect at the proper index only if it is higher in value than the current one there
static void SetEffectMaximum(GLOBAL_INTENSITY *max_effect, const std::vector<std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY> >&status_effects)
{
    for(size_t i = 0; i < status_effects.size(); ++i)
    {
        //we are only interested in attribute effects

        if(status_effects[i].first == GLOBAL_STATUS_INVALID ||
           status_effects[i].first > GLOBAL_STATUS_EVADE_LOWER)
            continue;
        if(status_effects[i].second < GLOBAL_INTENSITY_NEG_EXTREME ||
           status_effects[i].second > GLOBAL_INTENSITY_POS_EXTREME)
            continue;

        size_t index = (size_t) status_effects[i].first;
        if((int)max_effect[index] < (int)(status_effects[i].second))
            max_effect[index] = status_effects[i].second;
    }
}

void BattleMode::_ApplyPassiveStatusEffects(private_battle::BattleActor &character,
                                            const vt_global::GlobalWeapon* weapon,
                                            const std::vector<vt_global::GlobalArmor *>& armors)
{
    //we only count the first 12 status effects as valid
    //todo: allow a way for drain / regen
    const static size_t MAX_VALID_STATUS  = 12;
    //max value array for each of the status types.
    GLOBAL_INTENSITY max_effect[MAX_VALID_STATUS] = {GLOBAL_INTENSITY_INVALID};

    //adjust effects for the weapons
    if (weapon)
        SetEffectMaximum(max_effect, weapon->GetStatusEffects());

    //adjust effects for armor
    for(std::vector<vt_global::GlobalArmor *>::const_iterator itr = armors.begin(),
            end = armors.end(); itr != end; ++itr) {
        if((*itr))
            SetEffectMaximum(max_effect, (*itr)->GetStatusEffects());
    }

    //go through each effect (as a pair) looking for the highest one.
    for(size_t i = 0; i < MAX_VALID_STATUS; i += 2)
    {
        //no change for this status
        if(max_effect[i] == GLOBAL_INTENSITY_INVALID && max_effect[i + 1] == GLOBAL_INTENSITY_INVALID)
            continue;
        GLOBAL_STATUS max_status;
        GLOBAL_INTENSITY max_intensity;
        max_status = (int)max_effect[i] > (int)max_effect[i + 1] ? (GLOBAL_STATUS)i : (GLOBAL_STATUS)(i + 1);
        max_intensity = max_effect[(size_t)max_status];
        if(max_intensity == GLOBAL_INTENSITY_NEUTRAL)
            continue;
        //now, we manually effect the player with a "permenant effect" by creating
        //a battle effect and immediatly allowing it to modify the player.
        //this effect does NOT go into the effect controller
        private_battle::BattleStatusEffect scripted_effect(max_status, max_intensity, &character);
        //immediatly apply the full effect
        ScriptCallFunction<void>(scripted_effect.GetApplyFunction(), &scripted_effect);
    }

}

void BattleMode::_ResetPassiveStatusEffects(vt_battle::private_battle::BattleActor &character)
{
   _ResetAttributesFromGlobalActor(character);
   _ApplyPassiveStatusEffects(character, character.GetGlobalActor()->GetWeaponEquipped(), character.GetGlobalActor()->GetArmorEquipped());
}

void BattleMode::SetActorIdleStateTime(BattleActor *actor)
{
    if(!actor || actor->GetAgility() == 0)
        return;

    if(_highest_agility == 0 || _battle_type_time_factor == 0.0f)
        return;

    float proportion = static_cast<float>(_highest_agility)
                       / static_cast<float>(actor->GetAgility() * _battle_type_time_factor);

    actor->SetIdleStateTime(static_cast<uint32>(MIN_IDLE_WAIT_TIME * proportion));
}

void BattleMode::TriggerBattleParticleEffect(const std::string &effect_filename, uint32 x, uint32 y)
{
    BattleParticleEffect *effect = new BattleParticleEffect(effect_filename);

    effect->SetXLocation(x);
    effect->SetYLocation(y);

    effect->Start();

    _battle_particle_effects.push_back(effect);
}

void BattleMode::_DetermineActorLocations()
{
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
    const uint32 NUM_DEFAULT_LOCATIONS = 8;

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
    for(uint32 i = 0; i < _character_actors.size(); i++) {
        _character_actors[i]->SetXOrigin(position_x);
        _character_actors[i]->SetYOrigin(position_y);
        _character_actors[i]->SetXLocation(position_x);
        _character_actors[i]->SetYLocation(position_y);
        position_x -= 32.0f;
        position_y += 105.0f;
    }

    // Assign static locations to enemies
    uint32 default_pos_id = 0;
    for(uint32 i = 0; i < _enemy_actors.size(); ++i) {
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



uint32 BattleMode::_NumberEnemiesAlive() const
{
    uint32 enemy_count = 0;
    for(uint32 i = 0; i < _enemy_actors.size(); ++i) {
        if(_enemy_actors[i]->IsAlive()) {
            ++enemy_count;
        }
    }
    return enemy_count;
}


uint32 BattleMode::_NumberValidEnemies() const
{
    uint32 enemy_count = 0;
    for(uint32 i = 0; i < _enemy_actors.size(); ++i) {
        if(_enemy_actors[i]->IsValid()) {
            ++enemy_count;
        }
    }
    return enemy_count;
}


uint32 BattleMode::_NumberCharactersAlive() const
{
    uint32 character_count = 0;
    for(uint32 i = 0; i < _character_actors.size(); ++i) {
        if(_character_actors[i]->IsAlive()) {
            ++character_count;
        }
    }
    return character_count;
}


void BattleMode::_DrawBackgroundGraphics()
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_NO_BLEND, 0);
    VideoManager->Move(0.0f, 768.0f);
    _battle_media.background_image.Draw();

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
    // Booleans used to determine whether or not the actor selector and attack point selector graphics should be drawn
    bool draw_actor_selection = false;
    bool draw_point_selection = false;

    BattleTarget target = _command_supervisor->GetSelectedTarget(); // The target that the player has selected
    BattleActor *actor_target = target.GetActor(); // A pointer to an actor being targetted (value may be NULL if target is party)

    // Determine if selector graphics should be drawn
    if((_state == BATTLE_STATE_COMMAND)
            && ((_command_supervisor->GetState() == COMMAND_STATE_ACTOR) || (_command_supervisor->GetState() == COMMAND_STATE_POINT))) {
        draw_actor_selection = true;
        if((_command_supervisor->GetState() == COMMAND_STATE_POINT) && (IsTargetPoint(target.GetType()) == true))
            draw_point_selection = true;
    }

    // Draw the actor selector graphic
    if(draw_actor_selection == true) {
        VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
        if(actor_target != NULL) {
            VideoManager->Move(actor_target->GetXLocation(), actor_target->GetYLocation());
            VideoManager->MoveRelative(0.0f, 20.0f);
            _battle_media.actor_selection_image.Draw();
        } else if(IsTargetParty(target.GetType()) == true) {
            std::deque<BattleActor *>& party_target = *(target.GetParty());
            for(uint32 i = 0; i < party_target.size(); i++) {
                VideoManager->Move(party_target[i]->GetXLocation(),  party_target[i]->GetYLocation());
                VideoManager->MoveRelative(0.0f, 20.0f);
                _battle_media.actor_selection_image.Draw();
            }
            actor_target = NULL;
            // TODO: add support for drawing graphic under multiple actors if the target is a party
        }
        // Else this target is invalid so don't draw anything
    }

    // Draw sprites in order based on their x and y coordinates on the screen (bottom to top)
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    for(uint32 i = 0; i < _battle_objects.size(); ++i)
        _battle_objects[i]->DrawSprite();

    // Draw the attack point selector graphic
    if(draw_point_selection) {
        uint32 point = target.GetPoint();

        VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, VIDEO_BLEND, 0);
        VideoManager->Move(actor_target->GetXLocation(), actor_target->GetYLocation());
        VideoManager->MoveRelative(actor_target->GetAttackPoint(point)->GetXPosition(), -actor_target->GetAttackPoint(point)->GetYPosition());
        _battle_media.attack_point_indicator.Draw();
    }
} // void BattleMode::_DrawSprites()



void BattleMode::_DrawGUI()
{
    _DrawBottomMenu();
    _DrawStaminaBar();

    // Don't draw battle actor indicators at battle ends
    if(_state != BATTLE_STATE_VICTORY && _state != BATTLE_STATE_DEFEAT)
        _DrawIndicators();

    if(_command_supervisor->GetState() != COMMAND_STATE_INVALID) {
        // Do not draw the command selection GUI if the battle is in scene mode
        if(!IsInSceneMode() && !_last_enemy_dying)
            _command_supervisor->Draw();
    }

    if(_dialogue_supervisor->IsDialogueActive())
        _dialogue_supervisor->Draw();

    if((_state == BATTLE_STATE_VICTORY || _state == BATTLE_STATE_DEFEAT))
        _finish_supervisor->Draw();
}



void BattleMode::_DrawBottomMenu()
{
    // Draw the static image for the lower menu
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(0.0f, 768.0f);
    _battle_media.bottom_menu_image.Draw();

    if(_state != BATTLE_STATE_DEFEAT && _state != BATTLE_STATE_VICTORY) {
        // If the player is selecting a command for a particular character,
        // draw that character's portrait
        if(_command_supervisor->GetCommandCharacter())
            _command_supervisor->GetCommandCharacter()->DrawPortrait();

        // Draw the highlight images for the character that a command
        // is being selected for (if any) and/or any characters.
        // that are in the "command" state. The latter indicates that
        // these characters needs a command selected as soon as possible
        for(uint32 i = 0; i < _character_actors.size(); ++i) {
            if(_character_actors[i] == _command_supervisor->GetCommandCharacter()) {
                VideoManager->Move(148.0f, 683.0f + (25.0f * i));
                _battle_media.character_selected_highlight.Draw();
            } else if(_character_actors[i]->GetState() == ACTOR_STATE_COMMAND) {
                VideoManager->Move(148.0f, 683.0f + (25.0f * i));
                _battle_media.character_command_highlight.Draw();
            }
        }
    }

    // Draw the status information of all character actors
    for(uint32 i = 0; i < _character_actors.size(); i++) {
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

    BattleActor *selected_actor = NULL; // A pointer to the selected actor

    // Determine whether the selector graphics should be drawn
    if((_state == BATTLE_STATE_COMMAND)
            && ((_command_supervisor->GetState() == COMMAND_STATE_ACTOR) || (_command_supervisor->GetState() == COMMAND_STATE_POINT))) {
        BattleTarget target = _command_supervisor->GetSelectedTarget();

        draw_icon_selection = true;
        selected_actor = target.GetActor(); // Will remain NULL if the target type is a party

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
    _battle_media.stamina_meter.Draw();

    // Draw all stamina icons in order along with the selector graphic
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);

    for(uint32 i = 0; i < _character_actors.size(); ++i) {
        if(!_character_actors[i]->IsAlive())
            continue;

        _character_actors[i]->DrawStaminaIcon(Color(1.0f, 1.0f, 1.0f, _stamina_icon_alpha));

        if(!draw_icon_selection)
            continue;

        // Draw selections
        if((is_party_selected && !is_party_enemy) || _character_actors[i] == selected_actor)
            _battle_media.stamina_icon_selected.Draw();
    }

    for(uint32 i = 0; i < _enemy_actors.size(); ++i) {
        if(!_enemy_actors[i]->IsAlive())
            continue;

        _enemy_actors[i]->DrawStaminaIcon(Color(1.0f, 1.0f, 1.0f, _stamina_icon_alpha));

        if(!draw_icon_selection)
            continue;

        // Draw selections
        if((is_party_selected && is_party_enemy) || _enemy_actors[i] == selected_actor)
            _battle_media.stamina_icon_selected.Draw();
    }
} // void BattleMode::_DrawStaminaBar()



void BattleMode::_DrawIndicators()
{
    // Draw all character sprites
    for(uint32 i = 0; i < _character_actors.size(); ++i) {
        _character_actors[i]->DrawIndicators();
    }

    // Draw all enemy sprites
    for(uint32 i = 0; i < _enemy_actors.size(); ++i) {
        _enemy_actors[i]->DrawIndicators();
    }
}


// Available encounter sounds
static std::string encounter_sound_filenames[] = {
    "snd/battle_encounter_01.ogg",
    "snd/battle_encounter_02.ogg",
    "snd/battle_encounter_03.ogg"
};

TransitionToBattleMode::TransitionToBattleMode(BattleMode *BM):
    _position(0.0f),
    _BM(BM)
{

    _screen_capture = VideoManager->CaptureScreen();
    _screen_capture.SetDimensions(VIDEO_STANDARD_RES_WIDTH, VIDEO_STANDARD_RES_HEIGHT);
}

void TransitionToBattleMode::Update()
{
    // Process quit and pause events
    if(InputManager->QuitPress()) {
        ModeManager->Push(new PauseMode(true));
        return;
    } else if(InputManager->PausePress()) {
        ModeManager->Push(new PauseMode(false));
        return;
    }

    _transition_timer.Update();

    _position += _transition_timer.PercentComplete();

    if(_BM && _transition_timer.IsFinished()) {
        ModeManager->Pop();
        ModeManager->Push(_BM, true, true);
        _BM = NULL;
    }
}

void TransitionToBattleMode::Draw()
{
    // Draw the battle transition effect
    VideoManager->SetStandardCoordSys();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
    VideoManager->Move(0.0f, 0.0f);
    _screen_capture.Draw();
    VideoManager->Move(_position, _position);
    _screen_capture.Draw(Color(1.0f, 1.0f, 1.0f, 0.3f));
    VideoManager->Move(-_position, _position);
    _screen_capture.Draw(Color(1.0f, 1.0f, 1.0f, 0.3f));
    VideoManager->Move(-_position, -_position);
    _screen_capture.Draw(Color(1.0f, 1.0f, 1.0f, 0.3f));
    VideoManager->Move(_position, -_position);
    _screen_capture.Draw(Color(1.0f, 1.0f, 1.0f, 0.3f));
}

void TransitionToBattleMode::Reset()
{
    // Don't reset a transition in progress
    if(_transition_timer.IsRunning())
        return;

    _position = 0.0f;
    _transition_timer.Initialize(1500, SYSTEM_TIMER_NO_LOOPS);
    _transition_timer.Run();

    // Stop the map music
    AudioManager->StopAllMusic();

    // Play a random encounter sound
    uint32 file_id = vt_utils::RandomBoundedInteger(0, 2);
    vt_audio::AudioManager->PlaySound(encounter_sound_filenames[file_id]);
}

} // namespace vt_battle
