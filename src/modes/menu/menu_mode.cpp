///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "menu_mode.h"

#include "modes/menu/menu_states/menu_equip_state.h"
#include "modes/pause.h"

#include "common/message_window.h"
#include "common/global/global.h"
#include "common/global/actors/global_character.h"
#include "common/global/objects/global_weapon.h"
#include "common/global/objects/global_armor.h"

#include "engine/system.h"
#include "engine/video/video.h"
#include "engine/input.h"

#include "utils/exception.h"

namespace vt_menu
{

bool MENU_DEBUG = false;

MenuMode *MenuMode::_current_instance = nullptr;

// Window size helpers
const uint32_t win_start_x = (1024 - 800) / 2 - 40;
const uint32_t win_start_y = (768 - 600) / 2 + 15;
const uint32_t win_width = 208;

MenuMode::MenuMode() :
    GameMode(vt_mode_manager::MODE_MANAGER_MENU_MODE),
    _main_menu_state(this),
    _inventory_state(this),
    _party_state(this),
    _skills_state(this),
    _equip_state(this),
    _quests_state(this),
    _world_map_state(this),
    _inventory_window(this),
    _message_window(nullptr),
    _object(nullptr),
    _character(nullptr),
    _equip_view_type(private_menu::EQUIP_VIEW_NONE),
    _is_weapon(false),
    _spirit_number(0),
    _end_battle_mode(false)
{
    _current_instance = this;

    // Init the controls parameters.
    _time_text.SetTextStyle(vt_video::TextStyle("text22"));
    _time_text.SetAlignment(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_CENTER);
    _time_text.SetDimensions(200.0f, 30.0f);
    _time_text.SetPosition(140.0f, 620.0f);

    _drunes_text.SetTextStyle(vt_video::TextStyle("text22"));
    _drunes_text.SetAlignment(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_CENTER);
    _drunes_text.SetDimensions(200.0f, 30.0f);
    _drunes_text.SetPosition(140.0f, 650.0f);

    // Display the game time right away
    _update_of_time = 0;

    _locale_name.SetPosition(win_start_x + 40, win_start_y + 457);
    _locale_name.SetDimensions(500.0f, 50.0f);
    _locale_name.SetTextStyle(vt_video::TextStyle("title22"));
    _locale_name.SetAlignment(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_CENTER);
    _locale_name.SetDisplayText(vt_global::GlobalManager->GetMapHudName());

    // Initialize the location graphic
    _locale_graphic = vt_global::GlobalManager->GetMapImage();
    if(!_locale_graphic.GetFilename().empty())
        _locale_graphic.SetDimensions(480, 95);
    else
        _locale_graphic.SetDimensions(0, 0);

    // Save a copy of the current screen to use as the backdrop.
    try {
        _saved_screen = vt_video::VideoManager->CaptureScreen();
    }
    catch (const vt_utils::Exception& e) {
        IF_PRINT_WARNING(MENU_DEBUG) << e.ToString() << std::endl;
    }

    vt_global::GlobalMedia& media = vt_global::GlobalManager->Media();
    _key_item_icon = media.GetKeyItemIcon();

    _key_item_description.SetPosition(165, 600);
    _key_item_description.SetDimensions(700.0f, 50.0f);
    _key_item_description.SetTextStyle(vt_video::TextStyle("text20"));
    _key_item_description.SetAlignment(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_CENTER);
    _key_item_description.SetDisplayText(vt_system::UTranslate("This item is a key item and can be neither consumed nor sold."));

    _spirit_icon = media.GetSpiritSlotIcon();

    _spirit_description.SetPosition(165, 600);
    _spirit_description.SetDimensions(700.0f, 50.0f);
    _spirit_description.SetTextStyle(vt_video::TextStyle("text20"));
    _spirit_description.SetAlignment(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_CENTER);
    _spirit_description.SetDisplayText(vt_system::UTranslate("This item is an elemental spirit and can be associated with equipment."));

    _help_information.SetPosition(150, 570);
    _help_information.SetDimensions(700, 100);
    _help_information.SetTextStyle(vt_video::TextStyle("text20"));
    _help_information.SetDisplayMode(vt_gui::VIDEO_TEXT_INSTANT);
    _help_information.SetTextAlignment(vt_video::VIDEO_X_CENTER, vt_video::VIDEO_Y_TOP);

    _atk_icon = media.GetStatusIcon(vt_global::GLOBAL_STATUS_PHYS_ATK, vt_global::GLOBAL_INTENSITY_NEUTRAL);
    _matk_icon = media.GetStatusIcon(vt_global::GLOBAL_STATUS_MAG_ATK, vt_global::GLOBAL_INTENSITY_NEUTRAL);
    _def_icon = media.GetStatusIcon(vt_global::GLOBAL_STATUS_PHYS_DEF, vt_global::GLOBAL_INTENSITY_NEUTRAL);
    _mdef_icon = media.GetStatusIcon(vt_global::GLOBAL_STATUS_MAG_DEF, vt_global::GLOBAL_INTENSITY_NEUTRAL);

    _clock_icon = vt_global::GlobalManager->Media().GetClockIcon();
    _clock_icon->SetWidthKeepRatio(30.0f);
    _drunes_icon = vt_global::GlobalManager->Media().GetDrunesIcon();
    _drunes_icon->SetWidthKeepRatio(30.0f);

    //////////// Setup the menu windows
    // Width of each character window is 360 px.
    // Each char window will have an additional 16 px for the left border
    // The 4th (last) char window will have another 16 px for the right border
    // Height of the char window is 98 px.
    // The bottom window in the main view is 192 px high, and the full width which will be 216 * 4 + 16
    _character_window0.Create(360, 98, ~vt_gui::VIDEO_MENU_EDGE_BOTTOM, vt_gui::VIDEO_MENU_EDGE_BOTTOM);
    _character_window0.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    _character_window1.Create(360, 98, ~vt_gui::VIDEO_MENU_EDGE_BOTTOM,
                              vt_gui::VIDEO_MENU_EDGE_BOTTOM | vt_gui::VIDEO_MENU_EDGE_TOP);
    _character_window1.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 118));

    _character_window2.Create(360, 98, ~vt_gui::VIDEO_MENU_EDGE_BOTTOM,
                              vt_gui::VIDEO_MENU_EDGE_BOTTOM | vt_gui::VIDEO_MENU_EDGE_TOP);
    _character_window2.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 226));

    _character_window3.Create(360, 98, ~vt_gui::VIDEO_MENU_EDGE_BOTTOM,
                              vt_gui::VIDEO_MENU_EDGE_TOP | vt_gui::VIDEO_MENU_EDGE_BOTTOM);
    _character_window3.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 334));

    // The character windows
    ReloadCharacterWindows();

    // The bottom window for the menu
    _bottom_window.Create(static_cast<float>(win_width * 4 + 16), 140 + 16, vt_gui::VIDEO_MENU_EDGE_ALL);
    _bottom_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 442));

    _main_options_window.Create(static_cast<float>(win_width * 4 + 16), 60,
                                ~vt_gui::VIDEO_MENU_EDGE_BOTTOM, vt_gui::VIDEO_MENU_EDGE_BOTTOM);
    _main_options_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y - 50));

    // Set up the party window
    _party_window.Create(static_cast<float>(win_width * 4 + 16), 448, vt_gui::VIDEO_MENU_EDGE_ALL);
    _party_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    // Set up the party window
    _battle_formation_window.Create(static_cast<float>(win_width * 4 + 16), 448, vt_gui::VIDEO_MENU_EDGE_ALL);
    _battle_formation_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    // Set up the skills window
    _skills_window.Create(static_cast<float>(win_width * 4 + 16), 448, vt_gui::VIDEO_MENU_EDGE_ALL);
    _skills_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    // Set up the skilltree window
    _skilltree_window.Create(static_cast<float>(win_width * 4 + 16), 448, vt_gui::VIDEO_MENU_EDGE_ALL);
    _skilltree_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    // Set up the equipment window
    _equip_window.Create(static_cast<float>(win_width * 4 + 16), 448, vt_gui::VIDEO_MENU_EDGE_ALL);
    _equip_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    // Set up the inventory window
    _inventory_window.Create(static_cast<float>(win_width * 4 + 16), 448, vt_gui::VIDEO_MENU_EDGE_ALL);
    _inventory_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    // Set up the quest window
    _quest_window.Create(static_cast<float>(win_width * 4 + 16), 448, vt_gui::VIDEO_MENU_EDGE_ALL);
    _quest_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    // Set up the quest list window
    _quest_list_window.Create(360, 448, vt_gui::VIDEO_MENU_EDGE_ALL,
                              vt_gui::VIDEO_MENU_EDGE_TOP | vt_gui::VIDEO_MENU_EDGE_BOTTOM);
    _quest_list_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    // Set up the world map window
    _world_map_window.Create(static_cast<float>(win_width * 4 + 16), 448, vt_gui::VIDEO_MENU_EDGE_ALL);
    _world_map_window.SetPosition(static_cast<float>(win_start_x), static_cast<float>(win_start_y + 10));

    _current_menu_state = &_main_menu_state;

    // Reset states
    _main_menu_state.Reset();
    _inventory_state.Reset();
    _party_state.Reset();
    _skills_state.Reset();
    _equip_state.Reset();
    _quests_state.Reset();

    // Show all windows (make them visible)
    _bottom_window.Show();
    _main_options_window.Show();
    _character_window0.Show();
    _character_window1.Show();
    _character_window2.Show();
    _character_window3.Show();
    _inventory_window.Show();
    _quest_list_window.Show();
    _party_window.Show();
    _battle_formation_window.Show();
    _skills_window.Show();
    _skilltree_window.Show();
    _equip_window.Show();
    _quest_window.Show();
    _world_map_window.Show();

    // Init the equipment view members
    _phys_header.SetStyle(vt_video::TextStyle("text18"));
    _mag_header.SetStyle(vt_video::TextStyle("text18"));

    _phys_stat.SetStyle(vt_video::TextStyle("text18"));
    _mag_stat.SetStyle(vt_video::TextStyle("text18"));

    _object_name.SetStyle(vt_video::TextStyle("text20"));

    _phys_stat_diff.SetStyle(vt_video::TextStyle("text18"));
    _mag_stat_diff.SetStyle(vt_video::TextStyle("text18"));

    _equip_skills_header.SetStyle(vt_video::TextStyle("title20"));
    _equip_skills_header.SetText(vt_system::UTranslate("Skills obtained:"));
}

