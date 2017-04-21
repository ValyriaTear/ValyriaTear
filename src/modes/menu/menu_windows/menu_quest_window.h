///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_QUEST_WINDOW__
#define __MENU_QUEST_WINDOW__

#include "common/global/global.h"
#include "common/gui/textbox.h"

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

/**
*** \brief Represents the quest log main window
*** players can view their active quests as well as completed quests when this window is viewing
**/
class QuestWindow : public vt_gui::MenuWindow {

    friend class vt_menu::MenuMode;
    friend class QuestState;

public:
    QuestWindow();
    virtual ~QuestWindow() override {}

    //! \brief Draws window
    void Draw();

    /*!
    * \brief Draws the bottom window information
    * \note this only draws the location name and banner. we assume the
    * calling function draws the actual window and frame
    */
    void DrawBottom();

    //! \brief clears the info out.
    void ClearBottom()
    {
        _location_name.ClearText();
        _location_subname.ClearText();
        _location_image = nullptr;
        _location_subimage = nullptr;
    }

    //! \brief Performs updates
    void Update();

    //! \brief sets the viewing quest id information for the quest.
    //! We use this to query the text description
    void SetViewingQuestId(const std::string &quest_id)
    {
        _viewing_quest_id = quest_id;
    }

private:
    //! \brief the currently viewing quest id. this is set by the Quest List Window through the
    //! SetViewingQuestId() function
    std::string _viewing_quest_id;

    //! \brief sets the display text to be rendered, based on their quest key that is set
    vt_gui::TextBox _quest_description;
    //! \brief sets the display text to be rendered when the quest is completed. this is additional info
    vt_gui::TextBox _quest_completion_description;

    //! \brief the display text to be rendered for the location name and subname that the quest key is set to
    vt_gui::TextBox _location_name;
    vt_gui::TextBox _location_subname;

    //! \brief the currently viewing location image and location subimage
    const vt_video::StillImage* _location_image;
    const vt_video::StillImage* _location_subimage;
};

} // namespace private_menu

} // namespace vt_menu

#endif
