///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_dialogue.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for battle dialogue code.
*** ***************************************************************************/

// Allacrost engines
#include "input.h"
#include "video.h"

// Allacrost common
#include "dialogue.h"
#include "global.h"
#include "gui.h"

// Local battle mode headers
#include "battle.h"
#include "battle_actors.h"
#include "battle_dialogue.h"

using namespace std;

using namespace hoa_utils;
using namespace hoa_input;
using namespace hoa_video;
using namespace hoa_common;
using namespace hoa_gui;

namespace hoa_battle {

namespace private_battle {

///////////////////////////////////////////////////////////////////////////////
// BattleDialogue Class Functions
///////////////////////////////////////////////////////////////////////////////

BattleDialogue::BattleDialogue(uint32 id) :
	CommonDialogue(id),
	_halt_battle_action(true)
{}



void BattleDialogue::AddLine(string text, uint32 speaker) {
	AddLineTimed(text, speaker, COMMON_DIALOGUE_NEXT_LINE, COMMON_DIALOGUE_NO_TIMER);
}



void BattleDialogue::AddLine(string text, uint32 speaker, int32 next_line) {
	AddLineTimed(text, speaker, next_line, COMMON_DIALOGUE_NO_TIMER);
}



void BattleDialogue::AddLineTimed(string text, uint32 speaker, uint32 display_time) {
	AddLineTimed(text, speaker, COMMON_DIALOGUE_NEXT_LINE, display_time);
}



void BattleDialogue::AddLineTimed(string text, uint32 speaker, int32 next_line, uint32 display_time) {
	CommonDialogue::AddLineTimed(text, next_line, display_time);
	_speakers.push_back(speaker);
}



bool BattleDialogue::Validate() {
	if (CommonDialogue::Validate() == false) {
		// The BattleDialogue::Validate() call will print the appropriate warning if debugging is enabled (common code debugging that is)
		return false;
	}

	// Construct containers that hold all unique sprite and event ids for this dialogue
	set<uint32> speaker_ids;
	for (uint32 i = 0; i < _line_count; i++) {
		speaker_ids.insert(_speakers[i]);
	}

	// Check that all sprites and events referrenced by the dialogue exist
	for (set<uint32>::iterator i = speaker_ids.begin(); i != speaker_ids.end(); i++) {
		if (BattleMode::CurrentInstance()->GetDialogueSupervisor()->GetSpeaker(*i) == NULL) {
			IF_PRINT_WARNING(BATTLE_DEBUG) << "Validation failed for dialogue #" << _dialogue_id
				<< ": dialogue referenced invalid speaker with id: " << *i << endl;
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// DialogueSupervisor class methods
///////////////////////////////////////////////////////////////////////////////

DialogueSupervisor::DialogueSupervisor() :
	_state(DIALOGUE_STATE_INACTIVE),
	_current_dialogue(NULL),
	_current_options(NULL),
	_line_timer(),
	_line_counter(0),
	_dialogue_window()
{
	_dialogue_window.SetPosition(512.0f, 170.0f);
}



DialogueSupervisor::~DialogueSupervisor() {
	// Delete all dialogues
	for (map<uint32, BattleDialogue*>::iterator i = _dialogues.begin(); i != _dialogues.end(); i++) {
		delete i->second;
	}
	_dialogues.clear();
	_speakers.clear();
}



void DialogueSupervisor::Update() {
	if (_current_dialogue == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "attempted to update when no dialogue was active" << endl;
		return;
	}

	_line_timer.Update();

	switch (_state) {
		case DIALOGUE_STATE_LINE:
			_UpdateLine();
			break;
		case DIALOGUE_STATE_OPTION:
			_UpdateOptions();
			break;
		default:
			IF_PRINT_WARNING(BATTLE_DEBUG) << "dialogue supervisor was in an unknown state: " << _state << endl;
			_state = DIALOGUE_STATE_LINE;
			break;
	}
}



void DialogueSupervisor::Draw() {
	_dialogue_window.Draw();
}



void DialogueSupervisor::AddDialogue(BattleDialogue* dialogue) {
	if (dialogue == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL argument" << endl;
		return;
	}

	if (GetDialogue(dialogue->GetDialogueID()) != NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "a dialogue was already registered with this ID: " << dialogue->GetDialogueID() << endl;
		delete dialogue;
		return;
	}
	else {
		_dialogues.insert(make_pair(dialogue->GetDialogueID(), dialogue));
	}
}



void DialogueSupervisor::AddCharacterSpeaker(uint32 id, BattleCharacter* character) {
	if (character == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL character argument" << endl;
		return;
	}
	if (_speakers.find(id) != _speakers.end()) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "speaker already existed with requsted id: " << id << endl;
		return;
	}

	BattleSpeaker new_speaker;
	new_speaker.name = character->GetGlobalCharacter()->GetName();
	// TODO: this global function does not exist yet, add it in later after global code supports it
// 	new_speaker.portrait = character->GetGlobalCharacter()->GetPortrait();

	_speakers[id] = new_speaker;
}



void DialogueSupervisor::AddEnemySpeaker(uint32 id, BattleEnemy* enemy) {
	if (_speakers.find(id) != _speakers.end()) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "speaker already existed with requsted id: " << id << endl;
		return;
	}

	BattleSpeaker new_speaker;
	new_speaker.name = enemy->GetGlobalEnemy()->GetName();

	_speakers[id] = new_speaker;
}



void DialogueSupervisor::AddCustomSpeaker(uint32 id, string name, string portrait) {
	if (_speakers.find(id) != _speakers.end()) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "speaker already existed with requsted id: " << id << endl;
		return;
	}

	BattleSpeaker new_speaker;
	new_speaker.name = MakeUnicodeString(name);
	if (portrait != "") {
		if (new_speaker.portrait.Load(portrait) == false) {
			IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid image filename for new portrait: " << portrait << endl;
		}
	}

	_speakers[id] = new_speaker;
}



void DialogueSupervisor::ChangeSpeakerName(uint32 id, string name) {
	map<uint32, BattleSpeaker>::iterator speaker = _speakers.find(id);
	if (speaker == _speakers.end()) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "no speaker found with requested id: " << id << endl;
		return;
	}

