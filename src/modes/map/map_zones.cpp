///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
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

MapZone::MapZone(uint16_t left_col, uint16_t right_col, uint16_t top_row, uint16_t bottom_row) :
    _interaction_icon(nullptr)
{
    AddSection(left_col, right_col, top_row, bottom_row);
    // Register to the object supervisor
    MapMode::CurrentInstance()->GetObjectSupervisor()->AddZone(this);
}

MapZone::~MapZone()
{
    if (_interaction_icon)
        delete _interaction_icon;
}

MapZone* MapZone::Create(uint16_t left_col, uint16_t right_col, uint16_t top_row, uint16_t bottom_row)
{
    // The zone auto registers to the object supervisor
    // and will later handle deletion.
    return new MapZone(left_col, right_col, top_row, bottom_row);
}

void MapZone::AddSection(uint16_t left_col, uint16_t right_col, uint16_t top_row, uint16_t bottom_row)
{
    if(left_col >= right_col) {
        uint16_t temp = left_col;
        left_col = right_col;
        right_col = temp;
    }

    if(top_row >= bottom_row) {
        uint16_t temp = bottom_row;
        bottom_row = top_row;
        top_row = temp;
    }

    _sections.push_back(ZoneSection(left_col, right_col, top_row, bottom_row));
}

bool MapZone::IsInsideZone(float pos_x, float pos_y) const
{
    uint16_t x = (uint16_t)GetFloatInteger(pos_x);
    uint16_t y = (uint16_t)GetFloatInteger(pos_y);
    // Verify each section of the zone and check if the position is within the section bounds.
    for(std::vector<ZoneSection>::const_iterator it = _sections.begin(); it != _sections.end(); ++it) {
        if(x >= it->left_col && x <= it->right_col &&
                y >= it->top_row && y <= it->bottom_row) {
            return true;
        }
    }
    return false;
}

void MapZone::Update()
{
    if (_interaction_icon)
        _interaction_icon->Update();
}

void MapZone::Draw()
{
    // Verify each section of the zone and check if the position is within the section bounds.
    for(std::vector<ZoneSection>::const_iterator it = _sections.begin(); it != _sections.end(); ++it) {
        if(_ShouldDraw(*it)) {
            vt_video::VideoManager->DrawRectangle((it->right_col - it->left_col) * GRID_LENGTH,
                                                  (it->bottom_row - it->top_row) * GRID_LENGTH,
                                                   vt_video::Color(1.0f, 0.6f, 0.0f, 0.6f));
        }
    }
}

void MapZone::RandomPosition(float &x, float &y)
{
    // Select a random ZoneSection
    uint16_t i = RandomBoundedInteger(0, _sections.size() - 1);

    // Select a random x and y position inside that section
    x = (float)RandomBoundedInteger(_sections[i].left_col, _sections[i].right_col);
    y = (float)RandomBoundedInteger(_sections[i].top_row, _sections[i].bottom_row);
}

void MapZone::SetInteractionIcon(const std::string& animation_filename)
{
    if (_interaction_icon)
        delete _interaction_icon;
    _interaction_icon = new vt_video::AnimatedImage();
    if (!_interaction_icon->LoadFromAnimationScript(animation_filename)) {
        PRINT_WARNING << "Interaction icon animation filename couldn't be loaded: " << animation_filename << std::endl;
    }
}

