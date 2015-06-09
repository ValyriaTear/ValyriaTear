///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    utils_files.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for the utility code.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "utils_files.h"

namespace vt_utils
{

bool DoesFileExist(const std::string& file_name)
{
    // Modified to use platform specific code because on windows stat does not work on directories,
    // but on POSIX compliant systems it does, and GetFileAttributes works for both folders and
    // directories on win32
#ifdef _WIN32
    return GetFileAttributesA(file_name.c_str()) != INVALID_FILE_ATTRIBUTES;
#else
    struct stat buf;
    if(stat(file_name.c_str(), &buf) == 0)
        return true;
    else
        return false;
#endif
}

bool MoveFile(const std::string &source_name, const std::string &destination_name)
{
    if(DoesFileExist(destination_name))
        remove(destination_name.c_str());
    return (rename(source_name.c_str(), destination_name.c_str()) == 0);
}

void CopyFile(const std::string &source, const std::string &destination)
{
    if(DoesFileExist(destination))
        remove(destination.c_str());
    std::ifstream src(source.c_str());
    std::ofstream dst(destination.c_str());
    dst << src.rdbuf();
}

bool MakeDirectory(const std::string &dir_name)
{
    // Don't do anything if the directory already exists.
    if (DoesFileExist(dir_name)) {
        return true;
    }

    // Create the directory with mkdir(). Note that Windows does not require file permissions to be set, but
    // all other operating systems do.

#ifdef _WIN32
    int32 success = mkdir(dir_name.c_str());
#else
    int32 success = mkdir(dir_name.c_str(), S_IRWXG | S_IRWXO | S_IRWXU);
#endif

    if(success == -1) {
        std::cerr << "UTILS ERROR: could not create directory: " << dir_name.c_str() << std::endl;
        return false;
    }

    return true;
}

bool CleanDirectory(const std::string &dir_name)
{
    // Don't do anything if the directory doesn't exist.
    if (!DoesFileExist(dir_name)) {
        return true;
    }

#ifdef _WIN32
    //--- WINDOWS --------------------------------------------------------------

    WIN32_FIND_DATAA info = { 0 };
    HANDLE handle = nullptr;

    char file_found[1024];
    memset(file_found, 0, sizeof(file_found));
    sprintf(file_found, "%s*.*", dir_name.c_str());

    handle = FindFirstFileA(file_found, &info);
    if (handle != INVALID_HANDLE_VALUE) {
        // Remove each file from the directory.
        do {
            sprintf(file_found, "%s%s", dir_name.c_str(), info.cFileName);
            DeleteFileA(file_found);
        } while (FindNextFileA(handle, &info));
    }

    FindClose(handle);
#else
    //--- NOT WINDOWS ----------------------------------------------------------

    DIR *parent_dir;
    struct dirent *dir_file;

    parent_dir = opendir(dir_name.c_str());   // open the directory we want to clean
    if(!parent_dir) {
        std::cerr << "UTILS ERROR: failed to clean directory: " << dir_name << std::endl;
        return false;
    }

    std::string base_dir = dir_name;
    if(base_dir[base_dir.length() - 1] != '/')
        base_dir += "/";

    // Remove each file found in the parent directory
    while((dir_file = readdir(parent_dir))) {
        std::string file_name = base_dir + dir_file->d_name;
        remove(file_name.c_str());
    }

    closedir(parent_dir);

#endif

    return true;
}

bool RemoveDirectory(const std::string &dir_name)
{
    // Don't do anything if the directory doesn't exist
    if (!DoesFileExist(dir_name)) {
        return true;
    }

    // Remove any files that still reside in the directory
    CleanDirectory(dir_name);

    // Finally, remove the folder itself with rmdir()
    int32 success = rmdir(dir_name.c_str());

    if(success == -1) {
        std::cerr << "UTILS ERROR: could not delete directory: " << dir_name << std::endl;
        return false;
    }

    return true;
}

std::vector<std::string> ListDirectory(const std::string &dir_name, const std::string &filter)
{
    //create our vector
    std::vector<std::string> directoryList;

    // Don't try to list files if the directory does not exist.
    if (!DoesFileExist(dir_name)) {
        return directoryList;
    }

    //directory exists so lets list
#if defined _WIN32
    //--- WINDOWS --------------------------------------------------------------

    WIN32_FIND_DATAA info = { 0 };
    HANDLE handle = nullptr;

    char file_found[1024];
    memset(file_found, 0, sizeof(file_found));
    sprintf(file_found, "%s\\*.*", dir_name.c_str());

    if (handle != INVALID_HANDLE_VALUE) {
        // List each file from the directory.
        do {
            std::string file_name(file_found);
            if (filter == "") {
                directoryList.push_back(file_found);
            }
            else if (file_name.find(filter) != std::string::npos) {
                directoryList.push_back(file_found);
            }
        } while (FindNextFileA(handle, &info));
    }

    FindClose(handle);
#else
    //Not Windows
    DIR *dir;
    struct dirent *dir_file;
    dir = opendir(dir_name.c_str()); //open the directory for listing
    if(!dir) {
        std::cerr << "UTILS ERROR: Failed to list directory: " << dir_name << std::endl;
        return directoryList;
    }

    //List each file found in the directory as long as it end with .lua
    while((dir_file = readdir(dir))) {
        std::string fileName = dir_file->d_name;
        //contains a .lua ending so put it in the directory
        if(filter == "")
            directoryList.push_back(dir_file->d_name);
        else if(fileName.find(filter) != std::string::npos)
            directoryList.push_back(dir_file->d_name);
    }

    closedir(dir);

#endif

    return directoryList;
}

bool DeleteFile(const std::string &filename)
{
    if(DoesFileExist(filename)) {
        remove(filename.c_str());
        if(!DoesFileExist(filename))
            return true;
    }
    return false;
}

// Copy old save files from ~/.valyriatear to new path on unices
// And from the personal folder to the destination on Windows.
//! \DEPRECATED: Remove this in one or two releases.
static void _CopyOldSaveFiles(const std::string &destination_path)
{
    if(!DoesFileExist(destination_path)) {
        PRINT_WARNING << "No valid destination path given: " << destination_path
            << std::endl << "Can't copy old save files." << std::endl;
        return;
    }

#if (defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(SOLARIS))
    // We assume the old save files are in ~/.valyriatear
    passwd *pw = getpwuid(getuid());
    if(!pw)
        return;

    std::string old_path = std::string(pw->pw_dir) + "/." APPSHORTNAME "/";
    if(!DoesFileExist(old_path))
        return; // Nothing to do.
#elif defined _WIN32
    char path[MAX_PATH];
    // %USERPROFILE%\My Documents
    if(!(SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, 0, path))))
        return; // No folder, nothing to do.

    std::string old_path = std::string(path) + "/" APPUPCASEDIRNAME "/";
    if(!DoesFileExist(old_path))
        return; // nothing to do
