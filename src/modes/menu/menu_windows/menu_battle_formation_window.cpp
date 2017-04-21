///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_windows/menu_battle_formation_window.h"

#include "modes/menu/menu_mode.h"

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/system.h"

using namespace vt_menu::private_menu;
using namespace vt_utils;
using namespace vt_audio;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_global;
using namespace vt_input;
using namespace vt_system;

namespace vt_menu
{

namespace private_menu
{

BattleFormationWindow::BattleFormationWindow() :
    _formation_select_active(false)
{
    std::vector<GlobalCharacter *>* characters = GlobalManager->GetOrderedCharacters();
    uint32_t characters_number = characters->size() > 4 ? 4 : characters->size();

    // Init the option box.
    _formation_select.SetPosition(272.0f, 170.0f);
    _formation_select.SetDimensions(360.0f, 330.0f,
                                    1, characters_number,
                                    1, characters_number);
    _formation_select.SetCursorOffset(-50.0f, -6.0f);
    _formation_select.SetTextStyle(TextStyle("text20"));
    _formation_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _formation_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _formation_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    std::vector<ustring> options;

    _characters_position.resize(characters_number);

    // Set up the characters animations
    for(uint32_t i = 0; i < characters_number; ++i) {
        GlobalCharacter* character = characters->at(i);
        if (!character)
            continue;

        options.push_back(UTranslate("Switch"));

        // Load a copy of the corresponding battle character sprite.
        AnimatedImage anim;
        if (character->IsAlive())
            anim = *character->RetrieveBattleAnimation("idle");
        else
            anim = *character->RetrieveBattleAnimation("dead");
        anim.SetDimensions(anim.GetWidth() * 0.7f, anim.GetHeight() * 0.7f);
        _character_sprites.push_back(anim);

        // TODO: Get position from Save/Global data.
        _characters_position[i] = true;
    }

    _formation_select.SetOptions(options);
    _formation_select.SetSelection(0);
    _formation_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _character_target_texts.resize(characters_number);

    // We set them here in case the language has changed since the game start
    _help_text.SetStyle(TextStyle("text20"));
    _help_text.SetText(UTranslate("Select a battle formation to change character placement."));

    _rear_front_txt.SetStyle(TextStyle("text24"));
    _rear_front_txt.SetText(UTranslate("Rear - Front"));
    _modifier_txt.SetStyle(TextStyle("text24"));
    _modifier_txt.SetText("Modifiers");

    UpdateStatus();
}

void BattleFormationWindow::Activate(bool activated)
{
    if(activated) {
        _formation_select_active = true;
        _formation_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    } else {
        _formation_select_active = false;
        _formation_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    }
}

void BattleFormationWindow::Update()
{
    GlobalMedia& media = GlobalManager->Media();

    // Handle the appropriate input events
    if(InputManager->ConfirmPress()) {
        _formation_select.InputConfirm();
        // Switch character position.
        int32_t selection = _formation_select.GetSelection();
        _characters_position[selection] = !_characters_position[selection];
        UpdateStatus();
    } else if(InputManager->CancelPress()) {
        _formation_select.InputCancel();
        Activate(false);
        media.PlaySound("cancel");
    } else if(InputManager->LeftPress()) {
        media.PlaySound("bump");
        _formation_select.InputLeft();
    } else if(InputManager->RightPress()) {
        media.PlaySound("bump");
        _formation_select.InputRight();
    } else if(InputManager->UpPress()) {
        media.PlaySound("bump");
        _formation_select.InputUp();
    } else if(InputManager->DownPress()) {
        media.PlaySound("bump");
        _formation_select.InputDown();
    }

    _formation_select.Update();

    // Update characters animations
    for (uint32_t i = 0; i < _character_sprites.size(); ++i) {
        _character_sprites[i].Update();
    }

    // Update the status texts
    if (InputManager->AnyRegisteredKeyPress())
        UpdateStatus();
}

void BattleFormationWindow::UpdateStatus()
{
    _ComputeModificators();
}

void BattleFormationWindow::_ComputeModificators()
{
    uint32_t characters_number = _character_sprites.size();
    if (characters_number == 0)
        return;
    uint32_t chance_to_target = 100 / characters_number;

    // Compute whether everyone is on front or rear.
    bool is_front = _characters_position.at(0);
    bool all_front = true;
    uint32_t number_in_front = 0;
    for(uint32_t i = 0; i < characters_number; ++i) {
        if (is_front != _characters_position[i])
            all_front = false;
        if (_characters_position[i])
            ++number_in_front;
    }

    std::string chance_to_target_txt;
    // If all characters are in front/in rear, then everyone has the same chances.
    if (all_front) {
        for(uint32_t i = 0; i < characters_number; ++i) {
            chance_to_target_txt = VTranslate("%d %%", chance_to_target);
            _character_target_texts[i].SetStyle(TextStyle("text22"));
            _character_target_texts[i].SetText(chance_to_target_txt);
        }
        return;
    }

    // Otherwise the number of characters in front determines the protection
    // of the characters in rear.
    uint32_t pos_chance_modifier = (chance_to_target / 2) / number_in_front;
    uint32_t neg_chance_modifier = (chance_to_target / 2) / (characters_number - number_in_front);
    for(uint32_t i = 0; i < characters_number; ++i) {
        if (_characters_position[i])
            chance_to_target_txt = VTranslate("%d %%", chance_to_target + pos_chance_modifier);
        else
            chance_to_target_txt = VTranslate("%d %%", chance_to_target - neg_chance_modifier);
        _character_target_texts[i].SetStyle(TextStyle("text22"));
        _character_target_texts[i].SetText(chance_to_target_txt);
    }
}

void BattleFormationWindow::_DrawBottomWindowInfo()
{
    if (!_formation_select_active)
        return;
    VideoManager->Move(110.0f, 560.0f);
    _help_text.Draw();
}

void BattleFormationWindow::Draw()
{
    MenuWindow::Draw();
    _formation_select.Draw();

    // Draw Info
    VideoManager->Move(380.0f, 150.0f);
    _rear_front_txt.Draw();
    VideoManager->MoveRelative(160.0f, 0.0f);
    _modifier_txt.Draw();

    // Draw characters depending on the formation chosen.
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
    VideoManager->Move(440.0f, 170.0f);
    for (uint32_t i = 0; i < _character_sprites.size(); ++i) {
        if (!_characters_position[i])
            VideoManager->MoveRelative(-80.0f, 0.0f);
        _character_sprites[i].Draw();
        if (!_characters_position[i])
            VideoManager->MoveRelative(80.0f, 0.0f);
        VideoManager->MoveRelative(100.0f, 20.0f);
        _character_target_texts[i].Draw();
        VideoManager->MoveRelative(-100.0f, -20.0f);
        VideoManager->MoveRelative(0.0f, 80.0f);
    }

    _DrawBottomWindowInfo();
}

} // namespace private_menu

} // namespace vt_menu
