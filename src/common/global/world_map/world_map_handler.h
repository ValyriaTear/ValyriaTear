////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_WORLD_MAP_HANDLER_HEADER__
#define __GLOBAL_WORLD_MAP_HANDLER_HEADER__

#include "world_map.h"

#include "script/script_read.h"
#include "script/script_write.h"
#include "engine/video/image.h"
#include "utils/ustring.h"

#include <string>
#include <vector>
#include <map>

namespace vt_global
{

class WorldMapHandler
{
public:
    WorldMapHandler():
        _current_world_map(nullptr)
    {};

    ~WorldMapHandler() {
        ClearUIData();
    };

    //! \brief Clears all world map UI related data
    void ClearUIData();

    //! \brief Clear all data from handler (for new game purpose)
    void ClearAllData();

    //! \brief Loads each world location from the script into the world location entry map
    //! \param file Path to the file to world locations script
    //! \return true if successfully loaded
    bool LoadScript(const std::string& world_map_filename);

    /** \brief Sets the current viewable world map taken from the config file.
    *** empty strings are valid, and will set an empty world map.
    **/
    void SetCurrentWorldMap(const std::string& world_map_id);

    //! \brief gets the current world map image
    //! \return a pointer to the currently viewable World Map Image.
    const vt_video::StillImage& GetWorldMapImage() const {
        return _current_world_map_image;
    }

    //! \brief Indicates whether the current world map has an instanced image
    bool HasWorldMapImage() const {
        return !_current_world_map_image.GetFilename().empty();
    }

    //! \brief Provides the available world locations of the current map
    const WorldMapLocations& GetAllWorldMapLocations() const;

    //! \brief Provides the visible world locations of the current map
    const WorldMapLocations& GetVisibleWorldMapLocations() const;

    /** \brief Gets a reference to the current world location id
    *** \return Reference to the current id. this value always exists, but could be "" if
    *** the location is not set, or if the world map is cleared
    *** id that was also set as the current location. the calling code should check for this
    **/
    const std::string& GetCurrentLocationId() const {
        return _current_world_location_id;
    }

    /** \brief Sets the current location id
    *** \param the location id of the world location that is defaulted to as "here"
    *** when the world map menu is opened
    **/
    void SetCurrentLocationId(const std::string& location_id) {
        _current_world_location_id = location_id;
    }

    /** \brief adds a viewable location string id to the currently viewable
    *** set. This string IDs are maintained in the data/config/world_location.lua file.
    *** \param the string id to the currently viewable location
    *** \param visible Whether the location can be seen by the player
    **/
    void SetWorldLocationVisible(const std::string& location_id, bool visible);

    //! \brief Load world map and viewable information from the save game
    //! \param file Reference to an open file for reading save game data
    void LoadPlayerSaveGameWorldMap(vt_script::ReadScriptDescriptor& file);

    //! \brief Saves the current world map information. this is called from SaveGame()
    //! \param file Reference to open and valid file for writting the data
    void SavePlayerSaveGameWorldMap(vt_script::WriteScriptDescriptor& file);

private:
    //! \brief The container which stores all the available world maps information
    std::map<std::string, WorldMap> _world_map_info;

    //! \brief The current world map id that indicates where the player is
    std::string _current_world_map_id;

    //! \brief The current world map location id
    //! that indicates where the player is on the currently selected world map
    std::string _current_world_location_id;

    //! \brief the current world map image
    vt_video::StillImage _current_world_map_image;

    //! \brief Pointer to the currently used world map data
    WorldMap* _current_world_map;

    //! \brief default empty world map
    WorldMap defaultWorldMap;
};

} // namespace vt_global

#endif // __GLOBAL_WORLD_MAP_HANDLER_HEADER__
