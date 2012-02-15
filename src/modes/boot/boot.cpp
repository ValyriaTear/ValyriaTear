///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    boot.cpp
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \brief   Source file for boot mode interface.
*** ***************************************************************************/

#include <iostream>
#include <sstream>

#include "audio.h"
#include "script.h"
#include "input.h"
#include "system.h"

#include "global.h"

#include "boot.h"
#include "boot_credits.h"
#include "boot_welcome.h"

#include "map.h"
#include "save_mode.h"
// Files below are included temporarily, used for boot mode to do a test launch of other modes
#include "battle.h"
#include "menu.h"
#include "shop.h"

using namespace std;
using namespace hoa_utils;

using namespace hoa_audio;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_input;
using namespace hoa_mode_manager;
using namespace hoa_script;
using namespace hoa_system;

using namespace hoa_global;

using namespace hoa_boot::private_boot;

namespace hoa_boot {

bool BOOT_DEBUG = false;

// A temporary hack to boot mode used to make it possible to enter battle mode, menu mode, and
// shop mode from the main boot menu. Set this member to true to enable those options or false
// to disable them. Make sure to set this boolean to false for release builds!
bool TEMP_BOOT_TEST = true;

// Initialize static members here
bool BootMode::_initial_entry = true;
BootMode* BootMode::_current_instance = NULL;

// ****************************************************************************
// ***** BootMode public methods
// ****************************************************************************

BootMode::BootMode() :
	_fade_out(false),
	_has_modified_settings(false),
	_key_setting_function(NULL),
	_joy_setting_function(NULL),
	_joy_axis_setting_function(NULL),
	_overwrite_function(NULL),
	_message_window(ustring(), 210.0f, 35.0f),
	_file_name_alert(ustring(),300.0f,35.0f),
	_file_name_window(ustring(),150.0f,35.0f)
{
	IF_PRINT_DEBUG(BOOT_DEBUG) << "BootMode constructor invoked" << endl;
	mode_type = MODE_MANAGER_BOOT_MODE;

	_credits_window = new CreditsWindow();
	_welcome_window = new WelcomeWindow();

	_version_text.SetStyle(TextStyle("text20"));
	_version_text.SetText(UTranslate("Development Release"));
	_copyright_text.SetStyle(TextStyle("text20"));
	_copyright_text.SetText(UTranslate("© 2004 — 2011 The Allacrost Project"));

	ReadScriptDescriptor read_data;
	if (!read_data.OpenFile("dat/config/boot.lua")) {
		PRINT_ERROR << "failed to load boot data file" << endl;
	}

	// Load all bitmaps using this StillImage
	StillImage im;
	bool success = true;

	success &= im.Load(read_data.ReadString("background_image"), read_data.ReadFloat("background_image_width"), read_data.ReadFloat("background_image_height"));
	_boot_images.push_back(im);

	success &= im.Load(read_data.ReadString("logo_background"), read_data.ReadFloat("logo_background_width"), read_data.ReadFloat("logo_background_height"));
	_boot_images.push_back(im);

	success &= im.Load(read_data.ReadString("logo_sword"), read_data.ReadFloat("logo_sword_width"), read_data.ReadFloat("logo_sword_height"));
	_boot_images.push_back(im);

	success &= im.Load(read_data.ReadString("logo_text"), read_data.ReadFloat("logo_text_width"), read_data.ReadFloat("logo_text_height"));
	_boot_images.push_back(im);

	if (success == false) {
		PRINT_ERROR << "failed to load one or more boot images" << endl;
	}

	// Load audio data
	vector<string> new_music_files;
	vector<string> new_sound_files;
	read_data.ReadStringVector("music_files", new_music_files);
	read_data.ReadStringVector("sound_files", new_sound_files);
	if (read_data.IsErrorDetected()) {
		PRINT_ERROR << "an error occured during reading of the boot data file" << endl;
		PRINT_ERROR << read_data.GetErrorMessages() << endl;
	}
	read_data.CloseFile();

	_boot_music.resize(new_music_files.size(), MusicDescriptor());
	for (uint32 i = 0; i < new_music_files.size(); i++) {
		if (_boot_music[i].LoadAudio(new_music_files[i]) == false) {
			PRINT_ERROR << "failed to load music file: " << new_music_files[i] << endl;
			SystemManager->ExitGame();
			return;
		}
	}

	_boot_sounds.resize(new_sound_files.size(), SoundDescriptor());
	for (uint32 i = 0; i < new_sound_files.size(); i++) {
		if (_boot_sounds[i].LoadAudio(new_sound_files[i]) == false) {
			PRINT_ERROR << "failed to load sound file: " << new_sound_files[i] << endl;
			SystemManager->ExitGame();
			return;
		}
	}

	_options_window.Create(300.0f, 550.0f);
	_options_window.SetPosition(360.0f, 580.0f);
	_options_window.SetDisplayMode(VIDEO_MENU_INSTANT);
	_options_window.Hide();

/*
	if (!_is_windowed) // without a window
	{
		_active_menu.SetTextStyle(TextStyle("title22"));
// 		_active_menu.SetCellSize(150.0f, 70.0f);
		_active_menu.SetPosition(552.0f, 50.0f);
		_active_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
		_active_menu.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
		_active_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
		_active_menu.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
		_active_menu.SetCursorOffset(-50.0f, 28.0f);
// 		_active_menu.SetSize(_active_menu.GetNumberOptions(), 1);
	}
	else // windowed
	{
		_active_menu.SetTextStyle(TextStyle("title22"));
// 		_active_menu.SetCellSize(210.0f, 50.0f);
		_active_menu.SetPosition(150.0f, 200.0f);
		_active_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
		_active_menu.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
		_active_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
		_active_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
		_active_menu.SetCursorOffset(-50.0f, 28.0f);
// 		_active_menu.SetSize(1, _active_menu.GetNumberOptions());
		_active_menu.SetOwner(_options_window);
	}
*/

	// Setup all boot menu options and properties
	_SetupMainMenu();
	_SetupOptionsMenu();
	_SetupVideoOptionsMenu();
	_SetupAudioOptionsMenu();
	_SetupLanguageOptionsMenu();
	_SetupKeySetttingsMenu();
	_SetupJoySetttingsMenu();
	_SetupResolutionMenu();
	_SetupProfileMenu();
	_SetupLoadProfileMenu();
	_SetupSaveProfileMenu();
	_SetupDeleteProfileMenu();
	_SetupUserInputMenu();
	_active_menu = &_main_menu;

	// make sure message window is not visible
	_message_window.Hide();
	_file_name_alert.Hide();
	_file_name_window.Hide();
} // BootMode::BootMode()



BootMode::~BootMode() {
	delete _credits_window;
	delete _welcome_window;

	_options_window.Destroy();
	_SaveSettingsFile("");

	if (BOOT_DEBUG) cout << "BOOT: BootMode destructor invoked." << endl;

	for (uint32 i = 0; i < _boot_music.size(); i++)
		_boot_music[i].FreeAudio();

	for (uint32 i = 0; i < _boot_sounds.size(); i++)
		_boot_sounds[i].FreeAudio();

	_key_setting_function = NULL;
	_joy_setting_function = NULL;
	_joy_axis_setting_function = NULL;
	_overwrite_function = NULL;
}



void BootMode::Reset() {
	// Set the coordinate system that BootMode uses
	VideoManager->SetCoordSys(0.0f, 1023.0f, 0.0f, 767.0f);
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);

	GlobalManager->ClearAllData(); // Resets the game universe to a NULL state
	_current_instance = this;

	// Decide which music track to play
	if (_initial_entry == true)
		_boot_music.at(1).Play(); // Opening Effect
	else
		_boot_music.at(0).Play(); // Main theme
}



