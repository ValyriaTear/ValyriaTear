///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_windows/menu_inventory_window.h"

#include "modes/menu/menu_mode.h"

#include "common/global/actors/global_character.h"
#include "common/global/objects/global_weapon.h"
#include "common/global/objects/global_armor.h"

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/system.h"

using namespace vt_menu::private_menu;
using namespace vt_utils;
using namespace vt_audio;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_global;
using namespace vt_input;
using namespace vt_system;

namespace vt_menu
{

namespace private_menu
{

//! help message
static ustring inventory_help_message;
static ustring cannot_equip;
static ustring item_use;

InventoryWindow::InventoryWindow(MenuMode* mm) :
    _menu_mode(mm),
    _active_box(ITEM_ACTIVE_NONE),
    _previous_category(ITEM_ALL),
    _object(nullptr),
    _object_type(vt_global::GLOBAL_OBJECT_INVALID),
    _character(nullptr),
    _is_equipment(false),
    _can_equip(false)
{
    _InitCategory();
    _UpdateItemText();
    _InitInventoryItems();
    _InitCharSelect();

    //Initializes the description textbox for the bottom window
    _description.SetOwner(this);
    _description.SetPosition(30.0f, 525.0f);
    _description.SetDimensions(800.0f, 80.0f);
    _description.SetDisplaySpeed(SystemManager->GetMessageSpeed());
    _description.SetTextStyle(TextStyle("text20"));
    _description.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _description.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);

