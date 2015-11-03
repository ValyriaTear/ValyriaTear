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
*** \file    utils.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for the utility code.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "utils_strings.h"

// Fix linkage when using vsnprintf by undefining the libintl equivalent function.
#ifndef DISABLE_TRANSLATIONS
#undef vsnprintf
#endif

namespace vt_utils
{

std::string Upcase(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), ::toupper);
    return text;
}

std::string UpcaseFirst(std::string text)
{
    std::transform(text.begin(), ++text.begin(), text.begin(), ::toupper);
    return text;
}

std::string strprintf(char const *format, ...)
{
    char buf[256];
    va_list(args);
    va_start(args, format);
    int nb = vsnprintf(buf, 256, format, args);
    va_end(args);
    if (nb < 256)
    {
        return buf;
    }
    // The static size was not big enough, try again with a dynamic allocation.
    ++nb;
    char *buf2 = new char[nb];
    va_start(args, format);
    vsnprintf(buf2, nb, format, args);
    va_end(args);
    std::string res(buf2);
    delete [] buf2;
    return res;
}

// Returns true if the given text is a number
bool IsStringNumeric(const std::string &text)
{
    if(text.empty())
        return false;

    // Keep track of whether decimal point is allowed. It is allowed to be present in the text zero or one times only.
    bool decimal_allowed = true;

    size_t len = text.length();

    // Check each character of the string one at a time
    for(size_t c = 0; c < len; ++c) {
        // The only non numeric characters allowed are a - or + as the first character, and one decimal point anywhere
        bool numeric_char = (isdigit(static_cast<int32_t>(text[c]))) || (c == 0 && (text[c] == '-' || text[c] == '+'));

        if(!numeric_char) {
            // Check if the 'bad' character is a decimal point first before labeling the string invalid
            if(decimal_allowed && text[c] == '.') {
                decimal_allowed = false; // Decimal points are now invalid for the rest of the string
            } else {
                return false;
            }
        }
    }

    return true;
} // bool IsStringNumeric(const string& text)

} // namespace utils
