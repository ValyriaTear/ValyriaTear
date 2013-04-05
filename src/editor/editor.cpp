///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/*!****************************************************************************
 * \file    editor.cpp
 * \author  Philip Vorsilak, gorzuate@allacrost.org
 * \brief   Source file for editor's main window and user interface.
 *****************************************************************************/

#include "editor.h"

#include <QTableWidgetItem>
#include <QScrollBar>

using namespace hoa_utils;
using namespace hoa_script;
using namespace hoa_video;

namespace hoa_editor
{


///////////////////////////////////////////////////////////////////////////////
// Editor class -- public functions
///////////////////////////////////////////////////////////////////////////////

Editor::Editor() : QMainWindow(),
    _layer_up_button(0),
    _layer_down_button(0),
    _delete_layer_button(0)
{
    // create the undo stack
    _undo_stack = new QUndoStack();

    // set scollview to NULL because it's being checked inside _TilesEnableActions
    _ed_scrollarea = NULL;

    // create actions, menus, and toolbars
    _CreateActions();
    _CreateMenus();
    _CreateToolbars();
    _TilesEnableActions();

    connect(_undo_stack, SIGNAL(canRedoChanged(bool)), _redo_action, SLOT(setEnabled(bool)));
    connect(_undo_stack, SIGNAL(canUndoChanged(bool)), _undo_action, SLOT(setEnabled(bool)));

    // initialize viewing items
    _grid_on = false;
    _select_on = false;

    // create the main widget and layout
    _ed_splitter = new QSplitter(this);
    _ed_splitter->setOrientation(Qt::Horizontal);

    _ed_tabs = NULL;
    _ed_layer_view = NULL;
    _ed_layer_toolbar = NULL;
    setCentralWidget(_ed_splitter);
    resize(800, 600);

    _ed_tileset_layer_splitter = new QSplitter(_ed_splitter);
    _ed_tileset_layer_splitter->setOrientation(Qt::Vertical);

    // set the window icon
    setWindowIcon(QIcon("img/logos/program_icon.ico"));

    // Initialize the script manager
    ScriptManager = ScriptEngine::SingletonCreate();
    ScriptManager->SingletonInitialize();

    // Open the global script
    if (!_global_script.OpenFile("dat/config/editor.lua"))
        PRINT_ERROR << "failed to load the editor global script: " << "dat/config/editor.lua" << std::endl;
}



Editor::~Editor()
{
    if(_ed_scrollarea != NULL)
        delete _ed_scrollarea;

    if(_ed_tabs != NULL)
        delete _ed_tabs;

    if(_ed_layer_view != NULL)
        delete _ed_layer_view;

    if(_ed_layer_toolbar != NULL)
        delete _ed_layer_toolbar;

    delete _ed_tileset_layer_splitter;
    delete _ed_splitter;

    delete _undo_stack;

    VideoEngine::SingletonDestroy();

    // Close the global script.
    _global_script.CloseFile();

    // Do it last since all luabind objects must be freed before closing the lua state.
    ScriptEngine::SingletonDestroy();
}


///////////////////////////////////////////////////////////////////////////////
// Editor class -- protected function
///////////////////////////////////////////////////////////////////////////////

void Editor::closeEvent(QCloseEvent *)
{
    _FileQuit();
} // closeEvent(...)


///////////////////////////////////////////////////////////////////////////////
// Editor class -- private slots
///////////////////////////////////////////////////////////////////////////////

void Editor::_FileMenuSetup()
{
    if(_ed_scrollarea != NULL && _ed_scrollarea->_map != NULL) {
        _save_as_action->setEnabled(true);
        _save_action->setEnabled(_ed_scrollarea->_map->GetChanged());
        _close_action->setEnabled(true);
    } // map must exist in order to save or close it
    else {
        _save_as_action->setEnabled(false);
        _save_action->setEnabled(false);
        _close_action->setEnabled(false);
    } // map does not exist, can't save or close it
}



void Editor::_ViewMenuSetup()
{
    if(_ed_scrollarea != NULL && _ed_scrollarea->_map != NULL) {
        _toggle_grid_action->setEnabled(true);
    } // map must exist in order to set view options
    else {
        _toggle_grid_action->setEnabled(false);
    } // map does not exist, can't view it*/
}



void Editor::_TilesEnableActions()
{
    if(_ed_scrollarea != NULL && _ed_scrollarea->_map != NULL) {
        _undo_action->setText("Undo " + _undo_stack->undoText());
        _redo_action->setText("Redo " + _undo_stack->redoText());
        _layer_fill_action->setEnabled(true);
        _layer_clear_action->setEnabled(true);
        _toggle_select_action->setEnabled(true);
        _mode_paint_action->setEnabled(true);
        _mode_move_action->setEnabled(true);
        _mode_delete_action->setEnabled(true);
    } // map must exist in order to paint it
    else {
        _undo_action->setEnabled(false);
        _redo_action->setEnabled(false);
        _layer_fill_action->setEnabled(false);
        _layer_clear_action->setEnabled(false);
        _toggle_select_action->setEnabled(false);
        _mode_paint_action->setEnabled(false);
        _mode_move_action->setEnabled(false);
        _mode_delete_action->setEnabled(false);
    } // map does not exist, can't paint it
}



void Editor::_TilesetMenuSetup()
{
    // Don't edit tilesets if a map is open
    if(_ed_scrollarea != NULL && _ed_scrollarea->_map != NULL)
        _edit_tileset_action->setEnabled(false);
    else
        _edit_tileset_action->setEnabled(true);
}



void Editor::_MapMenuSetup()
{
    if(_ed_scrollarea != NULL && _ed_scrollarea->_map != NULL) {
        _map_properties_action->setEnabled(true);
    } // map must exist in order to set properties
    else {
        _map_properties_action->setEnabled(false);
    } // map does not exist, can't modify it
}


void Editor::SetupMainView()
{
    if(_ed_tabs != NULL)
        delete _ed_tabs;
    _ed_tabs = new QTabWidget();
    _ed_tabs->setTabPosition(QTabWidget::South);

    if(_ed_layer_view != NULL)
        delete _ed_layer_view;
    _ed_layer_view = new QTreeWidget();

    connect(_ed_layer_view,
            SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            this, SLOT(_UpdateSelectedLayer(QTreeWidgetItem *)));

    _ed_layer_view->setColumnCount(3);

    QStringList headers;
    headers << "id" << tr(" ") << tr("Layer") << tr("Type");
    _ed_layer_view->setHeaderLabels(headers);
    // Hide the id column as we'll only use it internally
    _ed_layer_view->setColumnHidden(0, true);

    // The button toolbar
    if(_ed_layer_toolbar != NULL)
        delete _ed_layer_toolbar;
    _ed_layer_toolbar = new QToolBar("Layers", _ed_tileset_layer_splitter);

    // Add the buttons
    QPushButton *button = new QPushButton(QIcon(QString("img/misc/editor-tools/document-new.png")), QString(), _ed_layer_toolbar);
    button->setContentsMargins(1, 1, 1, 1);
    button->setFixedSize(20, 20);
    button->setToolTip(tr("Add Layer"));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(_MapAddLayer()));
    _ed_layer_toolbar->addWidget(button);

    button = new QPushButton(QIcon(QString("img/misc/editor-tools/edit-rename.png")), QString(), _ed_layer_toolbar);
    button->setContentsMargins(1, 1, 1, 1);
    button->setFixedSize(20, 20);
    button->setToolTip(tr("Modify Layer"));
    _ed_layer_toolbar->addWidget(button);
    button->setDisabled(true);