#endif

#ifndef __APPLE__
    for (uint32 i = 0; i < 6; ++i) {
        std::stringstream save_filename;
        save_filename << "saved_game_" << i << ".lua";
        std::string old_file = old_path + save_filename.str();

        if(!DoesFileExist(old_file))
            return; // Nothing to do.

        std::string new_filename = destination_path + save_filename.str();

        if (!MoveFile(old_file, new_filename))
            PRINT_WARNING << "Couldn't move the save file ('" << save_filename.str() << "') to new location!" << std::endl;

        PRINT_WARNING << "Moved " << save_filename.str() << " file from: " << old_file << std::endl
            << "to: " << new_filename << std::endl;
    }
#endif
}

// Copy old save files from ~/.valyriatear to new path on unices
// And from the personal folder to the destination on Windows.
//! \DEPRECATED: Remove this in one or two releases.
static void _CopyOldSettingsFile(const std::string &destination_path)
{
    if(!DoesFileExist(destination_path)) {
        PRINT_WARNING << "No valid destination path given: " << destination_path
            << std::endl << "Can't copy old settings file." << std::endl;
        return;
    }

#if (defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(SOLARIS))
    // We assume the old settings.lua file is in ~/.valyriatear
    passwd *pw = getpwuid(getuid());
    if(!pw)
        return; // Nothing to do

    std::string old_file = std::string(pw->pw_dir) + "/." APPSHORTNAME "/settings.lua";
    if(!DoesFileExist(old_file))
        return; // Nothing to do.

#elif defined _WIN32
    char path[MAX_PATH];
    // %USERPROFILE%\My Documents
    if(!(SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, 0, path))))
        return; // No folder, nothing to do.

    std::string old_file = std::string(path) + "/" APPUPCASEDIRNAME "/settings.lua";
    if(!DoesFileExist(old_file))
        return; // nothing to do
#endif
#ifndef __APPLE__
    std::string new_filename = destination_path + "settings.lua";

    if (!MoveFile(old_file, new_filename))
        PRINT_WARNING << "Couldn't move the settings file to new location!" << std::endl;

    PRINT_WARNING << "Moved settings.lua file from: " << old_file << std::endl
        << "to: " << new_filename << std::endl;
#endif
}

//! \brief Finds the OS specific directory path to save and retrieve user data
static const std::string _SetupUserDataPath()
{
#if defined _WIN32
    char path[MAX_PATH];
    // %APPDATA% (%USERPROFILE%\Application Data)
    if(SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path))) {
        std::string user_path = std::string(path) + "/" APPUPCASEDIRNAME "/";
        if(!DoesFileExist(user_path))
            MakeDirectory(user_path);

        _CopyOldSaveFiles(user_path);

        return user_path;
    }

