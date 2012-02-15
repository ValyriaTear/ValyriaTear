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
#include <Q3ScrollView>
#include <QSplitter>
#include <QSpinBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>
#include <QUndoCommand>

#include "dialog_boxes.h"
#include "grid.h"
#include "sprites.h"
#include "skill_editor.h"
#include "tileset_editor.h"

//! \brief All editor code is contained within this namespace.
namespace hoa_editor {

//! \brief Various modes for tile editing
enum TILE_MODE_TYPE {
	INVALID_TILE   = -1,
	PAINT_TILE     = 0,
	MOVE_TILE      = 1,
	DELETE_TILE    = 2,
	TOTAL_TILE     = 3
};


//! \brief Represents different types of transition patterns for autotileable tiles.
enum TRANSITION_PATTERN_TYPE
{
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


class EditorScrollView;


class Editor: public QMainWindow {
	//! Macro needed to use Qt's slots and signals.
	Q_OBJECT

	// Needed for tile editing and accessing the map properties.
	friend class EditorScrollView;
	friend class MapPropertiesDialog;
	friend class MusicDialog;
	friend class ContextPropertiesDialog;
	friend class LayerCommand;

public:
	Editor();
	~Editor();

protected:
	/** \brief Handles close and/or quit events.
	*** \param QCloseEvent* Pointer to a QT close event object
	*** \note Method overloaded from QMainWindow.
	**/
	void closeEvent(QCloseEvent*);

private slots:
	//! \name Menu-Toolbar Setup Slots
	//! \brief These slots are used to gray out items in the menus and toolbars.
	//{@
	void _FileMenuSetup();
	void _ViewMenuSetup();
	void _TilesEnableActions();
	void _TilesetMenuSetup();
	void _MapMenuSetup();
	void _ScriptMenuSetup();
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

	//! \name View Menu Item Slots
	//! \brief These slots process selection for their item in the View menu.
	//{@
	void _ViewToggleGrid();
	void _ViewToggleLL();
	void _ViewToggleML();
	void _ViewToggleUL();
	void _ViewToggleOL();
	void _ViewCoordTile();
	void _ViewCoordCollision();
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
	void _TileEditLL();
	void _TileEditML();
	void _TileEditUL();
	void _TileEditOL();
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

	//! \name Script Menu Item Slots
	//! \brief These slots handle the events for the Script menu
	//{@
	void _ScriptEditSkills();
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
private:
	//! Helper function to the constructor, creates actions for use by menus
	//! and toolbars.
	void _CreateActions();
	//! Helper function to the constructor, creates the actual menus.
	void _CreateMenus();
	//! Helper function to the constructor, creates the actual toolbars.
	void _CreateToolbars();

	//! \brief Used to determine if it is safe to erase the current map.
	//!        Will prompt the user for action: to save or not to save.
	//! \return True if user decided to save the map or intentionally erase it;
	//!         False if user canceled the operation.
	bool _EraseOK();

	//! \name Application Menus
	//! \brief These are used to represent various menus found in the menu bar.
	//{@
	QMenu* _file_menu;
	QMenu* _view_menu;
	QMenu* _tiles_menu;
	QMenu* _map_menu;
	QMenu* _help_menu;
	QMenu* _tileset_menu;
	QMenu* _script_menu;
	//@}

	//! \name Application Toolbars
	//! \brief These are used to represent various toolbars found in the main window.
	//{@
	QToolBar* _tiles_toolbar;
	//@}

	//! \name Application Menu Actions
	//! \brief These are Qt's way of associating the same back-end functionality to occur whether a user
	//!        invokes a menu through the menu bar, a keyboard shortcut, a toolbar button, or other means.
	//{@
	QAction* _new_action;
	QAction* _open_action;
	QAction* _save_as_action;
	QAction* _save_action;
	QAction* _close_action;
	QAction* _quit_action;

	QAction* _toggle_grid_action;
	QAction* _toggle_ll_action;
	QAction* _toggle_ml_action;
	QAction* _toggle_ul_action;
	QAction* _toggle_ol_action;
	QAction* _coord_tile_action;
	QAction* _coord_collision_action;
	QAction* _view_textures_action;

	QAction* _undo_action;
	QAction* _redo_action;
	QAction* _layer_fill_action;
	QAction* _layer_clear_action;
	QAction* _toggle_select_action;
	QAction* _mode_paint_action;
	QAction* _mode_move_action;
	QAction* _mode_delete_action;
	QAction* _edit_ll_action;
	QAction* _edit_ml_action;
	QAction* _edit_ul_action;
	QAction* _edit_ol_action;
	QActionGroup* _mode_group;
	QActionGroup* _edit_group;

	QAction* _edit_tileset_action;

	QAction* _context_properties_action;
	QAction* _map_properties_action;
	QAction* _select_music_action;

	QAction* _edit_skill_action;

	QAction* _help_action;
	QAction* _about_action;
	QAction* _about_qt_action;
	//@}

	//! Tabbed widget of tilesets.
	QTabWidget* _ed_tabs;

	//! Used to add scrollbars to the QGLWidget of the map.
	EditorScrollView* _ed_scrollview;

	//! The skills editor window
	SkillEditor *_skill_editor;

