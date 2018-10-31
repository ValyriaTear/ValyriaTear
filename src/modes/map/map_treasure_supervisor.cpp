///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_treasure.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for map mode treasures.
*** ***************************************************************************/

#include "modes/map/map_treasure_supervisor.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_treasure_content.h"
#include "modes/map/map_object_supervisor.h"
#include "modes/map/map_objects/map_treasure.h"

#include "modes/menu/menu_mode.h"

#include "engine/input.h"
#include "engine/video/video.h"

#include "common/global/global.h"

using namespace vt_utils;
using namespace vt_input;
using namespace vt_mode_manager;
using namespace vt_system;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_global;
using namespace vt_menu;

namespace vt_map
{

namespace private_map
{

TreasureSupervisor::TreasureSupervisor() :
    _treasure(nullptr),
    _selection(ACTION_SELECTED),
    _window_title(UTranslate("You obtain"), TextStyle("title24", Color::white, VIDEO_TEXT_SHADOW_DARK, 1, -2)),
    _selection_name(),
    _selection_icon(nullptr),
    _is_key_item(false)
{
    // Create the menu windows and option boxes used for the treasure menu and
    // align them at the appropriate locations on the screen
    _action_window.Create(768.0f, 64.0f, ~VIDEO_MENU_EDGE_BOTTOM);
    _action_window.SetPosition(512.0f, 460.0f);
    _action_window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);

    _list_window.Create(768.0f, 236.0f);
    _list_window.SetPosition(512.0f, 516.0f);
    _list_window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);

    _action_options.SetPosition(30.0f, 18.0f);
    _action_options.SetDimensions(726.0f, 32.0f, 1, 1, 1, 1);
    _action_options.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _action_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _action_options.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _action_options.SetSelectMode(VIDEO_SELECT_SINGLE);
    _action_options.SetCursorOffset(-50.0f, -25.0f);
    _action_options.SetTextStyle(TextStyle("title22", Color::white, VIDEO_TEXT_SHADOW_DARK, 1, -2));
    _action_options.AddOption(UTranslate("Ok"));
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

    _detail_textbox.SetPosition(20.0f, 90.0f);
    _detail_textbox.SetDimensions(726.0f, 128.0f);
    _detail_textbox.SetDisplaySpeed(SystemManager->GetMessageSpeed());
    _detail_textbox.SetTextStyle(TextStyle("text22", Color::white, VIDEO_TEXT_SHADOW_DARK, 1, -2));
    _detail_textbox.SetDisplayMode(VIDEO_TEXT_REVEAL);
    _detail_textbox.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _detail_textbox.SetOwner(&_list_window);

    _selection_name.SetStyle(TextStyle("text22", Color::white, VIDEO_TEXT_SHADOW_DARK, 1, -2));
}

TreasureSupervisor::~TreasureSupervisor()
{
    _action_window.Destroy();
    _list_window.Destroy();
}

void TreasureSupervisor::Initialize(TreasureObject* map_object)
{
    if(map_object == nullptr) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function argument was nullptr" << std::endl;
        return;
    }

    Initialize(map_object->GetTreasure());
}

void TreasureSupervisor::Initialize(MapTreasureContent* treasure)
{
    if(!treasure) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function argument was nullptr" << std::endl;
        return;
    }
    _treasure = treasure;
    MapMode::CurrentInstance()->PushState(STATE_TREASURE);

    // Construct the object list, including any drunes that were contained within the treasure
    if(_treasure->_drunes != 0) {
        _list_options.AddOption(MakeUnicodeString("<data/inventory/drunes.png>       ") +
                                UTranslate("Drunes") +
                                MakeUnicodeString("<R>" + NumberToString(_treasure->_drunes)));
        GlobalManager->Media().PlaySound("coins");
    } else {
        GlobalManager->Media().PlaySound("item_pickup");
    }

    for(uint32_t i = 0; i < _treasure->_items_list.size(); i++) {
        if(_treasure->_items_list[i]->GetCount() > 1) {
            _list_options.AddOption(MakeUnicodeString("<" + _treasure->_items_list[i]->GetIconImage().GetFilename() + ">       ") +
                                    _treasure->_items_list[i]->GetName() +
                                    MakeUnicodeString("<R>x" + NumberToString(_treasure->_items_list[i]->GetCount())));
        } else {
            _list_options.AddOption(MakeUnicodeString("<" + _treasure->_items_list[i]->GetIconImage().GetFilename() + ">       ") +
                                    _treasure->_items_list[i]->GetName());
        }
    }

    for(uint32_t i = 0; i < _list_options.GetNumberOptions(); i++) {
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
    InventoryHandler& inventory_handler = GlobalManager->GetInventoryHandler();

    for (uint32_t i = 0; i < _treasure->_items_list.size(); ++i) {
        std::shared_ptr<GlobalObject> obj = _treasure->_items_list[i];
        if (!obj)
            continue;

        if (!inventory_handler.IsItemInInventory(obj->GetID())) {
            std::shared_ptr<vt_global::GlobalObject> obj_copy = GlobalCreateNewObject(obj->GetID(), obj->GetCount());
            inventory_handler.AddToInventory(obj_copy);
        } else {
            inventory_handler.IncrementItemCount(obj->GetID(), obj->GetCount());
        }
    }
}

void TreasureSupervisor::Update()
{
    _action_window.Update();
    _list_window.Update();
    _action_options.Update();
    _list_options.Update();
    _detail_textbox.Update();

    // Allow the user to go to menu mode at any time when the treasure menu is open
    if(InputManager->MenuPress()) {
        MenuMode *MM = new MenuMode();
        ModeManager->Push(MM);
        return;
    }

    // Update the menu according to which sub-window is currently selected
    if(_selection == ACTION_SELECTED)
        _UpdateAction();
    else if(_selection == LIST_SELECTED)
        _UpdateList();
    else if(_selection == DETAIL_SELECTED)
        _UpdateDetail();
    else
        IF_PRINT_WARNING(MAP_DEBUG) << "unknown selection state: " << _selection << std::endl;
}

