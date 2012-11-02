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

#include "engine/script/script.h"
#include "engine/video/video.h"

#include "global_skills.h"
#include "global.h"

using namespace hoa_utils;
using namespace hoa_video;
using namespace hoa_script;
using namespace hoa_battle;

namespace hoa_global
{

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
    _target_type(GLOBAL_TARGET_INVALID)
{
    // A pointer to the skill script which will be used to load this skill
    ReadScriptDescriptor *skill_script = NULL;

    if((_id > 0) && (_id <= MAX_ATTACK_ID)) {
        _type = GLOBAL_SKILL_ATTACK;
        skill_script = &(GlobalManager->GetAttackSkillsScript());
    } else if((_id > MAX_ATTACK_ID) && (_id <= MAX_DEFEND_ID)) {
        _type = GLOBAL_SKILL_DEFEND;
        skill_script = &(GlobalManager->GetDefendSkillsScript());
    } else if((_id > MAX_DEFEND_ID) && (_id <= MAX_SUPPORT_ID)) {
        _type = GLOBAL_SKILL_SUPPORT;
        skill_script = &(GlobalManager->GetSupportSkillsScript());
    } else {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "constructor received an invalid id argument: " << id << std::endl;
        _id = 0; // Indicate that this skill is invalid
        return;
    }

    // Load the skill properties from the script
    if(!skill_script->DoesTableExist(_id)) {
        PRINT_WARNING << "No valid data for skill in definition file: " << _id << std::endl;
        _id = 0; // Indicate that this skill is invalid
        return;
    }

    skill_script->OpenTable(_id);
    _name = MakeUnicodeString(skill_script->ReadString("name"));
    if(skill_script->DoesStringExist("description"))
        _description = MakeUnicodeString(skill_script->ReadString("description"));
    _sp_required = skill_script->ReadUInt("sp_required");
    _warmup_time = skill_script->ReadUInt("warmup_time");
    _cooldown_time = skill_script->ReadUInt("cooldown_time");
    _warmup_action_name = skill_script->ReadString("warmup_action_name");
    _action_name = skill_script->ReadString("action_name");
    _target_type = static_cast<GLOBAL_TARGET>(skill_script->ReadInt("target_type"));

    _battle_execute_function = new ScriptObject();
    *_battle_execute_function = skill_script->ReadFunctionPointer("BattleExecute");
    _field_execute_function = new ScriptObject();
    *_field_execute_function = skill_script->ReadFunctionPointer("FieldExecute");

    // Read all the battle animation scripts linked to this skill, if any
    if(skill_script->DoesTableExist("animation_scripts")) {
        std::vector<uint32> characters_ids;
        _animation_scripts.clear();
        skill_script->ReadTableKeys("animation_scripts", characters_ids);
        skill_script->OpenTable("animation_scripts");
        for(uint32 i = 0; i < characters_ids.size(); ++i) {
            _animation_scripts[characters_ids[i]] = skill_script->ReadString(characters_ids[i]);
        }
        skill_script->CloseTable(); // animation_scripts table
    }

    skill_script->CloseTable(); // id.

    if(skill_script->IsErrorDetected()) {
        PRINT_WARNING << "One or more errors occurred while reading skill data - they are listed below:	"
                      << std::endl << skill_script->GetErrorMessages() << std::endl;
        _id = 0; // Indicate that this skill is invalid
    }
} // GlobalSkill::GlobalSkill()

GlobalSkill::GlobalSkill(const GlobalSkill &copy)
{
    _name = copy._name;
    _description = copy._description;
    _id = copy._id;
    _type = copy._type;
    _sp_required = copy._sp_required;
    _warmup_time = copy._warmup_time;
    _cooldown_time = copy._cooldown_time;
    _action_name = copy._action_name;
    _target_type = copy._target_type;

    // Make copies of valid ScriptObject function pointers
    _battle_execute_function = copy._battle_execute_function;
    _field_execute_function = copy._field_execute_function;
    _animation_scripts = copy._animation_scripts;
}



GlobalSkill &GlobalSkill::operator=(const GlobalSkill &copy)
{
    if(this == &copy)  // Handle self-assignment case
        return *this;

    _name = copy._name;
    _description = copy._description;
    _id = copy._id;
    _type = copy._type;
    _sp_required = copy._sp_required;
    _warmup_time = copy._warmup_time;
    _cooldown_time = copy._cooldown_time;
    _action_name = copy._action_name;
    _target_type = copy._target_type;

    // Make copies of valid ScriptObject function pointers
    _battle_execute_function = copy._battle_execute_function;
    _field_execute_function = copy._field_execute_function;
    _animation_scripts = copy._animation_scripts;

    return *this;
}

bool GlobalSkill::ExecuteBattleFunction(private_battle::BattleActor *user, private_battle::BattleTarget target)
{
    if(!_battle_execute_function->is_valid()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "Can't execute invalid battle script function." << std::endl;
        return false;
    }

    try {
        ScriptCallFunction<void>(*_battle_execute_function, user, target);
    } catch(const luabind::error &err) {
        ScriptManager->HandleLuaError(err);
        return false;
    } catch(const luabind::cast_failed &e) {
        ScriptManager->HandleCastError(e);
        return false;
    }
    return true;
}

std::string GlobalSkill::GetAnimationScript(uint32 character_id)
{
    std::string script_file; // Empty by default

    std::map<uint32, std::string>::const_iterator it = _animation_scripts.find(character_id);
    if(it != _animation_scripts.end())
        script_file = it->second;
    return script_file;
}

} // namespace hoa_global
