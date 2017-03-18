///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \brief   Header file for map mode position
*** *****************************************************************************/

#ifndef __MAP_POSITION_HEADER__
#define __MAP_POSITION_HEADER__

#include <cmath>

namespace vt_map
{

//! \brief A map position in tiles
struct MapPosition {
    MapPosition() :
        x(0.0f),
        y(0.0f)
    {}

    MapPosition(float x_pos, float y_pos) :
        x(x_pos),
        y(y_pos)
    {}

    float length() const {
        return sqrtf(x * x + y * y);
    }

    MapPosition& operator= (const MapPosition& map_pos) {
        // Prevents upon-self copy.
        if (&map_pos == this)
            return *this;

        x = map_pos.x;
        y = map_pos.y;

        return *this;
    }

    float x;
    float y;
};

} // namespace vt_map

#endif // __MAP_POSITION_HEADER__
