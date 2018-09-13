///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/battle/battle_damage.h"

#include "modes/battle/battle_target.h"

#include "common/global/actors/global_attack_point.h"

#include "utils/utils_random.h"

using namespace vt_global;
using namespace vt_utils;

namespace vt_battle
{

namespace private_battle
{

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

uint32_t RndPhysicalDamage(BattleActor* attacker, BattleTarget* target_actor,
                           uint32_t add_atk, float mul_atk, int32_t attack_point)
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
        PRINT_WARNING << "function received nullptr attacker argument" << std::endl;
        return 0;
    }
    if(target_actor == nullptr) {
        PRINT_WARNING << "function received nullptr target_actor argument" << std::endl;
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
        PRINT_WARNING << "function received nullptr attacker argument" << std::endl;
        return 0;
    }
    if(target_actor == nullptr) {
        PRINT_WARNING << "function received nullptr target_actor argument" << std::endl;
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

} // namespace private_battle

} // namespace vt_battle
