///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************(
*** \file    utils_numeric.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the numeric utility code.
*** ***************************************************************************/

#ifndef __UTILS_NUMERIC_HEADER__
#define __UTILS_NUMERIC_HEADER__

#include "utils/utils_pch.h"

//! Contains utility code used across the entire source code
namespace vt_utils
{

/** \name Multiples of Pi constants
*** \brief PI and multiples of PI. Used in various math calculations such as interpolations.
***
*** These constants are mostly used in the video engine.
**/
//@{
const float UTILS_QUARTER_PI = 0.785398163f;
const float UTILS_HALF_PI    = 1.570796326f;
const float UTILS_PI         = 3.141592653f;
const float UTILS_2PI        = 6.283185307f;
//@}

/** \brief Linearly interpolates a value which is (alpha * 100) percent between initial and final
*** \param alpha Determines where inbetween initial (0.0f) and final (1.0f) the interpolation should be
*** \param initial The initial value
*** \param final_value The final value
*** \return the linear interpolated value
**/
float Lerp(float alpha, float initial, float final_value);

/** \brief Rounds an unsigned integer up to the nearest power of two.
*** \param x The number to round up.
*** \return The nearest power of two rounded up from the argument.
**/
uint32_t RoundUpPow2(uint32_t x);

/** \brief Determines if an unsigned integer is a power of two or not.
*** \param x The number to examine.
*** \return True if the number if a power of two, false otherwise.
**/
bool IsPowerOfTwo(uint32_t x);

/** \brief Determines if an integer is an odd number or not.
*** \param x The unsigned integer to examine.
*** \return True if the number is odd, false if it is not.
*** \note Using a signed integer with this function will yield the same result.
**/
bool IsOddNumber(uint32_t x);

/** \brief Determines if a floating point number is within a range of two numbers.
*** \param value The floating point value to compare.
*** \param lower The minimum bound (inclusive).
*** \param upper The maximum bound (inclusive).
*** \return True if the value lies within the two bounds.
*** This function should be used in place of direct comparison of two floating point
*** values. The reason for this is that there are small variations in floating point representation
*** across systems and different rounding schemes, so its best to examine numbers within a reasonably
*** sized range. For example, if you want to detect if a number is 1.0f, try 0.999f and 1.001f for
*** the bound arguments.
**/
bool IsFloatInRange(float value, float lower, float upper);


/** \brief Alternative function for determining if a floating point number is close to a specific value
*** \param value The floating point value to compare.
*** \param base The number that we want to compare the value to
*** \param delta The maximum degree of tolerance allowed (default value: 0.001f)
*** \return True if the value lies within the two bounds.
*** This is an alternative method for testing equality between two floating point values. The delta
*** member should always be positive.
**/
bool IsFloatEqual(float value, float base, float delta = 0.001f);


/** \brief Returns the fractional portion of a floating point value
*** \param value The floating point value to retrieve the fractional portion (format x.y)
*** \return A float containing only the fractional value (format 0.y)
**/
float GetFloatFraction(float value);


/** \brief Returns the integer portion of a floating point value
*** \param value The floating point value to retrieve the integer portion (format x.y)
*** \return A float containing only the integer value (format x.0)
**/
float GetFloatInteger(float value);


//! \brief Rounds down a float to a multiple of another float
/*!
 *  \return Rounded float
 *  \param value Number to be rounded
 *  \param multiple Multiple to be considered when rounding
 */
float FloorToFloatMultiple(const float value, const float multiple);

} // namespace vt_utils

#endif // __UTILS_NUMERIC_HEADER__