void MapZone::DrawInteractionIcon()
{
    if (!_interaction_icon)
        return;

    for(std::vector<ZoneSection>::const_iterator it = _sections.begin(); it != _sections.end(); ++it) {
        const ZoneSection& section = *it;
        if(!_ShouldDraw(section))
            continue;

        // Determine the center position coordinates for the camera
        MapRectangle rect;
        rect.top = section.top_row;
        rect.bottom = section.bottom_row;
        rect.left = section.left_col;
        rect.right = section.right_col;
        float x_pos = rect.left + (rect.right - rect.left) / 2;
        float y_pos = rect.top + (rect.bottom - rect.top);

        MapMode* map_mode = MapMode::CurrentInstance();
        vt_video::Color icon_color(1.0f, 1.0f, 1.0f, 0.0f);
        float icon_alpha = 1.0f - (fabs(x_pos - map_mode->GetCamera()->GetXPosition())
                                + fabs(y_pos - map_mode->GetCamera()->GetYPosition())) / INTERACTION_ICON_VISIBLE_RANGE;
        if (icon_alpha < 0.0f)
            icon_alpha = 0.0f;
        icon_color.SetAlpha(icon_alpha);

        vt_video::VideoManager->MoveRelative(0.0f, -1.0f * GRID_LENGTH);
        _interaction_icon->Draw(icon_color);
    }
}

bool MapZone::_ShouldDraw(const ZoneSection &section)
{
    MapMode* map_mode = MapMode::CurrentInstance();

    MapRectangle rect;
    rect.top = section.top_row;
    rect.bottom = section.bottom_row;
    rect.left = section.left_col;
    rect.right = section.right_col;

    // Determine if the sprite is off-screen and if so, don't draw it.
    if (!MapRectangle::CheckIntersection(rect, map_mode->GetMapFrame().screen_edges))
        return false;

    // Determine the center position coordinates for the camera
    float x_pos = rect.left + (rect.right - rect.left) / 2;
    float y_pos = rect.top + (rect.bottom - rect.top);

    // Move the drawing cursor to the appropriate coordinates for this sprite
    vt_video::VideoManager->Move(map_mode->GetScreenXCoordinate(x_pos),
                                 map_mode->GetScreenYCoordinate(y_pos));
    return true;
}

MapZone::MapZone(const MapZone&)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
}

MapZone& MapZone::operator=(const MapZone&)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
    return *this;
}

// -----------------------------------------------------------------------------
// ---------- CameraZone Class Functions
// -----------------------------------------------------------------------------

CameraZone::CameraZone(uint16_t left_col, uint16_t right_col, uint16_t top_row, uint16_t bottom_row) :
    MapZone(left_col, right_col, top_row, bottom_row),
    _camera_inside(false),
    _was_camera_inside(false)
{
}

CameraZone* CameraZone::Create(uint16_t left_col, uint16_t right_col, uint16_t top_row, uint16_t bottom_row)
{
    // The zone auto registers to the object supervisor
    // and will later handle deletion.
    return new CameraZone(left_col, right_col, top_row, bottom_row);
}

void CameraZone::Update()
{
    MapZone::Update();

    _was_camera_inside = _camera_inside;

    // Update only if camera is on a real sprite
    if(MapMode::CurrentInstance()->IsCameraOnVirtualFocus())
        return;

    VirtualSprite *camera = MapMode::CurrentInstance()->GetCamera();
    if(camera == nullptr) {
        _camera_inside = false;
    }
    // Camera must share a context with the zone and be within its borders
    else if(IsInsideZone(camera->GetXPosition(), camera->GetYPosition())) {
        _camera_inside = true;
    } else {
        _camera_inside = false;
    }
}

CameraZone::CameraZone(const CameraZone&) :
    MapZone(0, 0, 0, 0)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
}

CameraZone& CameraZone::operator=(const CameraZone&)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
    return *this;
}

// -----------------------------------------------------------------------------
// ---------- EnemyZone Class Functions
// -----------------------------------------------------------------------------

EnemyZone::EnemyZone(uint16_t left_col, uint16_t right_col,
                     uint16_t top_row, uint16_t bottom_row):
    MapZone(left_col, right_col, top_row, bottom_row),
    _enabled(true),
    _roaming_restrained(true),
    _active_enemies(0),
    _spawns_left(-1), // Infinite spawns permitted.
    _spawn_timer(STANDARD_ENEMY_FIRST_SPAWN_TIME),
    _dead_timer(STANDARD_ENEMY_DEAD_TIME),
    _spawn_zone(nullptr)
{
    // Done so that when the zone updates for the first time, an inactive enemy will immediately be selected and begin spawning
    _dead_timer.Finish();
}

