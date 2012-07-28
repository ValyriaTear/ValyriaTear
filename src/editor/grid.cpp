///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/*!****************************************************************************
 * \file    grid.cpp
 * \author  Philip Vorsilak, gorzuate@allacrost.org
 * \brief   Source file for editor's grid, used for the OpenGL map portion
 *          where tiles are painted, edited, etc.
 *****************************************************************************/

#include "grid.h"
#include "editor.h"

#include "engine/script/script_write.h"
#include "engine/script/script_read.h"

#include <QScrollBar>

#include <sstream>
#include <iostream>

using namespace hoa_script;
using namespace hoa_map::private_map;
using namespace hoa_video;
using namespace std;

namespace hoa_editor {

// Map editor markup lines
const char * BEFORE_TEXT_MARKER = "-- Valyria Tear map editor begin. Do not edit this line or put anything before this line. --";
const char * AFTER_TEXT_MARKER =  "-- Valyria Tear map editor end. Do not edit this line. Place your scripts after this line. --";

LAYER_TYPE getLayerType(const std::string& type)
{
	if (type == "ground")
		return GROUND_LAYER;
	else if (type == "sky")
		return SKY_LAYER;

	return INVALID_LAYER;

}


std::string getTypeFromLayer(const LAYER_TYPE& type) {

	switch (type) {
		case GROUND_LAYER:
			return "ground";
		case SKY_LAYER:
			return "sky";
		default:
			break;
	};
	return "other";
}



LAYER_TYPE& operator++(LAYER_TYPE& value, int /*dummy*/)
{
	value = static_cast<LAYER_TYPE>(static_cast<int>(value) + 1);
	return value;
}


Grid::Grid(QWidget* parent, const QString& name, uint32 width, uint32 height) :
	QGLWidget(parent, (const char*) name),
	_file_name(name),
	_height(height),
	_width(width),
	_context(0),
	_changed(false),
	_initialized(false),
	_grid_on(true),
	_select_on(false),
	_ol_on(true)
{
	resize(_width * TILE_WIDTH, _height * TILE_HEIGHT);
	setMouseTracking(true);

	// Initialize layers with -1 to indicate that no tile/object/etc. is
	// present at this location
	_select_layer.resize(_height);
	for (uint32 y = 0; y < _height; ++y) {
		_select_layer[y].resize(_width);
		for (uint32 x = 0; x < _width; ++x) {
			_select_layer[y][x] = -1;
		}
	}

	// Create default base context
	_tile_contexts.resize(1);
	_tile_contexts[0].name = tr("Base").toStdString();
	// Create default base layers
	_tile_contexts[0].layers.resize(4);
	// Add a default ground type and name to it
	_tile_contexts[0].layers[0].layer_type = GROUND_LAYER;
	_tile_contexts[0].layers[0].name = tr("Background").toStdString();
	_tile_contexts[0].layers[1].layer_type = GROUND_LAYER;
	_tile_contexts[0].layers[1].name = tr("Background 2").toStdString();
	_tile_contexts[0].layers[2].layer_type = GROUND_LAYER;
	_tile_contexts[0].layers[2].name = tr("Background 3").toStdString();
	_tile_contexts[0].layers[3].layer_type = SKY_LAYER;
	_tile_contexts[0].layers[3].name = tr("Sky").toStdString();

	// Set up its size, and fill it with empty values
	_tile_contexts[0].layers[0].tiles.resize(_height);
	_tile_contexts[0].layers[1].tiles.resize(_height);
	_tile_contexts[0].layers[2].tiles.resize(_height);
	_tile_contexts[0].layers[3].tiles.resize(_height);
	for (uint32 y = 0; y < _height; ++y) {
		_tile_contexts[0].layers[0].tiles[y].resize(_width);
		_tile_contexts[0].layers[1].tiles[y].resize(_width);
		_tile_contexts[0].layers[2].tiles[y].resize(_width);
		_tile_contexts[0].layers[3].tiles[y].resize(_width);
		for (uint32 x = 0; x < _width; ++x) {
			_tile_contexts[0].layers[0].tiles[y][x] = -1;
			_tile_contexts[0].layers[1].tiles[y][x] = -1;
			_tile_contexts[0].layers[2].tiles[y][x] = -1;
			_tile_contexts[0].layers[3].tiles[y][x] = -1;
		}
	}
} // Grid constructor


Grid::~Grid()
{
	for (vector<Tileset*>::iterator it = tilesets.begin();
	     it != tilesets.end(); it++)
		delete *it;
	VideoManager->SingletonDestroy();
} // Grid destructor


///////////////////////////////////////////////////////////////////////////////
// Grid class -- public functions
///////////////////////////////////////////////////////////////////////////////

QStringList Grid::GetContextNames()
{
	QStringList context_names;
	for (uint32 i = 0; i < _tile_contexts.size(); ++i) {
		context_names.append(QString::fromStdString(_tile_contexts[i].name));
	}
	return context_names;
}


void Grid::ClearSelectionLayer()
{
	for (uint32 y = 0; y < _height; ++y) {
		for(uint32 x = 0; x < _width; ++x) {
			_select_layer[y][x] = -1;
		}
	}
}

bool Grid::CreateNewContext(std::string name, int32 inherit_context)
{
	// Return false when the inheritance is invalid.
	if (inherit_context < -1 || inherit_context >= (int32)_tile_contexts.size())
		return false;

	int context_id = _tile_contexts.size();
	stringstream context;
	context << "context_";
	if (context_id < 10)
		context << "0";
	context << context_id;

	// Push a new base context copy
	// Make sure the context to be created is indeed the next one
	_tile_contexts.resize(context_id + 1);

	uint32 layers_num = _tile_contexts[0].layers.size();

	// Create an empty context when there is no inheritance
	if (inherit_context == -1) {
		// Resize the context to have the same size as the base one
		// The number of layers
		_tile_contexts[context_id].layers.resize(layers_num);
		// For each layer, set up the grid size
		for (uint32 layer_id = 0; layer_id < layers_num; ++layer_id) {
			// Type
			_tile_contexts[context_id].layers[layer_id].layer_type = _tile_contexts[0].layers[layer_id].layer_type;
			// Layer name
			_tile_contexts[context_id].layers[layer_id].name = _tile_contexts[0].layers[layer_id].name;
			// Height
			_tile_contexts[context_id].layers[layer_id].tiles.resize(_height);
			for (uint32 y = 0; y < _height; ++y) {
				// and width
				_tile_contexts[context_id].layers[layer_id].tiles[y].assign((size_t)_width, -1);
			}
		}
	}
	else {
		// Copy the context data from the parent one
		_tile_contexts[context_id] = _tile_contexts[inherit_context];
	}

	// Set the context name and inheritance info after that
	_tile_contexts[context_id].name = name;
	_tile_contexts[context_id].inherit_from_context_id = inherit_context;

	return true;
} // Grid::CreateNewContext(...)

bool Grid::LoadMap()
{
	// File descriptor for the map data that is to be read
	ReadScriptDescriptor read_data;
	// Used to read in vectors from the file
	vector<int32> vect;
	// Used as the window title for any errors that are detected and to notify
	// the user via a message box
	QString message_box_title("Load File Error");

	// Open the map file for reading
	if (read_data.OpenFile(string(_file_name.toAscii()), true) == false)
	{
		QMessageBox::warning(this, message_box_title,
			QString("Could not open file %1 for reading.").arg(_file_name));
		return false;
	}

	// Check that the main table containing the map exists and open it
	string main_map_table = string(_file_name.section('/', -1).
	                               remove(".lua").toAscii());
	if (read_data.DoesTableExist(main_map_table) == false)
	{
		QMessageBox::warning(this, message_box_title,
			QString("File did not contain the main map table: %1").
			        arg(QString::fromStdString(main_map_table)));
		return false;
	}

	read_data.OpenTable(main_map_table);

	// Load the map name and image
	map_name = QString::fromStdString(read_data.ReadString("map_name"));
	map_image_filename = QString::fromStdString(read_data.ReadString("map_image_filename"));
	map_subname = QString::fromStdString(read_data.ReadString("map_subname"));

	// Reset container data
	music_filename.clear();
	tileset_names.clear();
	tilesets.clear();
	_tile_contexts.clear();

	if (!read_data.DoesTableExist("contexts")) {
		QMessageBox::warning(this, message_box_title,
			QString(tr("No 'contexts' table found.")));
		return false;
	}

	// read context data
	read_data.OpenTable("contexts");
	uint32 num_contexts = read_data.GetTableSize();
	// There can't be more than 32 contexts
	if (num_contexts > 32)
		num_contexts = 32;

	// There can't be more than 32 different contexts
	for (uint32 context_id = 0; context_id < num_contexts; ++context_id) {
		if (!read_data.DoesTableExist(context_id))
			return false;

		// opens contexts[context_id]
		read_data.OpenTable(context_id);

		// Create the corresponding context data
		_tile_contexts.resize(context_id + 1);

		std::string name = read_data.ReadString("name");

		// Add the context name
		_tile_contexts[context_id].name = name;

		int32 inheritance = read_data.ReadInt("inherit_from");

		// The base context can't inherit from another one.
		if (context_id == 0)
			inheritance = -1;

		// One context can't inherit from itself or a context with a higher id.
		if ((int32)context_id <= inheritance)
			inheritance = -1;

		_tile_contexts[context_id].inherit_from_context_id = inheritance;

		// Closes contexts[context_id]
		read_data.CloseTable();
	}
	// Closes contexts table
	read_data.CloseTable();

	_height = read_data.ReadUInt("num_tile_rows");
	_width  = read_data.ReadUInt("num_tile_cols");

	if (read_data.IsErrorDetected())
   	{
		QMessageBox::warning(this, message_box_title,
			QString("Data read failure occurred for global map variables. Error messages:\n%1").
			        arg(QString::fromStdString(read_data.GetErrorMessages())));
		return false;
	}

	// Resize the widget to match the width and height of the map we are in the
	// process of loading
	resize(_width * TILE_WIDTH, _height * TILE_HEIGHT);

	// Create selection layer
	_select_layer.resize(_height);
	for (uint32 y = 0; y < _height; ++y) {
		_select_layer[y].resize(_width);
		for (uint32 x = 0; x < _width; ++x) {
			_select_layer[y][x] = -1;
		}
	}

	read_data.OpenTable("tileset_filenames");
	uint32 table_size = read_data.GetTableSize();
	for (uint32 i = 1; i <= table_size; i++)
		tileset_names.append(QString(read_data.ReadString(i).c_str()));
	read_data.CloseTable();

	// Load default music filename
	music_filename = QString::fromStdString(read_data.ReadString("music_filename"));

	// Loading the tileset images using LoadMultiImage is done in editor.cpp in
	// FileOpen via creation of the TilesetTable(s)

	if (!read_data.DoesTableExist("layers")) {
		QMessageBox::warning(this, message_box_title,
			QString(tr("No 'layers' table found.")));
		return false;
	}

	// Will keep each tile line data
	std::vector<int32> row_vect;
	// Stores the layers available indeces
	std::vector<int32> keys_vect;

	// Read the map tile layer data
	read_data.OpenTable("layers");
	uint32 layers_num = read_data.GetTableSize();

	// Parse the 'layers' table
	for (uint32 layer_id = 0; layer_id < layers_num; ++layer_id) {

		if (!read_data.DoesTableExist(layer_id))
			continue;

		// opens layers[layer_id]
		read_data.OpenTable(layer_id);

		LAYER_TYPE layer_type = getLayerType(read_data.ReadString("type"));

		if (layer_type == INVALID_LAYER) {
			QMessageBox::warning(this, message_box_title,
			QString(tr("Ignoring unexisting layer type: %i in file: %s").arg(
					   (int32)layer_type).arg(read_data.GetFilename().c_str())));
			read_data.CloseTable(); // layers[layer_id]
			return false;
		}

		// Add a new layer
		_tile_contexts[0].layers.resize(layer_id + 1);
		// Set the new layer type
		_tile_contexts[0].layers[layer_id].layer_type = layer_type;

		// the layer visible name
		_tile_contexts[0].layers[layer_id].name = read_data.ReadString("name");

		// Parse layers[layer_id].tiles[y]
		for (uint32 y = 0; y < _height; ++y)
		{
			if (!read_data.DoesTableExist(y)) {
				QMessageBox::warning(this, message_box_title,
				QString(tr("Missing layers[%i][%i] in file: %s")
				.arg(layer_id).arg(y).arg(read_data.GetFilename().c_str())));
				read_data.CloseTable(); // layers[layer_id]
				return false;
			}

			read_data.ReadIntVector(y, vect);

			// Prepare the the row
			_tile_contexts[0].layers[layer_id].tiles.resize(y + 1);

			if (vect.size() != _width) {
				QMessageBox::warning(this, message_box_title,
				QString(tr("Invalid line size of layers[%i][%i] in file: %s")
					.arg(layer_id).arg(y).arg(read_data.GetFilename().c_str())));
				read_data.CloseTable(); // layers[layer_id]
				return false;
			}

			for (vector<int32>::iterator it = vect.begin(); it != vect.end(); ++it)
				_tile_contexts[0].layers[layer_id].tiles[y].push_back(*it);
			vect.clear();
		} // iterate through the rows of the layer

		// Closes layers[layer_id]
		read_data.CloseTable();

	} // for each layers

	// close the 'layers' table
	read_data.CloseTable();

	if (read_data.IsErrorDetected())
   	{
		QMessageBox::warning(this, message_box_title,
			QString("Data read failure occurred for tile layer tables. Error messages:\n%1").
			        arg(QString::fromStdString(read_data.GetErrorMessages())));
		return false;
	}

	// Load any existing map context data
	for (uint32 ctxt = 1; ctxt < num_contexts; ++ctxt)
	{
		// In case of inheritence, copy the parent context data
		if (_tile_contexts[ctxt].inherit_from_context_id > -1
				&& _tile_contexts[ctxt].inherit_from_context_id < (int32)ctxt) {
			// First preserve the name and inheritance info
			std::string name = _tile_contexts[ctxt].name;
			int32 inherit_info = _tile_contexts[ctxt].inherit_from_context_id;

			// then copy the data
			_tile_contexts[ctxt] = _tile_contexts[_tile_contexts[ctxt].inherit_from_context_id];

			// and restore the info
			_tile_contexts[ctxt].name = name;
			_tile_contexts[ctxt].inherit_from_context_id = inherit_info;
		}
		else {
			// If non-inheriting context, start with an empty one.
			// Resize the context to have the same size as the base one
			// The number of layers
			_tile_contexts[ctxt].layers.resize(layers_num);
			// For each layer, set up the grid size
			for (uint32 layer_id = 0; layer_id < layers_num; ++layer_id) {
				// Type
				_tile_contexts[ctxt].layers[layer_id].layer_type = _tile_contexts[0].layers[layer_id].layer_type;
				// Layer name
				_tile_contexts[ctxt].layers[layer_id].name = _tile_contexts[0].layers[layer_id].name;
				// Height
				_tile_contexts[ctxt].layers[layer_id].tiles.resize(_height);
				for (uint32 y = 0; y < _height; ++y) {
					// and width
					_tile_contexts[ctxt].layers[layer_id].tiles[y].assign((size_t)_width, -1);
				}
			}
		}

		// Read the table corresponding to this context and modify each tile
		// accordingly. The context table is an array of integer data. The size
		// of this array should be divisible by four, as every consecutive
		// group of four integers in this table represent one tile context
		// element. The first integer corresponds to the tile layer (0 = lower,
		// 1 = middle, 2 = upper), the second and third represent the row and
		// column of the tile respectively, and the fourth value indicates
		// which tile image should be used for this context. So if the first
		// four entries in the context table were {0, 12, 26, 180}, this would
		// set the lower layer tile at position (12, 26) to the tile index 180.
		vector<int32> context_data;

		stringstream context;
		context << "context_";
		if (ctxt < 10)
			context << "0";
		context << ctxt;

		read_data.ReadIntVector(context.str(), context_data);
		for (uint32 j = 0; j < context_data.size(); j += 4) {
			int32 layer_id = context_data[j];
			int32 y = context_data[j + 1];
			int32 x = context_data[j + 2];
			int32 tile_id = context_data[j + 3];

			_tile_contexts[ctxt].layers[layer_id].tiles[y][x] = tile_id;

		} // iterate through all tiles in this context
	} // iterate through all existing contexts

	if (read_data.IsErrorDetected())
	{
		QMessageBox::warning(this, message_box_title,
			QString("Data read failure occurred for context tables. Error messages:\n%1").
			        arg(QString::fromStdString(read_data.GetErrorMessages())));
		return false;
	}

	read_data.CloseTable();

	// Gets the data at load time because we might change the filename during the session.
	GetScriptingData();

	return true;
} // Grid::LoadMap()

void Grid::GetScriptingData()
{
	std::ifstream file;
	const int32 BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];

