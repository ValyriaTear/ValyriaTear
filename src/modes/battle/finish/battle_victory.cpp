////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "modes/battle/finish/battle_victory.h"

#include "modes/battle/battle.h"
#include "modes/battle/objects/battle_character.h"
#include "modes/battle/objects/battle_enemy.h"

#include "modes/menu/menu_mode.h"

#include "common/global/global.h"
#include "common/global/actors/global_character.h"

#include "common/gui/menu_window.h"

#include "engine/video/video_utils.h"
#include "engine/system.h"
#include "engine/input.h"
#include "engine/audio/audio.h"

#include <assert.h>

using namespace vt_gui;
using namespace vt_video;
using namespace vt_system;
using namespace vt_global;
using namespace vt_input;
using namespace vt_utils;
using namespace vt_audio;

namespace vt_battle
{

namespace private_battle
{

//! \brief Draw position and dimension constants used for GUI objects
//@{
const float TOP_WINDOW_XPOS        = 240.0f;
const float TOP_WINDOW_YPOS        = 104.0f;
const float TOP_WINDOW_WIDTH       = 600.0f;
const float TOP_WINDOW_HEIGHT      = 64.0f;

const float CHAR_WINDOW_XPOS       = 360.0f;
const float CHAR_WINDOW_YPOS       = 104.0f+ TOP_WINDOW_HEIGHT - 1.0f;
const float CHAR_WINDOW_WIDTH      = 380.0f;
const float CHAR_WINDOW_HEIGHT     = 118.0f;

const float SPOILS_WINDOW_XPOS     = 550.0f;
const float SPOILS_WINDOW_YPOS     = 104.0f + TOP_WINDOW_HEIGHT - 1.0f;
const float SPOILS_WINDOW_WIDTH    = 360.0f;
const float SPOILS_WINDOW_HEIGHT   = 320.0f;
//@}

//! \brief The set of victory options that the player can select
//@{
//! End battle
const uint32_t VICTORY_OPTION_END_BATTLE     = 0;
//! Improve skills
const uint32_t VICTORY_OPTION_IMPROVE_SKILLS = 1;
//@}

BattleVictory::BattleVictory() :
    _state(VICTORY_INVALID),
    _characters_number(0),
    _xp_earned(0),
    _drunes_dropped(0),
    _begin_counting_xp(false),
    _begin_counting_drunes(false),
    _number_character_windows_created(0)
{
    _outcome_text.SetPosition(212.0f, 30.0f);
    _outcome_text.SetDimensions(512.0f, 50.0f);
    _outcome_text.SetTextStyle(TextStyle("text28", Color::white));
    _outcome_text.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _outcome_text.SetDisplayText(UTranslate("The heroes were victorious!"));

    _header_window.Create(TOP_WINDOW_WIDTH, TOP_WINDOW_HEIGHT, ~VIDEO_MENU_EDGE_BOTTOM, VIDEO_MENU_EDGE_BOTTOM);
    _header_window.SetPosition(TOP_WINDOW_XPOS, TOP_WINDOW_YPOS);
    _header_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _header_window.Show();

    _spoils_window.Create(SPOILS_WINDOW_WIDTH, SPOILS_WINDOW_HEIGHT);
    _spoils_window.SetPosition(SPOILS_WINDOW_XPOS, SPOILS_WINDOW_YPOS);
    _spoils_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _spoils_window.Show();

    _header_xp.SetOwner(&_header_window);
    _header_xp.SetPosition(75.0f, 10.0f);
    _header_xp.SetDimensions(TOP_WINDOW_WIDTH / 5.0f * 4.0f, 40.0f);
    _header_xp.SetTextStyle(TextStyle("text20", Color::white));
    _header_xp.SetDisplayMode(VIDEO_TEXT_INSTANT);

    // Reset raw XP bonus
    for(uint32_t i = 0; i < 4; i++) {
        _raw_xp_given[i] = true;
        _raw_xp_won[i] = false;
    }

    _drunes_dropped_text.SetOwner(&_header_window);
    _drunes_dropped_text.SetPosition(TOP_WINDOW_WIDTH / 5.0f * 3.1f, 10.0f);
    _drunes_dropped_text.SetDimensions(TOP_WINDOW_WIDTH / 5.0f * 2.0f, 40.0f);
    _drunes_dropped_text.SetTextStyle(TextStyle("text20", Color::white));
    _drunes_dropped_text.SetDisplayMode(VIDEO_TEXT_INSTANT);

    _total_drunes.SetOwner(&_spoils_window);
    _total_drunes.SetPosition(SPOILS_WINDOW_WIDTH / 4.0f, 0.0f);
    _total_drunes.SetDimensions(SPOILS_WINDOW_WIDTH / 4.0f * 3.0f, 40.0f);
    _total_drunes.SetTextStyle(TextStyle("text20", Color::white));
    _total_drunes.SetDisplayMode(VIDEO_TEXT_INSTANT);

    _object_header_text.SetOwner(&_spoils_window);
    _object_header_text.SetPosition(SPOILS_WINDOW_WIDTH / 4.0f, 50.0f);
    _object_header_text.SetDimensions(SPOILS_WINDOW_WIDTH / 4.0f * 3.0f, 40.0f);
    _object_header_text.SetTextStyle(TextStyle("title20", Color::white));
    _object_header_text.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _object_header_text.SetDisplayText(UTranslate("Items Found"));

    _object_list.SetOwner(&_spoils_window);
    _object_list.SetPosition(50.0f, 100.0f);
    _object_list.SetDimensions(250.0f, 180.0f, 1, 8, 1, 8);
    _object_list.SetTextStyle(TextStyle("text20", Color::white));
    _object_list.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _object_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _object_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);

