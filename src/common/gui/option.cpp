///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "option.h"
#include "video.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_video;
using namespace hoa_video::private_video;
using namespace hoa_input;
using namespace hoa_gui::private_gui;

namespace hoa_gui {

////////////////////////////////////////////////////////////////////////////////
// Option class methods
////////////////////////////////////////////////////////////////////////////////

Option::Option() :
	disabled(false),
	image(NULL)
{}



Option::~Option() {
	Clear();
}



Option::Option(const Option& copy) :
	disabled(copy.disabled),
	elements(copy.elements),
	text(copy.text)
{
	if (copy.image == NULL) {
		image = NULL;
	}
	else {
		image = new StillImage(*(copy.image));
	}
}



Option& Option::operator=(const Option& copy) {
	// Handle the case were a dumbass assigns an object to itself
	if (this == &copy) {
		return *this;
	}

	disabled = copy.disabled;
	elements = copy.elements;
	text = copy.text;
	if (copy.image == NULL) {
		image = NULL;
	}
	else {
		image = new StillImage(*(copy.image));
	}

	return *this;
}



void Option::Clear() {
	disabled = false;
	elements.clear();
	text.clear();
	if (image != NULL) {
		delete image;
		image = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////
// OptionBox class methods
////////////////////////////////////////////////////////////////////////////////

OptionBox::OptionBox() :
	GUIControl(),
	_initialized(false),
	_number_rows(1),
	_number_columns(1),
	_number_cell_rows(1),
	_number_cell_columns(1),
	_cell_width(0.0f),
	_cell_height(0.0f),
	_selection_mode(VIDEO_SELECT_SINGLE),
	_horizontal_wrap_mode(VIDEO_WRAP_MODE_NONE),
	_vertical_wrap_mode(VIDEO_WRAP_MODE_NONE),
	_skip_disabled(false),
	_enable_switching(false),
	_draw_left_column(0),
	_draw_top_row(0),
	_cursor_xoffset(0.0f),
	_cursor_yoffset(0.0f),
	_scroll_offset(0.0f),
	_option_xalign(VIDEO_X_LEFT),
	_option_yalign(VIDEO_Y_CENTER),
	_scissoring(false),
	_scissoring_owner(false),
	_draw_horizontal_arrows(false),
	_draw_vertical_arrows(false),
	_grey_up_arrow(false),
	_grey_down_arrow(false),
	_grey_left_arrow(false),
	_grey_right_arrow(false),
	_event(0),
	_selection(0),
	_first_selection(-1),
	_cursor_state(VIDEO_CURSOR_STATE_VISIBLE),
	_blink(false),
	_blink_time(0),
	_scrolling(false),
	_scroll_time(0),
	_scroll_direction(0),
	_horizontal_arrows_position(H_POSITION_BOTTOM),
	_vertical_arrows_position(V_POSITION_RIGHT)
{
	// TEMP
	_width = 1.0f;
	_height = 1.0f;
}

void OptionBox::Update(uint32 frame_time) {
	_event = 0; // Clear all events

	_blink = ((_blink_time / VIDEO_CURSOR_BLINK_RATE) % 2) == 1;
	_blink_time += frame_time;

	if (_scrolling) {
		_scroll_time += frame_time;

    	if (_scroll_time > VIDEO_OPTION_SCROLL_TIME) {
			_scroll_time = 0;
			_scrolling = false;
			_scroll_offset = 0.0f;
		}
		else {
			// [phuedx] Calculate the _scroll_offset independant of the coordinate system
			_scroll_offset = (_scroll_time / static_cast<float>(VIDEO_OPTION_SCROLL_TIME)) * _cell_height;
			if (_scroll_direction == -1 ) { // Up
				_scroll_offset = _cell_height - _scroll_offset;
			}
		}
	}
}



void OptionBox::Draw() {
	// Do nothing if the option box is not properly initialized
	if (!IsInitialized(_initialization_errors)) {
		cout << "ERROR: Could not draw OptionBox" << endl;
		return;
	}

	VideoManager->PushState();
	VideoManager->SetDrawFlags(_xalign, _yalign, VIDEO_BLEND, 0);
	VideoManager->DisableScissoring();

	// TODO: This call is also made at the end of this function. It is made here because for some
	// strange reason, only the option box outline is drawn and not the outline for the individual
	// cells. I'm not sure what part of the code between here and the end of this function could
	// affect that. This bug needs to be resolved and then this call to _DEBUG_DrawOutline() should
	// be removed, leaving only the call at the bottom of the function
	if (GUIManager->DEBUG_DrawOutlines() == true) {
		//_DEBUG_DrawOutline();
	}
	float left, right, bottom, top;

	// ---------- (1) Determine the edge dimensions of the option box
	left = 0.0f;
	right = _number_cell_columns * _cell_width;
	bottom = 0.0f;
	top = _number_cell_rows * _cell_height;
	CalculateAlignedRect(left, right, bottom, top);

	CoordSys &cs = VideoManager->_current_context.coordinate_system;

	// ---------- (2) Determine the option cells to be drawn and any offsets needed for scrolling
	VideoManager->SetDrawFlags(_option_xalign, _option_yalign, VIDEO_X_NOFLIP, VIDEO_Y_NOFLIP, VIDEO_BLEND, 0);

	float xoff = _cell_width * cs.GetHorizontalDirection();
	float yoff = -_cell_height * cs.GetVerticalDirection();
	bool finished = false;

	// [phuedx] Align the scroll offset with the current coordinate system
	_scroll_offset *= cs.GetVerticalDirection();

	OptionCellBounds bounds;
	bounds.y_top = top + _scroll_offset;
	bounds.y_center = bounds.y_top - 0.5f * _cell_height * cs.GetVerticalDirection();
	bounds.y_bottom = (bounds.y_center * 2.0f) - bounds.y_top;


  // ---------- (3) Iterate through all the visible option cells and draw them and the draw cursor
	for (uint32 row = _draw_top_row; row < _draw_top_row + _number_cell_rows && finished == false; row++) {

		bounds.x_left = left;
		bounds.x_center = bounds.x_left + (0.5f * _cell_width * cs.GetHorizontalDirection());
		bounds.x_right = (bounds.x_center * 2.0f) - bounds.x_left;

		// Draw the columns of options
		for (uint32 col = _draw_left_column; col < _draw_left_column + _number_cell_columns; ++col) {
			uint32 index = row * _number_cell_columns + col;

			// If there are more visible cells than there are options available we leave those cells empty
			if (index >= GetNumberOptions()) {
				finished = true;
				break;
			}

			float left_edge = 999999.0f; // The x offset to where the visible option contents begin
			_DrawOption(_options.at(index), bounds, _scroll_offset, left_edge);

			// Draw the cursor if the previously drawn option was or is selected
			if ((static_cast<int32>(index) == _selection || static_cast<int32>(index) == _first_selection) &&
				_cursor_state != VIDEO_CURSOR_STATE_HIDDEN && (_cursor_state != VIDEO_CURSOR_STATE_BLINKING || _blink == true))
			{
				// If this option was the first selection, draw it darkened so that it has a different appearance
				bool darken = (static_cast<int32>(index) == _first_selection) ? true : false;
				_DrawCursor(bounds, _scroll_offset, left_edge, darken);
			}

			bounds.x_left += xoff;
			bounds.x_center += xoff;
			bounds.x_right += xoff;
		} // for (int32 col = 0; col < _number_columns; ++col)

		bounds.y_top += yoff;
		bounds.y_center += yoff;
		bounds.y_bottom += yoff;
	} // for (int32 row = row_min; row < row_max; row++)

	// ---------- (4) Draw scroll arrows where appropriate
	_DetermineScrollArrows();
	std::vector<StillImage>* arrows = GUIManager->GetScrollArrows();


	float w, h;
	this->GetDimensions(w, h);

	if (_draw_vertical_arrows) {
		VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
		VideoManager->Move(right, top);
		if (_grey_up_arrow)
			arrows->at(4).Draw();
		else
			arrows->at(0).Draw();

		VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
		VideoManager->Move(right, bottom);
		if (_grey_down_arrow)
			arrows->at(5).Draw();
		else
			arrows->at(1).Draw();
	}

	if (_draw_horizontal_arrows) {
		VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
		VideoManager->Move(left, bottom);
		if (_grey_left_arrow)
			arrows->at(7).Draw();
		else
			arrows->at(3).Draw();

		VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
		VideoManager->Move(right, bottom);
		if (_grey_right_arrow)
			arrows->at(6).Draw();
		else
			arrows->at(2).Draw();
	}

	VideoManager->SetDrawFlags(_xalign, _yalign, VIDEO_BLEND, 0);

	if (GUIManager->DEBUG_DrawOutlines() == true)
		GUIControl::_DEBUG_DrawOutline();

	VideoManager->PopState();
} // void OptionBox::Draw()



void OptionBox::SetDimensions(float width, float height, uint8 num_cols, uint8 num_rows, uint8 cell_cols, uint8 cell_rows) {
	if (num_rows == 0 || num_cols == 0) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "num_rows/num_cols argument was zero" << endl;
		return;
	}

	if (cell_rows == 0 || cell_cols == 0) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "cell_rows/cell_cols argument was zero" << endl;
		return;
	}

	if (num_rows < cell_rows || num_cols < cell_cols) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "num_rows/num_cols was less than cell_rows/cell_cols" << endl;
		return;
	}

