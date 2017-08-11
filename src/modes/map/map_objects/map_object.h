///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_OBJECT_HEADER__
#define __MAP_OBJECT_HEADER__

#include "modes/map/map_utils.h"

#include "engine/video/image.h"
/*
#include "modes/map/map_treasure_supervisor.h"

namespace vt_script {
class ReadScriptDescriptor;
}

namespace vt_mode_manager {
class ParticleEffect;
}

namespace vt_defs {
void BindModeCode();
}
*/
namespace vt_map
{

class MapMode;

//! \brief Used to know on which draw layer the object should be registered.
enum MapObjectDrawLayer {
    // Before ground layer object. Used to display flat object on ground.
    FLATGROUND_OBJECT = 0,
    // Ground objects, such as characters and props.
    GROUND_OBJECT,
    // TODO: Objects used as bridges, removing the map collision when they are visible. They are displayed as flat ground objects.
    PASS_OBJECT,
    // Object displayed above all map layers.
    SKY_OBJECT,
    // An object not registered to be drawn on one specific map layer,
    // since it's has other means and shouldn't be taken in account for collisions.
    // E.g.: Ambient sound objects.
    NO_LAYER_OBJECT
};

namespace private_map
{

// Update the alpha of the interaction icon according to its distance from the player sprite.
const float INTERACTION_ICON_VISIBLE_RANGE = 10.0f;

class ContextZone;
class MapSprite;
class MapZone;
class VirtualSprite;

/** ****************************************************************************
*** \brief Abstract class that represents objects on a map
***
*** A map object can be anything from a sprite to a tree to a house. To state
*** it simply, a map object is a map image that is not tiled and need not be fixed
*** in place. Map objects are drawn in one of three layers: ground, pass, and sky
*** object layers.

*** All map objects have both a collision rectangle and an image rectangle.
*** The collision rectangle indicates what parts of the object may not overlap
*** with other collision rectangles and unwalkable sections of the map. The image
*** rectangle determines the size of the object as it is visible on the screen.
*** The collision rectangle and image rectangles do not need to be the same size.
*** Typically the collision rectangle is smaller than the image rectangle. It is
*** also possible to disable both rectangles via special properties that can be
*** enabled in this class. This would prevent the object from being a factor in
*** collision detection and/or it would never be drawn to the screen.
***
*** State information about map objects may need to be retained upon leaving a
*** map. For example, a treasure (which is a type of map object) needs to know
*** whether or not the player has retrieved its contents already so that they
*** can not be gained a second time. This data is stored in the saved game file
*** so that even when the player exits the game, the state information can be
*** retrieved when the application starts again later.
***
*** \note It is advised not to attempt to make map objects with dynamic sizes (i.e.
*** the various image frames that compose the object should all be the same size).
*** In theory, dynamically sized objects are feasible to implement in maps, but
*** they are much easier to be subject to bugs and other issues.
*** ***************************************************************************/
class MapObject
{
public:
    explicit MapObject(MapObjectDrawLayer layer);
    virtual ~MapObject();

    /** \brief Updates the state of an object.
    *** Many map objects may not actually have a use for this function. For example, animated objects
    *** like a tree will automatically have their frames updated by the video engine in the draw
    *** function. So it is the case that the implementation of this function in derived classes may
    *** simply do nothing.
    **/
    virtual void Update();

    /** \brief Draws the object to the frame buffer.
    *** Objects are drawn differently depending on what type of object they are and what their current
    *** state is. Note that calling this function does not guarantee that the object will be drawn.
    *** Many implementations of this function in the derived classes first call the ShouldDraw() method
    *** to determine if the object should be drawn at all.
    **/
    virtual void Draw() = 0;

    /** \brief Determines if an object should be drawn to the screen.
    *** \return True if the object should be drawn.
    *** \note This function also moves the draw cursor to the proper position if the object should be drawn
    ***
    *** This method performs the common drawing operations of identifying whether or not the object
    *** is visible on the screen and moving the drawing cursor to its location. The children classes
    *** of this class may choose to make use of it (or not).
    **/
    bool ShouldDraw();
    //@}

    //! \brief Retrieves the object type identifier
    MAP_OBJECT_TYPE GetObjectType() const {
        return _object_type;
    }

    /** \brief Tells whether the object is currently colliding with another object or a wall
    *** \param pos_x The tile x position to test against a collision
    *** \param pos_y The tile y position to test against a collision
    **/
    bool IsColliding(float pos_x, float pos_y);

    /** \brief Tells whether the object is currently colliding with another object or a wall
    *** \param object The other object to test against a collision
    **/
    bool IsCollidingWith(MapObject* other_object);

    //! \brief Returns the collision rectangle for the current object on the map collision grid
    vt_common::Rectangle2D GetGridCollisionRectangle() const;