    _victory_options.SetOwner(&_header_window);
    _victory_options.SetPosition(TOP_WINDOW_WIDTH / 2, 28.0f);
    _victory_options.SetDimensions(480.0f, 50.0f, 2, 1, 2, 1);
    _victory_options.SetTextStyle(TextStyle("title22", Color::white, VIDEO_TEXT_SHADOW_DARK));
    _victory_options.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _victory_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _victory_options.SetSelectMode(VIDEO_SELECT_SINGLE);
    _victory_options.SetHorizontalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _victory_options.SetCursorOffset(-60.0f, -25.0f);
    _victory_options.AddOption(UTranslate("End Battle"));
    _victory_options.AddOption(UTranslate("Improve Skills"));
    _victory_options.SetSelection(0);
}

BattleVictory::~BattleVictory()
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
        GlobalManager->GetInventoryHandler().AddToInventory(i->first->GetID(), i->second);
    }
    _objects_dropped.clear();

    // Update the HP and SP of all characters.
    _SetCharacterStatus();
}

void BattleVictory::Initialize()
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

    // Initialize dynamic texts
    _header_xp.SetDisplayText(UTranslate("XP Earned: ") + MakeUnicodeString(NumberToString(_xp_earned)));
    _drunes_dropped_text.SetDisplayText(UTranslate("Drunes Found: ") + MakeUnicodeString(NumberToString(_drunes_dropped)));
    _total_drunes.SetDisplayText(UTranslate("Total Drunes: ") + MakeUnicodeString(NumberToString(GlobalManager->GetDrunes())));

    // Start victory
    _state = VICTORY_START;
}

void BattleVictory::Update()
{
    switch(_state) {
    case VICTORY_START:
        // Go to exit menu once count is done
        if (_xp_earned == 0 && _drunes_dropped == 0) {
            _state = VICTORY_END;
        }

        _UpdateXP();
        _UpdateSpoils();
        break;

    case VICTORY_END:
        _UpdateEndMenu();
        break;

    default:
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid finish state: " << _state << std::endl;
        break;
    }
}

void BattleVictory::Draw()
{
    _outcome_text.Draw();

    // Header Windows
    _header_window.Draw();

    if (_state == VICTORY_END) {
        _victory_options.Draw();
    }
    else {
        _header_xp.Draw();
        _drunes_dropped_text.Draw();
    }

    for(uint32_t i = 0; i < _characters_number; ++i) {
        _character_window[i].Draw();
        _DrawXP(i);
    }

    // Items and drunes
    _spoils_window.Draw();
    _total_drunes.Draw();
    _object_header_text.Draw();
    _object_list.Draw();
}

void BattleVictory::_CreateCharacterGUIObjects()
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
        _level_text[i].SetPosition(130.0f, 20.0f);
        _level_text[i].SetDimensions(300.0f, 25.0f);
        _level_text[i].SetTextStyle(TextStyle("text20", Color::white));
        _level_text[i].SetDisplayMode(VIDEO_TEXT_INSTANT);

        _xp_text[i].SetOwner(&_character_window[i]);
        _xp_text[i].SetPosition(130.0f, 50.0f);
        _xp_text[i].SetDimensions(300.0f, 25.0f);
        _xp_text[i].SetTextStyle(TextStyle("text20", Color::white));
        _xp_text[i].SetDisplayMode(VIDEO_TEXT_INSTANT);

        _unspent_xp[i].SetOwner(&_character_window[i]);
        _unspent_xp[i].SetPosition(130.0f, 80.0f);
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
            _xp_text[i].SetDisplayText(UTranslate("XP for level up: ")
                                       + MakeUnicodeString(NumberToString(_characters[i]->GetExperienceForNextLevel())));
            _unspent_xp[i].SetDisplayText(UTranslate("XP for skills: ")
                                          + MakeUnicodeString(NumberToString(_characters[i]->GetUnspentExperiencePoints())));
        }
    }
}

