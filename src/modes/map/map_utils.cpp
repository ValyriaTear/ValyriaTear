///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_utils.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for map mode utility code
*** *****************************************************************************/

#include "map_utils.h"

namespace hoa_map
{

bool MAP_DEBUG = false;

namespace private_map
{

bool MapRectangle::CheckIntersection(const MapRectangle &first, const MapRectangle &second)
{
    if((first.left > second.right) ||
            (first.right < second.left) ||
            (first.top > second.bottom) ||
            (first.bottom < second.top))
        return false;
    else
        return true;
}

} // namespace private_map

} // namespace hoa_map

