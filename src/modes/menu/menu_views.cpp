///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
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
 * \author  Nik Nadig (IkarusDowned) nihonnik@gmail.com
 * \brief   Source file for various menu views.
 *****************************************************************************/

#include <iostream>
#include <sstream>
#include <algorithm>

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/system.h"

#include "modes/menu/menu.h"

using namespace hoa_menu::private_menu;
using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_global;
using namespace hoa_input;
using namespace hoa_system;

namespace hoa_menu
{

namespace private_menu
{


////////////////////////////////////////////////////////////////////////////////
// CharacterWindow Class
////////////////////////////////////////////////////////////////////////////////

CharacterWindow::CharacterWindow() : _char_id(GLOBAL_CHARACTER_INVALID)
{
}


void CharacterWindow::SetCharacter(GlobalCharacter *character)
{
    _char_id = character->GetID();

    if(character) {
        _portrait = character->GetPortrait();
        // Only size up valid portraits
        if(!_portrait.GetFilename().empty())
            _portrait.SetDimensions(100.0f, 100.0f);
    }
} // void CharacterWindow::SetCharacter(GlobalCharacter *character)



// Draw the window to the screen
void CharacterWindow::Draw()
{
    // Call parent Draw method, if failed pass on fail result
    MenuWindow::Draw();

    // check to see if this window is an actual character
    if(_char_id == hoa_global::GLOBAL_CHARACTER_INVALID)
        // no more to do here
        return;

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);

    // Get the window metrics
    float x, y, w, h;
    GetPosition(x, y);
    GetDimensions(w, h);
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
    VideoManager->MoveRelative(0, 19);
    VideoManager->Text()->Draw(UTranslate("Lv: ") + MakeUnicodeString(NumberToString(character->GetExperienceLevel())), TextStyle("text20"));

    // HP
    VideoManager->MoveRelative(0, 19);
    VideoManager->Text()->Draw(UTranslate("HP: ") + MakeUnicodeString(NumberToString(character->GetHitPoints()) +
                               " / " + NumberToString(character->GetMaxHitPoints())), TextStyle("text20"));

    // SP
    VideoManager->MoveRelative(0, 19);
    VideoManager->Text()->Draw(UTranslate("SP: ") + MakeUnicodeString(NumberToString(character->GetSkillPoints()) +
                               " / " + NumberToString(character->GetMaxSkillPoints())), TextStyle("text20"));

    // XP to level up
    VideoManager->MoveRelative(0, 19);
    VideoManager->Text()->Draw(UTranslate("XP to Next: ") +
                               MakeUnicodeString(NumberToString(character->GetExperienceForNextLevel())), TextStyle("text20"));

