////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "command_supervisor.h"

#include "modes/battle/battle.h"
#include "modes/battle/objects/battle_character.h"
#include "modes/battle/objects/battle_enemy.h"
#include "modes/battle/actions/item_action.h"
#include "modes/battle/actions/skill_action.h"

#include "engine/input.h"
#include "engine/system.h"
#include "engine/video/video.h"

#include "common/gui/option.h"
#include "common/global/global.h"
#include "common/global/actors/global_character.h"
#include "common/global/objects/global_weapon.h"
#include "common/global/actors/global_attack_point.h"

using namespace vt_gui;
using namespace vt_global;
using namespace vt_input;
using namespace vt_system;
using namespace vt_video;
using namespace vt_utils;

namespace vt_battle
{

namespace private_battle
{

const float TARGET_POSITION_X = 40.0f;
const float TARGET_POSITION_Y = 15.0f;
const float TARGET_SIZE_X = 450.0f;
const float TARGET_SIZE_Y = 100.0f;

CommandSupervisor::CommandSupervisor() :
    _state(COMMAND_STATE_INVALID),
    _active_settings(nullptr),
    _selected_skill(nullptr),
    _selected_item(nullptr),
    _item_command(_command_window),
    _skill_command(_command_window),
    _show_information(false)
{
    if(_command_window.Create(512.0f, 128.0f) == false) {
        PRINT_WARNING << "failed to create menu window" << std::endl;
    }
    _command_window.SetPosition(512.0f, 640.0f);
    _command_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _command_window.Show();

    _info_window.Create(512.0f, 150.0f);
    _info_window.SetPosition(512.0f, 470.0f);
    _info_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _info_window.Show();

    _category_icons.resize(4, StillImage());
    if(_category_icons[0].Load("data/gui/battle/default_weapon.png") == false)
        PRINT_ERROR << "failed to load category icon" << std::endl;
    if(_category_icons[1].Load("data/gui/battle/magic.png") == false)
        PRINT_ERROR << "failed to load category icon" << std::endl;
    if(_category_icons[2].Load("data/gui/battle/default_special.png") == false)
        PRINT_ERROR << "failed to load category icon" << std::endl;
    if(_category_icons[3].Load("data/gui/battle/item.png") == false)
        PRINT_ERROR << "failed to load category icon" << std::endl;

    _category_text.resize(4, TextImage("", TextStyle("title22")));

    _category_text[0].SetText(Translate("Weapon"));
    _category_text[1].SetText(Translate("Magic"));
    _category_text[2].SetText(""); // Default empty text
    _category_text[3].SetText(Translate("Items"));

    std::vector<ustring> option_text;
    option_text.push_back(MakeUnicodeString("<data/gui/battle/default_weapon.png>\n\n") + UTranslate("Weapon"));
    option_text.push_back(MakeUnicodeString("<data/gui/battle/magic.png>\n\n") + UTranslate("Magic"));
    option_text.push_back(MakeUnicodeString("")); // Special
    option_text.push_back(MakeUnicodeString("<data/gui/battle/item.png>\n\n") + UTranslate("Items"));

    _window_header.SetStyle(TextStyle("title22"));
    _selected_target_name.SetStyle(TextStyle("text20"));
    _info_header.SetStyle(TextStyle("title22"));
    _info_text.SetStyle(TextStyle("text20"));
    _info_text.SetWordWrapWidth(475);

    _category_options.SetOwner(&_command_window);
    _category_options.SetPosition(256.0f, 80.0f);
    _category_options.SetDimensions(460.0f, 100.0f, 4, 1, 4, 1);
    _category_options.SetCursorOffset(-20.0f, -25.0f);
    _category_options.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _category_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
    _category_options.SetTextStyle(TextStyle("title22"));
    _category_options.SetSelectMode(VIDEO_SELECT_SINGLE);
    _category_options.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _category_options.SetOptions(option_text);
    _category_options.SetSelection(0);

    // Skip disabled category items
    _category_options.SetSkipDisabled(true);

    _target_options.SetOwner(&_command_window);
    _target_options.SetPosition(TARGET_POSITION_X, TARGET_POSITION_Y);
    _target_options.SetDimensions(TARGET_SIZE_X, TARGET_SIZE_Y, 1, 255, 1, 4);
    _target_options.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _target_options.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _target_options.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _target_options.SetTextStyle(TextStyle("text20"));
    _target_options.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _target_options.SetCursorOffset(-50.0f, -25.0f);
}

CommandSupervisor::~CommandSupervisor()
{
    _command_window.Destroy();
    _info_window.Destroy();
}

void CommandSupervisor::ConstructMenus()
{
    _item_command.ConstructList();

    std::deque<BattleCharacter *>& characters = BattleMode::CurrentInstance()->GetCharacterActors();
    for(uint32_t i = 0; i < characters.size(); i++)
        _CreateCharacterSettings(characters[i]);
}

void CommandSupervisor::Initialize(BattleCharacter *character)
{
    if(character == nullptr) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received nullptr pointer argument" << std::endl;
        _state = COMMAND_STATE_INVALID;
        return;
    }

