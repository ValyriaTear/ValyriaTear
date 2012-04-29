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

#include "engine/audio/audio.h"
#include "engine/input.h"
#include "engine/system.h"

#include "common/global/global.h"

#include "modes/menu/menu.h"
#include "modes/pause.h"
#include "modes/boot/boot.h"
#include "modes/save/save_mode.h"

#include "modes/map/map_dialogue.h"
#include "modes/map/map_events.h"
#include "modes/map/map_objects.h"
#include "modes/map/map_sprites.h"
#include "modes/map/map_tiles.h"
#include "modes/map/map_treasure.h"
#include "modes/map/map_zones.h"

using namespace std;
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

namespace hoa_map {

// Initialize static class variables
MapMode* MapMode::_current_instance = NULL;

// ****************************************************************************
// ********** MapMode Public Class Methods
// ****************************************************************************

MapMode::MapMode(string filename) :
	GameMode(),
	_map_filename(filename),
	_map_tablespace(""),
	_map_event_group(NULL),
	_tile_supervisor(NULL),
	_object_supervisor(NULL),
	_event_supervisor(NULL),
	_dialogue_supervisor(NULL),
	_treasure_supervisor(NULL),
	_camera(NULL),
	_delta_x(0),
	_delta_y(0),
	_num_map_contexts(0),
	_running_disabled(false),
	_unlimited_stamina(false),
	_show_gui(true),
	_current_track(0),
	_run_stamina(10000)
{
	// Remove potential previous ambient overlay effects
	VideoManager->DisableFadeEffect();

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
	for (uint32 i = 0; i < active_save_point_animations.size(); ++i)
		ScaleToMapCoords(active_save_point_animations[i]);

	for (uint32 i = 0; i < inactive_save_point_animations.size(); ++i)
		ScaleToMapCoords(inactive_save_point_animations[i]);

	// Create the event group name by modifying the filename to consists only of alphanumeric characters and underscores
	// This will make it a valid identifier name in Lua syntax
	string event_group_name = _map_filename;
	std::replace(event_group_name.begin(), event_group_name.end(), '/', '_');
	std::replace(event_group_name.begin(), event_group_name.end(), '.', '_');

	if (GlobalManager->DoesEventGroupExist(event_group_name) == false) {
		GlobalManager->AddNewEventGroup(event_group_name);
	}
	_map_event_group = GlobalManager->GetEventGroup(event_group_name);

	_tile_supervisor = new TileSupervisor();
	_object_supervisor = new ObjectSupervisor();
	_event_supervisor = new EventSupervisor();
	_dialogue_supervisor = new DialogueSupervisor();
	_treasure_supervisor = new TreasureSupervisor();

	_intro_timer.Initialize(7000, 0);
	_intro_timer.EnableAutoUpdate(this);

	_camera_timer.Initialize(0, 1);

	if (!_Load()) {
	    PRINT_ERROR << "Couldn't load the map file: " << _map_filename << endl
	        << "Returning to boot mode. You should report this error." << endl;
	    BootMode *BM = new BootMode();
	    ModeManager->PopAll();
	    ModeManager->Push(BM);
	    return;
	}

	// Load miscellaneous map graphics
	_dialogue_icon.LoadFromAnimationScript("img/misc/dialogue_icon.lua");
	ScaleToMapCoords(_dialogue_icon);

	if (_stamina_bar_background.Load("img/misc/stamina_bar_background.png", 227, 24) == false)
		IF_PRINT_WARNING(MAP_DEBUG) << "failed to load the the stamina bar background image" << endl;

	if (_stamina_bar_infinite_overlay.Load("img/misc/stamina_bar_infinite_overlay.png", 227, 24) == false)
		IF_PRINT_WARNING(MAP_DEBUG) << "failed to load the the stamina bar infinite overlay image" << endl;
}



MapMode::~MapMode() {
	for (uint32 i = 0; i < _music.size(); i++)
		_music[i].FreeAudio();
	_music.clear();

	for (uint32 i = 0; i < _sounds.size(); i++)
		_sounds[i].FreeAudio();
	_sounds.clear();

	for (uint32 i = 0; i < _enemies.size(); i++)
		delete(_enemies[i]);
	_enemies.clear();

	delete(_tile_supervisor);
	delete(_object_supervisor);
	delete(_event_supervisor);
	delete(_dialogue_supervisor);
	delete(_treasure_supervisor);

	_map_script.CloseFile();
}



void MapMode::Reset() {
	// Reset video engine context properties
	VideoManager->SetCoordSys(0.0f, SCREEN_GRID_X_LENGTH, SCREEN_GRID_Y_LENGTH, 0.0f);
	VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_BOTTOM, 0);

