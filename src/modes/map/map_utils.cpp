///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** ***************************************************************************/

#include "common/include_pch.h"
#include "map_utils.h"

#include "utils/utils_common.h"

namespace vt_map
{

bool MAP_DEBUG = false;

namespace private_map
{

uint16_t GetOppositeDirection(const uint16_t direction)
{
    switch(direction) {
    case NORTH:
        return SOUTH;
    case SOUTH:
        return NORTH;
    case WEST:
        return EAST;
    case EAST:
        return WEST;
    case NW_NORTH:
        return SE_SOUTH;
    case NW_WEST:
        return SE_EAST;
    case NE_NORTH:
        return SW_SOUTH;
    case NE_EAST:
        return SW_WEST;
    case SW_SOUTH:
        return NE_NORTH;
    case SW_WEST:
        return NE_EAST;
    case SE_SOUTH:
        return NW_NORTH;
    case SE_EAST:
        return NW_WEST;
    default:
        IF_PRINT_WARNING(MAP_DEBUG) << "invalid direction argument: "
                                    << direction << std::endl;
        return SOUTH;
    }
}

} // namespace private_map

} // namespace vt_map
