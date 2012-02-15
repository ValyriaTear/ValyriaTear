///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    sprites.cpp
*** \author  Dale Ma, dalema22@gmail.com
*** \brief   Source file for editor sprites.
*** ***************************************************************************/

#include "utils.h"

#include "system.h"
#include "global.h"
#include "sprites.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_video;
using namespace hoa_script;
using namespace hoa_system;
using namespace hoa_global;

namespace hoa_editor {

// ****************************************************************************
// ********************* VirtualSprite Class Functions ************************
// ****************************************************************************

VirtualSprite::VirtualSprite() :
	direction(SOUTH),
	movement_speed(NORMAL_SPEED),
	moving(false),
	is_selected(false),
	sky_object(false),
	face_portrait( NULL ),
	//has_active_dialogue(true),
	current_action(-1),
	//forced_action(-1),
	_saved(false)
	//_current_dialogue(0),
	//_show_dialogue_icon(true),
	//_dialogue_icon_color(1.0f, 1.0f, 1.0f, 0.0f)
{
	//_object_type = VIRTUAL_SPRITE_OBJECT;
	_selected_image.Load("img/icons/battle/character_selector.png", 1.5f, 1.5f);
}



VirtualSprite::~VirtualSprite() {
	if (face_portrait != NULL) {
		delete face_portrait;
		face_portrait = NULL;
	}

	// Update the seen events for all dialogues
	//for (uint32 i = 0; i < dialogues.size(); ++i) {
	//	string event_name = "s" + NumberToString(object_id) + "_d" + NumberToString(dialogues.size() - 1);
	//	MapMode::_current_map->_map_event_group->SetEvent(event_name, dialogues[i]->GetTimesSeen());
	//	delete dialogues[i];
	//}
	//dialogues.clear();
}



/*void VirtualSprite::AddDialogue(MapDialogue* md) {
	dialogues.push_back(md);
	md->SetOwner(this);

	// Look up the event for this dialogue to see whether it has already been read or not
	string event_name = "s" + NumberToString(object_id) + "_d" + NumberToString(dialogues.size() - 1);
	md->SetEventName(event_name);
	GlobalEventGroup& event_group = *(MapMode::_loading_map->_map_event_group);

	if (event_group.DoesEventExist(event_name) == false) {
		event_group.AddNewEvent(event_name, 0);
		seen_all_dialogue = false;
	}
	else {
		md->SetTimesSeen(event_group.GetEvent(event_name));
		if (md->HasAlreadySeen() == false)
			seen_all_dialogue = false;
	}
}*/


/*void VirtualSprite::UpdateSeenDialogue() {
	// Check all dialogues for any which have not yet been read
	for (uint32 i = 0; i < dialogues.size(); i++) {
		if (dialogues[i]->HasAlreadySeen() == false) {
			seen_all_dialogue = false;
			return;
		}
	}

	seen_all_dialogue = true;
}*/



/*void VirtualSprite::UpdateActiveDialogue() {
	// Check all dialogues for any that are still active.
	for (size_t i = 0; i < dialogues.size(); i++) {
		if(dialogues[i]->IsActive()) {
			has_active_dialogue = true;
			return;
		}
	}
	has_active_dialogue = false;
}*/



uint16 VirtualSprite::CalculateOppositeDirection(const uint16 direction) {
	switch (direction) {
		case NORTH:      return SOUTH;
		case SOUTH:      return NORTH;
		case WEST:       return EAST;
		case EAST:       return WEST;
		case NW_NORTH:   return SE_SOUTH;
		case NW_WEST:    return SE_EAST;
		case NE_NORTH:   return SW_SOUTH;
		case NE_EAST:    return SW_WEST;
		case SW_SOUTH:   return NE_NORTH;
		case SW_WEST:    return NE_EAST;
		case SE_SOUTH:   return NW_NORTH;
		case SE_EAST:    return NW_WEST;
		default:
			//IF_PRINT_WARNING(MAP_DEBUG) << "invalid direction argument: " << direction << endl;
			return SOUTH;
	}
}



void VirtualSprite::Update() {
	//Update the alpha of the dialogue icon according to it's distance from the player to make it fade away
	// const float DIALOGUE_ICON_VISIBLE_RANGE = 30.0f;
	//float icon_alpha = 1.0f - (abs( ComputeXLocation() - MapMode::_current_map->_camera->ComputeXLocation()) + abs(ComputeYLocation() -
	//	MapMode::_current_map->_camera->ComputeYLocation())) / DIALOGUE_ICON_VISIBLE_RANGE;
	//if (icon_alpha < 0)
	//	icon_alpha = 0;
	//_dialogue_icon_color.SetAlpha(icon_alpha);
	//MapMode::_current_map->_new_dialogue_icon.Update();

	//if (!updatable) {
	//	return;
	//}

	// If the sprite was not forced to do a certain action
	/*if (forced_action < 0) {
		// Execute the sprite's action and if it is finished, update the action counter
		if (current_action >= 0) {
			actions[current_action]->Execute();
			if (actions[current_action]->IsFinishedReset()) {
				current_action++;
				if (static_cast<uint8>(current_action) >= actions.size())
					current_action = 0;
			}
		}
	}*/

	if (moving) {
		// Save the previous sprite's position temporarily
		//float tmp_x = x_offset;
		//float tmp_y = y_offset;

		//float distance_moved = static_cast<float>(MapMode::_current_map->_time_elapsed) / movement_speed;
		// Double the distance to move if the sprite is running
		//if (is_running == true)
		//	distance_moved *= 2.0f;
		// If the movement is diagonal, decrease the lateral movement distance by sin(45 degress)
		//if (direction & DIAGONAL_MOVEMENT)
		//	distance_moved *= 0.707f;

		// Move the sprite the appropriate distance in the appropriate Y direction
		//if (direction & (NORTH | NORTHWEST | NORTHEAST))
		//	y_offset -= distance_moved;
		//else if (direction & (SOUTH | SOUTHWEST | SOUTHEAST))
		//	y_offset += distance_moved;

		// Determine if the sprite may move to this new Y position
		/*if (MapMode::_current_map->_object_manager->DetectCollision(this)) {
			// Determine if we can slide on an object
			if( direction & (SOUTH | NORTH)) {
				//Start from a sprite's size away and get closer testing collision each time
				for( float i = 0; i < coll_half_width * 2; i += 0.1f ) {
					x_offset = tmp_x - ( coll_half_width * 2 ) + i;
					if (MapMode::_current_map->_object_manager->DetectCollision(this)) {
						//Try the other way, can't go that way
						x_offset = tmp_x + ( coll_half_width * 2 ) - i;
						if (MapMode::_current_map->_object_manager->DetectCollision(this)) {
							//Still can't slide, reset
							x_offset = tmp_x;
						}
						else {
							x_offset = tmp_x + distance_moved;
							break;
						}
					}
					else {
						x_offset = tmp_x - distance_moved;
						break;
					}
				}

				// Roll-over X position offsets if necessary
				while (x_offset < 0.0f) {
					x_position -= 1;
					x_offset += 1.0f;
				}
				while (x_offset > 1.0f) {
					x_position += 1;
					x_offset -= 1.0f;
				}
			}

			y_offset = tmp_y;

		}
		else {
			// Roll-over Y position offsets if necessary
			while (y_offset < 0.0f) {
				y_position -= 1;
				y_offset += 1.0f;
			}
			while (y_offset > 1.0f) {
				y_position += 1;
				y_offset -= 1.0f;
			}
		}*/

		// Move the sprite the appropriate distance in the appropriate X direction
		//if (direction & (WEST | NORTHWEST | SOUTHWEST))
		//	x_offset -= distance_moved;
		//else if (direction & (EAST | NORTHEAST | SOUTHEAST))
		//	x_offset += distance_moved;


		// Determine if the sprite may move to this new X position
		/*if (MapMode::_current_map->_object_manager->DetectCollision(this)) {
			// Determine if we can slide on an object
			if( direction & (WEST | EAST)) {
				//Start from a sprite's size away and get closer testing collision each time
				for( float i = 0; i < coll_height; i += 0.1f ) {
					y_offset = tmp_y - coll_height + i;
					if (MapMode::_current_map->_object_manager->DetectCollision(this)) {
						//Try the other way, can't go that way
						y_offset = tmp_y + coll_height - i;
						if (MapMode::_current_map->_object_manager->DetectCollision(this)) {
							//Still can't slide, reset
							y_offset = tmp_y;
						}
						else {
							y_offset = tmp_y + distance_moved;
							break;
						}
					}
					else {
						y_offset = tmp_y - distance_moved;
						break;
					}
				}

				// Roll-over Y position offsets if necessary
				while (y_offset < 0.0f) {
					y_position -= 1;
					y_offset += 1.0f;
				}
				while (y_offset > 1.0f) {
					y_position += 1;
					y_offset -= 1.0f;
				}
			}

			x_offset = tmp_x;
		}
		else {
			// Roll-over X position offsets if necessary
			while (x_offset < 0.0f) {
				x_position -= 1;
				x_offset += 1.0f;
			}
			while (x_offset > 1.0f) {
				x_position += 1;
				x_offset -= 1.0f;
			}
		}*/
	} // if (moving)
} // void VirtualSprite::Update()



void VirtualSprite::Draw() {
	/*if (HasDialogue()) {
		if (IsShowingDialogueIcon() && MapMode::_IsShowingDialogueIcons() && seen_all_dialogue == false) {
			VideoManager->MoveRelative(0, -GetImgHeight());
			MapMode::_current_map->_new_dialogue_icon.Draw(_dialogue_icon_color);
		}
	}*/
}



void VirtualSprite::SetDirection(uint16 dir) {
	// If the direction is a lateral one, simply set it and return
	if (dir & (NORTH | SOUTH | EAST | WEST)) {
		direction = dir;
		return;
	}

	// Otherwise the direction is diagonal, and we must figure out which way the sprite should face.
	if (dir & NORTHWEST) {
		if (direction & (FACING_NORTH | FACING_EAST))
			direction = NW_NORTH;
		else
			direction = NW_WEST;
	}
	else if (dir & SOUTHWEST) {
		if (direction & (FACING_SOUTH | FACING_EAST))
			direction = SW_SOUTH;
		else
			direction = SW_WEST;
	}
	else if (dir & NORTHEAST) {
		if (direction & (FACING_NORTH | FACING_WEST))
			direction = NE_NORTH;
		else
			direction = NE_EAST;
	}
	else if (dir & SOUTHEAST) {
		if (direction & (FACING_SOUTH | FACING_WEST))
			direction = SE_SOUTH;
		else
			direction = SE_EAST;
	}
	else { // Invalid
		//if (MAP_DEBUG)
		//	fprintf(stderr, "ERROR: in VirtualSprite::SetDirection tried to set an invalid direction (%d)\n", dir);
	}
} // void VirtualSprite::SetDirection(uint16 dir)



void VirtualSprite::SetFacePortrait(std::string pn) {
	if (face_portrait != NULL) {
		delete face_portrait;
	}

	face_portrait = new StillImage();
	face_portrait->Load(pn);
}



void VirtualSprite::SaveState() {
	_saved = true;

	_saved_direction = direction;
	_saved_movement_speed = movement_speed;
	_saved_moving = moving;
	_saved_name = name;
	//_saved_current_action = current_action;
	// TEMP
	//updatable = false;
}



bool VirtualSprite::LoadState() {
	if (_saved == false)
		return false;

	 direction = _saved_direction;
	 movement_speed = _saved_movement_speed;
	 moving = _saved_moving;
	 name = _saved_name;
	 //current_action = _saved_current_action;

	// TEMP
	//updatable = true;

	 return true;
}



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
			SetDirection(NORTHEAST);
			break;
		case 6:
			SetDirection(NORTHWEST);
			break;
		case 7:
			SetDirection(SOUTHEAST);
			break;
		case 8:
			SetDirection(SOUTHWEST);
			break;
		default:
			;
		//	if (MAP_DEBUG)
		//		cerr << "MAP WARNING: In VirtualSprite::SetRandomDirection(), invalid direction was picked" << endl;
	}
}

