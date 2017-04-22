///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_windows/menu_skilltree_window.h"

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

SkillTreeWindow::SkillTreeWindow() :
    _current_x_offset(0),
    _current_y_offset(0),
    _selected_node_index(0),
    _active(false)
{
    // TODO
    _location_marker.SetStatic(true);
    if(!_location_marker.LoadFromAnimationScript("data/gui/menus/rotating_crystal_grey.lua"))
        PRINT_ERROR << "Could not load marker image!" << std::endl;

    _location_pointer.SetStatic(true);
    if(!_location_pointer.Load("data/gui/menus/hand_down.png"))
        PRINT_ERROR << "Could not load pointer image!" << std::endl;
}

void SkillTreeWindow::Activate(bool new_state)
{
    _active = new_state;

    // TODO
    // TEMP: Set the selection node to where the character was last located.
    _selected_node_index = 0;
}

void SkillTreeWindow::Update()
{
    if (!_active)
        return;

    // TODO
}

void SkillTreeWindow::Draw()
{
    MenuWindow::Draw();

    if(_active)
    {
        float window_position_x, window_position_y;
        GetPosition(window_position_x, window_position_y);
        VideoManager->Move(window_position_x + _current_x_offset, window_position_y + _current_y_offset);

        // TODO
    }
}

} // namespace private_menu

} // namespace vt_menu
