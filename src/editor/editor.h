////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    editor.h
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for editor's main window and user interface.
*** ***************************************************************************/

#ifndef __EDITOR_HEADER__
#define __EDITOR_HEADER__

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
namespace vt_editor
{

class EditorScrollArea;


class Editor: public QMainWindow
{
    //! Macro needed to use Qt's slots and signals.
    Q_OBJECT

    // Needed for tile editing and accessing the map properties.
    friend class Grid;
    friend class MapPropertiesDialog;
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
    void _SetupMainView();

    //! \name View Menu Item Slots
    //! \brief These slots process selection for their item in the View menu.
    //{@
    void _ViewToggleGrid();
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
    void _MapProperties();
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

    QAction *_map_properties_action;

    QAction *_help_action;
    QAction *_about_action;
    QAction *_about_qt_action;
    //@}

    //! Tabbed widget of tilesets.
    QTabWidget *_ed_tabs;

    QTreeWidget *_ed_layer_view;

    // The map data handling and custom QGraphicsScene class
    Grid *_grid;

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

    //! The stack that contains the undo and redo operations.
    QUndoStack *_undo_stack;

    //! The editor global script: Used to run some global function needed there.
    vt_script::ReadScriptDescriptor _global_script;
}; // class Editor


class LayerCommand: public QUndoCommand
{
    // Needed for accessing the current map's layers.
    friend class Editor;
    friend class EditorScrollView;

public:
    LayerCommand(std::vector<QPoint> indeces, std::vector<int32> previous,
                 std::vector<int32> modified, uint32 layer_id, Editor *editor,
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

    //! A reference to the main window so we can get the current map.
    Editor *_editor;
}; // class LayerCommand: public QUndoCommand

} // namespace vt_editor

#endif
// __EDITOR_HEADER__
