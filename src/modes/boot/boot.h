///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
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
#include "engine/script/script.h"

#include "modes/menu/menu_views.h"

#include "boot_menu.h"

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

//! \brief An internal namespace to be used only within the boot code. Don't use this namespace anywhere else!
namespace private_boot
{

//! \brief Key changing management enum.
enum WAIT_FOR {
    WAIT_KEY,
    WAIT_JOY_BUTTON,
    WAIT_JOY_AXIS
};

} // namespace private_boot

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
***
*** \todo BootMode uses SDL functions directly for many of its functions that handle
*** keyboard/joystick input. This code should probably be moved to the input engine
*** so that boot mode does not have to directly interface with one of the game's
*** libraries.
***
*** \todo BootMode makes use of some classes available in the menu mode code and
*** needs to directly include a menu mode header file to do so. This is not good
*** coding practice. The class shared by boot mode and menu mode should be moved
*** to somewhere in src/common and appropriately renamed, or BootMode should implement
*** its own version of this class and not include "menu_views.h" anywhere
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

private:
    //! \brief The boot state used to know whether the menu has to be displayed.
    BOOT_STATE _boot_state;

    //! \brief A pointer to the most recently active instance of boot mode
    static BootMode *_current_instance;

    //! \brief True when the boot mode is in exitting state.
    bool _exiting_to_new_game;

    //! \brief Set to true when the player has made modification to any application settings
    bool _has_modified_settings;

    //! \brief Stores languages' corresponding gettext PO file names, where index in vector is equivalent to the language name's position in the language options window
    std::vector<std::string> _po_files;

    //! \brief Rendered text of the release version number
    vt_video::TextImage _version_text;

    //! \brief A simple menu window where the various options menus are displayed
    vt_gui::MenuWindow _options_window;

    //! \brief Pointer to the currently active boot menu object
    private_boot::BootMenu *_active_menu;

    //! \brief Keeps in memory whether this is the first app run ever.
    bool _first_run;

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
    private_boot::BootMenu _main_menu;
    private_boot::BootMenu _options_menu;
    private_boot::BootMenu _video_options_menu;
    private_boot::BootMenu _resolution_menu;
    private_boot::BootMenu _audio_options_menu;
    private_boot::BootMenu _language_options_menu;
    private_boot::BootMenu _key_settings_menu;
    private_boot::BootMenu _joy_settings_menu;
    //@}

    //! \brief A pointer to the function to call when a key has been pressed when we're waiting for one
    void (BootMode::*_key_setting_function)(const SDLKey &);

    //! \brief A pointer to the function to call when a joystick button has been pressed when we're waiting for one
    void (BootMode::*_joy_setting_function)(uint8 button);

    //! \brief A pointer to the function to call when a joyystick axis has been moved when we're waiting for one
    void (BootMode::*_joy_axis_setting_function)(int8 axis);

    //! \brief Window display message for "select a key"
    vt_menu::MessageWindow _message_window;

    //! The menu bar displayed below the main menu.
    vt_video::StillImage _menu_bar;
    float _menu_bar_alpha;

    //! A text displayed a few second at startup to remind the player about the keys to use.
    vt_video::TextImage _f1_help_text;
    float _help_text_alpha;

    vt_system::SystemTimer _boot_timer;

    // ---------- Setup and refresh methods for boot menus

    //! \brief Returns whether at least one save is available to be loaded.
    bool _SavesAvailable(int32 maxId = 6);

    /** \brief Reloads all the menus to refresh their translated text.
    *** This is to be used only by the language menu.
    *** \Note Only menus with translatable texts are reloaded.
    **/
    void _ReloadTranslatableMenus();

    /** \brief Reloads the options window to display the new default GUI skin.
    *** Called in the video options menu.
    **/
    void _ReloadGUIDefaultSkin();

    //! \brief Setup functions for the various boot menus
    //@{
    void _SetupMainMenu();
    void _SetupOptionsMenu();
    void _SetupVideoOptionsMenu();
    void _SetupAudioOptionsMenu();
    void _SetupLanguageOptionsMenu();
    void _SetupKeySettingsMenu();
    void _SetupJoySettingsMenu();
    void _SetupResolutionMenu();
    //@}

    //! \brief Refreshes the option text displays on various option menus
    //@{
    void _RefreshVideoOptions();
    void _RefreshAudioOptions();
    void _RefreshLanguageOptions();
    void _RefreshKeySettings();
    void _RefreshJoySettings();
    //@}

    // ---------- Handler methods for boot menus

    //! \brief Handler methods for the main menu
    //@{
    void _OnNewGame();
    void _OnLoadGame();
    void _OnOptions();
    void _OnQuit();

#ifdef DEBUG_FEATURES
    void _DEBUG_OnBattle();
    void _DEBUG_OnMenu();
    void _DEBUG_OnShop();
