///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2019 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "character_window.h"

#include "common/global/global.h"
#include "common/global/actors/global_character.h"

#include "engine/video/video.h"
#include "utils/ustring.h"

using namespace vt_global;
using namespace vt_video;
using namespace vt_utils;
using namespace vt_system;

namespace vt_common
{

// HP / SP bars
const float BAR_BASE_SIZE_X = 100.0f;
const float BAR_BASE_SIZE_Y = 5.0f;

CharacterWindow::CharacterWindow():
    _char_id(vt_global::GLOBAL_CHARACTER_INVALID),
    _hp_ratio(0.0f),
    _sp_ratio(0.0f)
{
    // Loads HP/SP icons
    vt_global::GlobalMedia& media = vt_global::GlobalManager->Media();
    _hp_icon = media.GetStatusIcon(vt_global::GLOBAL_STATUS_HP,
                                   vt_global::GLOBAL_INTENSITY_NEUTRAL);
    _sp_icon = media.GetStatusIcon(vt_global::GLOBAL_STATUS_SP,
                                   vt_global::GLOBAL_INTENSITY_NEUTRAL);
}

void CharacterWindow::SetCharacter(vt_global::GlobalCharacter* character)
{
    if(!character || character->GetID() == vt_global::GLOBAL_CHARACTER_INVALID) {
        _character_name.Clear();
        _character_data.Clear();
        _portrait = StillImage();
        _char_id = vt_global::GLOBAL_CHARACTER_INVALID;
        _hp_ratio = 0.0f;
        _sp_ratio = 0.0f;
        _UpdateActiveStatusEffects(nullptr);
        return;
    }

    _char_id = character->GetID();

    _portrait = character->GetPortrait();
    // Only size up valid portraits
    if(!_portrait.GetFilename().empty())
        _portrait.SetDimensions(100.0f, 100.0f);

    // the characters' name is already translated.
    _character_name.SetText(character->GetName(), TextStyle("title22"));

    // And the rest of the data
    /// tr: level
    ustring char_data = UTranslate("Lv: ") + MakeUnicodeString(NumberToString(character->GetExperienceLevel()) + " / ");
    char_data += UTranslate("Next: ") + MakeUnicodeString(NumberToString(character->GetExperienceForNextLevel()) + "\n");
    char_data += MakeUnicodeString("      " + NumberToString(character->GetHitPoints()) +
                 " / " + NumberToString(character->GetMaxHitPoints()) + "\n");
    char_data += MakeUnicodeString("      " + NumberToString(character->GetSkillPoints()) +
                 " / " + NumberToString(character->GetMaxSkillPoints()) + "\n");
    /// tr: experience points to be spent for skills
    char_data += UTranslate("XP for skills: ") + MakeUnicodeString(NumberToString(character->GetUnspentExperiencePoints()));

    _character_data.SetText(char_data, TextStyle("text20"));

    _hp_ratio = static_cast<float>(character->GetHitPoints()) / static_cast<float>(character->GetMaxHitPoints());
    _sp_ratio = static_cast<float>(character->GetSkillPoints()) / static_cast<float>(character->GetMaxSkillPoints());

    _UpdateActiveStatusEffects(character);
}

void CharacterWindow::Draw()
{
    // Call parent Draw method, if failed pass on fail result
    MenuWindow::Draw();

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);

    // Get the window metrics
    Position2D pos = GetPosition();
    // Adjust the current position to make it look better
    pos.y += 5;

    // Draw character portrait
    VideoManager->Move(pos.x + 12.0f, pos.y + 8.0f);
    _portrait.Draw();

    // Write character name
    VideoManager->MoveRelative(150.0f, -5.0f);
    _character_name.Draw();

    if (_hp_ratio > 0.0f) {
        VideoManager->MoveRelative(30.0f, 55.0f);
        // HP
        VideoManager->MoveRelative(0.0f, BAR_BASE_SIZE_Y - 2.0f);
        VideoManager->DrawRectangle(BAR_BASE_SIZE_X, 2.0f, Color::white);
        VideoManager->MoveRelative(0.0f, -BAR_BASE_SIZE_Y + 2.0f);
        if (_hp_ratio < 0.25f) {
            VideoManager->DrawRectangle(BAR_BASE_SIZE_X * _hp_ratio, BAR_BASE_SIZE_Y, Color::orange);
        }
        else {
            VideoManager->DrawRectangle(BAR_BASE_SIZE_X * _hp_ratio, BAR_BASE_SIZE_Y, Color::green_hp);
        }
        // SP
        VideoManager->MoveRelative(0.0f, 22.0f);
        VideoManager->MoveRelative(0.0f, BAR_BASE_SIZE_Y - 2.0f);
        VideoManager->DrawRectangle(BAR_BASE_SIZE_X, 2.0f, Color::white);
        VideoManager->MoveRelative(0.0f, -BAR_BASE_SIZE_Y + 2.0f);
        VideoManager->DrawRectangle(BAR_BASE_SIZE_X * _sp_ratio, BAR_BASE_SIZE_Y, Color::blue_sp);

        VideoManager->MoveRelative(-30.0f, -77.0f);
    }

    // Level, HP, SP, XP to Next Lvl
    VideoManager->MoveRelative(0.0f, 19.0f);
    _character_data.Draw();

    // HP/SP Icons
    if(_char_id != vt_global::GLOBAL_CHARACTER_INVALID) {
        VideoManager->MoveRelative(0.0f, 20.0f);
        _hp_icon->Draw();
        VideoManager->MoveRelative(0.0f, 20.0f);
        _sp_icon->Draw();
        VideoManager->MoveRelative(0.0f, -40.0f);
    }

    // Active status effects
    VideoManager->MoveRelative(-30.0f, -17.0f);
    uint32_t nb_effects = _active_status_effects.size();
    for (uint32_t i = 0; i < nb_effects && i < 6; ++i) {
        if (_active_status_effects[i])
            _active_status_effects[i]->Draw();
        VideoManager->MoveRelative(0.0f, 15.0f);
    }

    if (nb_effects < 6)
        return;

    // Show a second column when there are more than 6 active status effects
    VideoManager->MoveRelative(-15.0f, -6.0f * 15.0f);
    for (uint32_t i = 6; i < nb_effects && i < 12; ++i) {
        if (_active_status_effects[i])
            _active_status_effects[i]->Draw();
        VideoManager->MoveRelative(0.0f, 15.0f);
    }

    if (nb_effects < 12)
        return;

    // Show a third column when there are more than 12 active status effects
    // (maximum 15)
    VideoManager->MoveRelative(-15.0f, -6.0f * 15.0f);
    for (uint32_t i = 12; i < nb_effects; ++i) {
        if (_active_status_effects[i])
            _active_status_effects[i]->Draw();
        VideoManager->MoveRelative(0.0f, 15.0f);
    }
}

void CharacterWindow::_UpdateActiveStatusEffects(vt_global::GlobalCharacter* character)
{
    _active_status_effects.clear();
    if (!character)
        return;

    GlobalMedia& media = GlobalManager->Media();

    const std::vector<ActiveStatusEffect> effects =
        character->GetActiveStatusEffects();
    for (uint32_t i = 0; i < effects.size(); ++i) {
        GLOBAL_STATUS status = effects[i].GetEffect();
        GLOBAL_INTENSITY intensity = effects[i].GetIntensity();
        if (status != GLOBAL_STATUS_INVALID &&
                intensity != GLOBAL_INTENSITY_NEUTRAL) {
            StillImage* image = media.GetStatusIcon(status, intensity);
            if (image)
                _active_status_effects.push_back(image);
        }
    }
}

} // namespace vt_common
