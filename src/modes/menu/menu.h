///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    menu.h
*** \author  Daniel Steuernol steu@allacrost.org
*** \author  Andy Gardner chopperdave@allacrost.org
*** \author  Nik Nadig (IkarusDowned) nihonnik@gmail.com
*** \brief   Header file for menu mode interface.
***
*** This code handles the game event processing and frame drawing when the user
*** is in menu mode. This mode's primary objectives are to allow the user to
*** view stastics about their party and manage inventory and equipment.
*** ***************************************************************************/

#ifndef __MENU_HEADER__
#define __MENU_HEADER__

#include <string>
#include <vector>
#include <map>

#include "utils.h"
#include "defs.h"

#include "engine/video/video.h"

#include "common/global/global.h"

#include "engine/mode_manager.h"
#include "menu_views.h"

//! \brief All calls to menu mode are wrapped in this namespace.
namespace hoa_menu
{

class MenuMode;
//! \brief Determines whether the code in the hoa_menu namespace should print debug statements or not.
extern bool MENU_DEBUG;

//! \brief An internal namespace to be used only within the menu code. Don't use this namespace anywhere else!
namespace private_menu
{

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
    AbstractMenuState(const char *state_name, MenuMode* menu_mode);

    virtual ~AbstractMenuState()
    {}

    //! \brief used when the MenuMode is activated.
    //! \note While we provide a default "do nothing" Reset(), each child type should have its own Reset implementation
    virtual void Reset(){};

    //! \brief handles the drawing of the state
    void Draw();

    //! \brief handles updating the state
    void Update();

    //! \brief Updates the time and drunes text
    void UpdateTimeAndDrunes();

    /**
    *** \brief based on the selection (pased in via the OptionBox selection returns the next state to transition to
    *** \param selection selection state indicator
    *** \return either a valid AbstractMenuState pointer to the next valid transition state, or NULL
    *** \note NULL actions do not lead to a crash, but the assumption is that the state has some state-specific
    *** actions, such as activitaing a MenuView that take place
    **/
    virtual AbstractMenuState *GetTransitionState(uint32 selection)=0;

    //! \brief returns the name of the state
    const char *GetStateName() { return _state_name;}

    //! \brief returns a pointer to the OptionsBox associated with this state
    hoa_gui::OptionBox *GetOptions() { return &_options;}

protected:
    //! \brief default bottom menu drawing
    void _DrawBottomMenu();
    //! \brief action that takes place when we exit this state via a "cancel" input
    void _OnCancel();
    //! \brief handles any state preperation that needs to be done upon entry to this state
    //! \param the state that we are transitioning from
    //  this was simple enough to allow for inlining
    virtual void _OnEntry(AbstractMenuState *from_state) { _from_state = from_state; }
    //! \brief returns the default selection to use when we first load the menu after MenuMode is at the top of stack
    virtual uint32 _GetDefaultSelection() { return 0;}
    //! \brief handles updating the state when it is claimed as "active"
    virtual void _ActiveWindowUpdate(){}
    //! \brief returns wether or not the state is active
    virtual bool _IsActive() { return false;}
    //! \brief instance-specific main window drawing code goes in here. the default is to simply draws nothing
    virtual void _OnDrawMainWindow(){}
    //! \brief handles drawing the generalized equipment information
    void _DrawEquipmentInfo(hoa_global::GlobalCharacter *character);
    //! \brief draws the side window. Default draws the character windows
    virtual void _OnDrawSideWindow();
    //! \brief called when there is no state transition or active state
    //! to handle any state-specific updates that need to occur
    virtual void _OnUpdateState(){};
    // Options associated with this state
    hoa_gui::OptionBox _options;
    // state-specific name
    const char *_state_name;
    // a pointer to the active MenuMode
    MenuMode *_menu_mode;
    // a pointer to the state we should return to on a "cancel" press.
    AbstractMenuState *_from_state;

    // The default text (time and drunes) display on the bottom part.
    hoa_gui::TextBox _time_text;
    hoa_gui::TextBox _drunes_text;
};

/**
*** \brief Main Menu state. This is the entry point into the menu
***
*** The main menu has no "active" state
**/
class MainMenuState : virtual public AbstractMenuState {
public:
    //! \brief possible transition states from the main menu
    enum MAIN_CATEGORY {
        MAIN_OPTIONS_INVENTORY,
        MAIN_OPTIONS_SKILLS,
        MAIN_OPTIONS_PARTY,
        MAIN_OPTIONS_QUESTS,
        MAIN_OPTIONS_WORLDMAP,
        MAIN_OPTIONS_SIZE
    };

    //! \brief Main Menu State constructor
    MainMenuState(MenuMode *menu_mode):
        AbstractMenuState("Main Menu", menu_mode)
    {}

