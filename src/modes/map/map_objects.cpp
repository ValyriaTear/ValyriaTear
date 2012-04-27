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
#include "engine/audio/audio.h"
#include "engine/system.h"
#include "engine/video/video.h"

// Allacrost globals
#include "common/global/global.h"

// Local map mode headers
#include "modes/map/map.h"
#include "modes/map/map_dialogue.h"
#include "modes/map/map_objects.h"
#include "modes/map/map_sprites.h"

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
	if (!visible)
		return false;

	MapMode *map = MapMode::CurrentInstance();

	// If the context is not in one of the active context, don't display it.
	if (!(context & map->GetCurrentContext()))
		return false;

	// Determine if the sprite is off-screen and if so, don't draw it.
	if (!MapRectangle::CheckIntersection(GetImageRectangle(), map->GetMapFrame().screen_edges))
		return false;

	// Determine the center position coordinates for the camera
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
	VideoManager->Move(x_pos - map->GetMapFrame().screen_edges.left,
					   y_pos - map->GetMapFrame().screen_edges.top);
	return true;
} // bool MapObject::ShouldDraw()



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



MapRectangle MapObject::GetCollisionRectangle() const {
	float x_pos = ComputeXLocation();
	float y_pos = ComputeYLocation();

	MapRectangle rect;
	rect.left = x_pos - coll_half_width;
	rect.right = x_pos + coll_half_width;
	rect.top = y_pos - coll_height;
	rect.bottom = y_pos;

	return rect;
}


MapRectangle MapObject::GetCollisionRectangle(uint16 x, uint16 y,
											  float offset_x, float offset_y) const {
	float x_pos = static_cast<float>(x) + offset_x;
	float y_pos = static_cast<float>(y) + offset_y;

	MapRectangle rect;
	rect.left = x_pos - coll_half_width;
	rect.right = x_pos + coll_half_width;
	rect.top = y_pos - coll_height;
	rect.bottom = y_pos;
	return rect;
}


