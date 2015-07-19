///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    tileset_editor.cpp
*** \author  Bar�� Soner U�akl? blackkknight@hotmail.com
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for editor's tileset editor dialog
*******************************************************************************/

#include "utils/utils_pch.h"
#include "tileset_editor.h"

#include <QGraphicsView>
#include <boost/concept_check.hpp>

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

using namespace vt_script;

namespace vt_editor
{

////////////////////////////////////////////////////////////////////////////////
////////// TilesetDisplay class
////////////////////////////////////////////////////////////////////////////////

TilesetDisplay::TilesetDisplay():
    _last_x(-1),
    _last_y(-1),
    _is_adding_collision(false)
{
    tileset = new Tileset();
    // Red color with 50% transparency
    _red_square = QPixmap(16, 16);
    _red_square.fill(QColor(255, 0, 0, 125));

    setSceneRect(0, 0, 512, 512);
    graphic_view = new QGraphicsView(this);
    graphic_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphic_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphic_view->setFixedSize(512, 512);
}

TilesetDisplay::~TilesetDisplay()
{
    delete tileset;
    delete graphic_view;
}

void TilesetDisplay::UpdateScene()
{
    if (!tileset->IsInitialized())
        return;

    clear();
    setSceneRect(0, 0, 512, 512);
    setBackgroundBrush(QBrush(Qt::gray));

    // Draw the tileset
    addPixmap(tileset->tiles[0]);

    // Draw transparent red over the unwalkable tile quadrants
    for(uint32 i = 0; i < 16; ++i) {
        for(uint32 j = 0; j < 16; ++j) {

            if(tileset->walkability[i * 16 + j][0] != 0) {
                addPixmap(_red_square)->setPos(j * 32, i * 32);
            }

            if(tileset->walkability[i * 16 + j][1] != 0) {
                addPixmap(_red_square)->setPos(j * 32 + 16, i * 32);
            }

            if(tileset->walkability[i * 16 + j][2] != 0) {
                addPixmap(_red_square)->setPos(j * 32, i * 32 + 16);
            }

            if(tileset->walkability[i * 16 + j][3] != 0) {
                addPixmap(_red_square)->setPos(j * 32 + 16, i * 32 + 16);
            }
        }
    }

    // Draws the grid that visually seperates each tile in the tileset image
    _DrawGrid();

    update();
}

void TilesetDisplay::_DrawGrid()
{
    for(uint32 y = 0; y < 512; y+=16) {
        for(uint32 x = 0; x < 512; x+=16) {
            addLine(x, 0, x, 512, QPen(Qt::DashLine));
            addLine(0, y, 512, y, QPen(Qt::DashLine));
        }
    }
}

void TilesetDisplay::resizeScene(int /*w*/, int /*h*/)
{
    setSceneRect(0, 0, 512, 512);
    UpdateScene();
}

void TilesetDisplay::mousePressEvent(QGraphicsSceneMouseEvent *evt)
{
    if (evt->button() == Qt::LeftButton) {
        // Keeps in memory whether the user is adding or removing red squares
        // when doing a mouse drag.
        _is_adding_collision = !_GetTileCollisionValue(evt);

        mouseMoveEvent(evt);
    }
}

void TilesetDisplay::mouseReleaseEvent(QGraphicsSceneMouseEvent *evt)
{
    if (evt->button() == Qt::LeftButton) {
        // Reset the last position to permit drawing again
        _last_x = -1;
        _last_y = -1;
    }
}

void TilesetDisplay::mouseMoveEvent(QGraphicsSceneMouseEvent *evt)
{
    // Don't deal with the event if the left button isn't included.
    if (evt->buttons() ^= Qt::LeftButton)
        return;

    QPointF pos = evt->scenePos();
    // Don't process clicks outside of the tileset image
    if((pos.x() < 0) || (pos.y() < 0) || pos.x() >= 512 || pos.y() >= 512)
        return;

    // Prevent spamming the mouse move event.
    if (_last_x == (pos.x() / 16) && _last_y == (pos.y() / 16))
        return;

    _last_x = pos.x() / 16;
    _last_y = pos.y() / 16;

    _UpdateTiles(evt);
} // contentsMousePressEvent(...)

void TilesetDisplay::_UpdateTiles(QGraphicsSceneMouseEvent *evt)
{
    if (!tileset->IsInitialized())
        return;

    QPointF pos = evt->scenePos();
    // Determine which tile the user clicked
    int32 tile_x = pos.x() / 32;
    int32 x_offset = ((int32)pos.x()) % 32;
    int32 tile_y = pos.y() / 32;
    int32 y_offset = ((int32)pos.y()) % 32;

    int32 tile_index = 0;

    // Now determine which quadrant of that tile was clicked, and change it's walkability status
    if((x_offset < 16) && (y_offset < 16))  // Upper left quadrant (index 0)
        tile_index = 0;
    else if((x_offset >= 16) && (y_offset < 16)) // Upper right quadrant (index 1)
        tile_index = 1;
    else if((x_offset < 16) && (y_offset >= 16)) // Lower left quadrant (index 2)
        tile_index = 2;
    else if((x_offset >= 16) && (y_offset >= 16)) // Lower right quadrant (index 3)
        tile_index = 3;

    tileset->walkability[tile_y * 16 + tile_x][tile_index] = _is_adding_collision;

    UpdateScene();
}

bool TilesetDisplay::_GetTileCollisionValue(QGraphicsSceneMouseEvent *evt)
{
    if (!tileset->IsInitialized())
        return false;

    QPointF pos = evt->scenePos();
    // Determine which tile the user clicked
    int32 tile_x = pos.x() / 32;
    int32 x_offset = ((int32)pos.x()) % 32;
    int32 tile_y = pos.y() / 32;
    int32 y_offset = ((int32)pos.y()) % 32;

    if((x_offset < 16) && (y_offset < 16)) // Upper left quadrant (index 0)
        return tileset->walkability[tile_y * 16 + tile_x][0];
    else if((x_offset >= 16) && (y_offset < 16)) // Upper right quadrant (index 1)
        return tileset->walkability[tile_y * 16 + tile_x][1];
    else if((x_offset < 16) && (y_offset >= 16)) // Lower left quadrant (index 2)
        return tileset->walkability[tile_y * 16 + tile_x][2];
    else if((x_offset >= 16) && (y_offset >= 16)) // Lower right quadrant (index 3)
        return tileset->walkability[tile_y * 16 + tile_x][3];

    // Should not happen
    return false;
}

////////////////////////////////////////////////////////////////////////////////
////////// TilesetEditor class
////////////////////////////////////////////////////////////////////////////////

TilesetEditor::TilesetEditor(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Tileset Editor"));

    // Create GUI Items
    _new_pbut = new QPushButton(tr("New"), this);
    _open_pbut = new QPushButton(tr("Open"), this);
    _save_pbut = new QPushButton(tr("Save"), this);
    _exit_pbut = new QPushButton(tr("Exit"), this);
    _exit_pbut->setDefault(true);

    // Create the tileset view
    _tset_display = new TilesetDisplay;
    _tset_display->setSceneRect(0, 0, 512, 512);
    _tset_display->setBackgroundBrush(QBrush(Qt::black));
    _tset_display->graphic_view->setMinimumSize(512, 512);
    setMinimumSize(600, 600);

    // connect button signals
    connect(_new_pbut, SIGNAL(clicked()), this, SLOT(_NewFile()));
    connect(_open_pbut, SIGNAL(clicked()), this, SLOT(_OpenFile()));
    connect(_save_pbut, SIGNAL(clicked()), this, SLOT(_SaveFile()));
    connect(_exit_pbut, SIGNAL(released()), this, SLOT(reject()));

    // Add all of the aforementioned widgets into a nice-looking grid layout
    _dia_layout = new QGridLayout(this);
    _dia_layout->addWidget(_new_pbut, 0, 1);
    _dia_layout->addWidget(_open_pbut, 1, 1);
    _dia_layout->addWidget(_save_pbut, 2, 1);
    _dia_layout->addWidget(_exit_pbut, 3, 1);
    _dia_layout->addWidget(_tset_display->graphic_view, 0, 0, 3, 1);
}



TilesetEditor::~TilesetEditor()
{
    delete _new_pbut;
    delete _open_pbut;
    delete _save_pbut;
    delete _exit_pbut;
    delete _dia_layout;
    delete _tset_display;
}



void TilesetEditor::_NewFile()
{
    // Get the filename to open through the OpenFileName dialog
    QString filename = QFileDialog::getOpenFileName(this, "Map Editor -- File Open",
                       "data/tilesets", "Tileset Images (*.png)");

    if (filename.isEmpty())
        return;

    if (!_tset_display->tileset->New(filename, true)) {
        QMessageBox::warning(this, tr("Map Editor"),
                                tr("Failed to create new tileset."));
    }

    // Set the background image
    _tset_display->addPixmap(_tset_display->tileset->tiles[0]);

    // Refreshes the scene
    _tset_display->UpdateScene();
}



void TilesetEditor::_OpenFile()
{
    // Get the filename to open through the OpenFileName dialog
    QString file_name = QFileDialog::getOpenFileName(this, "Map Editor -- File Open",
                        "data/tilesets", "Tilesets (*.lua)");

    if (file_name.isEmpty())
        return;

    if (!_tset_display->tileset->Load(file_name, true)) {
        QMessageBox::warning(this, tr("Map Editor"),
                                tr("Failed to load existing tileset."));
    }

    // Refreshes the scene
    _tset_display->UpdateScene();
}


void TilesetEditor::_SaveFile()
{
    // Data must exist in order to save it
    if(_tset_display->tileset->IsInitialized())
        if(_tset_display->tileset->Save() == false)
            QMessageBox::warning(this, tr("Map Editor"),
                                 tr("Failed to save data to tileset definition file."));
}

} // namespace vt_editor
