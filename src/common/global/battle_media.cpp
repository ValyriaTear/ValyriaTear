////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_media.cpp
*** \author  Tyler Olsen, rootslinux@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** ***************************************************************************/ 

#include "utils/utils_pch.h"
#include "battle_media.h"

#include "engine/system.h"
#include "engine/audio/audio.h"

namespace vt_global
{

////////////////////////////////////////////////////////////////////////////////
// BattleMedia class
////////////////////////////////////////////////////////////////////////////////

// Filenames of the default music that is played when no specific music is requested
//@{
const std::string DEFAULT_BATTLE_MUSIC   = "data/music/heroism-OGA-Edward-J-Blakeley.ogg";
const std::string DEFAULT_VICTORY_MUSIC  = "data/music/Fanfare.ogg";
const std::string DEFAULT_DEFEAT_MUSIC   = "data/music/Battle_lost-OGA-Mumu.ogg";
//@}

BattleMedia::~BattleMedia()
{
    delete _auto_battle_activated;
}

void BattleMedia::Initialize()
{
    if(!background_image.Load("data/battles/battle_scenes/desert_cave/desert_cave.png"))
        PRINT_ERROR << "Failed to load default background image" << std::endl;

    if(stamina_icon_selected.Load("data/gui/battle/stamina_icon_selected.png") == false)
        PRINT_ERROR << "Failed to load stamina icon selected image" << std::endl;

    attack_point_indicator.SetDimensions(16.0f, 16.0f);
    if(attack_point_indicator.LoadFromFrameGrid("data/gui/battle/attack_point_target.png",
            std::vector<uint32_t>(4, 100), 1, 4) == false)
        PRINT_ERROR << "Failed to load attack point indicator." << std::endl;

    if(stamina_meter.Load("data/gui/battle/stamina_bar.png") == false)
        PRINT_ERROR << "Failed to load time meter." << std::endl;

    if(actor_selection_image.Load("data/gui/battle/character_selector.png") == false)
        PRINT_ERROR << "Unable to load player selector image" << std::endl;

    if(character_selected_highlight.Load("data/gui/battle/battle_character_selection.png") == false)
        PRINT_ERROR << "Failed to load character selection highlight image" << std::endl;

    if(character_command_highlight.Load("data/gui/battle/battle_character_command.png") == false)
        PRINT_ERROR << "Failed to load character command highlight image" << std::endl;

    if(bottom_menu_image.Load("data/gui/battle/battle_bottom_menu.png") == false)
        PRINT_ERROR << "Failed to load bottom menu image" << std::endl;

    if(vt_video::ImageDescriptor::LoadMultiImageFromElementGrid(character_action_buttons,
                                                                "data/gui/battle/battle_command_buttons.png", 2, 5) == false)
        PRINT_ERROR << "Failed to load character action buttons" << std::endl;

    if(vt_video::ImageDescriptor::LoadMultiImageFromElementGrid(_target_type_icons, "data/skills/targets.png", 1, 8) == false)
        PRINT_ERROR << "Failed to load character action buttons" << std::endl;

    // Set the default battle music.
    battle_music_filename = DEFAULT_BATTLE_MUSIC;
    if (!vt_audio::AudioManager->LoadMusic(DEFAULT_BATTLE_MUSIC))
        PRINT_WARNING << "Failed to load battle music file: " << DEFAULT_BATTLE_MUSIC << std::endl;

    if(victory_music.LoadAudio(DEFAULT_VICTORY_MUSIC) == false)
        PRINT_WARNING << "Failed to load victory music file: " << DEFAULT_VICTORY_MUSIC << std::endl;

    if(defeat_music.LoadAudio(DEFAULT_DEFEAT_MUSIC) == false)
        PRINT_WARNING << "Failed to load defeat music file: " << DEFAULT_DEFEAT_MUSIC << std::endl;

    if(!_stunned_icon.Load("data/entities/emotes/zzz.png"))
        PRINT_WARNING << "Failed to load stunned icon" << std::endl;

    if(!_escape_icon.Load("data/gui/battle/escape.png"))
        PRINT_WARNING << "Failed to load escape icon image" << std::endl;

    if(!_auto_battle_icon.Load("data/gui/battle/auto_battle.png"))
        PRINT_WARNING << "Failed to load auto-battle icon image" << std::endl;

    _auto_battle_activated = new vt_video::TextImage();
    _auto_battle_activated->SetText(vt_system::UTranslate("Auto-Battle"), vt_video::TextStyle("text20",
                                                                                              vt_video::Color::white,
                                                                                              vt_video::VIDEO_TEXT_SHADOW_NONE));
}

void BattleMedia::Update()
{
    attack_point_indicator.Update();
}

void BattleMedia::SetBackgroundImage(const std::string& filename)
{
    if(background_image.Load(filename) == false) {
        PRINT_WARNING << "Failed to load background image: " << filename << std::endl;
    }
}

void BattleMedia::SetBattleMusic(const std::string& filename)
{
    battle_music_filename = filename;
    if (!vt_audio::AudioManager->LoadMusic(filename))
        PRINT_WARNING << "Failed to load battle music file: " << filename << std::endl;
}

vt_video::StillImage* BattleMedia::GetCharacterActionButton(uint32_t index)
{
    if(index >= character_action_buttons.size()) {
        PRINT_WARNING << "Function received invalid index argument: " << index << std::endl;
        return nullptr;
    }

    return &(character_action_buttons[index]);
}

vt_video::StillImage* BattleMedia::GetTargetTypeIcon(vt_global::GLOBAL_TARGET target_type)
{
    switch(target_type) {
    case GLOBAL_TARGET_SELF_POINT:
        return &_target_type_icons[0];
    case GLOBAL_TARGET_ALLY_POINT:
        return &_target_type_icons[1];
    case GLOBAL_TARGET_FOE_POINT:
        return &_target_type_icons[2];
    case GLOBAL_TARGET_SELF:
        return &_target_type_icons[3];
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
    case GLOBAL_TARGET_DEAD_ALLY_ONLY:
        return &_target_type_icons[4];
    case GLOBAL_TARGET_FOE:
        return &_target_type_icons[5];
    case GLOBAL_TARGET_ALL_ALLIES:
        return &_target_type_icons[6];
    case GLOBAL_TARGET_ALL_FOES:
        return &_target_type_icons[7];
    default:
        PRINT_WARNING << "Function received invalid target type argument: " << target_type << std::endl;
        return nullptr;
    }
}

} // namespace vt_global
