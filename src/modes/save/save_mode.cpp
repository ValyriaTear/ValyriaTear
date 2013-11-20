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
*** \file    save_mode.cpp
*** \author  Jacob Rudolph, rujasu@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for save mode interface.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "save_mode.h"

#include "common/global/global.h"
#include "engine/audio/audio.h"
#include "engine/video/video.h"
#include "engine/input.h"
#include "modes/boot/boot.h"
#include "modes/map/map_mode.h"
#include "utils/utils_files.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_mode_manager;
using namespace vt_input;
using namespace vt_system;
using namespace vt_boot;
using namespace vt_global;
using namespace vt_map;
using namespace vt_script;

namespace vt_save
{

bool SAVE_DEBUG = false;

//! \name Save Options Constants
//@{
const uint8 SAVE_GAME           = 0;
const uint8 SAVE_LOAD_GAME      = 1;
//@}

//! \name SaveMode States
//@{
const uint8 SAVE_MODE_SAVING          = 0;
const uint8 SAVE_MODE_LOADING         = 1;
const uint8 SAVE_MODE_CONFIRMING_SAVE = 2;
const uint8 SAVE_MODE_SAVE_COMPLETE   = 3;
const uint8 SAVE_MODE_SAVE_FAILED     = 4;
const uint8 SAVE_MODE_FADING_OUT      = 5;
const uint8 SAVE_MODE_NO_VALID_SAVES  = 6;
//@}

SaveMode::SaveMode(bool save_mode, uint32 x_position, uint32 y_position) :
    GameMode(),
    _current_state(SAVE_MODE_LOADING),
    _dim_color(0.35f, 0.35f, 0.35f, 1.0f), // A grayish opaque color
    _x_position(x_position),
    _y_position(y_position),
    _save_mode(save_mode)
{
    mode_type = MODE_MANAGER_SAVE_MODE;

    _window.Create(600.0f, 500.0f);
    _window.SetPosition(212.0f, 138.0f);
    _window.Hide();

    _left_window.Create(150.0f, 500.0f);
    _left_window.SetPosition(212.0f, 138.0f);
    _left_window.Show();

    _title_window.Create(600.0f, 50.0f);
    _title_window.SetPosition(212.0f, 88.0f);
    _title_window.Show();

    // Initialize the save successful message box
    _title_textbox.SetPosition(552.0f, 103.0f);
    _title_textbox.SetDimensions(200.0f, 50.0f);
    _title_textbox.SetTextStyle(TextStyle("title22"));
    _title_textbox.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    if(_save_mode)
        _title_textbox.SetDisplayText(UTranslate("Save Game"));
    else
        _title_textbox.SetDisplayText(UTranslate("Load Game"));

    for(uint32 i = 0; i < 4; ++i) {
        _character_window[i].Create(450.0f, 100.0f);
        _character_window[i].Show();
    }

    _character_window[0].SetPosition(355.0f, 138.0f);
    _character_window[1].SetPosition(355.0f, 238.0f);
    _character_window[2].SetPosition(355.0f, 338.0f);
    _character_window[3].SetPosition(355.0f, 438.0f);

    // Initialize the save options box
    _file_list.SetPosition(315.0f, 384.0f);
    _file_list.SetDimensions(150.0f, 500.0f, 1, 6, 1, 6);
    _file_list.SetTextStyle(TextStyle("title22"));

    _file_list.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _file_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _file_list.SetSelectMode(VIDEO_SELECT_SINGLE);
    _file_list.SetCursorOffset(-58.0f, -18.0f);

    _file_list.AddOption(UTranslate("Slot 1"));
    _file_list.AddOption(UTranslate("Slot 2"));
    _file_list.AddOption(UTranslate("Slot 3"));
    _file_list.AddOption(UTranslate("Slot 4"));
    _file_list.AddOption(UTranslate("Slot 5"));
    _file_list.AddOption(UTranslate("Slot 6"));

    // Restore the cursor position to the last load/save position.
    uint32 slot_id = GlobalManager->GetGameSlotId();

    _file_list.SetSelection(slot_id);

    // Initialize the confirmation option box
    _confirm_save_optionbox.SetPosition(512.0f, 384.0f);
    _confirm_save_optionbox.SetDimensions(250.0f, 200.0f, 1, 2, 1, 2);
    _confirm_save_optionbox.SetTextStyle(TextStyle("title22"));

    _confirm_save_optionbox.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _confirm_save_optionbox.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _confirm_save_optionbox.SetSelectMode(VIDEO_SELECT_SINGLE);
    _confirm_save_optionbox.SetCursorOffset(-58.0f, -18.0f);

    _confirm_save_optionbox.AddOption(UTranslate("Confirm Save"));
    _confirm_save_optionbox.AddOption(UTranslate("Cancel"));
    _confirm_save_optionbox.SetSelection(0);

    // Initialize the save successful message box
    _save_success_message.SetPosition(552.0f, 314.0f);
    _save_success_message.SetDimensions(250.0f, 100.0f);
    _save_success_message.SetTextStyle(TextStyle("title22"));
    _save_success_message.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _save_success_message.SetDisplayText(UTranslate("Save successful!"));

    // Initialize the save failure message box
    _save_failure_message.SetPosition(512.0f, 384.0f);
    _save_failure_message.SetDimensions(250.0f, 100.0f);
    _save_failure_message.SetTextStyle(TextStyle("title22"));
    _save_failure_message.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _save_failure_message.SetDisplayText(UTranslate("Unable to save game!\nSave FAILED!"));

    _no_valid_saves_message.SetPosition(512.0f, 384.0f);
    _no_valid_saves_message.SetDimensions(250.0f, 100.0f);
    _no_valid_saves_message.SetTextStyle(TextStyle("title22"));
    _no_valid_saves_message.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _no_valid_saves_message.SetDisplayText(UTranslate("No valid saves found!"));

    // Initialize the save preview text boxes
    _map_name_textbox.SetPosition(600.0f, 558.0f);
    _map_name_textbox.SetDimensions(300.0f, 26.0f);
    _map_name_textbox.SetTextStyle(TextStyle("title22"));
    _map_name_textbox.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _map_name_textbox.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _map_name_textbox.SetDisplayText(" ");

    _time_textbox.SetPosition(600.0f, 583.0f);
    _time_textbox.SetDimensions(250.0f, 26.0f);
    _time_textbox.SetTextStyle(TextStyle("title22"));
    _time_textbox.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _time_textbox.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _time_textbox.ClearText();

    _drunes_textbox.SetPosition(600.0f, 613.0f);
    _drunes_textbox.SetDimensions(250.0f, 26.0f);
    _drunes_textbox.SetTextStyle(TextStyle("title22"));
    _drunes_textbox.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _drunes_textbox.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _drunes_textbox.ClearText();

    _clock_icon = vt_global::GlobalManager->Media().GetClockIcon();
    _clock_icon->SetWidthKeepRatio(30.0f);
    _drunes_icon = vt_global::GlobalManager->Media().GetDrunesIcon();
    _drunes_icon->SetWidthKeepRatio(30.0f);

    if(_save_mode) {
        _current_state = SAVE_MODE_SAVING;
    } else {
        // When in load mode, check the saves validity and skip invalid slots
        _file_list.SetSkipDisabled(true);
        if (!_CheckSavesValidity())
            _current_state = SAVE_MODE_NO_VALID_SAVES;
    }

    _window.Show();

    // Load the first slot data
    if(_file_list.GetSelection() > -1)
        _PreviewGame(_file_list.GetSelection());
}



SaveMode::~SaveMode()
{
    _window.Destroy();

    _left_window.Destroy();
    _title_window.Destroy();

    for(uint32 i = 0; i < 4; ++i) {
        _character_window[i].Destroy();
    }

}



void SaveMode::Reset()
{
    // Save a copy of the current screen to use as the backdrop
    try {
        _screen_capture = VideoManager->CaptureScreen();
    } catch(const Exception &e) {
        IF_PRINT_WARNING(SAVE_DEBUG) << e.ToString() << std::endl;
    }

    VideoManager->SetStandardCoordSys();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
}



void SaveMode::Update()
{
    if(InputManager->QuitPress() == true) {
        ModeManager->Pop();
        return;
    }

    // Screen is in the process of fading out, in order to load a game
    if(_current_state == SAVE_MODE_FADING_OUT) {
        return;
    }

    _file_list.Update();
    _confirm_save_optionbox.Update();

    // Otherwise, it's time to start handling events.
    if(InputManager->ConfirmPress()) {
        switch(_current_state) {
        case SAVE_MODE_SAVING:
            if(_file_list.GetSelection() > -1) {
                _current_state = SAVE_MODE_CONFIRMING_SAVE;
            }
            break;

        case SAVE_MODE_CONFIRMING_SAVE:
            if(_confirm_save_optionbox.GetSelection() == 0) {
                // note: using int here, because uint8 will NOT work
                // do not change unless you understand this and can test it properly!
                uint32 id = (uint32)_file_list.GetSelection();
                std::ostringstream f;
                f << GetUserDataPath() + "saved_game_" << id << ".lua";
                std::string filename = f.str();
                // now, attempt to save the game.  If failure, we need to tell the user that!
                if(GlobalManager->SaveGame(filename, id, _x_position, _y_position)) {
                    _current_state = SAVE_MODE_SAVE_COMPLETE;
                    AudioManager->PlaySound("snd/save_successful_nick_bowler_oga.wav");
                } else {
                    _current_state = SAVE_MODE_SAVE_FAILED;
                    AudioManager->PlaySound("snd/cancel.wav");
                }
            } else {
                _current_state = SAVE_MODE_SAVING;
            }
            break;

        case SAVE_MODE_SAVE_COMPLETE:
        case SAVE_MODE_SAVE_FAILED:
            _current_state = SAVE_MODE_SAVING;
            _PreviewGame(_file_list.GetSelection());
            break;

        case SAVE_MODE_LOADING:
            if(_file_list.GetSelection() > -1) {
                _LoadGame(_file_list.GetSelection());
            } else {
                // Leave right away where there is nothing else
                // to do than loading.
                ModeManager->Pop();
            }
            break;
        default:
        case SAVE_MODE_NO_VALID_SAVES:
                // Leave right away as there is nothing else to do
                ModeManager->Pop();
            break;
        } // end switch (_current_state)
    } // end if (InputManager->ConfirmPress())

    else if(InputManager->CancelPress()) {
        switch(_current_state) {
        default:
        case SAVE_MODE_NO_VALID_SAVES:
        case SAVE_MODE_SAVING:
        case SAVE_MODE_LOADING:
            // Leave right away where there is nothing else to do
            ModeManager->Pop();
            break;

        case SAVE_MODE_CONFIRMING_SAVE:
            _current_state = SAVE_MODE_SAVING;
            _PreviewGame(_file_list.GetSelection());
            break;
        } // end switch (_current_state)
    } // end if (InputManager->CancelPress())

    else if(InputManager->UpPress()) {
        switch(_current_state) {
        case SAVE_MODE_SAVING:
        case SAVE_MODE_LOADING:
            _file_list.InputUp();
            if(_file_list.GetSelection() > -1) {
                _PreviewGame(_file_list.GetSelection());
            } else {
                _ClearSaveData(false);
            }
            break;

        case SAVE_MODE_CONFIRMING_SAVE:
            _confirm_save_optionbox.InputUp();
            break;
        } // end switch (_current_state)
    } // end if (InputManager->UpPress())

    else if(InputManager->DownPress()) {
        switch(_current_state) {
        case SAVE_MODE_SAVING:
        case SAVE_MODE_LOADING:
            _file_list.InputDown();
            if(_file_list.GetSelection() > -1) {
                _PreviewGame(_file_list.GetSelection());
            }
            else {
                _ClearSaveData(false);
            }
            break;

        case SAVE_MODE_CONFIRMING_SAVE:
            _confirm_save_optionbox.InputDown();
            break;
        } // end switch (_current_state)
    } // end if (InputManager->DownPress())
} // void SaveMode::Update()



void SaveMode::DrawPostEffects()
{
    // Set the coordinate system for the background and draw
    float width = _screen_capture.GetWidth();
    float height = _screen_capture.GetHeight();
    VideoManager->SetCoordSys(0, width, 0, height);
    VideoManager->Move(0.0f, 0.0f);
    _screen_capture.Draw(_dim_color);

    // Re-set the coordinate system for everything else
    VideoManager->SetStandardCoordSys();

    _window.Draw();

    // Draw the title above everything else
    _title_window.Draw();
    _title_textbox.Draw();

    switch(_current_state) {
    case SAVE_MODE_SAVING:
    case SAVE_MODE_LOADING:
        _left_window.Draw(); // draw a panel on the left for the file list
        if(_file_list.GetSelection() > -1) {
            for(uint32 i = 0; i < 4; i++) {
                _character_window[i].Draw();
            }
        }
        _file_list.Draw();

        VideoManager->Move(420.0f, 638.0f);
        if (!_location_image.GetFilename().empty())
            _location_image.Draw(Color(1.0f, 1.0f, 1.0f, 0.4f));

        _map_name_textbox.Draw();

        if (_time_textbox.IsEmpty() || _drunes_textbox.IsEmpty())
            break;

        VideoManager->MoveRelative(15.0f, -35.0f);
        _clock_icon->Draw();
        _time_textbox.Draw();
        VideoManager->MoveRelative(0.0f, 30.0f);
        _drunes_icon->Draw();
        _drunes_textbox.Draw();
        break;
    case SAVE_MODE_CONFIRMING_SAVE:
        _confirm_save_optionbox.Draw();
        break;
    case SAVE_MODE_SAVE_COMPLETE:
        _save_success_message.Draw();
        break;
    case SAVE_MODE_SAVE_FAILED:
        _save_failure_message.Draw();
        break;
    case SAVE_MODE_NO_VALID_SAVES:
        _no_valid_saves_message.Draw();
        break;
    default:
    case SAVE_MODE_FADING_OUT:
        break;
    }
}

bool SaveMode::_LoadGame(uint32 id)
{
    std::ostringstream f;
    f << GetUserDataPath() + "saved_game_" << id << ".lua";
    std::string filename = f.str();

    if(DoesFileExist(filename)) {
        _current_state = SAVE_MODE_FADING_OUT;
        AudioManager->StopAllMusic();

        GlobalManager->LoadGame(filename, id);

        // Create a new map mode, and fade out and in
        ModeManager->PopAll();
        try {
            MapMode *MM = new MapMode(GlobalManager->GetMapDataFilename(), GlobalManager->GetMapScriptFilename());
            ModeManager->Push(MM, true, true);
        } catch(const luabind::error &e) {
            PRINT_ERROR << "Map::_Load -- Error loading map data "
                        << GlobalManager->GetMapDataFilename()
                        << ", script: " << GlobalManager->GetMapScriptFilename()
                        << ", returning to BootMode." << std::endl
                        << "Exception message:" << std::endl;
            ScriptManager->HandleLuaError(e);
            ModeManager->Push(new BootMode(), true, true);
        }
        return true;
    } else {
        PRINT_ERROR << "BOOT: No saved game file exists, can not load game: "
                    << filename << std::endl;
        return false;
    }
}


void SaveMode::_ClearSaveData(bool selected_file_exists)
{
    if (selected_file_exists)
        _map_name_textbox.SetDisplayText(UTranslate("Invalid data!"));
    else
        _map_name_textbox.SetDisplayText(UTranslate("No data"));
    _time_textbox.ClearText();
    _drunes_textbox.ClearText();
    _location_image.Clear();
    for (uint32 i = 0; i < 4; ++i)
        _character_window[i].SetCharacter(NULL);
}


bool SaveMode::_PreviewGame(uint32 id)
{
    std::ostringstream f;
    f << GetUserDataPath() + "saved_game_" << id << ".lua";
    std::string filename = f.str();

    // Check for the file existence, prevents a useless warning
    if(!vt_utils::DoesFileExist(filename)) {
        _ClearSaveData(false);
        return false;
    }

    ReadScriptDescriptor file;

    // Clear out the save data namespace to avoid loading false information
    // when dealing with a save game that has an invalid namespace
    ScriptManager->DropGlobalTable("save_game1");

    if(!file.OpenFile(filename)) {
        _ClearSaveData(true);
        return false;
    }

    if(!file.DoesTableExist("save_game1")) {
        file.CloseFile();
        _ClearSaveData(true);
        return false;
    }

    // open the namespace that the save game is encapsulated in.
    file.OpenTable("save_game1");

    // The map file, tested after the save game is closed.
    // DEPRECATED: Old way, will be removed in one release.
    std::string map_script_filename;
    std::string map_data_filename;
    if (file.DoesStringExist("map_filename")) {
        map_script_filename = file.ReadString("map_filename");
        map_data_filename = file.ReadString("map_filename");
    }
    else {
        map_script_filename = file.ReadString("map_script_filename");
        map_data_filename = file.ReadString("map_data_filename");
    }

    // DEPRECATED: Remove in one release
    // Hack to permit the split of last map data and scripts.
    if (!map_script_filename.empty() && map_data_filename == map_script_filename) {
        std::string map_common_name = map_data_filename.substr(0, map_data_filename.length() - 4);
        map_data_filename = map_common_name + "_map.lua";
        map_script_filename = map_common_name + "_script.lua";
    }

    // Check whether the map data file is available
    if (map_data_filename.empty() || !vt_utils::DoesFileExist(map_data_filename)) {
        file.CloseTable(); // save_game1
        file.CloseFile();
        _ClearSaveData(true);
        return false;
    }

    // Used to store temp data to populate text boxes
    int32 hours = file.ReadInt("play_hours");
    int32 minutes = file.ReadInt("play_minutes");
    int32 seconds = file.ReadInt("play_seconds");
    int32 drunes = file.ReadInt("drunes");

    if(!file.DoesTableExist("characters")) {
        file.CloseTable(); // save_game1
        file.CloseFile();
        _ClearSaveData(true);
        return false;
    }

    file.OpenTable("characters");
    std::vector<uint32> char_ids;
    file.ReadUIntVector("order", char_ids);
    GlobalCharacter *character[4];

    // Loads only up to the first four slots (Visible battle characters)
    for(uint32 i = 0; i < 4 && i < char_ids.size(); ++i) {
        // Create a new GlobalCharacter object using the provided id
        // This loads all of the character's "static" data, such as their name, etc.
        character[i] = new GlobalCharacter(char_ids[i], false);

        if(!file.DoesTableExist(char_ids[i]))
            continue;

        file.OpenTable(char_ids[i]);

        // Read in all of the character's stats data
        character[i]->SetExperienceLevel(file.ReadUInt("experience_level"));
        character[i]->SetExperiencePoints(file.ReadUInt("experience_points"));

        character[i]->SetMaxHitPoints(file.ReadUInt("max_hit_points"));
        character[i]->SetHitPoints(file.ReadUInt("hit_points"));
        character[i]->SetMaxSkillPoints(file.ReadUInt("max_skill_points"));
        character[i]->SetSkillPoints(file.ReadUInt("skill_points"));

        file.CloseTable(); // character id
    }
    file.CloseTable(); // characters

    // Report any errors detected from the previous read operations
    if(file.IsErrorDetected()) {
        PRINT_WARNING << "One or more errors occurred while reading the save game file - they are listed below:"
            << std::endl << file.GetErrorMessages() << std::endl;
            file.ClearErrors();
    }

    file.CloseTable(); // save_game1
    file.CloseFile();

    for(uint32 i = 0; i < 4 && i < char_ids.size(); ++i) {
        _character_window[i].SetCharacter(character[i]);
    }

    std::ostringstream time_text;
    time_text << (hours < 10 ? "0" : "") << static_cast<uint32>(hours) << ":";
    time_text << (minutes < 10 ? "0" : "") << static_cast<uint32>(minutes) << ":";
    time_text << (seconds < 10 ? "0" : "") << static_cast<uint32>(seconds);
    _time_textbox.SetDisplayText(MakeUnicodeString(time_text.str()));

    std::ostringstream drunes_amount;
    drunes_amount << drunes;
    _drunes_textbox.SetDisplayText(MakeUnicodeString(drunes_amount.str()));

    // Test the map file

    // Tests the map file and gets the untranslated map hud name from it.
    ReadScriptDescriptor map_file;
    if(!map_file.OpenFile(map_script_filename)) {
        _ClearSaveData(true);
        return false;
    }

    if (map_file.OpenTablespace().empty()) {
        _ClearSaveData(true);
        map_file.CloseFile();
        return false;
    }

    // Read the in-game location of the save
    std::string map_hud_name = map_file.ReadString("map_name");
    _map_name_textbox.SetDisplayText(UTranslate(map_hud_name));

    // Loads the potential location image
    std::string map_image_filename = map_file.ReadString("map_image_filename");
    if (map_image_filename.empty()) {
        _location_image.Clear();
    }
    else {
        if (_location_image.Load(map_image_filename))
            _location_image.SetWidthKeepRatio(340.0f);
    }

    map_file.CloseTable(); // Tablespace
    map_file.CloseFile();

    return true;
} // bool SaveMode::_PreviewGame(string& filename)

bool SaveMode::_CheckSavesValidity() {
    // check all available slots
    bool available_saves = false;
    for (uint32 i = 0; i < _file_list.GetNumberOptions(); ++i) {
        if (!_PreviewGame(i)) {
            _file_list.EnableOption(i, false);

            // If the current selection is disabled, reset it.
            if ((int32)i == _file_list.GetSelection())
                _file_list.SetSelection(i + 1);
        }
        else {
            available_saves = true;
        }
    }

    return available_saves;
}

////////////////////////////////////////////////////////////////////////////////
// SmallCharacterWindow Class
////////////////////////////////////////////////////////////////////////////////

SmallCharacterWindow::~SmallCharacterWindow()
{
    delete _character;
}

void SmallCharacterWindow::SetCharacter(GlobalCharacter *character)
{
    delete _character;
    _character = character;

    if(character) {
        _portrait = character->GetPortrait();
        // Only size up valid portraits
        if(!_portrait.GetFilename().empty())
            _portrait.SetDimensions(100.0f, 100.0f);
    }
} // void SmallCharacterWindow::SetCharacter(GlobalCharacter *character)



// Draw the window to the screen
void SmallCharacterWindow::Draw()
{
    // Call parent Draw method, if failed pass on fail result
    MenuWindow::Draw();

    // check to see if this window is an actual character
    if(_character == NULL)
        return;

    if(_character->GetID() == vt_global::GLOBAL_CHARACTER_INVALID)
        return;

    // Get the window metrics
    float x, y, w, h;
    GetPosition(x, y);
    GetDimensions(w, h);
    // Adjust the current position to make it look better
    y -= 5;

    //Draw character portrait
    VideoManager->Move(x + 50, y + 110);
    _portrait.Draw();

    // Write character name
    VideoManager->MoveRelative(125, -75);
    VideoManager->Text()->Draw(_character->GetName(), TextStyle("title22"));

    // Level
    VideoManager->MoveRelative(0, 20);
    VideoManager->Text()->Draw(UTranslate("Lv: ") + MakeUnicodeString(NumberToString(_character->GetExperienceLevel())), TextStyle("text20"));

    // HP
    VideoManager->MoveRelative(0, 20);
    VideoManager->Text()->Draw(UTranslate("HP: ") + MakeUnicodeString(NumberToString(_character->GetHitPoints()) +
                               " / " + NumberToString(_character->GetMaxHitPoints())), TextStyle("text20"));

    // SP
    VideoManager->MoveRelative(0, 20);
    VideoManager->Text()->Draw(UTranslate("SP: ") + MakeUnicodeString(NumberToString(_character->GetSkillPoints()) +
                               " / " + NumberToString(_character->GetMaxSkillPoints())), TextStyle("text20"));

    return;
}

} // namespace vt_save
