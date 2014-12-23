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
*** \file    options_handler.h
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the options menus
*** ***************************************************************************/

#ifndef __OPTION_HANDLER__
#define __OPTION_HANDLER__

#include "common/gui/option.h"
#include "common/gui/menu_window.h"

#include "common/message_window.h"

namespace vt_gui
{

namespace private_gui
{

class GameOptionsMenuHandler;

/** ****************************************************************************
*** \brief Used for the construction and operation of all boot mode menus
***
*** This class is an extension of the OptionBox class found in the GUI code. Its
*** primary feature is that it utilizes function pointers to the BootMode class,
*** which makes this class incredibly flexible and versatile. The way it works
*** is to keep several containers of function pointers, where the size of these
*** containers are equal to the number of options in the menu. The function pointers
*** are invoked on the selected option when an input command is received. For example,
*** if the second option is selected and a confirm press is registered, the appropriate
*** function pointed to for that option and that input event will be called. Other
*** than this simple yet powerful feature, this class operates and acts exactly the same
*** as a standard OptionBox object.
***
*** \note There are some OptionBox methods which should not be used for this class. Particularly
*** any methods that add or remove options should be avoided because they do not know to modify
*** the function pointer containers appropriately. Use only the methods specific to this class
*** to add or remove options.
*** ***************************************************************************/
class OptionMenu : public vt_gui::OptionBox
{
public:
    OptionMenu():
        _handler(NULL)
    {}

    ~OptionMenu()
    {}

    /** \brief Adds a new option to the menu with the desired function pointers attached
    *** \param text A text representing the new option
    *** \param object The GameOptionsMenuHandler Object pointer.
    *** \param *up_function BootMode handler function for up input events
    *** \param *down_function BootMode handler function for down input events
    *** \param *confirm_function BootMode handler function for confirm input events
    *** \param *left_function BootMode handler function for left input events
    *** \param *right_function BootMode handler function for right input events
    **/
    void AddOption(const vt_utils::ustring &text,
                   GameOptionsMenuHandler* handler,
                   void (GameOptionsMenuHandler::*confirm_function)() = NULL,
                   void (GameOptionsMenuHandler::*up_function)() = NULL,
                   void (GameOptionsMenuHandler::*down_function)() = NULL,
                   void (GameOptionsMenuHandler::*left_function)() = NULL,
                   void (GameOptionsMenuHandler::*right_function)() = NULL);

    //! \brief
    //@{
    void InputConfirm();
    void InputUp();
    void InputDown();
    void InputLeft();
    void InputRight();
    //@}

private:
    //! \brief the Handler within the functions are stored.
    GameOptionsMenuHandler* _handler;

    //! \brief Confirm input handlers for all options in the menu
    std::vector<void (GameOptionsMenuHandler::*)()> _confirm_handlers;

    //! \brief Up input handlers for all options in the menu
    std::vector<void (GameOptionsMenuHandler::*)()> _up_handlers;

    //! \brief Down input handlers for all options in the menu
    std::vector<void (GameOptionsMenuHandler::*)()> _down_handlers;

    //! \brief Left input handlers for all options in the menu
    std::vector<void (GameOptionsMenuHandler::*)()> _left_handlers;

    //! \brief Right input handlers for all options in the menu
    std::vector<void (GameOptionsMenuHandler::*)()> _right_handlers;
}; // class GameOptionsMenuHandlerMenu : public vt_video::OptionBox


//! \brief Key changing management enum.
enum WAIT_FOR {
    WAIT_KEY,
    WAIT_JOY_BUTTON,
    WAIT_JOY_AXIS
};

class GameOptionsMenuHandler {
public:
    //! Initializes all the common options menus
    GameOptionsMenuHandler(vt_mode_manager::GameMode* parent_mode);

    //! Deinit the handler, also saving the config
    ~GameOptionsMenuHandler();

    //! \brief Update the input and option menus
    void Update();

    //! \brief Draw the visible menu on screen
    void Draw();

    //! \brief Activate the menu handler
    void Activate();

    //! \brief Tells whether the handler is displaying some menu
    //! and/or handling some input.
    bool IsActive() const {
        return (_active_menu != NULL);
    }

    //! \brief Show first app run language option menu
    //! This is a special case handling function.
    //! This function will make a language selection option menu appear
    //! and select the language with the first key press that isn't an arrow key/joypad button.
    void ShowFirstRunLanguageSelection();

    //! \brief Tells whether the handler is showing the first run language menu.
    bool IsShowingFirstRunLanguageMenu() const {
        return _first_run;
    }

    //! \brief Reloads the menu including translatable texts to clearly show
    //! The change there.
    void ReloadTranslatableMenus();

private:
    //! \brief Keeps in memory whether this is the first app run ever.
    bool _first_run;

