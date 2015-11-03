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
*** \file    utils_random.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the random number utility code.
*** ***************************************************************************/

#ifndef __UTILS_RANDOM_HEADER__
#define __UTILS_RANDOM_HEADER__

//! Contains utility code used across the entire source code
namespace vt_utils
{

//! \name Random Variable Genreator Fucntions
//@{
/** \brief Creates a uniformly distributed random floating point number
*** \return A floating-point value between [0.0f, 1.0f]
**/
float RandomFloat();

/** \brief Creates a random float value between a and b.
*** \param a The lower bound value
*** \param b The upper bound value
*** \return A random float with a value between a and b
**/
float RandomFloat(float a, float b);

/** \brief Returns a random interger value uniformly distributed between two inclusive bounds
*** \param lower_bound The lower inclusive bound
*** \param upper_bound The upper inclusive bound
*** \return An integer between [lower_bound, upper_bound]
*** \note If the user specifies a lower bound that is greater than the upper bound, the two bounds
*** are switched.
**/
int32_t RandomBoundedInteger(int32_t lower_bound, int32_t upper_bound);

/** \brief Returns a random value with a given deviation.
*** \param base_value The base value.
*** \param max_diff The maximum difference with the base value.
*** \return A value between (base - diff) and (base + diff)
**/
int32_t RandomDiffValue(int32_t base_value, uint32_t max_diff);
//@}

} // namespace vt_utils

#endif // __UTILS_RANDOM_HEADER__
