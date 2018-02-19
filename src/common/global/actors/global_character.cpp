////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_character.h"
#include "global_attack_point.h"

#include "common/global/global.h"

#include "script/script_read.h"
#include "utils/utils_files.h"

using namespace vt_utils;
using namespace vt_video;
using namespace vt_script;

namespace vt_global
{

extern bool GLOBAL_DEBUG;

GlobalCharacter::GlobalCharacter(uint32_t id, bool initial) :
    _experience_level(0),
    _experience_points(0),
    _enabled(true),
    _weapon_equipped(nullptr),
    _experience_for_next_level(0),
    _hit_points_growth(0),
    _skill_points_growth(0),
    _phys_atk_growth(0),
    _mag_atk_growth(0),
    _phys_def_growth(0),
    _mag_def_growth(0),
    _stamina_growth(0),
    _evade_growth(0.0f),
    _current_skill_node_id(0)
{
    _id = id;

    // Gets the characters script file
    ReadScriptDescriptor& char_script = GlobalManager->GetCharactersScript();

    // Retrieve their basic character property data
    if (!char_script.OpenTable(_id)) {
        PRINT_ERROR << "Couldn't find character " << _id << " in: " << char_script.GetFilename() << std::endl;
        return;
    }

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
        _stamina_icon.Load("data/battles/stamina_icons/default_stamina_icon.png", 45.0f, 45.0f);

    // Load the character's battle portraits from a multi image
    _battle_portraits.assign(5, StillImage());
    for(uint32_t i = 0; i < _battle_portraits.size(); ++i) {
        _battle_portraits[i].SetDimensions(100.0f, 100.0f);
    }
    std::string battle_portraits_filename = char_script.ReadString("battle_portraits");
    if(battle_portraits_filename.empty() ||
            !ImageDescriptor::LoadMultiImageFromElementGrid(_battle_portraits,
                    battle_portraits_filename, 1, 5)) {
        // Load empty portraits when they don't exist.
        for(uint32_t i = 0; i < _battle_portraits.size(); ++i) {
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
        std::vector<uint32_t> bare_skills;
        char_script.ReadUIntVector("bare_hands_skills", bare_skills);
        for (uint32_t i = 0; i < bare_skills.size(); ++i)
            AddSkill(bare_skills[i]);
    }

    // Load the starting skill node
    _current_skill_node_id = char_script.ReadUInt("starting_skill_node_id", 0);

    // Read each battle_animations table keys and store the corresponding animation in memory.
    std::vector<std::string> keys_vect;
    char_script.ReadTableKeys("battle_animations", keys_vect);
    if (char_script.OpenTable("battle_animations")) {
        for(uint32_t i = 0; i < keys_vect.size(); ++i) {
            AnimatedImage animation;
            animation.LoadFromAnimationScript(char_script.ReadString(keys_vect[i]));
            _battle_animation[keys_vect[i]] = animation;
        }
        char_script.CloseTable(); // battle_animations
    }

    // Loads enemy battle animation scripts
    if (char_script.OpenTable("scripts")) {
        _death_script_filename = char_script.ReadString("death");
        _ai_script_filename = char_script.ReadString("battle_ai");
        char_script.CloseTable();
    }

    // Construct the character from the initial stats if necessary
    if(initial && char_script.OpenTable("initial_stats")) {
        char_script.OpenTable("initial_stats");
        _experience_level = char_script.ReadUInt("experience_level");
        _total_experience_points = char_script.ReadUInt("experience_points");
        _max_hit_points = char_script.ReadUInt("max_hit_points");
        _hit_points = _max_hit_points;
        _max_skill_points = char_script.ReadUInt("max_skill_points");
        _skill_points = _max_skill_points;
        _char_phys_atk.SetBase(char_script.ReadUInt("phys_atk"));
        _char_mag_atk.SetBase(char_script.ReadUInt("mag_atk"));
        _char_phys_def.SetBase(char_script.ReadUInt("phys_def"));
        _char_mag_def.SetBase(char_script.ReadUInt("mag_def"));
        _stamina.SetBase(char_script.ReadUInt("stamina"));
        _evade.SetBase(char_script.ReadFloat("evade"));

        // Add the character's initial equipment. If any equipment ids are zero, that indicates nothing is to be equipped.
        uint32_t equipment_id = 0;
        equipment_id = char_script.ReadUInt("weapon");
        if(equipment_id != 0)
            _weapon_equipped = std::make_shared<GlobalWeapon>(equipment_id);
        else
            _weapon_equipped = nullptr;

        equipment_id = char_script.ReadUInt("head_armor");
        if(equipment_id != 0)
            _armor_equipped.push_back(std::make_shared<GlobalArmor>(equipment_id));
        else
            _armor_equipped.push_back(nullptr);

        equipment_id = char_script.ReadUInt("torso_armor");
        if(equipment_id != 0)
            _armor_equipped.push_back(std::make_shared<GlobalArmor>(equipment_id));
        else
            _armor_equipped.push_back(nullptr);

        equipment_id = char_script.ReadUInt("arm_armor");
        if(equipment_id != 0)
            _armor_equipped.push_back(std::make_shared<GlobalArmor>(equipment_id));
        else
            _armor_equipped.push_back(nullptr);

        equipment_id = char_script.ReadUInt("leg_armor");
        if(equipment_id != 0)
            _armor_equipped.push_back(std::make_shared<GlobalArmor>(equipment_id));
        else
            _armor_equipped.push_back(nullptr);

        char_script.CloseTable(); // initial_stats
        if(char_script.IsErrorDetected()) {
            if(GLOBAL_DEBUG) {
                PRINT_WARNING << "one or more errors occurred while reading initial data - they are listed below"
                              << std::endl
                              << char_script.GetErrorMessages() << std::endl;
            }
        }
    } // if (initial)
    else {
        // Make sure the _armor_equipped vector is sized appropriately. Armor should be equipped on the character
        // externally to this constructor.
        _armor_equipped.resize(4, nullptr);
    }

    // Setup the character's attack points
    if (char_script.OpenTable("attack_points")) {
        for(uint32_t i = GLOBAL_POSITION_HEAD; i <= GLOBAL_POSITION_LEGS; ++i) {
            _attack_points.push_back(new GlobalAttackPoint(this));
            if (char_script.OpenTable(i)) {
                if(_attack_points[i]->LoadData(char_script) == false) {
                    PRINT_WARNING << "Failed to successfully load data for attack point: " << i << std::endl;
                }
                char_script.CloseTable(); // attack point N°i
            }
        }
        char_script.CloseTable(); // attack_points
    }

    if(char_script.IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading attack point data - they are listed below"
                          << std::endl << char_script.GetErrorMessages() << std::endl;
        }
    }

    // Construct the character's initial skill set if necessary
    if(initial) {
        if (char_script.OpenTable("skills")) {
            // The skills table contains key/value pairs. The key indicate the level required to learn the skill and the value is the skill's id
            std::vector<uint32_t> skill_levels;
            char_script.ReadTableKeys(skill_levels);

            // We want to add the skills beginning with the first learned to the last. ReadTableKeys does not guarantee returing the keys in a sorted order,
            // so sort the skills by level before checking each one.
            std::sort(skill_levels.begin(), skill_levels.end());

            // Only add the skills for which the experience level requirements are met
            for(uint32_t i = 0; i < skill_levels.size(); ++i) {
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
        }

        if (char_script.OpenTable("growth")) {
            // If initial, determine the character's XP for next level.
            std::vector<int32_t> xp_per_levels;
            char_script.ReadIntVector("experience_for_next_level", xp_per_levels);
            if (_experience_level <= xp_per_levels.size()) {
                _experience_for_next_level = xp_per_levels[_experience_level - 1];
            }
            else {
                PRINT_ERROR << "No XP for next level found for character id: " << _id
                    << " at level " << _experience_level << std::endl;
                // Bad default
                _experience_for_next_level = 100000;
            }
            char_script.CloseTable(); // growth
        }
    } // if (initial)

    // Reloads available skill according to equipment
    _UpdatesAvailableSkills();

    char_script.CloseTable(); // "characters[id]" to restore the script in its original state.

    // Close the script file and calculate all rating totals
    if(char_script.IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading final data - they are listed below"
                          << std::endl << char_script.GetErrorMessages() << std::endl;
        }
    }

    // Init and updates the status effects according to current equipment.
    _equipment_status_effects.resize(GLOBAL_STATUS_TOTAL, GLOBAL_INTENSITY_NEUTRAL);
    _UpdateEquipmentStatusEffects();

    // Init the active status effects data
    ResetActiveStatusEffects();

    _CalculateAttackRatings();
    _CalculateDefenseRatings();
    _CalculateEvadeRatings();
}

GlobalCharacter::~GlobalCharacter()
{
    //
    // Delete all equipment.
    //

    _weapon_equipped = nullptr;

    for (uint32_t i = 0; i < _armor_equipped.size(); ++i) {
        _armor_equipped[i] = nullptr;
    }
    _armor_equipped.clear();
}

bool GlobalCharacter::AddExperiencePoints(uint32_t xp)
{
    _total_experience_points += xp;
    _experience_for_next_level -= xp;
    return ReachedNewExperienceLevel();
}

void GlobalCharacter::AddPhysAtk(uint32_t amount)
{
    _char_phys_atk.SetBase(_char_phys_atk.GetBase() + (float)amount);
    _CalculateAttackRatings();
}

void GlobalCharacter::SubtractPhysAtk(uint32_t amount)
{
    float new_base = _char_phys_atk.GetBase() - (float)amount;
    _char_phys_atk.SetBase(new_base < 0.0f ? 0.0f : new_base);
    _CalculateAttackRatings();
}

void GlobalCharacter::AddMagAtk(uint32_t amount)
{
    _char_mag_atk.SetBase(_char_mag_atk.GetBase() + (float)amount);
    _CalculateAttackRatings();
}

void GlobalCharacter::SubtractMagAtk(uint32_t amount)
{
    float new_base = _char_mag_atk.GetBase() - (float)amount;
    _char_mag_atk.SetBase(new_base < 0.0f ? 0.0f : new_base);
    _CalculateAttackRatings();
}

void GlobalCharacter::AddPhysDef(uint32_t amount)
{
    _char_phys_def.SetBase(_char_phys_def.GetBase() + (float)amount);
    _CalculateDefenseRatings();
}

void GlobalCharacter::SubtractPhysDef(uint32_t amount)
{
    float new_base = _char_phys_def.GetBase() - (float)amount;
    _char_phys_def.SetBase(new_base < 0.0f ? 0.0f : new_base);
    _CalculateDefenseRatings();
}

void GlobalCharacter::AddMagDef(uint32_t amount)
{
    _char_mag_def.SetBase(_char_mag_def.GetBase() + (float)amount);
    _CalculateDefenseRatings();
}

void GlobalCharacter::SubtractMagDef(uint32_t amount)
{
    float new_base = _char_mag_def.GetBase() - (float)amount;
    _char_mag_def.SetBase(new_base < 0.0f ? 0.0f : new_base);
    _CalculateDefenseRatings();
}

std::shared_ptr<GlobalWeapon> GlobalCharacter::EquipWeapon(const std::shared_ptr<GlobalWeapon>& weapon)
{
    std::shared_ptr<GlobalWeapon> old_weapon = _weapon_equipped;
    _weapon_equipped = weapon;

    // Updates the equipment status effects first
    _UpdateEquipmentStatusEffects();

    _CalculateAttackRatings();
    _UpdatesAvailableSkills();

    return old_weapon;
}

std::shared_ptr<GlobalArmor> GlobalCharacter::_EquipArmor(const std::shared_ptr<GlobalArmor>& armor, uint32_t index)
{
    if(index >= _armor_equipped.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of pieces of armor equipped: " << index << std::endl;
        return armor;
    }

    std::shared_ptr<GlobalArmor> old_armor = _armor_equipped[index];
    _armor_equipped[index] = armor;

    if(old_armor != nullptr && armor != nullptr) {
        if(old_armor->GetObjectType() != armor->GetObjectType()) {
            IF_PRINT_WARNING(GLOBAL_DEBUG) << "old armor was replaced with a different type of armor" << std::endl;
        }
    }

    // Updates the equipment status effect first
    _UpdateEquipmentStatusEffects();
    // This is a subset of _CalculateDefenseRatings(), but just for the given armor.
    _attack_points[index]->CalculateTotalDefense(_armor_equipped[index]);

    // Reloads available skill according to equipment
    _UpdatesAvailableSkills();

    return old_armor;
}

std::shared_ptr<GlobalArmor> GlobalCharacter::GetArmorEquipped(uint32_t index) const
{
    if(index >= _armor_equipped.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded number of pieces of armor equipped: " << index << std::endl;
        return nullptr;
    }

    return _armor_equipped[index];
}

bool GlobalCharacter::HasEquipment() const
{
    if (_weapon_equipped)
        return true;

    for (uint32_t i = 0; i < _armor_equipped.size(); ++i) {
        if (_armor_equipped.at(i) != nullptr)
            return true;
    }
    return false;
}

void GlobalCharacter::_UpdateEquipmentStatusEffects()
{
    // Reset the status effect intensities
    for (uint32_t i = 0; i < _equipment_status_effects.size(); ++i)
        _equipment_status_effects[i] = GLOBAL_INTENSITY_NEUTRAL;

    // For each piece of equipment, we add the intensity of the given status
    // effect on the status effect cache
    if (_weapon_equipped) {
        const std::vector<std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY> >& _effects = _weapon_equipped->GetStatusEffects();

        for (uint32_t i = 0; i < _effects.size(); ++i) {

            GLOBAL_STATUS effect = _effects[i].first;
            GLOBAL_INTENSITY intensity = _effects[i].second;

            // Check bounds and update the intensity
            if (_equipment_status_effects[effect] + intensity > GLOBAL_INTENSITY_POS_EXTREME)
                _equipment_status_effects[effect] = GLOBAL_INTENSITY_POS_EXTREME;
            else if (_equipment_status_effects[effect] + intensity < GLOBAL_INTENSITY_NEG_EXTREME)
                _equipment_status_effects[effect] = GLOBAL_INTENSITY_NEG_EXTREME;
            else
                _equipment_status_effects[effect] = (GLOBAL_INTENSITY)(_equipment_status_effects[effect] + intensity);
        }
    }

    // armors
    for (uint32_t i = 0; i < _armor_equipped.size(); ++i) {
        if (!_armor_equipped[i])
            continue;

        const std::vector<std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY> >& _effects = _armor_equipped[i]->GetStatusEffects();

        for (uint32_t j = 0; j < _effects.size(); ++j) {

            GLOBAL_STATUS effect = _effects[j].first;
            GLOBAL_INTENSITY intensity = _effects[j].second;

            // Check bounds and update the intensity
            if (_equipment_status_effects[effect] + intensity > GLOBAL_INTENSITY_POS_EXTREME)
                _equipment_status_effects[effect] = GLOBAL_INTENSITY_POS_EXTREME;
            else if (_equipment_status_effects[effect] + intensity < GLOBAL_INTENSITY_NEG_EXTREME)
                _equipment_status_effects[effect] = GLOBAL_INTENSITY_NEG_EXTREME;
            else
                _equipment_status_effects[effect] = (GLOBAL_INTENSITY)(_equipment_status_effects[effect] + intensity);
        }
    }

    // Actually apply the effects on the character now
    ReadScriptDescriptor &script_file = vt_global::GlobalManager->GetStatusEffectsScript();
    for (uint32_t i = 0; i < _equipment_status_effects.size(); ++i) {
        GLOBAL_INTENSITY intensity = _equipment_status_effects[i];

        if (!script_file.OpenTable(i)) {
            PRINT_WARNING << "No status effect defined for this status value: " << i << std::endl;
            continue;
        }

        if (intensity == GLOBAL_INTENSITY_NEUTRAL) {

            // Call RemovePassive(global_actor)
            if(!script_file.DoesFunctionExist("RemovePassive")) {
                PRINT_WARNING << "No RemovePassive() function found in Lua definition file for status: " << i << std::endl;
                script_file.CloseTable(); // effect id
                continue;
            }

            luabind::object remove_passive_function = script_file.ReadFunctionPointer("RemovePassive");
            script_file.CloseTable(); // effect id

            if (!remove_passive_function.is_valid()) {
                PRINT_WARNING << "Invalid RemovePassive() function found in Lua definition file for status: " << i << std::endl;
                continue;
            }

            try {
                luabind::call_function<void>(remove_passive_function, this);
            } catch(const luabind::error &e) {
                PRINT_ERROR << "Error while loading status effect RemovePassive() function" << std::endl;
                ScriptManager->HandleLuaError(e);
            } catch(const luabind::cast_failed &e) {
                PRINT_ERROR << "Error while loading status effect RemovePassive() function" << std::endl;
                ScriptManager->HandleCastError(e);
            }
        }
        else {
            // Call ApplyPassive(global_actor, intensity)
            if(!script_file.DoesFunctionExist("ApplyPassive")) {
                PRINT_WARNING << "No ApplyPassive() function found in Lua definition file for status: " << i << std::endl;
                script_file.CloseTable(); // effect id
                continue;
            }

            luabind::object apply_passive_function = script_file.ReadFunctionPointer("ApplyPassive");
            script_file.CloseTable(); // effect id

            if (!apply_passive_function.is_valid()) {
                PRINT_WARNING << "Invalid ApplyPassive() function found in Lua definition file for status: " << i << std::endl;
                continue;
            }

            try {
                luabind::call_function<void>(apply_passive_function, this, intensity);
            } catch(const luabind::error &e) {
                PRINT_ERROR << "Error while loading status effect ApplyPassive() function" << std::endl;
                ScriptManager->HandleLuaError(e);
            } catch(const luabind::cast_failed &e) {
                PRINT_ERROR << "Error while loading status effect ApplyPassive() function" << std::endl;
                ScriptManager->HandleCastError(e);
            }
        } // Call function depending on intensity
    } // For each equipment status effect
}

bool GlobalCharacter::AddSkill(uint32_t skill_id, bool permanently)
{
    if(skill_id == 0) {
        PRINT_WARNING << "function received an invalid skill_id argument: " << skill_id << std::endl;
        return false;
    }

    GlobalSkill *skill = new GlobalSkill(skill_id);
    if(!skill->IsValid()) {
        PRINT_WARNING << "the skill to add failed to load: " << skill_id << std::endl;
        delete skill;
        return false;
    }

    if(HasSkill(skill_id)) {
        //Test whether the skill should become permanent
        if (permanently) {
            bool found = false;
            for (uint32_t i = 0; i < _permanent_skills.size(); ++i) {
                if (_permanent_skills[i] == skill_id) {
                    found = true;
                    i = _permanent_skills.size();
                }
            }

            // if the skill wasn't permanent, it will then become one.
            if (!found)
                _permanent_skills.push_back(skill->GetID());
        }

        // The character already knew the skill but that doesn't really matter.
        delete skill;
        return true;
    }

    // Insert the pointer to the new skill inside of the global skills map and the skill type vector
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
    case GLOBAL_SKILL_BARE_HANDS:
        _bare_hands_skills.push_back(skill);
        break;
    default:
        PRINT_WARNING << "loaded a new skill with an unknown skill type: " << skill->GetType() << std::endl;
        return false;
        break;
    }