// ****************************************************************************
// ************************ MapSprite Class Functions *************************
// ****************************************************************************

// Constructor for critical class members. Other members are initialized via support functions
MapSprite::MapSprite() :
	was_moving(false),
	has_running_anim(false),
	walk_sound(-1),
	current_animation(ANIM_STANDING_SOUTH)
{
	//_object_type = SPRITE_OBJECT;
	VirtualSprite::face_portrait = 0;
}


// Free all allocated images and other data
MapSprite::~MapSprite() {
	if (face_portrait != NULL) {
		delete face_portrait;
		face_portrait = NULL;
	}

	// Free animations
	for (vector<AnimatedImage>::iterator i = animations.begin(); i != animations.end(); ++i)
		(*i).Clear();
}


// Load in the appropriate images and other data for the sprite
bool MapSprite::LoadStandardAnimations(std::string filename) {
	// The speed to display each frame in the walking animation
	uint32 frame_speed = static_cast<uint32>(movement_speed / 10.0f);

	// Prepare the four standing and four walking animations
	for (uint8 i = 0; i < 8; i++)
		animations.push_back(AnimatedImage());

	// Load the multi-image, containing 24 frames total
	vector<StillImage> frames(24);
	for (uint8 i = 0; i < 24; i++)
		frames[i].SetDimensions(img_half_width * 2, img_height);

	if (ImageDescriptor::LoadMultiImageFromElementGrid(frames, filename, 4, 6) == false) {
		return false;
	}

	// Add standing frames to animations
	animations[ANIM_STANDING_SOUTH].AddFrame(frames[0], frame_speed);
	animations[ANIM_STANDING_NORTH].AddFrame(frames[6], frame_speed);
	animations[ANIM_STANDING_WEST].AddFrame(frames[12], frame_speed);
	animations[ANIM_STANDING_EAST].AddFrame(frames[18], frame_speed);

	// Add walking frames to animations
	animations[ANIM_WALKING_SOUTH].AddFrame(frames[1], frame_speed);
	animations[ANIM_WALKING_SOUTH].AddFrame(frames[2], frame_speed);
	animations[ANIM_WALKING_SOUTH].AddFrame(frames[3], frame_speed);
	animations[ANIM_WALKING_SOUTH].AddFrame(frames[1], frame_speed);
	animations[ANIM_WALKING_SOUTH].AddFrame(frames[4], frame_speed);
	animations[ANIM_WALKING_SOUTH].AddFrame(frames[5], frame_speed);

	animations[ANIM_WALKING_NORTH].AddFrame(frames[7], frame_speed);
	animations[ANIM_WALKING_NORTH].AddFrame(frames[8], frame_speed);
	animations[ANIM_WALKING_NORTH].AddFrame(frames[9], frame_speed);
	animations[ANIM_WALKING_NORTH].AddFrame(frames[7], frame_speed);
	animations[ANIM_WALKING_NORTH].AddFrame(frames[10], frame_speed);
	animations[ANIM_WALKING_NORTH].AddFrame(frames[11], frame_speed);

	animations[ANIM_WALKING_WEST].AddFrame(frames[13], frame_speed);
	animations[ANIM_WALKING_WEST].AddFrame(frames[14], frame_speed);
	animations[ANIM_WALKING_WEST].AddFrame(frames[15], frame_speed);
	animations[ANIM_WALKING_WEST].AddFrame(frames[13], frame_speed);
	animations[ANIM_WALKING_WEST].AddFrame(frames[16], frame_speed);
	animations[ANIM_WALKING_WEST].AddFrame(frames[17], frame_speed);

	animations[ANIM_WALKING_EAST].AddFrame(frames[19], frame_speed);
	animations[ANIM_WALKING_EAST].AddFrame(frames[20], frame_speed);
	animations[ANIM_WALKING_EAST].AddFrame(frames[21], frame_speed);
	animations[ANIM_WALKING_EAST].AddFrame(frames[19], frame_speed);
	animations[ANIM_WALKING_EAST].AddFrame(frames[22], frame_speed);
	animations[ANIM_WALKING_EAST].AddFrame(frames[23], frame_speed);

	return true;
} // bool MapSprite::LoadStandardAnimations(std::string filename)