	// Set the active instance pointer to this map
	MapMode::_current_instance = this;

	// Make the map location known globally to other code that may need to know this information
	GlobalManager->SetMap(_map_filename, _map_image.GetFilename());

	if (_music.size() > _current_track && _music[_current_track].GetState() != AUDIO_STATE_PLAYING) {
		_music[_current_track].Play();
	}

	_intro_timer.Run();
}



void MapMode::Update() {
	// Update the map frame coords
	// NOTE: It's done before handling pause so that the frame is updated at
	// least once before setting the pause mode, avoiding a crash.
	_UpdateMapFrame();

	// Process quit and pause events unconditional to the state of map mode
	if (InputManager->QuitPress()) {
		ModeManager->Push(new PauseMode(true));
		return;
	}
	else if (InputManager->PausePress()) {
		ModeManager->Push(new PauseMode(false));
		return;
	}

	_dialogue_icon.Update();

	// Call the map script's update function
	if (_update_function.is_valid())
		ScriptCallFunction<void>(_update_function);

	// Update all animated tile images
	_tile_supervisor->Update();
	_object_supervisor->Update();
	_object_supervisor->SortObjects();

	// Update the active state of the map
	switch (CurrentState()) {
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
			IF_PRINT_WARNING(MAP_DEBUG) << "map was set in an unknown state: " << CurrentState() << endl;
			ResetState();
			break;
	}

	// ---------- (4) Update the camera timer
	_camera_timer.Update();

	// ---------- (5) Update all active map events
	_event_supervisor->Update();

	GameMode::Update();
} // void MapMode::Update()



void MapMode::Draw() {
	if (_draw_function.is_valid())
		ScriptCallFunction<void>(_draw_function);
	else
		_DrawMapLayers();

	_object_supervisor->DrawDialogIcons();
}

void MapMode::DrawPostEffects() {
	VideoManager->SetCoordSys(0.0f, SCREEN_GRID_X_LENGTH, SCREEN_GRID_Y_LENGTH, 0.0f);
	// Halos are additive blending made, so they should be applied
	// as post-effects but before the GUI.
	_object_supervisor->DrawHalos();

	// Draw the gui, unaffected by potential
	// fading effects.
	_DrawGUI();
	if (CurrentState() == STATE_DIALOGUE) {
		_dialogue_supervisor->Draw();
	}
}



void MapMode::ResetState() {
	_state_stack.clear();
	_state_stack.push_back(STATE_INVALID);
}



void MapMode::PushState(MAP_STATE state) {
	_state_stack.push_back(state);
}



void MapMode::PopState() {
	_state_stack.pop_back();
	if (_state_stack.empty() == true) {
		IF_PRINT_WARNING(MAP_DEBUG) << "stack was empty after operation, reseting state stack" << endl;
		_state_stack.push_back(STATE_INVALID);
	}
}



MAP_STATE MapMode::CurrentState() {
	if (_state_stack.empty() == true) {
		IF_PRINT_WARNING(MAP_DEBUG) << "stack was empty, reseting state stack" << endl;
		_state_stack.push_back(STATE_INVALID);
	}
	return _state_stack.back();
}



void MapMode::AddGroundObject(MapObject *obj) {
	if (!obj) {
		PRINT_WARNING << "Couldn't add NULL object." << endl;
		return;
	}
	_object_supervisor->_ground_objects.push_back(obj);
	_object_supervisor->_all_objects.insert(make_pair(obj->object_id, obj));
}


void MapMode::AddPassObject(MapObject *obj) {
	if (!obj) {
		PRINT_WARNING << "Couldn't add NULL object." << endl;
		return;
	}
	_object_supervisor->_pass_objects.push_back(obj);
	_object_supervisor->_all_objects.insert(make_pair(obj->object_id, obj));
}


