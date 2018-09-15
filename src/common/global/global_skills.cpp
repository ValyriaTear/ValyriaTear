////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_skills.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for global game skills.
*** ***************************************************************************/

#include "global_skills.h"

#include "modes/battle/battle_target.h"

#include "script/script.h"
#include "engine/video/video.h"

#include "global.h"

using namespace vt_utils;
using namespace vt_video;
using namespace vt_script;
using namespace vt_battle;

namespace vt_battle {
extern bool BATTLE_DEBUG;
}

namespace vt_global
{

//using namespace private_global;

GlobalSkill::GlobalSkill(uint32_t id) :
    _id(id),
    _show_skill_notice(false),
    _type(GLOBAL_SKILL_INVALID),
    _sp_required(0),
    _warmup_time(0),
    _cooldown_time(0),
    _target_type(GLOBAL_TARGET_INVALID)
{
    // A pointer to the skill script which will be used to load this skill
    ReadScriptDescriptor *skill_script = nullptr;

    if((_id > 0) && (_id <= MAX_WEAPON_SKILL_ID)) {
        _type = GLOBAL_SKILL_WEAPON;
        skill_script = &(GlobalManager->GetWeaponSkillsScript());
    } else if((_id > MAX_WEAPON_SKILL_ID) && (_id <= MAX_MAGIC_SKILL_ID)) {
        _type = GLOBAL_SKILL_MAGIC;
        skill_script = &(GlobalManager->GetMagicSkillsScript());
    } else if((_id > MAX_MAGIC_SKILL_ID) && (_id <= MAX_SPECIAL_SKILL_ID)) {
        _type = GLOBAL_SKILL_SPECIAL;
        skill_script = &(GlobalManager->GetSpecialSkillsScript());
    } else if((_id > MAX_SPECIAL_SKILL_ID) && (_id <= MAX_BARE_HANDS_SKILL_ID)) {
        _type = GLOBAL_SKILL_BARE_HANDS;
        skill_script = &(GlobalManager->GetBareHandsSkillsScript());
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
    if(skill_script->DoesStringExist("icon"))
        _icon_filename = skill_script->ReadString("icon");
    if(skill_script->DoesBoolExist("show_notice"))
        _show_skill_notice = skill_script->ReadBool("show_notice");
    _sp_required = skill_script->ReadUInt("sp_required");
    _warmup_time = skill_script->ReadUInt("warmup_time");
    _cooldown_time = skill_script->ReadUInt("cooldown_time");
    _warmup_action_name = skill_script->ReadString("warmup_action_name");
    _action_name = skill_script->ReadString("action_name");
    _target_type = static_cast<GLOBAL_TARGET>(skill_script->ReadInt("target_type"));

    _battle_execute_function = skill_script->ReadFunctionPointer("BattleExecute");
    _field_execute_function = skill_script->ReadFunctionPointer("FieldExecute");

    // Read all the battle animation scripts linked to this skill, if any
    if(skill_script->DoesTableExist("animation_scripts")) {
        std::vector<uint32_t> characters_ids;
        _animation_scripts.clear();
        skill_script->ReadTableKeys("animation_scripts", characters_ids);
        skill_script->OpenTable("animation_scripts");
        for(uint32_t i = 0; i < characters_ids.size(); ++i) {
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
}

GlobalSkill::GlobalSkill(const GlobalSkill& copy):
    _icon_filename(copy._icon_filename),
    _show_skill_notice(false),
    _warmup_action_name(copy._warmup_action_name),
    _action_name(copy._action_name),
    _animation_scripts(copy._animation_scripts)
{
    _name = copy._name;
    _description = copy._description;
    _id = copy._id;
    _type = copy._type;
    _sp_required = copy._sp_required;
    _warmup_time = copy._warmup_time;
    _cooldown_time = copy._cooldown_time;
    _target_type = copy._target_type;

    // Make copies of valid luabind::object function pointers
    _battle_execute_function = copy._battle_execute_function;
    _field_execute_function = copy._field_execute_function;
}

GlobalSkill &GlobalSkill::operator=(const GlobalSkill& copy)
{
    if(this == &copy)  // Handle self-assignment case
        return *this;

    _name = copy._name;
    _description = copy._description;
    _icon_filename = copy._icon_filename;
    _id = copy._id;
    _show_skill_notice = copy._show_skill_notice;
    _type = copy._type;
    _sp_required = copy._sp_required;
    _warmup_time = copy._warmup_time;
    _cooldown_time = copy._cooldown_time;
    _warmup_action_name = copy._warmup_action_name;
    _action_name = copy._action_name;
    _target_type = copy._target_type;

    // Make copies of valid luabind::object function pointers
    _battle_execute_function = copy._battle_execute_function;
    _field_execute_function = copy._field_execute_function;
    _animation_scripts = copy._animation_scripts;

    return *this;
}

bool GlobalSkill::ExecuteBattleFunction(private_battle::BattleActor* battle_actor,
                                        private_battle::BattleTarget target)
{
    if(!_battle_execute_function.is_valid()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "Can't execute invalid battle script function." << std::endl;
        return false;
    }

    try {
        luabind::call_function<void>(_battle_execute_function, battle_actor, target);
    } catch(const luabind::error& err) {
        ScriptManager->HandleLuaError(err);
        return false;
    } catch(const luabind::cast_failed& e) {
        ScriptManager->HandleCastError(e);
        return false;
    }
    return true;
}

std::string GlobalSkill::GetAnimationScript(uint32_t character_id)
{
    std::string script_file; // Empty by default

    std::map<uint32_t, std::string>::const_iterator it = _animation_scripts.find(character_id);
    if(it != _animation_scripts.end())
        script_file = it->second;
    return script_file;
}

} // namespace vt_global