	// First, get the non-editor data (such as map scripting) from the file to save, so we don't clobber it.
	file.open(_file_name.toAscii(), ifstream::in);
	if (file.is_open()) {
		// Search for AFTER_TEXT_MARKER
		while (!file.eof()) {
			file.clear();
			file.getline(buffer, BUFFER_SIZE);
			if (strstr(buffer, AFTER_TEXT_MARKER))
				break;
		}

		// Put all text after AFTER_TEXT_MARKER into after_text string
		while (!file.eof()) {
			file.clear();
			file.getline(buffer, BUFFER_SIZE);
			if (!file.eof()) {
				after_text.append(buffer);
				after_text.push_back('\n');
			}
		}

		file.close();
	}
}


void Grid::SaveMap()
{
	WriteScriptDescriptor write_data;

	if (write_data.OpenFile(string(_file_name.toAscii())) == false) {
		QMessageBox::warning(this, "Saving File...", QString("ERROR: could not open %1 for writing!").arg(_file_name));
		return;
	}

	write_data.WriteLine(BEFORE_TEXT_MARKER);
	write_data.InsertNewLine();
	write_data.WriteComment("Set the namespace according to the map name.");
	string main_map_table = string(_file_name.section('/', -1).remove(".lua").toAscii());
	write_data.WriteNamespace(main_map_table);

	write_data.InsertNewLine();
	write_data.WriteComment("A reference to the C++ MapMode object that was created with this file");
	write_data.WriteLine("map = {}");

	write_data.InsertNewLine();
	write_data.WriteComment("The map name, subname and location image");
	write_data.WriteString("map_name", map_name.toStdString());
	write_data.WriteString("map_image_filename", map_image_filename.toStdString());
	write_data.WriteString("map_subname", map_subname.toStdString());

	write_data.InsertNewLine();
	write_data.WriteComment("The number of rows, and columns that compose the map");
	write_data.WriteInt("num_tile_cols", _width);
	write_data.WriteInt("num_tile_rows", _height);

	write_data.InsertNewLine();
	write_data.WriteComment("The contexts names and inheritance definition");
	write_data.WriteComment("Tells the context id the current context inherit from");
	write_data.WriteComment("This means that the parent context will be used as a base, and the current");
	write_data.WriteComment("context will only have its own differences from it.");
	write_data.WriteComment("At least, the base context (id:0) can't a parent context, thus it should be equal to -1.");
	write_data.WriteComment("Note that a context cannot inherit from itself or a context with a higher id");
	write_data.WriteComment("since it would lead to nasty and useless loading use cases.");

	write_data.BeginTable("contexts");
	for (uint32 context_id = 0; context_id < _tile_contexts.size(); ++context_id) {

		write_data.BeginTable(context_id);
		write_data.WriteString("name", _tile_contexts[context_id].name);
		write_data.WriteInt("inherit_from", _tile_contexts[context_id].inherit_from_context_id);
		write_data.EndTable();
	}
	write_data.EndTable();

	write_data.InsertNewLine();
	write_data.WriteComment("The music file used as default background music on this map.");
	write_data.WriteComment("Other musics will have to handled through scripting.");
	write_data.WriteString("music_filename", music_filename.ascii());

	write_data.InsertNewLine();
	write_data.WriteComment("The names of the tilesets used, with the path and file extension omitted");
	write_data.BeginTable("tileset_filenames");
	uint32 i = 0;
	for (QStringList::Iterator qit = tileset_names.begin();
	     qit != tileset_names.end(); ++qit)
	{
		++i;
		write_data.WriteString(i, (*qit).ascii());
	} // iterate through tileset_names writing each element
	write_data.EndTable();
	write_data.InsertNewLine();

	write_data.WriteComment("The map grid to indicate walkability. The size of the grid is 4x the size of the tile layer tables");
	write_data.WriteComment("Walkability status of tiles for 32 contexts. Zero indicates walkable for all contexts. Valid range: [0:2^32-1]");
	write_data.WriteComment("Example: 1 (BIN 001) = wall for first context only, 2 (BIN 010) means wall for second context only, 5 (BIN 101) means Wall for first and third context.");
	write_data.BeginTable("map_grid");
	//[layer][walkability]
	std::vector<std::vector<int32> > walk_vect;
	// Used to save the northern walkability info of tiles in all layers of
	// all contexts; initialize to walkable.
	std::vector<int32> map_row_north(_width * 2, 0);
	// Used to save the southern walkability info of tiles in all layers of
	// all contexts; initialize to walkable.
	std::vector<int32> map_row_south(_width * 2, 0);
	for (uint32 y = 0; y < _height; ++y)
	{
		// Iterate through all contexts of all layers, column by column,
		// row by row.
		for (int context = 0; context < static_cast<int>(_tile_contexts.size());
		     ++context)
		{
			for (uint32 x = 0; x < _width; ++x) {

				// Used to know if any tile at all on all combined layers exists.
				bool missing_tile = true;

				// linearized coords
				int32 col = y * _width + x;

				// Get walkability for each tile layers.
				for (uint32 layer_id = 0; layer_id < _tile_contexts[context].layers.size(); ++layer_id)
				{
					// Don't deal with sky layers
					if (_tile_contexts[context].layers[layer_id].layer_type == SKY_LAYER)
						continue;

					int tileset_index = _tile_contexts[context].layers[layer_id].tiles[y][x] / 256;
					int tile_index = 0;
					if (tileset_index == 0)
						tile_index = _tile_contexts[context].layers[layer_id].tiles[y][x];
					else  // Don't divide by 0
						tile_index = _tile_contexts[context].layers[layer_id].tiles[y][x] %
							(tileset_index * 256);

					// Push back a layer
					walk_vect.resize(layer_id + 1);

					if (tile_index == -1)
					{
						walk_vect[layer_id].push_back(0);
						walk_vect[layer_id].push_back(0);
						walk_vect[layer_id].push_back(0);
						walk_vect[layer_id].push_back(0);
					}
					else
					{
						missing_tile = false;
						walk_vect[layer_id] = tilesets[tileset_index]->walkability[tile_index];
					}
				} // For each layer

				if (missing_tile == true)
				{
					// NW corner
					map_row_north[col % _width * 2]     |= 1 << context;
					// NE corner
					map_row_north[col % _width * 2 + 1] |= 1 << context;
					// SW corner
					map_row_south[col % _width * 2]     |= 1 << context;
					// SE corner
					map_row_south[col % _width * 2 + 1] |= 1 << context;
				} // no tile exists at current location
				else
				{
					for (uint32 i = 0; i < walk_vect.size(); ++i)
					{
						// NW corner
						map_row_north[col % _width * 2] |= (walk_vect[i][0] << context);
						// NE corner
						map_row_north[col % _width * 2 + 1] |= (walk_vect[i][1] << context);
						// SW corner
						map_row_south[col % _width * 2] |= (walk_vect[i][2] << context);
						// SE corner
						map_row_south[col % _width * 2 + 1] |= (walk_vect[i][3] << context);
					}
				} // a real tile exists at current location

				walk_vect.clear();
			} // x
		} // iterate through each context

		write_data.WriteIntVector(y * 2,   map_row_north);
		write_data.WriteIntVector(y * 2 + 1, map_row_south);
		map_row_north.assign(_width * 2, 0);
		map_row_south.assign(_width * 2, 0);
	} // iterate through the rows (y axis) of the layers
	write_data.EndTable();
	write_data.InsertNewLine();

	write_data.WriteComment("The tile layers. The numbers are indeces to the tile_mappings table.");
	write_data.BeginTable("layers");

	uint32 layers_num = _tile_contexts[0].layers.size();
	for (uint32 layer_id = 0; layer_id < layers_num; ++layer_id) {

		write_data.BeginTable(layer_id);

		write_data.WriteString("type", getTypeFromLayer(_tile_contexts[0].layers[layer_id].layer_type));
		write_data.WriteString("name", _tile_contexts[0].layers[layer_id].name);

		std::vector<int32> layer_row;

		for (uint32 y = 0; y < _height; y++)
		{
			for (uint32 x = 0; x < _width; x++)
			{
				layer_row.push_back(_tile_contexts[0].layers[layer_id].tiles[y][x]);
			} // iterate through the columns of the lower layer
			write_data.WriteIntVector(y, layer_row);
			layer_row.clear();
		} // iterate through the rows of each layer

		write_data.EndTable(); // layer[layer_id]
		write_data.InsertNewLine();
	} // for each layers
	write_data.EndTable(); // Layers
	write_data.InsertNewLine();

	if (_tile_contexts.size() > 1)
		write_data.WriteComment("Contexts data");

	vector<int32> context_data;  // one vector of ints contains all the context info
	// Iterate through all contexts of all layers.
	for (uint32 context_id = 1; context_id < _tile_contexts.size(); ++context_id)
	{
		int32 context_inherit = _tile_contexts[context_id].inherit_from_context_id;

		for (uint32 layer_id = 0; layer_id < _tile_contexts[context_id].layers.size(); ++layer_id) {
			for (uint32 y = 0; y < _height; ++y)
			{
				for (uint32 x = 0; x < _width; ++x)
				{
					int32 ctxt_tile_id = _tile_contexts[context_id].layers[layer_id].tiles[y][x];
					// Record when :
					// - A different tile exists when inheriting of the parent context
					if (context_inherit > -1 && context_inherit < (int32)context_id) {
						int32 parent_tile_id = _tile_contexts[context_inherit].layers[layer_id].tiles[y][x];
						if (parent_tile_id != ctxt_tile_id)
						{
							context_data.push_back(layer_id);
							context_data.push_back(y);
							context_data.push_back(x);
							context_data.push_back(ctxt_tile_id);
						}
					}
					// - When ctxt_tile_id is not empty and when there is no inheritance.
					else if (context_inherit == -1 && ctxt_tile_id != -1) {
						context_data.push_back(layer_id);
						context_data.push_back(y);
						context_data.push_back(x);
						context_data.push_back(ctxt_tile_id);
					}

				} // iterate through the columns of the lower layer
			} // iterate through the rows of the lower layer
		} // Layers

		if (!context_data.empty())
		{
			stringstream context;
			context << "context_";
			if (context_id < 10)
				context << "0";
			context << context_id;

			write_data.WriteIntVector(context.str(), context_data);
			write_data.InsertNewLine();
			context_data.clear();
		} // write the vector if it has data in it
	} // iterate through all contexts of all layers, assuming all layers have same number of contexts

	write_data.WriteLine(AFTER_TEXT_MARKER);

	// Write the "after data" if this file is overwriting another
	if (!after_text.empty())
		write_data.WriteLine(after_text, false);

	write_data.CloseFile();

	_changed = false;
} // Grid::SaveMap()


