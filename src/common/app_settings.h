///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __APP_SETTINGS_HEADER__
#define __APP_SETTINGS_HEADER__

#include <string>

//! Contains utility code used across the entire source code
namespace vt_common
{

//! \brief Gives the OS specific directory path to save and retrieve user data
const std::string GetUserDataPath();

//! \brief Gives the OS specific directory path to save and retrieve user config data
const std::string GetUserConfigPath();

//! \brief Gives the path and filename of the settings file to use
//! \return A string with the settings filename, or an empty string if the settings file could not be found
const std::string GetSettingsFilename();

} // namespace vt_common

#endif // __APP_SETTINGS_HEADER__