    return;
}


////////////////////////////////////////////////////////////////////////////////
// InventoryWindow Class
////////////////////////////////////////////////////////////////////////////////


InventoryWindow::InventoryWindow() :
    _active_box(ITEM_ACTIVE_NONE),
    _previous_category(ITEM_ALL)
{
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

//Initializes the list of items
void InventoryWindow::_InitInventoryItems()
{
    // Set up the inventory option box
    _inventory_items.SetPosition(500.0f, 170.0f);
    _inventory_items.SetDimensions(400.0f, 360.0f, 1, 255, 1, 6);
    _inventory_items.SetTextStyle(TextStyle("text20"));
    _inventory_items.SetCursorOffset(-52.0f, -20.0f);
    _inventory_items.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _inventory_items.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _inventory_items.Scissoring(true, false);

    // Update the item text
    _UpdateItemText();
    if(_inventory_items.GetNumberOptions() > 0) {
        _inventory_items.SetSelection(0);
    }
    VideoManager->MoveRelative(-65, 20);
    // Initially hide the cursor
    _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

//Initalizes character select
void InventoryWindow::_InitCharSelect()
{
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

    //Use a blank string so the cursor has somewhere to point
    //String is overdrawn by char portraits, so no matter
    for(uint32 i = 0; i < size; i++) {
        options.push_back(MakeUnicodeString(" "));
    }

    _char_select.SetOptions(options);
    _char_select.SetSelection(0);
    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

//Initalizes the available item categories
void InventoryWindow::_InitCategory()
{
    _item_categories.SetPosition(458.0f, 120.0f);
    _item_categories.SetDimensions(448.0f, 30.0f, ITEM_CATEGORY_SIZE, 1, ITEM_CATEGORY_SIZE, 1);
    _item_categories.SetTextStyle(TextStyle("text20"));

    _item_categories.SetCursorOffset(-52.0f, -20.0f);
    _item_categories.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
    _item_categories.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _item_categories.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);

    std::vector<ustring> options;
    options.push_back(UTranslate("All"));
    options.push_back(UTranslate("Itm"));
    options.push_back(UTranslate("Wpn"));
    options.push_back(UTranslate("Hlm"));
    options.push_back(UTranslate("Tor"));
    options.push_back(CUTranslate("Menu|Arm"));
    options.push_back(UTranslate("Leg"));
    options.push_back(UTranslate("Key"));

    _item_categories.SetOptions(options);
    _item_categories.SetSelection(ITEM_ALL);
    _item_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

// Activates/deactivates inventory window
void InventoryWindow::Activate(bool new_status)
{
    // Set new status
    if(new_status) {
        _active_box = ITEM_ACTIVE_CATEGORY;
        // Update cursor state
        _item_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    } else {
        //FIX ME: Play N/A noise
        _active_box = ITEM_ACTIVE_NONE;
        _item_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    }
}

// Updates the window
void InventoryWindow::Update()
{

    //bool cancel = false;
    if(GlobalManager->GetInventory()->size() == 0) {
        // no more items in inventory, exit inventory window
        Activate(false);
        return;
    }

    // Points to the active option box
    OptionBox *active_option = NULL;

    _inventory_items.Update(SystemManager->GetUpdateTime());   //For scrolling

    switch(_active_box) {
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
    if(InputManager->ConfirmPress()) {
        active_option->InputConfirm();
    } else if(InputManager->CancelPress()) {
        active_option->InputCancel();
    } else if(InputManager->LeftPress()) {
        active_option->InputLeft();
    } else if(InputManager->RightPress()) {
        active_option->InputRight();
    } else if(InputManager->UpPress()) {
        active_option->InputUp();
    } else if(InputManager->DownPress()) {
        active_option->InputDown();
    }

    uint32 event = active_option->GetEvent();
    active_option->Update();
    // Handle confirm/cancel presses differently for each window
    switch(_active_box) {
        case ITEM_ACTIVE_NONE:
            break;

        case ITEM_ACTIVE_CATEGORY: {
            // Activate the item list for this category
            if(event == VIDEO_OPTION_CONFIRM) {
                if(_inventory_items.GetNumberOptions() > 0) {
                    _inventory_items.SetSelection(0);
                    _item_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                    _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                    _description.SetDisplayText(_item_objects[ 0 ]->GetDescription());
                    _active_box = ITEM_ACTIVE_LIST;
                    MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
                } // if _inventory_items.GetNumberOptions() > 0
            } // if VIDEO_OPTION_CONFIRM
            // Deactivate inventory
            else if(event == VIDEO_OPTION_CANCEL) {
                MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
                _item_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                Activate(false);
            } // if VIDEO_OPTION_CANCEL
            break;
        } // case ITEM_ACTIVE_CATEGORY

        case ITEM_ACTIVE_LIST: {
            //check if the items list is empty. if so, we exit up to the categories level
            if( _inventory_items.GetNumberOptions() <= 0)
            {
                _active_box = ITEM_ACTIVE_CATEGORY;
                _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                _item_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                break;
            }
            // Test whether the inventory cursor is out of bound after using
            // the item. Can happen when using/equipping the last item of the list.
            else if(_inventory_items.GetSelection() >= (int32)_inventory_items.GetNumberOptions()) {
                _inventory_items.SetSelection((uint32)_inventory_items.GetNumberOptions() - 1);
            }

            GlobalObject *obj = _item_objects[ _inventory_items.GetSelection() ];

            // Activate the character select for application
            if(event == VIDEO_OPTION_CONFIRM) {
                // Don't accept selecting shard items for now
                if(obj->GetObjectType() == GLOBAL_OBJECT_SHARD) {
                    MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
                    break;
                }

                _active_box = ITEM_ACTIVE_CHAR;
                _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_DARKEN);
                _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
            } // if VIDEO_OPTION_CONFIRM
            // Return to category selection
            else if(event == VIDEO_OPTION_CANCEL) {
                _active_box = ITEM_ACTIVE_CATEGORY;
                _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                _item_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
            } // else if VIDEO_OPTION_CANCEL
            else if(event == VIDEO_OPTION_BOUNDS_UP || VIDEO_OPTION_BOUNDS_DOWN) {
                _description.SetDisplayText(obj->GetDescription());
            } // else if VIDEO_OPTION_BOUNDS_UP
            break;
        } // case ITEM_ACTIVE_LIST

        case ITEM_ACTIVE_CHAR: {
            // Use the item on the chosen character
            if(event == VIDEO_OPTION_CONFIRM) {
                GlobalObject *obj = _item_objects[ _inventory_items.GetSelection() ];

                //values used for equipment selection
                bool is_equipable = false;
                GlobalArmor *selected_armor = NULL;
                GlobalWeapon *selected_weapon = NULL;
                GLOBAL_OBJECT obj_type = obj->GetObjectType();
                GlobalCharacter *ch = dynamic_cast<GlobalCharacter *>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));
                switch(obj_type)
                {
                    case GLOBAL_OBJECT_ITEM:
                    {
                        // Returns an item object, already removed from inventory.
                        // Don't forget to readd the item if not used, or to delete the pointer.
                        GlobalItem *item = (GlobalItem *)GlobalManager->RetrieveFromInventory(obj->GetID());
                        const ScriptObject &script_function = item->GetFieldUseFunction();
                        if(!script_function.is_valid()) {
                            IF_PRINT_WARNING(MENU_DEBUG) << "item did not have a menu use function" << std::endl;
                        } else {
                            if(IsTargetParty(item->GetTargetType()) == true) {
                                GlobalParty *ch_party = GlobalManager->GetActiveParty();

                                // If the item use failed, we readd it to inventory.
                                if(!ScriptCallFunction<bool>(script_function, ch_party))
                                    GlobalManager->AddToInventory(item);
                                else // delete the item instance when succeeded. Also, return back a level to the item selection list
                                {
                                    _active_box = ITEM_ACTIVE_LIST;
                                    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                                    _char_select.ResetViewableOption();
                                    //set the item select to by lightened
                                    _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                                    delete item;
                                }
                            } // if GLOBAL_TARGET_PARTY
                            else { // Use on a single character only


                                // If the item use failed, we readd it to inventory.
                                if(!ScriptCallFunction<bool>(script_function, ch))
                                    GlobalManager->AddToInventory(item);
                                else // delete the item instance when succeeded. Also, return back a level to the item selection list
                                {
                                    _active_box = ITEM_ACTIVE_LIST;
                                    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                                    _char_select.ResetViewableOption();
                                    //set the item select to by lightened
                                    _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                                    delete item;
                                }

                            }
                        }
                        break;
                    } // if GLOBAL_OBJECT_ITEM
                    case GLOBAL_OBJECT_WEAPON:
                    {
                        //get the item from the inventory list. this also removes the item from the list
                        selected_weapon = dynamic_cast<GlobalWeapon *>(GlobalManager->RetrieveFromInventory(obj->GetID()));
                        uint32 usability_bitmask = selected_weapon->GetUsableBy();
                        is_equipable = usability_bitmask & ch->GetID();
                        break;
                    }
                    case GLOBAL_OBJECT_HEAD_ARMOR:
                    case GLOBAL_OBJECT_TORSO_ARMOR:
                    case GLOBAL_OBJECT_ARM_ARMOR:
                    case GLOBAL_OBJECT_LEG_ARMOR:
                    {
                        //get the item from the inventory list. this also removes the item from the list
                        selected_armor = dynamic_cast<GlobalArmor *>(GlobalManager->RetrieveFromInventory(obj->GetID()));
                        uint32 usability_bitmask = selected_armor->GetUsableBy();
                        is_equipable = usability_bitmask & ch->GetID();
                        break;
                    }

                    default:
                        break;
                }
                //if we can equip this and it is armor
                if(is_equipable && selected_armor)
                {
                    //do swap of armor based on object type (aka armor type)
                    switch(obj_type)
                    {
                        case GLOBAL_OBJECT_HEAD_ARMOR:
                            selected_armor = ch->EquipHeadArmor(selected_armor);
                            break;
                        case GLOBAL_OBJECT_TORSO_ARMOR:
                            selected_armor = ch->EquipTorsoArmor(selected_armor);
                            break;
                        case GLOBAL_OBJECT_ARM_ARMOR:
                            selected_armor = ch->EquipArmArmor(selected_armor);
                            break;
                        case GLOBAL_OBJECT_LEG_ARMOR:
                            selected_armor = ch->EquipLegArmor(selected_armor);
                        default:
                            break;
                    }
                    //add the old armor back to the inventory
                    GlobalManager->AddToInventory(selected_armor);
                    //return back a level
                    _active_box = ITEM_ACTIVE_LIST;
                    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                    _char_select.ResetViewableOption();
                    //set the item select to by lightened
                    _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);

                }
                //if we can equuip and it is a weapon
                else if(is_equipable && selected_weapon)
                {
                    //get the old weapon by swapping the selected_weapon for the current one
                    selected_weapon = ch->EquipWeapon(selected_weapon);
                    //add the old weapon back into the inventory
                    GlobalManager->AddToInventory(selected_weapon);
                    //return back a level
                    _active_box = ITEM_ACTIVE_LIST;
                    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                    _char_select.ResetViewableOption();
                    //set the item select to by lightened
                    _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);

                }
                //if we cannot equip
                else
                {
                    //return the weapon to inventory
                    if(selected_weapon)
                        GlobalManager->AddToInventory(selected_weapon);
                    if(selected_armor)
                        GlobalManager->AddToInventory(selected_armor);
                    MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
                }

            } // if VIDEO_OPTION_CONFIRM
            // Return to item selection
            else if(event == VIDEO_OPTION_CANCEL) {
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
void InventoryWindow::_UpdateItemText()
{
    // This is a case only for equipment.
    // before we update the current inventory_items option box
    // if the actual available items WAS zero on the last frame, then we make sure
    // that the cursor is reset to hidden.
    // if you don't do this, then the previous ITEM_CATEGORY cursor state remains, and will
    // cause the darkened pointer to show
    if(_item_objects.size() == 0)
        _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _item_objects.clear();
    _inventory_items.ClearOptions();

    ITEM_CATEGORY current_selected_category = static_cast<ITEM_CATEGORY>(_item_categories.GetSelection());
    switch(current_selected_category) {
        case ITEM_ALL: {
            std::map<uint32, GlobalObject *>* inv = GlobalManager->GetInventory();
            for(std::map<uint32, GlobalObject *>::iterator it = inv->begin(); it != inv->end(); ++it) {
                _item_objects.push_back(it->second);
            }
            break;
        }
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

        case ITEM_KEY: {
            std::map<uint32, GlobalObject *>* inv = GlobalManager->GetInventory();
            for(std::map<uint32, GlobalObject *>::iterator it = inv->begin(); it != inv->end(); ++it) {
                if (it->second->IsKeyItem())
                    _item_objects.push_back(it->second);
            }
            break;
        }
        default:
            break;
        }

    ustring text;
    std::vector<ustring> inv_names;

    for(size_t ctr = 0; ctr < _item_objects.size(); ctr++) {
        text = MakeUnicodeString("<" + _item_objects[ctr]->GetIconImage().GetFilename() + "><32>     ") +
               _item_objects[ctr]->GetName() + MakeUnicodeString("<R><350>" + NumberToString(_item_objects[ctr]->GetCount()) + "   ");
        inv_names.push_back(text);
    }

    _inventory_items.SetOptions(inv_names);

    if(current_selected_category != _previous_category )
    {
        //swap to the new category
        _previous_category = current_selected_category ;
        //reset the top viewing inventory item
        _inventory_items.ResetViewableOption();
    }
} // void InventoryWindow::UpdateItemText()



void InventoryWindow::Draw()
{
    MenuWindow::Draw();

    // Update the item list
    _UpdateItemText();

    // Draw char select option box
    _char_select.Draw();

    // Draw item categories option box
    _item_categories.Draw();

    // Draw item list
    _inventory_items.Draw();
} // bool InventoryWindow::Draw()


////////////////////////////////////////////////////////////////////////////////
// PartyWindow Class
////////////////////////////////////////////////////////////////////////////////

PartyWindow::PartyWindow() :
    _char_select_active(FORM_ACTIVE_NONE)
{
    // Get party size for iteration
    uint32 partysize = GlobalManager->GetActiveParty()->GetPartySize();
    StillImage portrait;
    GlobalCharacter *ch;

    // Set up the full body portrait
    for(uint32 i = 0; i < partysize; i++) {
        ch = dynamic_cast<GlobalCharacter *>(GlobalManager->GetActiveParty()->GetActorAtIndex(i));
        _full_portraits.push_back(ch->GetFullPortrait());
    }

    // Init char select option box
    _InitCharSelect();
} // PartyWindow::PartyWindow()

// Activate/deactivate window
void PartyWindow::Activate(bool new_value)
{
    if(new_value) {
        _char_select_active = FORM_ACTIVE_CHAR;
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    } else {
        _char_select_active = FORM_ACTIVE_NONE;
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _second_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    }
}

void PartyWindow::_InitCharSelect()
{
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
    for(uint32 i = 0; i < size; i++) {
        options.push_back(MakeUnicodeString(" "));
    }

    _char_select.SetOptions(options);
    _char_select.SetSelection(0);
    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _second_char_select.SetOptions(options);
    _second_char_select.SetSelection(0);
    _second_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _help_text.SetStyle(TextStyle("text18"));

    _character_status_text.SetStyle(TextStyle("text20"));
    _character_status_numbers.SetStyle(TextStyle("text20"));
    _character_status_icons.Load("img/menus/status/menu_stats_icons.png");

    _average_text.SetStyle(TextStyle("text20"));
    _average_text.SetText(Translate("Attack / Defense:"));
    _focused_text.SetStyle(TextStyle("text20"));
    _focused_text.SetText(Translate("Detailed defense:"));

    _average_atk_def_text.SetStyle(TextStyle("text18"));
    hoa_utils::ustring text = UTranslate("Attack (ATK): ") + MakeUnicodeString("\n")
        + UTranslate("Magical Attack (M.ATK): ") + MakeUnicodeString("\n\n")
        + UTranslate("Defense (DEF): ") + MakeUnicodeString("\n")
        + UTranslate("Magical Defense (M.DEF): ");
    _average_atk_def_text.SetText(text);
    _average_atk_def_numbers.SetStyle(TextStyle("text18"));
    _average_atk_def_icons.Load("img/menus/status/menu_avg_atk_def.png");

    _focused_def_text.SetStyle(TextStyle("text18"));
    text = MakeUnicodeString("\n") // Skip title
        + UTranslate("Head: ") + MakeUnicodeString("\n")
        + UTranslate("Torso: ") + MakeUnicodeString("\n")
        + UTranslate("Arm: ") + MakeUnicodeString("\n")
        + UTranslate("Leg: ");
    _focused_def_text.SetText(text);
    _focused_def_numbers.SetStyle(TextStyle("text18"));
    _focused_mdef_numbers.SetStyle(TextStyle("text18"));

    _focused_def_category_icons.Load("img/menus/status/menu_point_atk_def.png");
    _focused_def_icon.Load("img/menus/status/menu_def_icon.png");
    _focused_mdef_icon.Load("img/menus/status/menu_mdef_icon.png");

    _UpdateStatus();
}

// Updates the status window
void PartyWindow::Update()
{
    static const ustring change_position_message = UTranslate("Select a character to change position with.");
    static const ustring change_formation_message = UTranslate("Select a character to change formation.");

    // Points to the active option box
    OptionBox *active_option = NULL;
    //choose correct menu
    switch(_char_select_active) {
    case FORM_ACTIVE_CHAR:
        active_option = &_char_select;
        _help_text.SetText(change_formation_message);
        break;
    case FORM_ACTIVE_SECOND:
        active_option = &_second_char_select;
        _help_text.SetText(change_position_message);
        break;
    }

    // Handle the appropriate input events
    if(InputManager->ConfirmPress()) {
        active_option->InputConfirm();
    } else if(InputManager->CancelPress()) {
        active_option->InputCancel();
    } else if(InputManager->LeftPress()) {
        active_option->InputLeft();
    } else if(InputManager->RightPress()) {
        active_option->InputRight();
    } else if(InputManager->UpPress()) {
        active_option->InputUp();
    } else if(InputManager->DownPress()) {
        active_option->InputDown();
    }

    uint32 event = active_option->GetEvent();
    active_option->Update();

    switch(_char_select_active) {
    case FORM_ACTIVE_CHAR:
        if(event == VIDEO_OPTION_CONFIRM) {
            _char_select_active = FORM_ACTIVE_SECOND;
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_DARKEN);
            _second_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
        } else if(event == VIDEO_OPTION_CANCEL) {
            Activate(false);
            MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
        }
        break;

    case FORM_ACTIVE_SECOND:
        if(event == VIDEO_OPTION_CONFIRM) {
            // Switch Characters
            GlobalManager->SwapCharactersByIndex(_char_select.GetSelection(), _second_char_select.GetSelection());
            std::swap(_full_portraits[_char_select.GetSelection()],_full_portraits[_second_char_select.GetSelection()]);

            // Update the character's view
            MenuMode::CurrentInstance()->ReloadCharacterWindows();

            _char_select_active = FORM_ACTIVE_CHAR;
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _second_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        } else if(event == VIDEO_OPTION_CANCEL) {
            _char_select_active = FORM_ACTIVE_CHAR;
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _second_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
        }
        break;
    } // switch
    _char_select.Update();

    // update the status text
    if (InputManager->AnyKeyPress())
        _UpdateStatus();
} // void PartyWindow::Update()

void PartyWindow::_UpdateStatus()
{
    _character_status_numbers.Clear();
    _average_atk_def_numbers.Clear();
    _focused_def_numbers.Clear();
    _focused_mdef_numbers.Clear();

    GlobalCharacter *ch =  dynamic_cast<GlobalCharacter *>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));
    if (!ch)
        return;

    hoa_utils::ustring text;
    text = UTranslate("Experience Level: ") + MakeUnicodeString(NumberToString(ch->GetExperienceLevel()))
        + MakeUnicodeString("\n\n\n")
        + UTranslate("Strength (STR): ") + MakeUnicodeString("\n\n")
        + UTranslate("Vigor (VIG): ") + MakeUnicodeString("\n\n")
        + UTranslate("Fortitude (FRT): ") + MakeUnicodeString("\n\n")
        + UTranslate("Protection (PRO): ") + MakeUnicodeString("\n\n")
        + UTranslate("Agility (AGI): ") + MakeUnicodeString("\n\n")
        + UTranslate("Evade (EVD): ");

    _character_status_text.SetText(text);

    text = MakeUnicodeString("\n\n\n")
        + MakeUnicodeString(NumberToString(ch->GetStrength())) + MakeUnicodeString("\n\n")
        + MakeUnicodeString(NumberToString(ch->GetVigor())) + MakeUnicodeString("\n\n")
        + MakeUnicodeString(NumberToString(ch->GetFortitude())) + MakeUnicodeString("\n\n")
        + MakeUnicodeString(NumberToString(ch->GetProtection())) + MakeUnicodeString("\n\n")
        + MakeUnicodeString(NumberToString(ch->GetAgility())) + MakeUnicodeString("\n\n")
        + MakeUnicodeString(NumberToString(ch->GetEvade())) + MakeUnicodeString("%");

    _character_status_numbers.SetText(text);

    text = MakeUnicodeString(NumberToString(ch->GetTotalPhysicalAttack())) + MakeUnicodeString("\n")
        + MakeUnicodeString(NumberToString(ch->GetTotalMagicalAttack())) + MakeUnicodeString("\n\n")
        + MakeUnicodeString(NumberToString(ch->GetAverageDefense())) + MakeUnicodeString("\n")
        + MakeUnicodeString(NumberToString(ch->GetAverageMagicalDefense()));

    _average_atk_def_numbers.SetText(text);

    _weapon_icon.Clear();
    GlobalWeapon *weapon = ch->GetWeaponEquipped();
    if (weapon)
        _weapon_icon.Load(weapon->GetIconImage().GetFilename());
    else
        _weapon_icon.Load("img/icons/weapons/fist-human.png");
    _weapon_icon.SetHeightKeepRatio(40);

    GlobalArmor *head_armor = ch->GetHeadArmorEquipped();
    _focused_def_armor_icons[0].Clear();
    if (head_armor) {
        _focused_def_armor_icons[0].Load(head_armor->GetIconImage().GetFilename());
        _focused_def_armor_icons[0].SetHeightKeepRatio(20);
    }

    _focused_def_armor_icons[1].Clear();
    GlobalArmor *torso_armor = ch->GetTorsoArmorEquipped();
    if (torso_armor) {
        _focused_def_armor_icons[1].Load(torso_armor->GetIconImage().GetFilename());
        _focused_def_armor_icons[1].SetHeightKeepRatio(20);
    }

    _focused_def_armor_icons[2].Clear();
    GlobalArmor *arm_armor = ch->GetArmArmorEquipped();
    if (arm_armor) {
        _focused_def_armor_icons[2].Load(arm_armor->GetIconImage().GetFilename());
        _focused_def_armor_icons[2].SetHeightKeepRatio(20);
    }

    _focused_def_armor_icons[3].Clear();
    GlobalArmor *leg_armor = ch->GetLegArmorEquipped();
    if (leg_armor) {
        _focused_def_armor_icons[3].Load(leg_armor->GetIconImage().GetFilename());
        _focused_def_armor_icons[3].SetHeightKeepRatio(20);
    }

    text = MakeUnicodeString("\n") // Skip titles
        + MakeUnicodeString(NumberToString(ch->GetFortitude() + (head_armor ? head_armor->GetPhysicalDefense() : 0)) + "\n")
        + MakeUnicodeString(NumberToString(ch->GetFortitude() + (torso_armor ? torso_armor->GetPhysicalDefense() : 0)) + "\n")
        + MakeUnicodeString(NumberToString(ch->GetFortitude() + (arm_armor ? arm_armor->GetPhysicalDefense() : 0)) + "\n")
        + MakeUnicodeString(NumberToString(ch->GetFortitude() + (leg_armor ? leg_armor->GetPhysicalDefense() : 0)) + "\n");

    _focused_def_numbers.SetText(text);

    text = MakeUnicodeString("\n") // Skip titles
        + MakeUnicodeString(NumberToString(ch->GetProtection() + (head_armor ? head_armor->GetMagicalDefense() : 0)) + "\n")
        + MakeUnicodeString(NumberToString(ch->GetProtection() + (torso_armor ? torso_armor->GetMagicalDefense() : 0)) + "\n")
        + MakeUnicodeString(NumberToString(ch->GetProtection() + (arm_armor ? arm_armor->GetMagicalDefense() : 0)) + "\n")
        + MakeUnicodeString(NumberToString(ch->GetProtection() + (leg_armor ? leg_armor->GetMagicalDefense() : 0)) + "\n");

    _focused_mdef_numbers.SetText(text);
}

void PartyWindow::_DrawBottomEquipmentInfo()
{
    VideoManager->Move(110.0f, 560.0f);
    _average_text.Draw();
    VideoManager->MoveRelative(-10.0f, 25.0f);
    _average_atk_def_text.Draw();
    VideoManager->MoveRelative(230.0f, 3.0f);
    _average_atk_def_icons.Draw();
    VideoManager->MoveRelative(25.0f, -3.0f);
    _average_atk_def_numbers.Draw();

    VideoManager->MoveRelative(40.0f, 0.0f);
    _weapon_icon.Draw();

    VideoManager->MoveRelative(125.0f, -25.0f);
    _focused_text.Draw();
    VideoManager->MoveRelative(-10.0f, 25.0f);
    _focused_def_text.Draw();
    VideoManager->MoveRelative(80.0f, 20.0f);
    _focused_def_category_icons.Draw();
    VideoManager->MoveRelative(25.0f, 0.0f);
    for (uint32 i = 0; i < 4; ++i) {
         _focused_def_armor_icons[i].Draw();
         VideoManager->MoveRelative(0.0f, 20.0f);
    }
    VideoManager->MoveRelative(50.0f, -100.0f);
    _focused_def_icon.Draw();
    _focused_def_numbers.Draw();

    VideoManager->MoveRelative(50.0f, 0.0f);
    _focused_mdef_icon.Draw();
    _focused_mdef_numbers.Draw();
}

// Draws the party window
void PartyWindow::Draw()
{
    MenuWindow::Draw();
    _char_select.Draw();
    _second_char_select.Draw();

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);