MapRectangle MapObject::GetImageRectangle() const {
	float x_pos = ComputeXLocation();
	float y_pos = ComputeYLocation();

	MapRectangle rect;
	rect.left = x_pos - img_half_width;
	rect.right = x_pos + img_half_width;
	rect.top = y_pos - img_height;
	rect.bottom = y_pos;
	return rect;
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


// Save points
SavePoint::SavePoint(uint16 x, uint16 y, MAP_CONTEXT map_context):
	MapObject(),
	_animations(0),
	_save_active(false)
{
	x_position = x;
	y_position = y;

	_object_type = SAVE_TYPE;
	context = map_context;
	no_collision = true;

	_animations = &MapMode::CurrentInstance()->inactive_save_point_animations;

	// Set the collision rectangle according to the dimensions of the first frame
	// Remove a margin to the save point so that the character has to actually
	// enter the save point before colliding with it.
	SetCollHalfWidth((_animations->at(0).GetWidth() - 1.0f) / 2.0f);
	SetCollHeight(_animations->at(0).GetHeight() - 0.3f);

	// Setup the image collision for the display update
	SetImgHalfWidth(_animations->at(0).GetWidth() / 2.0f);
	SetImgHeight(_animations->at(0).GetHeight());

	// Preload the save active sound
	AudioManager->LoadSound("snd/heal_spell.wav");
}

void SavePoint::Update() {
	if (!_animations)
		return;

	if (updatable) {
		for (uint32 i = 0; i < _animations->size(); ++i)
			_animations->at(i).Update();
	}
}


void SavePoint::Draw() {
	if (!_animations)
		return;

	if (MapObject::ShouldDraw()) {
		for (uint32 i = 0; i < _animations->size(); ++i)
			_animations->at(i).Draw();
	}
}

void SavePoint::SetActive(bool active) {
	if (active) {
		_animations = &MapMode::CurrentInstance()->active_save_point_animations;

		// Play a sound when the save point become active
		if (!_save_active)
			AudioManager->PlaySound("snd/heal_spell.wav");
	}
	else {
		_animations = &MapMode::CurrentInstance()->inactive_save_point_animations;
	}
	_save_active = active;
}


// Halos
Halo::Halo(const std::string& filename, uint16 x, uint16 y, const Color& color, MAP_CONTEXT map_context):
	MapObject()
{
	_color = color;
	x_position = x;
	y_position = y;

	_object_type = HALO_TYPE;
	context = map_context;
	no_collision = true;

	if (_animation.LoadFromAnimationScript(filename)) {
	    MapMode::ScaleToMapCoords(_animation);

	    // Setup the image collision for the display update
	    SetImgHalfWidth(_animation.GetWidth() / 2.0f);
	    SetImgHeight(_animation.GetHeight());
	}
}

void Halo::Update() {
	if (updatable)
		_animation.Update();
}


void Halo::Draw() {
	if (MapObject::ShouldDraw())
		VideoManager->DrawHalo(*_animation.GetCurrentFrame(), _color);
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

	for (uint32 i = 0; i < frames.size(); i++)
		MapMode::ScaleToMapCoords(frames[i]);

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

	// Set the collision rectangle according to the dimensions of the first frame
	SetCollHalfWidth(frames.at(0).GetWidth() / 2.0f);
	SetCollHeight(frames.at(0).GetHeight());
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
	_num_grid_x_axis(0),
	_num_grid_y_axis(0),
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
	for (uint32 i = 0; i < _save_points.size(); ++i) {
		delete(_save_points[i]);
	}
	for (uint32 i = 0; i < _pass_objects.size(); i++) {
		delete(_pass_objects[i]);
	}
	for (uint32 i = 0; i < _sky_objects.size(); i++) {
		delete(_sky_objects[i]);
	}
	for (uint32 i = 0; i < _halos.size(); ++i) {
		delete(_halos[i]);
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
	_num_grid_y_axis = map_file.GetTableSize();
	for (uint16 y = 0; y < _num_grid_y_axis; ++y) {
		_collision_grid.push_back(vector<uint32>());
		map_file.ReadUIntVector(y, _collision_grid.back());
	}
	map_file.CloseTable();
	_num_grid_x_axis = _collision_grid[0].size();
}



void ObjectSupervisor::Update() {
	for (uint32 i = 0; i < _ground_objects.size(); ++i)
		_ground_objects[i]->Update();
	// Update save point animation and activeness.
	_UpdateSavePoints();
	for (uint32 i = 0; i < _pass_objects.size(); ++i)
		_pass_objects[i]->Update();
	for (uint32 i = 0; i < _sky_objects.size(); ++i)
		_sky_objects[i]->Update();
	for (uint32 i = 0; i < _halos.size(); ++i)
		_halos[i]->Update();
	for (uint32 i = 0; i < _zones.size(); ++i)
		_zones[i]->Update();

	// TODO: examine all sprites for movement and context change, then check all resident zones to see if the sprite has entered
}


void ObjectSupervisor::DrawSavePoints() {
	for (uint32 i = 0; i < _save_points.size(); ++i) {
		_save_points[i]->Draw();
	}
}


void ObjectSupervisor::DrawGroundObjects(const bool second_pass) {
	for (uint32 i = 0; i < _ground_objects.size(); i++) {
		if (_ground_objects[i]->draw_on_second_pass == second_pass) {
			_ground_objects[i]->Draw();
		}
	}
}


void ObjectSupervisor::DrawPassObjects() {
	for (uint32 i = 0; i < _pass_objects.size(); i++) {
		_pass_objects[i]->Draw();
	}
}


void ObjectSupervisor::DrawSkyObjects() {
	for (uint32 i = 0; i < _sky_objects.size(); i++) {
		_sky_objects[i]->Draw();
	}
}


void ObjectSupervisor::DrawHalos() {
	for (uint32 i = 0; i < _halos.size(); ++i)
		_halos[i]->Draw();
}


void ObjectSupervisor::DrawDialogIcons() {
    MapSprite *mapSprite;
	for (uint32 i = 0; i < _ground_objects.size(); i++) {
		if (_ground_objects[i]->GetObjectType() == SPRITE_TYPE) {
		    mapSprite = static_cast<MapSprite*>(_ground_objects[i]);
			mapSprite->DrawDialog();
		}
	}
}


void ObjectSupervisor::_UpdateSavePoints() {
	VirtualSprite *sprite = MapMode::CurrentInstance()->GetCamera();

	MapRectangle spr_rect;
	if (sprite)
		spr_rect = sprite->GetCollisionRectangle();

	for (std::vector<SavePoint*>::iterator it = _save_points.begin();
		it != _save_points.end(); ++it) {
		(*it)->SetActive(MapRectangle::CheckIntersection(spr_rect,
										(*it)->GetCollisionRectangle()));
		(*it)->Update();
	}
}


MapObject* ObjectSupervisor::_FindNearestSavePoint(const VirtualSprite* sprite) {
	if (sprite == NULL)
	    return NULL;

	for (std::vector<SavePoint*>::iterator it = _save_points.begin();
		it != _save_points.end(); ++it) {

		// If the object and sprite do not exist in one of the same context,
		// do not consider the object for the search
		if (!((*it)->context & sprite->context))
			continue;

		if (MapRectangle::CheckIntersection(sprite->GetCollisionRectangle(),
										(*it)->GetCollisionRectangle())) {
			return (*it);
		}
	}
	return NULL;
}


MapObject* ObjectSupervisor::FindNearestObject(const VirtualSprite* sprite, float search_distance) {
	// NOTE: We don't check if the argument is NULL here for performance reasons

	// Using the sprite's direction, determine the boundaries of the search area to check for objects
	MapRectangle search_area = sprite->GetCollisionRectangle();
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

	// Go through all objects and determine which (if any) lie within the search area
	vector<MapObject*> valid_objects; // A vector to hold objects which are inside the search area (either partially or fully)
	vector<MapObject*>* search_vector = NULL; // A pointer to the vector of objects to search

	// Only search the object layer that the sprite resides on. Note that we do not consider searching the pass layer.
	if (sprite->sky_object)
		search_vector = &_sky_objects;
	else
		search_vector = &_ground_objects;

	for (vector<MapObject*>::iterator i = (*search_vector).begin(); i != (*search_vector).end(); i++) {
		if (*i == sprite) // Don't allow the sprite itself to be considered in the search
			continue;

		// If the object and sprite do not exist in one of the same contexts,
		// do not consider the object for the search
		if (!((*i)->context & sprite->context))
			continue;

		MapRectangle object_rect = (*i)->GetCollisionRectangle();
		if (MapRectangle::CheckIntersection(object_rect, search_area) == true)
			valid_objects.push_back(*i);
	} // for (map<MapObject*>::iterator i = _all_objects.begin(); i != _all_objects.end(); i++)

	if (valid_objects.empty()) {
		// If no sprite was here, try searching a save point.
		return _FindNearestSavePoint(sprite);
	}
	else if (valid_objects.size() == 1) {
		return valid_objects[0];
	}

	// Figure out which of the valid objects is the closest to the sprite
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



bool ObjectSupervisor::CheckObjectCollision(const MapRectangle& rect, const private_map::MapObject* const obj) {
	// NOTE: We don't check if the argument is NULL here for performance reasons
	MapRectangle obj_rect = obj->GetCollisionRectangle();
	return MapRectangle::CheckIntersection(rect, obj_rect);
}



COLLISION_TYPE ObjectSupervisor::GetCollisionFromObjectType(MapObject *obj) const {
	if (!obj)
		return NO_COLLISION;

	switch (obj->GetType()) {
		case VIRTUAL_TYPE:
		case SPRITE_TYPE:
			return CHARACTER_COLLISION;
			break;
		case ENEMY_TYPE:
			return ENEMY_COLLISION;
			break;
		case TREASURE_TYPE:
		case PHYSICAL_TYPE:
			return WALL_COLLISION;
			break;
		default:
			break;
	}
	return NO_COLLISION;
}



COLLISION_TYPE ObjectSupervisor::DetectCollision(VirtualSprite* sprite,
												 uint16 x, uint16 y,
												 float x_offset, float y_offset,
												 MapObject** collision_object_ptr) {
	// If the sprite has this property set it can not collide
	if (!sprite || sprite->no_collision)
		return NO_COLLISION;

	// Get the collision rectangle at the given position
	MapRectangle sprite_rect = sprite->GetCollisionRectangle(x, y, x_offset, y_offset);

	// Check if any part of the object's collision rectangle is outside of the map boundary
	if (sprite_rect.left < 0.0f || sprite_rect.right >= static_cast<float>(_num_grid_x_axis) ||
		sprite_rect.top < 0.0f || sprite_rect.bottom >= static_cast<float>(_num_grid_y_axis)) {
		return WALL_COLLISION;
	}

	// Check if the object's collision rectangel overlaps with any unwalkable elements on the collision grid
	// Grid based collision is not done for objects in the sky layer
	if (!sprite->sky_object) {
		// Determine if the object's collision rectangle overlaps any unwalkable tiles
		// Note that because the sprite's collision rectangle was previously determined to be within the map bounds,
		// the map grid tile indeces referenced in this loop are all valid entries and do not need to be checked for out-of-bounds conditions
		for (uint32 y = static_cast<uint32>(sprite_rect.top); y <= static_cast<uint32>(sprite_rect.bottom); ++y) {
			for (uint32 x = static_cast<uint32>(sprite_rect.left); x <= static_cast<uint32>(sprite_rect.right); ++x) {
				// Checks the collision grid at the row-column at the object's current context
				if ((_collision_grid[y][x] & sprite->context) != 0) {
					return WALL_COLLISION;
				}
			}
		}
	}

	vector<MapObject*>* objects = sprite->sky_object ?
		&_sky_objects : &_ground_objects;

	std::vector<hoa_map::private_map::MapObject*>::const_iterator it, it_end;
	for (it = objects->begin(), it_end = objects->end(); it != it_end; ++it) {
		MapObject *collision_object = *it;
		// Check if the object exists and has the no_collision property enabled
		if (!collision_object || collision_object->no_collision)
			continue;

		// Object and sprite are the same
		if (collision_object->object_id == sprite->object_id)
			continue;

		// If the two objects are not contained within one of the same contexts,
		// they can not overlap
		if (!(sprite->context & collision_object->context))
			continue;

		// If the two objects aren't colliding, try next.
		if (!CheckObjectCollision(sprite_rect, collision_object))
			continue;

		// The two objects are colliding, return the potentially asked pointer to it.
		if (collision_object_ptr != NULL)
			*collision_object_ptr = collision_object;
		return GetCollisionFromObjectType(collision_object);
	}

	return NO_COLLISION;
} // bool ObjectSupervisor::DetectCollision(VirtualSprite* sprite, uint16 x, uint16 y, float x_offset, float y_offset, MapObject** collision_object_ptr)



bool ObjectSupervisor::IsPositionOccupiedByObject(int16 x, int16 y, MapObject* object) {
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

	if (x >= tmp_x - object->GetCollHalfWidth() && x <= tmp_x + object->GetCollHalfWidth()) {
		if (y <= tmp_y + object->GetCollHeight() && y >= tmp_y) {
			return true;
		}
	}
	return false;
}



bool ObjectSupervisor::AdjustSpriteAroundCollision(private_map::VirtualSprite* sprite,
												   COLLISION_TYPE coll_type,
												   MapObject* coll_obj) {
	// 1) Check for special cases where we do not want to adjust the sprite's position
	// even though this function was called.
	// If the sprite collided with another sprite that is moving
	// and this sprite is not the map camera (not player-controlled),
	// don't attempt any adjustments. Instead we allow the other sprite to make its own adjustments.
	// TODO: maybe in this case, we should allow for position alignment but no other forms of movement adjustment
	if ((sprite != MapMode::CurrentInstance()->GetCamera())
		&& ((coll_type == CHARACTER_COLLISION) || (coll_type == ENEMY_COLLISION))) {
		VirtualSprite* coll_sprite = dynamic_cast<VirtualSprite*>(coll_obj);
		if (coll_sprite->moving == true) {
			return false;
		}
	}

	// Retrieve collision rectangle of the sprite and the collision object if available
	MapRectangle sprite_coll_rect = sprite->GetCollisionRectangle();
	MapRectangle object_coll_rect;
	if (coll_obj)
		object_coll_rect = coll_obj->GetCollisionRectangle();

	// Attempt alignment and adjustment changes to the sprite as appropriate
	if (sprite->direction & MOVING_ORTHOGONALLY) {
		if (_AlignSpriteWithCollision(sprite, sprite->direction, coll_type, sprite_coll_rect, object_coll_rect) == true) {
			return true;
		}
		else if (coll_type != WALL_COLLISION) {
			return _MoveSpriteAroundCollisionCorner(sprite, coll_type, sprite_coll_rect, object_coll_rect);
		}
	}
	else { // then (sprite->direction & MOVING_DIAGONALLY)
		return _MoveSpriteAroundCollisionDiagonal(sprite, coll_type, sprite_coll_rect, object_coll_rect);
	}
	return false;
} // bool ObjectSupervisor::AdjustSpriteAroundCollision(private_map::VirtualSprite* sprite, COLLISION_TYPE coll_type, MapObject* coll_obj);



std::vector<PathNode> ObjectSupervisor::FindPath(VirtualSprite* sprite, const PathNode& dest) {
	// NOTE: Refer to the implementation of the A* algorithm to understand
	// what all these lists and score values are for.
	std::vector<PathNode> path;

	// The starting node of this path discovery
	PathNode source_node(static_cast<int16>(sprite->x_position), static_cast<int16>(sprite->y_position));

	// Check that the source node is not the same as the destination node
	if (source_node == dest) {
		PRINT_ERROR << "source node coordinates are the same as the destination" << endl;
		// return an empty path.
		return path;
	}

	std::vector<PathNode> open_list;
	std::vector<PathNode> closed_list;

	// The current "best node"
	PathNode best_node;
	// Used to hold the eight adjacent nodes
	PathNode nodes[8];

	// Temporary delta variables used in calculation of a node's heuristic (h score)
	uint32 x_delta, y_delta;
	// The number to add to a node's g_score, depending on whether it is a lateral or diagonal movement
	int16 g_add;

	open_list.push_back(source_node);

	while (open_list.empty() == false) {
		sort(open_list.begin(), open_list.end());
		best_node = open_list.back();
		open_list.pop_back();
		closed_list.push_back(best_node);

		// Check if destination has been reached, and break out of the loop if so
		if (best_node == dest)
			break;

		// Setup the coordinates of the 8 adjacent nodes to the best node
		nodes[0].tile_x = best_node.tile_x - 1; nodes[0].tile_y = best_node.tile_y;
		nodes[1].tile_x = best_node.tile_x + 1; nodes[1].tile_y = best_node.tile_y;
		nodes[2].tile_x = best_node.tile_x;     nodes[2].tile_y = best_node.tile_y - 1;
		nodes[3].tile_x = best_node.tile_x;     nodes[3].tile_y = best_node.tile_y + 1;
		nodes[4].tile_x = best_node.tile_x - 1; nodes[4].tile_y = best_node.tile_y - 1;
		nodes[5].tile_x = best_node.tile_x - 1; nodes[5].tile_y = best_node.tile_y + 1;
		nodes[6].tile_x = best_node.tile_x + 1; nodes[6].tile_y = best_node.tile_y - 1;
		nodes[7].tile_x = best_node.tile_x + 1; nodes[7].tile_y = best_node.tile_y + 1;

		// Check the eight adjacent nodes
		for (uint8 i = 0; i < 8; ++i) {
			// ---------- (A): Check if all tiles are walkable
			// Don't use 0.0f here for both since errors at the border between
			// two positions may occure, especially when running.
			COLLISION_TYPE collision_type = DetectCollision(sprite,
															nodes[i].tile_x,
															nodes[i].tile_y,
															0.5f, 0.5f);
			// Can't go through walls.
			if (collision_type == WALL_COLLISION)
				continue;

			// ---------- (B): Check if the node is already in the closed list
			if (find(closed_list.begin(), closed_list.end(), nodes[i]) != closed_list.end())
				continue;

			// ---------- (C): If this point has been reached, the node is valid for the sprite to move to
			// If this is a lateral adjacent node, g_score is +10, otherwise diagonal adjacent node is +14
			if (i < 4)
				g_add = 10;
			else
				g_add = 14;

			// Add some g cost when there is another sprite there,
			// so the NPC try to get around when possible,
			// but will still go through it when there are no other choices.
			if (collision_type == CHARACTER_COLLISION
				|| collision_type == ENEMY_COLLISION)
				g_add += 20;

			// Set the node's parent and calculate its g_score
			nodes[i].parent_x = best_node.tile_x;
			nodes[i].parent_y = best_node.tile_y;
			nodes[i].g_score = best_node.g_score + g_add;

			// ---------- (D): Check to see if the node is already on the open list and update it if necessary
			vector<PathNode>::iterator iter = find(open_list.begin(), open_list.end(), nodes[i]);
			if (iter != open_list.end()) {
				// If its G is higher, it means that the path we are on is better, so switch the parent
				if (iter->g_score > nodes[i].g_score) {
					iter->g_score = nodes[i].g_score;
					iter->f_score = nodes[i].g_score + iter->h_score;
					iter->parent_x = nodes[i].parent_x;
					iter->parent_y = nodes[i].parent_y;
				}
			}
			// ---------- (E): Add the new node to the open list
			else {
				// Calculate the H and F score of the new node (the heuristic used is diagonal)
				x_delta = abs(dest.tile_x - nodes[i].tile_x);
				y_delta = abs(dest.tile_y - nodes[i].tile_y);
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
		return path;
	}

	// Add the destination node to the vector, retain its parent, and remove it from the closed list
	path.push_back(best_node);
	int16 parent_x = best_node.parent_x;
	int16 parent_y = best_node.parent_y;
	closed_list.pop_back();

	// Go backwards through the closed list following the parent nodes to construct the path
	for (vector<PathNode>::iterator iter = closed_list.end() - 1; iter != closed_list.begin(); --iter) {
		if (iter->tile_y == parent_y && iter->tile_x == parent_x) {
			path.push_back(*iter);
			parent_x = iter->parent_x;
			parent_y = iter->parent_y;
		}
	}
	std::reverse(path.begin(), path.end());

	return path;
} // bool ObjectSupervisor::FindPath(const VirtualSprite* sprite, const PathNode& dest)



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

	switch (coll_type) {
		case WALL_COLLISION:
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
		break;
		case CHARACTER_COLLISION:
		case ENEMY_COLLISION:
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
		break;
		default:
			IF_PRINT_WARNING(MAP_DEBUG) << "invalid collision type: " << coll_type << endl;
			return false;
		break;
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
	if (coll_type == CHARACTER_COLLISION || coll_type == ENEMY_COLLISION) {
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
		start_point = std::max(start_point, (int16)0);
		end_point = std::min(end_point, (int16)_num_grid_x_axis);
	}
	else {
		// +1 is added since the cast throws away everything after the decimal and we want a ceiling integer
		sprite_length = 1 + static_cast<uint16>(sprite_coll_rect.bottom - sprite_coll_rect.top);
		start_point = sprite->y_position - (2 * sprite_length);
		end_point = start_point + (3 * sprite_length);

		// Ensure that the line end points do not go outside of the map boundaries.
		start_point = std::max(start_point, (int16)0);
		end_point = std::min(end_point, (int16)_num_grid_y_axis);
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
			line_axis = (line_axis < _num_grid_y_axis) ? line_axis : _num_grid_y_axis - 1;
			break;
		case EAST:
			// Set to the column to the right of the right edge of the sprite's collision rectangle
			line_axis = static_cast<int16>(sprite_coll_rect.right) + 1;
			line_axis = (line_axis < _num_grid_x_axis) ? line_axis : _num_grid_x_axis - 1;
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
	else if (coll_type != CHARACTER_COLLISION && coll_type != ENEMY_COLLISION) {
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
	if (coll_type == WALL_COLLISION) {
		uint32 axis;

		check_vertical_align = false;
		axis = (north_or_south == true) ? static_cast<uint32>(mod_sprite_rect.top) : static_cast<uint32>(mod_sprite_rect.bottom);

		if (axis > _collision_grid.size() - 1)
			axis = _collision_grid.size() - 1;
		for (uint32 i = static_cast<uint32>(sprite_coll_rect.left); i <= static_cast<uint32>(sprite_coll_rect.right); ++i) {
			if ((i >= _collision_grid[axis].size()) ||
					(_collision_grid[axis][i] & sprite->context)) {
				check_vertical_align = true;
				break;
			}
		}

		check_horizontal_align = false;
		axis = (east_or_west == true) ? static_cast<uint32>(mod_sprite_rect.right) : static_cast<uint32>(mod_sprite_rect.left);
		for (uint32 i = static_cast<uint32>(sprite_coll_rect.top); i <= static_cast<uint32>(sprite_coll_rect.bottom); ++i) {
			if ((axis >= _collision_grid[i].size())
					|| (i >= _collision_grid.size())
					|| (_collision_grid[i][axis] & sprite->context)) {
				check_horizontal_align = true;
				break;
			}
		}
	}
	else if (coll_type == CHARACTER_COLLISION || coll_type == ENEMY_COLLISION) {
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



bool ObjectSupervisor::_ModifySpritePosition(VirtualSprite *sprite, uint16 direction, float distance) {

	float next_x_offset = sprite->x_offset;
	float next_y_offset = sprite->y_offset;

	if (direction & (NORTH | MOVING_NORTHWEST | MOVING_NORTHEAST))
		next_y_offset -= distance;
	else if (direction & (SOUTH | MOVING_SOUTHWEST | MOVING_SOUTHEAST))
		next_y_offset += distance;
	if (direction & (WEST | MOVING_NORTHWEST | MOVING_SOUTHWEST))
		next_x_offset -= distance;
	else if (direction & (EAST | MOVING_NORTHEAST | MOVING_SOUTHEAST))
		next_x_offset += distance;
	else {
		IF_PRINT_WARNING(MAP_DEBUG) << "invalid direction argument passed to this function: " << direction << endl;
		return false;
	}

	// Check for a collision in the next position
	if (DetectCollision(sprite, sprite->x_position,
			sprite->y_position, next_x_offset, next_y_offset, NULL) == NO_COLLISION) {
		// updates the position
		sprite->x_offset = next_x_offset;
		sprite->y_offset = next_y_offset;
		// Check the position offsets and state that the position has been changed
		sprite->CheckPositionOffsets();
		sprite->moved_position = true;
		return true;
	}
	return false;
} // bool ObjectSupervisor::_ModifySpritePosition(VirtualSprite* sprite, uint16 direction, float distance)

} // namespace private_map

} // namespace hoa_map