uint32 Grid::_GetNextLayerId(const LAYER_TYPE& layer_type)
{
	// Computes the new layer id
	LAYER_TYPE previous_layer_type = GROUND_LAYER;
	uint32 i = 0;
	for (; i < _tile_contexts[0].layers.size(); ++i)
	{
		LAYER_TYPE current_type = _tile_contexts[0].layers[i].layer_type;

		if (previous_layer_type == layer_type && current_type != layer_type)
			return i;

		// Not found yet
		previous_layer_type = current_type;
	}

	// Return the last layer id
	return i;
}

void Grid::AddLayer(const LayerInfo& layer_info)
{
	uint32 new_layer_id = _GetNextLayerId(layer_info.layer_type);

	// Prepare the new layer
	Layer layer;
	layer.layer_type = layer_info.layer_type;
	layer.name = layer_info.name;
	layer.Resize(_width, _height);
	layer.Fill(-1); // Make the layer empty

	// The layer id is completely new, so we push a new layer for each context
	if (new_layer_id >= _tile_contexts[0].layers.size())
	{
		assert (new_layer_id == _tile_contexts[0].layers.size());

		for (uint32 ctxt = 0; ctxt < _tile_contexts.size(); ++ctxt)
		{
			_tile_contexts[ctxt].layers.push_back(layer);
		}
		return;
	}

	// If the id is taken, we have to insert the layer before the one
	// with the same id.

	for (uint32 ctxt = 0; ctxt < _tile_contexts.size(); ++ctxt)
	{
		std::vector<Layer> new_layers;
		for (uint32 layer_id = 0; layer_id < _tile_contexts[ctxt].layers.size(); ++layer_id)
		{
			// If we have reached the wanted layer id, add the new layer
			if (layer_id == new_layer_id)
				new_layers.push_back(layer);

			// Push the other layer in any case
			new_layers.push_back(_tile_contexts[ctxt].layers[layer_id]);
		}

		// Once done, we can swap the data, replacing the layers with the one inserted.
		_tile_contexts[ctxt].layers.swap(new_layers);
	}
}

