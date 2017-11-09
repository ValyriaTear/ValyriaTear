///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    main_options.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header functions for handling command-line arguments
*** \note    Only main.cpp and main_options.cpp should need to include this file.
*** **************************************************************************/

#ifndef __MAIN_OPTIONS_HEADER__
#define __MAIN_OPTIONS_HEADER__

#include <cstdint>
#include <string>
#include <vector>

/** \brief Namespace containing functions central to main program execution.
*** \note Normally no other code should need to use this namespace.
**/
namespace vt_main {

/** \brief Parses command-line options and takes appropriate action on those options
*** \param return_code A reference to the return code to exit the program with.
*** \param argc The number of arguments given to the program
*** \param argv A pointer to the list of arguments
*** \return False if the program should exit with a return code, or true if it should continue
**/
bool ParseProgramOptions(int32_t& return_code, int32_t argc, char* argv[]);

/** \brief Parses a secondary list of options provided to a primary program option
*** \param vars The list of secondary options in single string format
*** \param options A reference to a container of strings that should hold each seconary option
*** \return False if there was an error while parsing the option list
**/
bool ParseSecondaryOptions(const std::string& vars, std::vector<std::string>& options);

//! \brief Prints out the program usage for running the program.
void PrintUsage();

/** \brief Prints information about the user's system.
*** \return False if an error occured while retrieving system information.
**/
bool PrintSystemInformation();

/** \brief Resets the game settings (audio volume, key mappings, etc.) to their default values.
*** \return False if the settings could not be restored, or if another problem occured.
**/
bool ResetSettings();

/** \brief Enables debugging print statements in various parts of the game engine.
*** \param vars The name(s) of the debugging variable(s) to enable.
*** \return False if a bad function argument was given, or true on success.
**/
bool EnableDebugging(const std::string& vars);

} // namespace vt_main

#endif // __MAIN_OPTIONS_HEADER__
