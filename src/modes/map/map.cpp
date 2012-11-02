///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for map mode interface.
*** ***************************************************************************/

#include "modes/map/map.h"

#include "modes/map/map_dialogue.h"
#include "modes/map/map_events.h"
#include "modes/map/map_objects.h"
#include "modes/map/map_sprites.h"
#include "modes/map/map_tiles.h"

#include "modes/menu/menu.h"
#include "modes/pause.h"
#include "modes/boot/boot.h"
#include "modes/save/save_mode.h"

#include "engine/audio/audio.h"
#include "engine/input.h"

#include "common/global/global.h"

using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_boot;
using namespace hoa_input;
using namespace hoa_mode_manager;
using namespace hoa_script;
using namespace hoa_system;
using namespace hoa_video;
using namespace hoa_global;
using namespace hoa_menu;
using namespace hoa_pause;
using namespace hoa_save;
using namespace hoa_map::private_map;

namespace hoa_map
{

// Initialize static class variables
MapMode *MapMode::_current_instance = NULL;

// ****************************************************************************
// ********** MapMode Public Class Methods
// ****************************************************************************

MapMode::MapMode(const std::string &filename) :
    GameMode(),
    _map_filename(filename),
    _map_tablespace(""),
    _tile_supervisor(NULL),
    _object_supervisor(NULL),
    _event_supervisor(NULL),
    _dialogue_supervisor(NULL),
    _treasure_supervisor(NULL),
    _camera_x_in_map_corner(false),
    _camera_y_in_map_corner(false),
    _camera(NULL),
    _delta_x(0),
    _delta_y(0),
    _num_map_contexts(0),
    _running_disabled(false),
    _unlimited_stamina(false),
    _show_gui(true),
    _run_stamina(10000)
{
    mode_type = MODE_MANAGER_MAP_MODE;
    _current_instance = this;

    ResetState();
    PushState(STATE_EXPLORE);

    // Load the save point animation files.
    AnimatedImage anim;
    anim.LoadFromAnimationScript("img/misc/save_point/save_point4.lua");
    active_save_point_animations.push_back(anim);

    anim.Clear();
    anim.LoadFromAnimationScript("img/misc/save_point/save_point3.lua");
    active_save_point_animations.push_back(anim);

    anim.Clear();
    anim.LoadFromAnimationScript("img/misc/save_point/save_point2.lua");
    active_save_point_animations.push_back(anim);

    anim.Clear();
    anim.LoadFromAnimationScript("img/misc/save_point/save_point1.lua");
    inactive_save_point_animations.push_back(anim);

    anim.Clear();
    anim.LoadFromAnimationScript("img/misc/save_point/save_point2.lua");
    inactive_save_point_animations.push_back(anim);

    // Transform the animation size to correspond to the map coodinates system.
    for(uint32 i = 0; i < active_save_point_animations.size(); ++i)
        ScaleToMapCoords(active_save_point_animations[i]);

    for(uint32 i = 0; i < inactive_save_point_animations.size(); ++i)
        ScaleToMapCoords(inactive_save_point_animations[i]);

    _tile_supervisor = new TileSupervisor();
    _object_supervisor = new ObjectSupervisor();
    _event_supervisor = new EventSupervisor();
    _dialogue_supervisor = new DialogueSupervisor();
    _treasure_supervisor = new TreasureSupervisor();

    _intro_timer.Initialize(4000, 0);
    _intro_timer.EnableAutoUpdate(this);

    _camera_timer.Initialize(0, 1);

    if(!_Load()) {
        PRINT_ERROR << "Couldn't load the map file: " << _map_filename << std::endl
                    << "Returning to boot mode. You should report this error." << std::endl;
        BootMode *BM = new BootMode();
        ModeManager->PopAll();
        ModeManager->Push(BM);
        return;
    }

    // Load miscellaneous map graphics
    _dialogue_icon.LoadFromAnimationScript("img/misc/dialogue_icon.lua");
    ScaleToMapCoords(_dialogue_icon);

    if(!_stamina_bar_background.Load("img/misc/stamina_bar_background.png", 227, 24))
        IF_PRINT_WARNING(MAP_DEBUG) << "failed to load the the stamina bar background image" << std::endl;

    if(!_stamina_bar_infinite_overlay.Load("img/misc/stamina_bar_infinite_overlay.png", 227, 24))
        IF_PRINT_WARNING(MAP_DEBUG) << "failed to load the the stamina bar infinite overlay image" << std::endl;

    // Init the script component.
    GetScriptSupervisor().Initialize(this);
}



MapMode::~MapMode()
{
    for(uint32 i = 0; i < _enemies.size(); ++i)
        delete(_enemies[i]);
    _enemies.clear();

    delete(_tile_supervisor);
    delete(_object_supervisor);
    delete(_event_supervisor);
    delete(_dialogue_supervisor);
    delete(_treasure_supervisor);
}



void MapMode::Reset()
{
    // Reset video engine context properties
    VideoManager->SetCoordSys(0.0f, SCREEN_GRID_X_LENGTH, SCREEN_GRID_Y_LENGTH, 0.0f);
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);