MenuMode::~MenuMode()
{
    IF_PRINT_WARNING(MENU_DEBUG)
            << "MENU: MenuMode destructor invoked." << std::endl;

    // Destroy all menu windows
    _bottom_window.Destroy();
    _character_window0.Destroy();
    _character_window1.Destroy();
    _character_window2.Destroy();
    _character_window3.Destroy();
    _inventory_window.Destroy();
    _quest_list_window.Destroy();
    _party_window.Destroy();
    _battle_formation_window.Destroy();
    _skills_window.Destroy();
    _skilltree_window.Destroy();
    _main_options_window.Destroy();
    _equip_window.Destroy();
    _quest_window.Destroy();
    _world_map_window.Destroy();

    _current_instance = nullptr;

    if(_message_window != nullptr)
        delete _message_window;
}

void MenuMode::Reset()
{
    _current_instance = this;

    // Reload the characters' information since
    // active status effects may have changed.
    ReloadCharacterWindows();
}

void MenuMode::UpdateTimeAndDrunes()
{
    // Only update the time every 900ms
    _update_of_time -= (int32_t) vt_system::SystemManager->GetUpdateTime();
    if (_update_of_time > 0)
        return;
    _update_of_time = 900;

    std::ostringstream os_time;
    uint8_t hours = vt_system::SystemManager->GetPlayHours();
    uint8_t minutes = vt_system::SystemManager->GetPlayMinutes();
    uint8_t seconds = vt_system::SystemManager->GetPlaySeconds();
    os_time << (hours < 10 ? "0" : "") << static_cast<uint32_t>(hours) << ":";
    os_time << (minutes < 10 ? "0" : "") << static_cast<uint32_t>(minutes) << ":";
    os_time << (seconds < 10 ? "0" : "") << static_cast<uint32_t>(seconds);

    _time_text.SetDisplayText(vt_utils::MakeUnicodeString(os_time.str()));
    _drunes_text.SetDisplayText(vt_utils::MakeUnicodeString(vt_utils::NumberToString(vt_global::GlobalManager->GetDrunes())));
}

