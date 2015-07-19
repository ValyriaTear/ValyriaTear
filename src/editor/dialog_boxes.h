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
*** \file    dialog_boxes.h
*** \author  Philip Vorsilak, gorzuate@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for all of the editor's dialog boxes.
*** **************************************************************************/

#ifndef __DIALOG_BOXES_HEADER__
#define __DIALOG_BOXES_HEADER__

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTreeWidget>

#include "editor.h"
#include "grid.h"

namespace vt_editor
{


/** ***************************************************************************
*** \brief A dialog box that allows the user to modify some properties of an
***        existing map.
***
*** The properties that may be modified through this dialog include the
*** following:
*** - The map dimensions (in tiles)
*** - Which tilesets are used by this map
*** **************************************************************************/
class MapPropertiesDialog: public QDialog
{
    // Macro needed to use Qt's slots and signals.
    Q_OBJECT

    // Needed for accessing map properties.
    friend class Editor;
    friend class EditorScrollArea;

public:
    /** \param parent The widget from which this dialog was invoked.
    *** \param name The name of this widget.
    *** \param prop True when accessing an already loaded map's properties,
    ***             false otherwise.
    ***
    *** This class is used in two instances. For presenting a dialog to the
    *** user to (1) create a new map or (2) modify the properties of an already
    *** existing map (such as height, width, or tilesets loaded in the bottom
    *** portion of the editor). For case #1, the parameter prop is false, and
    *** for case #2, it is true.
    **/
    MapPropertiesDialog(QWidget *parent, const QString &name, bool prop);

    ~MapPropertiesDialog();

    //! \name Class member accessor functions
    //@{
    //! \brief Returns the map height from the height spinbox.
    uint32 GetHeight() const {
        return _height_sbox->value();
    }
    //! \brief Returns the map width from the width spinbox.
    uint32 GetWidth()  const {
        return  _width_sbox->value();
    }

    QTreeWidget *GetTilesetTree() const {
        return _tileset_tree;
    }
    //@}

private slots:
    /** \brief Enables or disables the OK push button of this dialog depending
    ***        on whether any tilesets are checked or not.
    **/
    void _EnableOKButton();

private:
    //! \brief A tree for showing all available tilesets.
    QTreeWidget *_tileset_tree;

    //! \brief A spinbox for specifying the map's height.
    QSpinBox *_height_sbox;
    //! \brief A spinbox for specifying the map's width.
    QSpinBox *_width_sbox;

    //! \brief A label used to visually name the height spinbox.
    QLabel *_height_label;
    //! \brief A label used to visually name the width spinbox.
    QLabel *_width_label;

    //! \brief A pushbutton for canceling the new map dialog.
    QPushButton *_cancel_pbut;
    //! \brief A pushbutton for okaying the new map dialog.
    QPushButton *_ok_pbut;

    //! \brief A layout to manage all the labels, spinboxes, and listviews.
    QGridLayout *_dia_layout;
}; // class MapPropertiesDialog: public QDialog

/** ***************************************************************************
*** \brief A dialog box that allows the user to add/modify a layer
***
*** **************************************************************************/
class LayerDialog: public QDialog
{
    // Macro needed to use Qt's slots and signals.
    Q_OBJECT

    // Needed for accessing map properties.
    friend class Editor;
    friend class EditorScrollView;

public:
    /** \param parent The widget from which this dialog was invoked.
    *** \param name The name of this widget.
    **/
    LayerDialog(QWidget *parent, const QString &name);

    ~LayerDialog();

private slots:
    //! \brief Tells the layer info written in the dialog box
    LayerInfo _GetLayerInfo();

private:
    //! \brief A pushbutton for cancelling the action.
    QPushButton *_cancel_pbut;
    //! \brief A pushbutton for finishing the layer addition/modification.
    QPushButton *_ok_pbut;

    //! Layer position, name and type.
    QLabel *_name_label;
    QLabel *_type_label;

    QLineEdit *_name_edit;
    QComboBox *_type_cbox;

    //! \brief A layout to manage all the labels, buttons, and listviews.
    QGridLayout *_dialog_layout;

}; // class MusicDialog

} // namespace vt_editor

#endif // __DIALOG_BOXES_HEADER__