void MapMode::AddSkyObject(MapObject *obj) {
	if (!obj) {
		PRINT_WARNING << "Couldn't add NULL object." << endl;
		return;
	}
	_object_supervisor->_sky_objects.push_back(obj);
	_object_supervisor->_all_objects.insert(make_pair(obj->object_id, obj));
}


void MapMode::AddZone(MapZone *zone) {
	if (!zone) {
		PRINT_WARNING << "Couldn't add NULL zone." << endl;
		return;
	}
	_object_supervisor->_zones.push_back(zone);
}


void MapMode::AddSavePoint(float x, float y, MAP_CONTEXT map_context) {
	SavePoint *save_point = new SavePoint(x, y, map_context);
	_object_supervisor->_save_points.push_back(save_point);
}


void MapMode::AddHalo(const std::string& filename, float x, float y,
						const Color& color, MAP_CONTEXT map_context) {
	Halo *halo = new Halo(filename, x, y, color, map_context);
	_object_supervisor->_halos.push_back(halo);
}


bool MapMode::IsEnemyLoaded(uint32 id) const {
	for (uint32 i = 0; i < _enemies.size(); i++) {
		if (_enemies[i]->GetID() == id) {
			return true;
		}
	}
	return false;
}



void MapMode::PlayMusic(uint32 track_num) {
	_music[_current_track].Stop();
	_current_track = track_num;
	_music[_current_track].Play();
}



void MapMode::SetCamera(private_map::VirtualSprite* sprite, uint32 duration) {
    if (_camera == sprite) {
        IF_PRINT_WARNING(MAP_DEBUG) << "Camera was moved to the same sprite" << endl;
    }
    else {
        if (duration > 0) {
            _delta_x = _camera->ComputeXLocation() - sprite->ComputeXLocation();
            _delta_y = _camera->ComputeYLocation() - sprite->ComputeYLocation();
            _camera_timer.Reset();
            _camera_timer.SetDuration(duration);
            _camera_timer.Run();
        }
        _camera = sprite;
    }
}



void MapMode::MoveVirtualFocus(uint16 loc_x, uint16 loc_y) {
    _object_supervisor->VirtualFocus()->SetXPosition(loc_x, 0.0f);
    _object_supervisor->VirtualFocus()->SetYPosition(loc_y, 0.0f);
}



void MapMode::MoveVirtualFocus(uint16 loc_x, uint16 loc_y, uint32 duration) {
    if (_camera != _object_supervisor->VirtualFocus()) {
        IF_PRINT_WARNING(MAP_DEBUG) << "Attempt to move camera although on different sprite" << endl;
    }
    else {
        if (duration > 0) {
            _delta_x = _object_supervisor->VirtualFocus()->ComputeXLocation() - static_cast<float>(loc_x);
            _delta_y = _object_supervisor->VirtualFocus()->ComputeYLocation() - static_cast<float>(loc_y);
            _camera_timer.Reset();
            _camera_timer.SetDuration(duration);
            _camera_timer.Run();
        }
        MoveVirtualFocus(loc_x, loc_y);
    }
}


bool MapMode::IsCameraOnVirtualFocus() {
    return _camera == _object_supervisor->VirtualFocus();
}


MAP_CONTEXT MapMode::GetCurrentContext() const {
	if (_camera)
		return _camera->GetContext();
	return MAP_CONTEXT_01;
}


bool MapMode::AttackAllowed() {
    return (CurrentState() != STATE_DIALOGUE && CurrentState() != STATE_TREASURE && !IsCameraOnVirtualFocus());
}

// ****************************************************************************
// ********** MapMode Private Class Methods
// ****************************************************************************