    // We update it here to permit reloading it in case the language has changed
    // since the game start.
    inventory_help_message = UTranslate("Select an item to Equip or Use.");
    cannot_equip = UTranslate("This character cannot equip this item.");
    item_use = UTranslate("Select a character to use the item on.");

} // void InventoryWindow::InventoryWindow

//Initializes the list of items
void InventoryWindow::_InitInventoryItems()
{
    // Set up the inventory option box
    _inventory_items.SetPosition(500.0f, 170.0f);
    _inventory_items.SetDimensions(400.0f, 360.0f, 1, 255, 1, 10);
    _inventory_items.SetTextStyle(TextStyle("text20"));
    _inventory_items.SetCursorOffset(-52.0f, -20.0f);
    _inventory_items.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _inventory_items.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

    // Update the item text
    _UpdateItemText();
    if(_inventory_items.GetNumberOptions() > 0) {
        _inventory_items.SetSelection(0);
        _object = _item_objects[_inventory_items.GetSelection()];
        _object_type = _object->GetObjectType();
    }
    VideoManager->MoveRelative(-65, 20);
    // Initially hide the cursor
    _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

void InventoryWindow::_InitCharSelect()
{
    //character selection set up
    std::vector<ustring> options;
    uint32_t size = GlobalManager->GetCharacterHandler().GetActiveParty().GetPartySize();

    _char_select.SetPosition(72.0f, 109.0f);
    _char_select.SetDimensions(360.0f, 432.0f, 1, 4, 1, 4);
    _char_select.SetCursorOffset(-50.0f, -6.0f);
    _char_select.SetTextStyle(TextStyle("text20"));
    _char_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _char_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _char_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

    //Use a blank string so the cursor has somewhere to point
    //String is overdrawn by char portraits, so no matter
    for(uint32_t i = 0; i < size; i++) {
        options.push_back(MakeUnicodeString(" "));
    }

    _char_select.SetOptions(options);
    _char_select.SetSelection(0);
    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _character = GlobalManager->GetCharacterHandler().GetActiveParty().GetCharacterAtIndex(_char_select.GetSelection());
}

void InventoryWindow::_InitCategory()
{
    _item_categories.SetPosition(458.0f, 120.0f);
    _item_categories.SetDimensions(448.0f, 30.0f,
                                   ITEM_CATEGORY_SIZE, 1, ITEM_CATEGORY_SIZE, 1);
    _item_categories.SetTextStyle(TextStyle("text20"));

    _item_categories.SetCursorOffset(-52.0f, -20.0f);
    _item_categories.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _item_categories.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _item_categories.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _item_categories.SetSkipDisabled(true);

    // Add an option for every category + 1 (All items)
    for (uint32_t i = 0; i < ITEM_CATEGORY_SIZE + 1; ++i)
        _item_categories.AddOption();

    _item_categories.SetSelection(ITEM_ALL);
    _item_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    // Adds category images
    GlobalMedia& media = GlobalManager->Media();
    _item_categories.AddOptionElementImage(0, media.GetSmallItemCategoryIcon(ITEM_ALL));
    _item_categories.AddOptionElementImage(1, media.GetSmallItemCategoryIcon(ITEM_ITEM));
    _item_categories.AddOptionElementImage(2, media.GetSmallItemCategoryIcon(ITEM_WEAPON));
    _item_categories.AddOptionElementImage(3, media.GetSmallItemCategoryIcon(ITEM_HEAD_ARMOR));
    _item_categories.AddOptionElementImage(4, media.GetSmallItemCategoryIcon(ITEM_TORSO_ARMOR));
    _item_categories.AddOptionElementImage(5, media.GetSmallItemCategoryIcon(ITEM_ARMS_ARMOR));
    _item_categories.AddOptionElementImage(6, media.GetSmallItemCategoryIcon(ITEM_LEGS_ARMOR));
    _item_categories.AddOptionElementImage(7, media.GetSmallItemCategoryIcon(ITEM_KEY));

    _UpdateCategory();
}

void InventoryWindow::_UpdateCategory()
{
    _item_categories.EnableOption(1, !(GlobalManager->GetInventoryItems()->empty()));
    _item_categories.EnableOption(2, !(GlobalManager->GetInventoryWeapons()->empty()));
    _item_categories.EnableOption(3, !(GlobalManager->GetInventoryHeadArmors()->empty()));
    _item_categories.EnableOption(4, !(GlobalManager->GetInventoryTorsoArmors()->empty()));
    _item_categories.EnableOption(5, !(GlobalManager->GetInventoryArmArmors()->empty()));
    _item_categories.EnableOption(6, !(GlobalManager->GetInventoryLegArmors()->empty()));
    _item_categories.EnableOption(7, !(GlobalManager->GetInventoryKeyItems()->empty()));
}

void InventoryWindow::Activate(bool new_status)
{
    // Set new status
    if(new_status) {
        _active_box = ITEM_ACTIVE_CATEGORY;
        MenuMode::CurrentInstance()->_help_information.SetDisplayText(inventory_help_message);
        // Update cursor state
        _item_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    } else {
        GlobalManager->Media().PlaySound("bump");
        _active_box = ITEM_ACTIVE_NONE;
        _item_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    }
}

void InventoryWindow::Update()
{
    GlobalMedia& media = GlobalManager->Media();

    if(GlobalManager->GetInventory()->empty()) {
        // no more items in inventory, exit inventory window
        Activate(false);
        return;
    }

    // Points to the active option box
    OptionBox *active_option = nullptr;

    _inventory_items.Update(); // For scrolling

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
        media.PlaySound("bump");
        active_option->InputLeft();
    } else if(InputManager->RightPress()) {
        media.PlaySound("bump");
        active_option->InputRight();
    } else if(InputManager->UpPress()) {
        media.PlaySound("bump");
        active_option->InputUp();
    } else if(InputManager->DownPress()) {
        media.PlaySound("bump");
        active_option->InputDown();
    }

    // Update object and character data when necessary
    if (InputManager->AnyRegisteredKeyPress())
        _UpdateSelection();

    uint32_t event = active_option->GetEvent();
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
                    _description.SetDisplayText(_item_objects[0]->GetDescription());
                    _active_box = ITEM_ACTIVE_LIST;
                    media.PlaySound("confirm");
                } // if _inventory_items.GetNumberOptions() > 0
            } // if VIDEO_OPTION_CONFIRM
            // Deactivate inventory
            else if(event == VIDEO_OPTION_CANCEL) {
                media.PlaySound("cancel");
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
                MenuMode::CurrentInstance()->_help_information.SetDisplayText(inventory_help_message);
                _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                _item_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                break;
            }
            // Test whether the inventory cursor is out of bound after using
            // the item. Can happen when using/equipping the last item of the list.
            else if(_inventory_items.GetSelection() >= (int32_t)_inventory_items.GetNumberOptions()) {
                _inventory_items.SetSelection((uint32_t)_inventory_items.GetNumberOptions() - 1);
            }

            // Activate the character select for application
            if(event == VIDEO_OPTION_CONFIRM) {
                // Don't accept selecting spirit items for now
                if(_object_type == GLOBAL_OBJECT_SPIRIT) {
                    media.PlaySound("cancel");
                    break;
                }

                // Check first whether the item is usable from the menu
                std::shared_ptr<GlobalItem> item =
                    std::dynamic_pointer_cast<GlobalItem>(GlobalManager->GetGlobalObject(_object->GetID()));
                if (item && !item->IsUsableInField()) {
                    media.PlaySound("cancel");
                    break;
                }

                _active_box = ITEM_ACTIVE_CHAR;
                _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_DARKEN);
                _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);

                switch (_object_type) {
                    case GLOBAL_OBJECT_WEAPON:
                    case GLOBAL_OBJECT_ARM_ARMOR:
                    case GLOBAL_OBJECT_HEAD_ARMOR:
                    case GLOBAL_OBJECT_TORSO_ARMOR:
                    case GLOBAL_OBJECT_LEG_ARMOR:
                        MenuMode::CurrentInstance()->UpdateEquipmentInfo(_character,
                                                                         _object,
                                                                         EQUIP_VIEW_EQUIPPING);
                        break;
                    default: // other objects
                        break;
                }

                media.PlaySound("confirm");
            } // if VIDEO_OPTION_CONFIRM
            // Return to category selection
            else if(event == VIDEO_OPTION_CANCEL) {
                _active_box = ITEM_ACTIVE_CATEGORY;
                MenuMode::CurrentInstance()->_help_information.SetDisplayText(inventory_help_message);
                _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                _item_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                media.PlaySound("cancel");
            } // else if VIDEO_OPTION_CANCEL
            else if(event == VIDEO_OPTION_BOUNDS_UP || event == VIDEO_OPTION_BOUNDS_DOWN) {
                _description.SetDisplayText(_object->GetDescription());
            } // else if VIDEO_OPTION_BOUNDS_UP
            break;
        } // case ITEM_ACTIVE_LIST

        case ITEM_ACTIVE_CHAR: {
            // Use the item on the chosen character
            if(event == VIDEO_OPTION_CONFIRM) {
                //values used for equipment selection
                std::shared_ptr<GlobalArmor> selected_armor = nullptr;
                std::shared_ptr<GlobalWeapon> selected_weapon = nullptr;
                switch(_object_type)
                {
                    case GLOBAL_OBJECT_ITEM:
                    {
                        // Returns an item object, already removed from inventory.
                        // Don't forget to readd the item if not used, or to delete the pointer.
                        std::shared_ptr<GlobalItem> item =
                            std::dynamic_pointer_cast<GlobalItem>(GlobalManager->GetGlobalObject(_object->GetID()));
                        if (!item)
                            break;

                        const luabind::object& script_function = item->GetFieldUseFunction();
                        if(!script_function.is_valid()) {
                            IF_PRINT_WARNING(MENU_DEBUG) << "item did not have a menu use function" << std::endl;
                        } else {
                            if(IsTargetParty(item->GetTargetType())) {
                                GlobalParty& party = GlobalManager->GetCharacterHandler().GetActiveParty();

                                bool success = false;
                                try {
                                    success = luabind::call_function<bool>(script_function, party);
                                } catch(const luabind::error& e) {
                                    PRINT_ERROR << "Error while loading FieldUse() function" << std::endl;
                                    vt_script::ScriptManager->HandleLuaError(e);
                                    success = false;
                                } catch(const luabind::cast_failed& e) {
                                    PRINT_ERROR << "Error while loading FieldUse() function" << std::endl;
                                    vt_script::ScriptManager->HandleCastError(e);
                                    success = false;
                                }

                                if(success)
                                {
                                    // Delete the item instance when succeeded.
                                    // Also, return back a level to the item selection list
                                    _active_box = ITEM_ACTIVE_LIST;
                                    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                                    _char_select.ResetViewableOption();
                                    // Show the selected item
                                    _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                                    // We also update the Characters stats as the item might have some effects there.
                                    MenuMode::CurrentInstance()->ReloadCharacterWindows();
                                    item = nullptr;
                                    // Now the item is used, we can remove it from the inventory.
                                    if(_object->GetCount() == 1 && _object == _menu_mode->_object){
                                        _menu_mode->_object = nullptr;
                                    }
                                    GlobalManager->DecrementItemCount(_object->GetID(), 1);
                                    media.PlaySound("confirm");
                                }
                            } // if GLOBAL_TARGET_PARTY
                            else { // Use on a single character only
                                bool success = false;
                                try {
                                    success = luabind::call_function<bool>(script_function, _character);
                                } catch(const luabind::error& e) {
                                    PRINT_ERROR << "Error while loading FieldUse() function" << std::endl;
                                    vt_script::ScriptManager->HandleLuaError(e);
                                    success = false;
                                } catch(const luabind::cast_failed& e) {
                                    PRINT_ERROR << "Error while loading FieldUse() function" << std::endl;
                                    vt_script::ScriptManager->HandleCastError(e);
                                    success = false;
                                }

                                if(success)
                                {
                                    // delete the item instance when succeeded.
                                    // Also, return back a level to the item selection list
                                    _active_box = ITEM_ACTIVE_LIST;
                                    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                                    _char_select.ResetViewableOption();
                                    // Show the selected item
                                    _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                                    // We also update the Characters stats as the item might have some effects there.
                                    MenuMode::CurrentInstance()->ReloadCharacterWindows();
                                    item = nullptr;
                                    // Now the item is used, we can remove it from the inventory.
                                    if(_object->GetCount() == 1 &&
                                          _object == _menu_mode->_object){
                                        _menu_mode->_object = nullptr;
                                    }
                                    GlobalManager->DecrementItemCount(_object->GetID(), 1);
                                    media.PlaySound("confirm");
                                }
                            }
                        }
                        break;
                    } // if GLOBAL_OBJECT_ITEM
                    case GLOBAL_OBJECT_WEAPON:
                    {
                        //get the item from the inventory list. this also removes the item from the list
                        selected_weapon =
                            std::dynamic_pointer_cast<GlobalWeapon>(GlobalManager->GetGlobalObject(_object->GetID()));
                        break;
                    }
                    case GLOBAL_OBJECT_HEAD_ARMOR:
                    case GLOBAL_OBJECT_TORSO_ARMOR:
                    case GLOBAL_OBJECT_ARM_ARMOR:
                    case GLOBAL_OBJECT_LEG_ARMOR:
                    {
                        //get the item from the inventory list. this also removes the item from the list
                        selected_armor =
                            std::dynamic_pointer_cast<GlobalArmor>(GlobalManager->GetGlobalObject(_object->GetID()));
                        break;
                    }

                    default:
                        break;
                }

                //if we can equip this and it is armor
                if(_can_equip && selected_armor)
                {
                    //do swap of armor based on object type (aka armor type)
                    switch(_object_type)
                    {
                        case GLOBAL_OBJECT_HEAD_ARMOR:
                            selected_armor =
                                _character->EquipHeadArmor(selected_armor);
                            break;
                        case GLOBAL_OBJECT_TORSO_ARMOR:
                            selected_armor =
                                _character->EquipTorsoArmor(selected_armor);
                            break;
                        case GLOBAL_OBJECT_ARM_ARMOR:
                            selected_armor =
                                _character->EquipArmArmor(selected_armor);
                            break;
                        case GLOBAL_OBJECT_LEG_ARMOR:
                            selected_armor =
                                _character->EquipLegArmor(selected_armor);
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
                    // Remove the selected one from inventory
                    if(_object->GetCount() == 1 && _object == _menu_mode->_object){
                        _menu_mode->_object = nullptr;
                    }
                    GlobalManager->DecrementItemCount(_object->GetID(), 1);
                    media.PlaySound("confirm");
                }
                //if we can equip and it is a weapon
                else if(_can_equip && selected_weapon)
                {
                    // get the old weapon by swapping
                    // the selected_weapon for the current one
                    selected_weapon = _character->EquipWeapon(selected_weapon);
                    //add the old weapon back into the inventory
                    GlobalManager->AddToInventory(selected_weapon);
                    //return back a level
                    _active_box = ITEM_ACTIVE_LIST;
                    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                    _char_select.ResetViewableOption();
                    //set the item select to by lightened
                    _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                    // Remove the selected one from inventory
                    if(_object->GetCount() == 1 && _object == _menu_mode->_object){
                        _menu_mode->_object = nullptr;
                    }
                    GlobalManager->DecrementItemCount(_object->GetID(), 1);
                    media.PlaySound("confirm");
                }
                //if we cannot equip
                else
                {
                    media.PlaySound("cancel");
                }

                _UpdateSelection();

            } // if VIDEO_OPTION_CONFIRM
            // Return to item selection
            else if(event == VIDEO_OPTION_CANCEL) {
                _active_box = ITEM_ACTIVE_LIST;
                _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                media.PlaySound("cancel");
            } // if VIDEO_OPTION_CANCEL
            break;
        } // case ITEM_ACTIVE_CHAR
    } // switch (_active_box)
}

