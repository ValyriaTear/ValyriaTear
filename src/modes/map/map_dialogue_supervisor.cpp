///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_dialogue_supervisor.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_dialogues/map_sprite_dialogue.h"
#include "modes/map/map_dialogues/map_dialogue_options.h"
#include "modes/map/map_sprites/map_sprite.h"
#include "modes/map/map_event_supervisor.h"

#include "common/global/global.h"
#include "engine/input.h"

namespace vt_map
{

namespace private_map
{

MapDialogueSupervisor::MapDialogueSupervisor() :
    _state(vt_common::DIALOGUE_STATE_INACTIVE),
    _next_dialogue_id(1),
    _current_dialogue(nullptr),
    _current_options(nullptr),
    _line_timer(),
    _line_counter(0),
    _dialogue_window(),
    _emote_triggered(false)
{
    _dialogue_window.SetPosition(512.0f, 760.0f);
}

MapDialogueSupervisor::~MapDialogueSupervisor()
{
    _current_dialogue = nullptr;
    _current_options = nullptr;

    // Delete all dialogues
    for(auto it = _dialogues.begin(); it != _dialogues.end(); ++it) {
        delete it->second;
    }
    _dialogues.clear();
}

void MapDialogueSupervisor::Update()
{
    if(_current_dialogue == nullptr) {
        PRINT_WARNING << "attempted to update when no dialogue was active" << std::endl;
        return;
    }

    _line_timer.Update();

    switch(_state) {
    case vt_common::DIALOGUE_STATE_EMOTE:
        _UpdateEmote();
        break;
    case vt_common::DIALOGUE_STATE_LINE:
        _UpdateLine();
        break;
    case vt_common::DIALOGUE_STATE_OPTION:
        _UpdateOptions();
        break;
    default:
        PRINT_WARNING << "dialogue supervisor was in an unknown state: "
                      << _state << std::endl;
        _state = vt_common::DIALOGUE_STATE_LINE;
        break;
    }
}

void MapDialogueSupervisor::Draw()
{
    if(_state != vt_common::DIALOGUE_STATE_EMOTE)
        _dialogue_window.Draw();
}

void MapDialogueSupervisor::AddDialogue(SpriteDialogue* dialogue)
{
    if(dialogue == nullptr) {
        PRINT_WARNING << "function received nullptr argument" << std::endl;
        return;
    }

    if(GetDialogue(dialogue->GetDialogueID()) != nullptr) {
        PRINT_WARNING << "A dialogue was already registered with this ID: "
                      << dialogue->GetDialogueID() << std::endl;
        delete dialogue;
        return;
    } else {
        _dialogues.insert(std::make_pair(dialogue->GetDialogueID(), dialogue));
    }
}

void MapDialogueSupervisor::StartDialogue(const std::string& dialogue_id)
{
    SpriteDialogue *dialogue = GetDialogue(dialogue_id);

    if(dialogue == nullptr) {
        PRINT_WARNING << "Could not begin dialogue because none existed for id: "
                      << dialogue_id << std::endl
                      << "Did you register the dialogue using 'AddDialogue()'?" << std::endl;
        return;
    }

    if(_current_dialogue != nullptr) {
        PRINT_WARNING << "beginning a new dialogue while another dialogue is still active" << std::endl;
    }

    _line_counter = 0;
    _current_dialogue = dialogue;
    _emote_triggered = false;
    _BeginLine();
    MapMode::CurrentInstance()->PushState(STATE_DIALOGUE);
}

void MapDialogueSupervisor::EndDialogue()
{
    if(_current_dialogue == nullptr) {
        PRINT_WARNING << "tried to end a dialogue when there was no dialogue active" << std::endl;
        return;
    }

    _current_dialogue->SetAsSeen();

    // We only want to call the RestoreState function *once* for each speaker, so first we have to construct a list of pointers
    // for all speakers without duplication (i.e. the case where a speaker spoke more than one line of dialogue).

    MapMode *map_mode = MapMode::CurrentInstance();

    // Get a unique set of all sprites that participated in the dialogue
    std::set<MapSprite *> speakers;
    for(uint32_t i = 0; i < _current_dialogue->GetLineCount(); ++i) {
        MapSprite* speaker = _current_dialogue->GetLineSpeaker(i);
        speakers.insert(speaker);
    }

    for(std::set<MapSprite *>::iterator it = speakers.begin(); it != speakers.end(); ++it) {
        if((*it) == nullptr)
            continue;
        // Each sprite needs to know that this dialogue completed so that they can update their data accordingly
        (*it)->UpdateDialogueStatus();

        // Restore the state (orientation, animation, etc.) of all speaker sprites if necessary
        if(_current_dialogue->IsRestoreState()) {
            if((*it)->IsStateSaved())
                (*it)->RestoreState();
        }
    }

    map_mode->PopState();

    std::string event_id = _current_dialogue->GetEventAtDialogueEnd();
    if (!event_id.empty()) {
        // Trigger the event after popping the map state, permitting
        // to set a scene state afterward, for instance.
        map_mode->GetEventSupervisor()->StartEvent(event_id);
    }

    _current_dialogue = nullptr;
    _current_options = nullptr;
}

SpriteDialogue* MapDialogueSupervisor::GetDialogue(const std::string& dialogue_id)
{
    std::map<std::string, SpriteDialogue *>::iterator it = _dialogues.find(dialogue_id);
    if(it == _dialogues.end())
        return nullptr;

    return it->second;
}

void MapDialogueSupervisor::_UpdateEmote()
{
    MapObject* object = _current_dialogue->GetLineSpeaker(_line_counter);

    if(!object || !object->HasEmote()) {
        _emote_triggered = true;
        _BeginLine();
    }
}

void MapDialogueSupervisor::_UpdateLine()
{
    _dialogue_window.GetDisplayTextBox().Update();

    // If the line has a valid display time and the timer is finished, move on to the next line
    if((_line_timer.GetDuration() > 0) && (_line_timer.IsFinished())) {
        _EndLine();
        return;
    }

    // Set the correct indicator
    if(_current_dialogue->IsInputBlocked()
            || _current_options != nullptr
            || _dialogue_window.GetDisplayTextBox().IsFinished() == false) {
        _dialogue_window.SetIndicator(vt_common::DIALOGUE_NO_INDICATOR);
        vt_global::GlobalManager->Media().PlaySound("text");
    } else if(_line_counter == _current_dialogue->GetLineCount() - 1) {
        _dialogue_window.SetIndicator(vt_common::DIALOGUE_LAST_INDICATOR);
    } else {
        _dialogue_window.SetIndicator(vt_common::DIALOGUE_NEXT_INDICATOR);
    }

    // If this dialogue does not allow user input, we are finished
    if(_current_dialogue->IsInputBlocked()) {
        return;
    }

    if(vt_input::InputManager->ConfirmPress()) {
        // If the line is not yet finished displaying, display the rest of the text
        if(_dialogue_window.GetDisplayTextBox().IsFinished() == false) {
            _dialogue_window.GetDisplayTextBox().ForceFinish();
        }
        // Proceed to option selection if the line has options
        else if(_current_options != nullptr) {
            _state = vt_common::DIALOGUE_STATE_OPTION;
        } else {
            vt_global::GlobalManager->Media().PlaySound("confirm");
            _EndLine();
        }
    }
}

void MapDialogueSupervisor::_UpdateOptions()
{
    _dialogue_window.GetDisplayTextBox().Update();
    _dialogue_window.GetDisplayOptionBox().Update();
    vt_global::GlobalMedia& media = vt_global::GlobalManager->Media();

    if(vt_input::InputManager->ConfirmPress()) {
        _dialogue_window.GetDisplayOptionBox().InputConfirm();
        media.PlaySound("confirm");
        _EndLine();
    }
    else if(vt_input::InputManager->UpPress()) {
        media.PlaySound("bump");
        _dialogue_window.GetDisplayOptionBox().InputUp();
    }
    else if(vt_input::InputManager->DownPress()) {
        media.PlaySound("bump");
        _dialogue_window.GetDisplayOptionBox().InputDown();
    }
}

void MapDialogueSupervisor::_BeginLine()
{
    // Starts possible events at new line.
    MapMode* map_mode = MapMode::CurrentInstance();
    std::string line_event = _current_dialogue->GetLineBeginEvent(_line_counter);
    if(!line_event.empty() && !map_mode->GetEventSupervisor()->IsEventActive(line_event)) {
        map_mode->GetEventSupervisor()->StartEvent(line_event);
    }

    // The current speaker id
    MapSprite* sprite = _current_dialogue->GetLineSpeaker(_line_counter);

    // Starts possible emote first.
    std::string emote_event = _current_dialogue->GetLineEmote(_line_counter);
    if(!emote_event.empty() && !_emote_triggered && sprite) {
        sprite->Emote(emote_event, (vt_map::private_map::ANIM_DIRECTIONS)sprite->GetCurrentAnimationDirection());
        _state = vt_common::DIALOGUE_STATE_EMOTE;
        _emote_triggered = true;
        return;
    }

    _emote_triggered = true;
    _state = vt_common::DIALOGUE_STATE_LINE;
    _current_options = dynamic_cast<MapDialogueOptions *>(_current_dialogue->GetLineOptions(_line_counter));

    // Initialize the line timer
    if(_current_dialogue->GetLineDisplayTime(_line_counter) >= 0) {
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

    if(!sprite) {
        // Clear the speaker name and potential portrait.
        _dialogue_window.GetNameText().Clear();
        _dialogue_window.SetPortraitImage(nullptr);
    } else {
        _dialogue_window.GetNameText().SetText(sprite->GetName());
        _dialogue_window.SetPortraitImage(sprite->GetFacePortrait());
    }

    if(_current_options) {
        for(uint32_t i = 0; i < _current_options->GetNumberOptions(); ++i)
            _dialogue_window.GetDisplayOptionBox().AddOption(_current_options->GetOptionText(i));

        _dialogue_window.GetDisplayOptionBox().SetSelection(0);
        _state = vt_common::DIALOGUE_STATE_OPTION;
    }
}

void MapDialogueSupervisor::_EndLine()
{
    // Execute any scripted events that should occur after this line of dialogue has finished
    std::string line_event = _current_dialogue->GetLineEndEvent(_line_counter);
    if(!line_event.empty()) {
        MapMode::CurrentInstance()->GetEventSupervisor()->StartEvent(line_event);
    }

    if(_current_options != nullptr) {
        uint32_t selected_option = _dialogue_window.GetDisplayOptionBox().GetSelection();
        std::string selected_event = _current_options->GetOptionEvent(selected_option);
        if(!selected_event.empty()) {
            MapMode::CurrentInstance()->GetEventSupervisor()->StartEvent(selected_event);
        }
    }

    // Determine the next line to read
    int32_t next_line = _current_dialogue->GetLineNextLine(_line_counter);
    // If this line had options, the selected option next line overrides the line's next line that we set above
    if(_current_options != nullptr) {
        uint32_t selected_option = _dialogue_window.GetDisplayOptionBox().GetSelection();
        next_line = _current_options->GetOptionNextLine(selected_option);
    }

    // --- Case 1: Explicitly setting the next line. Warn and end the dialogue if the line to move to is invalid
    if(next_line >= 0) {
        if(static_cast<uint32_t>(next_line) >= _current_dialogue->GetLineCount()) {
            IF_PRINT_WARNING(MAP_DEBUG) << "dialogue #" << _current_dialogue->GetDialogueID()
                                        << " tried to set dialogue to invalid line. Current/next line values: {" << _line_counter
                                        << ", " << next_line << "}" << std::endl;
            next_line = vt_common::DIALOGUE_END;
        }
    }
    // --- Case 2: Request to incrementing the current line. If we're incrementing past the last line, end the dialogue
    else if(next_line == vt_common::DIALOGUE_NEXT_LINE) {
        next_line = _line_counter + 1;
        if(static_cast<uint32_t>(next_line) >= _current_dialogue->GetLineCount())
            next_line = vt_common::DIALOGUE_END;
    }
    // --- Case 3: Request to end the current dialogue
    else if(next_line == vt_common::DIALOGUE_END) {
        // Do nothing
    }
    // --- Case 4: Unknown negative value. Warn and end dialogue
    else {
        IF_PRINT_WARNING(MAP_DEBUG) << "dialogue #" << _current_dialogue->GetDialogueID()
                                    << " unknown next line control value: " << next_line << std::endl;
        next_line = vt_common::DIALOGUE_END;
    }

    // Now either end the dialogue or move on to the next line
    if(next_line == vt_common::DIALOGUE_END) {
        EndDialogue();
    } else {
        _line_counter = next_line;
        // Reset the emote trigger flag for the next line.
        _emote_triggered = false;
        _BeginLine();
    }
}

} // namespace private_map

} // namespace vt_map
