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

// Allacrost utilities
#include "utils.h"

// Allacrost engines
#include "audio.h"
#include "mode_manager.h"
#include "system.h"

// Allacrost globals
#include "global.h"

// Local map mode headers
#include "map.h"
#include "map_sprites.h"
#include "map_objects.h"
#include "map_dialogue.h"
#include "map_events.h"

// Other game mode headers
#include "battle.h"

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

	if (!updatable) {
		return;
	}

	// Determine if a movement event is controlling the sprite.
	if (moving == false) {
		return;
// 		if (control_event != NULL) {
// 			EVENT_TYPE event_type = control_event->GetEventType();
// 			if (event_type == PATH_MOVE_SPRITE_EVENT || event_type == RANDOM_MOVE_SPRITE_EVENT) {
// 				moving = true;
// 			}
// 		}
// 		// If the sprite still isn't moving, there's nothing more to update here
// 		if (moving == false) {
// 			return;
// 		}
	}

	// Save the previous sprite's position temporarily
	float tmp_x = x_offset;
	float tmp_y = y_offset;

	float distance_moved = CalculateDistanceMoved();

	// Move the sprite the appropriate distance in the appropriate Y and X direction
	if (direction & (NORTH | MOVING_NORTHWEST | MOVING_NORTHEAST))
		y_offset -= distance_moved;
	else if (direction & (SOUTH | MOVING_SOUTHWEST | MOVING_SOUTHEAST))
		y_offset += distance_moved;
	if (direction & (WEST | MOVING_NORTHWEST | MOVING_SOUTHWEST))
		x_offset -= distance_moved;
	else if (direction & (EAST | MOVING_NORTHEAST | MOVING_SOUTHEAST))
		x_offset += distance_moved;

	MapObject* collision_object = NULL;
	COLLISION_TYPE collision_type = NO_COLLISION;
	collision_type = MapMode::CurrentInstance()->GetObjectSupervisor()->DetectCollision(this, &collision_object);

	if (collision_type == NO_COLLISION) {
		CheckPositionOffsets();
		moved_position = true;
	}
	else {
		// Restore the sprite's position. The _ResolveCollision() call that follows may find an alternative
		// position to move the sprite to.
		x_offset = tmp_x;
		y_offset = tmp_y;

		_ResolveCollision(collision_type, collision_object);
	}
} // void VirtualSprite::Update()



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
	if (control_event != NULL)
		MapMode::CurrentInstance()->GetEventSupervisor()->PauseEvent(control_event->GetEventID());
}



void VirtualSprite::RestoreState() {
	if (_state_saved == false)
		IF_PRINT_WARNING(MAP_DEBUG) << "restoring state when no saved state was detected" << endl;

	_state_saved = false;
	 direction = _saved_direction;
	 movement_speed = _saved_movement_speed;
	 moving = _saved_moving;
	 if (control_event != NULL)
		MapMode::CurrentInstance()->GetEventSupervisor()->ResumeEvent(control_event->GetEventID());
}