    // Set the active instance pointer to this map
    MapMode::_current_instance = this;

    // Make the map location known globally to other code that may need to know this information
    GlobalManager->SetMap(_map_filename, _map_image.GetFilename(), _map_hud_name);

    // Only replace a different previous music.
    MusicDescriptor *music = AudioManager->RetrieveMusic(_music_filename);
    if(music && music->GetState() != AUDIO_STATE_PLAYING) {
        // In case the music volume was modified, we fade it back in smoothly
        if(music->GetVolume() < 1.0f)
            music->FadeIn(1000);
        else
            music->Play();
    }

    _intro_timer.Run();

    // Reset potential map scripts
    GetScriptSupervisor().Reset();

    // If the sprite is based on the battle formation, we'd better refresh the data now
    // if the game in in the default mode.
    // Still, we won't change the sprite in the middle of a scene for instance.
    // I.e: When going out of the menu mode.
    if(CurrentState() == private_map::STATE_EXPLORE)
        _object_supervisor->ReloadVisiblePartyMember();
}



void MapMode::Update()
{
    // Update the map frame coords
    // NOTE: It's done before handling pause so that the frame is updated at
    // least once before setting the pause mode, avoiding a crash.
    _UpdateMapFrame();

    // Process quit and pause events unconditional to the state of map mode
    if(InputManager->QuitPress()) {
        ModeManager->Push(new PauseMode(true));
        return;
    } else if(InputManager->PausePress()) {
        ModeManager->Push(new PauseMode(false));
        return;
    }

    _dialogue_icon.Update();

    // Call the map script's update function
    if(_update_function->is_valid())
        ScriptCallFunction<void>(*_update_function);

    // Update all animated tile images
    _tile_supervisor->Update();
    _object_supervisor->Update();
    _object_supervisor->SortObjects();

    // Update the active state of the map
    switch(CurrentState()) {
    case STATE_EXPLORE:
        _UpdateExplore();
        break;
    case STATE_SCENE:
        break;
    case STATE_DIALOGUE:
        _dialogue_supervisor->Update();
        break;
    case STATE_TREASURE:
        _camera->moving = false;
        _treasure_supervisor->Update();
        break;
    default:
        IF_PRINT_WARNING(MAP_DEBUG) << "map was set in an unknown state: "
                                    << CurrentState() << std::endl;
        ResetState();
        break;
    }

    // ---------- (4) Update the camera timer
    _camera_timer.Update();

    // ---------- (5) Update all active map events
    _event_supervisor->Update();

    GameMode::Update();
} // void MapMode::Update()



void MapMode::Draw()
{
    VideoManager->SetStandardCoordSys();
    GetScriptSupervisor().DrawBackground();

    VideoManager->SetCoordSys(0.0f, SCREEN_GRID_X_LENGTH, SCREEN_GRID_Y_LENGTH, 0.0f);
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
    if(_draw_function->is_valid())
        ScriptCallFunction<void>(*_draw_function);
    else
        _DrawMapLayers();

    VideoManager->SetStandardCoordSys();
    GetScriptSupervisor().DrawForeground();

    VideoManager->SetCoordSys(0.0f, SCREEN_GRID_X_LENGTH, SCREEN_GRID_Y_LENGTH, 0.0f);
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
    _object_supervisor->DrawDialogIcons();
}

void MapMode::DrawPostEffects()
{
    VideoManager->SetCoordSys(0.0f, SCREEN_GRID_X_LENGTH, SCREEN_GRID_Y_LENGTH, 0.0f);
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);
    // Halos are additive blending made, so they should be applied
    // as post-effects but before the GUI.
    _object_supervisor->DrawLights();

    VideoManager->SetStandardCoordSys();
    GetScriptSupervisor().DrawPostEffects();

    VideoManager->SetCoordSys(0.0f, SCREEN_GRID_X_LENGTH, SCREEN_GRID_Y_LENGTH, 0.0f);
    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);

    // Draw the gui, unaffected by potential
    // fading effects.
    _DrawGUI();
    if(CurrentState() == STATE_DIALOGUE) {
        _dialogue_supervisor->Draw();
    }

    // Draw the treasure menu if necessary
    if(CurrentState() == STATE_TREASURE)
        _treasure_supervisor->Draw();
}



void MapMode::ResetState()
{
    _state_stack.clear();
    _state_stack.push_back(STATE_INVALID);
}



void MapMode::PushState(MAP_STATE state)
{
    _state_stack.push_back(state);
}



