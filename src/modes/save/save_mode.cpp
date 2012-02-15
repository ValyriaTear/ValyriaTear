////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    save_mode.cpp
*** \author  Jacob Rudolph, rujasu@allacrost.org
*** \brief   Source file for save mode interface.
*** ***************************************************************************/

#include <iostream>
#include <sstream>

#include "audio.h"
#include "video.h"
#include "script.h"
#include "input.h"
#include "system.h"
#include "boot.h"
#include "map.h"
#include "mode_manager.h"

#include "save_mode.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_mode_manager;
using namespace hoa_input;
using namespace hoa_system;
using namespace hoa_boot;
using namespace hoa_global;
using namespace hoa_map;
using namespace hoa_script;

namespace hoa_save {

bool SAVE_DEBUG = false;

//! \name Save Options Constants
//@{
const uint8 SAVE_GAME           = 0;
const uint8 SAVE_LOAD_GAME      = 1;
const uint8 SAVE_CANCEL         = 2;
//@}

//! \name SaveMode States
//@{
const uint8 SAVE_MODE_NORMAL          = 0;
const uint8 SAVE_MODE_SAVING          = 1;
const uint8 SAVE_MODE_LOADING         = 2;
const uint8 SAVE_MODE_CONFIRMING_SAVE = 3;
const uint8 SAVE_MODE_SAVE_COMPLETE   = 4;
const uint8 SAVE_MODE_SAVE_FAILED     = 5;
const uint8 SAVE_MODE_FADING_OUT      = 6;
//@}

SaveMode::SaveMode(bool enable_saving) :
	GameMode(),
	_current_state(SAVE_MODE_NORMAL),
	_dim_color(0.35f, 0.35f, 0.35f, 1.0f), // A grayish opaque color
	_saving_enabled(enable_saving)
{
	_location_name = MakeUnicodeString("");

	mode_type = MODE_MANAGER_SAVE_MODE;

	_window.Create(600.0f, 500.0f);
	_window.SetPosition(212.0f, 630.0f);
	_window.SetDisplayMode(VIDEO_MENU_EXPAND_FROM_CENTER);
	_window.Hide();

	_left_window.Create(150.0f, 500.0f);
	_left_window.SetPosition(212.0f, 630.0f);
	_left_window.SetDisplayMode(VIDEO_MENU_EXPAND_FROM_CENTER);
	_left_window.Show();

	for (int i = 0; i < 4; i++) {
		_character_window[i].Create(450.0f, 100.0f);
		_character_window[i].SetDisplayMode(VIDEO_MENU_EXPAND_FROM_CENTER);
		_character_window[i].Show();
	}

	_character_window[0].SetPosition(355.0f, 630.0f);
	_character_window[1].SetPosition(355.0f, 530.0f);
	_character_window[2].SetPosition(355.0f, 430.0f);
	_character_window[3].SetPosition(355.0f, 330.0f);

	// Initialize the save options box
	_save_options.SetPosition(512.0f, 384.0f);
	_save_options.SetDimensions(250.0f, 200.0f, 1, 3, 1, 3);
	_save_options.SetTextStyle(TextStyle("title22"));

	_save_options.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_save_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_save_options.SetSelectMode(VIDEO_SELECT_SINGLE);
	_save_options.SetCursorOffset(-58.0f, 18.0f);

	_save_options.AddOption(UTranslate("Save Game"));
	_save_options.AddOption(UTranslate("Load Game"));
	_save_options.AddOption(UTranslate("Cancel"));
	_save_options.SetSelection(SAVE_CANCEL);

	_save_options.SetSkipDisabled(true);

	// Initialize the save options box
	_file_list.SetPosition(315.0f, 384.0f);
	_file_list.SetDimensions(150.0f, 500.0f, 1, 7, 1, 7);
	_file_list.SetTextStyle(TextStyle("title22"));

	_file_list.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_file_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_file_list.SetSelectMode(VIDEO_SELECT_SINGLE);
	_file_list.SetCursorOffset(-58.0f, 18.0f);

	_file_list.AddOption(UTranslate("Cancel"));
	_file_list.AddOption(UTranslate("File #1"));
	_file_list.AddOption(UTranslate("File #2"));
	_file_list.AddOption(UTranslate("File #3"));
	_file_list.AddOption(UTranslate("File #4"));
	_file_list.AddOption(UTranslate("File #5"));
	_file_list.AddOption(UTranslate("File #6"));
	_file_list.SetSelection(0);

	// Initialize the confirmation option box
	_confirm_save_optionbox.SetPosition(512.0f, 284.0f);
	_confirm_save_optionbox.SetDimensions(250.0f, 500.0f, 1, 7, 1, 7);
	_confirm_save_optionbox.SetTextStyle(TextStyle("title22"));

	_confirm_save_optionbox.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_confirm_save_optionbox.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_confirm_save_optionbox.SetSelectMode(VIDEO_SELECT_SINGLE);
	_confirm_save_optionbox.SetCursorOffset(-58.0f, 18.0f);

	_confirm_save_optionbox.AddOption(UTranslate("Confirm Save"));
	_confirm_save_optionbox.AddOption(UTranslate("Cancel"));
	_confirm_save_optionbox.SetSelection(0);

	// Initialize the save successful message box
	_save_success_message.SetPosition(512.0f, 384.0f);
	_save_success_message.SetDimensions(250.0f, 100.0f);
	_save_success_message.SetTextStyle(TextStyle("title22"));
	_save_success_message.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_save_success_message.SetDisplayText("Save successful!");

	// Initialize the save failure message box
	_save_failure_message.SetPosition(512.0f, 384.0f);
	_save_failure_message.SetDimensions(250.0f, 100.0f);
	_save_failure_message.SetTextStyle(TextStyle("title22"));
	_save_failure_message.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_save_failure_message.SetDisplayText("Unable to save game!\nSave FAILED!");

	// Initialize the save preview text boxes
	_location_name_textbox.SetPosition(600.0f, 215.0f);
	_location_name_textbox.SetDimensions(250.0f, 26.0f);
	_location_name_textbox.SetTextStyle(TextStyle("title22"));
	_location_name_textbox.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_location_name_textbox.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_location_name_textbox.SetDisplayText(" ");

	_time_textbox.SetPosition(600.0f, 185.0f);
	_time_textbox.SetDimensions(250.0f, 26.0f);
	_time_textbox.SetTextStyle(TextStyle("title22"));
	_time_textbox.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_time_textbox.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_time_textbox.SetDisplayText(" ");

	_drunes_textbox.SetPosition(600.0f, 155.0f);
	_drunes_textbox.SetDimensions(250.0f, 26.0f);
	_drunes_textbox.SetTextStyle(TextStyle("title22"));
	_drunes_textbox.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_drunes_textbox.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_drunes_textbox.SetDisplayText(" ");

	hoa_gui::TextBox _location_name_textbox;
		hoa_gui::TextBox _time_textbox;
		hoa_gui::TextBox _drunes_textbox;

	if (_saving_enabled == false) {
		_save_options.EnableOption(SAVE_GAME, false);
	}

	if (_save_music.LoadAudio("mus/Save_Game.ogg") == false) {
		PRINT_ERROR << "failed to load save/load music file: " << endl;
		SystemManager->ExitGame();
		return;
	}

	_window.Show();
}



SaveMode::~SaveMode() {
	_window.Destroy();

	_left_window.Destroy();

	for (int i = 0; i < 4; i++) {
		_character_window[i].Destroy();
	}

}



void SaveMode::Reset() {
	// Save a copy of the current screen to use as the backdrop
	try {
		_screen_capture = VideoManager->CaptureScreen();
	}
	catch(Exception e) {
		IF_PRINT_WARNING(SAVE_DEBUG) << e.ToString() << endl;
	}

	VideoManager->SetCoordSys(0.0f, 1023.0f, 0.0f, 767.0f);
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);

