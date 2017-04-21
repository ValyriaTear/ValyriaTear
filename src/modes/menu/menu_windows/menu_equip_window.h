///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_EQUIP_WINDOW__
#define __MENU_EQUIP_WINDOW__

#include "common/global/global.h"
#include "common/gui/textbox.h"

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

//! \brief The different equipment categories
enum EQUIP_CATEGORY {
    EQUIP_WEAPON = 0,
    EQUIP_HEAD = 1,
    EQUIP_TORSO = 2,
    EQUIP_ARMS = 3,
    EQUIP_LEGS = 4,
    EQUIP_CATEGORY_SIZE = 5
};

//! \brief The different option boxes that can be active for equipment
enum EQUIP_ACTIVE_OPTION {
    EQUIP_ACTIVE_NONE = 0,
    EQUIP_ACTIVE_CHAR = 1,
    EQUIP_ACTIVE_SELECT = 2,
    EQUIP_ACTIVE_LIST = 3,
    EQUIP_ACTIVE_SIZE = 4
};

/** ****************************************************************************
*** \brief Represents the Equipment window, allowing the player to change equipment.
***
*** This window changes a character's equipment.
*** You can choose a piece of equipment and replace with an item from the given list.
*** ***************************************************************************/
class EquipWindow : public vt_gui::MenuWindow
{
    friend class vt_menu::MenuMode;
    friend class InventoryState;
    friend class EquipState;
public:
    EquipWindow();

    ~EquipWindow()
    {}

    /*!
    * \brief Draws window
    * \return success/failure
    */
    void Draw();

    /*!
    * \brief Performs updates
    */
    void Update();

    /*!
    * \brief Checks to see if the equipment window is active
    * \return true if the window is active, false if it's not
    */
    bool IsActive() {
        return _active_box;
    }

    /*!
    * \brief Activates the window
    * \param new_value true to activate window, false to deactivate window
    * \param equip Tells Whether the window should be in equip or unequip mode.
    */
    void Activate(bool new_status, bool equip);

private:
    //! \brief Tells whether the window is in equip or unequip mode.
    bool _equip;

    //! Character selector
    vt_gui::OptionBox _char_select;

    //! Equipment selector
    vt_gui::OptionBox _equip_select;

    //! Replacement selector
    vt_gui::OptionBox _equip_list;

    //! \brief the items actual index in the replacor list
    //! Since not all the items are displayed in this list.
    std::vector<uint32_t> _equip_list_inv_index;

    //! Flag to specify the active option box
    uint32_t _active_box;

    //! equipment images
    std::vector<vt_video::StillImage> _equip_images;

    //! \brief The current character the equip window is dealing with.
    vt_global::GlobalCharacter* _character;

    //! \brief The current object the equip window is dealing with.
    std::shared_ptr<vt_global::GlobalObject> _object;

    //! \brief The different labels
    vt_video::TextImage _weapon_label;
    vt_video::TextImage _head_label;
    vt_video::TextImage _torso_label;
    vt_video::TextImage _arms_label;
    vt_video::TextImage _legs_label;

    //! \brief Set up char selector
    void _InitCharSelect();

    //! \brief Set up equipment selector
    void _InitEquipmentSelect();

    //! \brief Set up replacement selector
    void _InitEquipmentList();

    //! \brief Updates the equipment list
    void _UpdateEquipList();

    //! \brief Updates the selected object
    void _UpdateSelectedObject();

};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_EQUIP_WINDOW__
