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
*** \file    utils_files.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the files and paths utility code.
*** ***************************************************************************/

#ifndef __UTILS_FILES_HEADER__
#define __UTILS_FILES_HEADER__

//! Contains utility code used across the entire source code
namespace vt_utils
{

//! \name Directory and File Manipulation Functions
//@{
/** \brief Checks if a file exists.
*** \param file_name The name of the file to check (e.g. "saves/saved_game.lua")
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
bool MoveFile(const std::string &source_name, const std::string &destination_name);

/** \brief same as move, but leaves the source file and there is no return value
 **/
void CopyFile(const std::string &source, const std::string &destination);

/** \brief Removes all files in a directory.
*** \param dir_name The name of the directory to clean (e.g. "/temp/screenshots")
*** \return True upon success, false upon failure
**/
bool CleanDirectory(const std::string &dir_name);

/** \brief Creates a directory.
*** \param dir_name The name of the directory to create (e.g. "/temp/screenshots")
*** \return True upon success, false upon failure
**/
bool MakeDirectory(const std::string &dir_name);

/** \brief Deletes a directory and any files in the directory.
*** \param dir_name The name of the directory to remove (e.g. "/temp/screenshots")
*** \return True upon success, false upon failure
**/
bool RemoveDirectory(const std::string &dir_name);

/** \brief Lists the contents of a directory
*** \param dir_name The name of the directory to list (e.g. "/temp/screenshots")
*** \param filter A string to filter the results (e.g if the string was ".lua" only files containing ".lua" in the filename will be
*** displayed), an empty string value of "" can be passed if you wish to display everything with no filter.
*** \return A vector with the directory listing , a blank vector if the directory doesnt exist
**/
std::vector<std::string> ListDirectory(const std::string &dir_name, const std::string &filter);
//@}

/** \brief Deletes a specified file
*** \param filename The name of the file to be deleted
*** \return true on success false on failure
**/
bool DeleteFile(const std::string &filename);

//! \name User directory and settings paths
//@{
//! \brief Gives the OS specific directory path to save and retrieve user data
const std::string GetUserDataPath();

//! \brief Gives the OS specific directory path to save and retrieve user config data
const std::string GetUserConfigPath();

/** \brief Gives the path and filename of the settings file to use
*** \return A string with the settings filename, or an empty string if the settings file could not be found
**/
const std::string GetSettingsFilename();
//@}

} // namespace vt_utils

#endif // __UTILS_FILES_HEADER__
