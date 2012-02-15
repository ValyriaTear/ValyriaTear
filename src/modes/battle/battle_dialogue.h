///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_dialogue.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for battle dialogue code
***
*** Dialogue that occurs during a battle typically involves the characters in the
*** active party and their opponents, though this code supports external speakers
*** for those circumstances that require a speaker that is not directly on the
*** battlefield. Most of the time, the battle action is paused while a dialogue
*** is active. It is possible though to define dialogues that defy this common
*** behavior and are displayed while the battle is in progress. These types of
*** dialogues are typically timed or have some other termination condition, as
*** the player is unable to provide input to this type of dialogue.
***
*** The common format to constructing and executing a dialogue in a battle script
*** is to perform the following:
***
*** - In the script's "Load()" function, define all battle speakers and all dialogues
***   that may take place during the battle.
*** - In the script's "Update()" function, check for the conditions necessary to initiate
***   a dialogue and launch the dialogue when those conditions are met.
*** ***************************************************************************/

#ifndef __BATTLE_DIALOGUE_HEADER__
#define __BATTLE_DIALOGUE_HEADER__

// Allacrost utilities
#include "defs.h"
#include "utils.h"

// Allacrost engines
#include "video.h"

// Common code headers
#include "dialogue.h"


namespace hoa_battle {

namespace private_battle {

//! \brief Defines the different states the dialogue can be in.
enum DIALOGUE_STATE {
	DIALOGUE_STATE_INACTIVE =  0, //!< No dialogue is active
	DIALOGUE_STATE_LINE     =  1, //!< Active when the dialogue window is in the process of displaying a line of text
	DIALOGUE_STATE_OPTION   =  2, //!< Active when player-selectable options are present in the dialogue window
};


/** ****************************************************************************
*** \brief Conatiner for the data that represents a dialogue speaker
***
*** This is used only by the DialogueSupervisor to store its speaker data. No other
*** classes use nor need to know about this class. The ID of the speaker is omitted because
*** the supervisor class stores those values as keys to the std::map container that holds
*** instances of this speaker class.
*** *************************************************/
class BattleSpeaker {
public:
	//! \brief The name of this speaker as it will appear to the player
	hoa_utils::ustring name;

	/** \brief Holds a reference to the portrait image to use for this speaker
	***
	*** \note Not all speakers will have portraits available. For those that don't, this
	*** member will simply remain a blank image that is drawn to the screen.
	**/
	hoa_video::StillImage portrait;
}; // class BattleSpeaker


/** ****************************************************************************
*** \brief Represents a dialogue that occurs during the midst of battle
***
*** Battle dialogues are very similar to the common dialogue class and only expand
*** upon it in two ways. First, each line requires a speaker to be declared. This is
*** done by providing an integer ID that represents the speaker. Second, it provides
*** a single boolean member used to determine whether or not the battle action should
*** halt while the dialogue is active (the default behavior of a dialogue is to halt
*** battle action).
***
*** \note If you wish the dialogue to not halt the battle action, usually you want all
*** of the dialogue lines to be timed so that the dialogue progresses on its own. The
*** player is unable to send any input to the dialogue when this property is active. If you
*** do not have timing for all of the lines, then you will need some sort of mechanism in the
*** battle script that will terminate the dialogue or force it to proceed to the next line when
*** a certain condition is met. In general, be cautious about dialogues where battle action is
*** not halted.
*** ***************************************************************************/
class BattleDialogue : public hoa_common::CommonDialogue {
public:
	//! \param id The id number to represent the dialogue, which should be unique to other dialogue ids within this battle
	BattleDialogue(uint32 id);

	~BattleDialogue()
		{}

	/** \brief Adds a new line of text to the dialogue
	*** \param text The text to show on the screen
	*** \param speaker The ID of the speaker for this line
	***
	*** The following line properties are set when using this call:
	*** - proceed to next sequential line, no display time
	**/
	void AddLine(std::string text, uint32 speaker);

	/** \brief Adds a new line of text to the dialogue
	*** \param text The text to show on the screen
	*** \param speaker The ID of the speaker for this line
	*** \param next_line The line of dialogue which should follow this one
	***
	*** The following line properties are set when using this call:
	*** - no display time
	**/
	void AddLine(std::string text, uint32 speaker, int32 next_line);

