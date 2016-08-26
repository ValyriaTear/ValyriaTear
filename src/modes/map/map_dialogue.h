///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_dialogue.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for map mode dialogue
*** ***************************************************************************/

#ifndef __MAP_DIALOGUE_HEADER__
#define __MAP_DIALOGUE_HEADER__

#include "common/dialogue.h"

#include "map_utils.h"

namespace vt_map
{

namespace private_map
{

class MapSprite;

/** ****************************************************************************
*** \brief Represents a dialogue that occurs between one or more sprites on a map
*** ***************************************************************************/
class SpriteDialogue : public vt_common::Dialogue
{
public:
    //! \brief Constructor with auto-generated dialogue ID
    SpriteDialogue();

    //! \brief Dialogue with auto-generated id and event name
    //! \param dialogue_event_name event name used and stored in the 'dialogues' save data table.
    //! The event name is used to know whether the player has already seen a dialogue
    //! and display the dialogue bubble accordingly.
    //! If empty, the event is not stored.
    explicit SpriteDialogue(const std::string& dialogue_event_name);

    virtual ~SpriteDialogue() override
    {
    }

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static SpriteDialogue* Create();
    static SpriteDialogue* Create(const std::string& dialogue_event_name);

    //! \brief Indicates if this dialogue has already been seen by the player.
    bool HasAlreadySeen() const {
        return _dialogue_seen;
    }

    //! \brief Set the dialogue as seen by the player, and stores its states
    //! in the save data if its event name isn't empty.
    void SetAsSeen(bool seen = true);

    /** \brief Adds a new line of text to the dialogue
    *** \param text The text to show on the screen
    *** \param speaker The VirtualSprite speaking this line
    ***
    *** The following line properties are set when using this call:
    *** - proceed to next sequential line, no display time, no event
    **/
    void AddLine(const std::string &text, MapSprite *speaker);

    void AddLineEmote(const std::string &text, MapSprite *speaker,
                      const std::string &emote_id);

    /** \brief Adds a new line of text to the dialogue
    *** \param text The text to show on the screen
    *** \param speaker The VirtualSprite speaking this line
    *** \param next_line The line of dialogue which should follow this one
    ***
    *** The following line properties are set when using this call:
    *** - no display time, no event
    **/
    void AddLine(const std::string &text, MapSprite *speaker, int32_t next_line);

    /** \brief Adds a new line of text to the dialogue that uses a display time
    *** \param text The text to show on the screen
    *** \param speaker VirtualSprite speaking this line
    *** \param display_time The number of milliseconds that the line should be displayed for
    ***
    *** The following line properties are set when using this call:
    *** - proceed to next sequential line, no event
    **/
    void AddLineTimed(const std::string &text, MapSprite *speaker, uint32_t display_time);

    /** \brief Adds a new line of text to the dialogue that uses a display time
    *** \param text The text to show on the screen
    *** \param speaker The VirtualSprite speaking this line
    *** \param next_line The line of dialogue which should follow this one
    *** \param display_time The number of milliseconds that the line should be displayed for
    ***
    *** The following line properties are set when using this call:
    *** - no event
    **/
    void AddLineTimed(const std::string &text, MapSprite *speaker, int32_t next_line, uint32_t display_time);

    /** \brief Adds a new line of text to the dialogue that uses a map event
    *** \param text The text to show on the screen
    *** \param speaker The VirtualSprite speaking this line
    *** \param end_event_id The ID of the event to execute after this line finishes
    *** \param begin_event_id The ID of the event to execute when the line starts
    ***
    *** The following line properties are set when using this call:
    *** - proceed to next sequential line, no display time
    **/
    void AddLineEvent(const std::string &text, MapSprite *speaker,
                      const std::string &begin_event_id, const std::string &end_event_id);

    void AddLineEventEmote(const std::string &text, MapSprite *speaker,
                           const std::string &begin_event_id, const std::string &end_event_id,
                           const std::string &emote_id);

    /** \brief Adds a new line of text to the dialogue that uses a map event
    *** \param text The text to show on the screen
    *** \param speaker The VirtualSprite speaking this line
    *** \param next_line The line of dialogue which should follow this one
    *** \param display_time The number of milliseconds that the line should be displayed for
    ***
    *** The following line properties are set when using this call:
    *** - no event
    **/
    void AddLineEvent(const std::string &text, MapSprite *speaker, int32_t next_line,
                      const std::string &begin_event_id, const std::string &end_event_id);

    /** \brief Adds a new line of text to the dialogue that uses a map event
    *** \param text The text to show on the screen
    *** \param speaker The VirtualSprite speaking this line
    *** \param display_time The number of milliseconds that the line should be displayed for
    *** \param end_event_id The ID of the event to execute after this line finishes
    *** \param begin_event_id The ID of the event to execute when the line starts
    ***
    *** The following line properties are set when using this call:
    *** - proceed to next sequential line
    **/
    void AddLineTimedEvent(const std::string &text, MapSprite *speaker, uint32_t display_time,
                           const std::string &begin_event_id, const std::string &end_event_id);

