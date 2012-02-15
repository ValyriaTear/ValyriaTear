///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    tileset_editor.cpp
*** \author  Bar�� Soner U�akl? blackkknight@hotmail.com
*** \brief   Source file for editor's tileset editor dialog
*******************************************************************************/

#include "tileset_editor.h"

using namespace hoa_script;
using namespace hoa_video;

namespace hoa_editor
{

////////////////////////////////////////////////////////////////////////////////
////////// TilesetDisplay class
////////////////////////////////////////////////////////////////////////////////

TilesetDisplay::TilesetDisplay()
{
	tileset = new Tileset();
	// Red color with 50% transparency
	_red_square.SetColor(Color(1.0f, 0.0f, 0.0f, 0.5f));
	_red_square.SetDimensions(0.5f, 0.5f);

	setMouseTracking(true);
}



TilesetDisplay::~TilesetDisplay()
{
	delete tileset;
	VideoManager->SingletonDestroy();
}



void TilesetDisplay::initializeGL()
{
	// Destroy and recreate the video engine
	// NOTE: This is actually a very bad practice to do. We have to figure out an alternative.
	VideoManager->SingletonDestroy();
	VideoManager = VideoEngine::SingletonCreate();
	VideoManager->SetTarget(VIDEO_TARGET_QT_WIDGET);

	VideoManager->SingletonInitialize();

	VideoManager->ApplySettings();
	VideoManager->FinalizeInitialization();
	VideoManager->ToggleFPS();
}



void TilesetDisplay::paintGL()
{
	VideoManager->SetCoordSys(0.0f, VideoManager->GetScreenWidth() / TILE_WIDTH,
		VideoManager->GetScreenHeight() / TILE_HEIGHT, 0.0f);
	VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
	VideoManager->Clear(Color::blue);
	VideoManager->Move(0, 0);

	if (tileset->IsInitialized() == true) {
		// Draw the tileset as a single image
		tileset->tiles[0].Draw();

		// Draw transparent red over the unwalkable tile quadrants
		for (uint32 i = 0; i < 16; i++) {
			for (uint32 j = 0; j < 16; j++) {
				VideoManager->Move(j, i);

				if (tileset->walkability[i * 16 + j][0] != 0) {
					_red_square.Draw();
				}

				VideoManager->MoveRelative(0.5f, 0.0f);
				if (tileset->walkability[i * 16 + j][1] != 0) {
					_red_square.Draw();
				}

				VideoManager->MoveRelative(-0.5f, 0.5f);
				if (tileset->walkability[i * 16 + j][2] != 0) {
					_red_square.Draw();
				}

				VideoManager->MoveRelative(0.5f, 0.0f);
				if (tileset->walkability[i * 16 + j][3] != 0) {
					_red_square.Draw();
				}
			}
		}
	}

	// Draws the grid that visually seperates each tile in the tileset image
	VideoManager->DrawGrid(0.0f, 0.0f, 0.5f, 0.5f, Color::black);
}



void TilesetDisplay::resizeGL(int w, int h)
{
	VideoManager->SetResolution(512, 512);
	VideoManager->ApplySettings();
}



void TilesetDisplay::mousePressEvent(QMouseEvent* evt)
{
	// Don't process clicks outside of the tileset image
	if ((evt->x() < 0) || (evt->y() < 0) || evt->x() >= 512 || evt->y() >= 512)
		return;

	if (tileset->IsInitialized()) {
		// Determine which tile the user clicked
		int32 tile_x, tile_y;
		tile_x = evt->x() / 32;
		tile_y = evt->y() / 32;
	
		// Now determine which quadrant of that tile was clicked, and change it's walkability status
		if (((evt->x() % 32) < 16) && ((evt->y() % 32) < 16)) { // Upper left quadrant (index 0)
			tileset->walkability[tile_y * 16 + tile_x][0] = (tileset->walkability[tile_y * 16 + tile_x][0] ? 0 : 1);
		}
		else if (((evt->x() % 32) >= 16) && ((evt->y() % 32) < 16)) { // Upper right quadrant (index 1)
			tileset->walkability[tile_y * 16 + tile_x][1] = (tileset->walkability[tile_y * 16 + tile_x][1] ? 0 : 1);
		}
		else if (((evt->x() % 32) < 16) && ((evt->y() % 32) >= 16)) { // Lower left quadrant (index 2)
			tileset->walkability[tile_y * 16 + tile_x][2] = (tileset->walkability[tile_y * 16 + tile_x][2] ? 0 : 1);
		}
		else if (((evt->x() % 32) >= 16) && ((evt->y() % 32) >= 16)) { // Lower right quadrant (index 3)
			tileset->walkability[tile_y * 16 + tile_x][3] = (tileset->walkability[tile_y * 16 + tile_x][3] ? 0 : 1);
		}
	}

	updateGL();
} // contentsMousePressEvent(...)

////////////////////////////////////////////////////////////////////////////////
////////// TilesetEditor class
////////////////////////////////////////////////////////////////////////////////

TilesetEditor::TilesetEditor(QWidget* parent, const QString& name, bool prop)
	: QDialog(parent, (const char*)name)
{
	setCaption("Tileset Editor");

	// Create GUI Items
	_new_pbut = new QPushButton("New", this);
	_open_pbut = new QPushButton("Open", this);
	_save_pbut = new QPushButton("Save", this);
	_exit_pbut = new QPushButton("Exit", this);
	_exit_pbut->setDefault(true);

	// Create the window
	_tset_display = new TilesetDisplay;
	_tset_display->resize(512, 512);
	_tset_display->setFixedWidth(512);
	_tset_display->setFixedHeight(512);

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
	_dia_layout->addWidget(_tset_display, 0, 0, 3, 1);
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
	QString filename = QFileDialog::getOpenFileName(this, "HoA Level Editor -- File Open",
		"img/tilesets", "Tileset Images (*.png)");

	if (!filename.isEmpty())
	{
		if (QFile::exists(Tileset::CreateDataFilename(Tileset::CreateTilesetName(filename)))) {
			int response = QMessageBox::question(this, tr("HoA Level Editor"),
				tr("There already exists a data file that corresponds to this tileset image. "
				"Executing a save operation will overwrite all data in this file. "
				"Do you wish to continue anyway?"),
				QMessageBox::Yes,
				QMessageBox::No
			);

			if (response == QMessageBox::No)
				return;
		}

		if (_tset_display->tileset->New(filename, true) == false)
			QMessageBox::warning(this, tr("HoA Level Editor"),
				tr("Failed to create new tileset."));

		_tset_display->updateGL();
	} // there must be a file specified for loading
}



void TilesetEditor::_OpenFile()
{
	// Get the filename to open through the OpenFileName dialog
	QString file_name = QFileDialog::getOpenFileName(this, "HoA Level Editor -- File Open",
		"dat/tilesets", "Tilesets (*.lua)");

	if (!file_name.isEmpty())
	{
		// The tileset loading function takes just the name of the tileset
		// as an argument, so strip the file name accordingly.
		int i = file_name.lastIndexOf("/");
		file_name = file_name.remove(0, i + 1);
		file_name.chop(4);

		if (_tset_display->tileset->Load(file_name, true) == false)
			QMessageBox::warning(this, tr("HoA Level Editor"),
				tr("Failed to load existing tileset."));

		_tset_display->updateGL();
	} // file must exist in order to open it
}


void TilesetEditor::_SaveFile()
{
	// Data must exist in order to save it
	if (_tset_display->tileset->IsInitialized())
		if (_tset_display->tileset->Save() == false)
			QMessageBox::warning(this, tr("HoA Level Editor"),
				tr("Failed to save data to tileset definition file."));
}

} // namespace hoa_editor
