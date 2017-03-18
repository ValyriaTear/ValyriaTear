///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_utils.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for battle mode utility code
***
*** This file contains utility code that is shared among the various battle mode
*** classes.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/battle/battle_utils.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actors.h"

#include "common/global/global.h"

#include "engine/system.h"

#include "utils/utils_random.h"

using namespace vt_utils;
using namespace vt_system;
using namespace vt_global;

namespace vt_battle
{

namespace private_battle
{

////////////////////////////////////////////////////////////////////////////////
// Standard battle calculation functions
////////////////////////////////////////////////////////////////////////////////

bool RndEvade(BattleActor* target_actor)
{
    return RndEvade(target_actor, 0.0f, 1.0f, -1);
}

bool RndEvade(BattleActor* target_actor, float add_eva = 0.0f)
{
    return RndEvade(target_actor, add_eva, 1.0f, -1);
}

bool RndEvade(BattleActor* target_actor, float add_eva = 0.0f, float mul_eva = 1.0f)
{
    return RndEvade(target_actor, add_eva, mul_eva, -1);
}

bool RndEvade(BattleActor* target_actor, float add_eva, float mul_eva, int32_t attack_point)
{
    if (!target_actor)
        return true;

    // When stunned, the actor can't dodge.
    if (target_actor->IsStunned())
        return false;

    float evasion = 0.0f;
    if(attack_point > -1) {
        GlobalAttackPoint* atk_point = target_actor->GetAttackPoint(attack_point);
        evasion = atk_point ? atk_point->GetTotalEvadeRating() : target_actor->GetAverageEvadeRating();
    }
    else {
        evasion = target_actor->GetAverageEvadeRating();
    }

    evasion += add_eva;
    evasion *= mul_eva;

    // Check for absolute hit/miss conditions
    // and still give a slight chance for it to happen.
    if(evasion <= 0.0f)
        evasion = 0.05f;
    else if(evasion >= 100.0f)
        evasion = 0.95f;

    return RandomFloat(0.0f, 100.0f) <= evasion;
}

uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor)
{
    return RndPhysicalDamage(attacker, target_actor->GetActor(), 0, 1.0f, -1);
}

uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor, uint32_t add_atk)
{
    return RndPhysicalDamage(attacker, target_actor->GetActor(), add_atk, 1.0f, -1);
}

uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor, uint32_t add_atk, float mul_atk)
{
    return RndPhysicalDamage(attacker, target_actor->GetActor(), add_atk, mul_atk, -1);
}

uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor, uint32_t add_atk, float mul_atk, int32_t attack_point)
{
    return RndPhysicalDamage(attacker, target_actor->GetActor(), add_atk, mul_atk, attack_point);
}

uint32_t RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor)
{
    return RndPhysicalDamage(attacker, target_actor, 0, 1.0f, -1);
}

uint32_t RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor,
                         uint32_t add_atk)
{
    return RndPhysicalDamage(attacker, target_actor, add_atk, 1.0f, -1);
}

uint32_t RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor,
                         uint32_t add_atk, float mul_atk)
{
    return RndPhysicalDamage(attacker, target_actor, add_atk, mul_atk, -1);
}

uint32_t RndPhysicalDamage(BattleActor* attacker, BattleActor* target_actor,
                         uint32_t add_atk, float mul_atk, int32_t attack_point)
{
    if(attacker == nullptr) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received nullptr attacker argument" << std::endl;
        return 0;
    }
    if(target_actor == nullptr) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received nullptr target_actor argument" << std::endl;
        return 0;
    }

    // Holds the total physical attack of the attacker and modifier
    int32_t total_phys_atk = attacker->GetTotalPhysicalAttack() + add_atk;
    total_phys_atk = static_cast<int32_t>(static_cast<float>(total_phys_atk) * mul_atk);
    // Randomize the damage a bit.
    int32_t phys_atk_diff = total_phys_atk / 10;
    total_phys_atk = RandomBoundedInteger(total_phys_atk - phys_atk_diff, total_phys_atk + phys_atk_diff);

    if(total_phys_atk < 0)
        total_phys_atk = 0;

    // Holds the total physical defense of the target
    int32_t total_phys_def = 0;

    if(attack_point > -1) {
        GlobalAttackPoint* atk_point = target_actor->GetAttackPoint(attack_point);
        total_phys_def = atk_point ? atk_point->GetTotalPhysicalDefense() : target_actor->GetAverageDefense();
    }
    else {
        total_phys_def = target_actor->GetAverageDefense();
    }

    // Holds the total damage dealt
    int32_t total_dmg = total_phys_atk - total_phys_def;

    // If the total damage is zero, fall back to causing a small non-zero damage value
    if(total_dmg <= 0)
        return static_cast<uint32_t>(RandomBoundedInteger(1, 5 + attacker->GetPhysAtk() / 10));

    return static_cast<uint32_t>(total_dmg);
}

