///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_utils.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for map mode utility code
*** *****************************************************************************/

#ifndef __MAP_UTILS_HEADER__
#define __MAP_UTILS_HEADER__

// Allacrost utilities
#include "utils.h"
#include "defs.h"

namespace hoa_map {

//! Determines whether the code in the hoa_map namespace should print debug statements or not.
extern bool MAP_DEBUG;

namespace private_map {

/** \name Screen Coordiante System Constants
*** \brief Represents the size of the visible screen in map tiles and the collision grid
*** Every map tile is 32x32 pixels, and every collision grid element is one quarter of that
*** area (16x16). Thus the number of collision grid elements that compose the screen are
*** four times the number of tiles that are visible on the screen. This also means the number
*** of rows and columns of grid elements that encompass the screen are twice that of the
*** number of rows and columns of tiles.
**/
//@{
const float SCREEN_COLS = 32.0f;
const float SCREEN_ROWS = 24.0f;
const float HALF_SCREEN_COLS = SCREEN_COLS / 2;
const float HALF_SCREEN_ROWS = SCREEN_ROWS / 2;

const uint16 TILE_COLS = 16; // Number of tile columns that fit on the screen
const uint16 TILE_ROWS = 12; // Number of tile rows that fit on the screen
const uint16 HALF_TILE_COLS = TILE_COLS / 2;
const uint16 HALF_TILE_ROWS = TILE_ROWS / 2;

const uint16 GRID_LENGTH = 32; // Length of a grid element in pixels
const uint16 TILE_LENGTH = 64; // Length of a tile in pixels
const uint16 HALF_TILE_LENGTH = TILE_LENGTH / 2;
//@}


/** \name Map State Enum
*** \brief Represents the current state of operation during map mode.
**/
//@{
enum MAP_STATE {
	STATE_INVALID          = 0,
	STATE_EXPLORE          = 1, //!< Standard state, player has control to move about the map
	STATE_SCENE            = 2, //!< Like the explore state but player has no control (input is ignored)
	STATE_DIALOGUE         = 3, //!< When a dialogue is active
	STATE_TREASURE         = 4, //!< Active when a treasure has been procured by the player
	STATE_TOTAL            = 5
};
//@}


/** \name Map Context Constants
*** \brief Constants used to represent all 32 possible map contexts
***
*** Note that only one bit is set for each context. This is done so that the collision
*** grid for all contexts can be stored in a single integer. This also simplifies the
*** complexity of collision detection for map sprites.
***
*** \note Ideally these names would omit the "MAP_" affix. However, doing so creates some naming conflicts
*** with 64-bit versions of Windows so we cannot. In Lua though, we bind these names without the "MAP_" since
*** no conflict exists in the Lua environment.
**/
enum MAP_CONTEXT {
	MAP_CONTEXT_NONE  = 0x00000000,
	MAP_CONTEXT_01    = 0x00000001, // Also known as the base context
	MAP_CONTEXT_02    = 0x00000002,
	MAP_CONTEXT_03    = 0x00000004,
	MAP_CONTEXT_04    = 0x00000008,
	MAP_CONTEXT_05    = 0x00000010,
	MAP_CONTEXT_06    = 0x00000020,
	MAP_CONTEXT_07    = 0x00000040,
	MAP_CONTEXT_08    = 0x00000080,
	MAP_CONTEXT_09    = 0x00000100,
	MAP_CONTEXT_10    = 0x00000200,
	MAP_CONTEXT_11    = 0x00000400,
	MAP_CONTEXT_12    = 0x00000800,
	MAP_CONTEXT_13    = 0x00001000,
	MAP_CONTEXT_14    = 0x00002000,
	MAP_CONTEXT_15    = 0x00004000,
	MAP_CONTEXT_16    = 0x00008000,
	MAP_CONTEXT_17    = 0x00010000,
	MAP_CONTEXT_18    = 0x00020000,
	MAP_CONTEXT_19    = 0x00040000,
	MAP_CONTEXT_20    = 0x00080000,
	MAP_CONTEXT_21    = 0x00100000,
	MAP_CONTEXT_22    = 0x00200000,
	MAP_CONTEXT_23    = 0x00400000,
	MAP_CONTEXT_24    = 0x00800000,
	MAP_CONTEXT_25    = 0x01000000,
	MAP_CONTEXT_26    = 0x02000000,
	MAP_CONTEXT_27    = 0x04000000,
	MAP_CONTEXT_28    = 0x08000000,
	MAP_CONTEXT_29    = 0x10000000,
	MAP_CONTEXT_30    = 0x20000000,
	MAP_CONTEXT_31    = 0x40000000,
	MAP_CONTEXT_32    = 0x80000000,
	MAP_CONTEXT_ALL   = 0xFFFFFFFF,
};


/** \name Map Zone Types
*** \brief Identifier types for the various classes of map zones
***
*** \todo This enum is currently not in use by zone classes. Evaluate whether or not such a type identifier enum
*** is necessary and either add them to the zone classes or remove this enum.
**/
enum ZONE_TYPE {
	ZONE_INVALID    = 0,
	ZONE_MAP        = 1,
	ZONE_CAMERA     = 2,
	ZONE_RESIDENT   = 3,
	ZONE_ENEMY      = 4,
	ZONE_CONTEXT    = 5,
	MAP_ZONE_TOTAL  = 6,
};


//! \brief The number of tiles that are found in a tileset image (512x512 pixel image containing 32x32 pixel tiles)
const uint32 TILES_PER_TILESET = 256;


//! \brief Used to identify the type of map object
enum MAP_OBJECT_TYPE {
	PHYSICAL_TYPE = 0,
	VIRTUAL_TYPE = 1,
	SPRITE_TYPE = 2,
	ENEMY_TYPE = 3,
	TREASURE_TYPE = 4
};


/** \name Map Sprite Speeds
*** \brief Common speeds for sprite movement.
*** These values are the time (in milliseconds) that it takes a sprite to walk
*** the distance of one map grid (16 pixels).
**/
//@{
const float VERY_SLOW_SPEED  = 225.0f;
const float SLOW_SPEED       = 190.0f;
const float NORMAL_SPEED     = 150.0f;
const float FAST_SPEED       = 110.0f;
const float VERY_FAST_SPEED  = 75.0f;
//@}


/** \name Sprite Direction Constants
*** \brief Constants used for determining sprite directions
*** Sprites are allowed to travel in eight different directions, however the sprite itself
*** can only be facing one of four ways: north, south, east, or west. Because of this, it
*** is possible to travel, for instance, northwest facing north <i>or</i> northwest facing west.
*** The "NW_NORTH" constant means that the sprite is traveling to the northwest and is
*** facing towards the north.
***
*** \note These constants include a series of shorthands (MOVING_NORTHWEST, FACING_NORTH) used
*** to check for movement and facing directions.
**/
//@{
const uint16 NORTH     = 0x0001;
const uint16 SOUTH     = 0x0002;
const uint16 WEST      = 0x0004;
const uint16 EAST      = 0x0008;
const uint16 NW_NORTH  = 0x0010;
const uint16 NW_WEST   = 0x0020;
const uint16 NE_NORTH  = 0x0040;
const uint16 NE_EAST   = 0x0080;
const uint16 SW_SOUTH  = 0x0100;
const uint16 SW_WEST   = 0x0200;
const uint16 SE_SOUTH  = 0x0400;
const uint16 SE_EAST   = 0x0800;
// Used to check for movement direction regardless of facing direction
const uint16 MOVING_NORTHWARD = NORTH | NW_NORTH | NW_WEST | NE_NORTH | NE_EAST;
const uint16 MOVING_SOUTHWARD = SOUTH | SW_SOUTH | SW_WEST | SE_SOUTH | SE_EAST;
const uint16 MOVING_EASTWARD = EAST | NE_EAST | NE_NORTH | SE_EAST | SE_SOUTH;
const uint16 MOVING_WESTWARD = WEST | NW_WEST | NW_NORTH | SW_WEST | SW_SOUTH;
const uint16 MOVING_NORTHWEST = NW_NORTH | NW_WEST;
const uint16 MOVING_NORTHEAST = NE_NORTH | NE_EAST;
const uint16 MOVING_SOUTHWEST = SW_SOUTH | SW_WEST;
const uint16 MOVING_SOUTHEAST = SE_SOUTH | SE_EAST;
const uint16 MOVING_ORTHOGONALLY = NORTH | SOUTH | EAST | WEST;
const uint16 MOVING_DIAGONALLY = MOVING_NORTHWEST | MOVING_NORTHEAST | MOVING_SOUTHWEST | MOVING_SOUTHEAST;
// Used to check for facing direction regardless of moving direction
const uint16 FACING_NORTH = NORTH | NW_NORTH | NE_NORTH;
const uint16 FACING_SOUTH = SOUTH | SW_SOUTH | SE_SOUTH;
const uint16 FACING_WEST = WEST | NW_WEST | SW_WEST;
const uint16 FACING_EAST = EAST | NE_EAST | SE_EAST;
//@}


/** \name Map Sprite Animation Constants
*** These constants are used to index the MapSprite#animations vector to display the correct
*** animation. The first 8 entries in this vector always represent the same sets of animations
*** for each map sprite. Not all sprites have running animations, so the next 4 entries in the
*** sprite's animation vector are not necessarily running animations.
**/
//@{
const uint32 ANIM_STANDING_SOUTH = 0;
const uint32 ANIM_STANDING_NORTH = 1;
const uint32 ANIM_STANDING_WEST  = 2;
const uint32 ANIM_STANDING_EAST  = 3;
const uint32 ANIM_WALKING_SOUTH  = 4;
const uint32 ANIM_WALKING_NORTH  = 5;
const uint32 ANIM_WALKING_WEST   = 6;
const uint32 ANIM_WALKING_EAST   = 7;
const uint32 ANIM_RUNNING_SOUTH  = 8;
const uint32 ANIM_RUNNING_NORTH  = 9;
const uint32 ANIM_RUNNING_WEST   = 10;
const uint32 ANIM_RUNNING_EAST   = 11;
const uint32 ANIM_ATTACKING_EAST = 12;
//@}


//! \brief Represents the various types of collisions which may occur for a sprite
enum COLLISION_TYPE {
	NO_COLLISION = 0,       //!< Indicates that no collision has occurred
	BOUNDARY_COLLISION = 1, //!< Happens when the sprite attempts to move outside any of the map's boundaries
	GRID_COLLISION = 2,     //!< Condition when the sprite's collision rectangle overlaps an invalid element of the map's collision grid
	OBJECT_COLLISION = 3,   //!< Occurs when the sprite collides with another map object in the same object layer
};


//! \brief Identifiers for the similarly named classes of map events
enum EVENT_TYPE {
	INVALID_EVENT                   = 0,
	DIALOGUE_EVENT                  = 1,
	SHOP_EVENT                      = 2,
	SOUND_EVENT                     = 3,
	MAP_TRANSITION_EVENT            = 4,
	JOIN_PARTY_EVENT                = 5,
	BATTLE_ENCOUNTER_EVENT          = 6,
	SCRIPTED_EVENT                  = 7,
	SCRIPTED_SPRITE_EVENT           = 8,
	CHANGE_DIRECTION_SPRITE_EVENT   = 9,
	PATH_MOVE_SPRITE_EVENT          = 10,
	RANDOM_MOVE_SPRITE_EVENT        = 11,
	ANIMATE_SPRITE_EVENT            = 12,
	TOTAL_EVENT                     = 13
};


//! \brief Defines the different states the dialogue can be in.
enum DIALOGUE_STATE {
	DIALOGUE_STATE_INACTIVE =  0, //!< Active when the dialogue window is in the process of displaying a line of text
	DIALOGUE_STATE_LINE     =  1, //!< Active when the dialogue window is in the process of displaying a line of text
	DIALOGUE_STATE_OPTION   =  2, //!< Active when player-selectable options are present in the dialogue window
};


//! \brief The maximum number of options that a line of dialogue can present to the player
const uint32 MAX_DIALOGUE_OPTIONS = 5;


//! \brief The number of milliseconds to take to fade out the map
const uint32 MAP_FADE_OUT_TIME = 2000;


//! \brief The standard number of milliseconds it takes for enemies to spawn in an enemy zone
const uint32 STANDARD_ENEMY_SPAWN_TIME = 3000;


const uint32 STAMINA_EMPTY  = 0;
const uint32 STAMINA_FULL   = 10000;


/** \brief Returns the opposite facing direction of the direction given in parameter.
*** \return A direction that faces opposite to the argument direction
*** \note This is mostly used as an helper function to make sprites face each other in a conversation.
**/
uint16 CalculateOppositeDirection(const uint16 direction);


/** ****************************************************************************
*** \brief Represents a rectangular section of a map
***
*** This is a small class that is used to represent rectangular map areas. These
*** areas are used very frequently throughout the map code to check for collision
*** detection, determining objects that are within a certain radius of one
*** another, etc.
*** ***************************************************************************/
class MapRectangle {
public:
	MapRectangle() :
		left(0.0f), right(0.0f), top(0.0f), bottom(0.0f)
		{}