void VirtualSprite::_ResolveCollision(COLLISION_TYPE coll_type, MapObject* coll_obj) {
	// ---------- (1) First check for the case where the player has collided with a hostile enemy sprite
	if (coll_obj != NULL) {
		EnemySprite* enemy = NULL;
		if (this == MapMode::CurrentInstance()->GetCamera() && coll_obj->GetType() == ENEMY_TYPE) {
			enemy = reinterpret_cast<EnemySprite*>(coll_obj);
		}
		else if (coll_obj == MapMode::CurrentInstance()->GetCamera() && this->GetType() == ENEMY_TYPE) {
			enemy = reinterpret_cast<EnemySprite*>(this);
		}

		// If these two conditions are true, begin the battle
		if (enemy != NULL && enemy->IsHostile() && MapMode::CurrentInstance()->AttackAllowed()) {
			enemy->ChangeStateDead();

			BattleMode *BM = new BattleMode();

			string battle_background = enemy->GetBattleBackground();
			if (battle_background != "")
				BM->GetMedia().SetBackgroundImage(battle_background);

			string enemy_battle_music = enemy->GetBattleMusicTheme();
			if (enemy_battle_music != "")
				BM->GetMedia().SetBattleMusic(enemy_battle_music);

			const vector<uint32>& enemy_party = enemy->RetrieveRandomParty();
			for (uint32 i = 0; i < enemy_party.size(); i++) {
				BM->AddEnemy(enemy_party[i]);
			}

			string enemy_battle_script = enemy->GetBattleScript();
			if (enemy_battle_script != "")
				BM->LoadBattleScript(enemy_battle_script);

			ModeManager->Push(BM);

			// TODO: some sort of map-to-battle transition animation sequence needs to start here
			return;
		}
	}

	// ---------- (2) Adjust the sprite's position if no event was controlling this sprite
	// This sprite is assumed in this case to be controlled by the player since sprites don't move by themselves
	if (control_event == NULL) {
		MapMode::CurrentInstance()->GetObjectSupervisor()->AdjustSpriteAroundCollision(this, coll_type, coll_obj);
		return;
	}

	// ---------- (3) Call the appropriate collision resolution function for the various control events
	EVENT_TYPE event_type = control_event->GetEventType();
	if (event_type == PATH_MOVE_SPRITE_EVENT) {
		PathMoveSpriteEvent* path_event = dynamic_cast<PathMoveSpriteEvent*>(control_event);
		path_event->_ResolveCollision(coll_type, coll_obj);
	}
	else if (event_type == RANDOM_MOVE_SPRITE_EVENT) {
		RandomMoveSpriteEvent* random_event = dynamic_cast<RandomMoveSpriteEvent*>(control_event);
		random_event->_ResolveCollision(coll_type, coll_obj);
	}
	else {
		IF_PRINT_WARNING(MAP_DEBUG) << "collision occurred when sprite was controlled by a non-motion event" << endl;
	}
} // void VirtualSprite::_ResolveCollision(COLLISION_TYPE coll_type, MapObject* coll_obj)

// ****************************************************************************
// ********** MapSprite class methods
// ****************************************************************************

MapSprite::MapSprite() :
	_face_portrait(NULL),
	_has_running_animations(false),
	_current_animation(ANIM_STANDING_SOUTH),
	_next_dialogue(-1),
	_has_available_dialogue(false),
	_has_unseen_dialogue(false),
	_custom_animation_on(false),
	_saved_current_animation(0)
{
	MapObject::_object_type = SPRITE_TYPE;
}



MapSprite::~MapSprite() {
	if (_face_portrait != NULL) {
		delete _face_portrait;
		_face_portrait = NULL;
	}
}



bool MapSprite::LoadStandardAnimations(std::string filename) {
	// The speed to display each frame in the walking animation
	uint32 frame_speed = static_cast<uint32>(movement_speed / 10.0f);

	// Prepare the four standing and four walking _animations
	for (uint8 i = 0; i < 8; i++)
		_animations.push_back(AnimatedImage());

	// Load the multi-image, containing 24 frames total
	vector<StillImage> frames(24);
	for (uint8 i = 0; i < 24; i++)
		frames[i].SetDimensions(img_half_width * 2, img_height);

	if (ImageDescriptor::LoadMultiImageFromElementGrid(frames, filename, 4, 6) == false) {
		return false;
	}

	// Add standing frames to _animations
	_animations[ANIM_STANDING_SOUTH].AddFrame(frames[0], frame_speed);
	_animations[ANIM_STANDING_NORTH].AddFrame(frames[6], frame_speed);
	_animations[ANIM_STANDING_WEST].AddFrame(frames[12], frame_speed);
	_animations[ANIM_STANDING_EAST].AddFrame(frames[18], frame_speed);

	// Add walking frames to _animations
	_animations[ANIM_WALKING_SOUTH].AddFrame(frames[1], frame_speed);
	_animations[ANIM_WALKING_SOUTH].AddFrame(frames[2], frame_speed);
	_animations[ANIM_WALKING_SOUTH].AddFrame(frames[3], frame_speed);
	_animations[ANIM_WALKING_SOUTH].AddFrame(frames[1], frame_speed);
	_animations[ANIM_WALKING_SOUTH].AddFrame(frames[4], frame_speed);
	_animations[ANIM_WALKING_SOUTH].AddFrame(frames[5], frame_speed);

	_animations[ANIM_WALKING_NORTH].AddFrame(frames[7], frame_speed);
	_animations[ANIM_WALKING_NORTH].AddFrame(frames[8], frame_speed);
	_animations[ANIM_WALKING_NORTH].AddFrame(frames[9], frame_speed);
	_animations[ANIM_WALKING_NORTH].AddFrame(frames[7], frame_speed);
	_animations[ANIM_WALKING_NORTH].AddFrame(frames[10], frame_speed);
	_animations[ANIM_WALKING_NORTH].AddFrame(frames[11], frame_speed);

	_animations[ANIM_WALKING_WEST].AddFrame(frames[13], frame_speed);
	_animations[ANIM_WALKING_WEST].AddFrame(frames[14], frame_speed);
	_animations[ANIM_WALKING_WEST].AddFrame(frames[15], frame_speed);
	_animations[ANIM_WALKING_WEST].AddFrame(frames[13], frame_speed);
	_animations[ANIM_WALKING_WEST].AddFrame(frames[16], frame_speed);
	_animations[ANIM_WALKING_WEST].AddFrame(frames[17], frame_speed);

	_animations[ANIM_WALKING_EAST].AddFrame(frames[19], frame_speed);
	_animations[ANIM_WALKING_EAST].AddFrame(frames[20], frame_speed);
	_animations[ANIM_WALKING_EAST].AddFrame(frames[21], frame_speed);
	_animations[ANIM_WALKING_EAST].AddFrame(frames[19], frame_speed);
	_animations[ANIM_WALKING_EAST].AddFrame(frames[22], frame_speed);
	_animations[ANIM_WALKING_EAST].AddFrame(frames[23], frame_speed);

	return true;
} // bool MapSprite::LoadStandardAnimations(std::string filename)



