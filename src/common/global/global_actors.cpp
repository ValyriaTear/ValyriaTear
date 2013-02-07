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

#include "global_actors.h"

#include "engine/script/script_read.h"
#include "global_objects.h"
#include "global_effects.h"
#include "global_skills.h"

using namespace hoa_utils;
using namespace hoa_video;
using namespace hoa_script;

namespace hoa_global
{

////////////////////////////////////////////////////////////////////////////////
// GlobalAttackPoint class
////////////////////////////////////////////////////////////////////////////////

GlobalAttackPoint::GlobalAttackPoint(GlobalActor *owner) :
    _actor_owner(owner),
    _x_position(0),
    _y_position(0),
    _fortitude_modifier(0.0f),
    _protection_modifier(0.0f),
    _evade_modifier(0.0f),
    _total_physical_defense(0),
    _total_magical_defense(0),
    _total_evade_rating(0)
{}



bool GlobalAttackPoint::LoadData(ReadScriptDescriptor &script)
{
    if(script.IsFileOpen() == false) {
        return false;
    }

    _name = MakeUnicodeString(script.ReadString("name"));
    _x_position = script.ReadInt("x_position");
    _y_position = script.ReadInt("y_position");
    _fortitude_modifier = script.ReadFloat("fortitude_modifier");
    _protection_modifier = script.ReadFloat("protection_modifier");
    _evade_modifier = script.ReadFloat("evade_modifier");

    // Status effect data is optional so check if a status_effect table exists first
    if(script.DoesTableExist("status_effects") == true) {
        script.OpenTable("status_effects");

        std::vector<int32> table_keys;
        script.ReadTableKeys(table_keys);
        for(uint32 i = 0; i < table_keys.size(); i++) {
            float probability = script.ReadFloat(table_keys[i]);
            _status_effects.push_back(std::make_pair(static_cast<GLOBAL_STATUS>(table_keys[i]), probability));
        }

        script.CloseTable();
    }


    if(script.IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading the save game file - they are listed below"
                          << std::endl
                          << script.GetErrorMessages() << std::endl;
        }
        return false;
    }

    return true;
}



void GlobalAttackPoint::CalculateTotalDefense(const GlobalArmor *equipped_armor)
{
    if(_actor_owner == NULL) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "attack point has no owning actor" << std::endl;
        return;
    }

    // Calculate defense ratings from owning actor's base stat properties and the attack point modifiers
    if(_fortitude_modifier <= -1.0f)  // If the modifier is less than or equal to -100%, set the total defense to zero
        _total_physical_defense = 0;
    else
        _total_physical_defense = _actor_owner->GetFortitude() + static_cast<int32>(_actor_owner->GetFortitude() * _fortitude_modifier);

    if(_protection_modifier <= -1.0f)  // If the modifier is less than or equal to -100%, set the total defense to zero
        _total_magical_defense = 0;
    else
        _total_magical_defense = _actor_owner->GetProtection() + static_cast<int32>(_actor_owner->GetProtection() * _protection_modifier);

    // If present, add defense ratings from the armor equipped
    if(equipped_armor != NULL) {
        _total_physical_defense += equipped_armor->GetPhysicalDefense();
        _total_magical_defense += equipped_armor->GetMagicalDefense();
    }
}



void GlobalAttackPoint::CalculateTotalEvade()
{
    if(_actor_owner == NULL) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "attack point has no owning actor" << std::endl;
        return;
    }

    // Calculate evade ratings from owning actor's base evade stat and the evade modifier
    if(_evade_modifier <= -1.0f)  // If the modifier is less than or equal to -100%, set the total evade to zero
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
    _total_magical_attack(0),
    _weapon_equipped(NULL)
{}



GlobalActor::~GlobalActor()
{
    // Delete all attack points
    for(uint32 i = 0; i < _attack_points.size(); i++) {
        delete _attack_points[i];
    }
    _attack_points.clear();

    // Delete all equipment
    if(_weapon_equipped != NULL)
        delete _weapon_equipped;
    for(uint32 i = 0; i < _armor_equipped.size(); i++) {
        if(_armor_equipped[i] != NULL)
            delete _armor_equipped[i];
    }
    _armor_equipped.clear();

    // Delete all skills
    for(std::map<uint32, GlobalSkill *>::iterator i = _skills.begin(); i != _skills.end(); i++) {
        delete i->second;
    }
    _skills.clear();
}



