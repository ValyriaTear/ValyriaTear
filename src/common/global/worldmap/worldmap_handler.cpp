////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "worldmap_handler.h"

namespace vt_global
{

WorldMapHandler::WorldMapHandler() :
    _world_map_image(nullptr)
{
}

WorldMapHandler::~WorldMapHandler()
{
    ClearWorldMapImage();
}

bool WorldMapHandler::LoadScript(const std::string& world_locations_filename)
{
    _world_map_locations.clear();

    vt_script::ReadScriptDescriptor world_locations_script;
    if(!world_locations_script.OpenFile(world_locations_filename)) {
        PRINT_ERROR << "Couldn't open world map locations file: " << world_locations_filename << std::endl;
        return false;
    }

    if(!world_locations_script.DoesTableExist("world_locations"))
    {
        PRINT_ERROR << "No 'world_locations' table in file: " << world_locations_filename << std::endl;
        world_locations_script.CloseFile();
        return false;
    }

    std::vector<std::string> world_location_ids;
    world_locations_script.ReadTableKeys("world_locations", world_location_ids);
    if(world_location_ids.empty())
    {
        PRINT_ERROR << "No items in 'world_locations' table in file: " << world_locations_filename << std::endl;
        world_locations_script.CloseFile();
        return false;
    }

    world_locations_script.OpenTable("world_locations");
    for(uint32_t i = 0; i < world_location_ids.size(); ++i)
    {
        const std::string &id = world_location_ids[i];
        std::vector<std::string> values;
        world_locations_script.ReadStringVector(id,values);

        //check for existing location
        if(_world_map_locations.find(id) != _world_map_locations.end())
        {
            PRINT_WARNING << "duplicate world map location id found: " << id << std::endl;
            continue;
        }

        float x = atof(values[0].c_str());
        float y = atof(values[1].c_str());
        const std::string &location_name = values[2];
        const std::string &image_path = values[3];
        WorldMapLocation location(x, y, location_name, image_path, id);

        _world_map_locations[id] = location;
    }
    world_locations_script.CloseFile();

    return true;
}

void WorldMapHandler::ClearWorldMapImage()
{
    // Clear global world map file
    if (_world_map_image) {
        delete _world_map_image;
        _world_map_image = nullptr;
    }
}

const std::string& WorldMapHandler::GetWorldMapImageFilename() const
{
    if (_world_map_image)
        return _world_map_image->GetFilename();
    else
        return vt_utils::_empty_string;
}

void WorldMapHandler::SetWorldMapImage(const std::string& world_map_filename)
{
    ClearWorldMapImage();

    _viewable_world_locations.clear();
    _current_world_location_id.clear();
    _world_map_image = new vt_video::StillImage();
    _world_map_image->Load(world_map_filename);
}

void WorldMapHandler::SetWorldLocationVisible(const std::string& location_id, bool visible)
{
    if(location_id.empty())
        return;

    auto location_iter = _world_map_locations.find(location_id);
    if(location_iter == _world_map_locations.end()) {
      PRINT_WARNING << "Couldn't find map location to show: " << location_id << std::endl;
      return;
    }

    location_iter->second._visible = visible;
}

void WorldMapHandler::LoadWorldMap(vt_script::ReadScriptDescriptor& file)
{
    if(file.IsFileOpen() == false) {
        PRINT_WARNING << "the file provided in the function argument was not open" << std::endl;
        return;
    }

    std::string world_map = file.ReadString("world_map_file");

    SetWorldMapImage(world_map);

    std::vector<std::string> location_ids;
    file.ReadStringVector("viewable_locations", location_ids);
    for(uint32_t i = 0; i < location_ids.size(); ++i) {
        SetWorldLocationVisible(location_ids[i], true);
    }
    std::string current_location = file.ReadString("current_location");
    if (!current_location.empty())
        SetCurrentLocationId(current_location);

    file.CloseTable(); // worldmap
}

void WorldMapHandler::SaveWorldMap(vt_script::WriteScriptDescriptor& file)
{
    if(!file.IsFileOpen()) {
        PRINT_WARNING << "The file provided in the function argument was not open" << std::endl;
        return;
    }

    // Write the 'worldmap' table
    file.WriteLine("worldmap = {");

    // Write the world map filename
    file.WriteLine("\tworld_map_file = \"" + GetWorldMapImageFilename() + "\",");
    file.InsertNewLine();

    // Write the viewable locations
    file.WriteLine("\tviewable_locations = {");
    for(auto iter = _world_map_locations.begin(); iter != _world_map_locations.end(); ++iter) {
        if (iter->second._visible)
            file.WriteLine("\t\t\"" + iter->second._world_map_location_id + "\",");
    }
    file.WriteLine("\t},");
    file.InsertNewLine();

    file.WriteLine("\tcurrent_location = \"" + GetCurrentLocationId() + "\"");

    file.WriteLine("},"); // close the main table
    file.InsertNewLine();
}

} // namespace vt_global