bool MapSprite::LoadRunningAnimations(std::string filename) {
	// The speed to display each frame in the running animation
	uint32 frame_speed = static_cast<uint32>(movement_speed / 10.0f);

	// Prepare to add the four running _animations
	for (uint8 i = 0; i < 4; i++)
		_animations.push_back(AnimatedImage());

	// Load the multi-image, containing 24 frames total
	vector<StillImage> frames(24);
	for (uint8 i = 0; i < 24; i++)
		frames[i].SetDimensions(img_half_width * 2, img_height);

	if (ImageDescriptor::LoadMultiImageFromElementGrid(frames, filename, 4, 6) == false) {
		return false;
	}

	// Add walking frames to _animations
	_animations[ANIM_RUNNING_SOUTH].AddFrame(frames[1], frame_speed);
	_animations[ANIM_RUNNING_SOUTH].AddFrame(frames[2], frame_speed);
	_animations[ANIM_RUNNING_SOUTH].AddFrame(frames[3], frame_speed);
	_animations[ANIM_RUNNING_SOUTH].AddFrame(frames[1], frame_speed);
	_animations[ANIM_RUNNING_SOUTH].AddFrame(frames[4], frame_speed);
	_animations[ANIM_RUNNING_SOUTH].AddFrame(frames[5], frame_speed);

	_animations[ANIM_RUNNING_NORTH].AddFrame(frames[7], frame_speed);
	_animations[ANIM_RUNNING_NORTH].AddFrame(frames[8], frame_speed);
	_animations[ANIM_RUNNING_NORTH].AddFrame(frames[9], frame_speed);
	_animations[ANIM_RUNNING_NORTH].AddFrame(frames[7], frame_speed);
	_animations[ANIM_RUNNING_NORTH].AddFrame(frames[10], frame_speed);
	_animations[ANIM_RUNNING_NORTH].AddFrame(frames[11], frame_speed);

	_animations[ANIM_RUNNING_WEST].AddFrame(frames[13], frame_speed);
	_animations[ANIM_RUNNING_WEST].AddFrame(frames[14], frame_speed);
	_animations[ANIM_RUNNING_WEST].AddFrame(frames[15], frame_speed);
	_animations[ANIM_RUNNING_WEST].AddFrame(frames[13], frame_speed);
	_animations[ANIM_RUNNING_WEST].AddFrame(frames[16], frame_speed);
	_animations[ANIM_RUNNING_WEST].AddFrame(frames[17], frame_speed);

	_animations[ANIM_RUNNING_EAST].AddFrame(frames[19], frame_speed);
	_animations[ANIM_RUNNING_EAST].AddFrame(frames[20], frame_speed);
	_animations[ANIM_RUNNING_EAST].AddFrame(frames[21], frame_speed);
	_animations[ANIM_RUNNING_EAST].AddFrame(frames[19], frame_speed);
	_animations[ANIM_RUNNING_EAST].AddFrame(frames[22], frame_speed);
	_animations[ANIM_RUNNING_EAST].AddFrame(frames[23], frame_speed);

	_has_running_animations = true;
	return true;
} // bool MapSprite::LoadRunningAnimations(std::string filename)



