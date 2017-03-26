///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \brief   Source file for map mode escape circles handling.
*** ***************************************************************************/

#include "utils/utils_pch.h"

#include "modes/map/map_escape.h"
#include "modes/map/map_mode.h"

#include "engine/audio/audio.h"

#include "engine/input.h"
#include "engine/system.h"
#include "engine/video/video.h"

#include "common/global/global.h"

#include "utils/utils_files.h"

namespace vt_map
{

namespace private_map
{
EscapeSupervisor::EscapeSupervisor() :
    _window_title(vt_system::UTranslate("Escape to destination?"),
                  vt_video::TextStyle("title24",
                            vt_video::Color::white,
                            vt_video::VIDEO_TEXT_SHADOW_DARK,
                            1, -2)),
    _location_valid(false)
{
    // Create the menu windows and option boxes used for the treasure menu and
    // align them at the appropriate locations on the screen
    _escape_window.Create(700.0f, 250.0f);
    _escape_window.SetPosition(512.0f, 460.0f);
    _escape_window.SetAlignment(vt_video::VIDEO_X_CENTER, vt_video::VIDEO_Y_TOP);

    _action_options.SetPosition(150.0f, 53.0f);
    _action_options.SetDimensions(450.0f, 25.0f, 2, 1, 2, 1);
    _action_options.SetAlignment(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_TOP);
    _action_options.SetOptionAlignment(vt_video::VIDEO_X_CENTER, vt_video::VIDEO_Y_CENTER);
    _action_options.SetHorizontalWrapMode(vt_gui::VIDEO_WRAP_MODE_STRAIGHT);
    _action_options.SetSelectMode(vt_gui::VIDEO_SELECT_SINGLE);
    _action_options.SetCursorOffset(-50.0f, -25.0f);
    _action_options.SetTextStyle(vt_video::TextStyle("title22",
                                                     vt_video::Color::white,
                                                     vt_video::VIDEO_TEXT_SHADOW_DARK,
                                                     1, -2));
    _action_options.AddOption(vt_system::CUTranslate("EscapeMenu|No"));
    _action_options.AddOption(vt_system::CUTranslate("EscapeMenu|Yes"));
    _action_options.SetSelection(1);
    _action_options.SetOwner(&_escape_window);

    _map_hud_name.SetStyle(vt_video::TextStyle("title20",
                                               vt_video::Color::white,
                                               vt_video::VIDEO_TEXT_SHADOW_DARK,
                                               1, -2));
}

EscapeSupervisor::~EscapeSupervisor()
{
    _escape_window.Destroy();
}

void EscapeSupervisor::Initialize(const vt_map::MapLocation& map_location)
{
    // Init the map location
    _map_location = map_location;
    MapMode::CurrentInstance()->PushState(STATE_ESCAPE);

    _escape_window.Show();

    _location_valid = _map_location.IsValid();
    if (!_location_valid) {
        _action_options.EnableOption(1, false);
        _action_options.SetSelection(0);

        // Set a default text in that case
        _map_hud_name.SetText(vt_system::UTranslate("No available destination"));
        return;
    }
    _action_options.EnableOption(1, true);
    _action_options.SetSelection(1);

    // If valid, attempt to add the map image and texts
    _LoadMapLocationPreview();
}

void EscapeSupervisor::Update()
{
    _escape_window.Update();
    _action_options.Update();

    // Update action
    if(vt_input::InputManager->ConfirmPress()) {
        if(_action_options.GetSelection() == 0)
            Cancel();
        else
            Finish();
    }
    else if(vt_input::InputManager->CancelPress()) {
        Cancel();
    }
    else if(vt_input::InputManager->LeftPress()) {
        _action_options.InputLeft();
    }
    else if(vt_input::InputManager->RightPress()) {
        _action_options.InputRight();
    }
}

void EscapeSupervisor::Draw()
{
    vt_video::VideoManager->PushState();
    vt_video::VideoManager->SetStandardCoordSys();
    _escape_window.Draw();

    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT,
                                         vt_video::VIDEO_Y_CENTER, 0);
    vt_video::VideoManager->Move(195.0f, 500.0f);
    _window_title.Draw();

    _action_options.Draw();

    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT,
                                         vt_video::VIDEO_Y_TOP, 0);
    if (!_location_valid) {
        vt_video::VideoManager->MoveRelative(15.0f, _window_title.GetHeight() + 40.0f);
        _map_hud_name.Draw();
        return;
    }
    vt_video::VideoManager->MoveRelative((_escape_window.GetWidth() - _location_image.GetWidth()) / 2.0f,
                                         _action_options.GetHeight() + 20.0f);
    _location_image.Draw();
    vt_video::VideoManager->MoveRelative(0.0f, _location_image.GetHeight() + 15.0f);
    _map_hud_name.Draw();
    vt_video::VideoManager->MoveRelative(_map_hud_name.GetWidth() + 20.0f, 0.0f);
    _map_hud_subname.Draw();

    vt_video::VideoManager->PopState();
}

void EscapeSupervisor::Cancel()
{
    _escape_window.Hide();

    MapMode* map_mode = MapMode::CurrentInstance();

    if(map_mode->CurrentState() == vt_map::private_map::STATE_ESCAPE)
        map_mode->PopState();
}

void EscapeSupervisor::Finish()
{
    // Escape to given map coordinates.
    vt_audio::AudioManager->PlaySound("data/sounds/warp.ogg");
    vt_global::GlobalManager->SetPreviousLocation(std::string());
    MapMode* MM = new MapMode(_map_location.GetMapDataFilename(),
                              _map_location.GetMapScriptFilename(),
                              MapMode::CurrentInstance()->GetStamina());
    vt_mode_manager::ModeManager->Pop();
    vt_mode_manager::ModeManager->Push(MM, true, true);
}

bool EscapeSupervisor::_LoadMapLocationPreview()
{
    vt_script::ReadScriptDescriptor file;

    // Check whether the map script file is actually available
    const std::string& map_filename = _map_location.GetMapScriptFilename();
    if (map_filename.empty()
        || !vt_utils::DoesFileExist(map_filename)) {
        return false;
    }

    // Test the map file

    // Tests the map file and gets the untranslated map hud name from it.
    vt_script::ReadScriptDescriptor map_file;
    if(!map_file.OpenFile(map_filename)) {
        return false;
    }

    if (map_file.OpenTablespace().empty()) {
        map_file.CloseFile();
        return false;
    }

    // Read the in-game location of the save
    std::string map_hud_name = map_file.ReadString("map_name");
    _map_hud_name.SetText(vt_system::UTranslate(map_hud_name));
    map_hud_name = map_file.ReadString("map_subname");
    _map_hud_subname.SetText(vt_system::UTranslate(map_hud_name));

    // Loads the potential location image
    std::string map_image_filename = map_file.ReadString("map_image_filename");
    if (map_image_filename.empty()) {
        _location_image.Clear();
    }
    else {
        if (_location_image.Load(map_image_filename))
            _location_image.SetWidthKeepRatio(450.0f);
    }

    map_file.CloseTable(); // Tablespace
    map_file.CloseFile();

    return true;
}

} // namespace private_map

} // namespace vt_map