uint32_t RndMagicalDamage(BattleActor* attacker, BattleActor* target_actor, vt_global::GLOBAL_ELEMENTAL element)
{
    return RndMagicalDamage(attacker, target_actor, element, 0, 1.0f, -1);
}

uint32_t RndMagicalDamage(BattleActor* attacker, BattleActor* target_actor, GLOBAL_ELEMENTAL element,
                        uint32_t add_atk)
{
    return RndMagicalDamage(attacker, target_actor, element, add_atk, 1.0f, -1);
}

uint32_t RndMagicalDamage(BattleActor* attacker, BattleActor* target_actor, GLOBAL_ELEMENTAL element,
                        uint32_t add_atk, float mul_atk)
{
    return RndMagicalDamage(attacker, target_actor, element, add_atk, mul_atk, -1);
}

uint32_t RndMagicalDamage(BattleActor* attacker, BattleActor* target_actor, GLOBAL_ELEMENTAL element,
                        uint32_t add_atk, float mul_atk, int32_t attack_point)
{
    if(attacker == nullptr) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received nullptr attacker argument" << std::endl;
        return 0;
    }
    if(target_actor == nullptr) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received nullptr target_actor argument" << std::endl;
        return 0;
    }

    // Holds the total physical attack of the attacker and modifier
    int32_t total_mag_atk = attacker->GetTotalMagicalAttack(element) + add_atk;
    total_mag_atk = static_cast<int32_t>(static_cast<float>(total_mag_atk) * mul_atk);
    // Randomize the damage a bit.
    int32_t mag_atk_diff = total_mag_atk / 10;
    total_mag_atk = RandomBoundedInteger(total_mag_atk - mag_atk_diff, total_mag_atk + mag_atk_diff);

    if(total_mag_atk < 0)
        total_mag_atk = 0;

    // Holds the total physical defense of the target
    int32_t total_mag_def = 0;

    if(attack_point > -1) {
        GlobalAttackPoint* atk_point = target_actor->GetAttackPoint(attack_point);
        total_mag_def = atk_point ? atk_point->GetTotalMagicalDefense(element) : target_actor->GetAverageMagicalDefense(element);
    }
    else {
        total_mag_def = target_actor->GetAverageMagicalDefense(element);
    }

    // Holds the total damage dealt
    int32_t total_dmg = total_mag_atk - total_mag_def;
    if(total_dmg < 0)
        total_dmg = 0;

    // If the total damage is zero, fall back to causing a small non-zero damage value
    if(total_dmg <= 0)
        return static_cast<uint32_t>(RandomBoundedInteger(1, 5 + attacker->GetMagAtk() / 10));

    return static_cast<uint32_t>(total_dmg);
}


////////////////////////////////////////////////////////////////////////////////
// BattleTarget class
////////////////////////////////////////////////////////////////////////////////

BattleTarget::BattleTarget() :
    _type(GLOBAL_TARGET_INVALID),
    _attack_point(0),
    _actor_target(nullptr)
{}

void BattleTarget::InvalidateTarget()
{
    _type = GLOBAL_TARGET_INVALID;
    _attack_point = 0;
    _actor_target = nullptr;
    _party_target.clear();
}

