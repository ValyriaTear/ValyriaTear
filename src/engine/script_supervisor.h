///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    script_supervisor.h
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for managing scriptable animated effects for any game mode.
*** **************************************************************************/

#ifndef __SCRIPT_SUPERVISOR_HEADER__
#define __SCRIPT_SUPERVISOR_HEADER__

#include "utils/script/script_read.h"
#include "engine/video/video.h"

namespace vt_video {
enum VIDEO_DRAW_FLAGS;
}

namespace vt_mode_manager {
class GameMode;
}

class ScriptSupervisor
{
public:
    ScriptSupervisor()
    {}

    ~ScriptSupervisor();

    /** \brief Sets the name of the script to execute during the game mode
    *** \param filename The filename of the Lua script to load
    **/
    void AddScript(const std::string& filename) {
        if(!filename.empty()) _script_filenames.push_back(filename);
    }

    //! \brief Copy all the scene scripts filenames at once.
    void SetScripts(const std::vector<std::string>& scripts) {
        _script_filenames = scripts;
    }

    /** \brief Initializes all data necessary for the scripts to begin
    *** \param gm The game mode initializing the scene component.
    **/
    void Initialize(vt_mode_manager::GameMode *gm);

    ///! \brief Updates the different animations and media
    void Update();

    ///! \brief calls the reset function of each scripts.
    void Reset();

    ///! \brief calls the Restart function of each scripts.
    //! Used only when restarting a lost battle at the moment.
    void Restart();

    /** \brief Draws all background images and animations
    *** The images and effects drawn by this function will never be drawn over anything else
    *** (sprites, menus, etc.).
    **/
    void DrawBackground();

    /** \brief Draws all foreground images and animations
    *** The images and effects drawn by this function will be drawn over sprites,
    *** but not over the post effects and the gui.
    **/
    void DrawForeground();


    /** \brief Draws all foreground images and animations
    *** This permits custom effects just below the gui.
    **/
    void DrawPostEffects();

    /** \brief Loads a custom image, to be drawn through scripting
    *** \param filename The filename of the new image to load
    *** \return The image pointer.
    *** \note the object life cycle is handled by the engine, not the script.
    **/
    vt_video::StillImage* CreateImage(const std::string& filename);

    /** \brief Loads a custom lua animation files, to be drawn through scripting
    *** \param filename The filename of the new animated image to load
    *** \return id the id used to invoke the animation through scripted draw calls.
    *** \note the object life cycle is handled by the engine, not the script.
    **/
    vt_video::AnimatedImage* CreateAnimation(const std::string& filename);

    /** \brief Loads a custom TextImage, to be drawn by calling :Draw(Color&)
    *** \param text The text to display.
    *** \param style The TextStyle to use.
    *** \return The TextImage reference to use.
    **/
    vt_video::TextImage* CreateText(const vt_utils::ustring& text, const vt_video::TextStyle& style);
    vt_video::TextImage* CreateText(const std::string& text, const vt_video::TextStyle& style) {
        return CreateText(vt_utils::MakeUnicodeString(text), style);
    }

    //! \brief Used to permit changing a draw flag at boot time. Use with caution.
    void SetDrawFlag(vt_video::VIDEO_DRAW_FLAGS draw_flag);

private:
    //! \brief Contains a collection of custom created images
    std::vector<vt_video::TextImage*> _text_images;

    //! \brief Contains a collection of custom created images
    std::vector<vt_video::StillImage*> _still_images;

    //! \brief Contains a collection of custom created animations
    std::vector<vt_video::AnimatedImage*> _animated_images;

    //! \name Script data
    //@{
    //! \brief The name of the Lua files used to script this mode
    std::vector<std::string> _script_filenames;

    /** \brief Script functions which assists with the #Reset method
    *** Those functions execute any code that needs to be performed on a reset call. An example of
    *** one common operation is to reset the scene state when coming back to a given mode from another.
    **/
    std::vector<luabind::object> _reset_functions;

    /** \brief Script functions which assists with the #Restart method
    *** This function is called when a battle is restarted
    **/
    std::vector<luabind::object> _restart_functions;

    /** \brief Script functions which assists with the #Update method
    *** Those functions execute any code that needs to be performed on an update call. An example of
    *** one common operation is to detect certain conditions and respond appropriately, such as
    *** triggering a dialogue.
    **/
    std::vector<luabind::object> _update_functions;

    /** \brief Script functions which assists with the #DrawBackground method
    *** Those functions execute any code that needs to be performed on a draw call.
    *** This permits custom background effects.
    **/
    std::vector<luabind::object> _draw_background_functions;

    /** \brief Script functions which assists with the #DrawForeground method
    *** Those functions execute any code that needs to be performed on a draw call.
    *** This permits custom visual effects over the characters and enemies sprites.
    **/
    std::vector<luabind::object> _draw_foreground_functions;

    /** \brief Script functions which assists with the #DrawEffects methods
    *** Those functions execute any code that needs to be performed on a draw call.
    *** This permits custom effects just below the gui.
    **/
    std::vector<luabind::object> _draw_post_effects_functions;

    /** \brief Scripts objects keeping the corresponding lua coroutines alive.
    **/
    std::vector<vt_script::ReadScriptDescriptor*> _scene_scripts;
};

#endif
