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
#include "script.h"
#include "video.h"

// Local map mode headers
#include "map.h"
#include "map_tiles.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_script;
using namespace hoa_video;

namespace hoa_map {

namespace private_map {

TileSupervisor::TileSupervisor() :
	_num_tile_rows(0),
	_num_tile_cols(0)
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



void TileSupervisor::Load(ReadScriptDescriptor& map_file, const MapMode* map_instance) {
	// TODO: Add some more error checking in this function (such as checking for script errors after reading blocks of data from the map file)

	// ---------- (1) Load the map dimensions and do some basic sanity checks
	_num_tile_rows = map_file.ReadInt("num_tile_rows");
	_num_tile_cols = map_file.ReadInt("num_tile_cols");

	// Check to make sure tables are of the proper size
	// TODO: we only check that the number of rows are correct, but not the number of columns
	if (map_file.GetTableSize("lower_layer") != _num_tile_rows) {
		PRINT_ERROR << "the lower_layer table size was not equal to the number of tile rows specified by the map" << endl;
		return;
	}
	if (map_file.GetTableSize("middle_layer") != _num_tile_rows) {
		PRINT_ERROR << "the middle_layer table size was not equal to the number of tile rows specified by the map" << endl;
		return;
	}
	if (map_file.GetTableSize("upper_layer") != _num_tile_rows) {
		PRINT_ERROR << "the upper_layer table size was not equal to the number of tile rows specified by the map" << endl;
		return;
	}

	vector<uint32> context_inherits;
	map_file.ReadUIntVector("context_inherits", context_inherits);

	// ---------- (2) Load all of the tileset images that are used by this map

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
		if (ImageDescriptor::LoadMultiImageFromElementGrid(tileset_images[i], image_filename, 16, 16) == false) {
			PRINT_ERROR << "failed to load tileset image: " << image_filename << endl;
			exit(1);
		}
	}

	// ---------- (3) Read in the map tile indeces from all three tile layers for the base context
	// The indeces stored for the map layers in this file directly correspond to a location within a tileset. Tilesets contain a total of 256 tiles
	// each, so 0-255 correspond to the first tileset, 256-511 the second, etc. The tile location within the tileset is also determined by the index,
	// where the first 16 indeces in the tileset range are the tiles of the first row (left to right), and so on.

	// Create and add the 2D tile grid for the base context
	_tile_grid.clear();
	_tile_grid.insert(make_pair(MAP_CONTEXT_01, vector<vector<MapTile> >(_num_tile_rows)));
	for (uint32 r = 0; r < _num_tile_rows; r++) {
		_tile_grid[MAP_CONTEXT_01][r].resize(_num_tile_cols);
	}

	vector<int32> table_row; // Used to temporarily store a row of table indeces

	// Read the base context tables for all three layers
	map_file.OpenTable("lower_layer");
	for (uint32 r = 0; r < _num_tile_rows; r++) {
		table_row.clear();
		map_file.ReadIntVector(r, table_row);
		for (uint32 c = 0; c < _num_tile_cols; c++) {
			_tile_grid[MAP_CONTEXT_01][r][c].lower_layer = table_row[c];
		}
	}
	map_file.CloseTable();

	map_file.OpenTable("middle_layer");
	for (uint32 r = 0; r < _num_tile_rows; r++) {
		table_row.clear();
		map_file.ReadIntVector(r, table_row);
		for (uint32 c = 0; c < _num_tile_cols; c++) {
			_tile_grid[MAP_CONTEXT_01][r][c].middle_layer = table_row[c];
		}
	}
	map_file.CloseTable();

	map_file.OpenTable("upper_layer");
	for (uint32 r = 0; r < _num_tile_rows; r++) {
		table_row.clear();
		map_file.ReadIntVector(r, table_row);
		for (uint32 c = 0; c < _num_tile_cols; c++) {
			_tile_grid[MAP_CONTEXT_01][r][c].upper_layer = table_row[c];
		}
	}
	map_file.CloseTable();

	// ---------- (4) Create each additional context for the map by loading its table data

