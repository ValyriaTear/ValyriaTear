///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_windows/menu_skills_window.h"

#include "modes/menu/menu_mode.h"

#include "common/global/actors/global_character.h"
#include "common/global/objects/global_weapon.h"

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

//! \brief Menu translated strings
static ustring choose_character_message;
static ustring choose_skill_category_message;

SkillsWindow::SkillsWindow() :
    _active_box(SKILL_ACTIVE_NONE),
    _char_skillset(0)
{
    // Init option boxes
    _InitCharSelect();
    _InitSkillsList();
    _InitSkillsCategories();

    _description.SetOwner(this);
    _description.SetPosition(90.0f, 465.0f);
    _description.SetDimensions(740.0f, 80.0f);
    _description.SetDisplaySpeed(SystemManager->GetMessageSpeed());
    _description.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _description.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _description.SetTextStyle(TextStyle("text20"));

    // We set them here so that they are re-translated when changing the language.
    choose_character_message = UTranslate("Choose a character.");
    choose_skill_category_message = UTranslate("Choose a skill category to use.");

}

void SkillsWindow::Activate(bool is_active_state)
{
    // Activate window and first option box...or deactivate both
    if(is_active_state) {
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
        _active_box = SKILL_ACTIVE_CHAR;
    } else {
        _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        _active_box = SKILL_ACTIVE_NONE;
    }
}

