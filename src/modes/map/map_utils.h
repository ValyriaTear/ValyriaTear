///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_utils.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for map mode utility code
*** *****************************************************************************/

#ifndef __MAP_UTILS_HEADER__
#define __MAP_UTILS_HEADER__

#include "utils/utils_pch.h"
#include "engine/video/video_utils.h"

#include <cmath>
#include <vector>

namespace vt_map
{

//! Determines whether the code in the vt_map namespace should print debug statements or not.
extern bool MAP_DEBUG;

namespace private_map
{

/** \name Screen Coordiante System Constants
*** \brief Represents the size of the visible screen in map tiles and the collision grid
*** Every map tile is 32x32 pixels, and every collision grid element is one quarter of that
*** area (16x16). Thus the number of collision grid elements that compose the screen are
*** four times the number of tiles that are visible on the screen. This also means the number
*** of rows and columns of grid elements that encompass the screen are twice that of the
*** number of rows and columns of tiles.
**/
//@{
//! The zoom ratio determines how magnified map mode will display its tiles, sprites, and objects.
//  Zoom ratios < 1.0f are not fully supported because the secondary render target will not be
//  guaranteed to be large enough to composite the entire visual area of the map. 
const float MAP_ZOOM_RATIO = 2.0f;

const float SCREEN_GRID_X_LENGTH = 64.0f;
const float SCREEN_GRID_Y_LENGTH = 48.0f;

const uint16_t TILES_ON_X_AXIS = static_cast<uint16_t>(SCREEN_GRID_X_LENGTH / 2.0f); // Number of tile columns that fit on the screen.
const uint16_t TILES_ON_Y_AXIS = static_cast<uint16_t>(SCREEN_GRID_Y_LENGTH / 2.0f); // Number of tile rows that fit on the screen.

// Length of a grid element in pixels.
const uint16_t GRID_LENGTH = vt_video::VIDEO_STANDARD_RES_WIDTH / SCREEN_GRID_X_LENGTH;
// Length of a tile in pixels.
const uint16_t TILE_LENGTH = GRID_LENGTH * 2;
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

//! \brief The number of tiles that are found in a tileset image (512x512 pixel image containing 32x32 pixel tiles)
const uint32_t TILES_PER_TILESET = 256;

//! \brief Used to identify the type of map object
enum MAP_OBJECT_TYPE {
    OBJECT_TYPE   = -1, //! Default type
    PHYSICAL_TYPE = 0,  //! This is a physical (visible) object, like a rock, a door, ect...
    VIRTUAL_TYPE  = 1,  //! This is used as another type of NPC sprite, but without interaction.
    SPRITE_TYPE   = 2,  //! This is a NPC sprite. Blocking for the hero, but not for the other sprites.
    ENEMY_TYPE    = 3,  //! This is an enemy sprite. Blocking the hero, and triggering a battle.
    TREASURE_TYPE = 4,  /** This is a treasure, can obtain a treasure from it when exploring,
                        but blocking for movement. */
    SAVE_TYPE     = 5,  //! This is a save point. The player can save while its character is in it.
    TRIGGER_TYPE  = 6,  //! A trigger object.