    if(_HasCharacterSettings(character) == false)
        _CreateCharacterSettings(character);

    _ChangeState(COMMAND_STATE_CATEGORY);
    _active_settings = &(_character_settings.find(character)->second);
    // Update _skill_list to check, if some skills need to be deactivated due to low amount of SP
    _active_settings->RefreshLists();
    // Also refresh the item list
    _item_command.ConstructList();

    _category_options.SetSelection(_active_settings->GetLastCategory());

    // Determine which categories should be enabled or disabled

    // Weapon
    // Determine the weapon icon name
    std::string icon_name = "<";

    std::shared_ptr<GlobalWeapon> wpn = character->GetGlobalCharacter()->GetWeaponEquipped();
    if (wpn) {
        if (wpn->GetIconImage().GetFilename().empty())
            icon_name += "data/gui/battle/default_weapon.png";
        else
            icon_name += wpn->GetIconImage().GetFilename();
    }
    else {
        icon_name += "data/inventory/weapons/fist-human.png";
    }
    icon_name += ">\n\n";

    // When a character has no basic weapon skill, we add the 'pass turn' skill instead.
    if(wpn) {
        _category_options.SetOptionText(CATEGORY_WEAPON, MakeUnicodeString(icon_name) +  UTranslate("Weapon"));
        _category_text[CATEGORY_WEAPON].SetText(UTranslate("Weapon"));
    }
    else {
        _category_options.SetOptionText(CATEGORY_WEAPON, MakeUnicodeString(icon_name) +  UTranslate("Bare-hand"));
        _category_text[CATEGORY_WEAPON].SetText(UTranslate("Bare-hand"));
    }

    // If there are no skills, disable it, and print a warning
    if (_active_settings->GetWeaponSkillList()->GetNumberOptions() == 0) {
        _category_options.EnableOption(CATEGORY_WEAPON, false);
        PRINT_WARNING << "No weapon or bare hand skills, the battle might get stuck" << std::endl;
    }

    // Magic
    if(_active_settings->GetMagicSkillList()->GetNumberOptions() == 0)
        _category_options.EnableOption(CATEGORY_MAGIC, false);
    else
        _category_options.EnableOption(CATEGORY_MAGIC, true);

    // Special
    if(_active_settings->GetSpecialSkillList()->GetNumberOptions() == 0) {
        _category_options.SetOptionText(CATEGORY_SPECIAL, MakeUnicodeString(""));
        _category_options.EnableOption(CATEGORY_SPECIAL, false);
        _category_text[CATEGORY_SPECIAL].SetText("");
    }
    else {
        // Set icon from character config.
        std::string special_icon = character->GetGlobalCharacter()->GetSpecialCategoryIconFilename();
        if (special_icon.empty())
            special_icon = "data/gui/battle/default_special.png";

        vt_utils::ustring special_name = character->GetGlobalCharacter()->GetSpecialCategoryName();
        _category_options.SetOptionText(CATEGORY_SPECIAL, MakeUnicodeString("<" + special_icon + ">\n\n") + special_name);
        _category_options.EnableOption(CATEGORY_SPECIAL, true);
        // Use special name from character config
        _category_text[CATEGORY_SPECIAL].SetText(special_name);
    }

    // Items
    if(_item_command.GetNumberListOptions() == 0)
        _category_options.EnableOption(CATEGORY_ITEM, false);
    else
        _category_options.EnableOption(CATEGORY_ITEM, true);

