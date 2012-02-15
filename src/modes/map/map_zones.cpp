///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_zones.cpp
*** \author  Guillaume Anctil, drakkoon@allacrost.org
*** \brief   Source file for map mode zones.
*** ***************************************************************************/

// Allacrost engines
#include "system.h"

// Local map mode headers
#include "map.h"
#include "map_objects.h"
#include "map_sprites.h"
#include "map_zones.h"

using namespace std;
using namespace hoa_utils;

namespace hoa_map {

namespace private_map {

// -----------------------------------------------------------------------------
// ---------- MapZone Class Functions
// -----------------------------------------------------------------------------

MapZone::MapZone(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row) :
	_active_contexts(MAP_CONTEXT_NONE)
{
	AddSection(left_col, right_col, top_row, bottom_row);
}



MapZone::MapZone(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row, MAP_CONTEXT contexts) :
	_active_contexts(contexts)
{
	AddSection(left_col, right_col, top_row, bottom_row);
}



void MapZone::AddSection(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row) {
	if (left_col >= right_col) {
		IF_PRINT_WARNING(MAP_DEBUG) << "left and right coordinates are mismatched: section will not be added" << endl;
		return;
	}

	if (top_row >= bottom_row) {
		IF_PRINT_WARNING(MAP_DEBUG) << "top and bottom coordinates are mismatched: section will not be added" << endl;
		return;
	}

	_sections.push_back(ZoneSection(left_col, right_col, top_row, bottom_row));
}



bool MapZone::IsInsideZone(uint16 pos_x, uint16 pos_y) const {
	// Verify each section of the zone and check if the position is within the section bounds.
	for (vector<ZoneSection>::const_iterator i = _sections.begin(); i != _sections.end(); ++i) {
		if (pos_x >= i->left_col && pos_x <= i->right_col &&
			pos_y >= i->top_row && pos_y <= i->bottom_row)
		{
			return true;
		}
	}
	return false;
}



void MapZone::_RandomPosition(uint16& x, uint16& y) {
	// Select a random ZoneSection
	uint16 i = RandomBoundedInteger(0, _sections.size() - 1);

	// Select a random x and y position inside that section
	x = RandomBoundedInteger(_sections[i].left_col, _sections[i].right_col);
	y = RandomBoundedInteger(_sections[i].top_row, _sections[i].bottom_row);
}

// -----------------------------------------------------------------------------
// ---------- CameraZone Class Functions
// -----------------------------------------------------------------------------

CameraZone::CameraZone(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row) :
	MapZone(left_col, right_col, top_row, bottom_row),
	_camera_inside(false),
	_was_camera_inside(false)
{}



CameraZone::CameraZone(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row, MAP_CONTEXT contexts) :
	MapZone(left_col, right_col, top_row, bottom_row, contexts),
	_camera_inside(false),
	_was_camera_inside(false)
{}



void CameraZone::Update() {
	_was_camera_inside = _camera_inside;

	// Update only if camera is on a real sprite
	if (MapMode::CurrentInstance()->IsCameraOnVirtualFocus()) {
		return;
	}

	VirtualSprite* camera = MapMode::CurrentInstance()->GetCamera();
	if (camera == NULL) {
		_camera_inside = false;
	}
	// Camera must share a context with the zone and be within its borders
	else if ((_active_contexts & camera->GetContext()) && (IsInsideZone(camera->x_position, camera->y_position) == true)) {
		_camera_inside = true;
	}
	else {
		_camera_inside = false;
	}
}

// -----------------------------------------------------------------------------
// ---------- ResidentZone Class Functions
// -----------------------------------------------------------------------------

ResidentZone::ResidentZone(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row) :
	MapZone(left_col, right_col, top_row, bottom_row)
{}



ResidentZone::ResidentZone(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row, MAP_CONTEXT contexts) :
	MapZone(left_col, right_col, top_row, bottom_row, contexts)
{}



void ResidentZone::Update() {
	_entering_residents.clear();
	_exiting_residents.clear();

	// Holds a list of sprites that should be removed from the resident zone. This is necessary because we can't iterate through
	// the resident list and erase former residents without messing up the set iteration.
	vector<VirtualSprite*> remove_list;

	// Examine all residents to see if they still reside in the zone. If not, move them to the exiting residents list
	for (set<VirtualSprite*>::iterator i = _residents.begin(); i != _residents.end(); i++) {
		// Make sure that the resident is still in a context shared by the zone and located within the zone boundaries
		if ((((*i)->GetContext() & _active_contexts) == 0x0) || (IsInsideZone((*i)->x_position, (*i)->y_position) == false)) {
			remove_list.push_back(*i);
		}
	}

	for (uint32 i = 0; i < remove_list.size(); i++) {
		_exiting_residents.insert(remove_list[i]);
		_residents.erase(remove_list[i]);
	}
}



void ResidentZone::AddPotentialResident(VirtualSprite* sprite) {
	if (sprite == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "function received NULL argument" << endl;
		return;
	}

	// Check that sprite is not already a resident
	if (IsSpriteResident(sprite) == true) {
		return;
	}

	// Check that the sprite's context is compatible with this zone and is located within the zone boundaries
	if (sprite->GetContext() & _active_contexts) {
		if (IsInsideZone(sprite->x_position, sprite->y_position) == true) {
			_entering_residents.insert(sprite);
			_residents.insert(sprite);
		}
	}
}



VirtualSprite* ResidentZone::_GetSpriteInSet(const set<VirtualSprite*>& local_set, uint32 index) const {
	if (index >= local_set.size()) {
		return NULL;
	}

	uint32 counter = 0;
	for (set<VirtualSprite*>::const_iterator i = local_set.begin(); i != local_set.end(); i++) {
		if (index == counter) {
			return *i;
		}
		counter++;
	}

	IF_PRINT_WARNING(MAP_DEBUG) << "sprite not found after reaching end of set -- this should never happen" << endl;
	return NULL;
}

// -----------------------------------------------------------------------------
// ---------- EnemyZone Class Functions
// -----------------------------------------------------------------------------

EnemyZone::EnemyZone() :
	MapZone(),
	_roaming_restrained(true),
	_active_enemies(0),
	_spawn_timer(3000), // TEMP: use default spawn time constant,
	_spawn_zone(NULL)
{
	_spawn_timer.Run();
}



EnemyZone::EnemyZone(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row) :
	MapZone(left_col, right_col, top_row, bottom_row),
	_roaming_restrained(true),
	_active_enemies(0),
	_spawn_timer(3000), // TEMP: use default spawn time constant,
	_spawn_zone(NULL)
{
	_spawn_timer.Run();
}



EnemyZone::EnemyZone(const EnemyZone& copy) :
	MapZone(copy)
{
	_roaming_restrained = copy._roaming_restrained;
	_active_enemies = copy._active_enemies;
	_spawn_timer = copy._spawn_timer;
	if (copy._spawn_zone == NULL)
		_spawn_zone = NULL;
	else
		_spawn_zone = new MapZone(*(copy._spawn_zone));
}



EnemyZone& EnemyZone::operator=(const EnemyZone& copy) {
	if (this == &copy) // Handle self-assignment case
		return *this;

	MapZone::operator=(copy);
	_roaming_restrained = copy._roaming_restrained;
	_active_enemies = copy._active_enemies;
	_spawn_timer = copy._spawn_timer;
	if (copy._spawn_zone == NULL)
		_spawn_zone = NULL;
	else
		_spawn_zone = new MapZone(*(copy._spawn_zone));

	return *this;
}



void EnemyZone::AddEnemy(EnemySprite* enemy, MapMode* map, uint8 count) {
	if (count == 0) {
		IF_PRINT_WARNING(MAP_DEBUG) << "function called with a zero value count argument" << endl;
		return;
	}

	// Prepare the first enemy
	enemy->SetZone(this);
	map->AddGroundObject(enemy);
	_enemies.push_back(enemy);

	// Create any additional copies of the enemy and add them as well
	for (uint8 i = 1; i < count; i++) {
		EnemySprite* copy = new EnemySprite(*enemy);
		copy->SetObjectID(map->GetObjectSupervisor()->GenerateObjectID());
		// Add a 10% random margin of error to make enemies look less synchronized
		copy->SetTimeToChange(static_cast<uint32>(copy->GetTimeToChange() * (1 + RandomFloat() * 10)));
		copy->Reset();

		map->AddGroundObject(copy);
		_enemies.push_back(copy);
	}
}



void EnemyZone::AddSpawnSection(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row) {
	if (left_col >= right_col) {
		IF_PRINT_WARNING(MAP_DEBUG) << "left and right coordinates are mismatched: section will not be added" << endl;
		return;
	}

	if (top_row >= bottom_row) {
		IF_PRINT_WARNING(MAP_DEBUG) << "top and bottom coordinates are mismatched: section will not be added" << endl;
		return;
	}

	// Make sure that this spawn section fits entirely inside one of the roaming sections
	bool okay_to_add = false;
	for (uint32 i = 0; i < _sections.size(); i++) {
		if ((left_col >= _sections[i].left_col) && (right_col <= _sections[i].right_col)
			&& (top_row >= _sections[i].top_row) && (bottom_row <= _sections[i].bottom_row))
		{
			okay_to_add = true;
			break;
		}
	}

	if (okay_to_add == false) {
		IF_PRINT_WARNING(MAP_DEBUG) << "could not add section as it did not fit inside any single roaming zone section" << endl;
		return;
	}

	// Create the spawn zone if it does not exist and add the new section
	if (_spawn_zone == NULL) {
		_spawn_zone = new MapZone(left_col, right_col, top_row, bottom_row);
	}
	else {
		_spawn_zone->AddSection(left_col, right_col, top_row, bottom_row);
	}
}



void EnemyZone::EnemyDead() {
	if (_active_enemies == 0) {
		IF_PRINT_WARNING(MAP_DEBUG) << "function called when no enemies were active" << endl;
	}
	else {
		--_active_enemies;
	}
}



void EnemyZone::Update() {
	// When spawning an enemy in a random zone location, sometimes it is occupied by another
	// object or that section is unwalkable. We try only a few different spawn locations before
	// giving up and waiting for the next call to Update(). Otherwise this function could
	// potentially take a noticable amount of time to complete
	const int8 SPAWN_RETRIES = 5;

	if (_enemies.empty() == true)
		return;

	// Spawn new enemies only if there is at least one enemy that is not active
	if (_active_enemies >= _enemies.size())
		return;

	// Update the regeneration timer and return if the spawn time has not yet been reached
	_spawn_timer.Update();
	if (_spawn_timer.IsFinished() == false) {
		return;
	}

	// Otherwise, select a DEAD enemy to spawn
	uint32 index = 0;
	for (uint32 i = 0; i < _enemies.size(); i++) {
		if (_enemies[i]->IsDead() == true) {
			index = i;
			break;
		}
	}

	uint16 x, y; // Used to retain random position coordinates in the zone
	int8 retries = SPAWN_RETRIES; // Number of times to try finding a valid spawning location
	bool collision; // Holds the result of a collision detection check

	// Select a random position inside the zone to place the spawning enemy
	_enemies[index]->no_collision = false;
	MapZone* spawning_zone = NULL;
	if (HasSeparateSpawnZone() == false) {
		spawning_zone = this;
	}
	else {
		spawning_zone = _spawn_zone;
	}
	// If there is a collision, retry a different location
	do {
		spawning_zone->_RandomPosition(x, y);
		_enemies[index]->SetXPosition(x, 0.0f);
		_enemies[index]->SetYPosition(y, 0.0f);
		collision = MapMode::CurrentInstance()->GetObjectSupervisor()->DetectCollision(_enemies[index], NULL);
	} while (collision && --retries > 0);

	// If we didn't find a suitable spawning location, reset the collision info
	// on the enemy sprite and we will retry on the next call to this function
	if (collision) {
		_enemies[index]->no_collision = true;
	}

	// Otherwise, spawn the enemy and reset the spawn timer
	else {
		_spawn_timer.Reset();
		_spawn_timer.Run();
		_enemies[index]->ChangeStateSpawning();
		_active_enemies++;
	}
} // void EnemyZone::Update()

// -----------------------------------------------------------------------------
// ---------- ContextZone Class Functions
// -----------------------------------------------------------------------------

ContextZone::ContextZone(MAP_CONTEXT one, MAP_CONTEXT two) :
	_context_one(one),
	_context_two(two)
{
	if (_context_one == _context_two) {
		PRINT_ERROR << "tried to create a ContextZone with two equal context values: " << _context_one << endl;
		exit(1);
	}
}



void ContextZone::AddSection(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row) {
	IF_PRINT_WARNING(MAP_DEBUG) << "this method is invalid for this class and should not be called: section will not be added" << endl;
}



void ContextZone::AddSection(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row, bool context) {
	if (left_col >= right_col) {
		IF_PRINT_WARNING(MAP_DEBUG) << "left and right coordinates are mismatched: section will not be added" << endl;
		return;
	}

	if (top_row >= bottom_row) {
		IF_PRINT_WARNING(MAP_DEBUG) << "top and bottom coordinates are mismatched: section will not be added" << endl;
		return;
	}

	_sections.push_back(ZoneSection(left_col, right_col, top_row, bottom_row));
	_section_contexts.push_back(context);
}



void ContextZone::Update() {
	int16 index;

	// Check every ground object and determine if its context should be changed by this zone
	for (std::vector<MapObject*>::iterator i = MapMode::CurrentInstance()->GetObjectSupervisor()->_ground_objects.begin();
		i != MapMode::CurrentInstance()->GetObjectSupervisor()->_ground_objects.end(); i++)
	{
		// If the object does not have a context equal to one of the two switching contexts, do not examine it further
		if ((*i)->GetContext() != _context_one && (*i)->GetContext() != _context_two) {
			continue;
		}

		// If the object is inside the zone, set their context to that zone's context
		// (This may result in no change from the object's current context depending on the zone section)
		index = _IsInsideZone(*i);
		if (index >= 0) {
			(*i)->SetContext(_section_contexts[index] ? _context_one : _context_two);
		}
	}
}



int16 ContextZone::_IsInsideZone(MapObject* object) {
	// NOTE: argument is not NULL-checked here for performance reasons

	// Check each section of the zone to see if the object is located within
	for (uint16 i = 0; i < _sections.size(); i++) {
		if (object->x_position >= _sections[i].left_col && object->x_position <= _sections[i].right_col &&
			object->y_position >= _sections[i].top_row && object->y_position <= _sections[i].bottom_row)
		{
			return i;
		}
	}

	return -1;
}

} // namespace private_map

} // namespace hoa_map
