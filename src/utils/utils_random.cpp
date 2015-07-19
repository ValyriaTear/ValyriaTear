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

int32 RandomBoundedInteger(int32 lower_bound, int32 upper_bound)
{
    int32 range;  // The number of possible values we may return
    float result;

    range = upper_bound - lower_bound + 1;
    if(range < 0)
        range = range * -1;

    result = range * RandomFloat();
    result = result + lower_bound;

    return static_cast<int32>(result);
}

int32 RandomDiffValue(int32 base_value, uint32 max_diff)
{
    if (max_diff == 0)
        return base_value;

    int32 lower_bound = base_value - static_cast<int32>(max_diff);
    int32 upper_bound = base_value + static_cast<int32>(max_diff);
    return static_cast<int32>(RandomFloat(static_cast<float>(lower_bound), static_cast<float>(upper_bound)));
}

} // namespace utils
