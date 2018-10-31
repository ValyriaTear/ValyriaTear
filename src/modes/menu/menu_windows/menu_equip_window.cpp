///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_windows/menu_equip_window.h"

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

vt_global::GLOBAL_OBJECT GetObjectTypeFromEquipCategory(EQUIP_CATEGORY category)
{
    switch (category) {
    case EQUIP_WEAPON:
        return GLOBAL_OBJECT_WEAPON;
        break;
    case EQUIP_HEAD:
        return GLOBAL_OBJECT_HEAD_ARMOR;
        break;
    case EQUIP_TORSO:
        return GLOBAL_OBJECT_TORSO_ARMOR;
        break;
    case EQUIP_ARMS:
        return GLOBAL_OBJECT_ARM_ARMOR;
        break;
    case EQUIP_LEGS:
        return GLOBAL_OBJECT_LEG_ARMOR;
        break;
    default:
        break;
    }
    return GLOBAL_OBJECT_INVALID;
}

EquipWindow::EquipWindow() :
    _active_box(EQUIP_ACTIVE_NONE),
    _character(nullptr)
{
    // Init the labels
    _weapon_label.SetStyle(TextStyle("text20"));
    _weapon_label.SetText(UTranslate("Weapon"));
    _head_label.SetStyle(TextStyle("text20"));
    _head_label.SetText(UTranslate("Head"));
    _torso_label.SetStyle(TextStyle("text20"));
    _torso_label.SetText(UTranslate("Torso"));
    _arms_label.SetStyle(TextStyle("text20"));
    _arms_label.SetText(UTranslate("Arms"));
    _legs_label.SetStyle(TextStyle("text20"));
    _legs_label.SetText(UTranslate("Legs"));

    // Initialize option boxes
    _InitCharSelect();
    _InitEquipmentSelect();
    _InitEquipmentList();
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
    _equip_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
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
    uint32_t size = GlobalManager->GetCharacterHandler().GetActiveParty().GetPartySize();

    _char_select.SetPosition(72.0f, 109.0f);
    _char_select.SetDimensions(360.0f, 432.0f, 1, 4, 1, 4);
    _char_select.SetCursorOffset(-50.0f, -6.0f);
    _char_select.SetTextStyle(TextStyle("text20"));
    _char_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _char_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _char_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

    //Use blank strings....won't be seen anyway
    for(uint32_t i = 0; i < size; i++) {
        options.push_back(MakeUnicodeString(" "));
    }

    //Set options, selection and cursor state
    _char_select.SetOptions(options);
    _char_select.SetSelection(0);
    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _character = GlobalManager->GetCharacterHandler().GetActiveParty().GetCharacterAtIndex(_char_select.GetSelection());
}

void EquipWindow::_InitEquipmentSelect()
{
    //Set params
    _equip_select.SetPosition(680.0f, 145.0f);
    _equip_select.SetDimensions(180.0f, 350.0f,
                                1, EQUIP_CATEGORY_SIZE, 1, EQUIP_CATEGORY_SIZE);
    _equip_select.SetTextStyle(TextStyle("text20"));

    _equip_select.SetCursorOffset(-132.0f, -20.0f);
    _equip_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _equip_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _equip_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

    //Set options and default selection

    _equip_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _UpdateEquipList();
    _equip_select.SetSelection(EQUIP_WEAPON);
}

void EquipWindow::Update()
{
    // Points to the active option box
    OptionBox* active_option = nullptr;

    GlobalMedia& media = GlobalManager->Media();

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

    // update the concerned character on each change
    if (InputManager->AnyRegisteredKeyPress())
        _character = GlobalManager->GetCharacterHandler().GetActiveParty().GetCharacterAtIndex(_char_select.GetSelection());

    uint32_t event = active_option->GetEvent();
    active_option->Update();
    switch(_active_box) {
    //Choose character
    case EQUIP_ACTIVE_CHAR:
        if(event == VIDEO_OPTION_CONFIRM) {
            _active_box = EQUIP_ACTIVE_SELECT;
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_DARKEN);
            _equip_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            media.PlaySound("confirm");
        } else if(event == VIDEO_OPTION_CANCEL) {
            Activate(false, true);
            media.PlaySound("cancel");
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
                    media.PlaySound("confirm");
                } else {
                    _active_box = EQUIP_ACTIVE_SELECT;
                    media.PlaySound("cancel");
                }
            } // Unequip mode
            else {
                _Unequip();
                media.PlaySound("confirm");
            } // Equip/Unequip
        } // Confirm
        else if(event == VIDEO_OPTION_CANCEL) {
            _active_box = EQUIP_ACTIVE_CHAR;
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _equip_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            media.PlaySound("cancel");
        }
        break;

    // Choose replacement when equipping
    case EQUIP_ACTIVE_LIST:
        if(event == VIDEO_OPTION_CONFIRM) {
            _Equip();
            _active_box = EQUIP_ACTIVE_SELECT;
            _equip_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            _equip_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            media.PlaySound("confirm");
        } // if VIDEO_OPTION_CONFIRM
        else if(event == VIDEO_OPTION_CANCEL) {
            _active_box = EQUIP_ACTIVE_SELECT;
            media.PlaySound("cancel");
            _equip_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            _equip_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
        } // else if VIDEO_OPTION_CANCEL
        break;
    } // switch _active_box

    _UpdateEquipList();
    _UpdateSelectedObject();
}