    // Warn if there are no enabled options in the category list
    for(uint32_t i = 0; i < _category_options.GetNumberOptions(); i++) {
        if(_category_options.IsOptionEnabled(i))
            return;
    }

    PRINT_ERROR << "No category options were enabled. The game might be stuck." << std::endl;
}

void CommandSupervisor::Update()
{
    switch(_state) {
    case COMMAND_STATE_CATEGORY:
        _UpdateCategory();
        break;
    case COMMAND_STATE_ACTION:
        _UpdateAction();
        break;
    case COMMAND_STATE_ACTOR:
        _UpdateActorTarget();
        break;
    case COMMAND_STATE_POINT:
        _UpdateAttackPointTarget();
        break;
    default:
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid/unknown command state: " << _state << std::endl;
        _ChangeState(COMMAND_STATE_CATEGORY);
        return;
    }
}

void CommandSupervisor::Draw()
{
    _command_window.Draw();

    switch(_state) {
    case COMMAND_STATE_CATEGORY:
        _DrawCategory();
        break;
    case COMMAND_STATE_ACTION:
        // Show potential info about the current selection
        if (_show_information)
            _DrawActionInformation();

        _DrawAction();
        break;
    case COMMAND_STATE_ACTOR:
        _DrawActorTarget();
        break;
    case COMMAND_STATE_POINT:
        // Show potential info about the currently selected attack point
        if (_show_information)
            _DrawActionInformation();

        _DrawAttackPointTarget();
        break;
    default:
        break;
    }
}

void CommandSupervisor::NotifyActorDeath(BattleActor *actor)
{
    if(_state == COMMAND_STATE_INVALID) {
        IF_PRINT_WARNING(BATTLE_DEBUG)
                << "function called when class was in invalid state" << std::endl;
        return;
    }

    BattleCharacter *character = GetCommandCharacter();

    // If the character currently selecting a command dies, we get out.
    if(character == actor) {
        _ChangeState(COMMAND_STATE_INVALID);
        GlobalManager->Media().PlaySound("cancel");
        return;
    }

    // Update the selected target if the target is the actor who just deceased
    if(_selected_target.GetActor() == actor) {
        GlobalManager->Media().PlaySound("cancel");

        // Try selecting the next actor and fall back to the previous command menu
        // if not possible.
        if(!_selected_target.SelectNextActor()) {
            if(character && (_state == COMMAND_STATE_ACTOR || _state == COMMAND_STATE_POINT))
                _ChangeState(COMMAND_STATE_ACTION);
            return;
        }
    }
}

void CommandSupervisor::CancelCurrentCommand()
{
    if (_state == COMMAND_STATE_INVALID || GetCommandCharacter() == nullptr) {
        IF_PRINT_WARNING(BATTLE_DEBUG)
            << "function called when class was in invalid state" << std::endl;
        return;
    }

    _ChangeState(COMMAND_STATE_INVALID);
    BattleMode::CurrentInstance()->NotifyCommandCancel();
}

bool CommandSupervisor::_IsSkillCategorySelected() const
{
    int32_t category = _category_options.GetSelection();
    if((category == CATEGORY_WEAPON) || (category == CATEGORY_MAGIC) || (category == CATEGORY_SPECIAL))
        return true;
    else
        return false;
}

bool CommandSupervisor::_IsItemCategorySelected() const
{
    int32_t category = _category_options.GetSelection();
    if(category == CATEGORY_ITEM)
        return true;
    else
        return false;
}

GLOBAL_TARGET CommandSupervisor::_ActionTargetType()
{
    if(_IsSkillCategorySelected())
        return _skill_command.GetSelectedSkill()->GetTargetType();
    else if(_IsItemCategorySelected())
        return _item_command.GetSelectedItem()->GetTargetType();
    else
        return GLOBAL_TARGET_INVALID;
}

bool CommandSupervisor::_SetInitialTarget()
{
    BattleActor* actor = GetCommandCharacter();
    GLOBAL_TARGET target_type = _ActionTargetType();

    // Party targets are simple because we don't have to restore the last save target, since there is either the
    // ally party or foe party, and both parties are always valid targets (because otherwise the battle would have
    // already ended).
    if(IsTargetParty(target_type)) {
        // Party-type targets are always the same, so we don't need to recall the last target in this case
        return _selected_target.SetTarget(actor, target_type);
    }

    // If we don't memorize the last target, simply select the first one again.
    if(!SystemManager->GetBattleTargetMemory()) {
        // If the target type is invalid that means that there is no previous target so grab the initial target
        if(!_selected_target.SetTarget(actor, target_type)) {
            // No more target of that type, let's go back to the command state
            _selected_target.InvalidateTarget();
            return false;
        }
        return true;
    }

    // Retrieved the last saved target depending on the type (self/ally/foe)
    if(IsTargetSelf(target_type)) {
        _selected_target = _active_settings->GetLastSelfTarget();
    } else if(IsTargetAlly(target_type)) {
        _selected_target = _active_settings->GetLastCharacterTarget();
    } else if(IsTargetFoe(target_type)) {
        _selected_target = _active_settings->GetLastEnemyTarget();
    } else {
        PRINT_WARNING << "No conditions met for invalid target type: " << target_type << std::endl;
    }
    // Restore the target type after getting the target from memory.
    if (!_selected_target.SetTarget(actor, target_type, _selected_target.GetActor(), _selected_target.GetAttackPoint())) {
        // No more target of that type, let's go back to the command state
        _selected_target.InvalidateTarget();
        return false;
    }

    if (_selected_target.IsValid())
        return true;

    // Otherwise if the last target is set but no longer valid, select the next valid target in the serie.
    if(_selected_target.GetActor() != nullptr) {
        // Party targets should always be valid and attack points on actors do not disappear, so only the actor
        // must be invalid
        if(!_selected_target.SelectNextActor()) {
            // No more target of that type, let's go back to the command state
            // Invalidate the target so that one can get a completely new one
            _selected_target.InvalidateTarget();
            GlobalManager->Media().PlaySound("cancel");
            return false;
        }
        return true;
    }
    else if (!_selected_target.SetTarget(actor, target_type)) {
        // No more target of that type, let's go back to the command state
        _selected_target.InvalidateTarget();
        GlobalManager->Media().PlaySound("cancel");
        return false;
    }
    return true;
}

void CommandSupervisor::_ChangeState(COMMAND_STATE new_state)
{
    if(_state == new_state) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "class was already in state to change to: " << new_state << std::endl;
        return;
    }