	speaker->second.name = MakeUnicodeString(name);

	if (_current_dialogue != NULL) {
		if (_current_dialogue->GetLineSpeaker(_line_counter) == id) {
			_dialogue_window.GetNameText().SetText(speaker->second.name);
		}
	}
}



void DialogueSupervisor::ChangeSpeakerPortrait(uint32 id, string portrait) {
	map<uint32, BattleSpeaker>::iterator speaker = _speakers.find(id);
	if (speaker == _speakers.end()) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "no speaker found with requested id: " << id << endl;
		return;
	}

	if (portrait != "") {
		if (speaker->second.portrait.Load(portrait) == false) {
			IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid image filename for new portrait: " << portrait << endl;
			return;
		}
	}

	// Note: we don't have to also check whether or not the active portrait on the dialogue window needs to be
	// updated since the dialogue window simply retains a pointer to the image object. We only update the StillImage
	// class object contents in this function, not its address.
}



void DialogueSupervisor::BeginDialogue(uint32 dialogue_id) {
	BattleDialogue* dialogue = GetDialogue(dialogue_id);

	if (dialogue == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "could not begin dialogue because none existed for id# " << dialogue_id << endl;
		return;
	}

	if (_current_dialogue != NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "beginning a new dialogue while another dialogue is still active" << endl;
	}

	_line_counter = 0;
	_current_dialogue = dialogue;

	_BeginLine();
}



void DialogueSupervisor::EndDialogue() {
	if (_current_dialogue == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "tried to end a dialogue when none was active" << endl;
		return;
	}

	_current_dialogue->IncrementTimesSeen();
	_current_dialogue = NULL;
	_current_options = NULL;
	_line_timer.Finish();
}



void DialogueSupervisor::ForceNextLine() {
	if (_current_dialogue == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "function called when no dialogue was active" << endl;
		return;
	}

	_EndLine();
}



BattleDialogue* DialogueSupervisor::GetDialogue(uint32 dialogue_id) {
	map<uint32, BattleDialogue*>::iterator location = _dialogues.find(dialogue_id);
	if (location == _dialogues.end()) {
		return NULL;
	}
	else {
		return location->second;
	}
}



BattleSpeaker* DialogueSupervisor::GetSpeaker(uint32 speaker_id) {
	map<uint32, BattleSpeaker>::iterator i = _speakers.find(speaker_id);
	if (i != _speakers.end()) {
		return &(i->second);
	}

	return NULL;
}



void DialogueSupervisor::_UpdateLine() {
	_dialogue_window.GetDisplayTextBox().Update();

	if (_current_options != NULL) {
		if (_dialogue_window.GetDisplayTextBox().IsFinished() == true) {
			_state = DIALOGUE_STATE_OPTION;
			return;
		}
	}

	// If the line has a valid display time and the timer is finished, move on to the next line
	if ((_line_timer.GetDuration() > 0) && (_line_timer.IsFinished() == true)) {
		_EndLine();
		return;
	}

	// Set the correct indicator
	if (_current_dialogue->IsHaltBattleAction() == false || _current_options != NULL || _dialogue_window.GetDisplayTextBox().IsFinished() == false) {
		_dialogue_window.SetIndicator(COMMON_DIALOGUE_NO_INDICATOR);
	}
	else if (_line_counter == _current_dialogue->GetLineCount()-1) {
		_dialogue_window.SetIndicator(COMMON_DIALOGUE_LAST_INDICATOR);
	}
	else {
		_dialogue_window.SetIndicator(COMMON_DIALOGUE_NEXT_INDICATOR);
	}

	// If this dialogue does not halt the battle action, user input is not processed so we are finished
	if (_current_dialogue->IsHaltBattleAction() == false) {
		return;
	}

	if (InputManager->ConfirmPress()) {
		// If the line is not yet finished displaying, display the rest of the text
		if (_dialogue_window.GetDisplayTextBox().IsFinished() == false) {
			_dialogue_window.GetDisplayTextBox().ForceFinish();
		}
		// Proceed to option selection if the line has options
		else if (_current_options != NULL) {
			_state = DIALOGUE_STATE_OPTION;
		}
		else {
			_EndLine();
		}
	}
}