void EquipWindow::_Equip()
{
    GlobalMedia& media = GlobalManager->Media();
    InventoryHandler& inventory_handler = GlobalManager->GetInventoryHandler();

    // Get the actual inventory index.
    uint32_t inventory_id = _equip_list_inv_index[_equip_list.GetSelection()];

    switch(_equip_select.GetSelection()) {
    case EQUIP_WEAPON: {
        std::shared_ptr<GlobalWeapon> wpn = inventory_handler.GetInventoryWeapons().at(inventory_id);
        if(wpn->GetUsableBy() & _character->GetID()) {
            uint32_t weapon_id = wpn->GetID();
            std::shared_ptr<GlobalWeapon> gbl_wpn = std::dynamic_pointer_cast<GlobalWeapon>(inventory_handler.GetGlobalObject(weapon_id));
            inventory_handler.AddToInventory(_character->EquipWeapon(gbl_wpn));
            inventory_handler.DecrementItemCount(weapon_id, 1);
        } else {
            media.PlaySound("cancel");
        }
        break;
    }

    case EQUIP_HEAD:
    case EQUIP_TORSO:
    case EQUIP_ARMS:
    case EQUIP_LEGS: {
        EQUIP_CATEGORY category = static_cast<EQUIP_CATEGORY>(_equip_select.GetSelection());
        GLOBAL_OBJECT object_type = GetObjectTypeFromEquipCategory(category);
        std::shared_ptr<GlobalArmor> armor = inventory_handler.GetInventoryArmors(object_type).at(inventory_id);
        if(armor->GetUsableBy() & _character->GetID()) {
            uint32_t armor_id = armor->GetID();
            std::shared_ptr<GlobalArmor> gbl_arm = std::dynamic_pointer_cast<GlobalArmor>(inventory_handler.GetGlobalObject(armor_id));
            inventory_handler.AddToInventory(_character->EquipArmor(gbl_arm));
            inventory_handler.DecrementItemCount(armor_id, 1);
        } else {
            media.PlaySound("cancel");
        }
        break;
    }

    default:
        PRINT_WARNING << "Equip slot value is invalid: " << inventory_id << std::endl;
        break;
    }
}

void EquipWindow::_Unequip()
{
    InventoryHandler& inventory_handler = GlobalManager->GetInventoryHandler();

    EQUIP_CATEGORY category = static_cast<EQUIP_CATEGORY>(_equip_select.GetSelection());

    switch(category) {
        // Unequip and return the old weapon to inventory
    case EQUIP_WEAPON:
        inventory_handler.AddToInventory(_character->EquipWeapon(nullptr));
        break;
    case EQUIP_HEAD:
    case EQUIP_TORSO:
    case EQUIP_ARMS:
    case EQUIP_LEGS: {
        GLOBAL_OBJECT object_type = GetObjectTypeFromEquipCategory(category);
        inventory_handler.AddToInventory(_character->UnequipArmor(object_type));
        break;
    }
    default:
        PRINT_WARNING << "Unequip slot is invalid: " << category << std::endl;
        break;
    }
}

