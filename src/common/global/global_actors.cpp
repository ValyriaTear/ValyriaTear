////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_actors.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for global game actors
*** ***************************************************************************/

#include "video.h"
#include "global_actors.h"
#include "global_objects.h"
#include "global_effects.h"
#include "global_skills.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_video;
using namespace hoa_script;

namespace hoa_global {

////////////////////////////////////////////////////////////////////////////////
// GlobalAttackPoint class
////////////////////////////////////////////////////////////////////////////////

GlobalAttackPoint::GlobalAttackPoint(GlobalActor* owner) :
	_actor_owner(owner),
	_x_position(0),
	_y_position(0),
	_fortitude_modifier(0.0f),
	_protection_modifier(0.0f),
	_evade_modifier(0.0f),
	_total_physical_defense(0),
	_total_metaphysical_defense(0),
	_total_evade_rating(0)
{}



bool GlobalAttackPoint::LoadData(ReadScriptDescriptor& script) {
	if (script.IsFileOpen() == false) {
		return false;
	}

	_name = MakeUnicodeString(script.ReadString("name"));
	_x_position = script.ReadInt("x_position");
	_y_position = script.ReadInt("y_position");
	_fortitude_modifier = script.ReadFloat("fortitude_modifier");
	_protection_modifier = script.ReadFloat("protection_modifier");
	_evade_modifier = script.ReadFloat("evade_modifier");

	// Status effect data is optional so check if a status_effect table exists first
	if (script.DoesTableExist("status_effects") == true) {
		script.OpenTable("status_effects");

		std::vector<int32> table_keys;
		script.ReadTableKeys(table_keys);
		for (uint32 i = 0; i < table_keys.size(); i++) {
			float probability = script.ReadFloat(table_keys[i]);
			_status_effects.push_back(make_pair(static_cast<GLOBAL_STATUS>(table_keys[i]), probability));
		}

		script.CloseTable();
	}


	if (script.IsErrorDetected()) {
		if (GLOBAL_DEBUG) {
			PRINT_WARNING << "one or more errors occurred while reading the save game file - they are listed below" << endl;
			cerr << script.GetErrorMessages() << endl;
		}
		return false;
	}

	return true;
}



void GlobalAttackPoint::CalculateTotalDefense(const GlobalArmor* equipped_armor) {
	if (_actor_owner == NULL) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attack point has no owning actor" << endl;
		return;
	}

	// Calculate defense ratings from owning actor's base stat properties and the attack point modifiers
	if (_fortitude_modifier <= -1.0f) // If the modifier is less than or equal to -100%, set the total defense to zero
		_total_physical_defense = 0;
	else
		_total_physical_defense = _actor_owner->GetFortitude() + static_cast<int32>(_actor_owner->GetFortitude() * _fortitude_modifier);

	if (_protection_modifier <= -1.0f) // If the modifier is less than or equal to -100%, set the total defense to zero
		_total_metaphysical_defense = 0;
	else
		_total_metaphysical_defense = _actor_owner->GetProtection() + static_cast<int32>(_actor_owner->GetProtection() * _protection_modifier);

	// If present, add defense ratings from the armor equipped
	if (equipped_armor != NULL) {
		_total_physical_defense += equipped_armor->GetPhysicalDefense();
		_total_metaphysical_defense += equipped_armor->GetMetaphysicalDefense();
	}
}



void GlobalAttackPoint::CalculateTotalEvade() {
	if (_actor_owner == NULL) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "attack point has no owning actor" << endl;
		return;
	}

	// Calculate evade ratings from owning actor's base evade stat and the evade modifier
	if (_fortitude_modifier <= -1.0f) // If the modifier is less than or equal to -100%, set the total evade to zero
		_total_evade_rating = 0.0f;
	else
		_total_evade_rating = _actor_owner->GetEvade() + (_actor_owner->GetEvade() * _evade_modifier);
}

////////////////////////////////////////////////////////////////////////////////
// GlobalActor class
////////////////////////////////////////////////////////////////////////////////

GlobalActor::GlobalActor() :
	_id(0),
	_experience_level(0),
	_experience_points(0),
	_hit_points(0),
	_max_hit_points(0),
	_skill_points(0),
	_max_skill_points(0),
	_strength(0),
	_vigor(0),
	_fortitude(0),
	_protection(0),
	_agility(0),
	_evade(0.0f),
	_total_physical_attack(0),
	_total_metaphysical_attack(0),
	_weapon_equipped(NULL)
{}



GlobalActor::~GlobalActor() {
	// Delete all attack points
	for (uint32 i = 0; i < _attack_points.size(); i++) {
		delete _attack_points[i];
	}
	_attack_points.clear();

	// Delete all equipment
	if (_weapon_equipped != NULL)
		delete _weapon_equipped;
	for (uint32 i = 0; i < _armor_equipped.size(); i++) {
		if (_armor_equipped[i] != NULL)
			delete _armor_equipped[i];
	}
	_armor_equipped.clear();

	// Delete all skills
	for (map<uint32, GlobalSkill*>::iterator i = _skills.begin(); i != _skills.end(); i++) {
		delete i->second;
	}
	_skills.clear();
}



GlobalActor::GlobalActor(const GlobalActor& copy) {
	_id = copy._id;
	_name = copy._name;
	_filename = copy._filename;
	_experience_level = copy._experience_level;
	_experience_points = copy._experience_points;
	_hit_points = copy._hit_points;
	_max_hit_points = copy._max_hit_points;
	_skill_points = copy._skill_points;
	_max_skill_points = copy._max_skill_points;
	_strength = copy._strength;
	_vigor = copy._vigor;
	_fortitude = copy._fortitude;
	_protection = copy._protection;
	_agility = copy._agility;
	_evade = copy._evade;
	_total_physical_attack = copy._total_physical_attack;
	_total_metaphysical_attack = copy._total_metaphysical_attack;

	// Copy all attack points
	for (uint32 i = 0; i < copy._attack_points.size(); i++) {
		_attack_points.push_back(new GlobalAttackPoint(*copy._attack_points[i]));
		_attack_points[i]->SetActorOwner(this);
	}

	// Copy all equipment
	if (copy._weapon_equipped == NULL)
		_weapon_equipped = NULL;
	else
		_weapon_equipped = new GlobalWeapon(*copy._weapon_equipped);

	for (uint32 i = 0; i < _armor_equipped.size(); i++) {
		if (_armor_equipped[i] == NULL)
			_armor_equipped.push_back(NULL);
		else
			_armor_equipped.push_back(new GlobalArmor(*copy._armor_equipped[i]));
	}

	// Copy all skills
	for (map<uint32, GlobalSkill*>::const_iterator i = copy._skills.begin(); i != copy._skills.end(); i++) {
		_skills.insert(make_pair(i->first, new GlobalSkill(*(i->second))));
	}
}