void Grid::DeleteLayer(uint32 layer_id) {
	if (layer_id >= _tile_contexts[0].layers.size())
		return;

	for (uint32 ctxt = 0; ctxt < _tile_contexts.size(); ++ctxt)
	{
		uint32 layer = 0;
		std::vector<Layer>::iterator it = _tile_contexts[ctxt].layers.begin();
		std::vector<Layer>::iterator it_end = _tile_contexts[ctxt].layers.end();
		for (; it != it_end; ++it)
		{
			if (layer == layer_id) {
				_tile_contexts[ctxt].layers.erase(it);
				break;
			}
			++layer;
		}
	}
}

void Grid::InsertRow(uint32 tile_index_y)
{
// See bugs #153 & 154 as to why this function is not implemented for Windows
// TODO: Check that tile_index is within acceptable bounds
/*
#if !defined(WIN32)
	uint32 row = tile_index / _width;

	// Insert the row throughout all contexts
	for (uint32 i = 0; i < static_cast<uint32>(context_names.size()); i++)
	{
		_ground_layers[0][i].insert(_ground_layers[0][i].begin()   + row * _width, _width, -1);
		_fringe_layers[0][i].insert(_fringe_layers[0][i].begin() + row * _width, _width, -1);
		_sky_layers[0][i].insert(_sky_layers[0][i].begin()   + row * _width, _width, -1);
	} // iterate through all contexts

	_height++;
	resize(_width * TILE_WIDTH, _height * TILE_HEIGHT);
#endif*/
} // Grid::InsertRow(...)