bool MapMode::_Load() {
	// ---------- (1) Open map script file and read in the basic map properties and tile definitions
	if (!_map_script.OpenFile(_map_filename)) {
	    PRINT_ERROR << "Couldn't open map script file: " << _map_filename << endl;
		return false;
	}

	// The map tablespace is needed later by scripted events.
	_map_tablespace = _map_script.OpenTablespace();

	if (_map_tablespace.empty()) {
		PRINT_ERROR << "Couldn't open map name space: " << _map_filename << endl;
		return false;
	}

	// Read the number of map contexts, the name of the map, and load the location graphic image
	_num_map_contexts = _map_script.ReadUInt("num_map_contexts");
	_map_name = MakeUnicodeString(_map_script.ReadString("map_name"));
	std::string map_filename = _map_script.ReadString("map_image_filename");
	if (!map_filename.empty() && !_map_image.Load(_map_script.ReadString("map_image_filename")))
		PRINT_ERROR << "Failed to load location graphic image: " << _map_image.GetFilename() << endl;

	// Instruct the supervisor classes to perform their portion of the load operation
	if (!_tile_supervisor->Load(_map_script, this)) {
		PRINT_ERROR << "Failed to load the tile data." << endl;
		return false;
	}

	// NOTE: The object supervisor will complain itself about the error.
	if (!_object_supervisor->Load(_map_script))
		return false;

	// ---------- (3) Load map sounds and music
	vector<string> sound_filenames;
	_map_script.ReadStringVector("sound_filenames", sound_filenames);

	for (uint32 i = 0; i < sound_filenames.size(); i++) {
		_sounds.push_back(SoundDescriptor());
		if (_sounds.back().LoadAudio(sound_filenames[i]) == false) {
			PRINT_WARNING << "Failed to load map sound: " << sound_filenames[i] << endl;
		}
	}

	vector<string> music_filenames;
	_map_script.ReadStringVector("music_filenames", music_filenames);
	_music.resize(music_filenames.size(), MusicDescriptor());
	for (uint32 i = 0; i < music_filenames.size(); i++) {
		if (_music[i].LoadAudio(music_filenames[i]) == false) {
			PRINT_WARNING << "Failed to load map music: " << music_filenames[i] << endl;
		}
	}

	// ---------- (4) Create and store all enemies that may appear on this map
	vector<int32> enemy_ids;
	_map_script.ReadIntVector("enemy_ids", enemy_ids);
	for (uint32 i = 0; i < enemy_ids.size(); i++) {
		_enemies.push_back(new GlobalEnemy(enemy_ids[i]));
	}

	// ---------- (5) Call the map script's custom load function and get a reference to all other script function pointers
	ScriptObject map_table(luabind::from_stack(_map_script.GetLuaState(), hoa_script::private_script::STACK_TOP));
	ScriptObject function = map_table["Load"];

    bool loading_succeeded = true;
	if (function.is_valid()) {
		try {
		    ScriptCallFunction<void>(function, this);
		}
		catch (luabind::error e) {
			ScriptManager->HandleLuaError(e);
			loading_succeeded = false;
		}
		catch (luabind::cast_failed e) {
			ScriptManager->HandleCastError(e);
			loading_succeeded = false;
		}
	}
	else {
		loading_succeeded = false;
	}

	if (!loading_succeeded) {
		PRINT_ERROR << "Invalid map Load() function. The function wasn't called." << endl;
		return false;
	}

	_update_function = _map_script.ReadFunctionPointer("Update");
	_draw_function = _map_script.ReadFunctionPointer("Draw");

	// ---------- (6) Prepare all sprites with dialogue
	// This is done at this stage because the map script's load function creates the sprite and dialogue objects. Only after
	// both sets are created can we determine which sprites have active dialogue.

	// TODO: Need to figure out a new function appropriate for this code?
	// TEMP: The line below is very bad to do, but is necessary for the UpdateDialogueStatus function to work correctly
	_current_instance = this;
	for (map<uint16, MapObject*>::iterator i = _object_supervisor->_all_objects.begin(); i != _object_supervisor->_all_objects.end(); i++) {
		if (i->second->GetType() == SPRITE_TYPE) {
			MapSprite* sprite = dynamic_cast<MapSprite*>(i->second);
			sprite->UpdateDialogueStatus();
		}
	}

	_map_script.CloseAllTables();

	return true;
} // bool MapMode::_Load()