	_save_music.Play();
}



void SaveMode::Update() {
	if (InputManager->QuitPress() == true) {
		ModeManager->Pop();
		return;
	}

	_save_options.Update();
	_file_list.Update();

	// Screen is in the process of fading out, in order to load a game
	if (_current_state == SAVE_MODE_FADING_OUT)
	{
		// When the screen is finished fading to black, create a new map mode and fade back in
		if (!VideoManager->IsFading()) {
			ModeManager->PopAll();
			try {
				MapMode *MM = new MapMode(MakeStandardString(GlobalManager->GetLocationName()));
				ModeManager->Push(MM);
			} catch (luabind::error e) {
				PRINT_ERROR << "Map::_Load -- Error loading map " << MakeStandardString(GlobalManager->GetLocationName()) << ", returning to BootMode." << endl;
				cerr << "Exception message:" << endl;
				ScriptManager->HandleLuaError(e);
				ModeManager->Push(new BootMode());
			}
			VideoManager->FadeScreen(Color::clear, 1000);
		}
		return;
	}
	// Otherwise, it's time to start handling events.
	else if (InputManager->ConfirmPress() == true) {
		switch (_current_state) {
			case SAVE_MODE_NORMAL:
				if (_save_options.GetSelection() == SAVE_GAME && _saving_enabled) {
					_current_state = SAVE_MODE_SAVING;
				}
				else if (_save_options.GetSelection() == SAVE_LOAD_GAME) {
					_current_state = SAVE_MODE_LOADING;
				}
				else {
					ModeManager->Pop();
				}
				break;

			case SAVE_MODE_SAVING:
				if (_file_list.GetSelection() > 0) {
					_current_state = SAVE_MODE_CONFIRMING_SAVE;
				}
				else {
					_current_state = SAVE_MODE_NORMAL;
				}
				break;

			case SAVE_MODE_CONFIRMING_SAVE:
				if (_confirm_save_optionbox.GetSelection() == 0) {
					// note: using int here, because uint8 will NOT work
					// do not change unless you understand this and can test it properly!
					int id = _file_list.GetSelection();
					ostringstream f;
					f << GetUserDataPath(true) + "saved_game_" << id << ".lua";
					string filename = f.str();
					// now, attempt to save the game.  If failure, we need to tell the user that!
					if (GlobalManager->SaveGame(filename) == true) {
						_current_state = SAVE_MODE_SAVE_COMPLETE;
					}
					else {
						_current_state = SAVE_MODE_SAVE_FAILED;
					}
				}
				else {
					_current_state = SAVE_MODE_SAVING;
				}
				break;

			case SAVE_MODE_SAVE_COMPLETE: case SAVE_MODE_SAVE_FAILED:
				_current_state = SAVE_MODE_SAVING;
				_PreviewGame( _file_list.GetSelection() );
				break;

			case SAVE_MODE_LOADING:
				if (_file_list.GetSelection() > 0) {
					_LoadGame( _file_list.GetSelection() );
				}
				else {
					_current_state = SAVE_MODE_NORMAL;
				}
				break;
		} // end switch (_current_state)
	} // end if (InputManager->ConfirmPress() == true)

	else if (InputManager->CancelPress() == true) {
		switch (_current_state) {
			case SAVE_MODE_NORMAL:
				ModeManager->Pop();
				return;

			case SAVE_MODE_SAVING: case SAVE_MODE_LOADING:
				_current_state = SAVE_MODE_NORMAL;
				break;

			case SAVE_MODE_CONFIRMING_SAVE:
				_current_state = SAVE_MODE_SAVING;
				_PreviewGame( _file_list.GetSelection() );
				break;
		} // end switch (_current_state)
	} // end if (InputManager->CancelPress() == true)

	else if (InputManager->UpPress() == true) {
		switch (_current_state) {
			case SAVE_MODE_NORMAL:
				_save_options.InputUp();
				return;

			case SAVE_MODE_SAVING: case SAVE_MODE_LOADING:
				_file_list.InputUp();
				if (_file_list.GetSelection() > 0) {
					_PreviewGame( _file_list.GetSelection() );
				}
				else {
					_location_name_textbox.SetDisplayText(" ");
					_time_textbox.SetDisplayText(" ");
					_drunes_textbox.SetDisplayText(" ");
				}
				break;

			case SAVE_MODE_CONFIRMING_SAVE:
				_confirm_save_optionbox.InputUp();
				break;
		} // end switch (_current_state)
	} // end if (InputManager->UpPress() == true)

	else if (InputManager->DownPress() == true) {
		switch (_current_state) {
			case SAVE_MODE_NORMAL:
				_save_options.InputDown();
				return;

			case SAVE_MODE_SAVING: case SAVE_MODE_LOADING:
				_file_list.InputDown();
				if (_file_list.GetSelection() > 0) {
					_PreviewGame( _file_list.GetSelection() );
				}
				break;

			case SAVE_MODE_CONFIRMING_SAVE:
				_confirm_save_optionbox.InputDown();
				break;
		} // end switch (_current_state)
	} // end if (InputManager->DownPress() == true)
} // void SaveMode::Update()