    ~MainMenuState(){};
    void Reset();
    AbstractMenuState *GetTransitionState(uint32 selection);

protected:
    void _OnDrawMainWindow();
    void _OnDrawSideWindow();
    void _OnUpdateState();
};

/**
*** \brief Inventory State. Handles user interactions for item use and equiping
**/
class InventoryState : virtual public AbstractMenuState {
public:
    //! \brief the possible inventory options
    enum INVENTORY_CATEGORY {
        INV_OPTIONS_USE,
        INV_OPTIONS_EQUIP,
        INV_OPTIONS_REMOVE,
        INV_OPTIONS_BACK,
        INV_OPTIONS_SIZE
    };
    //! \brief InventoryState state constructor
    InventoryState(MenuMode *menu_mode):
        AbstractMenuState("Inventory State", menu_mode)
    {}
    ~InventoryState(){}
    void Reset();
    AbstractMenuState *GetTransitionState(uint32 selection);
protected:
    void _DrawItemDescription(hoa_global::GlobalObject &obj,
                              hoa_video::StillImage &item_image,
                              hoa_gui::TextBox &description);
    void _DrawBottomMenu();
    void _OnDrawMainWindow();
    void _ActiveWindowUpdate();
    bool _IsActive();
};

/**
*** \brief Party state. shows the user the character status information. Allows
*** switching of formation
**/
class PartyState : virtual public AbstractMenuState {
public:
    enum PARTY_CATEGORY {
        PARTY_OPTIONS_VIEW_ALTER,
        PARTY_OPTIONS_BACK,
        PARTY_OPTIONS_SIZE
    };

    //! \brief Party state constructor
    PartyState(MenuMode* menu_mode):
        AbstractMenuState("Party State", menu_mode)
    {}

    ~PartyState(){}
    void Reset();
    AbstractMenuState *GetTransitionState(uint32 selection);
protected:
    void _DrawBottomMenu();
    void _OnDrawMainWindow();
    void _ActiveWindowUpdate();
    bool _IsActive();

};

/**
*** \brief Skills state. Allows user to view their skills and use them
**/
class SkillsState : virtual public AbstractMenuState {
public:
    enum SKILLS_CATEGORY {
        SKILLS_OPTIONS_USE,
        SKILLS_OPTIONS_BACK,
        SKILLS_OPTIONS_SIZE

    };

    //! \brief Skills state constructor
    SkillsState(MenuMode *menu_mode):
        AbstractMenuState("Skills State", menu_mode)
    {}

    ~SkillsState(){}
    void Reset();
    AbstractMenuState *GetTransitionState(uint32 selection);
protected:
    void _DrawBottomMenu();
    void _OnDrawMainWindow();
    void _ActiveWindowUpdate();
    bool _IsActive();
};

/**
*** \brief Equip / Remove state. Allows players to modify the character equipment
**/
class EquipState : virtual public AbstractMenuState {
public:
    enum EQUIP_CATEGORY {
        EQUIP_OPTIONS_BACK,
        EQUIP_OPTIONS_SIZE
    };

    //! Equip state constructor
    EquipState(MenuMode *menu_mode):
        AbstractMenuState("Equip State", menu_mode)
    {}
    ~EquipState(){}
    void Reset();
    AbstractMenuState* GetTransitionState(uint32 selection);
    //handles drawing the bottom window info for equipment
    static void DrawEquipmentInfo(const hoa_utils::ustring &equipment_name, bool is_weapon = false,
                                  uint32 physical_attribute = 0, uint32 magical_attribute = 0,
                                  uint32 current_phys_attribute = 0, uint32 current_mag_attribute = 0);
protected:
    void _DrawBottomMenu();
    void _OnDrawMainWindow();
    void _OnEntry(AbstractMenuState *from_state);
    void _ActiveWindowUpdate();
    bool _IsActive();
};


/**
*** \brief Quest Log state. Allows users to view their active and finished quests
**/
class QuestState : virtual public AbstractMenuState {
public:
    enum QUEST_CATEGORY {
            QUEST_OPTIONS_VIEW,
            QUEST_OPTIONS_BACK,
            QUEST_OPTIONS_SIZE
    };

    //! \brief Quest state constructor
    QuestState(MenuMode *menu_mode):
        AbstractMenuState("Quest State", menu_mode)
    {}

    ~QuestState(){}

    void Reset();
    AbstractMenuState *GetTransitionState(uint32 selection);
protected:
    void _OnDrawMainWindow();
    void _OnDrawSideWindow();
    void _DrawBottomMenu();
    void _ActiveWindowUpdate();
    bool _IsActive();
    void _OnEntry(AbstractMenuState *from_state);
};

/**
*** \brief World Map State
*** Allows player to view the currently enabled world map,
*** along with the viewable locations on it
**/

class WorldMapState : virtual public AbstractMenuState
{
    //WorldMapState constructor
public:
    WorldMapState(MenuMode *menu_mode);

    ~WorldMapState()
    {
        _location_text.ClearText();
    }

    void Reset();
    AbstractMenuState *GetTransitionState(uint32 /*selection*/)
    { return NULL; }
protected:
    void _OnDrawMainWindow();
    //! \brief this function is overridden to do nothing, as the side window is not used in map mode
    inline void _OnDrawSideWindow(){};
    void _DrawBottomMenu();
    void _ActiveWindowUpdate();
    bool _IsActive();
    void _OnEntry(AbstractMenuState *from_state);

