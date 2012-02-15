///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    option.h
*** \author  Raj Sharma, roos@allacrost.org
*** \brief   Header file for OptionBox GUI control and supporting classes
***
*** OptionBox is a type of GUI control that allows you to create several
*** option choices, which the player can select from by using the arrow keys.
*** ***************************************************************************/

#ifndef __OPTION_HEADER__
#define __OPTION_HEADER__

#include "defs.h"
#include "utils.h"

#include "gui.h"
#include "system.h"

namespace hoa_gui {

  //! [phuedx] Should these constants not be in a core settings file?

//! \brief The number of milliseconds that the menu cursor blinks when in the blinking state
const int32 VIDEO_CURSOR_BLINK_RATE = 40;

//! \brief The number of milliseconds it takes to scroll when the cursor goes past the end of an option box
const int32 VIDEO_OPTION_SCROLL_TIME = 100;

//! \brief These are the types of events that an option box can generate
enum OptionBoxEvent {
	VIDEO_OPTION_INVALID          = -1,
	//! The selected option changed
	VIDEO_OPTION_SELECTION_CHANGE =  0,
	//! The player confirmed a selection
	VIDEO_OPTION_CONFIRM          =  1,
	//! The player pressed the cancel key
	VIDEO_OPTION_CANCEL           =  2,
	//! Two options were switched by the player
	VIDEO_OPTION_SWITCH           =  3,
	//! The player tried to exceed the top most option
	VIDEO_OPTION_BOUNDS_UP        =  4,
	//! The player tried to exceed the bottom most option
	VIDEO_OPTION_BOUNDS_DOWN      =  5,
	//! The player tried to exceed the left most option
	VIDEO_OPTION_BOUNDS_LEFT      =  6,
	//! The player tried to exceed the right most option
	VIDEO_OPTION_BOUNDS_RIGHT     =  7,
	VIDEO_OPTION_TOTAL            =  8
};


//! \brief Type identifiers for options, whether the option is text, an image, or an align flag
enum OptionElementType {
	VIDEO_OPTION_ELEMENT_INVALID      = -1,
	//! Identifies mark-up for left alignment
	VIDEO_OPTION_ELEMENT_LEFT_ALIGN   =  0,
	//! Identifies mark-up for center alignment
	VIDEO_OPTION_ELEMENT_CENTER_ALIGN =  1,
	//! Identifies mark-up for right alignment
	VIDEO_OPTION_ELEMENT_RIGHT_ALIGN  =  2,
	//! Identifies the position tag
	VIDEO_OPTION_ELEMENT_POSITION     =  3,
	//! Represents option images
	VIDEO_OPTION_ELEMENT_IMAGE        =  4,
	//! Represents option text
	VIDEO_OPTION_ELEMENT_TEXT         =  5,
	VIDEO_OPTION_ELEMENT_TOTAL        =  6
};


//! \brief For representing the visual state of the menu cursor
enum CursorState {
	VIDEO_CURSOR_STATE_INVALID  = -1,
	//! Hides the cursor so it is not drawn on the screen
	VIDEO_CURSOR_STATE_HIDDEN   =  0,
	//! Shows the cursor next to the selected option
	VIDEO_CURSOR_STATE_VISIBLE  =  1,
	//! Causes the cursor to continuously blink
	VIDEO_CURSOR_STATE_BLINKING =  2,
	VIDEO_CURSOR_STATE_TOTAL    =  3
};


//! \brief Modes to control how the cursor wraps around when the cursor exceeds the list boundary
enum WrapMode {
	VIDEO_WRAP_MODE_INVALID  = -1,
	//! Cursor retains its position on the list boundary
	VIDEO_WRAP_MODE_NONE     =  0,
	//! Cursor wraps around left to right, top to bottom, when exceeding the boundary
	VIDEO_WRAP_MODE_STRAIGHT =  1,
	//! Similar to straight, but the cursor will move one row or column when it exceeds a column or row boundary
	VIDEO_WRAP_MODE_SHIFTED  =  2,
	VIDEO_WRAP_MODE_TOTAL    =  3
};


//! \brief These select modes control how confirming works when you choose options
enum SelectMode {
	VIDEO_SELECT_INVALID = -1,
	//! Options only require a single confirmation
	VIDEO_SELECT_SINGLE  =  0,
	//! The first confirmation highlights the item, and the second confirms it.
	//! \note If you press confirm on one item and confirm again on a different item, the two items get switched.
	VIDEO_SELECT_DOUBLE  =  1,
	VIDEO_SELECT_TOTAL   =  2
};

enum HORIZONTAL_ARROWS_POSITION {
	H_POSITION_INVALID = -1,
	H_POSITION_BOTTOM = 0,
	H_POSITION_MIDDLE = 1,
	H_POSITION_TOP = 2,
	H_POSITION_TOTAL = 3
};

enum VERTICAL_ARROWS_POSITION {
	V_POSITION_INVALID = -1,
	V_POSITION_LEFT = 0,
	V_POSITION_CENTER = 1,
	V_POSITION_RIGHT = 2,
	V_POSITION_TOTAL = 3
};


namespace private_gui {

/** \name Option Tag Constants
*** These constants are used in option format strings to parse the text
*** and construct the options from that text.
**/
//@{
const uint16 OPEN_TAG     = static_cast<uint16>('<');
const uint16 END_TAG      = static_cast<uint16>('>');
const uint16 LEFT_TAG1    = static_cast<uint16>('l');
const uint16 CENTER_TAG1  = static_cast<uint16>('c');
const uint16 RIGHT_TAG1   = static_cast<uint16>('r');
const uint16 LEFT_TAG2    = static_cast<uint16>('L');
const uint16 CENTER_TAG2  = static_cast<uint16>('C');
const uint16 RIGHT_TAG2   = static_cast<uint16>('R');
//@}


/** ****************************************************************************
*** \brief A class which encapsulates the various contents of an option.
***
*** Contents can include text, images, mark-up tags, etc.
***
*** \todo Store the element content in here as well instead of in the Option
*** class. Will require multiple derived classes for images, text, alignment
*** flags, etc. Text should also be stored as rendered text images
*** ***************************************************************************/
class OptionElement {
public:
	//! \brief A type indentifier for determining what this option represents
	OptionElementType type;