	_width = width;
	_height = height;
	_number_columns = num_cols;
	_number_rows = num_rows;
	_number_cell_columns = cell_cols;
	_number_cell_rows = cell_rows;
	_cell_width = _width / cell_cols;
	_cell_height = _height / cell_rows;
}



void OptionBox::SetOptions(const vector<ustring>& option_text) {
	ClearOptions();
	for (vector<ustring>::const_iterator i = option_text.begin(); i != option_text.end(); i++) {
		const ustring& str = *i;
		Option option;

		if (_ConstructOption(str, option) == false) {
			ClearOptions();
			IF_PRINT_WARNING(VIDEO_DEBUG) << "an option contained an invalid formatted string: " << MakeStandardString(*i) << endl;
			return;
		}
		_options.push_back(option);
	}
}



void OptionBox::ClearOptions() {
	_options.clear();
}



void OptionBox::AddOption() {
	Option option;
	if (_ConstructOption(ustring(), option) == false) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "failed to construct option using an empty string"  << endl;
		return;
	}

	_options.push_back(option);
}



void OptionBox::AddOption(const hoa_utils::ustring& text) {
	Option option;
	if (_ConstructOption(text, option) == false) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "argument contained an invalid formatted string: " << MakeStandardString(text) << endl;
		return;
	}

	_options.push_back(option);
}



