///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/*!****************************************************************************
 * \file    menu_views.cpp
 * \author  Daniel Steuernol steu@allacrost.org
 * \author  Andy Gardner chopperdave@allacrost.org
 * \brief   Source file for various menu views.
 *****************************************************************************/

#include <iostream>
#include <sstream>

#include "audio.h"
#include "input.h"
#include "system.h"

#include "menu.h"

using namespace std;
using namespace hoa_menu::private_menu;
using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_global;
using namespace hoa_input;
using namespace hoa_system;

namespace hoa_menu {

namespace private_menu {


////////////////////////////////////////////////////////////////////////////////
// CharacterWindow Class
////////////////////////////////////////////////////////////////////////////////

CharacterWindow::CharacterWindow() : _char_id(GLOBAL_CHARACTER_INVALID) {
}



CharacterWindow::~CharacterWindow() {
}



void CharacterWindow::SetCharacter(GlobalCharacter *character) {
	_char_id = character->GetID();

	_portrait.SetStatic(true);
	_portrait.Load("img/portraits/menu/" + character->GetFilename() + "_small.png", 100, 100);
} // void CharacterWindow::SetCharacter(GlobalCharacter *character)



// Draw the window to the screen
void CharacterWindow::Draw() {
	// Call parent Draw method, if failed pass on fail result
	MenuWindow::Draw();

	// check to see if this window is an actual character
	if (_char_id == hoa_global::GLOBAL_CHARACTER_INVALID)
		// no more to do here
		return;

	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);

	// Get the window metrics
	float x, y, w, h;
	GetPosition(x,y);
	GetDimensions(w,h);
	// Adjust the current position to make it look better
	y += 5;

	GlobalCharacter *character = GlobalManager->GetCharacter(_char_id);

	//Draw character portrait
	VideoManager->Move(x + 12, y + 8);
	_portrait.Draw();

	// Write character name
	VideoManager->MoveRelative(150, -5);
	VideoManager->Text()->Draw(character->GetName(), TextStyle("title22"));

	// Level
	VideoManager->MoveRelative(0,20);
	VideoManager->Text()->Draw(UTranslate("Lv: ") + MakeUnicodeString(NumberToString(character->GetExperienceLevel())), TextStyle("text20"));

	// HP
	VideoManager->MoveRelative(0,20);
	VideoManager->Text()->Draw(UTranslate("HP: ") + MakeUnicodeString(NumberToString(character->GetHitPoints()) +
		" / " + NumberToString(character->GetMaxHitPoints())), TextStyle("text20"));

	// SP
	VideoManager->MoveRelative(0,20);
	VideoManager->Text()->Draw(UTranslate("SP: ") + MakeUnicodeString(NumberToString(character->GetSkillPoints()) +
		" / " + NumberToString(character->GetMaxSkillPoints())), TextStyle("text20"));

	// XP to level up
	VideoManager->MoveRelative(0, 20);
	VideoManager->Text()->Draw(UTranslate("XP to Next: ") +
		MakeUnicodeString(NumberToString(character->GetExperienceForNextLevel() - character->GetExperiencePoints())), TextStyle("text20"));

	return;
}


////////////////////////////////////////////////////////////////////////////////
// InventoryWindow Class
////////////////////////////////////////////////////////////////////////////////


InventoryWindow::InventoryWindow() : _active_box(ITEM_ACTIVE_NONE) {
	_InitCategory();
	_InitInventoryItems();
	_InitCharSelect();

	//Initializes the description textbox for the bottom window
	_description.SetOwner(this);
	_description.SetPosition(30.0f, 525.0f);
	_description.SetDimensions(800.0f, 80.0f);
	_description.SetDisplaySpeed(30);
	_description.SetTextStyle(TextStyle("text20"));
	_description.SetDisplayMode(VIDEO_TEXT_INSTANT);
	_description.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);

} // void InventoryWindow::InventoryWindow

InventoryWindow::~InventoryWindow()
{
}