bool MapSprite::LoadRunningAnimations(std::string filename) {
	// The speed to display each frame in the running animation
	uint32 frame_speed = static_cast<uint32>(movement_speed / 10.0f);

	// Prepare to add the four running animations
	for (uint8 i = 0; i < 4; i++)
		animations.push_back(AnimatedImage());

	// Load the multi-image, containing 24 frames total
	vector<StillImage> frames(24);
	for (uint8 i = 0; i < 24; i++)
		frames[i].SetDimensions(img_half_width * 2, img_height);

	if (ImageDescriptor::LoadMultiImageFromElementGrid(frames, filename, 4, 6) == false) {
		return false;
	}

	// Add walking frames to animations
	animations[ANIM_RUNNING_SOUTH].AddFrame(frames[1], frame_speed);
	animations[ANIM_RUNNING_SOUTH].AddFrame(frames[2], frame_speed);
	animations[ANIM_RUNNING_SOUTH].AddFrame(frames[3], frame_speed);
	animations[ANIM_RUNNING_SOUTH].AddFrame(frames[1], frame_speed);
	animations[ANIM_RUNNING_SOUTH].AddFrame(frames[4], frame_speed);
	animations[ANIM_RUNNING_SOUTH].AddFrame(frames[5], frame_speed);

	animations[ANIM_RUNNING_NORTH].AddFrame(frames[7], frame_speed);
	animations[ANIM_RUNNING_NORTH].AddFrame(frames[8], frame_speed);
	animations[ANIM_RUNNING_NORTH].AddFrame(frames[9], frame_speed);
	animations[ANIM_RUNNING_NORTH].AddFrame(frames[7], frame_speed);
	animations[ANIM_RUNNING_NORTH].AddFrame(frames[10], frame_speed);
	animations[ANIM_RUNNING_NORTH].AddFrame(frames[11], frame_speed);

	animations[ANIM_RUNNING_WEST].AddFrame(frames[13], frame_speed);
	animations[ANIM_RUNNING_WEST].AddFrame(frames[14], frame_speed);
	animations[ANIM_RUNNING_WEST].AddFrame(frames[15], frame_speed);
	animations[ANIM_RUNNING_WEST].AddFrame(frames[13], frame_speed);
	animations[ANIM_RUNNING_WEST].AddFrame(frames[16], frame_speed);
	animations[ANIM_RUNNING_WEST].AddFrame(frames[17], frame_speed);

	animations[ANIM_RUNNING_EAST].AddFrame(frames[19], frame_speed);
	animations[ANIM_RUNNING_EAST].AddFrame(frames[20], frame_speed);
	animations[ANIM_RUNNING_EAST].AddFrame(frames[21], frame_speed);
	animations[ANIM_RUNNING_EAST].AddFrame(frames[19], frame_speed);
	animations[ANIM_RUNNING_EAST].AddFrame(frames[22], frame_speed);
	animations[ANIM_RUNNING_EAST].AddFrame(frames[23], frame_speed);

	has_running_anim = true;
	return true;
} // bool MapSprite::LoadRunningAnimations(std::string filename)


