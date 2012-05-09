///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    grid.h
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \brief   Header file for the editor grid.
*** **************************************************************************/

#ifndef __GRID_HEADER__
#define __GRID_HEADER__

#ifdef _VS
	#include <GL/glew.h>
#endif
#include <QGLWidget>
#include <QStringList>
#include <QMessageBox>
#include <QTreeWidgetItem>

#include "tileset.h"
#include "sprites.h"

namespace hoa_editor {

//! \brief Different tile layers in the map.
enum LAYER_TYPE
{
	INVALID_LAYER = -1,
	GROUND_LAYER  =  0,
	FRINGE_LAYER  =  1,
	SKY_LAYER     =  2,
	SELECT_LAYER  =  3,
	OBJECT_LAYER  =  4,
	TOTAL_LAYER   =  5
};

//! \brief All types of objects that may be found in the object layer.
enum OBJECT_TYPE
{
	INVALID_OBJECT        = -1,
	VIRTUAL_SPRITE_OBJECT =  0,
	SPRITE_OBJECT         =  1,
	TOTAL_OBJECT          =  2
};


LAYER_TYPE& operator++(LAYER_TYPE& value, int dummy);

class EditorScrollView;

// A simplified struct used to pass everything but the tiles info
struct LayerInfo {
	std::string name;
	LAYER_TYPE layer_type;

	LayerInfo()
	{ layer_type = GROUND_LAYER; }
};

class Layer {
public:
	std::string name;
	LAYER_TYPE layer_type;
	// Represents the tile indeces: i.e: tiles[y][x] = tile_id at (x,y)
	std::vector< std::vector<int32> > tiles;

	Layer()
	{ layer_type = GROUND_LAYER; }

	// Resize a layer to the given map size
	void Resize(uint32 width, uint height)
	{
		tiles.resize(height);
		for (uint32 y = 0; y < height; ++y)
			tiles[y].resize(width);
	}

	// Fill a layer with the given tile index value.
	void Fill (int32 tile_id = -1)
	{
		for (uint32 y = 0; y < tiles.size(); ++y)
		{
			for (uint32 x = 0; x < tiles[y].size(); ++x)
				tiles[y][x] = tile_id;
		}
	}
};

struct Context {

	Context():
		inherit_from_context_id(-1)
	{}

	std::string name;
	std::vector<Layer> layers;

	// Tells the context id the current context inherit from
	// This means that the parent context will be used as a base, and the current
	// context will only have its own differences from it.
	// At least, the base context can't a parent context, thus marking it as -1
	// in that case.
	// Note that a context cannot inherit from itself, and cannot inherit
	// from a context with a higher id then its own (to avoid very complicated and useless
	// use cases.
	int32 inherit_from_context_id;
};

LAYER_TYPE getLayerType(const std::string& type);
std::string getTypeFromLayer(const LAYER_TYPE& type);


/** ***************************************************************************
*** \brief Used for the OpenGL map portion where tiles are painted and edited.
***
*** This class utilizes the video engine from the game to draw all of the tiles
*** and objects to the editor's main window screen. Its members store all of
*** the map data that the editor can manipulate.
***
*** \note The tileset images are not loaded by this class. They are created elsewhere
*** and then this class is populated with those images. Only after this class has
*** initialized its own members and received the tileset data is it ready for normal
*** operation. It is the responsibility of the user of this widget to call
*** SetInitialized(true), which will enable this class' drawing operation.
*** **************************************************************************/
class Grid : public QGLWidget
{
	Q_OBJECT     // macro needed to use QT's slots and signals

public:
	Grid(QWidget *parent = 0, const QString &name = QString("Untitled"),
	     uint32 width = 0, uint32 height = 0);

	~Grid();

	//! \brief Class member accessor functions
	//@{
	QString GetFileName() const { return _file_name; }
	uint32  GetHeight()   const { return _height; }
	uint32  GetWidth()    const { return _width; }
	uint32  GetContext()  const { return _context; }
	bool    GetChanged()  const { return _changed; }

	std::vector<Layer>& GetLayers(int context)
	{ return _tile_contexts[context].layers; }

	std::vector<std::vector<int32> >& GetSelectionLayer()
	{ return _select_layer; }

	QStringList GetContextNames();

	// Fill the selection layer with the empty tile (-1) value.
	void ClearSelectionLayer();

	void SetFileName(QString filename) { _file_name = filename; }
	void SetHeight(uint32 height)      { _height    = height; _changed = true; }
	void SetWidth(uint32 width)        { _width     = width;  _changed = true; }
	void SetContext(uint32 context)    { _context   = context; }
	void SetChanged(bool value)        { _changed   = value; }