    _skills_id.push_back(skill_id);
    _skills.push_back(skill);
    if (permanently)
        _permanent_skills.push_back(skill->GetID());

    return true;
}

bool GlobalCharacter::AddNewSkillLearned(uint32_t skill_id)
{
    if(skill_id == 0) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received an invalid skill_id argument: " << skill_id << std::endl;
        return false;
    }

    // Make sure we don't add a skill more than once
    for(std::vector<GlobalSkill*>::iterator it = _new_skills_learned.begin(); it != _new_skills_learned.end(); ++it) {
        if(skill_id == (*it)->GetID()) {
            IF_PRINT_WARNING(GLOBAL_DEBUG) << "the skill to add was already present in the list of newly learned skills: "
                                           << skill_id << std::endl;
            return false;
        }
    }

    if (!AddSkill(skill_id))
        PRINT_WARNING << "Failed because the new skill was not added successfully: " << skill_id << std::endl;
    else
        _new_skills_learned.push_back(_skills.back());

    return true;
}

void GlobalCharacter::_UpdatesAvailableSkills()
{
    // Clears out the skills <and parse the current equipment to tells which ones are available.
    for (uint32_t i = 0; i < _skills.size(); ++i) {
        delete _skills[i];
    }
    _skills.clear();
    _skills_id.clear();

    _bare_hands_skills.clear();
    _weapon_skills.clear();
    _magic_skills.clear();
    _special_skills.clear();

    // First readd the permanent ones
    for (uint32_t i = 0; i < _permanent_skills.size(); ++i) {
        // As the skill is already permanent, don't readd it as one.
        AddSkill(_permanent_skills[i], false);
    }

    // Now, add skill obtained through current equipment.
    if (_weapon_equipped) {
        const std::vector<uint32_t>& wpn_skills = _weapon_equipped->GetEquipmentSkills();

        for (uint32_t i = 0; i < wpn_skills.size(); ++i)
            AddSkill(wpn_skills[i], false);
    }

    for (uint32_t i = 0; i < _armor_equipped.size(); ++i) {
        if (!_armor_equipped[i])
            continue;

        const std::vector<uint32_t>& armor_skills = _armor_equipped[i]->GetEquipmentSkills();

        for (uint32_t j = 0; j < armor_skills.size(); ++j)
            AddSkill(armor_skills[j], false);
    }
}

