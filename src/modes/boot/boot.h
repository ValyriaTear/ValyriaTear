///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    boot.h
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \brief   Header file for boot mode interface.
***
*** This code handles the game event processing and frame drawing when the user
*** is in boot mode (the boot screen and menus).
*** ***************************************************************************/

#ifndef __BOOT_HEADER__
#define __BOOT_HEADER__

#include "defs.h"
#include "utils.h"

#include "mode_manager.h"
#include "video.h"

#include "boot_menu.h"
#include "menu_views.h"

//! \brief All calls to boot mode are wrapped in this namespace.
namespace hoa_boot {

//! \brief Determines whether the code in the hoa_boot namespace should print debug statements or not.
extern bool BOOT_DEBUG;

//! \brief An internal namespace to be used only within the boot code. Don't use this namespace anywhere else!
namespace private_boot {

const std::string _LANGUAGE_FILE = "dat/config/languages.lua";

//! \brief Various states that boot mode may be in
enum BOOT_STATE {
	BOOT_INVALID = 0,
	BOOT_INTRO   = 1,
	BOOT_MAIN    = 2,
	BOOT_LOAD    = 3,
	BOOT_OPTIONS = 4,
	BOOT_CREDITS = 5,
	BOOT_TOTAL   = 6
};

//! \brief ???
enum WAIT_FOR {
	WAIT_KEY,
	WAIT_JOY_BUTTON,
	WAIT_JOY_AXIS
};

//! \brief ???
enum PICK_LETTER {
	END = 27,
	BACK = 26,
	MAX_NAME = 19,
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
*** - "Credits": displays a rolling list of the game credits
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
*** \note BootMode is currently "hacked" to make it easy to arrive at and test
*** the various other game modes. This is temporary because we don't have any other
*** form of a testing interface for the game at this time. There are several methods
*** and variables pre-fixed with "TEMP" to indicate these testing routines. At some
*** point in the future, they should be removed permanently.
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
class BootMode : public hoa_mode_manager::GameMode {
public:
	BootMode();

	~BootMode();

	//! \brief Returns a pointer to the active instance of boot mode
	static BootMode* CurrentInstance()
		{ return _current_instance; }

	//! \brief Resets appropriate class members. Called whenever BootMode is made the active game mode.
	void Reset();

	//! \brief Handles user input and updates the boot menu
	void Update();

	//! \brief Draws the contents and menus of boot mode depending on the current state
	void Draw();

private:
	//! \brief Determines if this is the first time boot mode is entered. Used for the logo animation
	static bool _initial_entry;

	//! \brief A pointer to the most recently active instance of boot mode
	static BootMode* _current_instance;

	//! \brief When true boot mode is exiting and the screen should be faded out
	bool _fade_out;

	//! \brief Set to true when the player has made modification to any application settings
	bool _has_modified_settings;
	
	//! \brief Filename for profiles, this has to be global right now to work this will be fixed in the boot.cpp revision
	std::string _current_filename;
	
	//! \brief Stores languages' corresponding gettext PO file names, where index in vector is equivalent to the language name's position in the language options window
	std::vector<std::string> _po_files;
	
	//! \brief Images that will be used at the boot screen.
	std::vector<hoa_video::StillImage> _boot_images;

	//! \brief Music pieces to be used at the boot screen.
	std::vector<hoa_audio::MusicDescriptor> _boot_music;

	//! \brief Sounds that will be used at the boot screen.
	std::vector<hoa_audio::SoundDescriptor> _boot_sounds;

	//! \brief Rendered text of the release version number
	hoa_video::TextImage _version_text;

	//! \brief Rendered text of the game copyright notice
	hoa_video::TextImage _copyright_text;

	//! \brief A simple menu window where the various options menus are displayed
	hoa_gui::MenuWindow _options_window;

	//! \brief A window used to display the game credits
	private_boot::CreditsWindow* _credits_window;

	//! \brief A window used to display important information to the player
	private_boot::WelcomeWindow* _welcome_window;

	//! \brief Pointer to the currently active boot menu object
	private_boot::BootMenu* _active_menu;

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
	private_boot::BootMenu _user_input_menu;
	private_boot::BootMenu _profiles_menu;
	private_boot::BootMenu _load_profile_menu;
	private_boot::BootMenu _save_profile_menu;
	private_boot::BootMenu _delete_profile_menu;
	//@}