GlobalActor& GlobalActor::operator=(const GlobalActor& copy) {
	if (this == &copy) // Handle self-assignment case
		return *this;

	_id = copy._id;
	_name = copy._name;
	_filename = copy._filename;
	_experience_level = copy._experience_level;
	_experience_points = copy._experience_points;
	_hit_points = copy._hit_points;
	_max_hit_points = copy._max_hit_points;
	_skill_points = copy._skill_points;
	_max_skill_points = copy._max_skill_points;
	_strength = copy._strength;
	_vigor = copy._vigor;
	_fortitude = copy._fortitude;
	_protection = copy._protection;
	_agility = copy._agility;
	_evade = copy._evade;
	_total_physical_attack = copy._total_physical_attack;
	_total_metaphysical_attack = copy._total_metaphysical_attack;

	// Copy all attack points
	for (uint32 i = 0; i < copy._attack_points.size(); i++) {
		_attack_points.push_back(new GlobalAttackPoint(*_attack_points[i]));
		_attack_points[i]->SetActorOwner(this);
	}

	// Copy all equipment
	if (copy._weapon_equipped == NULL)
		_weapon_equipped = NULL;
	else
		_weapon_equipped = new GlobalWeapon(*copy._weapon_equipped);

	for (uint32 i = 0; i < _armor_equipped.size(); i++) {
		if (_armor_equipped[i] == NULL)
			_armor_equipped.push_back(NULL);
		else
			_armor_equipped.push_back(new GlobalArmor(*copy._armor_equipped[i]));
	}

	// Copy all skills
	for (map<uint32, GlobalSkill*>::const_iterator i = copy._skills.begin(); i != copy._skills.end(); i++) {
		_skills.insert(make_pair(i->first, new GlobalSkill(*(i->second))));
	}
	return *this;
}



GlobalWeapon* GlobalActor::EquipWeapon(GlobalWeapon* weapon) {
	GlobalWeapon* old_weapon = _weapon_equipped;
	_weapon_equipped = weapon;
	_CalculateAttackRatings();
	return old_weapon;
}



GlobalArmor* GlobalActor::EquipArmor(GlobalArmor* armor, uint32 index) {
	if (index >= _armor_equipped.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of pieces of armor equipped: " << index << endl;
		return armor;
	}

	GlobalArmor* old_armor = _armor_equipped[index];
	_armor_equipped[index] = armor;

	if (old_armor != NULL && armor != NULL) {
		if (old_armor->GetObjectType() != armor->GetObjectType()) {
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "old armor was replaced with a different type of armor" << endl;
		}
	}

	_attack_points[index]->CalculateTotalDefense(_armor_equipped[index]);
	return old_armor;
}



uint32 GlobalActor::GetTotalPhysicalDefense(uint32 index) const {
	if (index >= _attack_points.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of attack points: " << index << endl;
		return 0;
	}

	return _attack_points[index]->GetTotalPhysicalDefense();
}



uint32 GlobalActor::GetTotalMetaphysicalDefense(uint32 index) const {
	if (index >= _attack_points.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of attack points: " << index << endl;
		return 0;
	}

	return _attack_points[index]->GetTotalMetaphysicalDefense();
}



float GlobalActor::GetTotalEvadeRating(uint32 index) const {
	if (index >= _attack_points.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of attack points: " << index << endl;
		return 0.0f;
	}

	return _attack_points[index]->GetTotalEvadeRating();
}



GlobalArmor* GlobalActor::GetArmorEquipped(uint32 index) const {
	if (index >= _armor_equipped.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of pieces of armor equipped: " << index << endl;
		return NULL;
	}

	return _armor_equipped[index];
}



GlobalAttackPoint* GlobalActor::GetAttackPoint(uint32 index) const {
	if (index >= _attack_points.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of attack points: " << index << endl;
		return NULL;
	}

	return _attack_points[index];
}



GlobalSkill* GlobalActor::GetSkill(uint32 skill_id) const {
	map<uint32, GlobalSkill*>::const_iterator skill_location = _skills.find(skill_id);
	if (skill_location == _skills.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "actor did not have a skill with the requested skill_id: " << skill_id << endl;
		return NULL;
	}

	return skill_location->second;
}



GlobalSkill* GlobalActor::GetSkill(const GlobalSkill* skill) const {
	if (skill == NULL) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a NULL pointer argument" << endl;
		return NULL;
	}

	return GetSkill(skill->GetID());
}



void GlobalActor::AddHitPoints(uint32 amount) {
	if ((0xFFFFFFFF - amount) < _hit_points) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << endl;
		_hit_points = 0xFFFFFFFF;
	}
	else {
		_hit_points += amount;
	}

	if (_hit_points > _max_hit_points)
		_hit_points = _max_hit_points;
}



void GlobalActor::SubtractHitPoints(uint32 amount) {
	if (amount >= _hit_points)
		_hit_points = 0;
	else
		_hit_points -= amount;
}



void GlobalActor::AddMaxHitPoints(uint32 amount) {
	if ((0xFFFFFFFF - amount) < _max_hit_points) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << endl;
		_max_hit_points = 0xFFFFFFFF;
	}
	else {
		_max_hit_points += amount;
	}
}



void GlobalActor::SubtractMaxHitPoints(uint32 amount) {
	if (amount > _max_hit_points) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "argument value will cause max hit points to decrease to zero: " << amount << endl;
		_max_hit_points = 0;
		_hit_points = 0;
	}
	else {
		_max_hit_points -= amount;
		if (_hit_points > _max_hit_points)
			_hit_points = _max_hit_points;
	}
}



void GlobalActor::AddSkillPoints(uint32 amount) {
	if ((0xFFFFFFFF - amount) < _skill_points) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << endl;
		_skill_points = 0xFFFFFFFF;
	}
	else {
		_skill_points += amount;
	}

	if (_skill_points > _max_skill_points)
		_skill_points = _max_skill_points;
}



void GlobalActor::SubtractSkillPoints(uint32 amount) {
	if (amount >= _skill_points)
		_skill_points = 0;
	else
		_skill_points -= amount;
}



void GlobalActor::AddMaxSkillPoints(uint32 amount) {
	if ((0xFFFFFFFF - amount) < _max_skill_points) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << endl;
		_max_skill_points = 0xFFFFFFFF;
	}
	else {
		_max_skill_points += amount;
	}
}



void GlobalActor::SubtractMaxSkillPoints(uint32 amount) {
	if (amount > _max_skill_points) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "argument value will cause max skill points to decrease to zero: " << amount << endl;
		_max_skill_points = 0;
		_skill_points = 0;
	}
	else {
		_max_skill_points -= amount;
		if (_skill_points > _max_skill_points)
			_skill_points = _max_skill_points;
	}
}



void GlobalActor::AddStrength(uint32 amount) {
	if ((0xFFFFFFFF - amount) < _strength) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << endl;
		_strength = 0xFFFFFFFF;
	}
	else {
		_strength += amount;
	}

	_CalculateAttackRatings();
}