GlobalActor::GlobalActor(const GlobalActor &copy)
{
    _id = copy._id;
    _name = copy._name;
    _map_sprite_name = copy._map_sprite_name;
    _portrait = copy._portrait;
    _full_portrait = copy._full_portrait;
    _stamina_icon = copy._stamina_icon;
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
    _total_magical_attack = copy._total_magical_attack;

    // Copy all attack points
    for(uint32 i = 0; i < copy._attack_points.size(); i++) {
        _attack_points.push_back(new GlobalAttackPoint(*copy._attack_points[i]));
        _attack_points[i]->SetActorOwner(this);
    }

    // Copy all equipment
    if(copy._weapon_equipped == NULL)
        _weapon_equipped = NULL;
    else
        _weapon_equipped = new GlobalWeapon(*copy._weapon_equipped);

    for(uint32 i = 0; i < _armor_equipped.size(); i++) {
        if(_armor_equipped[i] == NULL)
            _armor_equipped.push_back(NULL);
        else
            _armor_equipped.push_back(new GlobalArmor(*copy._armor_equipped[i]));
    }

    // Copy all skills
    for(std::map<uint32, GlobalSkill *>::const_iterator i = copy._skills.begin(); i != copy._skills.end(); i++) {
        _skills.insert(std::make_pair(i->first, new GlobalSkill(*(i->second))));
    }
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
    _total_magical_attack = copy._total_magical_attack;

    // Copy all attack points
    for(uint32 i = 0; i < copy._attack_points.size(); i++) {
        _attack_points.push_back(new GlobalAttackPoint(*_attack_points[i]));
        _attack_points[i]->SetActorOwner(this);
    }

    // Copy all equipment
    if(copy._weapon_equipped == NULL)
        _weapon_equipped = NULL;
    else
        _weapon_equipped = new GlobalWeapon(*copy._weapon_equipped);

    for(uint32 i = 0; i < _armor_equipped.size(); i++) {
        if(_armor_equipped[i] == NULL)
            _armor_equipped.push_back(NULL);
        else
            _armor_equipped.push_back(new GlobalArmor(*copy._armor_equipped[i]));
    }

    // Copy all skills
    for(std::map<uint32, GlobalSkill *>::const_iterator i = copy._skills.begin(); i != copy._skills.end(); i++) {
        _skills.insert(std::make_pair(i->first, new GlobalSkill(*(i->second))));
    }
    return *this;
}



GlobalWeapon *GlobalActor::EquipWeapon(GlobalWeapon *weapon)
{
    GlobalWeapon *old_weapon = _weapon_equipped;
    _weapon_equipped = weapon;
    _CalculateAttackRatings();
    return old_weapon;
}



GlobalArmor *GlobalActor::EquipArmor(GlobalArmor *armor, uint32 index)
{
    if(index >= _armor_equipped.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of pieces of armor equipped: " << index << std::endl;
        return armor;
    }

    GlobalArmor *old_armor = _armor_equipped[index];
    _armor_equipped[index] = armor;

    if(old_armor != NULL && armor != NULL) {
        if(old_armor->GetObjectType() != armor->GetObjectType()) {
            IF_PRINT_WARNING(GLOBAL_DEBUG) << "old armor was replaced with a different type of armor" << std::endl;
        }
    }

    _attack_points[index]->CalculateTotalDefense(_armor_equipped[index]);
    return old_armor;
}



uint32 GlobalActor::GetTotalPhysicalDefense(uint32 index) const
{
    if(index >= _attack_points.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of attack points: " << index << std::endl;
        return 0;
    }

    return _attack_points[index]->GetTotalPhysicalDefense();
}



uint32 GlobalActor::GetTotalMagicalDefense(uint32 index) const
{
    if(index >= _attack_points.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of attack points: " << index << std::endl;
        return 0;
    }

    return _attack_points[index]->GetTotalMagicalDefense();
}



float GlobalActor::GetTotalEvadeRating(uint32 index) const
{
    if(index >= _attack_points.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of attack points: " << index << std::endl;
        return 0.0f;
    }

    return _attack_points[index]->GetTotalEvadeRating();
}



GlobalArmor *GlobalActor::GetArmorEquipped(uint32 index) const
{
    if(index >= _armor_equipped.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of pieces of armor equipped: " << index << std::endl;
        return NULL;
    }

    return _armor_equipped[index];
}

bool GlobalActor::HasEquipment() const
{
    if (_weapon_equipped)
        return true;

    for (uint32 i = 0; i < _armor_equipped.size(); ++i) {
        if (_armor_equipped.at(i) != NULL)
            return true;
    }
    return false;
}

GlobalAttackPoint *GlobalActor::GetAttackPoint(uint32 index) const
{
    if(index >= _attack_points.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of attack points: " << index << std::endl;
        return NULL;
    }

    return _attack_points[index];
}



GlobalSkill *GlobalActor::GetSkill(uint32 skill_id) const
{
    std::map<uint32, GlobalSkill *>::const_iterator skill_location = _skills.find(skill_id);
    if(skill_location == _skills.end()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "actor did not have a skill with the requested skill_id: " << skill_id << std::endl;
        return NULL;
    }

    return skill_location->second;
}



GlobalSkill *GlobalActor::GetSkill(const GlobalSkill *skill) const
{
    if(skill == NULL) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a NULL pointer argument" << std::endl;
        return NULL;
    }

    return GetSkill(skill->GetID());
}



void GlobalActor::AddHitPoints(uint32 amount)
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



void GlobalActor::SubtractHitPoints(uint32 amount)
{
    if(amount >= _hit_points)
        _hit_points = 0;
    else
        _hit_points -= amount;
}



void GlobalActor::AddMaxHitPoints(uint32 amount)
{
    if((0xFFFFFFFF - amount) < _max_hit_points) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << std::endl;
        _max_hit_points = 0xFFFFFFFF;
    } else {
        _max_hit_points += amount;
    }
}



void GlobalActor::SubtractMaxHitPoints(uint32 amount)
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



void GlobalActor::AddSkillPoints(uint32 amount)
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



void GlobalActor::SubtractSkillPoints(uint32 amount)
{
    if(amount >= _skill_points)
        _skill_points = 0;
    else
        _skill_points -= amount;
}