	/** \brief Adds a new line of text to the dialogue that uses a display time
	*** \param text The text to show on the screen
	*** \param speaker The ID of the speaker for this line
	*** \param display_time The number of milliseconds that the line should be displayed for
	***
	*** The following line properties are set when using this call:
	*** - proceed to next sequential line
	**/
	void AddLineTimed(std::string text, uint32 speaker, uint32 display_time);

	/** \brief Adds a new line of text to the dialogue that uses a display time
	*** \param text The text to show on the screen
	*** \param speaker The ID of the speaker for this line
	*** \param next_line The line of dialogue which should follow this one
	*** \param display_time The number of milliseconds that the line should be displayed for
	**/
	void AddLineTimed(std::string text, uint32 speaker, int32 next_line, uint32 display_time);

	/** \brief Checks all the data stored by the dialogue class to ensure that it is acceptable and ready for use
	*** \return True if the validation was successful, false if any problems were discovered
	***
	*** \note This function should not be called until after all battle speakers have been added to the battle dialogue supervisor.
	*** The function checks that each speaker reference is valid and stored in the supervisor class.
	**/
	bool Validate();

	//! \brief Returns the speaker ID for the line specified (or zero if the line index was invalid)
	uint32 GetLineSpeaker(uint32 line) const
		{ if (line >= _line_count) return 0; else return _speakers[line]; }

	//! \name Class Member Access Functions
	//@{
	bool IsHaltBattleAction() const
		{ return _halt_battle_action; }

	void SetHaltBattleAction(bool halt)
		{ _halt_battle_action = halt; }
	//@}

private:
	//! \brief When true, the battle action will be suspended while this dialogue is active
	bool _halt_battle_action;

	//! \brief Contains the speaker ID number corresponding to each line of text
	std::vector<uint32> _speakers;
}; // class BattleDialogue : public hoa_common::CommonDialogue


/** ****************************************************************************
*** \brief Handles the storage and execution of dialogues
***
*** This supervisor class retains two data containers. The first container holds
*** all of the speakers while the second holds all of the dialogues. The BattleMode
*** class creates an instance of this supervisor and passes it control
*** ***************************************************************************/
class DialogueSupervisor {
public:
	DialogueSupervisor();

	~DialogueSupervisor();

	//! \brief Processes user input and updates the state of the dialogue
	void Update();

	//! \brief Draws the dialogue window, text, portraits, and other visuals to the screen
	void Draw();

	/** \brief Adds a new dialogue to be managed by the supervisor
	*** \param dialogue Pointer to a BattleDialogue object that was created with the new operator
	***
	*** The dialogue to add must have a unique dialogue ID that is not already stored by this class
	*** instance. If a dialogue is found with the same ID, the dialogue will not be added and the
	*** dialogue object will be deleted from memory. All dialogues that are successfully added will
	*** be later deleted when this class' destructor is invoked, so make sure you only pass in objects
	*** that were created with the "new" operator.
	**/
	void AddDialogue(BattleDialogue* dialogue);

	/** \brief Adds a new speaker using existing character data
	*** \param id The unique ID number for this speaker
	*** \param character A pointer to the BattleCharacter to use to construct the speaker data
	*** \note The map portrait of the character is used for their dialogue portrait, not their battle portrait.
	***
	*** You are not required to use this function if you wish to create a speaker that represents a character.
	*** The AddCustomSpeaker function can be used if you want to provide a custom name or portrait.
	**/
	void AddCharacterSpeaker(uint32 id, BattleCharacter* character);

	/** \brief Adds a new speaker using existing enemy data
	*** \param id The unique ID number for this speaker
	*** \param enemy A pointer to the BattleEnemy to use to construct the speaker data
	*** \note This function will not set a portrait image for the enemy as this data is not defined for enemies.
	***
	*** Note that you are not required to use this function if you wish to create a speaker that represents
	*** an enemy. The AddCustomSpeaker function can be used if you want to provide a custom name or portrait.
	**/
	void AddEnemySpeaker(uint32 id, BattleEnemy* enemy);

	/** \brief Adds a new speaker using custom name and portrait data
	*** \param id The unique ID number for this speaker
	*** \param name The name of the speaker
	*** \param portrait_filename The filename for the portrait to add
	***
	*** Pass in an empty string to the arguments if you do not want the speaker to have a name or portrait image.
	**/
	void AddCustomSpeaker(uint32 id, std::string name, std::string portrait);

