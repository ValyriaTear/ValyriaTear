///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_DIALOGUE_SUPERVISOR_HEADER__
#define __MAP_DIALOGUE_SUPERVISOR_HEADER__

#include "common/dialogue.h"

namespace vt_map
{

namespace private_map
{

class SpriteDialogue;
class MapDialogueOptions;

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
    SpriteDialogue* GetDialogue(const std::string& dialogue_id);

    //! \name Class member access functions
    //@{
    vt_common::DIALOGUE_STATE GetDialogueState() const {
        return _state;
    }

    SpriteDialogue* GetCurrentDialogue() const {
        return _current_dialogue;
    }

    MapDialogueOptions* GetCurrentOptions() const {
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
    vt_common::DIALOGUE_STATE _state;

    //! \brief A numeric value used to generate unique dialogue ids.
    uint32_t _next_dialogue_id;

    //! \brief Contains all dialogues used in the map in a std::map structure. The dialogue IDs serve as the map keys
    std::map<std::string, SpriteDialogue*> _dialogues;

    //! \brief A pointer to the current piece of dialogue that is active
    SpriteDialogue* _current_dialogue;

    //! \brief A pointer to the current set of options for the active dialogue line
    MapDialogueOptions* _current_options;

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
};

} // namespace private_map

} // namespace vt_map

#endif // __MAP_DIALOGUE_SUPERVISOR_HEADER__
