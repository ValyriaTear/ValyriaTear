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
*** \file    editor.cpp
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for editor's main window and user interface.
*** **************************************************************************/

#include "utils/utils_pch.h"
#include "editor.h"

#include <QTableWidgetItem>
#include <QScrollBar>
#include <QGraphicsView>

using namespace vt_utils;
using namespace vt_script;

namespace vt_editor
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

    // set scollview to nullptr because it's being checked inside _TilesEnableActions
    _grid = nullptr;

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

    _ed_tabs = nullptr;
    _ed_layer_view = nullptr;
    _ed_layer_toolbar = nullptr;
    setCentralWidget(_ed_splitter);
    resize(800, 600);

    _ed_tileset_layer_splitter = new QSplitter(_ed_splitter);
    _ed_tileset_layer_splitter->setOrientation(Qt::Vertical);

    // set the window icon
    setWindowIcon(QIcon("data/icons/program_icon.ico"));

    // Initialize the script manager
    ScriptManager = ScriptEngine::SingletonCreate();
    ScriptManager->SingletonInitialize();

    // Open the global script
    if (!_global_script.OpenFile("data/editor/editor.lua"))
        PRINT_ERROR << "failed to load the editor global script: " << "data/editor/editor.lua" << std::endl;
}



Editor::~Editor()
{
    if(_grid != nullptr)
        delete _grid;

    if(_ed_tabs != nullptr)
        delete _ed_tabs;

    if(_ed_layer_view != nullptr)
        delete _ed_layer_view;

    if(_ed_layer_toolbar != nullptr)
        delete _ed_layer_toolbar;

    delete _ed_tileset_layer_splitter;
    delete _ed_splitter;

    delete _undo_stack;

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
    if(_grid != nullptr) {
        _save_as_action->setEnabled(true);
        _save_action->setEnabled(_grid->GetChanged());
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
    if(_grid != nullptr) {
        _toggle_grid_action->setEnabled(true);
    } // map must exist in order to set view options
    else {
        _toggle_grid_action->setEnabled(false);
    } // map does not exist, can't view it*/
}



void Editor::_TilesEnableActions()
{
    if(_grid != nullptr) {
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
    if(_grid != nullptr)
        _edit_tileset_action->setEnabled(false);
    else
        _edit_tileset_action->setEnabled(true);
}



void Editor::_MapMenuSetup()
{
    if(_grid != nullptr) {
        _map_properties_action->setEnabled(true);
    } // map must exist in order to set properties
    else {
        _map_properties_action->setEnabled(false);
    } // map does not exist, can't modify it
}


void Editor::_SetupMainView()
{
    // Can't be initialized if there is no QgraphicsScene ready
    if (!_grid)
        return;

    if(_ed_tabs != nullptr)
        delete _ed_tabs;
    _ed_tabs = new QTabWidget();
    _ed_tabs->setTabPosition(QTabWidget::South);

    if(_ed_layer_view != nullptr)
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
    if(_ed_layer_toolbar != nullptr)
        delete _ed_layer_toolbar;
    _ed_layer_toolbar = new QToolBar("Layers", _ed_tileset_layer_splitter);

    // Add the buttons
    QPushButton *button = new QPushButton(QIcon(QString("data/editor/document-new.png")), QString(), _ed_layer_toolbar);
    button->setContentsMargins(1, 1, 1, 1);
    button->setFixedSize(20, 20);
    button->setToolTip(tr("Add Layer"));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(_MapAddLayer()));
    _ed_layer_toolbar->addWidget(button);

    button = new QPushButton(QIcon(QString("data/editor/edit-rename.png")), QString(), _ed_layer_toolbar);
    button->setContentsMargins(1, 1, 1, 1);
    button->setFixedSize(20, 20);
    button->setToolTip(tr("Modify Layer"));
    _ed_layer_toolbar->addWidget(button);
    button->setDisabled(true);

    button = new QPushButton(QIcon(QString("data/editor/edit-delete.png")), QString(), _ed_layer_toolbar);
    button->setContentsMargins(1, 1, 1, 1);
    button->setFixedSize(20, 20);
    button->setToolTip(tr("Delete Layer"));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(_MapDeleteLayer()));
    _ed_layer_toolbar->addWidget(button);
    _delete_layer_button = button;

    button = new QPushButton(QIcon(QString("data/editor/go-up.png")), QString(), _ed_layer_toolbar);
    button->setContentsMargins(1, 1, 1, 1);
    button->setFixedSize(20, 20);
    button->setToolTip(tr("Move Layer Up"));
    _ed_layer_toolbar->addWidget(button);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(_MapMoveLayerUp()));
    // Keep a reference to later set the button state.
    _layer_up_button = button;

    button = new QPushButton(QIcon(QString("data/editor/go-down.png")), QString(), _ed_layer_toolbar);
    button->setContentsMargins(1, 1, 1, 1);
    button->setFixedSize(20, 20);
    button->setToolTip(tr("Move Layer Down"));
    _ed_layer_toolbar->addWidget(button);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(_MapMoveLayerDown()));
    // Keep a reference to later set the button state.
    _layer_down_button = button;

    button = new QPushButton(QIcon(QString("data/editor/eye.png")), QString(), _ed_layer_toolbar);
    button->setContentsMargins(1, 1, 1, 1);
    button->setFixedSize(20, 20);
    button->setToolTip(tr("Toggle visibility of the layer"));
    _ed_layer_toolbar->addWidget(button);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(_ToggleLayerVisibility()));

    // Left of the screen
    _ed_splitter->addWidget(_grid->_graphics_view);

    // right part
    _ed_tileset_layer_splitter->addWidget(_ed_layer_view);
    _ed_tileset_layer_splitter->addWidget(_ed_layer_toolbar);
    _ed_tileset_layer_splitter->addWidget(_ed_tabs);

    _ed_splitter->addWidget(_ed_tileset_layer_splitter);
}



