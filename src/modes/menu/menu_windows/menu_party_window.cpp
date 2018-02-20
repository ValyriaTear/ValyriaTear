///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_windows/menu_party_window.h"

#include "modes/menu/menu_mode.h"

#include "common/global/objects/global_weapon.h"
#include "common/global/objects/global_armor.h"

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

static ustring change_position_message;
static ustring change_formation_message;

PartyWindow::PartyWindow() :
    _char_select_active(FORM_ACTIVE_NONE),
    _focused_def_icon(nullptr),
    _focused_mdef_icon(nullptr)
{
    // Get party size for iteration
    uint32_t partysize = GlobalManager->GetActiveParty()->GetPartySize();
    StillImage portrait;

    // Set up the full body portrait
    for(uint32_t i = 0; i < partysize; i++) {
        GlobalCharacter* ch =
            GlobalManager->GetActiveParty()->GetCharacterAtIndex(i);
        _full_portraits.push_back(ch->GetFullPortrait());
    }

    // We set them here in case the language has changed since the game start
    change_position_message =
        UTranslate("Select a character to change position with.");
    change_formation_message =
        UTranslate("Select a character to change formation.");

    // Init char select option box
    _InitCharSelect();
}

void PartyWindow::Activate(bool new_value)
{
    if(new_value) {
        _char_select_active = FORM_ACTIVE_CHAR;
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    } else {
        _char_select_active = FORM_ACTIVE_NONE;
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _second_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    }
}

void PartyWindow::_InitCharSelect()
{
    //character selection set up
    std::vector<ustring> options;
    uint32_t size = GlobalManager->GetActiveParty()->GetPartySize();

    _char_select.SetPosition(72.0f, 109.0f);
    _char_select.SetDimensions(360.0f, 432.0f, 1, 4, 1, 4);
    _char_select.SetCursorOffset(-50.0f, -6.0f);
    _char_select.SetTextStyle(TextStyle("text20"));
    _char_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _char_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _char_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

    _second_char_select.SetPosition(72.0f, 109.0f);
    _second_char_select.SetDimensions(360.0f, 432.0f, 1, 4, 1, 4);
    _second_char_select.SetCursorOffset(-50.0f, -6.0f);
    _second_char_select.SetTextStyle(TextStyle("text20"));
    _second_char_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _second_char_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _second_char_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

    // Use blank string so cursor can point somewhere
    for(uint32_t i = 0; i < size; i++) {
        options.push_back(MakeUnicodeString(" "));
    }

    _char_select.SetOptions(options);
    _char_select.SetSelection(0);
    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _second_char_select.SetOptions(options);
    _second_char_select.SetSelection(0);
    _second_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _help_text.SetStyle(TextStyle("text18"));

    _character_status_text.SetStyle(TextStyle("text20"));
    _character_status_numbers.SetStyle(TextStyle("text20"));
    _character_status_icons.Load("data/gui/menus/menu_stats_icons.png");

    _average_text.SetStyle(TextStyle("text20"));
    _average_text.SetText(Translate("Global Attack / Defense:"));
    _focused_text.SetStyle(TextStyle("text20"));
    _focused_text.SetText(Translate("Detailed Defense:"));

    _average_atk_def_text.SetStyle(TextStyle("text18"));
    vt_utils::ustring text = UTranslate("Total Attack: ") + MakeUnicodeString("\n")
        + UTranslate("Total Magical Attack: ") + MakeUnicodeString("\n\n")
        + UTranslate("Average Defense: ") + MakeUnicodeString("\n")
        + UTranslate("Average Magical Defense: ");
    _average_atk_def_text.SetText(text);
    _average_atk_def_numbers.SetStyle(TextStyle("text18"));
    _average_atk_def_icons.Load("data/gui/menus/menu_avg_atk_def.png");

    _focused_def_text.SetStyle(TextStyle("text18"));
    text = MakeUnicodeString("\n") // Skip title
        + UTranslate("Head: ") + MakeUnicodeString("\n")
        + UTranslate("Torso: ") + MakeUnicodeString("\n")
        + UTranslate("Arm: ") + MakeUnicodeString("\n")
        + UTranslate("Leg: ");
    _focused_def_text.SetText(text);
    _focused_def_numbers.SetStyle(TextStyle("text18"));
    _focused_mdef_numbers.SetStyle(TextStyle("text18"));

    _focused_def_category_icons.Load("data/gui/menus/menu_point_atk_def.png");
    _focused_def_icon =
        GlobalManager->Media().GetStatusIcon(GLOBAL_STATUS_PHYS_DEF,
                                             GLOBAL_INTENSITY_NEUTRAL);
    _focused_mdef_icon =
        GlobalManager->Media().GetStatusIcon(GLOBAL_STATUS_MAG_DEF,
                                             GLOBAL_INTENSITY_NEUTRAL);

    UpdateStatus();
}