void GlobalActor::AddMaxSkillPoints(uint32 amount)
{
    if((0xFFFFFFFF - amount) < _max_skill_points) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << std::endl;
        _max_skill_points = 0xFFFFFFFF;
    } else {
        _max_skill_points += amount;
    }
}



void GlobalActor::SubtractMaxSkillPoints(uint32 amount)
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



void GlobalActor::AddStrength(uint32 amount)
{
    if((0xFFFFFFFF - amount) < _strength) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << std::endl;
        _strength = 0xFFFFFFFF;
    } else {
        _strength += amount;
    }

    _CalculateAttackRatings();
}



void GlobalActor::SubtractStrength(uint32 amount)
{
    if(amount >= _strength)
        _strength = 0;
    else
        _strength -= amount;

    _CalculateAttackRatings();
}



void GlobalActor::AddVigor(uint32 amount)
{
    if((0xFFFFFFFF - amount) < _vigor) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << std::endl;
        _vigor = 0xFFFFFFFF;
    } else {
        _vigor += amount;
    }

    _CalculateAttackRatings();
}



void GlobalActor::SubtractVigor(uint32 amount)
{
    if(amount >= _vigor)
        _vigor = 0;
    else
        _vigor -= amount;

    _CalculateAttackRatings();
}



void GlobalActor::AddFortitude(uint32 amount)
{
    if((0xFFFFFFFF - amount) < _fortitude) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << std::endl;
        _fortitude = 0xFFFFFFFF;
    } else {
        _fortitude += amount;
    }

    _CalculateDefenseRatings();
}



void GlobalActor::SubtractFortitude(uint32 amount)
{
    if(amount >= _fortitude)
        _fortitude = 0;
    else
        _fortitude -= amount;

    _CalculateDefenseRatings();
}



void GlobalActor::AddProtection(uint32 amount)
{
    if((0xFFFFFFFF - amount) < _protection) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << std::endl;
        _protection = 0xFFFFFFFF;
    } else {
        _protection += amount;
    }

    _CalculateDefenseRatings();
}



void GlobalActor::SubtractProtection(uint32 amount)
{
    if(amount >= _protection)
        _protection = 0;
    else
        _protection -= amount;

    _CalculateDefenseRatings();
}



void GlobalActor::AddAgility(uint32 amount)
{
    if((0xFFFFFFFF - amount) < _agility) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "integer overflow condition detected: " << amount << std::endl;
        _agility = 0xFFFFFFFF;
    } else {
        _agility += amount;
    }
}



void GlobalActor::SubtractAgility(uint32 amount)
{
    if(amount >= _agility)
        _agility = 0;
    else
        _agility -= amount;
}



void GlobalActor::AddEvade(float amount)
{
    if(amount < 0.0f) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received negative argument value: " << amount << std::endl;
        return;
    }

    float new_evade = _evade + amount;
    if(new_evade < _evade) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "floating point overflow condition detected: " << amount << std::endl;
        _evade = 1.0f;
    } else if(new_evade > 1.0f) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "evade rating increased above 1.0f: " << amount << std::endl;
        _evade = 1.0f;
    } else {
        _evade = new_evade;
    }

    _CalculateEvadeRatings();
}



void GlobalActor::SubtractEvade(float amount)
{
    if(amount > 0.0f) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received positive argument value: " << amount << std::endl;
        return;
    }

    float new_evade = _evade + amount;
    if(new_evade > _evade) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "floating point overflow condition detected: " << amount << std::endl;
        _evade = 0.0f;
    } else if(new_evade < 0.0f) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "evade rating decreased below 0.0f: " << amount << std::endl;
        _evade = 0.0f;
    } else {
        _evade = new_evade;
    }

    _CalculateEvadeRatings();
}



void GlobalActor::_CalculateAttackRatings()
{
    _total_physical_attack = _strength;
    _total_magical_attack = _vigor;

    if(_weapon_equipped != NULL) {
        _total_physical_attack += _weapon_equipped->GetPhysicalAttack();
        _total_magical_attack += _weapon_equipped->GetMagicalAttack();
    }
}



void GlobalActor::_CalculateDefenseRatings()
{
    // Re-calculate the defense ratings for all attack points
    for(uint32 i = 0; i < _attack_points.size(); i++) {
        if((i < _armor_equipped.size()) && (_armor_equipped[i] != NULL))
            _attack_points[i]->CalculateTotalDefense(_armor_equipped[i]);
        else
            _attack_points[i]->CalculateTotalDefense(NULL);
    }
}



void GlobalActor::_CalculateEvadeRatings()
{
    // Re-calculate the evade ratings for all attack points
    for(uint32 i = 0; i < _attack_points.size(); i++) {
        _attack_points[i]->CalculateTotalEvade();
    }
}

////////////////////////////////////////////////////////////////////////////////
// GlobalCharacter class
////////////////////////////////////////////////////////////////////////////////