     //Draw character full body portrait
    VideoManager->Move(440.0f, 130.0f);
    _full_portraits[_char_select.GetSelection()].Draw();

    VideoManager->Move(660.0f, 130.0f);
    _character_status_text.Draw();
    VideoManager->MoveRelative(200.0f, 0.0f);
    _character_status_numbers.Draw();
    VideoManager->MoveRelative(-25.0f, 67.0f);
    _character_status_icons.Draw();

    if (GetActiveState() != FORM_ACTIVE_NONE) {
        VideoManager->Move(450.0f, 500.0f);
        _help_text.Draw();
        // Draw equipment info
        _DrawBottomEquipmentInfo();
    }
} // void PartyWindow::Draw()

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
    _description.SetPosition(90.0f, 465.0f);
    _description.SetDimensions(740.0f, 80.0f);
    _description.SetDisplaySpeed(30);
    _description.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _description.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _description.SetTextStyle(TextStyle("text20"));

} // SkillsWindow::SkillsWindow()



void SkillsWindow::Activate(bool new_status)
{
    // Activate window and first option box...or deactivate both
    if(new_status) {
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
        _active_box = SKILL_ACTIVE_CHAR;
    } else {
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _active_box = SKILL_ACTIVE_NONE;
    }
}



void SkillsWindow::_InitSkillsList()
{
    // Set up the inventory option box
    _skills_list.SetPosition(500.0f, 170.0f);
    _skills_list.SetDimensions(180.0f, 360.0f, 1, 255, 1, 6);
    _skills_list.SetTextStyle(TextStyle("text20"));
    _skills_list.SetCursorOffset(-52.0f, -20.0f);
    _skills_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _skills_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _skills_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

    _UpdateSkillList();
    if(_skills_list.GetNumberOptions() > 0)
        _skills_list.SetSelection(0);
    _skills_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    // setup the cost option box
    _skill_cost_list.SetPosition(700.0f, 170.0f);
    _skill_cost_list.SetDimensions(180.0f, 360.0f, 1, 255, 1, 6);
    _skill_cost_list.SetTextStyle(TextStyle("text20"));
    _skill_cost_list.SetCursorOffset(-52.0f, -20.0f);
    _skill_cost_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _skill_cost_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _skill_cost_list.SetOptionAlignment(VIDEO_X_RIGHT, VIDEO_Y_CENTER);
    _skill_cost_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}