    if(new_state == COMMAND_STATE_INVALID) {
        _active_settings = nullptr;
        _selected_skill = nullptr;
        _selected_item = nullptr;
    } else if(new_state == COMMAND_STATE_CATEGORY) {
        // Nothing to do here. The Initialize() function performs all necessary actions when entering this state.
    } else if(new_state == COMMAND_STATE_ACTION) {
        // Construct the appropriate skill or item selection list if we're coming from the category state
        if(_state == COMMAND_STATE_CATEGORY) {
            switch(_category_options.GetSelection()) {
            case CATEGORY_WEAPON:
                if (GetCommandCharacter()->GetGlobalCharacter()->GetWeaponEquipped()) {
                    _skill_command.Initialize(GetCommandCharacter()->GetGlobalCharacter()->GetWeaponSkills(),
                                              _active_settings->GetWeaponSkillList(), _active_settings->GetWeaponTargetList());
                }
                else {
                    _skill_command.Initialize(GetCommandCharacter()->GetGlobalCharacter()->GetBareHandsSkills(),
                                              _active_settings->GetWeaponSkillList(), _active_settings->GetWeaponTargetList());
                }
                break;
            case CATEGORY_MAGIC:
                _skill_command.Initialize(GetCommandCharacter()->GetGlobalCharacter()->GetMagicSkills(),
                                          _active_settings->GetMagicSkillList(), _active_settings->GetMagicTargetList());
                break;
            case CATEGORY_SPECIAL:
                _skill_command.Initialize(GetCommandCharacter()->GetGlobalCharacter()->GetSpecialSkills(),
                                          _active_settings->GetSpecialSkillList(), _active_settings->GetSpecialTargetList());
                break;
            case CATEGORY_ITEM:
                _item_command.Initialize(_active_settings->GetLastItem());
                break;
            default:
                IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid category selection: " << _category_options.GetSelection() << std::endl;
                _category_options.SetSelection(0);
                return;
            }
        }
    } else if(new_state == COMMAND_STATE_ACTOR) {
        // Set the initial target if we're coming from the action selection state
        if(_state == COMMAND_STATE_ACTION) {
            if(!_SetInitialTarget()) {
                GlobalManager->Media().PlaySound("cancel");
                return;
            }
        }
        _target_options.ResetViewableOption();
        _UpdateActorTargetText();
    } else if(new_state == COMMAND_STATE_POINT) {
        _CreateAttackPointTargetText();
    }