void OptionBox::AddOptionElementText(uint32 option_index, const ustring& text) {
	if (option_index >= GetNumberOptions()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "out-of-range option_index argument: " << option_index << endl;
		return;
	}

	Option& this_option = _options[option_index];
	OptionElement new_element;

	new_element.type = VIDEO_OPTION_ELEMENT_TEXT;
	new_element.value = static_cast<int32>(this_option.text.size());
	this_option.text.push_back(text);
	this_option.elements.push_back(new_element);
}



void OptionBox::AddOptionElementImage(uint32 option_index, string& image_filename) {
	if (option_index >= GetNumberOptions()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "out-of-range option_index argument: " << option_index << endl;
		return;
	}

	Option& this_option = _options[option_index];
	OptionElement new_element;

	new_element.type = VIDEO_OPTION_ELEMENT_IMAGE;
	new_element.value = 0;

	this_option.image = new StillImage();
	if (this_option.image->Load(image_filename) == false) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "failed to add image element because image file load failed" << image_filename << endl;
		delete this_option.image;
		this_option.image = NULL;
		return;
	}

	this_option.elements.push_back(new_element);
}



void OptionBox::AddOptionElementImage(uint32 option_index, const StillImage* image) {
	if (option_index >= GetNumberOptions()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "out-of-range option_index argument: " << option_index << endl;
		return;
	}
	if (image == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "image argument was NULL" << endl;
		return;
	}

	Option& this_option = _options[option_index];
	OptionElement new_element;

	new_element.type = VIDEO_OPTION_ELEMENT_IMAGE;
	new_element.value = 0;

	this_option.image = new StillImage(*image);
	this_option.elements.push_back(new_element);
}



void OptionBox::AddOptionElementAlignment(uint32 option_index, OptionElementType position_type) {
	if (option_index >= GetNumberOptions()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "out-of-range option_index argument: " << option_index << endl;
		return;
	}
	if ((position_type != VIDEO_OPTION_ELEMENT_LEFT_ALIGN) &&
		(position_type != VIDEO_OPTION_ELEMENT_CENTER_ALIGN) &&
		(position_type != VIDEO_OPTION_ELEMENT_RIGHT_ALIGN))
	{
		IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid position_type argument" << position_type <<  endl;
	}

	Option& this_option = _options[option_index];
	OptionElement new_element;

	new_element.type = position_type;
	new_element.value = 0;
	this_option.elements.push_back(new_element);
}



void OptionBox::AddOptionElementPosition(uint32 option_index, uint32 position_length) {
	if (option_index >= GetNumberOptions()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "out-of-range option_index argument: " << option_index << endl;
		return;
	}

	Option& this_option = _options[option_index];
	OptionElement new_element;

	new_element.type = VIDEO_OPTION_ELEMENT_POSITION;
	new_element.value = position_length;
	this_option.elements.push_back(new_element);
}



bool OptionBox::SetOptionText(uint32 index, const hoa_utils::ustring &text) {
	if (index >= GetNumberOptions()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "argument was invalid (out of bounds): " << index << endl;
		return false;
	}

	_ConstructOption(text, _options[index]);
	return true;
}



void OptionBox::SetSelection(uint32 index) {
	if (index >= GetNumberOptions()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "argument was invalid (out of bounds): " << index << endl;
		return;
	}

	_selection = index;
	int32 select_row = _selection / _number_columns;

	// If the new selection isn't currently being displayed, instantly scroll to it
	if (select_row < _scroll_offset || select_row > (_scroll_offset + _number_rows - 1)) {
		_scroll_offset = select_row - _number_rows + 1;

		int32 total_num_rows = (GetNumberOptions() + _number_columns - 1) / _number_columns;

		if (_scroll_offset + _number_rows >= total_num_rows) {
			_scroll_offset = total_num_rows - _number_rows;
		}
	}
}



void OptionBox::EnableOption(uint32 index, bool enable) {
	if (index >= GetNumberOptions()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "argument index was invalid: " << index << endl;
		return;
	}

	_options[index].disabled = !enable;
}



bool OptionBox::IsOptionEnabled(uint32 index) {
	if (index >= GetNumberOptions()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "argument index was invalid: " << index << endl;
		return false;
	}

	return (!_options[index].disabled);
}



bool OptionBox::IsEnabled(uint32 index) const {
	if (index >= GetNumberOptions()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "argument index was invalid: " << index << endl;
		return false;
	}

	return !_options[index].disabled;
}



StillImage* OptionBox::GetEmbeddedImage(uint32 index) const {
	if (index >= GetNumberOptions()) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "argument index was invalid: " << index << endl;
		return false;
	}

	return _options[index].image;
}



