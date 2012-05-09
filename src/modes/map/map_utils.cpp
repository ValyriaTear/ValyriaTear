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

// Local map mode headers
#include "map_utils.h"

using namespace std;

namespace hoa_map {

bool MAP_DEBUG = false;

namespace private_map {

bool MapRectangle::CheckIntersection(const MapRectangle& first, const MapRectangle& second) {
	if ((first.left > second.right) ||
		(first.right < second.left) ||
		(first.top > second.bottom) ||
		(first.bottom < second.top))
		return false;
	else
		return true;
}


uint16 CalculateOppositeDirection(const uint16 direction) {
	switch (direction) {
		case NORTH:      return SOUTH;
		case SOUTH:      return NORTH;
		case WEST:       return EAST;
		case EAST:       return WEST;
		case NW_NORTH:   return SE_SOUTH;
		case NW_WEST:    return SE_EAST;
		case NE_NORTH:   return SW_SOUTH;
		case NE_EAST:    return SW_WEST;
		case SW_SOUTH:   return NE_NORTH;
		case SW_WEST:    return NE_EAST;
		case SE_SOUTH:   return NW_NORTH;
		case SE_EAST:    return NW_WEST;
		default:
			IF_PRINT_WARNING(MAP_DEBUG) << "invalid direction argument: " << direction << endl;
			return SOUTH;
	}
}


MAP_CONTEXT GetContextMaskFromConstextId(uint32 id) {

	switch (id) {
		case 1:
		return MAP_CONTEXT_01;
		case 2:
		return MAP_CONTEXT_02;
		case 3:
		return MAP_CONTEXT_03;
		case 4:
		return MAP_CONTEXT_04;
		case 5:
		return MAP_CONTEXT_05;
		case 6:
		return MAP_CONTEXT_06;
		case 7:
		return MAP_CONTEXT_07;
		case 8:
		return MAP_CONTEXT_08;
		case 9:
		return MAP_CONTEXT_09;
		case 10:
		return MAP_CONTEXT_10;
		case 11:
		return MAP_CONTEXT_11;
		case 12:
		return MAP_CONTEXT_12;
		case 13:
		return MAP_CONTEXT_13;
		case 14:
		return MAP_CONTEXT_14;
		case 15:
		return MAP_CONTEXT_15;
		case 16:
		return MAP_CONTEXT_16;
		case 17:
		return MAP_CONTEXT_17;
		case 18:
		return MAP_CONTEXT_18;
		case 19:
		return MAP_CONTEXT_19;
		case 20:
		return MAP_CONTEXT_20;
		case 21:
		return MAP_CONTEXT_21;
		case 22:
		return MAP_CONTEXT_22;
		case 23:
		return MAP_CONTEXT_23;
		case 24:
		return MAP_CONTEXT_24;
		case 25:
		return MAP_CONTEXT_25;
		case 26:
		return MAP_CONTEXT_26;
		case 27:
		return MAP_CONTEXT_27;
		case 28:
		return MAP_CONTEXT_28;
		case 29:
		return MAP_CONTEXT_29;
		case 30:
		return MAP_CONTEXT_30;
		case 31:
		return MAP_CONTEXT_31;
		case 32:
		return MAP_CONTEXT_32;
	default:
		break;
	}
	return MAP_CONTEXT_NONE;
}

} // namespace private_map

} // namespace hoa_map