void MapMode::PopState()
{
    _state_stack.pop_back();
    if(_state_stack.empty() == true) {
        IF_PRINT_WARNING(MAP_DEBUG)
                << "stack was empty after operation, reseting state stack"
                << std::endl;
        _state_stack.push_back(STATE_INVALID);
    }
}



MAP_STATE MapMode::CurrentState()
{
    if(_state_stack.empty() == true) {
        IF_PRINT_WARNING(MAP_DEBUG) << "stack was empty, reseting state stack"
                                    << std::endl;
        _state_stack.push_back(STATE_INVALID);
    }
    return _state_stack.back();
}

void MapMode::AddFlatGroundObject(MapObject *obj)
{
    if(!obj) {
        PRINT_WARNING << "Couldn't add NULL object." << std::endl;
        return;
    }
    _object_supervisor->_flat_ground_objects.push_back(obj);
    _object_supervisor->_all_objects.insert(std::make_pair(obj->object_id, obj));
}

void MapMode::AddGroundObject(MapObject *obj)
{
    if(!obj) {
        PRINT_WARNING << "Couldn't add NULL object." << std::endl;
        return;
    }
    _object_supervisor->_ground_objects.push_back(obj);
    _object_supervisor->_all_objects.insert(std::make_pair(obj->object_id, obj));
}


void MapMode::AddPassObject(MapObject *obj)
{
    if(!obj) {
        PRINT_WARNING << "Couldn't add NULL object." << std::endl;
        return;
    }
    _object_supervisor->_pass_objects.push_back(obj);
    _object_supervisor->_all_objects.insert(std::make_pair(obj->object_id, obj));
}


void MapMode::AddSkyObject(MapObject *obj)
{
    if(!obj) {
        PRINT_WARNING << "Couldn't add NULL object." << std::endl;
        return;
    }
    _object_supervisor->_sky_objects.push_back(obj);
    _object_supervisor->_all_objects.insert(std::make_pair(obj->object_id, obj));
}


void MapMode::AddZone(MapZone *zone)
{
    if(!zone) {
        PRINT_WARNING << "Couldn't add NULL zone." << std::endl;
        return;
    }
    _object_supervisor->_zones.push_back(zone);
}


void MapMode::AddSavePoint(float x, float y, MAP_CONTEXT map_context)
{
    SavePoint *save_point = new SavePoint(x, y, map_context);
    _object_supervisor->_save_points.push_back(save_point);
}

void MapMode::AddHalo(const std::string &filename, float x, float y,
                      const Color &color, MAP_CONTEXT map_context)
{
    Halo *halo = new Halo(filename, x, y, color, map_context);
    _object_supervisor->_halos.push_back(halo);
}

void MapMode::AddLight(const std::string &main_flare_filename,
                       const std::string &secondary_flare_filename,
                       float x, float y,
                       const Color &main_color,
                       const Color &secondary_color,
                       MAP_CONTEXT map_context)
{
    Light *light = new Light(main_flare_filename,
                             secondary_flare_filename,
                             x, y, main_color,
                             secondary_color,
                             map_context);
    _object_supervisor->_lights.push_back(light);
}

bool MapMode::IsEnemyLoaded(uint32 id) const
{
    for(uint32 i = 0; i < _enemies.size(); i++) {
        if(_enemies[i]->GetID() == id) {
            return true;
        }
    }
    return false;
}

void MapMode::SetCamera(private_map::VirtualSprite *sprite, uint32 duration)
{
    if(_camera == sprite) {
        IF_PRINT_WARNING(MAP_DEBUG) << "Camera was moved to the same sprite"
                                    << std::endl;
    } else {
        if(duration > 0) {
            _delta_x = _camera->GetXPosition() - sprite->GetXPosition();
            _delta_y = _camera->GetYPosition() - sprite->GetYPosition();
            _camera_timer.Reset();
            _camera_timer.SetDuration(duration);
            _camera_timer.Run();
        }
        _camera = sprite;
    }
}

void MapMode::MoveVirtualFocus(float loc_x, float loc_y)
{
    _object_supervisor->VirtualFocus()->SetPosition(loc_x, loc_y);
}

void MapMode::MoveVirtualFocus(float loc_x, float loc_y, uint32 duration)
{
    if(_camera != _object_supervisor->VirtualFocus()) {
        IF_PRINT_WARNING(MAP_DEBUG)
                << "Attempt to move camera although on different sprite" << std::endl;
    } else {
        if(duration > 0) {
            _delta_x = _object_supervisor->VirtualFocus()->GetXPosition() - static_cast<float>(loc_x);
            _delta_y = _object_supervisor->VirtualFocus()->GetYPosition() - static_cast<float>(loc_y);
            _camera_timer.Reset();
            _camera_timer.SetDuration(duration);
            _camera_timer.Run();
        }
        MoveVirtualFocus(loc_x, loc_y);
    }
}


bool MapMode::IsCameraOnVirtualFocus()
{
    return _camera == _object_supervisor->VirtualFocus();
}


