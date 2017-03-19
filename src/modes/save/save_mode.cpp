////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
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

//! \name SaveMode States
//@{
const uint8_t SAVE_MODE_SAVING           = 0;
const uint8_t SAVE_MODE_LOADING          = 1;
const uint8_t SAVE_MODE_CONFIRM_AUTOSAVE = 2;
const uint8_t SAVE_MODE_CONFIRMING_SAVE  = 3;
const uint8_t SAVE_MODE_SAVE_COMPLETE    = 4;
const uint8_t SAVE_MODE_SAVE_FAILED      = 5;
const uint8_t SAVE_MODE_FADING_OUT       = 6;
const uint8_t SAVE_MODE_NO_VALID_SAVES   = 7;
//@}

SaveMode::SaveMode(bool save_mode, uint32_t x_position, uint32_t y_position) :
    GameMode(MODE_MANAGER_SAVE_MODE),
    _current_state(SAVE_MODE_LOADING),
    _dim_color(0.35f, 0.35f, 0.35f, 1.0f), // A grayish opaque color
    _x_position(x_position),
    _y_position(y_position),
    _save_mode(save_mode)
{
    _window.Create(600.0f, 500.0f);
    _window.SetPosition(212.0f, 138.0f);
    _window.Hide();

    _left_window.Create(150.0f, 500.0f);
    _left_window.SetPosition(212.0f, 138.0f);
    _left_window.Show();

    _title_window.Create(600.0f, 50.0f);
    _title_window.SetPosition(212.0f, 88.0f);
    _title_window.Show();

    const float centered_text_xpos = _window.GetXPosition() + 50.0f;
    const float centered_text_width = _window.GetWidth() - 100.f;

    // Initialize the save successful message box
    _title_textbox.SetPosition(centered_text_xpos, 103.0f);
    _title_textbox.SetDimensions(centered_text_width, 50.0f);
    _title_textbox.SetTextStyle(TextStyle("title22"));
    _title_textbox.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _title_textbox.SetTextAlignment(VIDEO_X_CENTER, VIDEO_Y_BOTTOM);
    if(_save_mode)
        _title_textbox.SetDisplayText(UTranslate("Save Game"));
    else
        _title_textbox.SetDisplayText(UTranslate("Load Game"));

    for(uint32_t i = 0; i < 4; ++i) {
        _character_window[i].Create(450.0f, 100.0f);
        _character_window[i].Show();
    }

    _character_window[0].SetPosition(355.0f, 138.0f);
    _character_window[1].SetPosition(355.0f, 238.0f);
    _character_window[2].SetPosition(355.0f, 338.0f);
    _character_window[3].SetPosition(355.0f, 438.0f);

    // Initialize the save options box
    _file_list.SetPosition(315.0f, 384.0f);
    _file_list.SetDimensions(150.0f, 500.0f,
                             1,
                             SystemManager->GetGameSaveSlots(),
                             1,
                             SystemManager->GetGameSaveSlots() > 6 ? 6 : SystemManager->GetGameSaveSlots());
    _file_list.SetTextStyle(TextStyle("title22"));

    _file_list.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _file_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _file_list.SetSelectMode(VIDEO_SELECT_SINGLE);
    _file_list.SetCursorOffset(-58.0f, -18.0f);

    // Initialize the confirmation option box
    _confirm_save_optionbox.SetPosition(512.0f, 384.0f);
    _confirm_save_optionbox.SetDimensions(centered_text_width, 200.0f, 1, 2, 1, 2);
    _confirm_save_optionbox.SetTextStyle(TextStyle("title22"));

    _confirm_save_optionbox.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _confirm_save_optionbox.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _confirm_save_optionbox.SetSelectMode(VIDEO_SELECT_SINGLE);
    _confirm_save_optionbox.SetCursorOffset(-58.0f, -18.0f);

    _confirm_save_optionbox.AddOption(UTranslate("Confirm Save"));
    _confirm_save_optionbox.AddOption(UTranslate("Cancel"));
    _confirm_save_optionbox.SetSelection(0);

    // Initialize the auto-save option box
    _load_auto_save_optionbox.SetPosition(512.0f, 384.0f);
    _load_auto_save_optionbox.SetDimensions(centered_text_width, 200.0f, 1, 3, 1, 3);
    _load_auto_save_optionbox.SetTextStyle(TextStyle("title22"));

    _load_auto_save_optionbox.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _load_auto_save_optionbox.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _load_auto_save_optionbox.SetSelectMode(VIDEO_SELECT_SINGLE);
    _load_auto_save_optionbox.SetCursorOffset(-58.0f, -18.0f);

    _load_auto_save_optionbox.AddOption(UTranslate("Load Auto-Save"));
    _load_auto_save_optionbox.AddOption(UTranslate("Load Save"));
    _load_auto_save_optionbox.AddOption(UTranslate("Cancel"));
    _load_auto_save_optionbox.SetSelection(0);

    // Initialize the save successful message box
    _save_success_message.SetPosition(centered_text_xpos, 314.0f);
    _save_success_message.SetDimensions(centered_text_width, 100.0f);
    _save_success_message.SetTextStyle(TextStyle("title22"));
    _save_success_message.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _save_success_message.SetTextAlignment(VIDEO_X_CENTER, VIDEO_Y_BOTTOM);
    _save_success_message.SetDisplayText(UTranslate("Save successful!"));

    // Initialize the save failure message box
    _save_failure_message.SetPosition(centered_text_xpos, 314.0f);
    _save_failure_message.SetDimensions(centered_text_width, 100.0f);
    _save_failure_message.SetTextStyle(TextStyle("title22"));
    _save_failure_message.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _save_failure_message.SetTextAlignment(VIDEO_X_CENTER, VIDEO_Y_BOTTOM);
    _save_failure_message.SetDisplayText(UTranslate("Unable to save game!\nSave FAILED!"));

    _no_valid_saves_message.SetPosition(centered_text_xpos, 314.0f);
    _no_valid_saves_message.SetDimensions(centered_text_width, 100.0f);
    _no_valid_saves_message.SetTextStyle(TextStyle("title22"));
    _no_valid_saves_message.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _no_valid_saves_message.SetTextAlignment(VIDEO_X_CENTER, VIDEO_Y_BOTTOM);
    _no_valid_saves_message.SetDisplayText(UTranslate("No valid saves found!"));

    // Initialize the save preview text boxes
    _map_name_textbox.SetPosition(600.0f, 558.0f);
    _map_name_textbox.SetDimensions(320.0f, 26.0f);
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
        for (uint32_t i = 0; i < SystemManager->GetGameSaveSlots(); ++i) {
            _file_list.AddOption(MakeUnicodeString(VTranslate("Slot %d", i + 1)));
        }
        // Restore the cursor position to the last load/save position.
        _file_list.SetSelection(GlobalManager->GetGameSlotId());

        _current_state = SAVE_MODE_SAVING;
    } else {
        _InitSaveSlots();
    }

    _window.Show();

    // Load the first slot data
    if(_file_list.GetSelection() > -1)
        _PreviewGame(_BuildSaveFilename(_file_list.GetSelection()));
}