    _state = new_state;
}

void CommandSupervisor::_UpdateCategory()
{
    _category_options.Update();

    // Event priority is given to the player requesting to abort the command selection process
    if(InputManager->CancelPress()) {
        // The player has to enter a command for this character before the battle is allowed to continue.
        if(GetCommandCharacter()->GetState() != ACTOR_STATE_COMMAND) {
            CancelCurrentCommand();
        }

        GlobalManager->Media().PlaySound("cancel");
    }

    else if(InputManager->ConfirmPress()) {
        if(_category_options.IsOptionEnabled(_category_options.GetSelection())) {
            _active_settings->SetLastCategory(_category_options.GetSelection());
            _ChangeState(COMMAND_STATE_ACTION);
            GlobalManager->Media().PlaySound("confirm");
        } else {
            GlobalManager->Media().PlaySound("cancel");
        }
    }

    else if(InputManager->LeftPress()) {
        _category_options.InputLeft();
        GlobalManager->Media().PlaySound("bump");
    }

    else if(InputManager->RightPress()) {
        _category_options.InputRight();
        GlobalManager->Media().PlaySound("bump");
    }
}

void CommandSupervisor::_UpdateAction()
{
    if(InputManager->CancelPress()) {
        _ChangeState(COMMAND_STATE_CATEGORY);
        GlobalManager->Media().PlaySound("cancel");
        return;
    }

    // Handles showing skills/item info on menu key pressed state
    if(InputManager->MenuState()) {
        _show_information = true;
        _UpdateActionInformation();
    }
    else {
        _show_information = false;
    }

    if(_IsSkillCategorySelected()) {
        _selected_skill = _skill_command.GetSelectedSkill();

        if(InputManager->ConfirmPress()) {
            bool is_skill_enabled = _skill_command.GetSelectedSkillEnabled();
            if(is_skill_enabled) {
                _ChangeState(COMMAND_STATE_ACTOR);
                GlobalManager->Media().PlaySound("confirm");
            } else {
                GlobalManager->Media().PlaySound("cancel");
            }
        }
        else {
            _skill_command.UpdateList();
            _UpdateActionInformation();
        }
    } else if(_IsItemCategorySelected()) {
        _selected_item = _item_command.GetSelectedItem();

        if(InputManager->ConfirmPress()) {
            // Permit the selection only where are items left.
            if(_selected_item != nullptr && _item_command.IsSelectedItemAvailable()) {
                _ChangeState(COMMAND_STATE_ACTOR);
                GlobalManager->Media().PlaySound("confirm");
            } else {
                GlobalManager->Media().PlaySound("cancel");
            }
        }
        else {
            _item_command.UpdateList();
            _UpdateActionInformation();
        }
    } else {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid category selection: " << _category_options.GetSelection() << std::endl;
        _state = COMMAND_STATE_CATEGORY;
        _category_options.SetSelection(CATEGORY_WEAPON);
    }
}

void CommandSupervisor::_UpdateActorTarget()
{
    if(InputManager->CancelPress()) {
        _ChangeState(COMMAND_STATE_ACTION);
        GlobalManager->Media().PlaySound("cancel");
    }

    else if(InputManager->ConfirmPress()) {
        if(IsTargetPoint(_selected_target.GetType())) {
            _ChangeState(COMMAND_STATE_POINT);
        } else {
            _FinalizeCommand();
        }
    }

    else if(InputManager->UpPress() || InputManager->DownPress()) {
        if(InputManager->DownPress())
            _target_options.InputDown();
        else
            _target_options.InputUp();

        if((IsTargetActor(_selected_target.GetType())) || (IsTargetPoint(_selected_target.GetType()))) {
            _selected_target.SelectNextActor(InputManager->DownPress());
            _UpdateActorTargetText();
            GlobalManager->Media().PlaySound("bump");
        }
    }
    _target_options.Update();
}

