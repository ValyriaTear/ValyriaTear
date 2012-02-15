///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_treasure.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for map mode treasures.
*** ***************************************************************************/

// Allacrost engines
#include "input.h"
#include "mode_manager.h"
#include "system.h"
#include "video.h"

// Allacrost globals
#include "global.h"

// Other mode headers
#include "menu.h"

// Local map mode headers
#include "map.h"
#include "map_objects.h"
#include "map_treasure.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_input;
using namespace hoa_mode_manager;
using namespace hoa_system;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_global;
using namespace hoa_menu;

namespace hoa_map {

namespace private_map {

// -----------------------------------------------------------------------------
// ---------- MapTreasure Class Functions
// -----------------------------------------------------------------------------

MapTreasure::MapTreasure() :
	_taken(false),
	_drunes(0)
{}



MapTreasure::~MapTreasure() {
	for (uint32 i = 0; i < _objects_list.size(); i++) {
		delete _objects_list[i];
	}
}



bool MapTreasure::AddObject(uint32 id, uint32 quantity) {
	hoa_global::GlobalObject* obj = GlobalCreateNewObject(id, quantity);

	if (obj == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "invalid object id argument passed to function: " << id << endl;
		return false;
	}

	_objects_list.push_back(obj);
	return true;
}

// -----------------------------------------------------------------------------
// ---------- TreasureSupervisor class methods
// -----------------------------------------------------------------------------

TreasureSupervisor::TreasureSupervisor() :
	_treasure(NULL),
	_selection(ACTION_SELECTED),
	_window_title(UTranslate("Treasure Contents"), TextStyle("title24", Color::white, VIDEO_TEXT_SHADOW_DARK, 1, -2)),
	_selection_name(),
	_selection_icon(NULL)
{
	// Create the menu windows and option boxes used for the treasure menu and
	// align them at the appropriate locations on the screen
	_action_window.Create(768.0f, 64.0f, ~VIDEO_MENU_EDGE_BOTTOM);
	_action_window.SetPosition(512.0f, 460.0f);
	_action_window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
	_action_window.SetDisplayMode(VIDEO_MENU_INSTANT);

	_list_window.Create(768.0f, 236.0f);
	_list_window.SetPosition(512.0f, 516.0f);
	_list_window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
	_list_window.SetDisplayMode(VIDEO_MENU_INSTANT);

	_action_options.SetPosition(30.0f, 18.0f);
	_action_options.SetDimensions(726.0f, 32.0f, 1, 1, 1, 1);
	_action_options.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_action_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_action_options.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_action_options.SetSelectMode(VIDEO_SELECT_SINGLE);
	_action_options.SetCursorOffset(-50.0f, -25.0f);
	_action_options.SetTextStyle(TextStyle("title22", Color::white, VIDEO_TEXT_SHADOW_DARK, 1, -2));
	_action_options.AddOption(UTranslate("Finished"));
	_action_options.SetSelection(0);
	_action_options.SetOwner(&_action_window);

	_list_options.SetPosition(20.0f, 20.0f);
	_list_options.SetDimensions(726.0f, 200.0f, 1, 255, 1, 5);
	_list_options.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_list_options.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_list_options.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_list_options.SetSelectMode(VIDEO_SELECT_SINGLE);
	_list_options.SetCursorOffset(-50.0f, -25.0f);
	_list_options.SetTextStyle(TextStyle("text22", Color::white, VIDEO_TEXT_SHADOW_DARK, 1, -2));
	_list_options.SetOwner(&_list_window);
	// TODO: this currently does not work (text will be blank). Re-enable it once the scissoring bug is fixed in the video engine
// 	_list_options.Scissoring(true, true);

	_detail_textbox.SetPosition(20.0f, 90.0f);
	_detail_textbox.SetDimensions(726.0f, 128.0f);
	_detail_textbox.SetDisplaySpeed(50);
	_detail_textbox.SetTextStyle(TextStyle("text22", Color::white, VIDEO_TEXT_SHADOW_DARK, 1, -2));
	_detail_textbox.SetDisplayMode(VIDEO_TEXT_REVEAL);
	_detail_textbox.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_detail_textbox.SetOwner(&_list_window);

	_selection_name.SetStyle(TextStyle("text22", Color::white, VIDEO_TEXT_SHADOW_DARK, 1, -2));

	if (_drunes_icon.Load("img/icons/drunes.png") == false)
		IF_PRINT_WARNING(MAP_DEBUG) << "failed to load drunes icon for treasure menu" << endl;
} // TreasureSupervisor::TreasureSupervisor()



TreasureSupervisor::~TreasureSupervisor() {
	_action_window.Destroy();
	_list_window.Destroy();
}



void TreasureSupervisor::Initialize(TreasureObject* map_object) {
	if (map_object == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "function argument was NULL" << endl;
		return;
	}

	Initialize(map_object->GetTreasure());
}



void TreasureSupervisor::Initialize(MapTreasure* treasure) {
	if (treasure == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "function argument was NULL" << endl;
		return;
	}

	if (_treasure != NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "_treasure member was not NULL when method was called" << endl;
		return;
	}

	_treasure = treasure;
	MapMode::CurrentInstance()->PushState(STATE_TREASURE);

	// Construct the object list, including any drunes that were contained within the treasure
	if (_treasure->_drunes != 0) {
		_list_options.AddOption(MakeUnicodeString("<img/icons/drunes.png>       Drunes<R>" + NumberToString(_treasure->_drunes)));
	}

	for (uint32 i = 0; i < _treasure->_objects_list.size(); i++) {
		if (_treasure->_objects_list[i]->GetCount() > 1) {
			_list_options.AddOption(MakeUnicodeString("<" + _treasure->_objects_list[i]->GetIconImage().GetFilename() + ">       ") +
				_treasure->_objects_list[i]->GetName() +
				MakeUnicodeString("<R>x" + NumberToString(_treasure->_objects_list[i]->GetCount())));
		}
		else {
			_list_options.AddOption(MakeUnicodeString("<" + _treasure->_objects_list[i]->GetIconImage().GetFilename() + ">       ") +
				_treasure->_objects_list[i]->GetName());
		}
	}

	for (uint32 i = 0; i < _list_options.GetNumberOptions(); i++) {
		_list_options.GetEmbeddedImage(i)->SetDimensions(30.0f, 30.0f);
	}

	_action_options.SetSelection(0);
	_action_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	_list_options.SetSelection(0);
	_list_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

	_selection = ACTION_SELECTED;
	_action_window.Show();
	_list_window.Show();

	// Immediately add the drunes and objects to the player's inventory
	GlobalManager->AddDrunes(_treasure->_drunes);

	// The AddToInventory function will delete the pointer that it is given if that type of object
	// already exists in the inventory. Because we still require all of the object pointers to remain in
	// memory while the menu is being displayed, we check if an object exists in the inventory, increment
	// the inventory count if it does, and keep a record that we must delete the object once the menu is closed
	for (uint32 i = 0; i < _treasure->_objects_list.size(); i++) {
		GlobalObject* obj = _treasure->_objects_list[i];
		if (GlobalManager->IsObjectInInventory(obj->GetID()) == false) {
			GlobalManager->AddToInventory(_treasure->_objects_list[i]);
		}
		else {
			GlobalManager->IncrementObjectCount(obj->GetID(), obj->GetCount());
			_objects_to_delete.push_back(obj);
		}
	}
} // void TreasureSupervisor::Initialize(MapTreasure* treasure)



void TreasureSupervisor::Update() {
	_action_window.Update();
	_list_window.Update();
	_action_options.Update();
	_list_options.Update();
	_detail_textbox.Update();

	// Allow the user to go to menu mode at any time when the treasure menu is open
	if (InputManager->MenuPress()) {
		MenuMode *MM = new MenuMode(MapMode::CurrentInstance()->GetMapName(), MapMode::CurrentInstance()->GetLocationGraphic().GetFilename());
		ModeManager->Push(MM);
		return;
	}

	// Update the menu according to which sub-window is currently selected
	if (_selection == ACTION_SELECTED)
		_UpdateAction();
	else if (_selection == LIST_SELECTED)
		_UpdateList();
	else if (_selection == DETAIL_SELECTED)
		_UpdateDetail();
	else
		IF_PRINT_WARNING(MAP_DEBUG) << "unknown selection state: " << _selection << endl;
}



void TreasureSupervisor::Draw() {
	// We wait until the treasure is fully open before displaying any portions of the menu
	VideoManager->PushState();

	_action_window.Draw();
	if (_selection != DETAIL_SELECTED) {
		_action_options.Draw();
	}
	_list_window.Draw();
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
	VideoManager->Move(512.0f, 465.0f);
	_window_title.Draw();

	if (_selection == DETAIL_SELECTED) {
		VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);
		// Move to the upper left corner and draw the object icon
		if (_selection_icon != NULL) {
			VideoManager->Move(150.0f, 535.0f);
			_selection_icon->Draw();
		}

		// Draw the name of the selected object to the right of the icon
		VideoManager->MoveRelative(80.0f, 20.0f);
		_selection_name.Draw();

		_detail_textbox.Draw();
	}
	else {
		_list_options.Draw();
	}

