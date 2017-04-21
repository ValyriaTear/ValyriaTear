///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_windows/menu_worldmap_window.h"

#include "modes/menu/menu_mode.h"

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

WorldMapWindow::WorldMapWindow() :
    _current_world_map(nullptr),
    _current_image_x_offset(0),
    _current_image_y_offset(0),
    _location_pointer_index(0),
    _active(false)
{
    _location_marker.SetStatic(true);
    if(!_location_marker.LoadFromAnimationScript("data/gui/menus/rotating_crystal_grey.lua"))
        PRINT_ERROR << "Could not load marker image!" << std::endl;

    _location_pointer.SetStatic(true);
    if(!_location_pointer.Load("data/gui/menus/hand_down.png"))
        PRINT_ERROR << "Could not load pointer image!" << std::endl;
}

void WorldMapWindow::Draw()
{
    MenuWindow::Draw();
    if(_current_world_map == nullptr)
        return;
    float window_position_x, window_position_y;
    GetPosition(window_position_x, window_position_y);
    VideoManager->Move(window_position_x + _current_image_x_offset, window_position_y + _current_image_y_offset);

    _current_world_map->Draw();

    //draw the dots and currently selected location if active
    if(IsActive())
    {
        //get the list of currently viewable world locations with the pointer
        _DrawViewableLocations(window_position_x, window_position_y);
    }
}

void WorldMapWindow::_DrawViewableLocations(float window_position_x, float window_position_y)
{
    const std::vector<std::string> &current_location_ids = GlobalManager->GetViewableLocationIds();
    const uint32_t N = current_location_ids.size();
    for(uint32_t i = 0; i < N; ++i)
    {
        const WorldMapLocation *location = GlobalManager->GetWorldLocation(current_location_ids[i]);
        if(location == nullptr)
        {
            PRINT_WARNING << "location for id: "
                << current_location_ids[i]
                << " is not loaded into global manager" << std::endl;
            continue;
        }
        //draw the location marker
        VideoManager->Move(window_position_x + _current_image_x_offset + location->_x, window_position_y + _current_image_y_offset + location->_y);
        _location_marker.Draw();

        //draw the pointer
        if(i == _location_pointer_index)
        {
            //this is a slight offset for the pointer so that it points where we want it to, roughly in the center
            //of the location marker
            static const float minor_offset_x = 2.0f;
            static const float minor_offset_y = -8.0f;
            VideoManager->Move(window_position_x + _current_image_x_offset + location->_x + minor_offset_x,
                               window_position_y + _current_image_y_offset + location->_y - _location_pointer.GetHeight() + minor_offset_y);
            _location_pointer.Draw();
        }
    }
}

void WorldMapWindow::Update()
{
    _current_world_map = GlobalManager->GetWorldMapImage();
    if(!_current_world_map) {
        _active = false;
        return;
    }

    GlobalMedia& media = GlobalManager->Media();

    float image_width = _current_world_map->GetWidth();
    float image_height = _current_world_map->GetHeight();
    float window_width, window_height;
    GetDimensions(window_width, window_height);

    if( image_width > window_width)
        PRINT_WARNING << "World Map Image Width is too wide " << std::endl;

    //calculate neccesary offset from left.
    _current_image_x_offset = (window_width - image_width) / 2.0;
    //calculae neccesary offset fromtop
    _current_image_y_offset = (window_height - image_height) / 2.0;

    //if this window is active, we check the cursor states
    if(IsActive())
    {
        WORLDMAP_NAVIGATION worldmap_goto = WORLDMAP_NOPRESS;
        if(InputManager->CancelPress()) {
            worldmap_goto = WORLDMAP_CANCEL;
        } else if(InputManager->LeftPress()) {
            worldmap_goto = WORLDMAP_LEFT;
        } else if(InputManager->RightPress()) {
            worldmap_goto = WORLDMAP_RIGHT;
        }

        // cancel and exit
        if(worldmap_goto == WORLDMAP_CANCEL) {
            _active = false;
            media.PlaySound("cancel");
        }
        //otherwise check if there was a key press

        else if(worldmap_goto != WORLDMAP_NOPRESS)
        {
            //play confirm sound
            media.PlaySound("bump");
            _SetSelectedLocation(worldmap_goto);
        }

        _location_marker.Update();
    }
}

void WorldMapWindow::_SetSelectedLocation(WORLDMAP_NAVIGATION worldmap_goto)
{
    const std::vector<std::string> &current_location_ids =
        GlobalManager->GetViewableLocationIds();
    const uint32_t N = current_location_ids.size();
    if(N == 0)
        return;
    if(worldmap_goto == WORLDMAP_LEFT)
    {
        //cannot use mod properly since it is an unsigned value
        if(_location_pointer_index == 0)
            _location_pointer_index = N-1;
        else
            _location_pointer_index -= 1;
    }
    else if (worldmap_goto == WORLDMAP_RIGHT)
    {
        if(_location_pointer_index == N-1)
            _location_pointer_index = 0;
        else
            _location_pointer_index += 1;
    }
}

void WorldMapWindow::Activate(bool new_state)
{
    _active = new_state;

    //set the pointer to the appropriate location
    //we only do this on activation of the window. after that it is handled by the left / right press
    const std::string &location_id = GlobalManager->GetCurrentLocationId();
    const std::vector<std::string> &current_location_ids =
        GlobalManager->GetViewableLocationIds();
    std::vector<std::string>::const_iterator loc =
        std::find(current_location_ids.begin(), current_location_ids.end(), location_id);
    if(location_id.empty() || loc == current_location_ids.end() )
        _location_pointer_index = 0;
    else
        _location_pointer_index = loc - current_location_ids.begin();
}

} // namespace private_menu

} // namespace vt_menu
