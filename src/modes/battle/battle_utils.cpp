///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_utils.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for battle mode utility code
***
*** This file contains utility code that is shared among the various battle mode
*** classes.
*** ***************************************************************************/

#include "defs.h"
#include "utils.h"

#include "common/global/global.h"

#include "engine/system.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actors.h"
#include "modes/battle/battle_utils.h"

using namespace hoa_utils;
using namespace hoa_system;
using namespace hoa_global;

namespace hoa_battle {

namespace private_battle {

////////////////////////////////////////////////////////////////////////////////
// Standard battle calculation functions
////////////////////////////////////////////////////////////////////////////////

bool CalculateStandardEvasion(BattleTarget* target) {
	return CalculateStandardEvasionAdder(target, 0.0f);
}



bool CalculateStandardEvasionAdder(BattleTarget* target, float add_eva) {
	if (target == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL target argument" << std::endl;
		return false;
	}
	if (IsTargetParty(target->GetType()) == true) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "target was a party type: " << target->GetType() << std::endl;
		return false;
	}

	float evasion = 0.0f;
	if (IsTargetPoint(target->GetType()) == true) {
		evasion = target->GetActor()->GetAttackPoint(target->GetPoint())->GetTotalEvadeRating();
	}
	else if (IsTargetActor(target->GetType()) == true) {
		evasion = target->GetActor()->TotalEvadeRating();
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << target->GetType() << std::endl;
		return false;
	}

	evasion += add_eva;

	// Check for absolute hit/miss conditions
	if (evasion <= 0.0f)
		return false;
	else if (evasion >= 100.0f)
		return true;

	if (RandomFloat(0.0f, 100.0f) <= evasion)
		return true;
	else
		return false;
} // bool CalculateStandardEvasionAdder(BattleTarget* target, float add_evade)



bool CalculateStandardEvasionMultiplier(BattleTarget* target, float mul_eva) {
	if (target == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL target argument" << std::endl;
		return false;
	}
	if (IsTargetParty(target->GetType()) == true) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "target was a party type: " << target->GetType() << std::endl;
		return false;
	}
	if (mul_eva < 0.0f) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received negative multiplier argument: " << mul_eva << std::endl;
		mul_eva = fabs(mul_eva);
	}

	// Find the base evasion and apply the multiplier
	float evasion = 0.0f;
	if (IsTargetPoint(target->GetType()) == true) {
		evasion = target->GetActor()->GetAttackPoint(target->GetPoint())->GetTotalEvadeRating();
	}
	else if (IsTargetActor(target->GetType()) == true) {
		evasion = target->GetActor()->TotalEvadeRating();
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << target->GetType() << std::endl;
		return false;
	}

	evasion = evasion * mul_eva;

	// Check for absolute hit/miss conditions
	if (evasion <= 0.0f)
		return false;
	else if (evasion >= 100.0f)
		return true;

	if (RandomFloat(0.0f, 100.0f) > evasion)
		return false;
	else
		return true;
} // bool CalculateStandardEvasionMultiplier(BattleTarget* target, float mul_evade)



uint32 CalculatePhysicalDamage(BattleActor* attacker, BattleTarget* target) {
	return CalculatePhysicalDamageAdder(attacker, target, 0, 0.10f);
}



uint32 CalculatePhysicalDamage(BattleActor* attacker, BattleTarget* target, float std_dev) {
	return CalculatePhysicalDamageAdder(attacker, target, 0, std_dev);
}



uint32 CalculatePhysicalDamageAdder(BattleActor* attacker, BattleTarget* target, int32 add_atk) {
	return CalculatePhysicalDamageAdder(attacker, target, add_atk, 0.10f);
}



