///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************(
*** \file    utils.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for Allacrost utility code.
***
*** This code includes various utility functions that are used across different
*** parts of the code base. This file is included in every header file in the
*** Allacrost source tree.
***
*** \note Use the following macros for OS-dependent code.
***   - Windows    #ifdef _WIN32
***   - Mac OS X   #ifdef __MACH__
***   - OpenDarwin #ifdef __MACH__
***   - Linux      #ifdef __linux__
***   - FreeBSD    #ifdef __FreeBSD__
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
*** \note Use the following integer types throughout the entire Allacrost code.
***   - int32
***   - uint32
***   - int16
***   - uint16
***   - int8
***   - uint8
***
*** \note Use the following string types througout the entire Allacrost code.
***   - ustring   Unicode strings, meant only for text to be rendered on the screen.
***   - string    Standard C++ strings, used for all text that is not to be rendered to the screen.
***   - char*     Acceptable, but use strings instead wherever possible.
*** ***************************************************************************/

#ifndef __UTILS_HEADER__
#define __UTILS_HEADER__

#include <cstdlib>
#include <cmath>
#include <cstring> // For C string manipulation functions like strcmp

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <stdexcept>
#include <sstream>

// We include SDL_config.h, which compensates for non ISO C99 compilers.
// SDL_config.h defines the int??_t types for non ISO C99 compilers,
// and defines HAVE_STDINT_H for compliant compilers
#include <SDL/SDL_config.h>
#ifdef HAVE_STDINT_H
	#include <stdint.h> // Using the C header, because the C++ header, <cstdint> is only available in ISO C++0x
#endif

#ifdef __MACH__
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#else
	#include "al.h"
	#include "alc.h"
#endif

#ifdef _WIN32
	// Even though Allacrost is platform independent, OpenGL on Windows requires windows.h to be included
	#include <windows.h>
	// Case-insensitive string compare is called stricmp in Windows and strcasecmp everywhere else
	#ifndef strcasecmp
	#define strcasecmp stricmp
	#endif
#endif


/** \brief Forces the application to abort with an error
*** \param message An error message string to report
*** This macro throws an Exception which if unhandled, will be caught at the end of the main game loop.
*** Therefore the application will not terminate immediately, but instead will wait until the main game
*** loop reaches the end of the current iteration.
**/
#define ERROR_AND_ABORT(message) throw Exception(message, __FILE__, __LINE__, __FUNCTION__)

/** \name Print Message Helper Macros
*** These macros assist programmers with writing debug, warning, or error messages that are to be printed to
*** a user's terminal. They are formatted as follows: `MSGTYPE:FILE:FUNCTION:LINE: `. To use the macro, all
*** that is needed is to add `<< "print message" << std::endl;` after the macro name.
**/
//@{
#define PRINT_DEBUG std::cout << "DEBUG:" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": "
#define PRINT_WARNING std::cerr << "WARNING:" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": "
#define PRINT_ERROR std::cerr << "ERROR:" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": "
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
#define IF_PRINT_DEBUG(var) if (var) std::cout << "DEBUG:" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": "
#define IF_PRINT_WARNING(var) if (var) std::cerr << "WARNING:" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": "
//@}

/** \name Allacrost Integer Types
*** \brief These are the integer types used throughout the Allacrost source code.
*** These types are created by redefining the ANSI C types.
*** Use of the standard int, long, etc. is forbidden in Allacrost source code! Don't attempt to use any
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
namespace hoa_utils {

//! Determines whether the code in the hoa_utils namespace should print debug statements or not.
extern bool UTILS_DEBUG;

/** \name Multiples of Pi constants
*** \brief PI and multiples of PI. Used in various math calculations such as interpolations.
***
*** These constants are mostly used in the video engine.
**/
//@{
const float UTILS_QUARTER_PI = 0.785398163f;
const float UTILS_HALF_PI    = 1.570796326f;
const float UTILS_PI         = 3.141592653f;
const float UTILS_2PI        = 6.283185307f;
//@}

