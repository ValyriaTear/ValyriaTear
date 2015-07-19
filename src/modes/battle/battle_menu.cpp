////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_menu.cpp
*** \author  Christopher Berman, suitecake@gmail.com
*** \brief   Source file for battle menu
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "engine/input.h"
#include "modes/battle/battle_menu.h"

#include "engine/video/video.h"

using namespace vt_video;
using namespace vt_gui;
using namespace vt_system;
using namespace vt_input;

namespace vt_battle
{

namespace private_battle
{

const float WINDOW_POS_X = 512.0f;
const float WINDOW_POS_Y = 500.0f;
const float WINDOW_SIZE_X = 412.0f;
const float WINDOW_SIZE_Y = 120.0f;

const float OPTION_LIST_SIZE_X = WINDOW_SIZE_X;
const float OPTION_LIST_SIZE_Y = WINDOW_SIZE_Y;
const float OPTION_LIST_POS_X = WINDOW_POS_X + 65.0f;
const float OPTION_LIST_POS_Y = WINDOW_POS_Y + 60.0f;

const int AUTOBATTLE_MENU_INDEX = 0;

BattleMenu::BattleMenu():
    _explanation_window(vt_utils::ustring(), -1.0f, 650.0f, 750.0f, 100.0f),
    _auto_battle_active(false),
    _open(false)
{
    _window.Create(WINDOW_SIZE_X, WINDOW_SIZE_Y);
    _window.SetPosition(WINDOW_POS_X, WINDOW_POS_Y);
    _window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _window.Show();

    _options_list.SetPosition(OPTION_LIST_POS_X, OPTION_LIST_POS_Y);
    _options_list.SetDimensions(WINDOW_SIZE_X - 25.0f, WINDOW_SIZE_Y - 25.0f, 3, 2, 3, 2);
    _options_list.SetTextStyle(TextStyle("text22"));
    _options_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _options_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _options_list.SetSelectMode(VIDEO_SELECT_SINGLE);
    _options_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _options_list.SetSkipDisabled(true);
    _options_list.SetCursorState(vt_gui::VIDEO_CURSOR_STATE_HIDDEN);

    _options_selected_list.SetPosition(OPTION_LIST_POS_X - 35.0f, OPTION_LIST_POS_Y);
    _options_selected_list.SetDimensions(WINDOW_SIZE_X - 25.0f, WINDOW_SIZE_Y - 25.0f, 3, 2, 3, 2);
    _options_selected_list.SetTextStyle(TextStyle("text22"));
    _options_selected_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _options_selected_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _options_selected_list.SetSelectMode(VIDEO_SELECT_SINGLE);
    _options_selected_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _options_selected_list.SetCursorOffset(-52.0f, -28.0f);
    _options_selected_list.SetSkipDisabled(true);
    _options_selected_list.SetCursorState(vt_gui::VIDEO_CURSOR_STATE_VISIBLE);

    _explanation_window.SetDisplaySpeed(vt_system::SystemManager->GetMessageSpeed());

    _RefreshOptions();
}

BattleMenu::~BattleMenu()
{
    _window.Destroy();
}

void BattleMenu::Draw()
{
    _window.Draw();
    _options_list.Draw();
    _options_selected_list.Draw();

    _explanation_window.Draw();
}

void BattleMenu::Update()
{
    if (_open == false)
        return;

    _explanation_window.Update();

    if(InputManager->CancelPress()) {
        _open = false;
        return;
    }

    if (InputManager->ConfirmPress()) {
        if (_options_list.GetSelection() == AUTOBATTLE_MENU_INDEX) {
            _auto_battle_active = !_auto_battle_active;

            _RefreshOptions();
        }
    }

    _options_list.Update();
    _options_selected_list.Update();
}

void BattleMenu::SetAutoBattleActive(bool active)
{
    _auto_battle_active = active;
    _RefreshOptions();
}

void BattleMenu::_RefreshOptions()
{
    _options_list.ClearOptions();
    _options_selected_list.ClearOptions();

    // Auto-Battle
    _options_list.AddOption();
    _options_selected_list.AddOption();

    _options_selected_list.AddOptionElementText(AUTOBATTLE_MENU_INDEX, vt_utils::ustring());
    if (_auto_battle_active) {
        _options_selected_list.AddOptionElementImage(AUTOBATTLE_MENU_INDEX, "data/gui/menus/green_check.png");
        _options_selected_list.GetEmbeddedImage(AUTOBATTLE_MENU_INDEX)->SetWidthKeepRatio(24);
    }
    _options_list.AddOptionElementImage(AUTOBATTLE_MENU_INDEX, "data/gui/battle/auto_battle.png");
    _options_list.GetEmbeddedImage(AUTOBATTLE_MENU_INDEX)->SetWidthKeepRatio(32);
    _options_list.AddOptionElementPosition(AUTOBATTLE_MENU_INDEX, 40);
    _options_list.AddOptionElementText(AUTOBATTLE_MENU_INDEX, UTranslate("Auto-Battle"));

    _RefreshExplanationWindow();
}

void BattleMenu::_RefreshExplanationWindow()
{
    if (_options_selected_list.GetSelection() == AUTOBATTLE_MENU_INDEX) {
        _explanation_window.Show();
        _explanation_window.SetText(UTranslate("When the Auto-Battle mode is activated, your party will auto attack the opponents using default attacks. If you open a command menu, the Auto-Battle mode will be deactivated again."));
    }
    else {
        _explanation_window.Hide();
    }
}

} // namespace private_battle

} // namespace vt_battle