    hoa_gui::TextBox _location_text;
    hoa_video::StillImage *_location_image;

};

} // namespace private_menu

/** ****************************************************************************
*** \brief Handles game executing while in the main in-game menu.
***
*** This mode of game operation allows the player to examine and manage their
*** party, inventory, options, and save their game.
***
*** \note MenuMode is always entered from an instance of MapMode. However, there
*** may be certain conditions where MenuMode is entered from other game modes.
***
*** \note MenuMode does not play its own music, but rather it continues playing
*** music from the previous GameMode that created it.
*** ***************************************************************************/
class MenuMode : public hoa_mode_manager::GameMode
{
    friend class private_menu::CharacterWindow;
    friend class private_menu::InventoryWindow;
    friend class private_menu::PartyWindow;
    friend class private_menu::SkillsWindow;
    friend class private_menu::EquipWindow;
    friend class private_menu::QuestListWindow;
    friend class private_menu::QuestWindow;
    friend class private_menu::WorldMapWindow;

    friend class private_menu::AbstractMenuState;
    friend class private_menu::MainMenuState;
    friend class private_menu::InventoryState;
    friend class private_menu::PartyState;
    friend class private_menu::SkillsState;
    friend class private_menu::EquipState;
    friend class private_menu::QuestState;
    friend class private_menu::WorldMapState;
public:
    MenuMode();

    ~MenuMode();

    //! \brief Returns a pointer to the active instance of menu mode
    static MenuMode *CurrentInstance() {
        return _current_instance;
    }

    //! \brief Resets the menu mode back to its default setup.
    void Reset();

    //! \brief Updates the menu. Calls Update() on active window if there is one
    void Update();

    //! \brief Draws the menu. Calls Draw() on active window if there is one.
    void Draw();

    //! \brief (Re)Loads the characters windows based on the characters' positions in the party.
    void ReloadCharacterWindows();

private:
    //! \brief A static pointer to the last instantiated MenuMode object
    static MenuMode *_current_instance;

    //! \brief Text image which displays the name of the location in the game where MenuMode was invoked
    hoa_gui::TextBox _locale_name;

    /** \brief The graphic that represents the current map that the player is exploring
    *** This image is set using the string in the MenuMode constructor
    **/
    hoa_video::StillImage _locale_graphic;

    /** \brief Retains a snap-shot of the screen just prior to when menu mode was entered
    *** This image is perpetually drawn as the background while in menu mode
    **/
    hoa_video::StillImage _saved_screen;

    //! \brief The symbol indicating that the item is a key item.
    hoa_video::StillImage _key_item_symbol;
    //! \brief Test indicating that the item is a key item and cannot be used or sold.
    hoa_gui::TextBox _key_item_description;

    //! \brief The symbol indicating that the item is a crystal shard.
    hoa_video::StillImage _shard_symbol;
    //! \brief Test indicating that the item is a shard and can be associated with equipment.
    hoa_gui::TextBox _shard_description;

    //! \brief shared "help" information text box. can be used to display a short helpful message to the player
    hoa_gui::TextBox _help_information;

    /** \name Main Display Windows
    *** \brief The various menu windows that are displayed in menu mode
    **/
    //@{
    hoa_gui::MenuWindow _bottom_window;
    hoa_gui::MenuWindow _main_options_window;
    //@}

    /** \name currently available states
    *** using the menu mode is done through these states
    **/
    //@{
    private_menu::MainMenuState _main_menu_state;
    private_menu::InventoryState _inventory_state;
    private_menu::PartyState _party_state;
    private_menu::SkillsState _skills_state;
    private_menu::EquipState _equip_state;
    private_menu::QuestState _quests_state;
    private_menu::WorldMapState _world_map_state;
    //@}

    //! \brief currently viewing state
    private_menu::AbstractMenuState *_current_menu_state;

    private_menu::CharacterWindow _character_window0;
    private_menu::CharacterWindow _character_window1;
    private_menu::CharacterWindow _character_window2;
    private_menu::CharacterWindow _character_window3;
    private_menu::InventoryWindow _inventory_window;
    private_menu::QuestListWindow _quest_list_window;
    private_menu::PartyWindow _party_window;
    private_menu::SkillsWindow _skills_window;
    private_menu::EquipWindow _equip_window;
    private_menu::QuestWindow _quest_window;
    private_menu::WorldMapWindow _world_map_window;

    MessageWindow *_message_window;

    //! \brief A map of the sounds used while in MenuMode
    std::map<std::string, hoa_audio::SoundDescriptor> _menu_sounds;

    //! \name Option boxes that are used in the various menu windows
    //@{
    hoa_gui::OptionBox _menu_inventory;
    hoa_gui::OptionBox _menu_skills;
    hoa_gui::OptionBox _menu_status;
    hoa_gui::OptionBox _menu_options;
    hoa_gui::OptionBox _menu_equip;
    hoa_gui::OptionBox _menu_formation;
    //@}

    //! \brief Draws the 'Name' and 'Qty' tags for the item list.
    void _DrawItemListHeader()
    {}

}; // class MenuMode : public hoa_mode_manager::GameMode

} // namespace hoa_menu

#endif
