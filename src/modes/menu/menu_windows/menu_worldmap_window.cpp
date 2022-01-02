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

//! \brief Area where on can draw the world map
const float WORLD_MAP_AREA_WIDTH = 815.0f;
const float WORLD_MAP_AREA_HEIGHT = 415.0f;
const float WINDOW_BORDER_WIDTH = 18.0f;

WorldMapWindow::WorldMapWindow() :
    _active(false)
{
    _location_marker.SetStatic(true);
    if(!_location_marker.LoadFromAnimationScript("data/gui/menus/rotating_crystal_grey.lua")) {
        PRINT_ERROR << "Could not load marker image!" << std::endl;
    }
    else {
        _location_marker.SetColor(vt_video::Color(0.1f, 1.0f, 0.1f, 1.0f));
        _location_marker.SetHeightKeepRatio(24.0f);
    }

    _location_pointer.SetStatic(true);
    if(!_location_pointer.Load("data/gui/menus/hand_down.png")) {
        PRINT_ERROR << "Could not load pointer image!" << std::endl;
    }

    WorldMapHandler& world_map = GlobalManager->GetWorldMapData();

    // Center the world map image view by default
    if (world_map.HasWorldMapImage()) {
        auto world_map_image = world_map.GetWorldMapImage();
        const float image_width = world_map_image.GetWidth();
        const float image_height = world_map_image.GetHeight();
        _target_position.x = (WORLD_MAP_AREA_WIDTH - image_width) / 2.0f;
        _target_position.y = (WORLD_MAP_AREA_HEIGHT - image_height) / 2.0f;
    }
}

void WorldMapWindow::Draw()
{
    MenuWindow::Draw();

    const WorldMapHandler& world_map_data = GlobalManager->GetWorldMapData();
    if(!world_map_data.HasWorldMapImage()) {
        return;
    }

    // Scissor the view to cut the layout properly
    Position2D window_position = GetPosition();
    const float left = window_position.x + WINDOW_BORDER_WIDTH;
    const float top = window_position.y + WINDOW_BORDER_WIDTH;

    VideoManager->PushScissoredRect(left,
                                    top,
                                    WORLD_MAP_AREA_WIDTH,
                                    WORLD_MAP_AREA_HEIGHT);

    VideoManager->Move(window_position.x + _view_position.x,
                       window_position.y + _view_position.y);

    world_map_data.GetWorldMapImage().Draw();

    // Draw the dots and currently selected location if active
    if(IsActive())
    {
        // Get the list of currently viewable world locations with the pointer
        _DrawViewableLocations();
    }

    VideoManager->PopScissoredRect();
}

void WorldMapWindow::_DrawViewableLocations()
{
    WorldMapHandler& world_map_data = GlobalManager->GetWorldMapData();
    const std::map<std::string, WorldMapLocation>& world_map_locations = world_map_data.GetVisibleWorldMapLocations();
    Position2D window_position = GetPosition();
    for(auto iter = world_map_locations.begin(); iter != world_map_locations.end(); ++iter)
    {
        const std::string& world_map_location_id = iter->first;
        const WorldMapLocation& location = iter->second;
        if (!location.IsVisible()) {
          continue;
        }

        // Draw the location marker
        const Position2D& marker_pos = location.GetPosition();
        VideoManager->Move(window_position.x + _view_position.x + marker_pos.x
                           - (_location_marker.GetWidth() / 2.0f),
                           window_position.y + _view_position.y + marker_pos.y
                           - _location_marker.GetHeight());
        _location_marker.Draw();

        // Draw the pointer
        if(_current_location != nullptr && world_map_location_id == _current_location->GetId()) {
            // this is a slight offset for the pointer
            // so that it points where we want it to, roughly in the center
            // of the location marker
            const Position2D pointer_offset(2.0f, -8.0f);
            const Position2D& location_pos = location.GetPosition();
            VideoManager->Move(window_position.x + _view_position.x
                               + location_pos.x + pointer_offset.x
                               - (_location_marker.GetWidth() / 2.0f),
                               window_position.y + _view_position.y
                               + location_pos.y - _location_pointer.GetHeight()
                               + pointer_offset.y - _location_marker.GetHeight());
            _location_pointer.Draw();
        }
    }
}