    /** \brief Returns the collision rectangle for the current object for the given position
    *** \return rect A MapRectangle object storing the collision rectangle data
    *** using the given position.
    **/
    vt_common::Rectangle2D GetGridCollisionRectangle(float tile_x, float tile_y) const;

    //! \brief Returns the collision rectangle for the current object.
    vt_common::Rectangle2D GetScreenCollisionRectangle() const;

    /** \brief Returns the collision rectangle for the current object for the given position
    *** \return rect A MapRectangle object storing the collision rectangle data
    *** using the given pixel position.
    **/
    vt_common::Rectangle2D GetScreenCollisionRectangle(float screen_x, float screen_y) const;

    /** \brief Returns the image rectangle for the current object
    *** \param rect A MapRectangle object storing the image rectangle data
    **/
    virtual vt_common::Rectangle2D GetScreenImageRectangle() const;

    /** \brief Returns the image rectangle for the current object
    *** \param rect A MapRectangle object storing the image rectangle data
    **/
    virtual vt_common::Rectangle2D GetGridImageRectangle() const;

    /** \brief Restores the saved state of the object
    *** This state data is retained in the saved game file. When any map object is created and added
    *** to the map, this function is called to load any stored state data that there may be. Notice
    *** that the default implementation of this function does nothing.
    **/
    virtual void RestoreSaved()
    {}

    /** \name Lua Access Functions
    *** These functions are specifically written to enable Lua to access the members of this class.
    *** C++ code may also choose to use these functions, although all of the members here are public
    *** so it is not mandatory to do so.
    **/
    //@{
    void SetPosition(float x, float y) {
        _tile_position.x = x;
        _tile_position.y = y;
    }

    void SetXPosition(float x) {
        _tile_position.x = x;
    }

    void SetYPosition(float y) {
        _tile_position.y = y;
    }

    //! \brief Set the object image half width (in pixels).
    //! \note The value in map tiles is also stored.
    void SetImgPixelHalfWidth(float width) {
        _img_pixel_half_width = width;
        _img_screen_half_width = width * MAP_ZOOM_RATIO;
        _img_grid_half_width = width / GRID_LENGTH * MAP_ZOOM_RATIO;
    }

    //! \brief Set the object image half width (in pixels).
    //! \note The value in map tiles is also stored.
    void SetImgPixelHeight(float height) {
        _img_pixel_height = height;
        _img_screen_height = height * MAP_ZOOM_RATIO;
        _img_grid_height = height / GRID_LENGTH * MAP_ZOOM_RATIO;
    }

    void SetCollPixelHalfWidth(float collision) {
        _coll_pixel_half_width = collision;
        _coll_screen_half_width = collision * MAP_ZOOM_RATIO;
        _coll_grid_half_width = collision / GRID_LENGTH * MAP_ZOOM_RATIO;
    }

    void SetCollPixelHeight(float collision) {
        _coll_pixel_height = collision;
        _coll_screen_height = collision * MAP_ZOOM_RATIO;
        _coll_grid_height = collision / GRID_LENGTH * MAP_ZOOM_RATIO;
    }

    void SetUpdatable(bool update) {
        _updatable = update;
    }

    void SetVisible(bool vis) {
        _visible = vis;
    }

    // Use a set of COLLISION_TYPE bitmask values
    void SetCollisionMask(uint32_t collision_types) {
        _collision_mask = collision_types;
    }

    void SetDrawOnSecondPass(bool pass) {
        _draw_on_second_pass = pass;
    }

    //! \brief Tells the draw layer for faster deletion from the object supervisor.
    MapObjectDrawLayer GetObjectDrawLayer() const {
        return _draw_layer;
    }

    int16_t GetObjectID() const {
        return _object_id;
    }

    //! \brief Get the object position in tiles.
    vt_common::Position2D GetPosition() const {
        return _tile_position;
    }

    float GetXPosition() const {
        return _tile_position.x;
    }

    float GetYPosition() const {
        return _tile_position.y;
    }

    float GetImgScreenHalfWidth() const {
        return _img_screen_half_width;
    }

    float GetImgScreenHeight() const {
        return _img_screen_height;
    }

    float GetCollGridHalfWidth() const {
        return _coll_grid_half_width;
    }

    float GetCollGridHeight() const {
        return _coll_grid_height;
    }

    bool IsUpdatable() const {
        return _updatable;
    }

    bool IsVisible() const {
        return _visible;
    }

    uint32_t GetCollisionMask() const {
        return _collision_mask;
    }

    bool IsDrawOnSecondPass() const {
        return _draw_on_second_pass;
    }

    MAP_OBJECT_TYPE GetType() const {
        return _object_type;
    }

    virtual void SetGrayscale(bool grayscale = true) {
        _grayscale = grayscale;
    }

    bool IsGrayscale() const {
        return _grayscale;
    }

