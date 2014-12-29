///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    dialogue.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for common dialogue code
*** ***************************************************************************/

#ifndef __DIALOGUE_HEADER__
#define __DIALOGUE_HEADER__

#include "utils/ustring.h"
#include "utils/utils_strings.h"

#include "common/gui/textbox.h"
#include "common/gui/option.h"

namespace vt_common
{

class DialogueOptions;

//! \name Constants used among common dialogue classes
//@{
//! \brief Indicates that the next line to read should follow sequentially
const int32 DIALOGUE_NEXT_LINE      = -1;
//! \brief Indicates that the dialogue should end after the present line is finished
const int32 DIALOGUE_END            = -2;
//! \brief Indicates that a line has no display timer enabled
const int32 DIALOGUE_NO_TIMER       = -1;
//! \brief the default invalid value.
const int32 DIALOGUE_INVALID        = -1;

//! \brief The dialogue window should have no indicator (i.e. for automated text)
const uint8 DIALOGUE_NO_INDICATOR   = 0;
//! \brief The dialogue window should have the indicator that more lines exist
const uint8 DIALOGUE_NEXT_INDICATOR = 1;
//! \brief The dialogue window should have the indicator that the last line is reached
const uint8 DIALOGUE_LAST_INDICATOR = 2;
//@}

//! \brief Defines the different states the dialogue can be in.
enum DIALOGUE_STATE {
    DIALOGUE_STATE_INACTIVE =  0, //!< No dialogue is active
    DIALOGUE_STATE_LINE     =  1, //!< Active when the dialogue window is in the process of displaying a line of text
    DIALOGUE_STATE_OPTION   =  2, //!< Active when player-selectable options are present in the dialogue window
};

/** ****************************************************************************
*** \brief Container for the data that represents a dialogue speaker
***
*** This is used only by the DialogueSupervisor to store its speaker data. No other
*** classes use nor need to know about this class. The ID of the speaker is omitted because
*** the supervisor class stores those values as keys to the std::map container that holds
*** instances of this speaker class.
*** *************************************************/
class Speaker
{
public:
    //! \brief The name of this speaker as it will appear to the player
    vt_utils::ustring name;

    /** \brief Holds a reference to the portrait image to use for this speaker
    ***
    *** \note Not all speakers will have portraits available. For those that don't, this
    *** member will simply remain a blank image that is drawn to the screen.
    **/
    vt_video::StillImage portrait;
}; // class Speaker

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
*** - if (new_dialogue.Validate() == false) cout << "Dialogue object is invalid" << std::endl;
***
*** \note The interpretation of the display time member of the dialogue is subject to intrepretation
*** by the code that is processing the dialogue. For example, the code may read this to mean that the
*** dialogue will automatically move on after the timer has expired, or it may mean that the dialogue
*** must be displayed for that amount of time and that the player can not force the dialogue to move
*** faster.
*** ***************************************************************************/
class Dialogue
{
public:
    //! \param id The id number to represent the dialogue,
    //! which should be unique to other dialogue ids within the same game mode context
    Dialogue(const std::string& dialogue_id);

    ~Dialogue();

    /** \brief Adds a new line of text to the dialogue
    *** \param text The text to show on the screen
    ***
    *** Uses the default set of options for this line of text: proceed to next sequential line, no display timer.
    **/
    void AddLine(const std::string& text);

    /** \brief Adds a new line of text to the dialogue
    *** \param text The text to show on the screen
    *** \param next_line The line of dialogue which should follow this one
    ***
    *** No display timer is set for this version of AddText
    **/
    void AddLine(const std::string& text, int32 next_line);

    /** \brief Adds a new line of text to the dialogue
    *** \param text The text to show on the screen
    *** \param speaker The ID of the speaker for this line
    ***
    *** The following line properties are set when using this call:
    *** - proceed to next sequential line, no display time
    **/
    void AddLine(const std::string& text, const std::string& speaker_id);

    /** \brief Adds a new line of text to the dialogue
    *** \param text The text to show on the screen
    *** \param speaker The ID of the speaker for this line
    *** \param next_line The line of dialogue which should follow this one
    ***
    *** The following line properties are set when using this call:
    *** - no display time
    **/
    void AddLine(const std::string& text, const std::string& speaker_id, int32 next_line);

    /** \brief Adds a new line of text to the dialogue that uses a display time
    *** \param text The text to show on the screen
    *** \param display_time The number of milliseconds that the line should be displayed for
    ***
    *** The dialogue will proceed to the next sequential line for this line of text.
    **/
    void AddLineTimed(const std::string& text, uint32 display_time);

