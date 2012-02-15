///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    test_main.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for primary test code functions
*** **************************************************************************/

#ifndef __TEST_MAIN_HEADER__
#define __TEST_MAIN_HEADER__

/** \brief Namespace containing code used only for testing purposes
*** \note Normally no other code should need to use this namespace.
**/
namespace hoa_test {

/** \brief Executes a specific piece of code that is intended to test some piece of functionality
*** \param tests The list of tests to execute in a space delimited string format
*** \return False if one or more of the executed tests failed
***
*** This function is used to test specific aspects of the game code. Each test does its own engine initialization
*** and runs a custom game loop, subverting the primary game loop found in main.cpp. All test code is
*** contained in the src/test directory.
**/
bool ExecuteTests(std::string& tests);

} // namespace hoa_test

#endif // __TEST_MAIN_HEADER__