void BootMode::Update() {
	_options_window.Update(SystemManager->GetUpdateTime());
	if (InputManager->QuitPress() == true) {
		SystemManager->ExitGame();
		return;
	}

	// Screen is in the process of fading out
	if (_fade_out)
	{
		// When the screen is finished fading to black, create a new map mode and fade back in
		if (!VideoManager->IsFading()) {
			ModeManager->Pop();
			try {
				hoa_map::MapMode *MM = new hoa_map::MapMode(MakeStandardString(GlobalManager->GetLocationName()));
				ModeManager->Push(MM);
			} catch (luabind::error e) {
				PRINT_ERROR << "Map::_Load -- Error loading map " << MakeStandardString(GlobalManager->GetLocationName()) << ", returning to BootMode." << endl;
				cerr << "Exception message:" << endl;
				ScriptManager->HandleLuaError(e);
			}
			VideoManager->FadeScreen(Color::clear, 1000);
		}
		return;
	}
	else if (_initial_entry) // We're animating the opening logo
	{
		if (InputManager->AnyKeyPress()) // Check if we want to skip the demo
		{
			_EndLogoAnimation();
			return;
		}
		else
		{
			return; // Otherwise skip rest of the event handling for now
		}
	}

	// Update the credits window (because it may be hiding/showing!)
	_credits_window->Update();

	//CD: Handle key press here, just like any other time
	if (_welcome_window->IsActive())
	{
		if (InputManager->AnyKeyPress())
		{
			_boot_sounds.at(0).Play();
			_welcome_window->Hide();

			// save the settings (automatically changes the welcome variable to 0
			_has_modified_settings = true;
			_SaveSettingsFile("");
		}

		return;
	}

	// Check for waiting keypresses or joystick button presses
	SDL_Event ev = InputManager->GetMostRecentEvent();
	if (_joy_setting_function != NULL)
	{
		if (InputManager->AnyKeyPress() && ev.type == SDL_JOYBUTTONDOWN)
		{
			(this->*_joy_setting_function)(InputManager->GetMostRecentEvent().jbutton.button);
			_joy_setting_function = NULL;
			_has_modified_settings = true;
			_RefreshJoySettings();
			_message_window.Hide();
		}
		if (InputManager->CancelPress())
		{
			_joy_setting_function = NULL;
			_message_window.Hide();
		}
		return;
	}

	if (_joy_axis_setting_function != NULL)
	{
		int8 x = InputManager->GetLastAxisMoved();
		if (x != -1)
		{
			(this->*_joy_axis_setting_function)(x);
			_joy_axis_setting_function = NULL;
			_has_modified_settings = true;
			_RefreshJoySettings();
			_message_window.Hide();
		}
		if (InputManager->CancelPress())
		{
			_joy_axis_setting_function = NULL;
			_message_window.Hide();
		}
		return;
	}

	if (_key_setting_function != NULL)
	{
		if (InputManager->AnyKeyPress() && ev.type == SDL_KEYDOWN)
		{
			(this->*_key_setting_function)(InputManager->GetMostRecentEvent().key.keysym.sym);
			_key_setting_function = NULL;
			_has_modified_settings = true;
			_RefreshKeySettings();
			_message_window.Hide();
		}
		if (InputManager->CancelPress())
		{
			_key_setting_function = NULL;
			_message_window.Hide();
		}
		return;
	}

	if(_overwrite_function != NULL)
	{
		if(InputManager->ConfirmPress())
		{
			(this->*_overwrite_function)();
			_overwrite_function = NULL;
			_file_name_alert.Hide();
		}
		else if(InputManager->CancelPress())
		{
			_overwrite_function = NULL;
			_file_name_alert.Hide();
		}
		//dont want to execute the confirm command on my menu selection for a second time!
		return;
	}

	_active_menu->Update();

	// A confirm-key was pressed -> handle it (but ONLY if the credits screen isn't visible)
	if (InputManager->ConfirmPress() && !_credits_window->IsActive())
	{
		// Play 'confirm sound' if the selection isn't grayed out and it has a confirm handler
		if (_active_menu->IsEnabled(_active_menu->GetSelection()))
			_boot_sounds.at(0).Play();
		else
			_boot_sounds.at(3).Play(); // Otherwise play a different sound

		_active_menu->InputConfirm();

	}
	else if (InputManager->LeftPress() && !_credits_window->IsActive())
	{
		_active_menu->InputLeft();
	}
	else if(InputManager->RightPress() && !_credits_window->IsActive())
	{
		_active_menu->InputRight();
	}
	else if(InputManager->UpPress() && !_credits_window->IsActive())
	{
		_active_menu->InputUp();
	}
	else if(InputManager->DownPress() && !_credits_window->IsActive())
	{
		_active_menu->InputDown();
	}
	else if(InputManager->CancelPress())
	{
		// Close the credits-screen if it was visible
		if (_credits_window->IsActive())
		{
			_credits_window->Hide();
			_boot_sounds.at(1).Play(); // Play cancel sound here as well
		}
		else if (_active_menu == &_main_menu) {

		}
		else if (_active_menu == &_options_menu) {
			_options_window.Hide();
			_active_menu = &_main_menu;
		}
		else if (_active_menu == &_video_options_menu) {
			_active_menu = &_options_menu;
		}
		else if (_active_menu == &_audio_options_menu) {
			_active_menu = &_options_menu;
		}
		else if (_active_menu == &_language_options_menu) {
			_active_menu = &_options_menu;
		}
		else if (_active_menu == &_key_settings_menu) {
			_active_menu = &_options_menu;
		}
		else if (_active_menu == &_joy_settings_menu) {
			_active_menu = &_options_menu;
		}
		else if (_active_menu == &_resolution_menu) {
			_active_menu = &_video_options_menu;
		}
		else if (_active_menu == &_profiles_menu) {
			_active_menu = &_options_menu;
		}
		else if (_active_menu == &_load_profile_menu){
			_active_menu = &_profiles_menu;
		}
		else if (_active_menu == &_save_profile_menu){
			_active_menu = &_profiles_menu;
		}
		else if (_active_menu == &_delete_profile_menu){
			_active_menu = &_profiles_menu;
		}
		else if(_active_menu == &_user_input_menu) {
			_file_name_window.Hide();
			_file_name_alert.Hide();
			_active_menu = &_save_profile_menu;
		}
		// Play cancel sound
		_boot_sounds.at(1).Play();
	}

	// Update menu events
// 	_active_menu->GetEvent();
} // void BootMode::Update()



void BootMode::Draw() {
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);

	// If we're animating logo at the moment, handle all drawing in there and simply return
	if (_initial_entry) {
		_AnimateLogo();
		return;
	}

	_DrawBackgroundItems();

	_options_window.Draw();

	// Decide whether to draw the credits window, welcome window or the main menu
	if (_credits_window->IsActive())
		_credits_window->Draw();
	else if (_welcome_window->IsActive())
		_welcome_window->Draw();
	else if (_active_menu != NULL)
		_active_menu->Draw();

	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
	VideoManager->Move(10.0f, 10.0f);
	_version_text.Draw();
	VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, 0);
	VideoManager->Move(1013.0f, 10.0f);
	_copyright_text.Draw();

// 	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);
	VideoManager->Move(0.0f, 0.0f);
	_message_window.Draw();
	_file_name_alert.Draw();
	_file_name_window.Draw();
}

// ****************************************************************************
// ***** BootMode menu setup and refresh methods
// ****************************************************************************

void BootMode::_SetupMainMenu() {
	if (TEMP_BOOT_TEST == true) {
		_main_menu.SetPosition(512.0f, 80.0f);
		_main_menu.SetDimensions(1000.0f, 50.0f, 8, 1, 8, 1);
		_main_menu.SetTextStyle(TextStyle("title24"));
		_main_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
		_main_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
		_main_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
		_main_menu.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
		_main_menu.SetCursorOffset(-50.0f, 28.0f);
		_main_menu.SetSkipDisabled(true);

		// Add all the needed menu options to the main menu
		_main_menu.AddOption(UTranslate("New Game"), &BootMode::_OnNewGame);
		_main_menu.AddOption(UTranslate("Load Game"), &BootMode::_OnLoadGame);
		_main_menu.AddOption(UTranslate("Options"), &BootMode::_OnOptions);
		_main_menu.AddOption(UTranslate("Credits"), &BootMode::_OnCredits);
		_main_menu.AddOption(UTranslate("Battle"), &BootMode::_TEMP_OnBattle);
		_main_menu.AddOption(UTranslate("Menu"), &BootMode::_TEMP_OnMenu);
		_main_menu.AddOption(UTranslate("Shop"), &BootMode::_TEMP_OnShop);
		_main_menu.AddOption(UTranslate("Quit"), &BootMode::_OnQuit);
	}
	else {
		_main_menu.SetPosition(512.0f, 80.0f);
		_main_menu.SetDimensions(800.0f, 50.0f, 5, 1, 5, 1);
		_main_menu.SetTextStyle(TextStyle("title24"));
		_main_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
		_main_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
		_main_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
		_main_menu.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
		_main_menu.SetCursorOffset(-50.0f, 28.0f);

		// Add all the needed menu options to the main menu
		_main_menu.AddOption(UTranslate("New Game"), &BootMode::_OnNewGame);
		_main_menu.AddOption(UTranslate("Load Game"), &BootMode::_OnLoadGame);
		_main_menu.AddOption(UTranslate("Options"), &BootMode::_OnOptions);
		_main_menu.AddOption(UTranslate("Credits"), &BootMode::_OnCredits);
		_main_menu.AddOption(UTranslate("Quit"), &BootMode::_OnQuit);
	}

	string path = GetUserDataPath(true) + "saved_game_1.lua";
	if (DoesFileExist(path) == false) {
		_main_menu.EnableOption(1, false);
		_main_menu.SetSelection(0);
	}
	else {
		_main_menu.SetSelection(1);
	}
}



void BootMode::_SetupOptionsMenu() {
	_options_menu.SetPosition(512.0f, 300.0f);
	_options_menu.SetDimensions(300.0f, 600.0f, 1, 6, 1, 6);
	_options_menu.SetTextStyle(TextStyle("title22"));
	_options_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_options_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
	_options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_options_menu.SetCursorOffset(-50.0f, 28.0f);
	_options_menu.SetSkipDisabled(true);

	_options_menu.AddOption(UTranslate("Video"), &BootMode::_OnVideoOptions);
	_options_menu.AddOption(UTranslate("Audio"), &BootMode::_OnAudioOptions);
	_options_menu.AddOption(UTranslate("Language"), &BootMode::_OnLanguageOptions);
	_options_menu.AddOption(UTranslate("Key Settings"), &BootMode::_OnKeySettings);
	_options_menu.AddOption(UTranslate("Joystick Settings"), &BootMode::_OnJoySettings);
	_options_menu.AddOption(UTranslate("Profiles"), &BootMode::_OnProfiles);

	_options_menu.SetSelection(0);
}