	VideoManager->PopState();
} // void TreasureSupervisor::Draw()



void TreasureSupervisor::Finish() {
	for (uint32 i = 0; i < _objects_to_delete.size(); i++) {
		delete _objects_to_delete[i];
	}
	_objects_to_delete.clear();

	_treasure->_taken = true;
	_treasure->_drunes = 0;
	_treasure->_objects_list.clear();
	_treasure = NULL;

	_action_window.Hide();
	_list_window.Hide();
	_list_options.ClearOptions();

	MapMode::CurrentInstance()->PopState();
}



void TreasureSupervisor::_UpdateAction() {
	if (InputManager->ConfirmPress()) {
		if (_action_options.GetSelection() == 0) // "Take All" action
			Finish();
		else
			IF_PRINT_WARNING(MAP_DEBUG) << "unhandled action selection in OptionBox: " << _action_options.GetSelection() << endl;
	}

	else if (InputManager->LeftPress())
		_action_options.InputLeft();

	else if (InputManager->RightPress())
		_action_options.InputRight();

	else if (InputManager->UpPress()) {
		_selection = LIST_SELECTED;
		_list_options.SetSelection(_list_options.GetNumberOptions() - 1);
		_action_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
		_list_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	}

	else if (InputManager->DownPress()) {
		_selection = LIST_SELECTED;
		_list_options.SetSelection(0);
		_action_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
		_list_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	}
}



void TreasureSupervisor::_UpdateList() {
	if (InputManager->ConfirmPress()) {
		_selection = DETAIL_SELECTED;
		_list_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

		uint32 list_selection = _list_options.GetSelection();
		if (list_selection == 0 && _treasure->_drunes != 0) { // If true, the drunes have been selected
			_selection_name.SetText(UTranslate("Drunes"));
			_selection_icon = &_drunes_icon;
			_detail_textbox.SetDisplayText(UTranslate("With the additional ") + MakeUnicodeString(NumberToString(_treasure->_drunes)) +
			UTranslate(" drunes found in this treasure added, the party now holds a total of ") + MakeUnicodeString(NumberToString(GlobalManager->GetDrunes()))
			+ MakeUnicodeString(" drunes."));
		}
		else { // Otherwise, a GlobalObject is selected
			if (_treasure->_drunes != 0)
				list_selection--;
			_selection_name.SetText(_treasure->_objects_list[list_selection]->GetName());
			// TODO: this is not good practice. We should probably either remove the const status from the GetIconImage() call
			_selection_icon = const_cast<StillImage*>(&_treasure->_objects_list[list_selection]->GetIconImage());
			_detail_textbox.SetDisplayText(_treasure->_objects_list[list_selection]->GetDescription());
		}
	}

	else if (InputManager->CancelPress()) {
		_selection = ACTION_SELECTED;
		_action_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
		_list_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	}

	else if (InputManager->UpPress()) {
		if (_list_options.GetSelection() == 0) {
			_selection = ACTION_SELECTED;
			_action_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
			_list_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
		}
		else {
			_list_options.InputUp();
		}
	}

	else if (InputManager->DownPress()) {
		if (static_cast<uint32>(_list_options.GetSelection()) == (_list_options.GetNumberOptions() - 1)) {
			_selection = ACTION_SELECTED;
			_action_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
			_list_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
		}
		else {
			_list_options.InputDown();
		}
	}
}



void TreasureSupervisor::_UpdateDetail() {
	if (InputManager->ConfirmPress() || InputManager->CancelPress()) {
		if (_detail_textbox.IsFinished() == false) {
			_detail_textbox.ForceFinish();
		}
		else {
			_selection = LIST_SELECTED;
			_list_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
		}
	}
}

} // namespace private_map

} // namespace hoa_map