void DialogueSupervisor::_UpdateOptions() {
	_dialogue_window.GetDisplayOptionBox().Update();

	if (InputManager->ConfirmPress()) {
		_dialogue_window.GetDisplayOptionBox().InputConfirm();
		_EndLine();
	}

	else if (InputManager->UpPress()) {
		_dialogue_window.GetDisplayOptionBox().InputUp();
	}

	else if (InputManager->DownPress()) {
		_dialogue_window.GetDisplayOptionBox().InputDown();
	}
}



void DialogueSupervisor::_BeginLine() {
	_state = DIALOGUE_STATE_LINE;
	_current_options = _current_dialogue->GetLineOptions(_line_counter);

	// Initialize the line timer
	if (_current_dialogue->GetLineDisplayTime(_line_counter) >= 0) {
		_line_timer.Initialize(_current_dialogue->GetLineDisplayTime(_line_counter));
		_line_timer.Run();
	}
	// If the line has no timer specified, set the line time to zero and put the timer in the finished state
	else {
		_line_timer.Initialize(0);
		_line_timer.Finish();
	}

	// Setup the text and graphics for the dialogue window
	_dialogue_window.Clear();
	_dialogue_window.GetDisplayTextBox().SetDisplayText(_current_dialogue->GetLineText(_line_counter));
	if (_current_options != NULL) {
		for (uint32 i = 0; i < _current_options->GetNumberOptions(); i++) {
			_dialogue_window.GetDisplayOptionBox().AddOption(_current_options->GetOptionText(i));
		}

		_dialogue_window.GetDisplayOptionBox().SetSelection(0);
	}

	BattleSpeaker* line_speaker = GetSpeaker(_current_dialogue->GetLineSpeaker(_line_counter));
	if (line_speaker == NULL) {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "dialogue #" << _current_dialogue->GetDialogueID()
			<< " referenced a speaker that did not exist with id: " << _current_dialogue->GetLineSpeaker(_line_counter) << endl;
		_dialogue_window.GetNameText().SetText("");
		_dialogue_window.SetPortraitImage(NULL);
	}
	else {
		_dialogue_window.GetNameText().SetText(line_speaker->name);
		_dialogue_window.SetPortraitImage(&(line_speaker->portrait));
	}
}



void DialogueSupervisor::_EndLine() {
	// Determine the next line to read
	int32 next_line = _current_dialogue->GetLineNextLine(_line_counter);
	// If this line had options, the selected option next line overrides the line's next line that we set above
	if (_current_options != NULL) {
		uint32 selected_option = _dialogue_window.GetDisplayOptionBox().GetSelection();
		next_line = _current_options->GetOptionNextLine(selected_option);
	}

	// --- Case 1: Explicitly setting the next line. Warn and end the dialogue if the line to move to is invalid
	if (next_line >= 0) {
		if (static_cast<uint32>(next_line) >= _current_dialogue->GetLineCount()) {
			IF_PRINT_WARNING(BATTLE_DEBUG) << "dialogue #" << _current_dialogue->GetDialogueID()
				<< " tried to set dialogue to invalid line. Current/next line values: {" << _line_counter
				<< ", " << next_line << "}" << endl;
			next_line = COMMON_DIALOGUE_END;
		}
	}
	// --- Case 2: Request to incrementing the current line. If we're incrementing past the last line, end the dialogue
	else if (next_line == COMMON_DIALOGUE_NEXT_LINE) {
		next_line = _line_counter + 1;
		if (static_cast<uint32>(next_line) >= _current_dialogue->GetLineCount())
			next_line = COMMON_DIALOGUE_END;
	}
	// --- Case 3: Request to end the current dialogue
	else if (next_line == COMMON_DIALOGUE_END) {
		// Do nothing
	}
	// --- Case 4: Unknown negative value. Warn and end dialogue
	else {
		IF_PRINT_WARNING(BATTLE_DEBUG) << "dialogue #" << _current_dialogue->GetDialogueID()
			<< " unknown next line control value: " << next_line << endl;
		next_line = COMMON_DIALOGUE_END;
	}

	// Now either end the dialogue or move on to the next line
	if (next_line == COMMON_DIALOGUE_END) {
		EndDialogue();
	}
	else {
		_line_counter = next_line;
		_BeginLine();
	}
}

} // namespace private_battle

} // namespace hoa_battle
