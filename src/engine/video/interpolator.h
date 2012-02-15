///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software 
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    interpolator.h
*** \author  Raj Sharma, roos@allacrost.org
*** \brief   Header file for Interpolator class
***
*** The Interpolator class can interpolate between a single and final value
*** using various methods (linear, fast, slow, etc.)
*** ***************************************************************************/

#ifndef __INTERPOLATOR_HEADER__
#define __INTERPOLATOR_HEADER__

#include "defs.h"
#include "utils.h"

namespace hoa_video {

//! \brief The various ways to create smoothed values between two numbers (e.g. linear interpolation)
enum InterpolationMethod {
	VIDEO_INTERPOLATE_INVALID = -1,
	
	//! Rise from A to B and then down to A again
	VIDEO_INTERPOLATE_EASE = 0,
	//! Keeps a constant value of A
	VIDEO_INTERPOLATE_SRCA = 1,
	//! Keeps a constant value of B
	VIDEO_INTERPOLATE_SRCB = 2,
	//! Rises quickly at the beginning and levels out
	VIDEO_INTERPOLATE_FAST = 3,
	//! Rises slowly at the beginning then shoots up
	VIDEO_INTERPOLATE_SLOW = 4,
	//! Simple linear interpolation between A and B
	VIDEO_INTERPOLATE_LINEAR = 5,
	
	VIDEO_INTERPOLATE_TOTAL = 6
};


/** ****************************************************************************
*** \brief Enables various methods of interpolation
***
*** The basic way to use it is to set the interpolation method using
***        SetMethod(), then call Start() with the values you want to
***        interpolate between and the time to do it in.
*** ***************************************************************************/
class Interpolator {
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
	void Start(float a, float b, uint32 milliseconds);

	/** \brief Updates the interpolation timer and value
	*** \param frame_time The amount of milliseconds to update the time value by
	**/
	void Update(uint32 frame_time);
	
	//! \brief Returns true if the interpolation is complete
	bool IsFinished() const
		{ return _finished; }

	//! \brief Returns the current value for the interpolation in progress
	float GetValue() const
		{ return _current_value; }

private:
	//! \brief The interpolation method to be used
	InterpolationMethod _method;
	
	//! \brief The two numbers to interpolate between
	float _a, _b;
	
	//! \brief The current time in the interpolation
	uint32 _current_time;
	
	//! \brief The end of the interpolation
	uint32 _end_time;
	
	//! \brief The current interpolated value
	float _current_value;

	//! \brief Set to true if the interpolation is finished
	bool _finished;

	//! \brief Returns true if a valid interpolation method is set
	bool _ValidMethod()
		{ return (_method > VIDEO_INTERPOLATE_INVALID && _method < VIDEO_INTERPOLATE_TOTAL); }

	/** \brief Interpolates logarithmically. Increases quickly and then levels off.
	*** \param initial_value The initial value to interpolate. Should be between 0.0f and 1.0f (inclusive)
	*** \return The interpolated value
	**/
	float _FastTransform(float initial_value);
	
	/** \brief Interpolates exponentially. Increases slowly and then sky rockets.
	*** \param initial_value The initial value to interpolate. Should be between 0.0f and 1.0f (inclusive)
	*** \return The interpolated value
	**/
	float _SlowTransform(float initial_value);
	
	/** \brief Interpolates periodically. Increases slowly to 1.0f then back down to 0.0f via a sine function.
	*** \param initial_value The initial value to interpolate. Should be between 0.0f and 1.0f (inclusive)
	*** \return The interpolated value
	**/
	float _EaseTransform(float initial_value);
}; // class Interpolator

}  // namespace hoa_video

#endif // __INTERPOLATOR_HEADER__
