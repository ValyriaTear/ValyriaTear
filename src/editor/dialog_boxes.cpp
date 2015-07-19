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
*** \file    dialog_boxes.cpp
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for all of the editor's dialog boxes.
*** **************************************************************************/

#include "utils/utils_pch.h"
#include "dialog_boxes.h"

namespace vt_editor
{


///////////////////////////////////////////////////////////////////////////////
// MapPropertiesDialog class -- all functions
///////////////////////////////////////////////////////////////////////////////

MapPropertiesDialog::MapPropertiesDialog
(QWidget *parent, const QString & /*name*/, bool prop)
    : QDialog(parent)
{
    setWindowTitle("Map Properties...");

    // Set up the height spinbox
    _height_label = new QLabel("Height (in tiles):", this);
    _height_sbox  = new QSpinBox(this);
    _height_sbox->setMinimum(map_min_height);
    _height_sbox->setMaximum(100);
    _height_sbox->setValue(map_min_height * 2);

    // Set up the width spinbox
    _width_label = new QLabel(" Width (in tiles):", this);
    _width_sbox  = new QSpinBox(this);
    _width_sbox->setMinimum(map_min_width);
    _width_sbox->setMaximum(100);
    _width_sbox->setValue(map_min_width * 2);

    // Set up the cancel and okay push buttons
    _cancel_pbut = new QPushButton("Cancel", this);
    _ok_pbut     = new QPushButton("OK", this);
    _cancel_pbut->setDefault(true);

    // At construction no tilesets are checked, so disable the ok button
    _ok_pbut->setEnabled(false);
    connect(_ok_pbut,     SIGNAL(released()), this, SLOT(accept()));
    connect(_cancel_pbut, SIGNAL(released()), this, SLOT(reject()));

    // Set up the list of selectable tilesets
    QDir tileset_dir("data/tilesets");
    _tileset_tree = new QTreeWidget(this);
    _tileset_tree->setColumnCount(1);
    _tileset_tree->setHeaderLabels(QStringList("Tilesets"));
    connect(_tileset_tree, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this,
            SLOT(_EnableOKButton()));
    QList<QTreeWidgetItem *> tilesets;

    // Loop through all files in the tileset directory. Start the loop at 2 to
    // skip over the present and parent working directories ("." and "..")
    // Also add the data/tilesets path.
    for(uint32 i = 2; i < tileset_dir.count(); i++) {
        // Exclude the autotiling.lua file as it's no tileset.
        // TODO: Move files and handle this better...
        if (tileset_dir[i] == QString("autotiling.lua"))
            continue;

        QString tileset_definition_file = "data/tilesets/" + tileset_dir[i];
        tilesets.append(new QTreeWidgetItem((QTreeWidget *)0,
                                            QStringList(tileset_definition_file)));
        tilesets.back()->setCheckState(0, Qt::Unchecked); // enables checkboxes

        // Indicates that the user wants to edit the map's existing properties
        if(prop) {
            // Get a reference to the Editor
            Editor *editor = static_cast<Editor *>(parent);

            // Iterate through the names of the tabs to see which ones in the
            // list are already present and set their checkbox appropriately.
            for(int j = 0; j < editor->_ed_tabs->count(); j++)
                if(tilesets.back()->text(0) == editor->_ed_tabs->tabText(j)) {
                    tilesets.back()->setCheckState(0, Qt::Checked);
                    _ok_pbut->setEnabled(true);
                    break;
                } // both tileset names must match to set the checkbox
        } // edit map's properties
    } // iterate through all files in the tileset directory
    _tileset_tree->insertTopLevelItems(0, tilesets);

    // Indicates that the user wants to edit the map's existing properties
    if(prop) {
        // Get a reference to the Editor
        Editor *editor = static_cast<Editor *>(parent);

        _height_sbox->setValue(editor->_grid->GetHeight());
        _width_sbox->setValue(editor->_grid->GetWidth());
    }

    // Add all of the aforementioned widgets into a nice-looking grid layout
    _dia_layout = new QGridLayout(this);
    _dia_layout->addWidget(_height_label, 0, 0);
    _dia_layout->addWidget(_height_sbox,  1, 0);
    _dia_layout->addWidget(_width_label,  2, 0);
    _dia_layout->addWidget(_width_sbox,   3, 0);
    _dia_layout->addWidget(_tileset_tree, 0, 1, 5, -1);
    _dia_layout->addWidget(_cancel_pbut,  6, 0);
    _dia_layout->addWidget(_ok_pbut,      6, 1);
} // MapPropertiesDialog constructor


MapPropertiesDialog::~MapPropertiesDialog()
{
    delete _height_label;
    delete _height_sbox;
    delete _width_label;
    delete _width_sbox;
    delete _cancel_pbut;
    delete _ok_pbut;
    delete _tileset_tree;
    delete _dia_layout;
} // MapPropertiesDialog destructor


// ********** Private slot **********

void MapPropertiesDialog::_EnableOKButton()
{
    // Disable the ok button if no tilesets are checked, otherwise enable it
    int num_items = _tileset_tree->topLevelItemCount();
    for(int i = 0; i < num_items; i++) {
        // At least one tileset must be checked in order to enable push button
        if(_tileset_tree->topLevelItem(i)->checkState(0) == Qt::Checked) {
            _ok_pbut->setEnabled(true);
            return;
        }
    } // iterate through all items in the _tileset_tree

    // If this point is reached, no tilesets are checked.
    _ok_pbut->setEnabled(false);
} // MapPropertiesDialog::_EnableOKButton()

///////////////////////////////////////////////////////////////////////////////
// LayerDialog class -- all functions
///////////////////////////////////////////////////////////////////////////////

LayerDialog::LayerDialog(QWidget *parent, const QString & /*name*/)
    : QDialog(parent)
{
    setWindowTitle(tr("Layer properties"));
    _dialog_layout = new QGridLayout(this);

    // Set up the push buttons
    _cancel_pbut = new QPushButton(tr("Cancel"), this);
    _ok_pbut = new QPushButton(tr("OK"), this);
    _ok_pbut->setDefault(true);
    connect(_ok_pbut,     SIGNAL(released()), this, SLOT(accept()));
    connect(_cancel_pbut,     SIGNAL(released()), this, SLOT(reject()));

    _name_label = new QLabel(tr("Layer name: "), this);
    _name_edit = new QLineEdit(this);
    _name_label->setBuddy(_name_edit);

    _type_label = new QLabel(tr("Type: "), this);
    _type_cbox = new QComboBox(this);
    _type_label->setBuddy(_type_cbox);

    // Add layer types
    _type_cbox->addItem("ground");
    _type_cbox->addItem("sky");

    // Add all of the aforementioned widgets into a nice-looking grid layout
    _dialog_layout->addWidget(_name_label,     0, 0);
    _dialog_layout->addWidget(_name_edit,      1, 0);

    _dialog_layout->addWidget(_type_label,     0, 1);
    _dialog_layout->addWidget(_type_cbox,      1, 1);

    _dialog_layout->addWidget(_cancel_pbut,    2, 0);
    _dialog_layout->addWidget(_ok_pbut,        2, 1);
} // LayerDialog constructor


LayerDialog::~LayerDialog()
{
    delete _cancel_pbut;
    delete _ok_pbut;
    delete _name_label;
    delete _name_edit;
    delete _type_label;
    delete _type_cbox;

    delete _dialog_layout;
} // LayerDialog destructor


// ********** Private slots **********
LayerInfo LayerDialog::_GetLayerInfo()
{
    LayerInfo layer_info;

    layer_info.name = _name_edit->text().toStdString();
    layer_info.layer_type = getLayerType(_type_cbox->currentText().toStdString());

    return layer_info;
}

} // namespace vt_editor