    button = new QPushButton(QIcon(QString("img/misc/editor-tools/edit-delete.png")), QString(), _ed_layer_toolbar);
    button->setContentsMargins(1, 1, 1, 1);
    button->setFixedSize(20, 20);
    button->setToolTip(tr("Delete Layer"));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(_MapDeleteLayer()));
    _ed_layer_toolbar->addWidget(button);
    _delete_layer_button = button;

    button = new QPushButton(QIcon(QString("img/misc/editor-tools/go-up.png")), QString(), _ed_layer_toolbar);
    button->setContentsMargins(1, 1, 1, 1);
    button->setFixedSize(20, 20);
    button->setToolTip(tr("Move Layer Up"));
    _ed_layer_toolbar->addWidget(button);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(_MapMoveLayerUp()));
    // Keep a reference to later set the button state.
    _layer_up_button = button;

    button = new QPushButton(QIcon(QString("img/misc/editor-tools/go-down.png")), QString(), _ed_layer_toolbar);
    button->setContentsMargins(1, 1, 1, 1);
    button->setFixedSize(20, 20);
    button->setToolTip(tr("Move Layer Down"));
    _ed_layer_toolbar->addWidget(button);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(_MapMoveLayerDown()));
    // Keep a reference to later set the button state.
    _layer_down_button = button;

    button = new QPushButton(QIcon(QString("img/misc/editor-tools/eye.png")), QString(), _ed_layer_toolbar);
    button->setContentsMargins(1, 1, 1, 1);
    button->setFixedSize(20, 20);
    button->setToolTip(tr("Toggle visibility of the layer"));
    _ed_layer_toolbar->addWidget(button);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(_ToggleLayerVisibility()));

    // Left of the screen
    _ed_splitter->addWidget(_ed_scrollarea);

    // right part
    _ed_tileset_layer_splitter->addWidget(_ed_layer_view);
    _ed_tileset_layer_splitter->addWidget(_ed_layer_toolbar);
    _ed_tileset_layer_splitter->addWidget(_ed_tabs);

    _ed_splitter->addWidget(_ed_tileset_layer_splitter);
}



void Editor::_FileNew()
{
    if(_EraseOK()) {
        MapPropertiesDialog *new_map = new MapPropertiesDialog(this, "new_map", false);

        if(new_map->exec() == QDialog::Accepted) {
            if(_ed_scrollarea != NULL)
                delete _ed_scrollarea;
            _ed_scrollarea = new EditorScrollArea(NULL, new_map->GetWidth(), new_map->GetHeight());

            SetupMainView();

            QTreeWidget *tilesets = new_map->GetTilesetTree();
            int num_items     = tilesets->topLevelItemCount();
            int checked_items = 0;
            for(int i = 0; i < num_items; i++)
                if(tilesets->topLevelItem(i)->checkState(0) == Qt::Checked)
                    checked_items++;

            // Used to show the progress of tilesets that have been loaded.
            QProgressDialog *new_map_progress =
                new QProgressDialog(tr("Loading tilesets..."), NULL, 0, checked_items, this,
                                    Qt::Widget | Qt::FramelessWindowHint | Qt::WindowTitleHint);
            new_map_progress->setWindowTitle(tr("Creating Map..."));

            // Set location of and show the progress dialog
            new_map_progress->move(this->pos().x() + this->width() / 2  - new_map_progress->width() / 2,
                                   this->pos().y() + this->height() / 2 - new_map_progress->height() / 2);
            new_map_progress->show();

            checked_items = 0;
            for(int i = 0; i < num_items; i++) {
                if(tilesets->topLevelItem(i)->checkState(0) == Qt::Checked) {
                    new_map_progress->setValue(checked_items++);

                    TilesetTable *a_tileset = new TilesetTable();
                    if(!a_tileset->Load(tilesets->topLevelItem(i)->text(0))) {
                        const std::string mes = "Failed to load tileset image: "
                                                + tilesets->topLevelItem(i)->text(0).toStdString();
                        QMessageBox::critical(this, tr("Map Editor"),
                                              tr(mes.c_str()));
                    }
                    _ed_tabs->addTab(a_tileset->table, tilesets->topLevelItem(i)->text(0));
                    _ed_scrollarea->_map->tilesets.push_back(a_tileset);
                    _ed_scrollarea->_map->tileset_def_names.push_back(a_tileset->GetDefintionFilename());
                } // tileset must be checked
            } // iterate through all possible tilesets
            new_map_progress->setValue(checked_items);

            _ed_scrollarea->_map->SetInitialized(true);
            _ed_scrollarea->resize(new_map->GetWidth() * TILE_WIDTH, new_map->GetHeight() * TILE_HEIGHT);

            // Set the splitters sizes
            QList<int> sizes;
            sizes << 600 << 200;
            _ed_splitter->setSizes(sizes);

            sizes.clear();
            sizes << 150 << 50 << 400;
            _ed_tileset_layer_splitter->setSizes(sizes);

            _ed_splitter->show();

            _grid_on = false;
            if(_select_on)
                _TileToggleSelect();
            _ViewToggleGrid();

            // Enable appropriate actions
            _TilesEnableActions();

            // Set default edit mode
            _ed_scrollarea->_layer_id = 0;

            // Add default layers
            QIcon icon(QString("img/misc/editor-tools/eye.png"));
            QTreeWidgetItem *background = new QTreeWidgetItem(_ed_layer_view);
            background->setText(0, QString::number(0));
            background->setIcon(1, icon);
            background->setText(2, tr("Background"));
            background->setText(3, tr("ground"));
            QTreeWidgetItem *background2 = new QTreeWidgetItem(_ed_layer_view);
            background2->setText(0, QString::number(1));
            background2->setIcon(1, icon);
            background2->setText(2, tr("Background 2"));
            background2->setText(3, tr("ground"));
            QTreeWidgetItem *background3 = new QTreeWidgetItem(_ed_layer_view);
            background3->setText(0, QString::number(2));
            background3->setIcon(1, icon);
            background3->setText(2, tr("Background 3"));
            background3->setText(3, tr("ground"));
            QTreeWidgetItem *sky = new QTreeWidgetItem(_ed_layer_view);
            sky->setText(0, QString::number(3));
            sky->setIcon(1, icon);
            sky->setText(2, tr("Sky"));
            sky->setText(3, tr("sky"));

            _ed_layer_view->adjustSize();
            // Fix a bug in the width computation of the icon
            _ed_layer_view->setColumnWidth(1, 20);

            _ed_layer_view->setCurrentItem(background); // layer 0

            _ed_scrollarea->_tile_mode  = PAINT_TILE;

            _undo_stack->setClean();

            // Hide and delete progress bar
            new_map_progress->hide();
            delete new_map_progress;

            statusBar()->showMessage(tr("New map created"), 5000);
        } // only if the user pressed OK
        else
            statusBar()->showMessage(tr("No map created!"), 5000);

        delete new_map;
    } // make sure an unsaved map is not lost
} // void Editor::_FileNew()



void Editor::_FileOpen()
{
    if(_EraseOK()) {
        // file to open
        QString file_name = QFileDialog::getOpenFileName(this, tr("Map Editor -- File Open"),
                            "dat/maps", "Maps (*.lua)");

        if(!file_name.isEmpty()) {
            if(_ed_scrollarea != NULL)
                delete _ed_scrollarea;
            _ed_scrollarea = new EditorScrollArea(NULL, 0, 0);

            SetupMainView();

            _ed_scrollarea->_map->SetFileName(file_name);
            _ed_scrollarea->_map->LoadMap();

            _UpdateLayersView();

            // Count for the tileset names
            int num_items = _ed_scrollarea->_map->tileset_def_names.count();
            int progress_steps = 0;

            // Used to show the progress of tilesets has been loaded.
            QProgressDialog *new_map_progress =
                new QProgressDialog(tr("Loading tilesets..."), NULL, 0, num_items, this,
                                    Qt::Widget | Qt::FramelessWindowHint | Qt::WindowTitleHint);
            new_map_progress->setWindowTitle(tr("Creating Map..."));

            // Set the progress bar
            new_map_progress->move(this->pos().x() + this->width() / 2  - new_map_progress->width() / 2,
                                   this->pos().y() + this->height() / 2 - new_map_progress->height() / 2);
            new_map_progress->show();

            for(QStringList::ConstIterator it = _ed_scrollarea->_map->tileset_def_names.begin();
                    it != _ed_scrollarea->_map->tileset_def_names.end(); it++) {
                new_map_progress->setValue(progress_steps++);

                TilesetTable *a_tileset = new TilesetTable();
                if(!a_tileset->Load(*it)) {
                    const std::string mes = tr("Failed to load tileset image: ").toStdString()
                                            + (*it).toStdString();
                    QMessageBox::critical(this, tr("Map Editor"),
                                          tr(mes.c_str()));
                }

                _ed_tabs->addTab(a_tileset->table, *it);
                _ed_scrollarea->_map->tilesets.push_back(a_tileset);
            } // iterate through all tilesets in the map
            new_map_progress->setValue(progress_steps);

            _ed_scrollarea->_map->SetInitialized(true);
            _ed_scrollarea->resize(_ed_scrollarea->_map->GetWidth(),
                                   _ed_scrollarea->_map->GetHeight());

            // Set the splitters sizes
            QList<int> sizes;
            sizes << 600 << 200;
            _ed_splitter->setSizes(sizes);

            sizes.clear();
            sizes << 150 << 50 << 400;
            _ed_tileset_layer_splitter->setSizes(sizes);

            _ed_splitter->show();

            _grid_on = false;
            if(_select_on)
                _TileToggleSelect();
            _ViewToggleGrid();

            // Enable appropriate actions
            _TilesEnableActions();

            // Set default edit mode
            _ed_scrollarea->_layer_id = 0;
            _ed_scrollarea->_tile_mode  = PAINT_TILE;

            // Hide and delete progress bar
            new_map_progress->hide();
            delete new_map_progress;

            _undo_stack->setClean();
            statusBar()->showMessage(QString(tr("Opened \'%1\'")).
                                     arg(_ed_scrollarea->_map->GetFileName()), 5000);

            setWindowTitle(QString("Map Editor - ") + _ed_scrollarea->_map->GetFileName());
        } // file must exist in order to open it
        else
            statusBar()->showMessage(tr("No map created!"), 5000);
    } // make sure an unsaved map is not lost
} // void Editor::_FileOpen()



