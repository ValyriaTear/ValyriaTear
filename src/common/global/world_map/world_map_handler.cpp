////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "world_map_handler.h"

namespace vt_global
{

bool WorldMapHandler::LoadScript(const std::string& world_maps_filename)
{
    _world_map_info.clear();

    vt_script::ReadScriptDescriptor script;
    if(!script.OpenFile(world_maps_filename)) {
        PRINT_ERROR << "Couldn't open world map file: " << world_maps_filename << std::endl;
        return false;
    }

    if (!script.DoesTableExist("world_maps")) {
        PRINT_ERROR << "No 'world_maps' table in file: " << world_maps_filename << std::endl;
        script.CloseFile();
        return false;
    }

    std::vector<std::string> world_map_ids;
    script.ReadTableKeys("world_maps", world_map_ids);

    script.OpenTable("world_maps");
    for(const std::string& world_map_id : world_map_ids) {

        if (!script.OpenTable(world_map_id)) {
            PRINT_WARNING << "Could open world map '" << world_map_id << "' in file "
                << "' in file " << world_maps_filename << std::endl;
            continue;
        }

        std::string world_map_image = script.ReadString("image");
        if (world_map_image.empty()) {
          PRINT_WARNING << "No world map image for world map id: '" << world_map_id
              << "' in file " << world_maps_filename << std::endl;
        }

        WorldMap world_map = WorldMap();
        world_map.SetWorldMapImageFilename(world_map_image);

        if(!script.DoesTableExist("map_locations")) {
            continue;
        }

        std::vector<std::string> map_location_ids;
        script.ReadTableKeys("map_locations", map_location_ids);

        script.OpenTable("map_locations");
        for(const std::string& location_id : map_location_ids) {
            std::vector<std::string> values;
            script.ReadStringVector(location_id, values);

            float x = atof(values[0].c_str());
            float y = atof(values[1].c_str());
            const vt_utils::ustring& location_name = vt_utils::MakeUnicodeString(values[2]);
            const std::string& image_path = values[3];

            world_map.AddWorldMapLocation(
                    location_id,
                    WorldMapLocation(location_id, x, y, location_name, image_path));
        }
        script.CloseTable(); // map_locations

        script.CloseTable(); // world_map_id

        _world_map_info[world_map_id] = world_map;
    }
    script.CloseTable(); // world_maps

    script.CloseFile();
    return true;
}

void WorldMapHandler::ClearUIData() {
    _current_world_map_image.Clear();
    _current_world_map = nullptr;
}

void WorldMapHandler::ClearAllData() {
    ClearUIData();

    // Reset the world map data to default
    _current_world_map_id.clear();
    _current_world_location_id.clear();

    for (auto it = _world_map_info.begin(); it != _world_map_info.end(); ++it) {
        it->second.ResetWorldLocationVisibleState();
    }
}

const WorldMapLocations& WorldMapHandler::GetAllWorldMapLocations() const {
    if (_current_world_map) {
        return _current_world_map->GetAllWorldMapLocations();
    }
    return defaultWorldMap.GetAllWorldMapLocations();
}

const WorldMapLocations& WorldMapHandler::GetVisibleWorldMapLocations() const {
    if (_current_world_map) {
        return _current_world_map->GetVisibleWorldMapLocations();
    }
    return defaultWorldMap.GetVisibleWorldMapLocations();
}

void WorldMapHandler::SetCurrentWorldMap(const std::string& world_map_id)
{
    ClearUIData();
    _current_world_location_id.clear();

    _current_world_map_id = world_map_id;

    // Hide everything when it is requested.
    if (world_map_id.empty()) {
        _current_world_map = nullptr;
        return;
    }

    auto it = _world_map_info.find(world_map_id);
    if (it == _world_map_info.end()) {
        PRINT_WARNING << "Couldn't find world map to show: " << world_map_id << std::endl;
        return;
    }

    // Load all data needed by the UI for the new current map
    _current_world_map = &it->second;
    _current_world_map_image.Load(_current_world_map->GetWorldMapImageFilename());
}

void WorldMapHandler::SetWorldLocationVisible(const std::string& location_id, bool visible)
{
    if(location_id.empty()) {
        return;
    }

    if (_current_world_map) {
        _current_world_map->SetLocationVisible(location_id, visible);
    }
}

void WorldMapHandler::LoadPlayerSaveGameWorldMap(vt_script::ReadScriptDescriptor& file)
{
    if(file.IsFileOpen() == false) {
        PRINT_WARNING << "the file provided in the function argument was not open" << std::endl;
        return;
    }

    if (!file.OpenTable("world_map")) {
        PRINT_WARNING << "The save game file doesn't provide world map information" << std::endl;
        return;
    }

    std::string world_map_id = file.ReadString("world_map_id");

    SetCurrentWorldMap(world_map_id);

    std::vector<std::string> location_ids;
    file.ReadStringVector("viewable_locations", location_ids);
    for(const std::string& location_id : location_ids) {
        SetWorldLocationVisible(location_id, true);
    }
    std::string current_location = file.ReadString("current_location");
    if (!current_location.empty())
        SetCurrentLocationId(current_location);

    file.CloseTable(); // world_map
}

void WorldMapHandler::SavePlayerSaveGameWorldMap(vt_script::WriteScriptDescriptor& file)
{
    if(!file.IsFileOpen()) {
        PRINT_WARNING << "The file provided in the function argument was not open" << std::endl;
        return;
    }

    // Write the 'world_map' table
    file.WriteLine("world_map = {");

    // Write the world map filename
    file.WriteLine("\tworld_map_id = \"" + _current_world_map_id + "\",");
    file.InsertNewLine();

    // Write the viewable locations
    file.WriteLine("\tviewable_locations = {");
    if (_current_world_map) {
        auto world_map_locations = _current_world_map->GetVisibleWorldMapLocations();
        for(auto iter = world_map_locations.begin(); iter != world_map_locations.end(); ++iter) {
            const std::string& location_id = iter->first;
            file.WriteLine("\t\t\"" + location_id + "\",");
        }
    }
    file.WriteLine("\t},");
    file.InsertNewLine();

    file.WriteLine("\tcurrent_location = \"" + GetCurrentLocationId() + "\"");

    file.WriteLine("},"); // close the main table
    file.InsertNewLine();
}

} // namespace vt_global