GlobalCharacter::GlobalCharacter(uint32 id, bool initial) :
    _enabled(true),
    _experience_for_next_level(0),
    _hit_points_growth(0),
    _skill_points_growth(0),
    _strength_growth(0),
    _vigor_growth(0),
    _fortitude_growth(0),
    _protection_growth(0),
    _agility_growth(0),
    _evade_growth(0.0f)
{
    _id = id;

    // Open the characters script file
    std::string filename = "dat/actors/characters.lua";
    ReadScriptDescriptor char_script;
    if(!char_script.OpenFile(filename)) {
        PRINT_ERROR << "failed to open character data file: "
                    << filename << std::endl;
        return;
    }

    // Retrieve their basic character property data
    char_script.OpenTable("characters");
    char_script.OpenTable(_id);
    _name = MakeUnicodeString(char_script.ReadString("name"));

    // Load all the graphic data
    std::string portrait_filename = char_script.ReadString("portrait");
    if(DoesFileExist(portrait_filename)) {
        _portrait.Load(portrait_filename);
    }
    else if(!portrait_filename.empty()) {
        PRINT_WARNING << "Unavailable portrait image: " << portrait_filename
                      << " for character: " << MakeStandardString(_name) << std::endl;
    }

    std::string full_portrait_filename = char_script.ReadString("full_portrait");
    if(DoesFileExist(full_portrait_filename)) {
        _full_portrait.Load(full_portrait_filename);
    }
    else if(!full_portrait_filename.empty()) {
        PRINT_WARNING << "Unavailable full portrait image: " << full_portrait_filename
                      << " for character: " << MakeStandardString(_name) << std::endl;
    }

    std::string stamina_icon_filename = char_script.ReadString("stamina_icon");
    bool stamina_icon_loaded = false;
    if(DoesFileExist(stamina_icon_filename)) {
        if(_stamina_icon.Load(stamina_icon_filename, 45.0f, 45.0f))
            stamina_icon_loaded = true;
    } else {
        // Don't complain if no icon was provided on purpose
        if(!stamina_icon_filename.empty()) {
            PRINT_WARNING << "Unavailable stamina icon image: " << stamina_icon_filename
                          << " for character: " << MakeStandardString(_name) << ". Loading default one." << std::endl;
        }
    }

    // Load default in case of failure
    if(!stamina_icon_loaded)
        _stamina_icon.Load("img/icons/actors/default_stamina_icon.png", 45.0f, 45.0f);

    // Load the character's battle portraits from a multi image
    _battle_portraits.assign(5, StillImage());
    for(uint32 i = 0; i < _battle_portraits.size(); i++) {
        _battle_portraits[i].SetDimensions(100.0f, 100.0f);
    }
    std::string battle_portraits_filename = char_script.ReadString("battle_portraits");
    if(battle_portraits_filename.empty() ||
            !ImageDescriptor::LoadMultiImageFromElementGrid(_battle_portraits,
                    battle_portraits_filename, 1, 5)) {
        // Load empty portraits when they don't exist.
        for(uint32 i = 0; i < _battle_portraits.size(); ++i) {
            _battle_portraits[i].Clear();
            _battle_portraits[i].Load("", 1.0f, 1.0f);
        }
    }

    // Set up the map sprite name (untranslated) used as a string id to later link it with a map sprite.
    _map_sprite_name = char_script.ReadString("map_sprite_name");

    // Load the special skills category name and icon
    _special_category_name = MakeUnicodeString(char_script.ReadString("special_skill_category_name"));
    _special_category_icon = char_script.ReadString("special_skill_category_icon");

    // Load the bare hand skills available
    if (char_script.DoesTableExist("bare_hands_skills")) {
        std::vector<uint32> bare_skills;
        char_script.ReadUIntVector("bare_hands_skills", bare_skills);
        for (uint32 i = 0; i < bare_skills.size(); ++i)
            _AddBareHandsSkill(bare_skills[i]);
    }

    // Read each battle_animations table keys and store the corresponding animation in memory.
    std::vector<std::string> keys_vect;
    char_script.ReadTableKeys("battle_animations", keys_vect);
    char_script.OpenTable("battle_animations");
    for(uint32 i = 0; i < keys_vect.size(); ++i) {
        AnimatedImage animation;
        animation.LoadFromAnimationScript(char_script.ReadString(keys_vect[i]));
        _battle_animation[keys_vect[i]] = animation;
    }
    char_script.CloseTable();

    // Construct the character from the initial stats if necessary
    if(initial) {
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
        if(equipment_id != 0)
            _weapon_equipped = new GlobalWeapon(equipment_id);
        else
            _weapon_equipped = NULL;

        equipment_id = char_script.ReadUInt("head_armor");
        if(equipment_id != 0)
            _armor_equipped.push_back(new GlobalArmor(equipment_id));
        else
            _armor_equipped.push_back(NULL);

        equipment_id = char_script.ReadUInt("torso_armor");
        if(equipment_id != 0)
            _armor_equipped.push_back(new GlobalArmor(equipment_id));
        else
            _armor_equipped.push_back(NULL);

        equipment_id = char_script.ReadUInt("arm_armor");
        if(equipment_id != 0)
            _armor_equipped.push_back(new GlobalArmor(equipment_id));
        else
            _armor_equipped.push_back(NULL);

        equipment_id = char_script.ReadUInt("leg_armor");
        if(equipment_id != 0)
            _armor_equipped.push_back(new GlobalArmor(equipment_id));
        else
            _armor_equipped.push_back(NULL);

        char_script.CloseTable();
        if(char_script.IsErrorDetected()) {
            if(GLOBAL_DEBUG) {
                PRINT_WARNING << "one or more errors occurred while reading initial data - they are listed below"
                              << std::endl
                              << char_script.GetErrorMessages() << std::endl;
            }
        }
    } // if (initial == true)
    else {
        // Make sure the _armor_equipped vector is sized appropriately. Armor should be equipped on the character
        // externally to this constructor.
        _armor_equipped.resize(4, NULL);
    }

    // Setup the character's attack points
    char_script.OpenTable("attack_points");
    for(uint32 i = GLOBAL_POSITION_HEAD; i <= GLOBAL_POSITION_LEGS; i++) {
        _attack_points.push_back(new GlobalAttackPoint(this));
        char_script.OpenTable(i);
        if(_attack_points[i]->LoadData(char_script) == false) {
            IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to succesfully load data for attack point: " << i << std::endl;
        }
        char_script.CloseTable();
    }
    char_script.CloseTable();

    if(char_script.IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading attack point data - they are listed below"
                          << std::endl << char_script.GetErrorMessages() << std::endl;
        }
    }

    // Construct the character's initial skill set if necessary
    if(initial) {
        // The skills table contains key/value pairs. The key indicate the level required to learn the skill and the value is the skill's id
        std::vector<uint32> skill_levels;
        char_script.OpenTable("skills");
        char_script.ReadTableKeys(skill_levels);

        // We want to add the skills beginning with the first learned to the last. ReadTableKeys does not guarantee returing the keys in a sorted order,
        // so sort the skills by level before checking each one.
        std::sort(skill_levels.begin(), skill_levels.end());

        // Only add the skills for which the experience level requirements are met
        for(uint32 i = 0; i < skill_levels.size(); i++) {
            if(skill_levels[i] <= _experience_level) {
                AddSkill(char_script.ReadUInt(skill_levels[i]));
            }
            // Because skill_levels is sorted, all remaining skills will not have their level requirements met
            else {
                break;
            }
        }

        char_script.CloseTable(); // skills
        if(char_script.IsErrorDetected()) {
            if(GLOBAL_DEBUG) {
                PRINT_WARNING << "one or more errors occurred while reading skill data - they are listed below"
                              << std::endl << char_script.GetErrorMessages() << std::endl;
            }
        }

        // If initial, determine the character's XP for next level.
        std::vector<int32> xp_per_levels;
        char_script.OpenTable("growth");
        char_script.ReadIntVector("experience_for_next_level", xp_per_levels);
        if (_experience_level <= xp_per_levels.size()) {
            _experience_for_next_level = xp_per_levels[_experience_level - 1];
        }
        else {
            PRINT_ERROR << "No Xp for next level found for character id " << _id
                << " at level " << _experience_level << std::endl;
            // Bad default
            _experience_for_next_level = 100000;
        }
        char_script.CloseTable(); // growth
    } // if (initial)

    char_script.CloseTable(); // "characters[id]"
    char_script.CloseTable(); // "characters"

    // Close the script file and calculate all rating totals
    if(char_script.IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading final data - they are listed below"
                          << std::endl << char_script.GetErrorMessages() << std::endl;
        }
    }
    char_script.CloseFile();

    _CalculateAttackRatings();
    _CalculateDefenseRatings();
    _CalculateEvadeRatings();
} // GlobalCharacter::GlobalCharacter(uint32 id, bool initial)