vt_video::AnimatedImage* GlobalCharacter::RetrieveBattleAnimation(const std::string &name)
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
    ReadScriptDescriptor& character_script = GlobalManager->GetCharactersScript();

    // Clear the growth members before filling their data
    _hit_points_growth = 0;
    _skill_points_growth = 0;
    _phys_atk_growth = 0;
    _mag_atk_growth = 0;
    _phys_def_growth = 0;
    _mag_def_growth = 0;
    _stamina_growth = 0;
    _evade_growth = 0.0f;

    try {
        // Update Growth data and set XP for next level
        luabind::call_function<void>(character_script.GetLuaState(), "DetermineLevelGrowth", this);
    } catch(const luabind::error& e) {
        ScriptManager->HandleLuaError(e);
    } catch(const luabind::cast_failed& e) {
        ScriptManager->HandleCastError(e);
    }

    // Reset the skills learned container and add any skills learned at this level
    _new_skills_learned.clear();
    try {
        luabind::call_function<void>(character_script.GetLuaState(), "DetermineNewSkillsLearned", this);
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

    if(_phys_atk_growth != 0)
        AddPhysAtk(_phys_atk_growth);
    if(_mag_atk_growth != 0)
        AddMagAtk(_mag_atk_growth);
    if(_phys_def_growth != 0)
        AddPhysDef(_phys_def_growth);
    if(_mag_def_growth != 0)
        AddMagDef(_mag_def_growth);
    if(_stamina_growth != 0)
        AddStamina(_stamina_growth);
    if(!IsFloatEqual(_evade_growth, 0.0f))
        AddEvade(_evade_growth);
}