/** \brief Rounds an unsigned integer up to the nearest power of two.
*** \param x The number to round up.
*** \return The nearest power of two rounded up from the argument.
**/
uint32 RoundUpPow2(uint32 x);

/** \brief Determines if an unsigned integer is a power of two or not.
*** \param x The number to examine.
*** \return True if the number if a power of two, false otherwise.
**/
bool IsPowerOfTwo(uint32 x);

/** \brief Determines if an integer is an odd number or not.
*** \param x The unsigned integer to examine.
*** \return True if the number is odd, false if it is not.
*** \note Using a signed integer with this function will yield the same result.
**/
bool IsOddNumber(uint32 x);

/** \brief Determines if a floating point number is within a range of two numbers.
*** \param value The floating point value to compare.
*** \param lower The minimum bound (inclusive).
*** \param upper The maximum bound (inclusive).
*** \return True if the value lies within the two bounds.
*** This function should be used in place of direct comparison of two floating point
*** values. The reason for this is that there are small variations in floating point representation
*** across systems and different rounding schemes, so its best to examine numbers within a reasonably
*** sized range. For example, if you want to detect if a number is 1.0f, try 0.999f and 1.001f for
*** the bound arguments.
**/
bool IsFloatInRange(float value, float lower, float upper);


/** \brief Alternative function for determining if a floating point number is close to a specific value
*** \param value The floating point value to compare.
*** \param base The number that we want to compare the value to
*** \param delta The maximum degree of tolerance allowed (default value: 0.001f)
*** \return True if the value lies within the two bounds.
*** This is an alternative method for testing equality between two floating point values. The delta
*** member should always be positive.
**/
bool IsFloatEqual(float value, float base, float delta = 0.001f);


/** \brief Returns the fractional portion of a floating point value
*** \param value The floating point value to retrieve the fractional portion (format x.y)
*** \return A float containing only the fractional value (format 0.y)
**/
float GetFloatFraction(float value);


/** \brief Returns the integer portion of a floating point value
*** \param value The floating point value to retrieve the integer portion (format x.y)
*** \return A float containing only the integer value (format x.0)
**/
float GetFloatInteger(float value);


//! \brief Rounds down a float to a multiple of another float
/*!
 *  \return Rounded float
 *  \param value Number to be rounded
 *  \param multiple Multiple to be considered when rounding
 */
float FloorToFloatMultiple(const float value, const float multiple);


//!	\brief Converts data to string
/*! Thi function convers ata to string, if it has the sstring::operator<< implementated
 *  \param s String to be filled
 *  \param data Data to convert to string
 */
template<class T>
void DataToString(std::string &s, const T &data)
{
	std::ostringstream stream;
	stream << data;
	s = stream.str();
}


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
*** instead only the functions that we typically use in Allacrost. If you need a
*** basic_string function available that isn't already implemented in this class,
*** go ahead and add it yourself.
***
*** \note This class does not use wchar_t because it has poor compatibility.
*** ***************************************************************************/
class ustring {
public:
	ustring();

	ustring(const uint16*);

	static const size_t npos;

	void clear()
		{ _str.clear(); _str.push_back(0); }

	bool empty() const
		{ return _str.size() <= 1; }

	size_t length() const
		// We assume that there is always a null terminating character, hence the -1 subtracted from the size
		{ return _str.size() - 1; }

	size_t size() const
		{ return length(); }

	const uint16* c_str() const
		{ return &_str[0]; }

	size_t find(uint16 c, size_t pos = 0) const;

	size_t find(const ustring &s, size_t pos = 0) const;

	ustring substr(size_t pos = 0, size_t n = npos) const;

	ustring operator + (const ustring& s);

	ustring & operator += (uint16 c);

	ustring & operator += (const ustring& s);

	ustring & operator = (const ustring& s);

	uint16 & operator [] (size_t pos)
		{ return _str[pos]; }

	const uint16 & operator [] (size_t pos) const
		{ return _str[pos]; }

private:
	//! \brief The structure containing the unicode string data.
	std::vector<uint16> _str;
}; // class ustring

