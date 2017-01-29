////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __HOME_MAP_HEADER__
#define __HOME_MAP_HEADER__

#include <string>

namespace vt_global {

/**
 * \brief Contains the previous "home" map location data.
 * This is used to be able to go back to the latest "home" when permitted,
 * when using the corresponding item. (Currently, it is 'escape smoke'.)
 */
class HomeMap {
public:
    HomeMap():
        _x_save_home_map_position(0),
        _y_save_home_map_position(0)
    {};

    //! \brief Set the new home to the given map.
    //!
    //! \param map_data_filename The map data filename to use.
    //! \param map_script_filename The map script filename to load.
    //! \param x_pos The character x position in the given map, or 0 for default.
    //! \param y_pos The character x position in the given map, or 0 for default.
    HomeMap(const std::string& map_data_filename,
            const std::string& map_script_filename,
            uint32_t x_pos, uint32_t y_pos);
    void SetHomeMap(const std::string& map_data_filename,
                    const std::string& map_script_filename,
                    uint32_t x_pos, uint32_t y_pos);

    //! \brief Reset the home map data to no map.
    void ClearHomeMap();

    //! \brief Returns whether a home map was set
    bool IsHomeMapSet() const;

    //! \brief Get Home Map corresponding data
    const std::string& GetMapDataFilename() const {
        return _home_map_data_filename;
    }
    const std::string& GetMapScriptFilename() const {
        return _home_map_script_filename;
    }
    uint32_t GetMapXPos() const {
        return _x_save_home_map_position;
    }
    uint32_t GetMapYPos() const {
        return _y_save_home_map_position;
    }

private:
    //! \brief The map data and script filename the current party is on.
    std::string _home_map_data_filename;
    std::string _home_map_script_filename;

    //! \brief last save point map tile location.
    uint32_t _x_save_home_map_position;
    uint32_t _y_save_home_map_position;
};

} // namespace vt_global

#endif // __HOME_MAP_HEADER__