MAP_CONTEXT MapMode::GetCurrentContext() const
{
    if(_camera)
        return _camera->GetContext();
    return MAP_CONTEXT_01;
}


bool MapMode::AttackAllowed()
{
    return (CurrentState() != STATE_DIALOGUE && CurrentState() != STATE_TREASURE && !IsCameraOnVirtualFocus());
}

// ****************************************************************************
// ********** MapMode Private Class Methods
// ****************************************************************************

bool MapMode::_Load()
{
    // ---------- (1) Open map script file and read in the basic map properties and tile definitions
    if(!_map_script.OpenFile(_map_filename)) {
        PRINT_ERROR << "Couldn't open map script file: "
                    << _map_filename << std::endl;
        return false;
    }

    // The map tablespace is needed later by scripted events.
    _map_tablespace = _map_script.OpenTablespace();

    if(_map_tablespace.empty()) {
        PRINT_ERROR << "Couldn't open map name space: "
                    << _map_filename << std::endl;
        return false;
    }

    // Read the name of the map, and load the location graphic image
    _map_hud_name = MakeUnicodeString(_map_script.ReadString("map_name"));
    _map_hud_subname = MakeUnicodeString(_map_script.ReadString("map_subname"));
    std::string map_filename = _map_script.ReadString("map_image_filename");
    if(!map_filename.empty() && !_map_image.Load(_map_script.ReadString("map_image_filename")))
        PRINT_ERROR << "Failed to load location graphic image: "
                    << _map_image.GetFilename() << std::endl;

    // Instruct the supervisor classes to perform their portion of the load operation
    if(!_tile_supervisor->Load(_map_script)) {
        PRINT_ERROR << "Failed to load the tile data." << std::endl;
        return false;
    }

    // NOTE: The object supervisor will complain itself about the error.
    if(!_object_supervisor->Load(_map_script))
        return false;

    // Load map default music
    // NOTE: Other audio handling will be handled through scripting
    _music_filename = _map_script.ReadString("music_filename");
    if(!AudioManager->LoadMusic(_music_filename, this))
        PRINT_WARNING << "Failed to load map music: " << _music_filename << std::endl;


    // Create and store all enemies that may appear on this map
    std::vector<int32> enemy_ids;
    _map_script.ReadIntVector("enemy_ids", enemy_ids);
    for(uint32 i = 0; i < enemy_ids.size(); i++) {
        _enemies.push_back(new GlobalEnemy(enemy_ids[i]));
    }

    // Call the map script's custom load function and get a reference to all other script function pointers
    // We use a newly allocated pointer to avoid a memory corruption due to luabind's garbage collector.
    ScriptObject *map_table = new ScriptObject(luabind::from_stack(_map_script.GetLuaState(), hoa_script::private_script::STACK_TOP));
    ScriptObject *function = new ScriptObject();
    *function = (*map_table)["Load"];

    bool loading_succeeded = true;
    if(function->is_valid()) {
        try {
            ScriptCallFunction<void>(*function, this);
        } catch(const luabind::error &e) {
            ScriptManager->HandleLuaError(e);
            loading_succeeded = false;
        } catch(const luabind::cast_failed &e) {
            ScriptManager->HandleCastError(e);
            loading_succeeded = false;
        }
    } else {
        loading_succeeded = false;
    }

    if(!loading_succeeded) {
        PRINT_ERROR << "Invalid map Load() function. The function wasn't called."
                    << std::endl;
        _map_script.CloseAllTables();
        _map_script.CloseFile();
        return false;
    }

    _update_function = new ScriptObject();
    *_update_function = _map_script.ReadFunctionPointer("Update");
    _draw_function = new ScriptObject();
    *_draw_function = _map_script.ReadFunctionPointer("Draw");

    // ---------- (6) Prepare all sprites with dialogue
    // This is done at this stage because the map script's load function creates the sprite and dialogue objects. Only after
    // both sets are created can we determine which sprites have active dialogue.

    // TODO: Need to figure out a new function appropriate for this code?
    // TEMP: The line below is very bad to do, but is necessary for the UpdateDialogueStatus function to work correctly
    _current_instance = this;
    for(std::map<uint16, MapObject *>::iterator it = _object_supervisor->_all_objects.begin();
            it != _object_supervisor->_all_objects.end(); ++it) {
        if(it->second->GetType() == SPRITE_TYPE) {
            MapSprite *sprite = dynamic_cast<MapSprite *>(it->second);
            sprite->UpdateDialogueStatus();
        }
    }

    _map_script.CloseAllTables();
    _map_script.CloseFile(); // Free the map file once everyhting is loaded

    return true;
} // bool MapMode::_Load()



