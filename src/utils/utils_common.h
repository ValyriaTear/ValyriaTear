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
*** \file    utils_common.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the utility code.
***
*** This code includes the common types used all around the code.
***
*** \note Use the following macros for OS-dependent code.
***   - Windows    #ifdef _WIN32
***   - Mac OS X   #ifdef __APPLE__
***   - OpenDarwin #ifdef __APPLE__
***   - Linux      #ifdef __linux__
***   - FreeBSD    #ifdef __FreeBSD__
***   - OpenBSD    #ifdef __OpenBSD__
***   - Solaris    #ifdef SOLARIS
***   - BeOS       #ifdef __BEOS__
***
*** \note Use the following macros for compiler-dependent code.
***   - MSVC       #ifdef _MSC_VER
***   - g++        #ifdef __GNUC__
***
*** \note Use the following statements to determine system endianess.
***   - Big endian      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
***   - Little endian   if (SDL_BYTEORDER == SDL_LITTLE_ENDIAN)
***
*** \note Use the following integer types throughout the entire code.
***   - int32
***   - uint32
***   - int16
***   - uint16
***   - int8
***   - uint8
***
*** \note Use the following string types througout the entire code.
***   - ustring   Unicode strings, meant only for text to be rendered on the screen.
***   - string    Standard C++ strings, used for all text that is not to be rendered to the screen.
***   - char*     Acceptable, but use strings instead wherever possible.
*** ***************************************************************************/

#ifndef __UTILS_COMMON_HEADER__
#define __UTILS_COMMON_HEADER__

// We include SDL_config.h, which compensates for non ISO C99 compilers.
// SDL_config.h defines the int??_t types for non ISO C99 compilers,
// and defines HAVE_STDINT_H for compliant compilers
#include <SDL/SDL_config.h>
#ifdef HAVE_STDINT_H
#include <stdint.h> // Using the C header, because the C++ header, <cstdint> is only available in ISO C++0x
#endif

// defines NULL among other things
#include <cstdlib>

/** \name Print Message Helper Macros
*** These macros assist programmers with writing debug, warning, or error messages that are to be printed to
*** a user's terminal. They are formatted as follows: `MSGTYPE:FILE:FUNCTION:LINE: `. To use the macro, all
*** that is needed is to add `<< "print message" << std::endl;` after the macro name.
**/
//@{
#define PRINT_DEBUG std::cout << "DEBUG:" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": " << std::endl
#define PRINT_WARNING std::cout << "WARNING:" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": " << std::endl
#define PRINT_ERROR std::cout << "ERROR:" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": " << std::endl
//@}

/** \name Print Message Helper Macros With Conditional
*** \param var Any type of variable that can be used to evaluate a true/false condition
*** These macros perform the exact same function as the previous set of print message macros, but these include a conditional
*** parameter. If the parameter is true the message will be printed and if it is false, no message will be printed. Note that
*** the if statement is not enclosed in brackets, so the programmer is not required to add a terminating bracket after they
*** append their print message.
*** \note There is no error conditional macro because detected errors should always be printed when they are discovered
**/
//@{
#define IF_PRINT_DEBUG(var) if (var) std::cout << "DEBUG:" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": " << std::endl
#define IF_PRINT_WARNING(var) if (var) std::cout << "WARNING:" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": " << std::endl
//@}

//! \brief Different App full, shortnames, and directories
#define APPFULLNAME "Valyria Tear"
#define APPSHORTNAME "valyriatear"
#define APPUPCASEDIRNAME "ValyriaTear"

/** \name Integer Types
*** \brief These are the integer types used throughout the source code.
*** These types are created by redefining the ANSI C types.
*** Use of the standard int, long, etc. is forbidden in the source code! Don't attempt to use any
*** 64-bit types either, since a large population of PCs in our target audience are not a 64-bit
*** architecture.
**/
//@{
typedef int32_t   int32;
typedef uint32_t  uint32;
typedef int16_t   int16;
typedef uint16_t  uint16;
typedef int8_t    int8;
typedef uint8_t   uint8;
//@}

//! Contains utility code used across the entire source code
namespace vt_utils
{

//! Determines whether the code in the vt_utils namespace should print debug statements or not.
extern bool UTILS_DEBUG;

} // vt_utils

#endif // __UTILS_COMMON_HEADER__
