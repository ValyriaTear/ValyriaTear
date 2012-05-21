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

#include "utils.h"

#include "engine/audio/audio.h"
#include "engine/system.h"
#include "engine/video/video.h"

#include "common/global/global.h"

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
	rounded_x_offset = FloorToFloatMultiple(GetFloatFraction(GetXPosition()), x_pixel_length);
	rounded_y_offset = FloorToFloatMultiple(GetFloatFraction(GetYPosition()), y_pixel_length);
	x_pos = static_cast<float>(GetFloatInteger(GetXPosition())) + rounded_x_offset;
	y_pos = static_cast<float>(GetFloatInteger(GetYPosition())) + rounded_y_offset;

	// ---------- Move the drawing cursor to the appropriate coordinates for this sprite
	VideoManager->Move(x_pos - map->GetMapFrame().screen_edges.left,
					   y_pos - map->GetMapFrame().screen_edges.top);
	return true;
} // bool MapObject::ShouldDraw()


MapRectangle MapObject::GetCollisionRectangle() const {
	MapRectangle rect;
	rect.left = position.x - coll_half_width;
	rect.right = position.x + coll_half_width;
	rect.top = position.y - coll_height;
	rect.bottom = position.y;

	return rect;
}


MapRectangle MapObject::GetCollisionRectangle(float x, float y) const {
	MapRectangle rect;
	rect.left = x - coll_half_width;
	rect.right = x + coll_half_width;
	rect.top = y - coll_height;
	rect.bottom = y;
	return rect;
}



MapRectangle MapObject::GetImageRectangle() const {
	MapRectangle rect;
	rect.left = position.x - img_half_width;
	rect.right = position.x + img_half_width;
	rect.top = position.y - img_height;
	rect.bottom = position.y;
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
	if (!animations.empty() && updatable)
		animations[current_animation].Update();
}


void PhysicalObject::Draw() {
	if (!animations.empty() && MapObject::ShouldDraw()) {
		animations[current_animation].Draw();

		// Draw collision rectangle if the debug view is on.
		if (VideoManager->DebugInfoOn()) {
			float x, y = 0.0f;
			VideoManager->GetDrawPosition(x, y);
			MapRectangle rect = GetCollisionRectangle(x, y);
			VideoManager->DrawRectangle(rect.right - rect.left, rect.bottom - rect.top, Color(0.0f, 1.0f, 0.0f, 0.6f));
		}
	}
}


int32 PhysicalObject::AddAnimation(std::string animation_filename) {
	AnimatedImage new_animation;
	if (!new_animation.LoadFromAnimationScript(animation_filename)) {
		PRINT_WARNING << "Could not add animation because the animation filename was invalid: "
		    << animation_filename << endl;
		return -1;
	}
	new_animation.SetDimensions(img_half_width * 2, img_height);

	animations.push_back(new_animation);
	return (int32)animations.size() - 1;
}


int32 PhysicalObject::AddStillFrame(std::string image_filename) {
	AnimatedImage new_animation;
	if (!new_animation.AddFrame(image_filename, 100000)) {
		PRINT_WARNING << "Could not add a still frame because the image filename was invalid: "
		    << image_filename << endl;
		return -1;
	}
	new_animation.SetDimensions(img_half_width * 2, img_height);

	animations.push_back(new_animation);
	return (int32)animations.size() - 1;
}


void PhysicalObject::SetCurrentAnimation(uint32 animation_id) {
    if (animation_id < animations.size()) {
        animations[current_animation].SetTimeProgress(0);
        current_animation = animation_id;
    }
}


