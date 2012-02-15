///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    dialogue.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for common dialogue code
*** ***************************************************************************/

#ifndef __DIALOGUE_HEADER__
#define __DIALOGUE_HEADER__

// Allacrost utilities
#include "defs.h"
#include "utils.h"

// Allacrost engines
#include "video.h"

// Local common headers
#include "gui.h"

namespace hoa_common {

//! \name Constants used among common dialogue classes
//@{
//! \brief Indicates that the next line to read should follow sequentially
const int32 COMMON_DIALOGUE_NEXT_LINE        = -1;
//! \brief Indicates that the dialogue should end after the present line is finished
const int32 COMMON_DIALOGUE_END              = -2;
//! \brief Indicates that a line has no display timer enabled
const int32 COMMON_DIALOGUE_NO_TIMER         = -1;
//! \brief Indicates that the dialogue can be viewed an infinite number of times
const int32 COMMON_DIALOGUE_INFINITE_VIEWS   = -1;
//! \brief Returned by numerous dialogue functions to indicate a bad/invalid function call was made
const int32 COMMON_DIALOGUE_BAD_VALUE        = -5555;

//! \brief The dialogue window should have no indicator (i.e. for automated text)
const uint8 COMMON_DIALOGUE_NO_INDICATOR   = 0;
//! \brief The dialogue window should have the indicator that more lines exist
const uint8 COMMON_DIALOGUE_NEXT_INDICATOR = 1;
//! \brief The dialogue window should have the indicator that the last line is reached
const uint8 COMMON_DIALOGUE_LAST_INDICATOR = 2;
//@}


/** ****************************************************************************
*** \brief A collection of related text lines and options used in a dialogue
***
*** This class contains the most common set of data and methods necessary to execute
*** a dialogue on the screen. It does not contain any draw code nor does the class
*** "process" itself in displaying the dialogue. Instead, other classes are typically
*** employed for the display and execution of dialogue. Game modes which make use of
*** dialogue will likely require to extend the capabilities of this class to serve their
*** own needs. This class is designed with the expectation that other classes will inherit
*** from it and expand the base set of capabilities.
***
*** The standard order of lines in a dialogue begin with the first line that is added and ends
*** with the last. However, dialogues do not always progress in this linear manner and thus this
*** class supports "branching" of dialogue. Each line contains a pointer to the next line that
*** should be read. If no valid line follows, the dialogue will end after the current line.
***
*** Each line of text added to the dialogue has the following properties associated:
*** -# The text of the line
*** -# The next line that should be viewed after this line
*** -# An optional timer instructing how long the line should be displayed
*** -# An optional series of selectable options that the player may choose from on this line
***
*** An example of the typical usage of this class to construct a dialogue would be as follows:
*** - CommonDialogue new_dialogue(my_new_id);
*** - new_dialogue.AddText("This is the first line.");
*** - new_dialogue.AddTextTimed("Line #2 is displayed for 2.5 seconds.", 2500);
*** - new_dialogue.AddText("Select from the options below.");
*** - new_dialogue.AddOption("Return to beginning", 0);
*** - new_dialogue.AddOption("End Dialogue");
*** - if (new_dialogue.Validate() == false) cout << "Dialogue object is invalid" << endl;
***
*** \note The interpretation of the display time member of the dialogue is subject to intrepretation
*** by the code that is processing the dialogue. For example, the code may read this to mean that the
*** dialogue will automatically move on after the timer has expired, or it may mean that the dialogue
*** must be displayed for that amount of time and that the player can not force the dialogue to move
*** faster.
*** ***************************************************************************/
class CommonDialogue {
public:
	//! \param id The id number to represent the dialogue, which should be unique to other dialogue ids within the same gamemode context
	CommonDialogue(uint32 id);

	~CommonDialogue();

	/** \brief Adds a new line of text to the dialogue
	*** \param text The text to show on the screen
	***
	*** Uses the default set of options for this line of text: proceed to next sequential line, no display timer.
	**/
	void AddLine(std::string text);

	/** \brief Adds a new line of text to the dialogue
	*** \param text The text to show on the screen
	*** \param next_line The line of dialogue which should follow this one
	***
	*** No display timer is set for this version of AddText
	**/
	void AddLine(std::string text, int32 next_line);

	/** \brief Adds a new line of text to the dialogue that uses a display time
	*** \param text The text to show on the screen
	*** \param display_time The number of milliseconds that the line should be displayed for
	***
	*** The dialogue will proceed to the next sequential line for this line of text.
	**/
	void AddLineTimed(std::string text, uint32 display_time);

