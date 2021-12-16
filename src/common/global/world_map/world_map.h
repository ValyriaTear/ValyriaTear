////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2021 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __WORLD_MAP_HEADER__
#define __WORLD_MAP_HEADER__

#include "world_map_location.h"
#include <map>

namespace vt_global {

typedef std::map<std::string, WorldMapLocation> WorldMapLocations;

//! \brief World map related information
class WorldMap {
public:
    WorldMap() {}
    ~WorldMap() {}

    //! \brief Sets the world map image filename
    void SetWorldMapImageFilename(const std::string& image_filename) {
        _world_map_image_filename = image_filename;
    }

    //! \brief Adds world map location information to the map
    void AddWorldMapLocation(const std::string& id,
                             const WorldMapLocation& map_location) {
        if (!id.empty()) {
            _map_locations[id] = map_location;
        }
    }

    //! \brief Set the given location id as visible if found.
    void SetLocationVisible(const std::string& location_id, bool visible);

    //! \brief Returns the current world map image filename
    const std::string& GetWorldMapImageFilename() const {
        return _world_map_image_filename;
    }

    //! \brief Returns all the map locations if any
    const WorldMapLocations& GetAllWorldMapLocations() const {
        return _map_locations;
    }

    //! \brief Returns the visible map location if any
    const WorldMapLocations& GetVisibleWorldMapLocations() const {
        return _visible_locations;
    }

    //! \brief Resets the world map location visible states to not visible
    //! For new game purpose for instance.
    void ResetWorldLocationVisibleState();

private:
    //! \brief The world map visible image
    std::string _world_map_image_filename;

    //! \brief The map available locations
    WorldMapLocations _map_locations;

    //! \brief The visible locations, updated only when requested
    WorldMapLocations _visible_locations;
};

} // namespace vt_global

#endif // __WORLD_MAP_HEADER__