void GlobalActor::SubtractStrength(uint32 amount) {
	if (amount >= _strength)
		_strength = 0;
	else
		_strength -= amount;

	_CalculateAttackRatings();
}



void GlobalActor::AddVigor(uint32 amount) {
	if ((0xFFFFFFFF - amount) < _vigor) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << endl;
		_vigor = 0xFFFFFFFF;
	}
	else {
		_vigor += amount;
	}

	_CalculateAttackRatings();
}



void GlobalActor::SubtractVigor(uint32 amount) {
	if (amount >= _vigor)
		_vigor = 0;
	else
		_vigor -= amount;

	_CalculateAttackRatings();
}



void GlobalActor::AddFortitude(uint32 amount) {
	if ((0xFFFFFFFF - amount) < _fortitude) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << endl;
		_fortitude = 0xFFFFFFFF;
	}
	else {
		_fortitude += amount;
	}

	_CalculateDefenseRatings();
}



void GlobalActor::SubtractFortitude(uint32 amount) {
	if (amount >= _fortitude)
		_fortitude = 0;
	else
		_fortitude -= amount;

	_CalculateDefenseRatings();
}



void GlobalActor::AddProtection(uint32 amount) {
	if ((0xFFFFFFFF - amount) < _protection) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << endl;
		_protection = 0xFFFFFFFF;
	}
	else {
		_protection += amount;
	}

	_CalculateDefenseRatings();
}



void GlobalActor::SubtractProtection(uint32 amount) {
	if (amount >= _protection)
		_protection = 0;
	else
		_protection -= amount;

	_CalculateDefenseRatings();
}



void GlobalActor::AddAgility(uint32 amount)  {
	if ((0xFFFFFFFF - amount) < _agility) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << endl;
		_agility = 0xFFFFFFFF;
	}
	else {
		_agility += amount;
	}
}



void GlobalActor::SubtractAgility(uint32 amount) {
	if (amount >= _agility)
		_agility = 0;
	else
		_agility -= amount;
}



void GlobalActor::AddEvade(float amount) {
	if (amount < 0.0f) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received negative argument value: " << amount << endl;
		return;
	}

	float new_evade = _evade + amount;
	if (new_evade < _evade) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "floating point overflow condition detected: " << amount << endl;
		_evade = 1.0f;
	}
	else if (new_evade > 1.0f) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "evade rating increased above 1.0f: " << amount << endl;
		_evade = 1.0f;
	}
	else {
		_evade = new_evade;
	}

	_CalculateEvadeRatings();
}



void GlobalActor::SubtractEvade(float amount) {
	if (amount > 0.0f) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received positive argument value: " << amount << endl;
		return;
	}

	float new_evade = _evade + amount;
	if (new_evade > _evade) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "floating point overflow condition detected: " << amount << endl;
		_evade = 0.0f;
	}
	else if (new_evade < 0.0f) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "evade rating decreased below 0.0f: " << amount << endl;
		_evade = 0.0f;
	}
	else {
		_evade = new_evade;
	}

	_CalculateEvadeRatings();
}



void GlobalActor::_CalculateAttackRatings() {
	_total_physical_attack = _strength;
	_total_metaphysical_attack = _vigor;

	if (_weapon_equipped != NULL) {
		_total_physical_attack += _weapon_equipped->GetPhysicalAttack();
		_total_metaphysical_attack += _weapon_equipped->GetMetaphysicalAttack();
	}
}



void GlobalActor::_CalculateDefenseRatings() {
	// Re-calculate the defense ratings for all attack points
	for (uint32 i = 0; i < _attack_points.size(); i++) {
		if ((i < _armor_equipped.size()) && (_armor_equipped[i] != NULL))
			_attack_points[i]->CalculateTotalDefense(_armor_equipped[i]);
		else
			_attack_points[i]->CalculateTotalDefense(NULL);
	}
}



void GlobalActor::_CalculateEvadeRatings() {
	// Re-calculate the evade ratings for all attack points
	for (uint32 i = 0; i < _attack_points.size(); i++) {
		_attack_points[i]->CalculateTotalEvade();
	}
}

////////////////////////////////////////////////////////////////////////////////
// GlobalCharacterGrowth class
////////////////////////////////////////////////////////////////////////////////

GlobalCharacterGrowth::GlobalCharacterGrowth(GlobalCharacter* owner) :
	_character_owner(owner),
	_experience_level_gained(false),
	_growth_detected(false),
	_experience_for_next_level(0),
	_experience_for_last_level(0),
	_hit_points_growth(0),
	_skill_points_growth(0),
	_strength_growth(0),
	_vigor_growth(0),
	_fortitude_growth(0),
	_protection_growth(0),
	_agility_growth(0),
	_evade_growth(0.0f)
{}



GlobalCharacterGrowth::~GlobalCharacterGrowth() {
	for (uint32 i = 0; i < _skills_learned.size(); i++)
		delete _skills_learned[i];
	_skills_learned.clear();
}



void GlobalCharacterGrowth::AcknowledgeGrowth() {
	if (_growth_detected == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function was invoked when there was no character growth detected" << endl;
		return;
	}

	_growth_detected = false;

	// Add all growth stats to the character actor
	if (_hit_points_growth != 0) {
		_character_owner->AddMaxHitPoints(_hit_points_growth);
		_character_owner->AddHitPoints(_hit_points_growth);
	}
	if (_skill_points_growth != 0) {
		_character_owner->AddMaxSkillPoints(_skill_points_growth);
		_character_owner->AddSkillPoints(_skill_points_growth);
	}
	if (_strength_growth != 0)
		_character_owner->AddStrength(_strength_growth);
	if (_vigor_growth != 0)
		_character_owner->AddVigor(_vigor_growth);
	if (_fortitude_growth != 0)
		_character_owner->AddFortitude(_fortitude_growth);
	if (_protection_growth != 0)
		_character_owner->AddProtection(_protection_growth);
	if (_agility_growth != 0)
		_character_owner->AddAgility(_agility_growth);
	if (IsFloatEqual(_evade_growth, 0.0f) == false)
		_character_owner->AddEvade(_evade_growth);

	_hit_points_growth = 0;
	_skill_points_growth = 0;
	_strength_growth = 0;
	_vigor_growth = 0;
	_fortitude_growth = 0;
	_protection_growth = 0;
	_agility_growth = 0;
	_evade_growth = 0.0f;

	// If a new experience level has been gained, we must retrieve the growth data for the new experience level
	if (_experience_level_gained) {
		_character_owner->_experience_level += 1;
		_experience_level_gained = false;
		_DetermineNextLevelExperience();

		string filename = "dat/actors/characters.lua";
		ReadScriptDescriptor character_script;
		if (character_script.OpenFile(filename) == false) {
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to open character data file: " << filename << endl;
			return;
		}

		try {
			ScriptCallFunction<void>(character_script.GetLuaState(), "DetermineGrowth", _character_owner);
			_ConstructPeriodicGrowth();
			_CheckForGrowth();
		}
		catch (luabind::error e) {
			ScriptManager->HandleLuaError(e);
		}
		catch (luabind::cast_failed e) {
			ScriptManager->HandleCastError(e);
		}

		character_script.CloseFile();

		// Add any newly learned skills
		for (uint32 i = 0; i < _skills_learned.size(); i++) {
			GlobalSkill* skill = _skills_learned[i];
			if (_character_owner->_skills.find(skill->GetID()) != _character_owner->_skills.end()) {
				IF_PRINT_WARNING(GLOBAL_DEBUG) << "character had already learned the skill with the id: " << skill->GetID() << endl;
				delete _skills_learned[i];
				continue;
			}

			// Insert the pointer to the new skill inside of the global skills map and the skill type vector
			_character_owner->_skills.insert(make_pair(skill->GetID(), skill));
			switch (skill->GetType()) {
				case GLOBAL_SKILL_ATTACK:
					_character_owner->_attack_skills.push_back(skill);
					break;
				case GLOBAL_SKILL_DEFEND:
					_character_owner->_defense_skills.push_back(skill);
					break;
				case GLOBAL_SKILL_SUPPORT:
					_character_owner->_support_skills.push_back(skill);
					break;
				default:
					IF_PRINT_WARNING(GLOBAL_DEBUG) << "newly learned skill had an unknown skill type: " << skill->GetType() << endl;
					break;
			}
		}
		// skills have been given out, clean out the vector
		//CD: Negatory!  Leeave the vector as is.  I need it in order to display the
		// correct stuff in the skills learned window in FinishWindow.  I will clear it
		// myself from there.  Not only that, but if they level up multiple times, I need
		// to save the whole list so I can show all the skills they learned across
		// multiple levels.  If this were called, they would never know they learned a skill
		// at level 3 because they jumped to level 4 in the same battle
		//_skills_learned.clear();
	} // if (_experience_level_gained)
} // void GlobalCharacterGrowth::AcknowledgeGrowth()



