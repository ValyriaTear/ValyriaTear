////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_finish.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for battle finish menu
***
*** This code takes effect after either the character or enemy party has emerged
*** victorious in the battle.
*** ***************************************************************************/

#ifndef __BATTLE_FINISH_HEADER__
#define __BATTLE_FINISH_HEADER__

#include "utils.h"
#include "defs.h"

#include "video.h"

#include "gui.h"
#include "global.h"

#include "battle_utils.h"

namespace hoa_battle {

namespace private_battle {

//! \brief Enums for the various states that the FinishWindow class may be in
enum FINISH_STATE {
	FINISH_INVALID = -1,
	FINISH_ANNOUNCE_RESULT = 0, //!< Short sequence announcing outcome of the battle (victory or defeat) and showing GUI objects
	FINISH_DEFEAT_SELECT   = 1, //!< Player selects what to do after defeat (go to main menu, exit game, etc.)
	FINISH_DEFEAT_CONFIRM  = 2, //!< Player confirms defeat selection
	FINISH_VICTORY_GROWTH  = 4, //!< XP earned is displayed and gradually awarded to characters
	FINISH_VICTORY_SPOILS  = 5, //!< Drunes and objects dropped are displayed and gradually awarded to party
	FINISH_END             = 6, //!< Short sequence of hiding finish GUI objects

// 	//! Announces that the player is victorious and notes any characters who have gained an experience level
// 	FINISH_WIN_ANNOUNCE = 0,
// 	//! Initial display of character stats
// 	FINISH_WIN_SHOW_GROWTH = 1,
// 	//! Performs countdown of XP (adding it to chars) and triggers level ups
// 	FINISH_WIN_COUNTDOWN_GROWTH = 2,
// 	//! All XP has been added (or should be added instantly), shows final stats
// 	FINISH_WIN_RESOLVE_GROWTH = 3,
// 	//! Display of any skills learned
// 	FINISH_WIN_SHOW_SKILLS = 4,
// 	//! Reports all drunes earned and dropped items obtained
// 	FINISH_WIN_SHOW_SPOILS = 5,
// 	//! Adds $ earned to party's pot
// 	FINISH_WIN_COUNTDOWN_SPOILS = 6,
// 	//! All money and items have been added
// 	FINISH_WIN_RESOLVE_SPOILS = 7,
// 	//! We've gone through all the states of the FinishWindow in Win form
// 	FINISH_WIN_COMPLETE = 8,
// 	//! Announces that the player has lost and queries the player for an action
// 	FINISH_LOSE_ANNOUNCE = 9,
// 	//! Used to double-confirm when the player selects to quit the game or return to the main menu
// 	FINISH_LOSE_CONFIRM = 10,
// 	FINISH_TOTAL = 11
};

//! \brief The set of defeat options that the player can select
//@{
const uint32 DEFEAT_OPTION_RETRY     = 0;
const uint32 DEFEAT_OPTION_RESTART   = 1;
const uint32 DEFEAT_OPTION_RETURN    = 2;
const uint32 DEFEAT_OPTION_RETIRE    = 3;
//@}

//! \brief The maximum number of times that the player can try to win the battle
const uint32 MAX_BATTLE_ATTEMPTS     = 3;

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
class FinishDefeatAssistant {
public:
	FinishDefeatAssistant(FINISH_STATE& state);

	~FinishDefeatAssistant();

	void Initialize(uint32 retries_left);

	//! \brief Processes user input and updates the GUI controls
	void Update();

	//! \brief Draws the finish window and GUI contents to the screen
	void Draw();

	//! \brief Returns the defeat option that the player selected
	uint32 GetDefeatOption() const
		{ return _options.GetSelection(); }

private:
	//! \brief A reference to where the state of the finish GUI menus is maintained
	FINISH_STATE& _state;

	//! \brief The number of chances the player has left to retry the battle
	uint32 _retries_left;

	//! \brief The window that the defeat message and options are displayed upon
	hoa_gui::MenuWindow _options_window;

	//! \brief The window that the defeat message and options are displayed upon
	hoa_gui::MenuWindow _tooltip_window;

	//! \brief The list of options that the player may choose from when they lose the battle
	hoa_gui::OptionBox _options;

	//! \brief A simple "yes/no" confirmation to the selected option
	hoa_gui::OptionBox _confirm_options;

	//! \brief Tooltip text explaining the currently selected option
	hoa_gui::TextBox _tooltip;

