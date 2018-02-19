////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_enemy.h"

#include "global_attack_point.h"

#include "common/global/global.h"

#include "script/script_read.h"
#include "utils/utils_random.h"

using namespace vt_utils;
using namespace vt_video;
using namespace vt_script;

namespace vt_global
{

extern bool GLOBAL_DEBUG;

GlobalEnemy::GlobalEnemy(uint32_t id) :
    GlobalActor(),
    _experience_points(0),
    _sprite_width(0),
    _sprite_height(0),
    _drunes_dropped(0)
{
    _id = id;

    if(_id == 0) {
        PRINT_ERROR << "invalid id for loading enemy data: " << _id << std::endl;
        return;
    }

    // Open the script file and table that store the enemy data
    ReadScriptDescriptor& enemy_data = GlobalManager->GetEnemiesScript();

    if (!enemy_data.OpenTable(_id)) {
        PRINT_ERROR << "Failed to open the enemies[" << _id << "] table in: "
            << enemy_data.GetFilename() << std::endl;
        return;
    }

    // Load the enemy's name and sprite data
    _name = MakeUnicodeString(enemy_data.ReadString("name"));

    // Attempt to load the animations for each harm levels
    _battle_animations.assign(GLOBAL_ENEMY_HURT_TOTAL, AnimatedImage());
    if (enemy_data.OpenTable("battle_animations" )) {

        std::vector<uint32_t> animations_id;
        enemy_data.ReadTableKeys(animations_id);
        for (uint32_t i = 0; i < animations_id.size(); ++i) {
            uint32_t anim_id = animations_id[i];
            if (anim_id >= GLOBAL_ENEMY_HURT_TOTAL) {
                PRINT_WARNING << "Invalid table id in 'battle_animations' table for enemy: "
                    << _id << std::endl;
                continue;
            }

            _battle_animations[anim_id].LoadFromAnimationScript(enemy_data.ReadString(anim_id));

            // Updates the sprite dimensions
            if (_battle_animations[anim_id].GetWidth() > _sprite_width)
                _sprite_width =_battle_animations[anim_id].GetWidth();
            if (_battle_animations[anim_id].GetHeight() > _sprite_height)
                _sprite_height =_battle_animations[anim_id].GetHeight();
        }

        enemy_data.CloseTable(); // battle_animations
    }
    else {
        PRINT_WARNING << "No 'battle_animations' table for enemy: " << _id << std::endl;
    }

    std::string stamina_icon_filename = enemy_data.ReadString("stamina_icon");
    if(!stamina_icon_filename.empty()) {
        if(!_stamina_icon.Load(stamina_icon_filename)) {
            PRINT_WARNING << "Invalid stamina icon image: " << stamina_icon_filename
                          << " for enemy: " << MakeStandardString(_name) << ". Loading default one." << std::endl;

            _stamina_icon.Load("data/battles/stamina_icons/default_stamina_icon.png");
        }
    } else {
        _stamina_icon.Load("data/battles/stamina_icons/default_stamina_icon.png");
    }

    // Loads enemy battle animation scripts
    if (enemy_data.OpenTable("scripts")) {
        _death_script_filename = enemy_data.ReadString("death");
        _ai_script_filename = enemy_data.ReadString("battle_ai");
        enemy_data.CloseTable();
    }

    if (enemy_data.OpenTable("base_stats")) {
        _max_hit_points = enemy_data.ReadUInt("hit_points");
        _hit_points = _max_hit_points;
        _max_skill_points = enemy_data.ReadUInt("skill_points");
        _skill_points = _max_skill_points;
        _experience_points = enemy_data.ReadUInt("experience_points");
        _char_phys_atk.SetBase(enemy_data.ReadUInt("phys_atk"));
        _char_mag_atk.SetBase(enemy_data.ReadUInt("mag_atk"));
        _char_phys_def.SetBase(enemy_data.ReadUInt("phys_def"));
        _char_mag_def.SetBase(enemy_data.ReadUInt("mag_def"));
        _stamina.SetBase(enemy_data.ReadUInt("stamina"));
        _evade.SetBase(enemy_data.ReadFloat("evade"));
        _drunes_dropped = enemy_data.ReadUInt("drunes");
        enemy_data.CloseTable();
    }

    // Create the attack points for the enemy
    if (enemy_data.OpenTable("attack_points")) {
        uint32_t ap_size = enemy_data.GetTableSize();
        for(uint32_t i = 1; i <= ap_size; ++i) {
            _attack_points.push_back(new GlobalAttackPoint(this));
            if (enemy_data.OpenTable(i)) {
                if(_attack_points.back()->LoadData(enemy_data) == false) {
                    IF_PRINT_WARNING(GLOBAL_DEBUG) << "Failed to load data for an attack point: "
                        << i << std::endl;
                }
                enemy_data.CloseTable();
            }
        }
        enemy_data.CloseTable();
    }

    // Add the set of skills for the enemy
    if (enemy_data.OpenTable("skills")) {
        for(uint32_t i = 1; i <= enemy_data.GetTableSize(); ++i) {
            _skill_set.push_back(enemy_data.ReadUInt(i));
        }
        enemy_data.CloseTable();
    }

    // Load the possible items that the enemy may drop
    if (enemy_data.OpenTable("drop_objects")) {
        for(uint32_t i = 1; i <= enemy_data.GetTableSize(); ++i) {
            enemy_data.OpenTable(i);
            _dropped_objects.push_back(enemy_data.ReadUInt(1));
            _dropped_chance.push_back(enemy_data.ReadFloat(2));
            enemy_data.CloseTable();
        }
        enemy_data.CloseTable();
    }

    enemy_data.CloseTable(); // enemies[_id]

    if(enemy_data.IsErrorDetected()) {
        PRINT_WARNING << "One or more errors occurred while reading the enemy data - they are listed below"
                      << std::endl << enemy_data.GetErrorMessages() << std::endl;
    }

    // stats and skills.
    _Initialize();

    _CalculateAttackRatings();
    _CalculateDefenseRatings();
    _CalculateEvadeRatings();
}

bool GlobalEnemy::AddSkill(uint32_t skill_id)
{
    if(skill_id == 0) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received an invalid skill_id argument: " << skill_id << std::endl;
        return false;
    }

