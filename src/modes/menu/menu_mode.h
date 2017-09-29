///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_MODE_HEADER__
#define __MENU_MODE_HEADER__

#include "modes/menu/menu_states/menu_main_state.h"
#include "modes/menu/menu_states/menu_inventory_state.h"
#include "modes/menu/menu_states/menu_party_state.h"
#include "modes/menu/menu_states/menu_skill_state.h"
#include "modes/menu/menu_states/menu_equip_state.h"
#include "modes/menu/menu_states/menu_quest_state.h"
#include "modes/menu/menu_states/menu_worldmap_state.h"

#include "modes/menu/menu_windows/menu_battle_formation_window.h"
#include "modes/menu/menu_windows/menu_character_window.h"
#include "modes/menu/menu_windows/menu_equip_window.h"
#include "modes/menu/menu_windows/menu_inventory_window.h"
#include "modes/menu/menu_windows/menu_party_window.h"
#include "modes/menu/menu_windows/menu_quest_window.h"
#include "modes/menu/menu_windows/menu_quest_list_window.h"
#include "modes/menu/menu_windows/menu_skills_window.h"
#include "modes/menu/menu_windows/menu_skillgraph_window.h"
#include "modes/menu/menu_windows/menu_worldmap_window.h"

#include "engine/mode_manager.h"

namespace vt_common {
class MessageWindow;
}

namespace vt_menu {

class MenuMode;

//! \brief Determines whether the code in the vt_menu namespace should print debug statements or not.
extern bool MENU_DEBUG;

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
class MenuMode : public vt_mode_manager::GameMode
{
    friend class private_menu::CharacterWindow;
    friend class private_menu::InventoryWindow;
    friend class private_menu::PartyWindow;
    friend class private_menu::SkillsWindow;
    friend class private_menu::SkillGraphWindow;
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

    //! \brief Called when starting/returning to the menu mode.
    void Reset();

    //! \brief Updates the menu. Calls Update() on active window if there is one
    void Update();

    //! \brief Draws the menu. Calls Draw() on active window if there is one.
    void Draw();

    //! \brief (Re)Loads the characters windows based on the characters' positions in the party.
    void ReloadCharacterWindows();

    //! \brief Updates the time and drunes text
    void UpdateTimeAndDrunes();

    /** \brief handles drawing the generalized equipment information
    *** Used by both the inventory and equip windows and states.
    **/
    void UpdateEquipmentInfo(vt_global::GlobalCharacter *character,
                             const std::shared_ptr<vt_global::GlobalObject>& object,
                             private_menu::EQUIP_VIEW view_type);

    //! \brief Draws The current equipment info
    //! Used by both the inventory and equip windows and states.
    void DrawEquipmentInfo();

    //! \brief Forces update of time
    void ForceUpdateOfTime() {
        _update_of_time = 0;
    }

private:
    //! \brief A static pointer to the last instantiated MenuMode object
    static MenuMode *_current_instance;

    //! \brief The default text (time and drunes) display on the bottom part.
    vt_gui::TextBox _time_text;
    vt_gui::TextBox _drunes_text;

    //! \brief Time and drunes icons. Global media references, don't delete those!
    vt_video::StillImage* _clock_icon;
    vt_video::StillImage* _drunes_icon;

    //! \brief Text image which displays the name of the location in the game where MenuMode was invoked
    vt_gui::TextBox _locale_name;

    /** \brief The graphic that represents the current map that the player is exploring
    *** This image is set using the string in the MenuMode constructor
    **/
    vt_video::StillImage _locale_graphic;

    /** \brief Retains a snap-shot of the screen just prior to when menu mode was entered
    *** This image is perpetually drawn as the background while in menu mode
    **/
    vt_video::StillImage _saved_screen;

    //! \brief The symbol indicating that the item is a key item.
    //! \note Don't delete this, it is managed by the Global Manager.
    vt_video::StillImage* _key_item_icon;