	// Load the tile data for each additional map context
	for (uint32 i = 1; i < map_instance->GetNumMapContexts(); i++) {
		MAP_CONTEXT this_context = static_cast<MAP_CONTEXT>(1 << i);
		string context_name = "context_";
		if (i < 10) // precede single digit context names with a zero
			context_name += "0";
		context_name += NumberToString(i);

		// Initialize this context by making a copy of the base map context first, as most contexts re-use many of the same tiles from the base context
		// If non-inheriting context, start with empty map!
		if (context_inherits[i - 1] == 1) {
			_tile_grid.insert(make_pair(this_context, _tile_grid[MAP_CONTEXT_01]));
		}
		else {
			_tile_grid.insert(make_pair(this_context, vector<vector<MapTile> >(_num_tile_rows)));
			for (uint32 r = 0; r < _num_tile_rows; r++) {
				_tile_grid[this_context][r].resize(_num_tile_cols);
			}
		}

		// Read the table corresponding to this context and modify each tile accordingly.
		// The context table is an array of integer data. The size of this array should be divisible by four, as every consecutive group of four integers in
		// this table represent one tile context element. The first integer corresponds to the tile layer (0 = lower, 1 = middle, 2 = upper), the second
		// and third represent the row and column of the tile respectively, and the fourth value indicates which tile image should be used for this context.
		// So if the first four entries in the context table were {0, 12, 26, 180}, this would set the lower layer tile at position (12, 26) to the tile
		// index 180.
		vector<int32> context_data;
		map_file.ReadIntVector(context_name, context_data);
		if (context_data.size() % 4 != 0) {
			IF_PRINT_WARNING(MAP_DEBUG) << "for context " << this_context << ", context data was not evenly divisible by four (incomplete context data)" << endl;
			continue;
		}

		for (uint32 j = 0; j < context_data.size(); j += 4) {
			switch (context_data[j]) {
				case 0: // lower layer
					_tile_grid[this_context][context_data[j+1]][context_data[j+2]].lower_layer = context_data[j+3];
					break;
				case 1: // middle layer
					_tile_grid[this_context][context_data[j+1]][context_data[j+2]].middle_layer = context_data[j+3];
					break;
				case 2: // upper layer
					_tile_grid[this_context][context_data[j+1]][context_data[j+2]].upper_layer = context_data[j+3];
					break;
				default:
					IF_PRINT_WARNING(MAP_DEBUG) << "unknown tile layer index reference when loading map context tiles" << endl;
					break;
			}
		}
	} // for (uint32 i = 1; i < map_instance->_num_map_contexts; i++)


	// ---------- (5) Determine which tiles in each tileset are referenced in this map

	// Used to determine whether each tile is used by the map or not. An entry of -1 indicates that particular tile is not used
	vector<int16> tile_references;
	// Set size to be equal to the total number of tiles and initialize all entries to -1 (unreferenced)
	tile_references.assign(tileset_filenames.size() * TILES_PER_TILESET, -1);

	for (map<MAP_CONTEXT, vector<vector<MapTile> > >::iterator i = _tile_grid.begin(); i != _tile_grid.end(); i++) {
		for (uint32 r = 0; r < _num_tile_rows; r++) {
			for (uint32 c = 0; c < _num_tile_cols; c++) {
				if ((i->second)[r][c].lower_layer >= 0)
					tile_references[(i->second)[r][c].lower_layer] = 0;
				if ((i->second)[r][c].middle_layer >= 0)
					tile_references[(i->second)[r][c].middle_layer] = 0;
				if ((i->second)[r][c].upper_layer >= 0)
					tile_references[(i->second)[r][c].upper_layer] = 0;
			}
		}
	}

	// ---------- (6) Translate the tileset tile indeces into indeces for the vector of tile images

	// Here, we have to convert the original tile indeces defined in the map file into a new form. The original index
	// indicates the tileset where the tile is used and its location in that tileset. We need to convert those indeces
	// so that they serve as an index to the MapMode::_tile_images vector, where the tile images will soon be stored.

	// Keeps track of the next translated index number to assign
	uint32 next_index = 0;

	for (uint32 i = 0; i < tile_references.size(); i++) {
		if (tile_references[i] >= 0) {
			tile_references[i] = next_index;
			next_index++;
		}
	}

