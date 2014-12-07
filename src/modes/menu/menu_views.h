///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    menu_views.h
*** \author  Daniel Steuernol steu@allacrost.org
*** \author  Andy Gardner chopperdave@allacrost.org
*** \author  Nik Nadig (IkarusDowned) nihonnik@gmail.com
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for various menu views.
***
*** This code handles the different menu windows that the user will see while the
*** is in menu mode. These windows are used for displaying inventory lists,
*** character statistics, and various other pieces of information.
*** ***************************************************************************/

#ifndef __MENU_VIEWS__
#define __MENU_VIEWS__

#include "common/global/global.h"
#include "common/gui/textbox.h"

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_menu
{

class MenuMode;

namespace private_menu
{

//! \brief The different skill types
enum SKILL_CATEGORY {
    SKILL_ALL = 0,
    SKILL_FIELD = 1,
    SKILL_BATTLE = 2,
    SKILL_CATEGORY_SIZE = 3
};

//! \brief The different equipment categories
enum EQUIP_CATEGORY {
    EQUIP_WEAPON = 0,
    EQUIP_HEAD = 1,
    EQUIP_TORSO = 2,
    EQUIP_ARMS = 3,
    EQUIP_LEGS = 4,
    EQUIP_CATEGORY_SIZE = 5
};

//! \brief The different option boxes that can be active for items
enum ITEM_ACTIVE_OPTION {
    ITEM_ACTIVE_NONE = 0,
    ITEM_ACTIVE_CATEGORY = 1,
    ITEM_ACTIVE_LIST = 2,
    ITEM_ACTIVE_CHAR = 3,
    ITEM_ACTIVE_SIZE = 4
};

//! \brief The different option boxes that can be active for skills
enum SKILL_ACTIVE_OPTION {
    SKILL_ACTIVE_NONE = 0,
    SKILL_ACTIVE_CHAR = 1,
    SKILL_ACTIVE_CATEGORY = 2,
    SKILL_ACTIVE_LIST = 3,
    SKILL_ACTIVE_CHAR_APPLY = 4,
    SKILL_ACTIVE_SIZE = 5
};

//! \brief The different option boxes that can be active for equipment
enum EQUIP_ACTIVE_OPTION {
    EQUIP_ACTIVE_NONE = 0,
    EQUIP_ACTIVE_CHAR = 1,
    EQUIP_ACTIVE_SELECT = 2,
    EQUIP_ACTIVE_LIST = 3,
    EQUIP_ACTIVE_SIZE = 4
};

//! \brief The different option boxes that can be active for party formation
enum FORM_ACTIVE_OPTION {
    FORM_ACTIVE_NONE = 0,
    FORM_ACTIVE_CHAR = 1,
    FORM_ACTIVE_SECOND = 2,
    FORM_ACTIVE_SIZE = 3
};

//! \brief Possible values from the confirm window
enum CONFIRM_RESULT {
    CONFIRM_RESULT_YES = 0,
    CONFIRM_RESULT_NO = 1,
    CONFIRM_RESULT_NOTHING = 2,
    CONFIRM_RESULT_CANCEL = 3,
};



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
    //! The name of the character that this window corresponds) to
    uint32 _char_id;

    //! The image of the character
    vt_video::StillImage _portrait;

    //! The text along with the character portrait
    vt_video::TextImage _character_name;
    vt_video::TextImage _character_data;

    //! The character active status effects images.
    //! Do not delete them, as they are handled by the GlobalMedia class.
    std::vector<vt_video::StillImage*> _active_status_effects;

    /** Refreshes the active status effects images vector content.
    *** \param character the character to check status effects for.
    **/
    void _UpdateActiveStatusEffects(vt_global::GlobalCharacter* character);
}; // class CharacterWindow : public vt_video::MenuWindow



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
    InventoryWindow();

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
    //! Used for the current dungeon
    vt_video::StillImage _location_graphic;

    //! Flag to specify the active option box
    uint32 _active_box;

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
    std::vector< vt_global::GlobalObject * > _item_objects;

    //! holds previous category. we were looking at
    vt_global::ITEM_CATEGORY _previous_category;

    //! The currently selected object
    vt_global::GlobalObject* _object;

    //! The currently selected object type.
    vt_global::GLOBAL_OBJECT _object_type;

    //! The currently selected Character
    vt_global::GlobalCharacter* _character;

    //! Tells whether the item is a piece of equipment
    bool _is_equipment;

    //! Tells whether the character can equip the item
    bool _can_equip;

    /*!
    * \brief Updates the item text in the inventory items
    */
    void _UpdateItemText();

    //! \brief updates the selected item and character
    //! \note this also updates calls _UpdateItemText();
    void _UpdateSelection();

    /*!
    * \brief Initializes inventory items option box
    */
    void _InitInventoryItems();

    /*!
    * \brief Initializes char select option box
    */
    void _InitCharSelect();

    /*!
    * \brief Initializes item category select option box
    */
    void _InitCategory();

    //! Draws the special item description and image
    //! on the icon bottom right part of the item icon.
    //! (Used for key items and spirits)
    void _DrawSpecialItemDescription(vt_video::StillImage* special_image,
                                     vt_gui::TextBox& description);

    void _DrawBottomInfo();

    template <class T> std::vector<vt_global::GlobalObject *> _GetItemVector(std::vector<T *> *inv);

}; // class InventoryWindow : public vt_video::MenuWindow



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
    inline uint32 GetActiveState() {
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
    uint32 _char_select_active;

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

}; // class PartyWindow : public vt_video::MenuWindow