uint32 CalculatePhysicalDamageAdder(BattleActor* attacker, BattleTarget* target, int32 add_atk, float std_dev) {
	if (attacker == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL attacker argument" << std::endl;
		return 0;
	}
	if (target == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL target argument" << std::endl;
		return 0;
	}
	if (IsTargetParty(target->GetType()) == true) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "target was a party type: " << target->GetType() << std::endl;
		return 0;
	}
	if (std_dev < 0.0f) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received negative standard deviation argument: " << std_dev << std::endl;
		std_dev = fabs(std_dev);
	}

	// Holds the total physical attack of the attacker and modifier
	int32 total_phys_atk = attacker->GetTotalPhysicalAttack() + add_atk;
	if (total_phys_atk < 0)
		total_phys_atk = 0;

	// Holds the total physical defense of the target
	int32 total_phys_def = 0;

	if (IsTargetPoint(target->GetType()) == true) {
		total_phys_def = target->GetActor()->GetAttackPoint(target->GetPoint())->GetTotalPhysicalDefense();
	}
	else if (IsTargetActor(target->GetType()) == true) {
		total_phys_def = target->GetActor()->TotalPhysicalDefense();
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << target->GetType() << std::endl;
		return 0;
	}

	// Holds the total damage dealt
	int32 total_dmg = total_phys_atk - total_phys_def;

	// If the total damage is zero, fall back to causing a small non-zero damage value
	if (total_dmg <= 0)
		return static_cast<uint32>(RandomBoundedInteger(1, 5));

	// Holds the absolute standard deviation used in the GaussianRandomValue function
	float abs_std_dev = 0.0f;
	abs_std_dev = static_cast<float>(total_dmg) * std_dev;
	total_dmg = GaussianRandomValue(total_dmg, abs_std_dev, false);

	// If the total damage came to a value less than or equal to zero after the gaussian randomization,
	// fall back to returning a small non-zero damage value
	if (total_dmg <= 0)
		return static_cast<uint32>(RandomBoundedInteger(1, 5));

	return static_cast<uint32>(total_dmg);
} // uint32 CalculatePhysicalDamageAdder(BattleActor* attacker, BattleTarget* target, int32 add_atk, float std_dev)



uint32 CalculatePhysicalDamageMultiplier(BattleActor* attacker, BattleTarget* target, float mul_atk) {
	return CalculatePhysicalDamageMultiplier(attacker, target, mul_atk, 0.10f);
}



uint32 CalculatePhysicalDamageMultiplier(BattleActor* attacker, BattleTarget* target, float mul_atk, float std_dev) {
	if (attacker == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL attacker argument" << std::endl;
		return 0;
	}
	if (target == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL target argument" << std::endl;
		return 0;
	}
	if (IsTargetParty(target->GetType()) == true) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "target was a party type: " << target->GetType() << std::endl;
		return 0;
	}
	if (mul_atk < 0.0f) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received negative multiplier arument: " << mul_atk << std::endl;
		mul_atk = fabs(mul_atk);
	}
	if (std_dev < 0.0f) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received negative standard deviation argument: " << std_dev << std::endl;
		std_dev = fabs(std_dev);
	}

	// Retrieve the total physical attack of the attacker and apply the modifier
	int32 total_phys_atk = attacker->GetTotalPhysicalAttack();
	total_phys_atk = static_cast<int32>(static_cast<float>(total_phys_atk) * mul_atk);

	if (total_phys_atk < 0)
		total_phys_atk = 0;

	// Holds the total physical defense of the target
	int32 total_phys_def = 0;

	if (IsTargetPoint(target->GetType()) == true) {
		total_phys_def = target->GetActor()->GetAttackPoint(target->GetPoint())->GetTotalPhysicalDefense();
	}
	else if (IsTargetActor(target->GetType()) == true) {
		total_phys_def = target->GetActor()->TotalPhysicalDefense();
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << target->GetType() << std::endl;
		return 0;
	}

	// Holds the total damage dealt
	int32 total_dmg = total_phys_atk - total_phys_def;

	// If the total damage is zero, fall back to causing a small non-zero damage value
	if (total_dmg <= 0)
		return static_cast<uint32>(RandomBoundedInteger(1, 5));

	// Holds the absolute standard deviation used in the GaussianRandomValue function
	float abs_std_dev = 0.0f;
	// A value of "0.075f" means the standard deviation should be 7.5% of the mean (the total damage)
	abs_std_dev = static_cast<float>(total_dmg) * std_dev;
	total_dmg = GaussianRandomValue(total_dmg, abs_std_dev, false);

	// If the total damage came to a value less than or equal to zero after the gaussian randomization,
	// fall back to returning a small non-zero damage value
	if (total_dmg <= 0)
		return static_cast<uint32>(RandomBoundedInteger(1, 5));

	return static_cast<uint32>(total_dmg);
} // uint32 CalculatePhysicalDamageMultiplier(BattleActor* attacker, BattleTarget* target, float mul_phys, float std_dev)



uint32 CalculateMetaphysicalDamage(BattleActor* attacker, BattleTarget* target) {
	return CalculateMetaphysicalDamageAdder(attacker, target, 0, 0.10f);
}



uint32 CalculateMetaphysicalDamage(BattleActor* attacker, BattleTarget* target, float std_dev) {
	return CalculateMetaphysicalDamageAdder(attacker, target, 0, std_dev);
}