void SkillsWindow::_InitCharSelect()
{
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


    //Use blank strings....won't be seen anyway
    for(uint32 i = 0; i < size; i++) {
        options.push_back(MakeUnicodeString(" "));
    }

    //Set options, selection and cursor state
    _char_select.SetOptions(options);
    _char_select.SetSelection(0);
    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}



void SkillsWindow::_InitSkillsCategories()
{
    _skills_categories.SetPosition(458.0f, 120.0f);
    _skills_categories.SetDimensions(448.0f, 30.0f, SKILL_CATEGORY_SIZE, 1, SKILL_CATEGORY_SIZE, 1);
    _skills_categories.SetTextStyle(TextStyle("text20"));
    _skills_categories.SetCursorOffset(-52.0f, -20.0f);
    _skills_categories.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
    _skills_categories.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _skills_categories.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);

    // Create options
    std::vector<ustring> options;
    options.push_back(UTranslate("All"));
    options.push_back(UTranslate("Field"));
    options.push_back(UTranslate("Battle"));

    // Set options and default selection
    _skills_categories.SetOptions(options);
    _skills_categories.SetSelection(SKILL_ALL);
    _skills_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
} // void SkillsWindow::InitSkillsCategories()



void SkillsWindow::Update()
{
    OptionBox *active_option = NULL;

    //choose correct menu
    switch(_active_box) {
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
    if(InputManager->ConfirmPress()) {
        active_option->InputConfirm();
    } else if(InputManager->CancelPress()) {
        active_option->InputCancel();
    } else if(InputManager->LeftPress()) {
        active_option->InputLeft();
    } else if(InputManager->RightPress()) {
        active_option->InputRight();
    } else if(InputManager->UpPress()) {
        active_option->InputUp();
    } else if(InputManager->DownPress()) {
        active_option->InputDown();
    }

    uint32 event = active_option->GetEvent();
    active_option->Update();
    switch(_active_box) {
    case SKILL_ACTIVE_CHAR_APPLY:
        // Handle skill application
        if(event == VIDEO_OPTION_CONFIRM) {
            GlobalSkill *skill = _GetCurrentSkill();
            GlobalCharacter *target = dynamic_cast<GlobalCharacter *>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));
            GlobalCharacter *instigator = dynamic_cast<GlobalCharacter *>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_skillset));

            const ScriptObject &script_function = skill->GetFieldExecuteFunction();

            if(!script_function.is_valid()) {
                IF_PRINT_WARNING(MENU_DEBUG) << "selected skill may not be executed in menus" << std::endl;
                break;
            }
            if(skill->GetSPRequired() > instigator->GetSkillPoints()) {
                IF_PRINT_WARNING(MENU_DEBUG) << "did not have enough skill points to execute skill " << std::endl;
                break;
            }
            ScriptCallFunction<void>(script_function, target, instigator);
            instigator->SubtractSkillPoints(skill->GetSPRequired());
            MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
        } else if(event == VIDEO_OPTION_CANCEL) {
            _active_box = SKILL_ACTIVE_LIST;
            _skills_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            _char_select.SetSelection(_char_skillset);
            MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
        }
        break;

    case SKILL_ACTIVE_CHAR:
        // Choose character for skillset
        if(event == VIDEO_OPTION_CONFIRM) {
            _active_box = SKILL_ACTIVE_CATEGORY;
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            _skills_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _char_skillset = _char_select.GetSelection();
            MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
        } else if(event == VIDEO_OPTION_CANCEL) {
            Activate(false);
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
        }
        break;

    case SKILL_ACTIVE_LIST:
        // Choose skill
        if(event == VIDEO_OPTION_CONFIRM) {
            GlobalSkill *skill = _GetCurrentSkill();
            if(skill->IsExecutableInField()) {
                _active_box = SKILL_ACTIVE_CHAR_APPLY;
                _skills_list.SetCursorState(VIDEO_CURSOR_STATE_DARKEN);
                _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
            } else
                MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
        } else if(event == VIDEO_OPTION_CANCEL) {
            _active_box = SKILL_ACTIVE_CATEGORY;
            MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
            _skills_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            _skills_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
        }
        break;

    case SKILL_ACTIVE_CATEGORY:
        // Choose skill type
        if(event == VIDEO_OPTION_CONFIRM) {
            _skills_list.SetSelection(0);
            if(_skills_list.GetNumberOptions() > 0) {
                _active_box = SKILL_ACTIVE_LIST;
                _skills_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                _skills_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
            } else {
                MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
            }
        } else if(event == VIDEO_OPTION_CANCEL) {
            _active_box = SKILL_ACTIVE_CHAR;
            MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
            _skills_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _char_select.SetSelection(_char_skillset);
        }
        break;
    }

    if(_active_box != SKILL_ACTIVE_CHAR_APPLY)
        _UpdateSkillList();

    if(_skills_list.GetNumberOptions() > 0 && _skills_list.GetSelection() >= 0
            && static_cast<int32>(_skills_list.GetNumberOptions()) > _skills_list.GetSelection()) {

        GlobalSkill *skill = _GetCurrentSkill();
        GlobalCharacter *skill_owner = dynamic_cast<GlobalCharacter *>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_skillset));

        // Get the skill type
        hoa_utils::ustring skill_type;
        switch(skill->GetType()) {
            case GLOBAL_SKILL_WEAPON:
                if (skill_owner->GetWeaponEquipped())
                    skill_type = UTranslate("Weapon skill");
                else
                    skill_type = UTranslate("Bare hands");
                break;
            case GLOBAL_SKILL_MAGIC:
                skill_type = UTranslate("Magic skill");
                break;
            case GLOBAL_SKILL_SPECIAL:
                if (skill_owner)
                    skill_type = skill_owner->GetSpecialCategoryName();
                else
                    skill_type = UTranslate("Special skill");
                break;
            default:
            break;
        }

        hoa_utils::ustring description = skill->GetName();
        if (!skill_type.empty())
            description += MakeUnicodeString("  (") + skill_type + MakeUnicodeString(")");

        description += MakeUnicodeString("\n\n");
        description += skill->GetDescription();
        _description.SetDisplayText(description);

        // Load the skill icon
        if (!skill->GetIconFilename().empty()) {
            _skill_icon.Load(skill->GetIconFilename());
            if (_skill_icon.GetHeight() > 70)
                _skill_icon.SetHeightKeepRatio(70);
        }
        else
            _skill_icon.Clear();
    }

} // void SkillsWindow::Update()