//Initializes the list of items
void InventoryWindow::_InitInventoryItems() {
	// Set up the inventory option box
	_inventory_items.SetPosition(500.0f, 170.0f);
	_inventory_items.SetDimensions(400.0f, 360.0f, 1, 255, 1, 6);
	_inventory_items.SetTextStyle(TextStyle("text20"));
	_inventory_items.SetCursorOffset(-52.0f, -20.0f);
	_inventory_items.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_inventory_items.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_inventory_items.Scissoring( true, false );

	// Update the item text
	_UpdateItemText();
	if (_inventory_items.GetNumberOptions() > 0) {
		_inventory_items.SetSelection(0);
	}
	VideoManager->MoveRelative(-65, 20);
	// Initially hide the cursor
	_inventory_items.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

//Initalizes character select
void InventoryWindow::_InitCharSelect() {
	//character selection set up
	vector<ustring> options;
	uint32 size = GlobalManager->GetActiveParty()->GetPartySize();

	_char_select.SetPosition(72.0f, 109.0f);
	_char_select.SetDimensions(360.0f, 432.0f, 1, 4, 1, 4);
	_char_select.SetCursorOffset(-50.0f, -6.0f);
	_char_select.SetTextStyle(TextStyle("text20"));
	_char_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
	_char_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_char_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

	//Use a blank string so the cursor has somewhere to point
	//String is overdrawn by char portraits, so no matter
	for (uint32 i = 0; i < size; i++) {
		options.push_back(MakeUnicodeString(" "));
	}

	_char_select.SetOptions(options);
	_char_select.SetSelection(0);
	_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

//Initalizes the available item categories
void InventoryWindow::_InitCategory() {
	_item_categories.SetPosition(458.0f, 120.0f);
	_item_categories.SetDimensions(448.0f, 30.0f, ITEM_CATEGORY_SIZE, 1, ITEM_CATEGORY_SIZE, 1);
	_item_categories.SetTextStyle(TextStyle("text20"));

	_item_categories.SetCursorOffset(-52.0f, -20.0f);
	_item_categories.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
	_item_categories.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_item_categories.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);

	vector<ustring> options;
	options.push_back(UTranslate("All"));
	options.push_back(UTranslate("Itm"));
	options.push_back(UTranslate("Wpn"));
	options.push_back(UTranslate("Hlm"));
	options.push_back(UTranslate("Tor"));
	options.push_back(UTranslate("Arm"));
	options.push_back(UTranslate("Leg"));
	options.push_back(UTranslate("Key"));

	_item_categories.SetOptions(options);
	_item_categories.SetSelection(ITEM_ALL);
	_item_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

// Activates/deactivates inventory window
void InventoryWindow::Activate(bool new_status) {
	// Set new status
	if (new_status) {
		_active_box = ITEM_ACTIVE_CATEGORY;
		// Update cursor state
		_item_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	}
	else {
		//FIX ME: Play N/A noise
		_active_box = ITEM_ACTIVE_NONE;
		_item_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
		_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	}
}

// Updates the window
void InventoryWindow::Update() {

	//bool cancel = false;
	if ( GlobalManager->GetInventory()->size() == 0 )
	{
		// no more items in inventory, exit inventory window
		Activate(false);
		return;
	}

	// Points to the active option box
	OptionBox *active_option = NULL;

	_inventory_items.Update( SystemManager->GetUpdateTime() ); //For scrolling

	switch (_active_box) {
		case ITEM_ACTIVE_CATEGORY:
			active_option = &_item_categories;
			break;
		case ITEM_ACTIVE_CHAR:
			active_option = &_char_select;
			break;
		case ITEM_ACTIVE_LIST:
			active_option = &_inventory_items;
			break;
	}

	// Handle the appropriate input events
	if (InputManager->ConfirmPress())
	{
		active_option->InputConfirm();
	}
	else if (InputManager->CancelPress())
	{
		active_option->InputCancel();
	}
	else if (InputManager->LeftPress())
	{
		active_option->InputLeft();
	}
	else if (InputManager->RightPress())
	{
		active_option->InputRight();
	}
	else if (InputManager->UpPress())
	{
		active_option->InputUp();
	}
	else if (InputManager->DownPress())
	{
		active_option->InputDown();
	}

	uint32 event = active_option->GetEvent();
	active_option->Update();
	// Handle confirm/cancel presses differently for each window
	switch (_active_box) {
		case ITEM_ACTIVE_NONE:
			break;

		case ITEM_ACTIVE_CATEGORY:
		{
			// Activate the item list for this category
			if (event == VIDEO_OPTION_CONFIRM) {
				if (_inventory_items.GetNumberOptions() > 0) {
					_inventory_items.SetSelection(0);
					_item_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
					_inventory_items.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
					_description.SetDisplayText(_item_objects[ 0 ]->GetDescription());
					_active_box = ITEM_ACTIVE_LIST;
					MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
				} // if _inventory_items.GetNumberOptions() > 0
			} // if VIDEO_OPTION_CONFIRM
			// Deactivate inventory
			else if (event == VIDEO_OPTION_CANCEL) {
				MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
				_item_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
				Activate(false);
			} // if VIDEO_OPTION_CANCEL
			break;
		} // case ITEM_ACTIVE_CATEGORY

		case ITEM_ACTIVE_LIST:
		{
			// Activate the character select for application
			if (event == VIDEO_OPTION_CONFIRM) {
				_active_box = ITEM_ACTIVE_CHAR;
				_inventory_items.SetCursorState(VIDEO_CURSOR_STATE_BLINKING);
				_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
				MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
			} // if VIDEO_OPTION_CONFIRM
			// Return to category selection
			else if (event == VIDEO_OPTION_CANCEL) {
				_active_box = ITEM_ACTIVE_CATEGORY;
				_inventory_items.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
				_item_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
				MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
			} // else if VIDEO_OPTION_CANCEL
			else if ( event == VIDEO_OPTION_BOUNDS_UP || VIDEO_OPTION_BOUNDS_DOWN ) {
				_description.SetDisplayText(_item_objects[ _inventory_items.GetSelection() ]->GetDescription());
			} // else if VIDEO_OPTION_BOUNDS_UP
			break;
		} // case ITEM_ACTIVE_LIST

		case ITEM_ACTIVE_CHAR:
		{
			// Use the item on the chosen character
			if (event == VIDEO_OPTION_CONFIRM) {
				GlobalObject* obj = _item_objects[ _inventory_items.GetSelection() ];
				if (obj->GetObjectType() == GLOBAL_OBJECT_ITEM) {
					GlobalItem *item = (GlobalItem*)GlobalManager->RetrieveFromInventory(obj->GetID());
					const ScriptObject* script_function = item->GetFieldUseFunction();
					if (script_function == NULL) {
						IF_PRINT_WARNING(MENU_DEBUG) << "item did not have a menu use function" << endl;
					}
					else {
						if (IsTargetParty(item->GetTargetType()) == true) {
							GlobalParty *ch_party = GlobalManager->GetActiveParty();
							ScriptCallFunction<void>(*script_function, ch_party);
							item->DecrementCount();
						} // if GLOBAL_TARGET_PARTY
						else { // Use on a single character only
							GlobalCharacter *ch = dynamic_cast<GlobalCharacter*>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));
							ScriptCallFunction<void>(*script_function, ch);
							item->DecrementCount();
						}
					}
				} // if GLOBAL_OBJECT_ITEM
			} // if VIDEO_OPTION_CONFIRM
			// Return to item selection
			else if (event == VIDEO_OPTION_CANCEL) {
				_active_box = ITEM_ACTIVE_LIST;
				_inventory_items.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
				_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
				MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
			} // if VIDEO_OPTION_CANCEL
			break;
		} // case ITEM_ACTIVE_CHAR
	} // switch (_active_box)

	// Update the item list
	_UpdateItemText();
} // void InventoryWindow::Update()

