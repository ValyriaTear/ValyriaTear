////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __WORLDMAP_HEADER__
#define __WORLDMAP_HEADER__

#include "engine/video/image.h"

namespace vt_global {

/** *****************************************************************************
*** \brief Struct for world map locations
*** the parameters are all immutable and loaded at creation time
*** there should be no reason for these to be created outside the global manager
*** the key is the unique location id set in the script as a string
*** there is no need for accesor functions because this is just a storage struct
*** *****************************************************************************/
class WorldMapLocation
{
public:
    WorldMapLocation():
        _x(0.0f),
        _y(0.0f)
    {}

    WorldMapLocation(float x, float y, const std::string& location_name,
                     const std::string& image_path, const std::string& world_map_location_id);

    WorldMapLocation(const WorldMapLocation& other):
        _x(other._x),
        _y(other._y),
        _location_name(other._location_name),
        _world_map_location_id(other._world_map_location_id),
        _image(other._image)
    {}

    WorldMapLocation &operator=(const WorldMapLocation& other)
    {
        if(this == &other)
            return *this;
        _x = other._x;
        _y = other._y;
        _location_name = other._location_name;
        _world_map_location_id = other._world_map_location_id;
        _image = other._image;
        return *this;
    }

    ~WorldMapLocation() {
        _image.Clear();
    }

    float _x;
    float _y;
    std::string _location_name;
    std::string _world_map_location_id;
    vt_video::StillImage _image;
};

} // namespace vt_global

#endif // WORLDMAP_HEADER
