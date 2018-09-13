////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "command_settings.h"

#include "modes/battle/objects/battle_character.h"

#include "engine/video/video.h"

#include "common/global/global.h"
#include "common/global/battle_media.h"
#include "common/global/actors/global_character.h"

using namespace vt_global;
using namespace vt_gui;
using namespace vt_utils;
using namespace vt_video;

namespace vt_battle
{

namespace private_battle
{

const float LIST_POSITION_X = 140.0f;
const float LIST_POSITION_Y = 15.0f;
const float LIST_SIZE_X = 350.0f;
const float LIST_SIZE_Y = 100.0f;

// Offset used to properly align the target icons in the skill and item selection lists
const uint32_t TARGET_ICON_OFFSET = 288;

CommandSettings::CommandSettings(BattleCharacter* character, MenuWindow& window) :
    _character(character),
    _last_category(CATEGORY_WEAPON),
    _last_item(0),
    _last_self_target(BattleTarget()),
    _last_character_target(BattleTarget()),
    _last_enemy_target(BattleTarget())
{
    _weapon_skill_list.SetOwner(&window);
    _weapon_skill_list.SetPosition(LIST_POSITION_X, LIST_POSITION_Y);
    _weapon_skill_list.SetDimensions(LIST_SIZE_X, LIST_SIZE_Y, 1, 255, 1, 4);
    _weapon_skill_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _weapon_skill_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _weapon_skill_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _weapon_skill_list.SetTextStyle(TextStyle("text20"));
    _weapon_skill_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _weapon_skill_list.SetCursorOffset(-50.0f, -25.0f);
    _weapon_skill_list.AnimateScrolling(false);

    _weapon_target_list.SetOwner(&window);
    _weapon_target_list.SetPosition(LIST_POSITION_X + TARGET_ICON_OFFSET, LIST_POSITION_Y);
    _weapon_target_list.SetDimensions(LIST_SIZE_X - TARGET_ICON_OFFSET, LIST_SIZE_Y, 1, 255, 1, 4);
    _weapon_target_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _weapon_target_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _weapon_target_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _weapon_target_list.SetTextStyle(TextStyle("text20"));
    _weapon_target_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _weapon_target_list.AnimateScrolling(false);

    _magic_skill_list.SetOwner(&window);
    _magic_skill_list.SetPosition(LIST_POSITION_X, LIST_POSITION_Y);
    _magic_skill_list.SetDimensions(LIST_SIZE_X, LIST_SIZE_Y, 1, 255, 1, 4);
    _magic_skill_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _magic_skill_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _magic_skill_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _magic_skill_list.SetTextStyle(TextStyle("text20"));
    _magic_skill_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _magic_skill_list.SetCursorOffset(-50.0f, -25.0f);
    _magic_skill_list.AnimateScrolling(false);

    _magic_target_list.SetOwner(&window);
    _magic_target_list.SetPosition(LIST_POSITION_X + TARGET_ICON_OFFSET, LIST_POSITION_Y);
    _magic_target_list.SetDimensions(LIST_SIZE_X - TARGET_ICON_OFFSET, LIST_SIZE_Y, 1, 255, 1, 4);
    _magic_target_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _magic_target_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _magic_target_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _magic_target_list.SetTextStyle(TextStyle("text20"));
    _magic_target_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _magic_target_list.AnimateScrolling(false);

    _special_skill_list.SetOwner(&window);
    _special_skill_list.SetPosition(LIST_POSITION_X, LIST_POSITION_Y);
    _special_skill_list.SetDimensions(LIST_SIZE_X, LIST_SIZE_Y, 1, 255, 1, 4);
    _special_skill_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _special_skill_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _special_skill_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _special_skill_list.SetTextStyle(TextStyle("text20"));
    _special_skill_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _special_skill_list.SetCursorOffset(-50.0f, -25.0f);
    _special_skill_list.AnimateScrolling(false);

    _special_target_list.SetOwner(&window);
    _special_target_list.SetPosition(LIST_POSITION_X + TARGET_ICON_OFFSET, LIST_POSITION_Y);
    _special_target_list.SetDimensions(LIST_SIZE_X - TARGET_ICON_OFFSET, LIST_SIZE_Y, 1, 255, 1, 4);
    _special_target_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _special_target_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _special_target_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _special_target_list.SetTextStyle(TextStyle("text20"));
    _special_target_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _special_target_list.AnimateScrolling(false);

    if(_character == nullptr) {
        PRINT_WARNING << "constructor received nullptr character pointer" << std::endl;
        return;
    }

    BattleMedia& battle_media = GlobalManager->GetBattleMedia();

    // Construct the weapon, magic, and special skill lists for the character
    std::vector<GlobalSkill *>* skill_list = nullptr;
    std::shared_ptr<GlobalWeapon> char_wpn = _character->GetGlobalCharacter()->GetWeaponEquipped();

    if (char_wpn)
        skill_list = _character->GetGlobalCharacter()->GetWeaponSkills();
    else
        skill_list = _character->GetGlobalCharacter()->GetBareHandsSkills();
    for(uint32_t i = 0; i < skill_list->size(); i++) {
        _weapon_skill_list.AddOption(ustring());
        if (!skill_list->at(i)->GetIconFilename().empty()) {
            _weapon_skill_list.AddOptionElementImage(i, skill_list->at(i)->GetIconFilename());
            _weapon_skill_list.GetEmbeddedImage(i)->SetHeightKeepRatio(25);
            _weapon_skill_list.AddOptionElementPosition(i, 30);
        }
        else {
            // Check for the weapon icon
            std::string wpn_icon_filename;
            if (char_wpn)
                wpn_icon_filename = char_wpn->GetIconImage().GetFilename();
            else
                wpn_icon_filename = "data/inventory/weapons/fist-human.png";

            if (!wpn_icon_filename.empty()) {
                _weapon_skill_list.AddOptionElementImage(i, wpn_icon_filename);
                _weapon_skill_list.GetEmbeddedImage(i)->SetHeightKeepRatio(25);
                _weapon_skill_list.AddOptionElementPosition(i, 30);
            }
        }
        _weapon_skill_list.AddOptionElementText(i, skill_list->at(i)->GetName());

        _weapon_target_list.AddOption(ustring());
        _weapon_target_list.AddOptionElementImage(i, battle_media.GetTargetTypeIcon(skill_list->at(i)->GetTargetType()));
        _weapon_target_list.AddOptionElementPosition(i, 45);
        _weapon_target_list.AddOptionElementText(i, MakeUnicodeString(NumberToString(skill_list->at(i)->GetSPRequired())));
        if(skill_list->at(i)->GetSPRequired() > _character->GetGlobalCharacter()->GetSkillPoints()) {
            _weapon_skill_list.EnableOption(i, false);
            _weapon_target_list.EnableOption(i, false);
        }
    }
    if(!skill_list->empty()) {
        _weapon_skill_list.SetSelection(0);
        _weapon_target_list.SetSelection(0);
    }

    skill_list = _character->GetGlobalCharacter()->GetMagicSkills();
    for(uint32_t i = 0; i < skill_list->size(); i++) {
        _magic_skill_list.AddOption(ustring());
        if (!skill_list->at(i)->GetIconFilename().empty()) {
            _magic_skill_list.AddOptionElementImage(i, skill_list->at(i)->GetIconFilename());
            _magic_skill_list.GetEmbeddedImage(i)->SetHeightKeepRatio(25);
            _magic_skill_list.AddOptionElementPosition(i, 30);
        }
        _magic_skill_list.AddOptionElementText(i, skill_list->at(i)->GetName());

        _magic_target_list.AddOption(ustring());
        _magic_target_list.AddOptionElementImage(i, battle_media.GetTargetTypeIcon(skill_list->at(i)->GetTargetType()));
        _magic_target_list.AddOptionElementPosition(i, 45);
        _magic_target_list.AddOptionElementText(i, MakeUnicodeString(NumberToString(skill_list->at(i)->GetSPRequired())));
        if(skill_list->at(i)->GetSPRequired() > _character->GetGlobalCharacter()->GetSkillPoints()) {
            _magic_skill_list.EnableOption(i, false);
            _magic_target_list.EnableOption(i, false);
        }
    }
    if(!skill_list->empty()) {
        _magic_skill_list.SetSelection(0);
        _magic_target_list.SetSelection(0);
    }

    skill_list = _character->GetGlobalCharacter()->GetSpecialSkills();
    for(uint32_t i = 0; i < skill_list->size(); i++) {
        _special_skill_list.AddOption(ustring());
        if (!skill_list->at(i)->GetIconFilename().empty()) {
            _special_skill_list.AddOptionElementImage(i, skill_list->at(i)->GetIconFilename());
            _special_skill_list.GetEmbeddedImage(i)->SetHeightKeepRatio(25);
            _special_skill_list.AddOptionElementPosition(i, 30);
        }
        _special_skill_list.AddOptionElementText(i, skill_list->at(i)->GetName());

        _special_target_list.AddOption(ustring());
        _special_target_list.AddOptionElementImage(i, battle_media.GetTargetTypeIcon(skill_list->at(i)->GetTargetType()));
        _special_target_list.AddOptionElementPosition(i, 45);
        _special_target_list.AddOptionElementText(i, MakeUnicodeString(NumberToString(skill_list->at(i)->GetSPRequired())));
        if(skill_list->at(i)->GetSPRequired() > _character->GetGlobalCharacter()->GetSkillPoints()) {
            _special_skill_list.EnableOption(i, false);
            _special_target_list.EnableOption(i, false);
        }
    }
    if(!skill_list->empty()) {
        _special_skill_list.SetSelection(0);
        _special_target_list.SetSelection(0);
    }
}

void CommandSettings::RefreshLists()
{
    uint32_t require_sp = 0xFFFFFFFF;
    uint32_t current_sp = _character->GetSkillPoints();
    std::vector<GlobalSkill *>* skill_list = nullptr;

    skill_list = _character->GetGlobalCharacter()->GetWeaponSkills();
    for(uint32_t i = 0; i < skill_list->size(); ++i) {
        require_sp = skill_list->at(i)->GetSPRequired();
        if(require_sp > current_sp) {
            _weapon_skill_list.EnableOption(i, false);
            _weapon_target_list.EnableOption(i, false);
        }
        else {
            _weapon_skill_list.EnableOption(i, true);
            _weapon_target_list.EnableOption(i, true);
        }
    }

    skill_list = _character->GetGlobalCharacter()->GetMagicSkills();
    for(uint32_t i = 0; i < skill_list->size(); ++i) {
        require_sp = skill_list->at(i)->GetSPRequired();
        if(require_sp > current_sp) {
            _magic_skill_list.EnableOption(i, false);
            _magic_target_list.EnableOption(i, false);
        }
        else {
            _magic_skill_list.EnableOption(i, true);
            _magic_target_list.EnableOption(i, true);
        }
    }

    skill_list = _character->GetGlobalCharacter()->GetSpecialSkills();
    for(uint32_t i = 0; i < skill_list->size(); ++i) {
        require_sp = skill_list->at(i)->GetSPRequired();
        if(require_sp > current_sp) {
            _special_skill_list.EnableOption(i, false);
            _special_target_list.EnableOption(i, false);
        }
        else {
            _special_skill_list.EnableOption(i, true);
            _special_target_list.EnableOption(i, true);
        }
    }
}

void CommandSettings::SaveLastTarget(BattleTarget &target)
{
    switch(target.GetType()) {
    case GLOBAL_TARGET_SELF_POINT:
    case GLOBAL_TARGET_SELF:
        _last_self_target = target;
        break;
    case GLOBAL_TARGET_ALLY_POINT:
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
    case GLOBAL_TARGET_DEAD_ALLY_ONLY:
        _last_character_target = target;
        break;
    case GLOBAL_TARGET_FOE_POINT:
    case GLOBAL_TARGET_FOE:
        _last_enemy_target = target;
        break;
    case GLOBAL_TARGET_ALL_ALLIES:
    case GLOBAL_TARGET_ALL_FOES:
        break; // Party type targets are not retained
    default:
        PRINT_WARNING << "target argument was an invalid type: " << target.GetType() << std::endl;
        break;
    }
}

} // namespace private_battle

} // namespace vt_battle