	// Now, go back and re-assign all lower, middle, and upper tile layer indeces with the translated indeces
	for (map<MAP_CONTEXT, vector<vector<MapTile> > >::iterator i = _tile_grid.begin(); i != _tile_grid.end(); i++) {
		for (uint32 r = 0; r < _num_tile_rows; r++) {
			for (uint32 c = 0; c < _num_tile_cols; c++) {
				if ((i->second)[r][c].lower_layer >= 0)
					(i->second)[r][c].lower_layer = tile_references[(i->second)[r][c].lower_layer];
				if ((i->second)[r][c].middle_layer >= 0)
					(i->second)[r][c].middle_layer = tile_references[(i->second)[r][c].middle_layer];
				if ((i->second)[r][c].upper_layer >= 0)
					(i->second)[r][c].upper_layer = tile_references[(i->second)[r][c].upper_layer];
			}
		}
	}

	// ---------- (7) Parse all of the tileset definition files and create any animated tile images that will be used

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

	// ---------- (8) Add all referenced tiles to the _tile_images vector, in the proper order

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
} // void TileSupervisor::Load(ReadScriptDescriptor& map_file)



void TileSupervisor::Update() {
	for (uint32 i = 0; i < _animated_tile_images.size(); i++) {
		_animated_tile_images[i]->Update();
	}
}



void TileSupervisor::DrawLowerLayer(const MapFrame* const frame) {
	VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
	VideoManager->Move(frame->tile_x_start, frame->tile_y_start);
	for (uint32 r = static_cast<uint32>(frame->starting_row);
			r < static_cast<uint32>(frame->starting_row + frame->num_draw_rows); r++)
		{
		for (uint32 c = static_cast<uint32>(frame->starting_col);
				c < static_cast<uint32>(frame->starting_col + frame->num_draw_cols); c++)
		{
			// Draw a tile image if it exists at this location
			if (_tile_grid[MapMode::CurrentInstance()->GetCurrentContext()][r][c].lower_layer >= 0)
			{
				_tile_images[_tile_grid[MapMode::CurrentInstance()->GetCurrentContext()][r][c].lower_layer]->Draw();
			}
			VideoManager->MoveRelative(2.0f, 0.0f);
		}
		VideoManager->MoveRelative(-static_cast<float>(frame->num_draw_cols * 2), 2.0f);
	}
}



void TileSupervisor::DrawMiddleLayer(const MapFrame* const frame) {
	VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
	VideoManager->Move(frame->tile_x_start, frame->tile_y_start);
	for (uint32 r = static_cast<uint32>(frame->starting_row);
			r < static_cast<uint32>(frame->starting_row + frame->num_draw_rows); r++)
	{
		for (uint32 c = static_cast<uint32>(frame->starting_col);
				c < static_cast<uint32>(frame->starting_col + frame->num_draw_cols); c++)
		{
			// Draw a tile image if it exists at this location
			if (_tile_grid[MapMode::CurrentInstance()->GetCurrentContext()][r][c].middle_layer >= 0)
			{
				_tile_images[_tile_grid[MapMode::CurrentInstance()->GetCurrentContext()][r][c].middle_layer]->Draw();
			}
			VideoManager->MoveRelative(2.0f, 0.0f);
		}

		VideoManager->MoveRelative(-static_cast<float>(frame->num_draw_cols * 2), 2.0f);
	}
}



void TileSupervisor::DrawUpperLayer(const MapFrame* const frame) {
	VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
	VideoManager->Move(frame->tile_x_start, frame->tile_y_start);
	for (uint32 r = static_cast<uint32>(frame->starting_row);
			r < static_cast<uint32>(frame->starting_row + frame->num_draw_rows); r++)
	{
		for (uint32 c = static_cast<uint32>(frame->starting_col);
				c < static_cast<uint32>(frame->starting_col + frame->num_draw_cols); c++)
		{
			// Draw a tile image if it exists at this location
			if (_tile_grid[MapMode::CurrentInstance()->GetCurrentContext()][r][c].upper_layer >= 0)
			{
				_tile_images[_tile_grid[MapMode::CurrentInstance()->GetCurrentContext()][r][c].upper_layer]->Draw();
			}
			VideoManager->MoveRelative(2.0f, 0.0f);
		}
		VideoManager->MoveRelative(-static_cast<float>(frame->num_draw_cols * 2), 2.0f);
	}
}

} // namespace private_map

} // namespace hoa_map