uint32 CalculateMetaphysicalDamageAdder(BattleActor* attacker, BattleTarget* target, int32 add_atk) {
	return CalculateMetaphysicalDamageAdder(attacker, target, add_atk, 0.10f);
}



uint32 CalculateMetaphysicalDamageAdder(BattleActor* attacker, BattleTarget* target, int32 add_atk, float std_dev) {
	if (attacker == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL attacker argument" << std::endl;
		return 0;
	}
	if (target == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL target argument" << std::endl;
		return 0;
	}
	if (IsTargetParty(target->GetType()) == true) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "target was a party type: " << target->GetType() << std::endl;
		return 0;
	}
	if (std_dev < 0.0f) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received negative standard deviation argument: " << std_dev << std::endl;
		std_dev = fabs(std_dev);
	}

	// Holds the total physical attack of the attacker and modifier
	int32 total_meta_atk = 0;
	total_meta_atk = attacker->GetTotalMetaphysicalAttack() + add_atk;
	if (total_meta_atk < 0)
		total_meta_atk = 0;

	// Holds the total physical defense of the target
	int32 total_meta_def = 0;

	if (IsTargetPoint(target->GetType()) == true) {
		total_meta_def = target->GetActor()->GetAttackPoint(target->GetPoint())->GetTotalMetaphysicalDefense();
	}
	else if (IsTargetActor(target->GetType()) == true) {
		total_meta_def = target->GetActor()->TotalMetaphysicalDefense();
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << target->GetType() << std::endl;
		return 0;
	}

	// Holds the total damage dealt
	int32 total_dmg = total_meta_atk - total_meta_def;
	if (total_dmg < 0)
		total_dmg = 0;

	// If the total damage is zero, fall back to causing a small non-zero damage value
	if (total_dmg <= 0)
		return static_cast<uint32>(RandomBoundedInteger(1, 5));

	// Holds the absolute standard deviation used in the GaussianRandomValue function
	float abs_std_dev = 0.0f;
	abs_std_dev = static_cast<float>(total_dmg) * std_dev;
	total_dmg = GaussianRandomValue(total_dmg, abs_std_dev, false);

	// If the total damage came to a value less than or equal to zero after the gaussian randomization,
	// fall back to returning a small non-zero damage value
	if (total_dmg <= 0)
		return static_cast<uint32>(RandomBoundedInteger(1, 5));

	return static_cast<uint32>(total_dmg);
} // uint32 CalculateMetaphysicalDamageAdder(BattleActor* attacker, BattleTarget* target, int32 add_atk, float std_dev)



uint32 CalculateMetaphysicalDamageMultiplier(BattleActor* attacker, BattleTarget* target, float mul_atk) {
	return CalculateMetaphysicalDamageMultiplier(attacker, target, mul_atk, 0.10f);
}



uint32 CalculateMetaphysicalDamageMultiplier(BattleActor* attacker, BattleTarget* target, float mul_atk, float std_dev) {
	if (attacker == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL attacker argument" << std::endl;
		return 0;
	}
	if (target == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL target argument" << std::endl;
		return 0;
	}
	if (IsTargetParty(target->GetType()) == true) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "target was a party type: " << target->GetType() << std::endl;
		return 0;
	}
	if (mul_atk < 0.0f) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received negative multiplier arument: " << mul_atk << std::endl;
		mul_atk = fabs(mul_atk);
	}
	if (std_dev < 0.0f) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received negative standard deviation argument: " << std_dev << std::endl;
		std_dev = fabs(std_dev);
	}

	// Retrieve the total physical attack of the attacker and apply the modifier
	int32 total_meta_atk = static_cast<int32>(static_cast<float>(attacker->GetTotalMetaphysicalAttack()) * mul_atk);

	if (total_meta_atk < 0)
		total_meta_atk = 0;

	// Holds the total physical defense of the target
	int32 total_meta_def = 0;

	if (IsTargetPoint(target->GetType()) == true) {
		total_meta_def = target->GetActor()->GetAttackPoint(target->GetPoint())->GetTotalMetaphysicalDefense();
	}
	else if (IsTargetActor(target->GetType()) == true) {
		total_meta_def = target->GetActor()->TotalMetaphysicalDefense();
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << target->GetType() << std::endl;
		return 0;
	}

	// Holds the total damage dealt
	int32 total_dmg = total_meta_atk - total_meta_def;

	// If the total damage is zero, fall back to causing a small non-zero damage value
	if (total_dmg <= 0)
		return static_cast<uint32>(RandomBoundedInteger(1, 5));

	// Holds the absolute standard deviation used in the GaussianRandomValue function
	float abs_std_dev = 0.0f;
	// A value of "0.075f" means the standard deviation should be 7.5% of the mean (the total damage)
	abs_std_dev = static_cast<float>(total_dmg) * std_dev;
	total_dmg = GaussianRandomValue(total_dmg, abs_std_dev, false);

	// If the total damage came to a value less than or equal to zero after the gaussian randomization,
	// fall back to returning a small non-zero damage value
	if (total_dmg <= 0)
		return static_cast<uint32>(RandomBoundedInteger(1, 5));

	return static_cast<uint32>(total_dmg);
} // uint32 CalculateMetaphysicalDamageMultiplier(BattleActor* attacker, BattleTarget* target, float mul_phys, float std_dev)