/** ****************************************************************************
*** \brief This is the superclass for all the C++ exceptions used in HoA
***
***
***
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
    Exception(const std::string & message, const std::string & file="", const int line=-1, const std::string & function="") throw();

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


/** ****************************************************************************
*** \brief Used for transforming a standard class into a singleton class
***
*** This is a templated abstract class which classes may derive from to become
*** singleton classes. To create a new singleton type class, follow the steps below.
*** It is assumed that the desired class is called "ClassName".
***
*** -# In the header file, define the class as follows: class ClassName : public hoa_utils::Singleton<ClassName>
*** -# Make hoa_utils::Singleton<ClassName> a friend of ClassName in the header file
*** -# Put the ClassName() constructor in the private section of the class, and the destructor in the public section
*** -# Define the following function in the public section of the class and implement it: bool SingletonInitialize()
*** -# In the source file, set the static template member like so: template<> ClassName* Singleton<ClassName>::_singleton_reference = NULL
***
*** With this done, your new class should be ready to go. To create and use a singleton class, do the following:
***
*** -# Call ClassName* SingletonCreate() to get a pointer to the new singleton class instance
*** -# After the singleton object has been created, next call bool SingletonInitialize() to initialize the class
*** -# If you ever need to retreive a pointer for a singleton you've created, just call the static method const ClassName* SingletonGetReference()
*** -# Call SingletonDestroy to destroy the class and its underlying singleton. You can then create and initialize the singleton once more if you'd like.
***
*** \note The creation steps listed above are the only way to properly construct a singleton class object. Don't try to circumvent it, and never attempt to
*** modify the protected static member Singleton::_singleton_reference directly, except as instructed above.
***
*** \note Sometimes singleton classes need to refer to each other to initialize themselves, particularly with game engine components. That is the purpose
*** of the SingletonInitialize() method, so that all the singleton objects can be created and then reference each other when this method is invoked. It
*** can be viewed as a helper function to the class constructor.
***
*** \note For engine singleton classes, SingletonCreate(), SingletonDestroy(), and SingletonInitialize()
*** should only be called in main.cpp before the main game loop. There may be qualified exceptions to this
*** practice, however.
***
*** \note Most of our singleton classes also define a pointer to their singleton object inside the
*** source file of the class. For example, the AudioEngine singleton contains the AudioManager class object
*** name inside the hoa_audio namespace. Therefore you do not need to call the SingletonGetReference()
*** function when this object is made available.
*** ***************************************************************************/
template<typename T> class Singleton {
protected:
	//! \brief A reference to the singleton class instance itself
	static T* _singleton_reference;

	Singleton()
		{}

	virtual ~Singleton()
		{}

public:
	//! \brief Creates and returns an instance of the singleton class
	static T* SingletonCreate() {
		if (_singleton_reference == NULL) {
			_singleton_reference = new T();
		}
		else {
			if (UTILS_DEBUG)
				std::cerr << "UTILS WARNING: Singleton::SingletonCreate() was invoked when the class object was already instantiated" << std::endl;
		}
		return _singleton_reference;
	}

	//! \brief Destroys the singleton class instance
	static void SingletonDestroy() {
		if (_singleton_reference != NULL) {
			delete _singleton_reference;
		}
		else {
			if (UTILS_DEBUG)
				std::cerr << "UTILS WARNING: Singleton::SingletonDestroy() was invoked when the class object was not instantiated" << std::endl;
		}
		_singleton_reference = NULL;
	}

	//! \brief Returns a pointer to the singleton class instance (or NULL if the class is not instantiated)
	static const T* SingletonGetReference()
		{ return _singleton_reference; }

	/** \brief A method for the inheriting class to implement, which initializes the class
	*** \return True if initialization was successful, false if it was not
	**/
	virtual bool SingletonInitialize() = 0;

private:
	Singleton(const Singleton &s);
	Singleton& operator=(const Singleton &s);
}; // template<typename T> class Singleton



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

/** \brief Determines if a string is a valid numeric string
*** \param text The string to check
*** \return A std::string containing the parameter in string form
***
*** This function will accept strings with + or - characters as the first
*** string element and strings including a single decimal point.
*** Examples of valid numeric strings are: "50", ".2350", "-252.5"
**/
bool IsStringNumeric(const std::string& text);

