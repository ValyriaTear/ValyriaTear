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
#include "common/global/objects/global_armor.h"
#include "common/global/objects/global_weapon.h"

#include "script/script_read.h"
#include "utils/utils_files.h"

using namespace vt_utils;
using namespace vt_video;
using namespace vt_script;

namespace vt_global
{

extern bool GLOBAL_DEBUG;

GlobalCharacter::GlobalCharacter(uint32_t id, bool initial) :
    _enabled(true),
    _weapon_equipped(nullptr),
    _unspent_experience_points(0),
    _total_experience_points(0),
    _experience_level(0),
    _experience_for_next_level(0),
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
    // Delete all equipment.
    _weapon_equipped = nullptr;

    for (uint32_t i = 0; i < _armor_equipped.size(); ++i) {
        _armor_equipped[i] = nullptr;
    }
    _armor_equipped.clear();
}

bool GlobalCharacter::LoadCharacter(ReadScriptDescriptor& file)
{
    if(!file.IsFileOpen()) {
        PRINT_WARNING << "Can't load character, the file " << file.GetFilename()
            << " is not open." << std::endl;
        return false;
    }

    // This function assumes that the characters table in the saved game file is already open.
    // So all we need to open is the character's table
    if (!file.OpenTable(_id)) {
        PRINT_WARNING << "Can't load unexisting character id: " << _id << std::endl;
        return false;
    }

    // Gets whether the character is currently enabled
    if(file.DoesBoolExist("enabled")) {
        Enable(file.ReadBool("enabled"));
    }
    else { // old format DEPRECATED: Removed in one release
        Enable(true);
    }

    // Read in all of the character's stats data
    SetExperienceLevel(file.ReadUInt("experience_level"));

    uint32_t total_xp = file.ReadUInt("experience_points");
    // DEPRECATED: Used to upgrade old saves without skill trees.
    if (total_xp == 0) {
        total_xp = file.ReadUInt("total_experience_points");
    }
    SetTotalExperiencePoints(total_xp);

    _experience_for_next_level = file.ReadInt("experience_points_next");

    uint32_t unspent_xp = file.ReadUInt("unspent_experience_points", std::numeric_limits<uint32_t>::max());
    // DEPRECATED: Used to upgrade old saves without skill trees.
    if (unspent_xp == std::numeric_limits<uint32_t>::max()) {
        unspent_xp = GetTotalExperiencePoints();
    }
    _unspent_experience_points = unspent_xp;

    SetMaxHitPoints(file.ReadUInt("max_hit_points"));
    SetHitPoints(file.ReadUInt("hit_points"));
    SetMaxSkillPoints(file.ReadUInt("max_skill_points"));
    SetSkillPoints(file.ReadUInt("skill_points"));

    // DEPRECATED: Old confusing character's stats. Remove for Episode II release.
    if (file.DoesUIntExist("strength")) {
        SetPhysAtk(file.ReadUInt("strength"));
        SetMagAtk(file.ReadUInt("vigor"));
        SetPhysDef(file.ReadUInt("fortitude"));
        SetMagDef(file.ReadUInt("protection"));
        SetStamina(file.ReadUInt("agility"));
    }
    else {
        SetPhysAtk(file.ReadUInt("phys_atk"));
        SetMagAtk(file.ReadUInt("mag_atk"));
        SetPhysDef(file.ReadUInt("phys_def"));
        SetMagDef(file.ReadUInt("mag_def"));
        SetStamina(file.ReadUInt("stamina"));
    }
    SetEvade(file.ReadFloat("evade"));

    // Read the character's equipment and load it onto the character
    if (file.OpenTable("equipment")) {
        uint32_t equip_id;

        // Equip the objects on the character as long as valid equipment IDs were read
        equip_id = file.ReadUInt("weapon");
        if(equip_id != 0) {
            EquipWeapon(std::make_shared<GlobalWeapon>(equip_id));
        }

        equip_id = file.ReadUInt("head_armor");
        if(equip_id != 0) {
            EquipHeadArmor(std::make_shared<GlobalArmor>(equip_id));
        }

        equip_id = file.ReadUInt("torso_armor");
        if(equip_id != 0) {
            EquipTorsoArmor(std::make_shared<GlobalArmor>(equip_id));
        }

        equip_id = file.ReadUInt("arm_armor");
        if(equip_id != 0) {
            EquipArmArmor(std::make_shared<GlobalArmor>(equip_id));
        }

        equip_id = file.ReadUInt("leg_armor");
        if(equip_id != 0) {
            EquipLegArmor(std::make_shared<GlobalArmor>(equip_id));
        }

        file.CloseTable(); // equipment
    }

    // Read the character's skills and pass those onto the character object
    std::vector<uint32_t> skill_ids;

    skill_ids.clear();
    file.ReadUIntVector("skills", skill_ids);
    for(uint32_t i = 0; i < skill_ids.size(); i++) {
        // DEPRECATED HACK: Remove that in one release.
        // Turn old bare hands skills id into new ones at load time.
        if (skill_ids[i] == 999)
            skill_ids[i] = 30002;
        else if (skill_ids[i] == 1000)
            skill_ids[i] = 30001;

        AddSkill(skill_ids[i]);
    }

    //DEPRECATED: Will be removed in one release!
    skill_ids.clear();
    file.ReadUIntVector("weapon_skills", skill_ids);
    for(uint32_t i = 0; i < skill_ids.size(); i++) {
        // DEPRECATED HACK: Remove that in one release.
        // Turn old bare hands skills id into new ones at load time.
        if (skill_ids[i] == 999)
            skill_ids[i] = 30002;
        else if (skill_ids[i] == 1000)
            skill_ids[i] = 30001;

        AddSkill(skill_ids[i]);
    }
    //DEPRECATED: Will be removed in one release!
    skill_ids.clear();
    file.ReadUIntVector("magic_skills", skill_ids);
    for(uint32_t i = 0; i < skill_ids.size(); ++i) {
        AddSkill(skill_ids[i]);
    }
    //DEPRECATED: Will be removed in one release!
    skill_ids.clear();
    file.ReadUIntVector("special_skills", skill_ids);
    for(uint32_t i = 0; i < skill_ids.size(); ++i) {
        AddSkill(skill_ids[i]);
    }
    //DEPRECATED: Will be removed in one release!
    skill_ids.clear();
    file.ReadUIntVector("bare_hands_skills", skill_ids);
    for(uint32_t i = 0; i < skill_ids.size(); ++i) {
        AddSkill(skill_ids[i]);
    }

    // DEPRECATED: Remove in one release
    skill_ids.clear();
    file.ReadUIntVector("defense_skills", skill_ids);
    for(uint32_t i = 0; i < skill_ids.size(); ++i) {
        AddSkill(skill_ids[i]);
    }
    // DEPRECATED: Remove in one release
    file.ReadUIntVector("attack_skills", skill_ids);
    for(uint32_t i = 0; i < skill_ids.size(); i++) {
        AddSkill(skill_ids[i]);
    }
    // DEPRECATED: Remove in one release
    skill_ids.clear();
    file.ReadUIntVector("support_skills", skill_ids);
    for(uint32_t i = 0; i < skill_ids.size(); ++i) {
        AddSkill(skill_ids[i]);
    }

    // Read the character's obtained skill nodes
    ResetObtainedSkillNodes();
    std::vector<uint32_t> skill_node_ids;
    file.ReadUIntVector("obtained_skill_nodes", skill_node_ids);
    SetObtainedSkillNodes(skill_node_ids);

    // Read the current skill node location
    uint32_t default_character_location = file.ReadUInt("current_skill_node",
                                                        std::numeric_limits<uint32_t>::max());
    if (default_character_location != std::numeric_limits<uint32_t>::max()) {
        SetSkillNodeLocation(default_character_location);

        // Add the current node position as obtained if it is not in the data
        // This permits to fix obtaining the first nodes
        if (!IsSkillNodeObtained(default_character_location)) {
            _obtained_skill_nodes.push_back(default_character_location);
        }
    }

    // Read the character's active status effects data
    ResetActiveStatusEffects();
    std::vector<int32_t> status_effects_ids;
    file.ReadTableKeys("active_status_effects", status_effects_ids);

    if (file.OpenTable("active_status_effects")) {

        for(uint32_t i = 0; i < status_effects_ids.size(); ++i) {
            int32_t status_effect = status_effects_ids[i];

            if (!file.OpenTable(status_effect))
                continue;

            // Check the status effect validity
            if (status_effect <= (int32_t)GLOBAL_STATUS_INVALID || status_effect >= (int32_t)GLOBAL_STATUS_TOTAL) {
                file.CloseTable(); // status_effect
                continue;
            }

            // Check the status intensity validity
            int32_t intensity = file.ReadInt("intensity");
            if (intensity <= GLOBAL_INTENSITY_INVALID || intensity >= GLOBAL_INTENSITY_TOTAL) {
                file.CloseTable(); // status_effect
                continue;
            }

            uint32_t duration = file.ReadInt("duration");
            uint32_t elapsed_time = file.ReadInt("elapsed_time");

            SetActiveStatusEffect((GLOBAL_STATUS)status_effect,
                                  (GLOBAL_INTENSITY)intensity,
                                  duration, elapsed_time);

            file.CloseTable(); // status_effect
        }

        file.CloseTable(); // active_status_effects
    }

    file.CloseTable(); // character id
    return true;
}