bool MapSprite::LoadAttackAnimations(std::string filename) {
	// The speed to display each frame in the walking animation
	uint32 frame_speed = static_cast<uint32>(movement_speed / 10.0f);

	// Prepare the four standing and four walking _animations
	for (uint8 i = 0; i < 8; i++)
		_animations.push_back(AnimatedImage());

	// Load the multi-image, containing 24 frames total
	vector<StillImage> frames(5);
	for (uint8 i = 0; i < 5; i++)
		frames[i].SetDimensions(img_half_width * 4, img_height);

	if (ImageDescriptor::LoadMultiImageFromElementGrid(frames, filename, 1, 5) == false) {
		return false;
	}

	// Add attack frames to _animations
	_animations[ANIM_ATTACKING_EAST].AddFrame(frames[0], frame_speed);
	_animations[ANIM_ATTACKING_EAST].AddFrame(frames[1], frame_speed);
	_animations[ANIM_ATTACKING_EAST].AddFrame(frames[2], frame_speed);
	_animations[ANIM_ATTACKING_EAST].AddFrame(frames[3], frame_speed);
	_animations[ANIM_ATTACKING_EAST].AddFrame(frames[4], frame_speed);

	return true;
} // bool MapSprite::LoadAttackAnimations(std::string filename)



void MapSprite::LoadFacePortrait(std::string pn) {
	if (_face_portrait != NULL) {
		delete _face_portrait;
	}

	_face_portrait = new StillImage();
	if (_face_portrait->Load(pn) == false) {
		delete _face_portrait;
		_face_portrait = NULL;
		PRINT_ERROR << "failed to load face portrait" << endl;
	}
}



void MapSprite::Update() {
	// Stores the last value of moved_position to determine when a change in sprite movement between calls to this function occurs
	static bool was_moved = moved_position;

	// This call will update the sprite's position and perform collision detection
	VirtualSprite::Update();

	// if it's a custom animation, just display that and ignore everything else
	if (_custom_animation_on == true) {
		_animations[_current_animation].Update();
	}
	// Set the sprite's animation to the standing still position if movement has just stopped
	else if (moved_position == false) {
		if (was_moved == true) {
			// Set the current movement animation to zero progress
			_animations[_current_animation].SetTimeProgress(0);
		}

		// Determine the correct standing frame to display
		if (control_event == NULL || _state_saved == true) {
			if (direction & FACING_NORTH) {
				_current_animation = ANIM_STANDING_NORTH;
			}
			else if (direction & FACING_SOUTH) {
				_current_animation = ANIM_STANDING_SOUTH;
			}
			else if (direction & FACING_WEST) {
				_current_animation = ANIM_STANDING_WEST;
			}
			else if (direction & FACING_EAST) {
				_current_animation = ANIM_STANDING_EAST;
			}
			else {
				PRINT_ERROR << "invalid sprite direction, could not find proper standing animation to draw" << endl;
			}
		}
	}

	else { // then (moved_position == true)
		// Save the previous animation
		uint8 last_animation = _current_animation;

		// Determine the correct animation to display
		if (direction & FACING_NORTH) {
			_current_animation = ANIM_WALKING_NORTH;
		}
		else if (direction & FACING_SOUTH) {
			_current_animation = ANIM_WALKING_SOUTH;
		}
		else if (direction & FACING_WEST) {
			_current_animation = ANIM_WALKING_WEST;
		}
		else if (direction & FACING_EAST) {
			_current_animation = ANIM_WALKING_EAST;
		}
		else {
			PRINT_ERROR << "invalid sprite direction, could not find proper standing animation to draw" << endl;
		}

		// Increasing the animation index by four from the walking _animations leads to the running _animations
		if (is_running && _has_running_animations) {
			_current_animation += 4;
		}

		// If the direction of movement changed in mid-flight, update the animation timer on the
		// new animated image to reflect the old, so the walking _animations do not appear to
		// "start and stop" whenever the direction is changed.
		if (_current_animation != last_animation) {
			_animations[_current_animation].SetTimeProgress(_animations[last_animation].GetTimeProgress());
			_animations[last_animation].SetTimeProgress(0);
		}
		_animations[_current_animation].Update();
	}

	was_moved = moved_position;
} // void MapSprite::Update()


// Draw the appropriate sprite frame at the correct position on the screen
void MapSprite::Draw() {
	if (MapObject::ShouldDraw() == true) {
		_animations[_current_animation].Draw();
	}
}