void GlobalCharacterGrowth::_AddSkill(uint32 skill_id) {
	if (skill_id == 0) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received an invalid skill_id argument: " << skill_id << endl;
		return;
	}
	// Make sure we don't add a skill to learn more than once
	for (vector<GlobalSkill*>::iterator i = _skills_learned.begin(); i != _skills_learned.end(); i++) {
		if (skill_id == (*i)->GetID()) {
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "the skill to add was already present in the list of skills to learn: " << skill_id << endl;
			return;
		}
	}

	GlobalSkill* skill = new GlobalSkill(skill_id);
	if (skill->IsValid() == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "the skill to add failed to load: " << skill_id << endl;
		delete skill;
	}
	else {
		_skills_learned.push_back(skill);
	}
}



void GlobalCharacterGrowth::_CheckForGrowth() {
	// ----- (1): If a new experience level is gained, empty the periodic growth containers into the growth members
	if (_character_owner->GetExperiencePoints() >= _experience_for_next_level) {
		_experience_level_gained = true;
		_growth_detected = true;

		for (uint32 i = 0; i < _hit_points_periodic_growth.size(); i++)
			_hit_points_growth += _hit_points_periodic_growth[i].second;
		_hit_points_periodic_growth.clear();

		for (uint32 i = 0; i < _skill_points_periodic_growth.size(); i++)
			_skill_points_growth += _skill_points_periodic_growth[i].second;
		_skill_points_periodic_growth.clear();

		for (uint32 i = 0; i < _strength_periodic_growth.size(); i++)
			_strength_growth += _strength_periodic_growth[i].second;
		_strength_periodic_growth.clear();

		for (uint32 i = 0; i < _vigor_periodic_growth.size(); i++)
			_vigor_growth += _vigor_periodic_growth[i].second;
		_vigor_periodic_growth.clear();

		for (uint32 i = 0; i < _fortitude_periodic_growth.size(); i++)
			_fortitude_growth += _fortitude_periodic_growth[i].second;
		_fortitude_periodic_growth.clear();

		for (uint32 i = 0; i < _protection_periodic_growth.size(); i++)
			_protection_growth += _protection_periodic_growth[i].second;
		_protection_periodic_growth.clear();

		for (uint32 i = 0; i < _agility_periodic_growth.size(); i++)
			_agility_growth += _agility_periodic_growth[i].second;
		_agility_periodic_growth.clear();

		for (uint32 i = 0; i < _evade_periodic_growth.size(); i++)
			_evade_growth += _evade_periodic_growth[i].second;
		_evade_periodic_growth.clear();


		return;
	} // if (_actor_ower->GetExperiencePoints() >= _experience_for_next_level)

	// ----- (2): If there is no growth detected, check all periodic growth containers
	switch (_growth_detected) { // switch statement used instead of if statement here so we can break out of it early
		case true:
			break;
		case false:
			if (_hit_points_periodic_growth.empty() == false) {
				if (_character_owner->GetExperiencePoints() >= _hit_points_periodic_growth.front().first) {
					_growth_detected = true;
					break;
				}
			}

			if (_skill_points_periodic_growth.empty() == false) {
				if (_character_owner->GetExperiencePoints() >= _skill_points_periodic_growth.front().first) {
					_growth_detected = true;
					break;
				}
			}

			if (_strength_periodic_growth.empty() == false) {
				if (_character_owner->GetExperiencePoints() >= _strength_periodic_growth.front().first) {
					_growth_detected = true;
					break;
				}
			}

			if (_vigor_periodic_growth.empty() == false) {
				if (_character_owner->GetExperiencePoints() >= _vigor_periodic_growth.front().first) {
					_growth_detected = true;
					break;
				}
			}

			if (_fortitude_periodic_growth.empty() == false) {
				if (_character_owner->GetExperiencePoints() >= _fortitude_periodic_growth.front().first) {
					_growth_detected = true;
					break;
				}
			}

			if (_protection_periodic_growth.empty() == false) {
				if (_character_owner->GetExperiencePoints() >= _protection_periodic_growth.front().first) {
					_growth_detected = true;
					break;
				}
			}

			if (_agility_periodic_growth.empty() == false) {
				if (_character_owner->GetExperiencePoints() >= _agility_periodic_growth.front().first) {
					_growth_detected = true;
					break;
				}
			}

			if (_evade_periodic_growth.empty() == false) {
				if (_character_owner->GetExperiencePoints() >= _evade_periodic_growth.front().first) {
					_growth_detected = true;
					break;
				}
			}
			break;
	} // switch (_growth_detected)

	// ----- (3): If there is growth detected update all periodic growth containers
	if (_growth_detected == true) {
		while (_hit_points_periodic_growth.begin() != _hit_points_periodic_growth.end()) {
			if (_character_owner->GetExperiencePoints() >= _hit_points_periodic_growth.begin()->first) {
				_hit_points_growth += _hit_points_periodic_growth.begin()->second;
				_hit_points_periodic_growth.pop_front();
			}
			else {
				break;
			}
		}

		while (_skill_points_periodic_growth.begin() != _skill_points_periodic_growth.end()) {
			if (_character_owner->GetExperiencePoints() >= _skill_points_periodic_growth.begin()->first) {
				_skill_points_growth += _skill_points_periodic_growth.begin()->second;
				_skill_points_periodic_growth.pop_front();
			}
			else {
				break;
			}
		}

		while (_strength_periodic_growth.begin() != _strength_periodic_growth.end()) {
			if (_character_owner->GetExperiencePoints() >= _strength_periodic_growth.begin()->first) {
				_strength_growth += _strength_periodic_growth.begin()->second;
				_strength_periodic_growth.pop_front();
			}
			else {
				break;
			}
		}

		while (_vigor_periodic_growth.begin() != _vigor_periodic_growth.end()) {
			if (_character_owner->GetExperiencePoints() >= _vigor_periodic_growth.begin()->first) {
				_vigor_growth += _vigor_periodic_growth.begin()->second;
				_vigor_periodic_growth.pop_front();
			}
			else {
				break;
			}
		}

		while (_fortitude_periodic_growth.begin() != _fortitude_periodic_growth.end()) {
			if (_character_owner->GetExperiencePoints() >= _fortitude_periodic_growth.begin()->first) {
				_fortitude_growth += _fortitude_periodic_growth.begin()->second;
				_fortitude_periodic_growth.pop_front();
			}
			else {
				break;
			}
		}

		while (_protection_periodic_growth.begin() != _protection_periodic_growth.end()) {
			if (_character_owner->GetExperiencePoints() >= _protection_periodic_growth.begin()->first) {
				_protection_growth += _protection_periodic_growth.begin()->second;
				_protection_periodic_growth.pop_front();
			}
			else {
				break;
			}
		}

		while (_agility_periodic_growth.begin() != _agility_periodic_growth.end()) {
			if (_character_owner->GetExperiencePoints() >= _agility_periodic_growth.begin()->first) {
				_agility_growth += _agility_periodic_growth.begin()->second;
				_agility_periodic_growth.pop_front();
			}
			else {
				break;
			}
		}

		while (_evade_periodic_growth.begin() != _evade_periodic_growth.end()) {
			if (_character_owner->GetExperiencePoints() >= _evade_periodic_growth.begin()->first) {
				_evade_growth += _evade_periodic_growth.begin()->second;
				_evade_periodic_growth.pop_front();
			}
			else {
				break;
			}
		}
	} // if (_growth_detected == true)
} // void GlobalCharacterGrowth::_CheckForGrowth()