#elif defined __APPLE__
    passwd *pw = getpwuid(getuid());
    if(pw) {
        std::string path = std::string(pw->pw_dir) + "/Library/Application Support/" APPUPCASEDIRNAME "/";
        if(!DoesFileExist(path))
            MakeDirectory(path);
        return path;
    }

#else // Linux, BSD, other POSIX systems
    // Implementation of the freedesktop specs (at least partially)
    // http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html

    // $XDG_DATA_HOME/valyriatear/
    // equals to: ~/.local/share/valyriatear/ most of the time
    if (getenv("XDG_DATA_HOME")) {
        std::string path = std::string(getenv("XDG_DATA_HOME")) + "/" APPSHORTNAME "/";
        if(!DoesFileExist(path))
            MakeDirectory(path);
        _CopyOldSaveFiles(path);

        return path;
    }
    // We create a sane default: ~/.local/share/valyriatear
    passwd *pw = getpwuid(getuid());
    if(pw) {
        std::string path = std::string(pw->pw_dir) + "/.local/";
        if(!DoesFileExist(path))
            MakeDirectory(path);
        path += "/share/";
        if(!DoesFileExist(path))
            MakeDirectory(path);
        path += "/" APPSHORTNAME "/";
        if(!DoesFileExist(path))
            MakeDirectory(path);
        _CopyOldSaveFiles(path);

        return path;
    }
#endif

    // Default path if a specific solution could not be found. Note that this path may
    // not be writable by the user since it could be installed in administrator/root space
    PRINT_WARNING << "could not idenfity user path, defaulting to system path" << std::endl;
    return "data/";
}

//! \brief Finds the OS specific directory path to save and retrieve user config data
static const std::string _SetupUserConfigPath()
{
#if defined _WIN32
    char path[MAX_PATH];
    // %APPDATA% (%USERPROFILE%\Application Data)
    if(SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path))) {
        std::string user_path = std::string(path) + "/" APPUPCASEDIRNAME "/";
        if(!DoesFileExist(user_path))
            MakeDirectory(user_path);
        _CopyOldSettingsFile(user_path);
        return user_path;
     }

#elif defined __APPLE__
    passwd *pw = getpwuid(getuid());
    if(pw) {
        std::string path = std::string(pw->pw_dir) + "/Library/Preferences/" APPUPCASEDIRNAME "/";
        if(!DoesFileExist(path))
            MakeDirectory(path);
        return path;
    }

#else // Linux, BSD, other POSIX systems
    // Implementation of the freedesktop specs (at least partially)
    // http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html

    // $XDG_CONFIG_HOME/valyriatear/
    // equals to: ~/.config/valyriatear/ most of the time
    if (getenv("XDG_CONFIG_HOME")) {
        std::string path = std::string(getenv("XDG_CONFIG_HOME")) + "/" APPSHORTNAME "/";
        if(!DoesFileExist(path))
            MakeDirectory(path);
        _CopyOldSettingsFile(path);

        return path;
    }

    // We create a sane default: ~/.config/valyriatear
    passwd *pw = getpwuid(getuid());
    if(pw) {
        std::string path = std::string(pw->pw_dir) + "/.config/";
        if(!DoesFileExist(path))
            MakeDirectory(path);
        path += "/" APPSHORTNAME "/";
        if(!DoesFileExist(path))
            MakeDirectory(path);
        _CopyOldSettingsFile(path);

        return path;
    }
#endif

    // Default path if a specific solution could not be found. Note that this path may
    // not be writable by the user since it could be installed in administrator/root space
    PRINT_WARNING << "could not idenfity user config path, defaulting to system path" << std::endl;
    return "data/";
}

/** \brief Retrieves the path and filename of the settings file to use
*** \return A string with the settings filename, or an empty string if the settings file could not be found
**/
static const std::string _SetupSettingsFilename()
{
    std::string settings_file = GetUserConfigPath() + "settings.lua";
    if(!DoesFileExist(settings_file)) {
        #ifdef __APPLE__
            settings_file = "../Resources/data/config/settings.lua";
        #else
            settings_file = "data/config/settings.lua";
        #endif

        if(!DoesFileExist(settings_file))
            PRINT_WARNING << "settings.lua file not found." << std::endl;
    }

    return settings_file;
}

// Static variables storing the user data and config paths
static std::string _data_path;
static std::string _config_path;
static std::string _config_filename;

const std::string GetUserDataPath()
{
    if (_data_path.empty())
        _data_path = _SetupUserDataPath();

    return _data_path;
}

const std::string GetUserConfigPath()
{
    if (_config_path.empty())
        _config_path = _SetupUserConfigPath();

    return _config_path;
}

const std::string GetSettingsFilename()
{
    if (_config_filename.empty())
        _config_filename = _SetupSettingsFilename();

    return _config_filename;
}

} // namespace utils
