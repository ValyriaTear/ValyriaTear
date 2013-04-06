////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
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

#include <iostream>
#include <sstream>

#include "engine/script/script.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actions.h"
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

BattleAction::BattleAction(BattleActor *actor, BattleTarget target) :
    _actor(actor),
    _target(target)
{
    if(actor == NULL)
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL actor" << std::endl;
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
    if(skill == NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL skill argument" << std::endl;
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

    ReadScriptDescriptor anim_script;
    if(!anim_script.OpenFile(animation_script_file)) {
        anim_script.CloseFile();
        return;
    }

    if(anim_script.OpenTablespace().empty()) {
        PRINT_ERROR << "No namespace found in file: " << animation_script_file << std::endl;
        anim_script.CloseFile();
        return;
    }

    _init_function = anim_script.ReadFunctionPointer("Initialize");

    if(!_init_function.is_valid()) {
        anim_script.CloseFile();
        return;
    }

    // Attempt to load a possible update function.
    _update_function = anim_script.ReadFunctionPointer("Update");
    _is_scripted = true;
    anim_script.CloseFile();
}

void SkillAction::_InitAnimationScript()
{
    try {
        ScriptCallFunction<void>(_init_function, _actor, _target, _skill);
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
    // First check that the actor has sufficient XP to use the skill
    if(_actor->GetSkillPoints() < _skill->GetSPRequired())
        return false;

    // Ensure that the skill will affect a valid target
    if(!_target.IsValid()) {
        // TEMP: party targets should always be valid and attack points never disappear, so only the actor needs to be changed
// 		if (IsTargetPoint(_target.GetType()) == true)
// 			_target.SelectNextPoint();
// 		else if (IsTargetActor(_target.GetType()) == true)

        // TEMP: this should only be done if the skill has no custom checking for valid targets
        _target.SelectNextActor(_actor);
    }

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

// 	if (_target->GetType() == GLOBAL_TARGET_PARTY) {
    // TODO: loop through _target.GetParty() and apply the function
// 		if (_target->IsEnemy()) {
// 			BattleEnemy* enemy;
// 			//Loop through all enemies and apply the item
// 			for (uint32 i = 0; i < BattleMode::CurrentInstance()->GetNumberOfEnemies(); i++) {
// 				enemy = BattleMode::CurrentInstance()->GetEnemyActorAt(i);
// 				ScriptCallFunction<void>(*script_function, enemy, _source);
// 			}
// 		}
// 		else { // Target is a character
// 			BattleCharacter* character;
// 			//Loop through all party members and apply
// 			for (uint32 i = 0; i < BattleMode::CurrentInstance()->GetNumberOfCharacters(); i++) {
// 				character = BattleMode::CurrentInstance()->GetPlayerCharacterAt(i);
// 				ScriptCallFunction<void>(*script_function, character, _source);
// 			}
// 		}
// 	}
// 	else {
//
// 	}

    _actor->SubtractSkillPoints(_skill->GetSPRequired());
    return true;
}

bool SkillAction::Update()
{
    // When there is no update function, the animation is done.
    if(!_update_function.is_valid())
        return true;

    try {
        return ScriptCallFunction<bool>(_update_function);
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
    if(_skill == NULL)
        return ustring();
    else
        return _skill->GetName();
}

std::string SkillAction::GetWarmupActionName() const
{
    if(_skill == NULL)
        return std::string();
    else
        return _skill->GetWarmupActionName();
}

std::string SkillAction::GetActionName() const
{
    if(_skill == NULL)
        return std::string();
    else
        return _skill->GetActionName();
}

uint32 SkillAction::GetWarmUpTime() const
{
    if(_skill == NULL)
        return 0;
    else
        return _skill->GetWarmupTime();
}

uint32 SkillAction::GetCoolDownTime() const
{
    if(_skill == NULL)
        return 0;
    else
        return _skill->GetCooldownTime();
}

////////////////////////////////////////////////////////////////////////////////
// ItemAction class
////////////////////////////////////////////////////////////////////////////////

ItemAction::ItemAction(BattleActor *source, BattleTarget target, BattleItem *item) :
    BattleAction(source, target),
    _item(item),
    _action_canceled(false)
{
    if(item == NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL item argument" << std::endl;
        return;
    }

    if(item->GetItem().GetTargetType() == GLOBAL_TARGET_INVALID)
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received invalid item" << std::endl;
    if(item->GetItem().GetTargetType() != target.GetType())
        IF_PRINT_WARNING(BATTLE_DEBUG) << "item and target reference different target types" << std::endl;
    if(item->GetItem().IsUsableInBattle() == false)
        IF_PRINT_WARNING(BATTLE_DEBUG) << "item is not usable in battle" << std::endl;
}



bool ItemAction::Execute()
{
    // Note that the battle item is already removed from the item list at that
    // step.

    const ScriptObject &script_function = _item->GetItem().GetBattleUseFunction();
    bool ret = false;
    if(!script_function.is_valid()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "item did not have a battle use function" << std::endl;
    }

    try {
        ret = ScriptCallFunction<bool>(script_function, _actor, _target);
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
    _item->IncrementBattleCount();

    // Permit to cancel only once.
    _action_canceled = true;
}

ustring ItemAction::GetName() const
{
    if(_item == NULL)
        return UTranslate("Use: [error]");
    else
        return (UTranslate("Use: ") + (_item->GetItem()).GetName());
}

uint32 ItemAction::GetWarmUpTime() const
{
    if(_item == NULL)
        return 0;
    else
        return _item->GetWarmUpTime();
}

uint32 ItemAction::GetCoolDownTime() const
{
    if(_item == NULL)
        return 0;
    else
        return _item->GetCoolDownTime();
}

} // namespace private_battle

} // namespace vt_battle