void Editor::_FileSaveAs()
{
    // get the file name from the user
    QString file_name = QFileDialog::getSaveFileName(this,
                        tr("Map Editor -- File Save"), "dat/maps", "Maps (*.lua)");

    if(!file_name.isEmpty()) {
        _ed_scrollarea->_map->SetFileName(file_name);
        _FileSave();
        setWindowTitle(QString("Map Editor - ") + _ed_scrollarea->_map->GetFileName());
        return;
    } // make sure the file name is not blank

    statusBar()->showMessage("Save abandoned.", 5000);
}



void Editor::_FileSave()
{
    if(_ed_scrollarea->_map->GetFileName().isEmpty() ||
            _ed_scrollarea->_map->GetFileName() == tr("Untitled")) {
        _FileSaveAs();
        return;
    } // gets a file name if it is blank

    _ed_scrollarea->_map->SaveMap();      // actually saves the map
    _undo_stack->setClean();
    setWindowTitle(QString("%1").arg(_ed_scrollarea->_map->GetFileName()));
    statusBar()->showMessage(QString(tr("Saved \'%1\' successfully!")).
                             arg(_ed_scrollarea->_map->GetFileName()), 5000);
}



void Editor::_FileClose()
{
    // Checks to see if the map is unsaved.
    if(_EraseOK()) {
        if(_ed_scrollarea != NULL) {
            delete _ed_scrollarea;
            _ed_scrollarea = NULL;
            _undo_stack->clear();

            // Enable appropriate actions
            _TilesEnableActions();
        } // scrollview must exist first

        if(_ed_tabs != NULL) {
            delete _ed_tabs;
            _ed_tabs = NULL;
        } // tabs must exist first

        if(_ed_layer_toolbar != NULL) {
            delete _ed_layer_toolbar;
            _ed_layer_toolbar = NULL;
        }

        if(_ed_layer_view != NULL) {
            delete _ed_layer_view;
            _ed_layer_view = NULL;
        }

        setWindowTitle(tr("Map Editor"));
    } // make sure an unsaved map is not lost
}



void Editor::_FileQuit()
{
    // Checks to see if the map is unsaved.
    if(_EraseOK())
        qApp->exit(0);
}



void Editor::_ViewToggleGrid()
{
    if(_ed_scrollarea != NULL && _ed_scrollarea->_map != NULL) {
        _grid_on = !_grid_on;
        _toggle_grid_action->setChecked(_grid_on);
        _ed_scrollarea->_map->SetGridOn(_grid_on);
    } // map must exist in order to view things on it
}


void Editor::_TileLayerFill()
{
    // get reference to current tileset
    QTableWidget *table = static_cast<QTableWidget *>(_ed_tabs->currentWidget());

    // put selected tile from tileset into tile array at correct position
    int32 tileset_index = table->currentRow() * 16 + table->currentColumn();
    int32 multiplier = _ed_scrollarea->_map->tileset_def_names.indexOf(_ed_tabs->tabText(_ed_tabs->currentIndex()));

    if(multiplier == -1) {
        _ed_scrollarea->_map->tileset_def_names.append(_ed_tabs->tabText(_ed_tabs->currentIndex()));
        multiplier = _ed_scrollarea->_map->tileset_def_names.indexOf(_ed_tabs->tabText(_ed_tabs->currentIndex()));
    } // calculate index of current tileset

    std::vector<std::vector<int32> >& current_layer = _ed_scrollarea->GetCurrentLayer();

    // Record the information for undo/redo operations.
    std::vector<int32> previous;
    std::vector<int32> modified;
    std::vector<QPoint> indeces;;

    for(uint32 y = 0; y < current_layer.size(); ++y) {
        for(uint32 x = 0; x < current_layer[y].size(); ++x) {
            // Stores the indeces
            indeces.push_back(QPoint(x, y));
            previous.push_back(current_layer[y][x]);

            // Fill the layer
            _ed_scrollarea->_AutotileRandomize(multiplier, tileset_index);
            current_layer[y][x] = tileset_index + multiplier * 256;
            modified.push_back(tileset_index + multiplier * 256);
        }
    }

    LayerCommand *fill_command = new LayerCommand(indeces, previous, modified,
            _ed_scrollarea->_layer_id, this, "Fill Layer");
    _undo_stack->push(fill_command);
    indeces.clear();
    previous.clear();
    modified.clear();

    // Draw the changes.
    _ed_scrollarea->_map->SetChanged(true);
    _ed_scrollarea->_map->updateGL();
} // void Editor::_TileLayerFill()



void Editor::_TileLayerClear()
{
    std::vector<std::vector<int32> >::iterator it;    // used to iterate over an entire layer
    std::vector<std::vector<int32> >& current_layer = _ed_scrollarea->GetCurrentLayer();

    // Record the information for undo/redo operations.
    std::vector<std::vector<int32> > previous = current_layer;
    //std::vector<std::vector<int32> > modified(current_layer.size(), -1);
    std::vector<std::vector<int32> > indeces(current_layer.size());
    /*	for (int32 i = 0; i < static_cast<int32>(current_layer.size()); i++)
    		indeces[i] = i;

    	// Clear the layer.
    	for (it = current_layer.begin(); it != current_layer.end(); it++)
    		*it = -1;

    	LayerCommand* clear_command = new LayerCommand(indeces, previous, modified,
    		_ed_scrollarea->_layer_id, _ed_scrollarea->_map->GetContext(), this,
    		"Clear Layer");
    	_undo_stack->push(clear_command);
    	indeces.clear();
    	previous.clear();
    	modified.clear();
    */
    // Draw the changes.
    _ed_scrollarea->_map->SetChanged(true);
    _ed_scrollarea->_map->updateGL();
}


void Editor::_TileToggleSelect()
{
    if(_ed_scrollarea != NULL && _ed_scrollarea->_map != NULL) {
        _select_on = !_select_on;
        _toggle_select_action->setChecked(_select_on);
        _ed_scrollarea->_map->SetSelectOn(_select_on);
    } // map must exist in order to view things on it
}


void Editor::_TileModePaint()
{
    if(_ed_scrollarea != NULL) {
        // Clear the selection layer.
        if(_ed_scrollarea->_moving == true && _select_on == true) {
            _ed_scrollarea->_map->ClearSelectionLayer();
        } // clears when selected tiles were going to be moved but
        // user changed their mind in the midst of the move operation

        _ed_scrollarea->_tile_mode = PAINT_TILE;
        _ed_scrollarea->_moving = false;
    } // scrollview must exist in order to switch modes
}


void Editor::_TileModeMove()
{
    if(_ed_scrollarea != NULL) {
        // Clear the selection layer.
        if(_ed_scrollarea->_moving == true && _select_on == true) {
            _ed_scrollarea->_map->ClearSelectionLayer();
        } // clears when selected tiles were going to be moved but
        // user changed their mind in the midst of the move operation

        _ed_scrollarea->_tile_mode = MOVE_TILE;
        _ed_scrollarea->_moving = false;
    } // scrollview must exist in order to switch modes
}


void Editor::_TileModeDelete()
{
    if(_ed_scrollarea != NULL) {
        // Clear the selection layer.
        if(_ed_scrollarea->_moving == true && _select_on == true) {
            _ed_scrollarea->_map->ClearSelectionLayer();
        } // clears when selected tiles were going to be moved but
        // user changed their mind in the midst of the move operation

        _ed_scrollarea->_tile_mode = DELETE_TILE;
        _ed_scrollarea->_moving = false;
    } // scrollview must exist in order to switch modes
}