void BattleVictory::_CreateObjectList()
{
    // Add all dropped items in the list
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

    // Set item found title accordingly
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

void BattleVictory::_SetCharacterStatus()
{
    std::deque<BattleCharacter *>& battle_characters = BattleMode::CurrentInstance()->GetCharacterActors();

    for(std::deque<BattleCharacter *>::iterator i = battle_characters.begin(); i != battle_characters.end(); ++i) {
        GlobalCharacter *character = (*i)->GetGlobalCharacter();

        // Put back the current HP / SP onto the global characters.
        character->SetHitPoints((*i)->GetHitPoints());
        character->SetSkillPoints((*i)->GetSkillPoints());
    }
}

void BattleVictory::_UpdateXP()
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
        if(!_begin_counting_xp) {
            _begin_counting_xp = true;
        }
        // If confirm received during counting, instantly add all remaining XP
        else if(_xp_earned != 0) {
            xp_to_add = _xp_earned;
        }
    }

    // If counting has not began or counting is alreasy finished, there is nothing more to do here
    if(!_begin_counting_xp || (_xp_earned == 0))
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
            xp_text = UTranslate("XP for level up: ") + MakeUnicodeString(NumberToString(_characters[i]->GetExperienceForNextLevel()));
            if (_raw_xp_won[i])
                xp_text += UTranslate(" (+20%)");
            unspent_xp = UTranslate("XP for skills: ")
                         + MakeUnicodeString(NumberToString(_characters[i]->GetUnspentExperiencePoints()));
        }

        _level_text[i].SetDisplayText(level_text);
        _xp_text[i].SetDisplayText(xp_text);
        _unspent_xp[i].SetDisplayText(unspent_xp);
    }

    _xp_earned -= xp_to_add;
    _header_xp.SetDisplayText(UTranslate("XP Earned: ") + MakeUnicodeString(NumberToString(_xp_earned)));
}

void BattleVictory::_UpdateSpoils()
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
        if(!_begin_counting_drunes)
            _begin_counting_drunes = true;

        // If confirm received during counting, instantly add all remaining drunes
        else if(_drunes_dropped != 0) {
            drunes_to_add = _drunes_dropped;
        }
    }

    // If counting has not began or counting is alreasy finished, there is nothing more to do here
    if(!_begin_counting_drunes || (_drunes_dropped == 0))
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
        _drunes_dropped_text.SetDisplayText(UTranslate("Drunes Found: ") + MakeUnicodeString(NumberToString(_drunes_dropped)));
        _total_drunes.SetDisplayText(UTranslate("Total Drunes: ") + MakeUnicodeString(NumberToString(GlobalManager->GetDrunes())));
    }
}

void BattleVictory::_UpdateEndMenu()
{
    _victory_options.Update();

    if (InputManager->LeftPress()) {
        _victory_options.InputLeft();
    }
    else if (InputManager->RightPress()) {
        _victory_options.InputRight();
    }
    else if (InputManager->ConfirmPress()) {
        switch(_victory_options.GetSelection()) {
        default:
        case VICTORY_OPTION_END_BATTLE:
            BattleMode::CurrentInstance()->ChangeState(BATTLE_STATE_EXITING);
            break;
        case VICTORY_OPTION_IMPROVE_SKILLS:
            vt_menu::MenuMode* menu_mode = new vt_menu::MenuMode();
            menu_mode->GoToImproveSkillMenu();
            _victory_options.SetSelection(VICTORY_OPTION_END_BATTLE);
            vt_mode_manager::ModeManager->Push(menu_mode);
            break;
        }
    }
}

void BattleVictory::_DrawXP(uint32_t index)
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, 0);
    VideoManager->Move(CHAR_WINDOW_XPOS - (CHAR_WINDOW_WIDTH / 2) + 20.0f,
                       (CHAR_WINDOW_YPOS + 17.0f) + (CHAR_WINDOW_HEIGHT * index));
    _character_portraits[index].Draw();

    _level_text[index].Draw();
    _xp_text[index].Draw();
    _unspent_xp[index].Draw();
}

} // namespace private_battle

} // namespace vt_battle