void InventoryWindow::_UpdateSelection()
{
    // Update the item list
    _UpdateItemText();

    // Lower bound checks
    // Make the menu back-off when no more items are in the category list.
    if (_item_objects.empty()) {
        _object = nullptr;
        _UpdateCategory();
        _active_box = ITEM_ACTIVE_CATEGORY;
        _item_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
        _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        return;
    }

    // Make sure the selection is sane.
    if (_inventory_items.GetSelection() < 0)
        _inventory_items.SetSelection(0);

    // Upper bound check to avoid crashes when selecting the last item at list's end.
    if (static_cast<uint32_t>(_inventory_items.GetSelection())
           >= _item_objects.size())
        _inventory_items.SetSelection(_item_objects.size() - 1);

    _object = _item_objects[ _inventory_items.GetSelection() ];
    _object_type = _object->GetObjectType();
    _object_name.SetText(_object->GetName(), TextStyle("title22"));

    if (_active_box == ITEM_ACTIVE_CHAR)
        _character = GlobalManager->GetCharacterHandler().GetActiveParty().GetCharacterAtIndex(_char_select.GetSelection());

    //check the obj_type again to see if its a weapon or armor
    switch(_object_type) {
        case GLOBAL_OBJECT_WEAPON:
        {
            std::shared_ptr<GlobalWeapon> selected_weapon =
                std::dynamic_pointer_cast<GlobalWeapon>(_object);
            uint32_t usability_bitmask = selected_weapon->GetUsableBy();
            _is_equipment = true;
            _can_equip = usability_bitmask & _character->GetID();
            MenuMode::CurrentInstance()->UpdateEquipmentInfo(_character, _object,
                                                             EQUIP_VIEW_EQUIPPING);
            break;
        }
        case GLOBAL_OBJECT_HEAD_ARMOR:
        case GLOBAL_OBJECT_TORSO_ARMOR:
        case GLOBAL_OBJECT_ARM_ARMOR:
        case GLOBAL_OBJECT_LEG_ARMOR:
        {
            std::shared_ptr<GlobalArmor> selected_armor =
                std::dynamic_pointer_cast<GlobalArmor>(_object);
            uint32_t usability_bitmask = selected_armor->GetUsableBy();
            _is_equipment = true;
            _can_equip = usability_bitmask & _character->GetID();
            MenuMode::CurrentInstance()->UpdateEquipmentInfo(_character, _object,
                                                             EQUIP_VIEW_EQUIPPING);
            break;
        }
        default:
            _is_equipment = false;
            _can_equip = false;
            break;
    }

    _UpdateCategory();

    if (_is_equipment && !_can_equip)
        MenuMode::CurrentInstance()->_help_information.SetDisplayText(cannot_equip);
    else if (_active_box == ITEM_ACTIVE_CATEGORY)
        MenuMode::CurrentInstance()->_help_information.SetDisplayText(inventory_help_message);
    else
        // standard items
        MenuMode::CurrentInstance()->_help_information.SetDisplayText(item_use);
}

