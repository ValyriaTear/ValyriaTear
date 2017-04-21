///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_BATTLE_FORMATION_WINDOW__
#define __MENU_BATTLE_FORMATION_WINDOW__

#include "common/global/global.h"
#include "common/gui/textbox.h"

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

/** ****************************************************************************
*** \brief Represents the currently chosen battle window.
***
*** This window will display the different battle formation available and their impact
*** on the character chances to be targeted, attack, and defense.
*** ***************************************************************************/
class BattleFormationWindow : public vt_gui::MenuWindow
{
    friend class vt_menu::MenuMode;

public:
    BattleFormationWindow();

    virtual ~BattleFormationWindow()
    {}

    /*!
    * \brief render this window to the screen
    * \return success/failure
    */
    void Draw();

    /*!
    * \brief update function handles input to the window
    */
    void Update();

    /*!
    * \brief Get status window active state
    * \return the char select value when active, or zero if inactive
    */
    inline bool GetActiveState() const {
        return _formation_select_active;
    }

    /*!
    * \brief Active this window
    * \param activated true to activate window, false to deactivate window
    */
    void Activate(bool activated);

    //! \brief Updates the window content
    void UpdateStatus();

private:
    //! characters
    std::vector<vt_video::AnimatedImage> _character_sprites;

    //! \brief If the window is active or not
    bool _formation_select_active;

    //! \brief Character selection option box
    vt_gui::OptionBox _formation_select;

    //! \brief The characters status
    std::vector<vt_video::TextImage> _character_target_texts;

    //! \brief The rear/front status (true if on front)
    std::vector<bool> _characters_position;

    //! \brief Some help text displayed in the bottom window.
    vt_video::TextImage _help_text;
    vt_video::TextImage _rear_front_txt;
    vt_video::TextImage _modifier_txt;

    //! \brief Draws info in the bottom window.
    void _DrawBottomWindowInfo();

    //! \brief Updates the modificators depending on the characters modifications.
    void _ComputeModificators();
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_BATTLE_FORMATION_WINDOW__
