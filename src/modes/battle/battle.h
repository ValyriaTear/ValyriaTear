////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \author  Corey Hoffstein, visage@allacrost.org
*** \author  Andy Gardner, chopperdave@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for battle mode interface.
***
*** This code handles event processing, game state updates, and video frame
*** drawing when the user is fighting a battle.
*** ***************************************************************************/

#ifndef __BATTLE_HEADER__
#define __BATTLE_HEADER__

#include "battle_utils.h"
#include "battle_menu.h"

#include "engine/mode_manager.h"

#include "common/global/global_actors.h"

namespace vt_common
{
class DialogueSupervisor;
}

namespace vt_battle
{

//! \brief Determines whether the code in the vt_battle namespace should print debug statements or not.
extern bool BATTLE_DEBUG;

//! \brief An internal namespace to be used only within the battle code. Don't use this namespace anywhere else!
namespace private_battle
{

class BattleActor;
class BattleCharacter;
class BattleEnemy;
class BattleObject;
class BattleParticleEffect;
class BattleAnimation;
class CommandSupervisor;
class FinishSupervisor;
class SequenceSupervisor;

/** \name Battle setting type
*** \brief Represents the play types of battle that the player may have to deal with
**/
enum BATTLE_TYPE {
    BATTLE_TYPE_INVALID       = -1,
    //! Battle action will pause only when one or more characters is in the ACTOR_STATE_COMMAND state
    BATTLE_TYPE_WAIT          =  0,
    //! Monsters will wait for characters to finish their command before attacking
    //! but will move up until being ready anyway.
    BATTLE_TYPE_SEMI_ACTIVE   =  1,
    //! Battle action does not pause at any time during a battle, except for scripted events
    BATTLE_TYPE_ACTIVE        =  2,
    BATTLE_TYPE_TOTAL         =  3
};

/** \brief Factors which adjust the speed of all actors for the various battle types
*** This adjustment is necessary because a single speed will not work well for the various types,
*** as the player needs more time to select an action in the active type, where the battle action
*** never pauses, and less time in the wait type, where the battle will always pause when the
*** player is selecting an action.
**/
//@{
const float BATTLE_WAIT_FACTOR          = 3.0f;
const float BATTLE_SEMI_ACTIVE_FACTOR   = 1.5f;
const float BATTLE_ACTIVE_FACTOR        = 1.0f;
//@}

//! \brief A simple structure keeping the enemy info to later permit to reinsert
//! it as it was first requested, on battles restarts.
struct BattleEnemyInfo {
    BattleEnemyInfo():
        id(0),
        pos_x(0.0f),
        pos_y(0.0f)
    {}

    BattleEnemyInfo(uint32_t _id):
        id(_id),
        pos_x(0.0f),
        pos_y(0.0f)
    {}

    BattleEnemyInfo(uint32_t _id, float x, float y):
        id(_id),
        pos_x(x),
        pos_y(y)
    {}

    //! \brief the battle enemy id
    uint32_t id;
    //! \brief The x enemy position on the battle field, or 0 if default one.
    float pos_x;
    //! \brief The y enemy position on the battle field, or 0 if default one.
    float pos_y;
};

} // namespace private_battle

/** ****************************************************************************
*** \brief Manages all objects, events, and scenes that occur in a battle
***
*** To create a battle, first you must create an instance of this class. Next,
*** the battle must be populated with enemies by using the AddEnemy() methods
*** prescribed below. You must then call the InitializeEnemies() method so that
*** the added enemies are ready for the battle to come. This should all be done
*** prior the Reset() method being called. If you fail to add any enemies,
*** an error will occur and the battle will self-terminate itself.
*** ***************************************************************************/
class BattleMode : public vt_mode_manager::GameMode
{
    friend class private_battle::SequenceSupervisor;

public:
    BattleMode();

    ~BattleMode();

    //! \brief Returns a pointer to the currently active instance of battle mode
    static BattleMode *CurrentInstance() {
        return _current_instance;
    }

    //! \name Inherited methods for the GameMode class
    //@{
    //! \brief Resets appropriate class members. Called whenever BattleMode is made the active game mode.
    void Reset();