GlobalSkill *SkillsWindow::_GetCurrentSkill()
{
    GlobalCharacter *ch = dynamic_cast<GlobalCharacter *>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_skillset));

    std::vector<GlobalSkill *> menu_skills;
    std::vector<GlobalSkill *> battle_skills;
    std::vector<GlobalSkill *> all_skills;

    if (ch->GetWeaponEquipped())
        _BuildMenuBattleSkillLists(ch->GetWeaponSkills(), &menu_skills, &battle_skills, &all_skills);
    else
        _BuildMenuBattleSkillLists(ch->GetBareHandsSkills(), &menu_skills, &battle_skills, &all_skills);
    _BuildMenuBattleSkillLists(ch->GetMagicSkills(), &menu_skills, &battle_skills, &all_skills);
    _BuildMenuBattleSkillLists(ch->GetSpecialSkills(), &menu_skills, &battle_skills, &all_skills);

    GlobalSkill *skill;
    switch(_skills_categories.GetSelection()) {
    case SKILL_ALL:
        skill = all_skills.at(_skills_list.GetSelection());
        break;
    case SKILL_BATTLE:
        skill = battle_skills.at(_skills_list.GetSelection());
        break;
    case SKILL_FIELD:
        skill = menu_skills.at(_skills_list.GetSelection());
        break;
    default:
        skill = NULL;
        PRINT_ERROR << "MENU ERROR: Invalid skill type in SkillsWindow::_GetCurrentSkill()" << std::endl;
        break;
    }

    return skill;
}


void SkillsWindow::_UpdateSkillList()
{
    GlobalCharacter *ch = dynamic_cast<GlobalCharacter *>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));
    assert(ch);
    std::vector<ustring> options;
    std::vector<ustring> cost_options;

    std::vector<GlobalSkill *> menu_skills;
    std::vector<GlobalSkill *> battle_skills;
    std::vector<GlobalSkill *> all_skills;

    // Iterators
    std::vector<GlobalSkill *>::const_iterator it;
    std::vector<GlobalSkill*>::const_iterator it_begin;
    std::vector<GlobalSkill*>::const_iterator it_end;

    if (ch->GetWeaponEquipped())
        _BuildMenuBattleSkillLists(ch->GetWeaponSkills(), &menu_skills, &battle_skills, &all_skills);
    else
        _BuildMenuBattleSkillLists(ch->GetBareHandsSkills(), &menu_skills, &battle_skills, &all_skills);
    _BuildMenuBattleSkillLists(ch->GetMagicSkills(), &menu_skills, &battle_skills, &all_skills);
    _BuildMenuBattleSkillLists(ch->GetSpecialSkills(), &menu_skills, &battle_skills, &all_skills);

    switch(_skills_categories.GetSelection()) {
    case SKILL_ALL:
        it_begin = all_skills.begin();
        it_end = all_skills.end();
        break;
    case SKILL_BATTLE:
        it_begin = battle_skills.begin();
        it_end = battle_skills.end();
        break;
    case SKILL_FIELD:
        it_begin = menu_skills.begin();
        it_end = menu_skills.end();
        break;
    default:
        break;
    }

    for(it = it_begin; it != it_end; ++it) {
        // Check for the existence of an icon
        hoa_utils::ustring name;
        if((*it)->GetIconFilename().empty()) {
            // If no icon, use the weapon icon for weapon skills
            if ((*it)->GetType() == GLOBAL_SKILL_WEAPON &&
                 ch->GetWeaponEquipped() && !ch->GetWeaponEquipped()->GetIconImage().GetFilename().empty())
                name = MakeUnicodeString("<" + ch->GetWeaponEquipped()->GetIconImage().GetFilename() + ">");
            else if ((*it)->GetType() == GLOBAL_SKILL_WEAPON)
                name = MakeUnicodeString("<img/icons/weapons/fist-human.png>");

            name += MakeUnicodeString("<45>") + (*it)->GetName();
        }
        else {
            name += MakeUnicodeString("<" + (*it)->GetIconFilename() + "><45>") + (*it)->GetName();
        }

        options.push_back(name);

        std::string cost = NumberToString((*it)->GetSPRequired()) + Translate(" SP");
        cost_options.push_back(MakeUnicodeString(cost));
    }

    _skills_list.SetOptions(options);
    _skill_cost_list.SetOptions(cost_options);


    // Resize icons if necessary
    for (uint32 i = 0; i < _skills_list.GetNumberOptions(); ++i) {
        if (StillImage *image = _skills_list.GetEmbeddedImage(i))
            image->SetHeightKeepRatio(45);
    }

}

void SkillsWindow::_BuildMenuBattleSkillLists(std::vector<GlobalSkill *> *skill_list,
        std::vector<GlobalSkill *> *field, std::vector<GlobalSkill *> *battle, std::vector<GlobalSkill *> *all)
{
    std::vector<GlobalSkill *>::iterator it;
    for(it = skill_list->begin(); it != skill_list->end(); ++it) {
        if((*it)->IsExecutableInBattle())
            battle->push_back(*it);
        if((*it)->IsExecutableInField())
            field->push_back(*it);
        all->push_back(*it);
    }
}