void EquipWindow::_UpdateEquipList()
{
    InventoryHandler& inventory_handler = GlobalManager->GetInventoryHandler();

    std::vector<ustring> options;

    if(_active_box == EQUIP_ACTIVE_LIST) {
        std::vector<std::shared_ptr<GlobalObject>> equipment_list;

        EQUIP_CATEGORY category = static_cast<EQUIP_CATEGORY>(_equip_select.GetSelection());
        GLOBAL_OBJECT object_type = GetObjectTypeFromEquipCategory(category);

        switch(_equip_select.GetSelection()) {
            default:
                break;
            case EQUIP_WEAPON: {
                auto inv = inventory_handler.GetInventoryWeapons();
                equipment_list = std::vector<std::shared_ptr<GlobalObject>>(inv.begin(), inv.end());
                break;
            }
            case EQUIP_HEAD:
            case EQUIP_TORSO:
            case EQUIP_ARMS:
            case EQUIP_LEGS: {
                auto inv = inventory_handler.GetInventoryArmors(object_type);
                equipment_list = std::vector<std::shared_ptr<GlobalObject>>(inv.begin(), inv.end());
                break;
            }
        }

        // Clear the replacer ids
        _equip_list_inv_index.clear();

        // Add the options
        uint32_t gear_size = equipment_list.size();
        for(uint32_t j = 0; j < gear_size; j++) {
            uint32_t usability_bitmask = 0;
            if(_equip_select.GetSelection() == EQUIP_WEAPON) {
                std::shared_ptr<GlobalWeapon> selected_weapon =
                    std::dynamic_pointer_cast<GlobalWeapon>(equipment_list.at(j));
                usability_bitmask = selected_weapon->GetUsableBy();
            } else {
                std::shared_ptr<GlobalArmor> selected_armor =
                    std::dynamic_pointer_cast<GlobalArmor>(equipment_list.at(j));
                usability_bitmask = selected_armor->GetUsableBy();
            }

            // If the character can't equip the item, don't show it.
            if(_equip && !(usability_bitmask & _character->GetID()))
                continue;

            options.push_back(MakeUnicodeString("<") +
                              MakeUnicodeString(equipment_list.at(j)->GetIconImage().GetFilename()) +
                              MakeUnicodeString("><70>") +
                              equipment_list.at(j)->GetName());

            // Add the actual inventory index
            _equip_list_inv_index.push_back(j);
        }

        _equip_list.SetOptions(options);
    } // if EQUIP_ACTIVE_LIST

    else {
        // First, update the IMAGES of the equipped items
        _equip_images.clear();
        StillImage i;

        std::shared_ptr<GlobalWeapon> wpn = _character->GetEquippedWeapon();
        i.Load(wpn ? wpn->GetIconImage().GetFilename()
                   : "data/inventory/weapons/fist-human.png");
        _equip_images.push_back(i);

        std::shared_ptr<GlobalArmor> head_armor = _character->GetEquippedArmor(GLOBAL_OBJECT_HEAD_ARMOR);
        i.Load(head_armor ? head_armor->GetIconImage().GetFilename() : "");
        _equip_images.push_back(i);

        std::shared_ptr<GlobalArmor> torso_armor = _character->GetEquippedArmor(GLOBAL_OBJECT_TORSO_ARMOR);
        i.Load(torso_armor ? torso_armor->GetIconImage().GetFilename() : "");
        _equip_images.push_back(i);

        std::shared_ptr<GlobalArmor> arm_armor = _character->GetEquippedArmor(GLOBAL_OBJECT_ARM_ARMOR);
        i.Load(arm_armor ? arm_armor->GetIconImage().GetFilename() : "");
        _equip_images.push_back(i);

        std::shared_ptr<GlobalArmor> leg_armor = _character->GetEquippedArmor(GLOBAL_OBJECT_LEG_ARMOR);
        i.Load(leg_armor ? leg_armor->GetIconImage().GetFilename() : "");
        _equip_images.push_back(i);

        // Now, update the NAMES of the equipped items
        options.push_back(wpn ? wpn->GetName() : UTranslate("No weapon"));
        options.push_back(head_armor ? head_armor->GetName()
                                     : UTranslate("No head armor"));
        options.push_back(torso_armor ? torso_armor->GetName()
                                      : UTranslate("No torso armor"));
        options.push_back(arm_armor ? arm_armor->GetName()
                                    : UTranslate("No arms armor"));
        options.push_back(leg_armor ? leg_armor->GetName()
                                    : UTranslate("No legs armor"));

        _equip_select.SetOptions(options);
    }
}