    /** \brief Adds a new line of text to the dialogue that uses a map event
    *** \param text The text to show on the screen
    *** \param speaker The VirtualSprite speaking this line
    *** \param next_line The line of dialogue which should follow this one
    *** \param display_time The number of milliseconds that the line should be displayed for
    *** \param begin_event_id The ID of the event to execute when the line starts
    *** \param end_event_id The ID of the event to execute after this line finishes
    *** \param emote_id The ID of the emote to trigger on the speaker map sprite.
    **/
    void AddLineTimedEvent(const std::string &text, MapSprite *speaker, int32_t next_line, uint32_t display_time,
                           const std::string &begin_event_id, const std::string &end_event_id, const std::string& emote_id);

    /** \brief Adds an option to the most recently added line of text
    *** \param text The text for this particular option
    *** \note If no lines have been added to the dialogue yet, this option will not be added and a warning will be issued
    ***
    *** The following option properties are set when using this call:
    *** - proceed to next sequential line, no event
    **/
    void AddOption(const std::string &text);

    /** \brief Adds an option to the most recently added line of text
    *** \param text The text for this particular option
    *** \param next_line The index value of the next line of dialogue to display should this option be selected
    *** \note If no lines have been added to the dialogue yet, this option will not be added and a warning will be issued
    ***
    *** The following option properties are set when using this call:
    *** - no event
    **/
    void AddOption(const std::string &text, int32_t next_line);

    /** \brief Adds an option to the most recently added line of text
    *** \param text The text for this particular option
    *** \param event_id The ID of the event to execute should this option be selected
    *** \note If no lines have been added to the dialogue yet, this option will not be added and a warning will be issued
    ***
    *** The following option properties are set when using this call:
    *** - proceed to next sequential line
    **/
    void AddOptionEvent(const std::string &text, const std::string &event_id);

    /** \brief Adds an option to the most recently added line of text
    *** \param text The text for this particular option
    *** \param next_line The index value of the next line of dialogue to display should this option be selected
    *** \param event_id The ID of the event to execute should this option be selected
    *** \note If no lines have been added to the dialogue yet, this option will not be added and a warning will be issued
    **/
    void AddOptionEvent(const std::string &text, int32_t next_line, const std::string &event_id);

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
    //! \brief Returns the object of the speaker for the line specified (or nullptr if the line index was invalid)
    MapSprite* GetLineSpeaker(uint32_t line) const {
        if(line >= _line_count) return nullptr;
        else return _speakers[line];
    }

    //! \brief Returns the ID of the event to execute for the line specified (or zero if the line index was invalid)
    std::string GetLineEndEvent(uint32_t line) const {
        if(line >= _line_count) return std::string();
        else return _end_events[line];
    }

    //! \brief Returns the ID of the event to execute for the line specified (or zero if the line index was invalid)
    std::string GetLineBeginEvent(uint32_t line) const {
        if(line >= _line_count) return std::string();
        else return _begin_events[line];
    }

    std::string GetLineEmote(uint32_t line) const {
        if(line >= _line_count) return std::string();
        else return _emote_events[line];
    }
    //@}

    //! \name Class Member Access Functions
    //@{
    const std::string &GetEventName() const {
        return _event_name;
    }

    bool IsInputBlocked() const {
        return _input_blocked;
    }

    bool IsRestoreState() const {
        return _restore_state;
    }

    void SetInputBlocked(bool blocked) {
        _input_blocked = blocked;
    }

    void SetRestoreState(bool restore) {
        _restore_state = restore;
    }

    void SetEventAtDialogueEnd(const std::string& event_id) {
        _event_after_dialogue_end = event_id;
    }

    const std::string& GetEventAtDialogueEnd() const {
        return _event_after_dialogue_end;
    }
    //@}

private:
    //! \brief If true, dialogue will ignore user input and instead execute independently
    bool _input_blocked;

    //! \brief If true, the state of map sprites participating in this dialogue will be reset after the dialogue completes
    bool _restore_state;

    //! \brief The event name for this dialogue that is stored in the saved game file, if not empty.
    //! This permit the engine to know whether the player already saw a dialogue event when leaving the map
    //! and coming back.
    std::string _event_name;

    //! \brief Tells whether the dialogue has been seen by the player.
    bool _dialogue_seen;

    //! \brief Contains MapSprite speakers of each line
    std::vector<MapSprite*> _speakers;

    //! \brief An optional MapEvent that may occur when a line begins
    std::vector<std::string> _begin_events;

    //! \brief An optional MapEvent that may occur after each line is completed
    std::vector<std::string> _end_events;

    //! \brief the emote to play on the speaker sprite before starting the line (and if possible).
    std::vector<std::string> _emote_events;

    //! \brief The optional event id to trigger after dialogue's end.
    //! This is handly to trigger other scene events after a dialogue.
    std::string _event_after_dialogue_end;
}; // class SpriteDialogue : public vt_common::Dialogue


/** ***************************************************************************************
*** \brief A container class for option sets presented during a map dialogue
***
*** When the player reads a dialogue, they may be presented with a small number of options to
*** select from when coming to a particular line. The selected option determines the next line
*** that will follow. Optionally, each particular option may trigger a different map event when
*** it is selected.
*** **************************************************************************************/
class MapDialogueOptions : public vt_common::DialogueOptions
{
public:
    MapDialogueOptions()
    {}

