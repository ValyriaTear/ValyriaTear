///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_VIRTUAL_SPRITE_HEADER__
#define __MAP_VIRTUAL_SPRITE_HEADER__

#include "modes/map/map_objects/map_object.h"

namespace vt_map
{

namespace private_map
{

class SpriteEvent;

/** ****************************************************************************
*** \brief A special type of sprite with no physical image
***
*** The VirtualSprite is a special type of MapObject because it has no physical
*** form (no image). Virtual sprites may be manipulated to move around on the screen
*** just like any other sprite. VirtualSprites do take collision detection into account
*** by default, unless the no_collision member is set to true. Here are some examples of
*** where virtual sprites may be of use:
***
*** - As a mobile focusing point for the map camera
*** - As an impassible map location for ground objects in a specific context only
*** - To set impassible locations for objects in the sky layer
***
*** \note The VirtualSprite class serves as a base class for all other types of
*** sprites.
*** ***************************************************************************/
class VirtualSprite : public MapObject
{
public:
    explicit VirtualSprite(MapObjectDrawLayer layer);
    virtual ~VirtualSprite() override;

    //! \brief Updates the virtual object's position if it is moving, otherwise does nothing.
    virtual void Update() override;

    //! \brief Does nothing since virtual sprites have no image to draw
    virtual void Draw() override
    {
    }

    /** \note This method takes into account the current direction when setting the new direction
    *** in the case of diagonal movement. For example, if the sprite is currently facing north
    *** and this function indicates that the sprite should move northwest, it will face north
    *** during the northwest movement.
    **/
    void SetDirection(uint16_t dir);

    /** \brief Sets the sprite's direction to a random value
    *** This function is used mostly for the ActionRandomMove class.
    **/
    void SetRandomDirection();

    /**
    *** Make the sprite used the direction making it "look at" the given position or sprite.
    **/
    void LookAt(const MapPosition &pos);
    void LookAt(float x, float y) {
        LookAt(MapPosition(x, y));
    }
    void LookAt(MapObject *object) {
        if(object) LookAt(object->GetPosition());
    }

    /** \brief Calculates the distance the sprite should move given its velocity (speed and direction)
    *** \return A floating point value representing the distance moved
    *** \note This method does not check if the "moving" member is true but does factor in the "is_running"
    *** member in its calculation.
    **/
    float CalculateDistanceMoved();

    /** \brief Declares that an event is taking control over the sprite
    *** \param event The sprite event that is assuming control
    *** This function is not safe to call when there is an event already controlling the sprite.
    *** The previously controlling event should first release control (which will set the control_event
    *** member to nullptr) before a new event acquires it. The acquisition will be successful regardless
    *** of whether there is currently a controlling event or not, but a warning will be printed in the
    *** improper case.
    **/
    void AcquireControl(SpriteEvent* event);

    /** \brief Declares that an event is releasing control over the sprite
    *** \param event The sprite event that is releasing control
    *** The reason why the SpriteEvent has to pass a pointer to itself in this call is to make sure
    *** that this event is still controlling the sprite. If the control has switched to another event
    *** (because another event acquired it before this event released it), a warning will be printed
    *** and no change will be made (the control event will not change).
    **/
    void ReleaseControl(SpriteEvent* event);

    /** \brief Gets the SpriteEvent* currently controlling the sprite
    *** or nullptr is none.
    **/
    SpriteEvent* GetControlEvent() const {
        return _control_event;
    }

    /** \brief Saves the state of the sprite
    *** Attributes saved: direction, speed, moving state
    **/
    virtual void SaveState();

    /** \brief Restores the saved state of the sprite
    *** Attributes restored: direction, speed, moving state
    **/
    virtual void RestoreState();

    /** \name Lua Access Functions
    *** These functions are specifically written to enable Lua to access the members of this class.
    **/
    //@{
    bool IsStateSaved() const {
        return _state_saved;
    }

    void SetMovementSpeed(float speed) {
        _movement_speed = speed;
    }

    void SetMoving(bool motion) {
        _moving = motion;
    }

    bool GetMoving() const {
        return _moving;
    }

    uint16_t GetDirection() const {
        return _direction;
    }

    float GetMovementSpeed() const {
        return _movement_speed;
    }

    void SetRunning(bool running) {
        _is_running = running;
    }

    bool IsRunning() const {
        return _is_running;
    }

    bool HasMoved() const {
        return _moved_position;
    }
    //@}

protected:
    /** \brief A bit-mask for the sprite's draw orientation and direction vector.
    *** This member determines both where to move the sprite (8 directions) and
    *** which way the sprite is facing (4 directions). See the Sprite direction
    *** constants for the values that this member may be set to.
    **/
    uint16_t _direction;

    //! \brief The speed at which the sprite moves around the map.
    float _movement_speed;

    /** \brief Set to true when the sprite is currently in motion.
    *** This does not necessarily mean that the sprite actually is moving, but rather
    *** that the sprite is <i>trying</i> to move in a certain direction.
    **/
    bool _moving;

    /** \brief Set to true whenever the sprite's position was changed due to movement
    *** This is distinctly different than the moving member. Whereas the moving member
    *** indicates desired movement, this member indicates that positional change due to
    *** movement actually occurred. It is used for drawing functions to determine if they
    *** should draw the sprite in motion or not in motion
    **/
    bool _moved_position;

    //! \brief Set to true when the sprite is running rather than walking
    bool _is_running;

    //! \brief A pointer to the event that is controlling the action of this sprite
    SpriteEvent* _control_event;

    /** \name Saved state attributes
    *** These attributes are used to save and restore the state of a VirtualSprite
    **/
    //@{
    //! \brief Indicates if the other saved members are valid because the state has recently been saved
    bool _state_saved;
    uint16_t _saved_direction;
    float _saved_movement_speed;
    bool _saved_moving;
    //@}

    /** \brief Set the next sprite position, according to the current direction set.
    *** This function aims at finding the next correct position for the given sprite,
    *** and avoid the most possible to make it stop, except when walking against a wall.
    **/
    void _SetNextPosition();

    /** \brief Handles position corrections when the sprite is on the edge of
    *** physical obstacles. (NPC sprites, treasure, ... aren't considered here for playability purpose)
    **/
    bool _HandleWallEdges(float& next_pos_x, float& next_pos_y, float distance_moved,
                          MapObject* collision_object);
};

} // namespace private_map

} // namespace vt_map

#endif // __MAP_VIRTUAL_SPRITE_HEADER__
