////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    save_mode.h
*** \author  Jacob Rudolph, rujasu@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
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

namespace vt_global {
class GlobalCharacter;
}

//! \brief All calls to save mode are wrapped in this namespace.
namespace vt_save
{

//! \brief Determines whether the code in the vt_save namespace should print debug statements or not.
extern bool SAVE_DEBUG;

/** ****************************************************************************
*** \brief Represents an individual character window
***
*** There should be one of these windows for each character in the game.
*** It will contain all the information of the character and handle its draw
*** placement.
*** ***************************************************************************/
class SmallCharacterWindow : public vt_gui::MenuWindow
{
public:
    SmallCharacterWindow():
        _character(NULL)
    {}

    ~SmallCharacterWindow();

    /** \brief Set the character for this window
    *** \param character the character to associate with this window
    **/
    void SetCharacter(vt_global::GlobalCharacter *character);

    /** \brief render this window to the screen
    *** \return success/failure
    **/
    void Draw();

private:
    //! The name of the character that this window corresponds) to
    vt_global::GlobalCharacter *_character;

    //! The image of the character
    vt_video::StillImage _portrait;

    //! The text along with the character portrait
    vt_video::TextImage _character_name;
    vt_video::TextImage _character_data;
}; // class SmallCharacterWindow : public vt_video::MenuWindow


/** ****************************************************************************
*** \brief Handles saving and loading
***
*** ***************************************************************************/
class SaveMode : public vt_mode_manager::GameMode
{
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
    bool _LoadGame(uint32 id);

    //! \brief Loads preview data for the highlighted game
    bool _PreviewGame(uint32 id);

    //! \brief Clears out the data saves. Used especially when the data is invalid.
    //! \param selected_file_exists Tells whether the selected file exists.
    void _ClearSaveData(bool selected_file_exists);

    //! \brief Check the save validity of the save slots and disable those invalid.
    //! \returns whether at least one save is valid.
    bool _CheckSavesValidity();

    //! \brief The MenuWindow for the backdrop
    vt_gui::MenuWindow _window;

    //! \brief The MenuWindow for the left panel
    vt_gui::MenuWindow _left_window;

    //! \brief The save mode title window
    vt_gui::MenuWindow _title_window;
    vt_gui::TextBox _title_textbox;

    //! \brief Windows to display character previews
    vt_save::SmallCharacterWindow _character_window[4];

    //! \brief Current state of SaveMode
    uint8 _current_state;

    //! \brief A screen capture of the last frame rendered on the screen before SaveMode was invoked
    vt_video::StillImage _screen_capture;

    //! \brief The color used to dim the background screen capture image
    vt_video::Color _dim_color;

    //! \brief The list of files to save/load from
    vt_gui::OptionBox _file_list;

    //! \brief Box to confirm saving current file
    vt_gui::OptionBox _confirm_save_optionbox;

    //! \brief Displays message that game was saved successfully
    vt_gui::TextBox _save_success_message;

    //! \brief Displays message that game was saved successfully
    vt_gui::TextBox _save_failure_message;

    //! \brief Tells the user no saves are valid.
    vt_gui::TextBox _no_valid_saves_message;

    //! \brief Displays preview info for highlighted game
    vt_gui::TextBox _map_name_textbox;
    vt_gui::TextBox _time_textbox;
    vt_gui::TextBox _drunes_textbox;
    vt_video::StillImage _location_image;

    //! Icon references from Global Media, do not delete those!
    vt_video::StillImage* _clock_icon;
    vt_video::StillImage* _drunes_icon;

    //! \brief Used to know whether a save position has to be saved.
    uint32 _x_position, _y_position;

    //! \brief Tells whether we're in save or load mode.
    bool _save_mode;
}; // class SaveMode : public vt_mode_manager::GameMode

} // namespace vt_save

#endif // __SAVE_HEADER__
