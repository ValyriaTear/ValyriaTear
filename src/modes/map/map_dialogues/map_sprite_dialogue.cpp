///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_dialogues/map_sprite_dialogue.h"

#include "modes/map/map_dialogue_supervisor.h"
#include "modes/map/map_event_supervisor.h"
#include "modes/map/map_dialogues/map_dialogue_options.h"
#include "modes/map/map_sprites/map_sprite.h"
#include "modes/map/map_mode.h"

#include "common/global/global.h"

namespace vt_map
{

namespace private_map
{

SpriteDialogue::SpriteDialogue() :
    Dialogue(MapMode::CurrentInstance()->GetDialogueSupervisor()->GenerateDialogueID()),
    _input_blocked(false),
    _restore_state(true),
    _dialogue_seen(false)
{
    // Auto-registers the dialogue for later deletion handling.
    MapMode::CurrentInstance()->GetDialogueSupervisor()->AddDialogue(this);
}

SpriteDialogue::SpriteDialogue(const std::string& dialogue_event_name) :
    Dialogue(MapMode::CurrentInstance()->GetDialogueSupervisor()->GenerateDialogueID()),
    _input_blocked(false),
    _restore_state(true),
    _event_name(dialogue_event_name)
{
    // Check whether the dialogue as already been seen
    _dialogue_seen = false;
    if (_event_name.empty())
        return;

    int32_t seen = vt_global::GlobalManager->GetEventValue("dialogues", _event_name);
    if (seen > 0)
        _dialogue_seen = true;

    // Auto-registers the dialogue for later deletion handling.
    MapMode::CurrentInstance()->GetDialogueSupervisor()->AddDialogue(this);
}

SpriteDialogue* SpriteDialogue::Create()
{
    // The object auto register to the object supervisor
    // and will later handle deletion.
    return new SpriteDialogue();
}

SpriteDialogue* SpriteDialogue::Create(const std::string& dialogue_event_name)
{
    // The object auto register to the object supervisor
    // and will later handle deletion.
    return new SpriteDialogue(dialogue_event_name);
}

void SpriteDialogue::SetAsSeen(bool seen)
{
    _dialogue_seen = seen;

    if (_event_name.empty())
        return;

    // Stores the dialogue state in the save data
    int32_t event_value = 0;
    if (_dialogue_seen)
        event_value = 1;

    vt_global::GlobalManager->SetEventValue("dialogues", _event_name, event_value);
}

void SpriteDialogue::AddLine(const std::string &text, MapSprite *speaker)
{
    AddLineTimedEvent(text, speaker,
                      vt_common::DIALOGUE_NEXT_LINE, vt_common::DIALOGUE_NO_TIMER,
                      std::string(), std::string(), std::string());
}

void SpriteDialogue::AddLineEmote(const std::string &text, MapSprite *speaker,
                                  const std::string &emote_id)
{
    AddLineTimedEvent(text, speaker,
                      vt_common::DIALOGUE_NEXT_LINE, vt_common::DIALOGUE_NO_TIMER,
                      std::string(), std::string(), emote_id);
}

void SpriteDialogue::AddLine(const std::string &text, MapSprite *speaker, int32_t next_line)
{
    AddLineTimedEvent(text, speaker,
                      next_line, vt_common::DIALOGUE_NO_TIMER,
                      std::string(), std::string(), std::string());
}

void SpriteDialogue::AddLineTimed(const std::string &text, MapSprite *speaker, uint32_t display_time)
{
    AddLineTimedEvent(text, speaker,
                      vt_common::DIALOGUE_NEXT_LINE, display_time,
                      std::string(), std::string(), std::string());
}

void SpriteDialogue::AddLineTimed(const std::string &text, MapSprite *speaker, int32_t next_line, uint32_t display_time)
{
    AddLineTimedEvent(text, speaker,
                      next_line, display_time,
                      std::string(), std::string(), std::string());
}

void SpriteDialogue::AddLineEvent(const std::string &text, MapSprite *speaker,
                                  const std::string &begin_event_id, const std::string &end_event_id)
{
    AddLineTimedEvent(text, speaker,
                      vt_common::DIALOGUE_NEXT_LINE, vt_common::DIALOGUE_NO_TIMER,
                      begin_event_id, end_event_id, std::string());
}

void SpriteDialogue::AddLineEventEmote(const std::string &text,
                                       MapSprite *speaker,
                                       const std::string &begin_event_id,
                                       const std::string &end_event_id,
                                       const std::string &emote_id)
{
    AddLineTimedEvent(text, speaker,
                      vt_common::DIALOGUE_NEXT_LINE, vt_common::DIALOGUE_NO_TIMER,
                      begin_event_id, end_event_id, emote_id);
}

void SpriteDialogue::AddLineEvent(const std::string &text, MapSprite *speaker, int32_t next_line,
                                  const std::string &begin_event_id, const std::string &end_event_id)
{
    AddLineTimedEvent(text, speaker,
                      next_line, vt_common::DIALOGUE_NO_TIMER,
                      begin_event_id, end_event_id, std::string());
}

void SpriteDialogue::AddLineTimedEvent(const std::string &text, MapSprite *speaker, uint32_t display_time,
                                       const std::string &begin_event_id, const std::string &end_event_id)
{
    AddLineTimedEvent(text, speaker,
                      vt_common::DIALOGUE_NEXT_LINE, display_time,
                      begin_event_id, end_event_id, std::string());
}

void SpriteDialogue::AddLineTimedEvent(const std::string &text, MapSprite *speaker,
                                       int32_t next_line, uint32_t display_time,
                                       const std::string &begin_event_id, const std::string &end_event_id,
                                       const std::string& emote_id)
{
    Dialogue::AddLineTimed(text, next_line, display_time);
    _speakers.push_back(speaker);
    _begin_events.push_back(begin_event_id);
    _end_events.push_back(end_event_id);
    _emote_events.push_back(emote_id);
}

void SpriteDialogue::AddOption(const std::string &text)
{
    AddOptionEvent(text, vt_common::DIALOGUE_NEXT_LINE, std::string());
}

void SpriteDialogue::AddOption(const std::string &text, int32_t next_line)
{
    AddOptionEvent(text, next_line, std::string());
}

void SpriteDialogue::AddOptionEvent(const std::string &text, const std::string &event_id)
{
    AddOptionEvent(text, vt_common::DIALOGUE_NEXT_LINE, event_id);
}

void SpriteDialogue::AddOptionEvent(const std::string &text, int32_t next_line, const std::string &event_id)
{
    if(_line_count == 0) {
        IF_PRINT_WARNING(MAP_DEBUG) << "Attempted to add an option to a dialogue with no lines" << std::endl;
        return;
    }

    uint32_t current_line = _line_count - 1;

    // If the line the options will be added to currently has no options, create a new instance of the MapDialogueOptions class to store the options in.
    if(_options[current_line] == nullptr) {
        _options[current_line] = new MapDialogueOptions();
    }

    MapDialogueOptions *options = dynamic_cast<MapDialogueOptions *>(_options[current_line]);
    options->AddOptionEvent(text, next_line, event_id);
}

bool SpriteDialogue::Validate()
{
    if(Dialogue::Validate() == false) {
        // The CommonDialogue::Validate() call will print the appropriate warnings
        // if debugging is enabled (common code debugging that is)
        return false;
    }

    // Construct containers that hold all unique sprite and event ids for this dialogue
    std::set<std::string> event_ids;
    for(uint32_t i = 0; i < _line_count; i++) {
        event_ids.insert(_end_events[i]);
    }

    for(std::set<std::string>::iterator it = event_ids.begin(); it != event_ids.end(); ++it) {
        if((*it).empty())
            continue;

        if(MapMode::CurrentInstance()->GetEventSupervisor()->GetEvent(*it) == nullptr) {
            IF_PRINT_WARNING(MAP_DEBUG) << "Validation failed for dialogue #" << _dialogue_id
                                        << ": dialogue referenced invalid event with id: " << *it << std::endl;
            return false;
        }
    }

    return true;
}

} // namespace private_map

} // namespace vt_map
