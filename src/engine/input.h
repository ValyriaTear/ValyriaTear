///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file   input.h
*** \author Tyler Olsen, roots@allacrost.org
*** \author Yohann Ferreira, yohann ferreira orange fr
*** \brief  Header file for processing user input
*** **************************************************************************/

#ifndef __INPUT_HEADER__
#define __INPUT_HEADER__

#include "utils/utils_strings.h"
#include "utils/singleton.h"

//! All calls to the input engine are wrapped in this namespace.
namespace vt_input
{

class InputEngine;

//! The singleton pointer responsible for handling and updating user input.
extern InputEngine *InputManager;

//! Determines whether the code in the vt_input namespace should print debug statements or not.
extern bool INPUT_DEBUG;

//! An internal namespace to be used only within the input code.
namespace private_input
{

/** ***************************************************************************
*** \brief Retains information about the user-defined key settings.
***
*** This class is simply a container for various SDLKey structures that represent
*** the game's input keys.
*** **************************************************************************/
class KeyState
{
public:
    /** \name Generic key code names (layout-dependant, not scancodes)
    *** \brief Each member holds the actual keyboard key that corresponds to the named key event.
    *** \note that SDLK_ESCAPE and SDLK_F1 are reserved for Quit, and Help.
    **/
    //@{
    SDL_Keycode up;
    SDL_Keycode down;
    SDL_Keycode left;
    SDL_Keycode right;
    SDL_Keycode confirm;
    SDL_Keycode cancel;
    SDL_Keycode menu;
    SDL_Keycode minimap;
    SDL_Keycode pause;
    //@}
}; // class KeyState

/** ***************************************************************************
*** \brief Retains information about the user-defined joystick settings.
***
*** This class is simply a container for various SDL structures that represent
*** the joystick input. Because joystick axis movement is not a simple "on/off"
*** state as opposed to keys, we need a little extra logic so that it can be
*** represented as such. In the range of possible joystick values (-32768 to 32767),
*** we section off the region into thirds and label any crossing of these 'boundaries'
*** as state changes.
*** **************************************************************************/
class JoystickState
{
public:
    //! A pointer to the active joystick.
    SDL_Joystick *js;

    //! An index to the SDL joystick which should be made active.
    int32 joy_index;

    //! \name Generic button names.
    /**
    ***
    **/
    //@{
    //! \brief Each member retains the index that refers to the joystick button registered to the event.
    uint8 confirm;
    uint8 cancel;
    uint8 menu;
    uint8 minimap;
    uint8 pause;
    uint8 quit;
    //@}

    //! \brief Identify which axes to use for x and y.
    int8 x_axis;
    int8 y_axis;

    //! \brief The threshold value we use to partition the range of joystick values into on and off
    uint16 threshold;
}; // class JoystickState

} // namespace private_input

/** ***************************************************************************
*** \brief Processes and manages all user input events.
***
*** The way this class operates is by first retaining the user-defined keyboard
*** and joystick settings. The EventHandler() function is called once every
*** iteration of the main game loop to process all events that have accumulated
*** in the SDL input queue. Three boolean varaiables for each type of input event
*** are maintained to represent the state of each input:
***
*** - state   :: for when a key/button is being held down
*** - press   :: for when a key/button was previously untouched, but has since been pressed
*** - release :: for when a key/button was previously held down, but has since been released
***
*** The names of the primary game input events and their purposes are listed below:
***
*** - up           :: Moves a cursor/sprite upwards
*** - down         :: Moves a cursor/sprite downwards
*** - left         :: Moves a cursor/sprite left
*** - right        :: Moves a cursor/sprite right
*** - confirm      :: Confirms a menu selection or command
*** - cancel       :: Cancels a menu selection or command
*** - menu         :: Opens up a menu
*** - minimap      :: Used to toggle the minimap view when there is one.
*** - pause        :: Pauses the game
***
*** There are also other events and meta-key combination events that are handled within
*** this class itself:
***
*** - Ctrl+F     :: toggles the game between running in windowed and full screen mode
*** - Ctrl+Q     :: brings up the quit menu/quits the game
*** - Ctrl+S     :: saves a screenshot of the current screen
*** - Quit Event :: same as Ctrl+Q, this happens when the user tries to close the game window
***
*** \note This class is a singleton.
***
*** \note Unlike other inputs, pause and quit events are only monitored by presses and have no
*** state or release methods.
***
*** \note Keep in mind that these events are \b not mutually exclusive (an up press and a down
*** press may be registered at the same time). This class does not attempt to give one
*** event precedence over the other, except in the case of pause and quit events. Therefore,
*** your code you should deal with the problem of not having mutual exclusive events directly.
***
*** \note Because this class will be used quite often to check the status of the various
*** booleans, encapsulation has been used so that one can't accidentally change the value
*** of one of the members and introduce hard-to-find bugs in the code.
*** (eg. `if (up_state = true)` instead of `if (up_state == true)`.
***
*** \note In the end, all you really need to know about this class are the
*** member access functions in the public section of this class (its not that hard).
*** **************************************************************************/
class InputEngine : public vt_utils::Singleton<InputEngine>
{
    friend class vt_utils::Singleton<InputEngine>;

private:
    InputEngine();

