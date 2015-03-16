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
*** \file    utils_pch.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Precompiled Header file for the project.
***
*** This code includes the headers and common types used all around the code.
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

#ifndef __UTILS_PCH_HEADER__
#define __UTILS_PCH_HEADER__

//
// Include Common Headers
//

#ifdef _WIN32
#   include <windows.h>
#   include <direct.h>
#   include <shlobj.h>
#   include <stdlib.h>
#   ifndef PATH_MAX
#       define PATH_MAX _MAX_PATH   // redefine _MAX_PATH to be compatible with Darwin's PATH_MAX
#   endif
#else
#   include <dirent.h>
#   include <pwd.h>
#   include <sys/types.h>
#   include <unistd.h>
#endif

#ifdef __APPLE__
#ifndef EDITOR_BUILD
#   include <OpenAL/al.h>
#   include <OpenAL/alc.h>
#endif
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#   include <unistd.h>
#   undef check
#else
#ifndef EDITOR_BUILD
#   include "al.h"
#   include "alc.h"
#endif
#   include <GL/glew.h>
#   include <GL/gl.h>
#   include <GL/glu.h>
#endif

#ifdef __linux__
#   include <limits.h>
#endif

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <fstream>
#include <iconv.h>
#include <iostream>

#ifndef DISABLE_TRANSLATIONS
#ifndef EDITOR_BUILD
#   include <libintl.h>
#endif
#endif

#include <list>
#include <map>
#include <math.h>
#include <set>
#include <sstream>
#include <stack>
#include <stdarg.h>

#ifdef HAVE_STDINT_H
#   include <stdint.h> // Using the C header, because the C++ header, <cstdint> is only available in ISO C++0x
#endif

#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <vector>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <luabind/luabind.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/object.hpp>

#if LUA_VERSION_NUM < 502
# define lua_pushglobaltable(L) lua_pushvalue(L, LUA_GLOBALSINDEX)
#endif

#ifndef EDITOR_BUILD
#include <png.h>

// We include SDL_config.h, which compensates for non ISO C99 compilers.
// SDL_config.h defines the int??_t types for non ISO C99 compilers,
// and defines HAVE_STDINT_H for compliant compilers
#include <SDL2/SDL.h>
#include <SDL2/SDL_config.h>
#include <SDL2/SDL_endian.h>
#include <SDL_image.h>

#define NO_THREADS 0
#define SDL_THREADS 1

/* Set this to NO_THREADS to disable threads. Set this to SDL_THREADS to use
 * SDL Threads. */
#define THREAD_TYPE SDL_THREADS

#if (THREAD_TYPE == SDL_THREADS)
#   include <SDL2/SDL_thread.h>
#   include <SDL2/SDL_mutex.h>
    typedef SDL_Thread Thread;
    typedef SDL_sem Semaphore;
#else
    typedef int Thread;
    typedef int Semaphore;
#endif

#ifdef __APPLE__
#   include <SDL_ttf.h>
#else
#   include <SDL2/SDL_ttf.h>
#endif

#include <vorbis/vorbisfile.h>
#endif // ifndef EDITOR_BUILD

// The Windows API defines GetMessage and CreateSemaphore.
// Undefine it here to prevent conflicts within the code base.
// Case-insensitive string compare is called stricmp in Windows and strcasecmp everywhere else.
#ifdef _WIN32
#   undef GetMessage
#   undef CreateSemaphore
#   ifndef strcasecmp
#       define strcasecmp stricmp
#   endif
#endif

//
// Common Defines and Typedefs
//

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

#endif // __UTILS_PCH_HEADER__