void Grid::InsertCol(uint32 tile_index_x)
{/*
// See bugs #153 & 154 as to why this function is not implemented for Windows
// TODO: Check that tile_index is within acceptable bounds

#if !defined(WIN32)
	uint32 col = tile_index % _width;

	// Insert the column throughout all contexts
	for (uint32 i = 0; i < static_cast<uint32>(context_names.size()); i++)
	{
		// Iterate through all rows in each tile layer
		vector<int32>::iterator it = _ground_layers[0][i].begin() + col;
		for (uint32 row = 0; row < _height; row++)
		{
			it  = _ground_layers[0][i].insert(it, -1);
			it += _width + 1;
		} // iterate through the rows of the lower layer

		it = _fringe_layers[0][i].begin() + col;
		for (uint32 row = 0; row < _height; row++)
		{
			it  = _fringe_layers[0][i].insert(it, -1);
			it += _width + 1;
		} // iterate through the rows of the middle layer

		it = _sky_layers[0][i].begin() + col;
		for (uint32 row = 0; row < _height; row++)
		{
			it  = _sky_layers[0][i].insert(it, -1);
			it += _width + 1;
		} // iterate through the rows of the upper layer
	} // iterate through all contexts

	_width++;
	resize(_width * TILE_WIDTH, _height * TILE_HEIGHT);
#endif
*/
} // Grid::InsertCol(...)


