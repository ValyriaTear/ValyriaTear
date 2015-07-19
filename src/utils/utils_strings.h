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
*** \file    utils_strings.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the utility code.
***
*** This code includes various utility functions that are used for strings
*** manipulation.
*** ***************************************************************************/

#ifndef __UTILS_STRINGS_HEADER__
#define __UTILS_STRINGS_HEADER__

//! Contains utility code used across the entire source code
namespace vt_utils
{

//! A static empty string, used when needed to return a const reference to
//! that kind of data.
static const std::string _empty_string;

//! Determines whether the code in the vt_utils namespace should print debug statements or not.
extern bool UTILS_DEBUG;

//! \brief Returns the uppercased version of a string
std::string Upcase(std::string text);

//! \brief Returns the string with the first letter uppercased.
std::string UpcaseFirst(std::string text);

/** \brief A safe version of sprintf that returns a std::string of the result.
*** Copyright The Mana Developers (2012) - GPLv2
*/
std::string strprintf(char const *, ...)
#ifdef __GNUC__
    // This attribute is nice: it even works through gettext invokation. For
    // example, gcc will complain that strprintf(_("%s"), 42) is ill-formed.
    __attribute__((__format__(__printf__, 1, 2)))
#endif
;

//! \name String Utility Functions
//@{
/** \brief Converts an integer type into a standard string
*** \param T The integer type to convert to a string
*** \return A std::string containing the parameter in string form
**/
template <typename T>
std::string NumberToString(const T t)
{
    std::ostringstream text("");
    text << static_cast<int32>(t);
    return text.str();
}

//! Specialization for a float type
//! declared as inline to avoid linker errors.
template <>
inline std::string NumberToString<float>(const float t)
{
    std::ostringstream text("");
    text << t;
    return text.str();
}

/** \brief Determines if a string is a valid numeric string
*** \param text The string to check
*** \return A std::string containing the parameter in string form
***
*** This function will accept strings with + or - characters as the first
*** string element and strings including a single decimal point.
*** Examples of valid numeric strings are: "50", ".2350", "-252.5"
**/
bool IsStringNumeric(const std::string &text);
//@}

} // namespace vt_utils

#endif // __UTILS_STRINGS_HEADER__