// Updates the state of the sprite
void MapSprite::Update() {
	// Set the sprite's animation to the standing still position if movement has just stopped
	if (!moving) {
		if (was_moving) {
			// Set the current movement animation to zero progress
			animations[current_animation].SetTimeProgress(0);
			was_moving = false;
		}

		// Determine the correct standing frame to display
		if (current_action == -1) {
			if (direction & FACING_NORTH) {
				current_animation = ANIM_STANDING_NORTH;
			}
			else if (direction & FACING_SOUTH) {
				current_animation = ANIM_STANDING_SOUTH;
			}
			else if (direction & FACING_WEST) {
				current_animation = ANIM_STANDING_WEST;
			}
			else if (direction & FACING_EAST) {
				current_animation = ANIM_STANDING_EAST;
			}
			else {
				cerr << "MAP ERROR: could not find proper standing animation to draw" << endl;
			}
		}
	} // if (!moving)

	// This call will update the sprite's position and perform collision detection
	VirtualSprite::Update();

	if (moving) {
		// Save the previous animation
		uint8 last_animation = current_animation;

		// Determine the correct animation to display
		if (direction & FACING_NORTH) {
			current_animation = ANIM_WALKING_NORTH;
		}
		else if (direction & FACING_SOUTH) {
			current_animation = ANIM_WALKING_SOUTH;
		}
		else if (direction & FACING_WEST) {
			current_animation = ANIM_WALKING_WEST;
		}
		else if (direction & FACING_EAST) {
			current_animation = ANIM_WALKING_EAST;
		}
		else {
			cerr << "MAP ERROR: could not find proper movement animation to draw" << endl;
		}

		// Increasing the animation index by four from the walking animations leads to the running animations
		if (is_running && has_running_anim)
			current_animation += 4;

		// If the direction of movement changed in mid-flight, update the animation timer on the
		// new animated image to reflect the old, so the walking animations do not appear to
		// "start and stop" whenever the direction is changed.
		if (current_animation != last_animation) {
			animations[current_animation].SetTimeProgress(animations[last_animation].GetTimeProgress());
			animations[last_animation].SetTimeProgress(0);
		}
		animations[current_animation].Update();

		was_moving = true;
	} // if (moving)
} // void MapSprite::Update()


// Draw the appropriate sprite frame at the correct position on the screen
void MapSprite::Draw() {
	//if (MapObject::DrawHelper() == true) {
		animations[current_animation].SetHeight( img_height/2 );
		animations[current_animation].SetWidth( img_half_width );
		animations[current_animation].Draw();
	//	VirtualSprite::Draw();
	//}
}

void MapSprite::DrawSelection() {
	if(is_selected)
		_selected_image.Draw();
}



/*void MapSprite::SaveState() {
	VirtualSprite::SaveState();

	_saved_was_moving = was_moving;
	_saved_walk_sound = walk_sound;
	_saved_current_animation = current_animation;
}*/



/*bool MapSprite::LoadState() {
	if (!VirtualSprite::LoadState())
		return false;

	was_moving = _saved_was_moving;
	walk_sound = _saved_walk_sound;
	current_animation = _saved_current_animation;

	return true;
}*/

} // namespace hoa_editor