void Grid::DeleteRow(uint32 tile_index_y)
{/*
// See bugs #153 & 154 as to why this function is not implemented for Windows
// TODO: Check that tile_index is within acceptable bounds
// TODO: Check that deleting this row does not cause map height to fall below
//       minimum allowed value

#if !defined(WIN32)
	uint32 row = tile_index / _width;

	// Delete the row throughout each context
	for (uint32 i = 0; i < static_cast<uint32>(context_names.size()); i++)
	{
		_lower_layer[i].erase(_lower_layer[i].begin()   + row * _width,
		                      _lower_layer[i].begin()   + row * _width + _width);
		_middle_layer[i].erase(_middle_layer[i].begin() + row * _width,
		                       _middle_layer[i].begin() + row * _width + _width);
		_upper_layer[i].erase(_upper_layer[i].begin()   + row * _width,
		                      _upper_layer[i].begin()   + row * _width + _width);
	} // iterate through all contexts

	_height--;
	resize(_width * TILE_WIDTH, _height * TILE_HEIGHT);
#endif
*/
} // Grid::DeleteRow(...)


void Grid::DeleteCol(uint32 tile_index_x)
{/*
// See bugs #153 & 154 as to why this function is not implemented for Windows
// TODO: Check that tile_index is within acceptable bounds
// TODO: Check that deleting this column does not cause map width to fall below
//       minimum allowed value

#if !defined(WIN32)
	uint32 col = tile_index % _width;

	// Delete the column throughout each contexts
	for (uint32 i = 0; i < static_cast<uint32>(context_names.size()); i++)
	{
		// Iterate through all rows in each tile layer
		vector<int32>::iterator it = _lower_layer[i].begin() + col;
		for (uint32 row = 0; row < _height; row++)
		{
			it  = _lower_layer[i].erase(it);
			it += _width - 1;
		} // iterate through the rows of the lower layer

		it = _middle_layer[i].begin() + col;
		for (uint32 row = 0; row < _height; row++)
		{
			it  = _middle_layer[i].erase(it);
			it += _width - 1;
		} // iterate through the rows of the middle layer

		it = _upper_layer[i].begin() + col;
		for (uint32 row = 0; row < _height; row++)
		{
			it  = _upper_layer[i].erase(it);
			it += _width - 1;
		} // iterate through the rows of the upper layer
	} // iterate through all contexts

	_width--;
	resize(_width * TILE_WIDTH, _height * TILE_HEIGHT);
#endif
*/
} // Grid::DeleteCol(...)

