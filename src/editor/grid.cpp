///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    grid.cpp
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for editor's grid, used for the OpenGL map portion
***          where tiles are painted, edited, etc.
*** **************************************************************************/

#include "utils/utils_pch.h"
#include "grid.h"
#include "editor.h"

#include "engine/script/script_write.h"
#include "engine/script/script_read.h"

#include "engine/video/video.h"

#include <QScrollBar>
#include <QGraphicsView>

using namespace vt_script;

namespace vt_editor
{

LAYER_TYPE getLayerType(const std::string &type)
{
    if(type == "ground")
        return GROUND_LAYER;
    else if(type == "sky")
        return SKY_LAYER;

    return INVALID_LAYER;

}


std::string getTypeFromLayer(const LAYER_TYPE &type)
{

    switch(type) {
    case GROUND_LAYER:
        return "ground";
    case SKY_LAYER:
        return "sky";
    default:
        break;
    };
    return "other";
}



LAYER_TYPE &operator++(LAYER_TYPE &value, int /*dummy*/)
{
    value = static_cast<LAYER_TYPE>(static_cast<int>(value) + 1);
    return value;
}


Grid::Grid(QWidget *parent, const QString &name, uint32 width, uint32 height) :
    QGraphicsScene(parent),
    _ed_scrollarea(NULL),
    _file_name(name),
    _height(height),
    _width(width),
    _changed(false),
    _initialized(false),
    _grid_on(true),
    _select_on(false)
{
    setSceneRect(0, 0, _width * TILE_WIDTH, _height * TILE_HEIGHT);

    // Initialize layers with -1 to indicate that no tile/object/etc. is
    // present at this location
    _select_layer.resize(_height);
    for(uint32 y = 0; y < _height; ++y) {
        _select_layer[y].resize(_width);
        for(uint32 x = 0; x < _width; ++x) {
            _select_layer[y][x] = -1;
        }
    }

    // Create default base layers
    _tile_layers.resize(4);
    // Add a default ground type and name to it
    _tile_layers[0].layer_type = GROUND_LAYER;
    _tile_layers[0].name = tr("Background").toStdString();
    _tile_layers[1].layer_type = GROUND_LAYER;
    _tile_layers[1].name = tr("Background 2").toStdString();
    _tile_layers[2].layer_type = GROUND_LAYER;
    _tile_layers[2].name = tr("Background 3").toStdString();
    _tile_layers[3].layer_type = SKY_LAYER;
    _tile_layers[3].name = tr("Sky").toStdString();

    // Set up its size, and fill it with empty values
    _tile_layers[0].tiles.resize(_height);
    _tile_layers[1].tiles.resize(_height);
    _tile_layers[2].tiles.resize(_height);
    _tile_layers[3].tiles.resize(_height);
    for(uint32 y = 0; y < _height; ++y) {
        _tile_layers[0].tiles[y].resize(_width);
        _tile_layers[1].tiles[y].resize(_width);
        _tile_layers[2].tiles[y].resize(_width);
        _tile_layers[3].tiles[y].resize(_width);
        for(uint32 x = 0; x < _width; ++x) {
            _tile_layers[0].tiles[y][x] = -1;
            _tile_layers[1].tiles[y][x] = -1;
            _tile_layers[2].tiles[y][x] = -1;
            _tile_layers[3].tiles[y][x] = -1;
        }
    }

    // Creates the graphic view
    _graphics_view = new QGraphicsView(this);
    _graphics_view->setRenderHints(QPainter::Antialiasing);
    _graphics_view->setBackgroundBrush(QBrush(Qt::black));
    _graphics_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _graphics_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
} // Grid constructor


Grid::~Grid()
{
    for(std::vector<Tileset *>::iterator it = tilesets.begin();
            it != tilesets.end(); ++it)
        delete *it;

    delete _graphics_view;
} // Grid destructor


///////////////////////////////////////////////////////////////////////////////
// Grid class -- public functions
///////////////////////////////////////////////////////////////////////////////

void Grid::ClearSelectionLayer()
{
    for(uint32 y = 0; y < _height; ++y) {
        for(uint32 x = 0; x < _width; ++x) {
            _select_layer[y][x] = -1;
        }
    }
}

bool Grid::LoadMap()
{
    // File descriptor for the map data that is to be read
    ReadScriptDescriptor read_data;
    // Used to read in vectors from the file
    std::vector<int32> vect;
    // Used as the window title for any errors that are detected and to notify
    // the user via a message box
    QString message_box_title("Load File Error");

    // Open the map file for reading
    if(!read_data.OpenFile(_file_name.toStdString())) {
        read_data.CloseFile();
        QMessageBox::warning(_graphics_view, message_box_title,
                             QString("Could not open file %1 for reading.").arg(_file_name));
        return false;
    }

    if(!read_data.DoesTableExist("map_data")) {
        read_data.CloseFile();
        QMessageBox::warning(_graphics_view, message_box_title,
                             QString("File did not contain the main map table: 'map_data'"));
        return false;
    }

    read_data.OpenTable("map_data");

    // Reset container data
    tilesets.clear();
    _tile_layers.clear();

    _height = read_data.ReadUInt("num_tile_rows");
    _width  = read_data.ReadUInt("num_tile_cols");

    if(read_data.IsErrorDetected()) {
        read_data.CloseFile();
        QMessageBox::warning(_graphics_view, message_box_title,
                             QString("Data read failure occurred for global map variables. Error messages:\n%1").
                             arg(QString::fromStdString(read_data.GetErrorMessages())));
        return false;
    }

    // Resize the widget to match the width and height of the map we are in the
    // process of loading
    setSceneRect(0, 0, _width * TILE_WIDTH, _height * TILE_HEIGHT);

    // Create selection layer
    _select_layer.resize(_height);
    for(uint32 y = 0; y < _height; ++y) {
        _select_layer[y].resize(_width);
        for(uint32 x = 0; x < _width; ++x) {
            _select_layer[y][x] = -1;
        }
    }

    // Loads the tileset definition filenames
    tileset_def_names.clear();
    if (read_data.OpenTable("tileset_filenames")) {
        uint32 table_size = read_data.GetTableSize();
        for(uint32 i = 1; i <= table_size; ++i) {
            tileset_def_names.append(read_data.ReadString(i).c_str());
        }
        read_data.CloseTable();
    }

    // Loading the tileset images using LoadMultiImage is done in editor.cpp in
    // FileOpen via creation of the TilesetTable(s)
    if(!read_data.DoesTableExist("layers")) {
        read_data.CloseFile();
        QMessageBox::warning(_graphics_view, message_box_title,
                             QString(tr("No 'layers' table found.")));
        return false;
    }

    // Read the map tile layer data
    read_data.OpenTable("layers");
    uint32 layers_num = read_data.GetTableSize();

    // Parse the 'layers' table
    for(uint32 layer_id = 0; layer_id < layers_num; ++layer_id) {

        if(!read_data.DoesTableExist(layer_id))
            continue;

        // opens layers[layer_id]
        read_data.OpenTable(layer_id);

        LAYER_TYPE layer_type = getLayerType(read_data.ReadString("type"));

        if(layer_type == INVALID_LAYER) {
            read_data.CloseFile();
            QMessageBox::warning(_graphics_view, message_box_title,
                                 QString(tr("Ignoring unexisting layer type: %i in file: %s").arg(
                                             (int32)layer_type).arg(read_data.GetFilename().c_str())));
            return false;
        }

        // Add a new layer
        _tile_layers.resize(layer_id + 1);
        // Set the new layer type
        _tile_layers[layer_id].layer_type = layer_type;

        // the layer visible name
        _tile_layers[layer_id].name = read_data.ReadString("name");

        // Parse layers[layer_id].tiles[y]
        for(uint32 y = 0; y < _height; ++y) {
            if(!read_data.DoesTableExist(y)) {
                QMessageBox::warning(_graphics_view, message_box_title,
                                     QString(tr("Missing layers[%i][%i] in file: %s")
                                             .arg(layer_id).arg(y).arg(read_data.GetFilename().c_str())));
                read_data.CloseFile();
                return false;
            }

            read_data.ReadIntVector(y, vect);

            // Prepare the the row
            _tile_layers[layer_id].tiles.resize(y + 1);

            if(vect.size() != _width) {
                read_data.CloseFile();
                QMessageBox::warning(_graphics_view, message_box_title,
                                     QString(tr("Invalid line size of layers[%i][%i] in file: %s")
                                             .arg(layer_id).arg(y).arg(read_data.GetFilename().c_str())));
                return false;
            }

            for(std::vector<int32>::iterator it = vect.begin(); it != vect.end(); ++it)
                _tile_layers[layer_id].tiles[y].push_back(*it);
            vect.clear();
        } // iterate through the rows of the layer

        // Closes layers[layer_id]
        read_data.CloseTable();

    } // for each layers

    // close the 'layers' table
    read_data.CloseTable();

    if(read_data.IsErrorDetected()) {
        read_data.CloseFile();
        QMessageBox::warning(_graphics_view, message_box_title,
                             QString("Data read failure occurred for tile layer tables. Error messages:\n%1").
                             arg(QString::fromStdString(read_data.GetErrorMessages())));
        return false;
    }

    read_data.CloseFile();

    return true;
} // Grid::LoadMap()

void Grid::SaveMap()
{
    WriteScriptDescriptor write_data;

    if(!write_data.OpenFile(_file_name.toStdString())) {
        QMessageBox::warning(_graphics_view, "Saving File...", QString("ERROR: could not open %1 for writing!").arg(_file_name));
        return;
    }

    write_data.BeginTable("map_data");

    write_data.InsertNewLine();
    write_data.WriteComment("The number of rows, and columns that compose the map");
    write_data.WriteInt("num_tile_cols", _width);
    write_data.WriteInt("num_tile_rows", _height);

    write_data.InsertNewLine();
    write_data.WriteComment("The tilesets definition files used.");
    write_data.BeginTable("tileset_filenames");
    uint32 i = 0;
    for(QStringList::Iterator qit = tileset_def_names.begin();
            qit != tileset_def_names.end(); ++qit) {
        ++i;
        write_data.WriteString(i, (*qit).toStdString());
    } // iterate through tileset_names writing each element
    write_data.EndTable();
    write_data.InsertNewLine();

    write_data.WriteComment("The map grid to indicate walkability. 0 is walkable, 1 is not.");
    write_data.BeginTable("map_grid");
    //[layer][walkability]
    std::vector<std::vector<int32> > walk_vect;
    // Used to save the northern walkability info of tiles in all layers of
    // all contexts; initialize to walkable.
    std::vector<int32> map_row_north(_width * 2, 0);
    // Used to save the southern walkability info of tiles in all layers of
    // all contexts; initialize to walkable.
    std::vector<int32> map_row_south(_width * 2, 0);

    for(uint32 y = 0; y < _height; ++y) {
        // Iterate through all layers, column by column, row by row.
        for(uint32 x = 0; x < _width; ++x) {

            // Indicates whether a painted tile is present on at least one layer.
            bool no_tile_at_all = true;

            // linearized coords
            int32 col = y * _width + x;

            // Get walkability for each tile layers.
            for(uint32 layer_id = 0; layer_id < _tile_layers.size(); ++layer_id) {
                // Don't deal with sky layers
                if(_tile_layers[layer_id].layer_type == SKY_LAYER)
                    continue;

                int tileset_index = _tile_layers[layer_id].tiles[y][x] / 256;
                int tile_index = -1;
                if(tileset_index == 0) // First tileset
                    tile_index = _tile_layers[layer_id].tiles[y][x];
                else  // Don't divide by 0
                    tile_index = _tile_layers[layer_id].tiles[y][x] %
                                    (tileset_index * 256);

                // Push back a layer
                walk_vect.resize(layer_id + 1);

                if(tile_index == -1) {
                    // no tile on this layer we assume walkable (0) for now
                    // until all layers have been checked.
                    walk_vect[layer_id].push_back(0);
                    walk_vect[layer_id].push_back(0);
                    walk_vect[layer_id].push_back(0);
                    walk_vect[layer_id].push_back(0);
                } else {
                    no_tile_at_all = false;
                    walk_vect[layer_id] = tilesets[tileset_index]->walkability[tile_index];
                }
            } // For each layer

            if(no_tile_at_all) {
                // NW corner
                map_row_north[col % _width * 2]     = 1;
                // NE corner
                map_row_north[col % _width * 2 + 1] = 1;
                // SW corner
                map_row_south[col % _width * 2]     = 1;
                // SE corner
                map_row_south[col % _width * 2 + 1] = 1;
            }
            else {
                for(uint32 i = 0; i < walk_vect.size(); ++i) {
                    // NW corner
                    map_row_north[col % _width * 2] |= walk_vect[i][0];
                    // NE corner
                    map_row_north[col % _width * 2 + 1] |= walk_vect[i][1];
                    // SW corner
                    map_row_south[col % _width * 2] |= walk_vect[i][2];
                    // SE corner
                    map_row_south[col % _width * 2 + 1] |= walk_vect[i][3];
                }
            } // a real tile exists at current location

            walk_vect.clear();
        } // x

        write_data.WriteIntVector(y * 2,   map_row_north);
        write_data.WriteIntVector(y * 2 + 1, map_row_south);
        map_row_north.assign(_width * 2, 0);
        map_row_south.assign(_width * 2, 0);
    } // iterate through the rows (y axis) of the layers

    write_data.EndTable();
    write_data.InsertNewLine();

    write_data.WriteComment("The tile layers. The numbers are indeces to the tile_mappings table.");
    write_data.BeginTable("layers");

    uint32 layers_num = _tile_layers.size();
    for(uint32 layer_id = 0; layer_id < layers_num; ++layer_id) {

        write_data.BeginTable(layer_id);

        write_data.WriteString("type", getTypeFromLayer(_tile_layers[layer_id].layer_type));
        write_data.WriteString("name", _tile_layers[layer_id].name);

        std::vector<int32> layer_row;

        for(uint32 y = 0; y < _height; y++) {
            for(uint32 x = 0; x < _width; x++) {
                layer_row.push_back(_tile_layers[layer_id].tiles[y][x]);
            } // iterate through the columns of the lower layer
            write_data.WriteIntVector(y, layer_row);
            layer_row.clear();
        } // iterate through the rows of each layer

        write_data.EndTable(); // layer[layer_id]
        write_data.InsertNewLine();
    } // for each layers
    write_data.EndTable(); // Layers

    write_data.EndTable(); // map_data

    write_data.CloseFile();

    _changed = false;
} // Grid::SaveMap()


uint32 Grid::_GetNextLayerId(const LAYER_TYPE &layer_type)
{
    // Computes the new layer id
    LAYER_TYPE previous_layer_type = GROUND_LAYER;
    uint32 i = 0;
    for(; i < _tile_layers.size(); ++i) {
        LAYER_TYPE current_type = _tile_layers[i].layer_type;

        if(previous_layer_type == layer_type && current_type != layer_type)
            return i;

        // Not found yet
        previous_layer_type = current_type;
    }

    // Return the last layer id
    return i;
}

void Grid::AddLayer(const LayerInfo &layer_info)
{
    uint32 new_layer_id = _GetNextLayerId(layer_info.layer_type);

    // Prepare the new layer
    Layer layer;
    layer.layer_type = layer_info.layer_type;
    layer.name = layer_info.name;
    layer.Resize(_width, _height);
    layer.Fill(-1); // Make the layer empty

    // The layer id is completely new, so we push a new layer for each context
    if(new_layer_id >= _tile_layers.size()) {
        assert(new_layer_id == _tile_layers.size());
        _tile_layers.push_back(layer);
        return;
    }

    // If the id is taken, we have to insert the layer before the one
    // with the same id.
    std::vector<Layer> new_layers;
    for(uint32 layer_id = 0; layer_id < _tile_layers.size(); ++layer_id) {
        // If we have reached the wanted layer id, add the new layer
        if(layer_id == new_layer_id)
            new_layers.push_back(layer);

        // Push the other layer in any case
        new_layers.push_back(_tile_layers[layer_id]);
    }

    // Once done, we can swap the data, replacing the layers with the one inserted.
    _tile_layers.swap(new_layers);
}

void Grid::DeleteLayer(uint32 layer_id)
{
    if(layer_id >= _tile_layers.size())
        return;

    uint32 layer = 0;
    std::vector<Layer>::iterator it = _tile_layers.begin();
    std::vector<Layer>::iterator it_end = _tile_layers.end();
    for(; it != it_end; ++it) {
        if(layer == layer_id) {
            _tile_layers.erase(it);
            break;
        }
        ++layer;
    }
}

void Grid::InsertRow(uint32 /*tile_index_y*/)
{
// See bugs #153 & 154 as to why this function is not implemented for Windows
// TODO: Check that tile_index is within acceptable bounds
    /*
    #if !defined(WIN32)
    	uint32 row = tile_index / _width;

    	// Insert the row throughout all contexts
    	for (uint32 i = 0; i < static_cast<uint32>(context_names.size()); ++i)
    	{
    		_ground_layers[0][i].insert(_ground_layers[0][i].begin()   + row * _width, _width, -1);
    		_fringe_layers[0][i].insert(_fringe_layers[0][i].begin() + row * _width, _width, -1);
    		_sky_layers[0][i].insert(_sky_layers[0][i].begin()   + row * _width, _width, -1);
    	} // iterate through all contexts

    	_height++;
    	resize(_width * TILE_WIDTH, _height * TILE_HEIGHT);
    #endif*/
} // Grid::InsertRow(...)


void Grid::InsertCol(uint32 /*tile_index_x*/)
{
    /*
    // See bugs #153 & 154 as to why this function is not implemented for Windows
    // TODO: Check that tile_index is within acceptable bounds

    #if !defined(WIN32)
    uint32 col = tile_index % _width;

    // Insert the column throughout all contexts
    for (uint32 i = 0; i < static_cast<uint32>(context_names.size()); ++i)
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


void Grid::DeleteRow(uint32 /*tile_index_y*/)
{
    /*
    // See bugs #153 & 154 as to why this function is not implemented for Windows
    // TODO: Check that tile_index is within acceptable bounds
    // TODO: Check that deleting this row does not cause map height to fall below
    //       minimum allowed value

    #if !defined(WIN32)
    uint32 row = tile_index / _width;

    // Delete the row throughout each context
    for (uint32 i = 0; i < static_cast<uint32>(context_names.size()); ++i)
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


void Grid::DeleteCol(uint32 /*tile_index_x*/)
{
    /*
    // See bugs #153 & 154 as to why this function is not implemented for Windows
    // TODO: Check that tile_index is within acceptable bounds
    // TODO: Check that deleting this column does not cause map width to fall below
    //       minimum allowed value

    #if !defined(WIN32)
    uint32 col = tile_index % _width;

    // Delete the column throughout each contexts
    for (uint32 i = 0; i < static_cast<uint32>(context_names.size()); ++i)
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

std::vector<QTreeWidgetItem *> Grid::getLayerItems()
{
    std::vector<QTreeWidgetItem *> layers_names;
    for(uint32 layer_id = 0; layer_id < _tile_layers.size(); ++layer_id) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        // Check for empty names
        QString name = QString::fromStdString(_tile_layers[layer_id].name);
        if(name.size() == 0)
            name = QString::number(layer_id);

        item->setText(0, QString::number(layer_id));

        // Show the visible icon depending on the layer state
        if(_tile_layers[layer_id].visible)
            item->setIcon(1, QIcon(QString("img/misc/editor-tools/eye.png")));

        item->setText(2, name);
        item->setText(3, tr(getTypeFromLayer(_tile_layers[layer_id].layer_type).c_str()));
        layers_names.push_back(item);
    }

    return layers_names;
}

////////////////////////////////////////////////////////////////////////////////
// Grid class -- protected functions
////////////////////////////////////////////////////////////////////////////////

void Grid::drawForeground(QPainter *painter)
{
    //render();

    /*
    int32 x, y;                  // tile array loop index
    int layer_index;
    int tileset_index;               // index into the tileset_names vector
    int tile_index;                  // ranges from 0-255
    int left_tile;
    int right_tile;
    int top_tile;
    int bottom_tile;

    if(_initialized == false)
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
    while(y <= bottom_tile) {
        for(uint32 layer_id = 0; layer_id < _tile_layers.size(); ++layer_id) {
            // Don't draw the layer if it's not visible
            if(!_tile_layers[layer_id].visible)
                continue;

            layer_index = _tile_layers[layer_id].tiles[y][x];
            // Draw tile if one exists at this location
            if(layer_index != -1) {
                tileset_index = layer_index / 256;
                // Don't divide by zero
                if(tileset_index == 0)
                    tile_index = layer_index;
                else
                    tile_index = layer_index % (tileset_index * 256);
                tilesets[tileset_index]->tiles[tile_index].Draw();
            } // a tile exists to draw

        }

        if(x == right_tile) {
            x = left_tile;
            y++;
            VideoManager->MoveRelative(-(right_tile - left_tile), 1.0f);
        } else {
            x++;
            VideoManager->MoveRelative(1.0f, 0.0f);
        }
    }

    // Draw selection rectangle if this mode is active
    if(_select_on) {
        Color blue_selection(0.0f, 0.0f, 255.0f, 0.5f);

        // Start drawing from the top left
        VideoManager->Move(left_tile, top_tile);

        x = left_tile;
        y = top_tile;
        while(y <= bottom_tile) {
            layer_index = _select_layer[y][x];
            // Draw tile if one exists at this location
            if(layer_index != -1)
                VideoManager->DrawRectangle(1.0f, 1.0f, blue_selection);

            if(x == right_tile) {
                x = left_tile;
                y++;
                VideoManager->MoveRelative(-(right_tile - left_tile), 1.0f);
            } else {
                x++;
                VideoManager->MoveRelative(1.0f, 0.0f);
            }
        } // iterate through selection layer
    } // selection rectangle must be viewable

    // If grid is toggled on, draw it
    if(_grid_on)
        VideoManager->DrawGrid(0.0f, 0.0f, 1.0f, 1.0f, Color::black);
    */
} // void Grid::paintGL()


void Grid::resizeScene(int w, int h)
{
    setSceneRect(0, 0, w, h);
    //VideoManager->SetResolution(w, h);
    //VideoManager->ApplySettings();
} // Grid::resizeGL(...)

} // namespace vt_editor
