////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "home_map.h"

namespace vt_global {

HomeMap::HomeMap(const std::string& map_data_filename,
                 const std::string& map_script_filename,
                 uint32_t x_pos, uint32_t y_pos):
                 _home_map_data_filename(map_data_filename),
                 _home_map_script_filename(map_script_filename),
                 _x_save_home_map_position(x_pos),
                 _y_save_home_map_position(y_pos)
{
}

void HomeMap::SetHomeMap(const std::string& map_data_filename,
                         const std::string& map_script_filename,
                         uint32_t x_pos, uint32_t y_pos) {
    _home_map_data_filename = map_data_filename;
    _home_map_script_filename = map_script_filename;
    _x_save_home_map_position = x_pos;
    _y_save_home_map_position = y_pos;
}

void HomeMap::ClearHomeMap() {
    _home_map_data_filename.clear();
    _home_map_script_filename.clear();
    _x_save_home_map_position = 0;
    _y_save_home_map_position = 0;
}

bool HomeMap::IsHomeMapSet() const {
    if (_home_map_data_filename.empty())
        return false;

    if (_home_map_data_filename.empty())
        return false;
    return true;
}

} // namespace vt_global
