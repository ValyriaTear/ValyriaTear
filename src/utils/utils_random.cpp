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
*** \file    utils_random.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for the random number utility code.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "utils_random.h"

namespace vt_utils
{

float RandomFloat()
{
    return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}

float RandomFloat(float a, float b)
{
    if(a > b) {
        float c = a;
        a = b;
        b = c;
    }

    float r = static_cast<float>(rand() % 10001);
    return a + (b - a) * r / 10000.0f;
}

// Returns a random integer between two inclusive bounds
int32 RandomBoundedInteger(int32 lower_bound, int32 upper_bound)
{
    int32 range;  // The number of possible values we may return
    float result;

    range = upper_bound - lower_bound + 1;
    if(range < 0) {  // Oops, someone accidentally switched the lower/upper bound arguments
        IF_PRINT_WARNING(UTILS_DEBUG) << "UTILS WARNING: Call to RandomNumber had bound arguments swapped." << std::endl;
        range = range * -1;
    }

    result = range * RandomFloat();
    result = result + lower_bound; // Shift result so that it is within the correct bounds

    return static_cast<int32>(result);
} // int32 RandomBoundedInteger(int32 lower_bound, int32 upper_bound)

// Creates a Gaussian random interger value.
// std_dev and positive_value are optional arguments with default values 10.0f and true respectively
int32 GaussianRandomValue(int32 mean, float std_dev, bool positive_value)
{
    float x, y, r;  // x and y are coordinates on the unit circle
    float grv_unit; // Used to hold a Gaussian random variable on a normal distribution curve (mean 0, stand dev 1)
    float result;

    // Make sure that the standard deviation is positive
    if(std_dev < 0) {
        PRINT_WARNING << "UTILS WARNING: negative value (" << std_dev << ") for standard deviation argument in function GaussianValue," << std::endl
        << "Using the absolute value." << std::endl;
        std_dev = -1.0f * std_dev;
    }

    // Computes a standard Gaussian random number using the the polar form of the Box-Muller transformation.
    // The algorithm computes a random point (x, y) inside the unit circle centered at (0, 0) with radius 1.
    // Then a Gaussian random variable with mean 0 and standard deviation 1 is computed by:
    //
    // x * sqrt(-2.0 * log(r) / r)
    //
    // Reference: Knuth, The Art of Computer Programming, Volume 2, p. 122

    // This loop is executed 4 / pi = 1.273 times on average
    do {
        x = 2.0f * RandomFloat() - 1.0f;     // Get a random x-coordinate [-1.0f, 1.0f]
        y = 2.0f * RandomFloat() - 1.0f;     // Get a random y-coordinate [-1.0f, 1.0f]
        r = x * x + y * y;
    } while(r > 1.0f || r == 0.0f);
    grv_unit = x * sqrt(-2.0f * log(r) / r);

    // Use the standard gaussian value to create a random number with the desired mean and standard deviation.
    result = (grv_unit * std_dev) + mean;

    // Return zero if a negative result was found and only positive values were to be returned
    if(result < 0.0f && positive_value)
        return 0;
    else
        return static_cast<int32>(result);
} // int32 GaussianValue(int32 mean, float std_dev, bool positive_value)

// Returns true/false depending on the chance
bool Probability(uint32 chance)
{
    uint32 value = static_cast<uint32>(RandomBoundedInteger(1, 100));
    if(value <= chance)
        return true;
    else
        return false;
}

} // namespace utils