void SkillsWindow::Draw()
{
    MenuWindow::Draw();

    //Draw option boxes
    _char_select.Draw();
    _skills_categories.Draw();
    if(_active_box == SKILL_ACTIVE_NONE)
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


EquipWindow::~EquipWindow()
{
}


void EquipWindow::Activate(bool new_status, bool equip)
{

    _equip = equip;

    //Activate window and first option box...or deactivate both
    if(new_status) {
        _active_box = EQUIP_ACTIVE_CHAR;
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    } else {
        _active_box = EQUIP_ACTIVE_NONE;
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    }
}


void EquipWindow::_InitEquipmentList()
{
    // Set up the inventory option box
    _equip_list.SetPosition(500.0f, 170.0f);
    _equip_list.SetDimensions(400.0f, 360.0f, 1, 255, 1, 6);
    _equip_list.SetTextStyle(TextStyle("text20"));

    _equip_list.SetCursorOffset(-52.0f, -20.0f);
    _equip_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_SHIFTED);
    _equip_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _equip_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    // Update the equipment list
    _UpdateEquipList();
    if(_equip_list.GetNumberOptions() > 0) {
        _equip_list.SetSelection(0);
    }
    // Initially hide the cursor
    _equip_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}



void EquipWindow::_InitCharSelect()
{
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

    //Use blank strings....won't be seen anyway
    for(uint32 i = 0; i < size; i++) {
        options.push_back(MakeUnicodeString(" "));
    }

    //Set options, selection and cursor state
    _char_select.SetOptions(options);
    _char_select.SetSelection(0);
    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

} // void EquipWindow::InitCharSelect()



void EquipWindow::_InitEquipmentSelect()
{
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



void EquipWindow::Update()
{
    // Points to the active option box
    OptionBox *active_option = NULL;

    //choose correct menu
    switch(_active_box) {
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
    if(InputManager->ConfirmPress()) {
        active_option->InputConfirm();
    } else if(InputManager->CancelPress()) {
        active_option->InputCancel();
    } else if(InputManager->LeftPress()) {
        active_option->InputLeft();
    } else if(InputManager->RightPress()) {
        active_option->InputRight();
    } else if(InputManager->UpPress()) {
        active_option->InputUp();
    } else if(InputManager->DownPress()) {
        active_option->InputDown();
    }

    uint32 event = active_option->GetEvent();
    active_option->Update();
    switch(_active_box) {
        //Choose character
    case EQUIP_ACTIVE_CHAR:
        if(event == VIDEO_OPTION_CONFIRM) {
            _active_box = EQUIP_ACTIVE_SELECT;
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_DARKEN);
            _equip_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
        } else if(event == VIDEO_OPTION_CANCEL) {
            Activate(false, true);
            MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
        }
        break;

        //Choose equipment to replace/remove
    case EQUIP_ACTIVE_SELECT:
        if(event == VIDEO_OPTION_CONFIRM) {
            // Equip mode
            if(_equip) {
                _active_box = EQUIP_ACTIVE_LIST;
                _UpdateEquipList();
                if(_equip_list.GetNumberOptions() > 0) {
                    _equip_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                    _equip_list.SetSelection(0);
                    _equip_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                    MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
                } else {
                    _active_box = EQUIP_ACTIVE_SELECT;
                    MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
                }
            } // Unequip mode
            else {
                GlobalCharacter *ch = dynamic_cast<GlobalCharacter *>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));
                switch(_equip_select.GetSelection()) {
                    // Unequip and return the old weapon to inventory
                case EQUIP_WEAPON:
                    GlobalManager->AddToInventory(ch->EquipWeapon(NULL));
                    break;
                case EQUIP_HEADGEAR:
                    GlobalManager->AddToInventory(ch->EquipHeadArmor(NULL));
                    break;
                case EQUIP_BODYARMOR:
                    GlobalManager->AddToInventory(ch->EquipTorsoArmor(NULL));
                    break;
                case EQUIP_OFFHAND:
                    GlobalManager->AddToInventory(ch->EquipArmArmor(NULL));
                    break;
                case EQUIP_LEGGINGS:
                    GlobalManager->AddToInventory(ch->EquipLegArmor(NULL));
                    break;
                default:
                    PRINT_WARNING << "Unequip slot is invalid: " << _equip_select.GetSelection() << std::endl;
                    break;
                }
            } // Equip/Unequip
        } // Confirm
        else if(event == VIDEO_OPTION_CANCEL) {
            _active_box = EQUIP_ACTIVE_CHAR;
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _equip_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
        }
        break;

        // Choose replacement when equipping
    case EQUIP_ACTIVE_LIST:
        if(event == VIDEO_OPTION_CONFIRM) {
            GlobalCharacter *ch = dynamic_cast<GlobalCharacter *>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));
            // Equipment global Id.
            uint32 id_num = 0;
            // Get the actual inventory index.
            uint32 inventory_id = _equip_list_inv_index[_equip_list.GetSelection()];

            switch(_equip_select.GetSelection()) {
            case EQUIP_WEAPON: {
                GlobalWeapon *wpn = GlobalManager->GetInventoryWeapons()->at(inventory_id);
                if(wpn->GetUsableBy() & ch->GetID()) {
                    id_num = wpn->GetID();
                    GlobalManager->AddToInventory(ch->EquipWeapon((GlobalWeapon *)GlobalManager->RetrieveFromInventory(id_num)));
                } else {
                    MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
                }
                break;
            }

            case EQUIP_HEADGEAR: {
                GlobalArmor *hlm = GlobalManager->GetInventoryHeadArmor()->at(inventory_id);
                if(hlm->GetUsableBy() & ch->GetID()) {
                    id_num = hlm->GetID();
                    GlobalManager->AddToInventory(ch->EquipHeadArmor((GlobalArmor *)GlobalManager->RetrieveFromInventory(id_num)));
                } else {
                    MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
                }
                break;
            }

            case EQUIP_BODYARMOR: {
                GlobalArmor *arm = GlobalManager->GetInventoryTorsoArmor()->at(inventory_id);
                if(arm->GetUsableBy() & ch->GetID()) {
                    id_num = arm->GetID();
                    GlobalManager->AddToInventory(ch->EquipTorsoArmor((GlobalArmor *)GlobalManager->RetrieveFromInventory(id_num)));
                } else {
                    MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
                }
                break;
            }

            case EQUIP_OFFHAND: {
                GlobalArmor *shld = GlobalManager->GetInventoryArmArmor()->at(inventory_id);
                if(shld->GetUsableBy() & ch->GetID()) {
                    id_num = shld->GetID();
                    GlobalManager->AddToInventory(ch->EquipArmArmor((GlobalArmor *)GlobalManager->RetrieveFromInventory(id_num)));
                } else {
                    MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
                }
                break;
            }

            case EQUIP_LEGGINGS: {
                GlobalArmor *lgs = GlobalManager->GetInventoryLegArmor()->at(inventory_id);
                if(lgs->GetUsableBy() & ch->GetID()) {
                    id_num = lgs->GetID();
                    GlobalManager->AddToInventory(ch->EquipLegArmor((GlobalArmor *)GlobalManager->RetrieveFromInventory(id_num)));
                } else {
                    MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
                }
                break;
            }

            default:
                PRINT_WARNING << "Equip slot value is invalid: "
                              << inventory_id << std::endl;
                break;
            } // switch _equip_select.GetSelection()

            _active_box = EQUIP_ACTIVE_SELECT;
            _equip_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            _equip_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
        } // if VIDEO_OPTION_CONFIRM
        else if(event == VIDEO_OPTION_CANCEL) {
            _active_box = EQUIP_ACTIVE_SELECT;
            MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
            _equip_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            _equip_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
        } // else if VIDEO_OPTION_CANCEL
        break;
    } // switch _active_box

    _UpdateEquipList();
} // void EquipWindow::Update()