void Editor::_TilesetEdit()
{
    TilesetEditor *tileset_editor = new TilesetEditor(this);

    tileset_editor->exec();

    delete tileset_editor;
}


void Editor::_MapAddLayer()
{
    if(_ed_scrollarea == NULL || _ed_scrollarea->_map == NULL)
        return;

    LayerDialog *layer_dlg = new LayerDialog(this, "layer_dialog");

    if(layer_dlg->exec() == QDialog::Accepted) {
        // Apply changes
        LayerInfo layer_info = layer_dlg->_GetLayerInfo();

        _ed_scrollarea->_map->AddLayer(layer_info);

        _UpdateLayersView();

        // The map has been changed
        _ed_scrollarea->_map->SetChanged(true);

    } // only process results if user selected okay

    delete layer_dlg;
}

void Editor::_MapModifyLayer()
{
    if(_ed_scrollarea == NULL)
        return;

    // TODO
}

void Editor::_MapDeleteLayer()
{
    if(_ed_scrollarea == NULL || _ed_scrollarea->_map == NULL)
        return;

    if(!_CanDeleteLayer(_ed_layer_view->currentItem()))
        return;

    uint32 layer_id = _ed_layer_view->currentItem()->text(0).toUInt();
    Grid *grid = _ed_scrollarea->_map;
    std::vector<Layer>& layers = grid->GetLayers();
    if(layer_id >= layers.size())
        return;

    switch(QMessageBox::warning(this, tr("Delete layer"),
                                tr("Are you sure you want to delete this layer?"),
                                tr("&Yes"), tr("&No"), tr("&Cancel"),
                                0,		// Enter == button 0
                                2)) {	// Escape == button 2
    case 0: // Yes pressed.
        // continue below
        break;
    default: // Cancel clicked or Escape pressed
        // Cancel or no
        return;
    }

    // Apply changes
    _ed_scrollarea->_map->DeleteLayer(layer_id);

    _UpdateLayersView();

    // Select the previous layer when possible
    _SetSelectedLayer(layer_id == 0 ? layer_id : layer_id - 1);

    // The map has been changed
    _ed_scrollarea->_map->SetChanged(true);
}

void Editor::_MapMoveLayerUp()
{
    if(_ed_scrollarea == NULL || _ed_scrollarea->_map == NULL || !_CanLayerMoveUp(_ed_layer_view->currentItem()))
        return;

    uint32 layer_id = _ed_layer_view->currentItem()->text(0).toUInt();
    if(layer_id == 0)
        return;

    Grid *grid = _ed_scrollarea->_map;
    std::vector<Layer>& layers = grid->GetLayers();
    if(layers.size() < 2 || layer_id >= layers.size())
        return;

    // insert the layer before the previous one in the new vector
    uint32 old_id = 0;
    std::vector<Layer> new_layers;
    std::vector<Layer>::iterator it = layers.begin();
    for(; it != layers.end(); ++it) {
        // Add the layer before the previous one, plus the previous one after that
        if(old_id == layer_id - 1) {
            new_layers.push_back(layers.at(layer_id));
            new_layers.push_back(*it);
        } else if(old_id == layer_id) {
            // The layer has already been added, so don't do anything here.
        }
        // Add other layers normally
        else if(old_id != layer_id - 1) {
            new_layers.push_back(*it);
        }
        ++old_id;
    }
    // Once done, swap the layers
    layers.swap(new_layers);

    // Show the changes done.
    _UpdateLayersView();
    _ed_scrollarea->_map->updateGL();

    // Set the layer selection to follow the current layer
    _SetSelectedLayer(layer_id - 1);

    // The map has been changed
    _ed_scrollarea->_map->SetChanged(true);
}

void Editor::_MapMoveLayerDown()
{
    if(_ed_scrollarea == NULL || _ed_scrollarea->_map == NULL || !_CanLayerMoveDown(_ed_layer_view->currentItem()))
        return;

    uint32 layer_id = _ed_layer_view->currentItem()->text(0).toUInt();
    Grid *grid = _ed_scrollarea->_map;
    std::vector<Layer>& layers = grid->GetLayers();
    if(layers.size() < 2 || layer_id >= layers.size() - 1)
        return;

    // insert the layer after the next one in the new vector
    uint32 old_id = 0;
    std::vector<Layer> new_layers;
    std::vector<Layer>::iterator it = layers.begin();
    for(; it != layers.end(); ++it) {
        // Add the layer after the next one, plus the previous one after that
        if(old_id == layer_id + 1) {
            new_layers.push_back(*it);
            new_layers.push_back(layers.at(layer_id));
        } else if(old_id == layer_id) {
            // The layer has already been added, so don't do anything here.
        }
        // Add other layers normally
        else if(old_id != layer_id + 1) {
            new_layers.push_back(*it);
        }
        ++old_id;
    }
    // Once done, swap the layers
    layers.swap(new_layers);

    // Show the changes done.
    _UpdateLayersView();
    _ed_scrollarea->_map->updateGL();

    // Set the layer selection to follow the current layer
    _SetSelectedLayer(layer_id + 1);

    // The map has been changed
    _ed_scrollarea->_map->SetChanged(true);
}