bool OptionBox::IsInitialized(string& error_messages) {
	ostringstream s;
	error_messages.clear();

	if (_width <= 0.0f)
		s << "* Invalid width (" << _width << ")" << endl;

	if (_height <= 0.0f)
		s << "* Invalid height (" << _height << ")" << endl;

	if (_number_rows <= 0)
		s << "* Invalid number of rows (" << _number_rows << ")" << endl;

	if (_number_columns <= 0)
		s << "* Invalid number of columns (" << _number_columns << ")" << endl;

	if (_cell_width <= 0.0f && _number_columns > 1)
		s << "* Invalid horizontal spacing (" << _cell_width << ")" << endl;

	if (_cell_height <= 0.0f && _number_rows > 1)
		s << "* Invalid vertical spacing (" << _cell_height << ")" << endl;

	if (_option_xalign < VIDEO_X_LEFT || _option_xalign > VIDEO_X_RIGHT)
		s << "* Invalid x align (" << _option_xalign << ")" << endl;

	if (_option_yalign < VIDEO_Y_TOP || _option_yalign > VIDEO_Y_BOTTOM)
		s << "* Invalid y align (" << _option_yalign << ")" << endl;

	if (_text_style.font.empty())
		s << "* Invalid font (none has been set)" << endl;

	if (_selection_mode <= VIDEO_SELECT_INVALID || _selection_mode >= VIDEO_SELECT_TOTAL)
		s << "* Invalid selection mode (" << _selection_mode << ")" << endl;

	error_messages = s.str();

	if (error_messages.empty())
		_initialized = true;
	else
		_initialized = false;

	return _initialized;
}

// -----------------------------------------------------------------------------
// Input Handling Methods
// -----------------------------------------------------------------------------

void OptionBox::InputConfirm() {
	// Abort if an invalid option is selected
	if (_selection < 0 || _selection >= static_cast<int32>(GetNumberOptions())) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "an invalid (out of bounds) option was selected: " << _selection << endl;
		return;
	}

	// Ignore input while scrolling, or if an event has already been logged
	if (_scrolling || _event || _options[_selection].disabled)
		return;

	// Case #1: switch the position of two different options
	if (_enable_switching && _first_selection >= 0 && _selection != _first_selection) {
		Option temp = _options[_selection];
		_options[_selection] = _options[_first_selection];
		_options[_first_selection] = temp;
		_first_selection = -1; // Done so that we know we're not in switching mode any more
		_event = VIDEO_OPTION_SWITCH;
	}

	// Case #2: partial confirm (confirming the first element in a double confirm)
	else if (_selection_mode == VIDEO_SELECT_DOUBLE && _first_selection == -1) {
		_first_selection = _selection;
	}

	// Case #3: standard confirm
	else {
		if (_options[_selection].disabled) {
			// TODO: do something to tell player they confirmed on a disabled option?
			return;
		}
		_event = VIDEO_OPTION_CONFIRM;
		// Get out of switch mode
		_first_selection = -1;
	}
}



void OptionBox::InputCancel() {
	// Ignore input while scrolling, or if an event has already been logged
	if (_scrolling || _event)
		return;

	// If we're in switching mode unselect the first selection
	if (_first_selection >= 0)
		_first_selection = -1;
	else
		_event = VIDEO_OPTION_CANCEL;
}



void OptionBox::InputUp() {
	// Ignore input while scrolling, or if an event has already been logged
	if (_scrolling || _event)
		return;

	if (_ChangeSelection(-1, false) == false)
		return;

	int32 row = _selection / _number_columns;

    if (_skip_disabled) {
        while (_options[_selection].disabled == true) {
            if (row <= 0 && _vertical_wrap_mode == VIDEO_WRAP_MODE_NONE)
                InputDown();
            else
                InputUp();
        }
    }

	_event = VIDEO_OPTION_BOUNDS_UP;
}



void OptionBox::InputDown() {
	// Ignore input while scrolling, or if an event has already been logged
	if (_scrolling || _event)
		return;

	if (_ChangeSelection(1, false) == false)
		return;

	int32 row = _selection / _number_columns;

    if (_skip_disabled) {
        while (_options[_selection].disabled == true) {
            if (row >= _number_rows-1 && _vertical_wrap_mode == VIDEO_WRAP_MODE_NONE)
                InputUp();
            else
                InputDown();
        }
    }

	_event = VIDEO_OPTION_BOUNDS_DOWN;
}



void OptionBox::InputLeft() {
	// Ignore input while scrolling, or if an event has already been logged
	if (_scrolling || _event)
		return;

	if (_ChangeSelection(-1, true) == false)
		return;

	int32 col = _selection % _number_columns;

    if (_skip_disabled) {
        while (_options[_selection].disabled == true) {
            if (col <= 0 && _horizontal_wrap_mode == VIDEO_WRAP_MODE_NONE)
                InputRight();
            else
                InputLeft();
        }
    }

	_event = VIDEO_OPTION_BOUNDS_LEFT;
}



void OptionBox::InputRight() {
	// Ignore input while scrolling, or if an event has already been logged
	if (_scrolling || _event)
		return;

	if (_ChangeSelection(1, true) == false)
		return;

	int32 col = _selection % _number_columns;

    if (_skip_disabled) {
        while (_options[_selection].disabled == true) {
            if (col >= _number_columns-1 && _horizontal_wrap_mode == VIDEO_WRAP_MODE_NONE)
                InputLeft();
            else
                InputRight();
        }
    }

	_event = VIDEO_OPTION_BOUNDS_RIGHT;
}