void SkillsWindow::_InitSkillsList()
{
    // Set up the inventory option box
    _skills_list.SetPosition(500.0f, 170.0f);
    _skills_list.SetDimensions(180.0f, 360.0f, 1, 255, 1, 6);
    _skills_list.SetTextStyle(TextStyle("text20"));
    _skills_list.SetCursorOffset(-52.0f, -20.0f);
    _skills_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _skills_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _skills_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

    _UpdateSkillList();
    if(_skills_list.GetNumberOptions() > 0)
        _skills_list.SetSelection(0);
    _skills_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    // setup the cost option box
    _skill_cost_list.SetPosition(700.0f, 170.0f);
    _skill_cost_list.SetDimensions(180.0f, 360.0f, 1, 255, 1, 6);
    _skill_cost_list.SetTextStyle(TextStyle("text20"));
    _skill_cost_list.SetCursorOffset(-52.0f, -20.0f);
    _skill_cost_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _skill_cost_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _skill_cost_list.SetOptionAlignment(VIDEO_X_RIGHT, VIDEO_Y_CENTER);
    _skill_cost_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

void SkillsWindow::_InitCharSelect()
{
    //character selection set up
    std::vector<ustring> options;
    uint32_t size = GlobalManager->GetCharacterHandler().GetActiveParty().GetPartySize();

    _char_select.SetPosition(72.0f, 109.0f);
    _char_select.SetDimensions(360.0f, 432.0f, 1, 4, 1, 4);
    _char_select.SetCursorOffset(-50.0f, -6.0f);
    _char_select.SetTextStyle(TextStyle("text20"));
    _char_select.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _char_select.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _char_select.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);


    //Use blank strings....won't be seen anyway
    for(uint32_t i = 0; i < size; i++) {
        options.push_back(MakeUnicodeString(" "));
    }

    //Set options, selection and cursor state
    _char_select.SetOptions(options);
    _char_select.SetSelection(0);
    _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

void SkillsWindow::_InitSkillsCategories()
{
    _skills_categories.SetPosition(458.0f, 120.0f);
    _skills_categories.SetDimensions(448.0f, 30.0f,
                                     SKILL_CATEGORY_SIZE, 1,
                                     SKILL_CATEGORY_SIZE, 1);
    _skills_categories.SetTextStyle(TextStyle("text20"));
    _skills_categories.SetCursorOffset(-52.0f, -20.0f);
    _skills_categories.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _skills_categories.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _skills_categories.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);

    // Create options
    std::vector<ustring> options;
    options.push_back(UTranslate("All"));
    options.push_back(UTranslate("Field"));
    options.push_back(UTranslate("Battle"));

    // Set options and default selection
    _skills_categories.SetOptions(options);
    _skills_categories.SetSelection(SKILL_ALL);
    _skills_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

void SkillsWindow::Update()
{
    OptionBox *active_option = nullptr;

    GlobalMedia& media = GlobalManager->Media();

    //choose correct menu
    switch(_active_box) {
    case SKILL_ACTIVE_CATEGORY:
        active_option = &_skills_categories;
        break;
    case SKILL_ACTIVE_CHAR_APPLY:
    case SKILL_ACTIVE_CHAR:
        active_option = &_char_select;
        break;
    case SKILL_ACTIVE_LIST:
        active_option = &_skills_list;
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
        if (active_option == &_skills_list)
            _skill_cost_list.InputUp();
    } else if(InputManager->DownPress()) {
        media.PlaySound("bump");
        active_option->InputDown();
        if (active_option == &_skills_list)
            _skill_cost_list.InputDown();
    }

    uint32_t event = active_option->GetEvent();
    active_option->Update();
    if (active_option == &_skills_list)
        _skill_cost_list.Update();

    switch(_active_box) {
    case SKILL_ACTIVE_CHAR_APPLY:
        // Handle skill application
        if(event == VIDEO_OPTION_CONFIRM) {
            GlobalSkill* skill = _GetCurrentSkill();
            GlobalParty& party = GlobalManager->GetCharacterHandler().GetActiveParty();
            GlobalCharacter* user = party.GetCharacterAtIndex(_char_skillset);
            GlobalCharacter* target = party.GetCharacterAtIndex(_char_select.GetSelection());

            const luabind::object &script_function =
                skill->GetFieldExecuteFunction();

            if(!script_function.is_valid()) {
                media.PlaySound("cancel");
                break;
            }
            if(skill->GetSPRequired() > user->GetSkillPoints()) {
                media.PlaySound("cancel");
                break;
            }

            bool success = false;
            try {
                success =
                    luabind::call_function<bool>(script_function, user, target);
            } catch(const luabind::error& e) {
                PRINT_ERROR << "Error while loading FieldExecute() function"
                            << std::endl;
                vt_script::ScriptManager->HandleLuaError(e);
                success = false;
            } catch(const luabind::cast_failed& e) {
                PRINT_ERROR << "Error while loading FieldExecute() function"
                            << std::endl;
                vt_script::ScriptManager->HandleCastError(e);
                success = false;
            }

            if (success) {
                user->SubtractSkillPoints(skill->GetSPRequired());
                // We also update the Characters stats
                // as the item might have some effects there.
                MenuMode::CurrentInstance()->ReloadCharacterWindows();
                media.PlaySound("confirm");
            }
            else {
                media.PlaySound("cancel");
            }

        } else if(event == VIDEO_OPTION_CANCEL) {
            _active_box = SKILL_ACTIVE_LIST;
            _skills_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            _char_select.SetSelection(_char_skillset);
            media.PlaySound("cancel");
        }
        break;

    case SKILL_ACTIVE_CHAR:
        // Choose character for skillset
        if(event == VIDEO_OPTION_CONFIRM) {
            _active_box = SKILL_ACTIVE_CATEGORY;
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            _skills_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _char_skillset = _char_select.GetSelection();
            media.PlaySound("confirm");
        } else if(event == VIDEO_OPTION_CANCEL) {
            Activate(false);
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            media.PlaySound("cancel");
        }
        break;

    case SKILL_ACTIVE_LIST:
        // Choose skill
        if(event == VIDEO_OPTION_CONFIRM) {
            GlobalSkill *skill = _GetCurrentSkill();
            if(skill->IsExecutableInField()) {
                _active_box = SKILL_ACTIVE_CHAR_APPLY;
                _skills_list.SetCursorState(VIDEO_CURSOR_STATE_DARKEN);
                _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                media.PlaySound("confirm");
            } else
                media.PlaySound("cancel");
        } else if(event == VIDEO_OPTION_CANCEL) {
            _active_box = SKILL_ACTIVE_CATEGORY;
            media.PlaySound("cancel");
            _skills_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            _skills_categories.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
        }
        break;

    case SKILL_ACTIVE_CATEGORY:
        // Choose skill type
        if(event == VIDEO_OPTION_CONFIRM) {
            // Reset the list view
            _skills_list.SetSelection(0);
            _skill_cost_list.SetSelection(0);
            _skills_list.ResetViewableOption();
            _skill_cost_list.ResetViewableOption();

            if(_skills_list.GetNumberOptions() > 0) {
                _active_box = SKILL_ACTIVE_LIST;
                _skills_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
                _skills_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
                media.PlaySound("confirm");
            } else {
                media.PlaySound("cancel");
            }
        } else if(event == VIDEO_OPTION_CANCEL) {
            _active_box = SKILL_ACTIVE_CHAR;
            media.PlaySound("cancel");
            _skills_categories.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
            _char_select.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
            _char_select.SetSelection(_char_skillset);
        }
        break;
    }

    if(_active_box != SKILL_ACTIVE_CHAR_APPLY)
        _UpdateSkillList();

    // If the selection is invalid, we clear up the list and return
    if(_skills_list.GetNumberOptions() <= 0 || _skills_list.GetSelection() < 0
            || static_cast<int32_t>(_skills_list.GetNumberOptions())
               <= _skills_list.GetSelection()) {
        _skill_icon.Clear();
        _description.ClearText();
        return;
    }

    // If the menu isn't selecting any particular skill, we also return.
    if (_active_box != SKILL_ACTIVE_LIST &&
            _active_box != SKILL_ACTIVE_CHAR_APPLY) {
        _skill_icon.Clear();

        if (_active_box == SKILL_ACTIVE_NONE)
            _description.ClearText();
        if (_active_box == SKILL_ACTIVE_CHAR)
            _description.SetDisplayText(choose_character_message);
        else if (_active_box == SKILL_ACTIVE_CATEGORY)
            _description.SetDisplayText(choose_skill_category_message);
        return;
    }

    GlobalSkill* skill = _GetCurrentSkill();
    GlobalCharacter* skill_owner =
        GlobalManager->GetCharacterHandler().GetActiveParty().GetCharacterAtIndex(_char_skillset);

    // Get the skill type
    vt_utils::ustring skill_type;
    switch(skill->GetType()) {
        case GLOBAL_SKILL_WEAPON:
            if (skill_owner->GetEquippedWeapon())
                skill_type = UTranslate("Weapon skill");
            else
                skill_type = UTranslate("Bare hands");
            break;
        case GLOBAL_SKILL_MAGIC:
            skill_type = UTranslate("Magic skill");
            break;
        case GLOBAL_SKILL_SPECIAL:
            if (skill_owner)
                skill_type = skill_owner->GetSpecialCategoryName();
            else
                skill_type = UTranslate("Special skill");
            break;
        default:
        break;
    }

    vt_utils::ustring description = skill->GetName();
    if (!skill_type.empty())
        description += MakeUnicodeString("  (") + skill_type + MakeUnicodeString(")");

    description += MakeUnicodeString("\n\n");
    description += skill->GetDescription();
    _description.SetDisplayText(description);

    // Load the skill icon
    if (!skill->GetIconFilename().empty()) {
        _skill_icon.Load(skill->GetIconFilename());
        if (_skill_icon.GetHeight() > 70)
            _skill_icon.SetHeightKeepRatio(70);
    }
    else
        _skill_icon.Clear();

} // void SkillsWindow::Update()

GlobalSkill *SkillsWindow::_GetCurrentSkill()
{
    GlobalCharacter* ch =
        GlobalManager->GetCharacterHandler().GetActiveParty().GetCharacterAtIndex(_char_skillset);

    std::vector<GlobalSkill *> menu_skills;
    std::vector<GlobalSkill *> battle_skills;
    std::vector<GlobalSkill *> all_skills;

    if (ch->GetEquippedWeapon())
        _BuildMenuBattleSkillLists(ch->GetWeaponSkills(),
                                   &menu_skills, &battle_skills, &all_skills);
    else
        _BuildMenuBattleSkillLists(ch->GetBareHandsSkills(),
                                   &menu_skills, &battle_skills, &all_skills);
    _BuildMenuBattleSkillLists(ch->GetMagicSkills(),
                               &menu_skills, &battle_skills, &all_skills);
    _BuildMenuBattleSkillLists(ch->GetSpecialSkills(),
                               &menu_skills, &battle_skills, &all_skills);

    GlobalSkill *skill;
    switch(_skills_categories.GetSelection()) {
    case SKILL_ALL:
        skill = all_skills.at(_skills_list.GetSelection());
        break;
    case SKILL_BATTLE:
        skill = battle_skills.at(_skills_list.GetSelection());
        break;
    case SKILL_FIELD:
        skill = menu_skills.at(_skills_list.GetSelection());
        break;
    default:
        skill = nullptr;
        PRINT_ERROR << "MENU ERROR: Invalid skill type in SkillsWindow::_GetCurrentSkill()"
                    << std::endl;
        break;
    }

    return skill;
}

void SkillsWindow::_UpdateSkillList()
{
    GlobalCharacter* ch =
        GlobalManager->GetCharacterHandler().GetActiveParty().GetCharacterAtIndex(_char_select.GetSelection());
    assert(ch);
    std::vector<ustring> options;
    std::vector<ustring> cost_options;

    std::vector<GlobalSkill *> menu_skills;
    std::vector<GlobalSkill *> battle_skills;
    std::vector<GlobalSkill *> all_skills;

    // Iterators
    std::vector<GlobalSkill *>::const_iterator it;
    std::vector<GlobalSkill*>::const_iterator it_begin;
    std::vector<GlobalSkill*>::const_iterator it_end;

    if (ch->GetEquippedWeapon())
        _BuildMenuBattleSkillLists(ch->GetWeaponSkills(),
                                   &menu_skills, &battle_skills, &all_skills);
    else
        _BuildMenuBattleSkillLists(ch->GetBareHandsSkills(),
                                   &menu_skills, &battle_skills, &all_skills);
    _BuildMenuBattleSkillLists(ch->GetMagicSkills(),
                               &menu_skills, &battle_skills, &all_skills);
    _BuildMenuBattleSkillLists(ch->GetSpecialSkills(),
                               &menu_skills, &battle_skills, &all_skills);

    switch(_skills_categories.GetSelection()) {
    case SKILL_ALL:
        it_begin = all_skills.begin();
        it_end = all_skills.end();
        break;
    case SKILL_BATTLE:
        it_begin = battle_skills.begin();
        it_end = battle_skills.end();
        break;
    case SKILL_FIELD:
        it_begin = menu_skills.begin();
        it_end = menu_skills.end();
        break;
    default:
        break;
    }

    for(it = it_begin; it != it_end; ++it) {
        // Check for the existence of an icon
        vt_utils::ustring name;
        if((*it)->GetIconFilename().empty()) {
            // If no icon, use the weapon icon for weapon skills
            if ((*it)->GetType() == GLOBAL_SKILL_WEAPON &&
                 ch->GetEquippedWeapon() && !ch->GetEquippedWeapon()->GetIconImage().GetFilename().empty())
                name = MakeUnicodeString("<" + ch->GetEquippedWeapon()->GetIconImage().GetFilename() + ">");
            else if ((*it)->GetType() == GLOBAL_SKILL_BARE_HANDS)
                name = MakeUnicodeString("<data/inventory/weapons/fist-human.png>");

            name += MakeUnicodeString("<45>") + (*it)->GetName();
        }
        else {
            name += MakeUnicodeString("<" + (*it)->GetIconFilename() + "><45>") + (*it)->GetName();
        }

        options.push_back(name);

        std::string cost = NumberToString((*it)->GetSPRequired()) + Translate(" SP");
        cost_options.push_back(MakeUnicodeString(cost));
    }

    _skills_list.SetOptions(options);
    _skill_cost_list.SetOptions(cost_options);

    // Resize icons if necessary
    for (uint32_t i = 0; i < _skills_list.GetNumberOptions(); ++i) {
        if (StillImage *image = _skills_list.GetEmbeddedImage(i))
            image->SetHeightKeepRatio(45);
    }
}

void SkillsWindow::_BuildMenuBattleSkillLists(std::vector<GlobalSkill *> *skill_list,
                                              std::vector<GlobalSkill *> *field,
                                              std::vector<GlobalSkill *> *battle,
                                              std::vector<GlobalSkill *> *all)
{
    std::vector<GlobalSkill *>::iterator it;
    for(it = skill_list->begin(); it != skill_list->end(); ++it) {
        if((*it)->IsExecutableInBattle())
            battle->push_back(*it);
        if((*it)->IsExecutableInField())
            field->push_back(*it);
        all->push_back(*it);
    }
}

void SkillsWindow::Draw()
{
    MenuWindow::Draw();

    //Draw option boxes
    _char_select.Draw();
    _skills_categories.Draw();
    if(_active_box == SKILL_ACTIVE_NONE)
        _UpdateSkillList();
    _skills_list.Draw();
    _skill_cost_list.Draw();
}

} // namespace private_menu

} // namespace vt_menu