void MapMode::_UpdateExplore() {
	// First go to menu mode if the user requested it
	if (InputManager->MenuPress()) {
		MenuMode *MM = new MenuMode(_map_name, _map_image.GetFilename());
		ModeManager->Push(MM);
		return;
	}

	// Update the running state of the camera object. Check if the player wishes to continue running and if so,
	// update the stamina value if the operation is permitted
	_camera->is_running = false;
	if (_running_disabled == false && InputManager->CancelState() == true &&
		(InputManager->UpState() || InputManager->DownState() || InputManager->LeftState() || InputManager->RightState()))
	{
		if (_unlimited_stamina) {
			_camera->is_running = true;
		}
		else if (_run_stamina > SystemManager->GetUpdateTime() * 2) {
			_run_stamina -= (SystemManager->GetUpdateTime() * 2);
			_camera->is_running = true;
		}
		else {
			_run_stamina = 0;
		}
	}
	// Regenerate the stamina at 1/2 the consumption rate
	else if (_run_stamina < 10000) {
		_run_stamina += SystemManager->GetUpdateTime();
		if (_run_stamina > 10000)
			_run_stamina = 10000;
	}

	// If the user requested a confirm event, check if there is a nearby object that the player may interact with
	// Interactions are currently limited to dialogue with sprites and opening of treasures
	if (InputManager->ConfirmPress()) {
		MapObject* obj = _object_supervisor->FindNearestObject(_camera);

		if (obj != NULL) {
			if (obj->GetType() == SPRITE_TYPE) {
				MapSprite *sp = reinterpret_cast<MapSprite*>(obj);

				if (sp->HasAvailableDialogue()) {
					_camera->moving = false;
					_camera->is_running = false;
					sp->InitiateDialogue();
					return;
				}
			}
			else if (obj->GetType() == TREASURE_TYPE) {
				TreasureObject* treasure_object = reinterpret_cast<TreasureObject*>(obj);

				if (treasure_object->GetTreasure()->IsTaken() == false) {
				    _camera->moving = false;
					treasure_object->Open();
				}
			}
			else if (obj->GetType() == SAVE_TYPE) {
				SaveMode *save_mode = new SaveMode(true, obj->x_position, obj->y_position);
				ModeManager->Push(save_mode, false, false);
			}
		}
	}

	// Detect movement input from the user
	if (InputManager->UpState() || InputManager->DownState() || InputManager->LeftState() || InputManager->RightState()) {
		_camera->moving = true;
	}
	else {
		_camera->moving = false;
	}

	// Determine the direction of movement. Priority of movement is given to: up, down, left, right.
	// In the case of diagonal movement, the direction that the sprite should face also needs to be deduced.
	if (_camera->moving == true) {
		if (InputManager->UpState())
		{
			if (InputManager->LeftState())
				_camera->SetDirection(MOVING_NORTHWEST);
			else if (InputManager->RightState())
				_camera->SetDirection(MOVING_NORTHEAST);
			else
				_camera->SetDirection(NORTH);
		}
		else if (InputManager->DownState())
		{
			if (InputManager->LeftState())
				_camera->SetDirection(MOVING_SOUTHWEST);
			else if (InputManager->RightState())
				_camera->SetDirection(MOVING_SOUTHEAST);
			else
				_camera->SetDirection(SOUTH);
		}
		else if (InputManager->LeftState()) {
			_camera->SetDirection(WEST);
		}
		else if (InputManager->RightState()) {
			_camera->SetDirection(EAST);
		}
	} // if (_camera->moving == true)
} // void MapMode::_UpdateExplore()



