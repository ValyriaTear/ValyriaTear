////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __WORLDMAP_LOCATION_HEADER__
#define __WORLDMAP_LOCATION_HEADER__

#include "engine/video/image.h"

namespace vt_global {

/** *****************************************************************************
*** \brief Struct for world map locations
*** the parameters are all immutable and loaded at creation time
*** there should be no reason for these to be created outside the global manager
*** the key is the unique location id set in the script as a string
*** *****************************************************************************/
class WorldMapLocation
{
public:
    WorldMapLocation():
        _pos(0.0f, 0.0f),
        _visible(false)
    {}

    WorldMapLocation(float x, float y, const std::string& location_name,
                     const std::string& image_path, const std::string& world_map_location_id);

    WorldMapLocation(const WorldMapLocation& other):
        _world_map_location_id(other._world_map_location_id),
        _pos(other._pos),
        _location_name(other._location_name),
        _image(other._image),
        _visible(false)
    {}

    WorldMapLocation& operator=(const WorldMapLocation& other)
    {
        if(this == &other)
            return *this;
        _pos = other._pos;
        _location_name = other._location_name;
        _world_map_location_id = other._world_map_location_id;
        _image = other._image;
        _visible = other._visible;
        return *this;
    }

    ~WorldMapLocation() {
        _image.Clear();
    }

    const vt_common::Position2D& GetPosition() const {
      return _pos;
    }

    //! \brief The unique location id
    std::string _world_map_location_id;

    //! \brief The marker location on the worldmap image
    vt_common::Position2D _pos;

    //! \brief The translated location name
    std::string _location_name;

    //! \brief the marker image
    vt_video::StillImage _image;

    //! \brief Whether the player can see the location on the worldmap
    bool _visible;
};

} // namespace vt_global

#endif // __WORLDMAP_LOCATION_HEADER__