std::vector<QTreeWidgetItem*> Grid::getLayerItems()
{
	std::vector<QTreeWidgetItem*> layers_names;
	for (uint32 layer_id = 0; layer_id < _tile_contexts[0].layers.size(); ++layer_id)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		// Check for empty names
		QString name = QString::fromStdString(_tile_contexts[0].layers[layer_id].name);
		if (name.size() == 0)
			name = QString::number(layer_id);

		item->setText(0, QString::number(layer_id));

		// Show the visible icon depending on the layer state
		if (_tile_contexts[_context].layers[layer_id].visible)
			item->setIcon(1, QIcon(QString("img/misc/editor-tools/eye.png")));

		item->setText(2, name);
		item->setText(3, tr(getTypeFromLayer(_tile_contexts[0].layers[layer_id].layer_type).c_str()));
		layers_names.push_back(item);
	}

	return layers_names;
}

////////////////////////////////////////////////////////////////////////////////
// Grid class -- protected functions
////////////////////////////////////////////////////////////////////////////////

void Grid::initializeGL()
{
	// Destroy the video engine
	VideoManager->SingletonDestroy();
	// Recreate the video engine's singleton
	VideoManager = VideoEngine::SingletonCreate();
	VideoManager->SetTarget(VIDEO_TARGET_QT_WIDGET);

	VideoManager->SingletonInitialize();

	VideoManager->ApplySettings();
	VideoManager->FinalizeInitialization();
	VideoManager->ToggleFPS();

	// Don't use smooth map tiles. This is easing map making.
	VideoManager->SetPixelArtSmoothed(false);
} // Grid::initializeGL()


