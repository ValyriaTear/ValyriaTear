///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_zones.cpp
*** \author  Guillaume Anctil, drakkoon@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for map mode zones.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/map/map_zones.h"

#include "modes/map/map_sprites.h"

#include "utils/utils_random.h"

using namespace vt_utils;

namespace vt_map
{

namespace private_map
{

// -----------------------------------------------------------------------------
// ---------- MapZone Class Functions
// -----------------------------------------------------------------------------

MapZone::MapZone(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row)
{
    AddSection(left_col, right_col, top_row, bottom_row);
}

void MapZone::AddSection(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row)
{
    if(left_col >= right_col) {
        uint16 temp = left_col;
        left_col = right_col;
        right_col = temp;
    }

    if(top_row >= bottom_row) {
        uint16 temp = bottom_row;
        bottom_row = top_row;
        top_row = temp;
    }

    _sections.push_back(ZoneSection(left_col, right_col, top_row, bottom_row));
}

bool MapZone::IsInsideZone(float pos_x, float pos_y) const
{
    uint16 x = (uint16)GetFloatInteger(pos_x);
    uint16 y = (uint16)GetFloatInteger(pos_y);
    // Verify each section of the zone and check if the position is within the section bounds.
    for(std::vector<ZoneSection>::const_iterator it = _sections.begin(); it != _sections.end(); ++it) {
        if(x >= it->left_col && x <= it->right_col &&
                y >= it->top_row && y <= it->bottom_row) {
            return true;
        }
    }
    return false;
}

void MapZone::Draw()
{
    // Verify each section of the zone and check if the position is within the section bounds.
    for(std::vector<ZoneSection>::const_iterator it = _sections.begin(); it != _sections.end(); ++it) {
        if(_ShouldDraw(*it)) {
            float x, y;
            _GetDrawPosition(*it, x, y);
            const float w = it->right_col - it->left_col;
            const float h = it->bottom_row - it->top_row;
            vt_video::VideoManager->DrawRectangle(x, y, w, h, vt_video::Color(1.0f, 0.6f, 0.0f, 0.6f));
        }
    }
}

void MapZone::RandomPosition(float &x, float &y)
{
    // Select a random ZoneSection
    uint16 i = RandomBoundedInteger(0, _sections.size() - 1);

    // Select a random x and y position inside that section
    x = (float)RandomBoundedInteger(_sections[i].left_col, _sections[i].right_col);
    y = (float)RandomBoundedInteger(_sections[i].top_row, _sections[i].bottom_row);
}

bool MapZone::_ShouldDraw(const ZoneSection &section) const
{
    const MapMode *map = MapMode::CurrentInstance();

    MapRectangle rect;
    rect.top = section.top_row;
    rect.bottom = section.bottom_row;
    rect.left = section.left_col;
    rect.right = section.right_col;

    // Determine if the sprite is off-screen and if so, don't draw it.
    if(!MapRectangle::CheckIntersection(rect, map->GetMapFrame().screen_edges))
        return false;

    return true;
}

void MapZone::_GetDrawPosition(const ZoneSection &section, float &x, float &y) const
{
    const MapMode *map = MapMode::CurrentInstance();

    // Determine the center position coordinates for the camera
    float x_pos, y_pos; // Holds the final X, Y coordinates of the camera
    float x_pixel_length, y_pixel_length; // The X and Y length values that coorespond to a single pixel in the current coodinate system
    float rounded_x_offset, rounded_y_offset; // The X and Y position offsets of the object, rounded to perfectly align on a pixel boundary


    // TODO: the call to GetPixelSize() will return the same result every time so long as the coordinate system did not change. If we never
    // change the coordinate system in map mode, then this should be done only once and the calculated values should be saved for re-use.
    // However, we've discussed the possiblity of adding a zoom feature to maps, in which case we need to continually re-calculate the pixel size
    x_pos = section.left_col + (section.right_col - section.left_col) / 2;
    y_pos = section.top_row + (section.bottom_row - section.top_row);
    vt_video::VideoManager->GetPixelSize(x_pixel_length, y_pixel_length);
    rounded_x_offset = FloorToFloatMultiple(GetFloatFraction(x_pos), x_pixel_length);
    rounded_y_offset = FloorToFloatMultiple(GetFloatFraction(y_pos), y_pixel_length);
    x_pos = static_cast<float>(GetFloatInteger(x_pos)) + rounded_x_offset;
    y_pos = static_cast<float>(GetFloatInteger(y_pos)) + rounded_y_offset;

    // Move the drawing cursor to the appropriate coordinates for this sprite
    x = x_pos - map->GetMapFrame().screen_edges.left;
    y = y_pos - map->GetMapFrame().screen_edges.top;
}

// -----------------------------------------------------------------------------
// ---------- CameraZone Class Functions
// -----------------------------------------------------------------------------

CameraZone::CameraZone(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row) :
    MapZone(left_col, right_col, top_row, bottom_row),
    _camera_inside(false),
    _was_camera_inside(false)
{}

void CameraZone::Update()
{
    _was_camera_inside = _camera_inside;

    // Update only if camera is on a real sprite
    if(MapMode::CurrentInstance()->IsCameraOnVirtualFocus())
        return;

    VirtualSprite *camera = MapMode::CurrentInstance()->GetCamera();
    if(camera == NULL) {
        _camera_inside = false;
    }
    // Camera must share a context with the zone and be within its borders
    else if(IsInsideZone(camera->GetXPosition(), camera->GetYPosition())) {
        _camera_inside = true;
    } else {
        _camera_inside = false;
    }
}

// -----------------------------------------------------------------------------
// ---------- EnemyZone Class Functions
// -----------------------------------------------------------------------------

EnemyZone::EnemyZone() :
    MapZone(),
    _enabled(true),
    _roaming_restrained(true),
    _agression_roaming_restrained(false),
    _active_enemies(0),
    _spawns_left(-1), // Infinite spawns permitted.
    _spawn_timer(STANDARD_ENEMY_FIRST_SPAWN_TIME),
    _dead_timer(STANDARD_ENEMY_DEAD_TIME),
    _spawn_zone(NULL)
{
    // Done so that when the zone updates for the first time, an inactive enemy will immediately be selected and begin spawning
    _dead_timer.Finish();
}



EnemyZone::EnemyZone(uint16 left_col, uint16 right_col,
                     uint16 top_row, uint16 bottom_row):
    MapZone(left_col, right_col, top_row, bottom_row),
    _enabled(true),
    _roaming_restrained(true),
    _agression_roaming_restrained(false),
    _active_enemies(0),
    _spawns_left(-1), // Infinite spawns permitted.
    _spawn_timer(STANDARD_ENEMY_FIRST_SPAWN_TIME),
    _dead_timer(STANDARD_ENEMY_DEAD_TIME),
    _spawn_zone(NULL)
{
    // Done so that when the zone updates for the first time, an inactive enemy will immediately be selected and begin spawning
    _dead_timer.Finish();
}

EnemyZone::EnemyZone(const EnemyZone &copy) :
    MapZone(copy)
{
    _enabled = copy._enabled;
    _roaming_restrained = copy._roaming_restrained;
    _agression_roaming_restrained = copy._agression_roaming_restrained;
    _spawns_left = copy._spawns_left;
    _active_enemies = copy._active_enemies;
    _spawn_timer = copy._spawn_timer;
    _dead_timer = copy._dead_timer;
    if(copy._spawn_zone == NULL)
        _spawn_zone = NULL;
    else
        _spawn_zone = new MapZone(*(copy._spawn_zone));

    // Done so that when the zone updates for the first time, an inactive enemy will immediately be selected and begin spawning
    _dead_timer.Finish();
    _spawn_timer.Reset();
}

EnemyZone &EnemyZone::operator=(const EnemyZone &copy)
{
    if(this == &copy)  // Handle self-assignment case
        return *this;

    MapZone::operator=(copy);
    _enabled = copy._enabled;
    _roaming_restrained = copy._roaming_restrained;
    _agression_roaming_restrained = copy._agression_roaming_restrained;
    _spawns_left = copy._spawns_left;
    _active_enemies = copy._active_enemies;
    _spawn_timer = copy._spawn_timer;
    _dead_timer = copy._dead_timer;
    if(copy._spawn_zone == NULL)
        _spawn_zone = NULL;
    else
        _spawn_zone = new MapZone(*(copy._spawn_zone));

    // Done so that when the zone updates for the first time, an inactive enemy will immediately be selected and begin spawning
    _dead_timer.Finish();
    _spawn_timer.Reset();

    return *this;
}

void EnemyZone::AddEnemy(EnemySprite *enemy, MapMode *map_instance, uint8 enemy_number)
{
    if(enemy_number == 0) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function called with a zero value count argument" << std::endl;
        return;
    }