void MapMode::_UpdateExplore()
{
    // First go to menu mode if the user requested it
    if(InputManager->MenuPress()) {
        MenuMode *MM = new MenuMode(_map_hud_name, _map_image.GetFilename());
        ModeManager->Push(MM);
        return;
    }

    // Update the running state of the camera object. Check if the character is running and if so,
    // update the stamina value if the operation is permitted
    _camera->is_running = false;
    if(_camera->moved_position && !_running_disabled && InputManager->CancelState() &&
            (InputManager->UpState() || InputManager->DownState() || InputManager->LeftState() || InputManager->RightState())) {
        if(_unlimited_stamina) {
            _camera->is_running = true;
        } else if(_run_stamina > SystemManager->GetUpdateTime() * 2) {
            _run_stamina -= (SystemManager->GetUpdateTime() * 2);
            _camera->is_running = true;
        } else {
            _run_stamina = 0;
        }
    }
    // Regenerate the stamina at 1/2 the consumption rate
    else if(_run_stamina < 10000) {
        _run_stamina += SystemManager->GetUpdateTime();
        if(_run_stamina > 10000)
            _run_stamina = 10000;
    }

    // If the user requested a confirm event, check if there is a nearby object that the player may interact with
    // Interactions are currently limited to dialogue with sprites and opening of treasures
    if(InputManager->ConfirmPress()) {
        MapObject *obj = _object_supervisor->FindNearestInteractionObject(_camera);

        if(obj != NULL) {
            if(obj->GetType() == SPRITE_TYPE) {
                MapSprite *sp = reinterpret_cast<MapSprite *>(obj);

                if(sp->HasAvailableDialogue()) {
                    _camera->moving = false;
                    _camera->is_running = false;
                    sp->InitiateDialogue();
                    return;
                }
            } else if(obj->GetType() == TREASURE_TYPE) {
                TreasureObject *treasure_object = reinterpret_cast<TreasureObject *>(obj);

                if(!treasure_object->GetTreasure()->IsTaken()) {
                    _camera->moving = false;
                    treasure_object->Open();
                }
            } else if(obj->GetType() == SAVE_TYPE) {
                SaveMode *save_mode = new SaveMode(true, obj->GetXPosition(), obj->GetYPosition());
                ModeManager->Push(save_mode, false, false);
            }
        }
    }

    // Detect movement input from the user
    if(InputManager->UpState() || InputManager->DownState() || InputManager->LeftState() || InputManager->RightState()) {
        _camera->moving = true;
    } else {
        _camera->moving = false;
    }

    // Determine the direction of movement. Priority of movement is given to: up, down, left, right.
    // In the case of diagonal movement, the direction that the sprite should face also needs to be deduced.
    if(_camera->moving == true) {
        if(InputManager->UpState()) {
            if(InputManager->LeftState())
                _camera->SetDirection(MOVING_NORTHWEST);
            else if(InputManager->RightState())
                _camera->SetDirection(MOVING_NORTHEAST);
            else
                _camera->SetDirection(NORTH);
        } else if(InputManager->DownState()) {
            if(InputManager->LeftState())
                _camera->SetDirection(MOVING_SOUTHWEST);
            else if(InputManager->RightState())
                _camera->SetDirection(MOVING_SOUTHEAST);
            else
                _camera->SetDirection(SOUTH);
        } else if(InputManager->LeftState()) {
            _camera->SetDirection(WEST);
        } else if(InputManager->RightState()) {
            _camera->SetDirection(EAST);
        }
    } // if (_camera->moving == true)
} // void MapMode::_UpdateExplore()