void Editor::_MapProperties()
{
    MapPropertiesDialog *props = new MapPropertiesDialog(this, "map_properties", true);

    if(props->exec() == QDialog::Accepted) {
        /*
        #if !defined(WIN32)
        if (_ed_scrollarea->_map->GetWidth() < props->GetWidth())
        {
        	// User wants to make map wider so we must insert columns of tiles at the edge of the map.

        	int map_width     = _ed_scrollarea->_map->GetWidth();
        	int map_height    = _ed_scrollarea->_map->GetHeight();
        	int extra_columns = props->GetWidth() - map_width;

        	// Add in the extra columns one by one.
        	for (int col = extra_columns; col > 0; col--)
        	{
        		vector<int32>& lower_layer = _ed_scrollarea->_map->GetLayer(GROUND_LAYER, _ed_scrollarea->_map->GetContext());
        		vector<int32>::iterator it = lower_layer.begin() + map_width;
        		for (int row = 0; row < map_height; row++)
        		{
        			lower_layer.insert(it, -1);
        			it += map_width + 1;
        		} // iterate through the rows of the lower layer

        		vector<int32>& middle_layer = _ed_scrollarea->_map->GetLayer(FRINGE_LAYER, _ed_scrollarea->_map->GetContext());
        		it = middle_layer.begin() + map_width;
        		for (int row = 0; row < map_height; row++)
        		{
        			middle_layer.insert(it, -1);
        			it += map_width + 1;
        		} // iterate through the rows of the middle layer

        		vector<int32>& upper_layer = _ed_scrollarea->_map->GetLayer(SKY_LAYER, _ed_scrollarea->_map->GetContext());
        		it = upper_layer.begin() + map_width;
        		for (int row = 0; row < map_height; row++)
        		{
        			upper_layer.insert(it, -1);
        			it += map_width + 1;
        		} // iterate through the rows of the upper layer

        		map_width++;
        		_ed_scrollarea->_map->SetWidth(map_width);
        	} // add in all the extra columns
        } // insert columns
        else if (_ed_scrollarea->_map->GetWidth() > props->GetWidth())
        {
        	// User wants to make map less wide so we must delete columns of tiles from the edge of the map.

        	int map_width     = _ed_scrollarea->_map->GetWidth();
        	int map_height    = _ed_scrollarea->_map->GetHeight();
        	int extra_columns = map_width - props->GetWidth();

        	// Delete all the extra columns one by one.
        	for (int col = extra_columns; col > 0; col--)
        	{
        		vector<int32>& lower_layer = _ed_scrollarea->_map->GetLayer(GROUND_LAYER, _ed_scrollarea->_map->GetContext());
        		vector<int32>::iterator it = lower_layer.begin() + map_width - 1;
        		for (int row = 0; row < map_height; row++)
        		{
        			lower_layer.erase(it);
        			it += map_width - 1;
        		} // iterate through the rows of the lower layer

        		vector<int32>& middle_layer = _ed_scrollarea->_map->GetLayer(FRINGE_LAYER, _ed_scrollarea->_map->GetContext());
        		it = middle_layer.begin() + map_width - 1;
        		for (int row = 0; row < map_height; row++)
        		{
        			middle_layer.erase(it);
        			it += map_width - 1;
        		} // iterate through the rows of the middle layer

        		vector<int32>& upper_layer = _ed_scrollarea->_map->GetLayer(SKY_LAYER, _ed_scrollarea->_map->GetContext());
        		it = upper_layer.begin() + map_width - 1;
        		for (int row = 0; row < map_height; row++)
        		{
        			upper_layer.erase(it);
        			it += map_width - 1;
        		} // iterate through the rows of the upper layer

        		map_width--;
        		_ed_scrollarea->_map->SetWidth(map_width);
        	} // delete all the extra columns
        } // delete columns

        if (_ed_scrollarea->_map->GetHeight() < props->GetHeight())
        {
        	// User wants to make map taller so we must insert rows of tiles at the edge of the map.

        	int map_width = _ed_scrollarea->_map->GetWidth();
        	int extra_rows = props->GetHeight() - _ed_scrollarea->_map->GetHeight();

        	vector<int32>& lower_layer  = _ed_scrollarea->_map->GetLayer(GROUND_LAYER, _ed_scrollarea->_map->GetContext());
        	vector<int32>& middle_layer = _ed_scrollarea->_map->GetLayer(FRINGE_LAYER, _ed_scrollarea->_map->GetContext());
        	vector<int32>& upper_layer  = _ed_scrollarea->_map->GetLayer(SKY_LAYER, _ed_scrollarea->_map->GetContext());
        	lower_layer.insert( lower_layer.end(),  extra_rows * map_width, -1);
        	middle_layer.insert(middle_layer.end(), extra_rows * map_width, -1);
        	upper_layer.insert( upper_layer.end(),  extra_rows * map_width, -1);
        } // add rows
        else if (_ed_scrollarea->_map->GetHeight() > props->GetHeight())
        {
        	// User wants to make map less tall so we must delete rows of tiles from the edge of the map.

        	int map_width  = _ed_scrollarea->_map->GetWidth();
        	int extra_rows = _ed_scrollarea->_map->GetHeight() - props->GetHeight();

        	vector<int32>& lower_layer  = _ed_scrollarea->_map->GetLayer(GROUND_LAYER, _ed_scrollarea->_map->GetContext());
        	vector<int32>& middle_layer = _ed_scrollarea->_map->GetLayer(FRINGE_LAYER, _ed_scrollarea->_map->GetContext());
        	vector<int32>& upper_layer  = _ed_scrollarea->_map->GetLayer(SKY_LAYER, _ed_scrollarea->_map->GetContext());
        	lower_layer.erase( lower_layer.end()  - extra_rows * map_width, lower_layer.end());
        	middle_layer.erase(middle_layer.end() - extra_rows * map_width, middle_layer.end());
        	upper_layer.erase( upper_layer.end()  - extra_rows * map_width, upper_layer.end());
        } // delete rows

        // Resize the map, QOpenGL and QScrollView widgets.
        _ed_scrollarea->_map->SetHeight(props->GetHeight());
        _ed_scrollarea->_map->resize(props->GetWidth() * TILE_WIDTH, props->GetHeight() * TILE_HEIGHT);
        _ed_scrollarea->resize(props->GetWidth() * TILE_WIDTH, props->GetHeight() * TILE_HEIGHT);
        #endif


        */
        // User has the ability to add or remove tilesets being used. We don't want
        // to reload tilesets that have already been loaded before.

        QTreeWidget *tilesets = props->GetTilesetTree();

        // Put the names of the tabs into a nice list that can be easily searched
        // with one command instead of a loop.
        QStringList tab_names;
        for(int i = 0; i < _ed_tabs->count(); i++)
            tab_names << _ed_tabs->tabText(i);

        // Go through the list of tilesets, adding selected tilesets and removing
        // any unwanted tilesets.
        int num_items = tilesets->topLevelItemCount();
        for(int i = 0; i < num_items; i++) {
            if(tilesets->topLevelItem(i)->checkState(0) == Qt::Checked) {
                if(tab_names.contains(tilesets->topLevelItem(i)->text(0)) == false) {
                    TilesetTable *a_tileset = new TilesetTable();
                    a_tileset->Load(tilesets->topLevelItem(i)->text(0));
                    _ed_tabs->addTab(a_tileset->table, tilesets->topLevelItem(i)->text(0));
                    _ed_scrollarea->_map->tilesets.push_back(a_tileset);
                } // only add a tileset if it isn't already loaded
            } // tileset must be checked in order to add it
            else if(tilesets->topLevelItem(i)->checkState(0) == Qt::Unchecked &&
                    tab_names.contains(tilesets->topLevelItem(i)->text(0)))
                _ed_tabs->removeTab(tab_names.indexOf(tilesets->topLevelItem(i)->text(0)));
            // FIXME:
            // Where to add and remove tileset name from the tilesets list
            // in the _map? Do it here or when actually painting and deleting
            // tiles? Here the assumption is made that if the user is adding a
            // tileset, then s/he expects to use tiles from that tileset and we
            // can safely add the tileset name to the _map. Otherwise we would
            // have to constantly check every time a paint operation occurs
            // whether or not the tileset name of the selected tile was present
            // in the tileset name list in _map. That's cumbersome.
            //
            // When removing a tileset however, there might still be tiles in
            // the map from that tileset, and the user is only removing the
            // tileset from the view in the bottom of the map to unclutter
            // things. In this case we wouldn't want to remove the tileset name
            // from the list in _map.
        } // iterate through all possible tilesets
    } // only if the user pressed OK
    else
        statusBar()->showMessage("Properties not modified!", 5000);

    delete props;
} // void Editor::_MapProperties()


void Editor::_UpdateLayersView()
{
    _ed_layer_view->clear();
    std::vector<QTreeWidgetItem *> layer_names = _ed_scrollarea->_map->getLayerItems();
    for(uint32 i = 0; i < layer_names.size(); ++i) {
        _ed_layer_view->addTopLevelItem(layer_names[i]);
    }
    _ed_layer_view->adjustSize();
    // Fix a bug in the width computation of the icon
    _ed_layer_view->setColumnWidth(1, 20);

    _ed_layer_view->setCurrentItem(layer_names[0]); // layer 0

}


void Editor::_HelpHelp()
{
    QMessageBox::about(this, tr("Help"), tr("See http://allacrost.sourceforge.net/wiki/index.php/Code_Documentation#Map_Editor_Documentation for more details"));
}


void Editor::_HelpAbout()
{
    QMessageBox::about(this, tr("Map Editor -- About"),
                       tr("<center><h1><font color=blue>Map Editor<font>"
                          "</h1></center>"
                          "<center><h2><font color=blue>Copyright (c) 2004-2010<font></h2></center>"
                          "<p>A level editor created for the Hero of Allacrost project. "
                          "Maintained for Valyria Tear since 2012.</p>"));
}


void Editor::_HelpAboutQt()
{
    QMessageBox::aboutQt(this, tr("Map Editor -- About Qt"));
}


bool Editor::_CanLayerMoveUp(QTreeWidgetItem *item) const
{
    if(!item)
        return false;

    QTreeWidgetItem *item_up = _ed_layer_view->itemAbove(item);

    LAYER_TYPE ly_type_up = item_up ? getLayerType(item_up->text(3).toStdString()) : INVALID_LAYER;
    LAYER_TYPE ly_type = getLayerType(item->text(3).toStdString());
    return (ly_type == ly_type_up);
}

bool Editor::_CanLayerMoveDown(QTreeWidgetItem *item) const
{
    if(!item)
        return false;

    QTreeWidgetItem *item_down = _ed_layer_view->itemBelow(item);

    LAYER_TYPE ly_type_down = item_down ? getLayerType(item_down->text(3).toStdString()) : INVALID_LAYER;
    LAYER_TYPE ly_type = getLayerType(item->text(3).toStdString());
    return (ly_type == ly_type_down);
}

bool Editor::_CanDeleteLayer(QTreeWidgetItem *item) const
{
    if(!item)
        return false;

    LAYER_TYPE layer_type = getLayerType(item->text(3).toStdString());

    if(layer_type != GROUND_LAYER)
        return true;

    Grid *grid = _ed_scrollarea->_map;
    if(!grid)
        return false;

    // Count the available ground layers
    uint32 ground_layers_count = 0;
    std::vector<Layer>& layers = grid->GetLayers();

    for(uint32 i = 0; i < layers.size(); ++i) {
        if(layers[i].layer_type == GROUND_LAYER)
            ++ground_layers_count;
    }

    if(ground_layers_count > 1)
        return true;

    return false;
}