void PartyWindow::Update()
{
    GlobalMedia& media = GlobalManager->Media();

    // Points to the active option box
    OptionBox *active_option = nullptr;
    //choose correct menu
    switch(_char_select_active) {
    case FORM_ACTIVE_CHAR:
        active_option = &_char_select;
        _help_text.SetText(change_formation_message);
        break;
    case FORM_ACTIVE_SECOND:
        active_option = &_second_char_select;
        _help_text.SetText(change_position_message);
        break;
    }

    // Handle the appropriate input events
    if(InputManager->ConfirmPress()) {
        active_option->InputConfirm();
    } else if(InputManager->CancelPress()) {
        active_option->InputCancel();
    } else if(InputManager->LeftPress()) {
        media.PlaySound("bump");
        active_option->InputLeft();
    } else if(InputManager->RightPress()) {
        media.PlaySound("bump");
        active_option->InputRight();
    } else if(InputManager->UpPress()) {
        media.PlaySound("bump");
        active_option->InputUp();
    } else if(InputManager->DownPress()) {
        media.PlaySound("bump");
        active_option->InputDown();
    }

    uint32_t event = active_option->GetEvent();
    active_option->Update();

    switch(_char_select_active) {
    case FORM_ACTIVE_CHAR:
        if(event == VIDEO_OPTION_CONFIRM) {
            _char_select_active = FORM_ACTIVE_SECOND;
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_DARKEN);
            _second_char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            media.PlaySound("confirm");
        } else if(event == VIDEO_OPTION_CANCEL) {
            Activate(false);
            media.PlaySound("cancel");
        }
        break;

    case FORM_ACTIVE_SECOND:
        if(event == VIDEO_OPTION_CONFIRM) {
            // Switch Characters
            GlobalManager->SwapCharactersByIndex(_char_select.GetSelection(),
                                                 _second_char_select.GetSelection());
            std::swap(_full_portraits[_char_select.GetSelection()],
                      _full_portraits[_second_char_select.GetSelection()]);

            // Update the character's view
            MenuMode::CurrentInstance()->ReloadCharacterWindows();

            _char_select_active = FORM_ACTIVE_CHAR;
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _second_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            media.PlaySound("confirm");
        } else if(event == VIDEO_OPTION_CANCEL) {
            _char_select_active = FORM_ACTIVE_CHAR;
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _second_char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            media.PlaySound("cancel");
        }
        break;
    } // switch
    _char_select.Update();

    // update the status text
    if (InputManager->AnyRegisteredKeyPress())
        UpdateStatus();
}