// -----------------------------------------------------------------------------
// Member Access Functions
// -----------------------------------------------------------------------------

void OptionBox::SetTextStyle(const TextStyle& style) {
	if (TextManager->GetFontProperties(style.font) == NULL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "text style references an invalid font name: " << style.font << endl;
		return;
	}

	_text_style = style;
	_initialized = IsInitialized(_initialization_errors);
}



void OptionBox::SetCursorState(CursorState state) {
	if (state <= VIDEO_CURSOR_STATE_INVALID || state >= VIDEO_CURSOR_STATE_TOTAL) {
		IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid function argument : " << state << endl;
		return;
	}

	_cursor_state = state;
}



void OptionBox::SetHorizontalArrowsPosition(HORIZONTAL_ARROWS_POSITION position) {
	_horizontal_arrows_position = position;
}



void OptionBox::SetVerticalArrowsPosition(VERTICAL_ARROWS_POSITION position) {
	_vertical_arrows_position = position;
}

// -----------------------------------------------------------------------------
// Private Methods
// -----------------------------------------------------------------------------

bool OptionBox::_ConstructOption(const ustring& format_string, Option& op) {
	op.Clear();

	// This is a valid case. It simply means we add an option with no tags, text, or other data.
	if (format_string.empty()) {
		return true;
	}

	// Copy the format_string into a temporary string that we can manipulate
	ustring tmp = format_string;

	while (tmp.empty() == false) {
		OptionElement new_element;

		if (tmp[0] == OPEN_TAG) { // Process a new tag
			size_t length = tmp.length();

			if (length < 3) {
				// All formatting tags are at least 3 characters long because you need the opening (<)
				// and close (>) plus stuff in the middle. So anything less than 3 characters is a problem.

				IF_PRINT_WARNING(VIDEO_DEBUG) << "failed because a tag opening was detected with an inadequate "
					<< "number of remaining characters to construct a full tag: " << MakeStandardString(format_string) << endl;
				return false;
			}

			size_t end_position = tmp.find(END_TAG);

			if (end_position == ustring::npos) { // Did not find the end of the tag
				IF_PRINT_WARNING(VIDEO_DEBUG) << "failed because a matching end tag could not be found for an open tag: "
					<< MakeStandardString(format_string) << endl;
				return false;
			}

			if (tmp[2] == END_TAG) { // First check if the tag is a 1-character alignment tag
				if (tmp[1] == CENTER_TAG1 || tmp[1] == CENTER_TAG2) {
					new_element.type = VIDEO_OPTION_ELEMENT_CENTER_ALIGN;
				}
				else if (tmp[1] == RIGHT_TAG1 || tmp[1] == RIGHT_TAG2) {
					new_element.type = VIDEO_OPTION_ELEMENT_RIGHT_ALIGN;
				}
				else if (tmp[1] == LEFT_TAG1 || tmp[1] == LEFT_TAG2) {
					new_element.type = VIDEO_OPTION_ELEMENT_LEFT_ALIGN;
				}
			}
			else { // The tag contains more than 1-character
				// Extract the tag string
				string tag_text = MakeStandardString(tmp.substr(1, end_position - 1));

				if (IsStringNumeric(tag_text)) { // Then this must be a positioning tag
					new_element.type  = VIDEO_OPTION_ELEMENT_POSITION;
					new_element.value = atoi(tag_text.c_str());
				}
				else { // Then this must be an image tag
					if (op.image != NULL) {
						IF_PRINT_WARNING(VIDEO_DEBUG) << "failed because two image tags were embedded within a single option"
							<< MakeStandardString(format_string) << endl;
						return false;
					}
					op.image = new StillImage();
					if (op.image->Load(tag_text) == false) {
						IF_PRINT_WARNING(VIDEO_DEBUG) << "failed because of an invalid image tag: "
							<< MakeStandardString(format_string) << endl;
						return false;
					}
					new_element.type  = VIDEO_OPTION_ELEMENT_IMAGE;
					new_element.value = 0;
				}
			}

			// Finished processing the tag so update the tmp string
			if (end_position == length - 1) { // End of string
				tmp.clear();
			}
			else {
				tmp = tmp.substr(end_position + 1, length - end_position - 1);
			}
		} // if (tmp[0] == OPEN_TAG)

		else { // If this isn't a tag, then it is raw text that should be added to the option
			new_element.type = VIDEO_OPTION_ELEMENT_TEXT;
			new_element.value = static_cast<int32>(op.text.size());

			// find the distance until the next tag
			size_t tag_begin = tmp.find(OPEN_TAG);

			if (tag_begin == ustring::npos) { // There are no more tags remaining, so extract the entire string
				op.text.push_back(tmp);
				tmp.clear();
			}
			else { // Another tag remains to be processed, so extract the text substring
				op.text.push_back(tmp.substr(0, tag_begin));
				tmp = tmp.substr(tag_begin, tmp.length() - tag_begin);
			}
		}

		op.elements.push_back(new_element);
	} // while (tmp.empty() == false)

	return true;
} // bool _ConstructOption(const ustring& format_string, Option& option)



