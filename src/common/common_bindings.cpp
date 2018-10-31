///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    common_bindings.cpp
*** \author  Daniel Steuernol, steu@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Lua bindings for common game code
***
*** All bindings for the common code is contained within this file.
*** Therefore, everything that you see bound within this file will be made
*** available in Lua. This file also binds some of the utility code
*** found in src/utils.h.
***
*** \note To most C++ programmers, the syntax of the binding code found in this
*** file may be very unfamiliar and obtuse. Refer to the Luabind documentation
*** as necessary to gain an understanding of this code style.
*** **************************************************************************/

#include "common.h"
#include "dialogue.h"

#include "common/global/actors/global_attack_point.h"
#include "common/global/global.h"
#include "common/global/status_effects/global_effects.h"
#include "common/global/objects/global_weapon.h"
#include "common/global/objects/global_armor.h"
#include "common/global/global_skills.h"
#include "common/global/actors/global_character.h"
#include "common/global/actors/global_enemy.h"
#include "modes/battle/battle_target.h"

#include "utils/utils_random.h"

namespace vt_defs
{

void BindCommonCode()
{
    // ---------- Bind Utils Functions
    {
        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_utils")
        [
            luabind::class_<vt_utils::ustring>("ustring")
            .def(luabind::constructor<uint16_t *>())
            .def(luabind::constructor<>())
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_utils")
        [
            luabind::def("RandomFloat", (float( *)(void)) &vt_utils::RandomFloat),
            luabind::def("RandomBoundedInteger", &vt_utils::RandomBoundedInteger),
            luabind::def("MakeUnicodeString", &vt_utils::MakeUnicodeString),
            luabind::def("MakeStandardString", &vt_utils::MakeStandardString)
        ];
    }

    // ---------- Bind Common Components
    {
        using namespace vt_common;

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_common")
        [
            luabind::class_<Dialogue>("Dialogue")
            .def("AddLine", (void(Dialogue:: *)(const std::string&, const std::string&))&Dialogue::AddLine)
            .def("AddLine", (void(Dialogue:: *)(const std::string &, const std::string&, int32_t))&Dialogue::AddLine)
            .def("AddLineTimed", (void(Dialogue:: *)(const std::string &, const std::string&, uint32_t))&Dialogue::AddLineTimed)
            .def("AddLineTimed", (void(Dialogue:: *)(const std::string &, const std::string&, int32_t, uint32_t))&Dialogue::AddLineTimed)
            .def("AddOption", (void(Dialogue:: *)(const std::string &))&Dialogue::AddOption)
            .def("AddOption", (void(Dialogue:: *)(const std::string &, int32_t))&Dialogue::AddOption)
            .def("Validate", &Dialogue::Validate)
            .scope
            [   // Used for static members and nested classes.
                luabind::def("Create", &Dialogue::Create)
            ]
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_common")
        [
            luabind::class_<DialogueSupervisor>("DialogueSupervisor")
            .def("AddSpeaker", &DialogueSupervisor::AddSpeaker)
            .def("ChangeSpeakerName", &DialogueSupervisor::ChangeSpeakerName)
            .def("ChangeSpeakerPortrait", &DialogueSupervisor::ChangeSpeakerPortrait)
            .def("StartDialogue", &DialogueSupervisor::StartDialogue)
            .def("EndDialogue", &DialogueSupervisor::EndDialogue)
            .def("ForceNextLine", &DialogueSupervisor::ForceNextLine)
            .def("IsDialogueActive", &DialogueSupervisor::IsDialogueActive)
            .def("GetCurrentDialogue", &DialogueSupervisor::GetCurrentDialogue)
            .def("GetLineCounter", &DialogueSupervisor::GetLineCounter)
            .def("SetDialoguePosition", &DialogueSupervisor::SetDialoguePosition)
        ];

    } // End using common namespace


    // ---------- Bind Global Components
    {
        using namespace vt_global;

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<GameGlobal>("GameGlobal")
            .def("GetCharacterHandler", &GameGlobal::GetCharacterHandler)
            .def("GetInventoryHandler", &GameGlobal::GetInventoryHandler)
            .def("GetDrunes", &GameGlobal::GetDrunes)
            .def("SetDrunes", &GameGlobal::SetDrunes)
            .def("AddDrunes", &GameGlobal::AddDrunes)
            .def("SubtractDrunes", &GameGlobal::SubtractDrunes)
            .def("GetMaxExperienceLevel", &GameGlobal::GetMaxExperienceLevel)
            .def("SetMaxExperienceLevel", &GameGlobal::SetMaxExperienceLevel)
            .def("DoesEventGroupExist", &GameGlobal::DoesEventGroupExist)
            .def("DoesEventExist", &GameGlobal::DoesEventExist)
            .def("AddNewEventGroup", &GameGlobal::AddNewEventGroup)
            .def("GetEventGroup", &GameGlobal::GetEventGroup)
            .def("GetEventValue", &GameGlobal::GetEventValue)
            .def("SetEventValue", &GameGlobal::SetEventValue)
            .def("GetNumberEventGroups", &GameGlobal::GetNumberEventGroups)
            .def("GetNumberEvents", &GameGlobal::GetNumberEvents)
            .def("SetMapDataFilename", (void(GameGlobal:: *)(const std::string &)) &GameGlobal::SetMapDataFilename)
            .def("SetMapScriptFilename", (void(GameGlobal:: *)(const std::string &)) &GameGlobal::SetMapScriptFilename)
            .def("GetMapScriptFilename", &GameGlobal::GetMapScriptFilename)
            .def("SetMap", &GameGlobal::SetMap)
            .def("GetSaveLocationX", &GameGlobal::GetSaveLocationX)
            .def("GetSaveLocationY", &GameGlobal::GetSaveLocationY)
            .def("GetPreviousLocation", &GameGlobal::GetPreviousLocation)
            .def("AddQuestLog", &GameGlobal::AddQuestLog)
            .def("SetWorldMap", &GameGlobal::SetWorldMap)
            .def("ClearHomeMap", &GameGlobal::ClearHomeMap)
            .def("ShowWorldLocation", &GameGlobal::ShowWorldLocation)
            .def("HideWorldLocation", &GameGlobal::HideWorldLocation)
            .def("SetCurrentLocationId", &GameGlobal::SetCurrentLocationId)
            .def("GetBattleMedia", &GameGlobal::GetBattleMedia)

            // Namespace constants
            .enum_("constants") [
                // Object types
                luabind::value("GLOBAL_OBJECT_INVALID", GLOBAL_OBJECT_INVALID),
                luabind::value("GLOBAL_OBJECT_ITEM", GLOBAL_OBJECT_ITEM),
                luabind::value("GLOBAL_OBJECT_WEAPON", GLOBAL_OBJECT_WEAPON),
                luabind::value("GLOBAL_OBJECT_HEAD_ARMOR", GLOBAL_OBJECT_HEAD_ARMOR),
                luabind::value("GLOBAL_OBJECT_TORSO_ARMOR", GLOBAL_OBJECT_TORSO_ARMOR),
                luabind::value("GLOBAL_OBJECT_ARM_ARMOR", GLOBAL_OBJECT_ARM_ARMOR),
                luabind::value("GLOBAL_OBJECT_LEG_ARMOR", GLOBAL_OBJECT_LEG_ARMOR),
                luabind::value("GLOBAL_OBJECT_SPIRIT", GLOBAL_OBJECT_SPIRIT),
                // Item and skill alignment constants
                luabind::value("GLOBAL_POSITION_HEAD", GLOBAL_POSITION_HEAD),
                luabind::value("GLOBAL_POSITION_TORSO", GLOBAL_POSITION_TORSO),
                luabind::value("GLOBAL_POSITION_ARMS", GLOBAL_POSITION_ARMS),
                luabind::value("GLOBAL_POSITION_LEGS", GLOBAL_POSITION_LEGS),
                // Skill types
                luabind::value("GLOBAL_SKILL_INVALID", GLOBAL_SKILL_INVALID),
                luabind::value("GLOBAL_SKILL_WEAPON", GLOBAL_SKILL_WEAPON),
                luabind::value("GLOBAL_SKILL_MAGIC", GLOBAL_SKILL_MAGIC),
                luabind::value("GLOBAL_SKILL_SPECIAL", GLOBAL_SKILL_SPECIAL),
                luabind::value("GLOBAL_SKILL_BARE_HANDS", GLOBAL_SKILL_BARE_HANDS),
                // Elemental type constants
                luabind::value("GLOBAL_ELEMENTAL_FIRE", GLOBAL_ELEMENTAL_FIRE),
                luabind::value("GLOBAL_ELEMENTAL_WATER", GLOBAL_ELEMENTAL_WATER),
                luabind::value("GLOBAL_ELEMENTAL_VOLT", GLOBAL_ELEMENTAL_VOLT),
                luabind::value("GLOBAL_ELEMENTAL_EARTH", GLOBAL_ELEMENTAL_EARTH),
                luabind::value("GLOBAL_ELEMENTAL_LIFE", GLOBAL_ELEMENTAL_LIFE),
                luabind::value("GLOBAL_ELEMENTAL_DEATH", GLOBAL_ELEMENTAL_DEATH),
                luabind::value("GLOBAL_ELEMENTAL_NEUTRAL", GLOBAL_ELEMENTAL_NEUTRAL),
                // Status type constants
                luabind::value("GLOBAL_STATUS_INVALID", GLOBAL_STATUS_INVALID),
                luabind::value("GLOBAL_STATUS_PHYS_ATK", GLOBAL_STATUS_PHYS_ATK),
                luabind::value("GLOBAL_STATUS_MAG_ATK", GLOBAL_STATUS_MAG_ATK),
                luabind::value("GLOBAL_STATUS_PHYS_DEF", GLOBAL_STATUS_PHYS_DEF),
                luabind::value("GLOBAL_STATUS_MAG_DEF", GLOBAL_STATUS_MAG_DEF),
                luabind::value("GLOBAL_STATUS_STAMINA", GLOBAL_STATUS_STAMINA),
                luabind::value("GLOBAL_STATUS_EVADE", GLOBAL_STATUS_EVADE),
                luabind::value("GLOBAL_STATUS_HP", GLOBAL_STATUS_HP),
                luabind::value("GLOBAL_STATUS_SP", GLOBAL_STATUS_SP),
                luabind::value("GLOBAL_STATUS_PARALYSIS", GLOBAL_STATUS_PARALYSIS),
                luabind::value("GLOBAL_STATUS_FIRE", GLOBAL_STATUS_FIRE),
                luabind::value("GLOBAL_STATUS_WATER", GLOBAL_STATUS_WATER),
                luabind::value("GLOBAL_STATUS_VOLT", GLOBAL_STATUS_VOLT),
                luabind::value("GLOBAL_STATUS_EARTH", GLOBAL_STATUS_EARTH),
                luabind::value("GLOBAL_STATUS_LIFE", GLOBAL_STATUS_LIFE),
                luabind::value("GLOBAL_STATUS_DEATH", GLOBAL_STATUS_DEATH),
                luabind::value("GLOBAL_STATUS_NEUTRAL", GLOBAL_STATUS_NEUTRAL),
                // Intensity type constants
                luabind::value("GLOBAL_INTENSITY_NEG_EXTREME", GLOBAL_INTENSITY_NEG_EXTREME),
                luabind::value("GLOBAL_INTENSITY_NEG_GREATER", GLOBAL_INTENSITY_NEG_GREATER),
                luabind::value("GLOBAL_INTENSITY_NEG_MODERATE", GLOBAL_INTENSITY_NEG_MODERATE),
                luabind::value("GLOBAL_INTENSITY_NEG_LESSER", GLOBAL_INTENSITY_NEG_LESSER),
                luabind::value("GLOBAL_INTENSITY_NEUTRAL", GLOBAL_INTENSITY_NEUTRAL),
                luabind::value("GLOBAL_INTENSITY_POS_LESSER", GLOBAL_INTENSITY_POS_LESSER),
                luabind::value("GLOBAL_INTENSITY_POS_MODERATE", GLOBAL_INTENSITY_POS_MODERATE),
                luabind::value("GLOBAL_INTENSITY_POS_GREATER", GLOBAL_INTENSITY_POS_GREATER),
                luabind::value("GLOBAL_INTENSITY_POS_EXTREME", GLOBAL_INTENSITY_POS_EXTREME),
                // Target constants
                luabind::value("GLOBAL_TARGET_INVALID", GLOBAL_TARGET_INVALID),
                luabind::value("GLOBAL_TARGET_SELF_POINT", GLOBAL_TARGET_SELF_POINT),
                luabind::value("GLOBAL_TARGET_ALLY_POINT", GLOBAL_TARGET_ALLY_POINT),
                luabind::value("GLOBAL_TARGET_FOE_POINT", GLOBAL_TARGET_FOE_POINT),
                luabind::value("GLOBAL_TARGET_SELF", GLOBAL_TARGET_SELF),
                luabind::value("GLOBAL_TARGET_ALLY", GLOBAL_TARGET_ALLY),
                luabind::value("GLOBAL_TARGET_ALLY_EVEN_DEAD", GLOBAL_TARGET_ALLY_EVEN_DEAD),
                luabind::value("GLOBAL_TARGET_DEAD_ALLY_ONLY", GLOBAL_TARGET_DEAD_ALLY_ONLY),
                luabind::value("GLOBAL_TARGET_FOE", GLOBAL_TARGET_FOE),
                luabind::value("GLOBAL_TARGET_ALL_ALLIES", GLOBAL_TARGET_ALL_ALLIES),
                luabind::value("GLOBAL_TARGET_ALL_FOES", GLOBAL_TARGET_ALL_FOES),
                // Enemies hurt levels
                luabind::value("GLOBAL_ENEMY_HURT_NONE", GLOBAL_ENEMY_HURT_NONE),
                luabind::value("GLOBAL_ENEMY_HURT_SLIGHTLY", GLOBAL_ENEMY_HURT_SLIGHTLY),
                luabind::value("GLOBAL_ENEMY_HURT_MEDIUM", GLOBAL_ENEMY_HURT_MEDIUM),
                luabind::value("GLOBAL_ENEMY_HURT_HEAVILY", GLOBAL_ENEMY_HURT_HEAVILY)
            ]
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<CharacterHandler>("CharacterHandler")
            .def("AddCharacter", (void(CharacterHandler:: *)(uint32_t)) &CharacterHandler::AddCharacter)
            .def("RemoveCharacter", (void(CharacterHandler:: *)(uint32_t, bool)) &CharacterHandler::RemoveCharacter)
            .def("GetCharacter", &CharacterHandler::GetCharacter)
            .def("GetActiveParty", &CharacterHandler::GetActiveParty)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<GlobalParty>("GlobalParty")
            .def("AddHitPoints", &GlobalParty::AddHitPoints)
            .def("AddSkillPoints", &GlobalParty::AddSkillPoints)
            .def("GetPartyPosition", &GlobalParty::GetPartyPosition)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<GlobalAttackPoint>("GlobalAttackPoint")
            .def("GetName", &GlobalAttackPoint::GetName)
            .def("GetXPosition", &GlobalAttackPoint::GetXPosition)
            .def("GetYPosition", &GlobalAttackPoint::GetYPosition)
            .def("GetPhysDefModifier", &GlobalAttackPoint::GetPhysDefModifier)
            .def("GetMagDefModifier", &GlobalAttackPoint::GetMagDefModifier)
            .def("GetEvadeModifier", &GlobalAttackPoint::GetEvadeModifier)
            .def("GetActorOwner", &GlobalAttackPoint::GetActorOwner)
            .def("GetTotalPhysicalDefense", &GlobalAttackPoint::GetTotalPhysicalDefense)
            .def("GetTotalMagicalDefense", &GlobalAttackPoint::GetTotalMagicalDefense)
            .def("GetTotalEvadeRating", &GlobalAttackPoint::GetTotalEvadeRating)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<GlobalActor>("GlobalActor")
            .def("GetID", &GlobalActor::GetID)
            .def("GetName", &GlobalActor::GetName)

            .def("GetHitPoints", &GlobalActor::GetHitPoints)
            .def("GetMaxHitPoints", &GlobalActor::GetMaxHitPoints)
            .def("GetSkillPoints", &GlobalActor::GetSkillPoints)
            .def("GetMaxSkillPoints", &GlobalActor::GetMaxSkillPoints)
            .def("GetPhysAtk", &GlobalActor::GetPhysAtk)
            .def("GetMagAtk", &GlobalActor::GetMagAtk)
            .def("GetPhysDef", &GlobalActor::GetPhysDef)
            .def("GetMagDef", &GlobalActor::GetMagDef)
            .def("GetStamina", &GlobalActor::GetStamina)
            .def("GetEvade", &GlobalActor::GetEvade)

            .def("GetTotalPhysicalAttack", &GlobalActor::GetTotalPhysicalAttack)
            .def("GetTotalMagicalAttack", &GlobalActor::GetTotalMagicalAttack)
            .def("GetAverageDefense", &GlobalActor::GetAverageDefense)
            .def("GetAverageMagicalDefense", &GlobalActor::GetAverageMagicalDefense)
            .def("GetAverageEvadeRating", &GlobalActor::GetAverageEvadeRating)
            .def("GetAttackPoint", &GlobalActor::GetAttackPoint)

            .def("SetHitPoints", &GlobalActor::SetHitPoints)
            .def("SetSkillPoints", &GlobalActor::SetSkillPoints)
            .def("SetMaxHitPoints", &GlobalActor::SetMaxHitPoints)
            .def("SetMaxSkillPoints", &GlobalActor::SetMaxSkillPoints)
            .def("SetPhysAtk", &GlobalActor::SetPhysAtk)
            .def("SetMagAtk", &GlobalActor::SetMagAtk)
            .def("SetPhysDef", &GlobalActor::SetPhysDef)
            .def("SetMagDef", &GlobalActor::SetMagDef)
            .def("SetStamina", &GlobalActor::SetStamina)
            .def("SetEvade", &GlobalActor::SetEvade)
            .def("SetPhysAtkModifier", &GlobalActor::SetPhysAtkModifier)
            .def("SetMagAtkModifier", &GlobalActor::SetMagAtkModifier)
            .def("SetPhysDefModifier", &GlobalActor::SetPhysDefModifier)
            .def("SetMagDefModifier", &GlobalActor::SetMagDefModifier)
            .def("SetStaminaModifier", &GlobalActor::SetStaminaModifier)
            .def("SetEvadeModifier", &GlobalActor::SetEvadeModifier)
            .def("SetElementalModifier", &GlobalActor::SetElementalModifier)

            .def("AddHitPoints", &GlobalActor::AddHitPoints)
            .def("SubtractHitPoints", &GlobalActor::SubtractHitPoints)
            .def("AddMaxHitPoints", &GlobalActor::AddMaxHitPoints)
            .def("SubtractMaxHitPoints", &GlobalActor::SubtractMaxHitPoints)
            .def("AddSkillPoints", &GlobalActor::AddSkillPoints)
            .def("SubtractSkillPoints", &GlobalActor::SubtractSkillPoints)
            .def("AddMaxSkillPoints", &GlobalActor::AddMaxSkillPoints)
            .def("SubtractMaxSkillPoints", &GlobalActor::SubtractMaxSkillPoints)
            .def("AddPhysAtk", &GlobalActor::AddPhysAtk)
            .def("SubtractPhysAtk", &GlobalActor::SubtractPhysAtk)
            .def("AddMagAtk", &GlobalActor::AddMagAtk)
            .def("SubtractMagAtk", &GlobalActor::SubtractMagAtk)
            .def("AddPhysDef", &GlobalActor::AddPhysDef)
            .def("SubtractPhysDef", &GlobalActor::SubtractPhysDef)
            .def("AddMagDef", &GlobalActor::AddMagDef)
            .def("SubtractMagDef", &GlobalActor::SubtractMagDef)
            .def("AddStamina", &GlobalActor::AddStamina)
            .def("SubtractStamina", &GlobalActor::SubtractStamina)
            .def("AddEvade", &GlobalActor::AddEvade)
            .def("SubtractEvade", &GlobalActor::SubtractEvade)

            .def("IsAlive", &GlobalActor::IsAlive)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<GlobalCharacter, GlobalActor>("GlobalCharacter")
            .def("GetExperienceLevel", &GlobalCharacter::GetExperienceLevel)
            .def("SetExperienceLevel", &GlobalCharacter::SetExperienceLevel)
            .def("AddExperienceForNextLevel", &GlobalCharacter::AddExperienceForNextLevel)
            .def("AddSkill", (bool(GlobalCharacter:: *)(uint32_t)) &GlobalCharacter::AddSkill)
            .def("ApplyActiveStatusEffect", &GlobalCharacter::ApplyActiveStatusEffect)
            .def("RemoveActiveStatusEffect", &GlobalCharacter::RemoveActiveStatusEffect)
            .def("GetActiveStatusEffectIntensity", &GlobalCharacter::GetActiveStatusEffectIntensity)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<GlobalEnemy, GlobalActor>("GlobalEnemy")
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<InventoryHandler>("InventoryHandler")
            .def("AddToInventory", (void (InventoryHandler:: *)(uint32_t, uint32_t)) &InventoryHandler::AddToInventory)
            .def("RemoveFromInventory", (void (InventoryHandler:: *)(uint32_t)) &InventoryHandler::RemoveFromInventory)
            .def("IsItemInInventory", &InventoryHandler::IsItemInInventory)
            .def("IncrementItemCount", &InventoryHandler::IncrementItemCount)
            .def("DecrementItemCount", &InventoryHandler::DecrementItemCount)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<GlobalObject>("GlobalObject")
            .def("GetID", &GlobalObject::GetID)
            .def("GetName", &GlobalObject::GetName)
            .def("GetType", &GlobalObject::GetObjectType)
            .def("GetCount", &GlobalObject::GetCount)
            .def("IncrementCount", &GlobalObject::IncrementCount)
            .def("DecrementCount", &GlobalObject::DecrementCount)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<GlobalItem, GlobalObject>("GlobalItem")
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<GlobalWeapon, GlobalObject>("GlobalWeapon")
            .def("GetUsableBy", &GlobalWeapon::GetUsableBy)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<GlobalArmor, GlobalObject>("GlobalArmor")
            .def("GetUsableBy", &GlobalArmor::GetUsableBy)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<GlobalStatusEffect>("GlobalStatusEffect")
            .def("GetType", &GlobalStatusEffect::GetType)
            .def("GetIntensity", &GlobalStatusEffect::GetIntensity)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<GlobalSkill>("GlobalSkill")
            .def("GetID", &GlobalSkill::GetID)
            .def("GetSPRequired", &GlobalSkill::GetSPRequired)
            .def("ExecuteBattleFunction", &GlobalSkill::ExecuteBattleFunction)
        ];

        luabind::module(vt_script::ScriptManager->GetGlobalState(), "vt_global")
        [
            luabind::class_<BattleMedia>("BattleMedia")
            .def("SetBackgroundImage", &BattleMedia::SetBackgroundImage)
            .def("SetBattleMusic", &BattleMedia::SetBattleMusic)
        ];

    } // End using global namespaces

    // Bind the GlobalManager object to Lua
    luabind::object global_table = luabind::globals(vt_script::ScriptManager->GetGlobalState());
    global_table["GlobalManager"] = vt_global::GlobalManager;
} // void BindCommonCode()

} // namespace vt_defs