void MenuMode::Update()
{
    if(vt_input::InputManager->QuitPress()) {
        vt_mode_manager::ModeManager->Push(new vt_pause::PauseMode(true));
        return;
    } else if(vt_input::InputManager->PausePress()) {
        vt_mode_manager::ModeManager->Push(new vt_pause::PauseMode(false));
        return;
    }

    // check the message window
    if(_message_window != nullptr) {
        _message_window->Update();
        if(vt_input::InputManager->ConfirmPress()
                || vt_input::InputManager->CancelPress()) {
            delete _message_window;
            _message_window = nullptr;
        }
        return;
    }

    _current_menu_state->Update();

    // Update the overall game mode
    GameMode::Update();

}

void MenuMode::UpdateEquipmentInfo(vt_global::GlobalCharacter* character,
                                   const std::shared_ptr<vt_global::GlobalObject>& object,
                                   private_menu::EQUIP_VIEW view_type)
{
    // Only update when necessary
    if ((_object == object) && (_character == character) && (_equip_view_type == view_type))
        return;

    _object = object;
    _character = character;
    _equip_view_type = view_type;

    // Clear the corresponding texts when there is no corresponding data
    if (!_object) {
        _object_name.Clear();
        _status_icons.clear();
        _spirit_number = 0;
        _equip_skills.clear();
        _equip_skill_icons.clear();

        _phys_header.Clear();
        _mag_header.Clear();
        _phys_stat.Clear();
        _mag_stat.Clear();
    }

    if (view_type == private_menu::EQUIP_VIEW_NONE)
        return;

    // Don't show any diff when there no selected character,
    // or not showing equip/unequip diffs.
    if (!_character || view_type == private_menu::EQUIP_VIEW_CHAR) {
        _phys_stat_diff.Clear();
        _mag_stat_diff.Clear();
    }

    // If there is no object, we can return here.
    if (!_object)
        return;

    _object_name.SetText(_object->GetName());

    // Loads status effects.
    const std::vector<std::pair<vt_global::GLOBAL_STATUS, vt_global::GLOBAL_INTENSITY> >& status_effects = _object->GetStatusEffects();
    _status_icons.clear();
    for(std::vector<std::pair<vt_global::GLOBAL_STATUS, vt_global::GLOBAL_INTENSITY> >::const_iterator it = status_effects.begin();
            it != status_effects.end(); ++it) {
        if(it->second != vt_global::GLOBAL_INTENSITY_NEUTRAL)
            _status_icons.push_back(vt_global::GlobalManager->Media().GetStatusIcon(it->first, it->second));
    }

    uint32_t equip_phys_stat = 0;
    uint32_t equip_mag_stat = 0;

    switch (_object->GetObjectType()) {
        default: // Should never happen
            return;
        case vt_global::GLOBAL_OBJECT_WEAPON: {
            _is_weapon = true;
            std::shared_ptr<vt_global::GlobalWeapon> wpn = nullptr;
            // If character view or unequipping, we take the character current weapon as a base
            if (view_type == private_menu::EQUIP_VIEW_CHAR || view_type == private_menu::EQUIP_VIEW_UNEQUIPPING)
                wpn = _character ? _character->GetWeaponEquipped() : nullptr;
            else // We can take the given object as a base
                wpn = std::dynamic_pointer_cast<vt_global::GlobalWeapon>(_object);

            _spirit_number = wpn ? wpn->GetSpiritSlots().size() : 0;
            equip_phys_stat = wpn ? wpn->GetPhysicalAttack() : 0;
            equip_mag_stat = wpn ? wpn->GetMagicalAttack() : 0;

            const std::vector<uint32_t>& equip_skills = wpn->GetEquipmentSkills();
            _equip_skills.clear();
            _equip_skill_icons.clear();
            // Display a max of 5 skills
            for (uint32_t i = 0; i < equip_skills.size() && i < 5; ++i) {
                vt_global::GlobalSkill *skill = new vt_global::GlobalSkill(equip_skills[i]);
                if (skill && skill->IsValid()) {
                    _equip_skills.push_back(vt_video::TextImage(skill->GetName(),
                                                                vt_video::TextStyle("text20")));
                    _equip_skill_icons.push_back(vt_video::StillImage());
                    vt_video::StillImage& img = _equip_skill_icons.back();
                    img.Load(skill->GetIconFilename());
                    img.SetWidthKeepRatio(15.0f);
                }
                delete skill;
            }
            break;
        }

        case vt_global::GLOBAL_OBJECT_HEAD_ARMOR:
        case vt_global::GLOBAL_OBJECT_TORSO_ARMOR:
        case vt_global::GLOBAL_OBJECT_ARM_ARMOR:
        case vt_global::GLOBAL_OBJECT_LEG_ARMOR:
        {
            _is_weapon = false;
            std::shared_ptr<vt_global::GlobalArmor> armor = nullptr;

            // If character view or unequipping, we take the character current armor as a base
            if (view_type == private_menu::EQUIP_VIEW_CHAR || view_type == private_menu::EQUIP_VIEW_UNEQUIPPING) {
                uint32_t equip_index = GetEquipmentPositionFromObjectType(_object->GetObjectType());
                armor = _character ? _character->GetArmorEquipped(equip_index) : nullptr;
            }
            else { // We can take the given object as a base
                armor = std::dynamic_pointer_cast<vt_global::GlobalArmor>(_object);
            }

            _spirit_number = armor ? armor->GetSpiritSlots().size() : 0;
            equip_phys_stat = armor ? armor->GetPhysicalDefense() : 0;
            equip_mag_stat = armor ? armor->GetMagicalDefense() : 0;

            const std::vector<uint32_t>& equip_skills = armor->GetEquipmentSkills();
            _equip_skills.clear();
            _equip_skill_icons.clear();
            // Display a max of 5 skills
            for (uint32_t i = 0; i < equip_skills.size() && i < 5; ++i) {
                vt_global::GlobalSkill *skill = new vt_global::GlobalSkill(equip_skills[i]);
                if (skill && skill->IsValid()) {
                    _equip_skills.push_back(vt_video::TextImage(skill->GetName(), vt_video::TextStyle("text20")));
                    _equip_skill_icons.push_back(vt_video::StillImage());
                    vt_video::StillImage& img = _equip_skill_icons.back();
                    img.Load(skill->GetIconFilename());
                    img.SetWidthKeepRatio(15.0f);
                }
                delete skill;
            }
            break;
        }
    }

    if (_is_weapon) {
        _phys_header.SetText(vt_system::UTranslate("ATK:"));
        _mag_header.SetText(vt_system::UTranslate("M.ATK:"));
    }
    else {
        _phys_header.SetText(vt_system::UTranslate("DEF:"));
        _mag_header.SetText(vt_system::UTranslate("M.DEF:"));
    }

    _phys_stat.SetText(vt_utils::NumberToString(equip_phys_stat));
    _mag_stat.SetText(vt_utils::NumberToString(equip_mag_stat));

    // We can stop here if there is no valid character, or simply showing
    // the object stats
    if (!_character || view_type == private_menu::EQUIP_VIEW_CHAR)
        return;

    int32_t phys_stat_diff = 0;
    int32_t mag_stat_diff = 0;

    if (_is_weapon) {
        // Get the character's current attack
        std::shared_ptr<vt_global::GlobalWeapon> wpn = _character->GetWeaponEquipped();
        uint32_t char_phys_stat = 0;
        uint32_t char_mag_stat = 0;
        if (_equip_view_type == private_menu::EQUIP_VIEW_EQUIPPING) {
            char_phys_stat = (wpn ? wpn->GetPhysicalAttack() : 0);
            char_mag_stat = (wpn ? wpn->GetMagicalAttack() : 0);

            phys_stat_diff = equip_phys_stat - char_phys_stat;
            mag_stat_diff = equip_mag_stat - char_mag_stat;
        }
        else { // unequipping
            phys_stat_diff = char_phys_stat - equip_phys_stat;
            mag_stat_diff = char_mag_stat - equip_mag_stat;
        }
    }
    else { // armors
        uint32_t equip_index = GetEquipmentPositionFromObjectType(_object->GetObjectType());
        std::shared_ptr<vt_global::GlobalArmor> armor = _character->GetArmorEquipped(equip_index);
        uint32_t char_phys_stat = 0;
        uint32_t char_mag_stat = 0;
        if (_equip_view_type == private_menu::EQUIP_VIEW_EQUIPPING) {
            char_phys_stat = (armor ? armor->GetPhysicalDefense() : 0);
            char_mag_stat = (armor ? armor->GetMagicalDefense() : 0);

            phys_stat_diff = equip_phys_stat - char_phys_stat;
            mag_stat_diff = equip_mag_stat - char_mag_stat;
        }
        else { // unequiping
            phys_stat_diff = char_phys_stat - equip_phys_stat;
            mag_stat_diff = char_mag_stat - equip_mag_stat;
        }
    }

    // Compute the overall stats diff with selected equipment
    if (phys_stat_diff > 0) {
        _phys_stat_diff.SetText("+" + vt_utils::NumberToString(phys_stat_diff));
        _phys_diff_color.SetColor(vt_video::Color::green);
    }
    else if (phys_stat_diff < 0) {
        _phys_stat_diff.SetText(vt_utils::NumberToString(phys_stat_diff));
        _phys_diff_color.SetColor(vt_video::Color::red);
    }
    else {
        _phys_stat_diff.Clear();
    }

    if (mag_stat_diff > 0) {
        _mag_stat_diff.SetText("+" + vt_utils::NumberToString(mag_stat_diff));
        _mag_diff_color.SetColor(vt_video::Color::green);
    }
    else if (mag_stat_diff < 0) {
        _mag_stat_diff.SetText(vt_utils::NumberToString(mag_stat_diff));
        _mag_diff_color.SetColor(vt_video::Color::red);
    }
    else {
        _mag_stat_diff.Clear();
    }
}

