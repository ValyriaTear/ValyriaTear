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

#include "tileset.h"
#include "sprites.h"

namespace hoa_editor {

//! \brief Different tile layers in the map.
enum LAYER_TYPE
{
	INVALID_LAYER = -1,
	LOWER_LAYER   =  0,
	MIDDLE_LAYER  =  1,
	UPPER_LAYER   =  2,
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

	std::vector<int32>& GetLayer(LAYER_TYPE layer, int context);

	void SetFileName(QString filename) { _file_name = filename; }
	void SetHeight(uint32 height)      { _height    = height; _changed = true; }
	void SetWidth(uint32 width)        { _width     = width;  _changed = true; }
	void SetContext(uint32 context)    { _context   = context; }
	void SetChanged(bool value)        { _changed   = value; }

	void SetInitialized(bool ready) { _initialized = ready; }

	void SetGridOn(bool value)   { _grid_on   = value; updateGL(); }
	void SetSelectOn(bool value) { _select_on = value; updateGL(); }

	void SetDebugTexturesOn(bool value) { _debug_textures_on = value; updateGL(); }

	void SetLLOn(bool value) { _ll_on = value; updateGL(); }
	void SetMLOn(bool value) { _ml_on = value; updateGL(); }
	void SetULOn(bool value) { _ul_on = value; updateGL(); }
	void SetOLOn(bool value) { _ol_on = value; updateGL(); }
	//@}

	/** \brief Creates a new context for each layer.
	*** \param inherit_context The index of the context to inherit from.
	**/
	void CreateNewContext(uint32 inherit_context);

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

	//! \brief List of the tileset names being used.
	QStringList tileset_names;

	//! \brief A vector which contains a pointer to each tileset and the tiles it has loaded via LoadMultiImage.
	std::vector<Tileset*> tilesets;

	//! \brief A list which contains a pointer to each sprite; using list because of its efficiency.
	std::list<MapSprite*> sprites;

	/** \brief The names of each individual context
	*** \note Maximum size is 32 entries, the maximum amount of contexts that a
	***       single map supports.
	**/
	QStringList context_names;

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
	//! \brief When TRUE the lower layer of tiles is displayed.
	bool _ll_on;
	//! \brief When TRUE the middle layer of tiles is displayed.
	bool _ml_on;
	//! \brief When TRUE the upper layer of tiles is displayed.
	bool _ul_on;
	//! \brief When TRUE the object layer of tiles is displayed.
	bool _ol_on;

	//! \brief A vector of tiles in the lower layer.
	std::vector<std::vector<int32> > _lower_layer;
	//! \brief A vector of tiles in the middle layer.
	std::vector<std::vector<int32> > _middle_layer;
	//! \brief A vector of tiles in the upper layer.
	std::vector<std::vector<int32> > _upper_layer;
	//! \brief A vector of sprites in the object layer.
	std::vector<int32> _object_layer;

	/** \brief A vector of tiles in the selection rectangle.
	***
	*** This data exists only in the editor and is not a part of the map file
	*** nor the game. It acts similar to an actual tile layer as far as drawing
	*** is concerned.
	**/
	std::vector<int32> _select_layer;
}; // class Grid : public QGLWidget

} // namespace hoa_editor

#endif // __GRID_HEADER__