void GlobalCharacterGrowth::_ConstructPeriodicGrowth() {
	// TODO: Implement a gradual growth algorithm

	// TEMP: all growth is done when the experience level is gained
	_hit_points_periodic_growth.push_back(make_pair(_experience_for_next_level, _hit_points_growth));
	_skill_points_periodic_growth.push_back(make_pair(_experience_for_next_level, _skill_points_growth));
	_strength_periodic_growth.push_back(make_pair(_experience_for_next_level, _strength_growth));
	_vigor_periodic_growth.push_back(make_pair(_experience_for_next_level, _vigor_growth));
	_fortitude_periodic_growth.push_back(make_pair(_experience_for_next_level, _fortitude_growth));
	_protection_periodic_growth.push_back(make_pair(_experience_for_next_level, _protection_growth));
	_agility_periodic_growth.push_back(make_pair(_experience_for_next_level, _agility_growth));
	_evade_periodic_growth.push_back(make_pair(_experience_for_next_level, _evade_growth));

	_hit_points_growth = 0;
	_skill_points_growth = 0;
	_strength_growth = 0;
	_vigor_growth = 0;
	_fortitude_growth = 0;
	_protection_growth = 0;
	_agility_growth = 0;
	_evade_growth = 0.0f;
}



void GlobalCharacterGrowth::_DetermineNextLevelExperience() {
	uint32 base_xp = 0;
	uint32 new_xp = 0;

	// TODO: implement a real algorithm for determining the next experience goal
	base_xp = _character_owner->GetExperienceLevel() * 40;
	new_xp = GaussianRandomValue(base_xp, base_xp / 10.0f);

	_experience_for_last_level = _experience_for_next_level;
	_experience_for_next_level = _experience_for_last_level + new_xp;
}

////////////////////////////////////////////////////////////////////////////////
// GlobalCharacter class
////////////////////////////////////////////////////////////////////////////////