	/** \brief Adds a new line of text to the dialogue that uses a display time
	*** \param text The text to show on the screen
	*** \param next_line The line of dialogue which should follow this one
	*** \param display_time The number of milliseconds that the line should be displayed for
	***
	*** The dialogue will proceed to the next sequential line for this line of text.
	**/
	void AddLineTimed(std::string text, int32 next_line, uint32 display_time);

	/** \brief Adds an option to the most recently added line of text
	*** \param text The text for this particular option
	*** \note The next line will be the next sequential line should this option be selected
	*** \note If no lines have been added to the dialogue yet, this option will not be added and a warning will be issued
	**/
	void AddOption(std::string text);

	/** \brief Adds an option to the most recently added line of text
	*** \param text The text for this particular option
	*** \param next_line The index value of the next line of dialogue to display should this option be selected
	*** \note If no lines have been added to the dialogue yet, this option will not be added and a warning will be issued
	**/
	void AddOption(std::string text, int32 next_line);

	//! \brief Indicates if this dialogue has already been seen by the player.
	bool HasAlreadySeen() const
		{ return (_times_seen != 0); }

	//! \brief Return true if this dialogue is available to be viewed (_times_seen is still less than _max_views)
	bool IsAvailable() const
		{ if (_max_views == COMMON_DIALOGUE_INFINITE_VIEWS) return true; else return (static_cast<int32>(_times_seen) < _max_views); }

	/** \brief Checks all the data stored by the dialogue class to ensure that it is acceptable and ready for use
	*** \return True if the validation was successful, false if any problems were discovered
	***
	*** This function should be called after adding all the lines, options, and settings to a dialogue but before the dialogue is
	*** actually used. It checks that all data is valid and warns of potentially dangerous data, such as referring to a line index
	*** that doesn't exist. Any issues that raise concern will cause the function to return false. Additionally if debugging is enabled,
	*** messages will be printed to the console providing details about any bad data discovered.
	**/
	bool Validate();

	//! \name Methods for retrieving properties of a specific line
	//@{
	//! \brief Returns the text of the line specified
	hoa_utils::ustring GetLineText(uint32 line) const
		{ if (line >= _line_count) return hoa_utils::ustring(); else return _text[line]; }

	//! \brief Returns the line index that follows the line specified
	int32 GetLineNextLine(uint32 line) const
		{ if (line >= _line_count) return COMMON_DIALOGUE_BAD_VALUE; else return _next_lines[line]; }

	//! \brief Returns the display time of the line specified
	int32 GetLineDisplayTime(uint32 line) const
		{ if (line >= _line_count) return COMMON_DIALOGUE_BAD_VALUE; else return _display_times[line]; }

	//! \brief Returns the options container of the line specified
	CommonDialogueOptions* GetLineOptions(uint32 line) const
		{ if (line >= _line_count) return NULL; else return _options[line]; }
	//@}

	//! \name Class Member Access Functions
	//@{
	uint32 GetDialogueID() const
		{ return _dialogue_id; }

	int32 GetTimesSeen() const
		{ return _times_seen; }

	int32 GetMaxViews() const
		{ return _max_views; }

	uint32 GetLineCount() const
		{ return _line_count; }

	void ResetTimesSeen()
		{ _times_seen = 0; }

	void SetTimesSeen(uint32 times)
		{ _times_seen = times; }

	void IncrementTimesSeen()
		{ _times_seen++; }

	void SetMaxViews(int32 views)
		{ _max_views = views; }
	//@}

protected:
	//! \brief A unique identification number that represents this dialogue
	uint32 _dialogue_id;

	//! \brief Counts the number of time a player has seen this dialogue.
	uint32 _times_seen;

	//! \brief Declares the max number of times that this dialogue can be viewed (negative value indicates no limit)
	int32 _max_views;

	//! \brief Stores the amount of lines in the dialogue.
	uint32 _line_count;

	//! \brief The text of the conversation, split up into multiple lines
	std::vector<hoa_utils::ustring> _text;

	/** \brief Holds indeces pointing to which line should follow each line of text
	*** \note When a line contains options, the value stored in this container for that line is never used
	**/
	std::vector<int32> _next_lines;

	//! \brief The display time for each line in the dialogue
	std::vector<int32> _display_times;

