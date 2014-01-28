///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
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
int32 RandomBoundedInteger(int32 lower_bound, int32 upper_bound);

/** \brief Returns a Gaussian random value with specified mean and standard deviation
*** \param mean
*** \param std_dev The standard deviation of the Gaussian function (optional, default is 10.0f)
*** \param positive_value If true the function will not return a negative result (optional, default is true)
*** \return An Gaussian random integer with a mean and standard deviation as specified by the user
***
*** This function computes a random number based on a Gaussian Distribution Curve. This number will be between
*** mean - range and mean + range if range is greater than zero, otherwise it will return a true, unbounded
*** Gaussian random value. If positive_value is set to true, this function will only return a number that is
*** zero or positive.
***
*** Mean is (obviously) the mean, and the range represents the value for 3 standard deviations from the mean.
*** That means that 99.7% of the random values chosen will lay between mean - range and mean + range, if
*** range is a greater than or equal to zero.
**/
int32 GaussianRandomValue(int32 mean, float std_dev = 10.0f, bool positive_value = true);


/** \brief Returns true/false depending on the chance
*** \param chance Value between 0..100. 0 will always return false and >=100 will always return true.
*** \return True if the chance occurs
**/
bool Probability(uint32 chance);
//@}

} // namespace vt_utils

#endif // __UTILS_RANDOM_HEADER__
