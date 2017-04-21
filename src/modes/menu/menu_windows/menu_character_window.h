///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_CHARACTER_WINDOW__
#define __MENU_CHARACTER_WINDOW__

#include "common/global/global.h"
#include "common/gui/menu_window.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

/** ****************************************************************************
*** \brief Represents an individual character window
***
*** There should be one of these windows for each character in the game.
*** It will contain all the information of the character and handle its draw
*** placement.
*** ***************************************************************************/
class CharacterWindow : public vt_gui::MenuWindow
{
public:
    CharacterWindow();

    ~CharacterWindow()
    {}

    /** \brief Set the character for this window
    *** \param character the character to associate with this window
    **/
    void SetCharacter(vt_global::GlobalCharacter* character);

    /** \brief render this window to the screen
    *** \return success/failure
    **/
    void Draw();

private:
    //! \brief The name of the character that this window corresponds) to
    uint32_t _char_id;

    //! \brief The image of the character
    vt_video::StillImage _portrait;

    //! \brief The text along with the character portrait
    vt_video::TextImage _character_name;
    vt_video::TextImage _character_data;

    //! \brief The hp/sp icon displayed in this window.
    //! \note Do not delete them, as they are handled by the GlobalMedia class.
    vt_video::StillImage* _hp_icon;
    vt_video::StillImage* _sp_icon;

    //! \brief The character active status effects images.
    //! \note Do not delete them, as they are handled by the GlobalMedia class.
    std::vector<vt_video::StillImage*> _active_status_effects;

    /** \brief Refreshes the active status effects images vector content.
    *** \param character the character to check status effects for.
    **/
    void _UpdateActiveStatusEffects(vt_global::GlobalCharacter* character);
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_CHARACTER_WINDOW__
