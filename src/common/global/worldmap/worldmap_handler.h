////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_WORLDMAP_HEADER__
#define __GLOBAL_WORLDMAP_HEADER__

#include "worldmap_location.h"

#include "script/script_read.h"
#include "script/script_write.h"

#include <string>
#include <vector>
#include <map>

namespace vt_global
{

class WorldMapHandler
{
public:
    WorldMapHandler();
    ~WorldMapHandler();

    //! \brief Loads each world location from the script into the world location entry map
    //! \param file Path to the file to world locations script
    //! \return true if successfully loaded
    bool LoadScript(const std::string& world_locations_filename);

    //! \brief Clear worldmap image
    void ClearWorldMapImage();

    //! \brief gets the current world map image
    //! \return a pointer to the currently viewable World Map Image.
    //! \note returns nullptr if the filename has been set to ""
    vt_video::StillImage* GetWorldMapImage() const {
        return _world_map_image;
    }

    const std::string& GetWorldMapImageFilename() const;

    /** \brief sets the current viewable world map
    *** empty strings are valid, and will cause the return
    *** of a null pointer on GetWorldMap call.
    *** \note this will also clear the currently viewable locations and the current location id
    **/
    void SetWorldMapImage(const std::string& world_map_filename);

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

    /** \brief gets a reference to the worldmap location data
    *** \return reference to the current worldmap location data
    **/
    const std::map<std::string, WorldMapLocation>& GetWorldMapLocations() const {
        return _world_map_locations;
    }

    //! \brief Load world map and viewable information from the save game
    //! \param file Reference to an open file for reading save game data
    void LoadWorldMap(vt_script::ReadScriptDescriptor& file);

    //! \brief saves the world map information. this is called from SaveGame()
    //! \param file Reference to open and valid file for writting the data
    void SaveWorldMap(vt_script::WriteScriptDescriptor& file);

private:
    //! \brief The current graphical world map. If the filename is empty,
    //! then we are "hiding" the map
    vt_video::StillImage* _world_map_image;

    //! \brief The current viewable location ids on the current world map image
    //! \note this list is cleared when we call SetWorldMap. It is up to the
    //! script writter to maintain the properties of the map by either
    //!  1) call CopyViewableLocationList()
    //!  2) maintain in some other fashion the list
    std::vector<std::string> _viewable_world_locations;

    /** \brief the container which stores all the available world locations in the game.
    *** the world_location_id acts as the key
    **/
    std::map<std::string, WorldMapLocation> _world_map_locations;

    //! \brief the current world map location id that indicates where the player is
    std::string _current_world_location_id;
};

} // namespace vt_global

#endif // __GLOBAL_WORLDMAP_HEADER__
