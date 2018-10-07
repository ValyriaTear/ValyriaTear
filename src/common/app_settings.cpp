///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "app_settings.h"

#include "app_name.h"

#include "utils/utils_common.h"
#include "utils/utils_files.h"

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

#include <sys/stat.h>
#include <fstream>
#include <sstream>

using namespace vt_utils;

namespace vt_common
{

//! \brief Copy old save files from ~/.valyriatear to new path on unices
//! And from the personal folder to the destination on Windows.
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
    for (uint32_t i = 0; i < 6; ++i) {
        std::stringstream save_filename;
        save_filename << "saved_game_" << i << ".lua";
        const std::string old_file = old_path + save_filename.str();

        if(!DoesFileExist(old_file))
            return; // Nothing to do.

        const std::string new_filename = destination_path + save_filename.str();

        if (!MoveFile(old_file.c_str(), new_filename.c_str()))
            PRINT_WARNING << "Couldn't move the save file "
                          << "('" << save_filename.str() << "') "
                          << "to new location!" << std::endl;

        PRINT_WARNING << "Moved " << save_filename.str()
                      << " file from: " << old_file << std::endl
                      << "to: " << new_filename << std::endl;
    }
#endif
}

//! \brief Copy old save files from ~/.valyriatear to new path on unices
//! And from the personal folder to the destination on Windows.
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

    const std::string old_file = std::string(path) + "/" APPUPCASEDIRNAME "/settings.lua";
    if(!DoesFileExist(old_file))
        return; // nothing to do
#endif
#ifndef __APPLE__
    const std::string new_filename = destination_path + "settings.lua";

    if (!MoveFile(old_file.c_str(), new_filename.c_str()))
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
    PRINT_WARNING << "could not idenfity user path, defaulting to system path"
                  << std::endl;
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

//! \brief Retrieves the path and filename of the settings file to use
//! \return A string with the settings filename, or an empty string
//! if the settings file could not be found
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

//! \brief Static variables storing the user data and config paths
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

} // namespace common
