///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_tiles.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for map mode tile management.
*** ***************************************************************************/

// Allacrost engines
#include "engine/script/script.h"
#include "engine/video/video.h"

// Local map mode headers
#include "modes/map/map.h"
#include "modes/map/map_tiles.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_script;
using namespace hoa_video;

namespace hoa_map {

namespace private_map {

TileSupervisor::TileSupervisor() :
	_num_tile_on_x_axis(0),
	_num_tile_on_y_axis(0)
{}



TileSupervisor::~TileSupervisor() {
	// Delete all objects in _tile_images but *not* _animated_tile_images.
	// This is because _animated_tile_images is a subset of _tile_images.
	for (uint32 i = 0; i < _tile_images.size(); i++)
		delete(_tile_images[i]);

	_tile_grid.clear();
	_tile_images.clear();
	_animated_tile_images.clear();
}


LAYER_TYPE getLayerType(const std::string& type) {
	if (type == "ground")
		return GROUND_LAYER;
	else if (type == "sky")
		return SKY_LAYER;
	return INVALID_LAYER;
}


bool TileSupervisor::Load(ReadScriptDescriptor& map_file) {
	// Load the map dimensions and do some basic sanity checks
	_num_tile_on_y_axis = map_file.ReadInt("num_tile_rows");
	_num_tile_on_x_axis = map_file.ReadInt("num_tile_cols");

	vector<int32> context_inherits;
	//map_file.ReadUIntVector("context_inherits", context_inherits);
	map_file.OpenTable("contexts");
	uint32 num_contexts = map_file.GetTableSize();
	for (uint32 context_id = 0;  context_id < num_contexts; ++context_id) {
		map_file.OpenTable(context_id);
		int32 inheritance = map_file.ReadInt("inherit_from");

		// The base context can't inherit from another one
		if (context_id == 0)
			inheritance = -1;

		// A context can't inherit from a context with a higher id, or itself.
		if ((int32)context_id <= inheritance || inheritance < -1)
			inheritance = -1;

		context_inherits.push_back(inheritance);

		map_file.CloseTable();
	}
	map_file.CloseTable(); // contexts

	// Load all of the tileset images that are used by this map

	// Contains all of the tileset filenames used (string does not contain path information or file extensions)
	vector<string> tileset_filenames;
	// Temporarily retains all tile images loaded for each tileset. Each inner vector contains 256 StillImage objects
	vector<vector<StillImage> > tileset_images;

	map_file.ReadStringVector("tileset_filenames", tileset_filenames);

	for (uint32 i = 0; i < tileset_filenames.size(); i++) {
		// Construct the image filename from the tileset filename and create a new vector to use in the LoadMultiImage call
		string image_filename = "img/tilesets/" + tileset_filenames[i] + ".png";
		tileset_images.push_back(vector<StillImage>(TILES_PER_TILESET));

		// The map mode coordinate system used corresponds to a tile size of (2.0, 2.0)
		for (uint32 j = 0; j < TILES_PER_TILESET; j++) {
			tileset_images[i][j].SetDimensions(2.0f, 2.0f);
		}

		// Each tileset image is 512x512 pixels, yielding 16 * 16 (== 256) 32x32 pixel tiles each
		if (!ImageDescriptor::LoadMultiImageFromElementGrid(tileset_images[i], image_filename, 16, 16)) {
			PRINT_ERROR << "failed to load tileset image: " << image_filename << endl;
			return false;
		}
	}

	if (!map_file.DoesTableExist("layers")) {
		PRINT_ERROR << "No 'layers' table in the map file." << endl;
		return false;
	}

	// Read in the map tile indeces from all tile layers for the base context
	// The indeces stored for the map layers in this file directly correspond to a location within a tileset. Tilesets contain a total of 256 tiles
	// each, so 0-255 correspond to the first tileset, 256-511 the second, etc. The tile location within the tileset is also determined by the index,
	// where the first 16 indeces in the tileset range are the tiles of the first row (left to right), and so on.

	// Create the base context
	_tile_grid.clear();
	_tile_grid.insert(make_pair(MAP_CONTEXT_01, Context()));

	vector<int32> table_x_indeces; // Used to temporarily store a row of table indeces

	map_file.OpenTable("layers");

	uint32 layers_number = map_file.GetTableSize();

	// layers[0]-[n]
	for (uint32 layer_id = 0; layer_id < layers_number; ++layer_id) {
		// Opens the sub-table: layers[layer_id]
		if (!map_file.DoesTableExist(layer_id))
			continue;

		map_file.OpenTable(layer_id);

		// Add a layer for the base context
		_tile_grid[MAP_CONTEXT_01].resize(layer_id + 1);

		LAYER_TYPE layer_type = getLayerType(map_file.ReadString("type"));

		if (layer_type == INVALID_LAYER) {
			PRINT_WARNING << "Ignoring unexisting layer type: " << layer_type
				<< " in file: " << map_file.GetFilename() << endl;
			map_file.CloseTable(); // layers[i]
			continue;
		}

		_tile_grid[MAP_CONTEXT_01][layer_id].layer_type = layer_type;

		// Add the new tile rows (y axis)
		_tile_grid[MAP_CONTEXT_01][layer_id].tiles.resize(_num_tile_on_y_axis);

		// Read the tile data
		for (uint32 y = 0; y < _num_tile_on_y_axis; ++y) {
			table_x_indeces.clear();

			// Check to make sure tables are of the proper size
			if (!map_file.DoesTableExist(y)) {
				PRINT_ERROR << "the layers["<< layer_id <<"] table size was not equal to the number of tile rows specified by the map, "
					" first missing row: " << y << endl;
				return false;
			}

			map_file.ReadIntVector(y, table_x_indeces);

			// Check the number of columns
			if (table_x_indeces.size() != _num_tile_on_x_axis){
				PRINT_ERROR << "the layers[" << layer_id << "]["<< y << "] table size was not equal to the number of tile columns specified by the map, "
				"should have " << _num_tile_on_x_axis << " values."<< endl;
				return false;
			}

			// Prepare the columns (x axis)
			_tile_grid[MAP_CONTEXT_01][layer_id].tiles[y].resize(_num_tile_on_x_axis);

			for (uint32 x = 0; x < _num_tile_on_x_axis; ++x) {
				_tile_grid[MAP_CONTEXT_01][layer_id].tiles[y][x] = table_x_indeces[x];
			}
		}
		map_file.CloseTable(); // layers[layer_id]
	}

	map_file.CloseTable(); // layers

	// Create each additional context for the map by loading its table data

	// Load the tile data for each additional map context
	for (uint32 ctxt = 1; ctxt < num_contexts; ++ctxt) {
		MAP_CONTEXT this_context = static_cast<MAP_CONTEXT>(1 << ctxt);
		string context_name = "context_";
		if (ctxt < 10) // precede single digit context names with a zero
			context_name += "0";
		context_name += NumberToString(ctxt);

		// Check wether the context inhjeritance id is lower than the current one.
		if (context_inherits[ctxt] >= (int32)ctxt) {
			PRINT_WARNING << "Invalid context inheritance found for context id: " << ctxt
			<< ". Permitted values goes from -1 (none) to " << ctxt - 1 << endl;
			continue;
		}

		// Initialize this context by making a copy of the base map context first, as most contexts re-use many of the same tiles from the base context
		// If non-inheriting context, start with empty map!
		if (context_inherits[ctxt] > -1) {
			_tile_grid.insert(make_pair(this_context,
										_tile_grid[GetContextMaskFromConstextId(context_inherits[ctxt])]));
		}
		else {
			_tile_grid.insert(make_pair(this_context, Context()));

			// Resize the context to have the same size as the base one
			// The number of layers
			_tile_grid[this_context].resize(layers_number);
			// Fro each layer, set up the grid size
			for (uint32 layer_id = 0; layer_id < layers_number; ++layer_id) {
				// Type
				_tile_grid[this_context][layer_id].layer_type = _tile_grid[MAP_CONTEXT_01][layer_id].layer_type;
				// Height
				_tile_grid[this_context][layer_id].tiles.resize(_num_tile_on_y_axis);
				for (uint32 y = 0; y < _num_tile_on_y_axis; ++y) {
					// and width
					_tile_grid[this_context][layer_id].tiles[y].assign((size_t)_num_tile_on_x_axis, -1);
				}
			}
		}

		// Read the table corresponding to this context and modify each tile accordingly.
		// The context table is an array of integer data. The size of this array should be divisible by four, as every consecutive group of four integers in
		// this table represent one tile context element. The first integer corresponds to the tile layer (0 = lower, 1 = middle, 2 = upper), the second
		// and third represent the row and column of the tile respectively, and the fourth value indicates which tile image should be used for this context.
		// So if the first four entries in the context table were {0, 12, 26, 180}, this would set the lower layer tile at position (12, 26) to the tile
		// index 180.
		std::vector<int32> context_data;
		map_file.ReadIntVector(context_name, context_data);
		if (context_data.size() % 4 != 0) {
			PRINT_WARNING <<  ", context data was not evenly divisible by four (incomplete context data)"
				<< " in context: " << this_context << endl;
			continue;
		}

		for (uint32 j = 0; j < context_data.size(); j += 4) {
			int32 layer_id = context_data[j];
			int32 y = context_data[j + 1];
			int32 x = context_data[j + 2];
			int32 tile_id = context_data[j + 3];

			if (y >= _num_tile_on_y_axis ||
					x >= _num_tile_on_x_axis ||
					layer_id >= (int32)_tile_grid[this_context].size()) {
				PRINT_WARNING << "Invalid context data found for context: " << this_context << ": layer id: " << layer_id
					<< ", x: " << x << ", y: " << y << endl;
				continue;
			}

			_tile_grid[this_context][layer_id].tiles[y][x] = tile_id;
		}
	} // for (uint32 ctxt = 1; ctxt < map_instance->_num_map_contexts; ++ctxt)


	// Determine which tiles in each tileset are referenced in this map

	// Used to determine whether each tile is used by the map or not. An entry of -1 indicates that particular tile is not used
	vector<int16> tile_references;
	// Set size to be equal to the total number of tiles and initialize all entries to -1 (unreferenced)
	tile_references.assign(tileset_filenames.size() * TILES_PER_TILESET, -1);

	std::map<MAP_CONTEXT, Context>::const_iterator it = _tile_grid.begin();
	std::map<MAP_CONTEXT, Context>::const_iterator it_end = _tile_grid.end();
	// For each context
	for (; it != it_end; ++it) {
		// For each layer
		for (uint32 layer_id = 0; layer_id < layers_number; ++layer_id) {
			// For each tile id
			for (uint32 y = 0; y < _num_tile_on_y_axis; ++y) {
				for (uint32 x = 0; x < _num_tile_on_x_axis; ++x) {
					if ((it->second)[layer_id].tiles[y][x] >= 0)
						tile_references[ (it->second)[layer_id].tiles[y][x] ] = 0;
				}
			}
		}
	}

	// Translate the tileset tile indeces into indeces for the vector of tile images

	// Here, we have to convert the original tile indeces defined in the map file into a new form. The original index
	// indicates the tileset where the tile is used and its location in that tileset. We need to convert those indeces
	// so that they serve as an index to the MapMode::_tile_images vector, where the tile images will soon be stored.

	// Keeps track of the next translated index number to assign
	uint32 next_index = 0;

	for (uint32 i = 0; i < tile_references.size(); ++i) {
		if (tile_references[i] >= 0) {
			tile_references[i] = next_index;
			next_index++;
		}
	}

	// Now, go back and re-assign all tile layer indeces with the translated indeces
	std::map<MAP_CONTEXT, Context>::iterator it2 = _tile_grid.begin();
	std::map<MAP_CONTEXT, Context>::iterator it2_end = _tile_grid.end();
	// For each context
	for (; it2 != it2_end; ++it2) {
		// For each layer
		for (uint32 layer_id = 0; layer_id < layers_number; ++layer_id) {
			// For each tile id
			for (uint32 y = 0; y < _num_tile_on_y_axis; ++y) {
				for (uint32 x = 0; x < _num_tile_on_x_axis; ++x) {
					if ((it2->second)[layer_id].tiles[y][x] >= 0)
						(it2->second)[layer_id].tiles[y][x] = tile_references[ (it2->second)[layer_id].tiles[y][x] ];
				}
			}
		}
	}

	// Parse all of the tileset definition files and create any animated tile images that will be used

	// Used to access the tileset definition file
	ReadScriptDescriptor tileset_script;
	// Temporarily retains the animation data (every two elements corresponds to a pair of tile frame index and display time)
	vector<uint32> animation_info;
	// Temporarily holds all animated tile images. The map key is the value of the tile index, before reference translation is done in the next step
	map<uint32, AnimatedImage*> tile_animations;

	for (uint32 i = 0; i < tileset_filenames.size(); i++) {
		if (tileset_script.OpenFile("dat/tilesets/" + tileset_filenames[i] + ".lua") == false) {
			PRINT_ERROR << "map failed to load because it could not open a tileset definition file: " << tileset_script.GetFilename() << endl;
			exit(1);
		}
		tileset_script.OpenTable(tileset_filenames[i]);

		if (tileset_script.DoesTableExist("animated_tiles") == true) {
			tileset_script.OpenTable("animated_tiles");
			for (uint32 j = 1; j <= tileset_script.GetTableSize(); j++) {
				animation_info.clear();
				tileset_script.ReadUIntVector(j, animation_info);

				// The index of the first frame in the animation. (i * TILES_PER_TILESET) factors in which tileset the frame comes from
				uint32 first_frame_index = animation_info[0] + (i * TILES_PER_TILESET);

				// If the first tile frame index of this animation was not referenced anywhere in the map, then the animation is unused and
				// we can safely skip over it and move on to the next one. Otherwise if it is referenced, we have to construct the animated image
				if (tile_references[first_frame_index] == -1) {
					continue;
				}

				AnimatedImage* new_animation = new AnimatedImage();
				new_animation->SetDimensions(2.0f, 2.0f);

				// Each pair of entries in the animation info indicate the tile frame index (k) and the time (k+1)
				for (uint32 k = 0; k < animation_info.size(); k += 2) {
					new_animation->AddFrame(tileset_images[i][animation_info[k]], animation_info[k+1]);
				}
				tile_animations.insert(make_pair(first_frame_index, new_animation));
			}
			tileset_script.CloseTable();
		}

		tileset_script.CloseTable();
		tileset_script.CloseFile();
	} // for (uint32 i = 0; i < tileset_filenames.size(); i++)

	// Add all referenced tiles to the _tile_images vector, in the proper order

	for (uint32 i = 0; i < tileset_images.size(); i++) {
		for (uint32 j = 0; j < TILES_PER_TILESET; j++) {
			uint32 reference = (i * TILES_PER_TILESET) + j;

			if (tile_references[reference] >= 0) {
				// Add the tile as a StillImage
				if (tile_animations.find(reference) == tile_animations.end()) {
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

	if (tile_animations.empty() == false) {
		IF_PRINT_WARNING(MAP_DEBUG) << "one or more tile animations that were created were not added into the map -- this is a memory leak" << endl;
	}

	// Remove all tileset images. Any tiles which were not added to _tile_images will no longer exist in memory
	tileset_images.clear();

	return true;
} // bool TileSupervisor::Load(ReadScriptDescriptor& map_file)



void TileSupervisor::Update() {
	for (uint32 i = 0; i < _animated_tile_images.size(); i++) {
		_animated_tile_images[i]->Update();
	}
}


void TileSupervisor::DrawLayers(const MapFrame* frame, const LAYER_TYPE& layer_type) {
	MAP_CONTEXT context = MapMode::CurrentInstance()->GetCurrentContext();
	VideoManager->SetDrawFlags(VIDEO_BLEND, 0);

	std::map<MAP_CONTEXT, Context>::const_iterator it = _tile_grid.find(context);
	if (it == _tile_grid.end())
		return;
	const Context& layers = it->second;

	for (uint32 layer_id = 0; layer_id < layers.size(); ++layer_id) {

		const Layer& layer = layers[layer_id];
		if (layer.layer_type != layer_type)
			continue;

		VideoManager->Move(frame->tile_x_offset, frame->tile_y_offset);

		for (uint32 y = static_cast<uint32>(frame->tile_y_start);
			y < static_cast<uint32>(frame->tile_y_start + frame->num_draw_y_axis); ++y) {
			for (uint32 x = static_cast<uint32>(frame->tile_x_start);
					x < static_cast<uint32>(frame->tile_x_start + frame->num_draw_x_axis); ++x) {
				// Draw a tile image if it exists at this location
				if (layer.tiles[y][x] >= 0)
					_tile_images[ layer.tiles[y][x] ]->Draw();

				VideoManager->MoveRelative(2.0f, 0.0f);
			} // x
			VideoManager->MoveRelative(-static_cast<float>(frame->num_draw_x_axis * 2), 2.0f);
		} // y
	} // layer_id
}

} // namespace private_map

} // namespace hoa_map