    //! \brief Set to true when the player has made modification to any application settings
    bool _has_modified_settings;

    //! \brief Stores languages' corresponding gettext PO file names, where index in vector is equivalent to the language name's position in the language options window
    std::vector<std::string> _po_files;

    //! \brief The available screen resolutions
    std::vector<SDL_DisplayMode> _resolution_list;

    //! \brief A simple menu window where the various options menus are displayed.
    vt_gui::MenuWindow _options_window;

    //! \brief Pointer to the currently active boot menu object. Do not delete it.
    OptionMenu *_active_menu;

    OptionMenu _options_menu;
    OptionMenu _video_options_menu;
    OptionMenu _resolution_menu;
    OptionMenu _audio_options_menu;
    OptionMenu _game_options_menu;
    OptionMenu _language_options_menu;
    OptionMenu _key_settings_menu;
    OptionMenu _joy_settings_menu;

    //! \brief A pointer to the function to call when a key has been pressed when we're waiting for one
    void (GameOptionsMenuHandler::*_key_setting_function)(const SDL_Keycode &);

    //! \brief A pointer to the function to call when a joystick button has been pressed when we're waiting for one
    void (GameOptionsMenuHandler::*_joy_setting_function)(uint8 button);

    //! \brief A pointer to the function to call when a joystick axis has been moved when we're waiting for one
    void (GameOptionsMenuHandler::*_joy_axis_setting_function)(int8 axis);

    //! \brief Window display message for "select a key".
    vt_common::MessageWindow _message_window;

    //! \brief A menu window explaining the currently selected option.
    vt_common::MessageWindow _explanation_window;

    //! \brief The parent game mode the handler is a component of. Don't delete this.
    vt_mode_manager::GameMode* _parent_mode;

    /** \brief Reloads all the menus to refresh their translated text.
    *** This is to be used only by the language menu.
    *** \Note Only menus with translatable texts are reloaded.
    **/
    void _ReloadTranslatableMenus();

    /** \brief Reloads the options window to display the new default GUI skin.
    *** Called in the video options menu.
    **/
    void _ReloadGUIDefaultSkin();

    //! \brief Setup the different menus
    void _SetupOptionsMenu();
    void _SetupVideoOptionsMenu();
    void _SetupAudioOptionsMenu();
    void _SetupGameOptions();
    void _SetupLanguageOptionsMenu();
    void _SetupKeySettingsMenu();
    void _SetupJoySettingsMenu();
    void _SetupResolutionMenu();

    //! \brief Refreshes the option text displays on various option menus
    void _RefreshVideoOptions();
    void _RefreshAudioOptions();
    void _RefreshGameOptions();
    void _RefreshLanguageOptions();
    void _RefreshKeySettings();
    void _RefreshJoySettings();

    //! \brief Handler methods for the primary options menu
    void _OnVideoOptions();
    void _OnAudioOptions();
    void _OnGameOptions();
    void _OnLanguageOptions();
    void _OnKeySettings();
    void _OnJoySettings();

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

    //! \brief Handler methods for the game options menu
    //@{
    void _OnDialogueSpeedLeft();
    void _OnDialogueSpeedRight();
    void _OnBattleTargetCursorMemoryConfirm();
    void _OnGameDifficultyConfirm();
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

    /** \brief Shows the message window to display text that its waiting for either a joystick or keyboard event
    *** \param joystick True if the window should state its waiting for a joystick event, false for a keyboard event
    **/
    void _ShowMessageWindow(bool joystick);

    /** \brief Shows the message window to display text that its waiting for either a keyboard, joystick button, or joystick axis event
    *** \param wait The type of event the message box should state its waiting for
    **/
    void _ShowMessageWindow(private_gui::WAIT_FOR wait);

    //! \brief Sets the explanation text according to the current selection.
    void _UpdateExplanationText();

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
    bool _SaveSettingsFile(const std::string& filename = std::string());

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
    void _SetUpKey(const SDL_Keycode &key);
    void _SetDownKey(const SDL_Keycode &key);
    void _SetLeftKey(const SDL_Keycode &key);
    void _SetRightKey(const SDL_Keycode &key);
    void _SetConfirmKey(const SDL_Keycode &key);
    void _SetCancelKey(const SDL_Keycode &key);
    void _SetMenuKey(const SDL_Keycode &key);
    void _SetMinimapKey(const SDL_Keycode &key);
    void _SetPauseKey(const SDL_Keycode &key);
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
    void _RedefineHelpJoy();
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
    void _SetHelpJoy(uint8 button);
    void _SetQuitJoy(uint8 button);
    //@}
};

} // namespace private_gui

} // namespace vt_gui

#endif // __OPTION_HANDLER__
