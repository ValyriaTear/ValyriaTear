///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_states/menu_worldmap_state.h"

#include "modes/menu/menu_mode.h"

namespace vt_menu {

namespace private_menu {

WorldMapState::WorldMapState(MenuMode* menu_mode):
    AbstractMenuState("WorldMapState", menu_mode)
{
    _location_text.SetPosition(102, 556);
    _location_text.SetDimensions(500.0f, 50.0f);
    _location_text.SetTextStyle(vt_video::TextStyle("title22"));
    _location_text.SetAlignment(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_CENTER);
}

void WorldMapState::Reset()
{
    //automatically go into the world map window
    _menu_mode->_world_map_window.Activate(true);

    // defensive update to set up the initial values such as the
    // window offset and such
    _menu_mode->_world_map_window.Update();
}

void WorldMapState::_OnDrawMainWindow()
{
    _menu_mode->_world_map_window.Draw();
    _DrawBottomMenu();
}

void WorldMapState::_DrawBottomMenu()
{
    _menu_mode->_bottom_window.Draw();
    if(!_IsActive())
        return;

    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT,
                                         vt_video::VIDEO_Y_BOTTOM, 0);
    vt_video::VideoManager->Move(150, 580);
    // Display Location
    _location_text.Draw();
    if(!_location_image.GetFilename().empty())
    {
        vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT,
                                             vt_video::VIDEO_Y_BOTTOM, 0);
        vt_video::VideoManager->Move(390, 685);
        _location_image.Draw();
    }
}

bool WorldMapState::_IsActive()
{
    return _menu_mode->_world_map_window.IsActive();
}

void WorldMapState::_ActiveWindowUpdate()
{
    _menu_mode->_world_map_window.Update();
    if(!_IsActive()) {
        _OnCancel();
        return;
    }

    // Draw the current viewing location information
    const vt_global::WorldMapLocation* current_location = _menu_mode->_world_map_window.GetCurrentViewingLocation();
    if(current_location == nullptr)
    {
        _location_image.Clear();
        _location_text.ClearText();
        return;
    }
    _location_text.SetDisplayText(current_location->GetLocationName());
    _location_image.Load(current_location->GetLocationImageFileName());
}

} // namespace private_menu

} // namespace vt_menu
