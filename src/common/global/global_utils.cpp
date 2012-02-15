////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2007 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_utils.cpp
*** \author  Tyler Olsen, rootslinux@allacrost.org
*** \brief   Source file for global game utility code
*** ***************************************************************************/

#include "defs.h"
#include "utils.h"

#include "system.h"

#include "global.h"
#include "global_objects.h"
#include "global_utils.h"

using namespace std;

using namespace hoa_utils;

using namespace hoa_system;

using namespace hoa_global::private_global;

namespace hoa_global {

string GetTargetText(GLOBAL_TARGET target) {
	switch (target) {
		case GLOBAL_TARGET_SELF_POINT:
			return Translate("Self — Point");
		case GLOBAL_TARGET_ALLY_POINT:
			return Translate("Ally — Point");
		case GLOBAL_TARGET_FOE_POINT:
			return Translate("Foe — Point");
		case GLOBAL_TARGET_SELF:
			return Translate("Self");
		case GLOBAL_TARGET_ALLY:
			return Translate("Ally");
		case GLOBAL_TARGET_FOE:
			return Translate("Foe");
		case GLOBAL_TARGET_ALL_ALLIES:
			return Translate("All Allies");
		case GLOBAL_TARGET_ALL_FOES:
			return Translate("All Foes");
		default:
			return Translate("Invalid Target");
	}
}



bool IsTargetPoint(GLOBAL_TARGET target) {
	if ((target == GLOBAL_TARGET_SELF_POINT) || (target == GLOBAL_TARGET_ALLY_POINT) || (target == GLOBAL_TARGET_FOE_POINT))
		return true;
	else
		return false;
}



bool IsTargetActor(GLOBAL_TARGET target) {
	if ((target == GLOBAL_TARGET_SELF) || (target == GLOBAL_TARGET_ALLY) || (target == GLOBAL_TARGET_FOE))
		return true;
	else
		return false;
}



bool IsTargetParty(GLOBAL_TARGET target) {
	if ((target == GLOBAL_TARGET_ALL_ALLIES) || (target == GLOBAL_TARGET_ALL_FOES))
		return true;
	else
		return false;
}



bool IsTargetSelf(GLOBAL_TARGET target) {
	if ((target == GLOBAL_TARGET_SELF_POINT) || (target == GLOBAL_TARGET_SELF))
		return true;
	else
		return false;
}



bool IsTargetAlly(GLOBAL_TARGET target) {
	if ((target == GLOBAL_TARGET_ALLY_POINT) || (target == GLOBAL_TARGET_ALLY) || (target == GLOBAL_TARGET_ALL_ALLIES))
		return true;
	else
		return false;
}



bool IsTargetFoe(GLOBAL_TARGET target) {
	if ((target == GLOBAL_TARGET_FOE_POINT) || (target == GLOBAL_TARGET_FOE) || (target == GLOBAL_TARGET_ALL_FOES))
		return true;
	else
		return false;
}



GlobalObject* GlobalCreateNewObject(uint32 id, uint32 count) {
	GlobalObject* new_object = NULL;

	if ((id > 0) && (id <= MAX_ITEM_ID))
		new_object = new GlobalItem(id, count);
	else if ((id > MAX_ITEM_ID) && (id <= MAX_WEAPON_ID))
		new_object = new GlobalWeapon(id, count);
	else if ((id > MAX_WEAPON_ID) && (id <= MAX_LEG_ARMOR_ID))
		new_object = new GlobalArmor(id, count);
	else if ((id > MAX_LEG_ARMOR_ID) && (id <= MAX_SHARD_ID))
		new_object = new GlobalShard(id, count);
	else if ((id > MAX_SHARD_ID) && (id <= MAX_KEY_ITEM_ID))
		new_object = new GlobalKeyItem(id, count);
	else
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received an invalid id argument: " << id << endl;

	// If an object was created but its ID was set to NULL, this indicates that the object is invalid
	if ((new_object != NULL) && (new_object->GetID() == 0)) {
		delete new_object;
		new_object = NULL;
	}
	
	return new_object;
}



bool IncrementIntensity(GLOBAL_INTENSITY& intensity, uint8 amount) {
	if (amount == 0)
		return false;
	if ((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_POS_EXTREME))
		return false;

	// This check protects against overflow conditions
	if (amount > (GLOBAL_INTENSITY_TOTAL * 2)) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to increment intensity by an excessive amount: " << amount << endl;
		if (intensity == GLOBAL_INTENSITY_POS_EXTREME) {
			return false;
		}
		else {
			intensity = GLOBAL_INTENSITY_POS_EXTREME;
			return true;
		}
	}

	intensity = GLOBAL_INTENSITY(intensity + amount);
	if (intensity >= GLOBAL_INTENSITY_TOTAL)
		intensity = GLOBAL_INTENSITY_POS_EXTREME;
	return true;
}



bool DecrementIntensity(GLOBAL_INTENSITY& intensity, uint8 amount) {
	if (amount == 0)
		return false;
	if ((intensity <= GLOBAL_INTENSITY_NEG_EXTREME) || (intensity >= GLOBAL_INTENSITY_TOTAL))
		return false;

	// This check protects against overflow conditions
	if (amount > (GLOBAL_INTENSITY_TOTAL * 2)) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to decrement intensity by an excessive amount: " << amount << endl;
		if (intensity == GLOBAL_INTENSITY_NEG_EXTREME) {
			return false;
		}
		else {
			intensity = GLOBAL_INTENSITY_NEG_EXTREME;
			return true;
		}
	}

	intensity = GLOBAL_INTENSITY(intensity - amount);
	if (intensity <= GLOBAL_INTENSITY_INVALID)
		intensity = GLOBAL_INTENSITY_NEG_EXTREME;
	return true;
}

} // namespace hoa_global