bool GlobalCharacter::IsSkillNodeObtained(uint32_t skill_node_id) const
{
    for (uint32_t id : _obtained_skill_nodes) {
        if (id == skill_node_id) {
            return true;
        }
    }
    return false;
}

void GlobalCharacter::AddObtainedSkillNode(uint32_t skill_node_id) {
    // Check whether the skill node is already there
    if (IsSkillNodeObtained(skill_node_id)) {
        PRINT_WARNING << "Attempted to add already obtained node skill id: "
                      << skill_node_id << std::endl;
        return;
    }

    SkillNode* node = GlobalManager->GetSkillGraph().GetSkillNode(skill_node_id);
    if (!node) {
        PRINT_WARNING << "Invalid Skill Node id: " << skill_node_id << std::endl;
        return;
    }

    // FIXME: Removes XP
    //RemoveXP(node->GetExperiencePointsNeeded());

    // Apply stats upgrades
    const std::vector<std::pair<uint32_t, uint32_t> >& stats = node->GetStatsUpgrades();
    for (auto stat : stats) {
        switch (stat.first) {
            default:
                PRINT_WARNING << "Unsupported stat id: (" << stat.first
                              << ") in skill node id: (" << node->GetId() << ")" << std::endl;
            break;
            case GLOBAL_STATUS_PHYS_ATK: // FIXME: Improve API for stats, it is sooo duplicated.
                AddPhysAtk(stat.second);
                break;
            case GLOBAL_STATUS_MAG_ATK:
                AddMagAtk(stat.second);
                break;
            case GLOBAL_STATUS_PHYS_DEF:
                AddPhysDef(stat.second);
                break;
            case GLOBAL_STATUS_MAG_DEF:
                AddMagDef(stat.second);
                break;
            case GLOBAL_STATUS_STAMINA:
                AddStamina(stat.second);
                break;
            case GLOBAL_STATUS_EVADE:
                AddEvade(static_cast<float>(stat.second) / 10.0f);
                break;
            case GLOBAL_STATUS_HP:
                AddMaxHitPoints(stat.second);
                break;
            case GLOBAL_STATUS_SP:
                AddMaxSkillPoints(stat.second);
                break;
        }
    }

    // Permanently add new skill
    if (node->GetSkillIdLearned() != -1) {
        AddSkill(node->GetSkillIdLearned(), true);
    }

    _obtained_skill_nodes.emplace_back(skill_node_id);

    // The character location is on the latest obtained skill_node_id.
    SetSkillNodeLocation(skill_node_id);
}

