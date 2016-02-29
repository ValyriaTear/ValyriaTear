///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    boot.h
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for boot mode interface.
***
*** This code handles the game event processing and frame drawing when the user
*** is in boot mode (the boot screen and menus).
*** ***************************************************************************/

#ifndef __BOOT_HEADER__
#define __BOOT_HEADER__

#include "engine/mode_manager.h"

#include "common/options_handler.h"

//! \brief All calls to boot mode are wrapped in this namespace.
namespace vt_boot
{

//! \brief Various states that boot mode may be in
enum BOOT_STATE {
    BOOT_STATE_INTRO   = 0,
    BOOT_STATE_MENU    = 1
};

//! \brief Determines whether the code in the vt_boot namespace should print debug statements or not.
extern bool BOOT_DEBUG;

/** ****************************************************************************
*** \brief Handles the game execution while the player is on the boot menu screen.
***
*** This is the first mode that is pushed onto the game stack when the program starts.
*** Its primary functions are available from the highest level menu:
***
*** - "New Game": starts a new game
*** - "Load Game": activates save mode to allow the player to select a game to load
*** - "Options": enables the player to change various application settings
*** - "Quit": exits the application
***
*** The majority of the code in this class is for enabling the player to change their
*** application settings in the "Options" menu and its related sub-menus.
***
*** When boot mode is entered for the very first time upon the application starting
*** up, the opening animation of the title screen is displayed. This only occurs the
*** first time that boot mode is entered for the lifetime of the application process.
*** After the boot animation, a welcome screen may be displayed that lists the default
*** controls for the player. This screen is only visible the very first time the player
*** installs and starts the application and is hidden otherwise.
*** ***************************************************************************/
class BootMode : public vt_mode_manager::GameMode
{
public:
    BootMode();

    ~BootMode();

    //! \brief Returns a pointer to the active instance of boot mode
    static BootMode *CurrentInstance() {
        return _current_instance;
    }

    //! \brief Resets appropriate class members. Called whenever BootMode is made the active game mode.
    void Reset();

    //! \brief Handles user input and updates the boot menu
    void Update();

    //! \brief Draws the boot menu background depending on the current state
    void Draw();

    //! \brief Draws the contents and menus of boot mode depending on the current state
    void DrawPostEffects();

    //! \brief Tells the boot state
    BOOT_STATE GetState() const {
        return _boot_state;
    }

    //! \brief Sets the boot state
    void ChangeState(BOOT_STATE boot_state) {
        _boot_state = boot_state;
    }

    /** \brief Reloads all the menus to refresh their translated text.
    *** This is to be used only by the language menu.
    *** \Note Only menus with translatable texts are reloaded.
    **/
    void ReloadTranslatedTexts();

private:
    //! \brief The boot state used to know whether the menu has to be displayed.
    BOOT_STATE _boot_state;

    //! \brief A pointer to the most recently active instance of boot mode
    static BootMode* _current_instance;

    //! \brief True when the boot mode is in exiting state.
    bool _exiting_to_new_game;

    //! \brief True when the new game function has been called.
    //! \note Used to prevent calling it more than once.
    bool _new_game_called;

    //! \brief Rendered text of the release version number
    vt_video::TextImage _version_text;

    /** \name Various menus available in boot mode
    *** The name of the menu objects is rather self explanatory. There are a number of methods in
    *** BootMode that are used by these menus.
    ***
    *** -# Setup methods used to initialize the properties of these menus
    *** -# Refresh methods used to refresh the menu option text to reflect current state information
    *** -# Handler methods used as callback functions when input on the menus occur
    ***
    *** Some of these methods are exclusive to their respective menus while others are not. Some
    *** of these menus do not have all of these types of methods available for them.
    **/
    //@{
    vt_gui::private_gui::GameOptionsMenuHandler _menu_handler;
    //! \brief The main menu
    vt_gui::OptionBox _main_menu;
    //@}

    //! The menu bar displayed below the main menu.
    vt_video::StillImage _menu_bar;
    float _menu_bar_alpha;

    //! A text displayed a few second at startup to remind the player about the keys to use.
    vt_video::TextImage _f1_help_text;
    float _help_text_alpha;

    vt_system::SystemTimer _boot_timer;

    // ---------- Setup and refresh methods for boot menus

    //! \brief Returns whether at least one save is available to be loaded.
    bool _SavesAvailable();

    //! \brief Setup main menu
    //@{
    void _SetupMainMenu();
    //@}

    // ---------- Handler methods for boot menus

    //! \brief Handler methods for the main menu option box
    //@{
    void _OnNewGame();
    void _OnLoadGame();
    void _OnOptions();
    void _OnQuit();

#ifdef DEBUG_FEATURES
    //! \brief Open the debug script menu.
    void _DEBUG_OnDebugScriptList();

    //! \brief Run the selected debug script. The test function must be named 'TestFunction'.
    void _DEBUG_OnDebugScriptRun();

    //! \brief The option box listing available scripts.
    vt_gui::OptionBox _debug_script_menu;

    //! \brief The list of available files
    std::vector<std::string> _debug_scripts;

    //! \brief The debug scripts open.
    bool _debug_script_menu_open;

    //! \brief A simple menu window where the debug script list is displayed.
    vt_gui::MenuWindow _debug_scripts_window;
#endif
    //@}

    //! \brief Tests whether the language selection and the welcome help window should be shown.
    void _ShowLanguageSelectionWindow();
};

} // namespace vt_boot

#endif // __BOOT_HEADER__
