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
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \author  Corey Hoffstein, visage@allacrost.org
*** \author  Andy Gardner, chopperdave@allacrost.org
*** \brief   Header file for battle mode interface.
***
*** This code handles event processing, game state updates, and video frame
*** drawing when the user is fighting a battle.
*** ***************************************************************************/

#ifndef __BATTLE_HEADER__
#define __BATTLE_HEADER__

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

//! \brief Determines whether the code in the hoa_battle namespace should print debug statements or not.
extern bool BATTLE_DEBUG;

//! \brief An internal namespace to be used only within the battle code. Don't use this namespace anywhere else!
namespace private_battle {

/** ****************************************************************************
*** \brief A companion class to BattleMode that holds various multimedia data
***
*** Many of the battle mode interfaces require access to a common set of media data.
*** This class retains all of this common media data and makes it available for these
*** classes to utilize. It also serves to reduce the number of methods and members of
*** the BattleMode class.
***
*** \note Although most of the images and audio data here are public, you should take
*** extreme care when modifying any of the properties of this data, such as loading out
*** a different image or changing its size, as this could have implications for other
*** battle classes that also use this data.
*** ***************************************************************************/
class BattleMedia {
public:
	BattleMedia();

	~BattleMedia();

	/** \brief Sets the background image for the battle
	*** \param filename The filename of the new background image to load
	**/
	void SetBackgroundImage(const std::string& filename);

	/** \brief Sets the battle music to use
	*** \param filename The full filename of the music to play
	**/
	void SetBattleMusic(const std::string& filename);

	/** \brief Retrieves a specific button icon for character action
	*** \param index The index of the button to retrieve
	*** \return A pointer to the appropriate button image, or NULL if the index argument was out of bounds
	**/
	hoa_video::StillImage* GetCharacterActionButton(uint32 index);

	/** \brief Retrieves the appropriate icon image given a valid target type
	*** \param target_type The enumerated value that represents the type of target
	*** \return A pointer to the appropriate icon image, or NULL if the target type was invalid
	**/
	hoa_video::StillImage* GetTargetTypeIcon(hoa_global::GLOBAL_TARGET target_type);

	/** \brief Retrieves a specific status icon with the proper type and intensity
	*** \param type The type of status effect the user is trying to retrieve the icon for
	*** \param intensity The intensity level of the icon to retrieve
	*** \return The icon representation of the element type and intensity, or NULL if no appropriate image was found
	**/
	hoa_video::StillImage* GetStatusIcon(hoa_global::GLOBAL_STATUS type, hoa_global::GLOBAL_INTENSITY intensity);

	// ---------- Public members

	//! \brief The static background image to be used for the battle
	hoa_video::StillImage background_image;

	//! \brief The static image that is drawn for the bottom menus
	hoa_video::StillImage bottom_menu_image;

	/** \brief An image that indicates that a particular actor has been selected
	*** This image best suites character sprites and enemy sprites of similar size. It does not work
	*** well with larger or smaller sprites.
	**/
	hoa_video::StillImage actor_selection_image;

	/** \brief An image that points out the location of specific attack points on an actor
	*** This image may be used for both character and enemy actors. It is used to indicate an actively selected
	*** attack point, <b>not</b> just any attack points present.
	**/
	hoa_video::AnimatedImage attack_point_indicator;

	//! \brief Used to provide a background highlight for a selected character
	hoa_video::StillImage character_selected_highlight;

	//! \brief Used to provide a background highlight for a character that needs a command set
	hoa_video::StillImage character_command_highlight;

	//! \brief An image which contains the covers for the HP and SP bars
	hoa_video::StillImage character_bar_covers;

	/** \brief The universal stamina bar that is used to represent the state of battle actors
	*** All battle actors have a portrait that moves along this meter to signify their
	*** turn in the rotation.  The meter and corresponding portraits must be drawn after the
	*** character sprites.
	**/
	hoa_video::StillImage stamina_meter;

	//! \brief The image used to highlight stamina icons for selected actors
	hoa_video::StillImage stamina_icon_selected;

	/** \brief Image that indicates when a player may perform character swapping
	*** This image is drawn in the lower left corner of the screen. When no swaps are available to the player,
	*** the image is drawn in gray-scale.
	**/
	hoa_video::StillImage swap_icon;