// Updates the item list
void InventoryWindow::_UpdateItemText() {
	_item_objects.clear();
	_inventory_items.ClearOptions();

	switch (_item_categories.GetSelection()) {
		case ITEM_ALL:
		{
			std::map<uint32, GlobalObject*>* inv = GlobalManager->GetInventory();
			for (std::map<uint32, GlobalObject*>::iterator i = inv->begin(); i != inv->end(); i++) {
				_item_objects.push_back( i->second );
			}
		}
			break;

		case ITEM_ITEM:
			_item_objects = _GetItemVector(GlobalManager->GetInventoryItems());
			break;

		case ITEM_WEAPONS:
			_item_objects = _GetItemVector(GlobalManager->GetInventoryWeapons());
			break;

		case ITEM_HEAD_ARMOR:
			_item_objects = _GetItemVector(GlobalManager->GetInventoryHeadArmor());
			break;

		case ITEM_TORSO_ARMOR:
			_item_objects = _GetItemVector(GlobalManager->GetInventoryTorsoArmor());
			break;

		case ITEM_ARM_ARMOR:
			_item_objects = _GetItemVector(GlobalManager->GetInventoryArmArmor());
			break;

		case ITEM_LEG_ARMOR:
			_item_objects = _GetItemVector(GlobalManager->GetInventoryLegArmor());
			break;

		case ITEM_KEY:
			_item_objects = _GetItemVector(GlobalManager->GetInventoryKeyItems());
			break;
	}

	ustring text;
	std::vector<ustring> inv_names;

	for (size_t ctr = 0; ctr < _item_objects.size(); ctr++) {
		text = MakeUnicodeString("<" + _item_objects[ctr]->GetIconImage().GetFilename() + "><32>     ") +
			_item_objects[ctr]->GetName() + MakeUnicodeString("<R><350>" + NumberToString(_item_objects[ctr]->GetCount()) + "   ");
		inv_names.push_back(text);
	}

	_inventory_items.SetOptions(inv_names);
} // void InventoryWindow::UpdateItemText()



void InventoryWindow::Draw() {
	MenuWindow::Draw();

	// Update the item text in case the number of items changed.
	_UpdateItemText();

	// Draw char select option box
	_char_select.Draw();

	// Draw item categories option box
	_item_categories.Draw();

	// Draw item list
	_inventory_items.Draw();
} // bool InventoryWindow::Draw()


////////////////////////////////////////////////////////////////////////////////
// StatusWindow Class
////////////////////////////////////////////////////////////////////////////////

StatusWindow::StatusWindow() :
	_char_select_active(false)
{
	// Get party size for iteration
	uint32 partysize = GlobalManager->GetActiveParty()->GetPartySize();
	StillImage portrait;
	GlobalCharacter* ch;

	// Set up the full body portrait
	for (uint32 i = 0; i < partysize; i++) {
		ch = dynamic_cast<GlobalCharacter*>(GlobalManager->GetActiveParty()->GetActorAtIndex(i));
		portrait.SetStatic(true);
		portrait.Load("img/portraits/menu/" + ch->GetFilename() + "_large.png");
		_full_portraits.push_back(portrait);
	}

	// Init char select option box
	_InitCharSelect();
} // StatusWindow::StatusWindow()



StatusWindow::~StatusWindow() {

}

// Activate/deactivate window
void StatusWindow::Activate(bool new_value) {
	_char_select_active = new_value;

	if (_char_select_active)
		_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	else
		_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

void StatusWindow::_InitCharSelect() {
	//character selection set up
	vector<ustring> options;
	uint32 size = GlobalManager->GetActiveParty()->GetPartySize();

	_char_select.SetPosition(72.0f, 109.0f);
	_char_select.SetDimensions(360.0f, 432.0f, 1, 4, 1, 4);
	_char_select.SetCursorOffset(-50.0f, -6.0f);
	_char_select.SetTextStyle(TextStyle("text20"));
	_char_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
	_char_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_char_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

	// Use blank string so cursor can point somewhere
	for (uint32 i = 0; i < size; i++) {
		options.push_back(MakeUnicodeString(" "));
	}

	_char_select.SetOptions(options);
	_char_select.SetSelection(0);
	_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

// Updates the status window
void StatusWindow::Update() {
	// check input values
	if (InputManager->UpPress())
	{
		_char_select.InputUp();
	}
	else if (InputManager->DownPress())
	{
		_char_select.InputDown();
	}
	else if (InputManager->CancelPress())
	{
		_char_select.InputCancel();
	}

	if (_char_select.GetEvent() == VIDEO_OPTION_CANCEL) {
		Activate(false);
		MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
	}
	_char_select.Update();
} // void StatusWindow::Update()


// Draws the status window
void StatusWindow::Draw() {
	MenuWindow::Draw();

	GlobalCharacter* ch =  dynamic_cast<GlobalCharacter*>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));

	// Set drawing system
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);

	// window top corner is 432, 99
	VideoManager->Move(565, 130);

	//Draw character name and level
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, 0);
	VideoManager->Text()->Draw(ch->GetName());

	VideoManager->MoveRelative(0, 25);
	VideoManager->Text()->Draw(UTranslate("Experience Level: ") + MakeUnicodeString(NumberToString(ch->GetExperienceLevel())));

	VideoManager->SetDrawFlags(VIDEO_X_LEFT, 0);

	//Draw all character stats
	VideoManager->MoveRelative(-55, 60);
	VideoManager->Text()->Draw(UTranslate("HP: ") + MakeUnicodeString(NumberToString(ch->GetHitPoints()) +
		" (" + NumberToString(ch->GetMaxHitPoints()) + ")"));

	VideoManager->MoveRelative(0, 25);
	VideoManager->Text()->Draw(UTranslate("SP: ") + MakeUnicodeString(NumberToString(ch->GetSkillPoints()) +
		" (" + NumberToString(ch->GetMaxSkillPoints()) + ")"));

	VideoManager->MoveRelative(0, 25);
	VideoManager->Text()->Draw(UTranslate("XP to Next: ") + MakeUnicodeString(NumberToString(ch->GetExperienceForNextLevel() - ch->GetExperiencePoints())));

	VideoManager->MoveRelative(0, 25);
	VideoManager->Text()->Draw(UTranslate("Strength: ") + MakeUnicodeString(NumberToString(ch->GetStrength())));

	VideoManager->MoveRelative(0, 25);
	VideoManager->Text()->Draw(UTranslate("Vigor: ") + MakeUnicodeString(NumberToString(ch->GetVigor())));

	VideoManager->MoveRelative(0, 25);
	VideoManager->Text()->Draw(UTranslate("Fortitude: ") + MakeUnicodeString(NumberToString(ch->GetFortitude())));

	VideoManager->MoveRelative(0, 25);
	VideoManager->Text()->Draw(UTranslate("Protection: ") + MakeUnicodeString(NumberToString(ch->GetProtection())));

	VideoManager->MoveRelative(0, 25);
	VideoManager->Text()->Draw(UTranslate("Agility: ") + MakeUnicodeString(NumberToString(ch->GetAgility())));

	VideoManager->MoveRelative(0, 25);
	VideoManager->Text()->Draw(UTranslate("Evade: ") + MakeUnicodeString(NumberToString(ch->GetEvade()) + "%"));

	//Draw character full body portrait
	VideoManager->Move(855, 145);
	VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_TOP, 0);

	_full_portraits[_char_select.GetSelection()].Draw();

	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);

	_char_select.Draw();
} // void StatusWindow::Draw()

