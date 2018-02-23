////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "modes/battle/finish/battle_defeat.h"

#include "common/gui/menu_window.h"
#include "engine/video/video_utils.h"
#include "engine/system.h"
#include "engine/audio/audio.h"
#include "engine/input.h"

#include "utils/utils_common.h"

using namespace vt_gui;
using namespace vt_video;
using namespace vt_system;
using namespace vt_audio;
using namespace vt_input;

namespace vt_battle
{

namespace private_battle
{

//! \brief Draw position and dimension constants used for GUI objects
//@{
const float TOP_WINDOW_XPOS        = 512.0f;
const float TOP_WINDOW_YPOS        = 104.0f;
const float TOP_WINDOW_WIDTH       = 512.0f;
const float TOP_WINDOW_HEIGHT      = 64.0f;

const float TOOLTIP_WINDOW_XPOS    = TOP_WINDOW_XPOS;
const float TOOLTIP_WINDOW_YPOS    = TOP_WINDOW_YPOS + TOP_WINDOW_HEIGHT - 16.0f;
const float TOOLTIP_WINDOW_WIDTH   = TOP_WINDOW_WIDTH;
const float TOOLTIP_WINDOW_HEIGHT  = 112.0f;
//@}

BattleDefeat::BattleDefeat(DEFEAT_STATE state) :
    _state(state)
{
    _options_window.Create(TOP_WINDOW_WIDTH, TOP_WINDOW_HEIGHT, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM);
    _options_window.SetPosition(TOP_WINDOW_XPOS, TOP_WINDOW_YPOS);
    _options_window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
    _options_window.Show();

    _tooltip_window.Create(TOOLTIP_WINDOW_WIDTH, TOOLTIP_WINDOW_HEIGHT);
    _tooltip_window.SetPosition(TOOLTIP_WINDOW_XPOS, TOOLTIP_WINDOW_YPOS);
    _tooltip_window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
    _tooltip_window.Show();

    _options.SetOwner(&_options_window);
    _options.SetPosition(TOP_WINDOW_WIDTH / 2, 28.0f);
    _options.SetDimensions(480.0f, 50.0f, 2, 1, 2, 1);
    _options.SetTextStyle(TextStyle("title22", Color::white, VIDEO_TEXT_SHADOW_DARK));
    _options.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _options.SetSelectMode(VIDEO_SELECT_SINGLE);
    _options.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _options.SetCursorOffset(-60.0f, -25.0f);
    _options.AddOption(UTranslate("Retry"));
    _options.AddOption(UTranslate("End"));
    _options.SetSelection(0);

    _confirm_options.SetOwner(&_options_window);
    _confirm_options.SetPosition(TOP_WINDOW_WIDTH / 2, 28.0f);
    _confirm_options.SetDimensions(240.0f, 50.0f, 2, 1, 2, 1);
    _confirm_options.SetTextStyle(TextStyle("title22", Color::white, VIDEO_TEXT_SHADOW_DARK));
    _confirm_options.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _confirm_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _confirm_options.SetSelectMode(VIDEO_SELECT_SINGLE);
    _confirm_options.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _confirm_options.SetCursorOffset(-60.0f, -25.0f);
    _confirm_options.AddOption(UTranslate("OK"));
    _confirm_options.AddOption(UTranslate("Cancel"));
    _confirm_options.SetSelection(0);

    _tooltip.SetOwner(&_tooltip_window);
    _tooltip.SetPosition(32.0f, 40.0f);
    _tooltip.SetDimensions(480.0f, 80.0f);
    _tooltip.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _tooltip.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _tooltip.SetDisplaySpeed(SystemManager->GetMessageSpeed());
    _tooltip.SetTextStyle(TextStyle("text20", Color::white));
    _tooltip.SetDisplayMode(VIDEO_TEXT_INSTANT);
}

BattleDefeat::~BattleDefeat()
{
    _options_window.Destroy();
    _tooltip_window.Destroy();
}

void BattleDefeat::Initialize()
{
    _SetTooltipText();

    _options_window.Show();
    _tooltip_window.Show();
}

void BattleDefeat::Update()
{
    switch(_state) {
    case DEFEAT_SELECT:
        _options.Update();
        if(InputManager->ConfirmPress()) {
            if(!_options.IsOptionEnabled(_options.GetSelection())) {
                AudioManager->PlaySound("data/sounds/cancel.wav");
            } else {
                _state = DEFEAT_CONFIRM;
                // Set default confirm option to "Cancel"
                if(_options.GetSelection() == (int32_t)DEFEAT_OPTION_END)
                    _confirm_options.SetSelection(1);
                else
                    _confirm_options.SetSelection(0);

                _SetTooltipText();
            }
        }

        else if(InputManager->LeftPress()) {
            _options.InputLeft();
            _SetTooltipText();
        } else if(InputManager->RightPress()) {
            _options.InputRight();
            _SetTooltipText();
        }

        break;

    case DEFEAT_CONFIRM:
        _confirm_options.Update();
        if(InputManager->ConfirmPress()) {
            switch(_confirm_options.GetSelection()) {
            case 0: // "OK"
                _state = DEFEAT_END;
                _options_window.Hide();
                _tooltip_window.Hide();
                break;
            case 1: // "Cancel"
                _state = DEFEAT_SELECT;
                _SetTooltipText();
                break;
            default:
                PRINT_WARNING
                        << "invalid confirm option selection: "
                        << _confirm_options.GetSelection() << std::endl;
                break;
            }
        }
        else if(InputManager->CancelPress()) {
            _state = DEFEAT_SELECT;
            _SetTooltipText();
        }
        else if(InputManager->LeftPress()) {
            _confirm_options.InputLeft();
        } else if(InputManager->RightPress()) {
            _confirm_options.InputRight();
        }
        break;

    case DEFEAT_END:
        break;

    default:
        PRINT_WARNING << "invalid finish state: " << _state << std::endl;
        break;
    }
}

void BattleDefeat::Draw()
{
    _options_window.Draw();
    _tooltip_window.Draw();

    if(_state == DEFEAT_SELECT) {
        _options.Draw();
    } else if(_state == DEFEAT_CONFIRM) {
        _confirm_options.Draw();
    }

    _tooltip.Draw();
}

void BattleDefeat::_SetTooltipText()
{
    if(_state == DEFEAT_SELECT) {
        switch(_options.GetSelection()) {
        case DEFEAT_OPTION_RETRY:
            _tooltip.SetDisplayText(Translate("Start over from the beginning of this battle."));
            break;
        case DEFEAT_OPTION_END:
            _tooltip.SetDisplayText(UTranslate("Exit to main menu."));
            break;
        default:
            _tooltip.SetDisplayText("");
            break;
        }
    } else if(_state == DEFEAT_CONFIRM) {
        switch(_options.GetSelection()) {
        case DEFEAT_OPTION_RETRY:
            _tooltip.SetDisplayText(UTranslate("Confirm: retry battle."));
            break;
        case DEFEAT_OPTION_END:
            _tooltip.SetDisplayText(UTranslate("Confirm: return to main menu."));
            break;
        default:
            _tooltip.SetDisplayText("");
            break;
        }
    }
}

} // namespace private_battle

} // namespace vt_battle