void Editor::_SetSelectedLayer(uint32 layer_id)
{
    if(!_ed_layer_view)
        return;

    // Find the item with the given id.
    QList<QTreeWidgetItem *> item_list = _ed_layer_view->findItems(QString::number(layer_id), Qt::MatchFixedString, 0);
    // And set it, if existing
    if(!item_list.isEmpty())
        _ed_layer_view->setCurrentItem(item_list.first());
}

void Editor::_UpdateSelectedLayer(QTreeWidgetItem *item)
{
    if(!item)
        return;

    // Turns back the layer's id into an uint.
    uint32 layer_id = item->text(0).toUInt();

    if(_ed_scrollarea)
        _ed_scrollarea->_layer_id = layer_id;

    _layer_up_button->setEnabled(_CanLayerMoveUp(item));
    _layer_down_button->setEnabled(_CanLayerMoveDown(item));
    _delete_layer_button->setEnabled(_CanDeleteLayer(item));
}


void Editor::_ToggleLayerVisibility()
{
    Layer &layer = _ed_scrollarea->_map->GetLayers()[_ed_scrollarea->_layer_id];
    layer.visible = !layer.visible;

    // Show the change
    _ed_scrollarea->_map->updateGL();

    // Update the item icon
    if(layer.visible)
        _ed_layer_view->currentItem()->setIcon(1, QIcon(QString("img/misc/editor-tools/eye.png")));
    else
        _ed_layer_view->currentItem()->setIcon(1, QIcon());
}

///////////////////////////////////////////////////////////////////////////////
// Editor class -- private functions
///////////////////////////////////////////////////////////////////////////////

void Editor::_CreateActions()
{
    // Create menu actions related to the File menu

    _new_action = new QAction("&New...", this);
    _new_action->setShortcut(tr("Ctrl+N"));
    _new_action->setStatusTip("Create a new map");
    connect(_new_action, SIGNAL(triggered()), this, SLOT(_FileNew()));

    _open_action = new QAction("&Open...", this);
    _open_action->setShortcut(tr("Ctrl+O"));
    _open_action->setStatusTip("Open an existing map");
    connect(_open_action, SIGNAL(triggered()), this, SLOT(_FileOpen()));

    _save_as_action = new QAction("Save &As...", this);
    _save_as_action->setStatusTip("Save the map with another name");
    connect(_save_as_action, SIGNAL(triggered()), this, SLOT(_FileSaveAs()));

    _save_action = new QAction("&Save", this);
    _save_action->setShortcut(tr("Ctrl+S"));
    _save_action->setStatusTip("Save the map");
    connect(_save_action, SIGNAL(triggered()), this, SLOT(_FileSave()));

    _close_action = new QAction("&Close", this);
    _close_action->setShortcut(tr("Ctrl+W"));
    _close_action->setStatusTip("Close the map");
    connect(_close_action, SIGNAL(triggered()), this, SLOT(_FileClose()));

    _quit_action = new QAction("&Quit", this);
    _quit_action->setShortcut(tr("Ctrl+Q"));
    _quit_action->setStatusTip("Quits from the editor");
    connect(_quit_action, SIGNAL(triggered()), this, SLOT(_FileQuit()));

    // Create menu actions related to the View menu

    _toggle_grid_action = new QAction("&Grid", this);
    _toggle_grid_action->setStatusTip("Switches the grid on and off");
    _toggle_grid_action->setShortcut(tr("G"));
    _toggle_grid_action->setCheckable(true);
    connect(_toggle_grid_action, SIGNAL(triggered()), this, SLOT(_ViewToggleGrid()));

    // Create menu actions related to the Tiles menu

    _undo_action = new QAction(QIcon("img/misc/editor-tools/arrow-left.png"), "&Undo", this);
    _undo_action->setShortcut(tr("Ctrl+Z"));
    _undo_action->setStatusTip("Undoes the previous command");
    connect(_undo_action, SIGNAL(triggered()), _undo_stack, SLOT(undo()));

    _redo_action = new QAction(
        QIcon("img/misc/editor-tools/arrow-right.png"),
        "&Redo", this);
    _redo_action->setShortcut(tr("Ctrl+Y"));
    _redo_action->setStatusTip("Redoes the next command");
    connect(_redo_action, SIGNAL(triggered()), _undo_stack, SLOT(redo()));

    _layer_fill_action = new QAction(
        QIcon("img/misc/editor-tools/stock-tool-bucket-fill-22.png"), "&Fill layer", this);
    _layer_fill_action->setStatusTip("Fills current layer with selected tile");
    connect(_layer_fill_action, SIGNAL(triggered()), this, SLOT(_TileLayerFill()));

    _layer_clear_action = new QAction("&Clear layer", this);
    _layer_clear_action->setStatusTip("Clears current layer from any tiles");
    connect(_layer_clear_action, SIGNAL(triggered()), this, SLOT(_TileLayerClear()));

    _toggle_select_action = new QAction(QIcon("img/misc/editor-tools/stock-tool-rect-select-22.png"), "Marquee &Select", this);
    _toggle_select_action->setShortcut(tr("Shift+S"));
    _toggle_select_action->setStatusTip("Rectangularly select tiles on the map");
    _toggle_select_action->setCheckable(true);
    connect(_toggle_select_action, SIGNAL(triggered()), this, SLOT(_TileToggleSelect()));

    _mode_paint_action = new QAction(QIcon("img/misc/editor-tools/stock-tool-pencil-22.png"), "&Paint mode", this);
    _mode_paint_action->setShortcut(tr("Shift+P"));
    _mode_paint_action->setStatusTip("Switches to paint mode to draw tiles on the map");
    _mode_paint_action->setCheckable(true);
    connect(_mode_paint_action, SIGNAL(triggered()), this, SLOT(_TileModePaint()));

    _mode_move_action = new QAction(QIcon("img/misc/editor-tools/stock-tool-arrow.png"), "Mo&ve mode", this);
    _mode_move_action->setShortcut(tr("Shift+V"));
    _mode_move_action->setStatusTip("Switches to move mode to move tiles around on the map");
    _mode_move_action->setCheckable(true);
    connect(_mode_move_action, SIGNAL(triggered()), this, SLOT(_TileModeMove()));

    _mode_delete_action = new QAction(QIcon("img/misc/editor-tools/stock-tool-eraser-22.png"), "&Delete mode", this);
    _mode_delete_action->setShortcut(tr("Shift+D"));
    _mode_delete_action->setStatusTip("Switches to delete mode to erase tiles from the map");
    _mode_delete_action->setCheckable(true);
    connect(_mode_delete_action, SIGNAL(triggered()), this, SLOT(_TileModeDelete()));

    _mode_group = new QActionGroup(this);
    _mode_group->addAction(_mode_paint_action);
    _mode_group->addAction(_mode_move_action);
    _mode_group->addAction(_mode_delete_action);
    _mode_paint_action->setChecked(true);

    // Create tileset actions related to the Tileset Menu
    _edit_tileset_action = new QAction("Edit &Tileset", this);
    _edit_tileset_action->setStatusTip("Lets the user paint walkability on the tileset");
    //_edit_walkability_action->setCheckable(true);
    connect(_edit_tileset_action, SIGNAL(triggered()), this, SLOT(_TilesetEdit()));

    _map_properties_action = new QAction("&Properties...", this);
    _map_properties_action->setStatusTip("Modify the properties of the map");
    connect(_map_properties_action, SIGNAL(triggered()), this, SLOT(_MapProperties()));

    // Create menu actions related to the Help menu
    _help_action = new QAction("&Help", this);
    _help_action->setShortcut(Qt::Key_F1);
    _help_action->setStatusTip("Brings up help documentation for the editor");
    connect(_help_action, SIGNAL(triggered()), this, SLOT(_HelpHelp()));

    _about_action = new QAction("&About", this);
    _about_action->setStatusTip("Brings up information about the editor");
    connect(_about_action, SIGNAL(triggered()), this, SLOT(_HelpAbout()));

    _about_qt_action = new QAction("About &Qt", this);
    _about_qt_action->setStatusTip("Brings up information about Qt");
    connect(_about_qt_action, SIGNAL(triggered()), this, SLOT(_HelpAboutQt()));
} // void Editor::_CreateActions()



