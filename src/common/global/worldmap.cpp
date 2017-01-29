////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "worldmap.h"

namespace vt_global {

WorldMapLocation::WorldMapLocation(float x, float y, const std::string& location_name,
                                   const std::string& image_path, const std::string& world_map_location_id) :
    _x(x),
    _y(y),
    _location_name(location_name),
    _world_map_location_id(world_map_location_id)
{
    if(!_image.Load(image_path))
        PRINT_ERROR << "image: " << image_path << " not able to load" << std::endl;
}

} // namespace vt_global