    /** \brief Adds a new line of text to the dialogue that uses a display time
    *** \param text The text to show on the screen
    *** \param next_line The line of dialogue which should follow this one
    *** \param display_time The number of milliseconds that the line should be displayed for
    ***
    *** The dialogue will proceed to the next sequential line for this line of text.
    **/
    void AddLineTimed(const std::string& text, int32 next_line, uint32 display_time);

    /** \brief Adds a new line of text to the dialogue that uses a display time
    *** \param text The text to show on the screen
    *** \param speaker The ID of the speaker for this line
    *** \param display_time The number of milliseconds that the line should be displayed for
    ***
    *** The following line properties are set when using this call:
    *** - proceed to next sequential line
    **/
    void AddLineTimed(const std::string& text, const std::string& speaker_id, uint32 display_time);

    /** \brief Adds a new line of text to the dialogue that uses a display time
    *** \param text The text to show on the screen
    *** \param speaker The ID of the speaker for this line
    *** \param next_line The line of dialogue which should follow this one
    *** \param display_time The number of milliseconds that the line should be displayed for
    **/
    void AddLineTimed(const std::string& text, const std::string& speaker_id, int32 next_line, uint32 display_time);

    /** \brief Adds an option to the most recently added line of text
    *** \param text The text for this particular option
    *** \note The next line will be the next sequential line should this option be selected
    *** \note If no lines have been added to the dialogue yet, this option will not be added and a warning will be issued
    **/
    void AddOption(const std::string& text);

    /** \brief Adds an option to the most recently added line of text
    *** \param text The text for this particular option
    *** \param next_line The index value of the next line of dialogue to display should this option be selected
    *** \note If no lines have been added to the dialogue yet, this option will not be added and a warning will be issued
    **/
    void AddOption(const std::string& text, int32 next_line);

    /** \brief Checks all the data stored by the dialogue class to ensure that it is acceptable and ready for use
    *** \return True if the validation was successful, false if any problems were discovered
    ***
    *** This function should be called after adding all the lines, options, and settings to a dialogue but before the dialogue is
    *** actually used. It checks that all data is valid and warns of potentially dangerous data, such as referring to a line index
    *** that doesn't exist. Any issues that raise concern will cause the function to return false. Additionally if debugging is enabled,
    *** messages will be printed to the console providing details about any bad data discovered.
    *** \note This function should not be called until after all battle speakers have been added to the battle dialogue supervisor.
    *** The function checks that each speaker reference is valid and stored in the supervisor class.
    **/
    bool Validate();

    //! \name Methods for retrieving properties of a specific line
    //@{
    //! \brief Returns the text of the line specified
    vt_utils::ustring GetLineText(uint32 line) const {
        if(line >= _line_count) return vt_utils::ustring();
        else return _text[line];
    }

    //! \brief Returns the line index that follows the line specified
    int32 GetLineNextLine(uint32 line) const {
        if(line >= _line_count) return DIALOGUE_INVALID;
        else return _next_lines[line];
    }

    //! \brief Returns the display time of the line specified
    int32 GetLineDisplayTime(uint32 line) const {
        if(line >= _line_count) return DIALOGUE_INVALID;
        else return _display_times[line];
    }

    //! \brief Returns the options container of the line specified
    DialogueOptions* GetLineOptions(uint32 line) const {
        if(line >= _line_count) return NULL;
        else return _options[line];
    }
    //@}

    //! \brief Returns the speaker ID for the line specified (or zero if the line index was invalid)
    const std::string& GetLineSpeaker(uint32 line) const {
        if(line >= _line_count) return vt_utils::_empty_string;
        else return _speakers[line];
    }

    //! \name Class Member Access Functions
    //@{
    const std::string& GetDialogueID() const {
        return _dialogue_id;
    }

    uint32 GetLineCount() const {
        return _line_count;
    }
    //@}

protected:
    //! \brief A unique identification string that represents this dialogue
    std::string _dialogue_id;

    //! \brief Stores the amount of lines in the dialogue.
    uint32 _line_count;

    //! \brief The text of the conversation, split up into multiple lines
    std::vector<vt_utils::ustring> _text;

    /** \brief Holds indeces pointing to which line should follow each line of text
    *** \note When a line contains options, the value stored in this container for that line is never used
    **/
    std::vector<int32> _next_lines;

    //! \brief The display time for each line in the dialogue
    std::vector<int32> _display_times;

    //! \brief A set of dialogue options indexed according to the line of dialogue that they belong to
    std::vector<DialogueOptions *> _options;

