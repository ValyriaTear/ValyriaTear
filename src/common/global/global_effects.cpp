////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2007 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_effects.cpp
*** \author  Jacob Rudolph, rujasu@allacrost.org
*** \brief   Source file for global game effects
*** ***************************************************************************/

#include "script.h"
#include "system.h"

#include "global_effects.h"
#include "global.h"

using namespace std;

using namespace hoa_utils;

using namespace hoa_script;
using namespace hoa_system;

namespace hoa_global {

string GetElementName(GLOBAL_ELEMENTAL type) {
	switch (type) {
		case GLOBAL_ELEMENTAL_FIRE:
			return Translate("Fire");
		case GLOBAL_ELEMENTAL_WATER:
			return Translate("Water");
		case GLOBAL_ELEMENTAL_VOLT:
			return Translate("Volt");
		case GLOBAL_ELEMENTAL_EARTH:
			return Translate("Earth");
		case GLOBAL_ELEMENTAL_SLICING:
			return Translate("Slicing");
		case GLOBAL_ELEMENTAL_SMASHING:
			return Translate("Smashing");
		case GLOBAL_ELEMENTAL_MAULING:
			return Translate("Mauling");
		case GLOBAL_ELEMENTAL_PIERCING:
			return Translate("Piercing");
		default:
			return Translate("Invalid Elemental");
	}
}



string GetStatusName(GLOBAL_STATUS type) {
	string result;
	int32 table_id = static_cast<int32>(type);

	ReadScriptDescriptor& script_file = GlobalManager->GetStatusEffectsScript();
	if (script_file.DoesTableExist(table_id) == true) {
		script_file.OpenTable(table_id);
		if (script_file.DoesStringExist("name") == true) {
			result = script_file.ReadString("name");
		}
		else {
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "Lua definition file contained an entry but no name for status effect: " << type << endl;
		}
		script_file.CloseTable();
	}
	else {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "Lua definition file contained no entry for status effect: " << type << endl;
	}

	if (result == "") {
		result = Translate("Invalid Status");
	}
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// GlobalElementalEffect class
////////////////////////////////////////////////////////////////////////////////

void GlobalElementalEffect::IncrementIntensity(uint8 amount) {
	hoa_global::IncrementIntensity(_intensity, amount);
}



void GlobalElementalEffect::DecrementIntensity(uint8 amount) {
	hoa_global::DecrementIntensity(_intensity, amount);
}

////////////////////////////////////////////////////////////////////////////////
// GlobalStatusEffect class
////////////////////////////////////////////////////////////////////////////////

bool GlobalStatusEffect::IncrementIntensity(uint8 amount) {
	return hoa_global::IncrementIntensity(_intensity, amount);
}



bool GlobalStatusEffect::DecrementIntensity(uint8 amount) {
	GLOBAL_INTENSITY previous_intensity = _intensity;
	bool intensity_modified = hoa_global::DecrementIntensity(_intensity, amount);

	if (intensity_modified == true) {
		if (_intensity < GLOBAL_INTENSITY_NEUTRAL) {
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to decrement intensity below neutral level" << endl;
			_intensity = GLOBAL_INTENSITY_NEUTRAL;
			if (_intensity == previous_intensity)
				intensity_modified = false;
		}
	}
	return intensity_modified;
}

} // namespace hoa_global