    virtual ~MapDialogueOptions() override
    {}

    /** \brief Adds a new option to the set of options
    *** \param text The text for the new option
    ***
    *** The following option properties are set when using this call:
    *** - proceed to next sequential line, no event
    **/
    void AddOption(const std::string &text);

    /** \brief Adds a new option to the set of options
    *** \param text The text for the new option
    *** \param next_line An integer index of the next line of dialogue should this option be selected.
    ***
    *** The following option properties are set when using this call:
    *** - no event
    **/
    void AddOption(const std::string &text, int32_t next_line);

    /** \brief Adds a new option to the set of options with the addition of a map event to be executed
    *** \param text The text for the new option
    *** \param event_id The ID of the event to execute should this option be selected
    ***
    *** The following option properties are set when using this call:
    *** - proceed to next sequential line
    **/
    void AddOptionEvent(const std::string &text, const std::string &event_id);

    /** \brief Adds a new option to the set of options with the addition of a map event to be executed
    *** \param text The text for the new option
    *** \param next_line An integer index of the next line of dialogue should this option be selected.
    *** \param event_id The ID of the event to execute should this option be selected
    **/
    void AddOptionEvent(const std::string &text, int32_t next_line, const std::string &event_id);

    //! \name Methods for retrieving properties of a specific line
    //@{
    std::string GetOptionEvent(uint32_t option) const {
        if(option >= GetNumberOptions()) return std::string();
        else return _events[option];
    }
    //@}

    //! \brief Returns the number of options stored by this class
    uint32_t GetNumberOptions() const {
        return _text.size();
    }

private:
    //! \brief An optional MapEvent that may occur as a result of selecting each option
    std::vector<std::string> _events;
}; // class MapDialogueOptions : public vt_common::DialogueOptions


/** ****************************************************************************
*** \brief Manages dialogue execution on maps
***
*** The MapMode class creates an instance of this class to handle all dialogue
*** processing that occurs on the map. This includes containing the dialogue objects,
*** handling user input, processing of dialogue events, and display timing of the
*** dialogue.
*** ***************************************************************************/
class MapDialogueSupervisor
{
public:
    MapDialogueSupervisor();

    ~MapDialogueSupervisor();

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
    void AddDialogue(SpriteDialogue *dialogue);

    /** \brief Prepares the dialogue manager to begin processing a new dialogue
    *** \param dialogue_id The id number of the dialogue to begin
    **/
    void StartDialogue(const std::string& dialogue_id);

    //! \brief Immediately ends any dialogue that is taking place
    void EndDialogue();

    /** \brief Returns a pointer to the CommonDialogue with the requested ID value
    *** \param dialogue_id The identification number of the dialogue to retrieve
    *** \return A pointer to the dialogue requested, or nullptr if no such dialogue was found
    **/
    SpriteDialogue *GetDialogue(const std::string& dialogue_id);

    //! \name Class member access functions
    //@{
    DIALOGUE_STATE GetDialogueState() const {
        return _state;
    }

    SpriteDialogue *GetCurrentDialogue() const {
        return _current_dialogue;
    }

    MapDialogueOptions *GetCurrentOptions() const {
        return _current_options;
    }

    vt_system::SystemTimer &GetLineTimer() {
        return _line_timer;
    }

    uint32_t GetLineCounter() const {
        return _line_counter;
    }

    //! \brief Returns a new unique dialogue id string.
    std::string GenerateDialogueID() {
        return vt_utils::NumberToString(_next_dialogue_id++);
    }
    //@}

private:
    //! \brief Retains the current state of the dialogue execution
    DIALOGUE_STATE _state;

    //! \brief A numeric value used to generate unique dialogue ids.
    uint32_t _next_dialogue_id;

    //! \brief Contains all dialogues used in the map in a std::map structure. The dialogue IDs serve as the map keys
    std::map<std::string, SpriteDialogue *> _dialogues;

    //! \brief A pointer to the current piece of dialogue that is active
    SpriteDialogue *_current_dialogue;

    //! \brief A pointer to the current set of options for the active dialogue line
    MapDialogueOptions *_current_options;

    //! \brief A timer employed for dialogues which have a display time limit
    vt_system::SystemTimer _line_timer;

    //! \brief Keeps track of which line is active for the current dialogue
    uint32_t _line_counter;

    //! \brief Holds the text and graphics that should be displayed for the dialogue
    vt_common::DialogueWindow _dialogue_window;

    //! \brief Keeps in memory whether the emote event has been triggered.
    bool _emote_triggered;

    // ---------- Private methods

    //! \brief Basically wait for the emote to be finished before starting the line.
    void _UpdateEmote();

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
}; // class MapDialogueSupervisor

} // namespace private_map

} // namespace vt_map

#endif // __MAP_DIALOGUE_HEADER__