////////////////////////////////////////////////////////////////////////////////
// BattleTarget class
////////////////////////////////////////////////////////////////////////////////

BattleTarget::BattleTarget() :
	_type(GLOBAL_TARGET_INVALID),
	_point(0),
	_actor(NULL),
	_party(NULL)
{}



void BattleTarget::InvalidateTarget() {
	_type = GLOBAL_TARGET_INVALID;
	_point = 0;
	_actor = NULL;
	_party = NULL;
}



bool BattleTarget::SetInitialTarget(BattleActor* user, GLOBAL_TARGET type) {
	InvalidateTarget();

	if (user == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL argument" << std::endl;
		return false;
	}
	if ((type <= GLOBAL_TARGET_INVALID) || (type >= GLOBAL_TARGET_TOTAL)) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type argument: " << type << std::endl;
		return false;
	}

	// Determine what party the initial target will exist in
	std::deque<BattleActor*>* target_party;
	if ((type == GLOBAL_TARGET_ALLY_POINT) || (type == GLOBAL_TARGET_ALLY) || (type == GLOBAL_TARGET_ALL_ALLIES)
			|| (type == GLOBAL_TARGET_ALLY_EVEN_DEAD)) {
		if (user->IsEnemy() == false)
			target_party = &BattleMode::CurrentInstance()->GetCharacterParty();
		else
			target_party = &BattleMode::CurrentInstance()->GetEnemyParty();
	}
	else if ((type == GLOBAL_TARGET_FOE_POINT) || (type == GLOBAL_TARGET_FOE) || (type == GLOBAL_TARGET_ALL_FOES)) {
		if (user->IsEnemy() == false)
			target_party = &BattleMode::CurrentInstance()->GetEnemyParty();
		else
			target_party = &BattleMode::CurrentInstance()->GetCharacterParty();
	}
	else {
		target_party = NULL;
	}

	// Set the actor/party according to the target type
	switch (type) {
		case GLOBAL_TARGET_SELF_POINT:
		case GLOBAL_TARGET_SELF:
		case GLOBAL_TARGET_ALLY_POINT:
		case GLOBAL_TARGET_ALLY:
			_actor = user;
			break;
		case GLOBAL_TARGET_ALLY_EVEN_DEAD:
		case GLOBAL_TARGET_FOE_POINT:
		case GLOBAL_TARGET_FOE:
			_actor = target_party->at(0);
			break;
		case GLOBAL_TARGET_ALL_ALLIES:
		case GLOBAL_TARGET_ALL_FOES:
			_party = target_party;
			break;
		default:
			IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid type: " << type << std::endl;
			return false;
	}

	_type = type;

	// If the target is not a party and not the user themselves, select the first valid actor
	if ((_actor != NULL) && (_actor != user)) {
		if (!IsValid()) {
			if (!SelectNextActor(user, true, true)) {
				IF_PRINT_WARNING(BATTLE_DEBUG)
					<< "could not find an initial actor that was a valid target" << std::endl;
				return false;
			}
		}
	}
	return true;
}

bool BattleTarget::SetPointTarget(GLOBAL_TARGET type, uint32 attack_point, BattleActor* actor) {
	if (IsTargetPoint(type) == false) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received invalid type argument: " << type << std::endl;
		return false;
	}
	if ((actor == NULL) && (_actor == NULL)) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "attempted to set an attack point with no valid actor selected" << std::endl;
		return false;
	}
	else if ((actor == NULL) && (attack_point >= _actor->GetAttackPoints().size())) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "attack point index was out-of-range: " << attack_point << std::endl;
		return false;
	}
	else if ((_actor == NULL) && (attack_point >= actor->GetAttackPoints().size())) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "attack point index was out-of-range: " << attack_point << std::endl;
		return false;
	}

	_type = type;
	_point = attack_point;
	if (actor != NULL)
		_actor = actor;
	_party = NULL;
	return true;
}