    /** \brief Play the corresponding emote animation set in the emotes.lua file
    *** \see LoadEmotes() in the GameGlobal class.
    **/
    void Emote(const std::string& emote_name, vt_map::private_map::ANIM_DIRECTIONS dir = vt_map::private_map::ANIM_SOUTH);

    //! \brief Indicates whether the given map object is using an emote animation.
    bool HasEmote() const {
        return (_emote_animation);
    }

    //! \brief Loads the current animation file as the new interaction icon of the object.
    void SetInteractionIcon(const std::string& animation_filename);

    //! \brief Draws the interaction icon at the top of the sprite, if any.
    void DrawInteractionIcon();
    //@}

protected:
    /** \brief An identification number for the object as it is represented in the map file.
    *** Objects with an ID less than zero are invalid.
    **/
    int16_t _object_id;

    /** \brief Coordinates for the object's origin/position.
    *** The origin of every map object is the bottom center point of the object. These
    *** origin coordinates are used to determine where the object is on the map as well
    *** as where the objects collision rectangle lies.
    ***
    *** The position coordinates point to the map grid tile that the object currently occupies
    *** and may range from 0 to the number of columns or rows of grid tiles on the map.
    **/
    vt_common::Position2D _tile_position;

    //! \brief The originally desired half-width and height of the image, in pixels
    //! Used as a base value to later get the screen and tile corresponding values.
    float _img_pixel_half_width;
    float _img_pixel_height;

    //! \brief The image half-width and height as seen on screen. (pixel value * zoom ratio).
    float _img_screen_half_width;
    float _img_screen_height;

    //! \brief The half-width and height of the image, in map tile coordinates.
    //! (A grid unit is half a tile).
    float _img_grid_half_width;
    float _img_grid_height;

    /** \brief Determines the collision rectangle for the object in pixels.
    *** The collision area determines what portion of the map object may not be overlapped
    *** by other objects or unwalkable regions of the map.
    ***
    *** \note These members should always be positive and non-zero. Setting these members to
    *** zero does <b>not</b> eliminate collision detection for the object.
    ***
    *** Used as a base value to later get the screen and tile corresponding values.
    **/
    float _coll_pixel_half_width;
    float _coll_pixel_height;

    //! \brief The collision half-width and height as seen on screen in the debug view.
    //! (pixel value * zoom ratio).
    float _coll_screen_half_width;
    float _coll_screen_height;

    //! \brief Determines the collision rectangle for the object in map grid coordinates.
    //! (A grid unit is half a tile).
    float _coll_grid_half_width;
    float _coll_grid_height;

    //! \brief When false, the Update() function will do nothing (default == true).
    bool _updatable;

    //! \brief When false, the Draw() function will do nothing (default == true).
    bool _visible;

    //! \brief The collision mask indicating what the object will collide with. (i.e.: walls + objects, nothing, ...)
    //! \NOTE: COLLISION TYPE used as bitmask
    uint32_t _collision_mask;

    /** \brief When true, objects in the ground object layer will be drawn after the pass objects
    *** This member is only checked for objects that exist in the ground layer. It has no meaning
    *** for objects in the pass or sky layers. Its purpose is so that objects (such as a bridge)
    *** in the pass layer can be both walked over and walked under by sprites in the ground layer.
    **/
    bool _draw_on_second_pass;

    //! \brief This is used to identify the type of map object for inheriting classes.
    MAP_OBJECT_TYPE _object_type;

    //! \brief the emote animation to play.
    //! \note This is a reference pointer only, and shouldn't be deleted.
    vt_video::AnimatedImage* _emote_animation;

    //! \brief the interaction icon animation to play, if any.
    //! \note This one should be deleted in the destructor.
    vt_video::AnimatedImage* _interaction_icon;

    //! \brief The emote animation drawing offset in screen coordinates (pixel value * zoom ratio)
    //! (depending on the map object direction)
    vt_common::Position2D _emote_screen_offset;

    //! \brief the time the emote animation will last in milliseconds,
    int32_t _emote_time;

    //! \brief The object draw layer. Used to know where to register the MapObject,
    //! and when to delete it in the ObjectSupervisor.
    MapObjectDrawLayer _draw_layer;

    //! \brief Tells whether the map object sprite and animation should be displayed grayscaled or not.
    bool _grayscale;

    //! \brief Takes care of updating the emote animation and state.
    void _UpdateEmote();

    //! \brief Takes care of drawing the emote animation.
    void _DrawEmote();
}; // class MapObject


/** \brief This is a predicate used to sort MapObjects in correct draw order
*** \return True if the MapObject pointed by a should be drawn behind MapObject pointed by b
*** \note A simple '<' operator cannot be used with the sorting algorithm because it is sorting pointers.
**/
struct MapObject_Ptr_Less {
    bool operator()(const MapObject* a, const MapObject* b) {
        return (a->GetYPosition()) < (b->GetYPosition());
    }
};

} // namespace private_map

} // namespace vt_map

#endif // __MAP_OBJECT_HEADER__
