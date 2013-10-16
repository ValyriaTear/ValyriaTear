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

#include "utils/utils_random.h"

#include <QScrollBar>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>

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
    QGraphicsScene(),
    _ed_scrollarea(NULL),
    _file_name(name),
    _height(height),
    _width(width),
    _changed(false),
    _initialized(false),
    _grid_on(true),
    _select_on(false)
{
    // Blue selection tile with 50% transparency
    _blue_square = QPixmap(32, 32);
    _blue_square.fill(QColor(0, 0, 255, 125));

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
    _graphics_view = new QGraphicsView(parent);
    _graphics_view->setRenderHints(QPainter::Antialiasing);
    _graphics_view->setBackgroundBrush(QBrush(Qt::black));
    _graphics_view->setScene(this);
    _graphics_view->setMouseTracking(true);

    UpdateScene();

    // Set default editing modes.
    _tile_mode  = PAINT_TILE;
    _layer_id = 0;
    _moving     = false;

    // Clear the undo/redo vectors.
    _tile_indeces.clear();
    _previous_tiles.clear();
    _modified_tiles.clear();

    // Create menu actions related to the Context menu.
    _insert_row_action = new QAction("Insert row", this);
    _insert_row_action->setStatusTip("Inserts a row of empty tiles on all layers above the currently selected tile");
    connect(_insert_row_action, SIGNAL(triggered()), this, SLOT(_MapInsertRow()));
    _insert_column_action = new QAction("Insert column", this);
    _insert_column_action->setStatusTip("Inserts a column of empty tiles on all layers to the left of the currently selected tile");
    connect(_insert_column_action, SIGNAL(triggered()), this, SLOT(_MapInsertColumn()));
    _delete_row_action = new QAction("Delete row", this);
    _delete_row_action->setStatusTip("Deletes the currently selected row of tiles from all layers");
    connect(_delete_row_action, SIGNAL(triggered()), this, SLOT(_MapDeleteRow()));
    _delete_column_action = new QAction("Delete column", this);
    _delete_column_action->setStatusTip("Deletes the currently selected column of tiles from all layers");
    connect(_delete_column_action, SIGNAL(triggered()), this, SLOT(_MapDeleteColumn()));

    // Context menu creation.
    _context_menu = new QMenu(_graphics_view);
    _context_menu->addAction(_insert_row_action);
    _context_menu->addAction(_insert_column_action);
    _context_menu->addSeparator();
    _context_menu->addAction(_delete_row_action);
    _context_menu->addAction(_delete_column_action);
} // Grid constructor


