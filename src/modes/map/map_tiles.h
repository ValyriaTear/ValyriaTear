///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_tiles.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for map mode tile management.
***
*** This code encapsulates everything related to tiles and tile management in
*** map mode.
*** ***************************************************************************/

#ifndef __MAP_TILES_HEADER__
#define __MAP_TILES_HEADER__

#include "modes/map/map_utils.h"

#include "engine/script/script_read.h"

namespace vt_video {
class ImageDescriptor;
class AnimatedImage;
}

namespace vt_map
{

class MapMode;

namespace private_map
{

//! \brief Layer types: Drawn before, along, or after the map objects according to their types.
enum LAYER_TYPE {
    GROUND_LAYER = 0,
    SKY_LAYER = 1,
    INVALID_LAYER = 2
};

class Layer
{
public:
    LAYER_TYPE layer_type;
    // Represents the tile indeces: i.e: tiles[y][x] = tile_id at (x,y)
    std::vector< std::vector<int16_t> > tiles;

    Layer():
        layer_type(GROUND_LAYER)
    {}
};

/** ****************************************************************************
*** \brief A helper class to MapMode responsible for all tile data and operations
***
*** This class is responsible for loading, updating, and drawing all tile images
*** and managing the tile grid. The TileSupervisor does <b>not</b> manage the map
*** collision grid, which is used by map objects and sprites.
***
*** Maps have a minimum size of 24 rows and 32 columns of tiles. Theoretically
*** there is no upper limit on size.
*** ***************************************************************************/
class TileSupervisor
{
    friend class vt_map::MapMode;

public:
    TileSupervisor();

    ~TileSupervisor();

    /** \brief Handles all operations on loading tilesets and tile images from the map data file
    *** \param map_file A reference to the Lua file containing the map data
    *** \note The map file should already be opened with no Lua tables open
    **/
    bool Load(vt_script::ReadScriptDescriptor &map_file);

    //! \brief Updates all animated tile images
    void Update();

    /** \brief Draws the various tile layers to the screen
    *** \param frame A pointer to the computed information required to draw this frame
    ***
    *** \note This function does not reset the coordinate system and hence require
    *** that the proper coordinate system is already set prior to these function
    *** calls (0.0f, SCREEN_COLS, SCREEN_ROWS, 0.0f). These functions do make
    *** modifications to the blending draw flag and the draw cursor position
    *** which are not restored by the function upon its return, so take measures
    *** to retain this information before calling these functions if necessary.
    **/
    //@{
    void DrawLayers(const MapFrame *frame, const LAYER_TYPE &layer_type);
    //@}

private:
    /** \brief The number of columns of tiles in the map.
    *** This number must be greater than or equal to 32 for the map to be valid.
    **/
    uint16_t _num_tile_on_x_axis;

    /** \brief The number of rows of tiles in the map.
    *** This number must be greater than or equal to 24 for the map to be valid.
    **/
    uint16_t _num_tile_on_y_axis;

    //! \brief The map tile layers
    std::vector<Layer> _tile_grid;

    //! \brief Contains the image objects for all map tiles, both still and animated.
    std::vector<vt_video::ImageDescriptor *> _tile_images;

    /** \brief Contains all of the animated tile images used on the map.
    *** The purpose of this vector is to easily update all tile animations without stepping through the
    *** _tile_images vector, which contains both still and animated images.
    **/
    std::vector<vt_video::AnimatedImage *> _animated_tile_images;
}; // class TileSupervisor

} // namespace private_map

} // namespace vt_map

#endif // __MAP_TILES_HEADER__
