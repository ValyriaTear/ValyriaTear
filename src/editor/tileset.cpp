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
*** \file    tileset.cpp
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for editor's tileset, used for maintaining a visible
***          "list" of tiles to select from for painting on the map.
*** **************************************************************************/

#include "utils/utils_pch.h"
#include "tileset.h"

#include "engine/script/script_read.h"
#include "engine/script/script_write.h"

#include <QHeaderView>
#include <QFile>

using namespace vt_video;
using namespace vt_script;

namespace vt_editor
{

////////////////////////////////////////////////////////////////////////////////
// Tileset class -- all functions
////////////////////////////////////////////////////////////////////////////////

Tileset::Tileset() :
    _initialized(false)
{} // Tileset constructor


Tileset::~Tileset()
{
    for(std::vector<vt_video::StillImage>::iterator it = tiles.begin();
            it != tiles.end(); it++)
        (*it).Clear();
    tiles.clear();
} // Tileset destructor


QString Tileset::CreateTilesetName(const QString &filename)
{
    QString tname = filename;
    // Remove everything up to and including the final '/' character
    tname.remove(0, tname.lastIndexOf("/") + 1);
    // Chop off the appended four characters (the filename extension)
    tname.chop(4);
    return tname;
} // Tileset::CreateTilesetName(...)


bool Tileset::New(const QString &img_filename, bool one_image)
{
    if (img_filename.isEmpty())
        return false;

    _initialized = false;

    // Retrieve the tileset name from the image filename
    _tileset_name = CreateTilesetName(img_filename);
    _tileset_definition_filename = "dat/tilesets/" + _tileset_name + ".lua";

    // Check existence of a previous lua definition file
    if (QFile::exists(_tileset_definition_filename)) {
        _tileset_definition_filename.clear();
        _tileset_name.clear();
        return false;
    }

    _tileset_image_filename = "img/tilesets/" + _tileset_name + img_filename.mid(img_filename.length() - 4, 4);

    // Prepare the tile vector and load the tileset image
    if(one_image == true) {
        tiles.clear();
        tiles.resize(1);
        tiles[0].SetDimensions(16.0f, 16.0f);
        if(tiles[0].Load(img_filename.toStdString(), 16, 16) == false) {
            qDebug("Failed to load tileset image: %s",
                   img_filename.toStdString().c_str());
            return false;
        }
    } else {
        tiles.clear();
        tiles.resize(256);
        for(uint32 i = 0; i < 256; i++)
            tiles[i].SetDimensions(1.0f, 1.0f);
        if(ImageDescriptor::LoadMultiImageFromElementGrid(tiles,
                img_filename.toStdString(), 16, 16) == false) {
            qDebug("Failed to load tileset image: %s",
                   img_filename.toStdString().c_str());
            return false;
        }
    }

    // Initialize the rest of the tileset data
    std::vector<int32> blank_entry(4, 0);
    for(uint32 i = 0; i < 16; i++)
        for(uint32 j = 0; j < 16; j++)
            walkability.insert(std::make_pair(i * 16 + j, blank_entry));

    autotileability.clear();
    _animated_tiles.clear();

    _initialized = true;
    return true;
} // Tileset::New(...)


bool Tileset::Load(const QString &def_filename, bool one_image)
{
    if (def_filename.isEmpty())
        return false;

    _initialized = false;

    // Reset container data
    autotileability.clear();
    walkability.clear();
    _animated_tiles.clear();

    // Create filenames from the tileset name
    _tileset_name = CreateTilesetName(def_filename);
    _tileset_definition_filename = def_filename;

    // Set up for reading the tileset definition file.
    ReadScriptDescriptor read_data;
    if(!read_data.OpenFile(def_filename.toStdString())) {
        _initialized = false;
        return false;
    }

    if (!read_data.OpenTable("tileset")) {
        read_data.CloseFile();
        qDebug("Failed to open the 'tileset' table");
        return false;
    }

    _tileset_image_filename = QString::fromStdString(read_data.ReadString("image"));

    // Prepare the tile vector and load the tileset image
    if(one_image == true) {
        tiles.clear();
        tiles.resize(1);
        tiles[0].SetDimensions(16.0f, 16.0f);
        if (!tiles[0].Load(_tileset_image_filename.toStdString(), 16, 16))
            return false;
    } else {
        tiles.clear();
        tiles.resize(256);
        for(uint32 i = 0; i < 256; i++)
            tiles[i].SetDimensions(1.0f, 1.0f);
        if(ImageDescriptor::LoadMultiImageFromElementGrid(tiles,
                _tileset_image_filename.toStdString(), 16, 16) == false)
            return false;
    }

    // Read in autotiling information.
    if(read_data.DoesTableExist("autotiling") == true) {
        // Contains the keys (indeces, if you will) of this table's entries
        std::vector<int32> keys;
        uint32 table_size = read_data.GetTableSize("autotiling");
        read_data.OpenTable("autotiling");

        read_data.ReadTableKeys(keys);
        for(uint32 i = 0; i < table_size; i++)
            autotileability[keys[i]] = read_data.ReadString(keys[i]);
        read_data.CloseTable();
    } // make sure table exists first

    // Read in walkability information.
    if(read_data.DoesTableExist("walkability") == true) {
        std::vector<int32> vect;  // used to read in vectors from the data file
        read_data.OpenTable("walkability");

        for(int32 i = 0; i < 16; i++) {
            read_data.OpenTable(i);
            // Make sure that at least one row exists
            if(read_data.IsErrorDetected() == true) {
                read_data.CloseTable();
                read_data.CloseTable();
                read_data.CloseFile();
                _initialized = false;
                return false;
            }

            for(int32 j = 0; j < 16; j++) {
                read_data.ReadIntVector(j, vect);
                if(read_data.IsErrorDetected() == false)
                    walkability[i * 16 + j] = vect;
                vect.clear();
            } // iterate through all tiles in a row
            read_data.CloseTable();
        } // iterate through all rows of the walkability table
        read_data.CloseTable();
    } // make sure table exists first

    // Read in animated tiles.
    if(read_data.DoesTableExist("animated_tiles") == true) {
        uint32 table_size = read_data.GetTableSize("animated_tiles");
        read_data.OpenTable("animated_tiles");

        for(uint32 i = 1; i <= table_size; i++) {
            _animated_tiles.push_back(std::vector<AnimatedTileData>());
            std::vector<AnimatedTileData>& tiles = _animated_tiles.back();
            // Calculate loop end: an animated tile is comprised of a tile id
            // and a time, so the loop end is really half the table size.
            uint32 tile_count = read_data.GetTableSize(i) / 2;
            read_data.OpenTable(i);
            for(uint32 index = 1; index <= tile_count; index++) {
                AnimatedTileData anim_tile;
                anim_tile.tile_id = read_data.ReadUInt(index * 2 - 1);
                anim_tile.time    = read_data.ReadUInt(index * 2);
                tiles.push_back(anim_tile);
            } // iterate through all tiles in one animated tile
            read_data.CloseTable();
        } // iterate through all animated tiles in the table
        read_data.CloseTable();
    } // make sure table exists first

    read_data.CloseTable();
    read_data.CloseFile();

    _initialized = true;
    return true;
} // Tileset::Load(...)


bool Tileset::Save()
{
    WriteScriptDescriptor write_data;

    if (!write_data.OpenFile(_tileset_definition_filename.toStdString()))
        return false;

    // Write the main table for the tileset file
    write_data.BeginTable("tileset");
    write_data.InsertNewLine();

    // Write basic tileset properties
    write_data.WriteString("image", _tileset_image_filename.toStdString());
    write_data.WriteInt("num_tile_cols", 16);
    write_data.WriteInt("num_tile_rows", 16);
    write_data.InsertNewLine();

    // Write autotiling data
    if(autotileability.empty() == false) {
        write_data.BeginTable("autotiling");
        for(std::map<int, std::string>::iterator it = autotileability.begin();
                it != autotileability.end(); it++)
            write_data.WriteString((*it).first, (*it).second);
        write_data.EndTable();
        write_data.InsertNewLine();
    } // data must exist in order to save it

    // Write walkability data
    write_data.WriteComment("The general walkability of the tiles in the tileset. Zero indicates walkable. One tile has four walkable quadrants listed as: NW corner, NE corner, SW corner, SE corner.");
    write_data.BeginTable("walkability");
    for(uint32 row = 0; row < 16; row++) {
        write_data.BeginTable(row);
        for(uint32 col = 0; col < 16; col++)
            write_data.WriteIntVector(col, walkability[row * 16 + col]);
        write_data.EndTable();
    } // iterate through all rows of the tileset
    write_data.EndTable();
    write_data.InsertNewLine();

    // Write animated tile data
    if(_animated_tiles.empty() == false) {
        write_data.WriteComment("The animated tiles table has one row per animated tile, with each entry in a row indicating which tile in the tileset is the next part of the animation, followed by the time in ms that the tile will be displayed for.");
        write_data.BeginTable("animated_tiles");
        std::vector<uint32> vect;
        for(uint32 anim_tile = 0; anim_tile < _animated_tiles.size(); anim_tile++) {
            for(uint32 i = 0; i < _animated_tiles[anim_tile].size(); i++) {
                vect.push_back(_animated_tiles[anim_tile][i].tile_id);
                vect.push_back(_animated_tiles[anim_tile][i].time);
            } // iterate through all tiles in one animated tile
            write_data.WriteUIntVector(anim_tile + 1, vect);
            vect.clear();
        } // iterate through all animated tiles of the tileset
        write_data.EndTable(); // animated_tiles
    } // data must exist in order to save it

    write_data.EndTable(); // tileset

    if(write_data.IsErrorDetected()) {
        PRINT_ERROR << "Errors were detected when saving tileset file. The errors include: "
                    << std::endl << write_data.GetErrorMessages() << std::endl;
        write_data.CloseFile();
        return false;
    }

    write_data.CloseFile();
    return true;
} // Tileset::Save()


///////////////////////////////////////////////////////////////////////////////
// TilesetTable class -- all functions
///////////////////////////////////////////////////////////////////////////////

const uint32 num_rows = 16;
const uint32 num_cols = 16;

TilesetTable::TilesetTable() :
    Tileset()
{
    // Set up the QT table
    table = new QTableWidget(num_rows, num_cols);
    table->setShowGrid(false);
    table->setSelectionMode(QTableWidget::ContiguousSelection);
    table->setEditTriggers(QTableWidget::NoEditTriggers);
    table->setContentsMargins(0, 0, 0, 0);
    table->setDragEnabled(false);
    table->setAcceptDrops(false);
    table->setHorizontalHeaderLabels(QStringList());
    table->setVerticalHeaderLabels(QStringList());
    table->verticalHeader()->hide();
    table->verticalHeader()->setContentsMargins(0, 0, 0, 0);
    table->horizontalHeader()->hide();
    table->horizontalHeader()->setContentsMargins(0, 0, 0, 0);

    for(uint32 i = 0; i < num_rows; ++i)
        table->setRowHeight(i, TILE_HEIGHT);
    for(uint32 i = 0; i < num_cols; ++i)
        table->setColumnWidth(i, TILE_WIDTH);
} // TilesetTable constructor


TilesetTable::~TilesetTable()
{
    delete table;
} // TilesetTable destructor


bool TilesetTable::Load(const QString &def_filename)
{
    if (!Tileset::Load(def_filename))
        return false;

    // Read in tiles and create table items.
    // Historical note:
    // This was one ugly hack. It loads each individual tile's image and
    // puts it into a table. But each tile's image only exists together with a
    // bunch of other tiles in a tileset image. So we have to split them up.
    // Qt has no built-in function to split a big image into little ones (as
    // of Qt 4.4). This image information has already been loaded by the above
    // call to Tileset::Load(...). If we could somehow take that info and put
    // it into a Qt table, that would be ideal.
    //
    // This piece of code is what takes so long for the editor to load one
    // tileset.
    //
    // <<FIXED>>: The ugly hack has been fixed, I use the QImage to handle
    // directly to the bits, it's much faster. Contact me if there's any
    // problem with this fix, eguitarz (dalema22@gmail.com)
    QRect rectangle;
    QImage entire_tileset;
    entire_tileset.load(_tileset_image_filename, "png");
    for(uint32 row = 0; row < num_rows; ++row) {
        for(uint32 col = 0; col < num_cols; ++col) {
            rectangle.setRect(col * TILE_WIDTH, row * TILE_HEIGHT, TILE_WIDTH,
                              TILE_HEIGHT);
            QVariant variant = entire_tileset.copy(rectangle);
            if(!variant.isNull()) {
                QTableWidgetItem *item = new QTableWidgetItem(QTableWidgetItem::UserType);
                item->setData(Qt::DecorationRole, variant);
                item->setFlags(item->flags() &~ Qt::ItemIsEditable);

                table->setItem(row, col, item);
            } else
                qDebug("Image loading error!");
        } // iterate through the columns of the tileset
    } // iterate through the rows of the tileset


    // Select the top left item
    table->setCurrentCell(0, 0);

    return true;
} // TilesetTable::Load(...)

} // namespace vt_editor