    //! \brief This method calls different update functions depending on the battle state.
    void Update();

    //! \brief This method calls different draw functions depending on the battle state.
    void Draw();

    //! \brief This method calls different draw functions depending on the battle state.
    void DrawPostEffects();
    //@}

    /** \brief Adds a new active enemy to the battle field
    *** \param new_enemy_id The id number of the new enemy to add to the battle
    *** \param position_x, position_y The enemy sprite position on the battle ground in pixels
    *** If both are equal to 0.0f, the position is automatically computed.
    *** This method works precisely the same was as the method which takes a GlobalEnemy argument,
    *** only this version will construct the global enemy just using its id (meaning that it has
    *** to open up the Lua file which defines the enemy). If the GlobalEnemy has already been
    *** defined somewhere else, it is better to pass it in to the alternative definition of this
    *** function.
    **/
    void AddEnemy(uint32_t new_enemy_id, float position_x, float position_y);
    void AddEnemy(uint32_t new_enemy_id) {
        AddEnemy(new_enemy_id, 0.0f, 0.0f);
    }

    /** \brief Restores the battle to its initial state, allowing the player another attempt to achieve victory
    *** This function is permitted only when the battle state isn't invalid, as this value is reserved
    *** for battles that haven't started yet.
    **/
    void RestartBattle();

    //! \brief Tells the battle actor class whether
    //! it should update the state timer.
    bool AreActorStatesPaused() const {
        return _actor_state_paused;
    }

    void SetActorStatePaused(bool state) {
        _actor_state_paused = state;
    }

    private_battle::BATTLE_STATE GetState() const {
        return _state;
    }

    //! \brief Sets the battle in scene mode, pausing the actors actions and states.
    void SetSceneMode(bool scene_mode) {
        _scene_mode = scene_mode;
    }

    //! \brief Tells whether the battle is paused and playing a scene
    bool IsInSceneMode() const {
        return _scene_mode;
    }

    //! \brief Tells whether user input is accepted in dialogues.
    //! Used by the common dialogue supervisor.
    //! In the battle mode, dialogues can handle input only when in scene mode.
    bool AcceptUserInputInDialogues() const {
        return _scene_mode;
    }

    /** \brief Changes the state of the battle and performs any initializations and updates needed
    *** \param new_state The new state to change the battle to
    **/
    void ChangeState(private_battle::BATTLE_STATE new_state);

    /** \brief Requests battle mode to enter the command state and to open the command menu for a specific character
    *** \param character A pointer to the character to enter commands for
    *** \return True only if the requested operation was accepted
    ***
    *** This method does not guarantee that any change will take place. If the command menu is already open for a
    *** different character, it will reject the request.
    **/
    bool OpenCommandMenu(private_battle::BattleCharacter *character);

    //! \brief Returns true if the battle has finished and entered either the victory or defeat state
    bool IsBattleFinished() const {
        return ((_state == private_battle::BATTLE_STATE_VICTORY) || (_state == private_battle::BATTLE_STATE_DEFEAT));
    }

    //! \brief Returns the number of character actors in the battle, both living and dead
    uint32_t GetNumberOfCharacters() const {
        return _character_actors.size();
    }

    //! \brief Returns the number of enemy actors in the battle, both living and dead
    uint32_t GetNumberOfEnemies() const {
        return _enemy_actors.size();
    }

    //! \brief Computes whether at least one battle character is dead.
    bool isOneCharacterDead() const;

    /** \name Battle notification methods
    *** These methods are called by other battle classes to indicate events such as when an actor
    *** changes its state. Often BattleMode will respond by updating the state of one or more of its
    *** members and calling other battle classes to notify them of the event as well.
    **/
    //@{
    //! \brief Called whenever the player is in the command menu and exits it without selecting an action
    void NotifyCommandCancel();

    /** \brief Called whenever the player has finished selecting a command for a character
    *** \param character A pointer to the character that just had its command completed.
    **/
    void NotifyCharacterCommandComplete(private_battle::BattleCharacter *character);

    /** \brief Called to notify BattleMode when an actor is ready to execute an action
    *** \param actor A pointer to the actor who has entered the state ACTOR_STATE_READY
    **/
    void NotifyActorReady(private_battle::BattleActor *actor);