void Editor::_FileNew()
{
    if(!_EraseOK()) {
        statusBar()->showMessage(tr("No map created! Unsaved data is still existing."), 5000);
        return;
    }

    MapPropertiesDialog *new_map = new MapPropertiesDialog(this, "new_map", false);

    if(new_map->exec() != QDialog::Accepted) {
        statusBar()->showMessage(tr("No map created!"), 5000);
        delete new_map;
        return;
    }

    if(_grid)
        delete _grid;
    _grid = new Grid(_ed_splitter, tr("Untitled"), new_map->GetWidth(), new_map->GetHeight());
    // Set default edit mode
    _grid->_layer_id = 0;
    _grid->_tile_mode  = PAINT_TILE;

    _SetupMainView();

    QTreeWidget *tilesets = new_map->GetTilesetTree();
    int num_items     = tilesets->topLevelItemCount();
    int checked_items = 0;
    for(int i = 0; i < num_items; ++i) {
        if(tilesets->topLevelItem(i)->checkState(0) == Qt::Checked)
            ++checked_items;
    }

    // Used to show the progress of tilesets that have been loaded.
    QProgressDialog *new_map_progress =
        new QProgressDialog(tr("Loading tilesets..."), nullptr, 0, checked_items, this,
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
                statusBar()->showMessage(tr("Couldn't load map! Invalid tileset given"), 5000);

                // Hide and delete progress bar
                new_map_progress->hide();
                delete new_map_progress;
                delete new_map;
                _FileClose();
                return;
            }
            _ed_tabs->addTab(a_tileset->table, tilesets->topLevelItem(i)->text(0));
            _grid->tilesets.push_back(a_tileset);
            _grid->tileset_def_names.push_back(a_tileset->GetDefintionFilename());
        } // tileset must be checked
    } // iterate through all possible tilesets
    new_map_progress->setValue(checked_items);

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

    // Add default layers
    QIcon icon(QString("data/editor/eye.png"));
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

    // Adds the corresponding layers in the grid class memory
    std::vector<Layer> layers = _grid->GetLayers();
    for (uint32 i = 0; i < 4; ++i) {
        Layer layer;
        layer.layer_type = i < 3 ? GROUND_LAYER : SKY_LAYER;
        QString name = tr("Background %1").arg(i + 1);
        layer.name = name.toStdString();
        layer.Resize(new_map->GetWidth(), new_map->GetHeight());
        layer.Fill(-1); // Make the layer empty

        layers.push_back(layer);
    }

    _ed_layer_view->adjustSize();
    // Fix a bug in the width computation of the icon
    _ed_layer_view->setColumnWidth(1, 20);

    _ed_layer_view->setCurrentItem(background); // layer 0

    _undo_stack->setClean();

    // Hide and delete progress bar
    new_map_progress->hide();
    delete new_map_progress;

    delete new_map;

    _grid->SetInitialized(true);
    _grid->UpdateScene();

    statusBar()->showMessage(tr("New map created"), 5000);
} // void Editor::_FileNew()



