///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_tiles.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for map mode tile management.
***
*** This code encapsulates everything related to tiles and tile management in
*** map mode.
*** ***************************************************************************/

#ifndef __MAP_TILES_HEADER__
#define __MAP_TILES_HEADER__

// Allacrost utilities
#include "defs.h"
#include "utils.h"

// Local map mode headers
#include "map_utils.h"

namespace hoa_map {

namespace private_map {

/** ****************************************************************************
*** \brief Represents a single image tile on the map.
***
*** The images that a tile uses are not stored within this class. This class
*** only holds indices to the container class holding those images. This class
*** also does not contain any information about walkability or the collision grid.
*** That information is maintained in the map object manager.
***
*** \note The reason that tiles do not contain walkability information is that
*** each tile is 32x32 pixels, but walkability is defined on a 16x16 granularity,
*** meaning that there are four "walkable" sections to each tile. Certain code
*** such as pathfinding is more simple if all walkability information is kept in
*** in another form of container.
*** ***************************************************************************/
class MapTile {
public:
	/** \name Tile Layer Indeces
	*** \brief Indeces to the tile image container, mapping the three tile layers.
	*** \note A negative value means that no image is registered to that tile layer.
	**/
	//@{
	int16 lower_layer, middle_layer, upper_layer;
	//@}

	MapTile()
		{ lower_layer = -1; middle_layer = -1; upper_layer = -1; }

	MapTile(int16 lower, int16 middle, int16 upper)
		{ lower_layer = lower; middle_layer = middle; upper_layer = upper; }
}; // class MapTile


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
class TileSupervisor {
	friend class hoa_map::MapMode;

public:
	TileSupervisor();

	~TileSupervisor();

	/** \brief Handles all operations on loading tilesets and tile images from the map data file
	*** \param map_file A reference to the Lua file containing the map data
	*** \param map_instance A pointer to the MapMode object which invoked this function
	*** \note The map file should already be opened with no Lua tables open
	**/
	void Load(hoa_script::ReadScriptDescriptor& map_file, const MapMode* map_instance);

	//! \brief Updates all animated tile images
	void Update();

	/** \brief Draws the various tile layers to the screen
	*** \param frame A pointer to the computed information required to draw this frame
	***
	*** The implementation of these functions are nearly identical except for using
	*** a different layer index to reference the tile image and some minor
	*** differences in draw flags. We do not attempt to apply code reuse to these
	*** functions because we need them to be as fast as possible since they are
	*** each executed for every frame.
	***
	*** \note These functions do not reset the coordinate system and hence require
	*** that the proper coordinate system is already set prior to these function
	*** calls (0.0f, SCREEN_COLS, SCREEN_ROWS, 0.0f). These functions do make
	*** modifications to the blending draw flag and the draw cursor position
	*** which are not restored by the function upon its return, so take measures
	*** to retain this information before calling these functions if necessary.
	**/
	//@{
	void DrawLowerLayer(const MapFrame* const frame);
	void DrawMiddleLayer(const MapFrame* const frame);
	void DrawUpperLayer(const MapFrame* const frame);
	//@}

private:
	/** \brief The number of rows of tiles in the map.
	*** This number must be greater than or equal to 24 for the map to be valid.
	**/
	uint16 _num_tile_rows;

	/** \brief The number of columns of tiles in the map.
	*** This number must be greater than or equal to 32 for the map to be valid.
	**/
	uint16 _num_tile_cols;

	/** \brief A map of 2D vectors that contains all of the map's tile objects.
	*** Each key-value pair in the std::map represents a map context, thus the size of the std::map is equal to
	*** number of contexts in the game map (up to 32). The 2D vector represents the rows and columns of tiles,
	*** respectively, for the given map context.
	**/
	std::map<MAP_CONTEXT, std::vector<std::vector<MapTile> > > _tile_grid;

	//! \brief Contains the image objects for all map tiles, both still and animated.
	std::vector<hoa_video::ImageDescriptor*> _tile_images;

	/** \brief Contains all of the animated tile images used on the map.
	*** The purpose of this vector is to easily update all tile animations without stepping through the
	*** _tile_images vector, which contains both still and animated images.
	**/
	std::vector<hoa_video::AnimatedImage*> _animated_tile_images;
}; // class TileSupervisor

} // namespace private_map

} // namespace hoa_map

#endif // __MAP_TILES_HEADER__
