///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_tiles.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for map mode tile management.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/map/map_tiles.h"

#include "modes/map/map_mode.h"

#include "engine/video/video.h"

using namespace vt_utils;
using namespace vt_script;
using namespace vt_video;

namespace vt_map
{

namespace private_map
{

//! \brief A helper function to convert a string to a layer type.
static LAYER_TYPE StringToLayerType(const std::string& type)
{
    if(type == "ground")
        return GROUND_LAYER;
    else if(type == "sky")
        return SKY_LAYER;
    return INVALID_LAYER;
}

TileSupervisor::TileSupervisor() :
    _num_tile_on_x_axis(0),
    _num_tile_on_y_axis(0)
{
}

TileSupervisor::~TileSupervisor()
{
    // Delete all objects in _tile_images but *not* _animated_tile_images.
    // This is because _animated_tile_images is a subset of _tile_images.
    for(uint32_t i = 0; i < _tile_images.size(); i++)
        delete(_tile_images[i]);

    _tile_grid.clear();
    _tile_images.clear();
    _animated_tile_images.clear();
}

bool TileSupervisor::Load(ReadScriptDescriptor &map_file)
{
    // Load the map dimensions and do some basic sanity checks
    _num_tile_on_y_axis = map_file.ReadInt("num_tile_rows");
    _num_tile_on_x_axis = map_file.ReadInt("num_tile_cols");

    // Load all of the tileset images that are used by this map

    // Contains all of the tileset filenames used (string does not contain path information or file extensions)
    std::vector<std::string> tileset_filenames;
    // Temporarily retains all tile images loaded for each tileset. Each inner vector contains 256 StillImage objects
    std::vector<std::vector<StillImage> > tileset_images;

    map_file.ReadStringVector("tileset_filenames", tileset_filenames);

    for(uint32_t i = 0; i < tileset_filenames.size(); i++) {
        std::string tileset_file = tileset_filenames[i];

        ReadScriptDescriptor tileset_script;
        if (!tileset_script.OpenFile(tileset_file)) {
            PRINT_ERROR << "Couldn't open the tileset definition file: " << tileset_file << std::endl;
            return false;
        }

        if (!tileset_script.OpenTable("tileset")) {
            PRINT_ERROR << "Couldn't open the 'tileset' table from file: " << tileset_file << std::endl;
            tileset_script.CloseFile();
            return false;
        }

        std::string image_filename = tileset_script.ReadString("image");
        tileset_script.CloseFile();

        tileset_images.push_back(std::vector<StillImage>(TILES_PER_TILESET));

        // Each tileset image is 512x512 pixels, yielding 16 * 16 (== 256) 32x32 pixel tiles each
        if(!ImageDescriptor::LoadMultiImageFromElementGrid(tileset_images[i], image_filename, 16, 16)) {
            PRINT_ERROR << "failed to load tileset image: " << image_filename << std::endl;
            return false;
        }

        for(uint32_t j = 0; j < TILES_PER_TILESET; j++) {
            tileset_images[i][j].SetDimensions(TILE_LENGTH, TILE_LENGTH);
        }
    }

    if(!map_file.DoesTableExist("layers")) {
        PRINT_ERROR << "No 'layers' table in the map file." << std::endl;
        return false;
    }

    // Read in the map tile indeces from all tile layers.
    // The indeces stored for the map layers in this file directly correspond to a location within a tileset. Tilesets contain a total of 256 tiles
    // each, so 0-255 correspond to the first tileset, 256-511 the second, etc. The tile location within the tileset is also determined by the index,
    // where the first 16 indeces in the tileset range are the tiles of the first row (left to right), and so on.

    // Clears out the tiles grid
    _tile_grid.clear();

    std::vector<int32_t> table_x_indeces; // Used to temporarily store a row of table indeces

    map_file.OpenTable("layers");

    uint32_t layers_number = map_file.GetTableSize();

    // layers[0]-[n]
    for(uint32_t layer_id = 0; layer_id < layers_number; ++layer_id) {
        // Opens the sub-table: layers[layer_id]
        if(!map_file.DoesTableExist(layer_id))
            continue;

        map_file.OpenTable(layer_id);

        // Add a layer for the base context
        _tile_grid.resize(layer_id + 1);

        LAYER_TYPE layer_type = StringToLayerType(map_file.ReadString("type"));

        if(layer_type == INVALID_LAYER) {
            PRINT_WARNING << "Ignoring unexisting layer type: " << layer_type
                          << " in file: " << map_file.GetFilename() << std::endl;
            map_file.CloseTable(); // layers[i]
            continue;
        }

        _tile_grid[layer_id].layer_type = layer_type;

        // Add the new tile rows (y axis)
        _tile_grid[layer_id].tiles.resize(_num_tile_on_y_axis);

        // Read the tile data
        for(uint32_t y = 0; y < _num_tile_on_y_axis; ++y) {
            table_x_indeces.clear();

            // Check to make sure tables are of the proper size
            if(!map_file.DoesTableExist(y)) {
                PRINT_ERROR << "the layers[" << layer_id << "] table size was not equal to the number of tile rows specified by the map, "
                            " first missing row: " << y << std::endl;
                return false;
            }

            map_file.ReadIntVector(y, table_x_indeces);

            // Check the number of columns
            if(table_x_indeces.size() != _num_tile_on_x_axis) {
                PRINT_ERROR << "the layers[" << layer_id << "][" << y << "] table size was not equal to the number of tile columns specified by the map, "
                            "should have " << _num_tile_on_x_axis << " values." << std::endl;
                return false;
            }

            // Prepare the columns (x axis)
            _tile_grid[layer_id].tiles[y].resize(_num_tile_on_x_axis);

            for(uint32_t x = 0; x < _num_tile_on_x_axis; ++x) {
                _tile_grid[layer_id].tiles[y][x] = table_x_indeces[x];
            }
        }
        map_file.CloseTable(); // layers[layer_id]
    }

    map_file.CloseTable(); // layers

    // Determine which tiles in each tileset are referenced in this map

    // Used to determine whether each tile is used by the map or not. An entry of -1 indicates that particular tile is not used
    std::vector<int16_t> tile_references;
    // Set size to be equal to the total number of tiles and initialize all entries to -1 (unreferenced)
    tile_references.assign(tileset_filenames.size() * TILES_PER_TILESET, -1);

    // For each layer
    for(uint32_t layer_id = 0; layer_id < layers_number; ++layer_id) {
        // For each tile id
        for(uint32_t y = 0; y < _num_tile_on_y_axis; ++y) {
            for(uint32_t x = 0; x < _num_tile_on_x_axis; ++x) {
                if(_tile_grid[layer_id].tiles[y][x] >= 0)
                    tile_references[_tile_grid[layer_id].tiles[y][x] ] = 0;
            }
        }
    }

    // Translate the tileset tile indeces into indeces for the vector of tile images

    // Here, we have to convert the original tile indeces defined in the map file into a new form. The original index
    // indicates the tileset where the tile is used and its location in that tileset. We need to convert those indeces
    // so that they serve as an index to the MapMode::_tile_images vector, where the tile images will soon be stored.

    // Keeps track of the next translated index number to assign
    uint32_t next_index = 0;

    for(uint32_t i = 0; i < tile_references.size(); ++i) {
        if(tile_references[i] >= 0) {
            tile_references[i] = next_index;
            next_index++;
        }
    }

    // Now, go back and re-assign all tile layer indeces with the translated indeces
    // For each layer
    for(uint32_t layer_id = 0; layer_id < layers_number; ++layer_id) {
        // For each tile id
        for(uint32_t y = 0; y < _num_tile_on_y_axis; ++y) {
            for(uint32_t x = 0; x < _num_tile_on_x_axis; ++x) {
                if(_tile_grid[layer_id].tiles[y][x] >= 0)
                    _tile_grid[layer_id].tiles[y][x] = tile_references[_tile_grid[layer_id].tiles[y][x] ];
            }
        }
    }

    // Parse all of the tileset definition files and create any animated tile images that will be used

    // Used to access the tileset definition file
    ReadScriptDescriptor tileset_script;
    // Temporarily retains the animation data (every two elements corresponds to a pair of tile frame index and display time)
    std::vector<uint32_t> animation_info;
    // Temporarily holds all animated tile images. The map key is the value of the tile index, before reference translation is done in the next step
    std::map<uint32_t, AnimatedImage *> tile_animations;

    for(uint32_t i = 0; i < tileset_filenames.size(); i++) {
        if (!tileset_script.OpenFile(tileset_filenames[i])) {
            PRINT_ERROR << "map failed to load because it could not open a tileset definition file: "
                << tileset_filenames[i] << std::endl;
            return false;
        }

        if (!tileset_script.OpenTable("tileset")) {
            PRINT_ERROR << "map failed to load because it could not open the 'tileset' table from file: "
                << tileset_filenames[i] << std::endl;
            tileset_script.CloseFile();
            return false;
        }

        if(tileset_script.DoesTableExist("animated_tiles")) {
            tileset_script.OpenTable("animated_tiles");
            for(uint32_t j = 1; j <= tileset_script.GetTableSize(); j++) {
                animation_info.clear();
                tileset_script.ReadUIntVector(j, animation_info);

                // The index of the first frame in the animation. (i * TILES_PER_TILESET) factors in which tileset the frame comes from
                uint32_t first_frame_index = animation_info[0] + (i * TILES_PER_TILESET);

                // If the first tile frame index of this animation was not referenced anywhere in the map, then the animation is unused and
                // we can safely skip over it and move on to the next one. Otherwise if it is referenced, we have to construct the animated image
                if(tile_references[first_frame_index] == -1) {
                    continue;
                }

                AnimatedImage *new_animation = new AnimatedImage();
                new_animation->SetDimensions(TILE_LENGTH, TILE_LENGTH);

                // Each pair of entries in the animation info indicate the tile frame index (k) and the time (k+1)
                for(uint32_t k = 0; k < animation_info.size(); k += 2) {
                    new_animation->AddFrame(tileset_images[i][animation_info[k]], animation_info[k + 1]);
                }
                tile_animations.insert(std::make_pair(first_frame_index, new_animation));
            }
            tileset_script.CloseTable();
        }

        tileset_script.CloseTable();
        tileset_script.CloseFile();
    } // for (uint32_t i = 0; i < tileset_filenames.size(); i++)

    // Add all referenced tiles to the _tile_images vector, in the proper order

    for(uint32_t i = 0; i < tileset_images.size(); i++) {
        for(uint32_t j = 0; j < TILES_PER_TILESET; j++) {
            uint32_t reference = (i * TILES_PER_TILESET) + j;

            if(tile_references[reference] >= 0) {
                // Add the tile as a StillImage
                if(tile_animations.find(reference) == tile_animations.end()) {
                    _tile_images.push_back(new StillImage(tileset_images[i][j]));
                }

                // Add the tile as an AnimatedImage
                else {
                    _tile_images.push_back(tile_animations[reference]);
                    _animated_tile_images.push_back(tile_animations[reference]);
                    tile_animations.erase(reference);
                }
            }
        }
    }

    if(tile_animations.empty() == false) {
        IF_PRINT_WARNING(MAP_DEBUG) << "one or more tile animations that were created were not added into the map -- this is a memory leak" << std::endl;
    }

    // Remove all tileset images. Any tiles which were not added to _tile_images will no longer exist in memory
    tileset_images.clear();

    return true;
}

void TileSupervisor::Update()
{
    for(uint32_t i = 0; i < _animated_tile_images.size(); i++) {
        _animated_tile_images[i]->Update();
    }
}

void TileSupervisor::DrawLayers(const MapFrame *frame, const LAYER_TYPE &layer_type)
{
    // We'll use the top-left positions to render the tiles.
    VideoManager->SetDrawFlags(VIDEO_BLEND, VIDEO_X_LEFT, VIDEO_Y_TOP, 0);

    // Map frame ends
    int32_t y_end = static_cast<int32_t>(frame->tile_y_start + frame->num_draw_y_axis);
    int32_t x_end = static_cast<int32_t>(frame->tile_x_start + frame->num_draw_x_axis);

    int32_t layer_number = _tile_grid.size();
    for(int32_t layer_id = 0; layer_id < layer_number; ++layer_id) {

        const Layer &layer = _tile_grid.at(layer_id);
        if(layer.layer_type != layer_type)
            continue;

        //
        // TODO: TODO: Revert these over-draw changes once map dimensions
        //             are calculated more correctly for camera boundary cases.
        //

        // We substract 0.5 horizontally and 1.0 vertically here
        // because the video engine will display the map tiles using their
        // top left coordinates to avoid a position computation flaw when specifying the tile
        // coordinates from the bottom center point, as the engine does for everything else.
        VideoManager->Move(GRID_LENGTH * (frame->tile_x_offset - 1.0f), GRID_LENGTH * (frame->tile_y_offset - 2.0f));
        for(int32_t y = static_cast<int32_t>(frame->tile_y_start); y < y_end; ++y) {
            for(int32_t x = static_cast<int32_t>(frame->tile_x_start); x < x_end; ++x) {
                // Draw a tile image if it exists at this location
                if (x >= 0 &&
                    y >= 0 &&
                    layer.tiles.size() > y &&
                    layer.tiles.front().size() > x &&
                    layer.tiles[y][x] >= 0) {
                    _tile_images[layer.tiles[y][x]]->Draw();
                }

                VideoManager->MoveRelative(TILE_LENGTH, 0.0f);
            }
            VideoManager->MoveRelative(-static_cast<float>(frame->num_draw_x_axis) * TILE_LENGTH, TILE_LENGTH);
        }
    }

    // Restore the previous draw flags.
    VideoManager->SetDrawFlags(VIDEO_BLEND, VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
}

} // namespace private_map

} // namespace vt_map
