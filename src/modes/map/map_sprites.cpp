///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_sprites.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for map mode sprites.
*** ***************************************************************************/

#include "utils.h"

#include "engine/audio/audio.h"
#include "engine/mode_manager.h"
#include "engine/system.h"

#include "common/global/global.h"

#include "modes/map/map.h"
#include "modes/map/map_sprites.h"
#include "modes/map/map_objects.h"
#include "modes/map/map_dialogue.h"
#include "modes/map/map_events.h"

#include "modes/battle/battle.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_mode_manager;
using namespace hoa_video;
using namespace hoa_script;
using namespace hoa_system;
using namespace hoa_global;
using namespace hoa_battle;

namespace hoa_map {

namespace private_map {

// ****************************************************************************
// ********** VirtualSprite class methods
// ****************************************************************************

VirtualSprite::VirtualSprite() :
	direction(SOUTH),
	movement_speed(NORMAL_SPEED),
	moving(false),
	moved_position(false),
	is_running(false),
	control_event(NULL),
	_state_saved(false),
	_saved_direction(0),
	_saved_movement_speed(0.0f),
	_saved_moving(false)
{
	MapObject::_object_type = VIRTUAL_TYPE;
}



VirtualSprite::~VirtualSprite()
{}



void VirtualSprite::Update() {
	moved_position = false;

	if (!updatable || !moving)
		return;

	_SetNextPosition();
} // void VirtualSprite::Update()


void VirtualSprite::_SetNextPosition() {

	// Next sprite's position holders
	float next_pos_x = GetXPosition();
	float next_pos_y = GetYPosition();
	float distance_moved = CalculateDistanceMoved();

	// Move the sprite the appropriate distance in the appropriate Y and X direction
	if (direction & (NORTH | MOVING_NORTHWEST | MOVING_NORTHEAST))
		next_pos_y -= distance_moved;
	else if (direction & (SOUTH | MOVING_SOUTHWEST | MOVING_SOUTHEAST))
		next_pos_y += distance_moved;
	if (direction & (WEST | MOVING_NORTHWEST | MOVING_SOUTHWEST))
		next_pos_x -= distance_moved;
	else if (direction & (EAST | MOVING_NORTHEAST | MOVING_SOUTHEAST))
		next_pos_x += distance_moved;

	// When not moving, do not check anything else.
	if (next_pos_x == GetXPosition() && next_pos_y == GetYPosition())
		return;

	// We've got the next position, let's check whether the next position
	// should be revised.

	// Used to know whether we could fall back to a straight move
	// in case of collision.
	bool moving_diagonally = (direction & (MOVING_NORTHWEST | MOVING_NORTHEAST
											| MOVING_SOUTHEAST | MOVING_SOUTHWEST));

	// Handle collision with the first object encountered
	MapObject* collision_object = NULL;
	COLLISION_TYPE collision_type = NO_COLLISION;
	MapMode *map = MapMode::CurrentInstance();
	ObjectSupervisor *object_supervisor = map->GetObjectSupervisor();
	collision_type = object_supervisor->DetectCollision(this, next_pos_x,
														next_pos_y,
														&collision_object);
	// Try to fall back to straight direction
	if (moving_diagonally && collision_type != NO_COLLISION) {
		// Try on x axis
		if (object_supervisor->DetectCollision(this, position.x, next_pos_y, &collision_object) == NO_COLLISION) {
			next_pos_x = position.x;
			collision_type = NO_COLLISION;
		} // and then on y axis
		else if (object_supervisor->DetectCollision(this, next_pos_x, position.y, &collision_object) == NO_COLLISION) {
			next_pos_y = position.y;
			collision_type = NO_COLLISION;
		}
	}

	// Handles special collision handling first
	if (control_event) {
		switch (control_event->GetEventType()) {
		// Don't stuck the player's character or a sprite being controlled by a prepared path.
		// Plus, it's better not to change a path with encountered beings once started
		// for simplification purpose.
		case PATH_MOVE_SPRITE_EVENT:
			collision_type = NO_COLLISION;
			break;
		// Change the direction whenever something blocking is in the way.
		case RANDOM_MOVE_SPRITE_EVENT:
			if (collision_type != NO_COLLISION) {
				SetRandomDirection();
				return;
			}
		default:
			break;
		}
	}

    // Try to handle wall and physical collisions after a failed straight or diagonal move
    switch (collision_type) {
	case NO_COLLISION:
		default:
		break;
	case WALL_COLLISION:
		// When the sprite is controlled by the camera, let the player handle the position correction.
		if (this == map->GetCamera())
			return;

		// When being blocked and moving diagonally, the npc is stuck.
		if (moving_diagonally)
			return;

		// When it's a true wall, try against the collision grid
		if (!collision_object) {
			// Try a random diagonal to avoid the wall in straight direction
			if (direction & (NORTH | SOUTH))
				direction |= RandomBoundedInteger(0,1) ? EAST : WEST;
			else if (direction & (EAST | WEST))
				direction |= RandomBoundedInteger(0,1) ? NORTH : SOUTH;
				return;
		}
		// Physical and treasure objects are the only other matching "fake" walls
		else {
			// Try a diagonal to avoid the sprite in straight direction by comparing
			// each one coords.
			float diff_x = GetXPosition() - collision_object->GetXPosition();
			float diff_y = GetYPosition() - collision_object->GetYPosition();
				if (direction & (NORTH | SOUTH))
					direction |= diff_x >= 0.0f ? EAST : WEST;
				else if (direction & (EAST | WEST))
					direction |= diff_y >= 0.0f ? SOUTH : NORTH;
					return;
			}
			// Other cases shouldn't happen.
		break;
	case ENEMY_COLLISION:
		// Check only whether the player has collided with a monster
		if (this == map->GetCamera() &&
				collision_object && collision_object->GetObjectType() == ENEMY_TYPE) {
			EnemySprite* enemy = reinterpret_cast<EnemySprite*>(collision_object);

			if (enemy && enemy->IsHostile() && map->AttackAllowed()) {
				_StartBattleEncounter(enemy);
				return;
			}
		}

		break;
	case CHARACTER_COLLISION:
		// Check whether the sprite is tangled with another character, even without moving
		// For instance, when colliding with a path follower npc.
		// And let it through in that case.
		if (object_supervisor->CheckObjectCollision(GetCollisionRectangle(), collision_object)) {
			collision_type = NO_COLLISION;
			break;
		}

		// When the sprite is controlled by the camera, let the player handle the position correction.
		if (this == map->GetCamera())
			return;

		// Check whether an enemy has collided with the player
		if (this->GetType() == ENEMY_TYPE && collision_object == map->GetCamera()) {
			EnemySprite* enemy = reinterpret_cast<EnemySprite*>(this);

			if (enemy && enemy->IsHostile() && map->AttackAllowed()) {
				_StartBattleEncounter(enemy);
				return;
			}
		}

		// When being blocked and moving diagonally, the npc is stuck.
		if (moving_diagonally)
			return;

		if (!collision_object) // Should never happen
			return;

		// Try a diagonal to avoid the sprite in straight direction by comparing
		// each one coords.
		float diff_x = GetXPosition() - collision_object->GetXPosition();
		float diff_y = GetYPosition() - collision_object->GetYPosition();
		if (direction & (NORTH | SOUTH))
			direction |= diff_x >= 0.0f ? EAST : WEST;
		else if (direction & (EAST | WEST))
			direction |= diff_y >= 0.0f ? SOUTH : NORTH;
			return;
	}

	// Inform the overlay system of the parallax movement done if needed
	if (this == map->GetCamera()) {
		float x_parallax = !map->IsCameraXAxisInMapCorner() ?
			(GetXPosition() - next_pos_x) / SCREEN_GRID_X_LENGTH * VIDEO_STANDARD_RES_WIDTH :
			0.0f;
		float y_parallax = !map->IsCameraYAxisInMapCorner() ?
			(next_pos_y - GetYPosition()) / SCREEN_GRID_Y_LENGTH * VIDEO_STANDARD_RES_HEIGHT :
			0.0f;

		map->GetEffectSupervisor().AddParallax(x_parallax, y_parallax);
	}

	// Make the sprite advance at the end
	SetPosition(next_pos_x, next_pos_y);
	moved_position = true;
}


void VirtualSprite::SetDirection(uint16 dir) {
	// Nothing complicated needed for lateral directions
	if (dir & (NORTH | SOUTH | EAST | WEST)) {
		direction = dir;
	}
	// Otherwise if the direction is diagonal we must figure out which way the sprite should face.
	else if (dir & MOVING_NORTHWEST) {
		if (direction & (FACING_NORTH | FACING_EAST))
			direction = NW_NORTH;
		else
			direction = NW_WEST;
	}
	else if (dir & MOVING_SOUTHWEST) {
		if (direction & (FACING_SOUTH | FACING_EAST))
			direction = SW_SOUTH;
		else
			direction = SW_WEST;
	}
	else if (dir & MOVING_NORTHEAST) {
		if (direction & (FACING_NORTH | FACING_WEST))
			direction = NE_NORTH;
		else
			direction = NE_EAST;
	}
	else if (dir & MOVING_SOUTHEAST) {
		if (direction & (FACING_SOUTH | FACING_WEST))
			direction = SE_SOUTH;
		else
			direction = SE_EAST;
	}
	else {
		IF_PRINT_WARNING(MAP_DEBUG) << "attempted to set an invalid direction: " << dir << endl;
	}
} // void VirtualSprite::SetDirection(uint16 dir)



void VirtualSprite::SetRandomDirection() {
	switch (RandomBoundedInteger(1, 8)) {
		case 1:
			SetDirection(NORTH);
			break;
		case 2:
			SetDirection(SOUTH);
			break;
		case 3:
			SetDirection(EAST);
			break;
		case 4:
			SetDirection(WEST);
			break;
		case 5:
			SetDirection(MOVING_NORTHEAST);
			break;
		case 6:
			SetDirection(MOVING_NORTHWEST);
			break;
		case 7:
			SetDirection(MOVING_SOUTHEAST);
			break;
		case 8:
			SetDirection(MOVING_SOUTHWEST);
			break;
		default:
			IF_PRINT_WARNING(MAP_DEBUG) << "invalid randomized direction was chosen" << endl;
	}
}

void VirtualSprite::LookAt(const MapPosition& pos) {

	// If the two positions are the same,
	// don't update the direction since it's only a matter of keeping
	// the previous one.
	if (position.x == pos.x && position.y == pos.y)
		return;

	// First handle simple cases
	if (IsFloatEqual(position.x, pos.x, 0.5f))
	{
		if (position.y > pos.y)
			SetDirection(NORTH);
		else
			SetDirection(SOUTH);
		 return;
	}

	if (IsFloatEqual(position.y, pos.y, 0.5f))
	{
		if (position.x > pos.x)
			SetDirection(WEST);
		 else
			SetDirection(EAST);
		 return;
	}

	// Now let's handle diagonal cases
	// First, find the lower angle:
	if (position.x < pos.x)
	{
		// Up-right direction
		if (position.y > pos.y)
		{
			// Compute tan of the angle
			if ((position.y - pos.y) / (pos.x - position.x) < 1)
				// The angle is less than 45°, look to the right
				SetDirection(EAST);
			else
				SetDirection(NORTH);
			return;
		}
		else // Down-right
		{
			// Compute tan of the angle
			if ((pos.y - position.y) / (pos.x - position.x) < 1)
				// The angle is less than 45°, look to the right
				SetDirection(EAST);
			else
				SetDirection(SOUTH);
			return;
		}
	}
	else
	{
		// Up-left direction
		if (position.y > pos.y)
		{
			// Compute tan of the angle
			if ((position.y - pos.y) / (position.x - pos.x) < 1)
				// The angle is less than 45°, look to the left
				SetDirection(WEST);
			else
				SetDirection(NORTH);
			return;
		}
		else // Down-left
		{
			// Compute tan of the angle
			if ((pos.y - position.y) / (position.x - pos.x) < 1)
				// The angle is less than 45°, look to the left
				SetDirection(WEST);
			else
				SetDirection(SOUTH);
			return;
		}
	}
}

float VirtualSprite::CalculateDistanceMoved() {
	float distance_moved = static_cast<float>(SystemManager->GetUpdateTime()) / movement_speed;

	// Double the distance to move if the sprite is running
	if (is_running == true)
		distance_moved *= 2.0f;
	// If the movement is diagonal, decrease the lateral movement distance by sin(45 degress)
	if (direction & MOVING_DIAGONALLY)
		distance_moved *= 0.707f;

	return distance_moved;
}



void VirtualSprite::AcquireControl(SpriteEvent* event) {
	if (event == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "function argument was NULL" << endl;
		return;
	}

	if (control_event != NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "a new event is acquiring control when the previous event has not "
			"released control over this sprite, object id: " << GetObjectID() << endl;
	}
	control_event = event;
}



void VirtualSprite::ReleaseControl(SpriteEvent* event) {
	if (event == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "function argument was NULL" << endl;
		return;
	}

	if (control_event == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "no event had control over this sprite, object id: " << GetObjectID() << endl;
	}
	else if (control_event != event) {
		IF_PRINT_WARNING(MAP_DEBUG) << "a different event has control of this sprite, object id: " << GetObjectID() << endl;
	}
	else {
		control_event = NULL;
	}
}



void VirtualSprite::SaveState() {
	_state_saved = true;
	_saved_direction = direction;
	_saved_movement_speed = movement_speed;
	_saved_moving = moving;
	MapMode::CurrentInstance()->GetEventSupervisor()->PauseAllEvents(this);
}



void VirtualSprite::RestoreState() {
	if (_state_saved == false)
		IF_PRINT_WARNING(MAP_DEBUG) << "restoring state when no saved state was detected" << endl;

	_state_saved = false;
	direction = _saved_direction;
	movement_speed = _saved_movement_speed;
	moving = _saved_moving;
	MapMode::CurrentInstance()->GetEventSupervisor()->ResumeAllEvents(this);
}


void VirtualSprite::_StartBattleEncounter(EnemySprite* enemy) {

	// Start a map-to-battle transition animation sequence
	BattleMode* BM = new BattleMode();

	string battle_background = enemy->GetBattleBackground();
	if (!battle_background.empty())
		BM->GetMedia().SetBackgroundImage(battle_background);

	string enemy_battle_music = enemy->GetBattleMusicTheme();
	if (!enemy_battle_music.empty())
		BM->GetMedia().SetBattleMusic(enemy_battle_music);

	const vector<uint32>& enemy_party = enemy->RetrieveRandomParty();
	for (uint32 i = 0; i < enemy_party.size(); ++i) {
		BM->AddEnemy(enemy_party[i]);
	}

	std::vector<std::string> enemy_battle_scripts = enemy->GetBattleScripts();
	if (!enemy_battle_scripts.empty())
		BM->GetScriptSupervisor().SetScripts(enemy_battle_scripts);

	TransitionToBattleMode *TM = new TransitionToBattleMode(BM);

	// Make the enemy disappear, after creating the transition mode.
	enemy->ChangeStateDead();

	ModeManager->Push(TM);
}

// ****************************************************************************
// ********** MapSprite class methods
// ****************************************************************************

MapSprite::MapSprite() :
	_face_portrait(0),
	_has_running_animations(false),
	_current_anim_direction(ANIM_SOUTH),
	_current_custom_animation(0),
	_next_dialogue(-1),
	_has_available_dialogue(false),
	_has_unseen_dialogue(false),
	_custom_animation_on(false),
	_custom_animation_time(0),
	_saved_current_anim_direction(ANIM_SOUTH)
{
	MapObject::_object_type = SPRITE_TYPE;

	// Points the current animation to the standing animation vector by default
	_animation = &_standing_animations;
}

MapSprite::~MapSprite() {
	if (_face_portrait)
		delete _face_portrait;
}

bool _LoadAnimations(std::vector<hoa_video::AnimatedImage>& animations, const std::string& filename) {
	// Prepare to add the animations for each directions, if needed.
	if (animations.empty()) {
		for (uint8 i = 0; i < NUM_ANIM_DIRECTIONS; ++i)
			animations.push_back(AnimatedImage());
	}

	hoa_script::ReadScriptDescriptor animations_script;
	if (!animations_script.OpenFile(filename))
		return false;

	if (!animations_script.DoesTableExist("sprite_animation")) {
		PRINT_WARNING << "No 'sprite_animation' table in 4-direction animation script file: " << filename << std::endl;
		animations_script.CloseFile();
		return false;
	}

	animations_script.OpenTable("sprite_animation");

	std::string image_filename = animations_script.ReadString("image_filename");

	if (!hoa_utils::DoesFileExist(image_filename)) {
		PRINT_WARNING << "The image file doesn't exist: " << image_filename << std::endl;
		animations_script.CloseTable();
		animations_script.CloseFile();
		return false;
	}
	uint32 rows = animations_script.ReadUInt("rows");
	uint32 columns = animations_script.ReadUInt("columns");

	if (!animations_script.DoesTableExist("frames")) {
		animations_script.CloseAllTables();
		animations_script.CloseFile();
		PRINT_WARNING << "No frame table in file: " << filename << std::endl;
		return false;
	}

	std::vector<StillImage> image_frames;
	// Load the image data
	if (!ImageDescriptor::LoadMultiImageFromElementGrid(image_frames, image_filename, rows, columns)) {
		PRINT_WARNING << "Couldn't load elements from image file: " << image_filename
					  << " (in file: " << filename << ")" << std::endl;
		animations_script.CloseAllTables();
		animations_script.CloseFile();
		return false;
	}

	std::vector <uint32> frames_directions_ids;
	animations_script.ReadTableKeys("frames", frames_directions_ids);

	// open the frames table
	animations_script.OpenTable("frames");

	for (uint32 i = 0; i < frames_directions_ids.size(); ++i) {
		if (frames_directions_ids[i] >= NUM_ANIM_DIRECTIONS) {
			PRINT_WARNING << "Invalid direction id(" << frames_directions_ids[i]
				<< ") in file: " << filename << std::endl;
			continue;
		}

		uint32 anim_direction = frames_directions_ids[i];

		// Opens frames[ANIM_DIRECTION]
		animations_script.OpenTable(anim_direction);

		// Loads the frames data
		std::vector<uint32> frames_ids;
		std::vector<uint32> frames_duration;

		uint32 num_frames = animations_script.GetTableSize();
		for (uint32 frames_table_id = 0;  frames_table_id < num_frames; ++frames_table_id) {
			// Opens frames[ANIM_DIRECTION][frame_table_id]
			animations_script.OpenTable(frames_table_id);

			int32 frame_id = animations_script.ReadInt("id");
			int32 frame_duration = animations_script.ReadInt("duration");

			if (frame_id < 0 || frame_duration < 0 || frame_id >= (int32)image_frames.size()) {
				PRINT_WARNING << "Invalid frame (" << frames_table_id << ") in file: "
							  << filename << std::endl;
				PRINT_WARNING << "Request for frame id: " << frame_id << ", duration: "
							  << frame_duration << " is not possible." << std::endl;
				continue;
			}

			frames_ids.push_back((uint32)frame_id);
			frames_duration.push_back((uint32)frame_duration);

			animations_script.CloseTable(); // frames[ANIM_DIRECTION][frame_table_id] table
		}

		// Actually create the animation data
		animations[anim_direction].Clear();
		animations[anim_direction].ResetAnimation();
		for (uint32 j = 0; j < frames_ids.size(); ++j) {
			// Set the dimension of the requested frame
			animations[anim_direction].AddFrame(image_frames[frames_ids[j]], frames_duration[j]);
		}

		// Closes frames[ANIM_DIRECTION]
		animations_script.CloseTable();

	} // for each directions

	// Close the 'frames' table and set the dimensions
	animations_script.CloseTable();

	float frame_width = animations_script.ReadFloat("frame_width");
	float frame_height = animations_script.ReadFloat("frame_height");

	// Load requested dimensions
	for (uint8 i = 0; i < NUM_ANIM_DIRECTIONS; ++i) {
		if (frame_width > 0.0f && frame_height > 0.0f) {
			animations[i].SetDimensions(frame_width, frame_height);
		}
		else if (IsFloatEqual(animations[i].GetWidth(), 0.0f) && IsFloatEqual(animations[i].GetHeight(), 0.0f)) {
			// If the animation dimensions are not set, we're using the first frame size.
			animations[i].SetDimensions(image_frames.begin()->GetWidth(), image_frames.begin()->GetHeight());
		}

		// Rescale to fit the map mode coordinates system.
		MapMode::ScaleToMapCoords(animations[i]);
	}

	animations_script.CloseTable(); // sprite_animation table
	animations_script.CloseFile();

	return true;
} // bool _LoadAnimations()

bool MapSprite::LoadStandingAnimations(const std::string& filename) {
	return _LoadAnimations(_standing_animations, filename);
}

bool MapSprite::LoadWalkingAnimations(const std::string& filename) {
	return _LoadAnimations(_walking_animations, filename);
}

bool MapSprite::LoadRunningAnimations(const std::string& filename) {
	_has_running_animations = _LoadAnimations(_running_animations, filename);

	return _has_running_animations;
}

bool MapSprite::LoadCustomAnimation(const std::string& animation_name, const std::string& filename) {
	if (_custom_animations.find(animation_name) != _custom_animations.end()) {
		PRINT_WARNING << "The animation " << animation_name << " is already existing." << std::endl;
		return false;
	}

	AnimatedImage animation;
	if (animation.LoadFromAnimationScript(filename)) {
		MapMode::ScaleToMapCoords(animation);
		_custom_animations.insert(std::make_pair(animation_name, animation));
		return true;
	}

	return false;
} // bool MapSprite::LoadCustomAnimations()

void MapSprite::SetCustomAnimation(const std::string& animation_name, uint32 time) {
	// If there is no key, there will be no custom animation to display
	if (animation_name.empty()) {
		_custom_animation_on = false;
		return;
	}

	// Same if the key isn't found
	std::map<std::string, AnimatedImage>::iterator it = _custom_animations.find(animation_name);
	if (it == _custom_animations.end()) {
		_custom_animation_on = false;
		return;
	}

	AnimatedImage& animation = it->second;
	animation.ResetAnimation();
	if (time == 0) {
		time = animation.GetAnimationLength();
	}
	// Still check the animation length
	if (time == 0){
		_custom_animation_on = false;
		return;
	}

	_custom_animation_time = (int32)time;
	_current_custom_animation = &animation;
	_custom_animation_on = true;
}

void MapSprite::LoadFacePortrait(const std::string& filename) {
	if (_face_portrait)
		delete _face_portrait;

	_face_portrait = new StillImage();
	if (!_face_portrait->Load(filename)) {
		delete _face_portrait;
		_face_portrait = 0;
		PRINT_ERROR << "failed to load face portrait" << std::endl;
	}
}

void MapSprite::Update() {
	// Stores the last value of moved_position to determine when a change in sprite movement between calls to this function occurs
	static bool was_moved = moved_position;

	// This call will update the sprite's position and perform collision detection
	VirtualSprite::Update();

	// if it's a custom animation, just display that and ignore everything else
	if (_custom_animation_on && _current_custom_animation) {
		// Check whether the custom animation can be freed
		if (_custom_animation_time <= 0) {
			_custom_animation_on = false;
			_current_custom_animation = 0;
			_custom_animation_time = 0;
		}
		else {
			_custom_animation_time -= SystemManager->GetUpdateTime();
			_current_custom_animation->Update();
		}

		was_moved = moved_position;
		return;
	}

	// Save the previous animation state
	uint8 last_anim_direction = _current_anim_direction;
	std::vector<AnimatedImage>* last_animation = _animation;

	// Set the sprite's animation to the standing still position if movement has just stopped
	if (!moved_position) {
		// Set the current movement animation to zero progress
		if (was_moved)
			_animation->at(_current_anim_direction).SetTimeProgress(0);

		// Determine the correct standing frame to display
		if (!control_event || _state_saved) {
			_animation = &_standing_animations;

			if (direction & FACING_NORTH) {
				_current_anim_direction = ANIM_NORTH;
			}
			else if (direction & FACING_SOUTH) {
				_current_anim_direction = ANIM_SOUTH;
			}
			else if (direction & FACING_WEST) {
				_current_anim_direction = ANIM_WEST;
			}
			else if (direction & FACING_EAST) {
				_current_anim_direction = ANIM_EAST;
			}
			else {
				PRINT_ERROR << "invalid sprite direction, could not find proper standing animation to draw" << std::endl;
			}
		}
	}

	else { // then (moved_position)
		// Determine the correct animation to display
		if (direction & FACING_NORTH) {
			_current_anim_direction = ANIM_NORTH;
		}
		else if (direction & FACING_SOUTH) {
			_current_anim_direction = ANIM_SOUTH;
		}
		else if (direction & FACING_WEST) {
			_current_anim_direction = ANIM_WEST;
		}
		else if (direction & FACING_EAST) {
			_current_anim_direction = ANIM_EAST;
		}
		else {
			PRINT_ERROR << "invalid sprite direction, could not find proper standing animation to draw" << std::endl;
		}

		// Increasing the animation index by four from the walking _animations leads to the running _animations
		if (is_running && _has_running_animations) {
			_animation = &_running_animations;
		}
		else {
			_animation = &_walking_animations;
		}
	}

	// If the direction of movement changed in mid-flight, update the animation timer on the
	// new animated image to reflect the old, so the walking _animations do not appear to
	// "start and stop" whenever the direction is changed.
	if (last_anim_direction != _current_anim_direction || last_animation != _animation) {
		_animation->at(_current_anim_direction).SetTimeProgress(last_animation->at(last_anim_direction).GetTimeProgress());
		last_animation->at(last_anim_direction).SetTimeProgress(0);
	}

	_animation->at(_current_anim_direction).Update();

	was_moved = moved_position;
} // void MapSprite::Update()

void MapSprite::_DrawDebugInfo() {
	// Draw collision rectangle if the debug view is on.
	float x, y = 0.0f;
	VideoManager->GetDrawPosition(x, y);
	MapRectangle rect = GetCollisionRectangle(x, y);
	VideoManager->DrawRectangle(rect.right - rect.left, rect.bottom - rect.top, Color(0.0f, 0.0f, 1.0f, 0.6f));

	// Show a potential active path
	if (control_event && control_event->GetEventType() == PATH_MOVE_SPRITE_EVENT) {
		PathMoveSpriteEvent *path_event = (PathMoveSpriteEvent*)control_event;
		if (path_event) {
			Path path = path_event->GetPath();
			MapMode *map = MapMode::CurrentInstance();
			for (uint32 i = 0; i < path.size(); ++i) {
				float x_pos = path[i].x - map->GetMapFrame().screen_edges.left;
				float y_pos = path[i].y - map->GetMapFrame().screen_edges.top;
				VideoManager->Move(x_pos, y_pos);

				VideoManager->DrawRectangle(0.2, 0.2f, Color(0.0f, 1.0f, 1.0f, 0.6f));
			}
		}
	}
}

void MapSprite::Draw() {
	if (MapObject::ShouldDraw()) {
		if (_custom_animation_on && _current_custom_animation)
			_current_custom_animation->Draw();
		else
			_animation->at(_current_anim_direction).Draw();

		if (VideoManager->DebugInfoOn())
			_DrawDebugInfo();
	}
}

void MapSprite::DrawDialog()
{
    // Update the alpha of the dialogue icon according to it's distance from the player sprite
	const float DIALOGUE_ICON_VISIBLE_RANGE = 10.0f;

    if (MapObject::ShouldDraw()) {
		MapMode *map = MapMode::CurrentInstance();
        if (_has_available_dialogue && _has_unseen_dialogue
				&& map->IsShowGUI() && !map->IsCameraOnVirtualFocus()) {
			Color icon_color(1.0f, 1.0f, 1.0f, 0.0f);
			float icon_alpha = 1.0f - (fabs(GetXPosition() - map->GetCamera()->GetXPosition())
				+ fabs(GetYPosition() - map->GetCamera()->GetYPosition())) / DIALOGUE_ICON_VISIBLE_RANGE;

			if (icon_alpha < 0.0f)
				icon_alpha = 0.0f;
			icon_color.SetAlpha(icon_alpha);

			VideoManager->MoveRelative(0, -GetImgHeight());
			map->GetDialogueIcon().Draw(icon_color);
        }
	}
}

void MapSprite::AddDialogueReference(uint32 dialogue_id) {
	_dialogue_references.push_back(dialogue_id);
    UpdateDialogueStatus();
	// TODO: The call above causes a warning to be printed out if the sprite has been created but the dialogue has not yet.
	// Map scripts typically create all sprites first (including their dialogue references) before creating the dialogues.
	// We need a safe way to add dialogue references to the sprite without causing these warnings to be printed when the
	// map is loading.
}

void MapSprite::ClearDialogueReferences() {
    _dialogue_references.clear();
    UpdateDialogueStatus();
}

void MapSprite::RemoveDialogueReference(uint32 dialogue_id) {
    // Remove all dialogues with the given reference (for the case, the same dialogue was add several times)
    for (uint32 i = 0; i < _dialogue_references.size(); i++) {
        if (_dialogue_references[i] == dialogue_id)
            _dialogue_references.erase(_dialogue_references.begin()+i);
    }
    UpdateDialogueStatus();
}

void MapSprite::InitiateDialogue() {
	if (_dialogue_references.empty() == true) {
		IF_PRINT_WARNING(MAP_DEBUG) << "sprite: " << object_id << " has no dialogue referenced" << endl;
		return;
	}

	SaveState();
	moving = false;
	SetDirection(CalculateOppositeDirection(MapMode::CurrentInstance()->GetCamera()->GetDirection()));
	MapMode::CurrentInstance()->GetDialogueSupervisor()->BeginDialogue(_dialogue_references[_next_dialogue]);
	IncrementNextDialogue();
}

void MapSprite::UpdateDialogueStatus() {
	_has_available_dialogue = false;
	_has_unseen_dialogue = false;

	for (uint32 i = 0; i < _dialogue_references.size(); i++) {
		SpriteDialogue* dialogue = MapMode::CurrentInstance()->GetDialogueSupervisor()->GetDialogue(_dialogue_references[i]);
		if (dialogue == NULL) {
			IF_PRINT_WARNING(MAP_DEBUG) << "sprite: " << object_id << " is referencing unknown dialogue: " << _dialogue_references[i] << endl;
			continue;
		}

		if (dialogue->IsAvailable()) {
			_has_available_dialogue = true;
			if (_next_dialogue < 0)
				_next_dialogue = i;
		}
		if (dialogue->HasAlreadySeen() == false) {
			_has_unseen_dialogue = true;
		}
	}

	// TODO: if the sprite has available, unseen dialogue and the _next_dialogue pointer is pointing to a dialogue that is already seen, change it
	// to point to the unseen available dialogue
}

void MapSprite::IncrementNextDialogue() {
	// Handle the case where no dialogue is referenced by the sprite
	if (_next_dialogue < 0) {
		IF_PRINT_WARNING(MAP_DEBUG) << "function invoked when no dialogues were referenced by the sprite" << endl;
		return;
	}

	int16 last_dialogue = _next_dialogue;

	while (true) {
		_next_dialogue++;
		if (static_cast<uint16>(_next_dialogue) >= _dialogue_references.size())
			_next_dialogue = 0;

		SpriteDialogue* dialogue = MapMode::CurrentInstance()->GetDialogueSupervisor()->GetDialogue(_dialogue_references[_next_dialogue]);
		if (dialogue != NULL && dialogue->IsAvailable() == true) {
			return;
		}
		// If this case occurs, all dialogues are now unavailable
		else if (_next_dialogue == last_dialogue) {
			IF_PRINT_WARNING(MAP_DEBUG) << "all referenced dialogues are now unavailable for this sprite" << endl;
			_has_available_dialogue = false;
			_has_unseen_dialogue = false;
			return;
		}
	}
}

void MapSprite::SetNextDialogue(uint16 next) {
	// If a negative value is passed in, this means the user wants to disable
	if (next >= _dialogue_references.size()) {
		IF_PRINT_WARNING(MAP_DEBUG) << "tried to set _next_dialogue to an value that was invalid (exceeds maximum bounds): " << next << endl;
	}
	else {
		_next_dialogue = static_cast<int16>(next);
	}
}

void MapSprite::SaveState() {
	VirtualSprite::SaveState();

	_saved_current_anim_direction = _current_anim_direction;
}

void MapSprite::RestoreState() {
	VirtualSprite::RestoreState();

	_current_anim_direction = _saved_current_anim_direction;
}

// *****************************************************************************
// ********** EnemySprite class methods
// *****************************************************************************

EnemySprite::EnemySprite() :
	_zone(NULL),
	_color(1.0f, 1.0f, 1.0f, 0.0f),
	_aggro_range(8.0f),
	_time_dir_change(2500),
	_time_to_spawn(3500)
{
	_filename = "";
	MapObject::_object_type = ENEMY_TYPE;
	moving = true;
	Reset();
}

EnemySprite::EnemySprite(const std::string& file) :
	_zone(NULL),
	_color(1.0f, 1.0f, 1.0f, 0.0f),
	_aggro_range(8.0f),
	_time_dir_change(2500),
	_time_to_spawn(3500)
{
	_filename = file;
	MapObject::_object_type = ENEMY_TYPE;
	moving = true;
	Reset();
}
// Load in the appropriate images and other data for the sprite from a Lua file
bool EnemySprite::Load() {
	ReadScriptDescriptor sprite_script;
	if (!sprite_script.OpenFile(_filename))
		return false;

	ScriptCallFunction<void>(sprite_script.GetLuaState(), "Load", this);
	return true;
}

void EnemySprite::Reset() {
	updatable = false;
	no_collision = true;
	_state = DEAD;
	_time_elapsed = 0;
	_color.SetAlpha(0.0f);
	_out_of_zone = false;
}



void EnemySprite::AddEnemy(uint32 enemy_id) {
	if (_enemy_parties.empty()) {
		IF_PRINT_WARNING(MAP_DEBUG) << "can not add new enemy when no parties have been declared" << endl;
		return;
	}

	_enemy_parties.back().push_back(enemy_id);

	// Make sure that the GlobalEnemy has already been created for this enemy_id
	if (MAP_DEBUG) {
		if (MapMode::CurrentInstance()->IsEnemyLoaded(enemy_id) == false) {
			PRINT_WARNING << "enemy to add has id " << enemy_id << ", which does not exist in MapMode::_enemies" << endl;
		}
	}
}

static std::vector<uint32> empty_enemy_party;

const std::vector<uint32>& EnemySprite::RetrieveRandomParty() {
	if (_enemy_parties.empty()) {
		PRINT_ERROR << "No enemy parties exist and none can be created." << std::endl;
		return empty_enemy_party;
	}

	return _enemy_parties[rand() % _enemy_parties.size()];
}

void EnemySprite::Update() {
	switch (_state) {
		// Gradually increase the alpha while the sprite is fading in during spawning
		case SPAWNING:
			_time_elapsed += SystemManager->GetUpdateTime();
			if (_color.GetAlpha() < 1.0f) {
				_color.SetAlpha((_time_elapsed / static_cast<float>(_time_to_spawn)) * 1.0f);
			}
			else {
				ChangeStateHostile();
			}
			break;

		// Set the sprite's direction so that it seeks to collide with the map camera's position
		case HOSTILE:
		{
			// Holds the x and y deltas between the sprite and map camera coordinate pairs
			float xdelta, ydelta;
			VirtualSprite *camera = MapMode::CurrentInstance()->GetCamera();
			float camera_x = camera->GetXPosition();
			float camera_y = camera->GetYPosition();
			_time_elapsed += SystemManager->GetUpdateTime();

			xdelta = GetXPosition() - camera_x;
			ydelta = GetYPosition() - camera_y;

			// If the sprite has moved outside of its zone and it should not, reverse the sprite's direction
			if ( _zone != NULL && _zone->IsInsideZone(GetXPosition(), GetYPosition()) == false
					&& _zone->IsRoamingRestrained() ) {
				// Make sure it wasn't already out (stuck on boundaries fix)
				if( !_out_of_zone )
				{
					SetDirection(CalculateOppositeDirection(GetDirection()));
					// The sprite is now finding its way back into the zone
					_out_of_zone = true;
				}
			}
			// Otherwise, determine the direction that the sprite should move if the camera is within the sprite's aggression range
			else {
				_out_of_zone = false;

				// Enemies will only aggro if the camera is inside the zone, or the zone is non-restrictive
				// The order of comparaisons here is important, the NULL check MUST come before the rest or a null pointer exception could happen if no zone is registered
				if ( MapMode::CurrentInstance()->AttackAllowed()
						&& (_zone == NULL || ( fabs(xdelta) <= _aggro_range && fabs(ydelta) <= _aggro_range
						&& (!_zone->IsRoamingRestrained() || _zone->IsInsideZone(camera_x, camera_y)) )) )
				{
					if (xdelta > -0.5 && xdelta < 0.5 && ydelta < 0)
						SetDirection(SOUTH);
					else if (xdelta > -0.5 && xdelta < 0.5 && ydelta > 0)
						SetDirection(NORTH);
					else if (ydelta > -0.5 && ydelta < 0.5 && xdelta > 0)
						SetDirection(WEST);
					else if (ydelta > -0.5 && ydelta < 0.5 && xdelta < 0)
						SetDirection(EAST);
					else if (xdelta < 0 && ydelta < 0)
						SetDirection(MOVING_SOUTHEAST);
					else if (xdelta < 0 && ydelta > 0)
						SetDirection(MOVING_NORTHEAST);
					else if (xdelta > 0 && ydelta < 0)
						SetDirection(MOVING_SOUTHWEST);
					else
						SetDirection(MOVING_NORTHWEST);
				}
				// If the sprite is not within the aggression range, pick a random direction to move
				else {
					if (_time_elapsed >= GetTimeToChange()) {
						// TODO: needs comment
						SetDirection(1 << hoa_utils::RandomBoundedInteger(0,11));
						_time_elapsed = 0;
					}
				}
			}

			MapSprite::Update();
			break;
		}
		// Do nothing if the sprite is in the DEAD state, or any other state
		case DEAD:
		default:
			break;
	}
} // void EnemySprite::Update()

void EnemySprite::Draw() {
	// Otherwise, only draw it if it is not in the DEAD state
	if (MapObject::ShouldDraw() == true && _state != DEAD) {
		_animation->at(_current_anim_direction).Draw(_color);

		// Draw collision rectangle if the debug view is on.
		if (VideoManager->DebugInfoOn()) {
			float x, y = 0.0f;
			VideoManager->GetDrawPosition(x, y);
			MapRectangle rect = GetCollisionRectangle(x, y);
			VideoManager->DrawRectangle(rect.right - rect.left, rect.bottom - rect.top, Color(1.0f, 0.0f, 0.0f, 0.6f));
		}
	}
}

} // namespace private_map

} // namespace hoa_map
