///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    exception.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for the exception code.
*** ***************************************************************************/

#include "exception.h"

#include <sstream>

template <typename T>
std::string _NumberToString(const T t)
{
    std::ostringstream text("");
    text << static_cast<int32_t>(t);
    return text.str();
}

namespace vt_utils
{

Exception::Exception(const std::string &message, const std::string &file, const int line, const std::string &function) throw() :
    _message(message),
    _file(file),
    _line(line),
    _function(function)
{}

Exception::~Exception() throw()
{}

std::string Exception::ToString() const throw()
{
    return std::string("EXCEPTION:" + _file + ":" + _function + ":" + _NumberToString(_line) + ": " + _message);
}

std::string Exception::GetExceptionMessage() const throw()
{
    return _message;
}

std::string Exception::GetFile() const throw()
{
    return _file;
}

int Exception::GetLine() const throw()
{
    return _line;
}

std::string Exception::GetFunction() const throw()
{
    return _function;
}

} // namespace utils