void TreasureSupervisor::Draw()
{
    // We wait until the treasure is fully open before displaying any portions of the menu
    VideoManager->PushState();
    VideoManager->SetStandardCoordSys();
    _action_window.Draw();
    if(_selection != DETAIL_SELECTED) {
        _action_options.Draw();
    }
    _list_window.Draw();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_CENTER, 0);
    VideoManager->Move(152.0f, 495.0f);
    _window_title.Draw();

    if(_selection == DETAIL_SELECTED) {
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);
        // Move to the upper left corner and draw the object icon
        if(_selection_icon) {
            VideoManager->Move(150.0f, 535.0f);
            _selection_icon->Draw();
            if (_is_key_item) {
                StillImage* key_icon = GlobalManager->Media().GetKeyItemIcon();
                VideoManager->MoveRelative(_selection_icon->GetWidth() - key_icon->GetWidth() - 3.0f,
                                           _selection_icon->GetHeight() - key_icon->GetHeight() - 3.0f);
                key_icon->Draw();
                VideoManager->MoveRelative(-_selection_icon->GetWidth() + key_icon->GetWidth() + 3.0f,
                                           -_selection_icon->GetHeight() + key_icon->GetHeight() + 3.0f);
            }
        }

        // Draw the name of the selected object to the right of the icon
        VideoManager->MoveRelative(80.0f, 20.0f);
        _selection_name.Draw();

        _detail_textbox.Draw();
    } else {
        _list_options.Draw();
    }

    VideoManager->PopState();
} // void TreasureSupervisor::Draw()

void TreasureSupervisor::Finish()
{
    _treasure->SetTaken(true);
    _treasure = 0; // Forget about it once finished

    _action_window.Hide();
    _list_window.Hide();
    _list_options.ClearOptions();

    if(MapMode::CurrentInstance()->CurrentState() == vt_map::private_map::STATE_TREASURE)
        MapMode::CurrentInstance()->PopState();
}

void TreasureSupervisor::_UpdateAction()
{
    if(InputManager->ConfirmPress()) {
        if(_action_options.GetSelection() == 0)  // "Finish" lone action
            Finish();
        else
            IF_PRINT_WARNING(MAP_DEBUG) << "unhandled action selection in OptionBox: " << _action_options.GetSelection() << std::endl;
    }

    // when there is no item, no need to update anything
    if(_list_options.GetNumberOptions() == 0)
        return;

    if(InputManager->UpPress()) {
        _selection = LIST_SELECTED;
        _list_options.SetSelection(_list_options.GetNumberOptions() - 1);
        _action_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _list_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    } else if(InputManager->DownPress()) {
        _selection = LIST_SELECTED;
        _list_options.SetSelection(0);
        _action_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _list_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    }
}

void TreasureSupervisor::_UpdateList()
{
    if(InputManager->CancelPress() || _list_options.GetNumberOptions() == 0) {
        _selection = ACTION_SELECTED;
        _action_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
        _list_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    } else if(InputManager->ConfirmPress()) {
        _selection = DETAIL_SELECTED;
        _list_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

        uint32_t list_selection = _list_options.GetSelection();
        if(list_selection == 0 && _treasure->_drunes != 0) {
            // If true, the drunes have been selected
            _selection_name.SetText(UTranslate("Drunes"));
            _selection_icon = GlobalManager->Media().GetDrunesIcon();

            const std::string temp_string =
                /// tr: Part 2 of "With the additional %u drune(s) found in this treasure added, ... "
                NVTranslate("the party now holds a total of %u drune.", // singular
                            "the party now holds a total of %u drunes.", // plural
                            GlobalManager->GetDrunes());

            _detail_textbox.SetDisplayText(
                /// tr: %s will be replaced with "the party now holds a total of %u drune(s)."
                NVTranslate("With the additional %u drune found in this treasure added, %s.", // sinlgular
                            "With the additional %u drunes found in this treasure added, %s.", // plural
                            _treasure->_drunes,
                            temp_string));
            _is_key_item = false;
        } else { // Otherwise, a GlobalObject is selected
            if(_treasure->_drunes != 0)
                list_selection--;
            _selection_name.SetText(_treasure->_items_list[list_selection]->GetName());
            _is_key_item = _treasure->_items_list[list_selection]->IsKeyItem();
            // TODO: this is not good practice. We should probably either remove the const status from the GetIconImage() call
            _selection_icon = const_cast<StillImage *>(&_treasure->_items_list[list_selection]->GetIconImage());
            _detail_textbox.SetDisplayText(_treasure->_items_list[list_selection]->GetDescription());
        }
    } else if(InputManager->UpPress()) {
        if(_list_options.GetSelection() == 0) {
            _selection = ACTION_SELECTED;
            _action_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _list_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        } else {
            _list_options.InputUp();
        }
    } else if(InputManager->DownPress()) {
        if(static_cast<uint32_t>(_list_options.GetSelection()) == (_list_options.GetNumberOptions() - 1)) {
            _selection = ACTION_SELECTED;
            _action_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _list_options.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        } else {
            _list_options.InputDown();
        }
    }
}

void TreasureSupervisor::_UpdateDetail()
{
    if(InputManager->ConfirmPress() || InputManager->CancelPress()) {
        if(_detail_textbox.IsFinished() == false) {
            _detail_textbox.ForceFinish();
        } else {
            _selection = LIST_SELECTED;
            _list_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
        }
    }
}

} // namespace private_map

} // namespace vt_map