void PartyWindow::UpdateStatus()
{
    _character_status_numbers.Clear();
    _average_atk_def_numbers.Clear();
    _focused_def_numbers.Clear();
    _focused_mdef_numbers.Clear();

    GlobalCharacter *ch =
        GlobalManager->GetActiveParty()->GetCharacterAtIndex(_char_select.GetSelection());
    if (!ch)
        return;

    vt_utils::ustring text;
    text = UTranslate("Experience Level: ") + MakeUnicodeString(NumberToString(ch->GetExperienceLevel()))
        + MakeUnicodeString("\n\n\n")
        + UTranslate("Physical Attack: ") + MakeUnicodeString("\n\n")
        + UTranslate("Magical Attack: ") + MakeUnicodeString("\n\n")
        + UTranslate("Physical Defense: ") + MakeUnicodeString("\n\n")
        + UTranslate("Magical Defense: ") + MakeUnicodeString("\n\n")
        + UTranslate("Stamina: ") + MakeUnicodeString("\n\n")
        + UTranslate("Evade: ");

    _character_status_text.SetText(text);

    auto newline2 = MakeUnicodeString("\n\n");
    text = MakeUnicodeString("\n\n\n")
        + MakeUnicodeString(NumberToString(ch->GetPhysAtk())) + newline2
        + MakeUnicodeString(NumberToString(ch->GetMagAtk()))  + newline2
        + MakeUnicodeString(NumberToString(ch->GetPhysDef())) + newline2
        + MakeUnicodeString(NumberToString(ch->GetMagDef()))  + newline2
        + MakeUnicodeString(NumberToString(ch->GetStamina())) + newline2
        + MakeUnicodeString(NumberToString(ch->GetEvade())) + MakeUnicodeString("%");

    _character_status_numbers.SetText(text);

    text = MakeUnicodeString(NumberToString(ch->GetTotalPhysicalAttack()))
        + MakeUnicodeString("\n")
        + MakeUnicodeString(NumberToString(ch->GetTotalMagicalAttack(GLOBAL_ELEMENTAL_NEUTRAL)))
        + MakeUnicodeString("\n\n")
        + MakeUnicodeString(NumberToString(ch->GetAverageDefense()))
        + MakeUnicodeString("\n")
        + MakeUnicodeString(NumberToString(ch->GetAverageMagicalDefense(GLOBAL_ELEMENTAL_NEUTRAL)));

    _average_atk_def_numbers.SetText(text);

    _weapon_icon.Clear();
    std::shared_ptr<GlobalWeapon> weapon = ch->GetWeaponEquipped();
    if (weapon)
        _weapon_icon.Load(weapon->GetIconImage().GetFilename());
    else
        _weapon_icon.Load("data/inventory/weapons/fist-human.png");
    _weapon_icon.SetHeightKeepRatio(40);

    std::shared_ptr<GlobalArmor> head_armor = ch->GetHeadArmorEquipped();
    _focused_def_armor_icons[0].Clear();
    if (head_armor) {
        _focused_def_armor_icons[0].Load(head_armor->GetIconImage().GetFilename());
        _focused_def_armor_icons[0].SetHeightKeepRatio(20);
    }

    _focused_def_armor_icons[1].Clear();
    std::shared_ptr<GlobalArmor> torso_armor = ch->GetTorsoArmorEquipped();
    if (torso_armor) {
        _focused_def_armor_icons[1].Load(torso_armor->GetIconImage().GetFilename());
        _focused_def_armor_icons[1].SetHeightKeepRatio(20);
    }

    _focused_def_armor_icons[2].Clear();
    std::shared_ptr<GlobalArmor> arm_armor = ch->GetArmArmorEquipped();
    if (arm_armor) {
        _focused_def_armor_icons[2].Load(arm_armor->GetIconImage().GetFilename());
        _focused_def_armor_icons[2].SetHeightKeepRatio(20);
    }

    _focused_def_armor_icons[3].Clear();
    std::shared_ptr<GlobalArmor> leg_armor = ch->GetLegArmorEquipped();
    if (leg_armor) {
        _focused_def_armor_icons[3].Load(leg_armor->GetIconImage().GetFilename());
        _focused_def_armor_icons[3].SetHeightKeepRatio(20);
    }

    text = MakeUnicodeString("\n") // Skip titles
        + MakeUnicodeString(NumberToString(ch->GetPhysDef() + (head_armor ? head_armor->GetPhysicalDefense() : 0)) + "\n")
        + MakeUnicodeString(NumberToString(ch->GetPhysDef() + (torso_armor ? torso_armor->GetPhysicalDefense() : 0)) + "\n")
        + MakeUnicodeString(NumberToString(ch->GetPhysDef() + (arm_armor ? arm_armor->GetPhysicalDefense() : 0)) + "\n")
        + MakeUnicodeString(NumberToString(ch->GetPhysDef() + (leg_armor ? leg_armor->GetPhysicalDefense() : 0)) + "\n");

    _focused_def_numbers.SetText(text);

    text = MakeUnicodeString("\n") // Skip titles
        + MakeUnicodeString(NumberToString(ch->GetMagDef() + (head_armor ? head_armor->GetMagicalDefense() : 0)) + "\n")
        + MakeUnicodeString(NumberToString(ch->GetMagDef() + (torso_armor ? torso_armor->GetMagicalDefense() : 0)) + "\n")
        + MakeUnicodeString(NumberToString(ch->GetMagDef() + (arm_armor ? arm_armor->GetMagicalDefense() : 0)) + "\n")
        + MakeUnicodeString(NumberToString(ch->GetMagDef() + (leg_armor ? leg_armor->GetMagicalDefense() : 0)) + "\n");

    _focused_mdef_numbers.SetText(text);
}

