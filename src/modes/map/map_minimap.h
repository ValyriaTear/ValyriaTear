////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2013-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
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

// Forward declerations.
namespace vt_gui
{
    class MenuWindow;
} // vt_gui

namespace vt_map
{
namespace private_map
{

class ObjectSupervisor;
class VirtualSprite;

//! \brief Handles the Collision minimap generation, caching, drawing and updating the minimap
class Minimap {
public:
    /** \brief constructor creating the minimap image.
    *** \param minimap_image_filename filename of a pre-made minimap image.
    *** If empty, the minimap is generated using the map script collision map.
    **/
    Minimap(const std::string& minimap_image_filename = std::string());

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

    //! \brief objects for the "window" which will hold the map
    //! \note we plan to move this to a Controller object, or something similar
    //! that is a single instance held by the map itself
    vt_video::StillImage _background;

    //! \brief the location sprite
    vt_video::AnimatedImage _location_marker;

    //! \brief the current map locations
    vt_common::Position2D _current_position;

    //! \brief the current box length for this collision map
    uint32_t _box_x_length;
    uint32_t _box_y_length;

    //! \brief map offset information
    vt_common::Position2D _center_pos;
    vt_common::Position2D _half_len;

    //! \brief grid height and width
    uint32_t _grid_width;
    uint32_t _grid_height;

    //! \brief opacities for when the character is under the map location
    const vt_video::Color* _current_opacity;

    //! \brief specifies the additive alpha we get from the map class
    float _map_alpha_scale;

    //! \brief creates the procedural collision minimap image
    vt_video::StillImage _CreateProcedurally();

#ifdef DEBUG_FEATURES
    //! \brief Writes a XPM file with the minimap equivalient in it.
    //! It is used to easily have a base to create nicer minimaps.
    void _DEV_CreateXPMFromCollisionMap(const std::string& output_file);
#endif
};

} // private_map

} // vt_map

#endif // __MAP_MINIMAP_HEADER__