void MenuMode::Draw()
{
    _current_menu_state->Draw();

    if(_message_window)
        _message_window->Draw();
}

void MenuMode::DrawEquipmentInfo()
{
    if (!_object)
        return;

    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT, vt_video::VIDEO_Y_TOP, 0);
    vt_video::VideoManager->Move(100.0f, 560.0f);
    _object_name.Draw();

    vt_video::VideoManager->MoveRelative(0.0f, 30.0f);
    const vt_video::StillImage& obj_icon = _object->GetIconImage();
    obj_icon.Draw();

    // Key item/spirit part
    if (_object->IsKeyItem()) {
        int32_t key_pos_x = obj_icon.GetWidth() - _key_item_icon->GetWidth() - 3;
        int32_t key_pos_y = obj_icon.GetHeight() - _key_item_icon->GetHeight() - 3;
        vt_video::VideoManager->MoveRelative(key_pos_x, key_pos_y);
        _key_item_icon->Draw();
        vt_video::VideoManager->MoveRelative(-key_pos_x, -key_pos_y);
    }

    // Draw weapon stats
    vt_video::VideoManager->MoveRelative(70.0f, 0.0f);
    if (_is_weapon)
        _atk_icon->Draw();
    else
        _def_icon->Draw();
    vt_video::VideoManager->MoveRelative(25.0f, 0.0f);
    _phys_header.Draw();

    vt_video::VideoManager->MoveRelative(-25.0f, 30.0f);
    if (_is_weapon)
        _matk_icon->Draw();
    else
        _mdef_icon->Draw();
    vt_video::VideoManager->MoveRelative(25.0f, 0.0f);
    _mag_header.Draw();

    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_RIGHT, 0);
    const float header_width = std::max(_phys_header.GetWidth(), _mag_header.GetWidth()) + 16.0f;
    vt_video::VideoManager->MoveRelative(header_width, -30.0f);
    _phys_stat.Draw();
    vt_video::VideoManager->MoveRelative(0.0f, 30.0f);
    _mag_stat.Draw();

    // Draw diff with current weapon stat, if any
    vt_video::VideoManager->MoveRelative(50.0f, -30.0f);
    _phys_stat_diff.Draw(_phys_diff_color);
    vt_video::VideoManager->MoveRelative(0.0f, 30.0f);
    _mag_stat_diff.Draw(_mag_diff_color);

    vt_video::VideoManager->SetDrawFlags(vt_video::VIDEO_X_LEFT, 0);
    vt_video::VideoManager->MoveRelative(20.0f, 0.0f);
    float j = 0;
    for (uint32_t i = 0; i < _spirit_number; ++i) {
        _spirit_icon->Draw();
        if (i % 2 == 0) {
            vt_video::VideoManager->MoveRelative(15.0f , 0.0f);
            j -= 15.0f;
        }
        else {
            vt_video::VideoManager->MoveRelative(25.0f , 0.0f);
            j -= 25.0f;
        }
    }
    vt_video::VideoManager->MoveRelative(j, -55.0f);

    // Draw status effects icons
    uint32_t element_size = _status_icons.size() > 9 ? 9 : _status_icons.size();
    vt_video::VideoManager->MoveRelative((18.0f * element_size), 0.0f);
    for(uint32_t i = 0; i < element_size; ++i) {
        _status_icons[i]->Draw();
        vt_video::VideoManager->MoveRelative(-18.0f, 0.0f);
    }
    vt_video::VideoManager->MoveRelative(0.0f, 20.0f);
    if (_status_icons.size() > 9) {
        element_size = _status_icons.size();
        vt_video::VideoManager->MoveRelative((18.0f * (element_size - 9)), 0.0f);
        for(uint32_t i = 9; i < element_size; ++i) {
            _status_icons[i]->Draw();
            vt_video::VideoManager->MoveRelative(-18.0f, 0.0f);
        }
    }

    // Draw possible equipment skills
    vt_video::VideoManager->MoveRelative(250.0f, -20.0f);
    element_size = _equip_skills.size();
    if (element_size > 0)
        _equip_skills_header.Draw();
    vt_video::VideoManager->MoveRelative(10.0f, 20.0f);
    for (uint32_t i = 0; i < element_size; ++i) {
        _equip_skills[i].Draw();
        vt_video::VideoManager->MoveRelative(-20.0f, 5.0f);
        _equip_skill_icons[i].Draw();
        vt_video::VideoManager->MoveRelative(20.0f, 15.0f);
    }
}