void MapMode::_UpdateMapFrame()
{
    // Reinit map corner check members
    _camera_x_in_map_corner = false;
    _camera_y_in_map_corner = false;

    // Determine the center position coordinates for the camera
    float camera_x, camera_y; // Holds the final X, Y coordinates of the camera
    float x_pixel_length, y_pixel_length; // The X and Y length values that coorespond to a single pixel in the current coodinate system
    float rounded_x_offset, rounded_y_offset; // The X and Y position offsets of the camera, rounded to perfectly align on a pixel boundary

    uint16 current_x, current_y; // Actual position of the view, either the camera sprite or a point on the camera movement path
    float current_offset_x, current_offset_y; // Actual offset for the view

    // TODO: the call to GetPixelSize() will return the same result every time so long as the coordinate system did not change. If we never
    // change the coordinate system in map mode, then this should be done only once and the calculated values should be saved for re-use.
    // However, we've discussed the possiblity of adding a zoom feature to maps, in which case we need to continually re-calculate the pixel size
    VideoManager->GetPixelSize(x_pixel_length, y_pixel_length);

    float path_x, path_y = 0.0f;
    if(!_camera_timer.IsRunning()) {
        path_x = _camera->GetXPosition();
        path_y = _camera->GetYPosition();
    } else {
        path_x = _camera->GetXPosition() + (1 - _camera_timer.PercentComplete()) * _delta_x;
        path_y = _camera->GetYPosition() + (1 - _camera_timer.PercentComplete()) * _delta_y;
    }

    current_x = GetFloatInteger(path_x);
    current_y = GetFloatInteger(path_y);
    current_offset_x = GetFloatFraction(path_x);
    current_offset_y = GetFloatFraction(path_y);

    rounded_x_offset = FloorToFloatMultiple(current_offset_x, x_pixel_length);
    rounded_y_offset = FloorToFloatMultiple(current_offset_y, y_pixel_length);
    camera_x = static_cast<float>(current_x) + rounded_x_offset;
    camera_y = static_cast<float>(current_y) + rounded_y_offset;

    // Calculate all four screen edges and determine
    // Determine the draw coordinates of the top left corner using the camera's current position
    _map_frame.tile_x_offset = 1.0f - rounded_x_offset;
    if(IsOddNumber(current_x))
        _map_frame.tile_x_offset -= 1.0f;

    _map_frame.tile_y_offset = 2.0f - rounded_y_offset;
    if(IsOddNumber(current_y))
        _map_frame.tile_y_offset -= 1.0f;

    // The starting row and column of tiles to draw is determined by the map camera's position
    _map_frame.tile_x_start = (current_x / 2) - HALF_TILES_ON_X_AXIS;
    _map_frame.tile_y_start = (current_y / 2) - HALF_TILES_ON_Y_AXIS;

    _map_frame.screen_edges.top    = camera_y - HALF_SCREEN_GRID_Y_LENGTH;
    _map_frame.screen_edges.bottom = camera_y + HALF_SCREEN_GRID_Y_LENGTH;
    _map_frame.screen_edges.left   = camera_x - HALF_SCREEN_GRID_X_LENGTH;
    _map_frame.screen_edges.right  = camera_x + HALF_SCREEN_GRID_X_LENGTH;

    // Check for boundary conditions and re-adjust as necessary so we don't draw outside the map area

    // Usually the map centers on the camera's position, but when the camera becomes too close to
    // the edges of the map, we need to modify the drawing properties of the frame.

    // Camera exceeds the left boundary of the map
    if(_map_frame.tile_x_start < 0) {
        _map_frame.tile_x_start = 0;
        _map_frame.tile_x_offset = 1.0f;
        _map_frame.screen_edges.left = 0.0f;
        _map_frame.screen_edges.right = SCREEN_GRID_X_LENGTH;
        _camera_x_in_map_corner = true;
    }
    // Camera exceeds the right boundary of the map
    else if(_map_frame.tile_x_start + TILES_ON_X_AXIS >= _tile_supervisor->_num_tile_on_x_axis) {
        _map_frame.tile_x_start = static_cast<int16>(_tile_supervisor->_num_tile_on_x_axis - TILES_ON_X_AXIS);
        _map_frame.tile_x_offset = 1.0f;
        _map_frame.screen_edges.right = static_cast<float>(_object_supervisor->_num_grid_x_axis);
        _map_frame.screen_edges.left = _map_frame.screen_edges.right - SCREEN_GRID_X_LENGTH;
        _camera_x_in_map_corner = true;
    }

    // Camera exceeds the top boundary of the map
    if(_map_frame.tile_y_start < 0) {
        _map_frame.tile_y_start = 0;
        _map_frame.tile_y_offset = 2.0f;
        _map_frame.screen_edges.top = 0.0f;
        _map_frame.screen_edges.bottom = SCREEN_GRID_Y_LENGTH;
        _camera_y_in_map_corner = true;
    }
    // Camera exceeds the bottom boundary of the map
    else if(_map_frame.tile_y_start + TILES_ON_Y_AXIS >= _tile_supervisor->_num_tile_on_y_axis) {
        _map_frame.tile_y_start = static_cast<int16>(_tile_supervisor->_num_tile_on_y_axis - TILES_ON_Y_AXIS);
        _map_frame.tile_y_offset = 2.0f;
        _map_frame.screen_edges.bottom = static_cast<float>(_object_supervisor->_num_grid_y_axis);
        _map_frame.screen_edges.top = _map_frame.screen_edges.bottom - SCREEN_GRID_Y_LENGTH;
        _camera_y_in_map_corner = true;
    }

    // Determine the number of rows and columns of tiles that need to be drawn

    // When the tile images align perfectly with the screen, we can afford to draw one less row or column of tiles
    if(IsFloatInRange(_map_frame.tile_x_offset, 0.999f, 1.001f)) {
        _map_frame.num_draw_x_axis = TILES_ON_X_AXIS;
    } else {
        _map_frame.num_draw_x_axis = TILES_ON_X_AXIS + 1;
    }
    if(IsFloatInRange(_map_frame.tile_y_offset, 1.999f, 2.001f)) {
        _map_frame.num_draw_y_axis = TILES_ON_Y_AXIS;
    } else {
        _map_frame.num_draw_y_axis = TILES_ON_Y_AXIS + 1;
    }

    // Comment this out to print out map draw debugging info about once a second
// 	static int loops = 0;
// 	if (loops == 0) {
// 		printf("--- MAP DRAW INFO ---\n");
// 		printf("Pixel Size:        [%f, %f]\n", x_pixel_length, y_pixel_length);
// 		printf("Rounded offsets:   [%f, %f]\n", rounded_x_offset, rounded_y_offset);
// 		printf("Starting row, col: [%d, %d]\n", _map_frame.starting_row, _map_frame.starting_col);
// 		printf("# draw rows, cols: [%d, %d]\n", _map_frame.num_draw_rows, _map_frame.num_draw_cols);
// 		printf("Camera position:   [%f, %f]\n", camera_x, camera_y);
// 		printf("Tile draw start:   [%f, %f]\n", _map_frame.tile_x_start, _map_frame.tile_y_start);
// 		printf("Edges (T,B,L,R):   [%f, %f, %f, %f]\n", _map_frame.screen_edges.top, _map_frame.screen_edges.bottom,
// 			_map_frame.screen_edges.left, _map_frame.screen_edges.right);
// 	}
//
// 	if (loops >= 60) {
// 		loops = 0;
// 	}
// 	else {
// 		loops++;
// 	}
} // void MapMode::_UpdateMapFrame()


