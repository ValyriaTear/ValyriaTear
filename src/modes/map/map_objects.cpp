///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_objects.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for map mode objects.
*** ***************************************************************************/

// Allacrost utilities
#include "utils.h"

// Allacrost engines
#include "audio.h"
#include "system.h"
#include "video.h"

// Allacrost globals
#include "global.h"

// Local map mode headers
#include "map.h"
#include "map_dialogue.h"
#include "map_objects.h"
#include "map_sprites.h"

using namespace std;
using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_script;
using namespace hoa_system;
using namespace hoa_video;
using namespace hoa_global;

namespace hoa_map {

namespace private_map {

// ----------------------------------------------------------------------------
// ---------- MapObject Class Functions
// ----------------------------------------------------------------------------

MapObject::MapObject() :
	object_id(-1),
	context(MAP_CONTEXT_01),
	x_position(-1),
	y_position(-1),
	x_offset(0.0f),
	y_offset(0.0f),
	img_half_width(0.0f),
	img_height(0.0f),
	coll_half_width(0.0f),
	coll_height(0.0f),
	updatable(true),
	visible(true),
	no_collision(false),
	sky_object(false),
	draw_on_second_pass(false)
{}



bool MapObject::ShouldDraw() {
	if (visible == false)
		return false;

	if (context != MapMode::CurrentInstance()->GetCurrentContext())
		return false;

	// ---------- Determine if the sprite is off-screen and if so, don't draw it.
	MapRectangle img_rect;
	GetImageRectangle(img_rect);
	if (MapRectangle::CheckIntersection(img_rect, MapMode::CurrentInstance()->GetMapFrame().screen_edges) == false)
		return false;

	// ---------- (1) Determine the center position coordinates for the camera
	float x_pos, y_pos; // Holds the final X, Y coordinates of the camera
	float x_pixel_length, y_pixel_length; // The X and Y length values that coorespond to a single pixel in the current coodinate system
	float rounded_x_offset, rounded_y_offset; // The X and Y position offsets of the object, rounded to perfectly align on a pixel boundary


	// TODO: the call to GetPixelSize() will return the same result every time so long as the coordinate system did not change. If we never
	// change the coordinate system in map mode, then this should be done only once and the calculated values should be saved for re-use.
	// However, we've discussed the possiblity of adding a zoom feature to maps, in which case we need to continually re-calculate the pixel size
	VideoManager->GetPixelSize(x_pixel_length, y_pixel_length);
	rounded_x_offset = FloorToFloatMultiple(x_offset, x_pixel_length);
	rounded_y_offset = FloorToFloatMultiple(y_offset, y_pixel_length);
	x_pos = static_cast<float>(x_position) + rounded_x_offset;
	y_pos = static_cast<float>(y_position) + rounded_y_offset;

	// ---------- Move the drawing cursor to the appropriate coordinates for this sprite
	VideoManager->Move(x_pos - MapMode::CurrentInstance()->GetMapFrame().screen_edges.left, y_pos - MapMode::CurrentInstance()->GetMapFrame().screen_edges.top);
	return true;
} // bool MapObject::DrawHelper()



void MapObject::CheckPositionOffsets() {
	while (x_offset < 0.0f) {
		x_position -= 1;
		x_offset += 1.0f;
	}
	while (x_offset > 1.0f) {
		x_position += 1;
		x_offset -= 1.0f;
	}
	while (y_offset < 0.0f) {
		y_position -= 1;
		y_offset += 1.0f;
	}
	while (y_offset > 1.0f) {
		y_position += 1;
		y_offset -= 1.0f;
	}
}



void MapObject::GetCollisionRectangle(MapRectangle& rect) const {
	float x_pos = static_cast<float>(x_position) + x_offset;
	float y_pos = static_cast<float>(y_position) + y_offset;

	rect.left = x_pos - coll_half_width;
	rect.right = x_pos + coll_half_width;
	rect.top = y_pos - coll_height;
	rect.bottom = y_pos;
}



void MapObject::GetImageRectangle(MapRectangle& rect) const {
	float x_pos = static_cast<float>(x_position) + x_offset;
	float y_pos = static_cast<float>(y_position) + y_offset;

	rect.left = x_pos - img_half_width;
	rect.right = x_pos + img_half_width;
	rect.top = y_pos - img_height;
	rect.bottom = y_pos;
}

// ----------------------------------------------------------------------------
// ---------- PhysicalObject Class Functions
// ----------------------------------------------------------------------------

PhysicalObject::PhysicalObject() :
	current_animation(0)
{
	MapObject::_object_type = PHYSICAL_TYPE;
}



PhysicalObject::~PhysicalObject() {
	animations.clear();
}



void PhysicalObject::Update() {
	if (updatable)
		animations[current_animation].Update();
}



void PhysicalObject::Draw() {
	if (MapObject::ShouldDraw() == true)
		animations[current_animation].Draw();
}



void PhysicalObject::AddAnimation(string filename) {
	AnimatedImage new_animation;
	new_animation.SetDimensions(img_half_width * 2, img_height);
	if (new_animation.AddFrame(filename, 100000) == false) { // TODO: 1000000 is an arbitrary frame time
		IF_PRINT_WARNING(MAP_DEBUG) << "could not add animation because image filename was invalid: " << filename << endl;
		return;
	}

	animations.push_back(new_animation);
}

// ----------------------------------------------------------------------------
// ---------- TreasureObject Class Functions
// ----------------------------------------------------------------------------

TreasureObject::TreasureObject(string image_file, uint8 num_total_frames, uint8 num_closed_frames, uint8 num_open_frames) :
	PhysicalObject()
{
	const uint32 DEFAULT_FRAME_TIME = 10; // The default number of milliseconds for frame animations

	_object_type = TREASURE_TYPE;

	std::vector<StillImage> frames;

	// (1) Load a the single row, multiple column multi image containing all of the treasure frames
	if (ImageDescriptor::LoadMultiImageFromElementGrid(frames, image_file, 1, num_total_frames) == false ) {
		PRINT_ERROR << "failed to load image file: " << image_file << endl;
		// TODO: throw exception
		return;
	}

	// Update the frame image sizes to work in the MapMode coordinate system
	for (uint32 i = 0; i < frames.size(); i++) {
		frames[i].SetWidth(frames[i].GetWidth() / (GRID_LENGTH / 2));
		frames[i].SetHeight(frames[i].GetHeight() / (GRID_LENGTH / 2));
	}

	// (2) Now that we know the total number of frames in the image, make sure the frame count arguments make sense
	if (num_open_frames == 0 || num_closed_frames == 0 || num_open_frames + num_closed_frames >= num_total_frames) {
		PRINT_ERROR << "invalid treasure image for image file: " << image_file << endl;
		// TODO: throw exception
		return;
	}

	// (3) Dissect the frames and create the closed, opening, and open animations
	hoa_video::AnimatedImage closed_anim, opening_anim, open_anim;

	for (uint8 i = 0; i < num_closed_frames; i++) {
		closed_anim.AddFrame(frames[i], DEFAULT_FRAME_TIME);
	}
	for (uint8 i = num_total_frames - num_open_frames; i < num_total_frames; i++) {
		open_anim.AddFrame(frames[i], DEFAULT_FRAME_TIME);
	}

	// Loop the opening animation only once
	opening_anim.SetNumberLoops(0);

	// If there are no additional frames for the opening animation, set the opening animation to be the open animation
	if (num_total_frames - num_closed_frames - num_open_frames == 0) {
		opening_anim = open_anim;
	}
	else {
		for (uint8 i = num_closed_frames; i < num_total_frames - num_open_frames; i++) {
			opening_anim.AddFrame(frames[i], DEFAULT_FRAME_TIME);
		}
	}

	AddAnimation(closed_anim);
	AddAnimation(opening_anim);
	AddAnimation(open_anim);

	// (4) Set the collision rectangle according to the dimensions of the first frame
	SetCollHalfWidth(frames[0].GetWidth() / 2.0f);
	SetCollHeight(frames[0].GetHeight());
} // TreasureObject::TreasureObject(string image_file, uint8 num_total_frames, uint8 num_closed_frames, uint8 num_open_frames)



void TreasureObject::LoadState() {
	string event_name = GetEventName();

	// Check if the event corresponding to this treasure has already occurred
	if (MapMode::CurrentInstance()->GetMapEventGroup()->DoesEventExist(event_name) == true) {
		// If the event is non-zero, the treasure has already been opened
		if (MapMode::CurrentInstance()->GetMapEventGroup()->GetEvent(event_name) != 0) {
			SetCurrentAnimation(TREASURE_OPEN_ANIM);
			_treasure.SetTaken(true);
		}
	}
}



void TreasureObject::Open() {
	if (_treasure.IsTaken() == true) {
		IF_PRINT_WARNING(MAP_DEBUG) << "attempted to retrieve an already taken treasure: " << object_id << endl;
		return;
	}

	SetCurrentAnimation(TREASURE_OPENING_ANIM);
	string event_name = GetEventName();

	// Add an event to the map group indicating that the treasure has now been opened
	if (MapMode::CurrentInstance()->GetMapEventGroup()->DoesEventExist(event_name) == true) {
		MapMode::CurrentInstance()->GetMapEventGroup()->SetEvent(event_name, 1);
	}
	else {
		MapMode::CurrentInstance()->GetMapEventGroup()->AddNewEvent(event_name, 1);
	}
}



void TreasureObject::Update() {
	PhysicalObject::Update();

	if ((current_animation == TREASURE_OPENING_ANIM) && (animations[TREASURE_OPENING_ANIM].IsLoopsFinished() == true)) {
		SetCurrentAnimation(TREASURE_OPEN_ANIM);
		MapMode::CurrentInstance()->GetTreasureSupervisor()->Initialize(this);
	}
}

// ----------------------------------------------------------------------------
// ---------- ObjectSupervisor Class Functions
// ----------------------------------------------------------------------------

ObjectSupervisor::ObjectSupervisor() :
	_num_grid_rows(0),
	_num_grid_cols(0),
	_last_id(1000)
{
	_virtual_focus = new VirtualSprite();
	_virtual_focus->SetXPosition(0, 0.0f);
	_virtual_focus->SetYPosition(0, 0.0f);
	_virtual_focus->movement_speed = NORMAL_SPEED;
	_virtual_focus->SetNoCollision(true);
	_virtual_focus->SetVisible(false);
}



ObjectSupervisor::~ObjectSupervisor() {
	// Delete all of the map objects
	for (uint32 i = 0; i < _ground_objects.size(); i++) {
		delete(_ground_objects[i]);
	}
	for (uint32 i = 0; i < _pass_objects.size(); i++) {
		delete(_pass_objects[i]);
	}
	for (uint32 i = 0; i < _sky_objects.size(); i++) {
		delete(_sky_objects[i]);
	}
	delete(_virtual_focus);
}



MapObject* ObjectSupervisor::GetObjectByIndex(uint32 index) {
	if (index >= GetNumberObjects()) {
		return NULL;
	}

	uint32 counter = 0;
	for (map<uint16, MapObject*>::iterator i = _all_objects.begin(); i != _all_objects.end(); i++) {
		if (counter == index)
			return i->second;
		else
			counter++;
	}

	IF_PRINT_WARNING(MAP_DEBUG) << "object not found after reaching end of set -- this should never happen" << endl;
	return NULL;
}



MapObject* ObjectSupervisor::GetObject(uint32 object_id) {
	map<uint16, MapObject*>::iterator i = _all_objects.find(object_id);

	if (i == _all_objects.end())
		return NULL;
	else
		return i->second;
}



VirtualSprite* ObjectSupervisor::GetSprite(uint32 object_id) {
	MapObject* object = GetObject(object_id);

	if (object == NULL) {
		return NULL;
	}

	VirtualSprite* sprite = dynamic_cast<VirtualSprite*>(object);
	if (sprite == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "could not cast map object to sprite type, object id: " << object_id << endl;
		return NULL;
	}

	return sprite;
}



void ObjectSupervisor::SortObjects() {
	std::sort(_ground_objects.begin(), _ground_objects.end(), MapObject_Ptr_Less());
	std::sort(_pass_objects.begin(), _pass_objects.end(), MapObject_Ptr_Less());
	std::sort(_sky_objects.begin(), _sky_objects.end(), MapObject_Ptr_Less());
}



void ObjectSupervisor::Load(ReadScriptDescriptor& map_file) {
	// ---------- Construct the collision grid
	map_file.OpenTable("map_grid");
	_num_grid_rows = map_file.GetTableSize();
	for (uint16 r = 0; r < _num_grid_rows; r++) {
		_collision_grid.push_back(vector<uint32>());
		map_file.ReadUIntVector(r, _collision_grid.back());
	}
	map_file.CloseTable();
	_num_grid_cols = _collision_grid[0].size();
}



void ObjectSupervisor::Update() {
	for (uint32 i = 0; i < _ground_objects.size(); i++) {
		_ground_objects[i]->Update();
	}
	for (uint32 i = 0; i < _pass_objects.size(); i++) {
		_pass_objects[i]->Update();
	}
	for (uint32 i = 0; i < _sky_objects.size(); i++) {
		_sky_objects[i]->Update();
	}

	for (uint32 i = 0; i < _zones.size(); i++) {
		_zones[i]->Update();
	}

	// TODO: examine all sprites for movement and context change, then check all resident zones to see if the sprite has entered
}



void ObjectSupervisor::DrawGroundObjects(const MapFrame* const frame, const bool second_pass) {
	for (uint32 i = 0; i < _ground_objects.size(); i++) {
		if (_ground_objects[i]->draw_on_second_pass == second_pass) {
			_ground_objects[i]->Draw();
		}
	}
}



void ObjectSupervisor::DrawPassObjects(const MapFrame* const frame) {
	for (uint32 i = 0; i < _pass_objects.size(); i++) {
		_pass_objects[i]->Draw();
	}
}



void ObjectSupervisor::DrawSkyObjects(const MapFrame* const frame) {
	for (uint32 i = 0; i < _sky_objects.size(); i++) {
		_sky_objects[i]->Draw();
	}
}

void ObjectSupervisor::DrawDialogIcons(const MapFrame* const frame) {
    MapSprite *mapSprite;
	for (uint32 i = 0; i < _ground_objects.size(); i++) {
		if (_ground_objects[i]->GetObjectType() == SPRITE_TYPE) {
		    mapSprite = static_cast<MapSprite*>(_ground_objects[i]);
			mapSprite->DrawDialog();
		}
	}
}

MapObject* ObjectSupervisor::FindNearestObject(const VirtualSprite* sprite, float search_distance) {
	// NOTE: We don't check if the argument is NULL here for performance reasons
	MapRectangle search_area;

	// ---------- (1) Using the sprite's direction, determine the boundaries of the search area to check for objects
	sprite->GetCollisionRectangle(search_area);
	if (sprite->direction & FACING_NORTH) {
		search_area.bottom = search_area.top;
		search_area.top = search_area.top - search_distance;
	}
	else if (sprite->direction & FACING_SOUTH) {
		search_area.top = search_area.bottom;
		search_area.bottom = search_area.bottom + search_distance;
	}
	else if (sprite->direction & FACING_WEST) {
		search_area.right = search_area.left;
		search_area.left = search_area.left - search_distance;
	}
	else if (sprite->direction & FACING_EAST) {
		search_area.left = search_area.right;
		search_area.right = search_area.right + search_distance;
	}
	else {
		IF_PRINT_WARNING(MAP_DEBUG) << "sprite was set to invalid direction: " << sprite->direction << endl;
		return NULL;
	}

	// ---------- (2) Go through all objects and determine which (if any) lie within the search area
	vector<MapObject*> valid_objects; // A vector to hold objects which are inside the search area (either partially or fully)
	vector<MapObject*>* search_vector = NULL; // A pointer to the vector of objects to search

	// Only search the object layer that the sprite resides on. Note that we do not consider searching the pass layer.
	if (sprite->sky_object == true)
		search_vector = &_sky_objects;
	else
		search_vector = &_ground_objects;

	for (vector<MapObject*>::iterator i = (*search_vector).begin(); i != (*search_vector).end(); i++) {
		if (*i == sprite) // Don't allow the sprite itself to be considered in the search
			continue;

		// If the object and sprite do not exist in the same context, do not consider the object for the search
		if (((*i)->context & sprite->context) == 0)
			continue;

		MapRectangle object_rect;
		(*i)->GetCollisionRectangle(object_rect);
		if (MapRectangle::CheckIntersection(object_rect, search_area) == true)
			valid_objects.push_back(*i);
	} // for (map<MapObject*>::iterator i = _all_objects.begin(); i != _all_objects.end(); i++)

	// ---------- (3) Check for early exit conditions
	if (valid_objects.empty() == true) {
		return NULL;
	}
	else if (valid_objects.size() == 1) {
		return valid_objects[0];
	}

	// ---------- (4) Figure out which of the valid objects is the closest to the sprite
	// NOTE: For simplicity, we use the Manhattan distance to determine which object is the closest
	MapObject* closest_obj = valid_objects[0];

	// Used to hold the full position coordinates of the sprite
	float source_x = sprite->ComputeXLocation();
	float source_y = sprite->ComputeYLocation();
	// Holds the minimum distance found between the sprite and a valid object
	float min_distance = fabs(source_x - closest_obj->ComputeXLocation()) +
		fabs(source_y - closest_obj->ComputeYLocation());

	for (uint32 i = 1; i < valid_objects.size(); i++) {
		float dist = fabs(source_x - valid_objects[i]->ComputeXLocation()) +
			fabs(source_y - valid_objects[i]->ComputeYLocation());
		if (dist < min_distance) {
			closest_obj = valid_objects[i];
			min_distance = dist;
		}
	}
	return closest_obj;
} // MapObject* ObjectSupervisor::FindNearestObject(VirtualSprite* sprite, float search_distance)



bool ObjectSupervisor::CheckMapCollision(const private_map::MapObject* const obj) {
	// NOTE: We don't check if the argument is NULL here for performance reasons
	if (obj->no_collision == true) {
		return false;
	}

	MapRectangle coll_rect;
	obj->GetCollisionRectangle(coll_rect);

	// Check if any part of the object's collision rectangle is outside of the map boundary
	if (coll_rect.left < 0.0f || coll_rect.right >= static_cast<float>(_num_grid_cols) ||
		coll_rect.top < 0.0f || coll_rect.bottom >= static_cast<float>(_num_grid_rows)) {
		return true;
	}

	// Grid based collision is not done for objects in the sky layer
	if (obj->sky_object == true) {
		return false;
	}

	// Determine if the object's collision rectangle overlaps any unwalkable tiles
	// Note that because the sprite's collision rectangle was previously determined to be within the map bounds,
	// the map grid tile indeces referenced in this loop are all valid entries and do not need to be checked.
	for (uint32 r = static_cast<uint32>(coll_rect.top); r <= static_cast<uint32>(coll_rect.bottom); r++) {
		for (uint32 c = static_cast<uint32>(coll_rect.left); c <= static_cast<uint32>(coll_rect.right); c++) {
			// Checks the collision grid at the row-column at the object's current context
			if ((_collision_grid[r][c] & obj->context) != 0) {
				return true;
			}
		}
	}

	return false;
}



bool ObjectSupervisor::CheckObjectCollision(const MapRectangle& rect, const private_map::MapObject* const obj) {
	// NOTE: We don't check if the argument is NULL here for performance reasons
	MapRectangle obj_rect;
	obj->GetCollisionRectangle(obj_rect);
	return MapRectangle::CheckIntersection(rect, obj_rect);
}



bool ObjectSupervisor::DoObjectsCollide(const MapObject* const obj1, const MapObject* const obj2) {
	// NOTE: We don't check if the arguments are NULL here for performance reasons

	// Check if either of the two objects have the no_collision property enabled
	if (obj1->no_collision == true || obj2->no_collision == true) {
		return false;
	}

	// If the two objects are not contained within the same context, they can not overlap
	if (obj1->context != obj2->context) {
		return false;
	}

	MapRectangle rect1, rect2;
	obj1->GetCollisionRectangle(rect1);
	obj2->GetCollisionRectangle(rect2);

	return MapRectangle::CheckIntersection(rect1, rect2);
}



COLLISION_TYPE ObjectSupervisor::DetectCollision(VirtualSprite* sprite, MapObject** collision_object) {
	// NOTE: We don't check if the argument is NULL here for performance reasons

	// If the sprite has this property set it can not collide
	if (sprite->no_collision == true) {
		return NO_COLLISION;
	}

	MapRectangle coll_rect;
	sprite->GetCollisionRectangle(coll_rect);

	// ---------- (1) Check if any part of the object's collision rectangle is outside of the map boundary
	if (coll_rect.left < 0.0f || coll_rect.right >= static_cast<float>(_num_grid_cols) ||
		coll_rect.top < 0.0f || coll_rect.bottom >= static_cast<float>(_num_grid_rows)) {
		return BOUNDARY_COLLISION;
	}

	// ---------- (2) Check if the object's collision rectangel overlaps with any unwalkable elements on the collision grid
	// Grid based collision is not done for objects in the sky layer
	if (sprite->sky_object == false) {
		// Determine if the object's collision rectangle overlaps any unwalkable tiles
		// Note that because the sprite's collision rectangle was previously determined to be within the map bounds,
		// the map grid tile indeces referenced in this loop are all valid entries and do not need to be checked for out-of-bounds conditions
		for (uint32 r = static_cast<uint32>(coll_rect.top); r <= static_cast<uint32>(coll_rect.bottom); r++) {
			for (uint32 c = static_cast<uint32>(coll_rect.left); c <= static_cast<uint32>(coll_rect.right); c++) {
				// Checks the collision grid at the row-column at the object's current context
				if ((_collision_grid[r][c] & sprite->context) != 0) {
					return GRID_COLLISION;
				}
			}
		}
	}

	// ---------- (3) Determine which set of objects to do collision detection with
	MapObject* obstruction_object = NULL;
	vector<MapObject*>* objects = NULL; // A pointer to the layer of objects to do the collision detection with
	if (sprite->sky_object == false)
		objects = &_ground_objects;
	else
		objects = &_sky_objects;

	// ---------- (4) Check collision areas for all objects matching the layer and context of the sprite
	MapRectangle sprite_rect;
	sprite->GetCollisionRectangle(sprite_rect);

	for (uint32 i = 0; i < objects->size(); i++) {
		// Check for conditions where we would not want to do collision detection between the two objects
		if ((*objects)[i]->object_id == sprite->object_id) // Object and sprite are the same
			continue;
		if ((*objects)[i]->no_collision == true) // Object has no collision detection property set
			continue;
		if (((*objects)[i]->context & sprite->context) == 0) // Sprite and object do not exist in the same context
			continue;

		if (CheckObjectCollision(sprite_rect, (*objects)[i]) == true) {
			obstruction_object = (*objects)[i];
			break;
		}
	}

	if (obstruction_object != NULL) {
		if (collision_object != NULL) {
			*collision_object = obstruction_object;
		}
		return OBJECT_COLLISION;
	}

	return NO_COLLISION;
} // bool ObjectSupervisor::DetectCollision(VirtualSprite* sprite, MapObject** collision_object)



MapObject* ObjectSupervisor::IsPositionOccupied(int16 row, int16 col) {
	vector<MapObject*>* objects = &_ground_objects;

	uint16 tmp_x;
	uint16 tmp_y;
	float tmp_x_offset;
	float tmp_y_offset;

	for (uint32 i = 0; i < objects->size(); i++) {
		(*objects)[i]->GetXPosition(tmp_x, tmp_x_offset);
		(*objects)[i]->GetYPosition(tmp_y, tmp_y_offset);

		if (col >= tmp_x - (*objects)[i]->GetCollHalfWidth() && col <= tmp_x + (*objects)[i]->GetCollHalfWidth()) {
			if (row <= tmp_y + (*objects)[i]->GetCollHeight() && row >= tmp_y) {
				return (*objects)[i];
			}
		}
	}

	return NULL;
}



bool ObjectSupervisor::IsPositionOccupiedByObject(int16 row, int16 col, MapObject* object) {
	if (object == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "NULL pointer passed into function argument" << endl;
		return false;
	}

	uint16 tmp_x;
	uint16 tmp_y;
	float tmp_x_offset;
	float tmp_y_offset;

	object->GetXPosition(tmp_x, tmp_x_offset);
	object->GetYPosition(tmp_y, tmp_y_offset);

	if (col >= tmp_x - object->GetCollHalfWidth() && col <= tmp_x + object->GetCollHalfWidth()) {
		if (row <= tmp_y + object->GetCollHeight() && row >= tmp_y) {
			return true;
		}
	}
	return false;
}



bool ObjectSupervisor::AdjustSpriteAroundCollision(private_map::VirtualSprite* sprite, COLLISION_TYPE coll_type, MapObject* coll_obj) {
	// 1) Check for special cases where we do not want to adjust the sprite's position even though this function was called
	// If the sprite collided with another sprite that is moving and this sprite is not the map camera (not player-controlled),
	// don't attempt any adjustments. Instead we allow the other sprite to make its own adjustments.
	// TODO: maybe in this case, we should allow for position alignment but no other forms of movement adjustment
	if ((sprite != MapMode::CurrentInstance()->GetCamera()) && (coll_type == OBJECT_COLLISION)) {
		MAP_OBJECT_TYPE obj_type = coll_obj->GetType();
		if ((obj_type == VIRTUAL_TYPE) || (obj_type == SPRITE_TYPE) || (obj_type == ENEMY_TYPE)) {
			VirtualSprite* coll_sprite = dynamic_cast<VirtualSprite*>(coll_obj);
			if (coll_sprite->moving == true) {
				return false;
			}
		}
	}

	// Retrieve collision rectangle of the sprite and the collision object if available
	MapRectangle sprite_coll_rect, object_coll_rect;
	sprite->GetCollisionRectangle(sprite_coll_rect);
	if (coll_obj != NULL) {
		coll_obj->GetCollisionRectangle(object_coll_rect);
	}

	// Attempt alignment and adjustment changes to the sprite as appropriate
	if (sprite->direction & MOVING_ORTHOGONALLY) {
		if (_AlignSpriteWithCollision(sprite, sprite->direction, coll_type, sprite_coll_rect, object_coll_rect) == true) {
			return true;
		}
		else if (coll_type != BOUNDARY_COLLISION) {
			return _MoveSpriteAroundCollisionCorner(sprite, coll_type, sprite_coll_rect, object_coll_rect);
		}
	}
	else { // then (sprite->direction & MOVING_DIAGONALLY)
		return _MoveSpriteAroundCollisionDiagonal(sprite, coll_type, sprite_coll_rect, object_coll_rect);
	}
	return false;
} // bool ObjectSupervisor::AdjustSpriteAroundCollision(private_map::VirtualSprite* sprite, COLLISION_TYPE coll_type, MapObject* coll_obj);



bool ObjectSupervisor::FindPath(VirtualSprite* sprite, vector<PathNode>& path, const PathNode& dest) {
	// NOTE: Refer to the implementation of the A* algorithm to understand what all these lists and score values are for
	std::vector<PathNode> open_list;
	std::vector<PathNode> closed_list;

	// The starting node of this path discovery
	PathNode source_node(static_cast<int16>(sprite->y_position), static_cast<int16>(sprite->x_position));

	// The current "best node"
	PathNode best_node;
	// Used to hold the eight adjacent nodes
	PathNode nodes[8];

	// Temporary delta variables used in calculation of a node's heuristic (h score)
	uint32 x_delta, y_delta;
	// The number to add to a node's g_score, depending on whether it is a lateral or diagonal movement
	int16 g_add;

	// Original offset for sprite
	float x_offset, y_offset;

	path.clear();

	// Check that the source node is not the same as the destination node
	if (source_node == dest) {
		PRINT_ERROR << "source node coordinates are the same as the destination" << endl;
		return false;
	}

	// Check that the destination is valid for the sprite to move to
	x_offset = sprite->x_offset;
	y_offset = sprite->y_offset;

	sprite->x_position = dest.col;
	sprite->y_position = dest.row;
	// Don't use 0.0f here for both since errors at the border between two positions may occure, especially when running
	sprite->x_offset = 0.5f;
	sprite->y_offset = 0.5f;

	if (DetectCollision(sprite, NULL) != NO_COLLISION) {
		sprite->x_position = source_node.col;
		sprite->y_position = source_node.row;
		sprite->x_offset = x_offset;
		sprite->y_offset = y_offset;
		PRINT_ERROR << "sprite can not move to destination node on path because one or more grid tiles are unwalkable" << endl;
		return false;
	}

	open_list.push_back(source_node);

	while (open_list.empty() == false) {
		sort(open_list.begin(), open_list.end());
		best_node = open_list.back();
		open_list.pop_back();
		closed_list.push_back(best_node);

		// Check if destination has been reached, and break out of the loop if so
		if (best_node == dest) {
			break;
		}

		// Setup the coordinates of the 8 adjacent nodes to the best node
		nodes[0].row = best_node.row - 1; nodes[0].col = best_node.col;
		nodes[1].row = best_node.row + 1; nodes[1].col = best_node.col;
		nodes[2].row = best_node.row;     nodes[2].col = best_node.col - 1;
		nodes[3].row = best_node.row;     nodes[3].col = best_node.col + 1;
		nodes[4].row = best_node.row - 1; nodes[4].col = best_node.col - 1;
		nodes[5].row = best_node.row - 1; nodes[5].col = best_node.col + 1;
		nodes[6].row = best_node.row + 1; nodes[6].col = best_node.col - 1;
		nodes[7].row = best_node.row + 1; nodes[7].col = best_node.col + 1;

		// Check the eight adjacent nodes
		for (uint8 i = 0; i < 8; ++i) {
			// ---------- (A): Check if all tiles are walkable
			sprite->x_position = nodes[i].col;
			sprite->y_position = nodes[i].row;

			if (DetectCollision(sprite, NULL) != NO_COLLISION) {
				continue;
			}

			// ---------- (B): Check if the node is already in the closed list
			if (find(closed_list.begin(), closed_list.end(), nodes[i]) != closed_list.end()) {
				continue;
			}

			// ---------- (C): If this point has been reached, the node is valid for the sprite to move to
			// If this is a lateral adjacent node, g_score is +10, otherwise diagonal adjacent node is +14
			if (i < 4)
				g_add = 10;
			else
				g_add = 14;

			// Set the node's parent and calculate its g_score
			nodes[i].parent_row = best_node.row;
			nodes[i].parent_col = best_node.col;
			nodes[i].g_score = best_node.g_score + g_add;

			// ---------- (D): Check to see if the node is already on the open list and update it if necessary
			vector<PathNode>::iterator iter = find(open_list.begin(), open_list.end(), nodes[i]);
			if (iter != open_list.end()) {
				// If its G is higher, it means that the path we are on is better, so switch the parent
				if (iter->g_score > nodes[i].g_score) {
					iter->g_score = nodes[i].g_score;
					iter->f_score = nodes[i].g_score + iter->h_score;
					iter->parent_row = nodes[i].parent_row;
					iter->parent_col = nodes[i].parent_col;
				}
			}
			// ---------- (E): Add the new node to the open list
			else {
				// Calculate the H and F score of the new node (the heuristic used is diagonal)
				x_delta = abs(dest.col - nodes[i].col);
				y_delta = abs(dest.row - nodes[i].row);
				if (x_delta > y_delta)
					nodes[i].h_score = 14 * y_delta + 10 * (x_delta - y_delta);
				else
					nodes[i].h_score = 14 * x_delta + 10 * (y_delta - x_delta);

				nodes[i].f_score = nodes[i].g_score + nodes[i].h_score;
				open_list.push_back(nodes[i]);
			}
		} // for (uint8 i = 0; i < 8; ++i)
	} // while (open_list.empty() == false)

	if (open_list.empty() == true) {
		IF_PRINT_WARNING(MAP_DEBUG) << "could not find path to destination" << endl;
		return false;
	}

	// Add the destination node to the vector, retain its parent, and remove it from the closed list
	path.push_back(best_node);
	int16 parent_row = best_node.parent_row;
	int16 parent_col = best_node.parent_col;
	closed_list.pop_back();

	// Go backwards through the closed list following the parent nodes to construct the path
	for (vector<PathNode>::iterator iter = closed_list.end() - 1; iter != closed_list.begin(); --iter) {
		if (iter->col == parent_col && iter->row == parent_row) {
			path.push_back(*iter);
			parent_col = iter->parent_col;
			parent_row = iter->parent_row;
		}
	}
	std::reverse(path.begin(), path.end());

	// Move sprite back to original position
	sprite->x_position = source_node.col;
	sprite->y_position = source_node.row;
	sprite->x_offset = x_offset;
	sprite->y_offset = y_offset;

	return true;
} // bool ObjectSupervisor::FindPath(const VirtualSprite* sprite, std::vector<PathNode>& path, const PathNode& dest)



bool ObjectSupervisor::_AlignSpriteWithCollision(VirtualSprite* sprite, uint16 direction, COLLISION_TYPE coll_type,
	const MapRectangle& sprite_coll_rect, const MapRectangle& object_coll_rect)
{
	if ((direction != NORTH) && (direction != SOUTH) && (direction != EAST) && (direction != WEST)) {
		IF_PRINT_WARNING(MAP_DEBUG) << "invalid direction argument: " << direction << endl;
		return false;
	}

	// ---------- (1): Determine the border coordinates that should be examined
	// Used to hold the proper coordinate of the sprite and the collision border
	float pos_sprite = 0.0f, pos_border = 0.0f;

	if (coll_type == BOUNDARY_COLLISION) {
		switch (direction) {
			case NORTH:
				pos_sprite = sprite_coll_rect.top;
				pos_border = 0.0f;
				break;
			case SOUTH:
				pos_sprite = sprite_coll_rect.bottom;
				pos_border = static_cast<float>(_num_grid_rows);
				break;
			case EAST:
				pos_sprite = sprite_coll_rect.right;
				pos_border = static_cast<float>(_num_grid_cols);
				break;
			case WEST:
				pos_sprite = sprite_coll_rect.left;
				pos_border = 0.0f;
				break;
		}
	}
	else if (coll_type == GRID_COLLISION) {
		// When aligning with the grid, we only need to check that the fractional part of the float is equal to 0.0f
		switch (direction) {
			case NORTH:
				pos_sprite = GetFloatFraction(sprite_coll_rect.top);
				pos_border = 0.0f;
				break;
			case SOUTH:
				pos_sprite = 1.0f - GetFloatFraction(sprite_coll_rect.bottom);
				pos_border = 0.0f;
				break;
			case EAST:
				pos_sprite = 1.0f - GetFloatFraction(sprite_coll_rect.right);
				pos_border = 0.0f;
				break;
			case WEST:
				pos_sprite = GetFloatFraction(sprite_coll_rect.left);
				pos_border = 0.0f;
				break;
		}
	}
	else if (coll_type == OBJECT_COLLISION) {
		switch (direction) {
			case NORTH:
				pos_sprite = sprite_coll_rect.top;
				pos_border = object_coll_rect.bottom;
				break;
			case SOUTH:
				pos_sprite = sprite_coll_rect.bottom;
				pos_border = object_coll_rect.top;
				break;
			case EAST:
				pos_sprite = sprite_coll_rect.right;
				pos_border = object_coll_rect.left;
				break;
			case WEST:
				pos_sprite = sprite_coll_rect.left;
				pos_border = object_coll_rect.right;
				break;
		}
	}
	else {
		IF_PRINT_WARNING(MAP_DEBUG) << "invalid collision type: " << coll_type << endl;
		return false;
	}

	// ---------- (2): Check if the sprite is already aligned and modify the sprite's position if it is not
	if (IsFloatEqual(pos_sprite, pos_border, 0.001f) == true) {
		return false;
	}
	else {
		// 0.0005f is subtracted from the distance so that the alignment is never completely perfect. If it was perfect,
		// the alignment would fail because of the collision detection algorithm. For example, if we try to align a sprite
		// moving south with a row of collision grid elements at row 42, if we set the sprite's collision rectangle bottom to
		// 42.0f then the collision detection algorithm will include the collision grid row at 42 in its detection. So instead
		// we set the collision rectangle bottom to something slightly less than 42.0f (~41.9995f) so that this doesn't happen.
		float distance = fabs(pos_border - pos_sprite - 0.0005f);
		return _ModifySpritePosition(sprite, direction, distance);
	}
} // bool _AlignSpriteWithCollision(VirtualSprite* sprite, uint16 direction, COLLISION_TYPE coll_type ... )



bool ObjectSupervisor::_MoveSpriteAroundCollisionCorner(VirtualSprite* sprite, COLLISION_TYPE coll_type,
	const MapRectangle& sprite_coll_rect, const MapRectangle& object_coll_rect)
{
	// A horizontal adjustment means that the sprite was trying to move vertically and needs to be adjusted horizontally around a collision
	bool horizontal_adjustment = (sprite->direction & (NORTH | SOUTH));
	// Determines if the start or end directions of the grid should be examined in future steps
	bool check_start = true, check_end = true;

	// ---------- (1): If this was an object collision, first check if there is a corner close enough to move around
	if (coll_type == OBJECT_COLLISION) {
		if (horizontal_adjustment == true) {
			if (object_coll_rect.left < sprite_coll_rect.left) {
				check_start = false;
			}
			if (object_coll_rect.right > sprite_coll_rect.right) {
				check_end = false;
			}
		}
		else {
			if (object_coll_rect.top < sprite_coll_rect.top) {
				check_start = false;
			}
			if (object_coll_rect.bottom > sprite_coll_rect.bottom) {
				check_end = false;
			}
		}

		// If the object is big enough that the corners are far away, there's nothing more that can be done
		if ((check_start == false) && (check_end == false)) {
			return false;
		}
	}

	// ---------- (2): Determine the length of the sprite and the start/end points of the collision grid line to examine
	// The length or height of the sprite that determines the dimensions of the line, in collision grid units
	uint16 sprite_length;
	// Stores the col/row endpoints of the collision grid line
	// For horizontal adjustments, start/end represents is left/right directions while vertical adjustments represent top/bottom
	int16 start_point, end_point = 0;

	if (horizontal_adjustment == true) {
		// +1 is added since the cast throws away everything after the decimal and we want a ceiling integer
		sprite_length = 1 + static_cast<uint16>(sprite_coll_rect.right - sprite_coll_rect.left);
		start_point = sprite->x_position - ((3 * sprite_length) / 2);
		end_point = start_point + (3 * sprite_length);

		// Ensure that the line end points do not go outside of the map boundaries.
		start_point = (start_point < 0) ? 0 : start_point;
		end_point = (end_point >= _num_grid_cols) ? _num_grid_cols : end_point;
	}
	else {
		// +1 is added since the cast throws away everything after the decimal and we want a ceiling integer
		sprite_length = 1 + static_cast<uint16>(sprite_coll_rect.bottom - sprite_coll_rect.top);
		start_point = sprite->y_position - (2 * sprite_length);
		end_point = start_point + (3 * sprite_length);

		// Ensure that the line end points do not go outside of the map boundaries.
		start_point = (start_point < 0) ? 0 : start_point;
		end_point = (end_point >= _num_grid_rows) ? _num_grid_rows : end_point;
	}

	// ---------- (3): Determine the collision grid line axis based on the direction the sprite is trying to move
	// Stores the row/col axis of the line
	int16 line_axis = 0;

	switch (sprite->direction) {
		case NORTH:
			// Set to the row above the top of the sprite's collision rectangle
			line_axis = static_cast<int16>(sprite_coll_rect.top) - 1;
			line_axis = (line_axis >= 0) ? line_axis : 0;
			break;
		case SOUTH:
			// Set to the row below the bottom of the sprite's collision rectangle
			line_axis = static_cast<int16>(sprite_coll_rect.bottom) + 1;
			line_axis = (line_axis < _num_grid_rows) ? line_axis : _num_grid_rows - 1;
			break;
		case EAST:
			// Set to the column to the right of the right edge of the sprite's collision rectangle
			line_axis = static_cast<int16>(sprite_coll_rect.right) + 1;
			line_axis = (line_axis < _num_grid_cols) ? line_axis : _num_grid_cols - 1;
			break;
		case WEST:
			// Set to the column to the left of the left edge of the sprite's collision rectangle
			line_axis = static_cast<int16>(sprite_coll_rect.left) - 1;
			line_axis = (line_axis >= 0) ? line_axis : 0;
			break;
	}

	// ---------- (4): Populate the line based upon the collision grid and sprite context information
	// A vector of bools used to represent the collision grid line in front of the sprite that will be examined
	// True values in this vector indicate that the indexed area is not available for the sprite to move to
	// Note that (end_point - start_point) is usually equal to sprite_length * 3, except in some boundary conditions
	// when the grid line is made shorter
	vector<bool> grid_line(end_point - start_point);

	if (horizontal_adjustment == true) {
		for (uint16 i = start_point, j = 0; i <= end_point && i < _collision_grid[line_axis].size(); i++, j++) {
			grid_line[j] = (_collision_grid[line_axis][i] & sprite->context);
		}
	}
	else {
		for (uint16 i = start_point, j = 0; i <= end_point && i < _collision_grid.size(); i++, j++) {
			grid_line[j] = (_collision_grid[i][line_axis] & sprite->context);
		}
	}

	// ---------- (5): Starting from the center, examine both sides of the line for a gap wide enough for the sprite to fit through
	// A counter used for finding a gap of the appropriate size
	uint16 gap_counter = 0;
	// Used to determine how close the nearest available gap is
	int16 start_distance = -1, end_distance = -1;

	// Examine the line segment from the center to the start point
	if (check_start == true) {
		gap_counter = 0;
		for (int16 i = grid_line.size() / 2, j = 0; i >= 0; i--, j++) {
			if (grid_line[i] == true) {
				start_distance = -1;
				gap_counter = 0;
			}
			else {
				if (gap_counter == 0) {
					start_distance = j;
				}
				gap_counter++;
				if (gap_counter == sprite_length) {
					break;
				}
			}
		}
		// If no gap that was large enough was found, the sprite shouldn't adjust itself in the start direction
		if (gap_counter != sprite_length) {
			check_start = false;
		}
	}
	// Examine the line segement from the center to the end point
	if (check_end == true) {
		gap_counter = 0;
		for (int16 i = grid_line.size() / 2, j = 0; i < static_cast<int16>(grid_line.size()); i++, j++) {
			if (grid_line[i] == true) {
				end_distance = -1;
				gap_counter = 0;
			}
			else {
				if (gap_counter == 0) {
					end_distance = j;
				}
				gap_counter++;
				if (gap_counter == sprite_length) {
					break;
				}
			}
		}
		// If no gap that was large enough was found, the sprite shouldn't adjust itself in the start direction
		if (gap_counter != sprite_length) {
			check_end = false;
		}
	}

	// If no gaps were found there's nothing else that can be done here
	if ((check_start == false) && (check_end == false)) {
		return false;
	}

	// ---------- (6): Determine which side has the closest gap for the sprite to go through
	bool move_in_start_direction;

	if ((check_start == true) && (check_end == false)) {
		move_in_start_direction = true;
	}
	else if ((check_start == false) && (check_end == true)) {
		move_in_start_direction = false;
	}
	// In the following cases, both start and end sides are valid to make adjustments to
	else if (coll_type != OBJECT_COLLISION) {
		// Adjust in the position of least grid distance
		move_in_start_direction = (start_distance <= end_distance) ? true : false;
	}
	else {
		// In this case, the collided object must have a collision rectangle that is less than or equal to the width/height of the
		// sprite's collision rectangle. The appropriate sides (left/right or top/bottom) of the object can also not exceed beyond
		// the boundaries of the sprite. So we need to find out which side (start or end) has the most difference between the two
		// objects' edges and move the sprite in the direction of least distance.
		if (horizontal_adjustment == true) {
			move_in_start_direction = ((sprite_coll_rect.right - object_coll_rect.left) < (object_coll_rect.right - sprite_coll_rect.left)) ?
				true : false;
		}
		else {
			move_in_start_direction = ((sprite_coll_rect.bottom - object_coll_rect.top) < (object_coll_rect.bottom - sprite_coll_rect.top)) ?
				true : false;
		}
	}

	// ---------- (7): Adjust the sprite's movement in the appropriate direction
	uint16 direction;
	if (horizontal_adjustment == true) {
		direction = (move_in_start_direction == true) ? WEST : EAST;
	}
	else {
		direction = (move_in_start_direction == true) ? NORTH : SOUTH;
	}

	// Move the sprite in the appropriate direction and reduce the distance moved for this type of movement
	// The reduction of movement distance by sin(45) is the same factor that is used for diagonal movement
	return _ModifySpritePosition(sprite, direction, sprite->CalculateDistanceMoved() * 0.707f);
} // bool ObjectSupervisor::_MoveSpriteAroundCollisionCorner(VirtualSprite* sprite, COLLISION_TYPE coll_type ... )



bool ObjectSupervisor::_MoveSpriteAroundCollisionDiagonal(VirtualSprite* sprite, COLLISION_TYPE coll_type,
	const MapRectangle& sprite_coll_rect, const MapRectangle& object_coll_rect)
{
	// Determines the horizontal and vertical directions to examine (north/south and east/west correspond to true/false)
	bool north_or_south = false, east_or_west = false;
	// Determines if horizontal or vertical collision alignment should be performed
	bool check_horizontal_align = false, check_vertical_align = false;

	// ---------- (1): Determine the orthogonal movement directions and reconstruct the sprite's invalid collision rectangle
	switch (sprite->direction) {
		case NE_NORTH:
		case NE_EAST:
			north_or_south = true;
			east_or_west = true;
			break;
		case NW_NORTH:
		case NW_WEST:
			north_or_south = true;
			east_or_west = false;
			break;
		case SE_SOUTH:
		case SE_EAST:
			north_or_south = false;
			east_or_west = true;
			break;
		case SW_SOUTH:
		case SW_WEST:
			north_or_south = false;
			east_or_west = false;
			break;
	}

	// Reconstruct the sprite's collision rectangle at the state when it encountered the collision
	MapRectangle mod_sprite_rect = sprite_coll_rect;

	float distance_moved = sprite->CalculateDistanceMoved();
	if (north_or_south == true) {
		mod_sprite_rect.top -= distance_moved;
		mod_sprite_rect.bottom -= distance_moved;
	}
	else {
		mod_sprite_rect.top += distance_moved;
		mod_sprite_rect.bottom += distance_moved;
	}
	if (east_or_west == true) {
		mod_sprite_rect.left += distance_moved;
		mod_sprite_rect.right += distance_moved;
	}
	else {
		mod_sprite_rect.left -= distance_moved;
		mod_sprite_rect.right -= distance_moved;
	}

	// ---------- (2): Determine whether the collision occurred in the horizontal or vertical direction (or both)
	if (coll_type == BOUNDARY_COLLISION) {
		if (north_or_south == true) {
			check_vertical_align = (mod_sprite_rect.top < 0.0f) ? true : false;
		}
		else {
			check_vertical_align = (mod_sprite_rect.bottom > static_cast<float>(_num_grid_rows)) ? true : false;
		}
		if (sprite->direction == MOVING_EASTWARD) {
			check_horizontal_align = (mod_sprite_rect.right > static_cast<float>(_num_grid_cols)) ? true : false;
		}
		else {
			check_horizontal_align = (mod_sprite_rect.left < 0.0f) ? true : false;
		}
	}
	else if (coll_type == GRID_COLLISION) {
		uint32 axis;

		check_vertical_align = false;
		axis = (north_or_south == true) ? static_cast<uint32>(mod_sprite_rect.top) : static_cast<uint32>(mod_sprite_rect.bottom);
		for (uint32 i = static_cast<uint32>(sprite_coll_rect.left); i <= static_cast<uint32>(sprite_coll_rect.right); i++) {
			if (_collision_grid[axis][i] & sprite->context) {
				check_vertical_align = true;
				break;
			}
		}

		check_horizontal_align = false;
		axis = (east_or_west == true) ? static_cast<uint32>(mod_sprite_rect.right) : static_cast<uint32>(mod_sprite_rect.left);
		for (uint32 i = static_cast<uint32>(sprite_coll_rect.top); i <= static_cast<uint32>(sprite_coll_rect.bottom); i++) {
			if (_collision_grid[i][axis] & sprite->context) {
				check_horizontal_align = true;
				break;
			}
		}
	}
	else if (coll_type == OBJECT_COLLISION) {
		if (north_or_south == true) {
			check_vertical_align = (sprite_coll_rect.top > object_coll_rect.bottom) ? true : false;
		}
		else {
			check_vertical_align = (sprite_coll_rect.bottom < object_coll_rect.top) ? true : false;
		}
		if (east_or_west == true) {
			check_horizontal_align = (sprite_coll_rect.right < object_coll_rect.left) ? true : false;
		}
		else {
			check_horizontal_align = (sprite_coll_rect.left > object_coll_rect.right) ? true : false;
		}
	}

	// ---------- (3): Perform alignments and adjustments in the appropriate directions
	bool vertical_alignment_performed = false, horizontal_alignment_performed = false;
	if (check_vertical_align == true) {
		vertical_alignment_performed = _AlignSpriteWithCollision(sprite, (north_or_south) ? NORTH : SOUTH,
			coll_type, sprite_coll_rect, object_coll_rect);

	}
	if (check_horizontal_align == true) {
		horizontal_alignment_performed = _AlignSpriteWithCollision(sprite, (east_or_west) ? EAST : WEST,
			coll_type, sprite_coll_rect, object_coll_rect);
	}

	// If the sprite's position was changed due to either type of alignment, don't attempt further position changes
	if ((vertical_alignment_performed == true) || (horizontal_alignment_performed == true)) {
		return true;
	}
	// If both types of alignment were checked but no position change was made, the sprite is already aligned and can not be adjusted further
	else if ((check_vertical_align == true) && (check_horizontal_align == true)) {
		return false;
	}
	// If alignment was only checked in one direction but no position changed occurred, try moving the sprite in the non-aligned direction
	else if ((check_vertical_align == false) && (check_horizontal_align == true)) {
		return _ModifySpritePosition(sprite, (north_or_south) ? NORTH : SOUTH, sprite->CalculateDistanceMoved());
	}
	else if ((check_vertical_align == true) && (check_horizontal_align == false)) {
		return _ModifySpritePosition(sprite, (east_or_west) ? EAST : WEST, sprite->CalculateDistanceMoved());
	}
	else { // then ((check_vertical_align == false) && (check_horizontal_align == false))
		// This case should never happen. If it does, the collision detection algorithm may be at fault here.
		IF_PRINT_WARNING(MAP_DEBUG) << "no alignment check was performed against collision in diagonal movement" << endl;
	}
	return false;
} // bool ObjectSupervisor::_MoveSpriteAroundCollisionDiagonal(VirtualSprite* sprite, COLLISION_TYPE coll_type, ... )



bool ObjectSupervisor::_ModifySpritePosition(VirtualSprite* sprite, uint16 direction, float distance) {
	// Used to save the current position offset in case the adjustment fails
	float saved_offset;

	switch (direction) {
		case NORTH:
			saved_offset = sprite->y_offset;
			sprite->y_offset -= distance;
			break;
		case SOUTH:
			saved_offset = sprite->y_offset;
			sprite->y_offset += distance;
			break;
		case EAST:
			saved_offset = sprite->x_offset;
			sprite->x_offset += distance;
			break;
		case WEST:
			saved_offset = sprite->x_offset;
			sprite->x_offset -= distance;
			break;
		default:
			IF_PRINT_WARNING(MAP_DEBUG) << "invalid direction argument passed to this function: " << direction << endl;
			return false;
	}

	// Check for a collision in the newly adjusted position
	if (DetectCollision(sprite, NULL) != NO_COLLISION) {
		// Restore the sprite's position and give up any further efforts for movement adjustment
		if (direction & (NORTH | SOUTH)) {
			sprite->y_offset = saved_offset;
		}
		else {
			sprite->x_offset = saved_offset;
		}
		return false;
	}
	else {
		// The adjustment was successful, check the position offsets and state that the position has been changed
		sprite->CheckPositionOffsets();
		sprite->moved_position = true;
		return true;
	}
} // bool ObjectSupervisor::_ModifySpritePosition(VirtualSprite* sprite, uint16 direction, float distance)

} // namespace private_map

} // namespace hoa_map
