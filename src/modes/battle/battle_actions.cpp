////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
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
*** \brief   Source file for actions that occur in battles.
*** ***************************************************************************/

#include <iostream>
#include <sstream>

#include "script.h"

#include "battle.h"
#include "battle_actions.h"
#include "battle_actors.h"
#include "battle_utils.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_video;
using namespace hoa_input;
using namespace hoa_system;
using namespace hoa_global;
using namespace hoa_script;

namespace hoa_battle {

namespace private_battle {

////////////////////////////////////////////////////////////////////////////////
// BattleAction class
////////////////////////////////////////////////////////////////////////////////

BattleAction::BattleAction(BattleActor* actor, BattleTarget target) :
	_actor(actor),
	_target(target)
{
	if (actor == NULL)
		IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL actor" << endl;
	if (target.GetType() == GLOBAL_TARGET_INVALID)
		IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received invalid target" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// SkillAction class
////////////////////////////////////////////////////////////////////////////////

SkillAction::SkillAction(BattleActor* actor, BattleTarget target, GlobalSkill* skill) :
	BattleAction(actor, target),
	_skill(skill)
{
	if (skill == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL skill argument" << endl;
		return;
	}

	if (skill->GetTargetType() == GLOBAL_TARGET_INVALID)
		IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received invalid skill" << endl;
	if (skill->GetTargetType() != target.GetType())
		IF_PRINT_WARNING(BATTLE_DEBUG) << "skill and target reference different target types" << endl;
	if (skill->IsExecutableInBattle() == false)
		IF_PRINT_WARNING(BATTLE_DEBUG) << "skill is not executable in battle" << endl;
}



bool SkillAction::Execute() {
	// (1): First check that the actor has sufficient XP to use the skill
	if (_actor->GetSkillPoints() < _skill->GetSPRequired()) {
		// TODO: I think changing state to idle while skipping cool down will not delete the skill, test this
		// TODO: This call actually doesn't work, because another function in BattleActor sets the state to cool-down
		// whenever this function returns true. Need to find a solution to this.
		_actor->ChangeState(ACTOR_STATE_IDLE);
		// TODO: need to indicate the the skill execution failed to the user somehow
		return true;
	}

	// (2): Ensure that the skill will affect a valid target
	if (_target.IsValid() == false) {
		// TEMP: party targets should always be valid and attack points never disappear, so only the actor needs to be changed
// 		if (IsTargetPoint(_target.GetType()) == true)
// 			_target.SelectNextPoint();
// 		else if (IsTargetActor(_target.GetType()) == true)

		// TEMP: this should only be done if the skill has no custom checking for valid targets
		_target.SelectNextActor(_actor);
	}

	// (3): Retrieve and call the execution function of the script
	const ScriptObject* script_function = _skill->GetBattleExecuteFunction();
	if (script_function == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "failed to retrieve execution function" << endl;
		return true;
	}

	try {
		ScriptCallFunction<void>(*script_function, _actor, _target); }
	catch (luabind::error err) {
		ScriptManager->HandleLuaError(err);
	}

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



ustring SkillAction::GetName() const {
	if (_skill == NULL)
		return ustring();
	else
		return _skill->GetName();
}




uint32 SkillAction::GetWarmUpTime() const {
	if (_skill == NULL)
		return 0;
	else
		return _skill->GetWarmupTime();
}



uint32 SkillAction::GetCoolDownTime() const {
	if (_skill == NULL)
		return 0;
	else
		return _skill->GetCooldownTime();
}

////////////////////////////////////////////////////////////////////////////////
// ItemAction class
////////////////////////////////////////////////////////////////////////////////

ItemAction::ItemAction(BattleActor* source, BattleTarget target, BattleItem* item) :
	BattleAction(source, target),
	_item(item)
{
	if (item == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL item argument" << endl;
		return;
	}

	if (item->GetItem().GetTargetType() == GLOBAL_TARGET_INVALID)
		IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received invalid item" << endl;
	if (item->GetItem().GetTargetType() != target.GetType())
		IF_PRINT_WARNING(BATTLE_DEBUG) << "item and target reference different target types" << endl;
	if (item->GetItem().IsUsableInBattle() == false)
		IF_PRINT_WARNING(BATTLE_DEBUG) << "item is not usable in battle" << endl;
}



bool ItemAction::Execute() {
	// TODO: Check that item count > 0?

	const ScriptObject* script_function = _item->GetItem().GetBattleUseFunction();
	if (script_function == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "item did not have a battle use function" << endl;
	}

	try {
		ScriptCallFunction<void>(*script_function, _actor, _target); }
	catch (luabind::error err) {
		ScriptManager->HandleLuaError(err);
	}

	return true;
}



ustring ItemAction::GetName() const {
	if (_item == NULL)
		return UTranslate("Use: [error]");
	else
		return (UTranslate("Use: ") + (_item->GetItem()).GetName());
}

} // namespace private_battle

} // namespace hoa_battle
