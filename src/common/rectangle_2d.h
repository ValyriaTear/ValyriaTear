///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef RECTANGLE_2D_H
#define RECTANGLE_2D_H

#include "position_2d.h"

namespace vt_common
{

/** ****************************************************************************
*** \brief Represents a rectangular section
***
*** This is a small class that is used to represent rectangular areas. These
*** These areas are used very frequently throughout the map code to check for collision
*** detection, determining objects that are within a certain radius of one
*** another, etc.
*** ***************************************************************************/
class Rectangle2D
{
public:
    Rectangle2D() :
        left(0.0f), right(0.0f), top(0.0f), bottom(0.0f)
    {}

    Rectangle2D(float l, float r, float t, float b) :
        left(l), right(r), top(t), bottom(b)
    {}

    //! \brief The four edges of the rectangle's area
    float left, right, top, bottom;

    /** \brief Determines whether the rectangle intersects with another
    *** \param other_rect A reference to the second rectangle object
    *** \return True if the two rectangles intersect at any location.
    **/
    bool IntersectsWith(const Rectangle2D& other_rect) const
    {
        return !(left   > other_rect.right  ||
                 right  < other_rect.left   ||
                 top    > other_rect.bottom ||
                 bottom < other_rect.top);
    }

    /** \brief Determines whether the rectangle contains the point
    *** \param pos position to test
    *** \return True if the rectangle contains the point.
    **/
    bool Contains(const Position2D& pos) const
    {
        return !(left   > pos.x  ||
                 right  < pos.x   ||
                 top    > pos.y ||
                 bottom < pos.y);
    }
};

} // namespace vt_common

#endif // RECTANGLE_2D_H