void SaveMode::Draw() {
	// Set the coordinate system for the background and draw
	float width = _screen_capture.GetWidth();
	float height = _screen_capture.GetHeight();
	VideoManager->SetCoordSys(0, width, 0, height);
	VideoManager->Move(0.0f, 0.0f);
	_screen_capture.Draw(_dim_color);

	// Re-set the coordinate system for everything else
	VideoManager->SetCoordSys(0.0f, 1023.0f, 0.0f, 767.0f);

	_window.Draw();

	switch (_current_state) {
		case SAVE_MODE_NORMAL:
			_save_options.Draw();
			break;
		case SAVE_MODE_SAVING:
		case SAVE_MODE_LOADING:
			_left_window.Draw(); // draw a panel on the left for the file list
			if (_file_list.GetSelection() > 0) {
				for (uint32 i = 0; i < 4; i++) {
					_character_window[i].Draw();
				}
			}
			_file_list.Draw();
			_location_name_textbox.Draw();
			_time_textbox.Draw();
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
		case SAVE_MODE_FADING_OUT:

			break;
	}
}

bool SaveMode::_LoadGame(int id) {
	ostringstream f;
	f << GetUserDataPath(true) + "saved_game_" << id << ".lua";
	string filename = f.str();

	if (DoesFileExist(filename)) {
		_current_state = SAVE_MODE_FADING_OUT;
		VideoManager->FadeScreen(Color::black, 1000);
		// TODO: stop music, if it's playing

		GlobalManager->LoadGame(filename);

		return true;
	}
	else {
		cout << "BOOT: No saved game file exists, can not load game" << endl;
		cout << filename << endl;
		return false;
	}
}


