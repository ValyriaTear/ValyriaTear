///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_QUEST_LIST_WINDOW__
#define __MENU_QUEST_LIST_WINDOW__

#include "common/global/global.h"
#include "common/gui/textbox.h"

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

/** \brief Represents the quest log list window on the left side
*** this holds the options box "list" that players can cycle through to look at
*** their quests (in Quest Window)
**/
class QuestListWindow : public vt_gui::MenuWindow {
    friend class vt_menu::MenuMode;
    friend class QuestState;
    friend class QuestWindow;
public:
    QuestListWindow();
    virtual ~QuestListWindow() override {}

    //! \brief Draws window
    void Draw() override;

    //! \brief Performs updates
    void Update() override;

    /** \brief Result of whether or not this window is active
     *  \return true if this window is active
    **/
    bool IsActive() override
    {
        return _active_box;
    }

private:
    //! \brief the selectable list of quests
    vt_gui::OptionBox _quests_list;

    //! The currently active quest log entries.
    std::vector<vt_global::QuestLogEntry*> _quest_entries;

    //! \brief indicates whether _quests_list is active or not
    bool _active_box;

    //! Setup the quests log list
    void _SetupQuestsList();

    //! \brief updates the side window quest list based on the current quest log entries
    void _UpdateQuestList();
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_QUEST_LIST_WINDOW__
