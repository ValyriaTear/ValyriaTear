///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    dialog_boxes.h
*** \author  Philip Vorsilak, gorzuate@allacrost.org
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

#include "utils.h"

#include "editor.h"

namespace hoa_editor {


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
	friend class EditorScrollView;

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
	MapPropertiesDialog(QWidget* parent, const QString& name, bool prop);

	~MapPropertiesDialog();

	//! \name Class member accessor functions
	//@{
	//! \brief Returns the map height from the height spinbox.
	uint32 GetHeight() const { return _height_sbox->value(); }
	//! \brief Returns the map width from the width spinbox.
	uint32 GetWidth()  const { return  _width_sbox->value(); }

	QTreeWidget* GetTilesetTree() const { return _tileset_tree; }
	//@}

private slots:
	/** \brief Enables or disables the OK push button of this dialog depending
	***        on whether any tilesets are checked or not.
	**/
	void _EnableOKButton();

private:
	//! \brief A tree for showing all available tilesets.
	QTreeWidget* _tileset_tree;

	//! \brief A spinbox for specifying the map's height.
	QSpinBox* _height_sbox;
	//! \brief A spinbox for specifying the map's width.
	QSpinBox* _width_sbox;

	//! \brief A label used to visually name the height spinbox.
	QLabel* _height_label;
	//! \brief A label used to visually name the width spinbox.
	QLabel* _width_label;

	//! \brief A pushbutton for canceling the new map dialog.
	QPushButton* _cancel_pbut;
	//! \brief A pushbutton for okaying the new map dialog.
	QPushButton* _ok_pbut;

	//! \brief A layout to manage all the labels, spinboxes, and listviews.
	QGridLayout* _dia_layout;
}; // class MapPropertiesDialog: public QDialog


/** ***************************************************************************
*** \brief A dialog box that allows the user to declare what music files to 
***        load for this map.
***
*** This only defines the music files that the map should load. It does not
*** define when or how each file should be played. It populates two lists of
*** music files: one containing the music already chosen for the map, and the
*** other with all the remaining music files that are not chosen. Each file is
*** mutually exclusive and is only found in one list or the other, preventing
*** the user from declaring to use the same music file more than once.
*** **************************************************************************/
class MusicDialog: public QDialog
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
	MusicDialog(QWidget* parent, const QString& name);

	~MusicDialog();

	//! \name Class member accessor functions
	//@{
	QListWidget* GetMusicList() const { return _used_music_list; }
	//@}

private slots:
	/** \brief Adds music to the used music list and removes it from the
	***        available music list.
	**/
	void _AddMusic();

	/** \brief Removes music from the used music list and adds it to the
	***        available must list.
	**/
	void _RemoveMusic();

private:
	//! \brief A pushbutton for adding music to the map.
	QPushButton* _add_pbut;
	//! \brief A pushbutton for removing music from the map.
	QPushButton* _remove_pbut;
	//! \brief A pushbutton for finishing map music selection.
	QPushButton* _ok_pbut;

	//! \brief Label for listview showing available music to select from.
	QLabel* _available_label;
	//! \brief Label for listview showing music already in use by the map.
	QLabel* _used_label;

	//! \brief A layout to manage all the labels, buttons, and listviews.
	QGridLayout* _dia_layout;

	//! \brief A listview with all the remaining music files.
	QListWidget* _available_music_list;
	//! \brief A listview with all the already used music files.
	QListWidget* _used_music_list;
}; // class MusicDialog


/** ***************************************************************************
*** \brief A dialog box that allows the user to add new contexts to the map
***
*** This simple dialog allows the user to create a new context by inheriting
*** from one of the existing ones. Inheriting an existing context simply
*** "copies" all tiles over into the new context. This makes it easy to tell
*** which tiles need to be switched. For example, replacing the tiles making up
*** an exterior of a house or other structure with interior ones. Contexts are
*** given a name identifier to help the map designer manage their contexts, but
*** this information isn't displayed or otherwise presented in the game.
*** **************************************************************************/
class ContextPropertiesDialog : public QDialog
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
	ContextPropertiesDialog(QWidget* parent, const QString& name);

	~ContextPropertiesDialog();

	//! \name Class member accessor functions
	//@{
	//! \brief Returns the context name entered into the line edit widget by
	//         the user.
	QString GetName() const { return _name_ledit->text(); }

	QTreeWidget* GetContextTree() const { return _context_tree; }
	//@}

private slots:
	/** \brief Enables or disables the OK push button of this dialog depending
	***        on whether the line edit is empty.
	**/
	void _EnableOKButton();

private:
	//! \brief A tree for showing all available contexts.
	QTreeWidget* _context_tree;

	//! \brief A label used to visually name the line edit.
	QLabel* _name_label;
	//! \brief A line edit for entering in the context's name.
	QLineEdit* _name_ledit;

	//! \brief A pushbutton for canceling the context dialog.
	QPushButton* _cancel_pbut;
	//! \brief A pushbutton for okaying the context dialog.
	QPushButton* _ok_pbut;

	//! \brief A layout to manage all the labels, buttons, and line edits.
	QGridLayout* _dia_layout;
}; // class ContextPropertiesDialog : public QDialog

} // namespace hoa_editor

#endif // __DIALOG_BOXES_HEADER__