void EquipWindow::_UpdateEquipList()
{
    GlobalCharacter *ch = dynamic_cast<GlobalCharacter *>(GlobalManager->GetActiveParty()->GetActorAtIndex(_char_select.GetSelection()));
    std::vector<ustring> options;

    if(_active_box == EQUIP_ACTIVE_LIST) {
        uint32 gearsize = 0;
        std::vector<GlobalObject *>* equipment_list = NULL;

        switch(_equip_select.GetSelection()) {
        case EQUIP_WEAPON:
            equipment_list = reinterpret_cast<std::vector<GlobalObject *>*>(GlobalManager->GetInventoryWeapons());
            break;
        case EQUIP_HEADGEAR:
            equipment_list = reinterpret_cast<std::vector<GlobalObject *>*>(GlobalManager->GetInventoryHeadArmor());
            break;
        case EQUIP_BODYARMOR:
            equipment_list = reinterpret_cast<std::vector<GlobalObject *>*>(GlobalManager->GetInventoryTorsoArmor());
            break;
        case EQUIP_OFFHAND:
            equipment_list = reinterpret_cast<std::vector<GlobalObject *>*>(GlobalManager->GetInventoryArmArmor());
            break;
        case EQUIP_LEGGINGS:
            equipment_list = reinterpret_cast<std::vector<GlobalObject *>*>(GlobalManager->GetInventoryLegArmor());
            break;
        } // switch

        if(equipment_list != NULL)
            gearsize = equipment_list->size();

        // Clear the replacer ids
        _equip_list_inv_index.clear();
        // Add the options
        for(uint32 j = 0; j < gearsize; j++) {
            uint32 usability_bitmask = 0;
            if(_equip_select.GetSelection() == EQUIP_WEAPON) {
                GlobalWeapon *selected_weapon = dynamic_cast<GlobalWeapon *>(equipment_list->at(j));
                usability_bitmask = selected_weapon->GetUsableBy();
            } else {
                GlobalArmor *selected_armor = dynamic_cast<GlobalArmor *>(equipment_list->at(j));
                usability_bitmask = selected_armor->GetUsableBy();
            }

            // If the character can't equip the item, don't show it.
            if(_equip && !(usability_bitmask & ch->GetID()))
                continue;

            options.push_back(MakeUnicodeString("<") +
                              MakeUnicodeString(equipment_list->at(j)->GetIconImage().GetFilename()) +
                              MakeUnicodeString("><70>") +
                              equipment_list->at(j)->GetName());

            // Add the actual inventory index
            _equip_list_inv_index.push_back(j);
        }

        _equip_list.SetOptions(options);
    } // if EQUIP_ACTIVE_LIST

    else {
        // First, update the IMAGES of the equipped items
        _equip_images.clear();
        StillImage i;

        GlobalWeapon *wpn = ch->GetWeaponEquipped();
        i.Load(wpn ? wpn->GetIconImage().GetFilename() : "img/icons/weapons/fist-human.png");
        _equip_images.push_back(i);

        GlobalArmor *head_armor = ch->GetHeadArmorEquipped();
        i.Load(head_armor ? head_armor->GetIconImage().GetFilename() : "");
        _equip_images.push_back(i);

        GlobalArmor *torso_armor = ch->GetTorsoArmorEquipped();
        i.Load(torso_armor ? torso_armor->GetIconImage().GetFilename() : "");
        _equip_images.push_back(i);

        GlobalArmor *arm_armor = ch->GetArmArmorEquipped();
        i.Load(arm_armor ? arm_armor->GetIconImage().GetFilename() : "");
        _equip_images.push_back(i);

        GlobalArmor *leg_armor = ch->GetLegArmorEquipped();
        i.Load(leg_armor ? leg_armor->GetIconImage().GetFilename() : "");
        _equip_images.push_back(i);

        // Now, update the NAMES of the equipped items
        options.push_back(wpn ? wpn->GetName() : UTranslate("No weapon"));
        options.push_back(head_armor ? head_armor->GetName() : UTranslate("No head armor"));
        options.push_back(torso_armor ? torso_armor->GetName() : UTranslate("No torso armor"));
        options.push_back(arm_armor ? arm_armor->GetName() : UTranslate("No arm armor"));
        options.push_back(leg_armor ? leg_armor->GetName() : UTranslate("No leg armor"));

        _equip_select.SetOptions(options);
    }

} // void EquipWindow::UpdateEquipList()



void EquipWindow::Draw()
{
    MenuWindow::Draw();
    _UpdateEquipList();

    //Draw option boxes
    _char_select.Draw();

    if(_active_box == EQUIP_ACTIVE_LIST) {
        _equip_list.Draw();
        VideoManager->Move(660.0f, 135.0f);
        VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
        switch(_equip_select.GetSelection()) {
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
    } else {
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

        for(uint32 i = 0; i < _equip_images.size(); i++) {
            VideoManager->MoveRelative(0.0f, 70.0f);
            _equip_images[i].Draw();
        }
    }

} // void EquipWindow::Draw()

////////////////////////////////////////////////////////////////////////////////
// QuestListWindow Class
////////////////////////////////////////////////////////////////////////////////

// TODO: This should be const. OptionsBox doesn't take const ustrings.
static ustring spacing = MakeUnicodeString("<20>");
static ustring exclamation_file = MakeUnicodeString("<img/effects/emotes/exclamation.png>") + spacing;
static ustring check_file = MakeUnicodeString("<img/menus/green_check.png>") + spacing;

QuestListWindow::QuestListWindow() :
    _active_box(false)
{
    _quests_list.SetPosition(92.0f, 145.0f);
    _quests_list.SetDimensions(330.0f, 375.0f, 1, 255, 1, 8);
    //set the cursor offset next to where the exclamation point would be.
    //this prevents the arrow jumping
    _quests_list.SetCursorOffset(-75.0f, -15.0f);
    _quests_list.SetTextStyle(TextStyle("text20"));
    _quests_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_NONE);
    _quests_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _quests_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _quests_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    // enable viewing of grey options
    _quests_list.SetSkipDisabled(false);

    _SetupQuestsList();
}

void QuestListWindow::Draw()
{
    // Draw the menu area
    MenuWindow::Draw();

    // Draw the quest log list
    _quests_list.Draw();
}

void QuestListWindow::Update()
{
    //if empty, exit out immediatly
    if(GlobalManager->GetNumberQuestLogEntries() == 0)
    {
        MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
        _active_box = false;
        return;
    }

    // quest log is fairly simple. it only responds
    // to up / down and cancel
    if(InputManager->CancelPress()) {
        _quests_list.InputCancel();
    } else if(InputManager->UpPress()) {
        _quests_list.InputUp();
    } else if(InputManager->DownPress()) {
        _quests_list.InputDown();
    }

    uint32 event = _quests_list.GetEvent();
    // cancel and exit
    if(event == VIDEO_OPTION_CANCEL) {
        _active_box = false;
        _quests_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
    }
    //standard upate of quest list
    _UpdateQuestList();


}

/**
*** \brief sorts the quest log keys based on the actual quest entry number, putting the highest value at the top of the list
**/

static inline bool sort_by_number_reverse(QuestLogEntry* quest_log1, QuestLogEntry* quest_log2)
{
    return (quest_log1->GetQuestLogNumber() > quest_log2->GetQuestLogNumber());
}

void QuestListWindow::_UpdateQuestList()
{
    if(GlobalManager->GetNumberQuestLogEntries() == 0)
    {
        //set the QuestWindow key to "NULL", which is actually ""
        MenuMode::CurrentInstance()->_quest_window.SetViewingQuestId(std::string());
        return;
    }

    // Get the cursor selection
    int32 selection = _quests_list.GetSelection();

    QuestLogEntry *entry = _quest_entries[selection];
    const std::string& quest_id = entry->GetQuestId();
    ustring title = GlobalManager->GetQuestInfo(quest_id)._title;
    if(GlobalManager->IsQuestCompleted(quest_id))
    {
        _quests_list.SetOptionText(selection, check_file + title);
        _quests_list.SetCursorOffset(-55.0f, -15.0f);
        _quests_list.EnableOption(selection, false);
    }
    else
    {
        _quests_list.SetCursorOffset(-75.0f, -15.0f);
        _quests_list.SetOptionText(selection, spacing + title);
    }

    entry->SetRead();

    // Update the list box
    _quests_list.Update(SystemManager->GetUpdateTime());

    // Set the QuestWindow quest key value to the selected quest
    MenuMode::CurrentInstance()->_quest_window.SetViewingQuestId(quest_id);
}

void QuestListWindow::Activate(bool new_state)
{
    if(new_state)
    {
        _quests_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
        _quests_list.ResetViewableOption();
        _quests_list.SetSelection(0);
    }
    else
    {
        _quests_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    }
    _active_box = new_state;
}

void QuestListWindow::_SetupQuestsList() {
    // Recreate the quest log entries list.
    _quest_entries.clear();
    _quest_entries = GlobalManager->GetActiveQuestIds();

    // Recreate the quest option box list as well
    _quests_list.ClearOptions();

    // Reorder by sorting via the entry number
    std::sort(_quest_entries.begin(), _quest_entries.end(), sort_by_number_reverse);

    // Check whether some should be set as completed.
    for(uint32 i = 0; i < _quest_entries.size(); ++i)
    {
        QuestLogEntry *entry = _quest_entries[i];
        const std::string& quest_id = entry->GetQuestId();
        ustring title = GlobalManager->GetQuestInfo(quest_id)._title;

        //completed quest check.
        if(GlobalManager->IsQuestCompleted(quest_id)) {
            _quests_list.AddOption(check_file + title);
            _quests_list.EnableOption(i, false);
        }
        //if incomplete, then we check the read status
        else if(entry->IsRead())
            _quests_list.AddOption(spacing + title);
        else
            _quests_list.AddOption(exclamation_file + title);
    }
}