	void SetInitialized(bool ready) { _initialized = ready; }

	void SetGridOn(bool value)   { _grid_on   = value; updateGL(); }
	void SetSelectOn(bool value) { _select_on = value; updateGL(); }

	void SetDebugTexturesOn(bool value) { _debug_textures_on = value; updateGL(); }
	//@}

	/** \brief Creates a new context for each layer.
	*** \param inherit_context The index of the context to inherit from.
	**/
	bool CreateNewContext(std::string name, int32 inherit_context);

	/** \brief Loads a map from a Lua file when the user selects "Open Map"
	***        from the "File" menu.
	*** \return True only when the map data was loaded successfully
	***
	*** \note Loading of the tileset images is not performed in this function.
	***       This operation is done via the Editor::FileOpen() during the
	***       creation of the TilesetTable object(s).
	**/
	bool LoadMap();

	/** \brief Saves the map to a Lua file when the user selects "Save",
	***        "Save as", or "Quit" from the "File" menu.
	**/
	void SaveMap();

	/** \brief Add a new layer
	***
	*** depending on its type, the layer will be added after the last one
	*** of the same type.
	**/
	void AddLayer(const LayerInfo& layer_info);

	/** \name Context Modification Functions (Right-Click)
	*** \brief Functions to insert or delete rows or columns of tiles from the
	***        map.
	*** \param tile_index An ID (range: {0, width * height - 1}) of the tile
	***        used to determine the row or column upon which to perform the
	***        operation.
	***
	*** \note This feature is accessed by right-clicking on the map. It could
	***       be used elsewhere if the proper tile index is passed as a
	***       parameter.
	**/
	//{@
	void InsertRow(uint32 tile_index);
	void InsertCol(uint32 tile_index);
	void DeleteRow(uint32 tile_index);
	void DeleteCol(uint32 tile_index);
	//@}

	//! \brief List the layer names and types
	// Be sure to set the parent of the items returned, and add them in the corresponding Qt widget.
	std::vector<QTreeWidgetItem*> getLayerNames();

	//! \brief Gets the custom scripting data and stores it in an string buffer
	void GetScriptingData();
	//! \brief the scripting lines writtien after the map data.
	std::string after_text;

	//! \brief The map name presented to the player
	QString map_name;

	//! \brief The map image filename used to represent the map.
	QString map_image_filename;

	//! \brief List of the tileset names being used.
	QStringList tileset_names;

	//! \brief A vector which contains a pointer to each tileset and the tiles it has loaded via LoadMultiImage.
	std::vector<Tileset*> tilesets;

	//! \brief A list which contains a pointer to each sprite; using list because of its efficiency.
	std::list<MapSprite*> sprites;

	//! \brief A list storing the background music filenames.
	QStringList music_files;

	//! \brief Pointer to scrollview
	EditorScrollView * _ed_scrollview;

protected:
	//! \brief Sets up the rendering context of the OpenGL portion of the editor.
	void initializeGL();

	//! \brief Paints the entire map with the Allacrost video engine.
	void paintGL();

	//! \brief Performs a resize operation of the OpenGL widget when appropriate.
	void resizeGL(int w, int h);

private:
	// Computes the next layer id to put for the givent layer type,
	// Used when creating a new layer.
	uint32 _GetNextLayerId(const LAYER_TYPE& layer_type);

	//! \brief The map's file name.
	QString _file_name;
	//! \brief The height of the map in tiles.
	uint32 _height;
	//! \brief The width of the map in tiles.
	uint32 _width;
	//! \brief The active context for editing tiles.
	uint32 _context;

	//! \brief When TRUE the map has been modified.
	bool _changed;
	//! \brief When TRUE the map is ready to be drawn.
	bool _initialized;
	//! \brief When TRUE the grid between tiles is displayed.
	bool _grid_on;
	//! \brief When TRUE the rectangle of chosen tiles is displayed.
	bool _select_on;
	//! \brief When TRUE the texture sheets are displayed.
	bool _debug_textures_on;
	//! \brief When TRUE the object layer of tiles is displayed.
	bool _ol_on;

	//! \brief A vector of contexts containing the tiles in the each layers.
	std::vector<Context> _tile_contexts;

	/** \brief A vector of tiles in the selection rectangle.
	***
	*** This data exists only in the editor and is not a part of the map file
	*** nor the game. It acts similar to an actual tile layer as far as drawing
	*** is concerned.
	**/
	std::vector<std::vector<int32> > _select_layer;
}; // class Grid : public QGLWidget

} // namespace hoa_editor

#endif // __GRID_HEADER__