	//! \brief A simple integer value used for various purposes such as offsets
	int32 value;
};


/** ****************************************************************************
*** \brief Holds the bound coordinates for a particular "cell" in an option box.
***
*** This is used for calculations when drawing an option box
*** ***************************************************************************/
class OptionCellBounds {
public:

	//! \brief The y coordinate for the top, bottom, and center of the cell
	float y_top, y_center, y_bottom;

	//! \brief The x coordinate for the left, right, and center of the cell
	float x_left, x_center, x_right;
};


/** ****************************************************************************
*** \brief Represents one particular option in a list and all its elements
***
*** For example in a shop menu, one option might be "Mythril Knife" and contain
*** an icon of a knife, the text "Mythril Knife", a right alignment flag, and
*** finally the text "500 drunes".
***
*** \todo Text should contain rendered text images instead of text that needs
*** to be rendered and drawn every frame
***
*** \todo Add support for animated images? (Low priority task)
*** ***************************************************************************/
class Option {
public:
	Option();

	~Option();

	Option(const Option& copy);

	Option& operator=(const Option& copy);

	//! \brief Deletes all data maintained by the object
	void Clear();

	//! \brief A flag to specify whether this option is disabled or not
	bool disabled;

	//! \brief The elements that this option is composed of
	std::vector<OptionElement> elements;

	//! \brief Contains all pieces of text for this option
	std::vector<hoa_utils::ustring> text;

	//! \brief Contains all images used for this option
	hoa_video::StillImage* image;
}; // class Option

} // namespace private_gui


/** ****************************************************************************
*** \brief Represents rows and columns of options that the player may select
***
*** The OptionBox control is used for presenting the player with several choices,
*** of actions to take, wares to buy, etc.
***
*** Scrolling feature now supported.
***
*** Contains support for drawing scroll arrows on the option box to indicate
*** additional options that are not currently in view
***
*** \todo Allow the user to set an arbitrary cursor blink rate and scroll rate
***
*** \todo Support multiple forms of scroll animations.
*** (This should be considered a low priority task, if dealt with at all.)
*** ***************************************************************************/
class OptionBox : public private_gui::GUIControl {
public:
	OptionBox();

