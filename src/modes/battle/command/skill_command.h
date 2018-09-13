////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_SKILL_COMMAND_HEADER__
#define __BATTLE_SKILL_COMMAND_HEADER__

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_global {
class GlobalSkill;
}

namespace vt_battle
{

namespace private_battle
{

/** ****************************************************************************
*** \brief Manages the display of skills and skill information
***
*** Although this class manages lists of skills, it does not contain those skills
*** directly. They are contained externally on a per-character basis by the
*** CharacterCommandSettings class.
*** ***************************************************************************/
class SkillCommand
{
public:
    //! \param window A reference to the MenuWindow that the GUI objects should be owned by
    SkillCommand(vt_gui::MenuWindow& window);

    ~SkillCommand()
    {}

    /** \brief Initializes the class
    *** \param skills A pointer to the set of skills that the class will
    *** \param skill_list A pointer to the skill list option box
    *** \param target_n_cost_list A pointer to the target typ and skill cost option box
    **/
    void Initialize(std::vector<vt_global::GlobalSkill *>* skills,
                    vt_gui::OptionBox *skill_list,
                    vt_gui::OptionBox *target_n_cost_list);

    /** \brief Returns the currently selected skill
    *** This function will return nullptr if the class has not been initialized and there is no list of
    *** skills to select from.
    **/
    vt_global::GlobalSkill* GetSelectedSkill() const;

    /** \brief Returns if the selected skill is enabled
    *** This function will return true, if the selected skill is enabled and false otherwise. It will
    *** also return false, if the class has not been initialized and there is no list of
    *** skills to select from.
    **/
    bool GetSelectedSkillEnabled();

    //! \brief Updates the skill list and processes user input
    void UpdateList();

    //! \brief Draws the skill header and list
    void DrawList();

private:
    //! \brief A pointer to the vector of skills corresponding to the options in _skill_list
    std::vector<vt_global::GlobalSkill *>* _skills;

    //! \brief A single line of header text for the skill list option box
    vt_gui::OptionBox _skill_header;

    //! \brief A pointer to the list of skills that the class should operate on
    vt_gui::OptionBox* _skill_list;
    //! \brief A pointer to the list of skills targets and cost
    vt_gui::OptionBox* _target_n_cost_list;
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_SKILL_COMMAND_HEADER__
