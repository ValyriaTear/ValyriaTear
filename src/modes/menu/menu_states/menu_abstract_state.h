///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_ABSTRACT_STATE_HEADER__
#define __MENU_ABSTRACT_STATE_HEADER__

#include "common/gui/option.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

//! \brief Functions that initialize the numerous option boxes
void SetupOptionBoxCommonSettings(vt_gui::OptionBox* ob);

/**
*** \brief Defines a single menu state, which includes the currently viewing parameters and transition states
***
*** Each time we have a menu transition, we have a new visual layout, and selectable parameters
*** that we can choose from. Essentially, a single menu is made up of:
*** 1) The currently available options
*** 2) The Menu we entered this weindow from
*** 3) The Menu we wish to transition to when an option is selected
***   OR
*** 4) The "Active Mode" for the current menu itself
***
*** The AbstractMenuState handles most of the logic dealing with transitioning and updating the states
*** It is up to the programmer to create the actual state itself and define its transition and
*** rendering information
**/
class AbstractMenuState {
public:
    //! \brief base constructor
    //! \param state_name The individual name of the state that we are currently in. Mostly for debugging
    //! \param menu_mode Pointer to the active menu mode that we associate this state with (currently only one)
    AbstractMenuState(const std::string& state_name, MenuMode* menu_mode);

    virtual ~AbstractMenuState()
    {}

    //! \brief used when the MenuMode is activated.
    //! \note While we provide a default "do nothing" Reset(), each child type should have its own Reset implementation
    virtual void Reset()
    {}

    //! \brief handles the drawing of the state
    void Draw();

    //! \brief handles updating the state
    void Update();

    /**
    *** \brief based on the selection (pased in via the OptionBox selection returns the next state to transition to
    *** \param selection selection state indicator
    *** \return either a valid AbstractMenuState pointer to the next valid transition state, or nullptr
    *** \note nullptr actions do not lead to a crash, but the assumption is that the state has some state-specific
    *** actions, such as activitaing a MenuView that take place
    **/
    virtual AbstractMenuState* GetTransitionState(uint32_t selection) = 0;

    //! \brief returns the name of the state
    const std::string& GetStateName()
    { return _state_name; }

    //! \brief returns a pointer to the OptionsBox associated with this state
    vt_gui::OptionBox *GetOptions()
    { return &_options; }

protected:
    //! \brief default bottom menu drawing
    virtual void _DrawBottomMenu();

    //! \brief action that takes place when we exit this state via a "cancel" input
    void _OnCancel();

    //! \brief returns the default selection to use when we first load the menu after MenuMode is at the top of stack
    virtual uint32_t _GetDefaultSelection()
    { return 0; }

    //! \brief handles updating the state when it is claimed as "active"
    virtual void _ActiveWindowUpdate()
    {}

    //! \brief returns wether or not the state is active
    virtual bool _IsActive()
    { return false; }

    //! \brief instance-specific main window drawing code goes in here. the default is to simply draws nothing
    virtual void _OnDrawMainWindow()
    {}

    //! \brief draws the side window. Default draws the character windows
    virtual void _OnDrawSideWindow();

    //! \brief called when there is no state transition or active state
    //! to handle any state-specific updates that need to occur
    virtual void _OnUpdateState()
    {};

    // Options associated with this state
    vt_gui::OptionBox _options;

    //! \brief State-specific name
    std::string _state_name;

    //! \brief A pointer to the active MenuMode
    MenuMode* _menu_mode;

    //! \brief A pointer to the state we should return to on a "cancel" press.
    AbstractMenuState* _from_state;
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_ABSTRACT_STATE_HEADER__
