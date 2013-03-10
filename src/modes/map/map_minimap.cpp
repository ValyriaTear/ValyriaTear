////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2013 by Bertram (Valyria Tear)
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

#include "modes/map/map_minimap.h"

#include "modes/map/map_objects.h"
#include "modes/map/map_sprites.h"

#include "engine/video/video.h"
#include "common/gui/menu_window.h"

#include <algorithm>
#include <SDL_image.h>

namespace hoa_map
{

namespace private_map
{

//! \brief allows for scoped-based control of SDL Surfaces
struct SDLSurfaceController {
    SDL_Surface *_surface;
    inline SDLSurfaceController(const char *str) :
        _surface(IMG_Load(str))
    {
        if(!_surface)
            PRINT_ERROR << "Couldn't create white_noise image for collision map: " << SDL_GetError() << std::endl;
    }

    ~SDLSurfaceController()
    {
        if(_surface)
            SDL_FreeSurface(_surface);
    }
};

static hoa_video::Color default_opacity = hoa_video::Color(1.0f, 1.0f, 1.0f, 0.75f);
static hoa_video::Color overlap_opacity = hoa_video::Color(1.0f, 1.0f, 1.0f, 0.65f);

Minimap::Minimap(ObjectSupervisor *map_object_supervisor, const std::string &map_name) :
    _current_position_x(-1),
    _current_position_y(-1),
    _box_x_length(20.0f),
    _box_y_length(_box_x_length * .75f),
    _x_offset(0),
    _y_offset(0),
    _x_half_len(1.75f * TILES_ON_X_AXIS * _box_x_length),
    _y_half_len(1.75f * TILES_ON_Y_AXIS * _box_y_length),
    _map_alpha_scale(1.0f)
{
    if(!map_object_supervisor)
    {
        PRINT_ERROR << "map object supervisor is not instantiated" << std::endl;
        exit(EXIT_FAILURE);
    }

    //save the viewport
    hoa_video::VideoManager->GetCurrentViewport(_viewport_original_x, _viewport_original_y,
                                                _viewport_original_width, _viewport_original_height);

    //create a temporary SDL surface on which to generate the collision map
    SDL_Surface *temp_surface = _ProcedurallyDraw(map_object_supervisor);

    SDL_LockSurface(temp_surface);
    //setup a temporary memory space to copy the SDL data into
    hoa_video::private_video::ImageMemory temp_data;
    temp_data.rgb_format = false;
    temp_data.width = temp_surface->w;
    temp_data.height = temp_surface->h;
    size_t len = temp_surface->h * temp_surface->w * ((unsigned short) (temp_surface->format->BitsPerPixel) / 8) ;
    temp_data.pixels = malloc(len);
    //copy the data
    memcpy(temp_data.pixels, temp_surface->pixels, len);
    SDL_UnlockSurface(temp_surface);
    //at this point, the screen data is set up. We can get rid of unncesary data here
    SDL_FreeSurface(temp_surface);
    //do the image file creationg
    std::string map_name_cmap = map_name + "_cmap";
    _minimap_image = hoa_video::VideoManager->CreateImage(&temp_data, map_name_cmap);
    free(temp_data.pixels);

    //setup the map window, if it isn't already created
    _background.Load("img/menus/minimap_background.png");
    _background.SetStatic(true);
    _background.SetHeight(173.0f);
    _background.SetWidth(235.0f);

    //load the location market
    if(!_location_marker.LoadFromAnimationScript("img/menus/minimap_arrows.lua"))
        PRINT_ERROR << "Could not load marker image!" << std::endl;
    _location_marker.SetWidth(_box_x_length * 5);
    _location_marker.SetHeight(_box_y_length * 5);
    _location_marker.SetFrameIndex(0);
}

static inline void _PrepareSurface(SDL_Surface *temp_surface)
{
    static SDLSurfaceController white_noise("img/menus/minimap_collision.png");
    SDL_Rect r;
    r.x = r.y = 0;

    //prepare the surface with the image. we tile the white noise image onto the surface
    //with full alpha
    for(int x = 0; x < temp_surface->w; x += white_noise._surface->w)
    {
        r.x = x;
        for(int y = 0; y < temp_surface->h; y += white_noise._surface->h)
        {
            r.y = y;
            if(SDL_BlitSurface(white_noise._surface, NULL, temp_surface, &r))
            {
                PRINT_ERROR << "Couldnt fill a rect on temp_surface: " << SDL_GetError() << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
}

SDL_Surface *Minimap::_ProcedurallyDraw(ObjectSupervisor *map_object_supervisor)
{

    float x, y, width, height;
    hoa_video::VideoManager->GetCurrentViewport(x, y, width, height);

    map_object_supervisor->GetGridAxis(_grid_width, _grid_height);

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
        exit(EXIT_FAILURE);
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
    _PrepareSurface(temp_surface);

    for(uint32 row = 0; row < _grid_width; ++row)
    {
        r.y = 0;
        for(uint32 col = 0; col < _grid_height; ++col)
        {
            if(!map_object_supervisor->IsStaticCollision(row, col))
            {

                if(SDL_FillRect(temp_surface, &r, SDL_MapRGBA(temp_surface->format, 0x00, 0x00, 0x00, 0x00)))
                {
                    PRINT_ERROR << "Couldnt fill a rect on temp_surface: " << SDL_GetError() << std::endl;
                    exit(EXIT_FAILURE);
                }

            }
            r.y += _box_y_length;
        }
        r.x += _box_x_length;
    }

    //flush the SDL surface. This forces any pending writes onto the surface to complete
    SDL_UpdateRect(temp_surface, 0, 0, 0, 0);
    return temp_surface;
}

void Minimap::Draw()
{
    using namespace hoa_video;
    if(_current_position_x > -1)
    {
        Color resultant_opacity = *_current_opacity;
        if(_map_alpha_scale < resultant_opacity.GetAlpha())
            resultant_opacity.SetAlpha(_map_alpha_scale);
        //save the current video manager state
        VideoManager->PushState();
        //set the new coordinates to match our viewport
        VideoManager->SetStandardCoordSys();
        //draw the background in the current viewport and coordinate space
        VideoManager->Move(775, 545);
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);
        _background.Draw(resultant_opacity);
        //assign the viewport to be "inside" the above area
        VideoManager->SetViewport(610, 42, _viewport_width, _viewport_height);
        //scale and translate the orthographic projection such that it "centers" on our calculated positions
        VideoManager->SetCoordSys(_x_cent - _x_half_len, _x_cent + _x_half_len, _y_cent + _y_half_len, _y_cent - _y_half_len);

        float x_location = _current_position_x * _box_x_length - _location_marker.GetWidth() / 2.0f;
        float y_location = _current_position_y * _box_y_length - _location_marker.GetHeight()/ 2.0f;

        VideoManager->Move(0, 0);
        //adjust the currnet opacity for the map scale

        _minimap_image.Draw(resultant_opacity + Color(0.0f, 0.0f, 0.0f, -0.15f));

        VideoManager->Move(x_location, y_location);
        _location_marker.Draw(resultant_opacity);

        VideoManager->PopState();

        //remember kids: please be kind and rewind!
        VideoManager->SetViewport(_viewport_original_x, _viewport_original_y,
                                  _viewport_original_width, _viewport_original_height);
    }
}

void Minimap::Update(VirtualSprite *camera, float map_alpha_scale)
{
    //in case the camera isn't specified, we don't do anything
    if(!camera)
        return;

    _map_alpha_scale = map_alpha_scale;

    //get the collision-map transformed location of the camera
    _current_position_x = camera->GetPosition().x;
    _current_position_y = camera->GetPosition().y;
    _x_cent = _box_x_length * _current_position_x;
    _y_cent = _box_y_length * _current_position_y;

    //update the opacity based on the camera location.
    //we decrease the opacity if it is in the region covered by the collision map
    if((_minimap_image.GetWidth() - _x_cent) <= 180.0f &&
            (_minimap_image.GetHeight() - _y_cent) <= 115.0f)
        _current_opacity = &overlap_opacity;
    else
        _current_opacity = &default_opacity;

    //update the orthographic projection information based on the camera location
    //we "lock" the minimap so that if it is against an edge of the map the orthographic
    //projection doesn't roll over the edge.
    if(_x_cent - _x_half_len < 0)
        _x_cent = _x_half_len;
    if(_x_cent + _x_half_len > _grid_width * _box_x_length)
        _x_cent = _grid_width * _box_x_length - _x_half_len;

    if(_y_cent - _y_half_len < 0)
        _y_cent = _y_half_len;
    if(_y_cent + _y_half_len > _grid_height * _box_y_length)
        _y_cent = _grid_height * _box_y_length - _y_half_len;

    //set the indicator frame based on what direction the camera is moving
    switch(camera->GetDirection())
    {
        case 0x1:
            _location_marker.SetFrameIndex(0);
        break;
        case 0x8:
            _location_marker.SetFrameIndex(3);
        break;
        case 0x2:
            _location_marker.SetFrameIndex(2);
        break;
        case 0x4:
            _location_marker.SetFrameIndex(1);
        break;
        default:
        break;
    };

}

}

}
