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
*** \file    battle_finish.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for battle finish menu
***
*** This code takes effect after either the character or enemy party has emerged
*** victorious in the battle.
*** ***************************************************************************/

#ifndef __BATTLE_FINISH_HEADER__
#define __BATTLE_FINISH_HEADER__

#include "common/global/global_actors.h"
#include "common/gui/option.h"
#include "common/gui/menu_window.h"
#include "common/gui/textbox.h"

#include "modes/battle/battle_utils.h"

namespace vt_battle
{

namespace private_battle
{

//! \brief Enums for the various states that the FinishWindow class may be in
enum FINISH_STATE {
    FINISH_INVALID = -1,
    FINISH_ANNOUNCE_RESULT = 0, //!< Short sequence announcing outcome of the battle (victory or defeat) and showing GUI objects
    FINISH_DEFEAT_SELECT   = 1, //!< Player selects what to do after defeat (go to main menu, exit game, etc.)
    FINISH_DEFEAT_CONFIRM  = 2, //!< Player confirms defeat selection
    FINISH_VICTORY_GROWTH  = 4, //!< XP earned is displayed and gradually awarded to characters
    FINISH_VICTORY_SPOILS  = 5, //!< Drunes and objects dropped are displayed and gradually awarded to party
    FINISH_END             = 6, //!< Short sequence of hiding finish GUI objects
};

//! \brief The set of defeat options that the player can select
//@{
//! Retry the battle
const uint32_t DEFEAT_OPTION_RETRY     = 0;
//! End game and return to boot menu
const uint32_t DEFEAT_OPTION_END       = 1;
//@}


/** ****************************************************************************
*** \brief A container class for managing all of a character's growth information
***
*** Each character that participated in the battle will have an instance of this
*** object dedicated to it. This class is used to retain all of the growth in
*** stats and skills learned. The reason this class is needed is because when a
*** character gains a level, their internal growth data is reset, so we need to
*** retain this information until battle mode exits.
***
*** \note The public stats and skills members are kept public for convenience.
*** However, you should not change the values of these members as their data
*** is updated and managed intenerally.
*** ***************************************************************************/
class CharacterGrowth
{
public:
    //! \param ch A pointer to the character that this growth information represents
    CharacterGrowth(vt_global::GlobalCharacter* ch);

    ~CharacterGrowth()
        {}

    //! \brief Holds the accumulated growth for all stats
    //@{
    uint32_t hit_points;
    uint32_t skill_points;
    uint32_t strength;
    uint32_t vigor;
    uint32_t fortitude;
    uint32_t protection;
    uint32_t agility;
    float evade;
    //@}

    //! \brief A vector holding valid object pointers to all skills that have been learned
    std::vector<vt_global::GlobalSkill*> skills_learned;

    /** \brief Updates all class members with the latest growth from the character
    ***
    *** The best way to use this function is to call it after experience points have been added
    *** to the character. If GlobalCharacter::AddExperiencePoints() returns true (indicating growth),
    *** then call this method to handle all of the growth data. This method will make all necessary
    *** calls to GlobalCharacter::AcknowledgeGrowth() and handle corner cases such as multiple experience
    *** levels being gained.
    **/
    void UpdateGrowthData();

private:
    //! \brief A valid object pointer to the character that the growth in this class represents
    vt_global::GlobalCharacter* _character;

    //! \brief A counter that reflects the number of experience levels that the character has gained (0 for no levels gained)
    uint32_t _experience_levels_gained;
}; // class CharacterGrowth


/** ****************************************************************************
*** \brief Represents a collection of GUI objects drawn when the player loses the battle
***
*** This class assists the FinishSupervisor class. It is only utilized when the
*** player's characters are defeated in battle and presents the player with a
*** number of options.
***
*** - Retry: resets the state of the battle to the beginning
*** - Restart: loads the game state from the last save point
*** - Return: brings the player back to boot mode
*** - Retire: exits the game
*** ***************************************************************************/
class FinishDefeatAssistant
{
public:
    FinishDefeatAssistant(FINISH_STATE &state);

    ~FinishDefeatAssistant();

    void Initialize();

    //! \brief Processes user input and updates the GUI controls
    void Update();

    //! \brief Draws the finish window and GUI contents to the screen
    void Draw();

    //! \brief Returns the defeat option that the player selected
    uint32_t GetDefeatOption() const {
        return _options.GetSelection();
    }

private:
    //! \brief A reference to where the state of the finish GUI menus is maintained
    FINISH_STATE &_state;

    //! \brief The window that the defeat message and options are displayed upon
    vt_gui::MenuWindow _options_window;

    //! \brief The window that the defeat message and options are displayed upon
    vt_gui::MenuWindow _tooltip_window;

    //! \brief The list of options that the player may choose from when they lose the battle
    vt_gui::OptionBox _options;

    //! \brief A simple "yes/no" confirmation to the selected option
    vt_gui::OptionBox _confirm_options;

    //! \brief Tooltip text explaining the currently selected option
    vt_gui::TextBox _tooltip;

    //! \brief Changes the text displayed by the tooltip based on the current state and selected option
    void _SetTooltipText();
}; // class FinishDefeatAssistant


/** ****************************************************************************
*** \brief Manages game state after the battle has been won and processes rewards
***
*** This class presents the user with the results of the battle. More specifically,
*** the following events are accomplished
***
*** -#) Display experience points gained and any growth acquired for each character
*** -#) Display the number of drunes earned and the type and quantity of any objects recovered
***
*** If the player lost the battle one or more times before they achieved victory, their XP and
*** drune rewards will be cut significantly for each retry.
*** ***************************************************************************/
class FinishVictoryAssistant
{
public:
    FinishVictoryAssistant(FINISH_STATE &state);

