////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** *********************************************************************************
*** \file map_minimap.h
*** \author Nik N (IkarusDowned) nihonnik@gmail.com
*** \brief Header file for mini maps
***
*** This file contains the interface for the various supported minimaps in the game.
*** This includes the actual minimap image itself, along with any effect paramenters
*** that are specified for the map / toggled through game logic
*** ********************************************************************************/

#ifndef __MAP_MINIMAP_HEADER__
#define __MAP_MINIMAP_HEADER__

#include "engine/video/image.h"

#include <string>

//forward declerations
namespace vt_gui
{
    class MenuWindow;
}

namespace vt_map
{
namespace private_map
{

class ObjectSupervisor;
class VirtualSprite;

//! \brief Handles the Collision minimap generation, caching, drawing and updating the minimap
class Minimap {
public:
    /** \brief constructor taking the target map mode. This also creates the actual collision map
    *** Currently, there is only one global map supervisor instance, so technically I could call the static GetInstance function.
    *** However, I want the target map mode to be sent in explicitly. This will eliminate some of the confusion
    *** that can sometimes occur when having multiple targets and singletons -- IE temporal allocation is reduced since
    *** we explicitly force the pointer to be sent in
    *** \param map_object_supervisor the target map object supervisor
    *** \param map_name name of the actual map we are generating for
    **/
    Minimap(ObjectSupervisor *map_object_supervisor, const std::string &map_name);

    Minimap() {}

    ~Minimap() {
        _minimap_image.Clear();
        _location_marker.Clear();
    }

    /** updates the map with effect changes and player location information
    *** \param camera a VirtualSprite indicating the camera location
    *** \param the scaled alpha amount from the map
    **/
    void Update(VirtualSprite *camera, float map_alpha_scale = 1.0f);

    /** \brief draws the collision map along with the location cursor
    *** and any effects we might have taking place on the collision map
    **/
    void Draw();

private:
    //! \brief the generated collision map image for this collision map
    vt_video::StillImage _minimap_image;

    //! \brief creates the procedural collision map
    SDL_Surface *_ProcedurallyDraw(ObjectSupervisor *map_object_supervisor);

    //! \brief objects for the "window" which will hold the map
    //! \note we plan to move this to a Controller object, or something similar
    //! that is a single instance held by the map itself
    vt_video::StillImage _background;

    //! \brief the current map locations
    float _current_position_x;
    float _current_position_y;

    //! \brief the current box length for this collision map
    uint32 _box_x_length;
    uint32 _box_y_length;

    //! \brief the location sprite
    vt_video::AnimatedImage _location_marker;

    //! \brief the original viewport information
    float _viewport_original_x;
    float _viewport_original_y;
    float _viewport_original_width;
    float _viewport_original_height;

    //! \brief modified viewport information
    float _viewport_x;
    float _viewport_y;
    float _viewport_width;
    float _viewport_height;

    //! \brief map offset information
    float _x_offset, _y_offset;
    float _x_cent, _y_cent;
    float _x_half_len, _y_half_len;

    //! \brief grid height and width
    uint32 _grid_width;
    uint32 _grid_height;

    //! \brief opacities for when the character is under the map location
    const vt_video::Color *_current_opacity;

    //! \brief specifies the additive alpha we get from the map class
    float _map_alpha_scale;
};

}
}
#endif // __MAP_MINIMAP_HEADER__