SaveMode::~SaveMode()
{
    _window.Destroy();

    _left_window.Destroy();
    _title_window.Destroy();

    for(uint32_t i = 0; i < 4; ++i) {
        _character_window[i].Destroy();
    }

}

void SaveMode::Reset()
{
    // Save a copy of the current screen to use as the backdrop.
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
    if(InputManager->QuitPress()) {
        ModeManager->Pop();
        return;
    }

    // Screen is in the process of fading out, in order to load a game
    if(_current_state == SAVE_MODE_FADING_OUT) {
        return;
    }

    _file_list.Update();
    _confirm_save_optionbox.Update();
    _load_auto_save_optionbox.Update();

    GlobalMedia& media = GlobalManager->Media();

    // Otherwise, it's time to start handling events.
    if(InputManager->ConfirmPress()) {
        media.PlaySound("confirm");
        switch(_current_state) {
        case SAVE_MODE_SAVING:
            if(_file_list.GetSelection() > -1) {
                _current_state = SAVE_MODE_CONFIRMING_SAVE;
            }
            break;

        case SAVE_MODE_CONFIRMING_SAVE:
            if(_confirm_save_optionbox.GetSelection() == 0) {
                uint32_t id = static_cast<uint32_t>(_file_list.GetSelection());
                uint32_t stamina = MapMode::CurrentInstance() ?
                                   MapMode::CurrentInstance()->GetStamina() : 0;
                GlobalManager->SetSaveStamina(stamina);

                // Attempt to save the game
                if(GlobalManager->SaveGame(_BuildSaveFilename(id), id, _x_position, _y_position)) {
                    _current_state = SAVE_MODE_SAVE_COMPLETE;
                    AudioManager->PlaySound("data/sounds/save_successful_nick_bowler_oga.wav");
                    // Remove the autosave in that case.
                    _DeleteAutoSave(id);
                } else {
                    _current_state = SAVE_MODE_SAVE_FAILED;
                    AudioManager->PlaySound("data/sounds/cancel.wav");
                }
            } else {
                _current_state = SAVE_MODE_SAVING;
            }
            break;

        case SAVE_MODE_SAVE_COMPLETE:
        case SAVE_MODE_SAVE_FAILED:
            _current_state = SAVE_MODE_SAVING;
            _PreviewGame(_BuildSaveFilename(_file_list.GetSelection()));
            break;
        case SAVE_MODE_CONFIRM_AUTOSAVE:
            switch (_load_auto_save_optionbox.GetSelection()) {
            case 0: // Load autosave
                _LoadGame(_BuildSaveFilename(_file_list.GetSelection(), true));
                break;
            case 1: // Load save
                _LoadGame(_BuildSaveFilename(_file_list.GetSelection()));
                break;
            case 2: // Cancel
            default:
                _current_state = SAVE_MODE_LOADING;
            }
            break;
        case SAVE_MODE_LOADING:
            if(_file_list.GetSelection() > -1) {
                // Check whether a more recent autosave file exists
                uint32_t id = static_cast<uint32_t>(_file_list.GetSelection());
                if (_IsAutoSaveValid(id)) {
                    _current_state = SAVE_MODE_CONFIRM_AUTOSAVE;
                }
                else {
                    _LoadGame(_BuildSaveFilename(id));
                }
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
        }
    }
    else if(InputManager->CancelPress()) {
        media.PlaySound("cancel");
        switch(_current_state) {
        default:
        case SAVE_MODE_NO_VALID_SAVES:
        case SAVE_MODE_SAVING:
        case SAVE_MODE_LOADING:
            // Leave right away where there is nothing else to do
            ModeManager->Pop();
            break;
        case SAVE_MODE_CONFIRM_AUTOSAVE:
            _current_state = SAVE_MODE_LOADING;
            _PreviewGame(_BuildSaveFilename(_file_list.GetSelection()));
            break;
        case SAVE_MODE_CONFIRMING_SAVE:
            _current_state = SAVE_MODE_SAVING;
            _PreviewGame(_BuildSaveFilename(_file_list.GetSelection()));
            break;
        }
    }
    else if(InputManager->UpPress()) {
        media.PlaySound("bump");
        switch(_current_state) {
        case SAVE_MODE_SAVING:
        case SAVE_MODE_LOADING:
            _file_list.InputUp();
            if(_file_list.GetSelection() > -1) {
                _PreviewGame(_BuildSaveFilename(_file_list.GetSelection()));
            } else {
                _ClearSaveData(false);
            }
            break;

        case SAVE_MODE_CONFIRMING_SAVE:
            _confirm_save_optionbox.InputUp();
            break;
        case SAVE_MODE_CONFIRM_AUTOSAVE:
            _load_auto_save_optionbox.InputUp();
            break;
        }
    }
    else if(InputManager->DownPress()) {
        media.PlaySound("bump");
        switch(_current_state) {
        case SAVE_MODE_SAVING:
        case SAVE_MODE_LOADING:
            _file_list.InputDown();
            if(_file_list.GetSelection() > -1) {
                _PreviewGame(_BuildSaveFilename(_file_list.GetSelection()));
            }
            else {
                _ClearSaveData(false);
            }
            break;

        case SAVE_MODE_CONFIRMING_SAVE:
            _confirm_save_optionbox.InputDown();
            break;
        case SAVE_MODE_CONFIRM_AUTOSAVE:
            _load_auto_save_optionbox.InputDown();
            break;
        }
    }
}

void SaveMode::DrawPostEffects()
{
    // Set the coordinate system for the background and draw
    float width = _screen_capture.GetWidth();
    float height = _screen_capture.GetHeight();
    VideoManager->SetCoordSys(0, width, 0, height);
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);

    vt_video::DrawCapturedBackgroundImage(_screen_capture, 0.0f, 0.0f, _dim_color);

    // Reset the coordinate system for everything else
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
            for(uint32_t i = 0; i < 4; i++) {
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
    case SAVE_MODE_CONFIRM_AUTOSAVE:
        _load_auto_save_optionbox.Draw();
        break;
    default:
    case SAVE_MODE_FADING_OUT:
        break;
    }
}