bool BattleTarget::SetTarget(BattleActor* attacker, vt_global::GLOBAL_TARGET type, BattleActor* target, uint32_t attack_point)
{
    if((type <= GLOBAL_TARGET_INVALID) || (type >= GLOBAL_TARGET_TOTAL)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type argument: " << type << std::endl;
        return false;
    }

    if (attacker == nullptr) {
        PRINT_ERROR << "BattleTarget::SetTarget() called wirh nullptr attacker." << std::endl;
        return false;
    }

    InvalidateTarget();

    // Set the target party according to the target type
    std::deque<BattleActor *>* party_target = nullptr;
    switch(type) {
    case GLOBAL_TARGET_SELF_POINT:
    case GLOBAL_TARGET_ALLY_POINT:
    case GLOBAL_TARGET_SELF:
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
    case GLOBAL_TARGET_DEAD_ALLY_ONLY:
    case GLOBAL_TARGET_ALL_ALLIES:
        if(attacker->IsEnemy())
            party_target = &BattleMode::CurrentInstance()->GetEnemyParty();
        else
            party_target = &BattleMode::CurrentInstance()->GetCharacterParty();
        break;

    case GLOBAL_TARGET_FOE_POINT:
    case GLOBAL_TARGET_FOE:
    case GLOBAL_TARGET_ALL_FOES:
        if(attacker->IsEnemy())
            party_target = &BattleMode::CurrentInstance()->GetCharacterParty();
        else
            party_target = &BattleMode::CurrentInstance()->GetEnemyParty();
        break;

    default:
        // Shouldn't happen
        PRINT_WARNING << "Invalid target type argument: " << type << std::endl;
        return false;
        break;
    }

    // Check whether the actor is actually part of the party and fix this if needed.
    if (target && std::find(party_target->begin(), party_target->end(), target) == party_target->end())
        target = party_target->at(0);

    if (target != nullptr)
        _actor_target = target;
    else
        _actor_target = party_target->at(0);

    _type = type;

    _party_target.clear();
    for (size_t i = 0; i < party_target->size(); ++i) {
        if (type != GLOBAL_TARGET_ALL_FOES) {
            _party_target.push_back(party_target->at(i));
        } else {
            if (party_target->at(i)->CanFight()) {
                _party_target.push_back(party_target->at(i));
            }
        }
    }

    _attack_point = attack_point;
    if (_attack_point >= _actor_target->GetAttackPoints().size())
        _attack_point = 0;

    // If the target is not a party and not the user themselves, select the first valid actor
    if (type != GLOBAL_TARGET_ALL_FOES && (!IsValid() && !SelectNextActor())) {
        InvalidateTarget();

        PRINT_WARNING << "Could not find an initial actor that was a valid target" << std::endl;
        return false;
    }
    return true;
}

bool BattleTarget::IsValid()
{
    if (!_actor_target || _party_target.empty()) {
        PRINT_WARNING << "No valid actor or party set: " << _type << std::endl;
        return false;
    }

    if(IsTargetPoint(_type)) {
        if(_attack_point >= _actor_target->GetAttackPoints().size())
            return false;
    }

    // If we can't find the actor within the party, then it's bad.
    if (std::find(_party_target.begin(), _party_target.end(), _actor_target) == _party_target.end())
        return false;

    bool permit_dead_targets = (_type == GLOBAL_TARGET_ALLY_EVEN_DEAD || _type == GLOBAL_TARGET_DEAD_ALLY_ONLY);

    if (!_actor_target->IsAlive() || !_actor_target->CanFight() || _actor_target->GetHitPoints() == 0)
        return permit_dead_targets;

    return true;
}

bool BattleTarget::SelectNextPoint(bool direction)
{
    if(IsTargetPoint(_type) == false) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << _type << std::endl;
        return false;
    }
    if(_actor_target == nullptr || _party_target.empty()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "No valid target set" << std::endl;
        return false;
    }

    // First check for the case where we need to select a new actor first
    if(!IsValid()) {
        _attack_point = 0;
        return SelectNextActor();
    }

    // If the actor has only a single attack point, there's no way to select another attack point
    uint32_t num_points = _actor_target->GetAttackPoints().size();
    if(num_points == 1)
        return true;

    if(direction) {
        ++_attack_point;
        if(_attack_point >= num_points)
            _attack_point = 0;
    } else {
        if(_attack_point == 0)
            _attack_point = num_points - 1;
        else
            --_attack_point;
    }
    return true;
}