void CommandSupervisor::_UpdateAttackPointTarget()
{
    if(InputManager->CancelPress()) {
        _ChangeState(COMMAND_STATE_ACTOR);
        GlobalManager->Media().PlaySound("cancel");
    }

    else if(InputManager->ConfirmPress()
            || _selected_target.GetActor()->GetAttackPoints().size() == 1) {
        _FinalizeCommand();
        GlobalManager->Media().PlaySound("confirm");
    }

    else if(InputManager->UpPress() || InputManager->DownPress()) {
        if(InputManager->DownPress())
            _target_options.InputDown();
        else
            _target_options.InputUp();

        _selected_target.SelectNextPoint(InputManager->DownPress());
        GlobalManager->Media().PlaySound("bump");
    }

    // Handles showing target points info
    if(InputManager->MenuState()) {
        _show_information = true;
        _UpdateActionInformation();
    }
    else {
        _show_information = false;
    }

    _target_options.Update();
}

std::string _TurnIntoSeconds(uint32_t milliseconds)
{
    uint32_t seconds = milliseconds / 1000;
    uint32_t dec = (milliseconds / 100) - (seconds * 10);
    std::string formatted_seconds = NumberToString(seconds) + "." + NumberToString(dec);
    /// tr: this is about displaying a time: eg 4.2s
    formatted_seconds = VTranslate("%ss", formatted_seconds);
    return formatted_seconds;
}

void CommandSupervisor::_UpdateActionInformation()
{
    ustring info_text;

    if (_state == COMMAND_STATE_POINT) {
        // Show the target points information.
        BattleActor* actor = _selected_target.GetActor();
        uint32_t selected_point = _selected_target.GetAttackPoint();
        GlobalAttackPoint* attack_point = actor->GetAttackPoint(selected_point);

        _info_header.SetText(attack_point->GetName());

         // Set the text
        info_text = UTranslate("Enemy Defense Modifiers:\n");
        // Evade
        // Hack to get the text located right.
        // FIXME: Once image within text is supported, we can get rid of all that.
        float evade_modifier = attack_point->GetEvadeModifier();
        {
            std::string evade_str = "     ";
            if (evade_modifier > 0.0f)
                evade_str += "+" + NumberToString(evade_modifier) + "%";
            else
                evade_str += NumberToString(evade_modifier) + "%";

            // Make the text be of fixed width (kinda)...
            while (evade_str.length() < 20)
                evade_str += " ";

            info_text += MakeUnicodeString(evade_str);

        }
        // Physical defense
        float phys_def_modifier = attack_point->GetPhysDefModifier();
        {
            std::string phys_def_str;
            if (phys_def_modifier > 0.0f)
                phys_def_str = "+" + NumberToString(phys_def_modifier);
            else
                phys_def_str = NumberToString(phys_def_modifier);

            while (phys_def_str.length() < 15)
                phys_def_str += " ";
            info_text += MakeUnicodeString(phys_def_str);
        }
        // Magical defense
        float mag_def_modifier = attack_point->GetMagDefModifier();
        {
            std::string mag_def_str;
            if (mag_def_modifier > 0.0f)
                mag_def_str = "+" + NumberToString(mag_def_modifier);
            else
                mag_def_str = NumberToString(mag_def_modifier);

            info_text += MakeUnicodeString(mag_def_str);
        }

        // Display the percentage of status effect modification
        info_text += UTranslate("\n\nEffects:\n");

        _selected_attack_point_status_effects.clear();
        const std::vector<std::pair<GLOBAL_STATUS, float> >& effects = attack_point->GetStatusEffects();
        // Only show the first effects, in case of bad config.
        for (uint32_t i = 0; i < effects.size() && i < 2; ++i) {
            _selected_attack_point_status_effects.push_back(GlobalManager->Media().GetStatusIcon(effects[i].first, GLOBAL_INTENSITY_NEG_LESSER));
            info_text += MakeUnicodeString("      -> " + NumberToString(effects[i].second) + "%\n");
        }

    } else if(_IsSkillCategorySelected()) {
        _info_header.SetText(_selected_skill->GetName()
                             + MakeUnicodeString(" - "
                             + VTranslate("%s SP", NumberToString(_selected_skill->GetSPRequired()))));

        info_text = MakeUnicodeString(VTranslate("Target Type: %s", GetTargetText(_selected_skill->GetTargetType())) + "\n");
        info_text += MakeUnicodeString(VTranslate("Prep Time: %s", _TurnIntoSeconds(_selected_skill->GetWarmupTime())) + " - ");
        info_text += MakeUnicodeString(VTranslate("Cool Time: %s", _TurnIntoSeconds(_selected_skill->GetCooldownTime())) + "\n\n");
        info_text += _selected_skill->GetDescription();
    } else if(_IsItemCategorySelected()) {
        _info_header.SetText(_selected_item->GetGlobalItem().GetName()
                             + MakeUnicodeString(" x " + NumberToString(_selected_item->GetBattleCount())));
        info_text = MakeUnicodeString(VTranslate("Target Type: %s", GetTargetText(_selected_item->GetTargetType())) + "\n\n");
        info_text += _selected_item->GetGlobalItem().GetDescription();
    } else {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "unknown category selected: " << _category_options.GetSelection() << std::endl;
    }

    _info_text.SetText(info_text);
}