////////////////////////////////////////////////////////////////////////////////
// SkillsWindow Class
////////////////////////////////////////////////////////////////////////////////

SkillsWindow::SkillsWindow() :
	_active_box(SKILL_ACTIVE_NONE),
	_char_skillset(0)
{
	// Init option boxes
	_InitCharSelect();
	_InitSkillsList();
	_InitSkillsCategories();

	_description.SetOwner(this);
	_description.SetPosition(30.0f, 525.0f);
	_description.SetDimensions(800.0f, 80.0f);
	_description.SetDisplaySpeed(30);
	_description.SetDisplayMode(VIDEO_TEXT_INSTANT);
	_description.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
	_description.SetTextStyle(TextStyle("text20"));

} // SkillsWindow::SkillsWindow()



void SkillsWindow::Activate(bool new_status) {
	// Activate window and first option box...or deactivate both
	if (new_status) {
		_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
		_active_box = SKILL_ACTIVE_CHAR;
	}
	else {
		_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
		_active_box = SKILL_ACTIVE_NONE;
	}
}



void SkillsWindow::_InitSkillsList() {
	// Set up the inventory option box
	_skills_list.SetPosition(500.0f, 170.0f);
	_skills_list.SetDimensions(180.0f, 360.0f, 1, 255, 1, 4);
	_skills_list.SetTextStyle(TextStyle("text20"));
	_skills_list.SetCursorOffset(-52.0f, -20.0f);
	_skills_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_skills_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_skills_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

	_UpdateSkillList();
	if (_skills_list.GetNumberOptions() > 0)
		_skills_list.SetSelection(0);
	_skills_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

	// setup the cost option box
	_skill_cost_list.SetPosition(700.0f, 170.0f);
	_skill_cost_list.SetDimensions(180.0f, 360.0f, 1, 255, 1, 4);
	_skill_cost_list.SetTextStyle(TextStyle("text20"));
	_skill_cost_list.SetCursorOffset(-52.0f, -20.0f);
	_skill_cost_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_skill_cost_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_skill_cost_list.SetOptionAlignment(VIDEO_X_RIGHT, VIDEO_Y_CENTER);
	_skill_cost_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}



void SkillsWindow::_InitCharSelect() {
	//character selection set up
	vector<ustring> options;
	uint32 size = GlobalManager->GetActiveParty()->GetPartySize();

	_char_select.SetPosition(72.0f, 109.0f);
	_char_select.SetDimensions(360.0f, 432.0f, 1, 4, 1, 4);
	_char_select.SetCursorOffset(-50.0f, -6.0f);
	_char_select.SetTextStyle(TextStyle("text20"));
	_char_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
	_char_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_char_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);


	//Use blank strings....won't be seen anyway
	for (uint32 i = 0; i < size; i++) {
		options.push_back(MakeUnicodeString(" "));
	}

	//Set options, selection and cursor state
	_char_select.SetOptions(options);
	_char_select.SetSelection(0);
	_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}



void SkillsWindow::_InitSkillsCategories() {
	_skills_categories.SetPosition(458.0f, 120.0f);
	_skills_categories.SetDimensions(448.0f, 30.0f, SKILL_CATEGORY_SIZE, 1, SKILL_CATEGORY_SIZE, 1);
	_skills_categories.SetTextStyle(TextStyle("text20"));
	_skills_categories.SetCursorOffset(-52.0f, -20.0f);
	_skills_categories.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
	_skills_categories.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_skills_categories.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);

	// Create options
	vector<ustring> options;
	options.push_back(UTranslate("All"));
	options.push_back(UTranslate("Field"));
	options.push_back(UTranslate("Battle"));

	// Set options and default selection
	_skills_categories.SetOptions(options);
	_skills_categories.SetSelection(SKILL_ALL);
	_skills_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
} // void SkillsWindow::InitSkillsCategories()