bool OptionBox::_ChangeSelection(int32 offset, bool horizontal) {
	// Do nothing if the movement is horizontal and there is only one column with no horizontal wrap shifting
	if ((horizontal == true) && (_number_cell_columns == 1) &&( _horizontal_wrap_mode != VIDEO_WRAP_MODE_SHIFTED))
		return false;

	// Do nothing if the movement is vertical and there is only one row with no vertical wrap shifting
	if ((horizontal == false) && (_number_cell_rows == 1) && (_vertical_wrap_mode != VIDEO_WRAP_MODE_SHIFTED))
		return false;

	// Get the row, column coordinates for the current selection
	int32 row = _selection / _number_columns;
	int32 col = _selection % _number_columns;
	bool bounds_exceeded = false;

	// Determine if the movement selection will exceed a column or row bondary
	int new_row = (row + offset) * _number_columns;
	if ((horizontal == true && ((col + offset < 0) || (col + offset >= _number_columns) ||
			(col + offset >= static_cast<int32>(GetNumberOptions())))) ||
		(horizontal == false && ((new_row < 0) || (new_row >= _number_rows) ||
			(new_row >= static_cast<int32>(GetNumberOptions())))))
	{
		bounds_exceeded = true;
	}

	// Case #1: movement selection is within bounds
	if (bounds_exceeded == false) {
		if (horizontal)
			_selection += offset;
		else
			_selection += (offset * _number_columns);
	}

	// Case #2: movement exceeds bounds, no wrapping enabled
	else if ((horizontal == true && _horizontal_wrap_mode == VIDEO_WRAP_MODE_NONE) ||
		(horizontal == false && _vertical_wrap_mode == VIDEO_WRAP_MODE_NONE))
	{
		return false;
	}

	// Case #3: horizontal movement with wrapping enabled
	else if (horizontal == true) {
		if (col + offset <= 0) { // The left boundary was exceeded
			if (_horizontal_wrap_mode == VIDEO_WRAP_MODE_STRAIGHT) {
				offset = _number_columns - 1;
			}
			// Make sure vertical wrapping is allowed if horizontal wrap mode is shifting
			else if (_horizontal_wrap_mode == VIDEO_WRAP_MODE_SHIFTED && _vertical_wrap_mode != VIDEO_WRAP_MODE_NONE) {
				offset += GetNumberOptions();
			}
			else {
				return false;
			}
		}
		else { // The right boundary was exceeded
			if (_horizontal_wrap_mode == VIDEO_WRAP_MODE_STRAIGHT)
				offset -= _number_columns;
			// Make sure vertical wrapping is allowed if horizontal wrap mode is shifting
			else if (_horizontal_wrap_mode == VIDEO_WRAP_MODE_SHIFTED && _vertical_wrap_mode != VIDEO_WRAP_MODE_NONE) {
				offset = 0;
				_selection++;
			}
			else
				return false;
		}
		_selection = (_selection + offset) % GetNumberOptions();
	}

	// Case #4: vertical movement with wrapping enabled
	else {
		if (row + offset <= 0) { // The top boundary was exceeded
			if (_vertical_wrap_mode == VIDEO_WRAP_MODE_STRAIGHT)
				offset += GetNumberOptions();
			// Make sure horizontal wrapping is allowed if vertical wrap mode is shifting
			else if (_vertical_wrap_mode == VIDEO_WRAP_MODE_SHIFTED && _horizontal_wrap_mode != VIDEO_WRAP_MODE_NONE)
				offset += (_number_columns - 1);
			else
				return false;
		}
		else  { // The bottom boundary was exceeded
			if (_vertical_wrap_mode == VIDEO_WRAP_MODE_STRAIGHT) {
				if (row + offset >= _number_rows)
					offset -= GetNumberOptions();
			}
			// Make sure horizontal wrapping is allowed if vertical wrap mode is shifting
			else if (_vertical_wrap_mode == VIDEO_WRAP_MODE_SHIFTED && _horizontal_wrap_mode != VIDEO_WRAP_MODE_NONE)
				offset -= (_number_columns - 1);
			else
				return false;
		}
		_selection = (_selection + (offset * _number_columns)) % GetNumberOptions();
	}

	// Determine if the new selection is not displayed in any cells. If so, scroll it into view.
	int32 selection_row = _selection / _number_columns;
	int32 selection_col = _selection % _number_columns;

	if ((static_cast<uint32>(selection_row) < _draw_top_row)) {
		_scrolling = true;
		_scroll_time = 0;
		_draw_top_row = selection_row;

		if (selection_row < _scroll_offset)
			_scroll_direction = -1 * (_scroll_offset - row); // scroll up
		else
			_scroll_direction = 1 * (row - _number_rows - _scroll_offset + 1); // scroll down

		_scroll_offset += _scroll_direction;
	}

	else if ((static_cast<uint32>(selection_row) >= (_draw_top_row + _number_cell_rows)) ) {
		_scrolling = true;
		_scroll_time = 0;
		_draw_top_row = selection_row - _number_cell_rows + 1;

		if (selection_row < _scroll_offset)
			_scroll_direction = -1 * (_scroll_offset - row); // scroll up
		else
			_scroll_direction = 1 * (row - _number_rows - _scroll_offset + 1); // scroll down

		_scroll_offset += _scroll_direction;
	}

	else if ((static_cast<uint32>(selection_col) < _draw_left_column)) {
		_scrolling = true;
		_scroll_time = 0;
		_draw_left_column = selection_col;

		if (selection_row < _scroll_offset)
			_scroll_direction = -1 * (_scroll_offset - row); // scroll up
		else
			_scroll_direction = 1 * (row - _number_rows - _scroll_offset + 1); // scroll down

		_scroll_offset += _scroll_direction;
	}

	else if ((static_cast<uint32>(selection_col) >= (_draw_left_column + _number_cell_columns))) {
		_scrolling = true;
		_scroll_time = 0;
		_draw_left_column = selection_col - _number_cell_columns + 1;

		if (selection_row < _scroll_offset)
			_scroll_direction = -1 * (_scroll_offset - row); // scroll up
		else
			_scroll_direction = 1 * (row - _number_rows - _scroll_offset + 1); // scroll down

		_scroll_offset += _scroll_direction;
	}

	// If the new selection isn't currently being displayed, scroll it into view
// 	row = _selection / _number_columns;
// 	if (row < _scroll_offset || row >= _scroll_offset + _number_rows) {
// 		_scrolling = true;
// 		_scroll_time = 0;
//
// 		if (row < _scroll_offset)
// 			_scroll_direction = -1 * (_scroll_offset - row); // scroll up
// 		else
// 			_scroll_direction = 1 * (row - _number_rows - _scroll_offset + 1); // scroll down
//
// 		_scroll_offset += _scroll_direction;
// 	}

	_event = VIDEO_OPTION_SELECTION_CHANGE;
	return true;
} // bool OptionBox::_ChangeSelection(int32 offset, bool horizontal)



