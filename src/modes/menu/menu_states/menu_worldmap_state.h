///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_WORLDMAP_STATE_HEADER__
#define __MENU_WORLDMAP_STATE_HEADER__

#include "modes/menu/menu_states/menu_abstract_state.h"

#include "common/gui/textbox.h"

namespace vt_menu {

class MenuMode;

namespace private_menu {

/**
*** \brief World Map State
*** Allows player to view the currently enabled world map,
*** along with the viewable locations on it
**/
class WorldMapState : virtual public AbstractMenuState
{
public:
    explicit WorldMapState(MenuMode* menu_mode);

    ~WorldMapState() {}

    void Reset();

    AbstractMenuState *GetTransitionState(uint32_t /*selection*/)
    { return nullptr; }
protected:
    void _OnDrawMainWindow();

    //! \brief this function is overridden to do nothing, as the side window is not used in map mode
    inline void _OnDrawSideWindow()
    {}

    void _DrawBottomMenu();

    void _ActiveWindowUpdate();

    bool _IsActive();

    vt_gui::TextBox _location_text;
    vt_video::StillImage* _location_image;
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_WORLDMAP_STATE_HEADER__