    //! Holds the current user-defined key settings
    private_input::KeyState _key;

    //! Holds the current user-defined joystick settings
    private_input::JoystickState _joystick;

    //! \brief Tells whether the joystick input is disabled.
    //! Is useful on certain OS where other inputs are falsely taken as joysticks ones.
    bool _joysticks_enabled;

    //! Any key (or joystick button) pressed
    bool _any_key_press;

    //! Any key released
    bool _any_key_release;

    //! Any joystick axis moved
    int8 _last_axis_moved;

    /** \name  Input State Members
    *** \brief Retain whether an input key/button is currently being held down
    **/
    //@{
    bool _up_state;
    bool _down_state;
    bool _left_state;
    bool _right_state;
    bool _confirm_state;
    bool _cancel_state;
    //@}

    /** \name  Input Press Members
    *** \brief Retain whether an input key/button was just pressed
    **/
    //@{
    bool _up_press;
    bool _down_press;
    bool _left_press;
    bool _right_press;
    bool _confirm_press;
    bool _cancel_press;
    bool _menu_press;
    bool _minimap_press;
    bool _pause_press;
    bool _quit_press;
    bool _help_press;
    //@}

    /** \name  Input Release Members
    *** \brief Retain whether an input key/button was just released
    **/
    //@{
    bool _up_release;
    bool _down_release;
    bool _left_release;
    bool _right_release;
    bool _confirm_release;
    bool _cancel_release;
    bool _menu_release;
    bool _minimap_release;
    //@}

    /** \name  D-Pad/ Hat Input State Members
    *** \brief Retain whether an input key/button is currently being held down
    **/
    //@{
    bool _hat_up_state;
    bool _hat_down_state;
    bool _hat_left_state;
    bool _hat_right_state;
    //@}

    /** \brief Most recent SDL event
     **/
    SDL_Event _event;

    /** \brief Processes all keyboard input events
    *** \param key_event The event to process
    **/
    void _KeyEventHandler(SDL_KeyboardEvent &key_event);

    /** \brief Processes all joystick input events
    *** \param js_event The event to process
    **/
    void _JoystickEventHandler(SDL_Event &js_event);

    /** \brief Sets a new key over an older one. If the same key is used elsewhere, the older one is removed
    *** \param old_key key to be replaced (_key.up for example)
    *** \param new_key key to replace the old value
    **/
    void _SetNewKey(SDL_Keycode &old_key, SDL_Keycode new_key);

    /** \brief Sets a new joystick button over an older one. If the same button is used elsewhere, the older one is removed
    *** \param old_button to be replaced (_joystick.confirm for example)
    *** \param new_button button to replace the old value
    **/
    void _SetNewJoyButton(uint8 &old_button, uint8 new_button);
public:
    ~InputEngine();

    bool SingletonInitialize()
    { return true; }

    //! \brief Initialize the joysticks with SDL, delayed because we need info from the lua settings file first.
    void InitializeJoysticks();

    //! \brief Deinitialize the joysticks, if initialized.
    void DeinitializeJoysticks();

    /** \brief Loads the default key settings from the lua file and sets them back
    *** \return Returns false if the settings file couldn't be read
    **/
    bool RestoreDefaultKeys();

    /** \brief Loads the default joystick settings from the lua file and sets them back
    *** \return Returns false if the settings file couldn't be read
    **/
    bool RestoreDefaultJoyButtons();

    /** \brief Checks if any keyboard key or joystick button is pressed
    *** \return True if any key/button is pressed
    **/
    bool AnyKeyPress();

    /** \brief Checks if any keyboard key or joystick button is released
    *** \return True if any key/button is released
    **/
    bool AnyKeyRelease();

    /** \brief Returns the last joystick axis that has moved
    *** \return True if any joystick axis has moved
    **/
    uint8 GetLastAxisMoved() {
        return _last_axis_moved;
    }