void InventoryWindow::_UpdateItemText()
{
    _item_objects.clear();
    _inventory_items.ClearOptions();

    ITEM_CATEGORY current_selected_category =
        static_cast<ITEM_CATEGORY>(_item_categories.GetSelection());
    switch(current_selected_category) {
        case ITEM_ALL: {
            auto inv = GlobalManager->GetInventory();
            for (auto it = inv->begin(); it != inv->end(); ++it) {
                _item_objects.push_back(it->second);
            }
            break;
        }
        case ITEM_ITEM:
            _item_objects =
                _GetObjectVector(GlobalManager->GetInventoryItems());
            break;

        case ITEM_WEAPON:
            _item_objects =
                _GetObjectVector(GlobalManager->GetInventoryWeapons());
            break;

        case ITEM_HEAD_ARMOR:
            _item_objects =
                _GetObjectVector(GlobalManager->GetInventoryHeadArmors());
            break;

        case ITEM_TORSO_ARMOR:
            _item_objects =
                _GetObjectVector(GlobalManager->GetInventoryTorsoArmors());
            break;

        case ITEM_ARMS_ARMOR:
            _item_objects =
                _GetObjectVector(GlobalManager->GetInventoryArmArmors());
            break;

        case ITEM_LEGS_ARMOR:
            _item_objects =
                _GetObjectVector(GlobalManager->GetInventoryLegArmors());
            break;

        case ITEM_KEY: {
            _item_objects =
                _GetObjectVector(GlobalManager->GetInventoryKeyItems());
            break;
        }
        default:
            break;
        }

    // Before we update the current inventory_items option box,
    // if the actual available items WAS zero on the last frame, then we make sure
    // that the cursor is reset to hidden.
    // If you don't do this, then the previous ITEM_CATEGORY cursor state remains, and will
    // cause the darkened pointer to show.
    if(_item_objects.empty())
        _inventory_items.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    ustring text;
    std::vector<ustring> inv_names;

    for(size_t ctr = 0; ctr < _item_objects.size(); ++ctr) {
        text = MakeUnicodeString("<" + _item_objects[ctr]->GetIconImage().GetFilename() + "><20>     ") +
               _item_objects[ctr]->GetName() + MakeUnicodeString("<R><350>" + NumberToString(_item_objects[ctr]->GetCount()) + "   ");
        inv_names.push_back(text);
    }

    _inventory_items.SetOptions(inv_names);
    for (uint32_t i = 0; i < _inventory_items.GetNumberOptions(); ++i) {
        StillImage *image = _inventory_items.GetEmbeddedImage(i);
        if (image)
            image->SetWidthKeepRatio(32);
    }

    // Upper bound check to avoid a crash at when selecting the last item of the list's end.
    if (static_cast<uint32_t>(_inventory_items.GetSelection())
            >= _item_objects.size())
        _inventory_items.SetSelection(_item_objects.size() - 1);

    if(current_selected_category != _previous_category)
    {
        // Swap to the new category
        _previous_category = current_selected_category;
        // Reset the top viewing inventory item
        _inventory_items.ResetViewableOption();
    }
}

