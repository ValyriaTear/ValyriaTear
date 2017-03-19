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
*** \file    map_objects.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for map mode objects.
*** *****************************************************************************/

#ifndef __MAP_OBJECTS_HEADER__
#define __MAP_OBJECTS_HEADER__

#include "modes/map/map_treasure.h"

namespace vt_script {
class ReadScriptDescriptor;
}

namespace vt_mode_manager {
class ParticleEffect;
}

namespace vt_defs {
void BindModeCode();
}

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
    MapRectangle GetGridCollisionRectangle() const;

    /** \brief Returns the collision rectangle for the current object for the given position
    *** \return rect A MapRectangle object storing the collision rectangle data
    *** using the given position.
    **/
    MapRectangle GetGridCollisionRectangle(float tile_x, float tile_y) const;

    //! \brief Returns the collision rectangle for the current object.
    MapRectangle GetScreenCollisionRectangle() const;

    /** \brief Returns the collision rectangle for the current object for the given position
    *** \return rect A MapRectangle object storing the collision rectangle data
    *** using the given pixel position.
    **/
    MapRectangle GetScreenCollisionRectangle(float screen_x, float screen_y) const;

    /** \brief Returns the image rectangle for the current object
    *** \param rect A MapRectangle object storing the image rectangle data
    **/
    virtual MapRectangle GetScreenImageRectangle() const;

    /** \brief Returns the image rectangle for the current object
    *** \param rect A MapRectangle object storing the image rectangle data
    **/
    virtual MapRectangle GetGridImageRectangle() const;

    /** \brief Restores the saved state of the object
    *** This state data is retained in the saved game file. When any map object is created and added
    *** to the map, this function is called to load any stored state data that there may be. Notice
    *** that the default implementation of this function does nothing.
    **/
    virtual void RestoreSaved()
    {
    }

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
        _img_grid_half_width = width / GRID_LENGTH;
    }

    //! \brief Set the object image half width (in pixels).
    //! \note The value in map tiles is also stored.
    void SetImgPixelHeight(float height) {
        _img_pixel_height = height;
        _img_grid_height = height / GRID_LENGTH;
    }

    void SetCollPixelHalfWidth(float collision) {
        _coll_pixel_half_width = collision;
        _coll_grid_half_width = collision / GRID_LENGTH;
    }

    void SetCollPixelHeight(float collision) {
        _coll_pixel_height = collision;
        _coll_grid_height = collision / GRID_LENGTH;
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
    MapPosition GetPosition() const {
        return _tile_position;
    }

    float GetXPosition() const {
        return _tile_position.x;
    }

    float GetYPosition() const {
        return _tile_position.y;
    }

    float GetImgPixelHalfWidth() const {
        return _img_pixel_half_width;
    }

    float GetImgPixelHeight() const {
        return _img_pixel_height;
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
    MapPosition _tile_position;

    //! \brief The originally desired half-width and height of the image, in pixels.
    float _img_pixel_half_width;
    float _img_pixel_height;

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
    **/
    float _coll_pixel_half_width;
    float _coll_pixel_height;

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

    //! \brief The emote animation drawing offset.
    float _emote_pixel_offset_x;
    float _emote_pixel_offset_y;

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


/** ****************************************************************************
*** \brief Represents visible objects on the map that have no motion.
***
*** This class represents both still image and animated objects. These objects
*** are usually fixed in place and do not change their position.
***
*** \note If the object does not have any animated images, set the 'updatable'
*** member of the base class to false. Forgetting to do this will do no harm, but
*** it will make it heavier.
*** ***************************************************************************/
class PhysicalObject : public MapObject
{
public:
    explicit PhysicalObject(MapObjectDrawLayer layer);
    virtual ~PhysicalObject() override;

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static PhysicalObject* Create(MapObjectDrawLayer layer);

    //! \brief Updates the object's current animation.
    virtual void Update();

    //! \brief Draws the object to the screen, if it is visible.
    virtual void Draw();

    /** \name Lua Access Functions
    *** These functions are specifically to enable Lua to access the members of this class.
    **/
    //@{
    /** \brief Sets a new animation using the animation filename provided
    *** \param animation_filename The name of the animation file to use for the animation
    *** \return The animation id that can later be used with SetCurrentAnimation() or -1 if invalid
    **/
    int32_t AddAnimation(const std::string& animation_filename);

    /** \brief Sets a new still animation using the image filename provided
    *** \param image_filename The name of the image file to use for the animation
    *** \return The animation id that can later be used with SetCurrentAnimation() or -1 if invalid
    **/
    int32_t AddStillFrame(const std::string& image_filename);

    void AddAnimation(const vt_video::AnimatedImage& new_img) {
        _animations.push_back(new_img);
    }

    void SetCurrentAnimation(uint32_t animation_id);

    void SetAnimationProgress(uint32_t progress) {
        _animations[_current_animation_id].SetTimeProgress(progress);
    }

    uint32_t GetCurrentAnimationId() const {
        return _current_animation_id;
    }

    void RandomizeCurrentAnimationFrame() {
        _animations[_current_animation_id].RandomizeAnimationFrame();
    }

    /** \brief Adds an event triggered when talking to a physical object
    *** \param event_id The event string id
    **/
    void SetEventWhenTalking(const std::string& event_id) {
        _event_when_talking = event_id;
    }

    /** \brief Removes the event linked to a physical object
    **/
    void ClearEventWhenTalking() {
        _event_when_talking.clear();
    }

    //! \brief Returns the event id triggered when talking to the physical object.
    const std::string& GetEventIdWhenTalking() const {
        return _event_when_talking;
    }

    virtual void SetGrayscale(bool grayscale = true) {
        _grayscale = grayscale;
        for (vt_video::AnimatedImage& animation : _animations) {
            animation.SetGrayscale(grayscale);
        }
    }
    //@}

protected:
    /** \brief A vector containing all the object's animations.
    *** These need not be actual animations. If you just want a still image, add only a single
    *** frame to the animation. Usually only need a single still image or animation will be
    *** needed, but a vector is used here in case others are needed.
    **/
    std::vector<vt_video::AnimatedImage> _animations;

private:
    /** \brief The index to the animations vector that contains the current image to display
    *** When modifying this member, take care not to exceed the bounds of the animations vector
    **/
    uint32_t _current_animation_id;

    //! \brief The event id triggered when talking to the sprite.
    std::string _event_when_talking;
}; // class PhysicalObject : public MapObject

/** ****************************************************************************
*** \brief Represents particle object on the map
*** ***************************************************************************/
class ParticleObject : public MapObject
{
public:
    ParticleObject(const std::string& filename, float x, float y, MapObjectDrawLayer layer);
    virtual ~ParticleObject() override;

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static ParticleObject* Create(const std::string& filename, float x, float y, MapObjectDrawLayer layer);

    //! \brief Updates the object's current animation.
    //! \note the actual image resources is handled by the main map object.
    void Update();

    //! \brief Draws the object to the screen, if it is visible.
    //! \note the actual image resources is handled by the main map object.
    void Draw();

    //! \brief Start or restart the particle effect
    void Stop();

    //! \brief Stop the particle effect
    bool Start();

    //! \brief Tells whether there are particles still alive,
    //! even if the whole particle effect is stopping.
    bool IsAlive() const;

private:
    //! \brief A reference to the current map save animation.
    vt_mode_manager::ParticleEffect* _particle_effect;

    //@}
}; // class ParticleObject : public MapObject

/** ****************************************************************************
*** \brief Represents save point on the map
*** ***************************************************************************/
class SavePoint : public MapObject
{
public:
    SavePoint(float x, float y);
    virtual ~SavePoint() override
    {
    }

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static SavePoint* Create(float x, float y);

    //! \brief Updates the object's current animation.
    //! \note the actual image resources is handled by the main map object.
    void Update();

    //! \brief Draws the object to the screen, if it is visible.
    //! \note the actual image resources is handled by the main map object.
    void Draw();

    //! \brief Tells whether a character is in or not, and setup the animation
    //! accordingly.
    void SetActive(bool active);

private:
    //! \brief A reference to the current map save animation.
    std::vector<vt_video::AnimatedImage>* _animations;

    //! \brief The corresponding particle object for active/inactive save points pointers
    // Note that those pointers are managed by the object supervisor
    ParticleObject *_active_particle_object;
    ParticleObject *_inactive_particle_object;

    //! \brief Tells whether the save has become active
    bool _save_active;
    //@}
}; // class SavePoint : public MapObject


/** ****************************************************************************
*** \brief Represents a halo (source of light) on the map
*** ***************************************************************************/
class Halo : public MapObject
{
public:
    //! \brief setup a halo on the map, using the given animation file.
    Halo(const std::string& filename, float x, float y, const vt_video::Color& color);
    virtual ~Halo() override
    {
    }

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static Halo* Create(const std::string& filename, float x, float y,
                        const vt_video::Color& color);

    //! \brief Updates the object's current animation.
    //! \note the actual image resources is handled by the main map object.
    void Update();

    //! \brief Draws the object to the screen, if it is visible.
    //! \note the actual image resources is handled by the main map object.
    void Draw();

private:
    //! \brief A reference to the current map save animation.
    vt_video::AnimatedImage _animation;

    //! The blending color of the halo
    vt_video::Color _color;

    //@}
}; // class Halo : public MapObject

/** ****************************************************************************
*** \brief Represents a source of light on the map, changing its orientation
*** according to the camera view.
*** ***************************************************************************/
class Light : public MapObject
{
public:
    //! \brief setup a halo on the map, using the given animation file.
    Light(const std::string &main_flare_filename,
          const std::string &secondary_flare_filename,
          float x, float y,
          const vt_video::Color &main_color, const vt_video::Color &secondary_color);

    virtual ~Light() override
    {
    }

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static Light* Create(const std::string &main_flare_filename,
                         const std::string &secondary_flare_filename,
                         float x, float y,
                         const vt_video::Color &main_color,
                         const vt_video::Color &secondary_color);

    //! \brief Updates the object's current animation and orientation
    //! \note the actual image resources is handled by the main map object.
    void Update();

    //! \brief Draws the object to the screen, if it is visible.
    //! \note the actual image resources is handled by the main map object.
    void Draw();

    /** \brief Returns the image rectangle for the current object
    *** \param rect A MapRectangle object storing the image rectangle data
    **/
    MapRectangle GetGridImageRectangle() const;
private:
    //! Updates the angle and distance from the camera viewpoint
    void _UpdateLightAngle();

    //! \brief A reference to the current light animation.
    vt_video::AnimatedImage _main_animation;
    vt_video::AnimatedImage _secondary_animation;

    //! The blending color of the light
    vt_video::Color _main_color;
    vt_video::Color _secondary_color;

    //! The blending color with dynamic alpha, for better rendering
    vt_video::Color _main_color_alpha;
    vt_video::Color _secondary_color_alpha;

    //! used to compute the flare lines equation.
    float _a, _b;
    //! Distance between the light and the camera viewpoint.
    float _distance;

    //! Random distance factor used to make the secondary flares appear at random places
    float _distance_factor_1;
    float _distance_factor_2;
    float _distance_factor_3;
    float _distance_factor_4;

    /** \brief Used for optimization, keeps the last center position.
    *** So that we update the distance and angle only when this position has changed.
    **/
    MapPosition _last_center_pos;

    //@}
}; // class Light : public MapObject

/** ****************************************************************************
*** \brief Represents a sound source object on the map
*** ***************************************************************************/
class SoundObject : public MapObject
{
public:
    /** \brief An environmental sound object which sound is played looped and with a volume
    *** computed against the distance of the object with the camera.
    *** \param sound_filename The sound filename to play.
    *** \param x, y The sound map location
    *** \param strength The "strength" of the sound, the maximal distance
    in map tiles the sound can be heard within.
    *** The sound volume will be compute according that distance.
    **/
    SoundObject(const std::string& sound_filename, float x, float y, float strength);

    virtual ~SoundObject() override
    {
    }

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static SoundObject* Create(const std::string& sound_filename,
                               float x, float y, float strength);

    //! \brief Updates the object's current volume.
    void Update();

    //! \brief Does nothing
    void Draw()
    {}

    //! \brief Stop the ambient sound
    void Stop();

    //! \brief Start the ambient sound
    void Start();

    //! \brief Tells whether the ambient sound is active
    bool IsActive() const {
        return _activated;
    }

    //! \brief Sets the max sound volume of the ambient sound.
    //! From  0.0f to 1.0f
    void SetMaxVolume(float max_volume);

    //! \brief Gets the sound descriptor of the object.
    //! Used to apply changes directly to the sound object.
    vt_audio::SoundDescriptor& GetSoundDescriptor() {
        return _sound;
    }

private:
    //! \brief The sound object.
    vt_audio::SoundDescriptor _sound;

    //! \brief The maximal distance in map tiles the sound can be heard within.
    float _strength;

    //! \brief The maximal strength of the sound object. (0.0f - 1.0f)
    float _max_sound_volume;

    //! \brief The time remaining before next update
    int32_t _time_remaining;

    //! \brief Tells whether the sound is activated.
    bool _activated;

    //! \brief Tells whether the sound is currently playing or not
    //! This boolean is here to avoid calling fadeIn()/FadeOut()
    //! repeatedly on sounds.
    bool _playing;
}; // class SoundObject : public MapObject

/** ****************************************************************************
*** \brief Represents an obtainable treasure on the map which the player may access
***
*** A treasure is a specific type of physical object, usually in the form of a
*** treasure chest. When the player accesses these treasures, the chest animates as
*** it is being opened and the treasure supervisor is initialized once the opening
*** animation is complete. Each treasure object on a map has a global event associated
*** with it to determine whether the treasure contents have already been retrieved by
*** the player.
***
*** Image files for treasures are single row multi images where the frame ordering
*** goes from closed, to opening, to open. This means each map treasure has exactly
*** three animations. The closed and open animations are usually single frame images.
***
*** To add contents to the treasure for this object, you will need to retreive the
*** pointer to the MapTreasure object via the GetTreasure() method, then add drunes
*** and/or objects (items/equipment/etc) to the MapTreasure.
***
*** \todo Add support for more treasure features, such as locked chests, chests which
*** trigger a battle, etc.
*** ***************************************************************************/
class TreasureObject : public PhysicalObject
{
    //! \brief Constants representing the three types of animations for the treasure
    enum {
        TREASURE_CLOSED_ANIM   = 0,
        TREASURE_OPENING_ANIM  = 1,
        TREASURE_OPEN_ANIM     = 2
    };

public:
    /** \param treasure_name The name of the treasure. Used to store and load the treasure state.
    *** \param treasure The treasure object holding its content.
    *** \param closed_animation_file The animation file used to display the treasure when it is closed.
    *** \param opening_animation_file The animation file used to display the treasure when it is opening.
    *** \param open_animation_file The animation file used to display the treasure when it is open.
    **/
    TreasureObject(const std::string &treasure_name,
                   MapObjectDrawLayer layer,
                   const std::string &closed_animation_file,
                   const std::string &opening_animation_file,
                   const std::string &open_animation_file);

    ~TreasureObject() {
        delete _treasure;
    }

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static TreasureObject* Create(const std::string &treasure_name,
                                  MapObjectDrawLayer layer,
                                  const std::string &closed_animation_file,
                                  const std::string &opening_animation_file,
                                  const std::string &open_animation_file);

    std::string GetTreasureName() const {
        return _treasure_name;
    }

    //! \brief Opens the treasure, which changes the active animation and initializes the treasure supervisor when the opening animation finishes.
    void Open();

    //! \brief Changes the current animation if it has finished looping
    void Update();

    //! \brief Retrieves a pointer to the MapTreasure object holding the treasure.
    MapTreasure *GetTreasure() {
        return _treasure;
    }

    //! \brief Sets the number of drunes present in the chest's contents.
    void SetDrunes(uint32_t amount) {
        _treasure->SetDrunes(amount);
    }

    /** \brief Adds an item to the contents of the TreasureObject
    *** \param id The id of the GlobalObject to add
    *** \param quantity The number of the object to add (default == 1)
    *** \return True if the object was added successfully
    **/
    bool AddItem(uint32_t id, uint32_t quantity = 1);

    /** \brief Adds an event triggered at start of the treasure event.
    *** \param event_id The id of the event to add
    **/
    void AddEvent(const std::string& event_id);

private:
    //! \brief Stores the contents of the treasure which will be processed by the treasure supervisor
    MapTreasure *_treasure;

    //! \brief The treasure object name
    std::string _treasure_name;

    //! \brief Events triggered at the start of the treasure event.
    std::vector<std::string> _events;

    //! \brief Tells whether the events have been started. So we can keep track of
    //! whether they've finished before opening the treasure supervisor.
    bool _events_triggered;

    //! \brief Tells whether the treasure is being opened.
    bool _is_opening;

    //! \brief Loads the state of the chest from the global event corresponding to the current map
    void _LoadState();
}; // class TreasureObject : public PhysicalObject

// Trigger zone object
//! Represents a flat object that can be visually triggered when "talking" or stepping on it.
//! The object will then trigger an event.
class TriggerObject : public PhysicalObject
{
    //! \brief Constants representing the three types of animations for the treasure
    enum {
        TRIGGER_OFF_ANIM  = 0,
        TRIGGER_ON_ANIM = 1
    };

public:
    /** \param trigger_name The name of the trigger. Used to store and load the trigger state.
    *** \param off_animation_file The animation file used to display the treasure when it is closed.
    *** \param on_animation_file The animation file used to display the treasure when it is open.
    *** \param off_event_id The event id to call when setting the trigger to off.
    *** \param on_event_id The event id to call when setting the trigger to on.
    **/
    TriggerObject(const std::string &trigger_name, MapObjectDrawLayer layer,
                  const std::string &off_animation_file, const std::string &on_animation_file,
                  const std::string& off_event_id, const std::string& on_event_id);

    ~TriggerObject()
    {}

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static TriggerObject* Create(const std::string &trigger_name,
                                 MapObjectDrawLayer layer,
                                 const std::string &off_animation_file,
                                 const std::string &on_animation_file,
                                 const std::string& off_event_id,
                                 const std::string& on_event_id);

    //! \brief Changes the current animation if the character collides with the trigger.
    void Update();

    std::string GetTriggerName() const
    { return _trigger_name; }

    //! \brief Triggers the object from off to on, or the contrary, calling the on or off event.
    //! true == triggered/on.
    void SetState(bool state = true);

    bool GetState() const
    { return _trigger_state; }

    void ToggleState()
    { SetState(!_trigger_state); }

    //! \brief Set whether the trigger can be toggled by the character.
    void SetTriggerableByCharacter(bool triggerable)
    { _triggerable_by_character = triggerable; }

    //! \brief Set the new event name trigger when the trigger is pushed.
    //! if the event is empty, the trigger event is disabled.
    void SetOnEvent(const std::string& on_event)
    { _on_event = on_event; }

    //! \brief Set the new event name trigger when the trigger is set to not pushed.
    //! if the event is empty, the trigger event is disabled.
    void SetOffEvent(const std::string& off_event)
    { _off_event = off_event; }

private:
    //! \brief The treasure object name
    std::string _trigger_name;

    //! The trigger state (false == off)
    bool _trigger_state;

    //! \brief Tells whether the character can toggle the state by stepping on it.
    //! If not, only events can do that. (true by default)
    bool _triggerable_by_character;

    //! \brief Event triggered when the trigger is set to on.
    std::string _on_event;

    //! \brief Event triggered when the trigger is set to off.
    std::string _off_event;

    //! \brief Loads the state of the trigger from the global event corresponding to the current map
    //! It doesn't call the on/off events since this should be dealt with the trigger states at map load time.
    void _LoadState();
}; // class TreasureObject : public PhysicalObject

/** ****************************************************************************
*** \brief A helper class to MapMode responsible for management of all object and sprite data
***
*** This class is responsible for loading, updating, and drawing all map objects
*** and map sprites, in addition to maintaining the map's collision grid and map
*** zones. This class contains the implementation of the collision detection
*** and path finding algorithms.
***
*** \todo Each map object is assigned an ID and certain values of IDs are reserved
*** for different types of map objects. We need to find out what these are and
*** maintain a list of those ranges here.
*** ***************************************************************************/
class ObjectSupervisor
{
    friend class vt_map::MapMode;
    friend void vt_defs::BindModeCode();

public:
    ObjectSupervisor();

    ~ObjectSupervisor();

    //! \brief Returns a unique ID integer for an object to use
    //! Every object Id must be > 0 since 0 is reserved for speakerless dialogues.
    uint16_t GenerateObjectID() {
        return ++_last_id;
    }

    //! \brief Returns the number of objects stored by the supervisor, regardless of what layer they exist on
    uint32_t GetNumberObjects() const {
        return _all_objects.size();
    }

    /** \brief Retrieves a pointer to an object on this map
    *** \param object_id The id number of the object to retrieve
    *** \return A pointer to the map object, or nullptr if no object with that ID was found
    **/
    MapObject* GetObject(uint32_t object_id);

    /** \brief Retrieves a pointer to a sprite on this map
    *** \param object_id The id number of the sprite to retrieve
    *** \return A pointer to the sprite object, or nullptr if the object was not found or was not a sprite type
    **/
    VirtualSprite* GetSprite(uint32_t object_id);

    //! \brief Wrapper to add an object in the all objects vector.
    //! This should only be called by the MapObject constructor.
    void RegisterObject(MapObject* object);

    //! \brief Delete an object from memory.
    void DeleteObject(MapObject* object);

    //! \brief Add sound objects (Done within the sound object constructor)
    void AddAmbientSound(SoundObject* object);

    //! \brief Add a light object, often created through scripting.
    //! Called by the Light object constructor
    void AddLight(Light* light);

    //! \brief Add a halo object, often created through scripting.
    //! Called by the Halo object constructor
    void AddHalo(Halo* halo);

    //! \brief Add a save point.
    //! Called by the SavePoint object constructor
    void AddSavePoint(SavePoint* save_point);

    //! \brief Adds a new zone.
    // Called by the Mazone constructor.
    void AddZone(MapZone* zone);

    //! \brief Sorts objects on all three layers according to their draw order
    void SortObjects();

    /** \brief Loads the collision grid data and saved state of all map objects
    *** \param map_file A reference to the open map script file
    *** \return Whether the collision data loading was successful.
    ***
    *** The file must be open prior to making this call and additionally must
    *** be at the highest level scope (i.e., there are no actively open tables
    *** in the script descriptor object).
    **/
    bool Load(vt_script::ReadScriptDescriptor &map_file);

    //! \brief Updates the state of all map zones and objects
    void Update();

    /** \brief Draws the various object layers to the screen
    *** \param frame A pointer to the information required to draw this frame
    *** \note These functions do not reset the coordinate system and hence depend that the proper coordinate system
    *** is already set prior to these function calls (0.0f, SCREEN_COLS, SCREEN_ROWS, 0.0f). These functions do make
    *** modifications to the draw flags and the draw cursor position, which are not restored by the function
    *** upon its return. Take measures to retain this information before calling these functions if necessary.
    **/
    //@{
    void DrawSavePoints();
    void DrawFlatGroundObjects();
    void DrawGroundObjects(const bool second_pass);
    void DrawPassObjects();
    void DrawSkyObjects();
    void DrawLights();
    void DrawInteractionIcons();
    //@}

    /** \brief Finds the nearest interactable map object within a certain distance of a sprite
    *** \param sprite The sprite who is trying to find its nearest object
    *** \param search_distance The maximum distance to search for an object from the sprite (default == 3.0f)
    *** \return A pointer to the nearest map object, or nullptr if no such object was found.
    ***
    *** An interactable object must be in the same context as the function argument is. For an object
    *** to be valid, it's collision rectangle must be no greater than the search distance (in units of
    *** collision grid elements) from the sprite's "calling" axis. For example, if the search distance was 3.0f
    *** and the sprite was facing downwards, this function draws an imaginary rectangle below the sprite of height
    *** 3.0f and a length equal to the length of the sprite. Any objects that have their collision rectangles intersect
    *** with any portion of this search area are put on a list of valid objects, and once this list has been fully
    *** constructed the nearest of these objects will be returned.
    **/
    private_map::MapObject *FindNearestInteractionObject(const private_map::VirtualSprite *sprite, float search_distance = 3.0f);

    /** \brief Determines if a map object's collision rectangle intersects with a specified map area
    *** \param rect A reference to the rectangular section of the map to do collision detection with
    *** \param obj A pointer to a map object
    *** \return True if the objects collide with one another
    *** \note This test is "absolute", and does not factor in things such as map contexts or whether or
    *** not the no_collision property is enabled on the MapObject.
    **/
    bool CheckObjectCollision(const MapRectangle &rect, const private_map::MapObject *const obj);

    /** \brief Determines if a specific map object occupies a specific element of the collision grid
    *** \param x The x axis position
    *** \param y The y axis position
    *** \param object The object to check for occupation of the grid element
    *** \return True if the grid element is occupied by the object
    ***
    *** \todo Take into account the object/sprite's collision property and also add a parameter for map context
    **/
    bool IsPositionOccupiedByObject(float x, float y, MapObject *object);

    /** \brief Tells the collision type corresponding to an object type.
    *** \param obj A pointer to the map object to check
    *** \return The corresponding type of collision detected.
    **/
    COLLISION_TYPE GetCollisionFromObjectType(MapObject *obj) const;

    /** \brief Tells the collision type of a sprite when it is at the given position
    *** \param object A pointer to the map object to check
    *** \param x The collision point on the x axis
    *** \param y The collision point on the y axis
    *** \param coll_obj A pointer to the MapObject that the sprite has collided with, if any
    *** \return The type of collision detected, which may include NO_COLLISION
    *** if none was detected
    ***
    *** This method is invoked by a map sprite who wishes to check for its own collision.
    *** \See COLLISION_TYPE for more information.
    **/
    COLLISION_TYPE DetectCollision(MapObject* object, float x, float y,
                                   MapObject **collision_object_ptr = nullptr);

    /** \brief Finds a path from a sprite's current position to a destination
    *** \param sprite A pointer of the sprite to find the path for
    *** \param dest The destination coordinates
    *** \param path A vector of PathNode objects storing the path
    *** \param max_cost Tells how far a path node can be computed agains the starting path node.
    *** This is used to avoid heavy computations.
    *** If this param is equal to 0, there is no limitation.
    ***
    *** This algorithm uses the A* algorithm to find a path from a source to a destination.
    *** This function ignores the position of all other objects and only concerns itself with
    *** which map grid elements are walkable.
    ***
    *** \note If an error is detected or a path could not be found, the function will empty the path vector before returning
    **/
    Path FindPath(private_map::VirtualSprite *sprite, const MapPosition &destination, uint32_t max_cost = 0);

    /** \brief Tells the object supervisor that the given sprite pointer
    *** is the party member object.
    *** This later permits to refresh the sprite shown based on the battle
    *** formation front party member.
    **/
    void SetPartyMemberVisibleSprite(private_map::MapSprite *sprite) {
        _visible_party_member = sprite;
    }

    /** Updates the party member sprite based on the first active party member
    *** given by the global Manager.
    **/
    void ReloadVisiblePartyMember();

    /** \brief Changes the state of every registered enemy sprite to 'dead'
    *** Typically used just before a battle begins so that when the player returns to the map, they
    *** are not swarmed by nearby enemies and quickly forced into another battle. This applies to enemies
    *** on all object layers and in any context. Exercise caution when invoking this method.
    **/
    void SetAllEnemyStatesToDead();

    //! \brief Tells whether the collision coords are valid.
    bool IsWithinMapBounds(float x, float y) const;

    //! \brief Tells whether the sprite has got valid collision coordinates.
    bool IsWithinMapBounds(VirtualSprite *sprite) const;

    //! \brief Draw the collision rectangles. Used for debugging purpose.
    void DrawCollisionArea(const MapFrame *frame);

    //! \brief some retrieval functions. These are all const to indicate that
    //! external callers cannot modify the contents of the map_object;

    //! \brief get the number of rows and columns in the collision grid
    void GetGridAxis(uint32_t &x, uint32_t &y) const {
        x = _num_grid_x_axis;
        y = _num_grid_y_axis;
    }

    //! \brief checks to see if the location is a wall for the party or not. The naming is to indicate
    //! that we only check for non-moving objects. IE, characters / NPCs / enemies are not checked
    //! note that treasure boxes, save spots, etc are also skipped
    //! \param x x location on collision grid
    //! \param y y location on collision grid
    //! \return whether the location would be a "wall" for the party or not
    bool IsStaticCollision(float x, float y);

    //! \brief checks if the location on the grid has a simple map collision. This is different from
    //! IsStaticCollision, in that it DOES NOT check static objects, but only the collision value for the map
    bool IsMapCollision(uint32_t x, uint32_t y)
    { return (_collision_grid[y][x] > 0); }

    //! \brief returns a const reference to the ground objects in
    const std::vector<MapObject *>& GetGroundObjects() const
    { return _ground_objects; }

    //! \brief Stops sounds objects such as ambient sounds.
    //! Used when starting a battle for instance.
    void StopSoundObjects();

    //! \brief Restarts sounds objects that were previously stopped.
    //! Used when leaving a battle for instance.
    void RestartSoundObjects();

private:
    //! \brief Returns the nearest save point. Used by FindNearestObject.
    private_map::MapObject *_FindNearestSavePoint(const VirtualSprite *sprite);

    //! \brief Updates save points animation and active state.
    void _UpdateSavePoints();

    //! \brief Updates the ambient sounds volume according to the camera distance.
    void _UpdateAmbientSounds();

    //! \brief Debug: Draws the map zones in orange
    void _DrawMapZones();

    //! \brief Returns the MapObject vector corresponding to the draw layer.
    std::vector<MapObject*>& _GetObjectsFromDrawLayer(MapObjectDrawLayer layer);

    /** \brief The number of rows and columns in the collision grid
    *** The number of collision grid rows and columns is always equal to twice
    *** that of the number of rows and columns of tiles (stored in the TileManager).
    **/
    uint16_t _num_grid_x_axis, _num_grid_y_axis;

    //! \brief Holds the most recently generated object ID number
    uint16_t _last_id;

    /** \brief The party member object is used to keep in memory the active member
    *** seen on map. This is later useful in "dungeon" maps for instance, where
    *** the party member in front of the battle formation is the one shown on map.
    *** Do not create or delete it in the code, this is just a reference.
    **/
    private_map::MapSprite* _visible_party_member;

    /** \brief A 2D vector indicating which grid element on the map sprites may be occupied by objects.
    *** Each bit of each element in this grid corresponds to a context. So all together this entire grid
    *** stores the collision information for all 32 possible map contexts.
    *** \Note A position in this member is stored like this:
    *** _collision_grid[y][x]
    **/
    std::vector<std::vector<uint32_t> > _collision_grid;

    /** \brief A map containing pointers to all of the sprites on a map.
    *** This map does not include a pointer to the _virtual_focus object. The
    *** sprite's unique identifier integer is used as the vector key.
    *** MapObjects should only be deleted here.
    **/
    std::vector<MapObject *> _all_objects;

    /** \brief A container for all of the map objects located on the ground layer, and being flat.
    *** See this layer as a pre ground object layer
    **/
    std::vector<MapObject *> _flat_ground_objects;

    /** \brief A container for all of the map objects located on the ground layer.
    *** The ground object layer is where most objects and sprites exist in a typical map.
    **/
    std::vector<MapObject *> _ground_objects;

    /** \brief A container for all of the map objects located on the pass layer.
    *** The pass object layer is named so because objects on this layer can both be
    *** walked under or above by objects in the ground object layer. A good example
    *** of an object that would typically go on this layer would be a bridge. This
    *** layer usually has very few objects for the map.
    **/
    std::vector<MapObject *> _pass_objects;

    /** \brief A container for all of the map objects located on the sky layer.
    *** The sky object layer contains the last series of elements that are drawn on
    *** a map. These objects exist high in the sky above all other tiles and objects.
    *** Translucent clouds can make good use of this object layer, for instance.
    **/
    std::vector<MapObject *> _sky_objects;

    //! \brief A container for all of the save points, quite similar as the ground objects container.
    std::vector<SavePoint *> _save_points;

    //! \brief Ambient sound objects, that plays a sound with a volume according
    //! to the distance with the camera.
    std::vector<SoundObject *> _sound_objects;

    //! \brief The sound objects that can be restarted when the map is reset()
    std::vector<SoundObject *> _sound_objects_to_restart;

    //! \brief Containers for all of the map source of light, quite similar as the ground objects container.
    std::vector<Halo *> _halos;
    std::vector<Light *> _lights;

    //! \brief Container for all zones used in this map
    std::vector<MapZone *> _zones;
}; // class ObjectSupervisor

} // namespace private_map

} // namespace vt_map

#endif // __MAP_OBJECTS_HEADER__