void CommandSupervisor::_DrawCategory()
{
    _category_options.Draw();
}

void CommandSupervisor::_DrawAction()
{
    uint32_t category_index = _category_options.GetSelection();

    // Draw the corresponding category icon and text to the left side of the window
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
    VideoManager->Move(570.0f, 693.0);
    if (_category_options.GetEmbeddedImage(category_index))
        _category_options.GetEmbeddedImage(category_index)->Draw();
    else
        _category_icons[category_index].Draw();
    VideoManager->MoveRelative(0.0f, 35.0f);
    _category_text[category_index].Draw();

    // Draw the header and list for either the skills or items to the right side of the window
    if(_IsSkillCategorySelected()) {
        _skill_command.DrawList();
    } else if(_IsItemCategorySelected()) {
        _item_command.DrawList();
    }
}

void CommandSupervisor::_DrawActorTarget()
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(560.0f, 658.0f);
    _window_header.Draw();
    VideoManager->Move(560.0f, 683.0f);
    _selected_target_name.Draw();
    // We don't draw the target option as it may desync when enemies spawn/die.
    //_target_options.Draw();

    // Draw relevant active status effect icons
    float width = _selected_target_name.GetWidth() < 130.0f ? 130.0f : _selected_target_name.GetWidth() + 10.0f;
    VideoManager->MoveRelative(width, 0.0f);
    for (uint32_t i = 0; i < _selected_target_status_effects.size(); ++i) {
        _selected_target_status_effects[i]->Draw();
        VideoManager->MoveRelative(25.0f, 0.0f);
    }
}

void CommandSupervisor::_DrawAttackPointTarget()
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(560.0f, 658.0f);
    _window_header.Draw();
    VideoManager->Move(560.0f, 683.0f);
    _target_options.Draw();
}

void CommandSupervisor::_DrawActionInformation()
{
    _info_window.Draw();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(560.0f, 488.0f);
    _info_header.Draw();
    VideoManager->Move(530.0f, 510.0f);
    _info_text.Draw();

    if (_state != COMMAND_STATE_POINT)
            return;

    // Draw the modifiers icons.
    VideoManager->MoveRelative(0.0f, 25.0f);
    GlobalManager->Media().GetStatusIcon(GLOBAL_STATUS_EVADE, GLOBAL_INTENSITY_NEUTRAL)->Draw();
    VideoManager->MoveRelative(80.0f, 0.0f);
    GlobalManager->Media().GetStatusIcon(GLOBAL_STATUS_PHYS_DEF, GLOBAL_INTENSITY_NEUTRAL)->Draw();
    VideoManager->MoveRelative(80.0f, 0.0f);
    GlobalManager->Media().GetStatusIcon(GLOBAL_STATUS_MAG_DEF, GLOBAL_INTENSITY_NEUTRAL)->Draw();
    VideoManager->MoveRelative(-160.0f, 60.0f);

    // Draw the status effect applied next to their percentage.
    for (uint32_t i = 0; i < _selected_attack_point_status_effects.size(); ++i) {
        _selected_attack_point_status_effects[i]->Draw();
        VideoManager->MoveRelative(0.0f, 20.0f);
    }
}