	//! \brief A set of dialogue options indexed according to the line of dialogue that they belong to
	std::vector<CommonDialogueOptions*> _options;
}; // class CommonDialogue


/** ***************************************************************************************
*** \brief A container class for option sets presented during dialogue
***
*** When the player reads a dialogue, they may be presented with a small number of options to
*** select from when coming to a particular line. The selected option determines the next line
*** that will follow. Objects of this class are stored and managed by the CommonDialogue class
*** to allow options to occur duing a dialogue.
*** **************************************************************************************/
class CommonDialogueOptions {
public:
	CommonDialogueOptions()
		{}

	virtual ~CommonDialogueOptions()
		{}

	/** \brief Adds a new option to the set of options
	*** \param text The text for the new option
	***
	*** \note The next line to be read for this option will be the sequential next line of the dialogue
	**/
	virtual void AddOption(std::string text);

	/** \brief Adds a new option to the set of options
	*** \param text The text for the new option
	*** \param next_line An integer index of the next line of dialogue should this option be selected.
	**/
	virtual void AddOption(std::string text, int32 next_line);

	//! \name Methods for retrieving properties of a specific line
	//@{
	//! \brief Returns the text of the option specified
	hoa_utils::ustring GetOptionText(uint32 option) const
		{ if (option >= GetNumberOptions()) return hoa_utils::ustring(); else return _text[option]; }

	//! \brief Returns the line index that follows the line when the given option is selected
	int32 GetOptionNextLine(uint32 option) const
		{ if (option >= GetNumberOptions()) return COMMON_DIALOGUE_BAD_VALUE; else return _next_lines[option]; }
	//@}

	//! \brief Returns the number of options stored by this class
	uint32 GetNumberOptions() const
		{ return _text.size(); }

protected:
	//! \brief Contains the text of each option
	std::vector<hoa_utils::ustring> _text;

	//! \brief A index containing the next line of dialogue that should follow each option
	std::vector<int32> _next_lines;
}; // class CommonDialogueOptions



/** ****************************************************************************
*** \brief A display window for all GUI controls and graphics necessary to execute a dialogue
***
*** This class handles all graphical management of a dialgoue. It serves primarily
*** as a container class for dialogue graphics and is not responsibile for tasks such
*** as updating the text box display or processing user input. This class operates in a 1024x768
*** coordinate system (screen resolution), regardless of the coordinate system that is set by the
*** game mode in where it is used. In other words, you don't need to concern yourself with changing
*** the coordinate system prior to drawing the contents of this class since the class does this
*** internally itself.
***
*** \todo This class does not use a MenuWindow as the background parchment image contains a
*** window image directly. This means that we can not animate the appearance of the window
*** and its contents via the standard GUI animation controls.
***
*** \todo This class needs to be made more flexible so that the GUI window size and position
*** can be specified by the user. Currently the size and position of the window are static. We'll also
*** likely need some options that adjust the drawing code for cases where we do not use a name or
*** portrait.
*** ***************************************************************************/
class CommonDialogueWindow {
public:
	CommonDialogueWindow();

	~CommonDialogueWindow()
		{}

	/** \brief Sets the draw position for the window
	*** \param pos_x The x coordinate for the center of the window
	*** \param pos_y The y coordinate for the bottom of the window
	***
	*** These positions take effect in a 1024x768 coordinate system and a a (VIDEO_X_LEFT, VIDEO_Y_BOTTOM) draw alignment|
	*** This function should always be called once when setting up the dialogue window before the first draw call is made.
	**/
	void SetPosition(float pos_x, float pos_y);

	//! \brief Clears all text from the window
	void Clear();

	//! \brief Draws the dialogue window and all other visuals
	void Draw();

	//! \name Class member access methods
	//@{
	hoa_gui::TextBox& GetDisplayTextBox()
		{ return _display_textbox; }

	hoa_gui::OptionBox& GetDisplayOptionBox()
		{ return _display_optionbox; }

	hoa_video::TextImage& GetNameText()
		{ return _name_text; }

	hoa_video::StillImage* GetPortraitImage() const
		{ return _portrait_image; }

	void SetPortraitImage(hoa_video::StillImage* image)
		{ _portrait_image = image; }

	void SetIndicator(uint8 type)
		{ _indicator_symbol = type; }
	//@}

private:
	//! \brief Stores the draw coordinates for the bottom center of the dialogue window
	float _pos_x, _pos_y;

	//! \brief A parchment paper image embedded within the dialogue window
	hoa_video::StillImage _parchment_image;

