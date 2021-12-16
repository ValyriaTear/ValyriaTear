////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __WORLD_MAP_LOCATION_HEADER__
#define __WORLD_MAP_LOCATION_HEADER__

#include "common/position_2d.h"
#include "utils/ustring.h"

namespace vt_global {

/** *****************************************************************************
*** \brief world map locations class used to define its interactable elements
*** *****************************************************************************/
class WorldMapLocation
{
public:
    WorldMapLocation():
        _pos(0.0f, 0.0f),
        _visible(false)
    {}

    WorldMapLocation(float x, float y,
                     const vt_utils::ustring& location_name,
                     const std::string& image_filename,
                     bool visible = false):
        _pos(x, y),
        _location_name(location_name),
        _location_image_filename(image_filename),
        _visible(visible)
    {
    }

    WorldMapLocation(const WorldMapLocation& other):
        _pos(other._pos),
        _location_name(other._location_name),
        _location_image_filename(other._location_image_filename),
        _visible(other._visible)
    {}

    WorldMapLocation& operator=(const WorldMapLocation& other)
    {
        if(this == &other)
            return *this;

        _world_map_location_id = other._world_map_location_id;
        _pos = other._pos;
        _location_name = other._location_name;
        _location_image_filename = other._location_image_filename;
        _visible = other._visible;
        return *this;
    }

    ~WorldMapLocation() {
    }

    const vt_common::Position2D& GetPosition() const {
      return _pos;
    }

    const vt_utils::ustring& GetLocationName() const {
        return _location_name;
    }

    const std::string& GetLocationImageFileName() const {
        return _location_image_filename;
    }

    bool IsVisible() const {
        return _visible;
    }

    void SetVisible(bool visible) {
        _visible = visible;
    }

private:
    //! \brief The unique location id
    std::string _world_map_location_id;

    //! \brief The marker location on the world map image
    vt_common::Position2D _pos;

    //! \brief The translated location name
    vt_utils::ustring _location_name;

    //! \brief The location banner image filename
    std::string _location_image_filename;

    //! \brief Whether the player can see the location on the world map
    bool _visible;
};

} // namespace vt_global

#endif // __WORLD_MAP_LOCATION_HEADER__