void Editor::_FileOpen()
{
    if(!_EraseOK()) {
        statusBar()->showMessage(tr("No map created! Unsaved data is still existing."), 5000);
        return;
    }

    // file to open
    QString file_name = QFileDialog::getOpenFileName(this, tr("Map Editor -- File Open"),
                        "data/story", "Maps (*.lua)");

    if(file_name.isEmpty()) {
        statusBar()->showMessage(tr("No map created! Empty filename given."), 5000);
        return;
    }

    if(_grid)
        delete _grid;
    _grid = new Grid(_ed_splitter, tr("Untitled"), 0, 0);
    // Set default edit mode
    _grid->_tile_mode  = PAINT_TILE;
    _grid->_layer_id = 0;

    _SetupMainView();

    _grid->SetFileName(file_name);
    _grid->LoadMap();

    _UpdateLayersView();

    // Count for the tileset names
    int num_items = _grid->tileset_def_names.count();
    int progress_steps = 0;

    // Used to show the progress of tilesets has been loaded.
    QProgressDialog *new_map_progress =
        new QProgressDialog(tr("Loading tilesets..."), nullptr, 0, num_items, this,
                            Qt::Widget | Qt::FramelessWindowHint | Qt::WindowTitleHint);
    new_map_progress->setWindowTitle(tr("Creating Map..."));

    // Set the progress bar
    new_map_progress->move(this->pos().x() + this->width() / 2  - new_map_progress->width() / 2,
                            this->pos().y() + this->height() / 2 - new_map_progress->height() / 2);
    new_map_progress->show();

    for(QStringList::ConstIterator it = _grid->tileset_def_names.begin();
            it != _grid->tileset_def_names.end(); ++it) {
        new_map_progress->setValue(progress_steps++);

        TilesetTable *a_tileset = new TilesetTable();
        if(!a_tileset->Load(*it)) {
            const std::string mes = tr("Failed to load tileset image: ").toStdString()
                                    + (*it).toStdString();
            QMessageBox::critical(this, tr("Map Editor"),
                                    tr(mes.c_str()));
            statusBar()->showMessage(tr("Couldn't load map! Invalid tileset given"), 5000);

            // Hide and delete progress bar
            new_map_progress->hide();
            delete new_map_progress;
            _FileClose();
            return;
        }

        _ed_tabs->addTab(a_tileset->table, *it);
        _grid->tilesets.push_back(a_tileset);
    } // iterate through all tilesets in the map
    new_map_progress->setValue(progress_steps);

    _grid->Resize(_grid->GetWidth(), _grid->GetHeight());
    _grid->UpdateScene();
    _grid->SetInitialized(true);

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

    // Hide and delete progress bar
    new_map_progress->hide();
    delete new_map_progress;

    _undo_stack->setClean();
    statusBar()->showMessage(QString(tr("Opened \'%1\'")).
                                arg(_grid->GetFileName()), 5000);

    setWindowTitle(QString("Map Editor - ") + _grid->GetFileName());

} // void Editor::_FileOpen()



void Editor::_FileSaveAs()
{
    // get the file name from the user
    QString file_name = QFileDialog::getSaveFileName(this,
                        tr("Map Editor -- File Save"), "data/story", "Maps (*.lua)");

    if(file_name.isEmpty()) {
        statusBar()->showMessage("Save abandoned.", 5000);
        return;
    }

    _grid->SetFileName(file_name);
    _FileSave();
    setWindowTitle(QString("Map Editor - ") + _grid->GetFileName());
}