    /** \brief Performs any necessary changes in response to an actor's death
    *** \param actor A pointer to the actor who is now deceased
    **/
    void NotifyActorDeath(private_battle::BattleActor *actor);
    //@}

    //! \brief Tells the battle type: Wait, semi-wait, active.
    //! \see BATTLE_TYPE enum.
    vt_battle::private_battle::BATTLE_TYPE GetBattleType() const {
        return _battle_type;
    }

    //! \brief Tells the battle type: Wait, semi-wait, active.
    //! \see BATTLE_TYPE enum.
    void SetBattleType(vt_battle::private_battle::BATTLE_TYPE battle_type) {
        _battle_type = battle_type;
    }

    float GetBattleTypeTimeFactor() const {
        return _battle_type_time_factor;
    }

    //! \name Class member accessor methods
    //@{
    std::deque<private_battle::BattleCharacter *>& GetCharacterActors() {
        return _character_actors;
    }

    private_battle::BattleCharacter* GetCharacterActor(uint32_t index) {
        if (index >= _character_actors.size())
            return nullptr;
        return _character_actors[index];
    }

    std::deque<private_battle::BattleEnemy *>& GetEnemyActors() {
        return _enemy_actors;
    }

    private_battle::BattleEnemy* GetEnemyActor(uint32_t index) {
        if (index >= _enemy_actors.size())
            return nullptr;
        return _enemy_actors[index];
    }

    std::deque<private_battle::BattleActor *>& GetCharacterParty() {
        return _character_party;
    }

    std::deque<private_battle::BattleActor *>& GetEnemyParty() {
        return _enemy_party;
    }

    private_battle::CommandSupervisor* GetCommandSupervisor() {
        return _command_supervisor;
    }

    vt_common::DialogueSupervisor* GetDialogueSupervisor() {
        return _dialogue_supervisor;
    }

    //! \brief Sets or updates the battle actor idle state time to reflect its current stamina.
    //! \note the _highest_stamina and _battle_type_time_factor members must be set before calling
    //! this method.
    void SetActorIdleStateTime(private_battle::BattleActor *actor);

    //! \brief Triggers a battle particle effect at the given location.
    //! We do not use the particle manager here as we're considering the particle effect
    //! as a battle object which has to be drawn along other battle others sorted by the Y coordinate.
    //!
    //! \param The effect filename is the particle effect definition file.
    //! \param x the x coordinates of the particle effect in pixels.
    //! \param y the y coordinates of the particle effect in pixels.
    void TriggerBattleParticleEffect(const std::string& effect_filename, uint32_t x, uint32_t y);

    //! \brief Creates a battle animation object.
    //! Those objects are also drawn sorted by their Y coordinate value.
    //! Note that at the animation is created invisible at coordinate (0,0)
    //! and that you must call SetVisible(true) and move it somewhere visible
    //! for it to be shown.
    //! Once you don't need it anymore, you can throw it by calling Remove()
    //! and the animation will be freed from memory on the next Battle update.
    //!
    //! \param The animation filename is the animation definition file.
    //! \return the animation object for scripted manipulation purpose.
    private_battle::BattleAnimation* CreateBattleAnimation(const std::string& animation_filename);

    //! \brief Sets whether the current fight is a fight including a boss.
    //! N.B.: Certain items shouldn't work in a boss fight, for instance.
    void SetBossBattle(bool is_boss = true) {
        _is_boss_battle = is_boss;
    }

    //! \brief Tells whether the current fight is a fight including a boss.
    bool IsBossBattle() const {
        return _is_boss_battle;
    }

    //! \brief Tells the battle mode Heroes will receive an aguility boost at battle start.
    void BoostHeroPartyInitiative() {
        _hero_init_boost = true;
    }

    //! \brief Tells the battle mode Enemies will receive an aguility boost at battle start.
    void BoostEnemyPartyInitiative() {
        _enemy_init_boost = true;
    }
    //@}

private:

    //! \brief A static pointer to the currently active instance of battle mode
    static BattleMode* _current_instance;

    //! \brief Retains the current state of the battle
    private_battle::BATTLE_STATE _state;

