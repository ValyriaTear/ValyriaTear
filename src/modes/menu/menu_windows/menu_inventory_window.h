///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_INVENTORY_WINDOW__
#define __MENU_INVENTORY_WINDOW__

#include "common/global/global.h"
#include "common/gui/textbox.h"

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

//! \brief The different option boxes that can be active for items
enum ITEM_ACTIVE_OPTION {
    ITEM_ACTIVE_NONE = 0,
    ITEM_ACTIVE_CATEGORY = 1,
    ITEM_ACTIVE_LIST = 2,
    ITEM_ACTIVE_CHAR = 3,
    ITEM_ACTIVE_SIZE = 4
};

/** ****************************************************************************
*** \brief Represents the inventory window to browse the party's inventory
***
*** This handles item use.  You can also view all items by category.
*** ***************************************************************************/
class InventoryWindow : public vt_gui::MenuWindow
{
    friend class vt_menu::MenuMode;
    friend class InventoryState;

public:
    explicit InventoryWindow(MenuMode *);

    ~InventoryWindow()
    {}

    /** \brief Toggles the inventory window being in the active context for the player
    *** \param new_status Activates the inventory window when true, de-activates it when false
    **/
    void Activate(bool new_status);

    /** \brief Indicates whether the inventory window is in the active context
    *** \return True if the inventory window is in the active context
    **/
    bool IsActive() {
        return _active_box;
    }

    //! If the inventory window is ready to cancel out, or cancel out a sub-window
    //bool CanCancel();

    /*!
    * \brief Updates the inventory window.  Handles key presses, switches window context, etc.
    */
    void Update();

    /*!
    * \brief Draw the inventory window
    * \return success/failure
    */
    void Draw();

private:
    InventoryWindow();

    //! \brief The parent menu mode.
    MenuMode* _menu_mode;

    //! Used for the current dungeon
    vt_video::StillImage _location_graphic;

    //! Flag to specify the active option box
    uint32_t _active_box;

    //! OptionBox to display all of the items
    vt_gui::OptionBox _inventory_items;

    //! OptionBox to choose character
    vt_gui::OptionBox _char_select;

    //! OptionBox to choose item category
    vt_gui::OptionBox _item_categories;

    //! TextBox that holds the selected object's description
    vt_gui::TextBox _description;

    //! Used to render the current object name.
    vt_video::TextImage _object_name;

    //! Vector of GlobalObjects that corresponds to _inventory_items
    std::vector<std::shared_ptr<vt_global::GlobalObject>> _item_objects;

    //! holds previous category. we were looking at
    vt_global::ITEM_CATEGORY _previous_category;

    //! The currently selected object
    std::shared_ptr<vt_global::GlobalObject> _object;

    //! The currently selected object type.
    vt_global::GLOBAL_OBJECT _object_type;

    //! The currently selected Character
    vt_global::GlobalCharacter* _character;

    //! Tells whether the item is a piece of equipment
    bool _is_equipment;

    //! Tells whether the character can equip the item
    bool _can_equip;

    //! \brief Updates the item text in the inventory items.
    void _UpdateItemText();

    //! \brief updates the selected item and character.
    //! \note this also updates calls _UpdateItemText();
    void _UpdateSelection();

    //! \brief Initializes inventory items option box.
    void _InitInventoryItems();

    //! \brief Initializes char select option box.
    void _InitCharSelect();

    //! \brief Initializes item category select option box.
    void _InitCategory();

    //! \brief Updates item category select option box option availability.
    void _UpdateCategory();

    //! \brief Draws the special item description and image
    //! on the icon bottom right part of the item icon.
    //! (Used for key items and spirits)
    void _DrawSpecialItemDescription(vt_video::StillImage* special_image,
                                     vt_gui::TextBox& description);

    void _DrawBottomInfo();
};

} // namespace private_menu

} // namespace vt_menu

#endif // __MENU_INVENTORY_WINDOW__