	/** \brief Used for visual display of how many swaps a character may perform
	*** This image is drawn in the lower left corner of the screen, just above the swap indicator. This image
	*** may be drawn on the screen up to four times (in a card-stack fashion), one for each swap that is
	*** available to be used. It is not drawn when the player has no swaps available.
	**/
	hoa_video::StillImage swap_card;

	/** \brief Small button icons used to indicate when a player can select an action for their characters
	*** These buttons are used to indicate to the player what button to press to bring up a character's command
	*** menu. This vector is built from a 2-row, 5-column multi-image. The rows represent the buttons for when
	*** the character can be given a command (first row) versus when they may not (second row). The first element
	*** in each row is a "blank" button that is not used. The next four elements correspond to the characters on
	*** the screen, from top to bottom.
	**/
	std::vector<hoa_video::StillImage> character_action_buttons;

	//! \brief The music played during the battle
	hoa_audio::MusicDescriptor battle_music;

	//! \brief The music played after the player has won the battle
	hoa_audio::MusicDescriptor victory_music;

	//! \brief The music played after the player has lost the battle
	hoa_audio::MusicDescriptor defeat_music;

	//! \brief Various sounds that are played as the player performs menu actions
	//@{
	hoa_audio::SoundDescriptor confirm_sound;
	hoa_audio::SoundDescriptor cancel_sound;
	hoa_audio::SoundDescriptor cursor_sound;
	hoa_audio::SoundDescriptor invalid_sound;
	hoa_audio::SoundDescriptor finish_sound;
	//@}

private:
	/** \brief Container used to find the appropriate row index for each status type
	*** Status icons for all types of status are all contained within a single image. This container is used to
	*** quickly determine which row of icons in that image corresponds to each status type.
	**/
	std::map<hoa_global::GLOBAL_STATUS, uint32> _status_indeces;

	/** \brief Holds icon images that represent the different types of targets
	*** Target types include attack points, ally/enemy, and different parties.
	**/
	std::vector<hoa_video::StillImage> _target_type_icons;

	//! \brief Contains the entire set of status effect icons
	std::vector<hoa_video::StillImage> _status_icons;

}; // class BattleMedia

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
***
*** \bug If timers are paused when then the game enters pause mod or quit mode, when
*** it returns to battle mode the paused timers will incorrectly be resumed. Need
*** to save/restore additional state information about timers on a pause event.
*** ***************************************************************************/
class BattleMode : public hoa_mode_manager::GameMode {
	friend class private_battle::SequenceSupervisor;

public:
	BattleMode();

	~BattleMode();

	//! \brief Returns a pointer to the currently active instance of battle mode
	static BattleMode* CurrentInstance()
		{ return _current_instance; }

	//! \brief Provides access to the BattleMedia class object
	private_battle::BattleMedia& GetMedia()
		{ return _battle_media; }

	//! \name Inherited methods for the GameMode class
	//@{
	//! \brief Resets appropriate class members. Called whenever BattleMode is made the active game mode.
	void Reset();

	//! \brief This method calls different update functions depending on the battle state.
	void Update();

	//! \brief This method calls different draw functions depending on the battle state.
	void Draw();
	//@}

	/** \brief Sets the name of the script to execute during the battle
	*** \param filename The filename of the Lua script to load
	***
	*** This function should only be called once before the BattleMode class object is initialized (before Reset()
	*** is called for the first time). Calling it after the battle has been initialized will have no effect and
	*** print out a warning.
	**/
	void LoadBattleScript(const std::string& filename);

	/** \brief Adds a new active enemy to the battle field
	*** \param new_enemy A copy of the GlobalEnemy object to add to the battle
	*** This method uses the GlobalEnemy copy constructor to create a copy of the enemy. The GlobalEnemy
	*** passed as an argument should be in its default loaded state (that is, it should have an experience
	*** level equal to zero).
	**/
	void AddEnemy(hoa_global::GlobalEnemy* new_enemy);