    //! Objects without possible interaction
    //! Those objects are ignored when searching for the nearest interactable object.
    HALO_TYPE     = 7,  //! This is a source of light, here for eye candy.
    LIGHT_TYPE    = 8,  //! Another light type, but will change dynamically according to the map viewpoint.
    PARTICLE_TYPE = 9,  //! A particle object.
    SOUND_TYPE    = 10, //! An environmental sound
    SCENERY_TYPE  = 11  //! The object is a scenery animal or harmless thing, and nothing should collide with.
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
// Slightly higher than the normal speed, but less than a running character.
const float ENEMY_SPEED      = 130.0f;
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
const uint16_t NORTH     = 0x0001;
const uint16_t SOUTH     = 0x0002;
const uint16_t WEST      = 0x0004;
const uint16_t EAST      = 0x0008;
const uint16_t NW_NORTH  = 0x0010;
const uint16_t NW_WEST   = 0x0020;
const uint16_t NE_NORTH  = 0x0040;
const uint16_t NE_EAST   = 0x0080;
const uint16_t SW_SOUTH  = 0x0100;
const uint16_t SW_WEST   = 0x0200;
const uint16_t SE_SOUTH  = 0x0400;
const uint16_t SE_EAST   = 0x0800;
// Used to check for movement direction regardless of facing direction
const uint16_t MOVING_NORTHWARD = NORTH | NW_NORTH | NW_WEST | NE_NORTH | NE_EAST;
const uint16_t MOVING_SOUTHWARD = SOUTH | SW_SOUTH | SW_WEST | SE_SOUTH | SE_EAST;
const uint16_t MOVING_EASTWARD = EAST | NE_EAST | NE_NORTH | SE_EAST | SE_SOUTH;
const uint16_t MOVING_WESTWARD = WEST | NW_WEST | NW_NORTH | SW_WEST | SW_SOUTH;
const uint16_t MOVING_NORTHWEST = NW_NORTH | NW_WEST;
const uint16_t MOVING_NORTHEAST = NE_NORTH | NE_EAST;
const uint16_t MOVING_SOUTHWEST = SW_SOUTH | SW_WEST;
const uint16_t MOVING_SOUTHEAST = SE_SOUTH | SE_EAST;
const uint16_t MOVING_ORTHOGONALLY = NORTH | SOUTH | EAST | WEST;
const uint16_t MOVING_DIAGONALLY = MOVING_NORTHWEST | MOVING_NORTHEAST | MOVING_SOUTHWEST | MOVING_SOUTHEAST;
// Used to check for facing direction regardless of moving direction
const uint16_t FACING_NORTH = NORTH | NW_NORTH | NE_NORTH;
const uint16_t FACING_SOUTH = SOUTH | SW_SOUTH | SE_SOUTH;
const uint16_t FACING_WEST = WEST | NW_WEST | SW_WEST;
const uint16_t FACING_EAST = EAST | NE_EAST | SE_EAST;
//@}

/** \name Map Sprite Animation Constants
*** These constants are used to index the MapSprite#animations vector to display the correct
*** animation depending on the given direction.
**/
//@{
enum ANIM_DIRECTIONS {
    ANIM_SOUTH = 0,
    ANIM_NORTH = 1,
    ANIM_WEST  = 2,
    ANIM_EAST  = 3,
    NUM_ANIM_DIRECTIONS = 4
};
//@}

/** \brief Represents the various types of collisions which may occur for a sprite
*** NOTE: Used as a bitmask.
**/
enum COLLISION_TYPE {
    NO_COLLISION =        0, //!< Indicates that no collision has occurred.
    CHARACTER_COLLISION = 1, //!< Indicates that a collision with a character.
    ENEMY_COLLISION =     2, //!< Indicates that a collision with an enemy.
    WALL_COLLISION =      4, //! Indicates a collision with a wall. (Sky objects will have a wall collision on the map bounds)
    COLLISION_TYPES =     4
};
const uint32_t ALL_COLLISION = CHARACTER_COLLISION | ENEMY_COLLISION | WALL_COLLISION;

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
    TREASURE_EVENT                  = 13,
    LOOK_AT_SPRITE_EVENT            = 14,
    IF_EVENT                        = 15,
    TOTAL_EVENT                     = 16
};

//! \brief Defines the different states the dialogue can be in.
enum DIALOGUE_STATE {
    DIALOGUE_STATE_INACTIVE =  0, //!< Active when the dialogue window is in the process of displaying a line of text
    DIALOGUE_STATE_LINE     =  1, //!< Active when the dialogue window is in the process of displaying a line of text
    DIALOGUE_STATE_OPTION   =  2, //!< Active when player-selectable options are present in the dialogue window
    DIALOGUE_STATE_EMOTE    =  3  //!< Active when the dialogue supervisor is waiting for an emote event to finish before drawing a line.
};

//! \brief The maximum number of options that a line of dialogue can present to the player
const uint32_t MAX_DIALOGUE_OPTIONS = 5;

//! \brief The number of milliseconds to take to fade out the map
const uint32_t MAP_FADE_OUT_TIME = 800;

/** ****************************************************************************
*** \brief Represents a rectangular section of a map
***
*** This is a small class that is used to represent rectangular map areas. These
*** areas are used very frequently throughout the map code to check for collision
*** detection, determining objects that are within a certain radius of one
*** another, etc.
*** ***************************************************************************/
class MapRectangle
{
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
    static bool CheckIntersection(const MapRectangle &first, const MapRectangle &second);
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
class MapFrame
{
public:
    //! \brief The x and y indeces of the starting tile to draw (the top-left tile).
    int16_t tile_x_start, tile_y_start;

    //! \brief The x and y position screen coordinates to start drawing tiles from.
    float tile_x_offset, tile_y_offset;

    //! \brief The number of tiles to draw on the x and y axes on the screen.
    uint8_t num_draw_x_axis, num_draw_y_axis;

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
class PathNode
{
public:
    /** \brief The grid coordinates for this node
    *** These coordinates correspond to the collision grid, where each element
    *** is a 16x16 pixel space on the map.
    **/
    int16_t tile_x, tile_y;

    //! \name Path Scoring Members
    //@{
    //! \brief The total score for this node (f = g + h).
    int16_t f_score;

    //! \brief The score for this node relative to the source.
    int16_t g_score;

    //! \brief The Manhattan distance from this node to the destination.
    int16_t h_score;
    //@}

    //! \brief The grid coordinates for the parent of this node
    int16_t parent_x, parent_y;

    // ---------- Methods

    PathNode() : tile_x(-1), tile_y(-1), f_score(0), g_score(0), h_score(0), parent_x(0), parent_y(0)
    {}

    PathNode(int16_t x_, int16_t y_) : tile_x(x_), tile_y(y_), f_score(0), g_score(0), h_score(0), parent_x(0), parent_y(0)
    {}

    //! \brief Overloaded comparison operator, only checks that the tile_x and tile_y members are equal
    bool operator==(const PathNode &that) const {
        return ((this->tile_x == that.tile_x) && (this->tile_y == that.tile_y));
    }

    //! \brief Overloaded comparison operator, only checks that the tile_x or tile_y members are unequal
    bool operator!=(const PathNode &that) const {
        return ((this->tile_x != that.tile_x) || (this->tile_y != that.tile_y));
    }

    //! \brief Overloaded comparison operator only used for path finding, compares the two f_scores
    bool operator<(const PathNode &that) const {
        return this->f_score > that.f_score;
    }
}; // class PathNode

struct MapVector {
    MapVector() :
        x(0.0f),
        y(0.0f)
    {}

    MapVector(float x_, float y_) :
        x(x_),
        y(y_)
    {}

    float length() const {
        return sqrtf(x * x + y * y);
    }

    MapVector& operator= (const MapVector& map_vec) {
        // Prevents upon-self copy.
        if (&map_vec == this)
            return *this;

        x = map_vec.x;
        y = map_vec.y;

        return *this;
    }

    float x;
    float y;
};

typedef MapVector MapPosition;

typedef std::vector<MapPosition> Path;

} // namespace private_map

} // namespace vt_map

#endif // __MAP_UTILS_HEADER__
