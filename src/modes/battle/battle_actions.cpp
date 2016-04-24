////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_action.cpp
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \author  Andy Gardner, chopperdave@allacrost.org
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for actions that occur in battles.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/battle/battle_actions.h"

#include "engine/script/script.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actors.h"
#include "modes/battle/battle_utils.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_video;
using namespace vt_system;
using namespace vt_global;
using namespace vt_script;

namespace vt_battle
{

namespace private_battle
{

////////////////////////////////////////////////////////////////////////////////
// BattleAction class
////////////////////////////////////////////////////////////////////////////////

BattleAction::BattleAction(BattleActor* actor, BattleTarget target) :
    _actor(actor),
    _target(target),
    _is_scripted(false)
{
    if(actor == nullptr)
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received nullptr actor" << std::endl;
    if(target.GetType() == GLOBAL_TARGET_INVALID)
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received invalid target" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
// SkillAction class
////////////////////////////////////////////////////////////////////////////////

SkillAction::SkillAction(BattleActor *actor, BattleTarget target, GlobalSkill *skill) :
    BattleAction(actor, target),
    _skill(skill)
{
    if(skill == nullptr) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received nullptr skill argument" << std::endl;
        return;
    }

    if(skill->GetTargetType() == GLOBAL_TARGET_INVALID)
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received invalid skill" << std::endl;
    if(skill->GetTargetType() != target.GetType())
        IF_PRINT_WARNING(BATTLE_DEBUG) << "skill and target reference different target types" << std::endl;
    if(skill->IsExecutableInBattle() == false)
        IF_PRINT_WARNING(BATTLE_DEBUG) << "skill is not executable in battle" << std::endl;

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

////////////////////////////////////////////////////////////////////////////////
// ItemAction class
////////////////////////////////////////////////////////////////////////////////

ItemAction::ItemAction(BattleActor *source, BattleTarget target, const std::shared_ptr<BattleItem>& item) :
    BattleAction(source, target),
    _item(item),
    _action_canceled(false)
{
    if(item == nullptr) {
        PRINT_WARNING << "Item action without valid item!!" << std::endl;
        return;
    }

    if(item->GetGlobalItem().GetTargetType() == GLOBAL_TARGET_INVALID)
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received invalid item" << std::endl;
    if(item->GetGlobalItem().GetTargetType() != target.GetType())
        IF_PRINT_WARNING(BATTLE_DEBUG) << "item and target reference different target types" << std::endl;
    if(item->GetGlobalItem().IsUsableInBattle() == false)
        IF_PRINT_WARNING(BATTLE_DEBUG) << "item is not usable in battle" << std::endl;

    // Check for a custom skill animation script for the given character
    _is_scripted = false;
    std::string animation_script_file = item->GetGlobalItem().GetAnimationScript();

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

ItemAction::~ItemAction()
{
    // Remove reference from the init and update function
    // to permit their deletion when freeing the lua coroutine.
    _init_function = luabind::object();
    _update_function = luabind::object();

    _anim_script.CloseFile();
}

bool ItemAction::Initialize()
{
    if(IsScripted())
        _InitAnimationScript();
    return true;
}

bool ItemAction::Update()
{
    // When there is no update function, the animation is done.
    if(!_update_function.is_valid())
        return true;

    try {
        return luabind::call_function<bool>(_update_function);
    } catch(const luabind::error& err) {
        ScriptManager->HandleLuaError(err);
        return true;
    } catch(const luabind::cast_failed& e) {
        ScriptManager->HandleCastError(e);
        return true;
    }

    // Should never happen
    return true;
}

bool ItemAction::Execute()
{
    // Note that the battle item is already removed from the item list at that step.
    const luabind::object &script_function = _item->GetGlobalItem().GetBattleUseFunction();
    if(!script_function.is_valid()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "item did not have a battle use function" << std::endl;

        Cancel();
        return false;
    }

    bool ret = false;
    try {
        ret = luabind::call_function<bool>(script_function, _actor, _target);
    } catch(const luabind::error &err) {
        ScriptManager->HandleLuaError(err);
        ret = false;
    } catch(const luabind::cast_failed &e) {
        ScriptManager->HandleCastError(e);
        ret = false;
    }

    // Cancel item action when failed.
    if(!ret)
        Cancel();

    return ret;
}

void ItemAction::Cancel()
{
    if(_action_canceled)
        return;

    // Give the item back in that case
    if (_item)
        _item->IncrementBattleCount();

    // Permit to cancel only once.
    _action_canceled = true;
}

ustring ItemAction::GetName() const
{
    if(_item)
        return _item->GetGlobalItem().GetName();
    return UTranslate("[error]");
}

std::string ItemAction::GetIconFilename() const
{
    if(_item)
        return _item->GetGlobalItem().GetIconImage().GetFilename();
    return std::string();
}

uint32_t ItemAction::GetWarmUpTime() const
{
    if(_item == nullptr)
        return 0;
    else
        return _item->GetWarmUpTime();
}

uint32_t ItemAction::GetCoolDownTime() const
{
    if(_item == nullptr)
        return 0;
    else
        return _item->GetCoolDownTime();
}

void ItemAction::_InitAnimationScript()
{
    try {
        luabind::call_function<void>(_init_function, _actor, _target, _item);
    } catch(const luabind::error& err) {
        ScriptManager->HandleLuaError(err);
        // Fall back to hard-coded mode
        _is_scripted = false;
    } catch(const luabind::cast_failed& e) {
        ScriptManager->HandleCastError(e);
        // Fall back to hard-coded mode
        _is_scripted = false;
    }
}

} // namespace private_battle

} // namespace vt_battle