	MapRectangle(float l, float r, float t, float b) :
		left(l), right(r), top(t), bottom(b)
		{}

	//! \brief The four edges of the rectangle's area
	float left, right, top, bottom;

	/** \brief Determines if two rectangle objects intersect with one another
	*** \param first A reference to the first rectangle object
	*** \param second A reference to the second rectangle object
	*** \return True if the two rectangles intersect at any location
	***
	*** This function assumes that the rectangle objects hold map collision grid
	*** coordinates, where the top of the rectangle is a smaller number than the
	*** bottom of the rectangle and the left is a smaller number than the right.
	**/
	static bool CheckIntersection(const MapRectangle& first, const MapRectangle& second);
}; // class MapRectangle


/** ****************************************************************************
*** \brief Retains information about how the next map frame should be drawn.
***
*** This class is used by the MapMode class to determine how the next map frame
*** should be drawn. This includes which tiles will be visible and the offset
*** coordinates for the screen. Map objects also use this information to determine
*** where (and if) they should be drawn.
***
*** \note The MapMode class keeps an active object of this class with the latest
*** information about the map. It should be the only instance of this class that is
*** needed.
*** ***************************************************************************/
class MapFrame {
public:
	//! \brief The column and row indeces of the starting tile to draw (the top-left tile).
	int16 starting_col, starting_row;