GlobalCharacter::GlobalCharacter(uint32 id, bool initial) :
	_growth(this)
{
	_id = id;

	// ----- (1): Open the characters script file
	string filename = "dat/actors/characters.lua";
	ReadScriptDescriptor char_script;
	if (char_script.OpenFile(filename) == false) {
		PRINT_ERROR << "failed to open character data file: " << filename << endl;
		return;
	}

	// ----- (2): Retrieve their basic character property data
	char_script.OpenTable("characters");
	char_script.OpenTable(_id);
	_name = MakeUnicodeString(char_script.ReadString("name"));
	_filename = char_script.ReadString("filename");

	// ----- (3): Construct the character from the initial stats if necessary
	if (initial == true) {
		char_script.OpenTable("initial_stats");
		_experience_level = char_script.ReadUInt("experience_level");
		_experience_points = char_script.ReadUInt("experience_points");
		_max_hit_points = char_script.ReadUInt("max_hit_points");
		_hit_points = _max_hit_points;
		_max_skill_points = char_script.ReadUInt("max_skill_points");
		_skill_points = _max_skill_points;
		_strength = char_script.ReadUInt("strength");
		_vigor = char_script.ReadUInt("vigor");
		_fortitude = char_script.ReadUInt("fortitude");
		_protection = char_script.ReadUInt("protection");
		_agility = char_script.ReadUInt("agility");
		_evade = char_script.ReadFloat("evade");

		// Add the character's initial equipment. If any equipment ids are zero, that indicates nothing is to be equipped.
		uint32 equipment_id = 0;
		equipment_id = char_script.ReadUInt("weapon");
		if (equipment_id != 0)
			_weapon_equipped = new GlobalWeapon(equipment_id);
		else
			_weapon_equipped = NULL;

		equipment_id = char_script.ReadUInt("head_armor");
		if (equipment_id != 0)
			_armor_equipped.push_back(new GlobalArmor(equipment_id));
		else
			_armor_equipped.push_back(NULL);

		equipment_id = char_script.ReadUInt("torso_armor");
		if (equipment_id != 0)
			_armor_equipped.push_back(new GlobalArmor(equipment_id));
		else
			_armor_equipped.push_back(NULL);

		equipment_id = char_script.ReadUInt("arm_armor");
		if (equipment_id != 0)
			_armor_equipped.push_back(new GlobalArmor(equipment_id));
		else
			_armor_equipped.push_back(NULL);

		equipment_id = char_script.ReadUInt("leg_armor");
		if (equipment_id != 0)
			_armor_equipped.push_back(new GlobalArmor(equipment_id));
		else
			_armor_equipped.push_back(NULL);

		char_script.CloseTable();
		if (char_script.IsErrorDetected()) {
			if (GLOBAL_DEBUG) {
				PRINT_WARNING << "one or more errors occurred while reading initial data - they are listed below" << endl;
				cerr << char_script.GetErrorMessages() << endl;
			}
		}
	} // if (initial == true)
	else {
		 // Make sure the _armor_equipped vector is sized appropriately
		_armor_equipped.resize(4, NULL);
	}

	// ----- (4): Setup the character's attack points
	char_script.OpenTable("attack_points");
	for (uint32 i = GLOBAL_POSITION_HEAD; i <= GLOBAL_POSITION_LEGS; i++) {
		_attack_points.push_back(new GlobalAttackPoint(this));
		char_script.OpenTable(i);
		if (_attack_points[i]->LoadData(char_script) == false) {
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to succesfully load data for attack point: " << i << endl;
		}
		char_script.CloseTable();
	}
	char_script.CloseTable();

	if (char_script.IsErrorDetected()) {
		if (GLOBAL_DEBUG) {
			PRINT_WARNING << "one or more errors occurred while reading attack point data - they are listed below" << endl;
			cerr << char_script.GetErrorMessages() << endl;
		}
	}

	// ----- (5): Construct the character's initial skill set if necessary
	if (initial) {
		// The skills table contains key/value pairs. The key indicate the level required to learn the skill and the value is the skill's id
		vector<uint32> skill_levels;
		char_script.OpenTable("skills");
		char_script.ReadTableKeys(skill_levels);

		// We want to add the skills beginning with the first learned to the last. ReadTableKeys does not guarantee returing the keys in a sorted order,
		// so sort the skills by level before checking each one.
		sort(skill_levels.begin(), skill_levels.end());

		// Only add the skills for which the experience level requirements are met
		for (uint32 i = 0; i < skill_levels.size(); i++) {
			if (skill_levels[i] <= _experience_level) {
				AddSkill(char_script.ReadUInt(skill_levels[i]));
			}
			// Because skill_levels is sorted, all remaining skills will not have their level requirements met
			else {
				break;
			}
		}

		char_script.CloseTable();
		if (char_script.IsErrorDetected()) {
			if (GLOBAL_DEBUG) {
				PRINT_WARNING << "one or more errors occurred while reading skill data - they are listed below" << endl;
				cerr << char_script.GetErrorMessages() << endl;
			}
		}
	} // if (initial)

	char_script.CloseTable(); // "characters[id]"
	char_script.CloseTable(); // "characters"

	// ----- (6): Determine the character's initial growth if necessary
	if (initial) {
		// Initialize the experience level milestones
		_growth._experience_for_last_level = _experience_points;
		_growth._experience_for_next_level = _experience_points;
		_growth._DetermineNextLevelExperience();
		try {
			ScriptCallFunction<void>(char_script.GetLuaState(), "DetermineGrowth", this);
			_growth._ConstructPeriodicGrowth();
		}
		catch (luabind::error e) {
			ScriptManager->HandleLuaError(e);
		}
		catch (luabind::cast_failed e) {
			ScriptManager->HandleCastError(e);
		}
	}

	// ----- (7): Close the script file and calculate all rating totals
	if (char_script.IsErrorDetected()) {
		if (GLOBAL_DEBUG) {
			PRINT_WARNING << "one or more errors occurred while reading final data - they are listed below" << endl;
			cerr << char_script.GetErrorMessages() << endl;
		}
	}
	char_script.CloseFile();

	_CalculateAttackRatings();
	_CalculateDefenseRatings();
	_CalculateEvadeRatings();

	// ----- (8) Load character sprite and portrait images
	// NOTE: The code below is all TEMP and is subject to great change or removal in the future
	// TEMP: load standard map sprite walking frames
	if (ImageDescriptor::LoadMultiImageFromElementGrid(_map_frames_standard, "img/sprites/map/" + _filename + "_walk.png", 4, 6) == false) {
		exit(1);
	}

	// TEMP: Load the character's run animation
	AnimatedImage run;
	vector<StillImage> run_frames;

	if (ImageDescriptor::LoadMultiImageFromElementGrid(run_frames, "img/sprites/map/" + _filename + "_run.png", 4, 6) == false) {
		exit(1);
	}

	// Store only the right-facing run frames in the animated image
	for (uint32 i = 19; i < run_frames.size(); i++) {
		run.AddFrame(run_frames[i], 15);
	}
	run.SetDimensions(64, 128);
	_battle_animation["run"] = run;

	// TEMP: Load the character's idle animation
	AnimatedImage idle;
	idle.SetDimensions(128, 128);
	vector<uint32> idle_timings(4, 15);

	if (idle.LoadFromFrameGrid("img/sprites/map/" + _filename + "_idle.png", idle_timings, 1, 4) == false) {
		exit(1);
	}
	_battle_animation["idle"] = idle;

	// TEMP: Load the character's attack animation
	AnimatedImage attack;
	attack.SetDimensions(128, 128);
	vector<uint32> attack_timings(5, 10);

	if (attack.LoadFromFrameGrid("img/sprites/map/" + _filename + "_attack.png", attack_timings, 1, 5) == false) {
		exit(1);
	}
	_battle_animation["attack"] = attack;

	// TEMP: Load the character's battle portraits from a multi image
	_battle_portraits.assign(5, StillImage());
	for (uint32 i = 0; i < _battle_portraits.size(); i++) {
		_battle_portraits[i].SetDimensions(100, 100);
	}
	if (ImageDescriptor::LoadMultiImageFromElementGrid(_battle_portraits, "img/portraits/battle/" + _filename + "_damage.png", 1, 5) == false)
		exit(1);
} // GlobalCharacter::GlobalCharacter(uint32 id, bool initial)



bool GlobalCharacter::AddExperiencePoints(uint32 xp) {
	_experience_points += xp;
	_growth._CheckForGrowth();
	return _growth.IsGrowthDetected();
}