bool GlobalCharacter::AddExperiencePoints(uint32 xp)
{
    _experience_points += xp;
    _experience_for_next_level -= xp;
    return ReachedNewExperienceLevel();
}



void GlobalCharacter::AddSkill(uint32 skill_id)
{
    if(skill_id == 0) {
        PRINT_WARNING << "function received an invalid skill_id argument: " << skill_id << std::endl;
        return;
    }
    if(_skills.find(skill_id) != _skills.end()) {
        PRINT_WARNING << "failed to add skill because the character already knew this skill: "
            << skill_id << std::endl;
        return;
    }

    GlobalSkill *skill = new GlobalSkill(skill_id);
    if(!skill->IsValid()) {
        PRINT_WARNING << "the skill to add failed to load: " << skill_id << std::endl;
        delete skill;
        return;
    }

    // Insert the pointer to the new skill inside of the global skills map and the skill type vector
    _skills.insert(std::make_pair(skill_id, skill));
    switch(skill->GetType()) {
    case GLOBAL_SKILL_WEAPON:
        _weapon_skills.push_back(skill);
        break;
    case GLOBAL_SKILL_MAGIC:
        _magic_skills.push_back(skill);
        break;
    case GLOBAL_SKILL_SPECIAL:
        _special_skills.push_back(skill);
        break;
    default:
        PRINT_WARNING << "loaded a new skill with an unknown skill type: " << skill->GetType() << std::endl;
        break;
    }
}

void GlobalCharacter::AddNewSkillLearned(uint32 skill_id)
{
    if(skill_id == 0) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received an invalid skill_id argument: " << skill_id << std::endl;
        return;
    }

    // Make sure we don't add a skill more than once
    for(std::vector<GlobalSkill*>::iterator it = _new_skills_learned.begin(); it != _new_skills_learned.end(); ++it) {
        if(skill_id == (*it)->GetID()) {
            IF_PRINT_WARNING(GLOBAL_DEBUG) << "the skill to add was already present in the list of newly learned skills: "
                                           << skill_id << std::endl;
            return;
        }
    }

    AddSkill(skill_id);

    std::map<uint32, GlobalSkill *>::iterator skill = _skills.find(skill_id);
    if(skill == _skills.end()) {
        PRINT_WARNING << "failed because the new skill was not added successfully: " << skill_id << std::endl;
        return;
    }

    _new_skills_learned.push_back(skill->second);
}