bool SaveMode::_PreviewGame(int id) {
	ostringstream f;
	f << GetUserDataPath(true) + "saved_game_" << id << ".lua";
	string filename = f.str();

	ReadScriptDescriptor file, map_file;

	if (file.OpenFile(filename, true) == false) {
		_location_name_textbox.SetDisplayText("No Data");
		_time_textbox.SetDisplayText(" ");
		_drunes_textbox.SetDisplayText(" ");
		for (uint32 i = 0; i < 4; i++) {
			_character_window[i].SetCharacter(NULL);
		}
		return false;
	}

	// open the namespace that the save game is encapsulated in.
	file.OpenTable("save_game1");

	string location_filename = file.ReadString("location_name");

	// using ints to store temp data to populate text boxes
	int hours, minutes, seconds, drunes;
	hours = file.ReadInt("play_hours");
	minutes = file.ReadInt("play_minutes");
	seconds = file.ReadInt("play_seconds");
	drunes = file.ReadInt("drunes");


	file.OpenTable("characters");
	vector<uint32> char_ids;
	file.ReadUIntVector("order", char_ids);
	GlobalCharacter* character[4];

	for (uint32 i = 0; i < char_ids.size(); i++) {
		// Create a new GlobalCharacter object using the provided id
		// This loads all of the character's "static" data, such as their name, etc.
		character[i] = new GlobalCharacter(char_ids[i], false);

		file.OpenTable(char_ids[i]);

		// Read in all of the character's stats data
		character[i]->SetExperienceLevel(file.ReadUInt("experience_level"));
		character[i]->SetExperiencePoints(file.ReadUInt("experience_points"));

		character[i]->SetMaxHitPoints(file.ReadUInt("max_hit_points"));
		character[i]->SetHitPoints(file.ReadUInt("hit_points"));
		character[i]->SetMaxSkillPoints(file.ReadUInt("max_skill_points"));
		character[i]->SetSkillPoints(file.ReadUInt("skill_points"));

		file.CloseTable();
	}
	file.CloseTable();


	// Report any errors detected from the previous read operations
	if (file.IsErrorDetected()) {
		if (GLOBAL_DEBUG) {
			PRINT_WARNING << "one or more errors occurred while reading the save game file - they are listed below" << endl;
			cerr << file.GetErrorMessages() << endl;
			file.ClearErrors();
		}
	}

	file.CloseFile();

	// Load map file to get location name
	if (map_file.OpenFile(location_filename) == false) {
		return false;
	}

	// Determine the map's tablespacename and then open it. The tablespace is the name of the map file without
	// file extension or path information (for example, 'dat/maps/demo.lua' has a tablespace name of 'demo').
	int32 period = location_filename.find(".");
	int32 last_slash = location_filename.find_last_of("/");
	string map_tablespace = location_filename.substr(last_slash + 1, period - (last_slash + 1));
	map_file.OpenTable(map_tablespace);

	// Read the name of the map
	ustring location_name = MakeUnicodeString(map_file.ReadString("map_name"));

	map_file.CloseTable();
	map_file.CloseFile();


	for (uint32 i = 0; i < 4; i++) {
		_character_window[i].SetCharacter(character[i]);
	}

	_location_name_textbox.SetDisplayText(location_name);

	std::ostringstream time_text;
	time_text << "Time - ";
	time_text << (hours < 10 ? "0" : "") << static_cast<uint32>(hours) << ":";
	time_text << (minutes < 10 ? "0" : "") << static_cast<uint32>(minutes) << ":";
	time_text << (seconds < 10 ? "0" : "") << static_cast<uint32>(seconds);

	_time_textbox.SetDisplayText( time_text.str() );

	std::ostringstream drunes_text;
	drunes_text << "Drunes - " << drunes;

	_drunes_textbox.SetDisplayText( drunes_text.str() );
	return true;
} // bool SaveMode::_PreviewGame(string& filename)




