////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "modes/battle/finish/battle_finish_supervisor.h"

#include "modes/battle/battle.h"
#include "modes/boot/boot.h"
#include "engine/mode_manager.h"

#include "engine/video/video.h"
#include "engine/system.h"

using namespace vt_video;
using namespace vt_system;
using namespace vt_gui;
using namespace vt_mode_manager;


/*
#include "engine/audio/audio.h"

#include "engine/input.h"


#include "modes/battle/battle_actions.h"
#include "modes/battle/battle_actors.h"
#include "modes/battle/battle_utils.h"


using namespace vt_utils;
using namespace vt_audio;


using namespace vt_input;
using namespace vt_global;
*/
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

const float CHAR_WINDOW_XPOS       = TOP_WINDOW_XPOS;
const float CHAR_WINDOW_YPOS       = TOOLTIP_WINDOW_YPOS;
const float CHAR_WINDOW_WIDTH      = TOP_WINDOW_WIDTH;
const float CHAR_WINDOW_HEIGHT     = 120.0f;

const float SPOILS_WINDOW_XPOS     = TOP_WINDOW_XPOS;
const float SPOILS_WINDOW_YPOS     = CHAR_WINDOW_YPOS + CHAR_WINDOW_HEIGHT + 30;
const float SPOILS_WINDOW_WIDTH    = TOP_WINDOW_WIDTH;
const float SPOILS_WINDOW_HEIGHT   = 220.0f;
//@}

FinishSupervisor::FinishSupervisor() :
    _state(FINISH_INVALID),
    _is_battle_victory(false),
    _battle_defeat(DEFEAT_INVALID),
    _battle_victory(VICTORY_INVALID)
{
    _outcome_text.SetPosition(TOP_WINDOW_XPOS - TOP_WINDOW_WIDTH / 2.0f, 48.0f);
    _outcome_text.SetDimensions(TOP_WINDOW_WIDTH, 50.0f);
    _outcome_text.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _outcome_text.SetTextAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
    _outcome_text.SetDisplaySpeed(SystemManager->GetMessageSpeed());
    _outcome_text.SetTextStyle(TextStyle("text24", Color::white));
    _outcome_text.SetDisplayMode(VIDEO_TEXT_INSTANT);
}

void FinishSupervisor::Initialize(bool victory)
{
    _is_battle_victory = victory;
    _state = FINISH_ANNOUNCE_RESULT;

    if(_is_battle_victory) {
        _battle_victory.Initialize();
        _outcome_text.SetDisplayText(UTranslate("The heroes were victorious!"));
    } else {
        _battle_defeat.Initialize();
        _outcome_text.SetDisplayText(UTranslate("The heroes fell in battle..."));
    }
}

void FinishSupervisor::Update()
{
    if(_state == FINISH_ANNOUNCE_RESULT) {
        if(_is_battle_victory) {
            _state = FINISH_VICTORY_START;
        } else {
            _state = FINISH_DEFEAT_SELECT;
        }
        return;
    }

    if(_is_battle_victory) {
        _battle_victory.Update();
    } else {
        _battle_defeat.Update();
    }

    if(_state == FINISH_END) {
        if(_is_battle_victory) {
            BattleMode::CurrentInstance()->ChangeState(BATTLE_STATE_EXITING);
        }
        else {
            switch(_battle_defeat.GetDefeatOption()) {
            case DEFEAT_OPTION_RETRY:
                BattleMode::CurrentInstance()->RestartBattle();
                break;
            case DEFEAT_OPTION_END:
                ModeManager->PopAll();
                ModeManager->Push(new vt_boot::BootMode(), false, true);
                break;
            default:
                IF_PRINT_WARNING(BATTLE_DEBUG)
                        << "invalid defeat option selected: "
                        << _battle_defeat.GetDefeatOption() << std::endl;
                break;
            }
        }
    }
}

void FinishSupervisor::Draw()
{
    _outcome_text.Draw();

    if(_is_battle_victory) {
        _battle_victory.Draw();
    } else {
        _battle_defeat.Draw();
    }
}

} // namespace private_battle

} // namespace vt_battle