	//! \brief The nameplate image used along with the dialogue box image
	hoa_video::StillImage _nameplate_image;

	//! \brief The image indicating following dialogue lines
	hoa_video::StillImage _next_line_image;

	//! \brief The image indicating the last line of a dialogue
	hoa_video::StillImage _last_line_image;

	//! \brief The indicator symbol to use (arrow, end_of_dialogue_symbol, or none)
	uint8 _indicator_symbol;

	//! \brief The counter for the blinking indicators
	uint16 _blink_time;

	//! \brief The status of the blinking indicators
	bool _blink_state;

	//! \brief The textbox used for rendering the dialogue text
	hoa_gui::TextBox _display_textbox;

	//! \brief The option box used for rendering dialogue options where applicable
	hoa_gui::OptionBox _display_optionbox;

	//! \brief Holds the name of the speaker
	hoa_video::TextImage _name_text;

	//! \brief A pointer to a portrait image to display alongside the text. A NULL value will display no portrait
	hoa_video::StillImage* _portrait_image;
}; // class CommonDialogueWindow


/** ****************************************************************************
*** \brief Abstract class that assists in the execution of dialogues
***
*** This abstract class is a skeleton that other modes may choose to utilize if
*** they like in constructing their own supervisor-like classes for dialogue. The
*** base class does little here except for maintaining the necessary data structures
*** and processing dialogues. It does not implement any sort of update or draw
*** methods to move through the dialogue as there is no appropriate means for doing
*** so in an abstract, agnostic fashion that other classes are likely to adopt.
***
*** \note It may be the case that it is more appropriate to create a dialogue supervisor
*** class that stands independent of this base class (ie, the class does not inherit this).
*** This is perfectly acceptable, although the creator of such classes would be wise to try
*** and follow the structure and API laid out in this base class as closely as is reasonable.
*** ***************************************************************************/
class CommonDialogueSupervisor {
public:
	CommonDialogueSupervisor();

	~CommonDialogueSupervisor();

	//! \brief Processes user input and updates the state of the dialogue
	virtual void Update() = 0;

	//! \brief Draws the dialogue window, text, portraits, and other visuals to the screen
	virtual void Draw() = 0;

	/** \brief Adds a new dialogue to be managed by the supervisor
	*** \param dialogue Pointer to a CommonDialogue object that was created with the new operator
	***
	*** The dialogue to add must have a unique dialogue ID that is not already stored by this class
	*** instance. If a dialogue is found with the same ID, the dialogue will not be added and the
	*** dialogue object will be deleted from memory. All dialogues that are successfully added will
	*** be later deleted when this class' destructor is invoked, so make sure you only pass in objects
	*** that were created with the "new" operator.
	**/
	virtual void AddDialogue(CommonDialogue* dialogue);

	/** \brief Prepares the dialogue manager to begin processing a new dialogue
	*** \param dialogue_id The id number of the dialogue to begin
	**/
	virtual void BeginDialogue(uint32 dialogue_id);

	//! \brief Immediately ends any dialogue that is taking place
	virtual void EndDialogue();

	/** \brief Returns a pointer to the CommonDialogue with the requested ID value
	*** \param dialogue_id The identification number of the dialogue to retrieve
	*** \return A pointer to the dialogue requested, or NULL if no such dialogue was found
	**/
	CommonDialogue* GetDialogue(uint32 dialogue_id);

	//! \name Class member access functions
	//@{
	CommonDialogue* GetCurrentDialogue() const
		{ return _current_dialogue; }

	CommonDialogueOptions* GetCurrentOptions() const
		{ return _current_options; }

	hoa_system::SystemTimer& GetLineTimer()
		{ return _line_timer; }

	uint32 GetLineCounter() const
		{ return _line_counter; }
	//@}

protected:
	//! \brief Contains all dialogues managed by this class instance in a std::map structure. The dialogue ID is the map key
	std::map<uint32, CommonDialogue*> _dialogues;

	//! \brief A pointer to the current piece of dialogue that is active
	CommonDialogue* _current_dialogue;

	//! \brief A pointer to the current set of options for the active dialogue line
	CommonDialogueOptions* _current_options;

	//! \brief A timer that employed for dialogues which have a display time limit
	hoa_system::SystemTimer _line_timer;

	//! \brief Keeps track of which line is active for the current dialogue
	uint32 _line_counter;
}; // class CommonDialogueSupervisor

} // namespace hoa_common

#endif // __DIALOGUE_HEADER__