void BootMode::_SetupVideoOptionsMenu() {
	_video_options_menu.SetPosition(512.0f, 300.0f);
	_video_options_menu.SetDimensions(300.0f, 400.0f, 1, 4, 1, 4);
	_video_options_menu.SetTextStyle(TextStyle("title22"));
	_video_options_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_video_options_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_video_options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
	_video_options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_video_options_menu.SetCursorOffset(-50.0f, 28.0f);
	_video_options_menu.SetSkipDisabled(true);

	_video_options_menu.AddOption(UTranslate("Resolution:"), &BootMode::_OnResolution);
	// Left & right will change window mode as well as confirm
	_video_options_menu.AddOption(UTranslate("Window mode:"), &BootMode::_OnToggleFullscreen, NULL, NULL, &BootMode::_OnToggleFullscreen, &BootMode::_OnToggleFullscreen);
	_video_options_menu.AddOption(UTranslate("Brightness:"), NULL, NULL, NULL, &BootMode::_OnBrightnessLeft, &BootMode::_OnBrightnessRight);
	_video_options_menu.AddOption(UTranslate("Image quality:"));

	_video_options_menu.EnableOption(3, false); // Disable image quality

	_video_options_menu.SetSelection(0);
}



void BootMode::_SetupAudioOptionsMenu() {
	_audio_options_menu.SetPosition(512.0f, 300.0f);
	_audio_options_menu.SetDimensions(300.0f, 200.0f, 1, 2, 1, 2);
	_audio_options_menu.SetTextStyle(TextStyle("title22"));
	_audio_options_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_audio_options_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_audio_options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
	_audio_options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_audio_options_menu.SetCursorOffset(-50.0f, 28.0f);
	_audio_options_menu.SetSkipDisabled(true);

	_audio_options_menu.AddOption(UTranslate("Sound Volume: "), NULL, NULL, NULL, &BootMode::_OnSoundLeft, &BootMode::_OnSoundRight);
	_audio_options_menu.AddOption(UTranslate("Music Volume: "), NULL, NULL, NULL, &BootMode::_OnMusicLeft, &BootMode::_OnMusicRight);

	_audio_options_menu.SetSelection(0);
}



void BootMode::_SetupLanguageOptionsMenu() {
	_language_options_menu.SetPosition(512.0f, 300.0f);
	_language_options_menu.SetTextStyle(TextStyle("title22"));
	_language_options_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_language_options_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_language_options_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
	_language_options_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_language_options_menu.SetCursorOffset(-50.0f, 28.0f);
	_language_options_menu.SetSkipDisabled(true);


	// Get the list of languages from the Lua file.
	ReadScriptDescriptor read_data;
	if (!read_data.OpenFile(_LANGUAGE_FILE)) {
		PRINT_ERROR << "Failed to load language file: " << _LANGUAGE_FILE << endl;
		PRINT_ERROR << "The language list will be empty." << endl;
		return;
	}

	read_data.OpenTable("languages");
	uint32 table_size = read_data.GetTableSize();

	// Set up the dimensions of the window according to how many languages are available.
	_language_options_menu.SetDimensions(300.0f, 200.0f, 1, table_size, 1, table_size);

	_po_files.clear();
	for (uint32 i = 1; i <= table_size; i++) {
		read_data.OpenTable(i);
		_po_files.push_back(read_data.ReadString(2));
		_language_options_menu.AddOption(MakeUnicodeString(read_data.ReadString(1)),
										 &BootMode::_OnLanguageSelect);
		read_data.CloseTable();
	}

	read_data.CloseTable();
	if (read_data.IsErrorDetected())
		PRINT_ERROR << "Error occurred while loading language list: " << read_data.GetErrorMessages() << endl;
	read_data.CloseFile();
}



void BootMode::_SetupKeySetttingsMenu() {
	_key_settings_menu.SetPosition(512.0f, 300.0f);
	_key_settings_menu.SetDimensions(250.0f, 500.0f, 1, 12, 1, 12);
	_key_settings_menu.SetTextStyle(TextStyle("title22"));
	_key_settings_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_key_settings_menu.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_key_settings_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
	_key_settings_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_key_settings_menu.SetCursorOffset(-50.0f, 28.0f);
	_key_settings_menu.SetSkipDisabled(true);

	_key_settings_menu.AddOption(UTranslate("Up: "), &BootMode::_RedefineUpKey);
	_key_settings_menu.AddOption(UTranslate("Down: "), &BootMode::_RedefineDownKey);
	_key_settings_menu.AddOption(UTranslate("Left: "), &BootMode::_RedefineLeftKey);
	_key_settings_menu.AddOption(UTranslate("Right: "), &BootMode::_RedefineRightKey);
	_key_settings_menu.AddOption(UTranslate("Confirm: "), &BootMode::_RedefineConfirmKey);
	_key_settings_menu.AddOption(UTranslate("Cancel: "), &BootMode::_RedefineCancelKey);
	_key_settings_menu.AddOption(UTranslate("Menu: "), &BootMode::_RedefineMenuKey);
	_key_settings_menu.AddOption(UTranslate("Swap: "), &BootMode::_RedefineSwapKey);
	_key_settings_menu.AddOption(UTranslate("Left Select: "), &BootMode::_RedefineLeftSelectKey);
	_key_settings_menu.AddOption(UTranslate("Right Select: "), &BootMode::_RedefineRightSelectKey);
	_key_settings_menu.AddOption(UTranslate("Pause: "), &BootMode::_RedefinePauseKey);
	_key_settings_menu.AddOption(UTranslate("Restore defaults"), &BootMode::_OnRestoreDefaultKeys);
}



void BootMode::_SetupJoySetttingsMenu() {
	_joy_settings_menu.SetPosition(512.0f, 300.0f);
	_joy_settings_menu.SetDimensions(250.0f, 500.0f, 1, 10, 1, 10);
	_joy_settings_menu.SetTextStyle(TextStyle("title22"));
	_joy_settings_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_joy_settings_menu.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_joy_settings_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
	_joy_settings_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_joy_settings_menu.SetCursorOffset(-50.0f, 28.0f);
	_joy_settings_menu.SetSkipDisabled(true);

	ustring dummy;
	_joy_settings_menu.AddOption(dummy, &BootMode::_RedefineXAxisJoy);
	_joy_settings_menu.AddOption(dummy, &BootMode::_RedefineYAxisJoy);
//	_joy_settings_menu.AddOption(dummy, &BootMode::_RedefineThresholdJoy);

	_joy_settings_menu.AddOption(dummy, &BootMode::_RedefineConfirmJoy);
	_joy_settings_menu.AddOption(dummy, &BootMode::_RedefineCancelJoy);
	_joy_settings_menu.AddOption(dummy, &BootMode::_RedefineMenuJoy);
	_joy_settings_menu.AddOption(dummy, &BootMode::_RedefineSwapJoy);
	_joy_settings_menu.AddOption(dummy, &BootMode::_RedefineLeftSelectJoy);
	_joy_settings_menu.AddOption(dummy, &BootMode::_RedefineRightSelectJoy);
	_joy_settings_menu.AddOption(dummy, &BootMode::_RedefinePauseJoy);
//	_joy_settings_menu.AddOption(dummy, &BootMode::_RedefineQuitJoy);

	_joy_settings_menu.AddOption(UTranslate("Restore defaults"), &BootMode::_OnRestoreDefaultJoyButtons);
}



void BootMode::_SetupResolutionMenu() {
	_resolution_menu.SetPosition(512.0f, 300.0f);
	_resolution_menu.SetDimensions(300.0f, 200.0f, 1, 4, 1, 4);
	_resolution_menu.SetTextStyle(TextStyle("title22"));
	_resolution_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_resolution_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_resolution_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
	_resolution_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_resolution_menu.SetCursorOffset(-50.0f, 28.0f);
	_resolution_menu.SetSkipDisabled(true);

	_resolution_menu.AddOption(MakeUnicodeString("640 x 480"), &BootMode::_OnResolution640x480);
	_resolution_menu.AddOption(MakeUnicodeString("800 x 600"), &BootMode::_OnResolution800x600);
	_resolution_menu.AddOption(MakeUnicodeString("1024 x 768"), &BootMode::_OnResolution1024x768);
	_resolution_menu.AddOption(MakeUnicodeString("1280 x 1024"), &BootMode::_OnResolution1280x1024);

	if (VideoManager->GetScreenWidth() == 640)
		_resolution_menu.SetSelection(0);
	else if (VideoManager->GetScreenWidth() == 800)
		_resolution_menu.SetSelection(1);
	else if (VideoManager->GetScreenWidth() == 1024)
		_resolution_menu.SetSelection(2);
	else if(VideoManager->GetScreenWidth() == 1280)
		_resolution_menu.SetSelection(3);
}



void BootMode::_SetupProfileMenu() {
	_profiles_menu.SetPosition(512.0f, 300.0f);
	_profiles_menu.SetDimensions(300.0f, 300.0f, 1, 3, 1, 3);
	_profiles_menu.SetTextStyle(TextStyle("title22"));
	_profiles_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_profiles_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_profiles_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
	_profiles_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_profiles_menu.SetCursorOffset(-50.0f, 28.0f);
	_profiles_menu.SetSkipDisabled(true);

	_profiles_menu.AddOption(UTranslate("Save"), &BootMode::_OnSaveProfile);
	_profiles_menu.AddOption(UTranslate("Load"), &BootMode::_OnLoadProfile);
	_profiles_menu.AddOption(UTranslate("Delete"), &BootMode::_OnDeleteProfile);
}