    //! \brief Contains the speaker ID number corresponding to each line of text
    std::vector<std::string> _speakers;
}; // class Dialogue


/** ***************************************************************************************
*** \brief A container class for option sets presented during dialogue
***
*** When the player reads a dialogue, they may be presented with a small number of options to
*** select from when coming to a particular line. The selected option determines the next line
*** that will follow. Objects of this class are stored and managed by the CommonDialogue class
*** to allow options to occur duing a dialogue.
*** **************************************************************************************/
class DialogueOptions
{
public:
    DialogueOptions()
    {}

    virtual ~DialogueOptions()
    {}

    /** \brief Adds a new option to the set of options
    *** \param text The text for the new option
    ***
    *** \note The next line to be read for this option will be the sequential next line of the dialogue
    **/
    virtual void AddOption(const std::string &text);

    /** \brief Adds a new option to the set of options
    *** \param text The text for the new option
    *** \param next_line An integer index of the next line of dialogue should this option be selected.
    **/
    virtual void AddOption(const std::string &text, int32 next_line);

    //! \name Methods for retrieving properties of a specific line
    //@{
    //! \brief Returns the text of the option specified
    vt_utils::ustring GetOptionText(uint32 option) const {
        if(option >= GetNumberOptions()) return vt_utils::ustring();
        else return _text[option];
    }

    //! \brief Returns the line index that follows the line when the given option is selected
    int32 GetOptionNextLine(uint32 option) const {
        if(option >= GetNumberOptions()) return DIALOGUE_INVALID;
        else return _next_lines[option];
    }
    //@}

    //! \brief Returns the number of options stored by this class
    uint32 GetNumberOptions() const {
        return _text.size();
    }

protected:
    //! \brief Contains the text of each option
    std::vector<vt_utils::ustring> _text;

    //! \brief A index containing the next line of dialogue that should follow each option
    std::vector<int32> _next_lines;
}; // class DialogueOptions


/** ****************************************************************************
*** \brief A display window for all GUI controls and graphics necessary to execute a dialogue
***
*** This class handles all graphical management of a dialogue. It serves primarily
*** as a container class for dialogue graphics and is not responsible for tasks such
*** as updating the text box display or processing user input. This class operates in a 1024x768
*** coordinate system (screen resolution), regardless of the coordinate system that is set by the
*** game mode in where it is used. In other words, you don't need to concern yourself with changing
*** the coordinate system prior to drawing the contents of this class since the class does this
*** internally itself.
*** ***************************************************************************/
class DialogueWindow
{
public:
    DialogueWindow();

    ~DialogueWindow()
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
    vt_gui::TextBox &GetDisplayTextBox() {
        return _display_textbox;
    }

    vt_gui::OptionBox &GetDisplayOptionBox() {
        return _display_optionbox;
    }

    vt_video::TextImage &GetNameText() {
        return _name_text;
    }

    vt_video::StillImage *GetPortraitImage() const {
        return _portrait_image;
    }

    void SetPortraitImage(vt_video::StillImage *image) {
        _portrait_image = image;
    }

    void SetIndicator(uint8 type) {
        _indicator_symbol = type;
    }
    //@}

private:
    //! \brief Stores the draw coordinates for the bottom center of the dialogue window
    float _pos_x, _pos_y;

    //! \brief A parchment paper image embedded within the dialogue window
    vt_video::StillImage _parchment_image;

    //! \brief The nameplate image used along with the dialogue box image
    vt_video::StillImage _nameplate_image;

    //! \brief The image indicating following dialogue lines
    vt_video::StillImage _next_line_image;

    //! \brief The image indicating the last line of a dialogue
    vt_video::StillImage _last_line_image;

    //! \brief The indicator symbol to use (arrow, end_of_dialogue_symbol, or none)
    uint8 _indicator_symbol;

    //! \brief The counter for the blinking indicators
    uint16 _blink_time;

    //! \brief The status of the blinking indicators
    bool _blink_state;

    //! \brief The textbox used for rendering the dialogue text
    vt_gui::TextBox _display_textbox;

    //! \brief The option box used for rendering dialogue options where applicable
    vt_gui::OptionBox _display_optionbox;

    //! \brief Holds the name of the speaker
    vt_video::TextImage _name_text;

    //! \brief A pointer to a portrait image to display alongside the text. A NULL value will display no portrait
    vt_video::StillImage *_portrait_image;
}; // class DialogueWindow


/** ****************************************************************************
*** \brief Handles the storage and execution of dialogues
***
*** This supervisor class retains two data containers. The first container holds
*** all of the speakers while the second holds all of the dialogues.
*** ***************************************************************************/
class DialogueSupervisor
{
public:
    DialogueSupervisor();