void GlobalCharacter::ApplyActiveStatusEffect(GLOBAL_STATUS status_effect,
                                              GLOBAL_INTENSITY intensity,
                                              uint32_t duration)
{
    if (status_effect == GLOBAL_STATUS_INVALID || status_effect == GLOBAL_STATUS_TOTAL)
        return;

    if (intensity == GLOBAL_INTENSITY_INVALID || intensity == GLOBAL_INTENSITY_NEUTRAL)
        return;

    // Get the reference of the corresponding active effect.
    ActiveStatusEffect& effect = _active_status_effects[status_effect];
    // If there are no previously applied status effect, we set a new one.
    if (effect.GetIntensity() == GLOBAL_INTENSITY_INVALID
            || effect.GetIntensity() == GLOBAL_INTENSITY_NEUTRAL) {
        SetActiveStatusEffect(status_effect, intensity, duration, 0);
        return;
    }

    // If a previous one was active, we must take in account the previous effect intensity
    int32_t new_intensity = intensity + effect.GetIntensity();
    // We also check bounds
    if (new_intensity >= GLOBAL_INTENSITY_TOTAL)
        new_intensity = GLOBAL_INTENSITY_POS_EXTREME;
    else if (new_intensity <= GLOBAL_INTENSITY_INVALID)
        new_intensity = GLOBAL_INTENSITY_NEG_EXTREME;

    SetActiveStatusEffect(status_effect, (GLOBAL_INTENSITY)new_intensity, duration, 0);
}