	//! \brief The number of columns and rows of tiles to draw on the screen.
	uint8 num_draw_cols, num_draw_rows;

	//! \brief The x and y position screen coordinates to start drawing tiles from.
	float tile_x_start, tile_y_start;

	/** \brief The position coordinates of the screen edges.
	*** These members are in terms of the map grid 16x16 pixel coordinates that map objects use.
	*** The presense of these coordinates make it easier for map objects to figure out whether or
	*** not they should be drawn on the screen. Note that these are <b>not</b> used as drawing
	*** cursor positions, but rather are map grid coordinates indicating where the screen edges lie.
	**/
	MapRectangle screen_edges;
}; // class MapFrame


/** ****************************************************************************
*** \brief A container class for node information in pathfinding.
***
*** This class is used in the MapMode#_FindPath function to find an optimal
*** path from a given source to a destination. The path finding algorithm
*** employed is A* and thus many members of this class are particular to the
*** implementation of that algorithm.
*** ***************************************************************************/
class PathNode {
public:
	/** \brief The grid coordinates for this node
	*** These coordinates correspond to the collision grid, where each element
	*** is a 16x16 pixel space on the map.
	**/
	int16 row, col;

	//! \name Path Scoring Members
	//@{
	//! \brief The total score for this node (f = g + h).
	int16 f_score;

	//! \brief The score for this node relative to the source.
	int16 g_score;

	//! \brief The Manhattan distance from this node to the destination.
	int16 h_score;
	//@}

	//! \brief The grid coordinates for the parent of this node
	int16 parent_row, parent_col;

	// ---------- Methods

	PathNode() : row(-1), col(-1), f_score(0), g_score(0), h_score(0), parent_row(0), parent_col(0)
		{}

	PathNode(int16 r, int16 c) : row(r), col(c), f_score(0), g_score(0), h_score(0), parent_row(0), parent_col(0)
		{}

	//! \brief Overloaded comparison operator checks that row and col members are equal
	bool operator==(const PathNode& that) const
		{ return ((this->row == that.row) && (this->col == that.col)); }

	//! \brief Overloaded comparison operator checks that row or col members are unequal
	bool operator!=(const PathNode& that) const
		{ return ((this->row != that.row) || (this->col != that.col)); }

	//! \brief Overloaded comparison operator only used for path finding, compares the two f_scores
	bool operator<(const PathNode& that) const
		{ return this->f_score > that.f_score; }
}; // class PathNode

} // namespace private_map

} // namespace hoa_map

#endif // __MAP_UTILS_HEADER__