void MapMode::_DrawDebugGrid()
{

    float tiles_x = TILES_ON_X_AXIS / (SCREEN_GRID_X_LENGTH / 2);
    float tiles_y = TILES_ON_Y_AXIS / (SCREEN_GRID_Y_LENGTH / 2);
    // Collision grid
    VideoManager->DrawGrid(_map_frame.tile_x_offset, _map_frame.tile_y_offset, tiles_x, tiles_y,
                           Color(0.0f, 0.0f, 0.5f, 1.0f));
    // Tile grid
    VideoManager->DrawGrid(_map_frame.tile_x_offset, _map_frame.tile_y_offset, tiles_x * 2, tiles_y * 2,
                           Color(0.5f, 0.0f, 0.0f, 1.0f));
}


void MapMode::_DrawMapLayers()
{
    VideoManager->SetCoordSys(0.0f, SCREEN_GRID_X_LENGTH, SCREEN_GRID_Y_LENGTH, 0.0f);

    _tile_supervisor->DrawLayers(&_map_frame, GROUND_LAYER);
    // Save points are engraved on the ground, and thus shouldn't be drawn after walls.
    _object_supervisor->DrawSavePoints();

    _object_supervisor->DrawFlatGroundObjects();
    _object_supervisor->DrawGroundObjects(false); // First draw pass of ground objects
    _object_supervisor->DrawPassObjects();
    _object_supervisor->DrawGroundObjects(true); // Second draw pass of ground objects

    _tile_supervisor->DrawLayers(&_map_frame, SKY_LAYER);

    _object_supervisor->DrawSkyObjects();

    if(VideoManager->DebugInfoOn()) {
        _object_supervisor->DrawCollisionArea(&_map_frame);
        _object_supervisor->_DrawMapZones();
        _DrawDebugGrid();
    }
} // void MapMode::_DrawMapLayers()