void GlobalCharacter::_CalculateAttackRatings()
{
    _total_physical_attack = _char_phys_atk.GetValue();

    if(_weapon_equipped) {
        _total_physical_attack += _weapon_equipped->GetPhysicalAttack();
        for (uint32_t i = 0; i < GLOBAL_ELEMENTAL_TOTAL; ++i) {
            _total_magical_attack[i] = (_char_mag_atk.GetValue() + _weapon_equipped->GetMagicalAttack())
                                       * GetElementalModifier((GLOBAL_ELEMENTAL) i);
        }
    }
    else {
        for (uint32_t i = 0; i < GLOBAL_ELEMENTAL_TOTAL; ++i) {
            _total_magical_attack[i] = _char_mag_atk.GetValue() * GetElementalModifier((GLOBAL_ELEMENTAL) i);
        }
    }
}

void GlobalCharacter::_CalculateDefenseRatings()
{
    // Re-calculate the defense ratings for all attack points
    for(uint32_t i = 0; i < _attack_points.size(); ++i) {
        if((i < _armor_equipped.size()) && (_armor_equipped[i] != nullptr))
            _attack_points[i]->CalculateTotalDefense(_armor_equipped[i]);
        else
            _attack_points[i]->CalculateTotalDefense(nullptr);
    }
}

} // namespace vt_global
