////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_actor.h"

#include "global_attack_point.h"
#include "common/global/global_skills.h"

#include "script/script_read.h"

using namespace vt_utils;
using namespace vt_video;
using namespace vt_script;

namespace vt_global
{

extern bool GLOBAL_DEBUG;

GlobalActor::GlobalActor() :
    _id(0),
    _hit_points(0),
    _max_hit_points(0),
    _skill_points(0),
    _max_skill_points(0),
    _total_physical_attack(0)
{
    // Init the elemental strength intensity container
    _elemental_modifier.resize(GLOBAL_ELEMENTAL_TOTAL, 1.0f);

    for (uint32_t i = 0; i < GLOBAL_ELEMENTAL_TOTAL; ++i)
        _total_magical_attack[i] = 0;
}

GlobalActor::~GlobalActor()
{
    // Delete all attack points
    for(uint32_t i = 0; i < _attack_points.size(); ++i) {
        delete _attack_points[i];
    }
    _attack_points.clear();

    // Delete all skills
    for(uint32_t i = 0; i < _skills.size(); ++i)
        delete _skills[i];
    //_skills.clear();
    //_skill_ids.clear();
}

GlobalActor::GlobalActor(const GlobalActor &copy):
    _map_sprite_name(copy._map_sprite_name),
    _portrait(copy._portrait),
    _full_portrait(copy._full_portrait),
    _stamina_icon(copy._stamina_icon)
{
    _id = copy._id;
    _name = copy._name;
    _hit_points = copy._hit_points;
    _max_hit_points = copy._max_hit_points;
    _skill_points = copy._skill_points;
    _max_skill_points = copy._max_skill_points;

    _char_phys_atk.SetBase(copy._char_phys_atk.GetBase());
    _char_phys_atk.SetModifier(copy._char_phys_atk.GetModifier());
    _char_mag_atk.SetBase(copy._char_mag_atk.GetBase());
    _char_mag_atk.SetModifier(copy._char_mag_atk.GetModifier());
    _char_phys_def.SetBase(copy._char_phys_def.GetBase());
    _char_phys_def.SetModifier(copy._char_phys_def.GetModifier());
    _char_mag_def.SetBase(copy._char_mag_def.GetBase());
    _char_mag_def.SetModifier(copy._char_mag_def.GetModifier());
    _stamina.SetBase(copy._stamina.GetBase());
    _stamina.SetModifier(copy._stamina.GetModifier());
    _evade.SetBase(copy._evade.GetBase());
    _evade.SetModifier(copy._evade.GetModifier());

    _total_physical_attack = copy._total_physical_attack;
    // init the elemental modifier size to avoid a segfault
    _elemental_modifier.resize(GLOBAL_ELEMENTAL_TOTAL, 1.0f);
    for (uint32_t i = 0; i < GLOBAL_ELEMENTAL_TOTAL; ++i) {
        _total_magical_attack[i] = copy._total_magical_attack[i];
        _elemental_modifier[i] = copy._elemental_modifier[i];
    }

    // Copy all attack points
    for(uint32_t i = 0; i < copy._attack_points.size(); ++i) {
        _attack_points.push_back(new GlobalAttackPoint(*copy._attack_points[i]));
        _attack_points[i]->SetActorOwner(this);
    }

    // Copy all skills
    for (uint32_t i = 0; i < copy._skills.size(); ++i) {
        // Create a new instance as the skill is deleted on an actor object basis.
        _skills.push_back(new GlobalSkill(*copy._skills[i]));
        _skills_id.push_back(copy._skills_id[i]);
    }

    // Script files
    _death_script_filename = copy._death_script_filename;
    _ai_script_filename = copy._ai_script_filename;
}

GlobalActor &GlobalActor::operator=(const GlobalActor &copy)
{
    if(this == &copy)  // Handle self-assignment case
        return *this;

    _id = copy._id;
    _name = copy._name;
    _map_sprite_name = copy._map_sprite_name;
    _portrait = copy._portrait;
    _full_portrait = copy._full_portrait;
    _stamina_icon = copy._stamina_icon;
    _hit_points = copy._hit_points;
    _max_hit_points = copy._max_hit_points;
    _skill_points = copy._skill_points;
    _max_skill_points = copy._max_skill_points;

    _char_phys_atk.SetBase(copy._char_phys_atk.GetBase());
    _char_phys_atk.SetModifier(copy._char_phys_atk.GetModifier());
    _char_mag_atk.SetBase(copy._char_mag_atk.GetBase());
    _char_mag_atk.SetModifier(copy._char_mag_atk.GetModifier());
    _char_phys_def.SetBase(copy._char_phys_def.GetBase());
    _char_phys_def.SetModifier(copy._char_phys_def.GetModifier());
    _char_mag_def.SetBase(copy._char_mag_def.GetBase());
    _char_mag_def.SetModifier(copy._char_mag_def.GetModifier());
    _stamina.SetBase(copy._stamina.GetBase());
    _stamina.SetModifier(copy._stamina.GetModifier());
    _evade.SetBase(copy._evade.GetBase());
    _evade.SetModifier(copy._evade.GetModifier());

    _total_physical_attack = copy._total_physical_attack;
    _elemental_modifier.resize(GLOBAL_ELEMENTAL_TOTAL, 1.0f);
    for (uint32_t i = 0; i < GLOBAL_ELEMENTAL_TOTAL; ++i) {
        _total_magical_attack[i] = copy._total_magical_attack[i];
        _elemental_modifier[i] = copy._elemental_modifier[i];
    }

    // Copy all attack points
    for(uint32_t i = 0; i < copy._attack_points.size(); ++i) {
        _attack_points.push_back(new GlobalAttackPoint(*_attack_points[i]));
        _attack_points[i]->SetActorOwner(this);
    }

    // Copy all skills
    for (uint32_t i = 0; i < copy._skills.size(); ++i) {
        // Create a new instance as the skill is deleted on an actor object basis.
        _skills.push_back(new GlobalSkill(*copy._skills[i]));
        _skills_id.push_back(copy._skills_id[i]);
    }

    // Script files
    _death_script_filename = copy._death_script_filename;
    _ai_script_filename = copy._ai_script_filename;

    return *this;
}

bool GlobalActor::HasSkill(uint32_t skill_id)
{
    for (uint32_t i = 0; i < _skills_id.size(); ++i) {
        if (_skills_id.at(i) == skill_id)
            return true;
    }
    return false;
}

uint32_t GlobalActor::GetTotalPhysicalDefense(uint32_t index) const
{
    if(index >= _attack_points.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of attack points: " << index << std::endl;
        return 0;
    }

    return _attack_points[index]->GetTotalPhysicalDefense();
}

uint32_t GlobalActor::GetTotalMagicalDefense(uint32_t index, GLOBAL_ELEMENTAL element) const
{
    if(index >= _attack_points.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of attack points: " << index << std::endl;
        return 0;
    }

    if (element <= GLOBAL_ELEMENTAL_INVALID || element >= GLOBAL_ELEMENTAL_TOTAL)
        element = GLOBAL_ELEMENTAL_NEUTRAL;

    return _attack_points[index]->GetTotalMagicalDefense(element);
}

float GlobalActor::GetTotalEvadeRating(uint32_t index) const
{
    if(index >= _attack_points.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of attack points: " << index << std::endl;
        return 0.0f;
    }

    return _attack_points[index]->GetTotalEvadeRating();
}

uint32_t GlobalActor::GetAverageDefense()
{
    if (_attack_points.empty())
        return 0;

    uint32_t phys_defense = 0;

    for(uint32_t i = 0; i < _attack_points.size(); ++i)
        phys_defense += _attack_points[i]->GetTotalPhysicalDefense();
    phys_defense /= _attack_points.size();

    return phys_defense;
}

uint32_t GlobalActor::GetAverageMagicalDefense(GLOBAL_ELEMENTAL element)
{
    if (_attack_points.empty())
        return 0;

    uint32_t mag_defense = 0;

    for(uint32_t i = 0; i < _attack_points.size(); ++i)
        mag_defense += _attack_points[i]->GetTotalMagicalDefense(element);
    mag_defense /= _attack_points.size();
    return mag_defense;
}

float GlobalActor::GetAverageEvadeRating()
{
    if (_attack_points.empty())
        return 0;

    float evade = 0.0f;

    for(uint32_t i = 0; i < _attack_points.size(); ++i)
        evade += _attack_points[i]->GetTotalEvadeRating();
    evade /= static_cast<float>(_attack_points.size());

    return evade;
}

GlobalAttackPoint *GlobalActor::GetAttackPoint(uint32_t index) const
{
    if(index >= _attack_points.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of attack points: " << index << std::endl;
        return nullptr;
    }

    return _attack_points[index];
}

void GlobalActor::AddHitPoints(uint32_t amount)
{
    if((0xFFFFFFFF - amount) < _hit_points) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << std::endl;
        _hit_points = 0xFFFFFFFF;
    } else {
        _hit_points += amount;
    }