void Editor::_CreateMenus()
{
    // file menu creation
    _file_menu = menuBar()->addMenu("&File");
    _file_menu->addAction(_new_action);
    _file_menu->addAction(_open_action);
    _file_menu->addSeparator();
    _file_menu->addAction(_save_action);
    _file_menu->addAction(_save_as_action);
    _file_menu->addSeparator();
    _file_menu->addAction(_close_action);
    _file_menu->addAction(_quit_action);
    connect(_file_menu, SIGNAL(aboutToShow()), this, SLOT(_FileMenuSetup()));

    // view menu creation
    _view_menu = menuBar()->addMenu("&View");
    _view_menu->addAction(_toggle_grid_action);
    _view_menu->setTearOffEnabled(true);
    connect(_view_menu, SIGNAL(aboutToShow()), this, SLOT(_ViewMenuSetup()));

    // tile menu creation
    _tiles_menu = menuBar()->addMenu("&Tiles");
    _tiles_menu->addAction(_undo_action);
    _tiles_menu->addAction(_redo_action);
    _tiles_menu->addSeparator();
    _tiles_menu->addAction(_layer_fill_action);
    _tiles_menu->addAction(_layer_clear_action);
    _tiles_menu->addSeparator();
    _tiles_menu->addAction(_toggle_select_action);
    _tiles_menu->addSeparator()->setText("Editing Mode");
    _tiles_menu->addAction(_mode_paint_action);
    _tiles_menu->addAction(_mode_move_action);
    _tiles_menu->addAction(_mode_delete_action);
    _tiles_menu->addSeparator()->setText("Current Layer");

    _tiles_menu->setTearOffEnabled(true);
    connect(_tiles_menu, SIGNAL(aboutToShow()), this, SLOT(_TilesEnableActions()));

    // tileset menu creation
    _tileset_menu = menuBar()->addMenu("Tile&set");
    _tileset_menu->addAction(_edit_tileset_action);
    connect(_tileset_menu, SIGNAL(aboutToShow()), this, SLOT(_TilesetMenuSetup()));

    // map menu creation
    _map_menu = menuBar()->addMenu("&Map");
    _map_menu->addAction(_map_properties_action);
    connect(_map_menu, SIGNAL(aboutToShow()), this, SLOT(_MapMenuSetup()));

    // help menu creation
    _help_menu = menuBar()->addMenu("&Help");
    _help_menu->addAction(_help_action);
    _help_menu->addAction(_about_action);
    _help_menu->addAction(_about_qt_action);
} // void Editor::_CreateMenus()



void Editor::_CreateToolbars()
{
    _tiles_toolbar = addToolBar("Tiles");
    _tiles_toolbar->addAction(_layer_fill_action);
    _tiles_toolbar->addSeparator();
    _tiles_toolbar->addAction(_mode_paint_action);
    _tiles_toolbar->addAction(_mode_move_action);
    _tiles_toolbar->addAction(_mode_delete_action);
    _tiles_toolbar->addSeparator();
    _tiles_toolbar->addAction(_undo_action);
    _tiles_toolbar->addAction(_redo_action);
    _tiles_toolbar->addSeparator();
    _tiles_toolbar->addAction(_toggle_select_action);
}



