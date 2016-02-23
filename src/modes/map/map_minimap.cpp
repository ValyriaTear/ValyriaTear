////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2013-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    minimap.cpp
*** \author  TNik N (IkarusDowned) nihonnik@gmail.com
*** \brief   Source file for the minimaps.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/map/map_minimap.h"

#include "common/gui/menu_window.h"

#include "engine/video/video.h"

#include "modes/map/map_objects.h"
#include "modes/map/map_sprites.h"
#include "modes/map/map_utils.h"

// Used for the collision to XPM dev function.
#ifdef DEBUG_FEATURES
    #include "engine/script/script_write.h"
#endif

namespace vt_map
{

namespace private_map
{

//! \brief The default opcaity.
const vt_video::Color DEFAULT_OPACITY(1.0f, 1.0f, 1.0f, 0.75f);

//! \brief The overlap opcaity.
const vt_video::Color OVERLAP_OPACITY(1.0f, 1.0f, 1.0f, 0.45f);

//! \brief The X value for the minimap's position.
const float MINIMAP_POS_X = 775.0f;

//! \brief The Y value for the minimap's position.
const float MINIMAP_POS_Y = 545.0f;

//! \brief Allows for scoped-based control of SDL Surfaces.
struct SDLSurfaceController {
    SDL_Surface* _surface;

    explicit SDLSurfaceController(const char *str) :
        _surface(IMG_Load(str))
    {
        if (!_surface)
            PRINT_ERROR << "Couldn't create the white noise image for the collision map: " << SDL_GetError() << std::endl;
    }

    ~SDLSurfaceController()
    {
        if (_surface)
            SDL_FreeSurface(_surface);
    }

private:
    //
    // The copy constructor and assignment operator are hidden by design
    // to cause compilation errors when attempting to copy or assign this class.
    //

    SDLSurfaceController(const SDLSurfaceController&)
    {
        throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
    }