void BootMode::_SetupLoadProfileMenu() {
	_load_profile_menu.SetPosition(512.0f, 300.0f);
	_load_profile_menu.SetTextStyle(TextStyle("title22"));
	_load_profile_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_load_profile_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_load_profile_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
	_load_profile_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_load_profile_menu.SetCursorOffset(-50.0f, 28.0f);
	_load_profile_menu.SetSkipDisabled(true);

	_AddProfileOptions(&_load_profile_menu);
}



void BootMode::_SetupSaveProfileMenu() {
	_save_profile_menu.SetPosition(512.0f, 300.0f);
	_save_profile_menu.SetTextStyle(TextStyle("title22"));
	_save_profile_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_save_profile_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_save_profile_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
	_save_profile_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_save_profile_menu.SetCursorOffset(-50.0f, 28.0f);
	_save_profile_menu.SetSkipDisabled(true);

	//this option is selected when user wants to create a new file
	_save_profile_menu.AddOption(UTranslate("New Profile"), &BootMode::_OnSaveFile);

	_AddProfileOptions(&_save_profile_menu);
}



void BootMode::_SetupDeleteProfileMenu() {
	_delete_profile_menu.SetPosition(512.0f, 300.0f);
	_delete_profile_menu.SetTextStyle(TextStyle("title22"));
	_delete_profile_menu.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_delete_profile_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_delete_profile_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
	_delete_profile_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_delete_profile_menu.SetCursorOffset(-50.0f, 28.0f);
	_delete_profile_menu.SetSkipDisabled(true);

	_AddProfileOptions(&_delete_profile_menu);
}