void EquipWindow::_UpdateSelectedObject()
{
    // Only updates when some input is handled.
    if (!InputManager->AnyRegisteredKeyPress())
        return;

    // Don't show anything when there is no item selected
    if (_active_box == EQUIP_ACTIVE_CHAR || _active_box == EQUIP_ACTIVE_NONE) {
        _object = nullptr;
        MenuMode::CurrentInstance()->UpdateEquipmentInfo(_character, _object,
                                                         EQUIP_VIEW_NONE);
        return;
    }

    // We're listing the character's equipment.
    // Let's use the character object.
    if (_active_box == EQUIP_ACTIVE_SELECT) {

        EQUIP_CATEGORY category = static_cast<EQUIP_CATEGORY>(_equip_select.GetSelection());
        GLOBAL_OBJECT object_type = GetObjectTypeFromEquipCategory(category);

        switch (_equip_select.GetSelection()) {
            default:
                // Should never happen
                _object = nullptr;
                break;
            case EQUIP_WEAPON:
                _object = _character->GetEquippedWeapon();
                break;
            case EQUIP_HEAD:
            case EQUIP_TORSO:
            case EQUIP_ARMS:
            case EQUIP_LEGS:
                _object = _character->GetEquippedArmor(object_type);
                break;
        }
        EQUIP_VIEW view_type = _equip ? EQUIP_VIEW_CHAR : EQUIP_VIEW_UNEQUIPPING;
        MenuMode::CurrentInstance()->UpdateEquipmentInfo(_character, _object,
                                                         view_type);
        return;
    }

    // EQUIP_ACTIVE_LIST, we're choosing a new piece of equipment.

    // Get the actual inventory index.
    uint32_t inventory_id = _equip_list_inv_index[_equip_list.GetSelection()];
    InventoryHandler& inventory_handler = GlobalManager->GetInventoryHandler();

    EQUIP_CATEGORY category = static_cast<EQUIP_CATEGORY>(_equip_select.GetSelection());
    GLOBAL_OBJECT object_type = GetObjectTypeFromEquipCategory(category);

    switch(_equip_select.GetSelection()) {
        default:
            // Should never happen
            _object = nullptr;
            return;
            break;

        case EQUIP_WEAPON:
            _object = inventory_handler.GetInventoryWeapons().at(inventory_id);
            break;

        case EQUIP_HEAD:
        case EQUIP_TORSO:
        case EQUIP_ARMS:
        case EQUIP_LEGS:
            _object = inventory_handler.GetInventoryArmors(object_type).at(inventory_id);
            break;
    }

    // We now update equipment info
    MenuMode::CurrentInstance()->UpdateEquipmentInfo(_character, _object,
                                                     EQUIP_VIEW_EQUIPPING);
}

void EquipWindow::Draw()
{
    MenuMode::CurrentInstance()->_bottom_window.Draw();

    MenuWindow::Draw();

    //Draw option boxes
    _char_select.Draw();

    if(_active_box == EQUIP_ACTIVE_LIST) {
        _equip_list.Draw();
        VideoManager->Move(660.0f, 135.0f);
        VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
        switch(_equip_select.GetSelection()) {
        case EQUIP_WEAPON:
            _weapon_label.Draw();
            break;
        case EQUIP_HEAD:
            _head_label.Draw();
            break;
        case EQUIP_TORSO:
            _torso_label.Draw();
            break;
        case EQUIP_ARMS:
            _arms_label.Draw();
            break;
        case EQUIP_LEGS:
            _legs_label.Draw();
            break;
        }
    }
    else {
        // Show the character equipment.
        _equip_select.Draw();

        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);
        VideoManager->Move(450.0f, 170.0f);
        _weapon_label.Draw();
        VideoManager->MoveRelative(0.0f, 70.0f);
        _head_label.Draw();
        VideoManager->MoveRelative(0.0f, 70.0f);
        _torso_label.Draw();
        VideoManager->MoveRelative(0.0f, 70.0f);
        _arms_label.Draw();
        VideoManager->MoveRelative(0.0f, 70.0f);
        _legs_label.Draw();

        VideoManager->MoveRelative(150.0f, -370.0f);

        for(uint32_t i = 0; i < _equip_images.size(); ++i) {
            VideoManager->MoveRelative(0.0f, 70.0f);
            _equip_images[i].Draw();
        }
    }

    // Draw the equipment info in the bottom window
    MenuMode::CurrentInstance()->DrawEquipmentInfo();
}

} // namespace private_menu

} // namespace vt_menu
