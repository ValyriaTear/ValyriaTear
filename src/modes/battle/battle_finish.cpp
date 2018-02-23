////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_finish.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for battle finish menu
*** ***************************************************************************/

#include "modes/battle/battle_finish.h"

#include "engine/audio/audio.h"
#include "engine/mode_manager.h"
#include "engine/input.h"
#include "engine/system.h"
#include "engine/video/video.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actions.h"
#include "modes/battle/battle_actors.h"
#include "modes/battle/battle_utils.h"

#include "modes/boot/boot.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_video;
using namespace vt_gui;
using namespace vt_input;
using namespace vt_mode_manager;
using namespace vt_system;
using namespace vt_global;

namespace vt_battle
{

namespace private_battle
{

//! \brief Draw position and dimension constants used for GUI objects
//@{
const float TOP_WINDOW_XPOS        = 512.0f;
const float TOP_WINDOW_YPOS        = 104.0f;
const float TOP_WINDOW_WIDTH       = 512.0f;
const float TOP_WINDOW_HEIGHT      = 64.0f;

const float TOOLTIP_WINDOW_XPOS    = TOP_WINDOW_XPOS;
const float TOOLTIP_WINDOW_YPOS    = TOP_WINDOW_YPOS + TOP_WINDOW_HEIGHT - 16.0f;
const float TOOLTIP_WINDOW_WIDTH   = TOP_WINDOW_WIDTH;
const float TOOLTIP_WINDOW_HEIGHT  = 112.0f;

const float CHAR_WINDOW_XPOS       = TOP_WINDOW_XPOS;
const float CHAR_WINDOW_YPOS       = TOOLTIP_WINDOW_YPOS;
const float CHAR_WINDOW_WIDTH      = TOP_WINDOW_WIDTH;
const float CHAR_WINDOW_HEIGHT     = 120.0f;

const float SPOILS_WINDOW_XPOS     = TOP_WINDOW_XPOS;
const float SPOILS_WINDOW_YPOS     = CHAR_WINDOW_YPOS + CHAR_WINDOW_HEIGHT + 30;
const float SPOILS_WINDOW_WIDTH    = TOP_WINDOW_WIDTH;
const float SPOILS_WINDOW_HEIGHT   = 220.0f;
//@}

////////////////////////////////////////////////////////////////////////////////
// FinishDefeatAssistant class
////////////////////////////////////////////////////////////////////////////////

FinishDefeatAssistant::FinishDefeatAssistant(FINISH_STATE &state) :
    _state(state)
{
    _options_window.Create(TOP_WINDOW_WIDTH, TOP_WINDOW_HEIGHT, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM);
    _options_window.SetPosition(TOP_WINDOW_XPOS, TOP_WINDOW_YPOS);
    _options_window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
    _options_window.Show();

    _tooltip_window.Create(TOOLTIP_WINDOW_WIDTH, TOOLTIP_WINDOW_HEIGHT);
    _tooltip_window.SetPosition(TOOLTIP_WINDOW_XPOS, TOOLTIP_WINDOW_YPOS);
    _tooltip_window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
    _tooltip_window.Show();

    _options.SetOwner(&_options_window);
    _options.SetPosition(TOP_WINDOW_WIDTH / 2, 28.0f);
    _options.SetDimensions(480.0f, 50.0f, 2, 1, 2, 1);
    _options.SetTextStyle(TextStyle("title22", Color::white, VIDEO_TEXT_SHADOW_DARK));
    _options.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _options.SetSelectMode(VIDEO_SELECT_SINGLE);
    _options.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _options.SetCursorOffset(-60.0f, -25.0f);
    _options.AddOption(UTranslate("Retry"));
    _options.AddOption(UTranslate("End"));
    _options.SetSelection(0);

    _confirm_options.SetOwner(&_options_window);
    _confirm_options.SetPosition(TOP_WINDOW_WIDTH / 2, 28.0f);
    _confirm_options.SetDimensions(240.0f, 50.0f, 2, 1, 2, 1);
    _confirm_options.SetTextStyle(TextStyle("title22", Color::white, VIDEO_TEXT_SHADOW_DARK));
    _confirm_options.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _confirm_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _confirm_options.SetSelectMode(VIDEO_SELECT_SINGLE);
    _confirm_options.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _confirm_options.SetCursorOffset(-60.0f, -25.0f);
    _confirm_options.AddOption(UTranslate("OK"));
    _confirm_options.AddOption(UTranslate("Cancel"));
    _confirm_options.SetSelection(0);

    _tooltip.SetOwner(&_tooltip_window);
    _tooltip.SetPosition(32.0f, 40.0f);
    _tooltip.SetDimensions(480.0f, 80.0f);
    _tooltip.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _tooltip.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _tooltip.SetDisplaySpeed(SystemManager->GetMessageSpeed());
    _tooltip.SetTextStyle(TextStyle("text20", Color::white));
    _tooltip.SetDisplayMode(VIDEO_TEXT_INSTANT);
}

FinishDefeatAssistant::~FinishDefeatAssistant()
{
    _options_window.Destroy();
    _tooltip_window.Destroy();
}

void FinishDefeatAssistant::Initialize()
{
    _SetTooltipText();

    _options_window.Show();
    _tooltip_window.Show();
}

void FinishDefeatAssistant::Update()
{
    switch(_state) {
    case FINISH_DEFEAT_SELECT:
        _options.Update();
        if(InputManager->ConfirmPress()) {
            if(!_options.IsOptionEnabled(_options.GetSelection())) {
                AudioManager->PlaySound("data/sounds/cancel.wav");
            } else {
                _state = FINISH_DEFEAT_CONFIRM;
                // Set default confirm option to "Cancel"
                if(_options.GetSelection() == (int32_t)DEFEAT_OPTION_END)
                    _confirm_options.SetSelection(1);
                else
                    _confirm_options.SetSelection(0);

                _SetTooltipText();
            }
        }

        else if(InputManager->LeftPress()) {
            _options.InputLeft();
            _SetTooltipText();
        } else if(InputManager->RightPress()) {
            _options.InputRight();
            _SetTooltipText();
        }

        break;

    case FINISH_DEFEAT_CONFIRM:
        _confirm_options.Update();
        if(InputManager->ConfirmPress()) {
            switch(_confirm_options.GetSelection()) {
            case 0: // "OK"
                _state = FINISH_END;
                _options_window.Hide();
                _tooltip_window.Hide();
                break;
            case 1: // "Cancel"
                _state = FINISH_DEFEAT_SELECT;
                _SetTooltipText();
                break;
            default:
                IF_PRINT_WARNING(BATTLE_DEBUG)
                        << "invalid confirm option selection: "
                        << _confirm_options.GetSelection() << std::endl;
                break;
            }
        }

        else if(InputManager->CancelPress()) {
            _state = FINISH_DEFEAT_SELECT;
            _SetTooltipText();
        }

        else if(InputManager->LeftPress()) {
            _confirm_options.InputLeft();
        } else if(InputManager->RightPress()) {
            _confirm_options.InputRight();
        }

        break;

    case FINISH_END:
        break;

    default:
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid finish state: " << _state << std::endl;
        break;
    }
}

void FinishDefeatAssistant::Draw()
{
    _options_window.Draw();
    _tooltip_window.Draw();

    if(_state == FINISH_DEFEAT_SELECT) {
        _options.Draw();
    } else if(_state == FINISH_DEFEAT_CONFIRM) {
        _confirm_options.Draw();
    }

    _tooltip.Draw();
}

void FinishDefeatAssistant::_SetTooltipText()
{
    if((_state == FINISH_ANNOUNCE_RESULT) || (_state == FINISH_DEFEAT_SELECT)) {
        switch(_options.GetSelection()) {
        case DEFEAT_OPTION_RETRY:
            _tooltip.SetDisplayText(Translate("Start over from the beginning of this battle."));
            break;
        case DEFEAT_OPTION_END:
            _tooltip.SetDisplayText(UTranslate("Exit to main menu."));
            break;
        default:
            _tooltip.SetDisplayText("");
            break;
        }
    } else if(_state == FINISH_DEFEAT_CONFIRM) {
        switch(_options.GetSelection()) {
        case DEFEAT_OPTION_RETRY:
            _tooltip.SetDisplayText(UTranslate("Confirm: retry battle."));
            break;
        case DEFEAT_OPTION_END:
            _tooltip.SetDisplayText(UTranslate("Confirm: return to main menu."));
            break;
        default:
            _tooltip.SetDisplayText("");
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// FinishVictoryAssistant class
////////////////////////////////////////////////////////////////////////////////

FinishVictoryAssistant::FinishVictoryAssistant(FINISH_STATE& state) :
    _state(state),
    _characters_number(0),
    _xp_earned(0),
    _drunes_dropped(0),
    _begin_counting(false),
    _number_character_windows_created(0)
{
    _header_window.Create(TOP_WINDOW_WIDTH, TOP_WINDOW_HEIGHT, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM);
    _header_window.SetPosition(TOP_WINDOW_XPOS, TOP_WINDOW_YPOS);
    _header_window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
    _header_window.Show();

    // Note: Character windows are created later when the Initialize() function is called. This is done because the borders
    // used with these windows depend on the number of characters in the party.

    _spoils_window.Create(SPOILS_WINDOW_WIDTH, SPOILS_WINDOW_HEIGHT);
    _spoils_window.SetPosition(SPOILS_WINDOW_XPOS, SPOILS_WINDOW_YPOS);
    _spoils_window.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
    _spoils_window.Show();

    _header_xp.SetOwner(&_header_window);
    _header_xp.SetPosition(TOP_WINDOW_WIDTH / 2 - 200.0f, 15.0f);
    _header_xp.SetDimensions(CHAR_WINDOW_WIDTH - 100.0f, 40.0f);
    _header_xp.SetTextStyle(TextStyle("text20", Color::white));
    _header_xp.SetDisplayMode(VIDEO_TEXT_INSTANT);

    for(uint32_t i = 0; i < 4; i++) {
        _raw_xp_given[i] = true;
        _raw_xp_won[i] = false;
    }

    _object_header_text.SetOwner(&_spoils_window);
    _object_header_text.SetPosition(SPOILS_WINDOW_XPOS - SPOILS_WINDOW_WIDTH + 50.0f, 15.0f);
    _object_header_text.SetDimensions(SPOILS_WINDOW_WIDTH - 100.0f, 40.0f);
    _object_header_text.SetTextStyle(TextStyle("title20", Color::white));
    _object_header_text.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _object_header_text.SetDisplayText(UTranslate("Items Found"));

    _header_drunes_dropped.SetOwner(&_spoils_window);
    _header_drunes_dropped.SetPosition(SPOILS_WINDOW_XPOS - SPOILS_WINDOW_WIDTH + 50.0f, 15.0f);
    _header_drunes_dropped.SetDimensions(400.0f, 40.0f);
    _header_drunes_dropped.SetTextStyle(TextStyle("text20", Color::white));
    _header_drunes_dropped.SetDisplayMode(VIDEO_TEXT_INSTANT);

    _header_total_drunes.SetOwner(&_spoils_window);
    _header_total_drunes.SetPosition(TOP_WINDOW_WIDTH / 2 + 50.0f, 15.0f);
    _header_total_drunes.SetDimensions(400.0f, 40.0f);
    _header_total_drunes.SetTextStyle(TextStyle("text20", Color::white));
    _header_total_drunes.SetDisplayMode(VIDEO_TEXT_INSTANT);

    _object_list.SetOwner(&_spoils_window);
    _object_list.SetPosition(100.0f, 45.0f);
    _object_list.SetDimensions(300.0f, 160.0f, 1, 8, 1, 8);
    _object_list.SetTextStyle(TextStyle("text20", Color::white));
    _object_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _object_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _object_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
}

FinishVictoryAssistant::~FinishVictoryAssistant()
{
    _header_window.Destroy();
    _spoils_window.Destroy();

    for (uint32_t i = 0; i < _number_character_windows_created; ++i) {
        _character_window[i].Destroy();
    }

    // Add all the objects that were dropped by enemies to the party's inventory.
    for (auto i = _objects_dropped.begin(); i != _objects_dropped.end(); ++i) {

        // Add the item to the inventory.
        assert(i->first != nullptr);
        GlobalManager->AddToInventory(i->first->GetID(), i->second);
    }
    _objects_dropped.clear();

    // Update the HP and SP of all characters.
    _SetCharacterStatus();
}

void FinishVictoryAssistant::Initialize()
{
    // Prepare all character data
    std::deque<BattleCharacter *>& all_characters = BattleMode::CurrentInstance()->GetCharacterActors();
    // Reinit the number of living characters
    uint32_t alive_characters_number = 0;

    _characters_number = all_characters.size();

    for(uint32_t i = 0; i < _characters_number; ++i) {
        _characters.push_back(all_characters[i]->GetGlobalCharacter());
        _character_portraits[i] = all_characters[i]->GetPortrait();

        // Only size up non valid images
        if(!_character_portraits[i].GetFilename().empty())
            _character_portraits[i].SetDimensions(100.0f, 100.0f);

        if(all_characters[i]->IsAlive()) {
            // Set up the victory animation for the living beings
            all_characters[i]->ChangeSpriteAnimation("victory");
            // Adds a living player to later split xp with.
            ++alive_characters_number;
        } else {
            // Gray out portraits of deceased characters
            _character_portraits[i].SetGrayscale(true);
        }
    }

    // Collect the XP, drunes, and dropped objects for each defeated enemy
    std::deque<BattleEnemy*>& all_enemies = BattleMode::CurrentInstance()->GetEnemyActors();
    for (uint32_t i = 0; i < all_enemies.size(); ++i) {
        GlobalEnemy* enemy = all_enemies[i]->GetGlobalEnemy();
        assert(enemy != nullptr);

        _xp_earned += enemy->GetExperiencePoints();
        _drunes_dropped += enemy->GetDrunesDropped();

        std::vector<std::shared_ptr<GlobalObject>> objects = enemy->DetermineDroppedObjects();
        for (uint32_t j = 0; j < objects.size(); ++j) {
            // Check if the object to add is already in our list. If so, just increase the quantity of that object.
            // iter = _objects_dropped.find(objects[j]); // Will not work since each item is created with new.
            // Need to search for the item ID instead.
            auto iter = _objects_dropped.begin();
            while (iter != _objects_dropped.end()) {
                if (iter->first->GetID() == objects[j]->GetID()) {
                    break;
                }
                ++iter;
            }

            if (iter != _objects_dropped.end()) {
                ++iter->second;
            } else {
                _objects_dropped.insert(std::make_pair(objects[j], 1));
            }
        }
        objects.clear();
    }

    // Divide up the XP earnings by the number of players (only living ones)
    if (alive_characters_number > 0)
        _xp_earned /= alive_characters_number;
    else
        _xp_earned /= 4; // Should never happen.

    // Compute the raw fighting XP bonus for each characters (20% of character's XP)
    for(uint32_t i = 0; i < _characters.size() && i < 4; ++i) {
        if (_characters[i]->HasEquipment()) {
            _raw_xp_won[i] = false;
            _raw_xp_given[i] = true;
        }
        else {
            _raw_xp_won[i] = true;
            _raw_xp_given[i] = false;
        }
    }

    _CreateCharacterGUIObjects();
    _CreateObjectList();
    _SetHeaderText();
}

void FinishVictoryAssistant::Update()
{
    switch(_state) {
    case FINISH_VICTORY_GROWTH:
        _UpdateXP();
        _UpdateSpoils();
        break;

    case FINISH_VICTORY_MENU: //TODO
        if (InputManager->ConfirmPress())
            _state = FINISH_END;
        break;

    case FINISH_END:
        break;

    default:
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid finish state: " << _state << std::endl;
        break;
    }
}

void FinishVictoryAssistant::Draw()
{
    _header_window.Draw();

        _header_xp.Draw();
        for(uint32_t i = 0; i < _characters_number; ++i) {
            _character_window[i].Draw();
            _DrawXP(i);
        }

        _header_drunes_dropped.Draw();
        _header_total_drunes.Draw();
        _spoils_window.Draw();
        _DrawSpoils();
        _object_list.Draw();
}

void FinishVictoryAssistant::_SetHeaderText()
{
    if(_state == FINISH_ANNOUNCE_RESULT) {
        _header_xp.SetDisplayText(UTranslate("XP Earned: ") + MakeUnicodeString(NumberToString(_xp_earned)));
        _header_drunes_dropped.SetDisplayText(UTranslate("Drunes Found: ") + MakeUnicodeString(NumberToString(_drunes_dropped)));
        _header_total_drunes.SetDisplayText(UTranslate("Total Drunes: ") + MakeUnicodeString(NumberToString(GlobalManager->GetDrunes())));
    }

    const uint32_t no_of_options = _object_list.GetNumberOptions();
    _object_header_text.SetDisplayText(no_of_options == 0 ?
        // tr: Header in battle result screen: 0 items found
            UTranslate("No Items Found") :
                no_of_options == 1 ?
                    // tr: Header in battle result screen: 1 item found
                    UTranslate("Item Found") :
                    // tr: Header in battle result screen: more than 1 item found
                    UTranslate("Items Found"));
}

void FinishVictoryAssistant::_CreateCharacterGUIObjects()
{
    // Create the character windows. The lowest one does not have its lower border removed
    float next_ypos = CHAR_WINDOW_YPOS;
    for(uint32_t i = 0; i < _characters_number; ++i) {
        _number_character_windows_created++;
        if((i + 1) >= _characters_number) {
            _character_window[i].Create(CHAR_WINDOW_WIDTH, CHAR_WINDOW_HEIGHT);
        } else {
            _character_window[i].Create(CHAR_WINDOW_WIDTH, CHAR_WINDOW_HEIGHT, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM);
        }

        _character_window[i].SetPosition(CHAR_WINDOW_XPOS, next_ypos);
        _character_window[i].SetAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
        _character_window[i].Show();
        next_ypos += CHAR_WINDOW_HEIGHT;
    }

    // Construct GUI objects that will fill each character window
    for(uint32_t i = 0; i < _characters_number; ++i) {
        _level_text[i].SetOwner(&_character_window[i]);
        _level_text[i].SetPosition(130.0f, 10.0f);
        _level_text[i].SetDimensions(300.0f, 25.0f);
        _level_text[i].SetTextStyle(TextStyle("text20", Color::white));
        _level_text[i].SetDisplayMode(VIDEO_TEXT_INSTANT);

        _xp_text[i].SetOwner(&_character_window[i]);
        _xp_text[i].SetPosition(130.0f, 40.0f);
        _xp_text[i].SetDimensions(300.0f, 25.0f);
        _xp_text[i].SetTextStyle(TextStyle("text20", Color::white));
        _xp_text[i].SetDisplayMode(VIDEO_TEXT_INSTANT);

        _unspent_xp[i].SetOwner(&_character_window[i]);
        _unspent_xp[i].SetPosition(130.0f, 70.0f);
        _unspent_xp[i].SetDimensions(300.0f, 25.0f);
        _unspent_xp[i].SetTextStyle(TextStyle("text20", Color::white));
        _unspent_xp[i].SetDisplayMode(VIDEO_TEXT_INSTANT);

        // Don't show XP when the maximum level has been reached.
        if(_characters[i]->GetExperienceLevel() >= GlobalManager->GetMaxExperienceLevel()) {
            _level_text[i].SetDisplayText(UTranslate("Level (Max): ")
                                             + MakeUnicodeString(NumberToString(_characters[i]->GetExperienceLevel())));
            _xp_text[i].SetDisplayText(" ");
        } else {
            _level_text[i].SetDisplayText(UTranslate("Level: ")
                                             + MakeUnicodeString(NumberToString(_characters[i]->GetExperienceLevel())));
            _xp_text[i].SetDisplayText(UTranslate("XP left: ")
                                       + MakeUnicodeString(NumberToString(_characters[i]->GetExperienceForNextLevel())));
            _unspent_xp[i].SetDisplayText(UTranslate("XP available for skills: ")
                                          + MakeUnicodeString(NumberToString(_characters[i]->GetUnspentExperiencePoints())));
        }
    }
}

void FinishVictoryAssistant::_CreateObjectList()
{
    for (auto i = _objects_dropped.begin(); i != _objects_dropped.end(); ++i) {
        std::shared_ptr<GlobalObject> obj = i->first;
        if (obj->GetIconImage().GetFilename().empty()) {
            _object_list.AddOption(MakeUnicodeString("<30>") +
                                   obj->GetName() + MakeUnicodeString("<R>x" + NumberToString(i->second)));
        }
        else {
            _object_list.AddOption(MakeUnicodeString("<" + obj->GetIconImage().GetFilename() + "><30>") +
                                   obj->GetName() + MakeUnicodeString("<R>x" + NumberToString(i->second)));
        }
    }

    // Resize all icon images so that they are the same height as the text
    for(uint32_t i = 0; i < _object_list.GetNumberOptions(); ++i) {
        StillImage* image = _object_list.GetEmbeddedImage(i);
        if (image != nullptr)
            _object_list.GetEmbeddedImage(i)->SetDimensions(30.0f, 30.0f);
    }
}

void FinishVictoryAssistant::_SetCharacterStatus()
{
    std::deque<BattleCharacter *>& battle_characters = BattleMode::CurrentInstance()->GetCharacterActors();

    for(std::deque<BattleCharacter *>::iterator i = battle_characters.begin(); i != battle_characters.end(); ++i) {
        GlobalCharacter *character = (*i)->GetGlobalCharacter();

        // Put back the current HP / SP onto the global characters.
        character->SetHitPoints((*i)->GetHitPoints());
        character->SetSkillPoints((*i)->GetSkillPoints());
    }
}

void FinishVictoryAssistant::_UpdateXP()
{
    // The number of milliseconds that we wait in between updating the XP count
    const uint32_t UPDATE_PERIOD = 50;
    // A simple counter used to keep track of when the next XP count should begin
    static uint32_t time_counter = 0;

    // The amount of XP to add to each character this update cycle
    uint32_t xp_to_add = 0;

    // Process confirm press inputs.
    if(InputManager->ConfirmPress()) {
        // Begin counting out XP earned
        if(!_begin_counting) {
            _begin_counting = true;
        }
        // If confirm received during counting, instantly add all remaining XP
        else if(_xp_earned != 0) {
            xp_to_add = _xp_earned;
        }
        // Counting has finished. Move on to the spoils screen
        else {
            _state = FINISH_VICTORY_MENU;
            _SetHeaderText();
        }
    }

    // If counting has not began or counting is alreasy finished, there is nothing more to do here
    if(!_begin_counting || (_xp_earned == 0))
        return;

    // Update the timer and determine how much XP to add if the time has been reached
    // We don't want to modify the XP to add if a confirm event occurred in step (1)
    if(xp_to_add == 0) {
        time_counter += SystemManager->GetUpdateTime();
        if(time_counter >= UPDATE_PERIOD) {
            time_counter -= UPDATE_PERIOD;

            // Determine an appropriate amount of XP to add here
            if(_xp_earned > 10000)
                xp_to_add = 1000;
            else if(_xp_earned > 1000)
                xp_to_add = 100;
            else if(_xp_earned > 100)
                xp_to_add = 10;
            else
                xp_to_add = 1;
        }
    }

    // If there is no XP to add this update cycle, there is nothing left for us to do
    if(xp_to_add == 0)
        return;

    // Add the XP amount to the characters appropriately
    std::deque<BattleCharacter *>& battle_characters = BattleMode::CurrentInstance()->GetCharacterActors();
    for(uint32_t i = 0; i < _characters_number; ++i) {
        // Don't add experience points to dead characters
        if(!battle_characters[i]->IsAlive())
            continue;

        // Tells whether the character can receive XP
        bool level_maxed_out = false;

        // Don't permit to earn XP when the maximum level has been reached.
        if(battle_characters[i]->GetGlobalCharacter()->GetExperienceLevel() >= GlobalManager->GetMaxExperienceLevel())
            level_maxed_out = true;

        uint32_t xp_added = xp_to_add;
        // Add the raw bonus when not given yet (+20% XP)
        if (_raw_xp_given[i] == false) {
            if (_xp_earned > 100) {
                xp_added += (xp_to_add / 5);
            }
            else {
                // When giving one xp point at a time,
                // we give all the rest of the raw bonus and set it as done.
                xp_added += _xp_earned / 5;
                _raw_xp_given[i] = true;
            }

        }

        if(!level_maxed_out && _characters[i]->AddExperiencePoints(xp_added)) {
            AudioManager->PlaySound("data/sounds/levelup.wav");
        }

        ustring level_text;
        ustring xp_text;
        ustring unspent_xp;
        if(level_maxed_out) {
            level_text = UTranslate("Level (Max): ") + MakeUnicodeString(NumberToString(_characters[i]->GetExperienceLevel()));
        } else {
            level_text = UTranslate("Level: ") + MakeUnicodeString(NumberToString(_characters[i]->GetExperienceLevel()));
            xp_text = UTranslate("XP left: ") + MakeUnicodeString(NumberToString(_characters[i]->GetExperienceForNextLevel()));
            if (_raw_xp_won[i])
                xp_text += UTranslate(" (+20%)");
            unspent_xp = UTranslate("XP available for skills: ")
                         + MakeUnicodeString(NumberToString(_characters[i]->GetUnspentExperiencePoints()));
        }

        _level_text[i].SetDisplayText(level_text);
        _xp_text[i].SetDisplayText(xp_text);
        _unspent_xp[i].SetDisplayText(unspent_xp);
    }

    _xp_earned -= xp_to_add;
    _SetHeaderText();
}

void FinishVictoryAssistant::_UpdateSpoils()
{
    // The number of milliseconds that we wait in between updating the drunes count
    const uint32_t UPDATE_PERIOD = 50;
    // A simple counter used to keep track of when the next drunes count should begin
    static uint32_t time_counter = 0;
    // TODO: Add drunes gradually instead of all at once
    static uint32_t drunes_to_add = 0;

    // Process confirm press inputs.
    if(InputManager->ConfirmPress()) {
        // Begin counting out drunes dropped
        if(!_begin_counting)
            _begin_counting = true;

        // If confirm received during counting, instantly add all remaining drunes
        else if(_drunes_dropped != 0) {
            drunes_to_add = _drunes_dropped;
        }
    }

    // If counting has not began or counting is alreasy finished, there is nothing more to do here
    if(!_begin_counting || (_drunes_dropped == 0))
        return;

    // Update the timer and determine how many drunes to add if the time has been reached
    // We don't want to modify the drunes to add if a confirm event occurred in step (1)
    if(drunes_to_add == 0) {
        time_counter += SystemManager->GetUpdateTime();
        if(time_counter >= UPDATE_PERIOD) {
            time_counter -= UPDATE_PERIOD;

            // Determine an appropriate amount of drunes to add here
            if(_drunes_dropped > 10000)
                drunes_to_add = 1000;
            else if(_drunes_dropped > 1000)
                drunes_to_add = 100;
            else if(_drunes_dropped > 100)
                drunes_to_add = 10;
            else
                drunes_to_add = 1;
        }
    }

    // Add drunes and update the display
    if(drunes_to_add != 0) {
        // Avoid making _drunes_dropped a negative value
        if(drunes_to_add > _drunes_dropped) {
            drunes_to_add = _drunes_dropped;
        }

        GlobalManager->AddDrunes(drunes_to_add);
        _drunes_dropped -= drunes_to_add;
        _SetHeaderText();
    }
}

void FinishVictoryAssistant::_DrawXP(uint32_t index)
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);
    VideoManager->Move(CHAR_WINDOW_XPOS - (CHAR_WINDOW_WIDTH / 2) + 20.0f,
                       (CHAR_WINDOW_YPOS + 17.0f) + (CHAR_WINDOW_HEIGHT * index));
    _character_portraits[index].Draw();

    _level_text[index].Draw();
    _xp_text[index].Draw();
    _unspent_xp[index].Draw();
}

void FinishVictoryAssistant::_DrawSpoils()
{
    _object_header_text.Draw();
    _object_list.Draw();
}

////////////////////////////////////////////////////////////////////////////////
// FinishSupervisor class
////////////////////////////////////////////////////////////////////////////////

FinishSupervisor::FinishSupervisor() :
    _state(FINISH_INVALID),
    _battle_victory(false),
    _defeat_assistant(_state),
    _victory_assistant(_state)
{
    _outcome_text.SetPosition(TOP_WINDOW_XPOS - TOP_WINDOW_WIDTH / 2.0f, 48.0f);
    _outcome_text.SetDimensions(TOP_WINDOW_WIDTH, 50.0f);
    _outcome_text.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _outcome_text.SetTextAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
    _outcome_text.SetDisplaySpeed(SystemManager->GetMessageSpeed());
    _outcome_text.SetTextStyle(TextStyle("text24", Color::white));
    _outcome_text.SetDisplayMode(VIDEO_TEXT_INSTANT);
}

void FinishSupervisor::Initialize(bool victory)
{
    _battle_victory = victory;
    _state = FINISH_ANNOUNCE_RESULT;

    if(_battle_victory) {
        _victory_assistant.Initialize();
        _outcome_text.SetDisplayText(UTranslate("The heroes were victorious!"));
    } else {
        _defeat_assistant.Initialize();
        _outcome_text.SetDisplayText(UTranslate("The heroes fell in battle..."));
    }
}

void FinishSupervisor::Update()
{
    if(_state == FINISH_ANNOUNCE_RESULT) {
        if(_battle_victory) {
            _state = FINISH_VICTORY_GROWTH;
        } else {
            _state = FINISH_DEFEAT_SELECT;
        }
        return;
    }

    if(_battle_victory) {
        _victory_assistant.Update();
    } else {
        _defeat_assistant.Update();
    }

    if(_state == FINISH_END) {
        if(_battle_victory) {
            BattleMode::CurrentInstance()->ChangeState(BATTLE_STATE_EXITING);
        }

        else {
            switch(_defeat_assistant.GetDefeatOption()) {
            case DEFEAT_OPTION_RETRY:
                BattleMode::CurrentInstance()->RestartBattle();
                break;
            case DEFEAT_OPTION_END:
                ModeManager->PopAll();
                ModeManager->Push(new vt_boot::BootMode(), false, true);
                break;
            default:
                IF_PRINT_WARNING(BATTLE_DEBUG)
                        << "invalid defeat option selected: "
                        << _defeat_assistant.GetDefeatOption() << std::endl;
                break;
            }
        }
    }
}

void FinishSupervisor::Draw()
{
    _outcome_text.Draw();

    if(_battle_victory) {
        _victory_assistant.Draw();
    } else {
        _defeat_assistant.Draw();
    }
}

} // namespace private_battle

} // namespace vt_battle
