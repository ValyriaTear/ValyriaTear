///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/menu/menu_windows/menu_quest_list_window.h"

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

static const ustring spacing = MakeUnicodeString("<30>");
static const ustring new_quest_file =
    MakeUnicodeString("<data/gui/menus/star.png>") + spacing;
static const ustring check_file =
    MakeUnicodeString("<data/gui/menus/green_check.png>") + spacing;
static const ustring cross_file =
    MakeUnicodeString("<data/gui/menus/red_x.png>") + spacing;

QuestListWindow::QuestListWindow() :
    _active_box(false)
{
    _quests_list.SetPosition(92.0f, 145.0f);
    _quests_list.SetDimensions(330.0f, 375.0f, 1, 255, 1, 8);

    //set the cursor offset next to where the exclamation point would be.
    //this prevents the arrow jumping
    _quests_list.SetCursorOffset(-75.0f, -15.0f);
    _quests_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _quests_list.SetTextStyle(TextStyle("text20"));
    _quests_list.SetHorizontalWrapMode(VIDEO_WRAP_MODE_NONE);
    _quests_list.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);
    _quests_list.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);

    // Enable the viewing of grey options.
    _quests_list.SetSkipDisabled(false);

    _SetupQuestsList();
}

void QuestListWindow::Draw()
{
    // Draw the menu area.
    MenuWindow::Draw();

    // Draw the quest log list.
    _quests_list.Draw();
}

void QuestListWindow::Update()
{
    // Display the cursor.
    _quests_list.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);

    GlobalMedia& media = GlobalManager->Media();

    // If empty, exit out immediatly.
    if (GlobalManager->GetGameQuests().GetNumberQuestLogEntries() == 0)
    {
        media.PlaySound("cancel");
        _active_box = false;
        return;
    }

    // The quest log is fairly simple.
    // It only responds to up / down and cancel.
    if (InputManager->CancelPress()) {
        _quests_list.InputCancel();
    } else if (InputManager->UpPress()) {
        media.PlaySound("bump");
        _quests_list.InputUp();
    } else if (InputManager->DownPress()) {
        media.PlaySound("bump");
        _quests_list.InputDown();
    }

    uint32_t event = _quests_list.GetEvent();

    // Cancel and exit.
    if (event == VIDEO_OPTION_CONFIRM) {
        media.PlaySound("confirm");
    }
    else if (event == VIDEO_OPTION_CANCEL) {
        _active_box = false;
        _quests_list.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
        media.PlaySound("cancel");
    }

    // The standard update of the quest list.
    _UpdateQuestList();
}

/**
*** \brief sorts the quest log keys based on the actual quest entry number, putting the highest value at the top of the list
**/
static inline bool sort_by_number_reverse(QuestLogEntry* quest_log1,
                                          QuestLogEntry* quest_log2)
{
    return (quest_log1->GetQuestLogNumber() > quest_log2->GetQuestLogNumber());
}

void QuestListWindow::_UpdateQuestList()
{
    if(GlobalManager->GetGameQuests().GetNumberQuestLogEntries() == 0)
    {
        // Set the QuestWindow key to "nullptr", which is actually "".
        MenuMode::CurrentInstance()->_quest_window.SetViewingQuestId(std::string());
        return;
    }

    // Get the cursor selection.
    int32_t selection = _quests_list.GetSelection();

    QuestLogEntry *entry = _quest_entries[selection];
    const std::string& quest_id = entry->GetQuestId();
    ustring title = GlobalManager->GetGameQuests().GetQuestInfo(quest_id)._title;
    if (GlobalManager->GetGameQuests().IsQuestCompleted(quest_id))
    {
        _quests_list.SetOptionText(selection, check_file + title);
        _quests_list.SetCursorOffset(-55.0f, -15.0f);
        _quests_list.EnableOption(selection, false);
    }
    else if (!GlobalManager->GetGameQuests().IsQuestCompletable(quest_id)) {
        _quests_list.SetOptionText(selection, cross_file + title);
        _quests_list.SetCursorOffset(-55.0f, -15.0f);
        _quests_list.EnableOption(selection, false);
    }
    else
    {
        _quests_list.SetCursorOffset(-75.0f, -15.0f);
        _quests_list.SetOptionText(selection, spacing + title);
    }

    entry->SetRead();

    // Update the list box.
    _quests_list.Update(SystemManager->GetUpdateTime());

    // Set the QuestWindow quest key value to the selected quest.
    MenuMode::CurrentInstance()->_quest_window.SetViewingQuestId(quest_id);
}

void QuestListWindow::_SetupQuestsList() {
    // Recreate the quest log entries list.
    _quest_entries.clear();
    _quest_entries = GlobalManager->GetGameQuests().GetActiveQuestIds();

    // Recreate the quest option box list as well.
    _quests_list.ClearOptions();

    // Reorder by sorting via the entry number.
    std::sort(_quest_entries.begin(), _quest_entries.end(), sort_by_number_reverse);

    // Check whether some should be set as completed.
    for(uint32_t i = 0; i < _quest_entries.size(); ++i)
    {
        QuestLogEntry *entry = _quest_entries[i];
        const std::string& quest_id = entry->GetQuestId();
        ustring title = GlobalManager->GetGameQuests().GetQuestInfo(quest_id)._title;

        // Completed quest check.
        if(GlobalManager->GetGameQuests().IsQuestCompleted(quest_id)) {
            _quests_list.AddOption(check_file + title);
            _quests_list.EnableOption(i, false);
        }
        else if (!GlobalManager->GetGameQuests().IsQuestCompletable(quest_id)) {
            _quests_list.AddOption(cross_file + title);
            _quests_list.EnableOption(i, false);
        }

        // If incomplete, then check the read status.
        else if(entry->IsRead())
            _quests_list.AddOption(spacing + title);
        else
            _quests_list.AddOption(new_quest_file + title);
    }
}

} // namespace private_menu

} // namespace vt_menu