    void ResetLastAxisMoved() {
        _last_axis_moved = -1;
    }

    /** \brief Examines the SDL queue for all user input events and calls appropriate sub-functions.
    ***
    *** This function handles all the meta keyboard events (events when a modifier key like Ctrl or
    *** Alt is held down) and all miscellaneous user input events (like clicking on the window button
    *** to quit the game). Any keyboard or joystick events that occur are passed to the KeyEventHandler()
    *** and JoystickEventHandler() functions.
    ***
    *** \note EventHandler() should only be called in the main game loop. Do \b not call it anywhere else.
    **/
    void EventHandler();

    /** \name   Input state member access functions
    *** \return True if the input event key/button is being held down
    **/
    //@{
    bool UpState() const {
        return _up_state || _hat_up_state;
    }

    bool DownState() const {
        return _down_state || _hat_down_state;
    }

    bool LeftState() const {
        return _left_state || _hat_left_state;
    }

    bool RightState() const {
        return _right_state || _hat_right_state;
    }

    bool ConfirmState() const {
        return _confirm_state;
    }

    bool CancelState() const {
        return _cancel_state;
    }
    //@}

    /** \name Input press member access functions
    *** \return True if the input event key/button has just been pressed
    **/
    //@{
    bool UpPress() const {
        return _up_press;
    }

    bool DownPress() const {
        return _down_press;
    }

    bool LeftPress() const {
        return _left_press;
    }

    bool RightPress() const {
        return _right_press;
    }

    bool ConfirmPress() const {
        return _confirm_press;
    }

    bool CancelPress() const {
        return _cancel_press;
    }

    bool MenuPress() const {
        return _menu_press;
    }

    bool MinimapPress() const {
        return _minimap_press;
    }

    bool PausePress() const {
        return _pause_press;
    }

    bool QuitPress() const {
        return _quit_press;
    }

    bool HelpPress() const {
        return _help_press;
    }
    //@}

    /** \name Input release member access functions
    *** \return True if the input event key/button has just been released
    **/
    //@{
    bool UpRelease() const {
        return _up_release;
    }

    bool DownRelease() const {
        return _down_release;
    }

    bool LeftRelease() const {
        return _left_release;
    }

    bool RightRelease() const {
        return _right_release;
    }

    bool ConfirmRelease() const {
        return _confirm_release;
    }

    bool CancelRelease() const {
        return _cancel_release;
    }

    bool MenuRelease() const {
        return _menu_release;
    }

    bool MinimapRelease() const {
        return _minimap_release;
    }
    //@}

    /** \name Key name access functions
    *** \return Name of the key in std::string format
    **/
    //@{
    std::string GetUpKeyName() const {
        return vt_utils::UpcaseFirst(SDL_GetKeyName(_key.up));
    }

    std::string GetDownKeyName() const {
        return vt_utils::UpcaseFirst(SDL_GetKeyName(_key.down));
    }

    std::string GetLeftKeyName() const {
        return vt_utils::UpcaseFirst(SDL_GetKeyName(_key.left));
    }

    std::string GetRightKeyName() const {
        return vt_utils::UpcaseFirst(SDL_GetKeyName(_key.right));
    }

    std::string GetConfirmKeyName() const {
        return vt_utils::UpcaseFirst(SDL_GetKeyName(_key.confirm));
    }

    std::string GetCancelKeyName() const {
        return vt_utils::UpcaseFirst(SDL_GetKeyName(_key.cancel));
    }

    std::string GetMenuKeyName() const {
        return vt_utils::UpcaseFirst(SDL_GetKeyName(_key.menu));
    }

    std::string GetMinimapKeyName() const {
        return vt_utils::UpcaseFirst(SDL_GetKeyName(_key.minimap));
    }

    std::string GetPauseKeyName() const {
        return vt_utils::UpcaseFirst(SDL_GetKeyName(_key.pause));
    }

    std::string GetHelpKeyName() const {
        return vt_utils::UpcaseFirst(SDL_GetKeyName(SDLK_F1));
    }

    std::string GetQuitKeyName() const {
        return vt_utils::UpcaseFirst(SDL_GetKeyName(SDLK_ESCAPE));
    }
    //@}

    //! \brief Tells whether joysticks should have enabled or not.
    //! \note this isn't representing the SDL subsystem state, but a game option
    //! preventing them from being initialized if necessary.
    bool GetJoysticksEnabled() const {
        return _joysticks_enabled;
    }