void MapMode::_DrawStaminaBar(const hoa_video::Color &blending)
{
    const Color olive_green(0.0196f, 0.207f, 0.0196f, 1.0f);
    const Color lighter_green(0.419f, 0.894f, 0.0f, 1.0f);
    const Color light_green(0.0196f, 0.207f, 0.0196f, 1.0f);
    const Color medium_green(0.0509f, 0.556f, 0.0509f, 1.0f);
    const Color darkish_green(0.352f, 0.4f, 0.352f, 1.0f);
    const Color dark_green(0.0196f, 0.207f, 0.0196f, 1.0f);
    const Color bright_yellow(0.937f, 1.0f, 0.725f, 1.0f);
    const Color dark_orange(0.737f, 0.5f, 0.196f, 1.0f);
    const Color dark_red(0.737f, 0.0f, 0.125f, 1.0f);

    float fill_size = static_cast<float>(_run_stamina) / 10000.0f;

    VideoManager->PushState();
    VideoManager->SetStandardCoordSys();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);

    // Draw the background image
    VideoManager->Move(780, 747);
    _stamina_bar_background.Draw(blending);

    // Only do this if the bar is at least 4 pixels long
    if((200 * fill_size) >= 4) {
        VideoManager->Move(801, 739);
        VideoManager->DrawRectangle((200 * fill_size) - 2, 1, darkish_green * blending);

        VideoManager->Move(801, 738);
        VideoManager->DrawRectangle(1, 2, medium_green * blending);
        VideoManager->Move(800 + (fill_size * 200 - 2), 738); // Automatically reposition to be at moving endcap
        VideoManager->DrawRectangle(1, 2, medium_green * blending);
    }

    // the bar color depending on its size
    Color bar_color;
    if((200 * fill_size) > 75)
        bar_color = medium_green;
    else if((200 * fill_size) > 30)
        bar_color = dark_orange;
    else
        bar_color = dark_red;

    VideoManager->Move(800, 736);
    VideoManager->DrawRectangle(200 * fill_size, 5, bar_color * blending);

    // Only do this if the bar is at least 6 pixels long
    if((200 * fill_size) >= 6) {
        VideoManager->Move(802, 733);
        VideoManager->DrawRectangle((200 * fill_size) - 4, 1, bright_yellow * blending);
    }

    // Draw the rest only when the color is green
    if(bar_color != medium_green) {
        VideoManager->PopState();
        return;
    }

    // Draw the base color of the bar
    VideoManager->Move(800, 740);
    VideoManager->DrawRectangle(200 * fill_size, 10, olive_green * blending);

    // Shade the bar with a faux lighting effect
    VideoManager->Move(800, 739);
    VideoManager->DrawRectangle(200 * fill_size, 2, dark_green * blending);
    VideoManager->Move(800, 737);
    VideoManager->DrawRectangle(200 * fill_size, 7, darkish_green * blending);

    // Only do this if the bar is at least 4 pixels long
    if((200 * fill_size) >= 4) {
        VideoManager->Move(801, 735);
        VideoManager->DrawRectangle(1, 1, lighter_green * blending);
        VideoManager->Move(800 + (fill_size * 200 - 2), 735); // automatically reposition to be at moving endcap
        VideoManager->DrawRectangle(1, 1, lighter_green * blending);
        VideoManager->Move(800, 734);
        VideoManager->DrawRectangle(200 * fill_size, 2, lighter_green * blending);
    }

    if(_unlimited_stamina) {  // Draw the infinity symbol over the stamina bar
        VideoManager->Move(780, 747);
        _stamina_bar_infinite_overlay.Draw(blending);
    }
    VideoManager->PopState();
}

void MapMode::_DrawGUI()
{
    // Draw the introductory location name and graphic if necessary
    if(!_intro_timer.IsFinished()) {
        uint32 time = _intro_timer.GetTimeExpired();

        Color blend(1.0f, 1.0f, 1.0f, 1.0f);
        if(time < 1000) {  // Fade in
            blend.SetAlpha((static_cast<float>(time) / 1000.0f));
        } else if(time > 2000) { // Fade out
            blend.SetAlpha(1.0f - static_cast<float>(time - 2000) / 1000.0f);
        }

        // Don't draw the map location again, when it is the same as the last map.
        if(GlobalManager->ShouldDisplayHudNameOnMapIntro()) {
            VideoManager->PushState();
            VideoManager->SetStandardCoordSys();
            VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
            VideoManager->Move(512.0f, 100.0f);
            _map_image.Draw(blend);
            float shifting = (((float)time) - 2000.0f) / 100.0f;
            VideoManager->MoveRelative(0.0f + shifting, -80.0f);
            VideoManager->Text()->Draw(_map_hud_name, TextStyle("map_title", blend, VIDEO_TEXT_SHADOW_DARK));
            VideoManager->PopState();
        }

        // Show the map subname when there is one
        VideoManager->PushState();
        VideoManager->SetStandardCoordSys();
        VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
        (GlobalManager->ShouldDisplayHudNameOnMapIntro() && !_map_hud_name.empty()) ?
        VideoManager->Move(512.0f, 170.0f) : VideoManager->Move(512.0f, 20.0f);
        VideoManager->Text()->Draw(_map_hud_subname, TextStyle("title24", blend, VIDEO_TEXT_SHADOW_DARK));
        VideoManager->PopState();

        // Draw the unlimited stamina bar with a fade out
        if(_unlimited_stamina) {
            _DrawStaminaBar(blend);
        } else if(time < 1000) {
            // Draw the normal bar fade in only (no fade out)
            _DrawStaminaBar(blend);
        } else {
            _DrawStaminaBar();
        }
    }

    // Draw the stamina bar in the lower right corner
    if(!_unlimited_stamina && _intro_timer.IsFinished())
        _DrawStaminaBar();

    // Draw the debug info
    if(!VideoManager->DebugInfoOn())
        return;
    // Camera map coordinates
    VirtualSprite *cam = GetCamera();
    if(!cam)
        return;

    VideoManager->PushState();
    VideoManager->SetStandardCoordSys();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_CENTER, 0);

    float x_pos = cam->GetXPosition();
    float y_pos = cam->GetYPosition();
    std::ostringstream coord_txt;
    coord_txt << "Camera position: " << x_pos << ", " << y_pos;
    VideoManager->Move(10.0f, 10.0f);
    VideoManager->Text()->Draw(coord_txt.str(), TextStyle("title22", Color::white, VIDEO_TEXT_SHADOW_DARK));
    VideoManager->PopState();
} // void MapMode::_DrawGUI()

} // namespace hoa_map