void BootMode::_SetupUserInputMenu() {
	_user_input_menu.SetPosition(275.0f, 475.0f);
	_user_input_menu.SetDimensions(400.0f, 300.0f, 7, 4, 7, 4);
	_user_input_menu.SetTextStyle(TextStyle("title22"));
	_user_input_menu.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_user_input_menu.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_user_input_menu.SetSelectMode(VIDEO_SELECT_SINGLE);
	_user_input_menu.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_user_input_menu.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_user_input_menu.SetCursorOffset(-50.0f, 28.0f);
	_user_input_menu.SetSkipDisabled(true);


	//add in the letters :)
	_user_input_menu.AddOption(MakeUnicodeString("a"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("b"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("c"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("d"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("e"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("f"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("g"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("h"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("i"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("j"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("k"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("l"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("m"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("n"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("o"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("p"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("q"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("r"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("s"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("t"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("u"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("v"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("w"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("x"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("y"), &BootMode::_OnPickLetter);
	_user_input_menu.AddOption(MakeUnicodeString("z"), &BootMode::_OnPickLetter);

	//backspace to delete characters
	_user_input_menu.AddOption(UTranslate("back"), &BootMode::_OnPickLetter);

	//end to confirm the name
	_user_input_menu.AddOption(UTranslate("end"), &BootMode::_OnPickLetter);
}



void BootMode::_RefreshVideoOptions() {
	// Update resolution text
	std::ostringstream resolution("");
	resolution << "Resolution: " << VideoManager->GetScreenWidth() << " x " << VideoManager->GetScreenHeight();
	_video_options_menu.SetOptionText(0, MakeUnicodeString(resolution.str()));

	// Update text on current video mode
	if (VideoManager->IsFullscreen())
		_video_options_menu.SetOptionText(1, UTranslate("Window mode: fullscreen"));
	else
		_video_options_menu.SetOptionText(1, UTranslate("Window mode: windowed"));

	// Update brightness
	_video_options_menu.SetOptionText(2, UTranslate("Brightness: ") + MakeUnicodeString(NumberToString(VideoManager->GetGamma() * 50.0f + 0.5f) + " %"));
}



void BootMode::_RefreshAudioOptions() {
	_audio_options_menu.SetOptionText(0, UTranslate("Sound Volume: ") + MakeUnicodeString(NumberToString(static_cast<int32>(AudioManager->GetSoundVolume() * 100.0f + 0.5f)) + " %"));
	_audio_options_menu.SetOptionText(1, UTranslate("Music Volume: ") + MakeUnicodeString(NumberToString(static_cast<int32>(AudioManager->GetMusicVolume() * 100.0f + 0.5f)) + " %"));
}



void BootMode::_RefreshKeySettings() {
	// Update key names
	_key_settings_menu.SetOptionText(0, UTranslate("Move Up") + MakeUnicodeString("<r>" + InputManager->GetUpKeyName()));
	_key_settings_menu.SetOptionText(1, UTranslate("Move Down") + MakeUnicodeString("<r>" + InputManager->GetDownKeyName()));
	_key_settings_menu.SetOptionText(2, UTranslate("Move Left") + MakeUnicodeString("<r>" + InputManager->GetLeftKeyName()));
	_key_settings_menu.SetOptionText(3, UTranslate("Move Right") + MakeUnicodeString("<r>" + InputManager->GetRightKeyName()));
	_key_settings_menu.SetOptionText(4, UTranslate("Confirm") + MakeUnicodeString("<r>" + InputManager->GetConfirmKeyName()));
	_key_settings_menu.SetOptionText(5, UTranslate("Cancel") + MakeUnicodeString("<r>" + InputManager->GetCancelKeyName()));
	_key_settings_menu.SetOptionText(6, UTranslate("Menu") + MakeUnicodeString("<r>" + InputManager->GetMenuKeyName()));
	_key_settings_menu.SetOptionText(7, UTranslate("Swap") + MakeUnicodeString("<r>" + InputManager->GetSwapKeyName()));
	_key_settings_menu.SetOptionText(8, UTranslate("Left Select") + MakeUnicodeString("<r>" + InputManager->GetLeftSelectKeyName()));
	_key_settings_menu.SetOptionText(9, UTranslate("Right Select") + MakeUnicodeString("<r>" + InputManager->GetRightSelectKeyName()));
	_key_settings_menu.SetOptionText(10, UTranslate("Pause") + MakeUnicodeString("<r>" + InputManager->GetPauseKeyName()));
}



void BootMode::_RefreshJoySettings() {
	int32 i = 0;
	_joy_settings_menu.SetOptionText(i++, UTranslate("X Axis") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetXAxisJoy())));
	_joy_settings_menu.SetOptionText(i++, UTranslate("Y Axis") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetYAxisJoy())));
	_joy_settings_menu.SetOptionText(i++, UTranslate("Confirm: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetConfirmJoy())));
	_joy_settings_menu.SetOptionText(i++, UTranslate("Cancel: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetCancelJoy())));
	_joy_settings_menu.SetOptionText(i++, UTranslate("Menu: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetMenuJoy())));
	_joy_settings_menu.SetOptionText(i++, UTranslate("Swap: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetSwapJoy())));
	_joy_settings_menu.SetOptionText(i++, UTranslate("Left Select: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetLeftSelectJoy())));
	_joy_settings_menu.SetOptionText(i++, UTranslate("Right Select: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetRightSelectJoy())));
	_joy_settings_menu.SetOptionText(i++, UTranslate("Pause: Button") + MakeUnicodeString("<r>" + NumberToString(InputManager->GetPauseJoy())));
}



void BootMode::_RefreshSaveAndLoadProfiles() {
	//match the text with the directory listing :0
	for (uint32 i = 0; i < _GetDirectoryListingUserProfilePath().size(); i++) {
		_load_profile_menu.SetOptionText(i,MakeUnicodeString(_GetDirectoryListingUserProfilePath().at(i)));
		_delete_profile_menu.SetOptionText(i,MakeUnicodeString(_GetDirectoryListingUserProfilePath().at(i)));
	}
}

// ****************************************************************************
// ***** BootMode menu handler methods
// ****************************************************************************

void BootMode::_OnNewGame() {
	GlobalManager->NewGame();

	_fade_out = true;
	VideoManager->FadeScreen(Color::black, 1000); // Fade to black over the course of one second
//	_boot_music.at(0).SetFadeOutTime(500); // Fade out the music
	_boot_music.at(0).Stop();
}



void BootMode::_OnLoadGame() {
	_boot_music.at(0).Stop();
	// TODO: SaveMode music should take over when this is used for loading games...
	hoa_save::SaveMode *SVM = new hoa_save::SaveMode(false);
	ModeManager->Push(SVM);
}



void BootMode::_OnOptions() {
	_active_menu = &_options_menu;
	_options_window.Show();
}



void BootMode::_OnCredits() {
	_credits_window->Show();
}



void BootMode::_OnQuit() {
	SystemManager->ExitGame();
}



void BootMode::_TEMP_OnBattle() {
	ReadScriptDescriptor read_data;
	if (!read_data.OpenFile("dat/config/boot.lua")) {
		PRINT_ERROR << "failed to load boot data file" << endl;
	}

	ScriptCallFunction<void>(read_data.GetLuaState(), "BootBattleTest");

	if (read_data.IsErrorDetected()) {
		PRINT_ERROR << "an error occured during reading of the boot data file" << endl;
		PRINT_ERROR << read_data.GetErrorMessages() << endl;
	}
	read_data.CloseFile();
}



void BootMode::_TEMP_OnMenu() {
	ReadScriptDescriptor read_data;
	if (!read_data.OpenFile("dat/config/boot.lua")) {
		PRINT_ERROR << "failed to load boot data file" << endl;
	}

	ScriptCallFunction<void>(read_data.GetLuaState(), "BootMenuTest");

	if (read_data.IsErrorDetected()) {
		PRINT_ERROR << "an error occured during reading of the boot data file" << endl;
		PRINT_ERROR << read_data.GetErrorMessages() << endl;
	}
	read_data.CloseFile();

	// TEMP: remove this once menu mode can be created in Lua and then add to boot.lua
	GlobalManager->AddToInventory(1, 5);
	GlobalManager->AddCharacter(1);
	GlobalManager->AddCharacter(2);
	GlobalManager->AddCharacter(4);
	GlobalManager->AddCharacter(8);
	hoa_menu::MenuMode *MM = new hoa_menu::MenuMode(MakeUnicodeString("The Boot Screen"), "img/menus/locations/desert_cave.png");
	ModeManager->Push(MM);
}



void BootMode::_TEMP_OnShop() {
	ReadScriptDescriptor read_data;
	if (!read_data.OpenFile("dat/config/boot.lua")) {
		PRINT_ERROR << "failed to load boot data file" << endl;
	}

	ScriptCallFunction<void>(read_data.GetLuaState(), "BootShopTest");

	if (read_data.IsErrorDetected()) {
		PRINT_ERROR << "an error occured during reading of the boot data file" << endl;
		PRINT_ERROR << read_data.GetErrorMessages() << endl;
	}
	read_data.CloseFile();
}



void BootMode::_OnVideoOptions() {
	_active_menu = &_video_options_menu;
	_RefreshVideoOptions();
}



void BootMode::_OnAudioOptions() {
	// Switch the current menu
	_active_menu = &_audio_options_menu;
	_RefreshAudioOptions();
}



void BootMode::_OnLanguageOptions()
{
	// Switch the current menu
	_active_menu = &_language_options_menu;
	//_UpdateLanguageOptions();
}



void BootMode::_OnKeySettings() {
	_active_menu = &_key_settings_menu;
	_RefreshKeySettings();
}



void BootMode::_OnJoySettings() {
	_active_menu = &_joy_settings_menu;
	_RefreshJoySettings();
}



void BootMode::_OnProfiles() {
	_active_menu = &_profiles_menu;
}



void BootMode::_OnToggleFullscreen() {
	// Toggle fullscreen / windowed
	VideoManager->ToggleFullscreen();
	VideoManager->ApplySettings();
	_RefreshVideoOptions();
	_has_modified_settings = true;
}



void BootMode::_OnResolution() {
	_active_menu = &_resolution_menu;
}



void BootMode::_OnResolution640x480() {
	if (VideoManager->GetScreenWidth() != 640 && VideoManager->GetScreenHeight() != 480)
		_ChangeResolution(640, 480);
}



void BootMode::_OnResolution800x600() {
	if (VideoManager->GetScreenWidth() != 800 && VideoManager->GetScreenHeight() != 600)
		_ChangeResolution(800, 600);
}



void BootMode::_OnResolution1024x768() {
	if (VideoManager->GetScreenWidth() != 1024 && VideoManager->GetScreenHeight() != 768)
		_ChangeResolution(1024, 768);
}



void BootMode::_OnResolution1280x1024() {
	if(VideoManager->GetScreenWidth() != 1280 && VideoManager->GetScreenHeight() != 1024)
		_ChangeResolution(1280,1024);
}



void BootMode::_OnBrightnessLeft() {
	VideoManager->SetGamma(VideoManager->GetGamma() - 0.1f);
	_RefreshVideoOptions();
}



void BootMode::_OnBrightnessRight() {
	VideoManager->SetGamma(VideoManager->GetGamma() + 0.1f);
	_RefreshVideoOptions();
}



void BootMode::_OnSoundLeft() {
	AudioManager->SetSoundVolume(AudioManager->GetSoundVolume() - 0.1f);
	_RefreshAudioOptions();
	_boot_sounds.at(4).Play(); // Play a sound for user to hear new volume level.
	_has_modified_settings = true;
}



void BootMode::_OnSoundRight() {
	AudioManager->SetSoundVolume(AudioManager->GetSoundVolume() + 0.1f);
	_RefreshAudioOptions();
	_boot_sounds.at(4).Play(); // Play a sound for user to hear new volume level
	_has_modified_settings = true;
}



void BootMode::_OnMusicLeft() {
	AudioManager->SetMusicVolume(AudioManager->GetMusicVolume() - 0.1f);
	_RefreshAudioOptions();
	_has_modified_settings = true;
}



void BootMode::_OnMusicRight() {
	AudioManager->SetMusicVolume(AudioManager->GetMusicVolume() + 0.1f);
	_RefreshAudioOptions();
	_has_modified_settings = true;
}



void BootMode::_OnLanguageSelect() {
	SystemManager->SetLanguage(_po_files[_language_options_menu.GetSelection()]);
	_has_modified_settings = true;

	// TODO: when the new language is set by the above call, we need to reload/refresh all text,
	// otherwise the new language will not take effect.
}



void BootMode::_OnRestoreDefaultKeys() {
	InputManager->RestoreDefaultKeys();
	_RefreshKeySettings();
	_has_modified_settings = true;
}



void BootMode::_OnRestoreDefaultJoyButtons() {
	InputManager->RestoreDefaultJoyButtons();
	_RefreshJoySettings();
	_has_modified_settings = true;
}



void BootMode::_OnLoadProfile() {
	_active_menu = &_load_profile_menu;
}



void BootMode::_OnSaveProfile() {
	_active_menu = &_save_profile_menu;

}



void BootMode::_OnDeleteProfile() {
	_active_menu = &_delete_profile_menu;
}



void BootMode::_OnLoadFile() {
	//get the file path
	if (_load_profile_menu.GetSelection() < 0 || _load_profile_menu.GetSelection() >= (int32)_GetDirectoryListingUserProfilePath().size())
		cerr << "selection was out of range: " << _load_profile_menu.GetSelection() << " try another one " << endl;
	else {
		//we took off the .lua extension so that end users wouldn't see it but we need to add it back now
		const string& filename = GetUserProfilePath() + _GetDirectoryListingUserProfilePath().at(_load_profile_menu.GetSelection()) + ".lua";
		bool success = _LoadSettingsFile(filename);

		//load the file
		if (BOOT_DEBUG) {
			if (success)
				cout << "BOOT: profile was successfully loaded " << filename << endl;
			else
				cout << "BOOT ERROR: profile failed to load " << filename << endl;
		}

		//update all of the settings when loaded
		_RefreshKeySettings();
		_RefreshJoySettings();
		_RefreshVideoOptions();
		_RefreshAudioOptions();
	}
}



void BootMode::_OnSaveFile() {
	//if new profile was selected go to the user input menu
	if (_save_profile_menu.GetSelection() == 0) {
		_active_menu = &_user_input_menu;

		//show the alert windows before we switch
		_file_name_alert.SetPosition(275.0f, 575.0f);
		_file_name_alert.SetText(UTranslate("Please enter a name for your new profile"));
		_file_name_alert.Show();

		_file_name_window.SetPosition(275.0f, 150.0f);
		_file_name_window.Show();
	}
	else {
		_file_name_alert.SetPosition(360.0f, 115.0f);
		_file_name_alert.SetText(UTranslate("Overwrite? Confirm/Cancel"));
		_file_name_alert.Show();
		_overwrite_function = &BootMode::_OverwriteProfile;
	}
}



void BootMode::_OnDeleteFile() {
	if (_load_profile_menu.GetSelection() < 0 || _load_profile_menu.GetSelection() >= (int32)_GetDirectoryListingUserProfilePath().size())
		cerr << "selection was out of range: " << _load_profile_menu.GetSelection() << " try another one " << endl;
	else {
		//get the file path
		//we took off the .lua extension so that end users wouldn't see it but we need to add it back now
		const string& filename = GetUserProfilePath() + _GetDirectoryListingUserProfilePath().at(_load_profile_menu.GetSelection()) + ".lua";

		bool success = DeleteFile(filename);

		if (BOOT_DEBUG) {
			if (success)
				cout << "BOOT: profile was successfully deleted " << filename << endl;
			else
				cout << "BOOT ERROR: failed to delete profile " << filename << endl;
		}

		//Clear the option boxes on all the menus and reload them so we can get rid of the deleted profile
		_save_profile_menu.ClearOptions();
		_delete_profile_menu.ClearOptions();
		_load_profile_menu.ClearOptions();

		//reload the menus
		_SetupSaveProfileMenu();
		_SetupDeleteProfileMenu();
		_SetupLoadProfileMenu();
	}
}



void BootMode::_OnPickLetter() {
	//the letters from the table
	char letters[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

	if(_user_input_menu.GetSelection() == END) {
		//end, so save the file

		//add the .lua extension
		_current_filename += ".lua";

		//lets save this mofo :) and then add the option to save_profile menu and the load-profile menu
		_has_modified_settings = true;

		if(_SaveSettingsFile(_current_filename))
		{
			if(BOOT_DEBUG)
				cout << "BOOT: profile successfully saved.";
		}
		else
			cout << "BOOT ERROR: profile could not be saved.";


		//take off the .lua extension so the end user doesnt see it
		_current_filename.erase(_current_filename.size() - 4);
		_save_profile_menu.AddOption(MakeUnicodeString(_current_filename), &BootMode::_OnSaveFile);
		_load_profile_menu.AddOption(MakeUnicodeString(_current_filename), &BootMode::_OnLoadFile);
		_delete_profile_menu.AddOption(MakeUnicodeString(_current_filename), &BootMode::_OnDeleteFile);

		//make sure we reset the current _current_filename string
		_current_filename = "";

		//also make sure we hide the alert windows if they havent selected new profile
		_file_name_alert.Hide();
		_file_name_window.Hide();

		//update the profile menus
		_RefreshSaveAndLoadProfiles();

		//since we ended we have to go back to the save profile menu
		_active_menu = &_save_profile_menu;

	}
	else if(_user_input_menu.GetSelection() == BACK) {
		//take off the last letter
		//we subtract 1 because char arrays AKA strings start at position 0

		//make sure we dont try to erase letters that are not there
		if(_current_filename != "")
			_current_filename.erase(_current_filename.length()-1);
	}
	else {
		//add letter to the _current_filename the _current_filename can be no longer than 19 characters
		if(_current_filename.length() != MAX_NAME)
			_current_filename += letters[_user_input_menu.GetSelection()];
		else if(BOOT_DEBUG)
			cout << "BOOT ERROR: _current_filename cannot be longer than 19 characters";
	}

	_file_name_window.SetText(MakeUnicodeString(_current_filename));
} // void BootMode::_OnPickLetter()

// ****************************************************************************
// ***** BootMode helper methods
// ****************************************************************************

void BootMode::_DrawBackgroundItems() {
	VideoManager->Move(512.0f, 384.0f);
	VideoManager->SetDrawFlags(VIDEO_NO_BLEND, 0);
	_boot_images[0].Draw(); // Draw background

	VideoManager->Move(512.0f, 648.0f);
	VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
	_boot_images[1].Draw(); // Draw the logo background

	VideoManager->Move(762.0f, 578.0f);
	VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
	_boot_images[2].Draw(); // Draw the sword

	VideoManager->Move(512.0f, 648.0f);
	VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
	_boot_images[3].Draw(); // Draw the logo text
}



void BootMode::_AnimateLogo() {
	// Sequence starting times. Note: I've changed _every_ variable here into floats
	// to avoid unneccessary type casts that would kill performance! -Viljami
	static const float SEQUENCE_ONE = 0.0f;
	static const float SEQUENCE_TWO = SEQUENCE_ONE + 1000.0f;
	static const float SEQUENCE_THREE = SEQUENCE_TWO + 2000.0f;
	static const float SEQUENCE_FOUR = SEQUENCE_THREE + 575.0f;
	static const float SEQUENCE_FIVE = SEQUENCE_FOUR + 1900.0f;
	static const float SEQUENCE_SIX = SEQUENCE_FIVE + 1400.0f;
	static const float SEQUENCE_SEVEN = SEQUENCE_SIX + 3500.0f;

	// Sword image position and rotation
	static float sword_x = 670.0f;
	static float sword_y = 360.0f;
	static float rotation = -90.0f;

	// Total time in ms
	static float total_time = 0.0f;

	// Get the frametime and update total time
	float time_elapsed = static_cast<float>(SystemManager->GetUpdateTime());
	total_time += time_elapsed;

	// Sequence one: black
	if (total_time >= SEQUENCE_ONE && total_time < SEQUENCE_TWO) {
		// Nothing drawn during this sequence
	}
	// Sequence two: fade in logo+sword
	else if (total_time >= SEQUENCE_TWO && total_time < SEQUENCE_THREE) {
		float alpha = (total_time - SEQUENCE_TWO) / (SEQUENCE_THREE - SEQUENCE_TWO);

		VideoManager->Move(512.0f, 385.0f); // logo bg
		VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
		_boot_images[1].Draw(Color(alpha, alpha, alpha, 1.0f));
		VideoManager->Move(sword_x, sword_y); // sword
		VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
		VideoManager->Rotate(-90.0f);
		_boot_images[2].Draw(Color(alpha, alpha, alpha, 1.0f));
		VideoManager->Move(512.0f, 385.0f); // text
		VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
		_boot_images[3].Draw(Color(alpha, alpha, alpha, 1.0f));
	}
	// Sequence three: Sword unsheathe & slide
	else if (total_time >= SEQUENCE_THREE && total_time < SEQUENCE_FOUR) {
		float dt = (total_time - SEQUENCE_THREE) * 0.001f;
		sword_x = 670.0f + (dt * dt) * 660.0f; // s = s0 + 0.5 * a * t^2
		VideoManager->Move(512.0f, 385.0f); // logo bg
		VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
		_boot_images[1].Draw();
		VideoManager->Move(sword_x, sword_y); // sword
		VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
		VideoManager->Rotate(-90.0f);
		_boot_images[2].Draw();
		VideoManager->Move(512.0f, 385.0f); // text
		VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
		_boot_images[3].Draw();
	}
	// Sequence four: Spin around the sword
	else if (total_time >= SEQUENCE_FOUR && total_time < SEQUENCE_FIVE) {
		const float ROTATIONS = 720.0f + 90.0f;
		const float SPEED_LEFT = 35.0f;
		const float SPEED_UP = 750.0f;
		const float GRAVITY = 120.0f;

		// Delta goes from 0.0f to 1.0f
		float delta = ((total_time - SEQUENCE_FOUR) / (SEQUENCE_FIVE - SEQUENCE_FOUR));
		float dt = (total_time - SEQUENCE_FOUR) * 0.001f;
		sword_x = 885.941f - dt * dt * SPEED_LEFT; // Small accelerated movement to left
		sword_y = 360.0f - dt * dt * GRAVITY + SPEED_UP * delta;
		rotation = -90.0f + delta * ROTATIONS;

		VideoManager->Move(512.0f, 385.0f); // logo bg
		VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
		_boot_images[1].Draw();
		VideoManager->Move(512.0f, 385.0f); // text
		VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
		_boot_images[3].Draw();
		VideoManager->Move(sword_x, sword_y); // sword
		VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
		VideoManager->Rotate(rotation);
		_boot_images[2].Draw();
	}
	// Sequence five: Sword comes back
	else if (total_time >= SEQUENCE_FIVE && total_time < SEQUENCE_SIX) {
		// Delta goes from 0.0f to 1.0f
		float delta_root = (total_time - SEQUENCE_FIVE) / (SEQUENCE_SIX - SEQUENCE_FIVE);
		float delta = delta_root * delta_root * delta_root * delta_root;
		float newX = (1.0f - delta) * sword_x + 762.0f * delta;
		float newY = (1.0f - delta) * sword_y + 310.0f * delta;

		VideoManager->Move(512.0f, 385.0f); // logo bg
		VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
		_boot_images[1].Draw();
		VideoManager->Move(512.0f, 385.0f); // text
		VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
		_boot_images[3].Draw();
		VideoManager->Move(newX, newY); // sword
		VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
		_boot_images[2].Draw();
	}
	// Sequence six: flash of light
	else if (total_time >= SEQUENCE_SIX && total_time < SEQUENCE_SEVEN) {
		// Delta goes from 1.0f to 0.0f
		float delta = (total_time - SEQUENCE_SIX) / (SEQUENCE_SEVEN - SEQUENCE_SIX);
		delta = 1.0f - delta * delta;
		_DrawBackgroundItems();
		Color targetColor(Color::white);
		targetColor.SetAlpha(delta);
		VideoManager->DrawFullscreenOverlay(targetColor);
	}
	else if (total_time >= SEQUENCE_SEVEN) {
		_EndLogoAnimation();
		_DrawBackgroundItems();
	}
} // void BootMode::_AnimateLogo()



void BootMode::_EndLogoAnimation() {
	// Stop playing SFX and start playing the main theme
//	_boot_music.at(1).SetFadeOutTime(1000);
	_boot_music.at(1).Stop();
//	_boot_music.at(0).SetFadeInTime(5000);
	_boot_music.at(0).Play();

//	Effects::FadeOut(_boot_music.at(1), 10.0f);
//	Effects::FadeIn(_boot_music.at(0), 50.0f);

	// Load the settings file for reading in the welcome variable
	ReadScriptDescriptor settings_lua;
	string file = GetSettingsFilename();
	if (!settings_lua.OpenFile(file)) {
		PRINT_WARNING << "failed to load the boot settings file" << endl;
	}

	settings_lua.OpenTable("settings");
	if (settings_lua.ReadInt("welcome") == 1) {
		_welcome_window->Show();
	}
	settings_lua.CloseTable();
	settings_lua.CloseFile();
	_initial_entry = false;
}



void BootMode::_ShowMessageWindow(bool joystick) {
	if (joystick)
		_ShowMessageWindow(WAIT_JOY_BUTTON);
	else
		_ShowMessageWindow(WAIT_KEY);
}



void BootMode::_ShowMessageWindow(WAIT_FOR wait) {
	string message = "";
	if (wait == WAIT_JOY_BUTTON)
		_message_window.SetText(UTranslate("Please press a new joystick button."));
	else if (wait == WAIT_KEY)
		_message_window.SetText(UTranslate("Please press a new key."));
	else if (wait == WAIT_JOY_AXIS)
		_message_window.SetText(UTranslate("Please move an axis."));
	else {
		PRINT_WARNING << "Undefined wait value." << std::endl;
		return;
	}

	_message_window.Show();
}



void BootMode::_ChangeResolution(int32 width, int32 height) {
	VideoManager->SetResolution(width, height);
	VideoManager->ApplySettings();
// 	_active_menu = &_video_options_menu; // return back to video options
	_RefreshVideoOptions();
	_has_modified_settings = true;
}



bool BootMode::_LoadSettingsFile(const std::string& filename) {
	ReadScriptDescriptor settings;

	if (settings.OpenFile(filename) == false)
		return false;

	if(BOOT_DEBUG)
		cout << "BOOT: Opened file to load settings " << settings.GetFilename() << endl;

	settings.OpenTable("settings");

	// Load language settings
//	uint32 lang_code = static_cast<uint32>(settings.ReadInt("lang_code"));

//	if (lang_code == 1) SystemManager->SetLanguage("en@quot");
//	else if (lang_code == 2) SystemManager->SetLanguage("fr");
//	else if (lang_code == 3) SystemManager->SetLanguage("pt_BR");
//	else if (lang_code == 4) SystemManager->SetLanguage("es");
//	else if (lang_code == 5) SystemManager->SetLanguage("de");
//	else SystemManager->SetLanguage("en@quot");

	SystemManager->SetLanguage(static_cast<std::string>(settings.ReadString("language")));

	settings.OpenTable("key_settings");
	InputManager->SetUpKey(static_cast<SDLKey>(settings.ReadInt("up")));
	InputManager->SetDownKey(static_cast<SDLKey>(settings.ReadInt("down")));
	InputManager->SetLeftKey(static_cast<SDLKey>(settings.ReadInt("left")));
	InputManager->SetRightKey(static_cast<SDLKey>(settings.ReadInt("right")));
	InputManager->SetConfirmKey(static_cast<SDLKey>(settings.ReadInt("confirm")));
	InputManager->SetCancelKey(static_cast<SDLKey>(settings.ReadInt("cancel")));
	InputManager->SetMenuKey(static_cast<SDLKey>(settings.ReadInt("menu")));
	InputManager->SetSwapKey(static_cast<SDLKey>(settings.ReadInt("swap")));
	InputManager->SetLeftSelectKey(static_cast<SDLKey>(settings.ReadInt("left_select")));
	InputManager->SetRightSelectKey(static_cast<SDLKey>(settings.ReadInt("right_select")));
	InputManager->SetPauseKey(static_cast<SDLKey>(settings.ReadInt("pause")));
	settings.CloseTable();

	if (settings.IsErrorDetected()) {
		cerr << "SETTINGS LOAD ERROR: failure while trying to retrieve key map "
			<< "information from file: " << settings.GetFilename() << endl;
		cerr << settings.GetErrorMessages() << endl;
		return false;
	}

	settings.OpenTable("joystick_settings");
	// TEMP: this is a hack to disable joystick input to fix a bug with "phantom" joysticks on certain systems.
	// In the future it should call a method of the input engine to disable the joysticks.
	if (settings.DoesBoolExist("input_disabled") && settings.ReadBool("input_disabled") == true) {
		IF_PRINT_DEBUG(BOOT_DEBUG) << "settings file specified to disable joystick input" << endl;
		SDL_JoystickEventState(SDL_IGNORE);
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	}
	InputManager->SetJoyIndex(static_cast<int32>(settings.ReadInt("index")));
	InputManager->SetConfirmJoy(static_cast<uint8>(settings.ReadInt("confirm")));
	InputManager->SetCancelJoy(static_cast<uint8>(settings.ReadInt("cancel")));
	InputManager->SetMenuJoy(static_cast<uint8>(settings.ReadInt("menu")));
	InputManager->SetSwapJoy(static_cast<uint8>(settings.ReadInt("swap")));
	InputManager->SetLeftSelectJoy(static_cast<uint8>(settings.ReadInt("left_select")));
	InputManager->SetRightSelectJoy(static_cast<uint8>(settings.ReadInt("right_select")));
	InputManager->SetPauseJoy(static_cast<uint8>(settings.ReadInt("pause")));

	// WinterKnight: These are hidden settings. You can change them by editing settings.lua,
	// but they are not available in the options menu at this time.
	InputManager->SetQuitJoy(static_cast<uint8>(settings.ReadInt("quit")));
	if (settings.DoesIntExist("x_axis"))
		InputManager->SetXAxisJoy(static_cast<int8>(settings.ReadInt("x_axis")));
	if (settings.DoesIntExist("y_axis"))
		InputManager->SetYAxisJoy(static_cast<int8>(settings.ReadInt("y_axis")));
	if (settings.DoesIntExist("threshold"))
		InputManager->SetThresholdJoy(static_cast<uint16>(settings.ReadInt("threshold")));
	settings.CloseTable();

	if (settings.IsErrorDetected()) {
		cerr << "SETTINGS LOAD ERROR: an error occured while trying to retrieve joystick mapping information "
			<< "from file: " << settings.GetFilename() << endl;
		cerr << settings.GetErrorMessages() << endl;
		return false;
	}

	// Load video settings
	settings.OpenTable("video_settings");
	bool fullscreen = static_cast<bool>(settings.ReadBool("full_screen"));
	int32 resx = static_cast<int32>(settings.ReadInt("screen_resx"));


	//Set Resolution  according to width if no width matches our predefined resoultion set to lowest resolution
	if(resx == 800) {
		_OnResolution800x600();
		_resolution_menu.SetSelection(1);
	}
	else if(resx == 1024) {
		_OnResolution1024x768();
		_resolution_menu.SetSelection(2);
	}
	else if(resx == 1280) {
		_OnResolution1280x1024();
		_resolution_menu.SetSelection(3);
	}
	else {
		_OnResolution640x480();
		_resolution_menu.SetSelection(0);
	}

	//set the fullscreen and update video options
	if(VideoManager->IsFullscreen() && fullscreen == false)
		_OnToggleFullscreen();
	else if(VideoManager->IsFullscreen() == false && fullscreen)
		_OnToggleFullscreen();

	settings.CloseTable();

	if (settings.IsErrorDetected()) {
		cerr << "SETTINGS LOAD ERROR: failure while trying to retrieve video settings "
			<< "information from file: " << settings.GetFilename() << endl;
		cerr << settings.GetErrorMessages() << endl;
		return false;
	}

	// Load Audio settings
	if (AUDIO_ENABLE) {
		settings.OpenTable("audio_settings");
		AudioManager->SetMusicVolume(static_cast<float>(settings.ReadFloat("music_vol")));
		AudioManager->SetSoundVolume(static_cast<float>(settings.ReadFloat("sound_vol")));
	}
	settings.CloseAllTables();

	if (settings.IsErrorDetected()) {
		cerr << "SETTINGS LOAD ERROR: failure while trying to retrieve audio settings "
			<< "information from file: " << settings.GetFilename() << endl;
		cerr << settings.GetErrorMessages() << endl;
		return false;
	}

	settings.CloseFile();

	if(BOOT_DEBUG)
		cout << "Profile closed " << settings.GetFilename() << endl;


	return true;
} // bool BootMode::_LoadSettingsFile(const std::string& filename)



bool BootMode::_SaveSettingsFile(const std::string& filename) {

	// No need to save the settings if we haven't edited anything!
	if (!_has_modified_settings)
		return false;

	string file = "";
	string fileTemp = "";

	// Load the settings file for reading in the original data
	fileTemp = GetUserProfilePath() + "settings.lua";


	if(filename == "")
		file = fileTemp;
	else
		file = GetUserProfilePath() + filename;

	//copy the default file so we have an already set up lua file and then we can modify its settings
	if (!DoesFileExist(file))
		CopyFile(string("dat/config/settings.lua"), file);

	ModifyScriptDescriptor settings_lua;
	if (!settings_lua.OpenFile(file)) {
		cerr << "BOOT ERROR: failed to load the settings file!" << endl;
		return false;
	}

	// Write the current settings into the .lua file
	settings_lua.ModifyInt("settings.welcome", 0);

	//Save language settings
	settings_lua.ModifyString("settings.language", SystemManager->GetLanguage());
//	std::string lang_name = SystemManager->GetLanguage();

//	if (lang_name == "en@quot") settings_lua.ModifyInt("settings.lang_code", 1);
//	else if (lang_name == "fr") settings_lua.ModifyInt("settings.lang_code", 2);
//	else if (lang_name == "pt_br") settings_lua.ModifyInt("settings.lang_code", 3);
//	else if (lang_name == "es") settings_lua.ModifyInt("settings.lang_code", 4);
//	else if (lang_name == "de") settings_lua.ModifyInt("settings.lang_code", 5);
//	else settings_lua.ModifyInt("settings.lang_code", 1);

	// video
	settings_lua.OpenTable("settings");
	settings_lua.ModifyInt("video_settings.screen_resx", VideoManager->GetScreenWidth());
	settings_lua.ModifyInt("video_settings.screen_resy", VideoManager->GetScreenHeight());
	settings_lua.ModifyBool("video_settings.full_screen", VideoManager->IsFullscreen());
	//settings_lua.ModifyFloat("video_settings.brightness", VideoManager->GetGamma());

	// audio
	settings_lua.ModifyFloat("audio_settings.music_vol", AudioManager->GetMusicVolume());
	settings_lua.ModifyFloat("audio_settings.sound_vol", AudioManager->GetSoundVolume());

	// input
	settings_lua.ModifyInt("key_settings.up", InputManager->GetUpKey());
	settings_lua.ModifyInt("key_settings.down", InputManager->GetDownKey());
	settings_lua.ModifyInt("key_settings.left", InputManager->GetLeftKey());
	settings_lua.ModifyInt("key_settings.right", InputManager->GetRightKey());
	settings_lua.ModifyInt("key_settings.confirm", InputManager->GetConfirmKey());
	settings_lua.ModifyInt("key_settings.cancel", InputManager->GetCancelKey());
	settings_lua.ModifyInt("key_settings.menu", InputManager->GetMenuKey());
	settings_lua.ModifyInt("key_settings.swap", InputManager->GetSwapKey());
	settings_lua.ModifyInt("key_settings.left_select", InputManager->GetLeftSelectKey());
	settings_lua.ModifyInt("key_settings.right_select", InputManager->GetRightSelectKey());
	settings_lua.ModifyInt("key_settings.pause", InputManager->GetPauseKey());
	settings_lua.ModifyInt("joystick_settings.x_axis", InputManager->GetXAxisJoy());
	settings_lua.ModifyInt("joystick_settings.y_axis", InputManager->GetYAxisJoy());
	settings_lua.ModifyInt("joystick_settings.confirm", InputManager->GetConfirmJoy());
	settings_lua.ModifyInt("joystick_settings.cancel", InputManager->GetCancelJoy());
	settings_lua.ModifyInt("joystick_settings.menu", InputManager->GetMenuJoy());
	settings_lua.ModifyInt("joystick_settings.swap", InputManager->GetSwapJoy());
	settings_lua.ModifyInt("joystick_settings.left_select", InputManager->GetLeftSelectJoy());
	settings_lua.ModifyInt("joystick_settings.right_select", InputManager->GetRightSelectJoy());
	settings_lua.ModifyInt("joystick_settings.pause", InputManager->GetPauseJoy());

	// and save it!
	settings_lua.CommitChanges();
	settings_lua.CloseFile();

	_has_modified_settings = false;

	return true;
} // bool BootMode::_SaveSettingsFile(const std::string& filename)



vector<string> BootMode::_GetDirectoryListingUserProfilePath() {

	//get the entire directory listing for user data path
	vector<string> directory_listing = ListDirectory(GetUserProfilePath(), ".lua");

	if (directory_listing.empty()) {
		return directory_listing;
	}
	else {
		//as stated earlier this is for personalized profiles only
		directory_listing.erase(find(directory_listing.begin(), directory_listing.end(), "settings.lua"));

		//we also need to take off the .lua extension so end users do not see it
		for(uint32 i = 0; i < directory_listing.size(); i++)
			directory_listing.at(i).erase(directory_listing.at(i).size() - 4);

		return directory_listing;
	}
}
void BootMode::_AddProfileOptions(private_boot::BootMenu* menu)
{
	//setup the dimensions according to how many profiles we have available
	vector<std::string> profile_vector = _GetDirectoryListingUserProfilePath();
	menu->SetDimensions(300.0f, 200.0f, 1,profile_vector.size()+1, 1, profile_vector.size()+1);

	//add the options in for each file
	for (uint32 i = 0; i < profile_vector.size(); i++) {
		//this menu is for personalized profiles only do not include the default profile "restore defaults" already exists
		string filename = profile_vector.at(i);
		menu->AddOption(MakeUnicodeString(filename.c_str()), &BootMode::_OnLoadFile);
	}
}



void BootMode::_OverwriteProfile() {
	_has_modified_settings = true;

	//we subtract 1 to take into account the "new profile" option
	_SaveSettingsFile(_GetDirectoryListingUserProfilePath().at(_save_profile_menu.GetSelection() - 1) + ".lua");

	//if we got past the save settings without throwing an exception then we succeeded
	if (BOOT_DEBUG)
		cout << "BOOT: profile successfully overwritten " << _GetDirectoryListingUserProfilePath().at(_save_profile_menu.GetSelection() - 1) << endl;
}

// ****************************************************************************
// ***** BootMode input configuration methods
// ****************************************************************************

SDLKey BootMode::_WaitKeyPress() {
	SDL_Event event;
	while (SDL_WaitEvent(&event)) {
		if (event.type == SDL_KEYDOWN)
			break;
	}

	return event.key.keysym.sym;
}



uint8 BootMode::_WaitJoyPress() {
	SDL_Event event;
	while (SDL_WaitEvent(&event)) {
		if (event.type == SDL_JOYBUTTONDOWN)
			break;
	}

	return event.jbutton.button;
}



void BootMode::_RedefineUpKey() {
	_key_setting_function = &BootMode::_SetUpKey;
	_ShowMessageWindow(false);
}



void BootMode::_RedefineDownKey() {
	_key_setting_function = &BootMode::_SetDownKey;
	_ShowMessageWindow(false);
}



void BootMode::_RedefineLeftKey() {
	_key_setting_function = &BootMode::_SetLeftKey;
	_ShowMessageWindow(false);
}



void BootMode::_RedefineRightKey() {
	_key_setting_function = &BootMode::_SetRightKey;
	_ShowMessageWindow(false);
}



void BootMode::_RedefineConfirmKey() {
	_key_setting_function = &BootMode::_SetConfirmKey;
	_ShowMessageWindow(false);
}



void BootMode::_RedefineCancelKey() {
	_key_setting_function = &BootMode::_SetCancelKey;
	_ShowMessageWindow(false);
}



void BootMode::_RedefineMenuKey() {
	_key_setting_function = &BootMode::_SetMenuKey;
	_ShowMessageWindow(false);
}



void BootMode::_RedefineSwapKey() {
	_key_setting_function = &BootMode::_SetSwapKey;
	_ShowMessageWindow(false);
}



void BootMode::_RedefineLeftSelectKey() {
	_key_setting_function = &BootMode::_SetLeftSelectKey;
	_ShowMessageWindow(false);
}



void BootMode::_RedefineRightSelectKey() {
	_key_setting_function = &BootMode::_SetRightSelectKey;
	_ShowMessageWindow(false);
}



void BootMode::_RedefinePauseKey() {
	_key_setting_function = &BootMode::_SetPauseKey;
	_ShowMessageWindow(false);
}



void BootMode::_SetUpKey(const SDLKey &key) {
	InputManager->SetUpKey(key);
}



void BootMode::_SetDownKey(const SDLKey &key) {
	InputManager->SetDownKey(key);
}


void BootMode::_SetLeftKey(const SDLKey &key) {
	InputManager->SetLeftKey(key);
}



void BootMode::_SetRightKey(const SDLKey &key) {
	InputManager->SetRightKey(key);
}



void BootMode::_SetConfirmKey(const SDLKey &key) {
	InputManager->SetConfirmKey(key);
}



void BootMode::_SetCancelKey(const SDLKey &key) {
	InputManager->SetCancelKey(key);
}



void BootMode::_SetMenuKey(const SDLKey &key) {
	InputManager->SetMenuKey(key);
}



void BootMode::_SetSwapKey(const SDLKey &key) {
	InputManager->SetSwapKey(key);
}



void BootMode::_SetLeftSelectKey(const SDLKey &key) {
	InputManager->SetLeftSelectKey(key);
}



void BootMode::_SetRightSelectKey(const SDLKey &key) {
	InputManager->SetRightSelectKey(key);
}



void BootMode::_SetPauseKey(const SDLKey &key) {
	InputManager->SetPauseKey(key);
}



void BootMode::_RedefineXAxisJoy() {
	_joy_axis_setting_function = &BootMode::_SetXAxisJoy;
	_ShowMessageWindow(WAIT_JOY_AXIS);
	InputManager->ResetLastAxisMoved();
}



void BootMode::_RedefineYAxisJoy() {
	_joy_axis_setting_function = &BootMode::_SetYAxisJoy;
	_ShowMessageWindow(WAIT_JOY_AXIS);
	InputManager->ResetLastAxisMoved();
}



void BootMode::_RedefineThresholdJoy() {
	// TODO
}



void BootMode::_RedefineConfirmJoy() {
	_joy_setting_function = &BootMode::_SetConfirmJoy;
	_ShowMessageWindow(true);
}



void BootMode::_RedefineCancelJoy() {
	_joy_setting_function = &BootMode::_SetCancelJoy;
	_ShowMessageWindow(true);
}



void BootMode::_RedefineMenuJoy() {
	_joy_setting_function = &BootMode::_SetMenuJoy;
	_ShowMessageWindow(true);
}



void BootMode::_RedefineSwapJoy() {
	_joy_setting_function = &BootMode::_SetSwapJoy;
	_ShowMessageWindow(true);
}



void BootMode::_RedefineLeftSelectJoy() {
	_joy_setting_function = &BootMode::_SetLeftSelectJoy;
	_ShowMessageWindow(true);
}



void BootMode::_RedefineRightSelectJoy() {
	_joy_setting_function = &BootMode::_SetRightSelectJoy;
	_ShowMessageWindow(true);
}



void BootMode::_RedefinePauseJoy() {
	_joy_setting_function = &BootMode::_SetPauseJoy;
	_ShowMessageWindow(true);
}



void BootMode::_RedefineQuitJoy() {
	// TODO
}



void BootMode::_SetXAxisJoy(int8 axis) {
	InputManager->SetXAxisJoy(axis);
}



void BootMode::_SetYAxisJoy(int8 axis) {
	InputManager->SetYAxisJoy(axis);
}



void BootMode::_SetConfirmJoy(uint8 button) {
	InputManager->SetConfirmJoy(button);
}



void BootMode::_SetCancelJoy(uint8 button) {
	InputManager->SetCancelJoy(button);
}



void BootMode::_SetMenuJoy(uint8 button) {
	InputManager->SetMenuJoy(button);
}



void BootMode::_SetSwapJoy(uint8 button) {
	InputManager->SetSwapJoy(button);
}



void BootMode::_SetLeftSelectJoy(uint8 button) {
	InputManager->SetLeftSelectJoy(button);
}



void BootMode::_SetRightSelectJoy(uint8 button) {
	InputManager->SetRightSelectJoy(button);
}



void BootMode::_SetPauseJoy(uint8 button) {
	InputManager->SetPauseJoy(button);
}

} // namespace hoa_boot