    // Prepare the first enemy
    enemy->SetZone(this);
    map_instance->AddGroundObject(enemy);
    _enemies.push_back(enemy);

    // Create any additional copies of the enemy and add them as well
    for(uint8 i = 1; i < enemy_number; ++i) {
        EnemySprite *copy = new EnemySprite(*enemy);
        copy->SetObjectID(map_instance->GetObjectSupervisor()->GenerateObjectID());
        copy->Reset();

        map_instance->AddGroundObject(copy);
        _enemies.push_back(copy);
    }
}



void EnemyZone::AddSpawnSection(uint16 left_col, uint16 right_col, uint16 top_row, uint16 bottom_row)
{
    if(left_col >= right_col) {
        uint16 temp = left_col;
        left_col = right_col;
        right_col = temp;
    }

    if(top_row >= bottom_row) {
        uint16 temp = bottom_row;
        bottom_row = top_row;
        top_row = temp;
    }

    // Make sure that this spawn section fits entirely inside one of the roaming sections
    bool okay_to_add = false;
    for(uint32 i = 0; i < _sections.size(); i++) {
        if((left_col >= _sections[i].left_col) && (right_col <= _sections[i].right_col)
                && (top_row >= _sections[i].top_row) && (bottom_row <= _sections[i].bottom_row)) {
            okay_to_add = true;
            break;
        }
    }

    if(okay_to_add == false) {
        IF_PRINT_WARNING(MAP_DEBUG) << "could not add section as it did not fit inside any single roaming zone section" << std::endl;
        return;
    }

    // Create the spawn zone if it does not exist and add the new section
    if(_spawn_zone == NULL) {
        _spawn_zone = new MapZone(left_col, right_col, top_row, bottom_row);
    } else {
        _spawn_zone->AddSection(left_col, right_col, top_row, bottom_row);
    }
}

void EnemyZone::EnemyDead()
{
    if(_active_enemies == 0) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function called when no enemies were active" << std::endl;
    } else {
        --_active_enemies;
    }
}