void SkillsWindow::Update() {
	OptionBox *active_option = NULL;

	//choose correct menu
	switch (_active_box) {
		case SKILL_ACTIVE_CATEGORY:
			active_option = &_skills_categories;
			break;
		case SKILL_ACTIVE_CHAR_APPLY:
		case SKILL_ACTIVE_CHAR:
			active_option = &_char_select;
			break;
		case SKILL_ACTIVE_LIST:
			active_option = &_skills_list;
			break;
	}

	// Handle the appropriate input events
	if (InputManager->ConfirmPress())
	{
		active_option->InputConfirm();
	}
	else if (InputManager->CancelPress())
	{
		active_option->InputCancel();
	}
	else if (InputManager->LeftPress())
	{
		active_option->InputLeft();
	}
	else if (InputManager->RightPress())
	{
		active_option->InputRight();
	}
	else if (InputManager->UpPress())
	{
		active_option->InputUp();
	}
	else if (InputManager->DownPress())
	{
		active_option->InputDown();
	}

	uint32 event = active_option->GetEvent();
	active_option->Update();
	switch (_active_box) {
		case SKILL_ACTIVE_CHAR_APPLY:
			// Handle skill application
			if (event == VIDEO_OPTION_CONFIRM)
			{
				GlobalSkill *skill = _GetCurrentSkill();
				GlobalCharacter* target = dynamic_cast<GlobalCharacter*>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));
				GlobalCharacter* instigator = dynamic_cast<GlobalCharacter*>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_skillset));

				const ScriptObject* script_function = skill->GetFieldExecuteFunction();

				if (script_function == NULL) {
					IF_PRINT_WARNING(MENU_DEBUG) << "selected skill may not be executed in menus" << endl;
					break;
				}
				if (skill->GetSPRequired() > instigator->GetSkillPoints()) {
					IF_PRINT_WARNING(MENU_DEBUG) << "did not have enough skill points to execute skill " << endl;
					break;
				}
				ScriptCallFunction<void>(*script_function, target, instigator);
				instigator->SubtractSkillPoints(skill->GetSPRequired());
				MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
			}
			else if (event == VIDEO_OPTION_CANCEL) {
				_active_box = SKILL_ACTIVE_LIST;
				_skills_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
				_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
				_char_select.SetSelection(_char_skillset);
				MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
			}
			break;

		case SKILL_ACTIVE_CHAR:
			// Choose character for skillset
			if (event == VIDEO_OPTION_CONFIRM) {
				_active_box = SKILL_ACTIVE_CATEGORY;
				_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
				_skills_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
				_char_skillset = _char_select.GetSelection();
				MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
			}
			else if (event == VIDEO_OPTION_CANCEL) {
				Activate(false);
				_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
				MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
			}
			break;

		case SKILL_ACTIVE_LIST:
			// Choose skill
			if (event == VIDEO_OPTION_CONFIRM) {
				GlobalSkill *skill = _GetCurrentSkill();
				if (skill->IsExecutableInField())
				{
					_active_box = SKILL_ACTIVE_CHAR_APPLY;
					_skills_list.SetCursorState(VIDEO_CURSOR_STATE_BLINKING);
					_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
					MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
				}
				else
					MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
			}
			else if (event == VIDEO_OPTION_CANCEL) {
				_active_box = SKILL_ACTIVE_CATEGORY;
				MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
				_skills_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
				_skills_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
			}
			break;

		case SKILL_ACTIVE_CATEGORY:
			// Choose skill type
			if (event == VIDEO_OPTION_CONFIRM) {
				_skills_list.SetSelection(0);
				if (_skills_list.GetNumberOptions() > 0) {
					_active_box = SKILL_ACTIVE_LIST;
					_skills_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
					_skills_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
					MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
				}
				else {
					MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
				}
			}
			else if (event == VIDEO_OPTION_CANCEL) {
				_active_box = SKILL_ACTIVE_CHAR;
				MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
				_skills_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
				_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
				_char_select.SetSelection(_char_skillset);
			}
			break;
	}

	if (_active_box != SKILL_ACTIVE_CHAR_APPLY)
		_UpdateSkillList();

	if (_skills_list.GetNumberOptions() > 0 && _skills_list.GetSelection() >= 0 && static_cast<int32>(_skills_list.GetNumberOptions()) > _skills_list.GetSelection())
	{
		GlobalSkill *skill = _GetCurrentSkill();
		string desc = MakeStandardString(skill->GetName()) + "\n\n" + MakeStandardString(skill->GetDescription());
		_description.SetDisplayText(MakeUnicodeString(desc));
	}

} // void SkillsWindow::Update()

GlobalSkill *SkillsWindow::_GetCurrentSkill()
{
	GlobalCharacter* ch = dynamic_cast<GlobalCharacter*>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_skillset));

	vector<GlobalSkill *> *menu_skills = new vector<GlobalSkill *>();
	vector<GlobalSkill *> *battle_skills = new vector<GlobalSkill *>();
	vector<GlobalSkill *> *all_skills = new vector<GlobalSkill *>();

	_BuildMenuBattleSkillLists(ch->GetAttackSkills(), menu_skills, battle_skills, all_skills);
	_BuildMenuBattleSkillLists(ch->GetDefenseSkills(), menu_skills, battle_skills, all_skills);
	_BuildMenuBattleSkillLists(ch->GetSupportSkills(), menu_skills, battle_skills, all_skills);

	GlobalSkill *skill;
	switch (_skills_categories.GetSelection())
	{
	case SKILL_ALL:
		skill = all_skills->at(_skills_list.GetSelection());
		break;
	case SKILL_BATTLE:
		skill = battle_skills->at(_skills_list.GetSelection());
		break;
	case SKILL_FIELD:
		skill = menu_skills->at(_skills_list.GetSelection());
		break;
	default:
		skill = NULL;
		cerr << "MENU ERROR: Invalid skill type in SkillsWindow::_GetCurrentSkill()" << endl;
		break;
	}

	return skill;
}


void SkillsWindow::_UpdateSkillList() {
	GlobalCharacter* ch = dynamic_cast<GlobalCharacter*>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));
	assert(ch);
	vector<ustring> options;
	vector<ustring> cost_options;

	vector<GlobalSkill *> *menu_skills = new vector<GlobalSkill *>();
	vector<GlobalSkill *> *battle_skills = new vector<GlobalSkill *>();
	vector<GlobalSkill *> *all_skills = new vector<GlobalSkill *>();

	_BuildMenuBattleSkillLists(ch->GetAttackSkills(), menu_skills, battle_skills, all_skills);
	_BuildMenuBattleSkillLists(ch->GetDefenseSkills(), menu_skills, battle_skills, all_skills);
	_BuildMenuBattleSkillLists(ch->GetSupportSkills(), menu_skills, battle_skills, all_skills);

	vector<GlobalSkill *>::iterator i;

	switch (_skills_categories.GetSelection()) {
		case SKILL_ALL:
// 			_skills_list.SetSize(1, all_skills->size());
// 			_skill_cost_list.SetSize(1, all_skills->size());

			for (i = all_skills->begin(); i != all_skills->end(); ++i)
			{
				options.push_back((*i)->GetName());
				string cost = NumberToString((*i)->GetSPRequired()) + " SP";
				cost_options.push_back(MakeUnicodeString(cost));
			}
			break;
		case SKILL_BATTLE:
// 			_skills_list.SetSize(1,battle_skills->size());
// 			_skill_cost_list.SetSize(1, battle_skills->size());

			for (i = battle_skills->begin(); i != battle_skills->end(); ++i)
			{
				options.push_back((*i)->GetName());
				string cost = NumberToString((*i)->GetSPRequired()) + " SP";
				cost_options.push_back(MakeUnicodeString(cost));
			}
			break;
		case SKILL_FIELD:
// 			_skills_list.SetSize(1, menu_skills->size());
// 			_skill_cost_list.SetSize(1, menu_skills->size());

			for (i = menu_skills->begin(); i != menu_skills->end(); ++i)
			{
				options.push_back((*i)->GetName());
				string cost = NumberToString((*i)->GetSPRequired()) + " SP";
				cost_options.push_back(MakeUnicodeString(cost));
			}
			break;
		default:
// 			_skills_list.SetSize(1,0);
			break;
	}

	_skills_list.SetOptions(options);
	_skill_cost_list.SetOptions(cost_options);

	delete menu_skills;
	delete battle_skills;
	delete all_skills;
}