bool BattleTarget::SetActorTarget(GLOBAL_TARGET type, BattleActor* actor) {
	if (!IsTargetActor(type)) {
		IF_PRINT_WARNING(BATTLE_DEBUG)
			<< "function received invalid type argument: " << type << std::endl;
		return false;
	}
	if (!actor) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL argument"
			<< std::endl;
		return false;
	}

	_type = type;
	_point = 0;
	_actor = actor;
	_party = NULL;
	return true;
}

bool BattleTarget::SetPartyTarget(GLOBAL_TARGET type, std::deque<BattleActor*>* party) {
	if (!IsTargetParty(type)) {
		IF_PRINT_WARNING(BATTLE_DEBUG)
			<< "function received invalid type argument: " << type << std::endl;
		return false;
	}
	if (!party) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL argument"
			<< std::endl;
		return false;
	}

	_type = type;
	_point = 0;
	_actor = NULL;
	_party = party;
	return true;
}


bool BattleTarget::IsValid(bool permit_dead_targets) {
	// No dead enemies can be selected here.
	if (IsTargetPoint(_type)) {
		if (!_actor)
			return false;
		bool enemy_actor = _actor->IsEnemy();

		if (_point >= _actor->GetAttackPoints().size())
			return false;
		// We extra check the actor HP since the state might desynced on purpose.
		else if (!_actor->IsAlive() || _actor->GetHitPoints() == 0)
			return !enemy_actor && permit_dead_targets;
		else if (_actor->GetState() == ACTOR_STATE_DYING)
			return !enemy_actor && permit_dead_targets;
		else
			return true;
	}
	else if (IsTargetActor(_type) == true) {
		if (!_actor)
			return false;
		bool enemy_actor = _actor->IsEnemy();

		if (!_actor->IsAlive() || _actor->GetHitPoints() == 0)
			return !enemy_actor && permit_dead_targets;
		else if (_actor->GetState() == ACTOR_STATE_DYING)
			return !enemy_actor && permit_dead_targets;
		else
			return true;
	}
	else if (IsTargetParty(_type)) {
		if (!_party)
			return false;
		else
			return true;
	}
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << _type << std::endl;
		return false;
	}
}

bool BattleTarget::SelectNextPoint(BattleActor* user, bool direction, bool valid_criteria,
									bool permit_dead_targets) {
	if (user == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL argument" << std::endl;
		return false;
	}
	if (IsTargetPoint(_type) == false) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << _type << std::endl;
		return false;
	}
	if (_actor == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "no valid actor target" << std::endl;
		return false;
	}

	// First check for the case where we need to select a new actor
	if (valid_criteria && !IsValid(permit_dead_targets)) {
		_point = 0;
		return SelectNextActor(user, direction, valid_criteria, permit_dead_targets);
	}

	// If the actor has only a single attack point, there's no way to select another attack point
	uint32 num_points = _actor->GetAttackPoints().size();
	if (num_points == 1) {
		return false;
	}

	if (direction == true) {
		_point++;
		if (_point >= num_points)
			_point = 0;
	}
	else {
		if (_point == 0)
			_point = num_points - 1;
		else
			_point--;
	}
	return true;
}



