///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __POSITION_2D_HEADER__
#define __POSITION_2D_HEADER__

namespace vt_common
{

//! \brief A 2D position
//! Used as map position in tiles
//! Used as skill graph position in pixels, ...
struct Position2D {
    Position2D() :
        x(0.0f),
        y(0.0f)
    {}

    Position2D(float x_pos, float y_pos) :
        x(x_pos),
        y(y_pos)
    {}

    Position2D(const Position2D& other_pos) {
        // Prevents upon-self copy.
        if (&other_pos == this)
            return;

        x = other_pos.x;
        y = other_pos.y;
    }

    //! \brief Get length^2 from origin
    //! We avoid the costy square root call
    //! So we return power of 2 value.
    float GetLength2() const {
        return (x * x + y * y);
    }

    //! \brief Get the distance^2 with another point
    //! We avoid the costy square root call
    //! So we return power of 2 value.
    float GetDistance2(const Position2D& other_pos) const {
        float x2 = other_pos.x - x;
        x2 *= x2;
        float y2 = other_pos.y - y;
        y2 *= y2;
        return x2 + y2;
    }

    Position2D& operator= (const Position2D& other_pos) {
        // Prevents upon-self copy.
        if (&other_pos == this)
            return *this;

        x = other_pos.x;
        y = other_pos.y;

        return *this;
    }

    bool operator == (const Position2D& other_pos) {
        // Handles upon-self test.
        if (&other_pos == this)
            return true;

        return x == other_pos.x && y == other_pos.y;
    }

    float x;
    float y;
};

// Add a useful alias
typedef Position2D Vector2D;

//! \brief A simple line struct
struct Line2D {
    Position2D begin;
    Position2D end;
};

} // namespace vt_common

#endif // __POSITION_2D_HEADER__
