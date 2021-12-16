////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2021 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "world_map.h"

namespace vt_global {

void WorldMap::SetLocationVisible(const std::string& location_id, bool visible)
{
    auto it = _map_locations.find(location_id);
    if (it == _map_locations.end()) {
        return;
    }

    WorldMapLocation& location = it->second;
    location.SetVisible(visible);

    if (visible) {
        // Inserting on the same key to ensure no double insertion
        _visible_locations[location_id] = location;
    }
    else {
        // Removing the location if existing
        auto it_visible = _visible_locations.find(location_id);
        if (it_visible != _visible_locations.end()) {
            _visible_locations.erase(it_visible);
        }
    }
}

void WorldMap::ResetWorldLocationVisibleState() {
    for (auto it = _map_locations.begin(); it != _map_locations.end(); ++it) {
        it->second.SetVisible(false);
    }
    _visible_locations.clear();
}

} // namespace vt_global