void GlobalCharacter::_AddBareHandsSkill(uint32 skill_id)
{
    if(skill_id == 0) {
        PRINT_WARNING << "function received an invalid skill_id argument: " << skill_id << std::endl;
        return;
    }
    if(_skills.find(skill_id) != _skills.end()) {
        PRINT_WARNING << "failed to add skill because the character already knew this skill: "
            << skill_id << std::endl;
        return;
    }

    GlobalSkill *skill = new GlobalSkill(skill_id);
    if(!skill->IsValid()) {
        PRINT_WARNING << "the skill to add failed to load: " << skill_id << std::endl;
        delete skill;
        return;
    }

    // Insert the pointer to the new skill inside of the global skills map and the skill type vector
    _skills.insert(std::make_pair(skill_id, skill));
    _bare_hands_skills.push_back(skill);
}

hoa_video::AnimatedImage *GlobalCharacter::RetrieveBattleAnimation(const std::string &name)
{
    if(_battle_animation.find(name) == _battle_animation.end())
        return &_battle_animation["idle"];

    return &_battle_animation.at(name);
}

void GlobalCharacter::AcknowledgeGrowth() {
    if (!ReachedNewExperienceLevel())
        return;

    // A new experience level has been gained. Retrieve the growth data for the new experience level
    ++_experience_level;

    // Retrieve the growth data for the new experience level and check for any additional growth
    std::string filename = "dat/actors/characters.lua";
    ReadScriptDescriptor character_script;
    if(!character_script.OpenFile(filename)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to open character data file: " << filename << std::endl;
        return;
    }

    // Clear the growth members before filling their data
    _hit_points_growth = 0;
    _skill_points_growth = 0;
    _strength_growth = 0;
    _vigor_growth = 0;
    _fortitude_growth = 0;
    _protection_growth = 0;
    _agility_growth = 0;
    _evade_growth = 0.0f;

    try {
        // Update Growth data and set XP for next level
        ScriptCallFunction<void>(character_script.GetLuaState(), "DetermineLevelGrowth", this);
    } catch(const luabind::error& e) {
        ScriptManager->HandleLuaError(e);
    } catch(const luabind::cast_failed& e) {
        ScriptManager->HandleCastError(e);
    }

    // Reset the skills learned container and add any skills learned at this level
    _new_skills_learned.clear();
    try {
        ScriptCallFunction<void>(character_script.GetLuaState(), "DetermineNewSkillsLearned", this);
    } catch(const luabind::error& e) {
        ScriptManager->HandleLuaError(e);
    } catch(const luabind::cast_failed& e) {
        ScriptManager->HandleCastError(e);
    }

    // Add all growth stats to the character actor
    if(_hit_points_growth != 0) {
        AddMaxHitPoints(_hit_points_growth);
        if (_hit_points > 0)
            AddHitPoints(_hit_points_growth);
    }

    if(_skill_points_growth != 0) {
        AddMaxSkillPoints(_skill_points_growth);
        if (_skill_points > 0)
            AddSkillPoints(_skill_points_growth);
    }

    if(_strength_growth != 0)
        AddStrength(_strength_growth);
    if(_vigor_growth != 0)
        AddVigor(_vigor_growth);
    if(_fortitude_growth != 0)
        AddFortitude(_fortitude_growth);
    if(_protection_growth != 0)
        AddProtection(_protection_growth);
    if(_agility_growth != 0)
        AddAgility(_agility_growth);
    if(!IsFloatEqual(_evade_growth, 0.0f))
        AddEvade(_evade_growth);

    character_script.CloseFile();
    return;
} // bool GlobalCharacter::AcknowledgeGrowth()

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

    // Use the id member to determine the name of the data file that the enemy is defined in
    std::string file_ext;
    std::string filename;

    if(_id == 0)
        PRINT_ERROR << "invalid id for loading enemy data: " << _id << std::endl;

    // Open the script file and table that store the enemy data
    ReadScriptDescriptor enemy_data;
    if(!enemy_data.OpenFile("dat/actors/enemies.lua")) {
        PRINT_ERROR << "failed to open enemy data file: " << filename << std::endl;
        return;
    }

    enemy_data.OpenTable("enemies");
    enemy_data.OpenTable(_id);

    // Load the enemy's name and sprite data
    _name = MakeUnicodeString(enemy_data.ReadString("name"));
    _sprite_width = enemy_data.ReadInt("sprite_width");
    _sprite_height = enemy_data.ReadInt("sprite_height");

    // Attempt to load the MultiImage for the sprite's frames, which should contain one row and four columns of images
    _battle_sprite_frames.assign(4, StillImage());
    std::string sprite_filename = enemy_data.ReadString("battle_sprites");
    if(!ImageDescriptor::LoadMultiImageFromElementGrid(_battle_sprite_frames, sprite_filename, 1, 4))
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to load sprite frames for enemy: " << sprite_filename << std::endl;

    std::string stamina_icon_filename = enemy_data.ReadString("stamina_icon");
    if(!stamina_icon_filename.empty()) {
        if(!_stamina_icon.Load(stamina_icon_filename, 45.0f, 45.0f)) {
            PRINT_WARNING << "Invalid stamina icon image: " << stamina_icon_filename
                          << " for enemy: " << MakeStandardString(_name) << ". Loading default one." << std::endl;

            _stamina_icon.Load("img/icons/actors/default_stamina_icon.png", 45.0f, 45.0f);
        }
    } else {
        _stamina_icon.Load("img/icons/actors/default_stamina_icon.png", 45.0f, 45.0f);
    }

    // Load the enemy's base stats
    if(enemy_data.DoesBoolExist("no_stat_randomization") == true) {
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

    // Create the attack points for the enemy
    enemy_data.OpenTable("attack_points");
    uint32 ap_size = enemy_data.GetTableSize();
    for(uint32 i = 1; i <= ap_size; i++) {
        _attack_points.push_back(new GlobalAttackPoint(this));
        enemy_data.OpenTable(i);
        if(_attack_points.back()->LoadData(enemy_data) == false) {
            IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to load data for an attack point: " << i << std::endl;
        }
        enemy_data.CloseTable();
    }
    enemy_data.CloseTable();

    // Add the set of skills for the enemy
    enemy_data.OpenTable("skills");
    for(uint32 i = 1; i <= enemy_data.GetTableSize(); i++) {
        _skill_set.push_back(enemy_data.ReadUInt(i));
    }
    enemy_data.CloseTable();

    // Load the possible items that the enemy may drop
    enemy_data.OpenTable("drop_objects");
    for(uint32 i = 1; i <= enemy_data.GetTableSize(); i++) {
        enemy_data.OpenTable(i);
        _dropped_objects.push_back(enemy_data.ReadUInt(1));
        _dropped_chance.push_back(enemy_data.ReadFloat(2));
        enemy_data.CloseTable();
    }
    enemy_data.CloseTable();

    enemy_data.CloseTable(); // enemies[_id]
    enemy_data.CloseTable(); // enemies

    if(enemy_data.IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading the enemy data - they are listed below"
                          << std::endl << enemy_data.GetErrorMessages() << std::endl;
        }
    }

    enemy_data.CloseFile();

    _CalculateAttackRatings();
    _CalculateDefenseRatings();
    _CalculateEvadeRatings();
} // GlobalEnemy::GlobalEnemy(uint32 id)