	~OptionBox()
		{}

	/** \brief Updates any blinking or scrolling effects for the option box
	*** \param frame_time The number of milliseconds elapsed this frame
	*** This method also resets any registered events, allowing new events
	*** to be processed by the option box. Therefore it is recommended that
	*** this method be called on every frame while it is active.
	**/
	void Update(uint32 frame_time = hoa_system::SystemManager->GetUpdateTime());

	//! \brief Draws each enabled option to the screen
	void Draw();

	/** \brief Sets the visible dimensions, 2D data structure, and visible data layout
	*** \param width The width to set for the entire option box
	*** \param height The height to set for the entire option box
	*** \param num_cols The number of columns of options (should be non-zero)
	*** \param num_rows The number of rows of options (should be non-zero)
	*** \param cell_cols The number of columns of cells that should be visible in the display area (should be non-zero)
	*** \param cell_rows The number of rows of cells that should be visible in the display area (should be non-zero)
	***
	*** These settings will determine the size of each cell. Cell size is computed as (width/cell_cols), (height/cell_rows).
	*** The num_cols and num_rows must be greater than or equal to cell_rows and cell_cols. This requirement is necessary
	*** because it is silly to try to represent a structure of 2 columns and 2 rows in a layout of 4 columns and 4 rows. Failure
	*** to observe this requirement will result in no changes being made by this function.
	**/
	void SetDimensions(float width, float height, uint8 num_cols, uint8 num_rows, uint8 cell_cols, uint8 cell_rows);

	/** \brief Sets the options to display in this option box
	*** \param option_text A vector of unicode strings which contain the text for each item, along with any formatting tags
	*** \note Calling this function will clear any current options
	***
	*** If any single option contains formatting errors, then the entire set of options will not be added.
	*** Example of an option with formatting: "<img/weapons/mythril.png>Mythril knife<r>500 drunes"
	**/
	void SetOptions(const std::vector<hoa_utils::ustring>& option_text);

	//! \brief Removes all options and their allocated data from the OptionBox
	void ClearOptions();

	/** \brief Adds a blank new option to the OptionBox
	*** The option added is an empty string. Invoke the various AddOptionElement*() methods to construct the option after this call.
	**/
	void AddOption();

	/** \brief Adds a new option to the OptionBox
	*** \param text The formatting text for the new option
	*** The option will not be added if it contained formatting errors.
	**/
	void AddOption(const hoa_utils::ustring &text);

	/** \brief Appends a text string element to an existing option
	*** \param option_index The index of the option to append the text element to
	*** \param text The unicode string to add to the text
	*** \note This string is treated as pure text and formatting options embedded in the string will
	*** <b>not</b> be processed by this function.
	**/
	void AddOptionElementText(uint32 option_index, const hoa_utils::ustring& text);

	/** \brief Appends an image element to an existing option
	*** \param option_index The index of the option to append the image element to
	*** \param filename The name of the image file to load for use in this option
	**/
	void AddOptionElementImage(uint32 option_index, std::string& image_filename);

	/** \brief Appends an image element to an existing option
	*** \param option_index The index of the option to append the image element to
	*** \param image A pointer to the image to create a copy of for the option (must be non-NULL)
	**/
	void AddOptionElementImage(uint32 option_index, const hoa_video::StillImage* image);

	/** \brief Appends an alignment element to an existing option
	*** \param option_index The index of the option to append the alignment element to
	*** \param postion_type The only valid values for this argument are: VIDEO_OPTION_ELEMENT_LEFT_ALIGN,
	*** VIDEO_OPTION_ELEMENT_CENTER_ALIGN, and VIDEO_OPTION_ELEMENT_RIGHT_ALIGN. All other values will be
	*** ignored.
	**/
	void AddOptionElementAlignment(uint32 option_index, OptionElementType position_type);

	/** \brief Appends a position element to an existing option
	*** \param option_index The index of the option to append the alignment element to
	*** \param postion_length The number of pixels to move the draw cursor position
	**/
	void AddOptionElementPosition(uint32 option_index, uint32 position_length);

