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

#include "engine/system.h"
#include "common/global/global.h"
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
	current_action(-1),
	_saved(false)
{
	//_object_type = VIRTUAL_SPRITE_OBJECT;
	_selected_image.Load("img/icons/battle/character_selector.png", 1.5f, 1.5f);
}


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
	// STUB
} // void VirtualSprite::Update()



void VirtualSprite::Draw() {
	// STUB
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


void VirtualSprite::SaveState() {
	_saved = true;

	_saved_direction = direction;
	_saved_movement_speed = movement_speed;
	_saved_moving = moving;
	_saved_name = name;
}


bool VirtualSprite::LoadState() {
	if (_saved == false)
		return false;

	 direction = _saved_direction;
	 movement_speed = _saved_movement_speed;
	 moving = _saved_moving;
	 name = _saved_name;

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
}


// Free all allocated images and other data
MapSprite::~MapSprite() {
	// Free animations
	for (vector<AnimatedImage>::iterator i = animations.begin(); i != animations.end(); ++i)
		(*i).Clear();
}


// Load in the appropriate images and other data for the sprite
bool MapSprite::LoadStandardAnimations(std::string filename) {
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
	animations[ANIM_STANDING_SOUTH].AddFrame(frames[0], movement_speed);
	animations[ANIM_STANDING_NORTH].AddFrame(frames[6], movement_speed);
	animations[ANIM_STANDING_WEST].AddFrame(frames[12], movement_speed);
	animations[ANIM_STANDING_EAST].AddFrame(frames[18], movement_speed);

	// Add walking frames to animations
	animations[ANIM_WALKING_SOUTH].AddFrame(frames[1], movement_speed);
	animations[ANIM_WALKING_SOUTH].AddFrame(frames[2], movement_speed);
	animations[ANIM_WALKING_SOUTH].AddFrame(frames[3], movement_speed);
	animations[ANIM_WALKING_SOUTH].AddFrame(frames[1], movement_speed);
	animations[ANIM_WALKING_SOUTH].AddFrame(frames[4], movement_speed);
	animations[ANIM_WALKING_SOUTH].AddFrame(frames[5], movement_speed);

	animations[ANIM_WALKING_NORTH].AddFrame(frames[7], movement_speed);
	animations[ANIM_WALKING_NORTH].AddFrame(frames[8], movement_speed);
	animations[ANIM_WALKING_NORTH].AddFrame(frames[9], movement_speed);
	animations[ANIM_WALKING_NORTH].AddFrame(frames[7], movement_speed);
	animations[ANIM_WALKING_NORTH].AddFrame(frames[10], movement_speed);
	animations[ANIM_WALKING_NORTH].AddFrame(frames[11], movement_speed);

	animations[ANIM_WALKING_WEST].AddFrame(frames[13], movement_speed);
	animations[ANIM_WALKING_WEST].AddFrame(frames[14], movement_speed);
	animations[ANIM_WALKING_WEST].AddFrame(frames[15], movement_speed);
	animations[ANIM_WALKING_WEST].AddFrame(frames[13], movement_speed);
	animations[ANIM_WALKING_WEST].AddFrame(frames[16], movement_speed);
	animations[ANIM_WALKING_WEST].AddFrame(frames[17], movement_speed);

	animations[ANIM_WALKING_EAST].AddFrame(frames[19], movement_speed);
	animations[ANIM_WALKING_EAST].AddFrame(frames[20], movement_speed);
	animations[ANIM_WALKING_EAST].AddFrame(frames[21], movement_speed);
	animations[ANIM_WALKING_EAST].AddFrame(frames[19], movement_speed);
	animations[ANIM_WALKING_EAST].AddFrame(frames[22], movement_speed);
	animations[ANIM_WALKING_EAST].AddFrame(frames[23], movement_speed);

	return true;
} // bool MapSprite::LoadStandardAnimations(std::string filename)



bool MapSprite::LoadRunningAnimations(std::string filename) {
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
	animations[ANIM_RUNNING_SOUTH].AddFrame(frames[1], movement_speed);
	animations[ANIM_RUNNING_SOUTH].AddFrame(frames[2], movement_speed);
	animations[ANIM_RUNNING_SOUTH].AddFrame(frames[3], movement_speed);
	animations[ANIM_RUNNING_SOUTH].AddFrame(frames[1], movement_speed);
	animations[ANIM_RUNNING_SOUTH].AddFrame(frames[4], movement_speed);
	animations[ANIM_RUNNING_SOUTH].AddFrame(frames[5], movement_speed);

	animations[ANIM_RUNNING_NORTH].AddFrame(frames[7], movement_speed);
	animations[ANIM_RUNNING_NORTH].AddFrame(frames[8], movement_speed);
	animations[ANIM_RUNNING_NORTH].AddFrame(frames[9], movement_speed);
	animations[ANIM_RUNNING_NORTH].AddFrame(frames[7], movement_speed);
	animations[ANIM_RUNNING_NORTH].AddFrame(frames[10], movement_speed);
	animations[ANIM_RUNNING_NORTH].AddFrame(frames[11], movement_speed);

	animations[ANIM_RUNNING_WEST].AddFrame(frames[13], movement_speed);
	animations[ANIM_RUNNING_WEST].AddFrame(frames[14], movement_speed);
	animations[ANIM_RUNNING_WEST].AddFrame(frames[15], movement_speed);
	animations[ANIM_RUNNING_WEST].AddFrame(frames[13], movement_speed);
	animations[ANIM_RUNNING_WEST].AddFrame(frames[16], movement_speed);
	animations[ANIM_RUNNING_WEST].AddFrame(frames[17], movement_speed);

	animations[ANIM_RUNNING_EAST].AddFrame(frames[19], movement_speed);
	animations[ANIM_RUNNING_EAST].AddFrame(frames[20], movement_speed);
	animations[ANIM_RUNNING_EAST].AddFrame(frames[21], movement_speed);
	animations[ANIM_RUNNING_EAST].AddFrame(frames[19], movement_speed);
	animations[ANIM_RUNNING_EAST].AddFrame(frames[22], movement_speed);
	animations[ANIM_RUNNING_EAST].AddFrame(frames[23], movement_speed);

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

} // namespace hoa_editor