	/** \brief Adds a new active enemy to the battle field
	*** \param new_enemy_id The id number of the new enemy to add to the battle
	*** This method works precisely the same was as the method which takes a GlobalEnemy argument,
	*** only this version will construct the global enemy just using its id (meaning that it has
	*** to open up the Lua file which defines the enemy). If the GlobalEnemy has already been
	*** defined somewhere else, it is better to pass it in to the alternative definition of this
	*** function.
	**/
	void AddEnemy(uint32 new_enemy_id)
		{ AddEnemy(new hoa_global::GlobalEnemy(new_enemy_id)); }

	/** \brief Restores the battle to its initial state, allowing the player another attempt to achieve victory
	***
	***
	**/
	void RestartBattle();

	//! \brief Pauses all timers used in any battle mode classes
	void FreezeTimers();

	//! \brief Unpauses all timers used in any battle mode classes
	void UnFreezeTimers();

	private_battle::BATTLE_STATE GetState() const
		{ return _state; }

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
	bool OpenCommandMenu(private_battle::BattleCharacter* character);

	//! \brief Returns true if the battle has an open and active script file running
	bool IsBattleScripted() const
		{ return (_battle_script.IsFileOpen() == true); }

	//! \brief Returns true if the battle has finished and entered either the victory or defeat state
	bool IsBattleFinished() const
		 { return ((_state == private_battle::BATTLE_STATE_VICTORY) || (_state == private_battle::BATTLE_STATE_DEFEAT)); }

	//! \brief Exits the battle performing any final changes as needed
	void Exit();

	//! \brief Returns the number of character actors in the battle, both living and dead
	uint32 GetNumberOfCharacters() const
		{ return _character_actors.size(); }

	//! \brief Returns the number of enemy actors in the battle, both living and dead
	uint32 GetNumberOfEnemies() const
		{ return _enemy_actors.size(); }

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
	void NotifyCharacterCommandComplete(private_battle::BattleCharacter* character);

	/** \brief Called to notify BattleMode when an actor is ready to execute an action
	*** \param actor A pointer to the actor who has entered the state ACTOR_STATE_READY
	**/
	void NotifyActorReady(private_battle::BattleActor* actor);

	/** \brief Performs any necessary changes in response to an actor's death
	*** \param actor A pointer to the actor who is now deceased
	**/
	void NotifyActorDeath(private_battle::BattleActor* actor);
	//@}

	//! \name Class member accessor methods
	//@{
	std::deque<private_battle::BattleCharacter*>& GetCharacterActors()
		{ return _character_actors; }

	std::deque<private_battle::BattleEnemy*>& GetEnemyActors()
		{ return _enemy_actors; }

	std::deque<private_battle::BattleActor*>& GetCharacterParty()
		{ return _character_party; }

	std::deque<private_battle::BattleActor*>& GetEnemyParty()
		{ return _enemy_party; }

	private_battle::CommandSupervisor* GetCommandSupervisor()
		{ return _command_supervisor; }

	private_battle::DialogueSupervisor* GetDialogueSupervisor()
		{ return _dialogue_supervisor; }
	//@}

private:
	//! \brief A static pointer to the currently active instance of battle mode
	static BattleMode* _current_instance;

	//! \brief Retains the current state of the battle
	private_battle::BATTLE_STATE _state;

	//! \brief A pointer to the BattleMedia object created to coincide with this instance of BattleMode
	private_battle::BattleMedia _battle_media;

	//! \name Battle script data
	//@{
	//! \brief The name of the Lua file used for scripting this battle
	std::string _script_filename;

	/** \brief The interface to the file which contains the battle's scripted routines
	*** The script remains open for as long as the BattleMode object exists. The script is required to
	*** have the following functions defined: "Initialize", "Update", and "Draw"
	**/
	hoa_script::ReadScriptDescriptor _battle_script;

	/** \brief A script function which assists with the BattleMode#Update method
	*** This function executes any code that needs to be performed on an update call. An example of
	*** one common operation is to detect certain conditions in battle and respond appropriately, such as
	*** triggering a dialogue.
	**/
	ScriptObject _update_function;

	/** \brief Script function which assists with the MapMode#Draw method
	*** This function executes any code that needs to be performed on a draw call. This allows us battle's to
	*** utilize custom lighting or other visual effects.
	**/
	ScriptObject _draw_function;
	//@}

	//! \name Battle supervisor classes
	//@{
	//! \brief Manages update and draw calls during special battle sequences
	private_battle::SequenceSupervisor* _sequence_supervisor;