	/** \brief Changes the stored information of a particular option
	*** \param index The index of the option to change
	*** \param text The text to change the option to
	*** \return False if the option text could not be changed
	**/
	bool SetOptionText(uint32 index, const hoa_utils::ustring &text);

	/** \brief Sets the currently selected option (0 to # of options - 1)
	*** \param index The desired selection index in the list of options
	*** You should not call this method until after options have been added.
	*** If no options are currently stored when this method is called, the
	*** method will print a warning message and return.
	**/
	void SetSelection(uint32 index);

	/** \brief Enables or disables the option located at a specified index
	*** \param index The index of the option to enable or disable
	*** \param enable Set to true to enable, false to disable
	**/
	void EnableOption(uint32 index, bool enable);

	/** \brief Determines if an option is enabled or not
	*** \param index The index of the option in the list to check
	*** \return True if the option is enabled, false if it is not
	**/
	bool IsOptionEnabled(uint32 index);

	/** \brief Returns true if the given option is enabled
	*** \param index The index of the option to check
	*** \return True if option is enabled, false if it's not
	**/
	bool IsEnabled(uint32 index) const;

	/** \brief Retrieves a pointer to the image embedded within the option
	*** \param index The index of the option to retrieve the image
	*** \return NULL if the index is invalid or the option does not embed an image, otherwise a valid pointer to a StillImage
	**/
	hoa_video::StillImage* GetEmbeddedImage(uint32 index) const;

	/** \brief Used to determine whether the option box is initialized and ready for use
	*** \param error_messages Used to report the list of reasons why the option box is not initialized
	*** \return True if the option box is initialized, false if it is not
	**/
	bool IsInitialized(std::string& error_messages);

	/** \name Input Processing Methods
	*** \brief Processes the input commands for moving the cursor, selecting options, etcetra
	**/
	//@{
	void InputConfirm();
	void InputCancel();
	void InputUp();
	void InputDown();
	void InputLeft();
	void InputRight();
	//@}

	//! \name Member Access Functions
	//@{
	/** \brief Sets the alignment of the option text and cursor
	*** \param xalign Left/center/right alignment of text in the cell
	*** \param yalign Top/center/bottom alignment of text in the cell
	**/
	void SetOptionAlignment(int32 xalign, int32 yalign)
		{ _option_xalign = xalign; _option_yalign = yalign; _initialized = IsInitialized(_initialization_errors); }

	/** \brief Sets the option selection mode (single or double confirm)
	*** \param mode The selection mode to be set
	**/
	void SetSelectMode(SelectMode mode)
		{ _selection_mode = mode; _initialized = IsInitialized(_initialization_errors); }

	/** \brief Sets the behavior for vertical wrapping of the cursor
	*** \param mode The vertical wrap behavior to set
	**/
	void SetVerticalWrapMode(WrapMode mode)
		{ _vertical_wrap_mode = mode; }

	/** \brief Sets the behavior for horizontal wrapping of the cursor
	*** \param mode The horizontal wrap behavior to set
	**/
	void SetHorizontalWrapMode(WrapMode mode)
		{ _horizontal_wrap_mode = mode; }

    /** \brief Enables to skip disabled items while moving the cursor
    *** \param toggle True skippes disabled items
    **/
    void SetSkipDisabled(bool toggle)
        { _skip_disabled = toggle; }

	/** \brief Enables or disables the ability to switch the location of two options
	*** \param enable True enables switching, false disables it
	*** \note Switching is disabled by default
	**/
	void SetEnableSwitching(bool enable)
		{ _enable_switching = enable; }

	/** \brief Sets the cursor offset relative to the text position
	*** \param x Horizontal offset (the sign determines whether its left or right)
	*** \param y Vertical offset (the sign determines whether its up or down)
	**/
	void SetCursorOffset(float x, float y)
		{ _cursor_xoffset = x; _cursor_yoffset = y; }

	/** \brief Sets the text style to use for this textbox.
	*** \param style The style intended \see #TextStyle
	**/
	void SetTextStyle(const hoa_video::TextStyle& style);

	/** \brief Sets the state of the cursor icon
	*** \param state The cursor state to set
	**/
	void SetCursorState(CursorState state);