bool UTF16ToUTF8(const uint16 *source, char *dest, size_t length);
/** \brief Converts from UTF8 to UTF16 using iconv
*** \param source The utf8 character array to convert
*** \param dest   A utf16 destination array of size length + 1
*** \param length The number of characters to convert
***
*** This converts from UTF8 to UTF16 using the iconv library.
*** This is not expected to be used in most situations - it is called
*** by the MakeUnicodeString method. Note that the first character
*** output is the UTF16 "Byte Order Mark" and should be skipped
*** if only characters are desired.
**/
bool UTF8ToUTF16(const char *source, uint16 *dest, size_t length);

/** \brief Creates a ustring from a standard string
*** \param text The standard string to create the ustring equivalent for
*** \return A ustring containing the same information as the function parameter
***
*** This function is useful for hard-coding text to be displayed on the screen,
*** as unicode characters are the only characters allowed to be displayed. This
*** function serves primarily for debugging and diagnostic purposes.
**/
hoa_utils::ustring MakeUnicodeString(const std::string& text);

/** \brief Creates an starndard string from a ustring
*** \param text The ustring to create the equivalent standard string for
*** \return A string containing the same information as the ustring
***
*** This function is much less likely to be used as MakeUnicodeString.
*** Standard strings are used for resource loading (of images, sounds, etc.) so
*** this may come in use if a ustring contains file information.
**/
std::string MakeStandardString(const hoa_utils::ustring& text);
//@}


/** \brief A template function that returns the number of elements in an array
*** \param array The array of elements
*** \return The number of elements in the array
**/
template <typename T, size_t N>
size_t NumberElementsArray(T (&)[N])
	{ return N; }

//! \name Random Variable Genreator Fucntions
//@{
/** \brief Creates a uniformly distributed random floating point number
*** \return A floating-point value between [0.0f, 1.0f]
**/
float RandomFloat();

/** \brief Creates a random float value between a and b.
*** \param a The lower bound value
*** \param b The upper bound value
*** \return A random float with a value bewen a and b
**/
float RandomFloat(float a, float b);

/** \brief Returns a random interger value uniformally distributed between two inclusive bounds
*** \param lower_bound The lower inclusive bound
*** \param upper_bound The upper inclusive bound
*** \return An integer between [lower_bound, upper_bound]
*** \note If the user specifies a lower bound that is greater than the upper bound, the two bounds
*** are switched.
**/
int32 RandomBoundedInteger(int32 lower_bound, int32 upper_bound);

/** \brief Returns a Gaussian random value with specified mean and standard deviation
*** \param mean
*** \param std_dev The standard deviation of the Gaussian function (optional, default is 10.0f)
*** \param positive_value If true the function will not return a negative result (optional, default is true)
*** \return An Gaussian random integer with a mean and standard deviation as specified by the user
***
*** This function computes a random number based on a Gaussian Distribution Curve. This number will be between
*** mean - range and mean + range if range is greater than zero, otherwise it will return a true, unbounded
*** guassian random value. If positive_value is set to true, this function will only return a number that is
*** zero or positive.
***
*** Mean is (obviously) the mean, and the range represents the value for 3 standard deviations from the mean.
*** That means that 99.7% of the random values chosen will lay between mean - range and mean + range, if
*** range is a greater than or equal to zero.
**/
int32 GaussianRandomValue(int32 mean, float std_dev = 10.0f, bool positive_value = true);


/** \brief Returns true/false depending on the chance
*** \param chance Value between 0..100. 0 will always return false and >=100 will always return true.
*** \return True if the chance occurs
**/
bool Probability(uint32 chance);
//@}