void MapMode::_UpdateMapFrame() {
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

	if (!_camera_timer.IsRunning()) {
	    _camera->GetXPosition(current_x, current_offset_x);
	    _camera->GetYPosition(current_y, current_offset_y);
	}
	else {
	    // Calculate path
	    float path_x = _camera->ComputeXLocation()+(1-_camera_timer.PercentComplete())*_delta_x;
	    float path_y = _camera->ComputeYLocation()+(1-_camera_timer.PercentComplete())*_delta_y;

	    current_x = GetFloatInteger(path_x);
	    current_y = GetFloatInteger(path_y);

	    current_offset_x = GetFloatFraction(path_x);
	    current_offset_y = GetFloatFraction(path_y);
	}

	rounded_x_offset = FloorToFloatMultiple(current_offset_x, x_pixel_length);
	rounded_y_offset = FloorToFloatMultiple(current_offset_y, y_pixel_length);
	camera_x = static_cast<float>(current_x) + rounded_x_offset;
	camera_y = static_cast<float>(current_y) + rounded_y_offset;

	// Calculate all four screen edges and determine
	// Determine the draw coordinates of the top left corner using the camera's current position
	_map_frame.tile_x_offset = 1.0f - rounded_x_offset;
	if (IsOddNumber(current_x))
		_map_frame.tile_x_offset -= 1.0f;

	_map_frame.tile_y_offset = 2.0f - rounded_y_offset;
	if (IsOddNumber(current_y))
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
	if (_map_frame.tile_x_start < 0) {
		_map_frame.tile_x_start = 0;
		_map_frame.tile_x_offset = 1.0f;
		_map_frame.screen_edges.left = 0.0f;
		_map_frame.screen_edges.right = SCREEN_GRID_X_LENGTH;
	}
	// Camera exceeds the right boundary of the map
	else if (_map_frame.tile_x_start + TILES_ON_X_AXIS >= _tile_supervisor->_num_tile_on_x_axis) {
		_map_frame.tile_x_start = static_cast<int16>(_tile_supervisor->_num_tile_on_x_axis - TILES_ON_X_AXIS);
		_map_frame.tile_x_offset = 1.0f;
		_map_frame.screen_edges.right = static_cast<float>(_object_supervisor->_num_grid_x_axis);
		_map_frame.screen_edges.left = _map_frame.screen_edges.right - SCREEN_GRID_X_LENGTH;
	}

	// Camera exceeds the top boundary of the map
	if (_map_frame.tile_y_start < 0) {
		_map_frame.tile_y_start = 0;
		_map_frame.tile_y_offset = 2.0f;
		_map_frame.screen_edges.top = 0.0f;
		_map_frame.screen_edges.bottom = SCREEN_GRID_Y_LENGTH;
	}
	// Camera exceeds the bottom boundary of the map
	else if (_map_frame.tile_y_start + TILES_ON_Y_AXIS >= _tile_supervisor->_num_tile_on_y_axis) {
		_map_frame.tile_y_start = static_cast<int16>(_tile_supervisor->_num_tile_on_y_axis - TILES_ON_Y_AXIS);
		_map_frame.tile_y_offset = 2.0f;
		_map_frame.screen_edges.bottom = static_cast<float>(_object_supervisor->_num_grid_y_axis);
		_map_frame.screen_edges.top = _map_frame.screen_edges.bottom - SCREEN_GRID_Y_LENGTH;
	}

	// Determine the number of rows and columns of tiles that need to be drawn

	// When the tile images align perfectly with the screen, we can afford to draw one less row or column of tiles
	if (IsFloatInRange(_map_frame.tile_x_offset, 0.999f, 1.001f)) {
		_map_frame.num_draw_x_axis = TILES_ON_X_AXIS;
	}
	else {
		_map_frame.num_draw_x_axis = TILES_ON_X_AXIS + 1;
	}
	if (IsFloatInRange(_map_frame.tile_y_offset, 1.999f, 2.001f)) {
		_map_frame.num_draw_y_axis = TILES_ON_Y_AXIS;
	}
	else {
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
} // void MapMode::_CalculateMapFrame()



void MapMode::_DrawMapLayers() {
	VideoManager->SetCoordSys(0.0f, SCREEN_GRID_X_LENGTH, SCREEN_GRID_Y_LENGTH, 0.0f);

	_tile_supervisor->DrawLayers(&_map_frame, GROUND_LAYER);
	// Save points are engraved on the ground, and thus shouldn't be drawn after walls.
	_object_supervisor->DrawSavePoints();

	_tile_supervisor->DrawLayers(&_map_frame, FRINGE_LAYER);

	_object_supervisor->DrawGroundObjects(false); // First draw pass of ground objects
	_object_supervisor->DrawPassObjects();
	_object_supervisor->DrawGroundObjects(true); // Second draw pass of ground objects

	_tile_supervisor->DrawLayers(&_map_frame, SKY_LAYER);

	_object_supervisor->DrawSkyObjects();
} // void MapMode::_DrawMapLayers()



void MapMode::_DrawStaminaBar(const hoa_video::Color &blending) {
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
	if ((200 * fill_size) >= 4) {
		VideoManager->Move(801, 739);
		VideoManager->DrawRectangle((200 * fill_size) -2, 1, darkish_green * blending);

		VideoManager->Move(801, 738);
		VideoManager->DrawRectangle(1, 2, medium_green * blending);
		VideoManager->Move(800 + (fill_size * 200 - 2), 738); // Automatically reposition to be at moving endcap
		VideoManager->DrawRectangle(1, 2, medium_green * blending);
	}

	// the bar color depending on its size
	Color bar_color;
	if ((200 * fill_size) > 75)
		bar_color = medium_green;
	else if ((200 * fill_size) > 30)
		bar_color = dark_orange;
	else
		bar_color = dark_red;

	VideoManager->Move(800, 736);
	VideoManager->DrawRectangle(200 * fill_size, 5, bar_color * blending);

	// Only do this if the bar is at least 6 pixels long
	if ((200 * fill_size) >= 6) {
		VideoManager->Move(802, 733);
		VideoManager->DrawRectangle((200 * fill_size) - 4, 1, bright_yellow * blending);
	}

    // Draw the rest only when the color is green
    if (bar_color != medium_green) {
        VideoManager->PopState();
        return;
    }

	// Draw the base color of the bar
	VideoManager->Move(800, 740);
	VideoManager->DrawRectangle(200 * fill_size, 10, olive_green * blending);

	// Shade the bar with a faux lighting effect
	VideoManager->Move(800,739);
	VideoManager->DrawRectangle(200 * fill_size, 2, dark_green * blending);
	VideoManager->Move(800, 737);
	VideoManager->DrawRectangle(200 * fill_size, 7, darkish_green * blending);

	// Only do this if the bar is at least 4 pixels long
	if ((200 * fill_size) >= 4) {
		VideoManager->Move(801, 735);
		VideoManager->DrawRectangle(1, 1, lighter_green * blending);
		VideoManager->Move(800 + (fill_size * 200 - 2), 735); // automatically reposition to be at moving endcap
		VideoManager->DrawRectangle(1, 1, lighter_green * blending);
		VideoManager->Move(800, 734);
		VideoManager->DrawRectangle(200 * fill_size, 2, lighter_green * blending);
	}

	if (_unlimited_stamina) { // Draw the infinity symbol over the stamina bar
		VideoManager->Move(780, 747);
		_stamina_bar_infinite_overlay.Draw(blending);
	}
	VideoManager->PopState();
}

void MapMode::_DrawGUI() {
	// ---------- (1) Draw the introductory location name and graphic if necessary
	if (_intro_timer.IsFinished() == false) {
		uint32 time = _intro_timer.GetTimeExpired();

		Color blend(1.0f, 1.0f, 1.0f, 1.0f);
		if (time < 2000) { // Fade in
			blend.SetAlpha((static_cast<float>(time) / 2000.0f));
		}
		else if (time > 5000) { // Fade out
			blend.SetAlpha(1.0f - static_cast<float>(time - 5000) / 2000.0f);
		}

		VideoManager->PushState();
		VideoManager->SetStandardCoordSys();
		VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
		VideoManager->Move(512.0f, 100.0f);
		_map_image.Draw(blend);
		VideoManager->MoveRelative(0.0f, -80.0f);
		VideoManager->Text()->Draw(_map_name, TextStyle("title24", blend, VIDEO_TEXT_SHADOW_DARK));
		VideoManager->PopState();

		// Draw the unlimited stamina bar with a fade out
		if (_unlimited_stamina) {
			_DrawStaminaBar(blend);
		}
		else if (time < 2000) {
			// Draw the normal bar fade in only (no fade out)
			_DrawStaminaBar(blend);
		}
		else {
			_DrawStaminaBar();
		}
	}

	// ---------- (2) Draw the stamina bar in the lower right corner
	if (!_unlimited_stamina && _intro_timer.IsFinished())
		_DrawStaminaBar();

	// ---------- (3) Draw the treasure menu if necessary
	if (_treasure_supervisor->IsActive())
		_treasure_supervisor->Draw();
} // void MapMode::_DrawGUI()

} // namespace hoa_map