	//! \brief Manages state and visuals when the player is selecting a command for a character
	private_battle::CommandSupervisor* _command_supervisor;

	//! \brief Stores and processes any dialogue that is to occur on the battle
	private_battle::DialogueSupervisor* _dialogue_supervisor;

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
	std::deque<private_battle::BattleCharacter*> _character_actors;

	/** \brief Identical to the _character_actors container except that the elements are BattleActor pointers
	*** \note This container is necessary for the GlobalTarget class, which needs a common data type so that
	*** it may point to either the character or enemy party.
	**/
	std::deque<private_battle::BattleActor*> _character_party;

	/** \brief Enemies that are presently fighting in the battle
	*** There is a theoretical limit on how many enemies may fight in one battle, but that is dependent upon
	*** the sprite size of all active enemies and this limit will be detected by the BattleMode class.
	*** This structure includes enemies that have zero hit points.
	**/
	std::deque<private_battle::BattleEnemy*> _enemy_actors;

	/** \brief Identical to the _enemy_actors container except that the elements are BattleActor pointers
	*** \note This container is necessary for the GlobalTarget class, which needs a common data type so that
	*** it may point to either the character or enemy party.
	**/
	std::deque<private_battle::BattleActor*> _enemy_party;

	/** \brief A FIFO queue of all actors that are ready to perform an action
	*** When an actor has completed the wait time for their warm-up state, they enter the ready state and are
	*** placed in this queue. The actor at the front of the queue is in the acting state, meaning that they are
	*** executing their action. All other actors in the queue are waiting for the acting actor to finish and
	*** be removed from the queue before they can take their turn.
	**/
	std::list<private_battle::BattleActor*> _ready_queue;
	//@}

	/** \brief The number of character swaps that the player may currently perform
	*** The maximum number of swaps ever allowed is four, thus the value of this class member will always have the range [0, 4].
	*** This member is also used to determine how many swap cards to draw on the battle screen.
	**/
	uint8 _current_number_swaps;

	////////////////////////////// PRIVATE METHODS ///////////////////////////////

	//! \brief Initializes all data necessary for the battle to begin
	void _Initialize();

	/** \brief Manages battle mode when it is in the initial state
	***
	*** This function serves to achieve the following parts of the battle initialization sequence:
	***  - Fade in the background image
	***  - Bring in both character and enemy sprites from off screen
	***  - Bring in the stamina bar and icons from off screen
	***  - Bring in the bottom battle menu
	**/
	void _InitialSequence();

	/** \brief Sets the origin location of all character and enemy actors
	*** The location of the actors in both parties is dependent upon the number and physical size of the actor
	*** (the size of its sprite image). This function implements the algorithm that determines those locations.
	**/
	void _DetermineActorLocations();

	//! \brief Returns the number of enemies that are still alive in the battle
	uint32 _NumberEnemiesAlive() const;

	/** \brief Returns the number of characters that are still alive in the battle
	*** \note This function only counts the characters on the screen, not characters in the party reserves
	**/
	uint32 _NumberCharactersAlive() const;

	//! \name Draw assistant functions
	//@{
	/** \brief Draws all background images and animations
	*** The images and effects drawn by this function will never be drawn over anything else in the battle
	*** (battle sprites, menus, etc.).
	**/
	void _DrawBackgroundGraphics();

	/** \brief Draws all character and enemy sprites as well as any sprite visuals
	*** In addition to the sprites themselves, this function draws special effects and indicators for the sprites.
	*** For example, the actor selector image and any visible action effects like magic.
	**/
	void _DrawSprites();

	//! \brief Draws all GUI graphics on the screen
	void _DrawGUI();

	/** \brief Draws the bottom menu visuals and information
	*** The bottom menu contains a wide array of information, including swap cards, character portraits, character names,
	*** and both character and enemy status. This menu is perpetually drawn on the battle screen.
	**/
	void _DrawBottomMenu();

	//! \brief Draws the stamina bar and the icons of the actors of both parties
	void _DrawStaminaBar();

	//! \brief Draws indicator text and graphics for each actor on the field
	void _DrawIndicators();
	//@}
}; // class BattleMode : public hoa_mode_manager::GameMode

} // namespace hoa_battle

#endif // __BATTLE_HEADER__