//! \name Sorting Functions
//@{
/** \brief Performs an insertion sort on a vector of elements
*** \param swap_vec The vector of elements to be sorted.
***
*** Insertion sort should *only* be used for vectors that are already nearly sorted, or
*** for vectors of size 10 or less. Otherwise this algorithm becomes computationally expensive
*** and you should probably choose another sorting algorithm at that point. A good example of
*** code that uses this algorithm well can be found in map.cpp, which sorts map objects every frame.
*** Because map objects change position slowly, there is usually no change or little relative change
*** in sorting order from the previous pass.
***
*** \note The type of element that is passed should have its > and = operators functionally
*** correct (if T is a class, you must overload these operators). In general, its good
*** practice if you overload all comparison operators for these types.
***
*** \note Do not invoke this function with a vector of pointers to class-type objects, as it
*** will cause a compilation error.
**/
template <typename T>
void InsertionSort(std::vector<T>& swap_vec) {
	int32 i, j;
	T value;
	for (i = 1; i < swap_vec.size(); i++) {
		value = swap_vec[i];
		for (j = i - 1; j >= 0 && swap_vec[j] > value; j--) {
			swap_vec[j+1] = swap_vec[j];
		}
		swap_vec[j+1] = value;
	}
} // void InsertionSort(std::vector<T>& swap_vec)
//@}

//! \name Directory and File Manipulation Functions
//@{
/** \brief Checks if a file exists on the system or not
*** \param file_name The name of the file to check (e.g. "dat/saved_game.lua")
*** \return True if the file was found, or false if it was not found.
**/
bool DoesFileExist(const std::string& file_name);

/** \brief Moves a file from one location to another
*** \param source_name The name of the file that is to be moved
*** \param destination_name The location name to where the file should be moved to
*** \return True if the file was successfully moved, false if it was not
***
*** This function can also be used to rename files.
*** \note If there exists a file with the same name + path as the destination_name,
*** it will be overwritten without warning or indication. Be careful when using this
*** function! (Had to be modified for Win32 to explicitly delete the file)
**/
bool MoveFile(const std::string& source_name, const std::string& destination_name);

/** \brief same as move, but leaves the source file and there is no return value
 **/
void CopyFile(const std::string& source, const std::string& destination);

/** \brief Removes all files present in a directory
*** \param dir_name The name of the directory to clean (e.g. "img/screnshots")
*** \return True upon success, false upon failure
**/
bool CleanDirectory(const std::string& dir_name);

/** \brief Creates a directory relative to the path of the running application
*** \param dir_name The name of the directory to create (e.g. "img/screnshots")
*** \return True upon success, false upon failure
**/
bool MakeDirectory(const std::string& dir_name);

/** \brief Deletes a directory, as well as any files the directory may contain
*** \param dir_name The name of the directory to remove (e.g. "img/screnshots")
*** \return True upon success, false upon failure
**/
bool RemoveDirectory(const std::string& dir_name);

/** \brief Lists the contents of a directory
*** \param dir_name The name of the directory to list (e.g. "img/screenshots")
*** \param filter A string to filter the results (e.g if the string was ".lua" only files containing ".lua" in the filename will be 		***	displayed),an  empty string  value of "" can be passed if you wish to display everything with no filter.
*** \return A vector with the directory listing , a blank vector if the directory doesnt exist
**/
std::vector<std::string> ListDirectory(const std::string& dir_name, const std::string& filter);
//@}

/** \brief Deletes a specified file
*** \param filename The name of the file to be deleted
*** \return true on success false on failure
**/
bool DeleteFile(const std::string& filename);


//! \name User directory and settings paths
//@{
/** \brief Finds the OS specific directory path to save and retrieve user data
*** \param user_files If true, retrieves the path for user created content files. If false,
*** retrieves the path for application settings. Default value is true.
*** \note Only OS X systems make any use of the user_files parameter.
**/
const std::string GetUserDataPath(bool user_files = true);

/** \brief returns the directory path to save/load/delete user profiles
*** \return A string containing the path to the profiles directory
*** \TODO: might have to add in special support for OSX if we want to
*** 	   keep the profiles under the user creatd content
**/
const std::string GetUserProfilePath();

/** \brief Retrieves the path and filename of the settings file to use
*** \return A string with the settings filename, or an empty string if the settings file could not be found
**/
const std::string GetSettingsFilename();
//@}

} // namespace hoa_utils

#endif // __UTILS_HEADER__