    virtual ~DialogueSupervisor();

    //! \brief Processes user input and updates the state of the dialogue
    virtual void Update();

    //! \brief Draws the dialogue window, text, portraits, and other visuals to the screen
    virtual void Draw();

    /** \brief Adds a new speaker using custom name and portrait data
    *** \param id The unique ID number for this speaker
    *** \param name The name of the speaker
    *** \param portrait_filename The filename for the portrait to add
    ***
    *** Pass in an empty string to the arguments if you do not want the speaker to have a name or portrait image.
    **/
    virtual void AddSpeaker(const std::string& speaker_id, const std::string& name, const std::string& portrait);

    /** \brief Changes the name for a speaker that was previously added
    *** \param id The unique ID number for the speaker to change
    *** \param name The text to change the speaker's name to
    **/
    virtual void ChangeSpeakerName(const std::string& speaker_id, const std::string& name);

    /** \brief Changes the portrait image for a speaker that was previously added
    *** \param id The unique ID number for the speaker to change
    *** \param portrait The filename of the image to use as the speaker's portrait
    **/
    virtual void ChangeSpeakerPortrait(const std::string& speaker_id, const std::string& portrait);

    /** \brief Adds a new dialogue to be managed by the supervisor
    *** \param dialogue Pointer to a Dialogue object that was created with the new operator
    ***
    *** The dialogue to add must have a unique dialogue ID that is not already stored by this class
    *** instance. If a dialogue is found with the same ID, the dialogue will not be added and the
    *** dialogue object will be deleted from memory. All dialogues that are successfully added will
    *** be later deleted when this class' destructor is invoked, so make sure you only pass in objects
    *** that were created with the "new" operator.
    **/
    virtual void AddDialogue(Dialogue *dialogue);

    /** \brief Prepares the dialogue manager to begin processing a new dialogue
    *** \param dialogue_id The id number of the dialogue to begin
    **/
    virtual void StartDialogue(const std::string& dialogue_id);

    //! \brief Immediately ends any dialogue that is taking place
    virtual void EndDialogue();

    //! \brief Forces the current dialogue to proceed to the next line immediately
    virtual void ForceNextLine();

    /** \brief Returns a pointer to the Dialogue with the requested ID value
    *** \param dialogue_id The identification number of the dialogue to retrieve
    *** \return A pointer to the dialogue requested, or NULL if no such dialogue was found
    **/
    Dialogue* GetDialogue(const std::string& dialogue_id);

    /** \brief Returns a pointer to the BattleSpeaker with the requested ID value
    *** \param speaker The unique ID number of the speaker to retrieve
    *** \return A pointer to the stored speaker, or NULL if no speaker was found with the specified ID
    **/
    Speaker* GetSpeaker(const std::string& speaker_id);

    Dialogue* GetCurrentDialogue() const {
        return _current_dialogue;
    }

    //! \brief Returns true if any dialogue is currently active at this time
    bool IsDialogueActive() const {
        return (_current_dialogue != NULL);
    }

    DialogueOptions* GetCurrentOptions() const {
        return _current_options;
    }

    vt_system::SystemTimer& GetLineTimer() {
        return _line_timer;
    }

    uint32 GetLineCounter() const {
        return _line_counter;
    }

    //! \brief Sets the bottom center position of the dialogue window.
    void SetDialoguePosition(float x, float y);

protected:
    //! \brief Retains the current state of the dialogue execution
    DIALOGUE_STATE _state;

    //! \brief Contains data for all of the speakers for every dialogue and each line. The speaker ID is the map key.
    std::map<std::string, Speaker> _speakers;

    //! \brief Contains all dialogues managed by this class instance in a std::map structure. The dialogue ID is the map key
    std::map<std::string, Dialogue *> _dialogues;

    //! \brief A pointer to the current piece of dialogue that is active
    Dialogue *_current_dialogue;

    //! \brief A pointer to the current set of options for the active dialogue line
    DialogueOptions *_current_options;

    //! \brief A timer that employed for dialogues which have a display time limit
    vt_system::SystemTimer _line_timer;

    //! \brief Keeps track of which line is active for the current dialogue
    uint32 _line_counter;

    //! \brief Holds the text and graphics that should be displayed for the dialogue
    DialogueWindow _dialogue_window;

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

} // namespace vt_common

#endif // __DIALOGUE_HEADER__
