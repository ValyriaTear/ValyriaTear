///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    menu_window.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for menu window class
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "menu_window.h"

#include "engine/video/video.h"

using namespace vt_utils;
using namespace vt_video;
using namespace vt_video::private_video;
using namespace vt_gui::private_gui;

namespace vt_gui
{

MenuWindow::MenuWindow() :
    _window_state(VIDEO_MENU_STATE_HIDDEN)
{
    _skin = GUIManager->_GetDefaultMenuSkin();
}



bool MenuWindow::Create(const std::string &skin_name, float w, float h, int32 visible_flags, int32 shared_flags)
{
    _skin = GUIManager->_GetMenuSkin(skin_name);
    if(_skin == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "the requested menu skin was not found: " << skin_name << std::endl;
        return false;
    }

    return Create(w, h, visible_flags, shared_flags);
}



bool MenuWindow::Create(float w, float h, int32 visible_flags, int32 shared_flags)
{
    if(w <= 0 || h <= 0) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "width and/or height argument was invalid: "
                                      << "(width = " << w << ", height = " << h << ")" << std::endl;
        return false;
    }

    _width = w;
    _height = h;
    _edge_visible_flags = visible_flags;
    _edge_shared_flags = shared_flags;

    if(_skin == NULL)
        _skin = GUIManager->_GetDefaultMenuSkin();

    if(_skin == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "a default menu skin was unavailable (no skins were loaded)" << std::endl;
        return false;
    }

    if(_RecreateImage() == false)
        return false;

    // Add the new menu window to the menu map
    GUIManager->_AddMenuWindow(this);
    return true;
}



void MenuWindow::Destroy()
{
    _skin = NULL;
    GUIManager->_RemoveMenuWindow(this);
}

void MenuWindow::Draw(const Color& color)
{
    if(_window_state == VIDEO_MENU_STATE_HIDDEN)
        return;

    VideoManager->PushState();
    VideoManager->SetDrawFlags(_xalign, _yalign, VIDEO_BLEND, 0);

    VideoManager->Move(_x_position, _y_position);
    _menu_image.Draw(color);

    if(GUIManager->DEBUG_DrawOutlines() == true) {
        _DEBUG_DrawOutline();
    }

    VideoManager->PopState();
    return;
} // void MenuWindow::Draw()

void MenuWindow::SetDimensions(float w, float h)
{
    if(w <= 0.0f) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid width argument: " << w << std::endl;
        return;
    }

    if(h <= 0.0f) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid height argument: " << h << std::endl;
        return;
    }

    _width = w;
    _height = h;
    _RecreateImage();
}



void MenuWindow::SetMenuSkin(const std::string &skin_name)
{
    MenuSkin *new_skin = GUIManager->_GetMenuSkin(skin_name);
    if(new_skin == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "the skin_name \"" << skin_name << "\" was invalid" << std::endl;
        return;
    }

    _skin = new_skin;
    _RecreateImage();
}

