///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    grid.h
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the editor grid.
*** **************************************************************************/

#ifndef __GRID_HEADER__
#define __GRID_HEADER__

#include <QGraphicsScene>
#include <QStringList>
#include <QMessageBox>
#include <QTreeWidgetItem>

#include "tileset.h"

namespace vt_editor
{

//! \brief The map tile minimum width and height
const uint32 map_min_width = 16;
const uint32 map_min_height = 12;

//! \brief Represents different types of transition patterns for autotileable tiles.
enum TRANSITION_PATTERN_TYPE {
    INVALID_PATTERN     = -1,
    NW_BORDER_PATTERN   = 0,
    N_BORDER_PATTERN    = 1,
    NE_BORDER_PATTERN   = 2,
    E_BORDER_PATTERN    = 3,
    SE_BORDER_PATTERN   = 4,
    S_BORDER_PATTERN    = 5,
    SW_BORDER_PATTERN   = 6,
    W_BORDER_PATTERN    = 7,
    NW_CORNER_PATTERN   = 8,
    NE_CORNER_PATTERN   = 9,
    SE_CORNER_PATTERN   = 10,
    SW_CORNER_PATTERN   = 11,
    TOTAL_PATTERN       = 12
};

//! \brief Various modes for tile editing
enum TILE_MODE_TYPE {
    INVALID_TILE   = -1,
    PAINT_TILE     = 0,
    MOVE_TILE      = 1,
    DELETE_TILE    = 2,
    TOTAL_TILE     = 3
};

//! \brief Different tile layers in the map.
enum LAYER_TYPE {
    INVALID_LAYER = -1,
    GROUND_LAYER  =  0,
    SKY_LAYER     =  1,
    SELECT_LAYER  =  2,
    TOTAL_LAYER   =  3
};

LAYER_TYPE &operator++(LAYER_TYPE &value, int dummy);

class EditorScrollArea;

// A simplified struct used to pass everything but the tiles info
struct LayerInfo {
    std::string name;
    LAYER_TYPE layer_type;

    LayerInfo() {
        layer_type = GROUND_LAYER;
    }
};

class Layer
{
public:
    std::string name;
    LAYER_TYPE layer_type;
    // Represents the tile indeces: i.e: tiles[y][x] = tile_id at (x,y)
    std::vector< std::vector<int32> > tiles;
    // Tells whether the layer is currently visible.
    bool visible;

    Layer() {
        layer_type = GROUND_LAYER;
        visible = true;
    }

    // Resize a layer to the given map size
    void Resize(uint32 width, uint height) {
        tiles.resize(height);
        for(uint32 y = 0; y < height; ++y)
            tiles[y].resize(width);
    }

    // Fill a layer with the given tile index value.
    void Fill(int32 tile_id = -1) {
        for(uint32 y = 0; y < tiles.size(); ++y) {
            for(uint32 x = 0; x < tiles[y].size(); ++x)
                tiles[y][x] = tile_id;
        }
    }
};

LAYER_TYPE getLayerType(const std::string &type);
std::string getTypeFromLayer(const LAYER_TYPE &type);


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
class Grid : public QGraphicsScene
{
    Q_OBJECT     // macro needed to use QT's slots and signals

    //! Needed for changing the editing mode and painting, and accessing the map's properties.
    friend class Editor;
    friend class MapPropertiesDialog;
    friend class LayerDialog;
    friend class LayerCommand;

public:
    Grid(QWidget *parent = 0, const QString &name = QString(tr("Untitled")),
         uint32 width = 0, uint32 height = 0);

    ~Grid();

    //! \brief Class member accessor functions
    //@{
    QString GetFileName() const {
        return _file_name;
    }
    uint32  GetHeight()   const {
        return _height;
    }
    uint32  GetWidth()    const {
        return _width;
    }
    bool    GetChanged()  const {
        return _changed;
    }

    std::vector<Layer>& GetLayers() {
        return _tile_layers;
    }

    std::vector<std::vector<int32> >& GetSelectionLayer() {
        return _select_layer;
    }

    // Fill the selection layer with the empty tile (-1) value.
    void ClearSelectionLayer();

    void SetFileName(QString filename) {
        _file_name = filename;
    }
    void SetHeight(uint32 height)      {
        _height    = height;
        UpdateScene();
        _changed = true;
    }
    void SetWidth(uint32 width)        {
        _width     = width;
        UpdateScene();
        _changed = true;
    }

    //! Tells whether the map has been modified.
    void SetChanged(bool value)        {
        _changed   = value;
    }

    void SetInitialized(bool ready) {
        _initialized = ready;
    }

    void SetGridOn(bool value)   {
        _grid_on   = value;
        UpdateScene();
    }
    void SetSelectOn(bool value) {
        _select_on = value;
        UpdateScene();
    }
    //@}

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
    void AddLayer(const LayerInfo &layer_info);

    /** \brief Delete a layer
    ***
    *** \param layer_id the layer id to delete.
    **/
    void DeleteLayer(uint32 layer_id);

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

    //! \brief List the layer names, types, ...
    // Be sure to set the parent of the items returned, and add them in the corresponding Qt widget.
    std::vector<QTreeWidgetItem *> getLayerItems();

    //! \brief Performs a resize operation of the QGraphicsScene object when appropriate.
    void Resize(int w, int h);

    //! \brief List of the tileset definition files being used.
    QStringList tileset_def_names;

    //! \brief A vector which contains a pointer to each tileset and the tiles it has loaded via LoadMultiImage.
    std::vector<Tileset *> tilesets;

    //! \brief Pointer to scrollArea
    EditorScrollArea *_ed_scrollarea;