/** ****************************************************************************
*** \brief Represents the Skills window, displaying all the skills for the character.
***
*** This window display all the skills for a particular character.
*** You can scroll through them all, filter by category, choose one, and apply it
*** to a character.
*** ***************************************************************************/
class SkillsWindow : public vt_gui::MenuWindow
{
    friend class vt_menu::MenuMode;
    friend class SkillsState;
public:
    SkillsWindow();

    ~SkillsWindow()
    {}

    /*!
    * \brief Updates key presses and window states
    */
    void Update();

    /*!
    * \brief Draws the windows and option boxes
    * \return success/failure
    */
    void Draw();

    /*!
    * \brief Activates the window
    * \param new_value true to activate window, false to deactivate window
    */
    void Activate(bool new_status);

    /*!
    * \brief Checks to see if the skills window is active
    * \return true if the window is active, false if it's not
    */
    bool IsActive() {
        return _active_box;
    }

private:
    //! Flag to specify the active option box
    uint32 _active_box;

    //! The character select option box
    vt_gui::OptionBox _char_select;

    //! The skills categories option box
    vt_gui::OptionBox _skills_categories;

    //! The skills list option box
    vt_gui::OptionBox _skills_list;

    //! The skill SP cost option box
    vt_gui::OptionBox _skill_cost_list;

    //! TextBox that holds the selected skill's description
    vt_gui::TextBox _description;

    //! The current skill icon, if any
    vt_video::StillImage _skill_icon;

    //! Track which character's skillset was chosen
    int32 _char_skillset;

    /*!
    * \brief Initializes the skills category chooser
    */
    void _InitSkillsCategories();

    /*!
    * \brief Initializes the skills chooser
    */
    void _InitSkillsList();

    /*!
    * \brief Initializes the character selector
    */
    void _InitCharSelect();

    //! \brief Returns the currently selected skill
    vt_global::GlobalSkill *_GetCurrentSkill();

    /*!
    * \brief Sets up the skills that comprise the different categories
    */
    void _UpdateSkillList();

    vt_utils::ustring _BuildSkillListText(const vt_global::GlobalSkill *skill);

    //! \brief parses the 3 skill lists of the global character and sorts them according to use (menu/battle)
    void _BuildMenuBattleSkillLists(std::vector<vt_global::GlobalSkill *> *skill_list,
                                    std::vector<vt_global::GlobalSkill *> *field, std::vector<vt_global::GlobalSkill *> *battle,
                                    std::vector<vt_global::GlobalSkill *> *all);

}; //class SkillsWindow : public vt_video::MenuWindow


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
    std::vector<uint32> _equip_list_inv_index;

    //! Flag to specify the active option box
    uint32 _active_box;

    //! equipment images
    std::vector<vt_video::StillImage> _equip_images;

    //! \brief The current character the equip window is dealing with.
    vt_global::GlobalCharacter* _character;

    //! \brief The current object the equip window is dealing with.
    vt_global::GlobalObject* _object;

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

}; // class EquipWindow : public vt_video::MenuWindow

/**
*** \brief Represents the quest log list window on the left side
*** this holds the options box "list" that players can cycle through to look at
*** their quests (in Quest Window)
**/

class QuestListWindow : public vt_gui::MenuWindow {
    friend class vt_menu::MenuMode;
    friend class QuestState;
    friend class QuestWindow;
public:
    QuestListWindow();
    ~QuestListWindow() {}

    /*!
    * \brief Draws window
    */
    void Draw();

    /*!
    * \brief Performs updates
    */
    void Update();

    /*!
    * \brief Result of whether or not this window is active
    * \return true if this window is active
    */
    bool IsActive()
    {
        return _active_box;
    }

private:
    //! \brief the selectable list of quests
    vt_gui::OptionBox _quests_list;

    //! The currently active quest log entries.
    std::vector<vt_global::QuestLogEntry*> _quest_entries;

    //! \brief indicates whether _quests_list is active or not
    bool _active_box;

    //! Setup the quests log list
    void _SetupQuestsList();

    //! \brief updates the side window quest list based on the current quest log entries
    void _UpdateQuestList();
};
/**
*** \brief Represents the quest log main window
*** players can view their active quests as well as completed quests when this window is viewing
**/

class QuestWindow : public vt_gui::MenuWindow {

