///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_events.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for map mode events and event processing.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/map/map_events.h"

#include "modes/map/map_mode.h"

#include "modes/map/map_sprites.h"

#include "modes/shop/shop.h"
#include "modes/battle/battle.h"

using namespace vt_audio;
using namespace vt_mode_manager;
using namespace vt_script;
using namespace vt_system;
using namespace vt_video;

using namespace vt_battle;
using namespace vt_shop;

namespace vt_map
{

namespace private_map
{

MapEvent::MapEvent(const std::string& id, EVENT_TYPE type):
    _event_id(id),
    _event_type(type)
{
    vt_map::MapMode* map_mode = MapMode::CurrentInstance();
    if (!map_mode) {
        PRINT_ERROR << "Event created without existing map mode!!" << std::endl;
        return;
    }

    vt_map::private_map::EventSupervisor* event_sup = map_mode->GetEventSupervisor();
    if (!event_sup) {
        PRINT_ERROR << "Event created without existing event supervisor!!" << std::endl;
        return;
    }

    if (!event_sup->_RegisterEvent(this))
        PRINT_ERROR << "Couldn't register event: " << id << " in the event supervisor!!" << std::endl;
}

// -----------------------------------------------------------------------------
// ---------- DialogueEvent Class Methods
// -----------------------------------------------------------------------------

DialogueEvent::DialogueEvent(const std::string& event_id, SpriteDialogue* dialogue) :
    MapEvent(event_id, DIALOGUE_EVENT),
    _dialogue_id(dialogue->GetDialogueID()),
    _stop_camera_movement(false)
{}

DialogueEvent* DialogueEvent::Create(const std::string& event_id, SpriteDialogue* dialogue)
{
    return new DialogueEvent(event_id, dialogue);
}

void DialogueEvent::_Start()
{
    if(_stop_camera_movement == true) {
        MapMode::CurrentInstance()->GetCamera()->moving = false;
        MapMode::CurrentInstance()->GetCamera()->is_running = false;
    }

    MapMode::CurrentInstance()->GetDialogueSupervisor()->StartDialogue(_dialogue_id);
}

bool DialogueEvent::_Update()
{
    SpriteDialogue *active_dialogue = MapMode::CurrentInstance()->GetDialogueSupervisor()->GetCurrentDialogue();
    if((active_dialogue != NULL) && (active_dialogue->GetDialogueID() == _dialogue_id))
        return false;
    else
        return true;
}

// -----------------------------------------------------------------------------
// ---------- ShopEvent Class Methods
// -----------------------------------------------------------------------------

ShopEvent* ShopEvent::Create(const std::string& event_id)
{
    return new ShopEvent(event_id);
}

void ShopEvent::_Start()
{
    ShopMode *shop = new ShopMode();
    for(std::set<std::pair<uint32, uint32> >::iterator it = _items.begin(); it != _items.end(); ++it)
        shop->AddItem((*it).first, (*it).second);

    for(std::set<std::pair<uint32, uint32> >::iterator it = _trades.begin(); it != _trades.end(); ++it)
        shop->AddTrade((*it).first, (*it).second);

    // Adds optional custom shop name and greetings text
    if (!_shop_name.empty())
        shop->SetShopName(_shop_name);
    if (!_greeting_text.empty())
        shop->SetGreetingText(_greeting_text);

    // Sets the shop options.
    shop->SetSellModeEnabled(_enable_sell_mode);
    shop->SetPriceLevels(_buy_level, _sell_level);

    // Loads potential scripts
    if (!_shop_scripts.empty())
        shop->GetScriptSupervisor().SetScripts(_shop_scripts);

    ModeManager->Push(shop);
}

// -----------------------------------------------------------------------------
// ---------- SoundEvent Class Methods
// -----------------------------------------------------------------------------

SoundEvent* SoundEvent::Create(const std::string& event_id, const std::string& sound_filename)
{
    return new SoundEvent(event_id, sound_filename);
}

SoundEvent::SoundEvent(const std::string& event_id, const std::string& sound_filename) :
    MapEvent(event_id, SOUND_EVENT)
{
    if(_sound.LoadAudio(sound_filename) == false) {
        PRINT_WARNING << "failed to load sound event: "
            << sound_filename << std::endl;
    }
}

bool SoundEvent::_Update()
{
    if(_sound.GetState() == AUDIO_STATE_STOPPED ||
            _sound.GetState() == AUDIO_STATE_UNLOADED ||
            _sound.GetState() == AUDIO_STATE_PAUSED) {
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------
// ---------- MapTransitionEvent Class Methods
// -----------------------------------------------------------------------------

MapTransitionEvent::MapTransitionEvent(const std::string& event_id,
                                       const std::string& data_filename,
                                       const std::string& script_filename,
                                       const std::string& coming_from) :
    MapEvent(event_id, MAP_TRANSITION_EVENT),
    _transition_map_data_filename(data_filename),
    _transition_map_script_filename(script_filename),
    _transition_origin(coming_from),
    _done(false)
{}

MapTransitionEvent* MapTransitionEvent::Create(const std::string& event_id,
                                               const std::string& data_filename,
                                               const std::string& script_filename,
                                               const std::string& coming_from)
{
    return new MapTransitionEvent(event_id, data_filename, script_filename,
                                  coming_from);
}

void MapTransitionEvent::_Start()
{
    MapMode::CurrentInstance()->PushState(STATE_SCENE);

    VideoManager->_StartTransitionFadeOut(Color::black, MAP_FADE_OUT_TIME);
    _done = false;
}

bool MapTransitionEvent::_Update()
{
    if(VideoManager->IsFading())
        return false;

    // Only load the map once the fade out is done, since the load time can
    // break the fade smoothness and visible duration.
    if(!_done) {
        vt_global::GlobalManager->SetPreviousLocation(_transition_origin);
        MapMode* MM = new MapMode(_transition_map_data_filename,
                                  _transition_map_script_filename,
                                  MapMode::CurrentInstance()->GetStamina());
        ModeManager->Pop();
        ModeManager->Push(MM, false, true);
        _done = true;
    }
    return true;
}

// -----------------------------------------------------------------------------
// ---------- BattleEncounterEvent Class Methods
// -----------------------------------------------------------------------------

BattleEncounterEvent::BattleEncounterEvent(const std::string& event_id) :
    MapEvent(event_id, BATTLE_ENCOUNTER_EVENT),
    _battle_music("mus/heroism-OGA-Edward-J-Blakeley.ogg"),
    _battle_background("img/backdrops/battle/desert_cave/desert_cave.png"),
    _is_boss(false)
{}

BattleEncounterEvent* BattleEncounterEvent::Create(const std::string& event_id)
{
    return new BattleEncounterEvent(event_id);
}

void BattleEncounterEvent::AddEnemy(uint32 enemy_id, float position_x, float position_y)
{
    _enemies.push_back(BattleEnemyInfo(enemy_id, position_x, position_y));
}

void BattleEncounterEvent::_Start()
{
    try {
        // Check the current map stamina and apply a malus on agility when it is low
        MapMode* MM = MapMode::CurrentInstance();
        if (MM)
            MM->ApplyPotentialStaminaMalus();

        BattleMode *BM = new BattleMode();
        for(uint32 i = 0; i < _enemies.size(); ++i)
            BM->AddEnemy(_enemies.at(i).enemy_id, _enemies.at(i).position_x, _enemies.at(i).position_y);

        BM->GetMedia().SetBackgroundImage(_battle_background);
        BM->GetMedia().SetBattleMusic(_battle_music);
        for(uint32 i = 0; i < _battle_scripts.size(); ++i)
            BM->GetScriptSupervisor().AddScript(_battle_scripts[i]);

        BM->SetBossBattle(_is_boss);

        TransitionToBattleMode* TM = new TransitionToBattleMode(BM, _is_boss);

        ModeManager->Push(TM);
    } catch(const luabind::error& e) {
        PRINT_ERROR << "Error while loading battle encounter event!"
                    << std::endl;
        ScriptManager->HandleLuaError(e);
    } catch(const luabind::cast_failed& e) {
        PRINT_ERROR << "Error while loading battle encounter event!"
                    << std::endl;
        ScriptManager->HandleCastError(e);
    }
}

// -----------------------------------------------------------------------------
// ---------- IfEvent Class Methods
// -----------------------------------------------------------------------------

IfEvent::IfEvent(const std::string& event_id, const std::string& check_function,
                 const std::string& on_true_event, const std::string& on_false_event) :
    MapEvent(event_id, IF_EVENT)
{
    ReadScriptDescriptor &map_script = MapMode::CurrentInstance()->GetMapScript();
    if (!MapMode::CurrentInstance()->OpenMapTablespace(true))
        return;
    if (!map_script.OpenTable("map_functions"))
        return;

    if(!check_function.empty())
        _check_function = map_script.ReadFunctionPointer(check_function);

    map_script.CloseTable(); // map_functions
    map_script.CloseTable(); // tablespace

    _true_event_id = on_true_event;
    _false_event_id = on_false_event;
}

IfEvent* IfEvent::Create(const std::string& event_id, const std::string& check_function,
                         const std::string& on_true_event, const std::string& on_false_event)
{
    return new IfEvent(event_id, check_function, on_true_event, on_false_event);
}

void IfEvent::_Start()
{
    if(!_check_function.is_valid())
        return;

    EventSupervisor* events = MapMode::CurrentInstance()->GetEventSupervisor();

    try {
        // We had a timer of 100ms her to avoid launching an event within an event
        // for the sake of the engine loop. That time is unnoticeable, anyway.
        if (ScriptCallFunction<bool>(_check_function)
            && !_true_event_id.empty() && !events->IsEventActive(_true_event_id)) {
            events->StartEvent(_true_event_id, 100);
        }
        else if (!_false_event_id.empty() && !events->IsEventActive(_false_event_id)) {
            events->StartEvent(_false_event_id, 100);
        }
    } catch(const luabind::error &e) {
        PRINT_ERROR << "Error while loading IFEvent check function." << std::endl;
        ScriptManager->HandleLuaError(e);
    } catch(const luabind::cast_failed &e) {
        PRINT_ERROR << "Error while loading IFEvent check function." << std::endl;
        ScriptManager->HandleCastError(e);
    }
}

// -----------------------------------------------------------------------------
// ---------- ScriptedEvent Class Methods
// -----------------------------------------------------------------------------

ScriptedEvent::ScriptedEvent(const std::string& event_id,
                             const std::string& start_function,
                             const std::string& update_function) :
    MapEvent(event_id, SCRIPTED_EVENT)
{
    ReadScriptDescriptor &map_script = MapMode::CurrentInstance()->GetMapScript();
    if (!MapMode::CurrentInstance()->OpenMapTablespace(true))
        return;
    if (!map_script.OpenTable("map_functions"))
        return;

    if(!start_function.empty())
        _start_function = map_script.ReadFunctionPointer(start_function);

    if(!update_function.empty())
        _update_function = map_script.ReadFunctionPointer(update_function);

    map_script.CloseTable(); // map_functions
    map_script.CloseTable(); // tablespace
}

ScriptedEvent* ScriptedEvent::Create(const std::string& event_id,
                                     const std::string& start_function,
                                     const std::string& update_function)
{
    return new ScriptedEvent(event_id, start_function, update_function);
}

void ScriptedEvent::_Start()
{
    if(!_start_function.is_valid())
        return;

    try {
        ScriptCallFunction<void>(_start_function);
    } catch(const luabind::error &e) {
        PRINT_ERROR << "Error while loading ScriptedEvent start function" << std::endl;
        ScriptManager->HandleLuaError(e);
    } catch(const luabind::cast_failed &e) {
        PRINT_ERROR << "Error while loading ScriptedEvent start function" << std::endl;
        ScriptManager->HandleCastError(e);
    }
}

bool ScriptedEvent::_Update()
{
    if(!_update_function.is_valid())
        return true;

    try {
        return ScriptCallFunction<bool>(_update_function);
    } catch(const luabind::error &e) {
        PRINT_ERROR << "Error while loading ScriptedEvent update function" << std::endl;
        ScriptManager->HandleLuaError(e);
    } catch(const luabind::cast_failed &e) {
        PRINT_ERROR << "Error while loading ScriptedEvent update function" << std::endl;
        ScriptManager->HandleCastError(e);
    }
    return true;
}

// -----------------------------------------------------------------------------
// ---------- SpriteEvent Class Methods
// -----------------------------------------------------------------------------

SpriteEvent::SpriteEvent(const std::string& event_id, EVENT_TYPE event_type, uint16 sprite_id) :
    MapEvent(event_id, event_type),
    _sprite(NULL)
{
    _sprite = MapMode::CurrentInstance()->GetObjectSupervisor()->GetSprite(sprite_id);
    if(!_sprite) {
        IF_PRINT_WARNING(MAP_DEBUG)
                << "sprite_id argument did not correspond to a known sprite object: "
                << event_id << std::endl;
    }
}

SpriteEvent::SpriteEvent(const std::string& event_id, EVENT_TYPE event_type, VirtualSprite* sprite) :
    MapEvent(event_id, event_type),
    _sprite(sprite)
{
    if(sprite == NULL)
        IF_PRINT_WARNING(MAP_DEBUG) << "NULL sprite object passed into constructor: "
                                    << event_id << std::endl;
}

void SpriteEvent::_Start()
{
    if (!_sprite) {
        PRINT_WARNING << "No valid sprite given in event: " << GetEventID() << std::endl;
        return;
    }

    EventSupervisor *event_supervisor = MapMode::CurrentInstance()->GetEventSupervisor();
    // Terminate the previous event whenever it is another sprite event.
    if(dynamic_cast<SpriteEvent *>(_sprite->control_event) && event_supervisor) {
        event_supervisor->EndEvent(_sprite->control_event, false);
    }

    _sprite->AcquireControl(this);
}

void SpriteEvent::Terminate()
{
    // Frees the sprite from the event.
    if(_sprite && _sprite->control_event == this) {
        _sprite->ReleaseControl(this);
    }
}

// -----------------------------------------------------------------------------
// ---------- ScriptedSpriteEvent Class Methods
// -----------------------------------------------------------------------------

ScriptedSpriteEvent::ScriptedSpriteEvent(const std::string& event_id, uint16 sprite_id,
                                         const std::string& start_function,
                                         const std::string& update_function) :
    SpriteEvent(event_id, SCRIPTED_SPRITE_EVENT, sprite_id)
{
    ReadScriptDescriptor& map_script = MapMode::CurrentInstance()->GetMapScript();
    if (!MapMode::CurrentInstance()->OpenMapTablespace(true))
        return;
    if (!map_script.OpenTable("map_functions"))
        return;

    if(!start_function.empty())
        _start_function = map_script.ReadFunctionPointer(start_function);

    if(!update_function.empty())
        _update_function = map_script.ReadFunctionPointer(update_function);

    map_script.CloseTable(); // map_functions
    map_script.CloseTable(); // tablespace
}

ScriptedSpriteEvent::ScriptedSpriteEvent(const std::string& event_id, VirtualSprite* sprite,
                                         const std::string& start_function,
                                         const std::string& update_function) :
    SpriteEvent(event_id, SCRIPTED_SPRITE_EVENT, sprite)
{
    ReadScriptDescriptor &map_script = MapMode::CurrentInstance()->GetMapScript();
    if (!MapMode::CurrentInstance()->OpenMapTablespace(true))
        return;
    if (!map_script.OpenTable("map_functions"))
        return;

    if(!start_function.empty())
        _start_function = map_script.ReadFunctionPointer(start_function);

    if(!update_function.empty())
        _update_function = map_script.ReadFunctionPointer(update_function);

    map_script.CloseTable(); // map_functions
    map_script.CloseTable(); // tablespace
}

ScriptedSpriteEvent* ScriptedSpriteEvent::Create(const std::string& event_id,
                                                 VirtualSprite* sprite,
                                                 const std::string& start_function,
                                                 const std::string& update_function)
{
    return new ScriptedSpriteEvent(event_id, sprite, start_function, update_function);
}

void ScriptedSpriteEvent::_Start()
{
    SpriteEvent::_Start();
    if(_start_function.is_valid())
        ScriptCallFunction<void>(_start_function, _sprite);
}

bool ScriptedSpriteEvent::_Update()
{
    bool finished = false;
    if(_update_function.is_valid()) {
        finished = ScriptCallFunction<bool>(_update_function, _sprite);
    } else {
        finished = true;
    }

    if(finished)
        SpriteEvent::Terminate();

    return finished;
}

// -----------------------------------------------------------------------------
// ---------- ChangeDirectionSpriteEvent Class Methods
// -----------------------------------------------------------------------------

ChangeDirectionSpriteEvent::ChangeDirectionSpriteEvent(const std::string& event_id, uint16 sprite_id, uint16 direction) :
    SpriteEvent(event_id, CHANGE_DIRECTION_SPRITE_EVENT, sprite_id),
    _direction(direction)
{
    if((_direction != NORTH) && (_direction != SOUTH) && (_direction != EAST) && (_direction != WEST)) {
        IF_PRINT_WARNING(MAP_DEBUG) << "non-standard direction specified: "
                                    << event_id << std::endl;
    }
}

ChangeDirectionSpriteEvent::ChangeDirectionSpriteEvent(const std::string& event_id, VirtualSprite* sprite, uint16 direction) :
    SpriteEvent(event_id, CHANGE_DIRECTION_SPRITE_EVENT, sprite),
    _direction(direction)
{
    if((_direction != NORTH) && (_direction != SOUTH) && (_direction != EAST) && (_direction != WEST)) {
        IF_PRINT_WARNING(MAP_DEBUG) << "non-standard direction specified: "
                                    << event_id << std::endl;
    }
}

ChangeDirectionSpriteEvent* ChangeDirectionSpriteEvent::Create(const std::string& event_id,
                                                               VirtualSprite* sprite,
                                                               uint16 direction)
{
    return new ChangeDirectionSpriteEvent(event_id, sprite, direction);
}

void ChangeDirectionSpriteEvent::_Start()
{
    SpriteEvent::_Start();
    _sprite->SetDirection(_direction);
}

bool ChangeDirectionSpriteEvent::_Update()
{
    SpriteEvent::Terminate();
    return true;
}

// ---------- LookAtSpriteEvent Class Methods

LookAtSpriteEvent::LookAtSpriteEvent(const std::string& event_id, uint16 sprite_id, uint16 second_sprite_id) :
    SpriteEvent(event_id, LOOK_AT_SPRITE_EVENT, sprite_id)
{
    // Invalid position.
    _x = _y = -1.0f;

    _target_sprite = MapMode::CurrentInstance()->GetObjectSupervisor()->GetSprite(second_sprite_id);
    if(!_target_sprite) {
        IF_PRINT_WARNING(MAP_DEBUG) << "Invalid second sprite id specified in event: "
                                    << event_id << std::endl;
    }
}

LookAtSpriteEvent::LookAtSpriteEvent(const std::string& event_id, VirtualSprite* sprite, VirtualSprite* other_sprite) :
    SpriteEvent(event_id, LOOK_AT_SPRITE_EVENT, sprite)
{
    // Invalid position.
    _x = _y = -1.0f;
    _target_sprite = other_sprite;

    if(!_target_sprite)
        IF_PRINT_WARNING(MAP_DEBUG) << "Invalid other sprite specified in event: " << event_id << std::endl;
}

LookAtSpriteEvent::LookAtSpriteEvent(const std::string& event_id, VirtualSprite* sprite, float x, float y) :
    SpriteEvent(event_id, LOOK_AT_SPRITE_EVENT, sprite),
    _x(x),
    _y(y),
    _target_sprite(0)
{}

LookAtSpriteEvent* LookAtSpriteEvent::Create(const std::string& event_id,
                                             VirtualSprite* sprite,
                                             VirtualSprite* other_sprite)
{
    return new LookAtSpriteEvent(event_id, sprite, other_sprite);
}

LookAtSpriteEvent* LookAtSpriteEvent::Create(const std::string& event_id,
                                             VirtualSprite* sprite,
                                             float x, float y)
{
    return new LookAtSpriteEvent(event_id, sprite, x, y);
}

void LookAtSpriteEvent::_Start()
{
    SpriteEvent::_Start();

    // When there is a target sprite, use it.
    if(_target_sprite) {
        _x = _target_sprite->GetXPosition();
        _y = _target_sprite->GetYPosition();
    }

    if(_x >= 0.0f && _y >= 0.0f)
        _sprite->LookAt(_x, _y);
}

bool LookAtSpriteEvent::_Update()
{
    SpriteEvent::Terminate();
    return true;
}

// -----------------------------------------------------------------------------
// ---------- PathMoveSpriteEvent Class Methods
// -----------------------------------------------------------------------------

PathMoveSpriteEvent::PathMoveSpriteEvent(const std::string& event_id, uint16 sprite_id,
                                         float x_coord, float y_coord, bool run) :
    SpriteEvent(event_id, PATH_MOVE_SPRITE_EVENT, sprite_id),
    _destination_x(x_coord),
    _destination_y(y_coord),
    _target_sprite(NULL),
    _last_x_position(0.0f),
    _last_y_position(0.0f),
    _current_node_x(0.0f),
    _current_node_y(0.0f),
    _current_node(0),
    _run(run)
{}

PathMoveSpriteEvent::PathMoveSpriteEvent(const std::string& event_id, VirtualSprite* sprite,
                                         float x_coord, float y_coord, bool run) :
    SpriteEvent(event_id, PATH_MOVE_SPRITE_EVENT, sprite),
    _destination_x(x_coord),
    _destination_y(y_coord),
    _target_sprite(NULL),
    _last_x_position(0.0f),
    _last_y_position(0.0f),
    _current_node_x(0.0f),
    _current_node_y(0.0f),
    _current_node(0),
    _run(run)
{}

PathMoveSpriteEvent::PathMoveSpriteEvent(const std::string& event_id, VirtualSprite* sprite,
                                         VirtualSprite* target_sprite, bool run) :
    SpriteEvent(event_id, PATH_MOVE_SPRITE_EVENT, sprite),
    _destination_x(-1.0f),
    _destination_y(-1.0f),
    _target_sprite(target_sprite),
    _last_x_position(0.0f),
    _last_y_position(0.0f),
    _current_node_x(0.0f),
    _current_node_y(0.0f),
    _current_node(0),
    _run(run)
{}

PathMoveSpriteEvent* PathMoveSpriteEvent::Create(const std::string& event_id,
                                                 VirtualSprite* sprite,
                                                 float x, float y, bool run)
{
    return new PathMoveSpriteEvent(event_id, sprite, x, y, run);
}

PathMoveSpriteEvent* PathMoveSpriteEvent::Create(const std::string& event_id,
                                                 VirtualSprite* sprite,
                                                 VirtualSprite* target_sprite, bool run)
{
    return new PathMoveSpriteEvent(event_id, sprite, target_sprite, run);
}

void PathMoveSpriteEvent::SetDestination(float x_coord, float y_coord, bool run)
{
    if(MapMode::CurrentInstance()->GetEventSupervisor()->IsEventActive(GetEventID()) == true) {
        IF_PRINT_WARNING(MAP_DEBUG) << "attempted illegal operation while event was active: "
                                    << GetEventID() << std::endl;
        return;
    }

    _destination_x = x_coord;
    _destination_y = y_coord;
    _target_sprite = NULL;
    _path.clear();
    _run = run;
}

void PathMoveSpriteEvent::SetDestination(VirtualSprite* target_sprite, bool run)
{
    if(MapMode::CurrentInstance()->GetEventSupervisor()->IsEventActive(GetEventID()) == true) {
        IF_PRINT_WARNING(MAP_DEBUG) << "attempted illegal operation while event was active: "
                                    << GetEventID() << std::endl;
        return;
    }

    _destination_x = -1.0f;
    _destination_y = -1.0f;
    _target_sprite = target_sprite;
    _path.clear();
    _run = run;
}

void PathMoveSpriteEvent::_Start()
{
    SpriteEvent::_Start();

    _current_node = 0;
    _last_x_position = _sprite->GetXPosition();
    _last_y_position = _sprite->GetYPosition();
    _sprite->is_running = _run;

    // Only set the destination at start call since the target coord may have changed
    // between the load time and the event actual start.
    if(_target_sprite) {
        _destination_x = _target_sprite->GetXPosition();
        _destination_y = _target_sprite->GetYPosition();
    }

    MapPosition dest(_destination_x, _destination_y);

    // If the sprite is at the destination, we don't have to compute anything
    if ((uint32)_sprite->GetXPosition() == (uint32)_destination_x
            && (uint32)_sprite->GetYPosition() == (uint32)_destination_y)
        return;

    _path = MapMode::CurrentInstance()->GetObjectSupervisor()->FindPath(_sprite, dest);
    if(_path.empty()) {
        PRINT_ERROR << "No path to destination (" << _destination_x
                    << ", " << _destination_y << ") for sprite: "
                    << _sprite->GetObjectID() << std::endl;
        return;
    }

    _current_node_x = _path[_current_node].x;
    _current_node_y = _path[_current_node].y;

    _sprite->moving = true;
}

bool PathMoveSpriteEvent::_Update()
{
    if(_path.empty()) {
        // No path
        Terminate();
        return true;
    }

    float sprite_position_x = _sprite->GetXPosition();
    float sprite_position_y = _sprite->GetYPosition();
    float distance_moved = _sprite->CalculateDistanceMoved();

    // Check whether the sprite has arrived at the position of the current node
    if(vt_utils::IsFloatEqual(sprite_position_x, _current_node_x, distance_moved)
            && vt_utils::IsFloatEqual(sprite_position_y, _current_node_y, distance_moved)) {
        ++_current_node;

        if(_current_node < _path.size()) {
            _current_node_x = _path[_current_node].x;
            _current_node_y = _path[_current_node].y;
        }
    }
    // If the sprite has moved to a new position other than the next node, adjust its direction so it is trying to move to the next node
    else if((_sprite->position.x != _last_x_position) || (_sprite->position.y != _last_y_position)) {
        _last_x_position = _sprite->position.x;
        _last_y_position = _sprite->position.y;
    }

    _SetSpriteDirection();

    // End the path event
    if(vt_utils::IsFloatEqual(sprite_position_x, _destination_x, distance_moved)
            && vt_utils::IsFloatEqual(sprite_position_y, _destination_y, distance_moved)) {
        Terminate();
        return true;
    }

    return false;
}

void PathMoveSpriteEvent::Terminate()
{
    _sprite->moving = false;
    SpriteEvent::Terminate();
}

void PathMoveSpriteEvent::_SetSpriteDirection()
{
    uint16 direction = 0;

    float sprite_position_x = _sprite->GetXPosition();
    float sprite_position_y = _sprite->GetYPosition();
    float distance_moved = _sprite->CalculateDistanceMoved();

    if(sprite_position_y - _current_node_y > distance_moved) {
        direction |= NORTH;
    } else if(sprite_position_y - _current_node_y < -distance_moved) {
        direction |= SOUTH;
    }

    if(sprite_position_x - _current_node_x > distance_moved) {
        direction |= WEST;
    } else if(sprite_position_x - _current_node_x < -distance_moved) {
        direction |= EAST;
    }

    // Determine if the sprite should move diagonally to the next node
    if((direction & (NORTH | SOUTH)) && (direction & (WEST | EAST))) {
        switch(direction) {
        case(NORTH | WEST):
            direction = MOVING_NORTHWEST;
            break;
        case(NORTH | EAST):
            direction = MOVING_NORTHEAST;
            break;
        case(SOUTH | WEST):
            direction = MOVING_SOUTHWEST;
            break;
        case(SOUTH | EAST):
            direction = MOVING_SOUTHEAST;
            break;
        }
    }

    _sprite->SetDirection(direction);
}

// -----------------------------------------------------------------------------
// ---------- RandomMoveSpriteEvent Class Methods
// -----------------------------------------------------------------------------

RandomMoveSpriteEvent::RandomMoveSpriteEvent(const std::string& event_id, VirtualSprite* sprite,
        uint32 move_time, uint32 direction_time) :
    SpriteEvent(event_id, RANDOM_MOVE_SPRITE_EVENT, sprite),
    _total_movement_time(move_time),
    _total_direction_time(direction_time),
    _movement_timer(0),
    _direction_timer(0)
{}

RandomMoveSpriteEvent::~RandomMoveSpriteEvent()
{}

RandomMoveSpriteEvent* RandomMoveSpriteEvent::Create(const std::string& event_id,
                                                     VirtualSprite* sprite,
                                                     uint32 move_time, uint32 direction_time)
{
    return new RandomMoveSpriteEvent(event_id, sprite, move_time, direction_time);
}

void RandomMoveSpriteEvent::_Start()
{
    SpriteEvent::_Start();
    _sprite->SetRandomDirection();
    _sprite->moving = true;
}

bool RandomMoveSpriteEvent::_Update()
{
    _direction_timer += SystemManager->GetUpdateTime();
    _movement_timer += SystemManager->GetUpdateTime();

    // Check if we should change the sprite's direction
    if(_direction_timer >= _total_direction_time) {
        _direction_timer -= _total_direction_time;
        _sprite->SetRandomDirection();
    }

    if(_movement_timer >= _total_movement_time) {
        _movement_timer = 0;
        Terminate();
        return true;
    }

    return false;
}

void RandomMoveSpriteEvent::Terminate()
{
    _sprite->moving = false;
    SpriteEvent::Terminate();
}

// -----------------------------------------------------------------------------
// ---------- AnimateSpriteEvent Class Methods
// -----------------------------------------------------------------------------

AnimateSpriteEvent::AnimateSpriteEvent(const std::string &event_id, VirtualSprite *sprite,
                                       const std::string &animation_name, int32 animation_time) :
    SpriteEvent(event_id, ANIMATE_SPRITE_EVENT, sprite),
    _animation_name(animation_name),
    _animation_time(animation_time)
{
    _map_sprite = dynamic_cast<MapSprite *>(_sprite);
}

AnimateSpriteEvent* AnimateSpriteEvent::Create(const std::string& event_id,
                                               VirtualSprite* sprite,
                                               const std::string& animation_name,
                                               int32 animation_time)
{
    return new AnimateSpriteEvent(event_id, sprite, animation_name, animation_time);
}

void AnimateSpriteEvent::_Start()
{
    SpriteEvent::_Start();

    if(_map_sprite)
        _map_sprite->SetCustomAnimation(_animation_name, _animation_time);
}

bool AnimateSpriteEvent::_Update()
{
    if(!_map_sprite || !_map_sprite->IsAnimationCustom()) {
        Terminate();
        return true;
    }
    return false;
}

void AnimateSpriteEvent::Terminate()
{
    // Disable a possible still running custom animation.
    // Useful when calling EndAllEvents() on a sprite.
    if (_map_sprite)
        _map_sprite->DisableCustomAnimation();
    _map_sprite = 0;
    _animation_name.clear();
    _animation_time = 0;
    SpriteEvent::Terminate();
}

// -----------------------------------------------------------------------------
// ---------- TreasureEvent Class Methods
// -----------------------------------------------------------------------------

TreasureEvent::TreasureEvent(const std::string& event_id) :
    MapEvent(event_id, TREASURE_EVENT)
{
    _treasure = new MapTreasure();
}

TreasureEvent* TreasureEvent::Create(const std::string& event_id)
{
    return new TreasureEvent(event_id);
}

void TreasureEvent::_Start()
{
    if (_events.size() > 0) {
        for (uint32 i = 0; i < _events.size(); ++i) {
            MapMode::CurrentInstance()->GetEventSupervisor()->StartEvent(_events[i]);
        }
    }

    MapMode::CurrentInstance()->GetTreasureSupervisor()->Initialize(_treasure);
}

bool TreasureEvent::_Update()
{
    // If the treasure supervisor has finished, we can proceed
    if(MapMode::CurrentInstance()->CurrentState() != STATE_TREASURE)
        return true;
    return false;
}

bool TreasureEvent::AddItem(uint32 id, uint32 quantity)
{
    return _treasure->AddItem(id, quantity);
}

void TreasureEvent::AddEvent(const std::string& event_id)
{
    if (!event_id.empty())
        _events.push_back(event_id);
}

// -----------------------------------------------------------------------------
// ---------- EventSupervisor Class Methods
// -----------------------------------------------------------------------------

EventSupervisor::~EventSupervisor()
{
    _active_events.clear();
    _paused_events.clear();
    _active_delayed_events.clear();
    _paused_delayed_events.clear();

    for(std::map<std::string, MapEvent *>::iterator it = _all_events.begin(); it != _all_events.end(); ++it) {
        delete it->second;
    }
    _all_events.clear();
}

void EventSupervisor::StartEvent(const std::string &event_id)
{
    MapEvent *event = GetEvent(event_id);
    if(event == NULL) {
        PRINT_WARNING << "No event with this ID existed: '" << event_id
            << "' in map script: "
            << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    StartEvent(event);
}

void EventSupervisor::StartEvent(const std::string &event_id, uint32 launch_time)
{
    MapEvent *event = GetEvent(event_id);
    if(event == NULL) {
        PRINT_WARNING << "No event with this ID existed: '" << event_id
            << "' in map script: "
            << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    if(launch_time == 0)
        StartEvent(event);
    else
        _active_delayed_events.push_back(std::make_pair(static_cast<int32>(launch_time), event));
}

void EventSupervisor::StartEvent(MapEvent *event, uint32 launch_time)
{
    if(event == NULL) {
        IF_PRINT_WARNING(MAP_DEBUG) << "NULL argument passed to function"
                                    << std::endl;
        return;
    }

    if(launch_time == 0)
        StartEvent(event);
    else
        _active_delayed_events.push_back(std::make_pair(static_cast<int32>(launch_time), event));
}

void EventSupervisor::StartEvent(MapEvent *event)
{
    if(!event) {
        PRINT_WARNING << "NULL argument passed to function" << std::endl;
        return;
    }

    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to start the event: '" << event->GetEventID()
                      << "' within an update function. The StartEvent() call will be ignored. "
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    for(std::vector<MapEvent *>::iterator it = _active_events.begin(); it != _active_events.end(); ++it) {
        if((*it) == event) {
            PRINT_WARNING << "The event: '" << event->GetEventID()
                          << "' is already active and can be active only once at a time. "
                          << "The StartEvent() call will be ignored."
                          << std::endl << " You should fix the map script: "
                          << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
            return;
        }
    }

    _active_events.push_back(event);
    event->_Start();
    _ExamineEventLinks(event, true);
}

void EventSupervisor::PauseEvent(const std::string &event_id)
{
    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to pause the event: '" << event_id
                      << "' within an update function. The PauseEvent() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    // Search for active ones
    for(std::vector<MapEvent *>::iterator it = _active_events.begin(); it != _active_events.end();) {
        if((*it)->_event_id == event_id) {
            _paused_events.push_back(*it);
            it = _active_events.erase(it);
        } else {
            ++it;
        }
    }

    // and for the delayed ones
    for(std::vector<std::pair<int32, MapEvent *> >::iterator it = _active_delayed_events.begin();
            it != _active_delayed_events.end();) {
        if((*it).second->_event_id == event_id) {
            _paused_delayed_events.push_back(*it);
            it = _active_delayed_events.erase(it);
        } else {
            ++it;
        }
    }
}

void EventSupervisor::PauseAllEvents(VirtualSprite *sprite)
{
    if(!sprite)
        return;
    // Examine all potential active (now or later) events

    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to pause all events for sprite: " << sprite->GetObjectID()
                      << " within an update function. The PauseAllEvents() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    // Starting by active ones.
    for(std::vector<MapEvent *>::iterator it = _active_events.begin(); it != _active_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>(*it);
        if(event && event->GetSprite() == sprite) {
            _paused_events.push_back(*it);
            it = _active_events.erase(it);
        } else {
            ++it;
        }
    }

    // Looking at incoming ones.
    for(std::vector<std::pair<int32, MapEvent *> >::iterator it = _active_delayed_events.begin();
            it != _active_delayed_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>((*it).second);
        if(event && event->GetSprite() == sprite) {
            _paused_delayed_events.push_back(*it);
            it = _active_delayed_events.erase(it);
        } else {
            ++it;
        }
    }
}

void EventSupervisor::ResumeEvent(const std::string &event_id)
{
    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to resume event: '" << event_id
                      << "' within an update function. The ResumeEvent() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    for(std::vector<MapEvent *>::iterator it = _paused_events.begin();
            it != _paused_events.end();) {
        if((*it)->_event_id == event_id) {
            _active_events.push_back(*it);
            it = _paused_events.erase(it);
        } else {
            ++it;
        }
    }

    // and the delayed ones
    for(std::vector<std::pair<int32, MapEvent *> >::iterator it = _paused_delayed_events.begin();
            it != _paused_delayed_events.end();) {
        if((*it).second->_event_id == event_id) {
            _active_delayed_events.push_back(*it);
            it = _paused_delayed_events.erase(it);
        } else {
            ++it;
        }
    }
}

void EventSupervisor::ResumeAllEvents(VirtualSprite *sprite)
{
    if(!sprite)
        return;
    // Examine all potential active (now or later) events

    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to resume all events for sprite: " << sprite->GetObjectID()
                      << " within an update function. The EndAllEvents() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    // Starting by active ones.
    for(std::vector<MapEvent *>::iterator it = _paused_events.begin(); it != _paused_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>(*it);
        if(event && event->GetSprite() == sprite) {
            _active_events.push_back(*it);
            it = _paused_events.erase(it);
        } else {
            ++it;
        }
    }

    // Looking at incoming ones.
    for(std::vector<std::pair<int32, MapEvent *> >::iterator it = _paused_delayed_events.begin();
            it != _paused_delayed_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>((*it).second);
        if(event && event->GetSprite() == sprite) {
            _active_delayed_events.push_back(*it);
            it = _paused_delayed_events.erase(it);
        } else {
            ++it;
        }
    }
}

void EventSupervisor::EndEvent(const std::string &event_id, bool trigger_event_links)
{
    // Examine all potential active (now or later) events

    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to terminate the event: '" << event_id
                      << "' within an update function. The EndEvent() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    // Starting by active ones.
    for(std::vector<MapEvent *>::iterator it = _active_events.begin(); it != _active_events.end();) {
        if((*it)->_event_id == event_id) {
            SpriteEvent *sprite_event = dynamic_cast<SpriteEvent *>(*it);
            // Terminated sprite events need to release their owned sprite.
            if(sprite_event)
                sprite_event->Terminate();

            MapEvent *terminated_event = *it;
            it = _active_events.erase(it);
            // We examine the event links only after the event has been removed from the active list
            if(trigger_event_links)
                _ExamineEventLinks(terminated_event, false);
        } else {
            ++it;
        }
    }

    // Looking at incoming ones.
    for(std::vector<std::pair<int32, MapEvent *> >::iterator it = _active_delayed_events.begin();
            it != _active_delayed_events.end();) {
        if((*it).second->_event_id == event_id) {
            MapEvent *terminated_event = (*it).second;
            it = _active_delayed_events.erase(it);

            // We examine the event links only after the event has been removed from the active list
            if(trigger_event_links)
                _ExamineEventLinks(terminated_event, false);
        } else {
            ++it;
        }
    }

    // And paused ones
    for(std::vector<MapEvent *>::iterator it = _paused_events.begin(); it != _paused_events.end();) {
        if((*it)->_event_id == event_id) {
            SpriteEvent *sprite_event = dynamic_cast<SpriteEvent *>(*it);
            // Paused sprite events need to release their owned sprite as they have been previously started.
            if(sprite_event)
                sprite_event->Terminate();

            MapEvent *terminated_event = *it;
            it = _paused_events.erase(it);
            // We examine the event links only after the event has been removed from the list
            if(trigger_event_links)
                _ExamineEventLinks(terminated_event, false);
        } else {
            ++it;
        }
    }

    for(std::vector<std::pair<int32, MapEvent *> >::iterator it = _paused_delayed_events.begin();
            it != _paused_delayed_events.end();) {
        if((*it).second->_event_id == event_id) {
            MapEvent *terminated_event = (*it).second;
            it = _paused_delayed_events.erase(it);

            // We examine the event links only after the event has been removed from the list
            if(trigger_event_links)
                _ExamineEventLinks(terminated_event, false);
        } else {
            ++it;
        }
    }
}

void EventSupervisor::EndEvent(MapEvent *event, bool trigger_event_links)
{
    if(!event) {
        PRINT_ERROR << "Couldn't terminate NULL event" << std::endl;
        return;
    }

    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to terminate the event: '" << event->GetEventID()
                      << "' within an update function. The EndEvent() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    EndEvent(event->GetEventID(), trigger_event_links);
}

void EventSupervisor::EndAllEvents(VirtualSprite *sprite)
{
    if(!sprite)
        return;
    // Examine all potential active (now or later) events

    // Never ever do that when updating events.
    if(_is_updating) {
        PRINT_WARNING << "Tried to terminate all events for sprite: " << sprite->GetObjectID()
                      << " within an update function. The EndAllEvents() call will be ignored."
                      << std::endl << " You should fix the map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return;
    }

    // Starting by active ones.
    for(std::vector<MapEvent *>::iterator it = _active_events.begin(); it != _active_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>(*it);
        if(event && event->GetSprite() == sprite) {
            // Active events need to release their owned sprite upon termination.
            event->Terminate();

            it = _active_events.erase(it);
        } else {
            ++it;
        }
    }

    // Looking at incoming ones.
    for(std::vector<std::pair<int32, MapEvent *> >::iterator it = _active_delayed_events.begin();
            it != _active_delayed_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>((*it).second);
        if(event && event->GetSprite() == sprite)
            it = _active_delayed_events.erase(it);
        else
            ++it;
    }


    // And paused ones
    for(std::vector<MapEvent *>::iterator it = _paused_events.begin(); it != _paused_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>(*it);
        if(event && event->GetSprite() == sprite) {
            // Paused events have been started, so they might need to release their owned sprite.
            event->Terminate();

            it = _paused_events.erase(it);
        } else {
            ++it;
        }
    }

    for(std::vector<std::pair<int32, MapEvent *> >::iterator it = _paused_delayed_events.begin();
            it != _paused_delayed_events.end();) {
        SpriteEvent *event = dynamic_cast<SpriteEvent *>((*it).second);
        if(event && event->GetSprite() == sprite)
            it = _paused_delayed_events.erase(it);
        else
            ++it;
    }
}

void EventSupervisor::Update()
{
    // Store the events that became active in the delayed event loop.
    std::vector<MapEvent *> events_to_start;

    // Update all launch event timers and start all events whose timers have finished
    for(std::vector<std::pair<int32, MapEvent *> >::iterator it = _active_delayed_events.begin();
            it != _active_delayed_events.end();) {
        it->first -= SystemManager->GetUpdateTime();

        if(it->first <= 0) {  // Timer has expired
            MapEvent *start_event = it->second;
            it = _active_delayed_events.erase(it);

            // We add the event ready to start i a vector, waiting for the loop to end
            // before starting it.
            events_to_start.push_back(start_event);
        } else {
            ++it;
        }
    }

    // Starts the events that became active.
    for(std::vector<MapEvent *>::iterator it = events_to_start.begin(); it != events_to_start.end(); ++it)
        StartEvent(*it);

    // Store the events that ended within the update loop.
    std::vector<MapEvent *> finished_events;

    // Make the engine aware that the event supervisor is entering the event update loop
    _is_updating = true;

    // Check for active events which have finished
    for(std::vector<MapEvent *>::iterator it = _active_events.begin(); it != _active_events.end();) {
        if((*it)->_Update() == true) {
            // Add it ot the finished events list
            finished_events.push_back(*it);

            // Remove the finished event from the active queue.
            it = _active_events.erase(it);
        } else {
            ++it;
        }
    }

    _is_updating = false;

    // We examine the event links only after the events has been removed from the active list
    // and the active list has finished parsing, to avoid a crash when adding a new event within the update loop.
    for(std::vector<MapEvent *>::iterator it = finished_events.begin(); it != finished_events.end(); ++it) {
        _ExamineEventLinks(*it, false);
    }
}

bool EventSupervisor::IsEventActive(const std::string &event_id) const
{
    for(std::vector<MapEvent *>::const_iterator it = _active_events.begin(); it != _active_events.end(); ++it) {
        if((*it)->_event_id == event_id) {
            return true;
        }
    }
    return false;
}

MapEvent *EventSupervisor::GetEvent(const std::string &event_id) const
{
    std::map<std::string, MapEvent *>::const_iterator it = _all_events.find(event_id);

    if(it == _all_events.end())
        return NULL;
    else
        return it->second;
}

bool EventSupervisor::_RegisterEvent(MapEvent* new_event)
{
    if(new_event == NULL) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function argument was NULL" << std::endl;
        return false;
    }

    if(GetEvent(new_event->_event_id) != NULL) {
        PRINT_WARNING << "The event with this ID already existed: '"
                      << new_event->_event_id
                      << "' in map script: "
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
        return false;
    }

    _all_events.insert(std::make_pair(new_event->_event_id, new_event));
    return true;
}

void EventSupervisor::_ExamineEventLinks(MapEvent *parent_event, bool event_start)
{
    for(uint32 i = 0; i < parent_event->_event_links.size(); ++i) {
        EventLink &link = parent_event->_event_links[i];

        // Case 1: Start/finish launch member is not equal to the start/finish status of the parent event, so ignore this link
        if(link.launch_at_start != event_start) {
            continue;
        }
        // Case 2: The child event is to be launched immediately
        else if(link.launch_timer == 0) {
            StartEvent(link.child_event_id);
        }
        // Case 3: The child event has a timer associated with it and needs to be placed in the event launch container
        else {
            MapEvent *child = GetEvent(link.child_event_id);
            if(child == NULL) {
                PRINT_WARNING << "Couldn't launch child event, no event with this ID existed: '"
                              << link.child_event_id << "' from parent event ID: '"
                              << parent_event->GetEventID()
                              << "' in map script: "
                              << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
                continue;
            } else {
                _active_delayed_events.push_back(std::make_pair(static_cast<int32>(link.launch_timer), child));
            }
        }
    }
}

} // namespace private_map

} // namespace vt_map