bool Editor::_EraseOK()
{
    if(_ed_scrollarea != NULL && _ed_scrollarea->_map != NULL) {
        if(_ed_scrollarea->_map->GetChanged()) {
            switch(QMessageBox::warning(this, "Unsaved File", "The document contains unsaved changes!\n"
                                        "Do you want to save the changes before proceeding?", "&Save", "&Discard", "Cancel",
                                        0,		// Enter == button 0
                                        2)) {	// Escape == button 2
            case 0: // Save clicked or Alt+S pressed or Enter pressed.
                // save and exit
                _FileSave();
                break;
            case 1: // Discard clicked or Alt+D pressed
                // don't save but exit
                break;
            default: // Cancel clicked or Escape pressed
                // don't exit
                statusBar()->showMessage("Save abandoned", 5000);
                return false;
            } // warn the user to save
        } // map has been modified
    } // map must exist first

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// EditorScrollView class -- public functions
///////////////////////////////////////////////////////////////////////////////

EditorScrollArea::EditorScrollArea(QWidget *parent, int width, int height) :
    QScrollArea(parent)
{
    // Set default editing modes.
    _tile_mode  = PAINT_TILE;
    _layer_id = 0;
    _moving     = false;

    // Clear the undo/redo vectors.
    _tile_indeces.clear();
    _previous_tiles.clear();
    _modified_tiles.clear();

    // set viewport
    viewport()->setMouseTracking(true);

    // for tracking key events
    setFocusPolicy(Qt::StrongFocus);

    // Create a new map.
    _map = new Grid(viewport(), tr("Untitled"), width, height);
    _map->_ed_scrollarea = this;
    setWidget(_map);

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
    _context_menu = new QMenu(this);
    _context_menu->addAction(_insert_row_action);
    _context_menu->addAction(_insert_column_action);
    _context_menu->addSeparator();
    _context_menu->addAction(_delete_row_action);
    _context_menu->addAction(_delete_column_action);
}



EditorScrollArea::~EditorScrollArea()
{
    delete _map;
    delete _context_menu;
}



void EditorScrollArea::Resize(int width, int height)
{
    _map->resize(width * TILE_WIDTH, height * TILE_HEIGHT);
    _map->SetHeight(height);
    _map->SetWidth(width);
}



std::vector<std::vector<int32> >& EditorScrollArea::GetCurrentLayer()
{
    return _map->GetLayers()[_layer_id].tiles;
}


///////////////////////////////////////////////////////////////////////////////
// EditorScrollView class -- protected functions
///////////////////////////////////////////////////////////////////////////////

bool EditorScrollArea::event(QEvent *evt)
{
    // Recreate a mouse move event in case of simple hovering
    if(evt->type() == QEvent::HoverMove) {
        QHoverEvent *hover_event = dynamic_cast<QHoverEvent *>(evt);
        QMouseEvent *mouse_event = new QMouseEvent(QEvent::MouseMove,
                hover_event->pos(),
                Qt::NoButton,
                Qt::MouseButtons(),
                Qt::KeyboardModifiers());
        return contentsMouseMoveEvent(mouse_event);
    }

    QMouseEvent *mouse_event = dynamic_cast<QMouseEvent *>(evt);
    if(mouse_event) {
        switch(mouse_event->type()) {
        default:
            break;
        case QEvent::MouseButtonPress:
            return contentsMousePressEvent(mouse_event);
            break;
        case QEvent::MouseButtonRelease:
            return contentsMouseReleaseEvent(mouse_event);
            break;
        case QEvent::MouseMove:
            return contentsMouseMoveEvent(mouse_event);
            break;
        }
    }

    return QScrollArea::event(evt);
}

bool EditorScrollArea::contentsMousePressEvent(QMouseEvent *evt)
{
    // get reference to Editor
    Editor *editor = static_cast<Editor *>(topLevelWidget());

    // Takes in account the current scrolling
    int32 x = evt->x() + editor->_ed_scrollarea->horizontalScrollBar()->value();
    int32 y = evt->y() + editor->_ed_scrollarea->verticalScrollBar()->value();

    // don't draw outside the map
    if((y / TILE_HEIGHT) >= static_cast<uint32>(_map->GetHeight()) ||
            (x / TILE_WIDTH)  >= static_cast<uint32>(_map->GetWidth()) ||
            x < 0 || y < 0)
        return true;

    _map->SetChanged(true);

    // record location of pressed tile
    _tile_index_x = x / TILE_WIDTH;
    _tile_index_y = y / TILE_HEIGHT;

    // record the location of the beginning of the selection rectangle
    if(evt->button() == Qt::LeftButton && editor->_select_on == true &&
            _moving == false) {
        _first_corner_index_x = _tile_index_x;
        _first_corner_index_y = _tile_index_y;
        _map->GetSelectionLayer()[_tile_index_y][_tile_index_x] = 1;
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
        QMessageBox::warning(this, "Tile editing mode",
                             "ERROR: Invalid tile editing mode!");
    } // switch on tile editing mode

    // Draw the changes.
    _map->updateGL();
    return true;
} // void EditorScrollView::contentsMousePressEvent(QMouseEvent* evt)



bool EditorScrollArea::contentsMouseMoveEvent(QMouseEvent *evt)
{
    // get reference to Editor
    Editor *editor = static_cast<Editor *>(topLevelWidget());

    // Takes in account the current scrolling
    int32 x = evt->x() + editor->_ed_scrollarea->horizontalScrollBar()->value();
    int32 y = evt->y() + editor->_ed_scrollarea->verticalScrollBar()->value();

    // don't draw outside the map
    if((y / TILE_HEIGHT) >= static_cast<uint32>(_map->GetHeight()) ||
            (x / TILE_WIDTH)  >= static_cast<uint32>(_map->GetWidth()) ||
            x < 0 || y < 0) {
        editor->statusBar()->clearMessage();
        return true;
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
                    _map->GetSelectionLayer()[y][x] = 1;
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
            QMessageBox::warning(this, "Tile editing mode",
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
    _map->updateGL();
    return true;
} // void EditorScrollView::contentsMouseMoveEvent(QMouseEvent *evt)



bool EditorScrollArea::contentsMouseReleaseEvent(QMouseEvent *evt)
{
    std::vector<int32>::iterator it;    // used to iterate over an entire layer

    // get reference to Editor so we can access the undo stack
    Editor *editor = static_cast<Editor *>(topLevelWidget());

    // Takes in account the current scrolling
    int32 mouse_x = evt->x() + editor->_ed_scrollarea->horizontalScrollBar()->value();
    int32 mouse_y = evt->y() + editor->_ed_scrollarea->verticalScrollBar()->value();

    switch(_tile_mode) {
    case PAINT_TILE: { // wrap up painting tiles
        if(editor->_select_on == true) {
            std::vector<std::vector<int32> > select_layer = _map->GetSelectionLayer();
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
                std::vector<std::vector<int32> > select_layer = _map->GetSelectionLayer();
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
            std::vector<std::vector<int32> > select_layer = _map->GetSelectionLayer();
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
        QMessageBox::warning(this, "Tile editing mode",
                             "ERROR: Invalid tile editing mode!");
    } // switch on tile editing mode

    // Clear the selection layer.
    if((_tile_mode != MOVE_TILE || _moving == true) && editor->_select_on == true) {
        _map->ClearSelectionLayer();
    } // clears when not moving tiles or when moving tiles and not selecting them

    if(editor->_select_on == true && _moving == false && _tile_mode == MOVE_TILE)
        _moving = true;
    else
        _moving = false;

    // Draw the changes.
    _map->updateGL();
    return true;
} // void EditorScrollView::contentsMouseReleaseEvent(QMouseEvent *evt)



void EditorScrollArea::contentsContextMenuEvent(QContextMenuEvent *evt)
{
    // Don't popup a menu outside the map.
    if((evt->y() / TILE_HEIGHT) >= static_cast<uint32>(_map->GetHeight()) ||
            (evt->x() / TILE_WIDTH)  >= static_cast<uint32>(_map->GetWidth()) ||
            evt->x() < 0 || evt->y() < 0)
        return;

    _tile_index_x = evt->x() / TILE_WIDTH;
    _tile_index_y = evt->y() / TILE_HEIGHT;
    _context_menu->exec(QCursor::pos());
    (static_cast<Editor *>(topLevelWidget()))->statusBar()->clearMessage();
}



void EditorScrollArea::keyPressEvent(QKeyEvent *evt)
{
    if(evt->key() == Qt::Key_Delete) {
        // TODO: Handle object deletion
    }
}

///////////////////////////////////////////////////////////////////////////////
// EditorScrollView class -- private slots
///////////////////////////////////////////////////////////////////////////////

void EditorScrollArea::_MapInsertRow()
{
    _map->InsertRow(_tile_index_y);
}



void EditorScrollArea::_MapInsertColumn()
{
    _map->InsertCol(_tile_index_x);
}



void EditorScrollArea::_MapDeleteRow()
{
    _map->DeleteRow(_tile_index_y);
}



void EditorScrollArea::_MapDeleteColumn()
{
    _map->DeleteCol(_tile_index_x);
}


///////////////////////////////////////////////////////////////////////////////
// EditorScrollView class -- private functions
///////////////////////////////////////////////////////////////////////////////

void EditorScrollArea::_PaintTile(int32 index_x, int32 index_y)
{
    // get reference to current tileset
    Editor *editor = static_cast<Editor *>(topLevelWidget());
    QTableWidget *table = static_cast<QTableWidget *>(editor->_ed_tabs->currentWidget());
    QString tileset_name = editor->_ed_tabs->tabText(editor->_ed_tabs->currentIndex());

    // Detect the first selection range and use to paint an area
    QList<QTableWidgetSelectionRange> selections = table->selectedRanges();
    QTableWidgetSelectionRange selection;
    if(selections.size() > 0)
        selection = selections.at(0);

    int32 multiplier = _map->tileset_def_names.indexOf(tileset_name);
    if(multiplier == -1) {
        _map->tileset_def_names.append(tileset_name);
        multiplier = _map->tileset_def_names.indexOf(tileset_name);
    } // calculate index of current tileset

    if(selections.size() > 0 && (selection.columnCount() * selection.rowCount() > 1)) {
        // Draw tiles from tileset selection onto map, one tile at a time.
        for(int32 i = 0; i < selection.rowCount() && index_y + i < (int32)_map->GetHeight(); i++) {
            for(int32 j = 0; j < selection.columnCount() && index_x + j < (int32)_map->GetWidth(); j++) {
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



void EditorScrollArea::_DeleteTile(int32 index_x, int32 index_y)
{
    // Record information for undo/redo action.
    _tile_indeces.push_back(QPoint(index_x, index_y));
    _previous_tiles.push_back(GetCurrentLayer()[index_y][index_x]);
    _modified_tiles.push_back(-1);

    // Delete the tile.
    GetCurrentLayer()[index_y][index_x] = -1;
}



void EditorScrollArea::_AutotileRandomize(int32 &tileset_num, int32 &tile_index)
{
    std::map<int, std::string>::iterator it = _map->tilesets[tileset_num]->
            autotileability.find(tile_index);

    if(it != _map->tilesets[tileset_num]->autotileability.end()) {
        // Set up for opening autotiling.lua.
        ReadScriptDescriptor read_data;
        if(read_data.OpenFile("dat/tilesets/autotiling.lua") == false)
            QMessageBox::warning(this, "Loading File...",
                                 QString("ERROR: could not open dat/tilesets/autotiling.lua for reading!"));

        read_data.OpenTable(it->second);
        int32 random_index = RandomBoundedInteger(1, static_cast<int32>(read_data.GetTableSize()));
        read_data.OpenTable(random_index);
        std::string tileset_name = read_data.ReadString(1);
        tile_index = read_data.ReadInt(2);
        read_data.CloseTable();
        tileset_num = _map->tileset_def_names.indexOf(
                          QString(tileset_name.c_str()));
        read_data.CloseTable();

        read_data.CloseFile();

        _AutotileTransitions(tileset_num, tile_index, it->second);
    } // must have an autotileable tile
}



void EditorScrollArea::_AutotileTransitions(int32 &/*tileset_num*/, int32 &/*tile_index*/, const std::string &/*tile_group*/)
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



TRANSITION_PATTERN_TYPE EditorScrollArea::_CheckForTransitionPattern(const std::string &current_group,
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


///////////////////////////////////////////////////////////////////////////////
// LayerCommand class -- public functions
///////////////////////////////////////////////////////////////////////////////

LayerCommand::LayerCommand(std::vector<QPoint> indeces, std::vector<int32> previous, std::vector<int32> modified,
                           uint32 layer_id, Editor *editor, const QString &text, QUndoCommand *parent) :
    QUndoCommand(text, parent)
{
    _tile_indeces = indeces;
    _previous_tiles = previous;
    _modified_tiles = modified;
    _edited_layer_id = layer_id;
    _editor = editor;
}



void LayerCommand::undo()
{

    for(int32 i = 0; i < static_cast<int32>(_tile_indeces.size()); ++i) {
        _editor->_ed_scrollarea->_map->GetLayers()[_edited_layer_id].tiles[_tile_indeces[i].y()][_tile_indeces[i].x()] = _previous_tiles[i];
    }

    _editor->_ed_scrollarea->_map->updateGL();
}



void LayerCommand::redo()
{

    for(int32 i = 0; i < static_cast<int32>(_tile_indeces.size()); i++) {
        _editor->_ed_scrollarea->_map->GetLayers()[_edited_layer_id].tiles[_tile_indeces[i].y()][_tile_indeces[i].x()] = _modified_tiles[i];
    }
    _editor->_ed_scrollarea->_map->updateGL();
}

} // namespace hoa_editor
