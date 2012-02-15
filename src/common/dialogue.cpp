///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    dialogue.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for common dialogue code.
*** ***************************************************************************/

// Allacrost utilities
#include "utils.h"

// Allacrost engines
#include "video.h"

// Local common headers
#include "common.h"
#include "dialogue.h"
#include "gui.h"

using namespace std;

using namespace hoa_system;
using namespace hoa_utils;
using namespace hoa_video;
using namespace hoa_gui;

namespace hoa_common {

///////////////////////////////////////////////////////////////////////////////
// CommonDialogue Class Functions
///////////////////////////////////////////////////////////////////////////////

CommonDialogue::CommonDialogue(uint32 id) :
	_dialogue_id(id),
	_times_seen(0),
	_max_views(COMMON_DIALOGUE_INFINITE_VIEWS),
	_line_count(0)
{}



CommonDialogue::~CommonDialogue() {
	for (uint32 i = 0; i < _options.size(); i++) {
		if (_options[i] != NULL) {
			delete _options[i];
			_options[i] = NULL;
		}
	}
}



void CommonDialogue::AddLine(string text) {
	AddLine(text, COMMON_DIALOGUE_NEXT_LINE);
}



void CommonDialogue::AddLine(string text, int32 next_line) {
	_line_count++;
	_text.push_back(MakeUnicodeString(text));
	_next_lines.push_back(next_line);
	_display_times.push_back(COMMON_DIALOGUE_NO_TIMER);
	_options.push_back(NULL);
}



void CommonDialogue::AddLineTimed(string text, uint32 display_time) {
	AddLineTimed(text, COMMON_DIALOGUE_NEXT_LINE, display_time);
}



void CommonDialogue::AddLineTimed(string text, int32 next_line, uint32 display_time) {
	_line_count++;
	_text.push_back(MakeUnicodeString(text));
	_next_lines.push_back(next_line);
	_display_times.push_back(display_time);
	_options.push_back(NULL);
}



void CommonDialogue::AddOption(string text) {
	AddOption(text, COMMON_DIALOGUE_NEXT_LINE);
}



void CommonDialogue::AddOption(string text, int32 next_line) {
	if (_line_count == 0) {
		IF_PRINT_WARNING(COMMON_DEBUG) << "Attempted to add an option to a dialogue with no lines" << endl;
		return;
	}

	uint32 current_line = _line_count - 1;

	// If the line the options will be added to currently has no options, create a new instance of the CommonDialogueOptions class to store the options in.
	if (_options[current_line] == NULL) {
		_options[current_line] = new CommonDialogueOptions();
	}
	_options[current_line]->AddOption(text, next_line);
}



bool CommonDialogue::Validate() {
	// Valid dialogues need to have at least one line
	if (_line_count == 0) {
		IF_PRINT_WARNING(COMMON_DEBUG) << "Validation failed for dialogue #" << _dialogue_id << ": no lines" << endl;
		return false;
	}

	// Check that the dialogue has not been seen more times than it is allowed to be viewed
	if ((_max_views != COMMON_DIALOGUE_INFINITE_VIEWS) && (_times_seen > static_cast<uint32>(_max_views))) {
		IF_PRINT_WARNING(COMMON_DEBUG) << "Validation failed for dialogue #" << _dialogue_id
			<< ": discrepency in max/seen view counts" << endl;
		return false;
	}

	// Check that all next lines with positive values point to valid indeces
	for (uint32 i = 0; i < _line_count; i++) {
		if ((_next_lines[i] >= 0) && (static_cast<uint32>(_next_lines[i]) >= _line_count)) {
			IF_PRINT_WARNING(COMMON_DEBUG) << "Validation failed for dialogue #" << _dialogue_id
				<< ": next line referred to an invalid line index: " << _next_lines[i] << endl;
			return false;
		}

		// If this line has options, we have to examine the next line argument for each option as well
		if (_options[i] != 0) {
			if (_options[i]->GetNumberOptions() == 0) {
				IF_PRINT_WARNING(COMMON_DEBUG) << "Validation failed for dialogue #" << _dialogue_id
					<< ": line had options declared but no options defined" << endl;
				return false;
			}

			for (uint32 j = 0; j < _options[i]->GetNumberOptions(); j++) {
				int32 option_next_line = _options[i]->GetOptionNextLine(j);
				if ((option_next_line >= 0) && (static_cast<uint32>(option_next_line) >= _line_count)) {
					IF_PRINT_WARNING(COMMON_DEBUG) << "Validation failed for dialogue #" << _dialogue_id
						<< ": option's next line referred to an invalid line index: " << option_next_line << endl;
					return false;
				}
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// CommonDialogueOptions Functions
///////////////////////////////////////////////////////////////////////////////

void CommonDialogueOptions::AddOption(string text) {
	AddOption(text, COMMON_DIALOGUE_NEXT_LINE);
}



void CommonDialogueOptions::AddOption(string text, int32 next_line) {
	_text.push_back(MakeUnicodeString(text));
	_next_lines.push_back(next_line);
}

///////////////////////////////////////////////////////////////////////////////
// CommonDialogueWindow class methods
///////////////////////////////////////////////////////////////////////////////

CommonDialogueWindow::CommonDialogueWindow() :
	_pos_x(512.0f),
	_pos_y(512.0f),
	_indicator_symbol(COMMON_DIALOGUE_NO_INDICATOR),
	_blink_time(0),
	_blink_state(true),
	_portrait_image(NULL)
{
	if (_parchment_image.Load("img/menus/black_sleet_parch.png") == false)
		PRINT_ERROR << "failed to load dialogue image: " << _parchment_image.GetFilename() << endl;

	if (_nameplate_image.Load("img/menus/dialogue_nameplate.png") == false)
		PRINT_ERROR << "failed to load dialogue image: " << _nameplate_image.GetFilename() << endl;

	if (_next_line_image.Load("img/menus/dialogue_cont_arrow.png") == false)
		PRINT_ERROR << "failed to load dialogue image: " << _next_line_image.GetFilename() << endl;

	if (_last_line_image.Load("img/menus/dialogue_last_ind.png") == false)
		PRINT_ERROR << "failed to load dialogue image: " << _last_line_image.GetFilename() << endl;

	VideoManager->PushState();
	VideoManager->SetCoordSys(0.0f, 1024.0f, 768.0f, 0.0f);

	_display_textbox.SetDisplaySpeed(30);
	_display_textbox.SetPosition(260.0f, 596.0f);
	_display_textbox.SetDimensions(700.0f, 126.0f);
	_display_textbox.SetTextStyle(TextStyle("text20", Color::black, VIDEO_TEXT_SHADOW_LIGHT));
	_display_textbox.SetDisplayMode(VIDEO_TEXT_FADECHAR);
	_display_textbox.SetAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
	_display_textbox.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);

	_display_optionbox.SetPosition(300.0f, 630.0f);
	_display_optionbox.SetDimensions(660.0f, 90.0f, 1, 255, 1, 4);
	_display_optionbox.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
	_display_optionbox.SetTextStyle(TextStyle("title20", Color::black, VIDEO_TEXT_SHADOW_LIGHT));
	_display_optionbox.SetSelectMode(VIDEO_SELECT_SINGLE);
	_display_optionbox.SetCursorOffset(-55.0f, -25.0f);
	_display_optionbox.SetVerticalWrapMode(VIDEO_WRAP_MODE_NONE);
	_display_optionbox.SetSelection(0);

	_name_text.SetStyle(TextStyle("title22", Color::black, VIDEO_TEXT_SHADOW_LIGHT));

	VideoManager->PopState();
}



void CommonDialogueWindow::SetPosition(float pos_x, float pos_y) {
	_pos_x = pos_x;
	_pos_y = pos_y;

	_display_textbox.SetPosition(_pos_x + 120.0f, _pos_y - 90.0f);
	_display_optionbox.SetPosition(_pos_x - 220.0f, _pos_y - 112.0f);
}



void CommonDialogueWindow::Clear() {
	_display_textbox.ClearText();
	_display_optionbox.ClearOptions();
	_name_text.Clear();
	_portrait_image = NULL;
}



void CommonDialogueWindow::Draw() {
	// Temporarily change the coordinate system to 1024x768 and draw the contents of the dialogue window
	VideoManager->PushState();
	VideoManager->SetCoordSys(0.0f, 1024.0f, 768.0f, 0.0f);
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);

	VideoManager->Move(_pos_x, _pos_y);
	_parchment_image.Draw();

	// TODO: nameplate is not drawn for now because its not visually appealing. Eventually we'll either decide
	// to remove it entirely or re-enable it with an improved nameplate.
// 	VideoManager->MoveRelative(-370.0f, -10.0f);
// 	_nameplate_image.Draw();

	VideoManager->MoveRelative(-370.0f, -15.0f);
	_name_text.Draw();

	if (_portrait_image != NULL) {
		VideoManager->MoveRelative(0.0f, -25.0f);
		_portrait_image->Draw();
		VideoManager->MoveRelative(0.0f, 25.0f);
	}

	_blink_time += SystemManager->GetUpdateTime();
	if (_blink_time > 500) {
		_blink_time -= 500;
		_blink_state = _blink_state ? false : true;
	}

	if (_indicator_symbol == COMMON_DIALOGUE_NEXT_INDICATOR && _blink_state) {
		VideoManager->MoveRelative(830.0f, 0.0f);
		_next_line_image.Draw();
	}
	else if (_indicator_symbol == COMMON_DIALOGUE_LAST_INDICATOR && _blink_state) {
		VideoManager->MoveRelative(830.0f, 0.0f);
		_last_line_image.Draw();
	}

	_display_textbox.Draw();
	_display_optionbox.Draw();

	VideoManager->PopState();
}

///////////////////////////////////////////////////////////////////////////////
// CommonDialogueSupervisor class methods
///////////////////////////////////////////////////////////////////////////////

CommonDialogueSupervisor::CommonDialogueSupervisor() :
	_current_dialogue(NULL),
	_current_options(NULL),
	_line_timer(),
	_line_counter(0)
{}



CommonDialogueSupervisor::~CommonDialogueSupervisor() {
	// Delete all dialogues
	for (map<uint32, CommonDialogue*>::iterator i = _dialogues.begin(); i != _dialogues.end(); i++) {
		delete i->second;
	}
	_dialogues.clear();
}



void CommonDialogueSupervisor::AddDialogue(CommonDialogue* dialogue) {
	if (dialogue == NULL) {
		IF_PRINT_WARNING(COMMON_DEBUG) << "function received NULL argument" << endl;
		return;
	}

	if (GetDialogue(dialogue->GetDialogueID()) != NULL) {
		IF_PRINT_WARNING(COMMON_DEBUG) << "a dialogue was already registered with this ID: " << dialogue->GetDialogueID() << endl;
		delete dialogue;
		return;
	}
	else {
		_dialogues.insert(make_pair(dialogue->GetDialogueID(), dialogue));
	}
}




void CommonDialogueSupervisor::BeginDialogue(uint32 dialogue_id) {
	CommonDialogue* dialogue = GetDialogue(dialogue_id);

	if (dialogue == NULL) {
		IF_PRINT_WARNING(COMMON_DEBUG) << "could not begin dialogue because none existed for id# " << dialogue_id << endl;
		return;
	}

	if (_current_dialogue != NULL) {
		IF_PRINT_WARNING(COMMON_DEBUG) << "beginning a new dialogue while another dialogue is still active" << endl;
	}

	_line_counter = 0;
	_current_dialogue = dialogue;
	_current_options = _current_dialogue->GetLineOptions(_line_counter);
	// Setup the line timer only if the first line has a display time
	if (_current_dialogue->GetLineDisplayTime(_line_counter) >= 0) {
		_line_timer.Initialize(_current_dialogue->GetLineDisplayTime(_line_counter));
	}
	else {
		_line_timer.Finish();
	}
}



void CommonDialogueSupervisor::EndDialogue() {
	if (_current_dialogue == NULL) {
		IF_PRINT_WARNING(COMMON_DEBUG) << "tried to end a dialogue when none was active" << endl;
		return;
	}

	_current_dialogue = NULL;
	_current_options = NULL;
	_line_timer.Finish();
}



CommonDialogue* CommonDialogueSupervisor::GetDialogue(uint32 dialogue_id) {
	map<uint32, CommonDialogue*>::iterator location = _dialogues.find(dialogue_id);
	if (location == _dialogues.end()) {
		return NULL;
	}
	else {
		return location->second;
	}
}

} // namespace hoa_common
