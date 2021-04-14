///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
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
using namespace vt_common;

namespace vt_menu
{

namespace private_menu
{

WorldMapWindow::WorldMapWindow() :
    _current_world_map(nullptr),
    _view_position(0.0f, 0.0f),
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

    if(_current_world_map == nullptr) {
        return;
    }
    Position2D window_position = GetPosition();
    VideoManager->Move(window_position.x + _view_position.x,
                       window_position.y + _view_position.y);

    _current_world_map->Draw();

    // Draw the dots and currently selected location if active
    if(IsActive())
    {
        // Get the list of currently viewable world locations with the pointer
        _DrawViewableLocations();
    }
}

void WorldMapWindow::_DrawViewableLocations()
{
    WorldMapHandler& worldmap = GlobalManager->GetWorldMapData();
    const std::map<std::string, WorldMapLocation>& worldmap_locations = worldmap.GetWorldMapLocations();
    Position2D window_position = GetPosition();
    for(auto iter = worldmap_locations.begin(); iter != worldmap_locations.end(); ++iter)
    {
        const WorldMapLocation& location = iter->second;
        if (!location._visible) {
          continue;
        }

        // Draw the location marker
        const Position2D& marker_pos = location.GetPosition();
        VideoManager->Move(window_position.x + _view_position.x + marker_pos.x,
                           window_position.y + _view_position.y + marker_pos.y);
        _location_marker.Draw();

        // Draw the pointer
        if(location._world_map_location_id == _current_location_id) {
            // this is a slight offset for the pointer
            // so that it points where we want it to, roughly in the center
            // of the location marker
            const Position2D pointer_offset(2.0f, -8.0f);
            VideoManager->Move(window_position.x + _view_position.x
                               + location._pos.x + pointer_offset.x,
                               window_position.y + _view_position.y
                               + location._pos.y - _location_pointer.GetHeight()
                               + pointer_offset.y);
            _location_pointer.Draw();
        }
    }
}

void WorldMapWindow::Update()
{
    _current_world_map = GlobalManager->GetWorldMapData().GetWorldMapImage();
    if(!_current_world_map) {
        _active = false;
        return;
    }

    GlobalMedia& media = GlobalManager->Media();

    float image_width = _current_world_map->GetWidth();
    float image_height = _current_world_map->GetHeight();
    float window_width, window_height;
    GetDimensions(window_width, window_height);

    // Center the view by default
    _view_position.x = (window_width - image_width) / 2.0;
    _view_position.y = (window_height - image_height) / 2.0;
    if( image_width > window_width)
        PRINT_WARNING << "World Map Image Width is too wide " << std::endl;


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
    const std::map<std::string, WorldMapLocation>& worldmap_locations =
        GlobalManager->GetWorldMapData().GetWorldMapLocations();
    if(worldmap_locations.size() == 0)
        return;
    auto iter = worldmap_locations.find(_current_location_id);
    if (iter == worldmap_locations.end()) {
      iter = worldmap_locations.begin();
    }
    if(worldmap_goto == WORLDMAP_LEFT)
    {
        if(iter == worldmap_locations.begin())
            iter = --worldmap_locations.end(); // The last one
        else
            --iter;
    }
    else if (worldmap_goto == WORLDMAP_RIGHT)
    {
        ++iter;
        if(iter == worldmap_locations.end())
            iter = worldmap_locations.begin();
    }
    _current_location_id = iter->second._world_map_location_id;
}

void WorldMapWindow::Activate(bool new_state)
{
    WorldMapHandler& worldmap = GlobalManager->GetWorldMapData();

    _active = new_state;

    // set the pointer to the appropriate location
    // we only do this on activation of the window.
    // after that it is handled by the left / right press
    const std::string& location_id = worldmap.GetCurrentLocationId();
    const std::map<std::string, WorldMapLocation>& worldmap_locations = worldmap.GetWorldMapLocations();
    if (worldmap_locations.size() == 0) {
      return;
    }

    auto iter = worldmap_locations.find(location_id);
    if (iter == worldmap_locations.end()) {
        iter = worldmap_locations.begin();
    }
    _current_location_id = iter->second._world_map_location_id;
}

const vt_global::WorldMapLocation* WorldMapWindow::GetCurrentViewingLocation() const
{
    WorldMapHandler& worldmap = GlobalManager->GetWorldMapData();
    const std::map<std::string, WorldMapLocation>& worldmap_locations = worldmap.GetWorldMapLocations();
    auto iter = worldmap_locations.find(_current_location_id);
    if(iter == worldmap_locations.end())
        return nullptr;
    return &iter->second;
}

} // namespace private_menu

} // namespace vt_menu
