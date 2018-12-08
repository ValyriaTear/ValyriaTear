////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "map_data_handler.h"

#include "utils/utils_common.h"

using namespace vt_utils;

namespace vt_global
{

MapDataHandler::MapDataHandler() :
    _x_save_map_position(0),
    _y_save_map_position(0),
    _save_stamina(0),
    _same_map_hud_name_as_previous(false),
    _show_minimap(true)
{
}

MapDataHandler::~MapDataHandler()
{
    Clear();
}

void MapDataHandler::Clear()
{
    UnsetSaveData();

    // Clear out the map previous location
    _previous_location.clear();
    _map_data_filename.clear();
    _map_script_filename.clear();
    _map_hud_name.clear();

    // Show the minimap by default when available
    _show_minimap = true;

    ClearHomeMap();
}

bool MapDataHandler::Load(vt_script::ReadScriptDescriptor& file)
{
    if(!file.IsFileOpen()) {
        return false;
    }

    Clear();

    _map_data_filename = file.ReadString("map_data_filename");
    _map_script_filename = file.ReadString("map_script_filename");

    // Loads saved position, if any
    _x_save_map_position = file.ReadUInt("location_x");
    _y_save_map_position = file.ReadUInt("location_y");

    _save_stamina = file.ReadUInt("stamina");

    // Load home map data, if any
    if (file.OpenTable("home_map")) {
        std::string home_map_data = file.ReadString("map_data_filename");
        std::string home_map_script = file.ReadString("map_script_filename");
        uint32_t x_pos = file.ReadUInt("location_x");
        uint32_t y_pos = file.ReadUInt("location_y");

        _home_map = vt_map::MapLocation(home_map_data,
                                        home_map_script,
                                        x_pos, y_pos);

        file.CloseTable(); // home_map
    }

    return true;
}

bool MapDataHandler::Save(vt_script::WriteScriptDescriptor& file,
                          uint32_t x_position,
                          uint32_t y_position)
{
    if(!file.IsFileOpen()) {
        return false;
    }

    file.InsertNewLine();
    file.WriteLine("map_data_filename = \"" + _map_data_filename + "\",");
    file.WriteLine("map_script_filename = \"" + _map_script_filename + "\",");
    //! \note Coords are in map tiles
    file.WriteLine("location_x = " + NumberToString(x_position) + ",");
    file.WriteLine("location_y = " + NumberToString(y_position) + ",");
    file.WriteLine("stamina = " + NumberToString(_save_stamina) + ",");

    // Save latest home map data, if any.
    if (_home_map.IsValid()) {
        file.InsertNewLine();
        file.WriteLine("home_map = {");
        file.WriteLine("\tmap_data_filename = \"" + _home_map.GetMapDataFilename() + "\",");
        file.WriteLine("\tmap_script_filename = \"" + _home_map.GetMapDataFilename() + "\",");
        //! \note Coords are in map tiles
        file.WriteLine("\tlocation_x = " + NumberToString(_home_map.GetMapPosition().x) + ",");
        file.WriteLine("\tlocation_y = " + NumberToString(_home_map.GetMapPosition().y) + ",");
        file.WriteLine("},");
    }
    return true;
}

void MapDataHandler::SetMap(const std::string &map_data_filename,
                            const std::string &map_script_filename,
                            const std::string &map_image_filename,
                            const vt_utils::ustring &map_hud_name)
{
    _map_data_filename = map_data_filename;
    _map_script_filename = map_script_filename;

    if(!_map_image.Load(map_image_filename))
        PRINT_WARNING << "failed to load map image: " << map_image_filename << std::endl;

    // Updates the map hud names info.
    _previous_map_hud_name = _map_hud_name;
    _map_hud_name = map_hud_name;
    _same_map_hud_name_as_previous = (MakeStandardString(_previous_map_hud_name) == MakeStandardString(_map_hud_name));
}

} // namespace vt_global
