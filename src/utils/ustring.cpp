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
*** \file    ustring.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for the UTF16 string code.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "ustring.h"

namespace vt_utils
{

////////////////////////////////////////////////////////////////////////////////
///// ustring Class
////////////////////////////////////////////////////////////////////////////////

const size_t ustring::npos = ~0;

ustring::ustring()
{
    _str.push_back(0);
}

ustring::ustring(const uint16 *s)
{
    _str.clear();

    if(!s) {
        _str.push_back(0);
        return;
    }

    while(*s != 0) {
        _str.push_back(*s);
        ++s;
    }

    _str.push_back(0);
}

// Return a substring starting at pos, continuing for n elements
ustring ustring::substr(size_t pos, size_t n) const
{
    size_t len = length();

    if(pos >= len)
        throw std::out_of_range("pos passed to substr() was too large");

    ustring s;
    while(n > 0 && pos < len) {
        s += _str[pos];
        ++pos;
        --n;
    }

    return s;
}

// Concatenates string to another
ustring ustring::operator + (const ustring &s) const
{
    ustring temp = *this;

    // nothing to do for empty string
    if(s.empty())
        return temp;

    // add first character of string into the null character spot
    temp._str[length()] = s[0];

    // add rest of characters afterward
    size_t len = s.length();
    for(size_t j = 1; j < len; ++j) {
        temp._str.push_back(s[j]);
    }

    // Finish off with a null character
    temp._str.push_back(0);

    return temp;
}

// Adds a character to end of this string
ustring &ustring::operator += (uint16 c)
{
    _str[length()] = c;
    _str.push_back(0);

    return *this;
}

// Concatenate another string on to the end of this string
ustring &ustring::operator += (const ustring &s)
{
    // nothing to do for empty string
    if(s.empty())
        return *this;

    // add first character of string into the null character spot
    _str[length()] = s[0];

    // add rest of characters afterward
    size_t len = s.length();
    for(size_t j = 1; j < len; ++j) {
        _str.push_back(s[j]);
    }

    // Finish off with a null character
    _str.push_back(0);

    return *this;
}

// Will assign the current string to this string
ustring &ustring::operator = (const ustring &s)
{
    clear();
    operator += (s);

    return *this;
} // ustring & ustring::operator = (const ustring &s)

// Compare two substrings
bool ustring::operator == (const ustring &s) const
{
    size_t len = length();
    if (s.length() != len)
        return false;

    for(size_t j = 0; j < len; ++j) {
        if (_str[j] != s[j] )
            return false;
    }

    return true;
} // bool ustring::operator == (const ustring &s)

// Finds a character within a string, starting at pos. If nothing is found, npos is returned
size_t ustring::find(uint16 c, size_t pos) const
{
    size_t len = length();

    for(size_t j = pos; j < len; ++j) {
        if(_str[j] == c)
            return j;
    }

    return npos;
} // size_t ustring::find(uint16 c, size_t pos) const

// Finds a string within a string, starting at pos. If nothing is found, npos is returned
size_t ustring::find(const ustring &s, size_t pos) const
{
    size_t len = length();
    size_t total_chars = s.length();
    size_t chars_found = 0;

    for(size_t j = pos; j < len; ++j) {
        if(_str[j] == s[chars_found]) {
            ++chars_found;
            if(chars_found == total_chars) {
                return (j - chars_found + 1);
            }
        } else {
            chars_found = 0;
        }
    }

    return npos;
} // size_t ustring::find(const ustring &s, size_t pos) const

////////////////////////////////////////////////////////////////////////////////
///// ustring manipulator functions
////////////////////////////////////////////////////////////////////////////////

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define UTF_16_ICONV_NAME "UTF-16LE"
#else
#define UTF_16_ICONV_NAME "UTF-16BE"
#endif

#define UTF_16_BOM_STD 0xFEFF
#define UTF_16_BOM_REV 0xFFFE

static bool UTF8ToUTF16(const char *source, uint16 *dest, size_t length)
{
    if(!length)
        return true;

    iconv_t convertor = iconv_open(UTF_16_ICONV_NAME, "UTF-8");
    if(convertor == (iconv_t) - 1) {
        return false;
    }

#if (defined(_LIBICONV_VERSION) && _LIBICONV_VERSION == 0x0109) || defined(__FreeBSD__)
    // We are using an iconv API that uses const char*
    const char *sourceChar = source;
#else
    // The iconv API doesn't specify a const source for legacy support reasons.
    // Versions after 0x0109 changed back to char* for POSIX reasons.
    char *sourceChar = const_cast<char *>(source);
#endif
    char *destChar = reinterpret_cast<char *>(dest);
    size_t sourceLen = length;
    size_t destLen = (length + 1) * 2;
    size_t ret = iconv(convertor, &sourceChar, &sourceLen,
                       &destChar, &destLen);
    iconv_close(convertor);
    if(ret == (size_t) - 1) {
        perror("iconv");
        return false;
    }
    return true;
}

// Creates a ustring from a normal string
ustring MakeUnicodeString(const std::string &text)
{
    int32 length = static_cast<int32>(text.length() + 1);
    std::vector<uint16> ubuff(length+1,0);
    uint16 *utf16String = &ubuff[0];

    if(UTF8ToUTF16(text.c_str(), &ubuff[0], length)) {
        // Skip the "Byte Order Mark" from the UTF16 specification
        if(utf16String[0] == UTF_16_BOM_STD ||  utf16String[0] == UTF_16_BOM_REV) {
            utf16String = &ubuff[0] + 1;
        }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        // For some reason, using UTF-16BE to iconv on big-endian machines
        // still does not create correctly accented characters, so this
        // byte swapping must be performed (only for irregular characters,
        // hence the mask).

        for(int32 c = 0; c < length; c++)
            if(utf16String[c] & 0xFF80)
                utf16String[c] = (utf16String[c] << 8) | (utf16String[c] >> 8);
#endif
    } else {
        for(int32 c = 0; c < length; ++c) {
            ubuff[c] = static_cast<uint16>(text[c]);
        }
    }

    ustring new_ustr(utf16String);
    return new_ustr;
} // ustring MakeUnicodeString(const string& text)


// Creates a normal string from a ustring
std::string MakeStandardString(const ustring &text)
{
    const int32 length = static_cast<int32>(text.length());
    std::vector<unsigned char> strbuff(length+1,'\0');

    for(int32 c = 0; c < length; ++c) {
        uint16 curr_char = text[c];

        if(curr_char > 0xff)
            strbuff[c] = '?';
        else
            strbuff[c] = static_cast<unsigned char>(curr_char);
    }

    return std::string(reinterpret_cast<char *>(&strbuff[0]));
} // string MakeStandardString(const ustring& text)

} // namespace utils
