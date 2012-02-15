////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_skills.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for global game skills.
*** ***************************************************************************/

#include "script.h"
#include "video.h"

#include "global_skills.h"
#include "global.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_video;
using namespace hoa_script;


namespace hoa_global {

using namespace private_global;

////////////////////////////////////////////////////////////////////////////////
// GlobalSkill class
////////////////////////////////////////////////////////////////////////////////

GlobalSkill::GlobalSkill(uint32 id) :
	_id(id),
	_type(GLOBAL_SKILL_INVALID),
	_sp_required(0),
	_warmup_time(0),
	_cooldown_time(0),
	_target_type(GLOBAL_TARGET_INVALID),
	_battle_execute_function(NULL),
	_field_execute_function(NULL)
{
	// A pointer to the skill script which will be used to load this skill
	ReadScriptDescriptor *skill_script = NULL;

	if ((_id > 0) && (_id <= MAX_ATTACK_ID)) {
		_type = GLOBAL_SKILL_ATTACK;
		skill_script = &(GlobalManager->GetAttackSkillsScript());
	}
	else if ((_id > MAX_ATTACK_ID) && (_id <= MAX_DEFEND_ID)) {
		_type = GLOBAL_SKILL_DEFEND;
		skill_script = &(GlobalManager->GetDefendSkillsScript());
	}
	else if ((_id > MAX_DEFEND_ID) && (_id <= MAX_SUPPORT_ID)) {
		_type = GLOBAL_SKILL_SUPPORT;
		skill_script = &(GlobalManager->GetSupportSkillsScript());
	}
	else {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "constructor received an invalid id argument: " << id << endl;
		_id = 0; // Indicate that this skill is invalid
		return;
	}

	// Load the skill properties from the script
	if (skill_script->DoesTableExist(_id) == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "no valid data for skill in definition file: " << _id << endl;
		_id = 0; // Indicate that this skill is invalid
		return;
	}

	skill_script->OpenTable(_id);
	_name = MakeUnicodeString(skill_script->ReadString("name"));
	if (skill_script->DoesStringExist("description"))
		_description = MakeUnicodeString(skill_script->ReadString("description"));
	_sp_required = skill_script->ReadUInt("sp_required");
	_warmup_time = skill_script->ReadUInt("warmup_time");
	_cooldown_time = skill_script->ReadUInt("cooldown_time");
	_target_type = static_cast<GLOBAL_TARGET>(skill_script->ReadInt("target_type"));

	if (skill_script->DoesFunctionExist("BattleExecute")) {
		_battle_execute_function = new ScriptObject();
		*_battle_execute_function = skill_script->ReadFunctionPointer("BattleExecute");
	}
	if (skill_script->DoesFunctionExist("FieldExecute")) {
		_field_execute_function = new ScriptObject();
		*_field_execute_function = skill_script->ReadFunctionPointer("FieldExecute");
	}

	skill_script->CloseTable();
	if (skill_script->IsErrorDetected()) {
		if (GLOBAL_DEBUG) {
			PRINT_WARNING << "one or more errors occurred while reading skill data - they are listed below" << endl;
			cerr << skill_script->GetErrorMessages() << endl;
		}
		_id = 0; // Indicate that this skill is invalid
	}
} // GlobalSkill::GlobalSkill()



GlobalSkill::~GlobalSkill() {
	if (_battle_execute_function != NULL) {
		delete _battle_execute_function;
		_battle_execute_function = NULL;
	}

	if (_field_execute_function != NULL) {
		delete _field_execute_function;
		_field_execute_function = NULL;
	}
}



GlobalSkill::GlobalSkill(const GlobalSkill& copy) {
	_name = copy._name;
	_description = copy._description;
	_id = copy._id;
	_type = copy._type;
	_sp_required = copy._sp_required;
	_warmup_time = copy._warmup_time;
	_cooldown_time = copy._cooldown_time;
	_target_type = copy._target_type;

	// Make copies of valid ScriptObject function pointers
	if (copy._battle_execute_function == NULL)
		_battle_execute_function = NULL;
	else
		_battle_execute_function = new ScriptObject(*copy._battle_execute_function);

	if (copy._field_execute_function == NULL)
		_field_execute_function = NULL;
	else
		_field_execute_function = new ScriptObject(*copy._field_execute_function);
}



GlobalSkill& GlobalSkill::operator=(const GlobalSkill& copy) {
	if (this == &copy) // Handle self-assignment case
		return *this;

	_name = copy._name;
	_description = copy._description;
	_id = copy._id;
	_type = copy._type;
	_sp_required = copy._sp_required;
	_warmup_time = copy._warmup_time;
	_cooldown_time = copy._cooldown_time;
	_target_type = copy._target_type;

	// Make copies of valid ScriptObject function pointers
	if (copy._battle_execute_function == NULL)
		_battle_execute_function = NULL;
	else
		_battle_execute_function = new ScriptObject(*copy._battle_execute_function);

	if (copy._field_execute_function == NULL)
		_field_execute_function = NULL;
	else
		_field_execute_function = new ScriptObject(*copy._field_execute_function);

	return *this;
}

} // namespace hoa_global