	/** \brief Sets the horizontal arrows position
	*** \param position The position to set
	**/
	void SetHorizontalArrowsPosition(HORIZONTAL_ARROWS_POSITION position);

	/** \brief Sets the vertical arrows position
	*** \param position The position to set
	**/
	void SetVerticalArrowsPosition(VERTICAL_ARROWS_POSITION position);

	/** \brief Checks if the option box is in the process of scrolling
	*** \return True if the option box is scrolling, false if it is not
	**/
	bool IsScrolling() const
		{ return _scrolling; }

	/** \brief Retreives an event that has occurred, or zero if no event occurred.
	*** \return An integer int representing an option box event (i.e. cancel, confirm, left, right, etc.)
	*** \note Calling the Update() method will clear any registered events
	**/
	int32 GetEvent()
		{ return _event; }

	/** \brief Returns the index of the currently selected option
	*** \return The current selection index, or -1 if nothing is selected
	**/
	int32 GetSelection() const
		{ return _selection; }

	//! \brief Returns the number of rows of options
	int32 GetNumberRows() const
		{ return _number_rows; }

	//! \brief Returns the number of columns of options
	int32 GetNumberColumns() const
		{ return _number_columns; }

	//! \brief Retreives the number of options in the option box
	uint32 GetNumberOptions() const
		{ return _options.size(); }
	//@}

	/** \brief Used to enable scissoring of the option box
	*** \param enable Set to true to enable, or false to disable
	*** \param owner Set to true to scissor to the _owner's size, or false to scissor to the box's size
	**/
	void Scissoring( bool enable, bool owner )
		{ _scissoring = enable; _scissoring_owner = owner; }

private:
	//! \brief When set to true, indicates that the option box is initialized and ready to be used
	bool _initialized;

	//! \name Option Property Members
	//@{
	/** \brief The vector containing all of the options
	*** This 1D structure represents a 2D array of options. For an option box with 2 rows and 3 columns,
	*** the first 3 elements would contain the contents of the 1st row (left to right) and the last 3
	*** elements would contain the second row.
	**/
	std::vector<private_gui::Option> _options;

	//! \brief The total number of rows and columns of data represented by the box
	int32 _number_rows, _number_columns;

	//! \brief How many rows and columns of cells can fit in the option box dimensions
	int32 _number_cell_rows, _number_cell_columns;

	//! \brief The dimenions of each cell within the option box
	float _cell_width, _cell_height;

	//! \brief The selection mode for the option box (ie single or double confirm selection)
	SelectMode _selection_mode;

	//! \brief The wrapping modes used for horizontal and vertical cursor movement
	WrapMode _horizontal_wrap_mode, _vertical_wrap_mode;

	//! \brief Skips disabled items while moving the cursor
	bool _skip_disabled;

	//! \brief When set to true, the user may switch the locations of two different options
	bool _enable_switching;
	//@}

	//! \name Drawing Related Members
	//@{
	//! \brief The text style that the options should be rendered in
	hoa_video::TextStyle _text_style;

	//! \brief The column of row of data that is drawn in the top-left cell
	uint32 _draw_left_column, _draw_top_row;

	//! \brief Retains the x and y offsets for where the cursor should be drawn relative to the selected option
	float _cursor_xoffset, _cursor_yoffset;

	//! \brief A draw offset used for the option box when it is scrolling
	int32 _scroll_offset;

	//! \brief The horizontal alignment type for option cell contents
	int32 _option_xalign;

	//! \brief The vertical alignment type for option cell contents
	int32 _option_yalign;

	//! \brief True if scissoring is enabled
	bool _scissoring;

	//! \brief True if scissoring should be applied according to the owner window, false for the box's size
	bool _scissoring_owner;

	//! \brief When true the scroll arrows for the horizontal and vertical directions will be drawn
	bool _draw_horizontal_arrows, _draw_vertical_arrows;

	//! \brief When true the scroll arrows in these directions will be drawn with the grey arrow
	bool _grey_up_arrow, _grey_down_arrow, _grey_left_arrow, _grey_right_arrow;
	//@}