void EnemyZone::Update()
{
    // When spawning an enemy in a random zone location, sometimes it is occupied by another
    // object or that section is unwalkable. We try only a few different spawn locations before
    // giving up and waiting for the next call to Update(). Otherwise this function could
    // potentially take a noticable amount of time to complete
    const int8 SPAWN_RETRIES = 50;

    // Don't update when the zone is disabled.
    if (!_enabled)
        return;

    // Test whether a respawn is still permitted
    if (_spawns_left == 0)
        return;

    if (_enemies.empty())
        return;

    // Update timers
    _spawn_timer.Update();
    _dead_timer.Update();

    // If we're in the process of spawning an enemy, exit immediately as we want to wait for the timer to continue
    if (_spawn_timer.IsRunning()) {
        return;
    }

    // If no enemies are inactive (in the dead state), there's nothing left to do
    if (_active_enemies >= _enemies.size()) {
        return;
    }

    // If there are dead enemies, no enemies are respawning, and the dead timer is not active, begin the dead timer
    if (_dead_timer.IsInitial() == true) {
        _dead_timer.Run();
        return;
    }
    // If the dead timer hasn't completed, there's nothing left to do
    else if (_dead_timer.IsFinished() == false) {
        return;
    }

    // When the dead timer completes, spawn in a new enemy
    // Select a dead enemy to spawn
    uint32 index = 0;
    for(uint32 i = 0; i < _enemies.size(); ++i) {
        if (_enemies[i]->IsDead()) {
            index = i;
            break;
        }
    }

    // Used to retain random position coordinates in the zone
    float x = 0.0f;
    float y = 0.0f;
    // Number of times to try finding a valid spawning location
    int8 retries = SPAWN_RETRIES;
    // Holds the result of a collision detection check
    uint32 collision = NO_COLLISION;

    // Select a random position inside the zone to place the spawning enemy
    _enemies[index]->collision_mask = WALL_COLLISION | CHARACTER_COLLISION;
    MapZone *spawning_zone = NULL;
    if (!HasSeparateSpawnZone()) {
        spawning_zone = this;
    } else {
        spawning_zone = _spawn_zone;
    }
    // If there is a collision, retry a different location
    do {
        spawning_zone->RandomPosition(x, y);
        _enemies[index]->SetPosition(x, y);
        collision = MapMode::CurrentInstance()->GetObjectSupervisor()->DetectCollision(_enemies[index],
                    _enemies[index]->GetXPosition(),
                    _enemies[index]->GetYPosition(),
                    NULL);
    } while (collision != NO_COLLISION && --retries > 0);

    // Otherwise, spawn the enemy and reset the spawn timer
    if (collision == NO_COLLISION) {
        // Set the correct timer duration to whether do a quick first spawn,
        // or a longer standard spawn time from the second time.
        _dead_timer.Reset();
        _spawn_timer.SetDuration(_enemies[index]->GetTimeToSpawn());
        _spawn_timer.Reset();
        _spawn_timer.Run();
        _enemies[index]->ChangeStateSpawning();
        ++_active_enemies;
    } else {
        PRINT_WARNING << "Couldn't spawn a monster within " << SPAWN_RETRIES
                      << " tries. Check the enemy zones of map script:"
                      << MapMode::CurrentInstance()->GetMapScriptFilename() << std::endl;
    }
} // void EnemyZone::Update()

void EnemyZone::Draw()
{
    // Don't draw when the zone is disabled.
    if (!_enabled)
        return;

    // Verify each section of the zone and check if the position is within the section bounds.
    for(std::vector<ZoneSection>::const_iterator it = _sections.begin(); it != _sections.end(); ++it) {
        if(_ShouldDraw(*it)) {
            float x, y;
            _GetDrawPosition(*it, x, y);
            const float w = it->right_col - it->left_col;
            const float h = it->bottom_row - it->top_row;
            vt_video::VideoManager->DrawRectangle(x, y, w, h, vt_video::Color(0.0f, 0.0f, 0.0f, 0.5f));
        }
    }
}

} // namespace private_map

} // namespace vt_map
