///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
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

#include "utils.h"
#include "engine/script/script_read.h"
#include "engine/video/video.h"

namespace vt_mode_manager {
class GameMode;
}

class ScriptSupervisor
{
public:
    ScriptSupervisor()
    {}

    ~ScriptSupervisor()
    {}

    /** \brief Sets the name of the script to execute during the game mode
    *** \param filename The filename of the Lua script to load
    **/
    void AddScript(const std::string &filename) {
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


    /** \brief Loads a custom lua animation files, to be drawn through scripting
    *** \param filename The filename of the new background image to load
    *** \param width, height The animation image dimensions.
    *** \return id the id used to invoke the animation through scripted draw calls.
    **/
    int32 AddAnimation(const std::string &filename);
    int32 AddAnimation(const std::string &filename, float width, float height);

    /** \brief Draws a custom animation.
    *** \param custom image id, obtained through AddCustomAnimation()
    *** \param position screen position to draw at.
    **/
    void DrawAnimation(int32 id, float x, float y);

    /** \brief Loads a custom images, to be drawn through scripting
    *** \param filename The filename of the new background image to load
    *** \return id the id used to invoke the animation through scripted draw calls.
    **/
    int32 AddImage(const std::string &filename, float width, float height);

    /** \brief Draws a custom image.
    *** \param custom image id, obtained through AddCustomImage()
    *** \param position screen position to draw at.
    *** \param color to blend the image at.
    **/
    void DrawImage(int32 id, float x, float y, const vt_video::Color &color = vt_video::Color::white);

    //! \brief Same than @DrawImage but with a given rotation.
    void DrawRotatedImage(int32 id, float x, float y, const vt_video::Color &color, float angle);

    //! \brief Used to permit changing a draw flag at boot time. Use with caution.
    void SetDrawFlag(vt_video::VIDEO_DRAW_FLAGS draw_flag);

private:

    //! \brief Contains a collection of custom loaded images, usable to be drawn through scripting.
    std::vector<vt_video::StillImage> _script_images;

    //! \brief Contains a collection of custom loaded animations, usable to be drawn through scripting.
    std::vector<vt_video::AnimatedImage> _script_animations;

    //! \name Script data
    //@{
    //! \brief The name of the Lua files used to script this mode
    std::vector<std::string> _script_filenames;

    /** \brief Script functions which assists with the #Reset method
    *** Those functions execute any code that needs to be performed on a reset call. An example of
    *** one common operation is to reset the scene state when coming back to a given mode from another.
    **/
    std::vector<ScriptObject> _reset_functions;

    /** \brief Script functions which assists with the #Update method
    *** Those functions execute any code that needs to be performed on an update call. An example of
    *** one common operation is to detect certain conditions and respond appropriately, such as
    *** triggering a dialogue.
    **/
    std::vector<ScriptObject> _update_functions;

    /** \brief Script functions which assists with the #DrawBackground method
    *** Those functions execute any code that needs to be performed on a draw call.
    *** This permits custom background effects.
    **/
    std::vector<ScriptObject> _draw_background_functions;

    /** \brief Script functions which assists with the #DrawForeground method
    *** Those functions execute any code that needs to be performed on a draw call.
    *** This permits custom visual effects over the characters and enemies sprites.
    **/
    std::vector<ScriptObject> _draw_foreground_functions;

    /** \brief Script functions which assists with the #DrawEffects methods
    *** Those functions execute any code that needs to be performed on a draw call.
    *** This permits custom effects just below the gui.
    **/
    std::vector<ScriptObject> _draw_post_effects_functions;
};

#endif
