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
*** \file    exception.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the utility code.
***
*** Includes the exception handling class
*** ***************************************************************************/

#ifndef __EXCEPTION_HEADER__
#define __EXCEPTION_HEADER__

/** \brief Forces the application to abort with an error
*** \param message An error message string to report
*** This macro throws an Exception which if unhandled, will be caught at the end of the main game loop.
*** Therefore the application will not terminate immediately, but instead will wait until the main game
*** loop reaches the end of the current iteration.
**/
#define ERROR_AND_ABORT(message) throw Exception(message, __FILE__, __LINE__, __FUNCTION__)

//! Contains utility code used across the entire source code
namespace vt_utils
{

//! Determines whether the code in the vt_utils namespace should print debug statements or not.
extern bool UTILS_DEBUG;

/** ****************************************************************************
*** \brief This is the class used for all the C++ exceptions used in Valyria Tear
**/
class Exception
{
public:
    /** \brief The constructor
    ***
    *** The constructor of the Exception class. Use as follows:
    *** throw Exception("error message here", __FILE__, __LINE__, __FUNCTION__);
    ***
    *** \param message A message string that describes the cause of this exception
    *** \param file The file the exception was thrown
    *** \param line Line of the file the exception was thrown
    *** \param function Function the exception was thrown
    */
    Exception(const std::string &message, const std::string &file = std::string(), const int line = -1,
              const std::string &function = std::string()) throw();

    //! \brief The destructor
    virtual ~Exception() throw();

    //! \brief Converts the exception data to a single string object
    virtual std::string ToString() const throw();

    //! \brief Returns the message set by the user
    virtual std::string GetMessage() const throw();

    //! \brief Returns the file the exception was thrown
    virtual std::string GetFile() const throw();

    //! \brief Returns the line the exception was thrown
    virtual int GetLine() const throw();

    //! \brief Returns the function the exception was thrown
    virtual std::string GetFunction() const throw();

private:
    const std::string _message;
    const std::string _file;
    const int _line;
    const std::string _function;
};

} // namespace vt_utils

#endif // __EXCEPTION_HEADER__