void SkillsWindow::_BuildMenuBattleSkillLists(vector<GlobalSkill *> *skill_list,
		vector<GlobalSkill *> *field, vector<GlobalSkill *> *battle, vector<GlobalSkill *> *all)
{
	vector<GlobalSkill *>::iterator i;
	for (i = skill_list->begin(); i != skill_list->end(); ++i)
	{
		if ((*i)->IsExecutableInBattle())
			battle->push_back(*i);
		if ((*i)->IsExecutableInField())
			field->push_back(*i);
		all->push_back(*i);
	}
}


void SkillsWindow::Draw() {
	MenuWindow::Draw();

	//Draw option boxes
	_char_select.Draw();
	_skills_categories.Draw();
	if (_active_box == SKILL_ACTIVE_NONE)
		_UpdateSkillList();
	_skills_list.Draw();
	_skill_cost_list.Draw();
}

////////////////////////////////////////////////////////////////////////////////
// EquipWindow Class
////////////////////////////////////////////////////////////////////////////////

EquipWindow::EquipWindow() :
	_active_box(EQUIP_ACTIVE_NONE)
{
	// Initialize option boxes
	_InitCharSelect();
	_InitEquipmentSelect();
	_InitEquipmentList();
}



EquipWindow::~EquipWindow() {
}



void EquipWindow::Activate(bool new_status) {

	//Activate window and first option box...or deactivate both
	if (new_status) {
		_active_box = EQUIP_ACTIVE_CHAR;
		_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	}
	else {
		_active_box = EQUIP_ACTIVE_NONE;
		_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	}
}



void EquipWindow::_InitEquipmentList() {
	// Set up the inventory option box
// 	_equip_list.SetCellSize(180.0f, 30.0f);

	_equip_list.SetPosition(500.0f, 170.0f);
	_equip_list.SetDimensions(400.0f, 360.0f, 1, 255, 1, 6);
	_equip_list.SetTextStyle(TextStyle("text20"));

	_equip_list.SetCursorOffset(-52.0f, -20.0f);
	_equip_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
	_equip_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_equip_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	// Update the equipment list
	_UpdateEquipList();
	if (_equip_list.GetNumberOptions() > 0) {
		_equip_list.SetSelection(0);
	}
	// Initially hide the cursor
	_equip_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}



void EquipWindow::_InitCharSelect() {
	//character selection set up
	vector<ustring> options;
	uint32 size = GlobalManager->GetActiveParty()->GetPartySize();

	_char_select.SetPosition(72.0f, 109.0f);
	_char_select.SetDimensions(360.0f, 432.0f, 1, 4, 1, 4);
	_char_select.SetCursorOffset(-50.0f, -6.0f);
	_char_select.SetTextStyle(TextStyle("text20"));
	_char_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
	_char_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_char_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

	//Use blank strings....won't be seen anyway
	for (uint32 i = 0; i < size; i++) {
		options.push_back(MakeUnicodeString(" "));
	}

	//Set options, selection and cursor state
	_char_select.SetOptions(options);
	_char_select.SetSelection(0);
	_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

} // void EquipWindow::InitCharSelect()



void EquipWindow::_InitEquipmentSelect() {
	//Set params
	_equip_select.SetPosition(680.0f, 145.0f);
	_equip_select.SetDimensions(105.0f, 350.0f, 1, EQUIP_CATEGORY_SIZE, 1, EQUIP_CATEGORY_SIZE);
	_equip_select.SetTextStyle(TextStyle("text20"));

	_equip_select.SetCursorOffset(-132.0f, -20.0f);
	_equip_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
	_equip_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_equip_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

	//Set options and default selection

	_equip_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	_UpdateEquipList();
	_equip_select.SetSelection(EQUIP_WEAPON);
} // void EquipWindow::_InitEquipmentSelect()