    if(_hit_points > _max_hit_points)
        _hit_points = _max_hit_points;
}

void GlobalActor::SubtractHitPoints(uint32_t amount)
{
    if(amount >= _hit_points)
        _hit_points = 0;
    else
        _hit_points -= amount;
}

void GlobalActor::AddMaxHitPoints(uint32_t amount)
{
    if((0xFFFFFFFF - amount) < _max_hit_points) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << std::endl;
        _max_hit_points = 0xFFFFFFFF;
    } else {
        _max_hit_points += amount;
    }
}

void GlobalActor::SubtractMaxHitPoints(uint32_t amount)
{
    if(amount > _max_hit_points) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "argument value will cause max hit points to decrease to zero: " << amount << std::endl;
        _max_hit_points = 0;
        _hit_points = 0;
    } else {
        _max_hit_points -= amount;
        if(_hit_points > _max_hit_points)
            _hit_points = _max_hit_points;
    }
}

void GlobalActor::AddSkillPoints(uint32_t amount)
{
    if((0xFFFFFFFF - amount) < _skill_points) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << std::endl;
        _skill_points = 0xFFFFFFFF;
    } else {
        _skill_points += amount;
    }

    if(_skill_points > _max_skill_points)
        _skill_points = _max_skill_points;
}