////////////////////////////////////////////////////////////////////////////////
// QuestWindow Class
////////////////////////////////////////////////////////////////////////////////

QuestWindow::QuestWindow():
    _location_image(NULL),
    _location_subimage(NULL)
{
    _quest_description.SetPosition(445, 130);
    _quest_description.SetDimensions(455, 200);
    _quest_description.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _quest_description.SetTextStyle(TextStyle("text20"));
    _quest_description.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);

    // preferably, we want the completion description to show underneath the description text
    // last line. Unfortunatly, it seems CalculateTextHeight() doesn't work right
    // so we fix the position for now
    _quest_completion_description.SetPosition(445, 350);
    _quest_completion_description.SetDimensions(455, 200);
    _quest_completion_description.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _quest_completion_description.SetTextStyle(TextStyle("text20", Color::aqua));
    _quest_completion_description.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);

    _location_name.SetPosition(102, 556);
    _location_name.SetDimensions(500.0f, 150.0f);
    _location_name.SetTextStyle(TextStyle("text22"));
    _location_name.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _location_name.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);

    _location_subname.SetPosition(500, 556);
    _location_subname.SetDimensions(500.0f, 150.0f);
    _location_subname.SetTextStyle(TextStyle("text22"));
    _location_subname.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _location_subname.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);


}

void QuestWindow::Draw()
{
    MenuWindow::Draw();
    Update();
    if(MenuMode::CurrentInstance()->_quest_list_window.IsActive())
    {
        _quest_description.Draw();
        _quest_completion_description.Draw();
    }

}

void QuestWindow::DrawBottom()
{
    // Display Location name information
    _location_name.Draw();
    _location_subname.Draw();


    //check location image and draw
    if(_location_image != NULL && _location_image->GetFilename().empty() == false) {
        VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, 0);
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        VideoManager->Move(102, 685);
        _location_image->Draw();
    }

    //check location subimage and draw
    if(_location_subimage != NULL && _location_subimage->GetFilename().empty() == false) {
        VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, 0);
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        VideoManager->Move(500, 685);
        _location_subimage->Draw();
    }
}

void QuestWindow::Update()
{
    MenuWindow::Update();

    // Check to see if the id is empty or if the quest doesn't exist. if so, draw an empty space
    if(_viewing_quest_id.empty()) {
        _quest_description.ClearText();
        _location_name.ClearText();
        _location_subname.ClearText();
        _location_image = NULL;
        _location_subimage = NULL;
        return;
    }

    // otherwise, put the text description for the quest in
    // Not calling ClearText each time will permit to set up the textbox text only when necessary
    QuestLogInfo& info = GlobalManager->GetQuestInfo(_viewing_quest_id);
    if(!info._description.empty())
    {
        _quest_description.SetDisplayText(info._description);
        _location_name.SetDisplayText(info._location_name);
        _location_subname.SetDisplayText(info._location_subname);
        _location_image = &info._location_image;
        _location_subimage = &info._location_subimage;

        //set the completion description only if the quest is completed
        if(GlobalManager->IsQuestCompleted(_viewing_quest_id))
            _quest_completion_description.SetDisplayText(info._completion_description);
        else
            _quest_completion_description.ClearText();
    }

}

///////////////////////////
/// WorldMapWindow class
///////////////////////////

WorldMapWindow::WorldMapWindow() :
    _current_world_map(NULL),
    _current_image_x_offset(0),
    _current_image_y_offset(0),
    _location_pointer_index(0),
    _active(false)
{
    _location_marker.SetStatic(true);
    if(!_location_marker.LoadFromAnimationScript("img/menus/rotating_crystal_grey.lua"))
        PRINT_ERROR << "Could not load marker image!" << std::endl;

    _location_pointer.SetStatic(true);
    if(!_location_pointer.Load("img/menus/hand_down.png"))
        PRINT_ERROR << "Could not load pointer image!" << std::endl;
}

void WorldMapWindow::Draw()
{
    MenuWindow::Draw();
    if(_current_world_map == NULL)
        return;
    float window_position_x, window_position_y;
    GetPosition(window_position_x, window_position_y);
    VideoManager->Move(window_position_x + _current_image_x_offset, window_position_y + _current_image_y_offset);

    _current_world_map->Draw();

    //draw the dots and currently selected location if active
    if(IsActive())
    {
        //get the list of currently viewable world locations with the pointer
        _DrawViewableLocations(window_position_x, window_position_y);
    }
}

void WorldMapWindow::_DrawViewableLocations(float window_position_x, float window_position_y)
{
    const std::vector<std::string> &current_location_ids = GlobalManager->GetViewableLocationIds();
    const uint32 N = current_location_ids.size();
    for(uint32 i = 0; i < N; ++i)
    {
        const WorldMapLocation *location = GlobalManager->GetWorldLocation(current_location_ids[i]);
        if(location == NULL)
        {
            PRINT_WARNING << "location for id: "
                << current_location_ids[i]
                << " is not loaded into global manager" << std::endl;
            continue;
        }
        //draw the location marker
        VideoManager->Move(window_position_x + _current_image_x_offset + location->_x, window_position_y + _current_image_y_offset + location->_y);
        _location_marker.Draw();

        //draw the pointer
        if(i == _location_pointer_index)
        {
            //this is a slight offset for the pointer so that it points where we want it to, roughly in the center
            //of the location marker
            static const float minor_offset_x = 2.0f;
            static const float minor_offset_y = -8.0f;
            VideoManager->Move(window_position_x + _current_image_x_offset + location->_x + minor_offset_x,
                               window_position_y + _current_image_y_offset + location->_y - _location_pointer.GetHeight() + minor_offset_y);
            _location_pointer.Draw();
        }

    }
}

void WorldMapWindow::Update()
{
    _current_world_map = GlobalManager->GetWorldMapImage();
    if(_current_world_map == NULL)
    {
        _active = false;
        return;
    }

    float image_width = _current_world_map->GetWidth();
    float image_height = _current_world_map->GetHeight();
    float window_width, window_height;
    GetDimensions(window_width, window_height);

    if( image_width > window_width)
        PRINT_WARNING << "World Map Image Width is too wide " << std::endl;

    //calculate neccesary offset from left.
    _current_image_x_offset = (window_width - image_width) / 2.0;
    //calculae neccesary offset fromtop
    _current_image_y_offset = (window_height - image_height) / 2.0;

    //if this window is active, we check the cursor states
    if(IsActive())
    {
        WORLDMAP_NAVIGATION worldmap_goto = WORLDMAP_NOPRESS;
        if(InputManager->CancelPress()) {
            worldmap_goto = WORLDMAP_CANCEL;
        } else if(InputManager->LeftPress()) {
            worldmap_goto = WORLDMAP_LEFT;
        } else if(InputManager->RightPress()) {
            worldmap_goto = WORLDMAP_RIGHT;
        }

        // cancel and exit
        if(worldmap_goto == WORLDMAP_CANCEL) {
            _active = false;
            MenuMode::CurrentInstance()->_menu_sounds["cancel"].Play();
        }
        //otherwise check if there was a key press

        else if(worldmap_goto != WORLDMAP_NOPRESS)
        {
            //play confirm sound
            MenuMode::CurrentInstance()->_menu_sounds["confirm"].Play();
            _SetSelectedLocation(worldmap_goto);

        }

        _location_marker.Update();
    }
}

void WorldMapWindow::_SetSelectedLocation(WORLDMAP_NAVIGATION worldmap_goto)
{
    const std::vector<std::string> &current_location_ids = GlobalManager->GetViewableLocationIds();
    const uint32 N = current_location_ids.size();
    if(N == 0)
        return;
    if(worldmap_goto == WORLDMAP_LEFT)
    {
        //cannot use mod properly since it is an unsigned value
        if(_location_pointer_index == 0)
            _location_pointer_index = N-1;
        else
            _location_pointer_index -= 1;
    }
    else if (worldmap_goto == WORLDMAP_RIGHT)
    {
        if(_location_pointer_index == N-1)
            _location_pointer_index = 0;
        else
            _location_pointer_index += 1;
    }
}

void WorldMapWindow::Activate(bool new_state)
{
    _active = new_state;

    //set the pointer to the appropriate location
    //we only do this on activation of the window. after that it is handled by the left / right press
    const std::string &location_id = GlobalManager->GetCurrentLocationId();
    const std::vector<std::string> &current_location_ids = GlobalManager->GetViewableLocationIds();
    std::vector<std::string>::const_iterator loc = std::find(current_location_ids.begin(), current_location_ids.end(), location_id);
    if(location_id.empty() || loc == current_location_ids.end() )
        _location_pointer_index = 0;
    else
        _location_pointer_index = loc - current_location_ids.begin();

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