// Save points
SavePoint::SavePoint(float x, float y, MAP_CONTEXT map_context):
	MapObject(),
	_animations(0),
	_save_active(false)
{
	position.x = x;
	position.y = y;

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
Halo::Halo(const std::string& filename, float x, float y, const Color& color, MAP_CONTEXT map_context):
	MapObject()
{
	_color = color;
	position.x = x;
	position.y = y;

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
	_virtual_focus->SetPosition(0.0f, 0.0f);
	_virtual_focus->movement_speed = NORMAL_SPEED;
	_virtual_focus->SetNoCollision(true);
	_virtual_focus->SetVisible(false);
}



ObjectSupervisor::~ObjectSupervisor() {
	// Delete all of the map objects
	for (uint32 i = 0; i < _ground_objects.size(); ++i) {
		delete(_ground_objects[i]);
	}
	for (uint32 i = 0; i < _save_points.size(); ++i) {
		delete(_save_points[i]);
	}
	for (uint32 i = 0; i < _pass_objects.size();++i) {
		delete(_pass_objects[i]);
	}
	for (uint32 i = 0; i < _sky_objects.size(); ++i) {
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



bool ObjectSupervisor::Load(ReadScriptDescriptor& map_file) {
	if (!map_file.DoesTableExist("map_grid")) {
		PRINT_ERROR << "No map grid found in map file: " << map_file.GetFilename() << endl;
		return false;
	}

	// Construct the collision grid
	map_file.OpenTable("map_grid");
	_num_grid_y_axis = map_file.GetTableSize();
	for (uint16 y = 0; y < _num_grid_y_axis; ++y) {
		_collision_grid.push_back(vector<uint32>());
		map_file.ReadUIntVector(y, _collision_grid.back());
	}
	map_file.CloseTable();
	_num_grid_x_axis = _collision_grid[0].size();
	return true;
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
	float source_x = sprite->GetXPosition();
	float source_y = sprite->GetYPosition();
	// Holds the minimum distance found between the sprite and a valid object
	float min_distance = fabs(source_x - closest_obj->GetXPosition()) +
		fabs(source_y - closest_obj->GetYPosition());

	for (uint32 i = 1; i < valid_objects.size(); i++) {
		float dist = fabs(source_x - valid_objects[i]->GetXPosition()) +
			fabs(source_y - valid_objects[i]->GetYPosition());
		if (dist < min_distance) {
			closest_obj = valid_objects[i];
			min_distance = dist;
		}
	}
	return closest_obj;
} // MapObject* ObjectSupervisor::FindNearestObject(VirtualSprite* sprite, float search_distance)


bool ObjectSupervisor::CheckObjectCollision(const MapRectangle& rect, const private_map::MapObject* const obj) {
	if (!obj)
		return false;

	MapRectangle obj_rect = obj->GetCollisionRectangle();
	return MapRectangle::CheckIntersection(rect, obj_rect);
}


bool ObjectSupervisor::IsPositionOccupiedByObject(float x, float y, MapObject* object) {
	if (object == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "NULL pointer passed into function argument" << endl;
		return false;
	}

	MapRectangle rect = object->GetCollisionRectangle();

	if (x >= rect.left && x <= rect.right) {
		if (y <= rect.bottom && y >= rect.top) {
			return true;
		}
	}
	return false;
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
												 float x_pos, float y_pos,
												 MapObject** collision_object_ptr) {
	// If the sprite has this property set it can not collide
	if (!sprite || sprite->no_collision)
		return NO_COLLISION;

	// Get the collision rectangle at the given position
	MapRectangle sprite_rect = sprite->GetCollisionRectangle(x_pos, y_pos);

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
} // bool ObjectSupervisor::DetectCollision(VirtualSprite* sprite, float x, float y, MapObject** collision_object_ptr)


Path ObjectSupervisor::FindPath(VirtualSprite* sprite, const MapPosition& destination) {
	// NOTE: Refer to the implementation of the A* algorithm to understand
	// what all these lists and score values are for.

	// NOTE(bis): On the outer scope, we'll use float based positions,
	// but we still use integer positions for path finding.
	Path path;

	if (!MapMode::CurrentInstance()->GetObjectSupervisor()->IsWithinMapBounds(sprite)) {
		IF_PRINT_WARNING(MAP_DEBUG) << "Sprite position is invalid" << endl;
		return path;
	}

	// Return when the destination is unreachable
	if (DetectCollision(sprite, destination.x, destination.y) == WALL_COLLISION)
		return path;

	if (!MapMode::CurrentInstance()->GetObjectSupervisor()->IsWithinMapBounds(destination.x, destination.y)) {
		IF_PRINT_WARNING(MAP_DEBUG) << "Invalid destination coordinates" << endl;
		return path;
	}

	// The starting node of this path discovery
	PathNode source_node(static_cast<int16>(sprite->GetXPosition()), static_cast<int16>(sprite->GetYPosition()));
	// The ending node.
	PathNode dest(static_cast<int16>(destination.x), static_cast<int16>(destination.y));

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

	// We will try to keep the original offset all along.
	float offset_x = GetFloatFraction(destination.x);
	float offset_y = GetFloatFraction(destination.y);

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
															((float)nodes[i].tile_x) + offset_x,
															((float)nodes[i].tile_y) + offset_y);

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

	// Add the destination node to the vector.
	path.push_back(destination);

	// Retain the last node parent, and remove it from the closed list
	int16 parent_x = best_node.parent_x;
	int16 parent_y = best_node.parent_y;
	closed_list.pop_back();

	// Go backwards through the closed list following the parent nodes to construct the path
	for (vector<PathNode>::iterator iter = closed_list.end() - 1; iter != closed_list.begin(); --iter) {
		if (iter->tile_y == parent_y && iter->tile_x == parent_x) {
			MapPosition next_pos(((float)iter->tile_x) + offset_x, ((float)iter->tile_y) + offset_y);
			path.push_back(next_pos);

			parent_x = iter->parent_x;
			parent_y = iter->parent_y;
		}
	}
	std::reverse(path.begin(), path.end());

	return path;
} // Path ObjectSupervisor::FindPath(const VirtualSprite* sprite, const MapPosition& destination)


bool ObjectSupervisor::IsWithinMapBounds(float x, float y) const {
    return (x >= 0.0f && x < static_cast<float>(_num_grid_x_axis)
            && y >= 0.0f && y < static_cast<float>(_num_grid_y_axis));
}


bool ObjectSupervisor::IsWithinMapBounds(VirtualSprite *sprite) const {
    return sprite ? IsWithinMapBounds(sprite->GetXPosition(), sprite->GetYPosition())
        : false;
}


void ObjectSupervisor::DrawCollisionArea(const MapFrame* frame) {
	MAP_CONTEXT context_id = MapMode::CurrentInstance()->GetCurrentContext();

	VideoManager->Move(frame->tile_x_offset - 0.5f, frame->tile_y_offset - 1.0f);

	for (uint32 y = static_cast<uint32>(frame->tile_y_start * 2);
		y < static_cast<uint32>((frame->tile_y_start + frame->num_draw_y_axis) * 2); ++y) {
		for (uint32 x = static_cast<uint32>(frame->tile_x_start * 2);
				x < static_cast<uint32>((frame->tile_x_start + frame->num_draw_x_axis) * 2); ++x) {

			// Draw the collision rectangle
			if (_collision_grid[y][x] & context_id)
				VideoManager->DrawRectangle(1.0f, 1.0f, Color(1.0f, 0.0f, 0.0f, 0.6f));

			VideoManager->MoveRelative(1.0f, 0.0f);
		} // x
		VideoManager->MoveRelative(-static_cast<float>(frame->num_draw_x_axis * 2), 1.0f);
	} // y
}

} // namespace private_map

} // namespace hoa_map