	//! \brief A pointer to the function to call when a key has been pressed when we're waiting for one
	void (BootMode::*_key_setting_function)(const SDLKey &);

	//! \brief A pointer to the function to call when a joystick button has been pressed when we're waiting for one
	void (BootMode::*_joy_setting_function)(uint8 button);

	//! \brief A pointer to the function to call when a joyystick axis has been moved when we're waiting for one
	void (BootMode::*_joy_axis_setting_function)(int8 axis);

	//! \brief The function to call when we want to overwrite
	//! \todo I don't understand this function pointer. Is it necessary? What does it overwrite?
	void (BootMode::*_overwrite_function) ();

	//! \brief Window display message for "select a key"
	hoa_menu::MessageWindow _message_window;

	//! \brief Window display message for "please type a file name"
	hoa_menu::MessageWindow _file_name_alert;

	//! \brief Window displays of the actual filename being typed
	hoa_menu::MessageWindow _file_name_window;

	// ---------- Setup and refresh methods for boot menus

	//! \brief Setup functions for the various boot menus
	//@{
	void _SetupMainMenu();
	void _SetupOptionsMenu();
	void _SetupVideoOptionsMenu();
	void _SetupAudioOptionsMenu();
	void _SetupLanguageOptionsMenu();
	void _SetupKeySetttingsMenu();
	void _SetupJoySetttingsMenu();
	void _SetupResolutionMenu();
	void _SetupProfileMenu();
	void _SetupLoadProfileMenu();
	void _SetupSaveProfileMenu();
	void _SetupDeleteProfileMenu();
	void _SetupUserInputMenu();
	//@}

	//! \brief Refreshes the option text displays on various option menus
	//@{
	void _RefreshVideoOptions();
	void _RefreshAudioOptions();
	void _RefreshKeySettings();
	void _RefreshJoySettings();
	void _RefreshSaveAndLoadProfiles();
	//@}

	// ---------- Handler methods for boot menus

	//! \brief Handler methods for the main menu
	//@{
	void _OnNewGame();
	void _OnLoadGame();
	void _OnOptions();
	void _OnCredits();
	void _OnQuit();

	void _TEMP_OnBattle();
	void _TEMP_OnMenu();
	void _TEMP_OnShop();
	//@}

	//! \brief Handler methods for the primary options menu
	//@{
	void _OnVideoOptions();
	void _OnAudioOptions();
	void _OnLanguageOptions();
	void _OnKeySettings();
	void _OnJoySettings();
	void _OnProfiles();
	//@}

	//! \brief Handler methods for the video options menu
	//@{
	void _OnToggleFullscreen();
	void _OnResolution();
	void _OnResolution640x480();
	void _OnResolution800x600();
	void _OnResolution1024x768();
	void _OnResolution1280x1024();
	void _OnBrightnessLeft();
	void _OnBrightnessRight();
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
	void _OnRestoreDefaultJoyButtons();
	//@}

	//! \brief Handler methods for the profiles options menu
	//@{
	void _OnLoadProfile();
	void _OnSaveProfile();
	void _OnDeleteProfile();
	//@}

	//! \brief Handler methods for the specific profile sub-options menus
	//@{
	//! \brief Loads the settings file specified by the user
	void _OnLoadFile();
	//! \brief Asks user for filename and then saves the settings to a .lua file
	void _OnSaveFile();
	//! \brief Deletes the profile
	void _OnDeleteFile();
	//! \brief Adds a letter to the currently selected filename
	void _OnPickLetter();
	//@}

	// ---------- Helper methods not directly tied to any specific boot menu

	//! \brief Draws the background image, logo and sword at their standard locations
	void _DrawBackgroundItems();

	/** \brief Animates the game logo when this class is first initialized
	*** The logo animation is performed in a series of sequences. When the animation is finished,
	*** the _initial_entry member is set to false. The audio that plays during this animation is
	*** a special music file (.ogg) that was created specifically to the timings of this animation.
	*** Therefore, don't screw with the timings in this method! A visual description of this
	*** animation is described below.
	***
	*** -#) When the animation sequence begins, initially the screen is completely black.
	*** -#) The logo gradually fades in from the background to appear on the center of the screen,
	***     with the sword placed horizontally as if it were "sheathed" inside the word "Allacros".
	***     The background remains black with only the logo partially visible.
	*** -#) After the logo fade in is complete, the sword slides out ("unsheathes") and moves to
	***     the right
	*** -#) After the sword is completely removed, it moves upwards and performs two 360 degree
	***     swings as if an invisible person was swinging/twirling it. The speed slows to a rest
	***     above the logo with the sword tip facing downward.
	*** -#) The sword then comes crashing down into the logo and sets into its final vertical
	***     position.
	*** -#) A brilliant flash of white light eminates from the sword, quickly whiting out the
	***     entire screen
	*** -#) The light fades away, now revealing the background image instead of a black backdrop
	***     and the logo is now located at the top center of the screen.
	*** -#) When the light fade finishes, the boot menu and other text instantly appear.
	**/
	void _AnimateLogo();