    SDLSurfaceController& operator=(const SDLSurfaceController&)
    {
        throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
        return *this;
    }
};

//! \brief A helper function to prepare a SDL_Surface.
static bool _PrepareSurface(SDL_Surface* temp_surface)
{
    // A white noise texture.
    const SDLSurfaceController WHITE_NOISE("data/gui/map/minimap_collision.png");
    SDL_Rect r = { 0, 0, 0, 0 };

    // Prepare the surface with the image.  Tile the white noise image onto the surface with full alpha.
    for(int x = 0; x < temp_surface->w; x += WHITE_NOISE._surface->w) {
        r.x = x;
        for(int y = 0; y < temp_surface->h; y += WHITE_NOISE._surface->h) {
            r.y = y;
            if(SDL_BlitSurface(WHITE_NOISE._surface, nullptr, temp_surface, &r)) {
                PRINT_ERROR << "Couldn't fill a rect on temp_surface: " << SDL_GetError() << std::endl;
                return false;
            }
        }
    }

    return true;
}

Minimap::Minimap(const std::string& minimap_image_filename) :
    _current_position_x(-1.0f),
    _current_position_y(-1.0f),
    _box_x_length(10),
    _box_y_length(static_cast<uint16_t>(_box_x_length * 0.75f)),
    _x_cent(0.0f),
    _y_cent(0.0f),
    _x_half_len(1.75f * TILES_ON_X_AXIS * _box_x_length / vt_map::private_map::MAP_ZOOM_RATIO),
    _y_half_len(1.75f * TILES_ON_Y_AXIS * _box_y_length / vt_map::private_map::MAP_ZOOM_RATIO),
    _grid_width(0),
    _grid_height(0),
    _current_opacity(nullptr),
    _map_alpha_scale(1.0f)
{
    ObjectSupervisor *map_object_supervisor = MapMode::CurrentInstance()->GetObjectSupervisor();
    map_object_supervisor->GetGridAxis(_grid_width, _grid_height);

    // If no minimap image is given, we create one.
    if (minimap_image_filename.empty() ||
            !_minimap_image.Load(minimap_image_filename, _grid_width * _box_x_length, _grid_height * _box_y_length)) {
        _minimap_image = _CreateProcedurally();
    }

    //setup the map window, if it isn't already created
    _background.Load("data/gui/map/minimap_background.png");
    _background.SetStatic(true);
    _background.SetHeight(173.0f);
    _background.SetWidth(235.0f);

    //load the location market
    if(!_location_marker.LoadFromAnimationScript("data/gui/map/minimap_arrows.lua"))
        PRINT_ERROR << "Could not load marker image!" << std::endl;
    _location_marker.SetWidth(_box_x_length * 5);
    _location_marker.SetHeight(_box_y_length * 5);
    _location_marker.SetFrameIndex(0);
}

vt_video::StillImage Minimap::_CreateProcedurally()
{
    ObjectSupervisor *map_object_supervisor = MapMode::CurrentInstance()->GetObjectSupervisor();

    float x, y, width, height;
    vt_video::VideoManager->GetCurrentViewport(x, y, width, height);

    //create a new SDL surface that is the rendering dimensions we want.
    SDL_Surface *temp_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                                     _grid_width * _box_x_length, _grid_height * _box_y_length, 32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
                                                     0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
                                                     0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

    if(!temp_surface) {
        PRINT_ERROR << "Couldn't create temp_surface for collision map: " << SDL_GetError() << std::endl;
        MapMode::CurrentInstance()->ShowMinimap(false);
        return vt_video::StillImage();
    }

    //set the basic rect information
    SDL_Rect r;
    r.w = _box_x_length;
    r.h = _box_y_length;
    r.x = r.y = 0;

    //very simple. Just go through the entire grid, checking the visible-character's
    //context against the map-grid's collision context. if this is NOT a colidable location
    //fill that square in with a full alpha block
    //note that the ordering needs to be transposed for drawing
    if (!_PrepareSurface(temp_surface)) {
        SDL_FreeSurface(temp_surface);
        MapMode::CurrentInstance()->ShowMinimap(false);
        return vt_video::StillImage();
    }

    for(uint32_t row = 0; row < _grid_width; ++row)
    {
        r.y = 0;
        for(uint32_t col = 0; col < _grid_height; ++col)
        {
            if(!map_object_supervisor->IsStaticCollision(row, col))
            {

                if(SDL_FillRect(temp_surface, &r, SDL_MapRGBA(temp_surface->format, 0x00, 0x00, 0x00, 0x00)))
                {
                    PRINT_ERROR << "Couldn't fill a rect on temp_surface: " << SDL_GetError() << std::endl;
                    SDL_FreeSurface(temp_surface);
                    return vt_video::StillImage();
                }

            }
            r.y += _box_y_length;
        }
        r.x += _box_x_length;
    }

    //TEST: flush the SDL surface. This forces any pending writes onto the surface to complete
    //SDL_UpdateRect(temp_surface, 0, 0, 0, 0);

    if (!temp_surface) {
        MapMode::CurrentInstance()->ShowMinimap(false);
        return vt_video::StillImage();
    }

    SDL_LockSurface(temp_surface);

    // Setup a temporary memory space to copy the SDL data into
    vt_video::private_video::ImageMemory temp_data(temp_surface);

    SDL_UnlockSurface(temp_surface);
    // At this point, the screen data is set up. We can get rid of unnecessary data here
    SDL_FreeSurface(temp_surface);
    // Do the image file creation
    std::string map_name_cmap = MapMode::CurrentInstance()->GetMapScriptFilename() + "_cmap";
    vt_video::StillImage minimap_image = vt_video::VideoManager->CreateImage(&temp_data, map_name_cmap);

#ifdef DEBUG_FEATURES
    // Uncomment and compile this to generate XPM minimaps.
    //_DEV_CreateXPMFromCollisionMap(map_name_cmap + ".xpm");
#endif

    return minimap_image;
}

void Minimap::Draw()
{
    if (_current_position_x <= -1.0f)
        return;

    vt_video::Color resultant_opacity = *_current_opacity;
    if (_map_alpha_scale < resultant_opacity.GetAlpha())
        resultant_opacity.SetAlpha(_map_alpha_scale);

    // Save the current video manager state.
    vt_video::VideoManager->PushState();
    // Set the new coordinates to match our viewport.
    vt_video::VideoManager->SetStandardCoordSys();

    // Draw the background in the current viewport and coordinate space
    vt_video::VideoManager->Move(MINIMAP_POS_X, MINIMAP_POS_Y);
    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_TOP, 0);
    _background.Draw(resultant_opacity);

    // Store the current viewport.
    float viewport_original_x = 0.0f;
    float viewport_original_y = 0.0f;
    float viewport_original_width = 0.0f;
    float viewport_original_height = 0.0f;
    vt_video::VideoManager->GetCurrentViewport(viewport_original_x, viewport_original_y,
                                               viewport_original_width, viewport_original_height);

    // Compute the minimap's viewport.
    const float ratio_x = vt_video::VideoManager->GetViewportWidth() / 800.0f;
    const float ratio_y = vt_video::VideoManager->GetViewportHeight() / 600.0f;
    float viewport_x = (610.0f * ratio_x) + vt_video::VideoManager->GetViewportXOffset();
    float viewport_y = (42.0f * ratio_y) + vt_video::VideoManager->GetViewportYOffset();
    float viewport_width = 175.0f * ratio_x;
    float viewport_height = 128.0f * ratio_y;

    // Update the scissor rectangle.
    vt_video::VideoManager->EnableScissoring();
    vt_video::VideoManager->SetScissorRect(viewport_x, viewport_y, viewport_width, viewport_height);

    // Assign the viewport to be "inside" the above area.
    vt_video::VideoManager->SetViewport(viewport_x, viewport_y, viewport_width, viewport_height);