#endif
    //@}

    //! \brief Handler methods for the primary options menu
    //@{
    void _OnVideoOptions();
    void _OnAudioOptions();
    void _OnLanguageOptions();
    void _OnKeySettings();
    void _OnJoySettings();
    //@}

    //! \brief Handler methods for the video options menu
    //@{
    void _OnToggleFullscreen();
    void _OnTogglePixelArtSmoothed();
    void _OnResolution();
    void _OnResolutionConfirm();
    void _OnBrightnessLeft();
    void _OnBrightnessRight();
    void _OnUIThemeLeft();
    void _OnUIThemeRight();
    //@}

    //! \brief Handler methods for the audio options menu
    //@{
    void _OnSoundLeft();
    void _OnSoundRight();
    void _OnMusicLeft();
    void _OnMusicRight();
    //@}

    //! \brief Handler methods for the language options menu
    //@{
    void _OnLanguageSelect();
    //@}

    //! \brief Handler methods for the key settings options menu
    //@{
    void _OnRestoreDefaultKeys();
    //@}

    //! \brief Handler methods for the joystick settings options menu
    //@{
    void _OnToggleJoystickEnabled();
    void _OnThresholdJoyLeft();
    void _OnThresholdJoyRight();
    void _OnRestoreDefaultJoyButtons();
    //@}

    // ---------- Helper methods not directly tied to any specific boot menu

    //! \brief Tests whether the language selection and the welcome help window should be shown.
    void _ShowLanguageSelectionWindow();

    /** \brief Shows the message window to display text that its waiting for either a joystick or keyboard event
    *** \param joystick True if the window should state its waiting for a joystick event, false for a keyboard event
    **/
    void _ShowMessageWindow(bool joystick);

    /** \brief Shows the message window to display text that its waiting for either a keyboard, joystick button, or joystick axis event
    *** \param wait The type of event the message box should state its waiting for
    **/
    void _ShowMessageWindow(private_boot::WAIT_FOR wait);

    /** \brief Changes the screen resolution, applies the new settings, and refreshes the video options
    *** \param width The width of the new resolution in pixels
    *** \param height The height of the new resolution in pixels
    *** \return whether the resolution was applied.
    **/
    bool _ChangeResolution(int32 width, int32 height);

    /** \brief Saves the settings to a file specified by the user
    *** \param filename the name of the file for the settings to be saved to, if a blank string is
    *** passed the default "settings.lua" will be used
    *** \return true if file could be saved, false otherwise
    **/
    bool _SaveSettingsFile(const std::string &filename = std::string());

    // ---------- Input configuration methods

    //! \brief Redefines a key command to be mapped to another key
    //@{
    void _RedefineUpKey();
    void _RedefineDownKey();
    void _RedefineLeftKey();
    void _RedefineRightKey();
    void _RedefineConfirmKey();
    void _RedefineCancelKey();
    void _RedefineMenuKey();
    void _RedefineMinimapKey();
    void _RedefinePauseKey();
    //@}

    /** \brief Wrapper functions to the InputManager used by the redefine key methods
    *** \note This was stated to be necessary because on Windows, having the _Redefine*Key() function
    *** pointers directly access the InputManager caused heap corruption.
    **/
    //@{
    void _SetUpKey(const SDLKey &key);
    void _SetDownKey(const SDLKey &key);
    void _SetLeftKey(const SDLKey &key);
    void _SetRightKey(const SDLKey &key);
    void _SetConfirmKey(const SDLKey &key);
    void _SetCancelKey(const SDLKey &key);
    void _SetMenuKey(const SDLKey &key);
    void _SetMinimapKey(const SDLKey &key);
    void _SetPauseKey(const SDLKey &key);
    //@}

    //! \brief Redefines a joystick command to be mapped to another axis/button
    //@{
    void _RedefineXAxisJoy();
    void _RedefineYAxisJoy();
    void _RedefineConfirmJoy();
    void _RedefineCancelJoy();
    void _RedefineMenuJoy();
    void _RedefineMinimapJoy();
    void _RedefinePauseJoy();
    void _RedefineQuitJoy();
    //@}

    /** \brief Wrapper functions to the InputManager used by the redefine joy methods
    *** \note This was stated to be necessary because on Windows, having the _Redefine*Joy() function
    *** pointers directly access the InputManager caused heap corruption.
    **/
    //@{
    void _SetXAxisJoy(int8 axis);
    void _SetYAxisJoy(int8 axis);
    void _SetJoyThreshold(int16 threshold);
    void _SetConfirmJoy(uint8 button);
    void _SetCancelJoy(uint8 button);
    void _SetMenuJoy(uint8 button);
    void _SetMinimapJoy(uint8 button);
    void _SetPauseJoy(uint8 button);
    void _SetQuitJoy(uint8 button);
    //@}
}; // class BootMode : public vt_mode_manager::GameMode

} // namespace vt_boot

#endif // __BOOT_HEADER__