bool BattleTarget::SelectNextActor(BattleActor* user, bool direction, bool valid_criteria,
									bool permit_dead_targets) {
	if (!user) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL argument"
			<< std::endl;
		return false;
	}
	if ((!IsTargetPoint(_type)) && (!IsTargetActor(_type))) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << _type
			<< std::endl;
		return false;
	}
	if (!_actor) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "no valid actor target" << std::endl;
		return false;
	}

	// ----- (1): Retrieve the proper party container that contains the actors we would like to select from
	std::deque<BattleActor*>* target_party = NULL;
	if ((_type == GLOBAL_TARGET_SELF_POINT) || (_type == GLOBAL_TARGET_SELF)) {
		return false; // Self type targets do not have multiple actors to select from
	}
	else if ((_type == GLOBAL_TARGET_ALLY_POINT) || (_type == GLOBAL_TARGET_ALLY)
			|| (_type == GLOBAL_TARGET_ALLY_EVEN_DEAD)) {
		if (user->IsEnemy() == false)
			target_party = &BattleMode::CurrentInstance()->GetCharacterParty();
		else
			target_party = &BattleMode::CurrentInstance()->GetEnemyParty();
	}
	else if ((_type == GLOBAL_TARGET_FOE_POINT) || (_type == GLOBAL_TARGET_FOE)) {
		if (user->IsEnemy() == false)
			target_party = &BattleMode::CurrentInstance()->GetEnemyParty();
		else
			target_party = &BattleMode::CurrentInstance()->GetCharacterParty();
	}
	else {
		// This should never be reached because the target type was already determined to be a point or actor above
		IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << _type << std::endl;
		return false;
	}

	// ----- (2): Check the target party for early exit conditions
	if (target_party->empty() == true) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "actor target's party was empty" << std::endl;
		return false;
	}
	if (target_party->size() == 1) {
		return false; // No more actors to select from in the party
	}

	// ----- (3): Determine the index of the current actor in the target party
	uint32 original_target_index = 0xFFFFFFFF; // Initially set to an impossibly high index for error checking
	for (uint32 i = 0; i < target_party->size(); i++) {
		if (target_party->at(i) == _actor) {
			original_target_index = i;
			break;
		}
	}
	if (original_target_index == 0xFFFFFFFF) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "actor target was not found in party" << std::endl;
		return false;
	}

	// ----- (4): Starting from the index of the original actor, select the next available actor
	BattleActor* original_actor = _actor;
	uint32 new_target_index = original_target_index;
	while (true) {
		// Increment or decrement the target index based on the direction argument
		if (direction == true) {
			new_target_index = (new_target_index >= target_party->size() - 1) ? 0 : new_target_index + 1;
		}
		else {
			new_target_index = (new_target_index == 0) ? target_party->size() - 1 : new_target_index - 1;
		}

		// If we've reached the original target index then we were unable to select another actor target
		if (new_target_index == original_target_index) {
			_actor = original_actor;
			return false;
		}

		// Set the new actor target and if required, ascertain the new target's validity. If the new target
		// must be valid and this new actor is not, the loop will continue and will try again with the next actor
		_actor = target_party->at(new_target_index);
		if (valid_criteria == false) {
			return true;
		}
		else if (IsValid(permit_dead_targets)){
			return true;
		}
	}
} // bool BattleTarget::SelectNextActor(BattleActor* user, bool direction, bool valid_criteria)



BattleActor* BattleTarget::GetPartyActor(uint32 index) {
	if (_party == NULL) {
		return NULL;
	}

	if (index >= _party->size()) {
		return NULL;
	}

	return (*_party)[index];
}



ustring BattleTarget::GetName() {
	if ((_type == GLOBAL_TARGET_SELF_POINT) || (_type == GLOBAL_TARGET_ALLY_POINT) || (_type == GLOBAL_TARGET_FOE_POINT)) {
		return (_actor->GetName() + UTranslate(" — ") + (_actor->GetAttackPoints()).at(_point)->GetName());
	}
	else if ((_type == GLOBAL_TARGET_SELF) || (_type == GLOBAL_TARGET_ALLY) || (_type == GLOBAL_TARGET_FOE)
			|| (_type == GLOBAL_TARGET_ALLY_EVEN_DEAD)) {
		return _actor->GetName();
	}
	else if (_type == GLOBAL_TARGET_ALL_ALLIES) {
		return UTranslate("All Allies");
	}
	else if (_type == GLOBAL_TARGET_ALL_FOES) {
		return UTranslate("All Enemies");
	}

	return UTranslate("[Invalid Target]");
}

////////////////////////////////////////////////////////////////////////////////
// BattleItem class
////////////////////////////////////////////////////////////////////////////////

BattleItem::BattleItem(hoa_global::GlobalItem item) :
	_item(item),
	_battle_count(item.GetCount())
{
	if (item.GetID() == 0)
		IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received invalid item argument" << std::endl;
}


BattleItem::~BattleItem() {
	if (_battle_count != _item.GetCount())
		IF_PRINT_WARNING(BATTLE_DEBUG) << "actual count was not equal to available count upon destruction" << std::endl;
}


void BattleItem::IncrementBattleCount() {
	++_battle_count;
	if (_battle_count > _item.GetCount()) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "attempted to increment available count above actual count: " << _battle_count << std::endl;
		--_battle_count;
	}
}


void BattleItem::DecrementBattleCount() {
	if (_battle_count == 0) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "attempted to decrement available count below zero" << std::endl;
		return;
	}
	--_battle_count;
}


} // namespace private_shop

} // namespace hoa_shop