bool MenuWindow::_RecreateImage()
{
    if(_skin == NULL) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "no menu skin set when function was invoked" << std::endl;
        return false;
    }

    _menu_image.Clear();

    // Get information about the border sizes
    float left_border_size   = _skin->borders[1][0].GetWidth();
    float right_border_size  = _skin->borders[1][2].GetWidth();
    float top_border_size    = _skin->borders[2][1].GetHeight();
    float bottom_border_size = _skin->borders[0][1].GetHeight();

    float horizontal_border_size = left_border_size + right_border_size;
    float vertical_border_size   = top_border_size  + bottom_border_size;

    float top_width   = _skin->borders[2][1].GetWidth();
    float left_height = _skin->borders[1][0].GetHeight();

    // Calculate how many times the top/bottom images have to be tiled in order to make up the width of the window
    _inner_width = _width - horizontal_border_size;
    _inner_height = _height - vertical_border_size;

    if(_inner_width < 0.0f) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "_inner_width was computed as negative" << std::endl;
        return false;
    }

    if(_inner_height < 0.0f) {
        IF_PRINT_WARNING(VIDEO_DEBUG) << "_inner_height was computed as negative" << std::endl;
        return false;
    }

    // Will be true if there is a background image for the menu skin being used
    bool background_loaded = _skin->background.GetWidth();

    // Find how many times we have to tile the border images to fit the dimensions given
    float num_x_tiles = _inner_width  / top_width;
    float num_y_tiles = _inner_height / left_height;

    int32 inum_x_tiles = static_cast<int32>(num_x_tiles);
    int32 inum_y_tiles = static_cast<int32>(num_y_tiles);

    // Ideally, the border sizes should evenly divide into the window sizes but the person who
    // created the window might have passed in a width and height that is not a multiple of
    // the border sizes. If this is the case, we have to extend the dimensions a little bit.
    float dnum_x_tiles = num_x_tiles - inum_x_tiles;
    float dnum_y_tiles = num_y_tiles - inum_y_tiles;

    if(dnum_x_tiles > 0.001f) {
        float width_adjust = (1.0f - dnum_x_tiles) * top_width;
        _width += width_adjust;
        _inner_width += width_adjust;
        ++inum_x_tiles;
    }

    if(dnum_y_tiles > 0.001f) {
        float height_adjust = (1.0f - dnum_y_tiles) * top_width;
        _height += height_adjust;
        _inner_height += height_adjust;
        ++inum_y_tiles;
    }

    // Now we have all the information we need to create the window.
    // Begin by re-creating the overlay at the correct width and height
    Color c[4];
    _skin->borders[1][1].GetVertexColor(c[0], 0);
    _skin->borders[1][1].GetVertexColor(c[1], 1);
    _skin->borders[1][1].GetVertexColor(c[2], 2);
    _skin->borders[1][1].GetVertexColor(c[3], 3);

    _skin->borders[1][1].SetDimensions(left_border_size, top_border_size);
    _skin->borders[1][1].SetVertexColors(c[0], c[1], c[2], c[3]);

    // If a valid background image is loaded, then tile the interior of the window with it
    if(background_loaded) {
        _skin->background.SetVertexColors(c[0], c[1], c[2], c[3]);

        float width = _skin->background.GetWidth();
        float height = _skin->background.GetHeight();

        float min_x = 0;
        float min_y = 0;

        float max_x = _inner_width + horizontal_border_size;
        float max_y = _inner_height + vertical_border_size;

        if(_edge_visible_flags & VIDEO_MENU_EDGE_TOP)
            max_y -= (top_border_size / 2);
        if(_edge_visible_flags & VIDEO_MENU_EDGE_BOTTOM)
            min_y += (bottom_border_size / 2);
        if(_edge_visible_flags & VIDEO_MENU_EDGE_LEFT)
            min_x += (left_border_size / 2);
        if(_edge_visible_flags & VIDEO_MENU_EDGE_RIGHT)
            max_x -= (right_border_size / 2);

        for(float y = min_y ; y < max_y; y += height) {
            for(float x = min_x; x < max_x; x += width) {
                float u = 1.0, v = 1.0;

                if(x + width > max_x)
                    u = (max_x - x) / width;
                if(y + height > max_y)
                    v = (max_y - y) / height;

                _menu_image.AddImage(_skin->background, x, y, 0.0f, 0.0f, u, v);
            }
        }
    } else {
        // Otherwise re-create the overlay at the correct width and height
        _skin->borders[1][1].SetDimensions(_inner_width, _inner_height);
        _skin->borders[1][1].SetVertexColors(c[0], c[1], c[2], c[3]);
        _menu_image.AddImage(_skin->borders[1][1], left_border_size, bottom_border_size);
    }

    // First create the corners of the image
    float max_x = left_border_size + inum_x_tiles * top_width;
    float max_y = bottom_border_size + inum_y_tiles * left_height;
    float min_x = 0.0f;
    float min_y = 0.0f;

    // Bottom left
    if(_edge_visible_flags & VIDEO_MENU_EDGE_LEFT && _edge_visible_flags & VIDEO_MENU_EDGE_BOTTOM) {
        if(_edge_shared_flags & VIDEO_MENU_EDGE_LEFT && _edge_shared_flags & VIDEO_MENU_EDGE_BOTTOM)
            _menu_image.AddImage(_skin->connectors[4], min_x, min_y);
        else if(_edge_shared_flags & VIDEO_MENU_EDGE_LEFT)
            _menu_image.AddImage(_skin->connectors[1], min_x, min_y);
        else if(_edge_shared_flags & VIDEO_MENU_EDGE_BOTTOM)
            _menu_image.AddImage(_skin->connectors[2], min_x, min_y);
        else
            _menu_image.AddImage(_skin->borders[2][0], min_x, min_y);
    } else if(_edge_visible_flags & VIDEO_MENU_EDGE_LEFT)
        _menu_image.AddImage(_skin->borders[1][0], min_x, min_y);
    else if(_edge_visible_flags & VIDEO_MENU_EDGE_BOTTOM)
        _menu_image.AddImage(_skin->borders[0][1], min_x, min_y);
    else if(!background_loaded)
        _menu_image.AddImage(_skin->borders[1][1], min_x, min_y);

    // Bottom right
    if(_edge_visible_flags & VIDEO_MENU_EDGE_RIGHT && _edge_visible_flags & VIDEO_MENU_EDGE_BOTTOM) {
        if(_edge_shared_flags & VIDEO_MENU_EDGE_RIGHT && _edge_shared_flags & VIDEO_MENU_EDGE_BOTTOM)
            _menu_image.AddImage(_skin->connectors[4], max_x, min_y);
        else if(_edge_shared_flags & VIDEO_MENU_EDGE_RIGHT)
            _menu_image.AddImage(_skin->connectors[1], max_x, min_y);
        else if(_edge_shared_flags & VIDEO_MENU_EDGE_BOTTOM)
            _menu_image.AddImage(_skin->connectors[3], max_x, min_y);
        else
            _menu_image.AddImage(_skin->borders[2][2], max_x, min_y);
    } else if(_edge_visible_flags & VIDEO_MENU_EDGE_RIGHT)
        _menu_image.AddImage(_skin->borders[1][2], max_x, min_y);
    else if(_edge_visible_flags & VIDEO_MENU_EDGE_BOTTOM)
        _menu_image.AddImage(_skin->borders[2][1], max_x, min_y);
    else if(!background_loaded)
        _menu_image.AddImage(_skin->borders[1][1], max_x, min_y);

    // Top left
    if(_edge_visible_flags & VIDEO_MENU_EDGE_LEFT && _edge_visible_flags & VIDEO_MENU_EDGE_TOP) {
        if(_edge_shared_flags & VIDEO_MENU_EDGE_LEFT && _edge_shared_flags & VIDEO_MENU_EDGE_TOP)
            _menu_image.AddImage(_skin->connectors[4], min_x, max_y);
        else if(_edge_shared_flags & VIDEO_MENU_EDGE_LEFT)
            _menu_image.AddImage(_skin->connectors[0], min_x, max_y);
        else if(_edge_shared_flags & VIDEO_MENU_EDGE_TOP)
            _menu_image.AddImage(_skin->connectors[2], min_x, max_y);
        else
            _menu_image.AddImage(_skin->borders[0][0], min_x, max_y);
    } else if(_edge_visible_flags & VIDEO_MENU_EDGE_LEFT)
        _menu_image.AddImage(_skin->borders[1][0], min_x, max_y);
    else if(_edge_visible_flags & VIDEO_MENU_EDGE_TOP)
        _menu_image.AddImage(_skin->borders[0][1], min_x, max_y);
    else if(!background_loaded)
        _menu_image.AddImage(_skin->borders[1][1], min_x, max_y);

    // Top right
    if(_edge_visible_flags & VIDEO_MENU_EDGE_TOP && _edge_visible_flags & VIDEO_MENU_EDGE_RIGHT) {
        if(_edge_shared_flags & VIDEO_MENU_EDGE_RIGHT && _edge_shared_flags & VIDEO_MENU_EDGE_TOP)
            _menu_image.AddImage(_skin->connectors[4], max_x, max_y);
        else if(_edge_shared_flags & VIDEO_MENU_EDGE_RIGHT)
            _menu_image.AddImage(_skin->connectors[0], max_x, max_y);
        else if(_edge_shared_flags & VIDEO_MENU_EDGE_TOP)
            _menu_image.AddImage(_skin->connectors[3], max_x, max_y);
        else
            _menu_image.AddImage(_skin->borders[0][2], max_x, max_y);
    } else if(_edge_visible_flags & VIDEO_MENU_EDGE_TOP)
        _menu_image.AddImage(_skin->borders[0][1], max_x, max_y);
    else if(_edge_visible_flags & VIDEO_MENU_EDGE_RIGHT)
        _menu_image.AddImage(_skin->borders[1][2], max_x, max_y);
    else if(!background_loaded)
        _menu_image.AddImage(_skin->borders[1][1], max_x, max_y);

    // Iterate from left to right and fill in the horizontal borders
    for(int32 tile_x = 0; tile_x < inum_x_tiles; ++tile_x) {
        if(_edge_visible_flags & VIDEO_MENU_EDGE_TOP)
            _menu_image.AddImage(_skin->borders[0][1], left_border_size + top_width * tile_x, max_y);
        else if(!background_loaded)
            _menu_image.AddImage(_skin->borders[1][1], left_border_size + top_width * tile_x, max_y);

        if(_edge_visible_flags & VIDEO_MENU_EDGE_BOTTOM)
            _menu_image.AddImage(_skin->borders[2][1], left_border_size + top_width * tile_x, 0.0f);
        else if(!background_loaded)
            _menu_image.AddImage(_skin->borders[1][1], left_border_size + top_width * tile_x, 0.0f);
    }

    // Iterate from bottom to top and fill in the vertical borders
    for(int32 tile_y = 0; tile_y < inum_y_tiles; ++tile_y) {
        if(_edge_visible_flags & VIDEO_MENU_EDGE_LEFT)
            _menu_image.AddImage(_skin->borders[1][0], 0.0f, bottom_border_size + left_height * tile_y);
        else if(!background_loaded)
            _menu_image.AddImage(_skin->borders[1][1], 0.0f, bottom_border_size + left_height * tile_y);

        if(_edge_visible_flags & VIDEO_MENU_EDGE_RIGHT)
            _menu_image.AddImage(_skin->borders[1][2], max_x, bottom_border_size + left_height * tile_y);
        else if(!background_loaded)
            _menu_image.AddImage(_skin->borders[1][1], max_x, bottom_border_size + left_height * tile_y);
    }

    return true;
}

}  // namespace vt_gui
