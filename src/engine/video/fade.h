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
*** \file    fade.h
*** \author  Raj Sharma, roos@allacrost.org
*** \brief   Header file for ScreenFader class.
*** ***************************************************************************/

#ifndef __FADE_HEADER__
#define __FADE_HEADER__

#include "color.h"
#include "image.h"

namespace hoa_video
{

namespace private_video
{

/** ****************************************************************************
*** \brief Used to monitor progress for a fading screen.
***
*** This class is used internally by the video engine to calculate how much to
*** fade the screen by. It keeps track of the current color and figures out whether
*** it should implement the fade using modulation or an overlay.
***
*** \note Fades are either implemented with overlays or with modulation, depending
*** on whether it's a simple fade to black or a fade to a different color.
*** ***************************************************************************/
class ScreenFader
{
public:
    ScreenFader();

    /** \brief Begins a new screen fading process
    *** \param final The color to fade the screen to.
    *** \param time The number of milliseconds that the fade should last for.
    *** \param transitional whether the fading is done between two game modes.
    **/
    void BeginFade(const Color &final, uint32 time, bool transitional = false);

    /** \brief Updates the amount of fading for the screen
    *** \param time The number of milliseconds that have passed since last update.
    **/
    void Update(uint32 time);

    bool IsFading() const {
        return _is_fading;
    }

    /** \brief start a fade used as a transition between two game modes.
    *** It's a simple fade but flagged as special to let the mode manager handle it.
    **/
    void StartTransitionFadeOut(const Color &final, uint32 time) {
        BeginFade(final, time, true);
    }

    //! \brief A shortcut function used to make a fade in more explicitely.
    void TransitionalFadeIn(uint32 time) {
        BeginFade(Color::clear, time, true);
    }

    //! \brief A shortcut function used to make a fade in more explicitely.
    void FadeIn(uint32 time) {
        BeginFade(Color::clear, time);
    }

    //! \brief tells whether the last fade effect was transitional.
    bool IsLastFadeTransitional() const {
        return _transitional_fading;
    }

    //! \brief Draw the fading overlay
    void Draw();

private:
    //! \brief The current overlay color.
    Color _current_color;

    //! \brief The initial color of the screen before the fade started.
    Color _initial_color;

    //! \brief The destination color that the screen is being fade to.
    Color _final_color;

    //! \brief The number of milliseconds that have passed since the fading began.
    uint32 _current_time;

    //! \brief The number of milliseconds that this fade was set to last for.
    uint32 _end_time;

    //! \brief True if the class is currently in the process of fading
    bool _is_fading;

    //! Image used as a fading overlay
    StillImage _fade_overlay_img;

    //! \brief Set to true if the fading process requires interpolation of RGB values between colors
    bool _interpolate_rgb_values;

    //! \brief Tells whether the current fading is used to make a transition between two game mode.
    bool _transitional_fading;
    //@}
}; // class ScreenFader

} // namespace private_video

} // namespace hoa_video

#endif // __FADE_HEADER__
