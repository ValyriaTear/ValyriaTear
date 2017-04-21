///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_SKILLS_WINDOW__
#define __MENU_SKILLS_WINDOW__

#include "common/global/global.h"
#include "common/gui/textbox.h"

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

//! \brief The different skill types
enum SKILL_CATEGORY {
    SKILL_ALL = 0,
    SKILL_FIELD = 1,
    SKILL_BATTLE = 2,
    SKILL_CATEGORY_SIZE = 3
};

//! \brief The different option boxes that can be active for skills
enum SKILL_ACTIVE_OPTION {
    SKILL_ACTIVE_NONE = 0,
    SKILL_ACTIVE_CHAR = 1,
    SKILL_ACTIVE_CATEGORY = 2,
    SKILL_ACTIVE_LIST = 3,
    SKILL_ACTIVE_CHAR_APPLY = 4,
    SKILL_ACTIVE_SIZE = 5
};

/** ****************************************************************************
*** \brief Represents the Skills window, displaying all the skills for the character.
***
*** This window display all the skills for a particular character.
*** You can scroll through them all, filter by category, choose one, and apply it
*** to a character.
*** ***************************************************************************/
class SkillsWindow : public vt_gui::MenuWindow
{
    friend class vt_menu::MenuMode;
    friend class SkillsState;
public:
    SkillsWindow();

    ~SkillsWindow()
    {}

    /*!
    * \brief Updates key presses and window states
    */
    void Update();

    /*!
    * \brief Draws the windows and option boxes
    * \return success/failure
    */
    void Draw();

    /*!
    * \brief Activates the window
    * \param new_value true to activate window, false to deactivate window
    */
    void Activate(bool new_status);

    /*!
    * \brief Checks to see if the skills window is active
    * \return true if the window is active, false if it's not
    */
    bool IsActive() {
        return _active_box;
    }

private:
    //! Flag to specify the active option box
    uint32_t _active_box;

    //! The character select option box
    vt_gui::OptionBox _char_select;

    //! The skills categories option box
    vt_gui::OptionBox _skills_categories;

    //! The skills list option box
    vt_gui::OptionBox _skills_list;

    //! The skill SP cost option box
    vt_gui::OptionBox _skill_cost_list;

    //! TextBox that holds the selected skill's description
    vt_gui::TextBox _description;

    //! The current skill icon, if any
    vt_video::StillImage _skill_icon;

    //! Track which character's skillset was chosen
    int32_t _char_skillset;

    /*!
    * \brief Initializes the skills category chooser
    */
    void _InitSkillsCategories();

    /*!
    * \brief Initializes the skills chooser
    */
    void _InitSkillsList();

    /*!
    * \brief Initializes the character selector
    */
    void _InitCharSelect();

    //! \brief Returns the currently selected skill
    vt_global::GlobalSkill *_GetCurrentSkill();

    /*!
    * \brief Sets up the skills that comprise the different categories
    */
    void _UpdateSkillList();

    vt_utils::ustring _BuildSkillListText(const vt_global::GlobalSkill *skill);

    //! \brief parses the 3 skill lists of the global character and sorts them according to use (menu/battle)
    void _BuildMenuBattleSkillLists(std::vector<vt_global::GlobalSkill *> *skill_list,
                                    std::vector<vt_global::GlobalSkill *> *field, std::vector<vt_global::GlobalSkill *> *battle,
                                    std::vector<vt_global::GlobalSkill *> *all);

};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_SKILLS_WINDOW__