    // Scale and translate the orthographic projection such that it "centers" on our calculated positions.
    vt_video::VideoManager->SetCoordSys(_x_cent - _x_half_len, _x_cent + _x_half_len, _y_cent + _y_half_len, _y_cent - _y_half_len);

    float x_location = _current_position_x * _box_x_length - _location_marker.GetWidth() / 2.0f;
    float y_location = _current_position_y * _box_y_length - _location_marker.GetHeight() / 2.0f;

    vt_video::VideoManager->Move(0, 0);

    // Adjust the current opacity for the map scale.
    _minimap_image.Draw(resultant_opacity);

    vt_video::VideoManager->Move(x_location, y_location);
    _location_marker.Draw(resultant_opacity);

    vt_video::VideoManager->DisableScissoring();

    vt_video::VideoManager->PopState();

    // Reset the original viewport.
    vt_video::VideoManager->SetViewport(viewport_original_x, viewport_original_y,
                                        viewport_original_width, viewport_original_height);
}

void Minimap::Update(VirtualSprite *camera, float map_alpha_scale)
{
    // In case the camera isn't specified, we don't do anything
    if(!camera)
        return;

    MapMode* map_mode = MapMode::CurrentInstance();

    _map_alpha_scale = map_alpha_scale;

    // Get the collision-map transformed location of the camera
    _current_position_x = camera->GetPosition().x;
    _current_position_y = camera->GetPosition().y;
    _x_cent = _box_x_length * _current_position_x;
    _y_cent = _box_y_length * _current_position_y;

    // Update the opacity based on the camera location.
    // We decrease the opacity if it is in the region covered by the collision map
    if(map_mode->GetScreenXCoordinate(_current_position_x) >= MINIMAP_POS_X &&
            map_mode->GetScreenYCoordinate(_current_position_y) >= MINIMAP_POS_Y)
        _current_opacity = &OVERLAP_OPACITY;
    else
        _current_opacity = &DEFAULT_OPACITY;

    // Update the orthographic projection information based on the camera location
    // We "lock" the minimap so that if it is against an edge of the map the orthographic
    // projection doesn't roll over the edge.
    if(_x_cent - _x_half_len < 0)
        _x_cent = _x_half_len;
    if(_x_cent + _x_half_len > _grid_width * _box_x_length)
        _x_cent = _grid_width * _box_x_length - _x_half_len;

    if(_y_cent - _y_half_len < 0)
        _y_cent = _y_half_len;
    if(_y_cent + _y_half_len > _grid_height * _box_y_length)
        _y_cent = _grid_height * _box_y_length - _y_half_len;

    // Set the indicator frame based on what direction the camera is moving
    switch(camera->GetDirection())
    {
        case NORTH:
        case NW_NORTH:
        case NE_NORTH:
            _location_marker.SetFrameIndex(0);
        break;
        case EAST:
        case NE_EAST:
        case SE_EAST:
            _location_marker.SetFrameIndex(3);
        break;
        case SOUTH:
        case SW_SOUTH:
        case SE_SOUTH:
            _location_marker.SetFrameIndex(2);
        break;
        case WEST:
        case NW_WEST:
        case SW_WEST:
            _location_marker.SetFrameIndex(1);
        break;
        default:
        break;
    };

}

#ifdef DEBUG_FEATURES
void Minimap::_DEV_CreateXPMFromCollisionMap(const std::string& output_file)
{
    vt_script::WriteScriptDescriptor xpm_file;
    if(!xpm_file.OpenFile(output_file)) {
        PRINT_ERROR << "Failed to open xpm file: " << output_file << std::endl;
        return;
    }

    ObjectSupervisor *map_object_supervisor = MapMode::CurrentInstance()->GetObjectSupervisor();
    uint32_t grid_width = 0;
    uint32_t grid_height = 0;
    map_object_supervisor->GetGridAxis(grid_width, grid_height);

    xpm_file.WriteLine("/* XPM */");
    xpm_file.WriteLine("static char * minimap_xpm[] = {");
    std::ostringstream text("");
    text << "\"" << grid_width << " " << grid_height << " 2 1\",";
    xpm_file.WriteLine(text.str());
    xpm_file.WriteLine("\"1 c None\",");
    xpm_file.WriteLine("\"0 c #FFFFFF\",");

    for(uint32_t col = 0; col < grid_height; ++col)
    {
        std::ostringstream text("");
        text << "\"";

        for(uint32_t row = 0; row < grid_width; ++row)
        {
            if(map_object_supervisor->IsStaticCollision(row, col))
                text << "1";
            else
                text << "0";
        }

        text << "\",";
        xpm_file.WriteLine(text.str());
    }

    xpm_file.WriteLine("};");

    xpm_file.SaveFile();
    xpm_file.CloseFile();
}
#endif

} // private_map

} // vt_map