void Editor::_FileSave()
{
    if(_grid->GetFileName().isEmpty() ||
            _grid->GetFileName() == tr("Untitled")) {
        _FileSaveAs();
        return;
    } // gets a file name if it is blank

    _grid->SaveMap();      // actually saves the map
    _undo_stack->setClean();
    setWindowTitle(QString("%1").arg(_grid->GetFileName()));
    statusBar()->showMessage(QString(tr("Saved \'%1\' successfully!")).
                             arg(_grid->GetFileName()), 5000);
}



void Editor::_FileClose()
{
    // Checks to see if the map is unsaved.
    if(!_EraseOK())
        return;

    if(_grid != nullptr) {
        delete _grid;
        _grid = nullptr;
        _undo_stack->clear();

        // Enable appropriate actions
        _TilesEnableActions();
    } // scrollview must exist first

    if(_ed_tabs != nullptr) {
        delete _ed_tabs;
        _ed_tabs = nullptr;
    } // tabs must exist first

    if(_ed_layer_toolbar != nullptr) {
        delete _ed_layer_toolbar;
        _ed_layer_toolbar = nullptr;
    }

    if(_ed_layer_view != nullptr) {
        delete _ed_layer_view;
        _ed_layer_view = nullptr;
    }

    setWindowTitle(tr("Map Editor"));
}



void Editor::_FileQuit()
{
    // Checks to see if the map is unsaved.
    if(_EraseOK())
        qApp->exit(0);
}



void Editor::_ViewToggleGrid()
{
    if(!_grid)
        return;

    _grid_on = !_grid_on;
    _toggle_grid_action->setChecked(_grid_on);
    _grid->SetGridOn(_grid_on);
    _grid->UpdateScene();
}


void Editor::_TileLayerFill()
{
    // get reference to current tileset
    QTableWidget *table = static_cast<QTableWidget *>(_ed_tabs->currentWidget());

    // put selected tile from tileset into tile array at correct position
    int32 tileset_index = table->currentRow() * 16 + table->currentColumn();
    int32 multiplier = _grid->tileset_def_names.indexOf(_ed_tabs->tabText(_ed_tabs->currentIndex()));

    if(multiplier == -1) {
        _grid->tileset_def_names.append(_ed_tabs->tabText(_ed_tabs->currentIndex()));
        multiplier = _grid->tileset_def_names.indexOf(_ed_tabs->tabText(_ed_tabs->currentIndex()));
    } // calculate index of current tileset

    std::vector<std::vector<int32> >& current_layer = _grid->GetCurrentLayer();

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
            _grid->_AutotileRandomize(multiplier, tileset_index);
            current_layer[y][x] = tileset_index + multiplier * 256;
            modified.push_back(tileset_index + multiplier * 256);
        }
    }

    LayerCommand *fill_command = new LayerCommand(indeces, previous, modified,
            _grid->_layer_id, this, "Fill Layer");
    _undo_stack->push(fill_command);
    indeces.clear();
    previous.clear();
    modified.clear();

    // Draw the changes.
    _grid->SetChanged(true);
    _grid->UpdateScene();
} // void Editor::_TileLayerFill()



