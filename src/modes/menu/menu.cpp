///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    menu.cpp
*** \author  Daniel Steuernol steu@allacrost.org
*** \author  Andy Gardner chopperdave@allacrost.org
*** \brief   Source file for menu mode interface.
*** ***************************************************************************/

#include <iostream>
#include <sstream>

#include "system.h"
#include "input.h"
#include "audio.h"
#include "pause.h"

#include "save_mode.h" // TODO: remove this

#include "mode_manager.h"
#include "menu.h"


using namespace std;

using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_system;
using namespace hoa_mode_manager;
using namespace hoa_input;
using namespace hoa_global;
using namespace hoa_pause;

using namespace hoa_menu::private_menu;

namespace hoa_menu {

bool MENU_DEBUG = false;

MenuMode* MenuMode::_current_instance = NULL;

////////////////////////////////////////////////////////////////////////////////
// MenuMode class -- Initialization and Destruction Code
////////////////////////////////////////////////////////////////////////////////

MenuMode::MenuMode(ustring locale_name, string locale_image) :
	_message_window(NULL)
{
	if (MENU_DEBUG)
		cout << "MENU: MenuMode constructor invoked." << endl;

	_locale_name.SetStyle(TextStyle("title22"));
	_locale_name.SetText(locale_name);

	// Initialize the location graphic
	_locale_graphic.SetStatic(true);
	if (_locale_graphic.Load(locale_image, 500, 125) == false) {
		cerr << "MENU ERROR: failed to load locale graphic in MenuMode constructor: " << locale_image << endl;
		exit(1);
	}

	try {
		_saved_screen = VideoManager->CaptureScreen();
	}
	catch(Exception e) {
		cerr << e.ToString() << endl;
	}

	_current_window = WINDOW_INVENTORY;

	GlobalParty & characters = *GlobalManager->GetActiveParty();

	// Setup character windows based on active party size
	switch (characters.GetPartySize()) {
		case 4: _character_window3.SetCharacter(dynamic_cast<GlobalCharacter*>(characters.GetActorAtIndex(3)));
		case 3: _character_window2.SetCharacter(dynamic_cast<GlobalCharacter*>(characters.GetActorAtIndex(2)));
		case 2: _character_window1.SetCharacter(dynamic_cast<GlobalCharacter*>(characters.GetActorAtIndex(1)));
		case 1: _character_window0.SetCharacter(dynamic_cast<GlobalCharacter*>(characters.GetActorAtIndex(0)));
			break;
		default:
			cerr << "MENU ERROR: no characters in party!" << endl;
			exit(1);
	}

	//////////// Setup the menu windows
	uint32 start_x = (1024 - 800) / 2 - 40;
	uint32 start_y = (768 - 600) / 2 + 15;
	uint32 win_width = 208;

	//The bottom window for the menu
	_bottom_window.Create(static_cast<float>(win_width * 4 + 16), 140 + 16, VIDEO_MENU_EDGE_ALL);
	_bottom_window.SetPosition(static_cast<float>(start_x), static_cast<float>(start_y + 442));

	// Width of each character window is 360 px.
	// Each char window will have an additional 16 px for the left border
	// The 4th (last) char window will have another 16 px for the right border
	// Height of the char window is 98 px.
	// The bottom window in the main view is 192 px high, and the full width which will be 216 * 4 + 16
	_character_window0.Create(360, 98, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM);
	_character_window0.SetPosition(static_cast<float>(start_x), static_cast<float>(start_y + 10));

	_character_window1.Create(360, 98, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM | VIDEO_MENU_EDGE_TOP);
	_character_window1.SetPosition(static_cast<float>(start_x), static_cast<float>(start_y + 118));

	_character_window2.Create(360, 98, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM | VIDEO_MENU_EDGE_TOP);
	_character_window2.SetPosition(static_cast<float>(start_x), static_cast<float>(start_y + 226));

	_character_window3.Create(360, 98, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_TOP | VIDEO_MENU_EDGE_BOTTOM);
	_character_window3.SetPosition(static_cast<float>(start_x), static_cast<float>(start_y + 334));

	_main_options_window.Create(static_cast<float>(win_width * 4 + 16), 60, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM);
	_main_options_window.SetPosition(static_cast<float>(start_x), static_cast<float>(start_y - 50));

	// Set up the status window
	_status_window.Create(static_cast<float>(win_width * 4 + 16), 448, VIDEO_MENU_EDGE_ALL);
	_status_window.SetPosition(static_cast<float>(start_x), static_cast<float>(start_y + 10));

	//Set up the skills window
	_skills_window.Create(static_cast<float>(win_width * 4 + 16), 448, VIDEO_MENU_EDGE_ALL);
	_skills_window.SetPosition(static_cast<float>(start_x), static_cast<float>(start_y + 10));

	//Set up the equipment window
	_equip_window.Create(static_cast<float>(win_width * 4 + 16), 448, VIDEO_MENU_EDGE_ALL);
	_equip_window.SetPosition(static_cast<float>(start_x), static_cast<float>(start_y + 10));

	// Set up the inventory window
	_inventory_window.Create(static_cast<float>(win_width * 4 + 16), 448, VIDEO_MENU_EDGE_ALL);
	_inventory_window.SetPosition(static_cast<float>(start_x), static_cast<float>(start_y + 10));

// 	// TODO: Set up the formation window
	_formation_window.Create(static_cast<float>(win_width * 4 + 16), 448, VIDEO_MENU_EDGE_ALL);
	_formation_window.SetPosition(static_cast<float>(start_x), static_cast<float>(start_y + 10));


	// Set the menu to show the main options
	_current_menu_showing = SHOW_MAIN;
	_current_menu = &_main_options;

	// Load menu sounds
	_menu_sounds["confirm"] = SoundDescriptor();
	_menu_sounds["confirm"].LoadAudio("snd/confirm.wav");
	_menu_sounds["cancel"] = SoundDescriptor();
	_menu_sounds["cancel"].LoadAudio("snd/cancel.wav");
	_menu_sounds["bump"] = SoundDescriptor();
	_menu_sounds["bump"].LoadAudio("snd/bump.wav");

	if (_current_instance != NULL) {
		if (MENU_DEBUG)
			cerr << "MENU WARNING: attempting to create a new instance of MenuMode when one already seems to exist" << endl;
	}
	_current_instance = this;
} // MenuMode::MenuMode()



MenuMode::~MenuMode() {
	if (MENU_DEBUG)
		cout << "MENU: MenuMode destructor invoked." << endl;

	// Destroy all menu windows
	_bottom_window.Destroy();
	_character_window0.Destroy();
	_character_window1.Destroy();
	_character_window2.Destroy();
	_character_window3.Destroy();
	_inventory_window.Destroy();
	_status_window.Destroy();
	_skills_window.Destroy();
	_main_options_window.Destroy();
	_equip_window.Destroy();
	_formation_window.Destroy();

	// Free sounds
	_menu_sounds["confirm"].FreeAudio();
	_menu_sounds["bump"].FreeAudio();
	_menu_sounds["cancel"].FreeAudio();

	_current_instance = NULL;

	if (_message_window != NULL)
		delete _message_window;
} // MenuMode::~MenuMode()


// Resets configuration/data for the class as appropriate
void MenuMode::Reset() {
	// Top left corner coordinates in menu mode are always (0,0)
	VideoManager->SetCoordSys(0.0f, 1023.0f, 767.0f, 0.0f);

	// Show all windows (make them visible)
	_bottom_window.Show();
	_main_options_window.Show();
	_character_window0.Show();
	_character_window1.Show();
	_character_window2.Show();
	_character_window3.Show();
	_inventory_window.Show();
	_status_window.Show();
	_skills_window.Show();
	_equip_window.Show();
	_formation_window.Show();

	_inventory_window.Activate(false);
	_active_window = &_inventory_window;

	// Setup OptionBoxes
	_SetupMainOptionBox();
	_SetupInventoryOptionBox();
	_SetupSkillsOptionBox();
	_SetupStatusOptionBox();
	_SetupOptionsOptionBox();
	_SetupSaveOptionBox();
	_SetupEquipOptionBox();
	_SetupFormationOptionBox();
} // void MenuMode::Reset()

////////////////////////////////////////////////////////////////////////////////
// MenuMode class -- Update Code
////////////////////////////////////////////////////////////////////////////////

void MenuMode::Update() {
	if (InputManager->QuitPress() == true) {
		ModeManager->Push(new PauseMode(true));
		return;
	}
	else if (InputManager->PausePress() == true) {
		ModeManager->Push(new PauseMode(false));
		return;
	}

	// check the message window
	if (_message_window != NULL)
	{
		_message_window->Update();
		if (InputManager->ConfirmPress() || InputManager->CancelPress())
		{
			delete _message_window;
			_message_window = NULL;
		}
		return;
	}

	if (_active_window->IsActive())
	{
		_active_window->Update();
		return;
	}

	if (InputManager->CancelPress()) {
		// Play sound.
		_menu_sounds["cancel"].Play();
		// If in main menu, return to previous Mode, else return to main menu.
		if (_current_menu_showing == SHOW_MAIN) {
			ModeManager->Pop();
		}
		else {
			_current_menu_showing = SHOW_MAIN;
			_current_menu = &_main_options;
			_current_menu->Update();
		}
	}
	else if (InputManager->ConfirmPress()) {
		// Play Sound
		if (_current_menu->IsEnabled(_current_menu->GetSelection()))
			_menu_sounds["confirm"].Play();

		_current_menu->InputConfirm();
	}
	else if (InputManager->LeftPress()) {
		// Play Sound
		_current_menu->InputLeft();
	}
	else if (InputManager->RightPress()) {
		// Play Sound
		_current_menu->InputRight();
	}
	else if (InputManager->SwapPress()) {
		// Play Sound
		_char_select.SetSelection((_char_select.GetSelection() + 1) %
			GlobalManager->GetActiveParty()->GetPartySize());
	}

	// Get the latest event from the current menu
	int32 event = _current_menu->GetEvent();

	// If confirm was pressed
	if (event == VIDEO_OPTION_CONFIRM) {
		// Handle options for the current menu
		switch (_current_menu_showing) {
			case SHOW_MAIN:
				_HandleMainMenu();
				break;

			case SHOW_INVENTORY:
				_HandleInventoryMenu();
				break;

			case SHOW_SKILLS:
				_HandleSkillsMenu();
				break;

			case SHOW_STATUS:
				_HandleStatusMenu();
				break;

			case SHOW_EQUIP:
				_HandleEquipMenu();
				break;

			case SHOW_FORMATION:
				_HandleFormationMenu();
				break;

			/*case SHOW_OPTIONS:
				_HandleOptionsMenu();
				break;*/

			case SHOW_SAVE:
				_HandleSaveMenu();
				break;

			default:
				cerr << "MENU: ERROR: Invalid menu showing!" << endl;
				break;
		} // switch (_current_menu_showing)
		_GetNextActiveWindow();
	} // if VIDEO_OPTION_CONFIRM

	_current_menu->Update();

} // void MenuMode::Update()

////////////////////////////////////////////////////////////////////////////////
// MenuMode class -- Draw Code
////////////////////////////////////////////////////////////////////////////////
void MenuMode::Draw() {
	// Draw the saved screen background
	// For that, set the system coordinates to the size of the window (same with the save-screen)
	int32 width = VideoManager->GetScreenWidth();
	int32 height = VideoManager->GetScreenHeight();
	VideoManager->SetCoordSys(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
	Color grayed(0.35f, 0.35f, 0.35f, 1.0f);
	VideoManager->Move(0.0f, 0.0f);
	_saved_screen.Draw();

	// Restore the Coordinate system (that one is menu mode coodinate system)
	VideoManager->SetCoordSys(0.0f, 1024.0f, 768.0f, 0.0f);


	uint32 draw_window;

	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);

	// Move to the top left corner
	VideoManager->Move(0.0f, 0.0f);

	_main_options_window.Draw();
	_DrawBottomMenu();

	// Detects which option is highlighted in main menu choices and sets that to the current window
	// to draw
	if (_current_menu_showing == SHOW_MAIN) {
		draw_window = _current_menu->GetSelection() + 1;
	}
	else {
		draw_window = _current_menu_showing;
	}

	// Draw the chosen window
	switch (draw_window) {
		case SHOW_MAIN:
			break;

		case SHOW_INVENTORY:
			_inventory_window.Draw();
			break;

		case SHOW_STATUS:
			_status_window.Draw();
			break;

		case SHOW_SKILLS:
			_skills_window.Draw();
			break;

		case SHOW_EQUIP:
			_equip_window.Draw();
			break;

		/*case SHOW_OPTIONS:
			_HandleOptionsMenu();
			break;*/

		case SHOW_SAVE:
		case SHOW_EXIT:
		case SHOW_FORMATION:
			_formation_window.Draw();
			break;
	} // switch draw_window

	// Draw character windows
	_character_window0.Draw();
	_character_window1.Draw();
	_character_window2.Draw();
	_character_window3.Draw();

	// Draw currently active options box
	_current_menu->Draw();

	// Draw message window if it's active
	if (_message_window != NULL)
		_message_window->Draw();
} // void MenuMode::Draw()


//FIX ME:  Adjust for new layout
void MenuMode::_HandleMainMenu() {
	switch (_main_options.GetSelection()) {
		case MAIN_INVENTORY:
			_current_menu_showing = SHOW_INVENTORY;
			_current_menu = &_menu_inventory;
			break;

		case MAIN_SKILLS:
			_current_menu_showing = SHOW_SKILLS;
			_current_menu = &_menu_skills;
			break;

		/*case MAIN_OPTIONS:
			_current_menu_showing = SHOW_OPTIONS;
			_current_menu = &_menu_options;
			break;*/

		case MAIN_FORMATION:
			_current_menu_showing = SHOW_FORMATION;
			_current_menu = &_menu_formation;
			break;

		case MAIN_STATUS:
			_current_menu_showing = SHOW_STATUS;
			_current_menu = &_menu_status;
			break;

		case MAIN_EQUIP:
			_current_menu_showing = SHOW_EQUIP;
			_current_menu = &_menu_equip;
			break;

		case MAIN_SAVE:
//			_current_menu_showing = SHOW_SAVE;
//			_current_menu = &_menu_save;
		{	hoa_save::SaveMode *SVM = new hoa_save::SaveMode(true);
			ModeManager->Push(SVM);
			break;
		} // end scope

		default:
			cerr << "MENU ERROR: Invalid option in MenuMode::_HandleMainMenu()" << endl;
			break;
	}
} // void MenuMode::_HandleMainMenu()

void MenuMode::_HandleInventoryMenu() {
	switch (_menu_inventory.GetSelection()) {
		case INV_USE:
			if (GlobalManager->GetInventory()->size() == 0)
				return;
			_inventory_window.Activate(true);
			break;

/*		case INV_SORT:
			// TODO: Handle the sort inventory comand
			cout << "MENU: Inventory sort command!" << endl;
			break;*/

		case INV_BACK:
			_current_menu_showing = SHOW_MAIN;
			_current_menu = &_main_options;
			break;

		default:
			cerr << "MENU ERROR: Invalid option in MenuMode::_HandleInventoryMenu()" << endl;
			break;
	}
}

void MenuMode::_SetupOptionBoxCommonSettings(OptionBox *ob) {
	// Set all the default options
	ob->SetTextStyle(TextStyle("title22"));
	ob->SetPosition(142.0f, 85.0f);
	ob->SetDimensions(115.0f, 50.0f, 1, 1, 1, 1);
	ob->SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	ob->SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	ob->SetSelectMode(VIDEO_SELECT_SINGLE);
	ob->SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	ob->SetCursorOffset(-52.0f, -20.0f);
}



void MenuMode::_SetupMainOptionBox() {
	// Setup the main options box
	_SetupOptionBoxCommonSettings(&_main_options);
	_main_options.SetDimensions(745.0f, 50.0f, MAIN_SIZE, 1, 6, 1);

	// Generate the strings
	vector<ustring> options;
	options.push_back(UTranslate("Inventory"));
	options.push_back(UTranslate("Skills"));
	options.push_back(UTranslate("Equip"));
	options.push_back(UTranslate("Status"));
	options.push_back(UTranslate("Save"));
	options.push_back(UTranslate("Formation"));

	// Add strings and set default selection.
	_main_options.SetOptions(options);
	_main_options.SetSelection(MAIN_INVENTORY);
}



void MenuMode::_SetupInventoryOptionBox() {
	// Setup the option box
	_SetupOptionBoxCommonSettings(&_menu_inventory);
	_menu_inventory.SetDimensions(115.0f, 50.0f, INV_SIZE, 1, INV_SIZE, 1);

	// Generate the strings
	vector<ustring> options;
	options.push_back(UTranslate("Use"));
//	options.push_back(UTranslate("Sort"));
	options.push_back(UTranslate("Back"));

	// Add strings and set default selection.
	_menu_inventory.SetOptions(options);
	_menu_inventory.SetSelection(INV_USE);
}



void MenuMode::_SetupSkillsOptionBox() {
	// Setup the option box
	_SetupOptionBoxCommonSettings(&_menu_skills);
	_menu_skills.SetDimensions(115.0f, 50.0f, SKILLS_SIZE, 1, SKILLS_SIZE, 1);

	// Generate the strings
	vector<ustring> options;
	options.push_back(UTranslate("Use"));
	options.push_back(UTranslate("Back"));

	// Add strings and set default selection.
	_menu_skills.SetOptions(options);
	_menu_skills.SetSelection(SKILLS_USE);
}

void MenuMode::_SetupStatusOptionBox() {
	// Setup the status option box
	_SetupOptionBoxCommonSettings(&_menu_status);
	_menu_status.SetDimensions(115.0f, 50.0f, STATUS_SIZE, 1, STATUS_SIZE, 1);

	// Generate the strings
	vector<ustring> options;
	options.push_back(UTranslate("View"));
	options.push_back(UTranslate("Back"));

	// Add strings and set default selection.
	_menu_status.SetOptions(options);
	_menu_status.SetSelection(STATUS_VIEW);
}



void MenuMode::_SetupOptionsOptionBox() {
	// Setup the options option box
	_SetupOptionBoxCommonSettings(&_menu_options);
	_menu_options.SetDimensions(115.0f, 50.0f, OPTIONS_SIZE, 1, OPTIONS_SIZE, 1);

	// Generate the strings
	vector<ustring> options;
	options.push_back(UTranslate("Edit"));
	options.push_back(UTranslate("Save"));
	options.push_back(UTranslate("Back"));

	// Add strings and set default selection.
	_menu_options.SetOptions(options);
	_menu_options.SetSelection(OPTIONS_EDIT);
}

void MenuMode::_SetupSaveOptionBox() {
	// setup the save options box
	_SetupOptionBoxCommonSettings(&_menu_save);
// 	_menu_save.SetSize(SAVE_SIZE, 1);

	// Generate the strings
	vector<ustring> options;
	options.push_back(UTranslate("Save"));
	options.push_back(UTranslate("Back"));

	// Add strings and set default selection.
	_menu_save.SetOptions(options);
	_menu_save.SetSelection(SAVE_SAVE);
}

void MenuMode::_SetupFormationOptionBox() {
	// setup the save options box
	_SetupOptionBoxCommonSettings(&_menu_formation);
	_menu_formation.SetDimensions(115.0f, 50.0f, FORMATION_SIZE, 1, FORMATION_SIZE, 1);

	// Generate the strings
	vector<ustring> options;
	options.push_back(UTranslate("Switch"));
	options.push_back(UTranslate("Back"));

	// Add strings and set default selection.
	_menu_formation.SetOptions(options);
	_menu_formation.SetSelection(FORMATION_SWITCH);
}

void MenuMode::_SetupEquipOptionBox() {
	// Setup the status option box
	_SetupOptionBoxCommonSettings(&_menu_equip);
	_menu_equip.SetDimensions(195.0f, 50.0f, EQUIP_SIZE, 1, EQUIP_SIZE, 1);

	// Generate the strings
	vector<ustring> options;
	options.push_back(UTranslate("Equip"));
	options.push_back(UTranslate("Remove"));
	options.push_back(UTranslate("Back"));

	// Add strings and set default selection.
	_menu_equip.SetOptions(options);
	_menu_equip.SetSelection(EQUIP_EQUIP);

}


void MenuMode::_HandleSkillsMenu() {
	switch (_menu_skills.GetSelection()) {
		case SKILLS_BACK:
			_current_menu_showing = SHOW_MAIN;
			_current_menu = &_main_options;
			break;

		case SKILLS_USE:
			_skills_window.Activate(true);
			break;

		default:
			cerr << "MENU ERROR: Invalid option in MenuMode::_HandleSkillsMenu()" << endl;
			break;
	}
}


void MenuMode::_HandleStatusMenu() {
	switch (_menu_status.GetSelection()) {
		case STATUS_VIEW:
			_status_window.Activate(true);
			break;

		case STATUS_BACK:
			_current_menu_showing = SHOW_MAIN;
			_current_menu = &_main_options;
			break;

		default:
			cerr << "MENU ERROR: Invalid option in MenuMode::_HandleStatusMenu()" << endl;
			break;
	}
}

void MenuMode::_HandleOptionsMenu() {
	switch (_menu_options.GetSelection()) {
		case OPTIONS_EDIT:
			// TODO: Handle the Options - Edit command
			cout << "MENU: Options - Edit command!" << endl;
			break;

		case OPTIONS_SAVE:
			// TODO: Handle the Options - Save command
			cout << "MENU: Options - Save command!" << endl;
			break;

		case OPTIONS_BACK:
			_current_menu_showing = SHOW_MAIN;
			_current_menu = &_main_options;
			break;

		default:
			cerr << "MENU ERROR: Invalid option in MenuMode::_HandleOptionsMenu()" << endl;
			break;
	}
}



void MenuMode::_HandleFormationMenu() {
	switch (_menu_formation.GetSelection()) {
		case FORMATION_SWITCH:
			_formation_window._char_select.SetSelection(0);
			_formation_window.Activate(true);
			break;

		case FORMATION_BACK:
			_current_menu_showing = SHOW_MAIN;
			_current_menu = &_main_options;
			break;

		default:
			cerr << "MENU ERROR: Invalid option in MenuMode::_HandleFormationMenu()" << endl;
			break;
	}
}



void MenuMode::_HandleSaveMenu() {
	string file_name;
	switch (_menu_save.GetSelection()) {
		case SAVE_SAVE: {
			hoa_save::SaveMode *SVM = new hoa_save::SaveMode(true);
			ModeManager->Push(SVM);
			break;
		}

		case SAVE_BACK: {
			_current_menu_showing = SHOW_MAIN;
			_current_menu = &_main_options;
			break;
		}

		default: {
			cerr << "MENU ERROR: Invalid option in MenuMode::_HandleSaveMenu()" << endl;
		}
	}
}

void MenuMode::_HandleEquipMenu() {
	switch (_menu_equip.GetSelection()) {
		case EQUIP_EQUIP:
			_equip_window.Activate(true);
			break;

		case EQUIP_REMOVE:
			// TODO: Handle the remove command
			cout << "MENU: Equip - Remove command!" << endl;
			break;

		case EQUIP_BACK:
			_current_menu_showing = SHOW_MAIN;
			_current_menu = &_main_options;
			break;

		default:
			cerr << "MENU ERROR: Invalid option in MenuMode::_HandleEquipMenu()" << endl;
			break;
	}
}



void MenuMode::_GetNextActiveWindow()
{
	switch (_current_menu_showing)
	{
	case SHOW_MAIN:
	case SHOW_INVENTORY:
		_active_window = &_inventory_window;
		break;
	case SHOW_EQUIP:
		_active_window = &_equip_window;
		break;
	case SHOW_SKILLS:
		_active_window = &_skills_window;
		break;
	case SHOW_FORMATION:
		_active_window = &_formation_window;
		break;
	case SHOW_STATUS:
		_active_window = &_status_window;
		break;
	}
}


//FIX ME:  Make dynamic, move category id and select state enums to this class
void MenuMode::_DrawBottomMenu() {
	_bottom_window.Draw();

	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
	VideoManager->Move(150, 580);

	if (_current_menu_showing == SHOW_INVENTORY ) {
		if (_inventory_window._active_box == ITEM_ACTIVE_LIST) {
			GlobalObject* obj = _inventory_window._item_objects[ _inventory_window._inventory_items.GetSelection() ];

			VideoManager->SetDrawFlags(VIDEO_X_LEFT,VIDEO_Y_CENTER,0);

			VideoManager->Move(100, 600);
			obj->GetIconImage().Draw();
			VideoManager->MoveRelative(65, 0);
			VideoManager->Text()->Draw(obj->GetName());
			VideoManager->SetDrawFlags(VIDEO_X_LEFT,VIDEO_Y_BOTTOM,0);
			_inventory_window._description.Draw();
		} // if ITEM_ACTIVE_LIST
	} // if SHOW_INVENTORY
	else if (_current_menu_showing == SHOW_SKILLS ) {
		_skills_window._description.Draw();
	} // if SHOW_SKILLS
	else if (_current_menu_showing == SHOW_EQUIP) {
		GlobalCharacter* ch = dynamic_cast<GlobalCharacter*>(GlobalManager->GetActiveParty()->GetActorAtIndex(_equip_window._char_select.GetSelection()));
		VideoManager->Text()->Draw(UTranslate("STR: ") + MakeUnicodeString(NumberToString(ch->GetStrength())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("VIG: ") + MakeUnicodeString(NumberToString(ch->GetVigor())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("FRT: ") + MakeUnicodeString(NumberToString(ch->GetFortitude())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("PRO: ") + MakeUnicodeString(NumberToString(ch->GetProtection())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("AGI: ") + MakeUnicodeString(NumberToString(ch->GetAgility())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("EVD: ") + MakeUnicodeString(NumberToString(ch->GetEvade()) + "%"));

		VideoManager->Move(310, 577);

		VideoManager->Text()->Draw(UTranslate("Current Equipment:"));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("Weapon"));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("Head"));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("Torso"));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("Arm"));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("Legs"));

		VideoManager->Move(400, 577);

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("PHYS ATK: ") + MakeUnicodeString(NumberToString(ch->GetWeaponEquipped()->GetPhysicalAttack())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("PHYS DEF: ") + MakeUnicodeString(NumberToString(ch->GetHeadArmorEquipped()->GetPhysicalDefense())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("PHYS DEF: ") + MakeUnicodeString(NumberToString(ch->GetTorsoArmorEquipped()->GetPhysicalDefense())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("PHYS DEF: ") + MakeUnicodeString(NumberToString(ch->GetArmArmorEquipped()->GetPhysicalDefense())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("PHYS DEF: ") + MakeUnicodeString(NumberToString(ch->GetLegArmorEquipped()->GetPhysicalDefense())));

		VideoManager->Move(550, 577);

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("META ATK: ") + MakeUnicodeString(NumberToString(ch->GetWeaponEquipped()->GetMetaphysicalAttack())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("META DEF: ") + MakeUnicodeString(NumberToString(ch->GetHeadArmorEquipped()->GetMetaphysicalDefense())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("META DEF: ") + MakeUnicodeString(NumberToString(ch->GetTorsoArmorEquipped()->GetMetaphysicalDefense())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("META DEF: ") + MakeUnicodeString(NumberToString(ch->GetArmArmorEquipped()->GetMetaphysicalDefense())));

		VideoManager->MoveRelative(0, 20);
		VideoManager->Text()->Draw(UTranslate("META DEF: ") + MakeUnicodeString(NumberToString(ch->GetLegArmorEquipped()->GetMetaphysicalDefense())));
		VideoManager->SetDrawFlags(VIDEO_X_CENTER,VIDEO_Y_BOTTOM,0);


		if (_equip_window._active_box == EQUIP_ACTIVE_LIST) {
			VideoManager->Move(755, 577);

			switch (_equip_window._equip_select.GetSelection()) {
				case EQUIP_WEAPON:
				{
					GlobalWeapon* weapon = GlobalManager->GetInventoryWeapons()->at(_equip_window._equip_list.GetSelection());

					VideoManager->Text()->Draw(weapon->GetName());
					VideoManager->MoveRelative(0, 20);

					VideoManager->Text()->Draw(UTranslate("PHYS ATK:"));
					VideoManager->MoveRelative(0, 20);
					VideoManager->Text()->Draw(MakeUnicodeString(NumberToString(weapon->GetPhysicalAttack())));
					VideoManager->MoveRelative(0, 20);

					VideoManager->Text()->Draw(UTranslate("META ATK:"));
					VideoManager->MoveRelative(0, 20);
					VideoManager->Text()->Draw(MakeUnicodeString(NumberToString(weapon->GetMetaphysicalAttack())));
					VideoManager->MoveRelative(0, 20);

					break;
				} // case EQUIP_WEAPON
				case EQUIP_HEADGEAR:
				{
					GlobalArmor* armor = GlobalManager->GetInventoryHeadArmor()->at(_equip_window._equip_list.GetSelection());

					VideoManager->Text()->Draw(armor->GetName());
					VideoManager->MoveRelative(0, 20);

					VideoManager->Text()->Draw(UTranslate("PHYS DEF:"));
					VideoManager->MoveRelative(0, 20);
					VideoManager->Text()->Draw(MakeUnicodeString(NumberToString(armor->GetPhysicalDefense())));
					VideoManager->MoveRelative(0, 20);

					VideoManager->Text()->Draw(UTranslate("META DEF:"));
					VideoManager->MoveRelative(0, 20);
					VideoManager->Text()->Draw(MakeUnicodeString(NumberToString(armor->GetMetaphysicalDefense())));
					VideoManager->MoveRelative(0, 20);

					break;
				} // case EQUIP_HEADGEAR
				case EQUIP_BODYARMOR:
				{
					GlobalArmor* armor = GlobalManager->GetInventoryTorsoArmor()->at(_equip_window._equip_list.GetSelection());

					VideoManager->Text()->Draw(armor->GetName());
					VideoManager->MoveRelative(0, 20);

					VideoManager->Text()->Draw(UTranslate("PHYS DEF:"));
					VideoManager->MoveRelative(0, 20);
					VideoManager->Text()->Draw(MakeUnicodeString(NumberToString(armor->GetPhysicalDefense())));
					VideoManager->MoveRelative(0, 20);

					VideoManager->Text()->Draw(UTranslate("META DEF:"));
					VideoManager->MoveRelative(0, 20);
					VideoManager->Text()->Draw(MakeUnicodeString(NumberToString(armor->GetMetaphysicalDefense())));
					VideoManager->MoveRelative(0, 20);

					break;
				} // case EQUIP_BODYARMOR
				case EQUIP_OFFHAND:
				{
					GlobalArmor* armor = GlobalManager->GetInventoryArmArmor()->at(_equip_window._equip_list.GetSelection());

					VideoManager->Text()->Draw(armor->GetName());
					VideoManager->MoveRelative(0, 20);

					VideoManager->Text()->Draw(UTranslate("PHYS DEF:"));
					VideoManager->MoveRelative(0, 20);
					VideoManager->Text()->Draw(MakeUnicodeString(NumberToString(armor->GetPhysicalDefense())));
					VideoManager->MoveRelative(0, 20);

					VideoManager->Text()->Draw(UTranslate("META DEF:"));
					VideoManager->MoveRelative(0, 20);
					VideoManager->Text()->Draw(MakeUnicodeString(NumberToString(armor->GetMetaphysicalDefense())));
					VideoManager->MoveRelative(0, 20);

					break;
				} // case EQUIP_OFFHAND
				case EQUIP_LEGGINGS:
				{
					GlobalArmor* armor = GlobalManager->GetInventoryLegArmor()->at(_equip_window._equip_list.GetSelection());

					VideoManager->Text()->Draw(armor->GetName());
					VideoManager->MoveRelative(0, 20);

					VideoManager->Text()->Draw(UTranslate("PHYS DEF:"));
					VideoManager->MoveRelative(0, 20);
					VideoManager->Text()->Draw(MakeUnicodeString(NumberToString(armor->GetPhysicalDefense())));
					VideoManager->MoveRelative(0, 20);

					VideoManager->Text()->Draw(UTranslate("META DEF:"));
					VideoManager->MoveRelative(0, 20);
					VideoManager->Text()->Draw(MakeUnicodeString(NumberToString(armor->GetMetaphysicalDefense())));
					VideoManager->MoveRelative(0, 20);

					break;
				} // case EQUIP_LEGGINGS

				default:
					break;
			} // switch
		} // if EQUIP_ACTIVE_LIST
	} // if SHOW_EQUIP
	else {
		// Display Location
		_locale_name.Draw();

		// Draw Played Time
		VideoManager->MoveRelative(-40, 60);
		std::ostringstream os_time;
		uint8 hours = SystemManager->GetPlayHours();
		uint8 minutes = SystemManager->GetPlayMinutes();
		uint8 seconds = SystemManager->GetPlaySeconds();
		os_time << (hours < 10 ? "0" : "") << static_cast<uint32>(hours) << ":";
		os_time << (minutes < 10 ? "0" : "") << static_cast<uint32>(minutes) << ":";
		os_time << (seconds < 10 ? "0" : "") << static_cast<uint32>(seconds);

		std::string time = std::string("Time: ") + os_time.str();
		VideoManager->Text()->Draw(MakeUnicodeString(time));

		// Display the current funds that the party has
		VideoManager->MoveRelative(0, 30);
		VideoManager->Text()->Draw(UTranslate("Drunes: ") + MakeUnicodeString(NumberToString(GlobalManager->GetDrunes())));

		VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, 0);
		VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
		VideoManager->Move(390, 685);
		_locale_graphic.Draw();
	}
} // void MenuMode::_DrawBottomMenu()


void MenuMode::_DrawItemListHeader()
{ }

} // namespace hoa_menu
