////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    save_mode.h
*** \author  Jacob Rudolph, rujasu@allacrost.org
*** \brief   Header file for save interface.
***
*** This code is for saving and loading.
*** ***************************************************************************/

#ifndef __SAVE_HEADER__
#define __SAVE_HEADER__

#include "engine/mode_manager.h"

#include "common/gui/menu_window.h"
#include "common/gui/textbox.h"
#include "common/gui/option.h"

//! \brief All calls to save mode are wrapped in this namespace.
namespace hoa_save {

//! \brief Determines whether the code in the hoa_save namespace should print debug statements or not.
extern bool SAVE_DEBUG;

/** ****************************************************************************
*** \brief Represents an individual character window
***
*** There should be one of these windows for each character in the game.
*** It will contain all the information of the character and handle its draw
*** placement.
*** ***************************************************************************/
class SmallCharacterWindow : public hoa_gui::MenuWindow {
private:
	//! The name of the character that this window corresponds) to
	hoa_global::GlobalCharacter* _character;

	//! The image of the character
	hoa_video::StillImage _portrait;

public:
	SmallCharacterWindow();

	~SmallCharacterWindow();

	/** \brief Set the character for this window
	*** \param character the character to associate with this window
	**/
	void SetCharacter(hoa_global::GlobalCharacter *character);

	/** \brief render this window to the screen
	*** \return success/failure
	**/
	void Draw();
}; // class SmallCharacterWindow : public hoa_video::MenuWindow


/** ****************************************************************************
*** \brief Handles saving and loading
***
*** ***************************************************************************/
class SaveMode : public hoa_mode_manager::GameMode {
public:
	/**
	*** \param enable_saving Tells whether the save mode permits saving.
	*** \param positions When used in a save point, the save map tile positions are given there.
	**/
	SaveMode(bool save_mode, uint32 x_position = 0, uint32 y_position = 0);

	~SaveMode();

	//! \brief Resets appropriate class members. Called whenever SaveMode is made the active game mode.
	void Reset();

	//! \brief Updates the game state by the amount of time that has elapsed
	void Update();

	//! \brief Draws the next frame to be displayed on the screen
	void DrawPostEffects();

	// Empty since everything is drawn in post effects.
	void Draw()
	{}

private:
	//! \brief Attempts to load a game. returns true on success, false on fail
	bool _LoadGame(int);

	//! \brief Loads preview data for the highlighted game
	bool _PreviewGame(int);

	//! \brief Clears out the data saves. Used especially when the data is invalid.
	void _ClearSaveData();

	//! \brief The MenuWindow for the backdrop
	hoa_gui::MenuWindow _window;

	//! \brief The MenuWindow for the left panel
	hoa_gui::MenuWindow _left_window;

	//! \brief The save mode title window
	hoa_gui::MenuWindow _title_window;
	hoa_gui::TextBox _title_textbox;

	//! \brief Windows to display character previews
	hoa_save::SmallCharacterWindow _character_window[4];

	//! \brief Current state of SaveMode
	uint8 _current_state;

	//! \brief A screen capture of the last frame rendered on the screen before SaveMode was invoked
	hoa_video::StillImage _screen_capture;

	//! \brief The color used to dim the background screen capture image
	hoa_video::Color _dim_color;

	//! \brief The list of files to save/load from
	hoa_gui::OptionBox _file_list;

	//! \brief Box to confirm saving current file
	hoa_gui::OptionBox _confirm_save_optionbox;

	//! \brief Displays message that game was saved successfully
	hoa_gui::TextBox _save_success_message;

	//! \brief Displays message that game was saved successfully
	hoa_gui::TextBox _save_failure_message;

	//! \brief Displays preview info for highlighted game
	hoa_gui::TextBox _map_name_textbox;
	hoa_gui::TextBox _time_textbox;
	hoa_gui::TextBox _drunes_textbox;

	//! \brief Used to know whether a save position has to be saved.
	uint32 _x_position, _y_position;

	//! \brief Tells whether we're in save or load mode.
	bool _save_mode;
}; // class SaveMode : public hoa_mode_manager::GameMode

} // namespace hoa_save

#endif // __SAVE_HEADER__