void GlobalCharacter::AddSkill(uint32 skill_id) {
	if (skill_id == 0) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received an invalid skill_id argument: " << skill_id << endl;
		return;
	}
	if (_skills.find(skill_id) != _skills.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to add skill because the character already knew this skill: " << skill_id << endl;
		return;
	}

	GlobalSkill* skill = new GlobalSkill(skill_id);
	if (skill->IsValid() == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "the skill to add failed to load: " << skill_id << endl;
		delete skill;
		return;
	}

	// Insert the pointer to the new skill inside of the global skills map and the skill type vector
	_skills.insert(make_pair(skill_id, skill));
	switch (skill->GetType()) {
		case GLOBAL_SKILL_ATTACK:
			_attack_skills.push_back(skill);
			break;
		case GLOBAL_SKILL_DEFEND:
			_defense_skills.push_back(skill);
			break;
		case GLOBAL_SKILL_SUPPORT:
			_support_skills.push_back(skill);
			break;
		default:
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "loaded a new skill with an unknown skill type: " << skill->GetType() << endl;
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////
// GlobalEnemy class
////////////////////////////////////////////////////////////////////////////////

GlobalEnemy::GlobalEnemy(uint32 id) :
	GlobalActor(),
	_no_stat_randomization(false),
	_sprite_width(0),
	_sprite_height(0),
	_drunes_dropped(0)
{
	_id = id;

	// ----- (1): Use the id member to determine the name of the data file that the enemy is defined in
	string file_ext;
	string filename;

	if (_id == 0)
		PRINT_ERROR << "invalid id for loading enemy data: " << _id << endl;
	else if ((_id > 0) && (_id <= 100))
		file_ext = "01";
	else if ((_id > 100) && (_id <= 200))
		file_ext = "02";

	filename = "dat/actors/enemies_set_" + file_ext + ".lua";

	// ----- (2): Open the script file and table that store the enemy data
	ReadScriptDescriptor enemy_data;
	if (enemy_data.OpenFile(filename) == false) {
		PRINT_ERROR << "failed to open enemy data file: " << filename << endl;
		return;
	}

	enemy_data.OpenTable("enemies");
	enemy_data.OpenTable(_id);

	// ----- (3): Load the enemy's name and sprite data
	_name = MakeUnicodeString(enemy_data.ReadString("name"));
	_filename = enemy_data.ReadString("filename");
	_sprite_width = enemy_data.ReadInt("sprite_width");
	_sprite_height = enemy_data.ReadInt("sprite_height");

	// ----- (4): Attempt to load the MultiImage for the sprite's frames, which should contain one row and four columns of images
	_battle_sprite_frames.assign(4, StillImage());
	string sprite_filename = "img/sprites/battle/enemies/" + _filename + ".png";
	if (ImageDescriptor::LoadMultiImageFromElementGrid(_battle_sprite_frames, sprite_filename, 1, 4) == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to load sprite frames for enemy: " << sprite_filename << endl;
	}

	// ----- (5): Load the enemy's base stats
	if (enemy_data.DoesBoolExist("no_stat_randomization") == true) {
		_no_stat_randomization = enemy_data.ReadBool("no_stat_randomization");
	}

	enemy_data.OpenTable("base_stats");
	_max_hit_points = enemy_data.ReadUInt("hit_points");
	_hit_points = _max_hit_points;
	_max_skill_points = enemy_data.ReadUInt("skill_points");
	_skill_points = _max_skill_points;
	_experience_points = enemy_data.ReadUInt("experience_points");
	_strength = enemy_data.ReadUInt("strength");
	_vigor = enemy_data.ReadUInt("vigor");
	_fortitude = enemy_data.ReadUInt("fortitude");
	_protection = enemy_data.ReadUInt("protection");
	_agility = enemy_data.ReadUInt("agility");
	_evade = enemy_data.ReadFloat("evade");
	_drunes_dropped = enemy_data.ReadUInt("drunes");
	enemy_data.CloseTable();

	// ----- (6): Create the attack points for the enemy
	enemy_data.OpenTable("attack_points");
	uint32 ap_size = enemy_data.GetTableSize();
	for (uint32 i = 1; i <= ap_size; i++) {
		_attack_points.push_back(new GlobalAttackPoint(this));
		enemy_data.OpenTable(i);
		if (_attack_points.back()->LoadData(enemy_data) == false) {
			IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to load data for an attack point: " << i << endl;
		}
		enemy_data.CloseTable();
	}
	enemy_data.CloseTable();

	// ----- (7): Add the set of skills for the enemy
	enemy_data.OpenTable("skills");
	for (uint32 i = 1; i <= enemy_data.GetTableSize(); i++) {
		_skill_set.push_back(enemy_data.ReadUInt(i));
	}
	enemy_data.CloseTable();

	// ----- (8): Load the possible items that the enemy may drop
	enemy_data.OpenTable("drop_objects");
	for (uint32 i = 1; i <= enemy_data.GetTableSize(); i++) {
		enemy_data.OpenTable(i);
		_dropped_objects.push_back(enemy_data.ReadUInt(1));
		_dropped_chance.push_back(enemy_data.ReadFloat(2));
		enemy_data.CloseTable();
	}
	enemy_data.CloseTable();

	enemy_data.CloseTable(); // enemies[_id]
	enemy_data.CloseTable(); // enemies

	if (enemy_data.IsErrorDetected()) {
		if (GLOBAL_DEBUG) {
			PRINT_WARNING << "one or more errors occurred while reading the enemy data - they are listed below" << endl;
			cerr << enemy_data.GetErrorMessages() << endl;
		}
	}

	enemy_data.CloseFile();

	_CalculateAttackRatings();
	_CalculateDefenseRatings();
	_CalculateEvadeRatings();
} // GlobalEnemy::GlobalEnemy(uint32 id)



void GlobalEnemy::AddSkill(uint32 skill_id) {
	if (skill_id == 0) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received an invalid skill_id argument: " << skill_id << endl;
		return;
	}
	if (_skills.find(skill_id) != _skills.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to add skill because the enemy already knew this skill: " << skill_id << endl;
		return;
	}

	GlobalSkill* skill = new GlobalSkill(skill_id);
	if (skill->IsValid() == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "the skill to add failed to load: " << skill_id << endl;
		delete skill;
		return;
	}

	// Insert the pointer to the new skill inside of the global skills map and the skill type vector
	_skills.insert(make_pair(skill_id, skill));
}



void GlobalEnemy::Initialize() {
	if (_skills.empty() == false) { // Indicates that the enemy has already been initialized
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function was invoked for an already initialized enemy: " << _id << endl;
		return;
	}

	// TODO: we may wish to actually define XP levels for enemies in their data table, though I don't know what purpose it may serve
	_experience_level = 1;

	// ----- (1): Add all new skills that should be available at the current experience level
	for (uint32 i = 0; i < _skill_set.size(); i++) {
		AddSkill(_skill_set[i]);
	}

	if (_skills.empty()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "no skills were added for the enemy: " << _id << endl;
	}

	// ----- (3): Randomize the stats by using a guassian random variable
	if (_no_stat_randomization == false) {
		// Use the base stats as the means and a standard deviation of 10% of the mean
		_max_hit_points     = GaussianRandomValue(_max_hit_points, _max_hit_points / 10.0f);
		_max_skill_points   = GaussianRandomValue(_max_skill_points, _max_skill_points / 10.0f);
		_experience_points  = GaussianRandomValue(_experience_points, _experience_points / 10.0f);
		_strength           = GaussianRandomValue(_strength, _strength / 10.0f);
		_vigor              = GaussianRandomValue(_strength, _strength / 10.0f);
		_fortitude          = GaussianRandomValue(_fortitude, _fortitude / 10.0f);
		_protection         = GaussianRandomValue(_protection, _protection / 10.0f);
		_agility            = GaussianRandomValue(_agility, _agility / 10.0f);
		// TODO: need a gaussian random var function that takes a float arg
		//_evade              = static_cast<float>(GaussianRandomValue(_evade, _evade / 10.0f));
		_drunes_dropped     = GaussianRandomValue(_drunes_dropped, _drunes_dropped / 10.0f);
	}

	// ----- (4): Set the current hit points and skill points to their new maximum values
	_hit_points = _max_hit_points;
	_skill_points = _max_skill_points;
} // void GlobalEnemy::Initialize(uint32 xp_level)