void Grid::paintGL()
{
	int32 x, y;                  // tile array loop index
	int layer_index;
	vector<int32>::iterator it;      // used to iterate through tile arrays
	int tileset_index;               // index into the tileset_names vector
	int tile_index;                  // ranges from 0-255
	int left_tile;
	int right_tile;
	int top_tile;
	int bottom_tile;

	if (_initialized == false)
		return;

	// Setup drawing parameters
	VideoManager->SetCoordSys(0.0f, VideoManager->GetScreenWidth() / TILE_WIDTH,
	                          VideoManager->GetScreenHeight() / TILE_HEIGHT, 0.0f);
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
	VideoManager->Clear(Color::black);

	// Setup drawing bounds so only visible tiles are drawn. These bounds are
	// valid for all layers.
	int num_tiles_width  = _ed_scrollarea->viewport()->width() / TILE_WIDTH;
	int num_tiles_height = _ed_scrollarea->viewport()->height() / TILE_HEIGHT;
	left_tile   = _ed_scrollarea->horizontalScrollBar()->value() / TILE_WIDTH;
	top_tile    = _ed_scrollarea->verticalScrollBar()->value() / TILE_HEIGHT;
	right_tile  = left_tile + num_tiles_width + 1;
	bottom_tile = top_tile + num_tiles_height + 1;
	right_tile  = (right_tile  < (int32)_width)  ? right_tile  : _width  - 1;
	bottom_tile = (bottom_tile < (int32)_height) ? bottom_tile : _height - 1;

	// Start drawing from the top left
	VideoManager->Move(left_tile, top_tile);

	x = left_tile;
	y = top_tile;
	while (y <= bottom_tile)
	{
		for (uint32 layer_id = 0; layer_id < _tile_contexts[_context].layers.size(); ++layer_id) {
			// Don't draw the layer if it's not visible
			if (!_tile_contexts[_context].layers[layer_id].visible)
			  continue;

			layer_index = _tile_contexts[_context].layers[layer_id].tiles[y][x];
			// Draw tile if one exists at this location
			if (layer_index != -1)
			{
				tileset_index = layer_index / 256;
				// Don't divide by zero
				if (tileset_index == 0)
					tile_index = layer_index;
				else
					tile_index = layer_index % (tileset_index * 256);
				tilesets[tileset_index]->tiles[tile_index].Draw();
			} // a tile exists to draw

		}

		if (x == right_tile)
		{
			x = left_tile;
			y++;
			VideoManager->MoveRelative(-(right_tile - left_tile), 1.0f);
		}
		else
		{
			x++;
			VideoManager->MoveRelative(1.0f, 0.0f);
		}
	}


	// Draw object layer if it is enabled for viewing
	if (_ol_on)
	{
		// TODO: Readd object layer support in a simple way.
	} // object layer must be viewable

	// Draw selection rectangle if this mode is active
	if (_select_on)
	{
		Color blue_selection(0.0f, 0.0f, 255.0f, 0.5f);

		// Start drawing from the top left
		VideoManager->Move(left_tile, top_tile);

		x = left_tile;
		y = top_tile;
		while (y <= bottom_tile)
		{
			layer_index = _select_layer[y][x];
			// Draw tile if one exists at this location
			if (layer_index != -1)
				VideoManager->DrawRectangle(1.0f, 1.0f, blue_selection);

			if (x == right_tile)
			{
				x = left_tile;
				y++;
				VideoManager->MoveRelative(-(right_tile - left_tile), 1.0f);
			}
			else
		   	{
				x++;
				VideoManager->MoveRelative(1.0f, 0.0f);
			}
		} // iterate through selection layer
	} // selection rectangle must be viewable

	// If grid is toggled on, draw it
	if (_grid_on)
		VideoManager->DrawGrid(0.0f, 0.0f, 1.0f, 1.0f, Color::black);

	if (_debug_textures_on)
		VideoManager->Textures()->DEBUG_ShowTexSheet();
} // void Grid::paintGL()


void Grid::resizeGL(int w, int h)
{
	VideoManager->SetResolution(w, h);
	VideoManager->ApplySettings();
} // Grid::resizeGL(...)

} // namespace hoa_editor