    //! \name Battle supervisor classes
    //@{
    //! \brief Manages update and draw calls during special battle sequences
    private_battle::SequenceSupervisor* _sequence_supervisor;

    //! \brief Manages state and visuals when the player is selecting a command for a character
    private_battle::CommandSupervisor* _command_supervisor;

    //! \brief Stores and processes any dialogue that is to occur on the battle
    vt_common::DialogueSupervisor* _dialogue_supervisor;

    //! \brief Presents player with information and options after a battle has concluded
    private_battle::FinishSupervisor* _finish_supervisor;
    //@}

    //! \name Battle Actor Containers
    //@{
    /** \brief Characters that are presently fighting in the battle
    *** No more than four characters may be fighting at any given time, thus this structure will never
    *** contain more than four BattleCharacter objects. This structure does not include any characters
    *** that are in the party, but not actively fighting in the battle. This structure includes
    *** characters that have zero hit points.
    **/
    std::deque<private_battle::BattleCharacter *> _character_actors;

    /** \brief Identical to the _character_actors container except that the elements are BattleActor pointers
    *** \note This container is necessary for the GlobalTarget class, which needs a common data type so that
    *** it may point to either the character or enemy party.
    **/
    std::deque<private_battle::BattleActor *> _character_party;

    /** \brief Enemies that are presently fighting in the battle
    *** There is a theoretical limit on how many enemies may fight in one battle, but that is dependent upon
    *** the sprite size of all active enemies and this limit will be detected by the BattleMode class.
    *** This structure includes enemies that have zero hit points.
    **/
    std::deque<private_battle::BattleEnemy *> _enemy_actors;

    /** \brief Identical to the _enemy_actors container except that the elements are BattleActor pointers
    *** \note This container is necessary for the GlobalTarget class, which needs a common data type so that
    *** it may point to either the character or enemy party.
    **/
    std::deque<private_battle::BattleActor *> _enemy_party;

    //! \brief A copy of the enemy actors id at the beginning of the battle. Useful when restarting the battle,
    //! as the number of enemies might have changed.
    std::deque<private_battle::BattleEnemyInfo> _initial_enemy_actors_info;

    /** \brief The effects container.
    *** It will permit to draw particle effects and animations in the right order,
    *** and will get rid of the "dead" useless effects at update time.
    **/
    std::vector<private_battle::BattleObject *> _battle_effects;

    /** \brief A FIFO queue of all actors that are ready to perform an action
    *** When an actor has completed the wait time for their warm-up state, they enter the ready state and are
    *** placed in this queue. The actor at the front of the queue is in the acting state, meaning that they are
    *** executing their action. All other actors in the queue are waiting for the acting actor to finish and
    *** be removed from the queue before they can take their turn.
    **/
    std::list<private_battle::BattleActor *> _ready_queue;
    //@}

    /** \brief Vector used to draw all battle objects based on their y coordinate.
    *** Sorted in the update() method.
    **/
    std::vector<private_battle::BattleObject *> _battle_objects;

    /** \brief The number of character swaps that the player may currently perform
    *** The maximum number of swaps ever allowed is four, thus the value of this class member will always have the range [0, 4].
    *** This member is also used to determine how many swap cards to draw on the battle screen.
    **/
    uint8_t _current_number_swaps;

    /** \brief Tells whether the last enemy is dying.
    *** In that case, the battle character action must be canceled, and the command made unavailable
    *** until the last die animation is done.
    **/
    bool _last_enemy_dying;

    //! \brief the Stamina Icon general transluency. Used to make the characters's stamina icon disappear on wins.
    float _stamina_icon_alpha;

    //! \brief Tells whether the state of battle actors should be paused. Used in wait battle types.
    bool _actor_state_paused;

    //! Tells whether the battle is in scene mode
    //! The actor states should then be paused, the dialogues played if there are some,
    //! But the actors animations and indicators should still updates.
    //! The effects shouldn't update though.
    bool _scene_mode;

    //! \brief Retains the play type setting for battle that the user requested (e.g. wait mode, active mode, etc).
    vt_battle::private_battle::BATTLE_TYPE _battle_type;