void GlobalEnemy::AddSkill(uint32 skill_id)
{
    if(skill_id == 0) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received an invalid skill_id argument: " << skill_id << std::endl;
        return;
    }
    if(_skills.find(skill_id) != _skills.end()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to add skill because the enemy already knew this skill: " << skill_id << std::endl;
        return;
    }

    GlobalSkill *skill = new GlobalSkill(skill_id);
    if(skill->IsValid() == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "the skill to add failed to load: " << skill_id << std::endl;
        delete skill;
        return;
    }

    // Insert the pointer to the new skill inside of the global skills map and the skill type vector
    _skills.insert(std::make_pair(skill_id, skill));
}



void GlobalEnemy::Initialize()
{
    if(_skills.empty() == false) { // Indicates that the enemy has already been initialized
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function was invoked for an already initialized enemy: " << _id << std::endl;
        return;
    }

    // TODO: we may wish to actually define XP levels for enemies in their data table, though I don't know what purpose it may serve
    _experience_level = 1;

    // ----- (1): Add all new skills that should be available at the current experience level
    for(uint32 i = 0; i < _skill_set.size(); i++) {
        AddSkill(_skill_set[i]);
    }

    if(_skills.empty()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "no skills were added for the enemy: " << _id << std::endl;
    }

    // ----- (3): Randomize the stats by using a guassian random variable
    if(_no_stat_randomization == false) {
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



void GlobalEnemy::DetermineDroppedObjects(std::vector<GlobalObject *>& objects)
{
    objects.clear();

    for(uint32 i = 0; i < _dropped_objects.size(); i++) {
        if(RandomFloat() < _dropped_chance[i]) {
            objects.push_back(GlobalCreateNewObject(_dropped_objects[i]));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// GlobalParty class
////////////////////////////////////////////////////////////////////////////////

void GlobalParty::AddActor(GlobalActor *actor, int32 index)
{
    if(actor == NULL) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a NULL actor argument" << std::endl;
        return;
    }

    if(_allow_duplicates == false) {
        // Check that this actor is not already in the party
        for(uint32 i = 0; i < _actors.size(); i++) {
            if(actor->GetID() == _actors[i]->GetID()) {
                IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to add an actor that was already in the party "
                                               << "when duplicates were not allowed: " << actor->GetID() << std::endl;
                return;
            }
        }
    }

    // Add actor to the end of the party if index is negative
    if(index < 0) {
        _actors.push_back(actor);
        return;
    }

    // Check that the requested index does not exceed the size of the container
    if(static_cast<uint32>(index) >= _actors.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded the current party size: " << index << std::endl;
        _actors.push_back(actor); // Add the actor to the end of the party instead
        return;
    } else {
        std::vector<GlobalActor *>::iterator position = _actors.begin();
        for(int32 i = 0; i < index; i++, position++);
        _actors.insert(position, actor);
    }
}



GlobalActor *GlobalParty::RemoveActorAtIndex(uint32 index)
{
    if(index >= _actors.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded current party size: "
                                       << index << std::endl;
        return NULL;
    }

    GlobalActor *removed_actor = _actors[index];
    std::vector<GlobalActor *>::iterator position = _actors.begin();
    for(uint32 i = 0; i < index; i++, position++);
    _actors.erase(position);

    return removed_actor;
}



GlobalActor *GlobalParty::RemoveActorByID(uint32 id)
{
    if(_allow_duplicates) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "tried to remove actor when duplicates were allowed in the party: " << id << std::endl;
        return NULL;
    }

    GlobalActor *removed_actor = NULL;
    for(std::vector<GlobalActor *>::iterator position = _actors.begin(); position != _actors.end(); position++) {
        if(id == (*position)->GetID()) {
            removed_actor = *position;
            _actors.erase(position);
            break;
        }
    }

    if(removed_actor == NULL) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an actor in the party with the requested id: " << id << std::endl;
    }

    return removed_actor;
}



GlobalActor *GlobalParty::GetActorAtIndex(uint32 index) const
{
    if(index >= _actors.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded current party size: " << index << std::endl;
        return NULL;
    }

    return _actors[index];
}



GlobalActor *GlobalParty::GetActorByID(uint32 id) const
{
    if(_allow_duplicates) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "tried to retrieve actor when duplicates were allowed in the party: " << id << std::endl;
        return NULL;
    }

    for(uint32 i = 0; i < _actors.size(); i++) {
        if(_actors[i]->GetID() == id) {
            return _actors[i];
        }
    }

    IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an actor in the party with the requested id: " << id << std::endl;
    return NULL;
}



void GlobalParty::SwapActorsByIndex(uint32 first_index, uint32 second_index)
{
    if(first_index == second_index) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "first_index and second_index arguments had the same value: " << first_index << std::endl;
        return;
    }
    if(first_index >= _actors.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "first_index argument exceeded current party size: " << first_index << std::endl;
        return;
    }
    if(second_index >= _actors.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "second_index argument exceeded current party size: " << second_index << std::endl;
        return;
    }

    GlobalActor *tmp = _actors[first_index];
    _actors[first_index] = _actors[second_index];
    _actors[second_index] = tmp;
}