bool BattleTarget::SelectNextActor(bool direction)
{
    if(!IsTargetPoint(_type) && !IsTargetActor(_type)) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "Invalid target type: " << _type << std::endl;
        return false;
    }

    // Check the target party for early exit conditions
    if(_party_target.empty()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "Actor target's party was empty" << std::endl;
        return false;
    }
    if(_party_target.size() == 1) {
        return false; // No more actors to select from in the party
    }

    // The target died in between events. Let's reset it.
    if(!_actor_target) {
        _actor_target = _party_target.at(0);
    }

    // Determine the index of the current actor in the target party
    uint32_t original_target_index = 0xFFFFFFFF; // Initially set to an impossibly high index for error checking
    for(uint32_t i = 0; i < _party_target.size(); ++i) {
        if(_party_target.at(i) == _actor_target) {
            original_target_index = i;
            break;
        }
    }
    if(original_target_index == 0xFFFFFFFF) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "actor target was not found in party" << std::endl;
        return false;
    }

    // Starting from the index of the original actor, select the next available actor
    BattleActor* original_actor = _actor_target;
    uint32_t new_target_index = original_target_index;
    while(true) {
        // Increment or decrement the target index based on the direction argument
        if(direction) {
            new_target_index = (new_target_index >= _party_target.size() - 1) ? 0 : new_target_index + 1;
        } else {
            new_target_index = (new_target_index == 0) ? _party_target.size() - 1 : new_target_index - 1;
        }

        // If we've reached the original target index then we were unable to select another actor target
        if(new_target_index == original_target_index) {
            _actor_target = original_actor;
            return false;
        }

        // Set the new actor target and if required, ascertain the new target's validity. If the new target
        // must be valid and this new actor is not, the loop will continue and will try again with the next actor
        _actor_target = _party_target.at(new_target_index);

        // Since we're changing the target, we reinit the attack point to the first one,
        // as the new target may have less attack points than the latest one.
        ReinitAttackPoint();

        if (IsValid())
            return true;
    }
}

BattleActor* BattleTarget::GetPartyActor(uint32_t index)
{
    if (index >= _party_target.size())
        return nullptr;

    return _party_target.at(index);
}

ustring BattleTarget::GetName()
{
    switch(_type) {
    default:
        return UTranslate("[Invalid Target]");

    case GLOBAL_TARGET_SELF_POINT:
    case GLOBAL_TARGET_ALLY_POINT:
    case GLOBAL_TARGET_FOE_POINT:
        return (_actor_target->GetName() + UTranslate(" — ") + (_actor_target->GetAttackPoints()).at(_attack_point)->GetName());

    case GLOBAL_TARGET_SELF:
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
    case GLOBAL_TARGET_DEAD_ALLY_ONLY:
    case GLOBAL_TARGET_FOE:
        return _actor_target->GetName();

    case GLOBAL_TARGET_ALL_ALLIES:
        return UTranslate("All Allies");

    case GLOBAL_TARGET_ALL_FOES:
        return UTranslate("All Enemies");
    }
}

BattleTarget& BattleTarget::operator=(const BattleTarget& copy)
{
    if(this == &copy)  // Handle self-assignment case
        return *this;

    _type = copy._type;
    _attack_point = copy._attack_point;
    _actor_target = copy._actor_target;

    std::deque<BattleActor *> _party_target;
    for (uint32_t i = 0; i < copy._party_target.size(); ++i) {
        _party_target.push_back(copy._party_target[i]);
    }

    return *this;
}

////////////////////////////////////////////////////////////////////////////////
// BattleItem class
////////////////////////////////////////////////////////////////////////////////

BattleItem::BattleItem(vt_global::GlobalItem item) :
    _item(item),
    _battle_count(item.GetCount())
{
    if(item.GetID() == 0)
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received invalid item argument" << std::endl;
}


BattleItem::~BattleItem()
{
    if(_battle_count != _item.GetCount())
        IF_PRINT_WARNING(BATTLE_DEBUG) << "actual count was not equal to available count upon destruction" << std::endl;
}


void BattleItem::IncrementBattleCount()
{
    ++_battle_count;
    if(_battle_count > _item.GetCount()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "attempted to increment available count above actual count: " << _battle_count << std::endl;
        --_battle_count;
    }
}


void BattleItem::DecrementBattleCount()
{
    if(_battle_count == 0) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "attempted to decrement available count below zero" << std::endl;
        return;
    }
    --_battle_count;
}


} // namespace private_battle

} // namespace vt_battle