Grid::~Grid()
{
    for(std::vector<Tileset *>::iterator it = tilesets.begin();
            it != tilesets.end(); ++it)
        delete *it;

    delete _context_menu;
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
            tileset_def_names.push_back(read_data.ReadString(i).c_str());
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

    UpdateScene();

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

std::vector<std::vector<int32> >& Grid::GetCurrentLayer()
{
    return GetLayers()[_layer_id].tiles;
}

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

    UpdateScene();
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

    UpdateScene();
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

void Grid::UpdateScene()
{
    if(_initialized == false)
        return;

    // Setup drawing parameters
    clear();
    setSceneRect(0, 0, _width * TILE_WIDTH, _height * TILE_HEIGHT);
    setBackgroundBrush(QBrush(Qt::gray));

    // Start drawing from the top left
    for (uint32 x = 0; x < _width; ++x) {
        for (uint32 y = 0; y < _height; ++y) {
            for(uint32 layer_id = 0; layer_id < _tile_layers.size(); ++layer_id) {
                // Don't draw the layer if it's not visible
                if(!_tile_layers[layer_id].visible)
                    continue;

                 int32 layer_index = _tile_layers[layer_id].tiles[y][x];
                 // Draw tile if one exists at this location
                 if(layer_index == -1)
                     continue;

                int32 tileset_index = layer_index / 256;
                // Don't divide by zero
                int32 tile_index = 0;
                if(tileset_index == 0)
                    tile_index = layer_index;
                else
                    tile_index = layer_index % (tileset_index * 256);
                addPixmap(tilesets[tileset_index]->tiles[tile_index])->setPos(x * TILE_WIDTH, y * TILE_HEIGHT);
            }

            // Draw the selection square
            if(!_select_on)
                continue;

            int32 select_layer_index = _select_layer[y][x];
            if(select_layer_index == -1)
                continue;
            addPixmap(_blue_square)->setPos(x * TILE_WIDTH, y * TILE_HEIGHT);
        }
    }
/*
    // Draw selection rectangle if this mode is active
    if(_select_on) {

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
*/
    // If grid is toggled on, draw it
    if(_grid_on)
        _DrawGrid();

} // void Grid::UpdateScene()

void Grid::_DrawGrid()
{
    for (uint32 y = 0; y < (_height * TILE_HEIGHT); y+=32) {
        for (uint32 x = 0; x < (_width * TILE_WIDTH); x+=32) {
            addLine(0, y, _width * TILE_WIDTH, y, QPen(Qt::DotLine));
            addLine(x, 0, x, _height * TILE_HEIGHT, QPen(Qt::DotLine));
        }
    }
}

void Grid::Resize(int w, int h)
{
    setSceneRect(0, 0, w, h);
    _width = w;
    _height = h;
    _changed = true;
    UpdateScene();
} // Grid::Resize(...)

void Grid::mousePressEvent(QGraphicsSceneMouseEvent *evt)
{
    // get reference to Editor
    Editor *editor = static_cast<Editor *>(_graphics_view->topLevelWidget());

    // Takes in account the current scrolling
    int32 x = evt->scenePos().x();
    int32 y = evt->scenePos().y();

    // don't draw outside the map
    if((y / TILE_HEIGHT) >= static_cast<uint32>(GetHeight()) ||
            (x / TILE_WIDTH)  >= static_cast<uint32>(GetWidth()) ||
            x < 0 || y < 0)
        return;

    SetChanged(true);

    // record location of pressed tile
    _tile_index_x = x / TILE_WIDTH;
    _tile_index_y = y / TILE_HEIGHT;

    // record the location of the beginning of the selection rectangle
    if(evt->button() == Qt::LeftButton && editor->_select_on == true &&
            _moving == false) {
        _first_corner_index_x = _tile_index_x;
        _first_corner_index_y = _tile_index_y;
        GetSelectionLayer()[_tile_index_y][_tile_index_x] = 1;
    } // selection mode is on


    switch(_tile_mode) {
    case PAINT_TILE: { // start painting tiles
        if(evt->button() == Qt::LeftButton && editor->_select_on == false)
            _PaintTile(_tile_index_x, _tile_index_y);

        break;
    } // edit mode PAINT_TILE

    case MOVE_TILE: { // start moving a tile
        // select tiles
        _move_source_index_x = _tile_index_x;
        _move_source_index_y = _tile_index_y;
        if(editor->_select_on == false)
            _moving = true;
        break;
    } // edit mode MOVE_TILE

    case DELETE_TILE: { // start deleting tiles
        if(evt->button() == Qt::LeftButton && editor->_select_on == false)
            _DeleteTile(_tile_index_x, _tile_index_y);
        break;
    } // edit mode DELETE_TILE

    default:
        QMessageBox::warning(_graphics_view, "Tile editing mode",
                             "ERROR: Invalid tile editing mode!");
    } // switch on tile editing mode

    // Draw the changes.
    UpdateScene();
    return;
} // void Grid::mousePressEvent(QGraphicsSceneMouseEvent *evt)


void Grid::mouseMoveEvent(QGraphicsSceneMouseEvent *evt)
{
    // get reference to Editor
    Editor *editor = static_cast<Editor *>(_graphics_view->topLevelWidget());

    // Takes in account the current scrolling
    int32 x = evt->scenePos().x();
    int32 y = evt->scenePos().y();

    // don't draw outside the map
    if((y / TILE_HEIGHT) >= static_cast<uint32>(GetHeight()) ||
            (x / TILE_WIDTH)  >= static_cast<uint32>(GetWidth()) ||
            x < 0 || y < 0) {
        editor->statusBar()->clearMessage();
        return;
    }

    int32 index_x = x / TILE_WIDTH;
    int32 index_y = y / TILE_HEIGHT;

    if(index_x != _tile_index_x || index_y != _tile_index_y) { // user has moved onto another tile
        _tile_index_x = index_x;
        _tile_index_y = index_y;

        if(evt->buttons() == Qt::LeftButton && editor->_select_on == true &&
                _moving == false) {
            // Calculate the actual selection rectangle here, otherwise it's just
            // like selecting individual tiles...
            int x_old = _first_corner_index_x;
            int y_old = _first_corner_index_y;
            int x_new = _tile_index_x;
            int y_new = _tile_index_y;

            // Swap the coordinates around so *_old is always smaller than *_new.
            int temp;
            if(x_old > x_new) {
                temp = x_old;
                x_old = x_new;
                x_new = temp;
            }
            if(y_old > y_new) {
                temp = y_old;
                y_old = y_new;
                y_new = temp;
            }

            for(int y = y_old; y <= y_new; y++)
                for(int x = x_old; x <= x_new; x++)
                    GetSelectionLayer()[y][x] = 1;
        } // left mouse button was pressed and selection mode is on

        switch(_tile_mode) {
        case PAINT_TILE: { // continue painting tiles
            if(evt->buttons() == Qt::LeftButton && editor->_select_on == false)
                _PaintTile(_tile_index_x, _tile_index_y);

            break;
        } // edit mode PAINT_TILE

        case MOVE_TILE: { // continue moving a tile
            break;
        } // edit mode MOVE_TILE

        case DELETE_TILE: { // continue deleting tiles
            if(evt->buttons() == Qt::LeftButton && editor->_select_on == false)
                _DeleteTile(_tile_index_x, _tile_index_y);

            break;
        } // edit mode DELETE_TILE

        default:
            QMessageBox::warning(_graphics_view, "Tile editing mode",
                                 "ERROR: Invalid tile editing mode!");
        } // switch on tile editing mode
    } // mouse has moved to a new tile position

    // Display mouse position in tile and collision coordinates format
    QString position;
    // Tile position
    position = QString("Tiles: (x: %1  y: %2)").arg(static_cast<double>(x / TILE_WIDTH), 0, 'f', 0).arg(
                   static_cast<double>(y / TILE_HEIGHT), 0, 'f', 0);
    // Collision coordinates
    position.append(QString(" / Collision: (x: %1  y: %2)").arg(static_cast<double>(x * 2 / TILE_WIDTH), 0, 'f', 0).arg(
                        static_cast<double>(y * 2 / TILE_HEIGHT), 0, 'f', 0));
    // Sprite coordinates
    position.append(QString(" / Sprites: (x: %1  y: %2)").arg(x * 2 / static_cast<float>(TILE_WIDTH), 0, 'f', 1).arg(
                        y * 2 / static_cast<float>(TILE_HEIGHT), 0, 'f', 1));
    editor->statusBar()->showMessage(position);

    // Draw the changes.
    UpdateScene();
    return;
} // void Grid::mouseMoveEvent(QGraphicsSceneMouseEvent *evt)



void Grid::mouseReleaseEvent(QGraphicsSceneMouseEvent *evt)
{
    // get reference to Editor so we can access the undo stack
    Editor *editor = static_cast<Editor *>(_graphics_view->topLevelWidget());

    // Takes in account the current scrolling
    int32 mouse_x = evt->scenePos().x();
    int32 mouse_y = evt->scenePos().y();

    switch(_tile_mode) {
    case PAINT_TILE: { // wrap up painting tiles
        if(editor->_select_on == true) {
            std::vector<std::vector<int32> > select_layer = GetSelectionLayer();
            for(int32 y = 0; y < static_cast<int32>(select_layer.size()); ++y) {
                for(int32 x = 0; x < static_cast<int32>(select_layer[y].size()); ++x) {
                    // Works because the selection layer and the current layer
                    // have the same size.
                    if(select_layer[y][x] != -1)
                        _PaintTile(x, y);

                } // x
            } // y
        } // only if painting a bunch of tiles

        // Push command onto the undo stack.
        LayerCommand *paint_command = new LayerCommand(_tile_indeces,
                _previous_tiles, _modified_tiles, _layer_id, editor, "Paint");
        editor->_undo_stack->push(paint_command);
        _tile_indeces.clear();
        _previous_tiles.clear();
        _modified_tiles.clear();
        break;
    } // edit mode PAINT_TILE

    case MOVE_TILE: { // wrap up moving tiles
        if(_moving == true) {
            // record location of released tile
            _tile_index_x = mouse_x / TILE_WIDTH;
            _tile_index_y = mouse_y / TILE_HEIGHT;
            std::vector<std::vector<int32> >& layer = GetCurrentLayer();

            if(editor->_select_on == false) {
                // Record information for undo/redo action.
                //_tile_indeces.push_back(_move_source_index);
                _previous_tiles.push_back(layer[_move_source_index_y][_move_source_index_x]);
                _modified_tiles.push_back(-1);
                //_tile_indeces.push_back(_tile_index);
                _previous_tiles.push_back(layer[_tile_index_y][_tile_index_x]);
                _modified_tiles.push_back(layer[_move_source_index_y][_move_source_index_x]);

                // Perform the move.
                layer[_tile_index_y][_tile_index_x] = layer[_move_source_index_y][_move_source_index_x];
                layer[_move_source_index_y][_move_source_index_x] = -1;
            } // only moving one tile at a time
            else {
                std::vector<std::vector<int32> > select_layer = GetSelectionLayer();
                for(int32 y = 0; y < static_cast<int32>(select_layer.size()); ++y) {
                    for(int32 x = 0; x < static_cast<int32>(select_layer[y].size()); ++x) {
                        // Works because the selection layer and the current layer
                        // have the same size.
                        if(select_layer[y][x] != -1) {
                            // Record information for undo/redo action.
                            _tile_indeces.push_back(QPoint(x, y));
                            _previous_tiles.push_back(layer[y][x]);
                            _modified_tiles.push_back(-1);
                            _tile_indeces.push_back(QPoint(x + _tile_index_x - _move_source_index_x, y + _tile_index_y - _move_source_index_y));
                            _previous_tiles.push_back(layer[y + _tile_index_y - _move_source_index_y][x + _tile_index_x - _move_source_index_x]);
                            _modified_tiles.push_back(layer[y][x]);

                            // Perform the move.
                            layer[y + _tile_index_y - _move_source_index_y][x + _tile_index_x - _move_source_index_x] = layer[y][x];
                            layer[y][x] = -1;
                        } // only if current tile is selected
                    } // x
                } // y
            } // moving a bunch of tiles at once

            // Push command onto the undo stack.
            LayerCommand *move_command = new LayerCommand(_tile_indeces,
                    _previous_tiles, _modified_tiles, _layer_id, editor, "Move");
            editor->_undo_stack->push(move_command);
            _tile_indeces.clear();
            _previous_tiles.clear();
            _modified_tiles.clear();
        } // moving tiles and not selecting them

        break;
    } // edit mode MOVE_TILE

    case DELETE_TILE: { // wrap up deleting tiles
        if(editor->_select_on == true) {
            std::vector<std::vector<int32> > select_layer = GetSelectionLayer();
            for(int32 y = 0; y < static_cast<int32>(select_layer.size()); ++y) {
                for(int32 x = 0; x < static_cast<int32>(select_layer[y].size()); ++x) {
                    // Works because the selection layer and the current layer
                    // are the same size.
                    if(select_layer[y][x] != -1)
                        _DeleteTile(x, y);
                } // x
            } // y
        } // only if deleting a bunch of tiles

        // Push command onto undo stack.
        LayerCommand *delete_command = new LayerCommand(_tile_indeces,
                _previous_tiles, _modified_tiles, _layer_id, editor, "Delete");
        editor->_undo_stack->push(delete_command);
        _tile_indeces.clear();
        _previous_tiles.clear();
        _modified_tiles.clear();
        break;
    } // edit mode DELETE_TILE

    default:
        QMessageBox::warning(_graphics_view, "Tile editing mode",
                             "ERROR: Invalid tile editing mode!");
    } // switch on tile editing mode

    // Clear the selection layer.
    if((_tile_mode != MOVE_TILE || _moving == true) && editor->_select_on == true) {
        ClearSelectionLayer();
    } // clears when not moving tiles or when moving tiles and not selecting them

    if(editor->_select_on == true && _moving == false && _tile_mode == MOVE_TILE)
        _moving = true;
    else
        _moving = false;

    // Draw the changes.
    UpdateScene();
    return;
} // void Grid::mouseReleaseEvent(QGraphicsSceneMouseEvent *evt)



void Grid::contextMenuEvent(QGraphicsSceneContextMenuEvent *evt)
{
    // Takes in account the current scrolling
    int32 mouse_x = evt->scenePos().x();
    int32 mouse_y = evt->scenePos().y();

    // Don't popup a menu outside the map.
    if ((mouse_y / TILE_HEIGHT) >= static_cast<uint32>(GetHeight()) ||
            (mouse_x / TILE_WIDTH)  >= static_cast<uint32>(GetWidth()) ||
            mouse_x < 0 || mouse_y < 0)
        return;

    _tile_index_x = mouse_x / TILE_WIDTH;
    _tile_index_y = mouse_y / TILE_HEIGHT;
    _context_menu->exec(QCursor::pos());
    (static_cast<Editor *>(_graphics_view->topLevelWidget()))->statusBar()->clearMessage();
}



void Grid::keyPressEvent(QKeyEvent *evt)
{
    if(evt->key() == Qt::Key_Delete) {
        // TODO: Handle object deletion
    }
}

///////////////////////////////////////////////////////////////////////////////
// Ex-EditorScrollView class -- private slots
///////////////////////////////////////////////////////////////////////////////

void Grid::_MapInsertRow()
{
    InsertRow(_tile_index_y);
}



void Grid::_MapInsertColumn()
{
    InsertCol(_tile_index_x);
}



void Grid::_MapDeleteRow()
{
    DeleteRow(_tile_index_y);
}



void Grid::_MapDeleteColumn()
{
    DeleteCol(_tile_index_x);
}


///////////////////////////////////////////////////////////////////////////////
// Ex-EditorScrollView class -- private functions
///////////////////////////////////////////////////////////////////////////////

void Grid::_PaintTile(int32 index_x, int32 index_y)
{
    // get reference to current tileset
    Editor *editor = static_cast<Editor *>(_graphics_view->topLevelWidget());
    QTableWidget *table = static_cast<QTableWidget *>(editor->_ed_tabs->currentWidget());
    QString tileset_name = editor->_ed_tabs->tabText(editor->_ed_tabs->currentIndex());

    // Detect the first selection range and use to paint an area
    QList<QTableWidgetSelectionRange> selections = table->selectedRanges();
    QTableWidgetSelectionRange selection;
    if(selections.size() > 0)
        selection = selections.at(0);

    int32 multiplier = tileset_def_names.indexOf(tileset_name);
    if(multiplier == -1) {
        tileset_def_names.append(tileset_name);
        multiplier = tileset_def_names.indexOf(tileset_name);
    } // calculate index of current tileset

    if(selections.size() > 0 && (selection.columnCount() * selection.rowCount() > 1)) {
        // Draw tiles from tileset selection onto map, one tile at a time.
        for(int32 i = 0; i < selection.rowCount() && index_y + i < (int32)GetHeight(); i++) {
            for(int32 j = 0; j < selection.columnCount() && index_x + j < (int32)GetWidth(); j++) {
                int32 tileset_index = (selection.topRow() + i) * 16 + (selection.leftColumn() + j);

                // perform randomization for autotiles
                _AutotileRandomize(multiplier, tileset_index);

                // Record information for undo/redo action.
                _tile_indeces.push_back(QPoint(index_x + j, index_y + i));
                _previous_tiles.push_back(GetCurrentLayer()[index_y + i][index_x + j]);
                _modified_tiles.push_back(tileset_index + multiplier * 256);

                GetCurrentLayer()[index_y + i][index_x + j] = tileset_index + multiplier * 256;
            } // iterate through columns of selection
        } // iterate through rows of selection
    } // multiple tiles are selected
    else {
        // put selected tile from tileset into tile array at correct position
        int32 tileset_index = table->currentRow() * 16 + table->currentColumn();

        // perform randomization for autotiles
        _AutotileRandomize(multiplier, tileset_index);

        // Record information for undo/redo action.
        _tile_indeces.push_back(QPoint(index_x, index_y));
        _previous_tiles.push_back(GetCurrentLayer()[index_y][index_x]);
        _modified_tiles.push_back(tileset_index + multiplier * 256);

        GetCurrentLayer()[index_y][index_x] = tileset_index + multiplier * 256;
    } // a single tile is selected
}



void Grid::_DeleteTile(int32 index_x, int32 index_y)
{
    // Record information for undo/redo action.
    _tile_indeces.push_back(QPoint(index_x, index_y));
    _previous_tiles.push_back(GetCurrentLayer()[index_y][index_x]);
    _modified_tiles.push_back(-1);

    // Delete the tile.
    GetCurrentLayer()[index_y][index_x] = -1;
}



void Grid::_AutotileRandomize(int32 &tileset_num, int32 &tile_index)
{
    std::map<int, std::string>::iterator it = tilesets[tileset_num]->
            autotileability.find(tile_index);

    if(it != tilesets[tileset_num]->autotileability.end()) {
        // Set up for opening autotiling.lua.
        ReadScriptDescriptor read_data;
        if(read_data.OpenFile("dat/tilesets/autotiling.lua") == false)
            QMessageBox::warning(_graphics_view, "Loading File...",
                                 QString("ERROR: could not open dat/tilesets/autotiling.lua for reading!"));

        read_data.OpenTable(it->second);
        int32 random_index = vt_utils::RandomBoundedInteger(1, static_cast<int32>(read_data.GetTableSize()));
        read_data.OpenTable(random_index);
        std::string tileset_name = read_data.ReadString(1);
        tile_index = read_data.ReadInt(2);
        read_data.CloseTable();
        tileset_num = tileset_def_names.indexOf(
                          QString(tileset_name.c_str()));
        read_data.CloseTable();

        read_data.CloseFile();

        _AutotileTransitions(tileset_num, tile_index, it->second);
    } // must have an autotileable tile
}



void Grid::_AutotileTransitions(int32 &/*tileset_num*/, int32 &/*tile_index*/, const std::string &/*tile_group*/)
{
    /*
    // These 2 vectors have a one-to-one correspondence. They should always
    // contain 8 entries.
    vector<int32>  existing_tiles;   // This vector will contain all the tiles around the current painted tile that need to be examined.
    vector<string> existing_groups;  // This vector will contain the autotileable groups of the existing tiles.

    // These booleans are used to know whether the current tile being painted is on the edge of the map.
    // This will affect the transition/border algorithm.
    //bool top_edge    = (_tile_index - _map->GetWidth()) < 0;
    bool top_edge    =  _tile_index < (int32)_map->GetWidth();
    bool bottom_edge = (_tile_index + _map->GetWidth()) >= (_map->GetWidth() * _map->GetHeight());
    bool left_edge   = ( _tile_index    % _map->GetWidth()) == 0;
    bool right_edge  = ((_tile_index+1) % _map->GetWidth()) == 0;


    // Now figure out which tiles surround the current painted one and put them into the existing_tiles vector.
    if (!top_edge)
    {
        if (!left_edge)
            existing_tiles.push_back(GetCurrentLayer()[_tile_index - _map->GetWidth() - 1]);
        else
            existing_tiles.push_back(-1);
        existing_tiles.push_back(GetCurrentLayer()[_tile_index - _map->GetWidth()]);
        if (!right_edge)
            existing_tiles.push_back(GetCurrentLayer()[_tile_index - _map->GetWidth() + 1]);
        else
            existing_tiles.push_back(-1);
    } // make sure there is a row of tiles above the painted one
    else
    {
        existing_tiles.push_back(-1);
        existing_tiles.push_back(-1);
        existing_tiles.push_back(-1);
    } // these tiles don't exist

    if (!left_edge)
        existing_tiles.push_back(GetCurrentLayer()[_tile_index - 1]);
    else
        existing_tiles.push_back(-1);

    if (!right_edge)
        existing_tiles.push_back(GetCurrentLayer()[_tile_index + 1]);
    else
        existing_tiles.push_back(-1);

    if (!bottom_edge)
    {
        if (!left_edge)
            existing_tiles.push_back(GetCurrentLayer()[_tile_index + _map->GetWidth() - 1]);
        else
            existing_tiles.push_back(-1);
        existing_tiles.push_back(GetCurrentLayer()[_tile_index + _map->GetWidth()]);
        if (!right_edge)
            existing_tiles.push_back(GetCurrentLayer()[_tile_index + _map->GetWidth() + 1]);
        else
            existing_tiles.push_back(-1);
    } // make sure there is a row of tiles below the painted one
    else
    {
        existing_tiles.push_back(-1);
        existing_tiles.push_back(-1);
        existing_tiles.push_back(-1);
    } // these tiles don't exist


    // Now figure out what groups the existing tiles belong to.
    for (unsigned int i = 0; i < existing_tiles.size(); i++)
    {
        int32 multiplier    = existing_tiles[i] / 256;
        int32 tileset_index = existing_tiles[i] % 256;
        map<int, string>::iterator it = _map->tilesets[multiplier]->
            autotileability.find(tileset_index);

        // Here we check to make sure the tile exists in the autotileability
        // table. But if the tile in question is a transition tile with multiple
        // variations, we want to assign it a group name of "none", otherwise
        // the pattern detection algorithm won't work properly. Transition tiles
        // with multiple variations are still handled correctly.
        if (it != _map->tilesets[multiplier]->autotileability.end() &&
            it->second.find("east", 0)      == string::npos &&
            it->second.find("north", 0)     == string::npos &&
            it->second.find("_ne", 0)       == string::npos &&
            it->second.find("ne_corner", 0) == string::npos &&
            it->second.find("_nw", 0)       == string::npos &&
            it->second.find("nw_corner", 0) == string::npos &&
            it->second.find("_se", 0)       == string::npos &&
            it->second.find("se_corner", 0) == string::npos &&
            it->second.find("south", 0)     == string::npos &&
            it->second.find("_sw", 0)       == string::npos &&
            it->second.find("sw_corner", 0) == string::npos &&
            it->second.find("west", 0)      == string::npos)
            existing_groups.push_back(it->second);
        else
            existing_groups.push_back("none");
    } // iterate through the existing_tiles vector


    // Transition tiles exist only for certain patterns of tiles surrounding the painted tile.
    // Check for any of these patterns, and if one exists, transition magic begins!

    string transition_group = "none";  // autotileable grouping for the border tile if it exists
    TRANSITION_PATTERN_TYPE pattern = _CheckForTransitionPattern(tile_group, existing_groups,
        transition_group);

    if (pattern != INVALID_PATTERN)
    {
        transition_group = tile_group + "_" + transition_group;

        // Set up for opening autotiling.lua.
        ReadScriptDescriptor read_data;
        if (read_data.OpenFile("dat/tilesets/autotiling.lua", true) == false)
            QMessageBox::warning(this, "Loading File...",
                QString("ERROR: could not open dat/tilesets/autotiling.lua for reading!"));

        // Extract the correct transition tile from autotiling.lua as determined by
        // _CheckForTransitionPattern(...).
        if (read_data.DoesTableExist(transition_group) == true)
        {
            read_data.OpenTable(transition_group);

            switch (pattern)
            {
                case NW_BORDER_PATTERN:
                    //cerr << "nw_border" << std::endl;
                    read_data.OpenTable(1);
                    break;
                case N_BORDER_PATTERN:
                    //cerr << "n_border" << std::endl;
                    read_data.OpenTable(2);
                    break;
                case NE_BORDER_PATTERN:
                    //cerr << "ne_border" << std::endl;
                    read_data.OpenTable(3);
                    break;
                case E_BORDER_PATTERN:
                    //cerr << "e_border" << std::endl;
                    read_data.OpenTable(4);
                    break;
                case SE_BORDER_PATTERN:
                    //cerr << "se_border" << std::endl;
                    read_data.OpenTable(5);
                    break;
                case S_BORDER_PATTERN:
                    //cerr << "s_border" << std::endl;
                    read_data.OpenTable(6);
                    break;
                case SW_BORDER_PATTERN:
                    //cerr << "sw_border" << std::endl;
                    read_data.OpenTable(7);
                    break;
                case W_BORDER_PATTERN:
                    //cerr << "w_border" << std::endl;
                    read_data.OpenTable(8);
                    break;
                case NW_CORNER_PATTERN:
                    //cerr << "nw_corner" << std::endl;
                    read_data.OpenTable(9);
                    break;
                case NE_CORNER_PATTERN:
                    //cerr << "ne_corner" << std::endl;
                    read_data.OpenTable(10);
                    break;
                case SE_CORNER_PATTERN:
                    //cerr << "se_corner" << std::endl;
                    read_data.OpenTable(11);
                    break;
                case SW_CORNER_PATTERN:
                    //cerr << "sw_corner" << std::endl;
                    read_data.OpenTable(12);
                    break;
                default: // should never get here
                    read_data.CloseTable();
                    read_data.CloseFile();
                    QMessageBox::warning(this, "Transition detection...",
                        QString("ERROR: Invalid pattern detected! No autotiling will occur for this tile!"));
                    return;
            } // switch on transition pattern

            string tileset_name = read_data.ReadString(1);
            tile_index = read_data.ReadInt(2);
            read_data.CloseTable();
            tileset_num = _map->tileset_names.indexOf(
                QString(tileset_name.c_str()));

            read_data.CloseTable();

            // Border/transition tiles may also have variations, so randomize them.
            //assert(tileset_num != -1);
            _AutotileRandomize(tileset_num, tile_index);
        } // make sure the selected transition tiles exist

        read_data.CloseFile();
    } // make sure a transition pattern exists
    */
}

TRANSITION_PATTERN_TYPE Grid::_CheckForTransitionPattern(const std::string &current_group,
        const std::vector<std::string>& surrounding_groups, std::string &border_group)
{
    // Assumes that surrounding_groups always has 8 entries. Well, it's an error if it doesn't,
    // and technically should never happen.

    if(
        (surrounding_groups[0] == surrounding_groups[1] || surrounding_groups[0] == "none") &&
        (surrounding_groups[2] == surrounding_groups[1] || surrounding_groups[2] == "none") &&
        (surrounding_groups[1] != current_group && surrounding_groups[1] != "none" &&
         current_group != "none") &&
        (surrounding_groups[3] == current_group ||
         surrounding_groups[3] == "none" ||
         surrounding_groups[3] == surrounding_groups[1]) &&
        (surrounding_groups[4] == current_group ||
         surrounding_groups[4] == "none" ||
         surrounding_groups[4] == surrounding_groups[1]) &&
        (surrounding_groups[5] != surrounding_groups[1]) &&
        (surrounding_groups[7] != surrounding_groups[1]) &&
        (surrounding_groups[6] != surrounding_groups[1])) {
        border_group = surrounding_groups[1];
        return N_BORDER_PATTERN;
    } // check for the northern border pattern

    else if(
        (surrounding_groups[2] == surrounding_groups[4] || surrounding_groups[2] == "none") &&
        (surrounding_groups[7] == surrounding_groups[4] || surrounding_groups[7] == "none") &&
        (surrounding_groups[4] != current_group && surrounding_groups[4] != "none" &&
         current_group != "none") &&
        (surrounding_groups[1] == current_group ||
         surrounding_groups[1] == "none" ||
         surrounding_groups[1] == surrounding_groups[4]) &&
        (surrounding_groups[6] == current_group ||
         surrounding_groups[6] == "none" ||
         surrounding_groups[6] == surrounding_groups[4]) &&
        (surrounding_groups[0] != surrounding_groups[4]) &&
        (surrounding_groups[5] != surrounding_groups[4]) &&
        (surrounding_groups[3] != surrounding_groups[4])) {
        border_group = surrounding_groups[4];
        return E_BORDER_PATTERN;
    } // check for the eastern border pattern

    else if(
        (surrounding_groups[7] == surrounding_groups[6] || surrounding_groups[7] == "none") &&
        (surrounding_groups[5] == surrounding_groups[6] || surrounding_groups[5] == "none") &&
        (surrounding_groups[6] != current_group && surrounding_groups[6] != "none" &&
         current_group != "none") &&
        (surrounding_groups[3] == current_group ||
         surrounding_groups[3] == "none" ||
         surrounding_groups[3] == surrounding_groups[6]) &&
        (surrounding_groups[4] == current_group ||
         surrounding_groups[4] == "none" ||
         surrounding_groups[4] == surrounding_groups[6]) &&
        (surrounding_groups[2] != surrounding_groups[6]) &&
        (surrounding_groups[0] != surrounding_groups[6]) &&
        (surrounding_groups[1] != surrounding_groups[6])) {
        border_group = surrounding_groups[6];
        return S_BORDER_PATTERN;
    } // check for the southern border pattern

    else if(
        (surrounding_groups[0] == surrounding_groups[3] || surrounding_groups[0] == "none") &&
        (surrounding_groups[5] == surrounding_groups[3] || surrounding_groups[5] == "none") &&
        (surrounding_groups[3] != current_group && surrounding_groups[3] != "none" &&
         current_group != "none") &&
        (surrounding_groups[1] == current_group ||
         surrounding_groups[1] == "none" ||
         surrounding_groups[1] == surrounding_groups[3]) &&
        (surrounding_groups[6] == current_group ||
         surrounding_groups[6] == "none" ||
         surrounding_groups[6] == surrounding_groups[3]) &&
        (surrounding_groups[2] != surrounding_groups[3]) &&
        (surrounding_groups[7] != surrounding_groups[3]) &&
        (surrounding_groups[4] != surrounding_groups[3])) {
        border_group = surrounding_groups[3];
        return W_BORDER_PATTERN;
    } // check for the western border pattern

    else if(
        (surrounding_groups[1] == surrounding_groups[0]) &&
        (surrounding_groups[3] == surrounding_groups[0]) &&
        (surrounding_groups[0] != current_group && surrounding_groups[0] != "none" &&
         current_group != "none") &&
        (surrounding_groups[4] == current_group || surrounding_groups[4] == "none") &&
        (surrounding_groups[6] == current_group || surrounding_groups[6] == "none") &&
        (surrounding_groups[7] != surrounding_groups[0])) {
        border_group = surrounding_groups[0];
        return NW_BORDER_PATTERN;
    } // check for the northwestern border pattern

    else if(
        (surrounding_groups[1] == surrounding_groups[2]) &&
        (surrounding_groups[4] == surrounding_groups[2]) &&
        (surrounding_groups[2] != current_group && surrounding_groups[2] != "none" &&
         current_group != "none") &&
        (surrounding_groups[3] == current_group || surrounding_groups[3] == "none") &&
        (surrounding_groups[6] == current_group || surrounding_groups[6] == "none") &&
        (surrounding_groups[5] != surrounding_groups[2])) {
        border_group = surrounding_groups[2];
        return NE_BORDER_PATTERN;
    } // check for the northeastern border pattern

    else if(
        (surrounding_groups[4] == surrounding_groups[7]) &&
        (surrounding_groups[6] == surrounding_groups[7]) &&
        (surrounding_groups[7] != current_group && surrounding_groups[7] != "none" &&
         current_group != "none") &&
        (surrounding_groups[1] == current_group || surrounding_groups[1] == "none") &&
        (surrounding_groups[3] == current_group || surrounding_groups[3] == "none") &&
        (surrounding_groups[0] != surrounding_groups[7])) {
        border_group = surrounding_groups[7];
        return SE_BORDER_PATTERN;
    } // check for the southeastern border pattern

    else if(
        (surrounding_groups[3] == surrounding_groups[5]) &&
        (surrounding_groups[6] == surrounding_groups[5]) &&
        (surrounding_groups[5] != current_group && surrounding_groups[5] != "none" &&
         current_group != "none") &&
        (surrounding_groups[1] == current_group || surrounding_groups[1] == "none") &&
        (surrounding_groups[4] == current_group || surrounding_groups[4] == "none") &&
        (surrounding_groups[2] != surrounding_groups[5])) {
        border_group = surrounding_groups[5];
        return SW_BORDER_PATTERN;
    } // check for the southwestern border pattern

    else if(
        (surrounding_groups[0] != current_group && surrounding_groups[0] != "none" &&
         current_group != "none") &&
        (surrounding_groups[1] == current_group || surrounding_groups[1] == "none") &&
        (surrounding_groups[3] == current_group || surrounding_groups[3] == "none") &&
        (surrounding_groups[2] != surrounding_groups[0]) &&
        (surrounding_groups[4] != surrounding_groups[0]) &&
        (surrounding_groups[5] != surrounding_groups[0]) &&
        (surrounding_groups[6] != surrounding_groups[0]) &&
        (surrounding_groups[7] != surrounding_groups[0])) {
        border_group = surrounding_groups[0];
        return NW_CORNER_PATTERN;
    } // check for the northwestern corner pattern

    else if(
        (surrounding_groups[2] != current_group && surrounding_groups[2] != "none" &&
         current_group != "none") &&
        (surrounding_groups[1] == current_group || surrounding_groups[1] == "none") &&
        (surrounding_groups[4] == current_group || surrounding_groups[4] == "none") &&
        (surrounding_groups[0] != surrounding_groups[2]) &&
        (surrounding_groups[3] != surrounding_groups[2]) &&
        (surrounding_groups[5] != surrounding_groups[2]) &&
        (surrounding_groups[6] != surrounding_groups[2]) &&
        (surrounding_groups[7] != surrounding_groups[2])) {
        border_group = surrounding_groups[2];
        return NE_CORNER_PATTERN;
    } // check for the northeastern corner pattern

    else if(
        (surrounding_groups[7] != current_group && surrounding_groups[7] != "none" &&
         current_group != "none") &&
        (surrounding_groups[4] == current_group || surrounding_groups[4] == "none") &&
        (surrounding_groups[6] == current_group || surrounding_groups[6] == "none") &&
        (surrounding_groups[0] != surrounding_groups[7]) &&
        (surrounding_groups[1] != surrounding_groups[7]) &&
        (surrounding_groups[2] != surrounding_groups[7]) &&
        (surrounding_groups[3] != surrounding_groups[7]) &&
        (surrounding_groups[5] != surrounding_groups[7])) {
        border_group = surrounding_groups[7];
        return SE_CORNER_PATTERN;
    } // check for the southeastern corner pattern

    else if(
        (surrounding_groups[5] != current_group && surrounding_groups[5] != "none" &&
         current_group != "none") &&
        (surrounding_groups[3] == current_group || surrounding_groups[3] == "none") &&
        (surrounding_groups[6] == current_group || surrounding_groups[6] == "none") &&
        (surrounding_groups[0] != surrounding_groups[5]) &&
        (surrounding_groups[1] != surrounding_groups[5]) &&
        (surrounding_groups[2] != surrounding_groups[5]) &&
        (surrounding_groups[4] != surrounding_groups[5]) &&
        (surrounding_groups[7] != surrounding_groups[5])) {
        border_group = surrounding_groups[5];
        return SW_CORNER_PATTERN;
    } // check for the southwestern corner pattern

    return INVALID_PATTERN;
} // TRANSITION_PATTERN_TYPE EditorScrollView::_CheckForTransitionPattern(...)

} // namespace vt_editor
