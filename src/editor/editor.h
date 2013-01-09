////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    editor.h
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \brief   Header file for editor's main window and user interface.
*** ***************************************************************************/

#ifndef __EDITOR_HEADER__
#define __EDITOR_HEADER__

#include <map>

#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QErrorMessage>
#include <QFileDialog>
#include <QLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QProgressDialog>
#include <QScrollArea>
#include <QSplitter>
#include <QSpinBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>
#include <QUndoCommand>

#include "dialog_boxes.h"
#include "grid.h"
#include "tileset_editor.h"

#include "engine/script/script_read.h"

//! \brief All editor code is contained within this namespace.
namespace hoa_editor
{

//! \brief Various modes for tile editing
enum TILE_MODE_TYPE {
    INVALID_TILE   = -1,
    PAINT_TILE     = 0,
    MOVE_TILE      = 1,
    DELETE_TILE    = 2,
    TOTAL_TILE     = 3
};


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


//! \brief The maximum number of allowable contexts on a map.
const uint32 MAX_CONTEXTS = 32;


class EditorScrollArea;


class Editor: public QMainWindow
{
    //! Macro needed to use Qt's slots and signals.
    Q_OBJECT

    // Needed for tile editing and accessing the map properties.
    friend class EditorScrollArea;
    friend class MapPropertiesDialog;
    friend class MusicDialog;
    friend class ContextPropertiesDialog;
    friend class LayerDialog;
    friend class LayerCommand;

public:
    Editor();
    ~Editor();

protected:
    /** \brief Handles close and/or quit events.
    *** \param QCloseEvent* Pointer to a QT close event object
    *** \note Method overloaded from QMainWindow.
    **/
    void closeEvent(QCloseEvent *);

private slots:
    //! \name Menu-Toolbar Setup Slots
    //! \brief These slots are used to gray out items in the menus and toolbars.
    //{@
    void _FileMenuSetup();
    void _ViewMenuSetup();
    void _TilesEnableActions();
    void _TilesetMenuSetup();
    void _MapMenuSetup();
    //@}

    //! \name File Menu Item Slots
    //! \brief These slots process selection for their item in the File menu.
    //{@
    void _FileNew();
    void _FileOpen();
    void _FileSaveAs();
    void _FileSave();
    void _FileClose();
    void _FileQuit();
    //@}

    //! \brief Setup the main editor view (used for FileNew and FileOpen)
    void SetupMainView();

    //! \name View Menu Item Slots
    //! \brief These slots process selection for their item in the View menu.
    //{@
    void _ViewToggleGrid();
    void _ViewTextures();
    //@}

    //! \name Tiles Menu Item Slots
    //! \brief These slots process selection for their item in the Tiles menu.
    //{@LayerCommand
    void _TileLayerFill();
    void _TileLayerClear();
    void _TileToggleSelect();
    void _TileModePaint();
    void _TileModeMove();
    void _TileModeDelete();
    //@}

    //! \name Tileset Menu Item Slots
    //! \brief These slots process selection for their item in the Tileset menu.
    //{@
    void _TilesetEdit();
    //@}

    //! \name Map Menu Item Slots
    //! \brief These slots process selection for their item in the Map menu.
    //{@
    void _MapSelectMusic();
    void _MapProperties();
    void _MapAddContext();
    //@}

    // Handles layer interaction
    void _MapAddLayer();
    void _MapModifyLayer();
    void _MapDeleteLayer();
    void _MapMoveLayerUp();
    void _MapMoveLayerDown();
    //@}

    //! \name Help Menu Item Slots
    //! \brief These slots process selection for their item in the Help menu.
    //{@
    void _HelpHelp();
    void _HelpAbout();
    void _HelpAboutQt();
    //@}

    //! This slot switches the map context to the designated one for editing.
    void _SwitchMapContext(int context);