bool GlobalCharacter::SaveCharacter(WriteScriptDescriptor& file)
{
    if(!file.IsFileOpen()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "the file provided in the function argument was not open" << std::endl;
        return false;
    }

    file.WriteLine("\t[" + NumberToString(GetID()) + "] = {");

    // Store whether the character is available
    file.WriteLine("\t\tenabled = " + std::string(IsEnabled() ? "true" : "false") + ",");

    // Write out the character's stats
    file.WriteLine("\t\texperience_level = " + NumberToString(GetExperienceLevel()) + ",");
    file.WriteLine("\t\tunspent_experience_points = " + NumberToString(GetUnspentExperiencePoints()) + ", ");
    file.WriteLine("\t\ttotal_experience_points = " + NumberToString(GetTotalExperiencePoints()) + ",");
    file.WriteLine("\t\texperience_points_next = " + NumberToString(GetExperienceForNextLevel()) + ", ");

    // The values stored are the unmodified ones.
    file.WriteLine("\t\tmax_hit_points = " + NumberToString(GetMaxHitPoints()) + ",");
    file.WriteLine("\t\thit_points = " + NumberToString(GetHitPoints()) + ",");
    file.WriteLine("\t\tmax_skill_points = " + NumberToString(GetMaxSkillPoints()) + ",");
    file.WriteLine("\t\tskill_points = " + NumberToString(GetSkillPoints()) + ",");

    file.WriteLine("\t\tphys_atk = " + NumberToString(GetPhysAtkBase()) + ",");
    file.WriteLine("\t\tmag_atk = " + NumberToString(GetMagAtkBase()) + ",");
    file.WriteLine("\t\tphys_def = " + NumberToString(GetPhysDefBase()) + ",");
    file.WriteLine("\t\tmag_def = " + NumberToString(GetMagDefBase()) + ",");
    file.WriteLine("\t\tstamina = " + NumberToString(GetStaminaBase()) + ",");
    file.WriteLine("\t\tevade = " + NumberToString(GetEvadeBase()) + ",");

    // Write out the character's equipment
    uint32_t weapon_id = GetWeaponEquipped() ? GetWeaponEquipped()->GetID() : 0;
    uint32_t head_id = GetHeadArmorEquipped() ? GetHeadArmorEquipped()->GetID() : 0;
    uint32_t torso_id = GetTorsoArmorEquipped() ? GetTorsoArmorEquipped()->GetID() : 0;
    uint32_t arm_id = GetArmArmorEquipped() ? GetArmArmorEquipped()->GetID() : 0;
    uint32_t leg_id = GetLegArmorEquipped() ? GetLegArmorEquipped()->GetID() : 0;

    file.InsertNewLine();
    file.WriteLine("\t\tequipment = {");
    file.WriteLine("\t\t\tweapon = " + NumberToString(weapon_id) + ",");
    file.WriteLine("\t\t\thead_armor = " + NumberToString(head_id) + ",");
    file.WriteLine("\t\t\ttorso_armor = " + NumberToString(torso_id) + ",");
    file.WriteLine("\t\t\tarm_armor = " + NumberToString(arm_id) + ",");
    file.WriteLine("\t\t\tleg_armor = " + NumberToString(leg_id));
    file.WriteLine("\t\t},");

    // Write out the character's permanent skills.
    // The equipment skills will be reloaded through equipment.
    file.InsertNewLine();
    file.WriteLine("\t\tskills = {");
    const std::vector<uint32_t>& skill_vector = GetPermanentSkills();
    for(uint32_t i = 0; i < skill_vector.size(); i++) {
        uint32_t skill_id = skill_vector.at(i);

        if(i == 0)
            file.WriteLine("\t\t\t", false);
        else
            file.WriteLine(", ", false);
        file.WriteLine(NumberToString(skill_id), false);
    }
    file.WriteLine("\n\t\t},");

    // Write out the character's obtained skill nodes.
    file.InsertNewLine();
    file.WriteLine("\t\tobtained_skill_nodes = {");
    const std::vector<uint32_t>& skill_nodes = GetObtainedSkillNodes();
    for(uint32_t i = 0; i < skill_nodes.size(); i++) {
        uint32_t skill_node = skill_nodes.at(i);

        if(i == 0)
            file.WriteLine("\t\t\t", false);
        else
            file.WriteLine(", ", false);
        bool newline = (i > 0) && !(i % 10);
        file.WriteLine(NumberToString(skill_node), newline);
    }
    file.WriteLine("\n\t\t},");
    file.WriteLine("\t\tcurrent_skill_node = " + NumberToString(GetSkillNodeLocation()) + ",");

    // Writes active status effects at the time of the save
    file.InsertNewLine();
    file.WriteLine("\t\tactive_status_effects = {");
    for(uint32_t i = 0; i < _active_status_effects.size(); ++i) {
        const ActiveStatusEffect& effect = _active_status_effects.at(i);
        if (!effect.IsActive())
            continue;

        std::string effect_str = "\t\t\t[" + NumberToString((int32_t)effect.GetEffect()) + "] = { ";
        effect_str += "intensity = " + NumberToString((int32_t)effect.GetIntensity()) + ", ";
        effect_str += "duration = " + NumberToString((int32_t)effect.GetEffectTime()) + ", ";
        effect_str += "elapsed_time = " + NumberToString((int32_t)effect.GetElapsedTime()) + "},";

        file.WriteLine(effect_str);
    }
    file.WriteLine("\n\t\t}");

    file.WriteLine("\t},");
    return true;
}

bool GlobalCharacter::AddExperiencePoints(uint32_t xp)
{
    _total_experience_points += xp;
    _experience_for_next_level -= xp;
    _unspent_experience_points += xp;

    if (_experience_for_next_level <= 0) {
        ++_experience_level;
        return true;
    }
    return false;
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

    // Removes unspent XP
    RemoveUnspentExperiencePoints(node->GetExperiencePointsNeeded());

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