	//! \brief Changes the text displayed by the tooltip based on the current state and selected option
	void _SetTooltipText();
}; // class FinishDefeatAssistant


/** ****************************************************************************
*** \brief Manages game state after the battle has been won and processes rewards
***
*** This class presents the user with the results of the battle. More specifically,
*** the following events are accomplished
***
*** -#) Display experience points gained and any change in stats for each character
*** -#) Display the number of drunes earned and the type and quantity of any objects recovered
***
*** If the player lost the battle one or more times before they achieved victory, their XP and
*** drune rewards will be cut significantly for each retry.
*** ***************************************************************************/
class FinishVictoryAssistant {
public:
	FinishVictoryAssistant(FINISH_STATE& state);

	~FinishVictoryAssistant();

	/** \brief Instructs the class to prepare itself for future updating and drawing
	*** \param retries_used The number of retries that the player used before the battle was won
	**/
	void Initialize(uint32 retries_used);

	//! \brief Updates the state of the victory displays
	void Update();

	//! \brief Draws the appropriate information to the screen
	void Draw();

private:
	//! \brief A reference to where the state of the finish GUI menus is maintained
	FINISH_STATE& _state;

	//! \brief The number of retries the player used before achieving this victory
	uint32 _retries_used;

	//! \brief The total number of characters in the victorious party, living or dead
	uint32 _number_characters;

	//! \brief The amount of xp earned for the victory (per character)
	uint32 _xp_earned;

	//! \brief The amount of drunes dropped by the enemy party
	uint32 _drunes_dropped;

	//! \brief Retains the number of character windows that were created
	uint32 _number_character_windows_created;

	//! \brief Pointers to all characters who took part in the battle
	std::vector<hoa_global::GlobalCharacter*> _characters;

	//! \brief The growth members for all members of the _characters table
	std::vector<hoa_global::GlobalCharacterGrowth*> _character_growths;

	//! \brief Holds portrait images for each character portraits
	hoa_video::StillImage _character_portraits[4];

	//! \brief Holds all objects that were dropped by the defeated enemy party (<ID, quantity>)
	std::map<hoa_global::GlobalObject*, int32> _objects_dropped;

	//! \brief The top window in the GUI display that contains header text
	hoa_gui::MenuWindow _header_window;

	//! \brief A window for each character showing any change to their attributes
	hoa_gui::MenuWindow _character_window[4];

	//! \brief A window used to display details about objects dropped by the defeated enemies
	hoa_gui::MenuWindow _spoils_window;

	//! \brief Drawn to the top header window displaying information about the
	hoa_gui::TextBox _header_text;

	//! \brief Four row, four column option box for each character to display their stat growth
	hoa_gui::OptionBox _growth_list[4];

	//! \brief Holds the experience level and XP points remaining for each character
	hoa_gui::TextBox _level_xp_text[4];

	//! \brief Holds the text indicating new skills that each character has learned
	hoa_gui::TextBox _skill_text[4];

	//! \brief Header text for the object list option box
	hoa_gui::TextBox _object_header_text;

	//! \brief Displays all objects obtained by the character party
	hoa_gui::OptionBox _object_list;

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
	void _DrawGrowth(uint32 index);

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
class FinishSupervisor {
public:
	FinishSupervisor();

	~FinishSupervisor();

	/** \brief Un-hides the window display and creates the window contents
	*** \param victory Set to true if the player's party was victorious in battle; false if he/she was defeated
	**/
	void Initialize(bool victory);

	//! \brief Updates the state of the window
	void Update();

	//! \brief Draws the window and its contents
	void Draw();

	FINISH_STATE GetState() const
		{ return _state; }

private:
	//! \brief Maintains state information to determine how to process user input and what to draw to the screen
	FINISH_STATE _state;

	//! \brief Tracks how many attempts the player has made thus far to win this battle
	uint32 _attempt_number;

	//! \brief Boolean used to determine if the battle was victorious for the player (true) or if the player was defeated (false)
	bool _battle_victory;

	//! \brief Assists this class when the player was defeated in battle
	FinishDefeatAssistant _defeat_assistant;

	//! \brief Assists this class when the player was victorious in battle
	FinishVictoryAssistant _victory_assistant;

	//! \brief Used to announce the battle's outcome (victory or defeat)
	hoa_gui::TextBox _outcome_text;
}; // class FinishSupervisor

} // namespace private_battle

} // namespace hoa_battle

#endif // __BATTLE_FINISH_HEADER__
