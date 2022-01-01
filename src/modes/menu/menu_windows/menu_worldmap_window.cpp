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
    _view_position(0.0f, 0.0f),
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
}

//! \brief Area where on can draw the world map
const float WORLDMAP_AREA_WIDTH = 815.0f;
const float WORLDMAP_AREA_HEIGHT = 415.0f;
const float WINDOW_BORDER_WIDTH = 18.0f;

void WorldMapWindow::Draw()
{
    MenuWindow::Draw();

    const WorldMapHandler& world_map_data = GlobalManager->GetWorldMapData();
    if(!world_map_data.HasWorldMapImage()) {
        return;
    }

    // Scissor the view to cut the layout properly
    Position2D window_position = GetPosition();
    float left = window_position.x + WINDOW_BORDER_WIDTH;
    float top = window_position.y + WINDOW_BORDER_WIDTH;
    float width = WORLDMAP_AREA_WIDTH;
    float height = WORLDMAP_AREA_HEIGHT;

    VideoManager->PushScissoredRect(left,
                                    top,
                                    width,
                                    height);

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
        if(world_map_location_id == _current_location_id) {
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

    auto current_world_map = GlobalManager->GetWorldMapData().GetWorldMapImage();
    float image_width = current_world_map.GetWidth();
    float image_height = current_world_map.GetHeight();
    float window_width, window_height;
    GetDimensions(window_width, window_height);

    // Handle scrolling to the known location position
    if (_current_location_id.empty()) {
        // Center the view by default
        _view_position.x = (window_width - image_width) / 2.0f;
        _view_position.y = (window_height - image_height) / 2.0f;
    }
    else {
        // Smooth the view move
        _view_position.x = vt_utils::Lerp(_view_position.x, _target_position.x, 0.12f);
        _view_position.y = vt_utils::Lerp(_view_position.y, _target_position.y, 0.12f);
    }

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
        // Otherwise check if there was a key press
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
    const std::map<std::string, WorldMapLocation>& world_map_locations =
        GlobalManager->GetWorldMapData().GetVisibleWorldMapLocations();
    if(world_map_locations.empty()) {
        _current_location_id.clear();
        return;
    }

    auto iter = world_map_locations.find(_current_location_id);
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
    _current_location_id = iter->first;
    _current_location_pos = iter->second.GetPosition();

    // Get the target offset of the selected marker
    Position2D target_position = Position2D();
    _target_position.x = target_position.x
                         + (WORLDMAP_AREA_WIDTH / 2.0f)
                         - _current_location_pos.x;
    _target_position.y = target_position.y
                         + (WORLDMAP_AREA_HEIGHT / 2.0f)
                         - _current_location_pos.y;
}

void WorldMapWindow::Activate(bool new_state)
{
    WorldMapHandler& world_map = GlobalManager->GetWorldMapData();

    _active = new_state;

    // set the pointer to the appropriate location
    // we only do this on activation of the window.
    // after that it is handled by the left / right press
    const std::string& location_id = world_map.GetCurrentLocationId();
    const std::map<std::string, WorldMapLocation>& world_map_locations = world_map.GetVisibleWorldMapLocations();
    if (world_map_locations.empty()) {
      return;
    }

    auto iter = world_map_locations.find(location_id);
    if (iter == world_map_locations.end()) {
        iter = world_map_locations.begin();
    }
    _current_location_id = iter->first;
    _current_location_pos = iter->second.GetPosition();

    // Get the target offset of the selected marker
    Position2D target_position = Position2D();
    _target_position.x = target_position.x
                         + (WORLDMAP_AREA_WIDTH / 2.0f)
                         - _current_location_pos.x;
    _target_position.y = target_position.y
                         + (WORLDMAP_AREA_HEIGHT / 2.0f)
                         - _current_location_pos.y;
}

const vt_global::WorldMapLocation* WorldMapWindow::GetCurrentViewingLocation() const
{
    WorldMapHandler& world_map = GlobalManager->GetWorldMapData();
    const std::map<std::string, WorldMapLocation>& world_map_locations = world_map.GetVisibleWorldMapLocations();
    auto iter = world_map_locations.find(_current_location_id);
    if(iter == world_map_locations.end())
        return nullptr;
    return &iter->second;
}

} // namespace private_menu

} // namespace vt_menu
