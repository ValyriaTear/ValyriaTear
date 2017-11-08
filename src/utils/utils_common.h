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
*** \file    utils_common.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Common Header file for the project.
***
*** This code includes the headers and common types used all around the code.
***
**/

#ifndef __UTILS_COMMON_HEADER__
#define __UTILS_COMMON_HEADER__

#include <iostream>

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

#endif // __UTILS_COMMON_HEADER__
