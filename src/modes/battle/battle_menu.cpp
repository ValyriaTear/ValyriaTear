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
const float OPTION_LIST_POS_X = WINDOW_POS_X + 25.0f;
const float OPTION_LIST_POS_Y = WINDOW_POS_Y + 60.0f;

const int RUSH_INDEX = 0;

BattleMenu::BattleMenu()
{
    _window.Create(WINDOW_SIZE_X, WINDOW_SIZE_Y);
    _window.SetPosition(WINDOW_POS_X, WINDOW_POS_Y);
    _window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _window.Show();

    _options_list.SetPosition(OPTION_LIST_POS_X, OPTION_LIST_POS_Y);
    _options_list.SetDimensions(WINDOW_SIZE_X - 25.0f, WINDOW_SIZE_Y - 25.0f, 3, 2, 3, 2);
    _options_list.SetTextStyle(TextStyle("title22"));
    _options_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _options_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _options_list.SetSelectMode(VIDEO_SELECT_SINGLE);
    _options_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _options_list.SetCursorOffset(-82.0f, -28.0f);
    _options_list.SetSkipDisabled(true);

    _RefreshOptions();
}

BattleMenu::~BattleMenu()
{
    _window.Destroy();
}

void BattleMenu::Draw()
{
    assert(_open);

    _window.Draw();
    _options_list.Draw();
}

void BattleMenu::Update()
{
    if (_open == false)
        return;

    if(InputManager->CancelPress()) {
        _open = false;
        return;
    }

    if (InputManager->ConfirmPress()) {
        if (_options_list.GetSelection() == RUSH_INDEX) {
            _rushActive = !_rushActive;

            // Automatically close menu if rush is turned on
            if (_rushActive)
                _open = false;

            _RefreshOptions();
        }
    }
}

void BattleMenu::Open()
{
    _open = true;
}

void BattleMenu::Close()
{
    _open = false;
}

bool BattleMenu::IsRushActive() const
{
    return _rushActive;
}

bool BattleMenu::IsOpen() const
{
    return _open;
}

void BattleMenu::_RefreshOptions()
{
    _options_list.ClearOptions();

    _options_list.AddOption();
    if (_rushActive) {
        _options_list.SetCursorOffset(-50.0f, -28.0f);
        _options_list.AddOptionElementImage(RUSH_INDEX, "data/gui/menus/star.png");
    }
    else {
        _options_list.SetCursorOffset(-82.0f, -28.0f);
    }
    _options_list.AddOptionElementPosition(RUSH_INDEX, 32);
    _options_list.AddOptionElementText(RUSH_INDEX, UTranslate("Rush"));
}

} // namespace private_battle

} // namespace vt_battle
