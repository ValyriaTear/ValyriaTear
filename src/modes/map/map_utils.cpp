///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_utils.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for map mode utility code
*** *****************************************************************************/

#include "utils/utils_pch.h"
#include "map_utils.h"

namespace vt_map
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

} // namespace vt_map