void OptionBox::_SetupAlignment(int32 xalign, int32 yalign, const OptionCellBounds& bounds, float& x, float& y) {
	VideoManager->SetDrawFlags(xalign, yalign, 0);

	switch (xalign) {
		case VIDEO_X_LEFT:
			x = bounds.x_left;
			break;
		case VIDEO_X_CENTER:
			x = bounds.x_center;
			break;
		default:
			x = bounds.x_right;
			break;
	}

	switch (yalign) {
		case VIDEO_Y_TOP:
			y = bounds.y_top;
			break;
		case VIDEO_Y_CENTER:
			y = bounds.y_center;
			break;
		default:
			y = bounds.y_bottom;
			break;
	}

	VideoManager->Move(x, y);
} // void OptionBox::_SetupAlignment(int32 xalign, int32 yalign, const OptionCellBounds& bounds, float& x, float& y)



void OptionBox::_DetermineScrollArrows() {
	_grey_up_arrow = false;
	_grey_down_arrow = false;
	_grey_left_arrow = false;
	_grey_right_arrow = false;

	_draw_horizontal_arrows = (_number_cell_columns < _number_columns) && (static_cast<int32>(GetNumberOptions()) > _number_cell_columns);
	_draw_vertical_arrows = (_number_cell_rows < _number_rows) && (static_cast<int32>(GetNumberOptions()) > _number_columns * _number_cell_rows);

	if (_horizontal_wrap_mode == VIDEO_WRAP_MODE_NONE) {
		if (_draw_left_column == 0)
			_grey_left_arrow = true;
		if (static_cast<int32>(_draw_left_column + _number_cell_columns) >= _number_columns)
			_grey_right_arrow = true;
		if (_selection >= static_cast<int32>(_options.size() - 1))
			_grey_right_arrow = true;
	}

	if (_vertical_wrap_mode == VIDEO_WRAP_MODE_NONE) {
		if (_draw_top_row == 0)
			_grey_up_arrow = true;
		if (static_cast<int32>(_draw_top_row + _number_cell_rows) > _number_rows)
			_grey_down_arrow = true;
		if (_selection + _number_cell_columns >= static_cast<int32>(_options.size()))
			_grey_down_arrow = true;
	}
}