    //! Tells whether a given layer can be moved up or down, or deleted.
    bool _CanLayerMoveUp(QTreeWidgetItem *item) const;
    bool _CanLayerMoveDown(QTreeWidgetItem *item) const;
    bool _CanDeleteLayer(QTreeWidgetItem *item) const;

    //! Switch to the new map layer using the Item selected
    void _UpdateSelectedLayer(QTreeWidgetItem *item);

    //! Select the layer id in the layer view.
    void _SetSelectedLayer(uint32 layer_id);

    //! Toggle the current layer visibility
    void _ToggleLayerVisibility();
private:
    //! Helper function to the constructor, creates actions for use by menus
    //! and toolbars.
    void _CreateActions();
    //! Helper function to the constructor, creates the actual menus.
    void _CreateMenus();
    //! Helper function to the constructor, creates the actual toolbars.
    void _CreateToolbars();

    // Clears and refill the layer view with the current layers found in the base context.
    void _UpdateLayersView();

    bool _update_view_on_layer_change;

    //! \brief Used to determine if it is safe to erase the current map.
    //!        Will prompt the user for action: to save or not to save.
    //! \return True if user decided to save the map or intentionally erase it;
    //!         False if user canceled the operation.
    bool _EraseOK();

    //! \name Application Menus
    //! \brief These are used to represent various menus found in the menu bar.
    //{@
    QMenu *_file_menu;
    QMenu *_view_menu;
    QMenu *_tiles_menu;
    QMenu *_map_menu;
    QMenu *_help_menu;
    QMenu *_tileset_menu;
    QMenu *_script_menu;
    //@}

    //! \name Application Toolbars
    //! \brief These are used to represent various toolbars found in the main window.
    //{@
    QToolBar *_tiles_toolbar;
    //@}

    //! \name Application Menu Actions
    //! \brief These are Qt's way of associating the same back-end functionality to occur whether a user
    //!        invokes a menu through the menu bar, a keyboard shortcut, a toolbar button, or other means.
    //{@
    QAction *_new_action;
    QAction *_open_action;
    QAction *_save_as_action;
    QAction *_save_action;
    QAction *_close_action;
    QAction *_quit_action;

    QAction *_toggle_grid_action;
    QAction *_view_textures_action;

    QAction *_undo_action;
    QAction *_redo_action;
    QAction *_layer_fill_action;
    QAction *_layer_clear_action;
    QAction *_toggle_select_action;
    QAction *_mode_paint_action;
    QAction *_mode_move_action;
    QAction *_mode_delete_action;
    QAction *_edit_layer_action;
    QActionGroup *_mode_group;
    QActionGroup *_edit_group;

    QAction *_edit_tileset_action;

    QAction *_context_properties_action;
    QAction *_map_properties_action;
    QAction *_select_music_action;

    QAction *_help_action;
    QAction *_about_action;
    QAction *_about_qt_action;
    //@}

    //! Tabbed widget of tilesets.
    QTabWidget *_ed_tabs;

    //! Used to add scrollbars to the QGLWidget of the map.
    EditorScrollArea *_ed_scrollarea;

    QTreeWidget *_ed_layer_view;

    // Used to add / modify / remove layers.
    QToolBar *_ed_layer_toolbar;

    //! The layer up/down buttons reference. Used to set their enabled state depending on the selected layer.
    // Created and deleted by the layer view toolbar. Don't delete it.
    QPushButton *_layer_up_button;
    QPushButton *_layer_down_button;
    //! Delete layer button reference to prevent being able to delete the last ground layer
    QPushButton *_delete_layer_button;

    //! Used as the main widget in the editor since it enables user-sizable sub-widgets.
    QSplitter *_ed_splitter;

    QSplitter *_ed_tileset_layer_splitter;

    //! Grid toggle view switch.
    bool _grid_on;

    //! Selection rectangle toggle view switch.
    bool _select_on;

    //! Textures toggle view switch.
    bool _textures_on;

