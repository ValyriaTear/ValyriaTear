////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_OBJECT_HEADER__
#define __BATTLE_OBJECT_HEADER__

#include "common/position_2d.h"

namespace vt_battle
{

namespace private_battle
{

/** \brief An abstract class for representing an object in the battle
*** Used to properly draw objects based on their Y coordinate.
**/
class BattleObject
{
public:
    BattleObject():
        _origin(0.0f, 0.0f),
        _location(0.0f, 0.0f)
    {}
    virtual ~BattleObject()
    {}

    float GetXOrigin() const {
        return _origin.x;
    }

    float GetYOrigin() const {
        return _origin.y;
    }

    float GetXLocation() const {
        return _location.x;
    }

    float GetYLocation() const {
        return _location.y;
    }

    void SetXOrigin(float x_origin) {
        _origin.x = x_origin;
    }

    void SetYOrigin(float y_origin) {
        _origin.y = y_origin;
    }

    void SetXLocation(float x_location) {
        _location.x = x_location;
    }

    void SetYLocation(float y_location) {
        _location.y = y_location;
    }

    virtual void DrawSprite()
    {}

    //! \brief Tells whether the object can be removed from memory.
    //! \note Only visual effects are throwable once used.
    virtual bool CanBeRemoved() const {
        return false;
    }

    virtual void Update()
    {}

protected:
    //! \brief The "home" coordinates for the actor's default location on the battle field
    vt_common::Position2D _origin;

    //! \brief The x and y coordinates of the actor's current location on the battle field
    vt_common::Position2D _location;
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_OBJECT_HEADER__