EnemyZone::~EnemyZone()
{
    if (_spawn_zone != nullptr) {
        delete _spawn_zone;
        _spawn_zone = nullptr;
    }

    for (auto& enemy_owned : _enemies_owned) {
        if (enemy_owned != nullptr) {
            delete enemy_owned;
            enemy_owned = nullptr;
        }
    }
    _enemies_owned.clear();
}

EnemyZone* EnemyZone::Create(uint16_t left_col, uint16_t right_col, uint16_t top_row, uint16_t bottom_row)
{
    // The zone auto registers to the object supervisor
    // and will later handle deletion.
    return new EnemyZone(left_col, right_col, top_row, bottom_row);
}

void EnemyZone::AddEnemy(EnemySprite* enemy, uint8_t enemy_number)
{
    if(enemy_number == 0) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function called with a zero value count argument" << std::endl;
        return;
    }

    // Prepare the first enemy
    enemy->SetZone(this);
    _enemies.push_back(enemy);

    // Create any additional copies of the enemy and add them as well
    for (uint8_t i = 1; i < enemy_number; ++i) {
        EnemySprite* copy = new EnemySprite(*enemy);
        copy->Reset();
        _enemies.push_back(copy);
        _enemies_owned.push_back(copy);
    }
}

void EnemyZone::AddSpawnSection(uint16_t left_col, uint16_t right_col, uint16_t top_row, uint16_t bottom_row)
{
    if(left_col >= right_col) {
        uint16_t temp = left_col;
        left_col = right_col;
        right_col = temp;
    }

    if(top_row >= bottom_row) {
        uint16_t temp = bottom_row;
        bottom_row = top_row;
        top_row = temp;
    }

    // Make sure that this spawn section fits entirely inside one of the roaming sections
    bool okay_to_add = false;
    for(uint32_t i = 0; i < _sections.size(); i++) {
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
    if(_spawn_zone == nullptr) {
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
    const int8_t SPAWN_RETRIES = 50;

    // Don't update when the zone is disabled.
    if (!_enabled)
        return;

    // Test whether a respawn is still permitted
    if (_spawns_left == 0)
        return;

    if (_enemies.empty())
        return;

    MapZone::Update();

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
    if (_dead_timer.IsInitial()) {
        _dead_timer.Run();
        return;
    }
    // If the dead timer hasn't completed, there's nothing left to do
    else if (_dead_timer.IsFinished() == false) {
        return;
    }

    // When the dead timer completes, spawn in a new enemy
    // Select a dead enemy to spawn
    uint32_t index = 0;
    for(uint32_t i = 0; i < _enemies.size(); ++i) {
        if (_enemies[i]->IsDead()) {
            index = i;
            break;
        }
    }

    // Used to retain random position coordinates in the zone
    float x = 0.0f;
    float y = 0.0f;
    // Number of times to try finding a valid spawning location
    int8_t retries = SPAWN_RETRIES;
    // Holds the result of a collision detection check
    uint32_t collision = NO_COLLISION;

    // Select a random position inside the zone to place the spawning enemy
    _enemies[index]->SetCollisionMask(WALL_COLLISION | CHARACTER_COLLISION);
    MapZone* spawning_zone = nullptr;
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
                    nullptr);
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
            vt_video::VideoManager->DrawRectangle((it->right_col - it->left_col) * GRID_LENGTH,
                                                  (it->bottom_row - it->top_row) * GRID_LENGTH,
                                                   vt_video::Color(0.0f, 0.0f, 0.0f, 0.5f));
        }
    }
}

EnemyZone::EnemyZone(const EnemyZone&) :
    MapZone(0, 0, 0, 0)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
}

EnemyZone& EnemyZone::operator=(const EnemyZone&)
{
    throw vt_utils::Exception("Not Implemented!", __FILE__, __LINE__, __FUNCTION__);
    return *this;
}

} // namespace private_map

} // namespace vt_map