    //! The stack that contains the undo and redo operations.
    QUndoStack *_undo_stack;

    //! The combobox that allows the user to change the current map context
    //! for editing. Contains a list of all existing contexts.
    QComboBox *_context_cbox;

    //! An error dialog for exceeding the maximum allowable number of contexts.
    QErrorMessage *_error_max_contexts;

    //! The editor global script: Used to run some global function needed there.
    hoa_script::ReadScriptDescriptor _global_script;
}; // class Editor


class EditorScrollArea : public QScrollArea
{
    //! Macro needed to use Qt's slots and signals.
    Q_OBJECT

    //! Needed for changing the editing mode and painting, and accessing the map's properties.
    friend class Editor;
    friend class MapPropertiesDialog;
    friend class MusicDialog;
    friend class LayerDialog;
    friend class ContextPropertiesDialog;
    friend class LayerCommand;

public:
    EditorScrollArea(QWidget *parent, int width, int height);
    ~EditorScrollArea();

    //! Resizes the map.
    //! \param width Width of the map.
    //! \param height Height of the map.
    void Resize(int width, int height);

    //! Gets currently edited layer
    std::vector<std::vector<int32> >& GetCurrentLayer();

protected:
    //! Reimplement event handling to permit tracking mouse
    virtual bool event(QEvent *evt);

    //! \name Mouse Processing Functions
    //! \brief Functions to process mouse events on the map.
    //! \note Reimplemented from QScrollArea.
    //! \param evt A pointer to the QMouseEvent generated by the mouse.
    //{@
    bool contentsMousePressEvent(QMouseEvent *evt);
    bool contentsMouseMoveEvent(QMouseEvent *evt);
    bool contentsMouseReleaseEvent(QMouseEvent *evt);
    void contentsContextMenuEvent(QContextMenuEvent *evt);
    void keyPressEvent(QKeyEvent *evt);
    //@}

private slots:
    //! \name Context Menu Slots
    //! \brief These slots process selection for their item in the Context menu,
    //!        which pops up on right-clicks of the mouse on the map.
    //{@
    void _ContextInsertRow();
    void _ContextInsertColumn();
    void _ContextDeleteRow();
    void _ContextDeleteColumn();
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

    //! Current working map.
    Grid *_map;
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
}; // class EditorScrollView : public Q3ScrollView



class LayerCommand: public QUndoCommand
{
    // Needed for accessing the current map's layers.
    friend class Editor;
    friend class EditorScrollView;

public:
    LayerCommand(std::vector<QPoint> indeces, std::vector<int32> previous,
                 std::vector<int32> modified, uint32 layer_id, int context, Editor *editor,
                 const QString &text = "Layer Operation", QUndoCommand *parent = 0);

    //! \name Undo Functions
    //! \brief Reimplemented from the QUndoCommand class to provide specific undo/redo capability towards the map.
    //{@
    void undo();
    void redo();
    //@}

private:
    //! \name Tile Vectors
    //! \brief The following three vectors are used to know how to perform undo and redo operations
    //!        for this command. They should be the same size and one-to-one. So, the j-th element
    //!        of each vector should correspond to the j-th element of the other vectors.
    //{@
    std::vector<QPoint> _tile_indeces;  //! A vector of tile indeces in the map that were modified by this command.
    std::vector<int32> _previous_tiles;//! A vector of indeces into tilesets of the modified tiles before they were modified.
    std::vector<int32> _modified_tiles;//! A vector of indeces into tilesets of the modified tiles after they were modified.
    //@}

    //! Indicates which map layer this command was performed upon.
    uint32 _edited_layer_id;

    //! A record of the active context when this command was performed.
    int _context;

    //! A reference to the main window so we can get the current map.
    Editor *_editor;
}; // class LayerCommand: public QUndoCommand

} // namespace hoa_editor

#endif
// __EDITOR_HEADER__