    // Pointer to the graphic view class, used to display the graphics widgets.
    QGraphicsView* _graphics_view;

    //! \brief Paints the entire map with the video engine.
    void UpdateScene();

private:
    // Computes the next layer id to put for the givent layer type,
    // Used when creating a new layer.
    uint32 _GetNextLayerId(const LAYER_TYPE &layer_type);

    //! \brief The map's file name.
    QString _file_name;
    //! \brief The height of the map in tiles.
    uint32 _height;
    //! \brief The width of the map in tiles.
    uint32 _width;

    //! \brief When TRUE the map has been modified.
    bool _changed;
    //! \brief When TRUE the map is ready to be drawn.
    bool _initialized;
    //! \brief When TRUE the grid between tiles is displayed.
    bool _grid_on;
    //! \brief When TRUE the rectangle of chosen tiles is displayed.
    bool _select_on;

    //! The selection tile square
    QPixmap _blue_square;

    //! \brief A vector of layers.
    std::vector<Layer> _tile_layers;

    /** \brief A vector of tiles in the selection rectangle.
    ***
    *** This data exists only in the editor and is not a part of the map file
    *** nor the game. It acts similar to an actual tile layer as far as drawing
    *** is concerned.
    **/
    std::vector<std::vector<int32> > _select_layer;

    // Draw the tile grid (actually adds the line to the graphics scene)
    void _DrawGrid();

    //! Gets currently edited layer
    std::vector<std::vector<int32> >& GetCurrentLayer();

protected:
    //! \name Mouse Processing Functions
    //! \brief Functions to process mouse events on the map.
    //! \note Reimplemented from QScrollArea.
    //! \param evt A pointer to the QMouseEvent generated by the mouse.
    //{@
    void mousePressEvent(QGraphicsSceneMouseEvent *evt);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *evt);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *evt);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *evt);
    void keyPressEvent(QKeyEvent *evt);
    //@}

private slots:
    //! \name Contextual Menu Slots
    //! \brief These slots process selection for their item in the contextual menu,
    //!        which pops up on right-clicks of the mouse on the map.
    //{@
    void _MapInsertRow();
    void _MapInsertColumn();
    void _MapDeleteRow();
    void _MapDeleteColumn();
    //@}

private:
    //! \name Tile Editing Functions
    //! \brief These functions perform the gritty details of tile modification
    //!        such as painting, deleting, and moving.
    //! \param index The index on the map/grid of the tile to modify.
    //{@
    void _PaintTile(int32 x, int32 y);
    //void _MoveTile(int32 index);
    void _DeleteTile(int32 x, int32 y);
    //@}

    //! \name Autotiling Functions
    //! \brief These functions perform all the nitty gritty details associated
    //!        with autotiling. _AutotileRandomize randomizes tiles being painted
    //!        on the map, and _AutotileTransitions calculates which tiles need
    //!        border transitions from one tile group to the next.
    //!        _CheckForTransitionPattern checks tiles surrounding the current tile
    //!        for patterns necessary to put in a transition tile. It's a helper to
    //!        _AutotileTransitions.
    //! \param tileset_num The index of the specified tileset as loaded in the
    //!                    QTabWidget.
    //! \param tile_index The index of the selected tile in its tileset.
    //! \param tile_group The autotileable group that the current tile belongs to.
    //{@
    void _AutotileRandomize(int32 &tileset_num, int32 &tile_index);
    void _AutotileTransitions(int32 &tileset_num, int32 &tile_index, const std::string &tile_group);
    TRANSITION_PATTERN_TYPE _CheckForTransitionPattern(const std::string &current_group,
            const std::vector<std::string>& surrounding_groups, std::string &border_group);
    //@}

    //! \name Context Menu Actions
    //! \brief These are Qt's way of associating the same back-end functionality to occur whether a user
    //!        invokes a menu through the menu bar, a keyboard shortcut, a toolbar button, or other means.
    //{@
    QAction *_insert_row_action;
    QAction *_insert_column_action;
    QAction *_delete_row_action;
    QAction *_delete_column_action;
    //@}

    //! Current tile edit mode being used.
    TILE_MODE_TYPE _tile_mode;
    //! Current layer being edited.
    uint32 _layer_id;
    //! Mouse is at this tile index on the map.
    int32 _tile_index_x;
    int32 _tile_index_y;
    //! Menu used on right-clicks of the mouse on the map.
    QMenu *_context_menu;

    //! Stores first index, i.e. beginning, of the selection rectangle.
    int32 _first_corner_index_x;
    int32 _first_corner_index_y;
    //! Stores source index of the moved tile.
    int32 _move_source_index_x;
    int32 _move_source_index_y;
    //! Moving tiles has 2 phases to it when using the selection rectangle
    //! and hence moving more than one tile at a time. This determines which phase
    //! is in effect: false is the first phase, when the user creates the
    //! selection rectangle; true is the second phase, when the user clicks on
    //! the rectangle and moves it to another location.
    bool _moving;

    //! \name Tile Vectors
    //! \brief The following three vectors are used to know how to perform undo and redo operations
    //!        for this command. They should be the same size and one-to-one. So, the j-th element
    //!        of each vector should correspond to the j-th element of the other vectors.
    //{@
    std::vector<QPoint> _tile_indeces;  //! A vector of tile indeces in the map that were modified by a command.
    std::vector<int32> _previous_tiles;//! A vector of indeces into tilesets of the modified tiles before they were modified.
    std::vector<int32> _modified_tiles;//! A vector of indeces into tilesets of the modified tiles after they were modified.
    //@}

}; // class Grid : public QGraphicsScene

} // namespace vt_editor

#endif // __GRID_HEADER__
