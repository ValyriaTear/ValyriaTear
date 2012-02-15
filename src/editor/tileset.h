///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    tileset.h
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \brief   Header file for editor's tileset, used for maintaining a visible
***          "list" of tiles to select from for painting on a map.
*** **************************************************************************/

#ifndef __TILESET_HEADER__
#define __TILESET_HEADER__

//#include <QHeaderView>
#include <QImageReader>
#include <QRect>
#include <Q3Table>
//#include <QTableWidgetItem>
#include <QVariant>

#include "defs.h"
#include "script.h"
#include "utils.h"
#include "video.h"

//! All calls to the editor are wrapped in this namespace.
namespace hoa_editor
{

//! \brief Standard tile dimensions in number of pixels.
//@{
const unsigned int TILE_WIDTH  = 32;
const unsigned int TILE_HEIGHT = 32;
//@}


/** ***************************************************************************
*** \brief Represents an animated tile
*** **************************************************************************/
struct AnimatedTileData
{
	//! \brief Index into tileset represents tile which will be part of the
	//         animation sequence.
	uint32 tile_id;
	//! \brief Time in milliseconds to display this particular tile.
	uint32 time;
};


/** ***************************************************************************
*** \brief Represents a tileset and retains the tileset's image and properties
***
*** This is a container of tileset data. The tileset's properties are contained
*** within a Lua file specific to the tileset. The Lua file is located in a
*** separate path from the tileset's image file. Currently this class assumes
*** and only supports the Allacrost standard tileset of 512x512 pixels with
*** 32x32 pixel tiles (256 total tiles in one tileset file).
***
*** \todo Add support for animated tiles (display, editing)
*** **************************************************************************/
class Tileset
{
public:
	Tileset();

	~Tileset();

	/** \brief Returns the filename of a tileset image given the tileset's name
	*** \param tileset_name The name of the tileset (e.g. "mountain_village")
	**/
	static QString CreateImageFilename(const QString& tileset_name);

	/** \brief Returns the filename of a tileset definition file given the
	***        tileset's name
	*** \param tileset_name The name of the tileset (e.g. "mountain_village")
	**/
	static QString CreateDataFilename(const QString& tileset_name);

	/** \brief Returns the tileset name that corresponds to either an image or
	***        data filename
	*** \param filename The name of the file, which may or may not include the
	***                 path
	**/
	static QString CreateTilesetName(const QString& filename);

	//! \brief Class member accessor functions
	//@{
	bool IsInitialized() const { return _initialized; }
	//@}

	/** \brief Creates a new tileset object using only a tileset image
	*** \param img_filename The path + name of the image file to use for the
	***                     tileset
	*** \param one_image If true, the tiles vector will contain a single image
	***                  for the entire tileset
	*** \return True if the tileset image was loaded successfully
	*** \note A tileset image is required to use this function, but nothing else
	**/
	virtual bool New(const QString& img_filename, bool one_image = false);

	/** \brief Loads the tileset definition file and stores its data in the
	***        class containers
	*** \param set_name The unique name that identifies the tileset (not a
	***                 filename)
	*** \param one_image If true, the tiles vector will contain a single image
	***                  for the entire tileset
	*** \return True if the tileset was loaded successfully
	*** \note This function will clear the previously loaded contents when it
	***       is called
	**/
	virtual bool Load(const QString& set_name, bool one_image = false);

	/** \brief Saves the tileset data to its tileset definition file
	*** \return True if the save operation was successful
	**/
	bool Save();

	//! \brief The name of the tileset this table is representing.
	QString tileset_name;

	//! \brief Contains the StillImage tiles of the tileset, used in grid.cpp.
	std::vector<hoa_video::StillImage> tiles;

	//! \brief Contains walkability information for each tile.
	std::map<int, std::vector<int32> > walkability;

	//! \brief Contains autotiling information for any autotileable tile.
	std::map<int, std::string> autotileability;

protected:
	//! \brief True if the class is holding valid, loaded tileset data.
	bool _initialized;

	//! \brief Contains animated tile information for any animated tile.
	std::vector<std::vector<AnimatedTileData> > _animated_tiles;
}; // class Tileset


/** ***************************************************************************
*** \brief Used to visually represent a tileset via a QT table
*** **************************************************************************/
class TilesetTable : public Tileset
{
public:
	TilesetTable();

	~TilesetTable();

	//! \note Inherited methods from Tileset class that need to be overridden
	//@{
// 	bool New(const QString& img_filename);
	bool Load(const QString& set_name);
	//@}

	//! Reference to the table implementation of this tileset
	Q3Table* table;
}; // class TilesetTable : public Tileset

} // namespace hoa_editor

#endif // __TILESET_HEADER__
