////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_FINISH_SUPERVISOR_HEADER__
#define __BATTLE_FINISH_SUPERVISOR_HEADER__

#include "battle_defeat.h"
#include "battle_victory.h"

/*
#include "common/gui/option.h"
#include "common/gui/menu_window.h"
#include "common/gui/textbox.h"

#include "modes/battle/battle_utils.h"

namespace vt_global {
class GlobalSkill;
class GlobalCharacter;
}
*/
namespace vt_battle
{

namespace private_battle
{

//! \brief Enums for the various states that the FinishWindow class may be in
enum FINISH_STATE {
    FINISH_INVALID = -1,
    FINISH_ANNOUNCE_RESULT = 0, //!< Short sequence announcing outcome of the battle (victory or defeat) and showing GUI objects
    FINISH_DEFEAT_SELECT   = 1, //!< Player selects what to do after defeat (go to main menu, exit game, etc.)
    FINISH_DEFEAT_CONFIRM  = 2, //!< Player confirms defeat selection
    FINISH_VICTORY_START   = 4, //!< Drunes and objects dropped, XP earned are displayed and gradually awarded to characters
    FINISH_VICTORY_MENU    = 5, //!< Menu to quit combat or improve skills is displayed.
    FINISH_END             = 6, //!< Short sequence of hiding finish GUI objects
};

/** ****************************************************************************
*** \brief Manages game state after the battle has been either won or lost
***
*** Most of the grunt work is done by either the FinishDefeatAssistant or FinishVictoryAssistant
*** classes, depending on what the outcome of the battle was. This class does
*** some of the display and update work on its own however, such as adjusting
*** the scene lighting and displaying the victory/defeat message.
*** ***************************************************************************/
class FinishSupervisor
{
public:
    FinishSupervisor();

    ~FinishSupervisor()
    {}

    /** \brief Un-hides the window display and creates the window contents
    *** \param victory Set to true if the player's party was victorious in battle; false if he/she was defeated
    **/
    void Initialize(bool victory);

    //! \brief Updates the state of the window
    void Update();

    //! \brief Draws the window and its contents
    void Draw();

    FINISH_STATE GetState() const {
        return _state;
    }

private:
    //! \brief Maintains state information to determine how to process user input and what to draw to the screen
    FINISH_STATE _state;

    //! \brief Determine whether the battle was victorious
    bool _is_battle_victory;

    //! \brief Assists this class when the player was defeated in battle
    BattleDefeat _battle_defeat;

    //! \brief Assists this class when the player was victorious in battle
    BattleVictory _battle_victory;

    //! \brief Used to announce the battle's outcome (victory or defeat)
    vt_gui::TextBox _outcome_text;
}; // class FinishSupervisor

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_FINISH_SUPERVISOR_HEADER__
