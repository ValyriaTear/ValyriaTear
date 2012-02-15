///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    utils.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for Allacrost utility code.
*** ***************************************************************************/

// Headers included for directory manipulation. Windows has its own way of
// dealing with directories, hence the need for conditional includes
#ifdef _WIN32
	#include <direct.h>
	#include <shlobj.h>
#else
	#include <dirent.h>
	#include <sys/types.h>
	#include <pwd.h>
#endif

#include <fstream>
#include <sys/stat.h>
#include <iconv.h>

#include "utils.h"
#include <SDL/SDL.h>

using namespace std;

namespace hoa_utils {

bool UTILS_DEBUG = false;

////////////////////////////////////////////////////////////////////////////////
///// Numeric utility functions
////////////////////////////////////////////////////////////////////////////////

uint32 RoundUpPow2(uint32 x) {
	x -= 1;
	x |= x >>  1;
	x |= x >>  2;
	x |= x >>  4;
	x |= x >>  8;
	x |= x >> 16;
	return x + 1;
}



bool IsPowerOfTwo(uint32 x) {
	return ((x & (x-1)) == 0);
}



bool IsOddNumber(uint32 x) {
	// NOTE: this happens to work for both little and big endian systems
	return (x & 0x00000001);
}



bool IsFloatInRange(float value, float lower, float upper) {
	return (value >= lower && value <= upper);
}



bool IsFloatEqual(float value, float base, float delta) {
	return (value >= (base - delta) && value <= (base + delta));
}



float GetFloatFraction(float value) {
	return  (value - GetFloatInteger(value));
}



float GetFloatInteger(float value) {
	return static_cast<float>(static_cast<int>(value));
}



float FloorToFloatMultiple (const float value, const float multiple) {
	return multiple * floor(value / multiple);
}

////////////////////////////////////////////////////////////////////////////////
///// ustring Class
////////////////////////////////////////////////////////////////////////////////

const size_t ustring::npos = ~0;



ustring::ustring() {
	_str.push_back(0);
}



ustring::ustring(const uint16 *s) {
	_str.clear();

	if (!s) {
		_str.push_back(0);
		return;
	}

	while (*s != 0) {
		_str.push_back(*s);
		++s;
	}

	_str.push_back(0);
}


// Return a substring starting at pos, continuing for n elements
ustring ustring::substr(size_t pos, size_t n) const
{
	size_t len = length();

	if (pos >= len)
		throw std::out_of_range("pos passed to substr() was too large");

	ustring s;
	while (n > 0 && pos < len) {
		s += _str[pos];
		++pos;
		--n;
	}

	return s;
}


// Concatenates string to another
ustring ustring::operator + (const ustring& s)
{
    ustring temp = *this;

	// nothing to do for empty string
	if (s.empty())
		return temp;

	// add first character of string into the null character spot
	temp._str[length()] = s[0];

	// add rest of characters afterward
	size_t len = s.length();
	for (size_t j = 1; j < len; ++j) {
		temp._str.push_back(s[j]);
	}

	// Finish off with a null character
	temp._str.push_back(0);

	return temp;
}


// Adds a character to end of this string
ustring & ustring::operator += (uint16 c) {
	_str[length()] = c;
	_str.push_back(0);

	return *this;
}


// Concatenate another string on to the end of this string
ustring & ustring::operator += (const ustring &s) {
	// nothing to do for empty string
	if (s.empty())
		return *this;

	// add first character of string into the null character spot
	_str[length()] = s[0];

	// add rest of characters afterward
	size_t len = s.length();
	for (size_t j = 1; j < len; ++j) {
		_str.push_back(s[j]);
	}

	// Finish off with a null character
	_str.push_back(0);

	return *this;
}


// Will assign the current string to this string
ustring & ustring::operator = (const ustring &s) {
	clear();
	operator += (s);

	return *this;
} // ustring & ustring::operator = (const ustring &s)


// Finds a character within a string, starting at pos. If nothing is found, npos is returned
size_t ustring::find(uint16 c, size_t pos) const {
	size_t len = length();

	for (size_t j = pos; j < len; ++j) {
		if (_str[j] == c)
			return j;
	}

	return npos;
} // size_t ustring::find(uint16 c, size_t pos) const


// Finds a string within a string, starting at pos. If nothing is found, npos is returned
size_t ustring::find(const ustring &s, size_t pos) const {
	size_t len = length();
	size_t total_chars = s.length();
	size_t chars_found = 0;

	for (size_t j = pos; j < len; ++j) {
		if (_str[j] == s[chars_found]) {
			++chars_found;
			if (chars_found == total_chars) {
				return (j - chars_found + 1);
			}
		}
		else {
			chars_found = 0;
		}
	}

	return npos;
} // size_t ustring::find(const ustring &s, size_t pos) const


////////////////////////////////////////////////////////////////////////////////
///// Exception class
////////////////////////////////////////////////////////////////////////////////

Exception::Exception(const std::string & message, const std::string & file, const int line, const std::string & function) throw() :
    _message(message),
    _file(file),
    _line(line),
    _function(function)
{}



Exception::~Exception() throw()
{}



string Exception::ToString() const throw() {
    return string("EXCEPTION:" + _file + ":" + _function + ":" + NumberToString(_line) + ": " + _message);
}


string Exception::GetMessage() const throw() {
    return _message;
}


string Exception::GetFile() const throw() {
    return _file;
}


int Exception::GetLine() const throw() {
    return _line;
}


string Exception::GetFunction() const throw() {
    return _function;
}

////////////////////////////////////////////////////////////////////////////////
///// string and ustring manipulator functions
////////////////////////////////////////////////////////////////////////////////

// Returns true if the given text is a number
bool IsStringNumeric(const string& text) {
	if (text.empty())
		return false;

	// Keep track of whether decimal point is allowed. It is allowed to be present in the text zero or one times only.
	bool decimal_allowed = true;

	size_t len = text.length();

	// Check each character of the string one at a time
	for (size_t c = 0; c < len; ++c) {
		// The only non numeric characters allowed are a - or + as the first character, and one decimal point anywhere
		bool numeric_char = (isdigit(static_cast<int32>(text[c]))) || (c==0 && (text[c] == '-' || text[c] == '+'));

		if (!numeric_char) {
			// Check if the 'bad' character is a decimal point first before labeling the string invalid
			if (decimal_allowed && text[c] == '.') {
				decimal_allowed = false; // Decimal points are now invalid for the rest of the string
			}
			else {
				return false;
			}
		}
	}

	return true;
} // bool IsStringNumeric(const string& text)

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
  #define UTF_16_ICONV_NAME "UTF-16LE"
#else
  #define UTF_16_ICONV_NAME "UTF-16BE"
#endif

#define UTF_16_BOM_STD 0xFEFF
#define UTF_16_BOM_REV 0xFFFE

// Converts from UTF16 to UTF8, using iconv
bool UTF16ToUTF8(const uint16 *source, char *dest, size_t length) {
	if (!length)
		return true;

	iconv_t convertor = iconv_open("UTF-8", UTF_16_ICONV_NAME);
	if (convertor == (iconv_t) -1) {
		return false;
	}

	const char *source_char = reinterpret_cast<const char *>(source);
	#if (defined(_LIBICONV_VERSION) && _LIBICONV_VERSION == 0x0109)
	// We are using an iconv API that uses const char*
	const char *sourceChar = source_char;
	#else
	// The iconv API doesn't specify a const source for legacy support reasons.
	// Versions after 0x0109 changed back to char* for POSIX reasons.
	char *sourceChar = const_cast<char *>(source_char);
	#endif
	char *destChar   = dest;
	size_t sourceLen = length;
	size_t destLen   = length;
	size_t ret = iconv(convertor, &sourceChar, &sourceLen,
				      &destChar,   &destLen);
	iconv_close(convertor);
	if (ret == (size_t) -1) {
		perror("iconv");
		return false;
	}
	return true;
}

// Converts from UTF8 to UTF16, including the Byte Order Mark, using iconv
// Skip the first uint16 to skip the BOM.
bool UTF8ToUTF16(const char *source, uint16 *dest, size_t length) {
	if (!length)
		return true;

	iconv_t convertor = iconv_open(UTF_16_ICONV_NAME, "UTF-8");
	if (convertor == (iconv_t) -1) {
		return false;
	}

	#if (defined(_LIBICONV_VERSION) && _LIBICONV_VERSION == 0x0109)
		// We are using an iconv API that uses const char*
		const char *sourceChar = source;
	#else
		// The iconv API doesn't specify a const source for legacy support reasons.
		// Versions after 0x0109 changed back to char* for POSIX reasons.
		char *sourceChar = const_cast<char *>(source);
	#endif
	char *destChar   = reinterpret_cast<char *>(dest);
	size_t sourceLen = length;
	size_t destLen   = (length + 1) * 2;
	size_t ret = iconv(convertor, &sourceChar, &sourceLen,
				      &destChar,   &destLen);
	iconv_close(convertor);
	if (ret == (size_t) -1) {
		perror("iconv");
		return false;
	}
	return true;
}

// Creates a ustring from a normal string
ustring MakeUnicodeString(const string& text) {
	int32 length = static_cast<int32>(text.length() + 1);
	uint16 *ubuff = new uint16[length + 1];
	memset(ubuff, 0, 2*(length+1));
	uint16 *utf16String = ubuff;

	if (UTF8ToUTF16(text.c_str(), ubuff, length)) {
		// Skip the "Byte Order Mark" from the UTF16 specification
		if (utf16String[0] == UTF_16_BOM_STD ||  utf16String[0] == UTF_16_BOM_REV) {
			utf16String = ubuff + 1;
		}

		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			// For some reason, using UTF-16BE to iconv on big-endian machines
			// still does not create correctly accented characters, so this
			// byte swapping must be performed (only for irregular characters,
			// hence the mask).

			for (int32 c = 0; c < length; c++)
				if (utf16String[c] & 0xFF80)
					utf16String[c] = (utf16String[c] << 8) | (utf16String[c] >> 8);
		#endif
	}
	else {
		for (int32 c = 0; c < length; ++c) {
			ubuff[c] = static_cast<uint16>(text[c]);
		}
	}

	ustring new_ustr(utf16String);
	delete[] ubuff;

	return new_ustr;
} // ustring MakeUnicodeString(const string& text)


// Creates a normal string from a ustring
string MakeStandardString(const ustring& text) {
	int32 length = static_cast<int32>(text.length());

	unsigned char *strbuff = new unsigned char[length+1];
	strbuff[length] = '\0';

	for (int32 c = 0; c < length; ++c) {
		uint16 curr_char = text[c];

		if(curr_char > 0xff)
			strbuff[c] = '?';
		else
			strbuff[c] = static_cast<unsigned char>(curr_char);
	}

	string new_str(reinterpret_cast<char*>(strbuff));
	delete [] strbuff;

	return new_str;
} // string MakeStandardString(const ustring& text)

////////////////////////////////////////////////////////////////////////////////
///// Random number generator functions
////////////////////////////////////////////////////////////////////////////////

float RandomFloat() {
	return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}



float RandomFloat(float a, float b) {
	if (a > b) {
		float c = a;
		a = b;
		b = c;
	}

	float r = static_cast<float>(rand() % 10001);
	return a + (b - a) * r / 10000.0f;
}


// Returns a random integer between two inclusive bounds
int32 RandomBoundedInteger(int32 lower_bound, int32 upper_bound) {
	int32 range;  // The number of possible values we may return
	float result;

	range = upper_bound - lower_bound + 1;
	if (range < 0) { // Oops, someone accidentally switched the lower/upper bound arguments
		if (UTILS_DEBUG) cerr << "UTILS WARNING: Call to RandomNumber had bound arguments swapped." << endl;
		range = range * -1;
	}

	result = range * RandomFloat();
	result = result + lower_bound; // Shift result so that it is within the correct bounds

	return static_cast<int32>(result);
} // int32 RandomBoundedInteger(int32 lower_bound, int32 upper_bound)


// Creates a Gaussian random interger value.
// std_dev and positive_value are optional arguments with default values 10.0f and true respectively
int32 GaussianRandomValue(int32 mean, float std_dev, bool positive_value) {
	float x, y, r;  // x and y are coordinates on the unit circle
	float grv_unit; // Used to hold a Gaussian random variable on a normal distribution curve (mean 0, stand dev 1)
	float result;

	// Make sure that the standard deviation is positive
	if (std_dev < 0) {
		cerr << "UTILS WARNING: negative value for standard deviation argument in function GaussianValue" << endl;
		std_dev = -1.0f * std_dev;
	}

	// Computes a standard Gaussian random number using the the polar form of the Box-Muller transformation.
	// The algorithm computes a random point (x, y) inside the unit circle centered at (0, 0) with radius 1.
	// Then a Gaussian random variable with mean 0 and standard deviation 1 is computed by:
	//
	// x * sqrt(-2.0 * log(r) / r)
	//
	// Reference: Knuth, The Art of Computer Programming, Volume 2, p. 122

	// This loop is executed 4 / pi = 1.273 times on average
	do {
		x = 2.0f * RandomFloat() - 1.0f;     // Get a random x-coordinate [-1.0f, 1.0f]
		y = 2.0f * RandomFloat() - 1.0f;     // Get a random y-coordinate [-1.0f, 1.0f]
		r = x*x + y*y;
	} while (r > 1.0f || r == 0.0f);
	grv_unit = x * sqrt(-2.0f * log(r) / r);

	// Use the standard gaussian value to create a random number with the desired mean and standard deviation.
	result = (grv_unit * std_dev) + mean;

	// Return zero if a negative result was found and only positive values were to be returned
	if (result < 0.0f && positive_value)
		return 0;
	else
		return static_cast<int32>(result);
} // int32 GaussianValue(int32 mean, float std_dev, bool positive_value)


// Returns true/false depending on the chance
bool Probability(uint32 chance) {
	uint32 value = static_cast<uint32>(RandomBoundedInteger(1, 100));
	if (value <= chance)
		return true;
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////
///// Directory manipulation functions
////////////////////////////////////////////////////////////////////////////////

bool DoesFileExist(const std::string& file_name) {
	// Modified to use platform specific code because on windows stat does not work on directories,
	// but on POSIX compliant systems it does, and GetFileAttributes works for both folders and
	// directories on win32
	#ifdef _WIN32
		return GetFileAttributes(file_name.c_str()) != INVALID_FILE_ATTRIBUTES;
	#else
		struct stat buf;
		if (stat(file_name.c_str(), &buf) == 0)
			return true;
		else
			return false;
	#endif
}



bool MoveFile(const std::string& source_name, const std::string& destination_name) {
	if (DoesFileExist(destination_name))
		remove(destination_name.c_str());
	return (rename(source_name.c_str(), destination_name.c_str()) == 0);
}



void CopyFile(const std::string& source, const std::string& destination) {
	if (DoesFileExist(destination))
		remove(destination.c_str());
	ifstream src(source.c_str());
	ofstream dst(destination.c_str());
	dst << src.rdbuf();
}



bool MakeDirectory(const std::string& dir_name) {
	// Don't do anything if the directory already exists
	struct stat buf;
	int32 i = stat(dir_name.c_str(), &buf);
	if (i == 0)
		return true;

	// Create the directory with mkdir(). Note that Windows does not require file permissions to be set, but
	// all other operating systems do.

	#ifdef _WIN32
		int32 success = mkdir(dir_name.c_str());
	#else
		int32 success = mkdir(dir_name.c_str(), S_IRWXG | S_IRWXO | S_IRWXU);
	#endif

	if (success == -1) {
		cerr << "UTILS ERROR: could not create directory: " << dir_name.c_str() << endl;
		return false;
	}

	return true;
}



bool CleanDirectory(const std::string& dir_name) {
	// Don't do anything if the directory doesn't exist
	struct stat buf;
	int32 i = stat(dir_name.c_str(), &buf);
	if (i != 0)
		return true;

	#ifdef _WIN32
		//--- WINDOWS --------------------------------------------------------------

		// Get the current directory that the Allacrost application resides in
		char app_path[1024];
		GetCurrentDirectoryA(1024, app_path);

		int32 app_path_len = static_cast<int32>(strlen(app_path));
		if (app_path_len <= 0)
			return false;
		if(app_path[app_path_len-1] == '\\')    // Remove the ending slash if one is there
			app_path[app_path_len-1] = '\0';

		string full_path = app_path;

		if (dir_name[0] == '/' || dir_name[0] == '\\') {
			full_path += dir_name;
		}
		else {
			full_path += "\\";
			full_path += dir_name;
		}

		char file_found[1024];
		WIN32_FIND_DATAA info;
		HANDLE hp;
		sprintf(file_found, "%s\\*.*", full_path.c_str());
		hp = FindFirstFileA(file_found, &info);

		if (hp != INVALID_HANDLE_VALUE) {
			// Remove each file from the full_path directory
			do {
				sprintf(file_found, "%s\\%s", full_path.c_str(), info.cFileName);
				DeleteFileA(file_found);
			} while(FindNextFileA(hp, &info));
		}
		FindClose(hp);

	#else
		//--- NOT WINDOWS ----------------------------------------------------------

	DIR *parent_dir;
	struct dirent *dir_file;

	parent_dir = opendir(dir_name.c_str());   // open the directory we want to clean
	if (!parent_dir) {
		cerr << "UTILS ERROR: failed to clean directory: " << dir_name << endl;
		return false;
	}

	string base_dir = dir_name;
	if (base_dir[base_dir.length()-1] != '/')
		base_dir += "/";

	// Remove each file found in the parent directory
	while ((dir_file = readdir(parent_dir))) {
		string file_name = base_dir + dir_file->d_name;
		remove(file_name.c_str());
	}

	closedir(parent_dir);

	#endif

	return true;
}



bool RemoveDirectory(const std::string& dir_name) {
	// Don't do anything if the directory doesn't exist
	struct stat buf;
	int32 i = stat(dir_name.c_str(), &buf);
	if (i != 0)
		return true;

	// Remove any files that still reside in the directory
	CleanDirectory(dir_name);

	// Finally, remove the folder itself with rmdir()
	int32 success = rmdir(dir_name.c_str());

	if (success == -1) {
		cerr << "UTILS ERROR: could not delete directory: " << dir_name << endl;
		return false;
	}

	return true;
}

vector<string> ListDirectory(const std::string& dir_name, const std::string& filter) {
	//create our vector
	vector<string> directoryList;

	//Don't try to list if the directory does not exist
	struct stat buf;
	int32 i = stat(dir_name.c_str(), &buf);
	if (i != 0)
		return directoryList;

	//directory exists so lets list
	#if defined _WIN32
	//Windows platform

		// Get the current directory that the Allacrost application resides in
		char app_path[1024];
		GetCurrentDirectoryA(1024, app_path);

		int32 app_path_len = static_cast<int32>(strlen(app_path));
		if (app_path_len <= 0)
			return directoryList;
		if(app_path[app_path_len-1] == '\\')    // Remove the ending slash if one is there
			app_path[app_path_len-1] = '\0';

		string full_path = app_path;

		if (dir_name[0] == '/' || dir_name[0] == '\\') {
			full_path += dir_name;
		}
		else {
			full_path += "\\";
			full_path += dir_name;
		}

		char file_found[1024];
		WIN32_FIND_DATAA info;
		HANDLE hp;
		sprintf(file_found, "%s\\*.*", full_path.c_str());
		hp = FindFirstFileA(file_found, &info);

		if (hp != INVALID_HANDLE_VALUE) {
			// List each file from the full_path directory
			do {
			   std::string fileName(file_found);
				if(filter == "")
					directoryList.push_back(file_found);
				else if(fileName.find(filter) != string::npos)
					directoryList.push_back(file_found);
			} while(FindNextFileA(hp, &info));
		}
		FindClose(hp);
	#else
	//Not Windows
	DIR *dir;
	struct dirent *dir_file;
	dir = opendir(dir_name.c_str()); //open the directory for listing
	if(!dir) {
		cerr << "UTILS ERROR: Failed to list directory: " << dir_name << endl;
		return directoryList;
	}

	//List each file found in the directory as long as it end with .lua
	while ((dir_file = readdir(dir))) {
		string fileName = dir_file->d_name;
		//contains a .lua ending so put it in the directory
		if(filter == "")
			directoryList.push_back(dir_file->d_name);
		else if(fileName.find(filter) != string::npos)
			directoryList.push_back(dir_file->d_name);
	}

	closedir(dir);

	#endif

	return directoryList;
}

bool DeleteFile(const std::string &filename) {
	if(DoesFileExist(filename.c_str())) {
		remove(filename.c_str());
		if(!DoesFileExist(filename.c_str()))
			return true;
	}
	return false;
}




const std::string GetUserDataPath(bool user_files) {
	#if defined _WIN32
		TCHAR path[MAX_PATH];

		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, path))) {
			string user_path = string(path) + "/Allacrost/";
			if (DoesFileExist(user_path) == false)
				MakeDirectory(user_path);
			return user_path;
		}

	#elif defined __MACH__
		passwd *pw = getpwuid(getuid());
		if (pw) {
			string path = "";
			if (user_files)
				path = string(pw->pw_dir) + "/Library/Application Support/Allacrost/";
			else
				path = string(pw->pw_dir) + "/Library/Preferences/Allacrost/";
			if (DoesFileExist(path) == false)
				MakeDirectory(path);
			return path;
		}

	#else // Linux, BSD, other POSIX systems
		passwd *pw = getpwuid(getuid());
		if (pw) {
			string path = string(pw->pw_dir) + "/.allacrost/";
			if (DoesFileExist(path) == false)
				MakeDirectory(path);
			return path;
		}
	#endif

	// Default path if a specific solution could not be found. Note that this path may
	// not be writable by the user since it could be installed in administrator/root space
	PRINT_WARNING << "could not idenfity user path, defaulting to system path" << endl;
	return "dat/";
}

const std::string GetUserProfilePath()
{
	string profile_path = GetUserDataPath(false) + "profiles/";
	if(!DoesFileExist(profile_path))
		MakeDirectory(profile_path);

	return profile_path;
}



const std::string GetSettingsFilename() {
	std::string settings_file;

	settings_file = GetUserProfilePath() + "settings.lua";
	if (DoesFileExist(settings_file) == false) {
		settings_file = "dat/config/settings.lua";
		if (DoesFileExist(settings_file) == false) {
			PRINT_WARNING << "settings.lua file not found." << endl;
		}
	}

	return settings_file;
}

} // namespace utils