    ~FinishVictoryAssistant();

    //! \brief Instructs the class to prepare itself for future updating and drawing
    void Initialize();

    //! \brief Updates the state of the victory displays
    void Update();

    //! \brief Draws the appropriate information to the screen
    void Draw();

private:
    //! \brief A reference to where the state of the finish GUI menus is maintained
    FINISH_STATE &_state;

    //! \brief The total number of characters in the victorious party, living or dead
    uint32_t _characters_number;

    //! \brief The amount of xp earned for the victory (per character)
    uint32_t _xp_earned;

    //! \brief Tells to which character the raw fighting XP bonus is given to,
    //! and whether it has been given. True if given.
    //! The Raw fighting XP bonus is given when a character has won
    //! without wearing any equipment.
    bool _raw_xp_given[4];
    //! \brief Tells whether the Raw XP bonus was won.
    bool _raw_xp_won[4];

    //! \brief The amount of drunes dropped by the enemy party
    uint32_t _drunes_dropped;

    //! \brief When set to true, counting out of XP or Drunes will begin
    bool _begin_counting;

    //! \brief Retains the number of character windows that were created
    uint32_t _number_character_windows_created;

    //! \brief Pointers to all characters who took part in the battle
    std::vector<vt_global::GlobalCharacter *> _characters;

    //! \brief The growth data container objects for each corresponding character in _characters
    std::vector<CharacterGrowth> _character_growths;

    //! \brief Holds portrait images for each character portraits
    vt_video::StillImage _character_portraits[4];

    //! \brief Holds all objects that were dropped by the defeated enemy party (<ID, quantity>)
    std::map<vt_global::GlobalObject *, int32_t> _objects_dropped;

    //! \brief The top window in the GUI display that contains header text
    vt_gui::MenuWindow _header_window;

    //! \brief A window for each character showing any change to their attributes
    vt_gui::MenuWindow _character_window[4];

    //! \brief A window used to display details about objects dropped by the defeated enemies
    vt_gui::MenuWindow _spoils_window;

    //! \brief Drawn to the top header window displaying information about the stats/items obtained
    vt_gui::TextBox _header_growth;
    vt_gui::TextBox _header_drunes_dropped;
    vt_gui::TextBox _header_total_drunes;

    //! \brief Four row, four column option box for each character to display their stat growth
    vt_gui::OptionBox _growth_list[4];

    //! \brief Holds the experience level and XP points remaining for each character
    vt_gui::TextBox _level_text[4];
    vt_gui::TextBox _xp_text[4];

    //! \brief Holds the text indicating new skills that each character has learned
    vt_gui::TextBox _skill_text[4];

    //! \brief Header text for the object list option box
    vt_gui::TextBox _object_header_text;

    //! \brief Displays all objects obtained by the character party
    vt_gui::OptionBox _object_list;

    //! \brief Creates the character windows and any GUI objects that populate them
    void _CreateCharacterGUIObjects();

    //! \brief Populates the object list with the objects contained in the _dropped_objects container
    void _CreateObjectList();

    //! \brief Updates the character HP/SP before the battle exits
    void _SetCharacterStatus();

    //! \brief Sets the text to display in the header window depending upon the current state
    void _SetHeaderText();

    //! \brief Gradually rewards the characters with the XP that they earned
    void _UpdateGrowth();

    //! \brief Gradually counts out the amount of drunes that the party has earned
    void _UpdateSpoils();

    //! \brief Draws the XP earned by the party and any attribute growth they have made
    void _DrawGrowth(uint32_t index);

    //! \brief Draws the number of drunes and items dropped by the enemy party
    void _DrawSpoils();
}; // class FinishVictoryAssistant


/** ****************************************************************************
*** \brief Manages game state after the battle has been either won or lost
***
*** Most of the grunt work is done by either the FinishDefeatAssistant or FinishVictoryAssistant
*** classes, depending on what the outcome of the battle was. This class does
*** some of the display and update work on its own however, such as adjusting
*** the scene lighting and displaying the victory/defeat message.
*** ***************************************************************************/
class FinishSupervisor
{
public:
    FinishSupervisor();

    ~FinishSupervisor()
    {}

    /** \brief Un-hides the window display and creates the window contents
    *** \param victory Set to true if the player's party was victorious in battle; false if he/she was defeated
    **/
    void Initialize(bool victory);

    //! \brief Updates the state of the window
    void Update();

    //! \brief Draws the window and its contents
    void Draw();

    FINISH_STATE GetState() const {
        return _state;
    }

private:
    //! \brief Maintains state information to determine how to process user input and what to draw to the screen
    FINISH_STATE _state;

    //! \brief Boolean used to determine if the battle was victorious for the player (true) or if the player was defeated (false)
    bool _battle_victory;

    //! \brief Assists this class when the player was defeated in battle
    FinishDefeatAssistant _defeat_assistant;

    //! \brief Assists this class when the player was victorious in battle
    FinishVictoryAssistant _victory_assistant;

    //! \brief Used to announce the battle's outcome (victory or defeat)
    vt_gui::TextBox _outcome_text;
}; // class FinishSupervisor

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_FINISH_HEADER__