bool SaveMode::_LoadGame(const std::string& filename)
{
    if(DoesFileExist(filename)) {
        _current_state = SAVE_MODE_FADING_OUT;
        AudioManager->StopActiveMusic();

        GlobalManager->LoadGame(filename, _file_list.GetSelection());

        // Create a new map mode, and fade out and in
        ModeManager->PopAll();
        try {
            // TODO: Save and restore stamina at load time
            MapMode *MM = new MapMode(GlobalManager->GetMapDataFilename(),
                                      GlobalManager->GetMapScriptFilename(),
                                      GlobalManager->GetSaveStamina(),
                                      false);
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
    for (uint32_t i = 0; i < 4; ++i)
        _character_window[i].SetCharacter(nullptr);
}


bool SaveMode::_PreviewGame(const std::string& filename)
{
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

    // DEPRECATED: Remove in one release
    // test whether the beginning of the filepath is 'dat/maps/' and replace with 'data/story/'
    if (map_data_filename.substr(0, 9) == "dat/maps/")
        map_data_filename = std::string("data/story/") + map_data_filename.substr(9, map_data_filename.length() - 9);
    if (map_script_filename.substr(0, 9) == "dat/maps/")
        map_script_filename = std::string("data/story/") + map_script_filename.substr(9, map_script_filename.length() - 9);

    // Check whether the map data file is available
    if (map_data_filename.empty() || !vt_utils::DoesFileExist(map_data_filename)) {
        file.CloseTable(); // save_game1
        file.CloseFile();
        _ClearSaveData(true);
        return false;
    }

    // Used to store temp data to populate text boxes
    int32_t hours = file.ReadInt("play_hours");
    int32_t minutes = file.ReadInt("play_minutes");
    int32_t seconds = file.ReadInt("play_seconds");
    int32_t drunes = file.ReadInt("drunes");

    if(!file.DoesTableExist("characters")) {
        file.CloseTable(); // save_game1
        file.CloseFile();
        _ClearSaveData(true);
        return false;
    }

    file.OpenTable("characters");
    std::vector<uint32_t> char_ids;
    file.ReadUIntVector("order", char_ids);
    GlobalCharacter *character[4];

    // Loads only up to the first four slots (Visible battle characters)
    for(uint32_t i = 0; i < 4 && i < char_ids.size(); ++i) {
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

    for(uint32_t i = 0; i < 4 && i < char_ids.size(); ++i) {
        _character_window[i].SetCharacter(character[i]);
    }

    std::ostringstream time_text;
    time_text << (hours < 10 ? "0" : "") << static_cast<uint32_t>(hours) << ":";
    time_text << (minutes < 10 ? "0" : "") << static_cast<uint32_t>(minutes) << ":";
    time_text << (seconds < 10 ? "0" : "") << static_cast<uint32_t>(seconds);
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
}

bool SaveMode::_IsAutoSaveValid(uint32_t id)
{
    std::string autosave_filename = _BuildSaveFilename(id, true);
    std::string save_filename = _BuildSaveFilename(id, false);
    if (!vt_utils::DoesFileExist(autosave_filename) || !vt_utils::DoesFileExist(save_filename))
        return false;

    // Check whether the autosave is strictly more recent than the save.
    if (vt_utils::GetFileModTime(autosave_filename) <= vt_utils::GetFileModTime(_BuildSaveFilename(id)))
        return false;

    // And check whether the autosave is valid.
    if (!_PreviewGame(autosave_filename))
        return false;

    return true;
}

void SaveMode::_InitSaveSlots()
{
    // Check all available slots for saves and autosaves.
    bool available_saves = false;

    // When in load mode, check the saves validity and skip invalid slots
    _file_list.SetSkipDisabled(true);

    for (uint32_t i = 0; i < SystemManager->GetGameSaveSlots(); ++i) {
        _file_list.AddOption(MakeUnicodeString("     " + VTranslate("Slot %d", i + 1)));

        // Add a key to slots with valid autosaves
        if (_IsAutoSaveValid(i)) {
            _file_list.AddOptionElementImage(i, GlobalManager->Media().GetKeyItemIcon());
            _file_list.GetEmbeddedImage(i)->SetHeightKeepRatio(25);
            _file_list.AddOptionElementPosition(i, 30);
        }

        if (!_PreviewGame(_BuildSaveFilename(i))) {
            _file_list.EnableOption(i, false);

            // If the current selection is disabled, reset it.
            if (static_cast<int32_t>(i) == _file_list.GetSelection())
                _file_list.SetSelection(i + 1);
        }
        else {
            available_saves = true;
        }
    }

    if (!available_saves)
        _current_state = SAVE_MODE_NO_VALID_SAVES;
}

std::string SaveMode::_BuildSaveFilename(uint32_t id, bool autosave)
{
    std::ostringstream file;
    file << GetUserDataPath() + "saved_game_" << id;
    if (autosave)
        file << "_autosave.lua";
    else
        file << ".lua";
    return file.str();
}

void SaveMode::_DeleteAutoSave(uint32_t id)
{
    std::string filename = _BuildSaveFilename(id, true);
    vt_utils::DeleteFile(filename);
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

    if(!_character || _character->GetID() == vt_global::GLOBAL_CHARACTER_INVALID) {
        _character_name.Clear();
        _character_data.Clear();
        _portrait = StillImage();
        return;
    }

    _portrait = character->GetPortrait();
    // Only size up valid portraits
    if(!_portrait.GetFilename().empty())
        _portrait.SetDimensions(100.0f, 100.0f);

    // the characters' name is already translated.
    _character_name.SetText(_character->GetName(), TextStyle("title22"));

    // And the rest of the data
    ustring char_data = UTranslate("Lv: ") + MakeUnicodeString(NumberToString(_character->GetExperienceLevel()) + "\n");
    char_data += UTranslate("HP: ") + MakeUnicodeString(NumberToString(_character->GetHitPoints()) +
                               " / " + NumberToString(_character->GetMaxHitPoints()) + "\n");
    char_data += UTranslate("SP: ") + MakeUnicodeString(NumberToString(_character->GetSkillPoints()) +
                               " / " + NumberToString(_character->GetMaxSkillPoints()));

    _character_data.SetText(char_data, TextStyle("text20"));
} // void SmallCharacterWindow::SetCharacter(GlobalCharacter *character)



// Draw the window to the screen
void SmallCharacterWindow::Draw()
{
    // Call parent Draw method, if failed pass on fail result
    MenuWindow::Draw();

    // Get the window metrics
    float x, y;
    GetPosition(x, y);
    // Adjust the current position to make it look better
    y -= 5;

    //Draw character portrait
    VideoManager->Move(x + 50, y + 110);
    _portrait.Draw();

    // Write character name
    VideoManager->MoveRelative(125, -70);
    _character_name.Draw();

    // Level, HP, SP
    VideoManager->MoveRelative(0, 20);
    _character_data.Draw();
}

} // namespace vt_save