	//! \name Active State Members
	//@{
	//! \brief Holds the most recent OptionBox event that occurred
	int32 _event;

	//! \brief The index of the currently selected option
	int32 _selection;

	/** \brief The first selection that the player made when in double-confirm selection mode
	*** \note This is set to -1 when no first selection has been made
	**/
	int32 _first_selection;

	//! \brief The current cursor state (blinking, visible, hidden, etc)
	CursorState _cursor_state;

	//! \brief Used for the blinking cursor feature. When false, the cursor will not be drawn.
	bool _blink;

	//! \brief The timer used for controlling the cursor blinking rate
	int32 _blink_time;

	//! \brief Set to true if the box is currently in the middle of scrolling
	bool _scrolling;

	//! \brief The timer used for controlling option scrolling
	int32 _scroll_time;

	//! \brief Indicates the scrolling direction; 1 for down or -1 for up
	int32 _scroll_direction;

	//! \brief The position of the horizontal scroll arrows
	HORIZONTAL_ARROWS_POSITION _horizontal_arrows_position;

	//! \brief The position of the vertical scroll arrows
	VERTICAL_ARROWS_POSITION _vertical_arrows_position;

	//@}

	// ---------- Private methods

	/** \brief helper function to parse text for an option box, and fill an Option structure
	*** \param format_string the formatted string, using the XML structure described by SetOptions()
	*** \param option which option the string corresponds to
	*** \return True if the option was successfully constructed from the string, or false if there was an error.
	**/
	bool _ConstructOption(const hoa_utils::ustring &format_string, private_gui::Option &option);

	/** \brief Changes the selected option by making a movement relative to the current selection
	*** \param offset The amount to move in specified direction (ie 1 row up, 1 column right, etc.)
	*** \param horizontal true if moving horizontally, false if moving vertically
	*** \return False if the selection does not change
	**/
	bool _ChangeSelection(int32 offset, bool horizontal);

	/** \brief Sets draw flags and determines the x and y coordinates for rendering an option inside a cell
	*** \param xalign The x alignement for the cell contents
	*** \param yalign The y alignement for the cell contents
	*** \param bounds The boundary coordinates for the option cell
	*** \param x A reference to return the x position for the cursor
	*** \param y A reference to return the y position for the cursor
	***
	*** This function also moves the draw cursor to the position specified by the alignment and the cell boundaries.
	**/
	void _SetupAlignment(int32 xalign, int32 yalign, const private_gui::OptionCellBounds& bounds, float& x, float& y);

	/** \brief Calling this method will re-examine which scroll arrow boolean members need to be set
	*** This should be called whenever the data or cell columns/rows changed, whenever the selection in changed,
	*** and in general whenever the viewable contents of the option box are changed.
	**/
	void _DetermineScrollArrows();

	/** \brief Draws a single option cell
	*** \param op The option contents to draw within the cell
	*** \param bounds The boundary coordinates for the information cell
	*** \param scroll_offset A draw offset for when the option box is in the process of scrolling from one option to another
	*** \param left_edge Returns a coordinate that represents the left edge of the cell content (as opposed to strictly the cell boundary)
	**/
	void _DrawOption(const private_gui::Option& op, const private_gui::OptionCellBounds &bounds, float cell_offset, float &left_edge);

	/** \brief Draws the cursor
	*** \param op The option contents to draw within the cell
	*** \param bounds The boundary coordinates for the information cell
	*** \param scroll_offset A draw offset for when the option box is in the process of scrolling from one option to another
	*** \param left_edge A draw coordiante that represents the left edge of the cell content (as opposed to strictly the cell boundary)
	*** \param darken If true, the cursor image will be drawn with a 50% alpha black color applied
	***
	*** This method should be called immediately proceeding the _DrawOption method so that it can retrieve the correct value for left_edge
	*** from the cell which should have a cursor drawn.
	**/
	void _DrawCursor(const private_gui::OptionCellBounds &bounds, float cell_offset, float left_edge, bool darken);

	//! \brief Draws an outline of the option box and the inner cell boundaries
	void _DEBUG_DrawOutline();
}; // class OptionBox : public private_gui::GUIControl

} // namespace hoa_gui

#endif  // __OPTION_HEADER__