void EquipWindow::Update() {
	// Points to the active option box
	OptionBox *active_option = NULL;

	//choose correct menu
	switch (_active_box) {
		case EQUIP_ACTIVE_CHAR:
			active_option = &_char_select;
			break;
		case EQUIP_ACTIVE_SELECT:
			active_option = &_equip_select;
			break;
		case EQUIP_ACTIVE_LIST:
			active_option = &_equip_list;
			break;
	}

	// Handle the appropriate input events
	if (InputManager->ConfirmPress())
	{
		active_option->InputConfirm();
	}
	else if (InputManager->CancelPress())
	{
		active_option->InputCancel();
	}
	else if (InputManager->LeftPress())
	{
		active_option->InputLeft();
	}
	else if (InputManager->RightPress())
	{
		active_option->InputRight();
	}
	else if (InputManager->UpPress())
	{
		active_option->InputUp();
	}
	else if (InputManager->DownPress())
	{
		active_option->InputDown();
	}

	uint32 event = active_option->GetEvent();
	active_option->Update();
	switch (_active_box) {
	//Choose character
	case EQUIP_ACTIVE_CHAR:
		if (event == VIDEO_OPTION_CONFIRM) {
			_active_box = EQUIP_ACTIVE_SELECT;
			_char_select.SetCursorState(VIDEO_CURSOR_STATE_BLINKING);
			_equip_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
			MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
		}
		else if (event == VIDEO_OPTION_CANCEL) {
			Activate(false);
			MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
		}
		break;

	//Choose equipment to replace
	case EQUIP_ACTIVE_SELECT:
		if (event == VIDEO_OPTION_CONFIRM) {
			_active_box = EQUIP_ACTIVE_LIST;
			_UpdateEquipList();
			if (_equip_list.GetNumberOptions() > 0) {
				_equip_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
				_equip_list.SetSelection(0);
				_equip_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
				MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
			}
			else {
				_active_box = EQUIP_ACTIVE_SELECT;
				MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
			}
		}
		else if (event == VIDEO_OPTION_CANCEL) {
			_active_box = EQUIP_ACTIVE_CHAR;
			_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
			_equip_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
			MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
		}
		break;

	//Choose replacement
	case EQUIP_ACTIVE_LIST:
		if (event == VIDEO_OPTION_CONFIRM) {
			GlobalCharacter* ch = dynamic_cast<GlobalCharacter*>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));
			uint32 id_num;

			switch ( _equip_select.GetSelection() ) {
			case EQUIP_WEAPON:
			{	GlobalWeapon* wpn = GlobalManager->GetInventoryWeapons()->at(_equip_list.GetSelection());
				if (wpn->GetUsableBy() & ch->GetID()) {
					id_num = wpn->GetID();
					GlobalManager->AddToInventory(ch->EquipWeapon((GlobalWeapon*)GlobalManager->RetrieveFromInventory(id_num)));
				}
				else {
					MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
				}
				break;}

			case EQUIP_HEADGEAR:
			{	GlobalArmor* hlm = GlobalManager->GetInventoryHeadArmor()->at(_equip_list.GetSelection());
				if (hlm->GetUsableBy() & ch->GetID()) {
					id_num = hlm->GetID();
					GlobalManager->AddToInventory(ch->EquipHeadArmor((GlobalArmor*)GlobalManager->RetrieveFromInventory(id_num)));
				}
				else {
					MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
				}
				break;}

			case EQUIP_BODYARMOR:
			{	GlobalArmor* arm = GlobalManager->GetInventoryTorsoArmor()->at(_equip_list.GetSelection());
				if (arm->GetUsableBy() & ch->GetID()) {
					id_num = arm->GetID();
					GlobalManager->AddToInventory(ch->EquipTorsoArmor((GlobalArmor*)GlobalManager->RetrieveFromInventory(id_num)));
				}
				else {
					MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
				}
				break;}

			case EQUIP_OFFHAND:
			{	GlobalArmor* shld = GlobalManager->GetInventoryArmArmor()->at(_equip_list.GetSelection());
				if (shld->GetUsableBy() & ch->GetID()) {
					id_num = shld->GetID();
					GlobalManager->AddToInventory(ch->EquipArmArmor((GlobalArmor*)GlobalManager->RetrieveFromInventory(id_num)));
				}
				else {
					MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
				}
				break;}

			case EQUIP_LEGGINGS:
			{	GlobalArmor* lgs = GlobalManager->GetInventoryLegArmor()->at(_equip_list.GetSelection());
				if (lgs->GetUsableBy() & ch->GetID()) {
					id_num = lgs->GetID();
					GlobalManager->AddToInventory(ch->EquipLegArmor((GlobalArmor*)GlobalManager->RetrieveFromInventory(id_num)));
				}
				else {
					MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
				}
				break;}

			default:
				cout << "MENU ERROR: _equip_select.GetSelection value is invalid: " << _equip_select.GetSelection() << endl;
				break;
			} // switch _equip_select.GetSelection()

			_active_box = EQUIP_ACTIVE_SELECT;
			_equip_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
			_equip_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
			MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
		} // if VIDEO_OPTION_CONFIRM
		else if (event == VIDEO_OPTION_CANCEL) {
			_active_box = EQUIP_ACTIVE_SELECT;
			MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
			_equip_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
			_equip_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
		} // else if VIDEO_OPTION_CANCEL
		break;
	} // switch _active_box

	_UpdateEquipList();
} // void EquipWindow::Update()



void EquipWindow::_UpdateEquipList() {
	GlobalCharacter* ch = dynamic_cast<GlobalCharacter*>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));
	std::vector<ustring> options;

	if (_active_box == EQUIP_ACTIVE_LIST) {
		uint32 gearsize = 0;
//		vector<hoa_global::GlobalWeapon*> weapons;
//		vector<hoa_global::GlobalArmor*> armor;

		switch (_equip_select.GetSelection()) {
			case EQUIP_WEAPON:
				gearsize = GlobalManager->GetInventoryWeapons()->size();

				for (uint32 j = 0; j < gearsize; j++) {
					options.push_back(GlobalManager->GetInventoryWeapons()->at(j)->GetName());
				}

				break;

			case EQUIP_HEADGEAR:
				gearsize = GlobalManager->GetInventoryHeadArmor()->size();

				for (uint32 j = 0; j < gearsize; j++) {
					options.push_back(GlobalManager->GetInventoryHeadArmor()->at(j)->GetName());
				}

				break;

			case EQUIP_BODYARMOR:
				gearsize = GlobalManager->GetInventoryTorsoArmor()->size();

				for (uint32 j = 0; j < gearsize; j++) {
					options.push_back(GlobalManager->GetInventoryTorsoArmor()->at(j)->GetName());
				}

				break;

			case EQUIP_OFFHAND:
				gearsize = GlobalManager->GetInventoryArmArmor()->size();

				for (uint32 j = 0; j < gearsize; j++) {
					options.push_back(GlobalManager->GetInventoryArmArmor()->at(j)->GetName());
				}

				break;

			case EQUIP_LEGGINGS:
				gearsize = GlobalManager->GetInventoryLegArmor()->size();

				for (uint32 j = 0; j < gearsize; j++) {
					options.push_back(GlobalManager->GetInventoryLegArmor()->at(j)->GetName());
				}

				break;
		} // switch
// 		_equip_list.SetSize(1, gearsize);
		_equip_list.SetOptions(options);
	} // if EQUIP_ACTIVE_LIST

	else {
		// First, update the IMAGES of the equipped items
		_equip_images.clear();
		StillImage i;

		i.Load(ch->GetWeaponEquipped()->GetIconImage().GetFilename(), 60, 60);
		_equip_images.push_back(i);

		i.Load(ch->GetHeadArmorEquipped()->GetIconImage().GetFilename(), 60, 60);
		_equip_images.push_back(i);

		i.Load(ch->GetTorsoArmorEquipped()->GetIconImage().GetFilename(), 60, 60);
		_equip_images.push_back(i);

		i.Load(ch->GetArmArmorEquipped()->GetIconImage().GetFilename(), 60, 60);
		_equip_images.push_back(i);

		i.Load(ch->GetLegArmorEquipped()->GetIconImage().GetFilename(), 60, 60);
		_equip_images.push_back(i);

		// Now, update the NAMES of the equipped items

		options.push_back(ch->GetWeaponEquipped()->GetName());
		options.push_back(ch->GetHeadArmorEquipped()->GetName());
		options.push_back(ch->GetTorsoArmorEquipped()->GetName());
		options.push_back(ch->GetArmArmorEquipped()->GetName());
		options.push_back(ch->GetLegArmorEquipped()->GetName());

// 		_equip_select.SetSize(1, 5);
		_equip_select.SetOptions(options);
	}

} // void EquipWindow::UpdateEquipList()