void InventoryWindow::Draw()
{
    _DrawBottomInfo();

    MenuWindow::Draw();

    // Draw char select option box
    _char_select.Draw();

    // Draw item categories option box
    _item_categories.Draw();

    // Draw item list
    _inventory_items.Draw();
}

void InventoryWindow::_DrawSpecialItemDescription(vt_video::StillImage* special_image,
                                                  vt_gui::TextBox& description)
{
    int32_t key_pos_x = 100 + _object->GetIconImage().GetWidth() - special_image->GetWidth() - 3;
    int32_t key_pos_y = 600 + _object->GetIconImage().GetHeight() - special_image->GetHeight() - 3;
    VideoManager->Move(key_pos_x, key_pos_y);
    special_image->Draw();
    VideoManager->Move(185, 600);
    description.Draw();
}

void InventoryWindow::_DrawBottomInfo()
{
    //if we are out of items, the bottom view should do no work
    if(GlobalManager->GetInventory()->empty() || _item_objects.empty())
        return;

    MenuMode* menu = MenuMode::CurrentInstance();

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
    VideoManager->Move(150, 580);

    if(_active_box == ITEM_ACTIVE_CATEGORY) {
        menu->_bottom_window.Draw();
        menu->_help_information.Draw();
    }
    else if(_active_box == ITEM_ACTIVE_LIST) {
        menu->_bottom_window.Draw();

        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_CENTER, 0);
        VideoManager->Move(100, 600);
        _object->GetIconImage().Draw();
        VideoManager->MoveRelative(65, -15);
        _object_name.Draw();
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);
        _description.Draw();

        if (_object->IsKeyItem())
            _DrawSpecialItemDescription(menu->_key_item_icon,
                                        menu->_key_item_description);
        else if (_object_type == GLOBAL_OBJECT_SPIRIT)
            _DrawSpecialItemDescription(menu->_spirit_icon,
                                        menu->_spirit_description);
    }
    else if(_active_box == ITEM_ACTIVE_CHAR) {
        menu->_bottom_window.Draw();

        if (_is_equipment && _can_equip)
            menu->DrawEquipmentInfo();
        else
            menu->_help_information.Draw();
    }
}

} // namespace private_menu

} // namespace vt_menu