void GlobalParty::SwapActorsByID(uint32 first_id, uint32 second_id)
{
    if(first_id == second_id) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "first_id and second_id arguments had the same value: " << first_id << std::endl;
        return;
    }
    if(_allow_duplicates) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "tried to swap actors when duplicates were allowed in the party: " << first_id << std::endl;
        return;
    }

    std::vector<GlobalActor *>::iterator first_position;
    std::vector<GlobalActor *>::iterator second_position;
    for(first_position = _actors.begin(); first_position != _actors.end(); first_position++) {
        if((*first_position)->GetID() == first_id)
            break;
    }
    for(second_position = _actors.begin(); second_position != _actors.end(); second_position++) {
        if((*second_position)->GetID() == second_id)
            break;
    }

    if(first_position == _actors.end()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an actor in the party with the requested first_id: " << first_id << std::endl;
        return;
    }
    if(second_position == _actors.end()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an actor in the party with the requested second_id: " << second_id << std::endl;
        return;
    }

    GlobalActor *tmp = *first_position;
    *first_position = *second_position;
    *second_position = tmp;
}



GlobalActor *GlobalParty::ReplaceActorByIndex(uint32 index, GlobalActor *new_actor)
{
    if(new_actor == NULL) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a NULL new_actor argument" << std::endl;
        return NULL;
    }
    if(index >= _actors.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded current party size: " << index << std::endl;
        return NULL;
    }

    GlobalActor *tmp = _actors[index];
    _actors[index] = new_actor;
    return tmp;
}



GlobalActor *GlobalParty::ReplaceActorByID(uint32 id, GlobalActor *new_actor)
{
    if(_allow_duplicates) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "tried to replace actor when duplicates were allowed in the party: " << id << std::endl;
        return NULL;
    }
    if(new_actor == NULL) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a NULL new_actor argument" << std::endl;
        return NULL;
    }

    GlobalActor *removed_actor = NULL;
    for(std::vector<GlobalActor *>::iterator position = _actors.begin(); position != _actors.end(); position++) {
        if((*position)->GetID() == id) {
            removed_actor = *position;
            *position = new_actor;
            break;
        }
    }

    if(removed_actor == NULL) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an actor in the party with the requested id: " << id << std::endl;
    }

    return removed_actor;
}



float GlobalParty::AverageExperienceLevel() const
{
    if(_actors.empty())
        return 0.0f;

    float xp_level_sum = 0.0f;
    for(uint32 i = 0; i < _actors.size(); i++)
        xp_level_sum += static_cast<float>(_actors[i]->GetExperienceLevel());
    return (xp_level_sum / static_cast<float>(_actors.size()));
}



void GlobalParty::AddHitPoints(uint32 hp)
{
    for(std::vector<GlobalActor *>::iterator i = _actors.begin(); i != _actors.end(); i++) {
        (*i)->AddHitPoints(hp);
    }
}



void GlobalParty::AddSkillPoints(uint32 sp)
{
    for(std::vector<GlobalActor *>::iterator i = _actors.begin(); i != _actors.end(); i++) {
        (*i)->AddSkillPoints(sp);
    }
}

} // namespace hoa_global
