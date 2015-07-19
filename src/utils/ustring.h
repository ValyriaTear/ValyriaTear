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
*** \file    ustring.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for UTF16 support
***
*** The class implementing UTF16 text support
***
*** ***************************************************************************/

#ifndef __USTRING_HEADER__
#define __USTRING_HEADER__

#include "utils/utils_pch.h"

//! Contains utility code used across the entire source code
namespace vt_utils
{

/** ****************************************************************************
*** \brief Implements unicode strings with uint16 as the character type
***
*** This class functions identically to the std::string class provided in the C++
*** standard library. The critical difference is that each character is 2 bytes
*** (16 bits) wide instead of 1 byte (8 bits) wide so that it may implement the
*** full unicode character set.
***
*** \note This class intentionally ignores the code standard convention for class
*** names because the class objects are to be used as if they were a standard C++ type.
***
*** \note This class does not implement a destructor because the only data member
*** (a std::vector) will automatically destroy itself when the no-arg destructor is invoked.
***
*** \note The member functions of this class are not documented because they function
*** in the exact same manner that the C++ string class does.
***
*** \note There are some libstdc++ compatability problems with simply defining
*** basic_string<uint16>, so this class is a custom version of it.
***
*** \note Currently not all functionality of basic_string has been implemented, but
*** instead only the functions that we typically use in the game. If you need a
*** basic_string function available that isn't already implemented in this class,
*** go ahead and add it yourself.
***
*** \note This class does not use wchar_t because it has poor compatibility.
*** ***************************************************************************/
class ustring
{
public:
    ustring();

    ustring(const uint16 *);

    static const size_t npos;

    void clear() {
        _str.clear();
        _str.push_back(0);
    }

    bool empty() const {
        return _str.size() <= 1;
    }

    size_t length() const
    // We assume that there is always a null terminating character, hence the -1 subtracted from the size
    {
        return _str.size() - 1;
    }

    size_t size() const {
        return length();
    }

    const uint16 *c_str() const {
        return &_str[0];
    }

    size_t find(uint16 c, size_t pos = 0) const;

    size_t find(const ustring &s, size_t pos = 0) const;

    ustring substr(size_t pos = 0, size_t n = npos) const;

    ustring operator + (const ustring &s) const;

    ustring &operator += (uint16 c);

    ustring &operator += (const ustring &s);

    ustring &operator = (const ustring &s);

    bool operator == (const ustring &s) const;

    uint16 &operator [](size_t pos) {
        return _str[pos];
    }

    const uint16 &operator [](size_t pos) const {
        return _str[pos];
    }

private:
    //! \brief The structure containing the unicode string data.
    std::vector<uint16> _str;
}; // class ustring

/** \brief Creates a ustring from a standard string
*** \param text The standard string to create the ustring equivalent for
*** \return A ustring containing the same information as the function parameter
***
*** This function is useful for hard-coding text to be displayed on the screen,
*** as unicode characters are the only characters allowed to be displayed. This
*** function serves primarily for debugging and diagnostic purposes.
**/
vt_utils::ustring MakeUnicodeString(const std::string &text);

/** \brief Creates an starndard string from a ustring
*** \param text The ustring to create the equivalent standard string for
*** \return A string containing the same information as the ustring
***
*** This function is much less likely to be used as MakeUnicodeString.
*** Standard strings are used for resource loading (of images, sounds, etc.) so
*** this may come in use if a ustring contains file information.
**/
std::string MakeStandardString(const vt_utils::ustring &text);
//@}

} // namespace vt_utils

#endif // __USTRING_HEADER__