void Editor::_TileLayerClear()
{
    /*std::vector<std::vector<int32> >::iterator it;    // used to iterate over an entire layer
    std::vector<std::vector<int32> >& current_layer = _ed_scrollarea->GetCurrentLayer();

    // Record the information for undo/redo operations.
    std::vector<std::vector<int32> > previous = current_layer;
    //std::vector<std::vector<int32> > modified(current_layer.size(), -1);
    std::vector<std::vector<int32> > indeces(current_layer.size());
    	for (int32 i = 0; i < static_cast<int32>(current_layer.size()); i++)
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
    _grid->SetChanged(true);
    _grid->UpdateScene();
}


void Editor::_TileToggleSelect()
{
    if(!_grid)
        return;

    _select_on = !_select_on;
    _toggle_select_action->setChecked(_select_on);
    _grid->SetSelectOn(_select_on);
}


void Editor::_TileModePaint()
{
    if(!_grid)
        return;

    // Clear the selection layer.
    if(_grid->_moving == true && _select_on == true) {
        _grid->ClearSelectionLayer();
    } // clears when selected tiles were going to be moved but
    // user changed their mind in the midst of the move operation

    _grid->_tile_mode = PAINT_TILE;
    _grid->_moving = false;
}


void Editor::_TileModeMove()
{
    if(!_grid)
        return;

    // Clear the selection layer.
    if(_grid->_moving == true && _select_on == true) {
        _grid->ClearSelectionLayer();
    } // clears when selected tiles were going to be moved but
    // user changed their mind in the midst of the move operation

    _grid->_tile_mode = MOVE_TILE;
    _grid->_moving = false;
}


void Editor::_TileModeDelete()
{
    if(!_grid)
        return;

    // Clear the selection layer.
    if(_grid->_moving == true && _select_on == true) {
        _grid->ClearSelectionLayer();
    } // clears when selected tiles were going to be moved but
    // user changed their mind in the midst of the move operation

    _grid->_tile_mode = DELETE_TILE;
    _grid->_moving = false;
}


void Editor::_TilesetEdit()
{
    TilesetEditor *tileset_editor = new TilesetEditor(this);

    tileset_editor->exec();

    delete tileset_editor;
}


void Editor::_MapAddLayer()
{
    if(!_grid)
        return;

    LayerDialog *layer_dlg = new LayerDialog(this, "layer_dialog");

    if(layer_dlg->exec() == QDialog::Accepted) {
        // Apply changes
        LayerInfo layer_info = layer_dlg->_GetLayerInfo();

        _grid->AddLayer(layer_info);

        _UpdateLayersView();

        // The map has been changed
        _grid->SetChanged(true);

    } // only process results if user selected okay

    delete layer_dlg;
}

void Editor::_MapModifyLayer()
{
    if(!_grid)
        return;

    // TODO
}

void Editor::_MapDeleteLayer()
{
    if(!_grid)
        return;

    if(!_CanDeleteLayer(_ed_layer_view->currentItem()))
        return;

    uint32 layer_id = _ed_layer_view->currentItem()->text(0).toUInt();
    std::vector<Layer>& layers = _grid->GetLayers();
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
    _grid->DeleteLayer(layer_id);

    _UpdateLayersView();

    // Select the previous layer when possible
    _SetSelectedLayer(layer_id == 0 ? layer_id : layer_id - 1);

    // The map has been changed
    _grid->SetChanged(true);
}

void Editor::_MapMoveLayerUp()
{
    if(!_grid || !_CanLayerMoveUp(_ed_layer_view->currentItem()))
        return;

    uint32 layer_id = _ed_layer_view->currentItem()->text(0).toUInt();
    if(layer_id == 0)
        return;

    std::vector<Layer>& layers = _grid->GetLayers();
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
    _grid->UpdateScene();

    // Set the layer selection to follow the current layer
    _SetSelectedLayer(layer_id - 1);

    // The map has been changed
    _grid->SetChanged(true);
}

void Editor::_MapMoveLayerDown()
{
    if(!_grid || !_CanLayerMoveDown(_ed_layer_view->currentItem()))
        return;

    uint32 layer_id = _ed_layer_view->currentItem()->text(0).toUInt();
    std::vector<Layer>& layers = _grid->GetLayers();
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
    _grid->UpdateScene();

    // Set the layer selection to follow the current layer
    _SetSelectedLayer(layer_id + 1);

    // The map has been changed
    _grid->SetChanged(true);
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
                    _grid->tilesets.push_back(a_tileset);
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
    std::vector<QTreeWidgetItem *> layer_names = _grid->getLayerItems();
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

    if(!_grid)
        return false;

    // Count the available ground layers
    uint32 ground_layers_count = 0;
    std::vector<Layer>& layers = _grid->GetLayers();

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

    if(_grid)
        _grid->_layer_id = layer_id;

    _layer_up_button->setEnabled(_CanLayerMoveUp(item));
    _layer_down_button->setEnabled(_CanLayerMoveDown(item));
    _delete_layer_button->setEnabled(_CanDeleteLayer(item));
}


void Editor::_ToggleLayerVisibility()
{
    Layer &layer = _grid->GetLayers()[_grid->_layer_id];
    layer.visible = !layer.visible;

    // Show the change
    _grid->UpdateScene();

    // Update the item icon
    if(layer.visible)
        _ed_layer_view->currentItem()->setIcon(1, QIcon(QString("data/editor/eye.png")));
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

    _undo_action = new QAction(QIcon("data/editor/arrow-left.png"), "&Undo", this);
    _undo_action->setShortcut(tr("Ctrl+Z"));
    _undo_action->setStatusTip("Undoes the previous command");
    connect(_undo_action, SIGNAL(triggered()), _undo_stack, SLOT(undo()));

    _redo_action = new QAction(
        QIcon("data/editor/arrow-right.png"),
        "&Redo", this);
    _redo_action->setShortcut(tr("Ctrl+Y"));
    _redo_action->setStatusTip("Redoes the next command");
    connect(_redo_action, SIGNAL(triggered()), _undo_stack, SLOT(redo()));

    _layer_fill_action = new QAction(
        QIcon("data/editor/stock-tool-bucket-fill-22.png"), "&Fill layer", this);
    _layer_fill_action->setStatusTip("Fills current layer with selected tile");
    connect(_layer_fill_action, SIGNAL(triggered()), this, SLOT(_TileLayerFill()));

    _layer_clear_action = new QAction("&Clear layer", this);
    _layer_clear_action->setStatusTip("Clears current layer from any tiles");
    connect(_layer_clear_action, SIGNAL(triggered()), this, SLOT(_TileLayerClear()));

    _toggle_select_action = new QAction(QIcon("data/editor/stock-tool-rect-select-22.png"), "Marquee &Select", this);
    _toggle_select_action->setShortcut(tr("Shift+S"));
    _toggle_select_action->setStatusTip("Rectangularly select tiles on the map");
    _toggle_select_action->setCheckable(true);
    connect(_toggle_select_action, SIGNAL(triggered()), this, SLOT(_TileToggleSelect()));

    _mode_paint_action = new QAction(QIcon("data/editor/stock-tool-pencil-22.png"), "&Paint mode", this);
    _mode_paint_action->setShortcut(tr("Shift+P"));
    _mode_paint_action->setStatusTip("Switches to paint mode to draw tiles on the map");
    _mode_paint_action->setCheckable(true);
    connect(_mode_paint_action, SIGNAL(triggered()), this, SLOT(_TileModePaint()));

    _mode_move_action = new QAction(QIcon("data/editor/stock-tool-arrow.png"), "Mo&ve mode", this);
    _mode_move_action->setShortcut(tr("Shift+V"));
    _mode_move_action->setStatusTip("Switches to move mode to move tiles around on the map");
    _mode_move_action->setCheckable(true);
    connect(_mode_move_action, SIGNAL(triggered()), this, SLOT(_TileModeMove()));

    _mode_delete_action = new QAction(QIcon("data/editor/stock-tool-eraser-22.png"), "&Delete mode", this);
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
    if(!_grid)
        return true;

    if(!_grid->GetChanged())
        return true;

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

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// LayerCommand class -- public functions
///////////////////////////////////////////////////////////////////////////////

LayerCommand::LayerCommand(std::vector<QPoint> indeces, std::vector<int32> previous, std::vector<int32> modified,
                           uint32 layer_id, Editor *editor, const QString &text, QUndoCommand *parent) :
    QUndoCommand(text, parent),
    _tile_indeces(indeces),
    _previous_tiles(previous),
    _modified_tiles(modified),
    _edited_layer_id(layer_id),
    _editor(editor)
{}

void LayerCommand::undo()
{

    for(int32 i = 0; i < static_cast<int32>(_tile_indeces.size()); ++i) {
        _editor->_grid->GetLayers()[_edited_layer_id].tiles[_tile_indeces[i].y()][_tile_indeces[i].x()] = _previous_tiles[i];
    }

    _editor->_grid->UpdateScene();
}



void LayerCommand::redo()
{

    for(int32 i = 0; i < static_cast<int32>(_tile_indeces.size()); i++) {
        _editor->_grid->GetLayers()[_edited_layer_id].tiles[_tile_indeces[i].y()][_tile_indeces[i].x()] = _modified_tiles[i];
    }
    _editor->_grid->update();
}

} // namespace vt_editor
