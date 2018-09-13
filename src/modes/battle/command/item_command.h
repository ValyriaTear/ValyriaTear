////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_ITEM_COMMAND_HEADER__
#define __BATTLE_ITEM_COMMAND_HEADER__

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

#include "modes/battle/battle_item.h"

namespace vt_battle
{

namespace private_battle
{

class BattleCharacter;

/** ****************************************************************************
*** \brief Manages the character party's usable items
***
*** This class is an assistant to the CommandSupervisor class. It serves to manage
*** the items that the player may select to use in battle and provides displays
*** to those items. Upon initialization, this class creats a copy of every item in the
*** character party's inventory that may be used in battle. The class also maintains
*** certain GUI displays to assist the CommandSupervisor in displaying the list of
*** items available to use.
*** ***************************************************************************/
class ItemCommand
{
public:
    //! \param window A reference to the MenuWindow that the GUI objects should be owned by
    ItemCommand(vt_gui::MenuWindow& window);

    ~ItemCommand()
    {}

    /** \brief Constructs the _item_list option box from scratch using the _items container
    *** This will also reset the selection on the item list to the first element. Typically this only needs to be
    *** called once, during battle mode initialization.
    **/
    void ConstructList();

    /** \brief Initializes the item list by setting the selected list option
    *** \param item_index The index of the item to select
    *** \note If the selection argument is out-of-range, no change will take place.
    **/
    void Initialize(uint32_t item_index);

    /** \brief Returns a pointer to the currently selected item
    *** This function will return nullptr if the class has not been initialized and there is no list of
    *** items to select from.
    **/
    std::shared_ptr<BattleItem> GetSelectedItem();

    /** \brief Returns whether there are still available item from the selected type.
    **/
    bool IsSelectedItemAvailable() const;

    //! \brief Updates the item list and processes user input
    void UpdateList();

    //! \brief Draws the item header and list
    void DrawList();

    /** \brief Modifies the character party's global inventory to match the counts of the items in this class
    *** This should be called only after the battle has finished. There is no need to modify the party's
    *** inventory while the battle is still progressing.
    **/
    void CommitChangesToInventory();

    //! \brief Reset the item list content, used to get inventory items at battle (re)starts.
    void ResetItemList();

    //! \brief Returns the number of items that will be displayed in the list
    uint32_t GetNumberListOptions() const {
        return _item_list.GetNumberOptions();
    }

private:
    /** \brief Container for all available items at battle start
    *** The size of the container does not change, even when the available
    *** count of a specific item becomes zero.
    *** This list only updates the battle Items count and is used to change the global inventory,
    *** when a battle is won.
    **/
    std::vector<std::shared_ptr<BattleItem>> _battle_items;

    /** \brief A sub-list of BattleItem pointing on _battle_items members that are still considered valid
    *** and thus added to the item menu list.
    **/
    std::vector<std::shared_ptr<BattleItem>> _menu_items;

    //! \brief A single line of header text for the item list option box
    vt_gui::OptionBox _item_header;

    //! \brief A display list of all usable items
    vt_gui::OptionBox _item_list;
    //! \brief A display list of all usable items target type and number in inventory.
    vt_gui::OptionBox _item_target_list;
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_COMMAND_HEADER__