	//! \brief Immediately finishes and ends the opening logo animation
	void _EndLogoAnimation();

	/** \brief Shows the message window to display text that its waiting for either a joystick or keyboard event
	*** \param joystick True if the window should state its waiting for a joystick event, false for a keyboard event
	**/
	void _ShowMessageWindow(bool joystick);

	/** \brief Shows the message window to display text that its waiting for either a keybpard, joystick button, or joystick axis event
	*** \param wait The type of event the message box should state its waiting for
	**/
	void _ShowMessageWindow(private_boot::WAIT_FOR wait);

	/** \brief Changes the screen resolution, applies the new settings, and refreshes the video options
	*** \param width The width of the new resolution in pixels
	*** \param height The height of the new resolution in pixels
	**/
	void _ChangeResolution(int32 width, int32 height);

	/** \brief Saves the settings to a file specified by the user
	*** \param filename the name of the file for the settings to be loaded from if a blank string
	*** is passed the default "settings.lua" will be used
	*** \return true if file could be loaded, false otherwise
	**/
	bool _LoadSettingsFile(const std::string& filename);

	/** \brief Saves the settings to a file specified by the user
	*** \param filename the name of the file for the settings to be saved to, if a blank string is
	*** passed the default "settings.lua" will be used
	*** \return true if file could be saved, false otherwise
	**/
	bool _SaveSettingsFile(const std::string& filename);

	/** \brief Returns the directory listing for the user data path
	*** \return A vector listing all the files in the directory not including the default
	*** settings.lua file, this is meant for personalized profiles only
	**/
	std::vector<std::string> _GetDirectoryListingUserProfilePath();

	/** /brief Adds in the profiles as options under whichever menu you pass in
	**/
	void _AddProfileOptions(private_boot::BootMenu* menu);

	//! \brief Overwrites the selected profile Lua file
	void _OverwriteProfile();

	// ---------- Input configuration methods

	/** \brief Waits indefinitely until the player presses any key
	*** \return The SDL key symbol of the key which was pressed
	**/
	SDLKey _WaitKeyPress();

	/** \brief Waits indefinitely until the player presses any joystick button
	*** \return The SDL integer ID of the joystick button which was pressed
	**/
	uint8 _WaitJoyPress();

	//! \brief Redefines a key command to be mapped to another key
	//@{
	void _RedefineUpKey();
	void _RedefineDownKey();
	void _RedefineLeftKey();
	void _RedefineRightKey();
	void _RedefineConfirmKey();
	void _RedefineCancelKey();
	void _RedefineMenuKey();
	void _RedefineSwapKey();
	void _RedefineLeftSelectKey();
	void _RedefineRightSelectKey();
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
	void _SetSwapKey(const SDLKey &key);
	void _SetLeftSelectKey(const SDLKey &key);
	void _SetRightSelectKey(const SDLKey &key);
	void _SetPauseKey(const SDLKey &key);
	//@}

	//! \brief Redefines a joystick command to be mapped to another axis/button
	//@{
	void _RedefineXAxisJoy();
	void _RedefineYAxisJoy();
	void _RedefineThresholdJoy();
	void _RedefineConfirmJoy();
	void _RedefineCancelJoy();
	void _RedefineMenuJoy();
	void _RedefineSwapJoy();
	void _RedefineLeftSelectJoy();
	void _RedefineRightSelectJoy();
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
	void _SetConfirmJoy(uint8 button);
	void _SetCancelJoy(uint8 button);
	void _SetMenuJoy(uint8 button);
	void _SetSwapJoy(uint8 button);
	void _SetLeftSelectJoy(uint8 button);
	void _SetRightSelectJoy(uint8 button);
	void _SetPauseJoy(uint8 button);
	//@}
}; // class BootMode : public hoa_mode_manager::GameMode

} // namespace hoa_boot

#endif // __BOOT_HEADER__
