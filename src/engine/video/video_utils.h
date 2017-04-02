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
*** \file    video_utils.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Daniel Steuernol, steu@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for video utils constants.
***
*** Provides the default constants used for the Video component.
*** ***************************************************************************/

#ifndef __VIDEO_UTILS_HEADER__
#define __VIDEO_UTILS_HEADER__

//! \brief All calls to the video engine are wrapped in this namespace.
namespace vt_video
{

//! \brief The standard screen resolution
const float VIDEO_STANDARD_RES_WIDTH  = 1024.0f;
const float VIDEO_STANDARD_RES_HEIGHT = 768.0f;

//! \brief The number of FPS samples to retain across frames
const uint32_t FPS_SAMPLES = 250;

//! \brief Draw flags to control x and y alignment, flipping, and texture blending.
enum VIDEO_DRAW_FLAGS {
    VIDEO_DRAW_FLAGS_INVALID = -1,

    //! X draw alignment flags
    //@{
    VIDEO_X_LEFT = 1,
    VIDEO_X_CENTER = 2,
    VIDEO_X_RIGHT = 3,
    //@}

    //! Y draw alignment flags
    //@{
    VIDEO_Y_TOP = 4,
    VIDEO_Y_CENTER = 5,
    VIDEO_Y_BOTTOM = 6,
    //@}

    //! X flip flags
    //@{
    VIDEO_X_FLIP = 7,
    VIDEO_X_NOFLIP = 8,
    //@}

    //! Y flip flags
    //@{
    VIDEO_Y_FLIP = 9,
    VIDEO_Y_NOFLIP = 10,
    //@}

    //! Texture blending flags
    //@{
    VIDEO_NO_BLEND = 11,
    VIDEO_BLEND = 12,
    VIDEO_BLEND_ADD = 13,
    //@}

    VIDEO_DRAW_FLAGS_TOTAL = 14
};

} // namespace vt_video

#endif // __VIDEO_UTILS_HEADER__
