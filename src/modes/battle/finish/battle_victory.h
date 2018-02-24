////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_VICTORY_HEADER__
#define __BATTLE_VICTORY_HEADER__

#include "battle_finish.h"

#include "common/gui/menu_window.h"
#include "common/gui/textbox.h"
#include "common/gui/option.h"

#include <memory>

namespace vt_global {
class GlobalCharacter;
class GlobalObject;
}

namespace vt_battle
{

namespace private_battle
{

//! \brief Enums for the various states that the FinishWindow class may be in
enum VICTORY_STATE {
    VICTORY_INVALID = -1,
    VICTORY_START   = 0, //!< Drunes and objects dropped, XP earned are displayed and gradually awarded to characters
    VICTORY_MENU    = 1, //!< Menu to quit combat or improve skills is displayed.
    VICTORY_END     = 2, //!< Short sequence of hiding finish GUI objects
};

/** ****************************************************************************
*** \brief Manages game state after the battle has been won and processes rewards
***
*** This class presents the user with the results of the battle. More specifically,
*** the following events are accomplished
***
*** -#) Display experience points gained for each character
*** -#) Display the number of drunes earned and the type and quantity of any objects recovered
***
*** If the player lost the battle one or more times before they achieved victory, their XP and
*** drune rewards will be cut significantly for each retry.
*** ***************************************************************************/
class BattleVictory : public BattleFinish
{
public:
    BattleVictory();

    virtual ~BattleVictory();

    //! \brief Instructs the class to prepare itself for future updating and drawing
    virtual void Initialize() override;

    //! \brief Updates the state of the victory displays
    virtual void Update() override;

    //! \brief Draws the appropriate information to the screen
    virtual void Draw() override;

private:
    //! \brief A reference to where the state of the finish GUI menus is maintained
    VICTORY_STATE _state;

    //! \brief Used to announce the battle's outcome
    vt_gui::TextBox _outcome_text;

    //! \brief The total number of characters in the victorious party, living or dead
    uint32_t _characters_number;

    //! \brief The amount of xp earned for the victory (per character)
    uint32_t _xp_earned;

    //! \brief Tells to which character the raw fighting XP bonus is given to,
    //! and whether it has been given. True if given.
    //! The Raw fighting XP bonus is given when a character has won
    //! without wearing any equipment.
    bool _raw_xp_given[4];
    //! \brief Tells whether the Raw XP bonus was won.
    bool _raw_xp_won[4];

    //! \brief The amount of drunes dropped by the enemy party
    uint32_t _drunes_dropped;

    //! \brief When set to true, counting out of XP or Drunes will begin
    bool _begin_counting;

    //! \brief Retains the number of character windows that were created
    uint32_t _number_character_windows_created;

    //! \brief Pointers to all characters who took part in the battle
    std::vector<vt_global::GlobalCharacter *> _characters;

    //! \brief Holds portrait images for each character portraits
    vt_video::StillImage _character_portraits[4];

    //! \brief Holds all objects that were dropped by the defeated enemy party (<ID, quantity>)
    std::map<std::shared_ptr<vt_global::GlobalObject>, int32_t> _objects_dropped;

    //! \brief The top window in the GUI display that contains header text
    vt_gui::MenuWindow _header_window;

    //! \brief A window for each character showing any change to their attributes
    vt_gui::MenuWindow _character_window[4];

    //! \brief A window used to display details about objects dropped by the defeated enemies
    vt_gui::MenuWindow _spoils_window;

    //! \brief Drawn to the top header window displaying information about the stats/items obtained
    vt_gui::TextBox _header_xp;
    vt_gui::TextBox _header_drunes_dropped;
    vt_gui::TextBox _header_total_drunes;

    //! \brief Four XP of total unspent XP point for each character
    vt_gui::TextBox _unspent_xp[4];

    //! \brief Holds the experience level and XP points remaining for each character
    vt_gui::TextBox _level_text[4];
    vt_gui::TextBox _xp_text[4];

    //! \brief Header text for the object list option box
    vt_gui::TextBox _object_header_text;

    //! \brief Displays all objects obtained by the character party
    vt_gui::OptionBox _object_list;

    //! \brief Creates the character windows and any GUI objects that populate them
    void _CreateCharacterGUIObjects();

    //! \brief Populates the object list with the objects contained in the _dropped_objects container
    void _CreateObjectList();

    //! \brief Updates the character HP/SP before the battle exits
    void _SetCharacterStatus();

    //! \brief Sets the text to display in the header window depending upon the current state
    void _SetHeaderText();

    //! \brief Gradually rewards the characters with the XP that they earned
    void _UpdateXP();

    //! \brief Gradually counts out the amount of drunes that the party has earned
    void _UpdateSpoils();

    //! \brief Draws the XP earned by the party
    void _DrawXP(uint32_t index);

    //! \brief Draws the number of drunes and items dropped by the enemy party
    void _DrawSpoils();
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_VICTORY_HEADER__