void OptionBox::_DrawOption(const Option& op, const OptionCellBounds &bounds, float scroll_offset, float& left_edge) {
	// TODO: this function doesn't make use of the scroll_offset parameter currently, but I'm pretty sure it is
	// needed somewhere to get scrolling full working. Once the scrolling feature has been enabled and verified
	// for correctness if this paramater is still unused, remove it.

	float x, y;
	int32 xalign = _option_xalign;
	int32 yalign = _option_yalign;
	CoordSys &cs = VideoManager->_current_context.coordinate_system;

	_SetupAlignment(xalign, yalign, bounds, x, y);

	// Iterate through all option elements in the current option
	for (int32 element = 0; element < static_cast<int32>(op.elements.size()); element++) {
		switch (op.elements[element].type) {
			case VIDEO_OPTION_ELEMENT_LEFT_ALIGN:
			{
				xalign = VIDEO_X_LEFT;
				_SetupAlignment(xalign, _option_yalign, bounds, x, y);
				break;
			}
			case VIDEO_OPTION_ELEMENT_CENTER_ALIGN:
			{
				xalign = VIDEO_X_CENTER;
				_SetupAlignment(xalign, _option_yalign, bounds, x, y);
				break;
			}
			case VIDEO_OPTION_ELEMENT_RIGHT_ALIGN:
			{
				xalign = VIDEO_X_RIGHT;
				_SetupAlignment(xalign, _option_yalign, bounds, x, y);
				break;
			}
			case VIDEO_OPTION_ELEMENT_IMAGE:
			{
				if (op.disabled)
					op.image->Draw(Color::gray);
				else
					op.image->Draw(Color::white);

				float width = op.image->GetWidth();
				float edge = x - bounds.x_left; // edge value for VIDEO_X_LEFT
				if (xalign == VIDEO_X_CENTER)
					edge -= width * 0.5f * cs.GetHorizontalDirection();
				else if (xalign == VIDEO_X_RIGHT)
					edge -= width * cs.GetHorizontalDirection();
				if (edge < left_edge)
					left_edge = edge;
				break;
			}
			case VIDEO_OPTION_ELEMENT_POSITION:
			{
				x = bounds.x_left + op.elements[element].value * cs.GetHorizontalDirection();
				VideoManager->Move(x, y);
				break;
			}
			case VIDEO_OPTION_ELEMENT_TEXT:
			{
				int32 text_index = op.elements[element].value;

				if (text_index >= 0 && text_index < static_cast<int32>(op.text.size())) {
					const ustring& text = op.text[text_index];
					float width = static_cast<float>(VideoManager->Text()->CalculateTextWidth(_text_style.font, text));
					float edge = x - bounds.x_left; // edge value for VIDEO_X_LEFT

					if (xalign == VIDEO_X_CENTER)
						edge -= width * 0.5f * cs.GetHorizontalDirection();
					else if (xalign == VIDEO_X_RIGHT)
						edge -= width * cs.GetHorizontalDirection();

					if (edge < left_edge)
						left_edge = edge;
					if (op.disabled) {
						Color saved = _text_style.color;
						_text_style.color = Color::gray;
						TextManager->Draw(text, _text_style);
						_text_style.color = saved;
					}
					else {
						TextManager->Draw(text, _text_style);
					}
				}

				break;
			}
			case VIDEO_OPTION_ELEMENT_INVALID:
			case VIDEO_OPTION_ELEMENT_TOTAL:
			default:
			{
				IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid option element type was present" << endl;
				break;
			}
		} // switch (op.elements[element].type)
	} // for (int32 element = 0; element < static_cast<int32>(op.elements.size()); element++)
} // void OptionBox::_DrawOption(const Option& op, const OptionCellBounds &bounds, float scroll_offset, float& left_edge)



void OptionBox::_DrawCursor(const OptionCellBounds &bounds, float scroll_offset, float left_edge, bool darken) {
	// [phuedx] In this case the scroll offset is not used, however it should be.
	// The Draw() function (and all helper functions) should be able able to
	// render without knowledge of the private member variable _scroll_offset.

	float x, y;

	// Should never scissor the cursor
	VideoManager->DisableScissoring();

	float cursor_offset = 0.0f;

	// [phuedx] The scroll_offset has already been calculated and projected on to the current coordinate system
	if (_scrolling) {
		cursor_offset = -scroll_offset;
	}

	_SetupAlignment(VIDEO_X_LEFT, _option_yalign, bounds, x, y);
	VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
	VideoManager->MoveRelative(left_edge + _cursor_xoffset, _cursor_yoffset + cursor_offset);

	StillImage *default_cursor = VideoManager->GetDefaultCursor();

	if (default_cursor == NULL)
		IF_PRINT_WARNING(VIDEO_DEBUG) << "invalid (NULL) cursor image" << endl;

	if (darken == false)
		default_cursor->Draw();
	else
		default_cursor->Draw(Color(1.0f, 1.0f, 1.0f, 0.5f));
} // void OptionBox::_DrawCursor(const OptionCellBounds &bounds, float scroll_offset, float left_edge, bool darken)



void OptionBox::_DEBUG_DrawOutline() {
	float left = 0.0f;
	float right = _width;
	float bottom = 0.0f;
	float top = _height;

	// Draw the outline of the option box area
	VideoManager->Move(0.0f, 0.0f);
	CalculateAlignedRect(left, right, bottom, top);
	VideoManager->DrawRectangleOutline(left, right, bottom, top, 3, alpha_black);
	VideoManager->DrawRectangleOutline(left, right, bottom, top, 1, alpha_white);

	// Draw outline for inner cell rows
	float cell_row = top;
	for (int32 i = 1; i < _number_cell_rows; i++) {
		cell_row += _cell_height;
		VideoManager->DrawLine(left, cell_row, right, cell_row, 3, alpha_black);
		VideoManager->DrawLine(left, cell_row, right, cell_row, 1, alpha_white);
	}

	// Draw outline for inner cell columns
	float cell_col = left;
	for (int32 i = 1; i < _number_cell_columns; i++) {
		cell_col += _cell_width;
		VideoManager->DrawLine(cell_col, bottom, cell_col, top, 3, alpha_black);
		VideoManager->DrawLine(cell_col, bottom, cell_col, top, 1, alpha_white);
	}
}

} // namespace hoa_gui
