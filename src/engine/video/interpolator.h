///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    interpolator.h
*** \author  Raj Sharma, roos@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for Interpolator class
***
*** The Interpolator class can interpolate between a single and final value
*** using various methods (linear, fast, slow, etc.)
*** ***************************************************************************/

#ifndef __INTERPOLATOR_HEADER__
#define __INTERPOLATOR_HEADER__

namespace vt_mode_manager
{

//! \brief The various ways to create smoothed values between two numbers (e.g. linear interpolation)
enum InterpolationMethod {
    INTERPOLATE_INVALID = -1,

    //! Rise from A to B and then down to A again
    INTERPOLATE_EASE = 0,
    //! Keeps a constant value of A
    INTERPOLATE_SRCA = 1,
    //! Keeps a constant value of B
    INTERPOLATE_SRCB = 2,
    //! Rises quickly at the beginning and levels out
    INTERPOLATE_FAST = 3,
    //! Rises slowly at the beginning then shoots up
    INTERPOLATE_SLOW = 4,
    //! Simple linear interpolation between A and B
    INTERPOLATE_LINEAR = 5,

    INTERPOLATE_TOTAL = 6
};


/** ****************************************************************************
*** \brief Enables various methods of interpolation
***
*** The basic way to use it is to set the interpolation method using
***        SetMethod(), then call Start() with the values you want to
***        interpolate between and the time to do it in.
*** ***************************************************************************/
class Interpolator
{
public:
    Interpolator();

    /** \brief Sets the interpolation method to use
    *** \param method interpolation method to use
    *** \note This method should only be invoked when there is no interpolation
    *** taking place. If the class is in the middle of an interpolation, the new
    *** method will not be set.
    **/
    void SetMethod(InterpolationMethod method);

    /** \brief Begins a new interpolation
    *** \param a The start value of interpolation
    *** \param b The end value of interpolation
    *** \param milliseconds The amount of time, in milliseconds, to interpolate over
    **/
    void Start(float a, float b, uint32_t milliseconds);

    /** \brief Updates the interpolation timer and value
    *** \param frame_time The amount of milliseconds to update the time value by
    **/
    void Update(uint32_t frame_time);

    //! \brief Returns true if the interpolation is complete
    bool IsFinished() const {
        return _finished;
    }

    //! \brief Returns the current value for the interpolation in progress
    float GetValue() const {
        return _current_value;
    }

private:
    //! \brief The interpolation method to be used
    InterpolationMethod _method;

    //! \brief The two numbers to interpolate between
    float _a, _b;

    //! \brief The current time in the interpolation
    uint32_t _current_time;

    //! \brief The end of the interpolation
    uint32_t _end_time;

    //! \brief The current interpolated value
    float _current_value;

    //! \brief Set to true if the interpolation is finished
    bool _finished;

    //! \brief Returns true if a valid interpolation method is set
    bool _ValidMethod() {
        return (_method > INTERPOLATE_INVALID && _method < INTERPOLATE_TOTAL);
    }
}; // class Interpolator

}  // namespace vt_video

#endif // __INTERPOLATOR_HEADER__