	/** \brief Changes the name for a speaker that was previously added
	*** \param id The unique ID number for the speaker to change
	*** \param name The text to change the speaker's name to
	**/
	void ChangeSpeakerName(uint32 id, std::string name);

	/** \brief Changes the portrait image for a speaker that was previously added
	*** \param id The unique ID number for the speaker to change
	*** \param portrait The filename of the image to use as the speaker's portrait
	**/
	void ChangeSpeakerPortrait(uint32 id, std::string portrait);

	/** \brief Prepares the dialogue manager to begin processing a new dialogue
	*** \param dialogue_id The id number of the dialogue to begin
	**/
	void BeginDialogue(uint32 dialogue_id);

	//! \brief Immediately ends any dialogue that is taking place
	void EndDialogue();

	//! \brief Forces the current dialogue to proceed to the next line immediately
	void ForceNextLine();

	//! \brief Returns true if any dialogue is currently active at this time
	bool IsDialogueActive() const
		{ return (_current_dialogue != NULL); }

	/** \brief Returns a pointer to the BattleDialogue with the requested ID value
	*** \param dialogue_id The identification number of the dialogue to retrieve
	*** \return A pointer to the dialogue requested, or NULL if no such dialogue was found
	**/
	BattleDialogue* GetDialogue(uint32 dialogue_id);

	/** \brief Returns a pointer to the BattleSpeaker with the requested ID value
	*** \param speaker_id The unique ID number of the speaker to retrieve
	*** \return A pointer to the stored speaker, or NULL if no speaker was found with the specified ID
	**/
	BattleSpeaker* GetSpeaker(uint32 speaker_id);

	//! \name Class member access functions
	//@{
	BattleDialogue* GetCurrentDialogue() const
		{ return _current_dialogue; }

	hoa_common::CommonDialogueOptions* GetCurrentOptions() const
		{ return _current_options; }

	hoa_system::SystemTimer& GetLineTimer()
		{ return _line_timer; }

	uint32 GetLineCounter() const
		{ return _line_counter; }
	//@}

private:
	//! \brief Retains the current state of the dialogue execution
	DIALOGUE_STATE _state;

	//! \brief Contains data for all of the speakers for every dialogue and each line. The speaker ID is the map key.
	std::map<uint32, BattleSpeaker> _speakers;

	//! \brief Contains all dialogues that may begin for this battle. The dialogue ID is the map key.
	std::map<uint32, BattleDialogue*> _dialogues;

	//! \brief A pointer to the current piece of dialogue that is active
	BattleDialogue* _current_dialogue;

	//! \brief A pointer to the current set of options for the active dialogue line
	hoa_common::CommonDialogueOptions* _current_options;

	//! \brief A timer that employed for dialogues which have a display time limit
	hoa_system::SystemTimer _line_timer;

	//! \brief Keeps track of which line is active for the current dialogue
	uint32 _line_counter;

	//! \brief Holds the text and graphics that should be displayed for the dialogue
	hoa_common::CommonDialogueWindow _dialogue_window;

	// ---------- Private methods

	//! \brief Updates the dialogue when it is in the line state
	void _UpdateLine();

	//! \brief Updates the dialogue when it is in the option state
	void _UpdateOptions();

	/** \brief Begins the display of the line indexed by the value of _line_counter
	***
	*** This is called whenever a dialogue begins or is moved to the next line. Its duties include updating the
	*** dialogue state, dialogue window displays with data from the new line, and setting up the line timer.
	***
	*** \note This method does not check that the _line_counter member refers to a valid line. It is the caller's
	*** responsibility to ensure that _line_counter is valid prior to calling this method.
	**/
	void _BeginLine();

	/** \brief Finishes the current dialogue line and moves the dialogue forward to the next line
	***
	*** This function determines the next line that the dialogue should proceed to based on the properties of the
	*** current line. This includes "branching" to the appropriate next line based on the option selected by the player
	*** when options were enabled on the current line. Should the line counter become invalid or the dialogue is to end
	*** after the present line, this function will invoke the EndDialogue() method. In addition to proceeding to the next
	*** line, this method is also responsible for invoking any events that were to occur at the conclusion of the present
	*** line.
	**/
	void _EndLine();
}; // class DialogueSupervisor

} // namespace private_battle

} // namespace hoa_battle

#endif // __BATTLE_DIALOGUE_HEADER__
