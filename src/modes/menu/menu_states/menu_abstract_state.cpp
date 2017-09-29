///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_states/menu_abstract_state.h"

#include "modes/menu/menu_mode.h"

#include "engine/video/video.h"
#include "common/global/global.h"
#include "engine/input.h"

namespace vt_menu {

namespace private_menu {

void SetupOptionBoxCommonSettings(vt_gui::OptionBox* ob)
{
    // Set all the default options
    ob->SetTextStyle(vt_video::TextStyle("text24"));
    ob->SetPosition(142.0f, 85.0f);
    ob->SetDimensions(115.0f, 50.0f, 1, 1, 1, 1);
    ob->SetAlignment(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_CENTER);
    ob->SetOptionAlignment(vt_video::VIDEO_X_CENTER, vt_video::VIDEO_Y_CENTER);
    ob->SetSelectMode(vt_gui::VIDEO_SELECT_SINGLE);
    ob->SetHorizontalWrapMode(vt_gui::VIDEO_WRAP_MODE_STRAIGHT);
    ob->SetCursorOffset(-52.0f, -20.0f);
}

AbstractMenuState::AbstractMenuState(const std::string& state_name, MenuMode* menu_mode):
    _state_name(state_name),
    _menu_mode(menu_mode),
    _from_state(nullptr)
{
}

void AbstractMenuState::Update()
{
    vt_global::GlobalMedia& media = vt_global::GlobalManager->Media();

    // if the current state is set to active, to an active update instead and return
    if(_IsActive())
    {
        _ActiveWindowUpdate();
        return;
    }

    // handle a cancel press. in the case that we are at the main_menu state, pop the ModeManager off
    // the Mode stack as well.
    if(vt_input::InputManager->CancelPress())
    {
        media.PlaySound("cancel");
        if(_menu_mode->_current_menu_state == &(_menu_mode->_main_menu_state))
            vt_mode_manager::ModeManager->Pop();
        // do instance specific cancel logic
        _OnCancel();
        return;
    }
    // handle left / right option box movement
    else if(vt_input::InputManager->LeftPress())
    {
        media.PlaySound("bump");
        _options.InputLeft();
        return;
    }
    else if(vt_input::InputManager->RightPress())
    {
        media.PlaySound("bump");
        _options.InputRight();
        return;
    }
    // play a sound if the option is selected
    else if(vt_input::InputManager->ConfirmPress())
    {
        if(_options.IsOptionEnabled((_options.GetSelection())))
            media.PlaySound("confirm");
        _options.InputConfirm();
    }
    // return the event type from the option
    int32_t event = _options.GetEvent();
    // update the current option box for this state, thus clearing the event flag
    // if we don't do this, then upon return we enter right back into the state we wanted
    // to return from
    _options.Update();

    if(event == vt_gui::VIDEO_OPTION_CONFIRM) {
        uint32_t selection = _options.GetSelection();
        AbstractMenuState* next_state = GetTransitionState(selection);
        // if the next state is the state we came from, it is similar to "cancel"
        if(next_state == _from_state)
        {
            _OnCancel();
            return;
        }
        // otherwise, if the state is valid and not this state itself, handle the transition
        else if(next_state != nullptr && next_state != this)
        {
            // change the static current menu state
            _menu_mode->_current_menu_state = next_state;

            next_state->_from_state = this;
            next_state->Reset();
        }
        // When we change the state, update the time immediately to avoid
        // showing outdated or empty time info
        _menu_mode->ForceUpdateOfTime();
    }

    // update the current state
    _OnUpdateState();
    // update the options for the currently active state
    _menu_mode->_current_menu_state->GetOptions()->Update();

    _menu_mode->UpdateTimeAndDrunes();
}

void AbstractMenuState::Draw()
{
    // Draw the saved screen background
    // For that, set the system coordinates to the size of the window (same with the save-screen)
    int32_t width = vt_video::VideoManager->GetViewportWidth();
    int32_t height = vt_video::VideoManager->GetViewportHeight();
    vt_video::VideoManager->SetCoordSys(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT,
                                         vt_video::VIDEO_Y_BOTTOM,
                                         vt_video::VIDEO_BLEND, 0);

    vt_video::DrawCapturedBackgroundImage(_menu_mode->_saved_screen, 0.0f, 0.0f);

    // Restore the Coordinate system (that one is menu mode coordinate system).
    vt_video::VideoManager->SetStandardCoordSys();
    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT,
                                         vt_video::VIDEO_Y_TOP,
                                         vt_video::VIDEO_BLEND, 0);

    // Move to the top left corner
    vt_video::VideoManager->Move(0.0f, 0.0f);

    _menu_mode->_main_options_window.Draw();

    // do instance specific main window rendering
    _OnDrawMainWindow();
    // do instance specific side window rendering
    _OnDrawSideWindow();
    // Draw currently active options box
    _options.Draw();
}

void AbstractMenuState::_OnDrawSideWindow()
{
    _menu_mode->_character_window0.Draw();
    _menu_mode->_character_window1.Draw();
    _menu_mode->_character_window2.Draw();
    _menu_mode->_character_window3.Draw();
}

void AbstractMenuState::_DrawBottomMenu()
{
    _menu_mode->_bottom_window.Draw();

    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT,
                                         vt_video::VIDEO_Y_BOTTOM, 0);
    vt_video::VideoManager->Move(150.0f, 580.0f);
    // Display Location
    _menu_mode->_locale_name.Draw();

    // Draw Played Time
    _menu_mode->_time_text.Draw();

    // Display the current funds that the party has
    _menu_mode->_drunes_text.Draw();

    vt_video::VideoManager->MoveRelative(-50.0f, 60.0f);
    _menu_mode->_clock_icon->Draw();
    vt_video::VideoManager->MoveRelative(0.0f, 30.0f);
    _menu_mode->_drunes_icon->Draw();

    if(!_menu_mode->_locale_graphic.GetFilename().empty()) {
        vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_RIGHT,
                                             vt_video::VIDEO_Y_BOTTOM, 0);
        vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT,
                                             vt_video::VIDEO_Y_BOTTOM, 0);
        vt_video::VideoManager->Move(390.0f, 685.0f);
        _menu_mode->_locale_graphic.Draw();
    }
}

void AbstractMenuState::_OnCancel()
{
    // as long as the calling state is valid and not equal to this, simply switch back to it
    if(_from_state && _from_state != this)
        _menu_mode->_current_menu_state = _from_state;
}

} // namespace private_menu

} // namespace vt_menu