void MapSprite::DrawDialog()
{
    // Update the alpha of the dialogue icon according to it's distance from the player sprite
	const float DIALOGUE_ICON_VISIBLE_RANGE = 10.0f;

    if (MapObject::ShouldDraw() == true)
        if (_has_available_dialogue == true && _has_unseen_dialogue == true && MapMode::CurrentInstance()->IsShowGUI() == true && !MapMode::CurrentInstance()->IsCameraOnVirtualFocus()) {
                Color icon_color(1.0f, 1.0f, 1.0f, 0.0f);
                float icon_alpha = 1.0f - (fabs(ComputeXLocation() - MapMode::CurrentInstance()->GetCamera()->ComputeXLocation()) + fabs(ComputeYLocation() -
                    MapMode::CurrentInstance()->GetCamera()->ComputeYLocation())) / DIALOGUE_ICON_VISIBLE_RANGE;

                if (icon_alpha < 0.0f)
                    icon_alpha = 0.0f;
                icon_color.SetAlpha(icon_alpha);

                VideoManager->MoveRelative(0, -GetImgHeight());
                MapMode::CurrentInstance()->GetDialogueIcon().Draw(icon_color);
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

	_saved_current_animation = _current_animation;
}



void MapSprite::RestoreState() {
	VirtualSprite::RestoreState();

	_current_animation = _saved_current_animation;
}

// *****************************************************************************
// ********** EnemySprite class methods
// *****************************************************************************

EnemySprite::EnemySprite() :
	_zone(NULL),
	_color(1.0f, 1.0f, 1.0f, 0.0f),
	_aggro_range(8.0f),
	_time_dir_change(2500),
	_time_to_spawn(3500),
	_music_theme(""),
	_bg_file(""),
	_script_file("")
{
	filename = "";
	MapObject::_object_type = ENEMY_TYPE;
	moving = true;
	Reset();
}



EnemySprite::EnemySprite(std::string file) :
	_zone(NULL),
	_color(1.0f, 1.0f, 1.0f, 0.0f),
	_aggro_range(8.0f),
	_time_dir_change(2500),
	_time_to_spawn(3500),
	_music_theme(""),
	_bg_file(""),
	_script_file("")
{
	filename = file;
	MapObject::_object_type = ENEMY_TYPE;
	moving = true;
	Reset();
}


// Load in the appropriate images and other data for the sprite from a Lua file
bool EnemySprite::Load() {
	ReadScriptDescriptor sprite_script;
	if (sprite_script.OpenFile(filename) == false) {
		return false;
	}

	ScriptCallFunction<void>(sprite_script.GetLuaState(), "Load", this);
	string sprite_sheet = sprite_script.ReadString("sprite_sheet");
	return MapSprite::LoadStandardAnimations(sprite_sheet);
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



const std::vector<uint32>& EnemySprite::RetrieveRandomParty() {
	if (_enemy_parties.empty()) {
		PRINT_ERROR << "call invoked when no enemy parties existed" << endl;
		exit(1);
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
			// Holds the x and y deltas between the sprite and map camera coordinate pairs
			float xdelta, ydelta;
			_time_elapsed += SystemManager->GetUpdateTime();

			xdelta = ComputeXLocation() - MapMode::CurrentInstance()->GetCamera()->ComputeXLocation();
			ydelta = ComputeYLocation() - MapMode::CurrentInstance()->GetCamera()->ComputeYLocation();

			// If the sprite has moved outside of its zone and it should not, reverse the sprite's direction
			if ( _zone != NULL && _zone->IsInsideZone(x_position, y_position) == false && _zone->IsRoamingRestrained() ) {
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
				if ( MapMode::CurrentInstance()->AttackAllowed() && (_zone == NULL || ( fabs(xdelta) <= _aggro_range && fabs(ydelta) <= _aggro_range
					 && (!_zone->IsRoamingRestrained() || _zone->IsInsideZone(MapMode::CurrentInstance()->GetCamera()->x_position, MapMode::CurrentInstance()->GetCamera()->y_position)) )) )
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

		// Do nothing if the sprite is in the DEAD state, or any other state
		case DEAD:
		default:
			break;
	}
} // void EnemySprite::Update()



void EnemySprite::Draw() {
	// Otherwise, only draw it if it is not in the DEAD state
	if (MapObject::ShouldDraw() == true && _state != DEAD) {
		_animations[_current_animation].Draw(_color);
		return;
	}
}

} // namespace private_map

} // namespace hoa_map