void GlobalEnemy::DetermineDroppedObjects(vector<GlobalObject*>& objects) {
	objects.clear();

	for (uint32 i = 0; i < _dropped_objects.size(); i++) {
		if (RandomFloat() < _dropped_chance[i]) {
			objects.push_back(GlobalCreateNewObject(_dropped_objects[i]));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// GlobalParty class
////////////////////////////////////////////////////////////////////////////////

void GlobalParty::AddActor(GlobalActor* actor, int32 index) {
	if (actor == NULL) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a NULL actor argument" << endl;
		return;
	}

	if (_allow_duplicates == false) {
		// Check that this actor is not already in the party
		for (uint32 i = 0; i < _actors.size(); i++) {
			if (actor->GetID() == _actors[i]->GetID()) {
				IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to add an actor that was already in the party "
					<< "when duplicates were not allowed: " << actor->GetID() << endl;
				return;
			}
		}
	}

	// Add actor to the end of the party if index is negative
	if (index < 0) {
		_actors.push_back(actor);
		return;
	}

	// Check that the requested index does not exceed the size of the container
	if (static_cast<uint32>(index) >= _actors.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded the current party size: " << index << endl;
		_actors.push_back(actor); // Add the actor to the end of the party instead
		return;
	}
	else {
		vector<GlobalActor*>::iterator position = _actors.begin();
		for (int32 i = 0; i < index; i++, position++);
		_actors.insert(position, actor);
	}
}



GlobalActor* GlobalParty::RemoveActorAtIndex(uint32 index) {
	if (index >= _actors.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded current party size: " << index << endl;
		return NULL;
	}

	GlobalActor* removed_actor = _actors[index];
	vector<GlobalActor*>::iterator position = _actors.begin();
	for (uint32 i = 0; i < index; i++, position++);
	_actors.erase(position);

	return removed_actor;
}



GlobalActor* GlobalParty::RemoveActorByID(uint32 id) {
	if (_allow_duplicates) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "tried to remove actor when duplicates were allowed in the party: " << id << endl;
		return NULL;
	}

	GlobalActor* removed_actor = NULL;
	for (vector<GlobalActor*>::iterator position = _actors.begin(); position != _actors.end(); position++) {
		if (id == (*position)->GetID()) {
			removed_actor = *position;
			_actors.erase(position);
			break;
		}
	}

	if (removed_actor == NULL) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an actor in the party with the requested id: " << id << endl;
	}

	return removed_actor;
}



GlobalActor* GlobalParty::GetActorAtIndex(uint32 index) const {
	if (index >= _actors.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded current party size: " << index << endl;
		return NULL;
	}

	return _actors[index];
}



GlobalActor* GlobalParty::GetActorByID(uint32 id) const {
	if (_allow_duplicates) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "tried to retrieve actor when duplicates were allowed in the party: " << id << endl;
		return NULL;
	}

	for (uint32 i = 0; i < _actors.size(); i++) {
		if (_actors[i]->GetID() == id) {
			return _actors[i];
		}
	}

	IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an actor in the party with the requested id: " << id << endl;
	return NULL;
}



void GlobalParty::SwapActorsByIndex(uint32 first_index, uint32 second_index) {
	if (first_index == second_index) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "first_index and second_index arguments had the same value: " << first_index << endl;
		return;
	}
	if (first_index >= _actors.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "first_index argument exceeded current party size: " << first_index << endl;
		return;
	}
	if (second_index >= _actors.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "second_index argument exceeded current party size: " << second_index << endl;
		return;
	}

	GlobalActor* tmp = _actors[first_index];
	_actors[first_index] = _actors[second_index];
	_actors[second_index] = tmp;
}



void GlobalParty::SwapActorsByID(uint32 first_id, uint32 second_id) {
	if (first_id == second_id) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "first_id and second_id arguments had the same value: " << first_id << endl;
		return;
	}
	if (_allow_duplicates) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "tried to swap actors when duplicates were allowed in the party: " << first_id << endl;
		return;
	}

	vector<GlobalActor*>::iterator first_position;
	vector<GlobalActor*>::iterator second_position;
	for (first_position = _actors.begin(); first_position != _actors.end(); first_position++) {
		if ((*first_position)->GetID() == first_id)
			break;
	}
	for (second_position = _actors.begin(); second_position != _actors.end(); second_position++) {
		if ((*second_position)->GetID() == second_id)
			break;
	}

	if (first_position == _actors.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an actor in the party with the requested first_id: " << first_id << endl;
		return;
	}
	if (second_position == _actors.end()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an actor in the party with the requested second_id: " << second_id << endl;
		return;
	}

	GlobalActor* tmp = *first_position;
	*first_position = *second_position;
	*second_position = tmp;
}



GlobalActor* GlobalParty::ReplaceActorByIndex(uint32 index, GlobalActor* new_actor) {
	if (new_actor == NULL) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a NULL new_actor argument" << endl;
		return NULL;
	}
	if (index >= _actors.size()) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded current party size: " << index << endl;
		return NULL;
	}

	GlobalActor* tmp = _actors[index];
	_actors[index] = new_actor;
	return tmp;
}



GlobalActor* GlobalParty::ReplaceActorByID(uint32 id, GlobalActor* new_actor) {
	if (_allow_duplicates) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "tried to replace actor when duplicates were allowed in the party: " << id << endl;
		return NULL;
	}
	if (new_actor == NULL) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a NULL new_actor argument" << endl;
		return NULL;
	}

	GlobalActor* removed_actor = NULL;
	for (vector<GlobalActor*>::iterator position = _actors.begin(); position != _actors.end(); position++) {
		if ((*position)->GetID() == id) {
			removed_actor = *position;
			*position = new_actor;
			break;
		}
	}

	if (removed_actor == NULL) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an actor in the party with the requested id: " << id << endl;
	}

	return removed_actor;
}



float GlobalParty::AverageExperienceLevel() const {
	if (_actors.empty())
		return 0.0f;

	float xp_level_sum = 0.0f;
	for (uint32 i = 0; i < _actors.size(); i++)
		xp_level_sum += static_cast<float>(_actors[i]->GetExperienceLevel());
	return (xp_level_sum / static_cast<float>(_actors.size()));
}



void GlobalParty::AddHitPoints(uint32 hp) {
	for (vector<GlobalActor*>::iterator i = _actors.begin(); i != _actors.end(); i++) {
		(*i)->AddHitPoints(hp);
	}
}

} // namespace hoa_global