    //! \brief Setup at battle start, and used to normalize the battle actors speed in battle.
    uint32_t _highest_stamina;

    //! \brief the battle type time factor, speeding the battle actors depending on the battle type.
    float _battle_type_time_factor;

    //! \brief Tells whether the battle is a boss fight.
    bool _is_boss_battle;

    //! \brief The battle menu
    vt_battle::private_battle::BattleMenu _battle_menu;

    //! \brief Whether the hero party should get an initiative boost at battle start.
    bool _hero_init_boost;

    //! \brief Whether the enemy party should get an initiative boost at battle start.
    bool _enemy_init_boost;


    ////////////////////////////// PRIVATE METHODS ///////////////////////////////

    //! \brief Initializes all data necessary for the battle to begin
    void _Initialize();

    //! \brief Set the battle music state
    void _ResetMusicState();

    /** \brief Applies a battle command to a given character automatically.
    *** \param character The character which will receive the command.
    **/
    void _AutoCharacterCommand(private_battle::BattleCharacter* character);

    /** \brief Sets the origin location of all character and enemy actors
    *** The location of the actors in both parties is dependent upon the number and physical size of the actor
    *** (the size of its sprite image). This function implements the algorithm that determines those locations.
    **/
    void _DetermineActorLocations();

    //! \brief Returns the number of enemies that are still alive in the battle
    uint32_t _NumberEnemiesAlive() const;

    /** \brief Returns the number of enemies that are still capable to fight in the battle.
    *** Which isn't the number of alive enemies, since that function can tell whether an enemy
    *** is currently dying.
    **/
    uint32_t _NumberValidEnemies() const;

    /** \brief Returns the number of characters that are still alive in the battle
    *** \note This function only counts the characters on the screen, not characters in the party reserves
    **/
    uint32_t _NumberCharactersAlive() const;


    //! \name Draw assistant functions
    //@{
    /** \brief Draws all background images and animations
    *** The images and effects drawn by this function will never be drawn over anything else in the battle
    *** (battle sprites, menus, etc.).
    **/
    void _DrawBackgroundGraphics();

    /** \brief Draws all characters, enemy sprites as well as any sprite visuals
    *** In addition to the sprites themselves, this function draws special effects and indicators for the sprites.
    *** For example, the actor selector image and any visible action effects like magic.
    **/
    void _DrawSprites();

    /** \brief Draws all foreground images and animations
    *** The images and effects drawn by this function will be drawn over sprites,
    *** but not over the post effects and the gui.
    **/
    void _DrawForegroundGraphics();

    //! \brief Draws all GUI graphics on the screen
    void _DrawGUI();

    /** \brief Draws the bottom menu visuals and information
    *** The bottom menu contains a wide array of information, including swap cards, character portraits, character names,
    *** and both character and enemy status. This menu is perpetually drawn on the battle screen.
    **/
    void _DrawBottomMenu();

    //! \brief Draws the stamina bar and the icons of the actors of both parties
    void _DrawStaminaBar();
    //@}
}; // class BattleMode : public vt_mode_manager::GameMode


/** ****************************************************************************
*** \brief Handles transition from an event or a mode to the battle mode
***
*** Must be called without fade transition, as it will do it.
***
*** ***************************************************************************/
class TransitionToBattleMode : public vt_mode_manager::GameMode
{
public:
    TransitionToBattleMode(BattleMode *BM, bool is_boss = false);

    ~TransitionToBattleMode() {
        // If the game quits while in pause mode during a transition to battle,
        // The battle mode object needs to be freed.
        if (_BM)
            delete _BM;
    }

    void Update();

    void Draw();

    void Reset();

private:
    //! \brief The screen capture of the moment of the encounter
    vt_video::StillImage _screen_capture;

    //! \brief The transition timer, used to display the encounter visual effect
    vt_system::SystemTimer _transition_timer;

    //! \brief Used to display the effect
    float _position;

    //! \brief Tells whether the boss trigger sound is to be played or not.
    bool _is_boss;

    //! \brief The Battle mode to trigger afterward. Must not be nullptr.
    BattleMode *_BM;
};

} // namespace vt_battle

#endif // __BATTLE_HEADER__
