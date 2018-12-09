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

void WorldMapHandler::ShowWorldLocation(const std::string& location_id)
{
    // Defensive check. do not allow blank ids.
    // If you want to remove an id, call HideWorldLocation
    if(location_id.empty())
        return;
    // Check to make sure this location isn't already visible
    if(std::find(_viewable_world_locations.begin(),
                 _viewable_world_locations.end(),
                 location_id) == _viewable_world_locations.end())
    {
        _viewable_world_locations.push_back(location_id);
    }
}

void WorldMapHandler::HideWorldLocation(const std::string &location_id)
{
    auto rem_iterator = std::find(_viewable_world_locations.begin(),
                                  _viewable_world_locations.end(),
                                  location_id);
    if(rem_iterator != _viewable_world_locations.end())
        _viewable_world_locations.erase((rem_iterator));
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
    for(uint32_t i = 0; i < location_ids.size(); ++i)
        ShowWorldLocation(location_ids[i]);

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
    for(uint32_t i = 0; i < _viewable_world_locations.size(); ++i)
        file.WriteLine("\t\t\"" + _viewable_world_locations[i]+"\",");
    file.WriteLine("\t},");
    file.InsertNewLine();

    file.WriteLine("\tcurrent_location = \"" + GetCurrentLocationId() + "\"");

    file.WriteLine("},"); // close the main table
    file.InsertNewLine();
}

} // namespace vt_global
