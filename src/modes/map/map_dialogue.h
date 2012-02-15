///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_dialogue.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for map mode dialogue
*** ***************************************************************************/

#ifndef __MAP_DIALOGUE_HEADER__
#define __MAP_DIALOGUE_HEADER__

// Allacrost utilities
#include "utils.h"
#include "defs.h"

// Allacrost engines
#include "script.h"
#include "video.h"

// Allacrost common
#include "dialogue.h"
#include "gui.h"

// Local map mode headers
#include "map_utils.h"

namespace hoa_map {

namespace private_map {

/** ****************************************************************************
*** \brief Represents a dialogue that occurs between one or more sprites on a map
*** ***************************************************************************/
class SpriteDialogue : public hoa_common::CommonDialogue {
public:
	//! \param id The id number to represent the dialogue, which should be unique to other dialogue ids within this map
	SpriteDialogue(uint32 id);

	~SpriteDialogue()
		{}

	/** \brief Adds a new line of text to the dialogue
	*** \param text The text to show on the screen
	*** \param speaker The object ID of the sprite speaking this line
	***
	*** The following line properties are set when using this call:
	*** - proceed to next sequential line, no display time, no event
	**/
	void AddLine(std::string text, uint32 speaker);

	/** \brief Adds a new line of text to the dialogue
	*** \param text The text to show on the screen
	*** \param speaker The object ID of the sprite speaking this line
	*** \param next_line The line of dialogue which should follow this one
	***
	*** The following line properties are set when using this call:
	*** - no display time, no event
	**/
	void AddLine(std::string text, uint32 speaker, int32 next_line);

	/** \brief Adds a new line of text to the dialogue that uses a display time
	*** \param text The text to show on the screen
	*** \param speaker The object ID of the sprite speaking this line
	*** \param display_time The number of milliseconds that the line should be displayed for
	***
	*** The following line properties are set when using this call:
	*** - proceed to next sequential line, no event
	**/
	void AddLineTimed(std::string text, uint32 speaker, uint32 display_time);

	/** \brief Adds a new line of text to the dialogue that uses a display time
	*** \param text The text to show on the screen
	*** \param speaker The object ID of the sprite speaking this line
	*** \param next_line The line of dialogue which should follow this one
	*** \param display_time The number of milliseconds that the line should be displayed for
	***
	*** The following line properties are set when using this call:
	*** - no event
	**/
	void AddLineTimed(std::string text, uint32 speaker, int32 next_line, uint32 display_time);

	/** \brief Adds a new line of text to the dialogue that uses a map event
	*** \param text The text to show on the screen
	*** \param speaker The object ID of the sprite speaking this line
	*** \param event_id The ID of the event to execute after this line finishes
	***
	*** The following line properties are set when using this call:
	*** - proceed to next sequential line, no display time
	**/
	void AddLineEvent(std::string text, uint32 speaker, uint32 event_id);

	/** \brief Adds a new line of text to the dialogue that uses a map event
	*** \param text The text to show on the screen
	*** \param speaker The object ID of the sprite speaking this line
	*** \param next_line The line of dialogue which should follow this one
	*** \param display_time The number of milliseconds that the line should be displayed for
	***
	*** The following line properties are set when using this call:
	*** - no event
	**/
	void AddLineEvent(std::string text, uint32 speaker, int32 next_line, uint32 event_id);

	/** \brief Adds a new line of text to the dialogue that uses a map event
	*** \param text The text to show on the screen
	*** \param speaker The object ID of the sprite speaking this line
	*** \param display_time The number of milliseconds that the line should be displayed for
	*** \param event_id The ID of the event to execute after this line finishes
	***
	*** The following line properties are set when using this call:
	*** - proceed to next sequential line
	**/
	void AddLineTimedEvent(std::string text, uint32 speaker, uint32 display_time, uint32 event_id);

	/** \brief Adds a new line of text to the dialogue that uses a map event
	*** \param text The text to show on the screen
	*** \param speaker The object ID of the sprite speaking this line
	*** \param next_line The line of dialogue which should follow this one
	*** \param display_time The number of milliseconds that the line should be displayed for
	*** \param event_id The ID of the event to execute after this line finishes
	**/
	void AddLineTimedEvent(std::string text, uint32 speaker, int32 next_line, uint32 display_time, uint32 event_id);