    if(HasSkill(skill_id)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to add skill because the enemy already knew this skill: " << skill_id << std::endl;
        return false;
    }

    GlobalSkill *skill = new GlobalSkill(skill_id);
    if(skill->IsValid() == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "the skill to add failed to load: " << skill_id << std::endl;
        delete skill;
        return false;
    }

    // Insert the pointer to the new skill inside of the global skills vectors
    _skills.push_back(skill);
    _skills_id.push_back(skill_id);
    return true;
}

void GlobalEnemy::_Initialize()
{
    // Add all new skills that should be available at the current experience level
    _skills.clear();
    for(uint32_t i = 0; i < _skill_set.size(); ++i)
        AddSkill(_skill_set[i]);

    if(_skills.empty())
        PRINT_WARNING << "No skills were added for the enemy: " << _id << std::endl;

    // Randomize the stats by using a random diff of 10%
    _max_hit_points = RandomDiffValue(_max_hit_points, _max_hit_points / 10.0f);
    _max_skill_points = RandomDiffValue(_max_skill_points, _max_skill_points / 10.0f);
    _experience_points = RandomDiffValue(_experience_points, _experience_points / 10.0f);
    _char_phys_atk.SetBase(RandomDiffValue(_char_phys_atk.GetBase(), _char_phys_atk.GetBase() / 10.0f));
    _char_mag_atk.SetBase(RandomDiffValue(_char_mag_atk.GetBase(), _char_mag_atk.GetBase() / 10.0f));
    _char_phys_def.SetBase(RandomDiffValue(_char_phys_def.GetBase(), _char_phys_def.GetBase() / 10.0f));
    _char_mag_def.SetBase(RandomDiffValue(_char_mag_def.GetBase(), _char_mag_def.GetBase() / 10.0f));
    _stamina.SetBase(RandomDiffValue(_stamina.GetBase(), _stamina.GetBase() / 10.0f));

    // Multiply the evade value by 10 to permit the decimal to be kept
    float evade = _evade.GetBase() * 10.0f;
    _evade.SetBase(static_cast<float>(RandomDiffValue(evade, evade / 10.0f)) / 10.0f);

    _drunes_dropped = RandomDiffValue(_drunes_dropped, _drunes_dropped / 10.0f);

    // Set the current hit points and skill points to their new maximum values
    _hit_points = _max_hit_points;
    _skill_points = _max_skill_points;
}

std::vector<std::shared_ptr<GlobalObject>> GlobalEnemy::DetermineDroppedObjects()
{
    std::vector<std::shared_ptr<GlobalObject>> result;

    for (uint32_t i = 0; i < _dropped_objects.size(); ++i) {
        if (RandomFloat() < _dropped_chance[i]) {
            std::shared_ptr<GlobalObject> global_object = GlobalCreateNewObject(_dropped_objects[i]);
            result.push_back(global_object);
        }
    }

    return result;
}

} // namespace vt_global