void CommandSupervisor::_UpdateActorTargetText()
{
    _window_header.SetText(UTranslate("Select Target"));

    _target_options.ClearOptions();
    if(IsTargetParty(_selected_target.GetType())) {
        if(_selected_target.GetType() == GLOBAL_TARGET_ALL_ALLIES) {
            _target_options.AddOption(UTranslate("All Allies"));
        } else {
            _target_options.AddOption(UTranslate("All Enemies"));
        }
    } else if(IsTargetSelf(_selected_target.GetType())) {
        _target_options.AddOption(_selected_target.GetActor()->GetName());
    } else if(IsTargetAlly(_selected_target.GetType())) {
        for(uint32_t i = 0; i < BattleMode::CurrentInstance()->GetCharacterActors().size(); i++) {
            _target_options.AddOption(BattleMode::CurrentInstance()->GetCharacterActors().at(i)->GetName());
            if(_selected_target.GetType() != GLOBAL_TARGET_ALLY_EVEN_DEAD
                    && !BattleMode::CurrentInstance()->GetCharacterActors().at(i)->IsAlive()) {
                _target_options.EnableOption(i, false);
            }
        }
    } else if(IsTargetFoe(_selected_target.GetType())) {
        for(uint32_t i = 0; i < BattleMode::CurrentInstance()->GetEnemyActors().size(); i++) {
            _target_options.AddOption(BattleMode::CurrentInstance()->GetEnemyActors().at(i)->GetName());
            if(BattleMode::CurrentInstance()->GetEnemyActors().at(i)->IsAlive() == false) {
                _target_options.EnableOption(i, false);
            }
        }
    } else {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid target type: " << _selected_target.GetType() << std::endl;
    }

    // Clear the shown effects first.
    _selected_target_status_effects.clear();

    if(IsTargetParty(_selected_target.GetType())) {
        if(_selected_target.GetType() == GLOBAL_TARGET_ALL_ALLIES) {
            _selected_target_name.SetText(UTranslate("All Allies"));
        } else {
            _selected_target_name.SetText(UTranslate("All Enemies"));
        }
    } else {
        _selected_target_name.SetText(_selected_target.GetActor()->GetName());

        // Get every non neutral status effects.
        for (uint32_t i = 0; i < GLOBAL_STATUS_TOTAL; ++i) {
            GLOBAL_STATUS status = static_cast<GLOBAL_STATUS>(i);
            GLOBAL_INTENSITY intensity = _selected_target.GetActor()->GetActiveStatusEffectIntensity(status);
            if (intensity == GLOBAL_INTENSITY_NEUTRAL ||
                    intensity <= GLOBAL_INTENSITY_INVALID ||
                    intensity >= GLOBAL_INTENSITY_TOTAL) {
                continue;
            }

            _selected_target_status_effects.push_back(vt_global::GlobalManager->Media().GetStatusIcon(status, intensity));
        }
    }
}

void CommandSupervisor::_CreateAttackPointTargetText()
{
    _window_header.SetText(UTranslate("Select Attack Point"));

    BattleActor* actor = _selected_target.GetActor();
    uint32_t selected_point = _selected_target.GetAttackPoint();

    if (actor == nullptr) {
        PRINT_ERROR << "nullptr actor when selection target attack points!" << std::endl;
        return;
    }

    _target_options.ClearOptions();
    for(uint32_t i = 0; i < actor->GetAttackPoints().size(); i++) {
        _target_options.AddOption(actor->GetAttackPoints().at(i)->GetName());
    }

    _target_options.ResetViewableOption();
    _target_options.SetSelection(selected_point);
}

void CommandSupervisor::_FinalizeCommand()
{
    BattleAction* new_action = nullptr;
    BattleCharacter* character = GetCommandCharacter();

    _active_settings->SaveLastTarget(_selected_target);

    if (_IsSkillCategorySelected()) {
        new_action = new SkillAction(character, _selected_target, _selected_skill);
    } else if (_IsItemCategorySelected()) {
        new_action = new ItemAction(character, _selected_target, _selected_item);

        // Reserve the item for use by the character.
        _selected_item->DecrementBattleCount();
    } else {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "did not create action for character, unknown category selected: " << _category_options.GetSelection() << std::endl;
    }
    character->SetAction(new_action);

    _ChangeState(COMMAND_STATE_INVALID);
    BattleMode::CurrentInstance()->NotifyCharacterCommandComplete(character);
}

} // namespace private_battle

} // namespace vt_battle