	/** \brief Adds an option to the most recently added line of text
	*** \param text The text for this particular option
	*** \note If no lines have been added to the dialogue yet, this option will not be added and a warning will be issued
	***
	*** The following option properties are set when using this call:
	*** - proceed to next sequential line, no event
	**/
	void AddOption(std::string text);

	/** \brief Adds an option to the most recently added line of text
	*** \param text The text for this particular option
	*** \param next_line The index value of the next line of dialogue to display should this option be selected
	*** \note If no lines have been added to the dialogue yet, this option will not be added and a warning will be issued
	***
	*** The following option properties are set when using this call:
	*** - no event
	**/
	void AddOption(std::string text, int32 next_line);

	/** \brief Adds an option to the most recently added line of text
	*** \param text The text for this particular option
	*** \param event_id The ID of the event to execute should this option be selected
	*** \note If no lines have been added to the dialogue yet, this option will not be added and a warning will be issued
	***
	*** The following option properties are set when using this call:
	*** - proceed to next sequential line
	**/
	void AddOptionEvent(std::string text, uint32 event_id);

	/** \brief Adds an option to the most recently added line of text
	*** \param text The text for this particular option
	*** \param next_line The index value of the next line of dialogue to display should this option be selected
	*** \param event_id The ID of the event to execute should this option be selected
	*** \note If no lines have been added to the dialogue yet, this option will not be added and a warning will be issued
	**/
	void AddOptionEvent(std::string text, int32 next_line, uint32 event_id);

	/** \brief Checks all the data stored by the dialogue class to ensure that it is acceptable and ready for use
	*** \return True if the validation was successful, false if any problems were discovered
	***
	*** \note This function should not be called until after all map sprites have been added. The function checks that each
	*** speaker is valid and stored in the map's object list, so if you perform this check before you've added all the speakers
	*** to the object list of MapMode, the validation will fail.
	**/
	bool Validate();

	//! \name Methods for retrieving properties of a specific line
	//@{
	//! \brief Returns the object ID of the speaker for the line specified (or zero if the line index was invalid)
	uint32 GetLineSpeaker(uint32 line) const
		{ if (line >= _line_count) return 0; else return _speakers[line]; }

	//! \brief Returns the ID of the event to execute for the line specified (or zero if the line index was invalid)
	uint32 GetLineEvent(uint32 line) const
		{ if (line >= _line_count) return 0; else return _events[line]; }
	//@}

	//! \name Class Member Access Functions
	//@{
	std::string GetEventName() const
		{ return _event_name; }

	bool IsInputBlocked() const
		{ return _input_blocked; }

	bool IsRestoreState() const
		{ return _restore_state; }

	void SetInputBlocked(bool blocked)
		{ _input_blocked = blocked; }

	void SetRestoreState(bool restore)
		{ _restore_state = restore; }
	//@}

private:
	//! \brief If true, dialogue will ignore user input and instead execute independently
	bool _input_blocked;

	//! \brief If true, the state of map sprites participating in this dialogue will be reset after the dialogue completes
	bool _restore_state;

	//! \brief The event name for this dialogue that is stored in the saved game file, of the form "dialogue#"
	std::string _event_name;

	//! \brief Contains object ID numbers that declare the speaker of each line
	std::vector<uint32> _speakers;

	//! \brief An optional MapEvent that may occur after each line is completed
	std::vector<uint32> _events;
}; // class SpriteDialogue : public hoa_common::CommonDialogue


/** ***************************************************************************************
*** \brief A container class for option sets presented during a map dialogue
***
*** When the player reads a dialogue, they may be presented with a small number of options to
*** select from when coming to a particular line. The selected option determines the next line
*** that will follow. Optionally, each particular option may trigger a different map event when
*** it is selected.
*** **************************************************************************************/
class MapDialogueOptions : public hoa_common::CommonDialogueOptions {
public:
	MapDialogueOptions()
		{}

	~MapDialogueOptions()
		{}

	/** \brief Adds a new option to the set of options
	*** \param text The text for the new option
	***
	*** The following option properties are set when using this call:
	*** - proceed to next sequential line, no event
	**/
	void AddOption(std::string text);