    /** \name Joystick axis access functions
    *** \return axis number or threshold value
    **/
    //@{
    int8 GetXAxisJoy() const {
        return _joystick.x_axis;
    }

    int8 GetYAxisJoy() const {
        return _joystick.y_axis;
    }

    int16 GetThresholdJoy() const {
        return _joystick.threshold;
    }
    //@}

    /** \name Joystick button handle access functions
    *** \return Joystick button number for the action
    **/
    //@{
    int32 GetConfirmJoy() const {
        return _joystick.confirm;
    }

    int32 GetCancelJoy() const {
        return _joystick.cancel;
    }

    int32 GetMenuJoy() const {
        return _joystick.menu;
    }

    int32 GetMinimapJoy() const {
        return _joystick.minimap;
    }

    int32 GetPauseJoy() const {
        return _joystick.pause;
    }

    int32 GetQuitJoy() const {
        return _joystick.quit;
    }
    //@}

    /** \name Key re-mapping functions
    *** \param key New key for the action
    **/
    //@{
    void SetUpKey(const SDL_Keycode &key) {
        _SetNewKey(_key.up, key);
    }

    void SetDownKey(const SDL_Keycode &key) {
        _SetNewKey(_key.down, key);
    }

    void SetLeftKey(const SDL_Keycode &key) {
        _SetNewKey(_key.left, key);
    }

    void SetRightKey(const SDL_Keycode &key) {
        _SetNewKey(_key.right, key);
    }

    void SetConfirmKey(const SDL_Keycode &key) {
        _SetNewKey(_key.confirm, key);
    }

    void SetCancelKey(const SDL_Keycode &key) {
        _SetNewKey(_key.cancel, key);
    }

    void SetMenuKey(const SDL_Keycode &key) {
        _SetNewKey(_key.menu, key);
    }

    void SetMinimapKey(const SDL_Keycode &key) {
        _SetNewKey(_key.minimap, key);
    }

    void SetPauseKey(const SDL_Keycode &key) {
        _SetNewKey(_key.pause, key);
    }
    //@}

    //! \brief Tells whether joysticks should have enabled or not.
    //! \note this isn't representing the SDL subsystem state, but a game option
    //! preventing them from being initialized if necessary.
    void SetJoysticksEnabled(bool enabled) {
        _joysticks_enabled = enabled;
    }

    /** \name Joystick button re-mapping functions
    *** \param	key New button for the action
    **/
    //@{
    void SetJoyIndex(int32 joy_index) {
        _joystick.joy_index = joy_index;
    }

    void SetConfirmJoy(uint8 button) {
        _SetNewJoyButton(_joystick.confirm, button);
    }

    void SetCancelJoy(uint8 button) {
        _SetNewJoyButton(_joystick.cancel, button);
    }

    void SetMenuJoy(uint8 button) {
        _SetNewJoyButton(_joystick.menu, button);
    }

    void SetMinimapJoy(uint8 button) {
        _SetNewJoyButton(_joystick.minimap, button);
    }

    void SetPauseJoy(uint8 button) {
        _SetNewJoyButton(_joystick.pause, button);
    }

    void SetQuitJoy(uint8 button) {
        _SetNewJoyButton(_joystick.quit, button);
    }

    void SetXAxisJoy(int8 axis) {
        _joystick.x_axis = axis;
    }

    void SetYAxisJoy(int8 axis) {
        _joystick.y_axis = axis;
    }

    void SetThresholdJoy(int16 threshold) {
        _joystick.threshold = threshold;
    }
    //@}

    /** \name   Returns currently set keys' virtual key codes (SDLKeys)
    *** \return Integer according to the currently set key
    **/
    //@{
    int32 GetUpKey() const {
        return _key.up;
    }

    int32 GetDownKey() const {
        return _key.down;
    }

    int32 GetLeftKey() const {
        return _key.left;
    }

    int32 GetRightKey() const {
        return _key.right;
    }

    int32 GetConfirmKey() const {
        return _key.confirm;
    }

    int32 GetCancelKey() const {
        return _key.cancel;
    }

    int32 GetMenuKey() const {
        return _key.menu;
    }

    int32 GetMinimapKey() const {
        return _key.minimap;
    }

    int32 GetPauseKey() const {
        return _key.pause;
    }
    //@}

    //! \brief Returns the most recent event retrieved from SDL
    const SDL_Event &GetMostRecentEvent() const {
        return _event;
    }
}; // class InputEngine : public vt_utils::Singleton<InputEngine>

} // namespace vt_input

#endif