void MenuMode::ReloadCharacterWindows()
{
    vt_global::GlobalParty &characters = *vt_global::GlobalManager->GetActiveParty();

    // Setup character windows based on active party size
    switch(characters.GetPartySize()) {
    case 4:
        _character_window3.SetCharacter(characters.GetCharacterAtIndex(3));
        /* Falls through. */
    case 3:
        _character_window2.SetCharacter(characters.GetCharacterAtIndex(2));
        /* Falls through. */
    case 2:
        _character_window1.SetCharacter(characters.GetCharacterAtIndex(1));
        /* Falls through. */
    case 1:
        _character_window0.SetCharacter(characters.GetCharacterAtIndex(0));
        break;
    default:
        PRINT_ERROR << "No characters in party!" << std::endl;
        vt_mode_manager::ModeManager->Pop();
        break;
    }
}

void MenuMode::GoToImproveSkillMenu()
{
    // Small hack to go directly to the improve selection
    _current_menu_state = &_skills_state;
    _skills_state.GetOptions()->InputRight();
    _skills_state._current_category = private_menu::SkillsState::SKILLS_CATEGORY::SKILLS_OPTIONS_SKILL_GRAPH;
    _skilltree_window.SetActive(true);
    _end_battle_mode = true;
}

} // namespace vt_menu