void GlobalActor::SubtractSkillPoints(uint32_t amount)
{
    if(amount >= _skill_points)
        _skill_points = 0;
    else
        _skill_points -= amount;
}

void GlobalActor::AddMaxSkillPoints(uint32_t amount)
{
    if((0xFFFFFFFF - amount) < _max_skill_points) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << std::endl;
        _max_skill_points = 0xFFFFFFFF;
    } else {
        _max_skill_points += amount;
    }
}

void GlobalActor::SubtractMaxSkillPoints(uint32_t amount)
{
    if(amount > _max_skill_points) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "argument value will cause max skill points to decrease to zero: " << amount << std::endl;
        _max_skill_points = 0;
        _skill_points = 0;
    } else {
        _max_skill_points -= amount;
        if(_skill_points > _max_skill_points)
            _skill_points = _max_skill_points;
    }
}

void GlobalActor::AddPhysAtk(uint32_t amount)
{
    _char_phys_atk.SetBase(_char_phys_atk.GetBase() + (float)amount);
    _CalculateAttackRatings();
}

void GlobalActor::SubtractPhysAtk(uint32_t amount)
{
    float new_base = _char_phys_atk.GetBase() - (float)amount;
    _char_phys_atk.SetBase(new_base < 0.0f ? 0.0f : new_base);
    _CalculateAttackRatings();
}

void GlobalActor::AddMagAtk(uint32_t amount)
{
    _char_mag_atk.SetBase(_char_mag_atk.GetBase() + (float)amount);
    _CalculateAttackRatings();
}

void GlobalActor::SubtractMagAtk(uint32_t amount)
{
    float new_base = _char_mag_atk.GetBase() - (float)amount;
    _char_mag_atk.SetBase(new_base < 0.0f ? 0.0f : new_base);
    _CalculateAttackRatings();
}

void GlobalActor::AddPhysDef(uint32_t amount)
{
    _char_phys_def.SetBase(_char_phys_def.GetBase() + (float)amount);
    _CalculateDefenseRatings();
}

void GlobalActor::SubtractPhysDef(uint32_t amount)
{
    float new_base = _char_phys_def.GetBase() - (float)amount;
    _char_phys_def.SetBase(new_base < 0.0f ? 0.0f : new_base);
    _CalculateDefenseRatings();
}

void GlobalActor::AddMagDef(uint32_t amount)
{
    _char_mag_def.SetBase(_char_mag_def.GetBase() + (float)amount);
    _CalculateDefenseRatings();
}

void GlobalActor::SubtractMagDef(uint32_t amount)
{
    float new_base = _char_mag_def.GetBase() - (float)amount;
    _char_mag_def.SetBase(new_base < 0.0f ? 0.0f : new_base);
    _CalculateDefenseRatings();
}

void GlobalActor::AddStamina(uint32_t amount)
{
    _stamina.SetBase(_stamina.GetBase() + (float)amount);
}

void GlobalActor::SubtractStamina(uint32_t amount)
{
    float new_base = _stamina.GetBase() - (float)amount;
    _stamina.SetBase(new_base < 0.0f ? 0.0f : new_base);
}

void GlobalActor::AddEvade(float amount)
{
    _evade.SetBase(_evade.GetBase() + amount);
    _CalculateEvadeRatings();
}

void GlobalActor::SubtractEvade(float amount)
{
    float new_base = _evade.GetBase() - amount;
    _evade.SetBase(new_base < 0.0f ? 0.0f : new_base);
    _CalculateEvadeRatings();
}

void GlobalActor::_CalculateAttackRatings()
{
    _total_physical_attack = _char_phys_atk.GetValue();
    for (uint32_t i = 0; i < GLOBAL_ELEMENTAL_TOTAL; ++i)
        _total_magical_attack[i] = _char_mag_atk.GetValue() * _elemental_modifier[i];
}

void GlobalActor::_CalculateDefenseRatings()
{
    // Re-calculate the defense ratings for all attack points
    for(uint32_t i = 0; i < _attack_points.size(); ++i)
        _attack_points[i]->CalculateTotalDefense(nullptr);
}

void GlobalActor::_CalculateEvadeRatings()
{
    // Re-calculate the evade ratings for all attack points
    for(uint32_t i = 0; i < _attack_points.size(); ++i) {
        _attack_points[i]->CalculateTotalEvade();
    }
}

} // namespace vt_global