void WorldMapWindow::Update()
{
    auto world_map_data = GlobalManager->GetWorldMapData();
    if(!world_map_data.HasWorldMapImage()) {
        _active = false;
        return;
    }

    GlobalMedia& media = GlobalManager->Media();

    // Handle scrolling to the known location position
    _view_position.x = vt_utils::Lerp(_view_position.x, _target_position.x, 0.12f);
    _view_position.y = vt_utils::Lerp(_view_position.y, _target_position.y, 0.12f);

    // If this window is active, we check the cursor states
    if(IsActive())
    {
        WORLDMAP_NAVIGATION world_map_goto = WORLDMAP_NOPRESS;
        if(InputManager->CancelPress()) {
            world_map_goto = WORLDMAP_CANCEL;
        } else if(InputManager->LeftPress()) {
            world_map_goto = WORLDMAP_LEFT;
        } else if(InputManager->RightPress()) {
            world_map_goto = WORLDMAP_RIGHT;
        }

        // Cancel and exit
        if(world_map_goto == WORLDMAP_CANCEL) {
            _active = false;
            media.PlaySound("cancel");
        }
        // Otherwise, check if there was a key press
        else if(world_map_goto != WORLDMAP_NOPRESS)
        {
            // Play confirm sound
            media.PlaySound("bump");
            _SetSelectedLocation(world_map_goto);
        }

        _location_marker.Update();
    }
}

void WorldMapWindow::_SetSelectedLocation(WORLDMAP_NAVIGATION world_map_goto)
{
    auto world_map_data = GlobalManager->GetWorldMapData();
    if(!world_map_data.HasWorldMapImage()) {
        _current_location = nullptr;
        return;
    }

    // Center the view for small maps and no selected locations
    auto current_world_map = world_map_data.GetWorldMapImage();
    const float image_width = current_world_map.GetWidth();
    const float image_height = current_world_map.GetHeight();
    _target_position.x = (WORLD_MAP_AREA_WIDTH - image_width) / 2.0f;
    _target_position.y = (WORLD_MAP_AREA_HEIGHT - image_height) / 2.0f;

    const std::map<std::string, WorldMapLocation>& world_map_locations =
        world_map_data.GetVisibleWorldMapLocations();
    if(world_map_locations.empty()) {
        _current_location = nullptr;
        return;
    }

    const std::string& location_id = _current_location ? _current_location->GetId() :
                                     world_map_data.GetCurrentLocationId();
    auto iter = world_map_locations.find(location_id);
    if (iter == world_map_locations.end()) {
      iter = world_map_locations.begin();
    }

    if(world_map_goto == WORLDMAP_LEFT)
    {
        if(iter == world_map_locations.begin())
            iter = --world_map_locations.end(); // The last one
        else
            --iter;
    }
    else if (world_map_goto == WORLDMAP_RIGHT)
    {
        ++iter;
        if(iter == world_map_locations.end())
            iter = world_map_locations.begin();
    }
    _current_location = &iter->second;

    const Position2D& location_pos = _current_location->GetPosition();

    // Deal with the world map image standard offset
    _target_position.x = (WORLD_MAP_AREA_WIDTH / 2.0f) - location_pos.x;
    _target_position.y = (WORLD_MAP_AREA_HEIGHT / 2.0f) - location_pos.y;

    if (WORLD_MAP_AREA_WIDTH >= image_width) {
        // Center the view axis when the map is smaller than the view
        _target_position.x = (WORLD_MAP_AREA_WIDTH - image_width) / 2.0f;
    }
    else {
        // Prevent showing over the map boundaries
        if (location_pos.x <= WORLD_MAP_AREA_WIDTH / 2.0f) {
            _target_position.x = 0.0f;
        }
        else if (location_pos.x >= image_width - (WORLD_MAP_AREA_WIDTH / 2.0f)) {
            _target_position.x = (WORLD_MAP_AREA_WIDTH - image_width) / 2.0f;
        }
    }

    if (WORLD_MAP_AREA_HEIGHT >= image_height) {
        // Center the view axis when the map is smaller than the view
        _target_position.y = (WORLD_MAP_AREA_HEIGHT - image_height) / 2.0f;
    }
    else {
        // Prevent showing over the map boundaries
        if (location_pos.y <= WORLD_MAP_AREA_HEIGHT / 2.0f) {
            _target_position.y = 0.0f;
        }
        else if (location_pos.y >= image_height - (WORLD_MAP_AREA_HEIGHT / 2.0f)) {
            _target_position.y = (WORLD_MAP_AREA_HEIGHT - image_height) / 2.0f;
        }
    }
}

void WorldMapWindow::Activate(bool new_state)
{
    _active = new_state;
    if (!_active) {
        _current_location = nullptr;
        return;
    }

    // Set the pointer to the appropriate location
    // we only do this on activation of the window.
    // after that it is handled by the left / right press
    WorldMapHandler& world_map_data = GlobalManager->GetWorldMapData();
    const std::map<std::string, WorldMapLocation>& world_map_locations = world_map_data.GetVisibleWorldMapLocations();
    if (world_map_locations.empty()) {
        _current_location = nullptr;
        return;
    }

    const std::string& location_id = world_map_data.GetCurrentLocationId();
    auto iter = world_map_locations.find(location_id);
    if (iter == world_map_locations.end()) {
        iter = world_map_locations.begin();
    }
    _current_location = &iter->second;

    // Update the map offset when activating the view
    _SetSelectedLocation(WORLDMAP_NOPRESS);

}

} // namespace private_menu

} // namespace vt_menu