	//! Used as the main widget in the editor since it enables user-sizable sub-widgets.
	QSplitter* _ed_splitter;

	//! Grid toggle view switch.
	bool _grid_on;

	//! Selection rectangle toggle view switch.
	bool _select_on;

	//! Textures toggle view switch.
	bool _textures_on;

	//! Lower layer toggle view switch.
	bool _ll_on;

	//! Middle layer toggle view switch.
	bool _ml_on;

	//! Upper layer toggle view switch.
	bool _ul_on;

	//! Object layer toggle view switch.
	bool _ol_on;

	//! Coordinate display type (0 = tile, 1 = collision, 2 = classic)
	int32 _coord_type;

	//! The stack that contains the undo and redo operations.
	QUndoStack* _undo_stack;

	//! The combobox that allows the user to change the current map context
	//! for editing. Contains a list of all existing contexts.
	QComboBox* _context_cbox;

	//! An error dialog for exceeding the maximum allowable number of contexts.
	QErrorMessage* _error_max_contexts;
}; // class Editor


// TODO: Q3ScrollView is a deprecated QT class. It should be replaced to a QT4 equivalent class such as QScrollArea
class EditorScrollView : public Q3ScrollView {
	//! Macro needed to use Qt's slots and signals.
	Q_OBJECT

	//! Needed for changing the editing mode and painting, and accessing the map's properties.
	friend class Editor;
	friend class MapPropertiesDialog;
	friend class MusicDialog;
	friend class ContextPropertiesDialog;
	friend class LayerCommand;

public:
	EditorScrollView(QWidget* parent, const QString& name, int width, int height);
	~EditorScrollView();

	//! Resizes the map.
	//! \param width Width of the map.
	//! \param height Height of the map.
	void Resize(int width, int height);

	//! Gets currently edited layer
	std::vector<int32>& GetCurrentLayer();

protected:
	//! \name Mouse Processing Functions
	//! \brief Functions to process mouse events on the map.
	//! \note Reimplemented from QScrollView.
	//! \param evt A pointer to the QMouseEvent generated by the mouse.
	//{@
	void contentsMousePressEvent(QMouseEvent *evt);
	void contentsMouseMoveEvent(QMouseEvent *evt);
	void contentsMouseReleaseEvent(QMouseEvent *evt);
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
	void _PaintTile(int32 index);
	//void _MoveTile(int32 index);
	void _DeleteTile(int32 index);
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
	void _AutotileRandomize(int32& tileset_num, int32& tile_index);
	void _AutotileTransitions(int32& tileset_num, int32& tile_index, const std::string tile_group);
	TRANSITION_PATTERN_TYPE _CheckForTransitionPattern(const std::string current_group,
		const std::vector<std::string>& surrounding_groups, std::string& border_group);
	//@}

	//! \name Context Menu Actions
	//! \brief These are Qt's way of associating the same back-end functionality to occur whether a user
	//!        invokes a menu through the menu bar, a keyboard shortcut, a toolbar button, or other means.
	//{@
	QAction* _insert_row_action;
	QAction* _insert_column_action;
	QAction* _delete_row_action;
	QAction* _delete_column_action;
	//@}

	//! Current working map.
	Grid* _map;
	//! Current tile edit mode being used.
	TILE_MODE_TYPE _tile_mode;
	//! Current layer being edited.
	LAYER_TYPE _layer_edit;
	//! Mouse is at this tile index on the map.
	int32 _tile_index;
	//! Menu used on right-clicks of the mouse on the map.
	QMenu* _context_menu;

	//! Stores first index, i.e. beginning, of the selection rectangle.
	int32 _first_corner_index;
	//! Stores source index of the moved tile.
	int32 _move_source_index;
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
	std::vector<int32> _tile_indeces;  //! A vector of tile indeces in the map that were modified by a command.
	std::vector<int32> _previous_tiles;//! A vector of indeces into tilesets of the modified tiles before they were modified.
	std::vector<int32> _modified_tiles;//! A vector of indeces into tilesets of the modified tiles after they were modified.
	//@}
}; // class EditorScrollView : public Q3ScrollView



class LayerCommand: public QUndoCommand {
	// Needed for accessing the current map's layers.
	friend class Editor;
	friend class EditorScrollView;

public:
	LayerCommand(std::vector<int32> indeces, std::vector<int32> previous,
		std::vector<int32> modified, LAYER_TYPE layer, int context, Editor* editor,
		const QString& text = "Layer Operation", QUndoCommand* parent = 0);

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
	std::vector<int32> _tile_indeces;  //! A vector of tile indeces in the map that were modified by this command.
	std::vector<int32> _previous_tiles;//! A vector of indeces into tilesets of the modified tiles before they were modified.
	std::vector<int32> _modified_tiles;//! A vector of indeces into tilesets of the modified tiles after they were modified.
	//@}

	//! Indicates which map layer this command was performed upon.
	LAYER_TYPE _edited_layer;

	//! A record of the active context when this command was performed.
	int _context;

	//! A reference to the main window so we can get the current map.
	Editor* _editor;
}; // class LayerCommand: public QUndoCommand

} // namespace hoa_editor

#endif
// __EDITOR_HEADER__