    //! \brief Test indicating that the item is a key item and cannot be used or sold.
    vt_gui::TextBox _key_item_description;

    //! \brief The symbol indicating that the item is a crystal spirit.
    //! \note Don't delete this, it is managed by the Global Manager.
    vt_video::StillImage* _spirit_icon;

    //! \brief Test indicating that the item is a spirit and can be associated with equipment.
    vt_gui::TextBox _spirit_description;

    //! \brief Icons representing the atk/def of the equipment/character.
    //! \note Don't delete those, they are managed by the Global Manager.
    vt_video::StillImage* _atk_icon;
    vt_video::StillImage* _def_icon;
    vt_video::StillImage* _matk_icon;
    vt_video::StillImage* _mdef_icon;

    //! \brief shared "help" information text box. can be used to display a short helpful message to the player
    vt_gui::TextBox _help_information;

    /** \name Main Display Windows
    *** \brief The various menu windows that are displayed in menu mode
    **/
    //@{
    vt_gui::MenuWindow _bottom_window;
    vt_gui::MenuWindow _main_options_window;
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
    private_menu::AbstractMenuState* _current_menu_state;

    private_menu::CharacterWindow _character_window0;
    private_menu::CharacterWindow _character_window1;
    private_menu::CharacterWindow _character_window2;
    private_menu::CharacterWindow _character_window3;
    private_menu::InventoryWindow _inventory_window;
    private_menu::QuestListWindow _quest_list_window;
    private_menu::PartyWindow _party_window;
    private_menu::BattleFormationWindow _battle_formation_window;
    private_menu::SkillsWindow _skills_window;
    private_menu::SkillGraphWindow _skilltree_window;
    private_menu::EquipWindow _equip_window;
    private_menu::QuestWindow _quest_window;
    private_menu::WorldMapWindow _world_map_window;

    vt_common::MessageWindow* _message_window;

    //! \name Option boxes that are used in the various menu windows
    //@{
    vt_gui::OptionBox _menu_inventory;
    vt_gui::OptionBox _menu_skills;
    vt_gui::OptionBox _menu_status;
    vt_gui::OptionBox _menu_options;
    vt_gui::OptionBox _menu_equip;
    vt_gui::OptionBox _menu_formation;
    //@}

    /** \brief Equipment view data used to display a common equipment view
    **/
    //@{

protected:
    //! \brief The selected object instance
    std::shared_ptr<vt_global::GlobalObject> _object;

private:

    //! \brief The selected character
    vt_global::GlobalCharacter* _character;

    //! \brief the current equipment view type
    private_menu::EQUIP_VIEW _equip_view_type;

    //! \brief The name of the selected object
    vt_video::TextImage _object_name;

    //! \brief When the object type is equipment, this tells whether it is a weapon.
    bool _is_weapon;

    //! \brief The text headers
    vt_video::TextImage _phys_header;
    vt_video::TextImage _mag_header;

    //! \brief The equipment stats
    vt_video::TextImage _phys_stat;
    vt_video::TextImage _mag_stat;

    //! \brief The overall atk/def diff with current equipment
    vt_video::TextImage _phys_stat_diff;
    vt_video::TextImage _mag_stat_diff;
    vt_video::Color _phys_diff_color;
    vt_video::Color _mag_diff_color;

    //! \brief Icon images representing status effects and intensity properties of the selected object
    std::vector<vt_video::StillImage *> _status_icons;

    //! \brief The number of spirit the equipment can support.
    uint32_t _spirit_number;

    //! \brief The skills earned when equipping info
    vt_video::TextImage _equip_skills_header;
    std::vector<vt_video::TextImage> _equip_skills;
    std::vector<vt_video::StillImage> _equip_skill_icons;

    //! \brief A static variable permitting to update the displayed game time only every 900ms
    int32_t _update_of_time = 0;
    //@}
};

} // namespace vt_menu

#endif // __MENU_MODE_HEADER__
