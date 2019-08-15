////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "skill_action.h"

#include "common/global/global_skills.h"

#include "utils/ustring.h"

using namespace vt_global;
using namespace vt_script;
using namespace vt_utils;

namespace vt_battle
{

namespace private_battle
{

SkillAction::SkillAction(BattleActor* actor, BattleTarget target, GlobalSkill *skill) :
    BattleAction(actor, target),
    _skill(skill)
{
    if(skill == nullptr) {
        PRINT_WARNING << "constructor received nullptr skill argument" << std::endl;
        return;
    }

    if(skill->GetTargetType() == GLOBAL_TARGET_INVALID)
        PRINT_WARNING << "constructor received invalid skill" << std::endl;
    if(skill->GetTargetType() != target.GetType())
        PRINT_WARNING << "skill and target reference different target types" << std::endl;
    if(skill->IsExecutableInBattle() == false)
        PRINT_WARNING << "skill is not executable in battle" << std::endl;

    // Check for a custom skill animation script for the given character
    _is_scripted = false;
    std::string animation_script_file = skill->GetAnimationScript(_actor->GetID());

    if(animation_script_file.empty())
        return;

    // Clears out old script data
    std::string tablespace = ScriptEngine::GetTableSpace(animation_script_file);
    ScriptManager->DropGlobalTable(tablespace);

    if(!_anim_script.OpenFile(animation_script_file)) {
        _anim_script.CloseFile();
        return;
    }

    if(_anim_script.OpenTablespace().empty()) {
        PRINT_ERROR << "No namespace found in file: " << animation_script_file << std::endl;
        _anim_script.CloseFile();
        return;
    }

    _init_function = _anim_script.ReadFunctionPointer("Initialize");

    if(!_init_function.is_valid()) {
        _anim_script.CloseFile();
        return;
    }

    // Attempt to load a possible update function.
    _update_function = _anim_script.ReadFunctionPointer("Update");
    _is_scripted = true;
}

SkillAction::~SkillAction()
{
    // Remove reference from the init and update function
    // to permit their deletion when freeing the lua coroutine.
    _init_function = luabind::object();
    _update_function = luabind::object();

    _anim_script.CloseFile();
}

bool SkillAction::ShouldShowSkillNotice() const
{
    if (!_skill)
        return false;

    return _skill->ShouldShowSkillNotice();
}

void SkillAction::_InitAnimationScript()
{
    try {
        luabind::call_function<void>(_init_function, _actor, _target, _skill);
    } catch(const luabind::error &err) {
        ScriptManager->HandleLuaError(err);
        // Fall back to hard-coded mode
        _is_scripted = false;
    } catch(const luabind::cast_failed &e) {
        ScriptManager->HandleCastError(e);
        // Fall back to hard-coded mode
        _is_scripted = false;
    }
}

bool SkillAction::Initialize()
{
    // First check that the actor has sufficient SP to use the skill
    if(_actor->GetSkillPoints() < _skill->GetSPRequired())
        return false;

    // Ensure that the skill will affect a valid target
    if(!_target.IsValid())
        _target.SelectNextActor();

    if(IsScripted())
        _InitAnimationScript();
    return true;
}

void SkillAction::Warmup()
{
    if(IsScripted())
        return;

    _skill->ExecuteBattleWarmupFunction(_actor, _target);
}

bool SkillAction::Execute()
{
    if(IsScripted())
        return false;

    if(!_skill->ExecuteBattleFunction(_actor, _target))
        return false;

    _actor->SubtractSkillPoints(_skill->GetSPRequired());
    return true;
}

bool SkillAction::Update()
{
    // When there is no update function, the animation is done.
    if(!_update_function.is_valid())
        return true;

    try {
        return luabind::call_function<bool>(_update_function);
    } catch(const luabind::error &err) {
        ScriptManager->HandleLuaError(err);
        return true;
    } catch(const luabind::cast_failed &e) {
        ScriptManager->HandleCastError(e);
        return true;
    }

    // Should never happen
    return true;
}

ustring SkillAction::GetName() const
{
    if(_skill == nullptr)
        return ustring();
    else
        return _skill->GetName();
}

std::string SkillAction::GetIconFilename() const
{
    if (!_skill)
        return std::string();

    if (!_skill->GetIconFilename().empty())
        return _skill->GetIconFilename();

    switch (_skill->GetType()) {
    default:
        break;
    case vt_global::GLOBAL_SKILL_WEAPON:
        return std::string("weapon"); // alias used to know the weapon icon needs to used.
    case GLOBAL_SKILL_BARE_HANDS:
        return std::string("data/inventory/weapons/fist-human.png");
    }
    return _skill->GetIconFilename();
}

std::string SkillAction::GetWarmupActionName() const
{
    if(_skill == nullptr)
        return std::string();
    else
        return _skill->GetWarmupActionName();
}

std::string SkillAction::GetActionName() const
{
    if(_skill == nullptr)
        return std::string();
    else
        return _skill->GetActionName();
}

uint32_t SkillAction::GetWarmUpTime() const
{
    if(_skill == nullptr)
        return 0;
    else
        return _skill->GetWarmupTime();
}

uint32_t SkillAction::GetCoolDownTime() const
{
    if(_skill == nullptr)
        return 0;
    else
        return _skill->GetCooldownTime();
}

} // namespace private_battle

} // namespace vt_battle