	/** \brief Adds a new option to the set of options
	*** \param text The text for the new option
	*** \param next_line An integer index of the next line of dialogue should this option be selected.
	***
	*** The following option properties are set when using this call:
	*** - no event
	**/
	void AddOption(std::string text, int32 next_line);

	/** \brief Adds a new option to the set of options with the addition of a map event to be executed
	*** \param text The text for the new option
	*** \param event_id The ID of the event to execute should this option be selected
	***
	*** The following option properties are set when using this call:
	*** - proceed to next sequential line
	**/
	void AddOptionEvent(std::string text, uint32 event_id);

	/** \brief Adds a new option to the set of options with the addition of a map event to be executed
	*** \param text The text for the new option
	*** \param next_line An integer index of the next line of dialogue should this option be selected.
	*** \param event_id The ID of the event to execute should this option be selected
	**/
	void AddOptionEvent(std::string text, int32 next_line, uint32 event_id);

	//! \name Methods for retrieving properties of a specific line
	//@{
	uint32 GetOptionEvent(uint32 option) const
		{ if (option >= GetNumberOptions()) return 0; else return _events[option]; }
	//@}

	//! \brief Returns the number of options stored by this class
	uint32 GetNumberOptions() const
		{ return _text.size(); }

private:
	//! \brief An optional MapEvent that may occur as a result of selecting each option
	std::vector<uint32> _events;
}; // class MapDialogueOptions : public hoa_common::CommonDialogueOptions


/** ****************************************************************************
*** \brief Manages dialogue execution on maps
***
*** The MapMode class creates an instance of this class to handle all dialogue
*** processing that occurs on the map. This includes containing the dialogue objects,
*** handling user input, processing of dialogue events, and display timing of the
*** dialogue.
***
*** \todo Add support so that the player may backtrack through lines in a
*** dialogue (without re-processing selected options or previous script events).
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
	*** \param dialogue Pointer to a CommonDialogue object that was created with the new operator
	***
	*** The dialogue to add must have a unique dialogue ID that is not already stored by this class
	*** instance. If a dialogue is found with the same ID, the dialogue will not be added and the
	*** dialogue object will be deleted from memory. All dialogues that are successfully added will
	*** be later deleted when this class' destructor is invoked, so make sure you only pass in objects
	*** that were created with the "new" operator.
	**/
	void AddDialogue(SpriteDialogue* dialogue);

	/** \brief Prepares the dialogue manager to begin processing a new dialogue
	*** \param dialogue_id The id number of the dialogue to begin
	**/
	void BeginDialogue(uint32 dialogue_id);

	//! \brief Immediately ends any dialogue that is taking place
	void EndDialogue();

	/** \brief Returns a pointer to the CommonDialogue with the requested ID value
	*** \param dialogue_id The identification number of the dialogue to retrieve
	*** \return A pointer to the dialogue requested, or NULL if no such dialogue was found
	**/
	SpriteDialogue* GetDialogue(uint32 dialogue_id);

	//! \name Class member access functions
	//@{
	DIALOGUE_STATE GetDialogueState() const
		{ return _state; }

	SpriteDialogue* GetCurrentDialogue() const
		{ return _current_dialogue; }

	MapDialogueOptions* GetCurrentOptions() const
		{ return _current_options; }

	hoa_system::SystemTimer& GetLineTimer()
		{ return _line_timer; }

	uint32 GetLineCounter() const
		{ return _line_counter; }
	//@}

private:
	//! \brief Retains the current state of the dialogue execution
	DIALOGUE_STATE _state;

	//! \brief Contains all dialogues used in the map in a std::map structure. The dialogue IDs serve as the map keys
	std::map<uint32, SpriteDialogue*> _dialogues;

	//! \brief A pointer to the current piece of dialogue that is active
	SpriteDialogue* _current_dialogue;

	//! \brief A pointer to the current set of options for the active dialogue line
	MapDialogueOptions* _current_options;

	//! \brief A timer employed for dialogues which have a display time limit
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

	//! \brief Restores participating sprites to their state before this dialogue started
	void _RestoreSprites();
}; // class DialogueSupervisor

} // namespace private_map

} // namespace hoa_map

#endif // __MAP_DIALOGUE_HEADER__