void PartyWindow::_DrawBottomEquipmentInfo()
{
    VideoManager->Move(110.0f, 560.0f);
    _average_text.Draw();
    VideoManager->MoveRelative(-10.0f, 25.0f);
    _average_atk_def_text.Draw();
    VideoManager->MoveRelative(230.0f, 3.0f);
    _average_atk_def_icons.Draw();
    VideoManager->MoveRelative(25.0f, -3.0f);
    _average_atk_def_numbers.Draw();

    VideoManager->MoveRelative(40.0f, 0.0f);
    _weapon_icon.Draw();

    VideoManager->MoveRelative(125.0f, -25.0f);
    _focused_text.Draw();
    VideoManager->MoveRelative(-10.0f, 25.0f);
    _focused_def_text.Draw();
    VideoManager->MoveRelative(80.0f, 20.0f);
    _focused_def_category_icons.Draw();
    VideoManager->MoveRelative(25.0f, 0.0f);
    for (uint32_t i = 0; i < 4; ++i) {
         _focused_def_armor_icons[i].Draw();
         VideoManager->MoveRelative(0.0f, 20.0f);
    }
    VideoManager->MoveRelative(60.0f, -100.0f);
    _focused_def_icon->Draw();
    VideoManager->MoveRelative(5.0f, 5.0f);
    _focused_def_numbers.Draw();

    VideoManager->MoveRelative(60.0f, -5.0f);
    _focused_mdef_icon->Draw();
    VideoManager->MoveRelative(5.0f, 5.0f);
    _focused_mdef_numbers.Draw();
}

void PartyWindow::Draw()
{
    MenuWindow::Draw();
    _char_select.Draw();
    _second_char_select.Draw();

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);

    // Draw character full body portrait
    VideoManager->Move(440.0f, 130.0f);
    _full_portraits[_char_select.GetSelection()].Draw();

    const float status_x = _position.x + _width - 48.0f
                           - _character_status_text.GetWidth()
                           - _character_status_numbers.GetWidth()
                           - _character_status_icons.GetWidth();
    VideoManager->Move(status_x, 140.0f);
    _character_status_text.Draw();
    VideoManager->MoveRelative(_character_status_text.GetWidth() + 8.0f, 67.0f);
    _character_status_icons.Draw();
    VideoManager->MoveRelative(_character_status_icons.GetWidth() + 8.0f, -67.0f);
    _character_status_numbers.Draw();

    if (GetActiveState() != FORM_ACTIVE_NONE) {
        VideoManager->Move(450.0f, 500.0f);
        _help_text.Draw();
        // Draw equipment info
        _DrawBottomEquipmentInfo();
    }
}

} // namespace private_menu

} // namespace vt_menu
