////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_menu.h
*** \author  Christopher Berman, suitecake@gmail.com
*** \brief   Header file for battle sub-menu
***
*** This code is responsible for both the display and logic of the main battle
*** menu, which includes options such as Rush. This class is directly tied to
*** the CommandSupervisor class.
*** ***************************************************************************/

#ifndef __BATTLE_MENU_HEADER__
#define __BATTLE_MENU_HEADER__

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_battle
{

namespace private_battle
{
//! \brief Encapsulates the battle menu
class BattleMenu
{
public:
    //! \brief The class constructor generates the GUI
    BattleMenu();

    ~BattleMenu();

    //! \brief Draws the menu
    void Draw();

    //! \brief Updates the menu state
    void Update();

    //! \brief Opens the menu
    inline void Open()
    { _open = true; }

    //! \brief Closes the menu
    inline void Close()
    { _open = false; }

    //! \brief Sets whether auto-battle mode is active
    void SetAutoBattleActive(bool active);

    //! \brief Returns whether auto-battle mode is active
    inline bool IsAutoBattleActive() const
    { return _auto_battle_active; }

    //! \brief Returns whether the menu is open
    inline bool IsOpen() const
    { return _open; }

private:
    //! \brief The window where all information about the currently selected action is drawn
    vt_gui::MenuWindow _window;

    //! \brief The list of menu options
    vt_gui::OptionBox _options_list;

    //! \brief Whether auto-battle mode is active.
    bool _auto_battle_active;

    //! \brief Whether the menu is open
    bool _open;

    //! \brief Redraws the options
    void _RefreshOptions();
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_MENU_HEADER__
