///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_LOCATION_HEADER__
#define __MAP_LOCATION_HEADER__

#include "modes/map/map_utils.h"

namespace vt_map
{

/** ****************************************************************************
*** \brief Indicates the location in tiles in a specific map file
*** (tiles + script definition.
*** ***************************************************************************/
class MapLocation {
public:
    MapLocation()
    {}

    //! \brief A specific map location.
    //! \param map_data_filename The map tile definition filename.
    //! \param map_script_filename The map script filename to load.
    //! \param map_position The map position to set the character location to
    //! when loading the map.
    MapLocation(const std::string& map_data_filename,
                const std::string& map_script_filename,
                const vt_common::Position2D& map_position):
        _map_data_filename(map_data_filename),
        _map_script_filename(map_script_filename),
        _map_position(map_position)
    {}

    MapLocation(const std::string& map_data_filename,
                const std::string& map_script_filename,
                float x, float y):
        _map_data_filename(map_data_filename),
        _map_script_filename(map_script_filename),
        _map_position(vt_common::Position2D(x, y))
    {}

    //! \brief Returns whether the current location is valid.
    //! N.B: It checks neither filename validity nor position.
    //! As this is the goal of the map mode to do it,
    //! and position validity may depend on the specific script.
    bool IsValid() const {
        return !_map_data_filename.empty()
            && !_map_script_filename.empty();
    }

    const std::string& GetMapDataFilename() const {
        return _map_data_filename;
    }

    const std::string& GetMapScriptFilename() const {
        return _map_script_filename;
    }

    //! \brief Returns the map position in tiles.
    const vt_common::Position2D& GetMapPosition() const {
        return _map_position;
    }

    //! \brief Clears map location data. Makes it invalid.
    void Clear() {
        _map_data_filename.clear();
        _map_script_filename.clear();
        _map_position.x = -1.0f;
        _map_position.y = -1.0f;
    }

private:
    //! \brief The map filenames
    std::string _map_data_filename;
    std::string _map_script_filename;

    //! \brief The position inside the given map in tiles
    vt_common::Position2D _map_position;
};

} // namespace vt_map

#endif // __MAP_LOCATION_HEADER__