////////////////////////////////////////////////////////////////////////////////
// SmallCharacterWindow Class
////////////////////////////////////////////////////////////////////////////////

SmallCharacterWindow::SmallCharacterWindow() : _character(NULL) {
}



SmallCharacterWindow::~SmallCharacterWindow() {
}



void SmallCharacterWindow::SetCharacter(GlobalCharacter *character) {
	_character = character;

	if (character != NULL) {
		_portrait.SetStatic(true);
		_portrait.Load("img/portraits/menu/" + character->GetFilename() + "_small.png", 100, 100);
	}
} // void SmallCharacterWindow::SetCharacter(GlobalCharacter *character)



// Draw the window to the screen
void SmallCharacterWindow::Draw() {
	// Call parent Draw method, if failed pass on fail result
	MenuWindow::Draw();

	// check to see if this window is an actual character
	if (_character == NULL)
		return;

	if (_character->GetID() == hoa_global::GLOBAL_CHARACTER_INVALID)
		return;

//	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);

	// Get the window metrics
	float x, y, w, h;
	GetPosition(x,y);
	GetDimensions(w,h);
	// Adjust the current position to make it look better
	y += 5;

	//Draw character portrait
	VideoManager->Move(x + 50, y - 110);
	_portrait.Draw();

	// Write character name
	VideoManager->MoveRelative(125, 75);
	VideoManager->Text()->Draw(_character->GetName(), TextStyle("title22"));

	// Level
	VideoManager->MoveRelative(0,-20);
	VideoManager->Text()->Draw(UTranslate("Lv: ") + MakeUnicodeString(NumberToString(_character->GetExperienceLevel())), TextStyle("text20"));

	// HP
	VideoManager->MoveRelative(0,-20);
	VideoManager->Text()->Draw(UTranslate("HP: ") + MakeUnicodeString(NumberToString(_character->GetHitPoints()) +
		" / " + NumberToString(_character->GetMaxHitPoints())), TextStyle("text20"));

	// SP
	VideoManager->MoveRelative(0,-20);
	VideoManager->Text()->Draw(UTranslate("SP: ") + MakeUnicodeString(NumberToString(_character->GetSkillPoints()) +
		" / " + NumberToString(_character->GetMaxSkillPoints())), TextStyle("text20"));

	return;
}

} // namespace hoa_save