    friend class vt_menu::MenuMode;
    friend class QuestState;

public:
    QuestWindow();
    ~QuestWindow() {}

    /*!
    * \brief Draws window
    */
    void Draw();

    /*!
    * \brief Draws the bottom window information
    * \note this only draws the location name and banner. we assume the
    * calling function draws the actual window and frame
    */
    void DrawBottom();

    /*!
    * \brief clears the info out.
    */
    void ClearBottom()
    {
        _location_name.ClearText();
        _location_subname.ClearText();
        _location_image = NULL;
        _location_subimage = NULL;
    }

    /*!
    * \brief Performs updates
    */
    void Update();

    /*!
    * \brief sets the viewing quest id information for the quest. we use this to query the text description
    */
    void SetViewingQuestId(const std::string &quest_id)
    {
        _viewing_quest_id = quest_id;
    }

private:
    //! \brief the currently viewing quest id. this is set by the Quest List Window through the
    //! SetViewingQuestId() function
    std::string _viewing_quest_id;

    //! \brief sets the display text to be rendered, based on their quest key that is set
    vt_gui::TextBox _quest_description;
    //! \brief sets the display text to be rendered when the quest is completed. this is additional info
    vt_gui::TextBox _quest_completion_description;

    //! \brief the display text to be rendered for the location name and subname that the quest key is set to
    vt_gui::TextBox _location_name;
    vt_gui::TextBox _location_subname;

    //! \brief the currently viewing location image and location subimage
    const vt_video::StillImage *_location_image;
    const vt_video::StillImage *_location_subimage;
};

/**
*** \brief handles showing the currently set world map
*** upon selection, based on the key press we cycle thru locations that are
*** set as "revealed" on the map
***
*** \note WorldMap has no left window. This means that the entire screen rendering takes place here
*** based on the Wold Map selection here, we update the WorldMapState such that for the
*** bottom window render, we have all the information needed to show
***
**/

class WorldMapWindow : public vt_gui::MenuWindow
{
    friend class vt_menu::MenuMode;
    friend class WorldMapState;

    enum WORLDMAP_NAVIGATION {
        WORLDMAP_NOPRESS,   //no key press.
        WORLDMAP_CANCEL,   //a cancel press to exit from viewing the window
        WORLDMAP_LEFT,      //a left press to move "up" the list of locations
        WORLDMAP_RIGHT      //a right press to move "down" the list of locations
    };
public:
    WorldMapWindow();

    ~WorldMapWindow()
    {
        _location_marker.Clear();
        _location_pointer.Clear();
    }

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
    * \brief Result of whether or not this window is active
    * \return true if this window is active
    */
    bool IsActive()
    {
        return _active;
    }

    /*!
    * \brief switch the active state of this window, and do any associated work
    * \param activate or deactivate
    */
    void Activate(bool new_state);

    /*!
    * \brief gets the WorldMapLocation pointer to the currently pointing
    * location, or NULL if it deson't exist
    * \return Pointer to the currently indexes WorldMapLocation
    */
    vt_global::WorldMapLocation *GetCurrentViewingLocation()
    {
        const std::vector<std::string> &current_location_ids = vt_global::GlobalManager->GetViewableLocationIds();
        const uint32 N = current_location_ids.size();
        if( N == 0 || _location_pointer_index > N)
            return NULL;
        return vt_global::GlobalManager->GetWorldLocation(current_location_ids[_location_pointer_index]);
    }

private:

    //! \brief based on the worldmap selection, sets the pointer on the
    //! current map
    void _SetSelectedLocation(WORLDMAP_NAVIGATION worldmap_goto);

    //! \brief draws the locations and the pointer based on
    //! the currently active location ids and what we have selected
    //! \param window_position_x The X position of the window
    //! \param window_position_y The Y position of the window
    void _DrawViewableLocations(float window_position_x, float window_position_y);

    //! \brief pointer to the currently loaded world map image
    vt_video::StillImage *_current_world_map;

    //! \brief the location marker. this is loaded in the ctor
    vt_video::AnimatedImage _location_marker;

    //! \brief the location pointer. this is loaded in the ctor
    vt_video::StillImage _location_pointer;

    //! \brief offsets for the current image to view in the center of the window
    float _current_image_x_offset;
    float _current_image_y_offset;

    //! \brief the current index to the location the pointer should be on
    uint32 _location_pointer_index;

    //! \brief indicates whether this window is active or not
    bool _active;

};

/*!
* \brief Converts a vector of GlobalItem*, etc. to a vector of GlobalObjects*
* \return the same vector, with elements of type GlobalObject*
*/
template <class T> std::vector<vt_global::GlobalObject *> InventoryWindow::_GetItemVector(std::vector<T *>* inv)
{
    std::vector<vt_global::GlobalObject *> obj_vector;

    for(typename std::vector<T *>::iterator i = inv->begin(); i != inv->end(); i++) {
        obj_vector.push_back(*i);
    }

    return obj_vector;
}

} // namespace private_menu

} // namespace vt_menu

#endif