void EquipWindow::Draw() {
	MenuWindow::Draw();
	_UpdateEquipList();

	//Draw option boxes
	_char_select.Draw();

	if (_active_box == EQUIP_ACTIVE_LIST) {
		_equip_list.Draw();
		VideoManager->Move(660.0f, 135.0f);
		VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
		switch (_equip_select.GetSelection()) {
			case EQUIP_WEAPON:
				VideoManager->Text()->Draw(UTranslate("Weapons"));
				break;
			case EQUIP_HEADGEAR:
				VideoManager->Text()->Draw(UTranslate("Headgear"));
				break;
			case EQUIP_BODYARMOR:
				VideoManager->Text()->Draw(UTranslate("Body Armor"));
				break;
			case EQUIP_OFFHAND:
				VideoManager->Text()->Draw(UTranslate("Offhand"));
				break;
			case EQUIP_LEGGINGS:
				VideoManager->Text()->Draw(UTranslate("Leggings"));
				break;
		}
	}
	else {
		_equip_select.Draw();

		//FIX ME: Use XML tags for formatting option boxes
		VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);
		VideoManager->Move(450.0f, 170.0f);
		VideoManager->Text()->Draw(UTranslate("Weapon"));
		VideoManager->MoveRelative(0.0f, 70.0f);
		VideoManager->Text()->Draw(UTranslate("Headgear"));
		VideoManager->MoveRelative(0.0f, 70.0f);
		VideoManager->Text()->Draw(UTranslate("Body Armor"));
		VideoManager->MoveRelative(0.0f, 70.0f);
		VideoManager->Text()->Draw(UTranslate("Offhand"));
		VideoManager->MoveRelative(0.0f, 70.0f);
		VideoManager->Text()->Draw(UTranslate("Leggings"));

		VideoManager->MoveRelative(150.0f, -370.0f);

		for (uint32 i = 0; i < _equip_images.size(); i++) {
			VideoManager->MoveRelative(0.0f, 70.0f);
			_equip_images[i].Draw();
		}
	}

} // void EquipWindow::Draw()


FormationWindow::FormationWindow() : _active_box(FORM_ACTIVE_NONE) {
	_InitCharSelect();
}


FormationWindow::~FormationWindow() {
}


void FormationWindow::_InitCharSelect() {
	//character selection set up
	std::vector<ustring> options;
	uint32 size = GlobalManager->GetActiveParty()->GetPartySize();

	_char_select.SetPosition(72.0f, 109.0f);
	_char_select.SetDimensions(360.0f, 432.0f, 1, 4, 1, 4);
	_char_select.SetCursorOffset(-50.0f, -6.0f);
	_char_select.SetTextStyle(TextStyle("text20"));
	_char_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
	_char_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_char_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

	_second_char_select.SetPosition(72.0f, 109.0f);
	_second_char_select.SetDimensions(360.0f, 432.0f, 1, 4, 1, 4);
	_second_char_select.SetCursorOffset(-50.0f, -6.0f);
	_second_char_select.SetTextStyle(TextStyle("text20"));
	_second_char_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
	_second_char_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
	_second_char_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);


	// Use blank string so cursor can point somewhere
	for (uint32 i = 0; i < size; i++) {
		options.push_back(MakeUnicodeString(" "));
	}

	_char_select.SetOptions(options);
	_char_select.SetSelection(0);
	_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

	_second_char_select.SetOptions(options);
	_second_char_select.SetSelection(0);
	_second_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

}


void FormationWindow::Update() {
	// Points to the active option box
	OptionBox *active_option = NULL;

	//choose correct menu
	switch (_active_box) {
		case FORM_ACTIVE_CHAR:
			active_option = &_char_select;
			break;
		case FORM_ACTIVE_SECOND:
			active_option = &_second_char_select;
			break;
	}

	// Handle the appropriate input events
	if (InputManager->ConfirmPress())
	{
		active_option->InputConfirm();
	}
	else if (InputManager->CancelPress())
	{
		active_option->InputCancel();
	}
	else if (InputManager->LeftPress())
	{
		active_option->InputLeft();
	}
	else if (InputManager->RightPress())
	{
		active_option->InputRight();
	}
	else if (InputManager->UpPress())
	{
		active_option->InputUp();
	}
	else if (InputManager->DownPress())
	{
		active_option->InputDown();
	}

	uint32 event = active_option->GetEvent();
	active_option->Update();

	switch (_active_box) {
		case FORM_ACTIVE_CHAR:
			if (event == VIDEO_OPTION_CONFIRM) {
				_active_box = FORM_ACTIVE_SECOND;
				_char_select.SetCursorState(VIDEO_CURSOR_STATE_BLINKING);
				_second_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
				MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
			}
			else if (event == VIDEO_OPTION_CANCEL) {
				Activate(false);
				MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
			}
			break;

		case FORM_ACTIVE_SECOND:
			if (event == VIDEO_OPTION_CONFIRM) {
				// TODO: Implement Character Switch
				_active_box = FORM_ACTIVE_CHAR;
				_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
				_second_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
			}
			else if (event == VIDEO_OPTION_CANCEL) {
				_active_box = FORM_ACTIVE_CHAR;
				_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
				_second_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
				MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
			}
			break;
	} // switch
	_char_select.Update();
}


void FormationWindow::Draw() {
	MenuWindow::Draw();
	_char_select.Draw();
	_second_char_select.Draw();
}


void FormationWindow::Activate(bool new_status) {
	if (new_status) {
		_active_box = FORM_ACTIVE_CHAR;
		_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
	}
	else {
		_active_box = FORM_ACTIVE_NONE;
		_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
		_second_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
	}
}


} // namespace private_menu


MessageWindow::MessageWindow(const ustring &message, float w, float h) :
	_message(message)
{
	float start_x = (1024 - w) / 2;
	float start_y = (768 - h) / 2;

	MenuWindow::Create(w, h);
	MenuWindow::SetPosition(start_x, start_y);
	MenuWindow::Show();

	_textbox.SetPosition(30, 5);
	_textbox.SetDimensions(w, h);
	_textbox.SetTextStyle(TextStyle("text22"));
	_textbox.SetDisplayMode(VIDEO_TEXT_INSTANT);
	_textbox.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_textbox.SetDisplayText(_message);
	_textbox.SetOwner(this);
}

MessageWindow::~MessageWindow()
{
	MenuWindow::Destroy();
}

void MessageWindow::Draw()
{
	MenuWindow::Draw();
	_textbox.Draw();
}

} // namespace hoa_menu
