///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_PARTY_WINDOW__
#define __MENU_PARTY_WINDOW__

#include "common/global/global.h"
#include "common/gui/textbox.h"

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

//! \brief The different option boxes that can be active for party formation
enum FORM_ACTIVE_OPTION {
    FORM_ACTIVE_NONE = 0,
    FORM_ACTIVE_CHAR = 1,
    FORM_ACTIVE_SECOND = 2,
    FORM_ACTIVE_SIZE = 3
};

/** ****************************************************************************
*** \brief Represents the Party window, displaying all the information about the character.
***
*** This window display all the attributes of the character.
*** You can scroll through them all as well, to view all the different characters.
*** You can also reorder the position of characters
*** ***************************************************************************/
class PartyWindow : public vt_gui::MenuWindow
{
    friend class vt_menu::MenuMode;

public:
    PartyWindow();

    ~PartyWindow()
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
    inline uint32_t GetActiveState() const {
        return _char_select_active;
    }

    /*!
    * \brief Active this window
    * \param new_value true to activate window, false to deactivate window
    */
    void Activate(bool new_value);

    //! \brief Updates the status text (and icons)
    void UpdateStatus();

private:
    //! char portraits
    std::vector<vt_video::StillImage> _full_portraits;

    //! if the window is active or not
    uint32_t _char_select_active;

    //! character selection option box
    vt_gui::OptionBox _char_select;

    //! The character select option box once first character has been selected
    vt_gui::OptionBox _second_char_select;

    //! \brief The character status
    vt_video::TextImage _character_status_text;
    vt_video::TextImage _character_status_numbers;
    vt_video::StillImage _character_status_icons;

    //! \brief Some help text displayed in the middle window
    //! To give some place in the bottom window for equipment.
    vt_video::TextImage _help_text;

    //! \brief Text and image used to display equipment info
    //! The average and focused (attack points) atk/dev labels.
    vt_video::TextImage _average_text;
    vt_video::TextImage _focused_text;
    //! The average atk/def image and text
    vt_video::TextImage _average_atk_def_text;
    vt_video::TextImage _average_atk_def_numbers;
    vt_video::StillImage _average_atk_def_icons;
    vt_video::StillImage _weapon_icon;

    //! The focused def text and images
    //! Do not delete those pointers, they are handled by the GlobalMedia class
    vt_video::StillImage* _focused_def_icon;
    vt_video::StillImage* _focused_mdef_icon;

    vt_video::TextImage _focused_def_text;
    vt_video::TextImage _focused_def_numbers;
    vt_video::TextImage _focused_mdef_numbers;
    vt_video::StillImage _focused_def_category_icons;
    //! The actual character armor icon if any
    vt_video::StillImage _focused_def_armor_icons[4];

    //! \brief Draws equipment stat info in the bottom window.
    void _DrawBottomEquipmentInfo();

    //! \brief initialize character selection option box
    void _InitCharSelect();

};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_PARTY_WINDOW__
