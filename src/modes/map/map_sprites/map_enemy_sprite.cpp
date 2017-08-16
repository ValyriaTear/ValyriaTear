///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_sprites/map_enemy_sprite.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_zones.h"

using namespace vt_common;

namespace vt_map
{

namespace private_map
{

EnemySprite::EnemySprite() :
    MapSprite(GROUND_OBJECT),
    _zone(nullptr),
    _color(1.0f, 1.0f, 1.0f, 0.0f),
    _aggro_range(8.0f),
    _time_before_new_destination(1200),
    _time_to_spawn(STANDARD_ENEMY_FIRST_SPAWN_TIME),
    _time_to_respawn(STANDARD_ENEMY_SPAWN_TIME),
    _is_boss(false),
    _use_path(false)
{
    _object_type = ENEMY_TYPE;
    _moving = false;
    Reset();
}

EnemySprite::~EnemySprite()
{
}

EnemySprite* EnemySprite::Create()
{
    // The object auto register to the object supervisor
    // and will later handle deletion.
    return new EnemySprite();
}

void EnemySprite::Reset()
{
    _updatable = false;
    _collision_mask = NO_COLLISION;
    _state = DEAD;
    _time_elapsed = 0;
    _color.SetAlpha(0.0f);

    // Reset path finding info
    _last_node_position = Position2D(0.0f, 0.0f);
    _current_node = Position2D(0.0f, 0.0f);
    _destination = Position2D(0.0f, 0.0f);
    _current_node_id = 0;
    _path.clear();
    _use_path = false;

    // Reset the currently selected way point
    _current_way_point_id = 0;
}

void EnemySprite::AddEnemy(uint32_t enemy_id,
                           float position_x, float position_y)
{
    if(_enemy_parties.empty()) {
        IF_PRINT_WARNING(MAP_DEBUG) << "can not add new enemy when no parties have been declared"
                                    << std::endl;
        return;
    }

    vt_battle::BattleEnemyInfo enemy_info(enemy_id, position_x, position_y);
    _enemy_parties.back().push_back(enemy_info);
}

// Static empty enemy party used to prevent temporary reference returns.
static const std::vector<vt_battle::BattleEnemyInfo> empty_enemy_party;

const std::vector<vt_battle::BattleEnemyInfo>& EnemySprite::RetrieveRandomParty() const
{
    if(_enemy_parties.empty()) {
        PRINT_ERROR << "No enemy parties exist and none can be created."
                    << std::endl;
        return empty_enemy_party;
    }

    return _enemy_parties[rand() % _enemy_parties.size()];
}

void EnemySprite::ChangeStateHostile()
{
    _updatable = true;
    _state = HOSTILE;
    _collision_mask = WALL_COLLISION | CHARACTER_COLLISION;
    _color.SetAlpha(1.0);
    // Set the next spawn time, usually longer than the first one.
    _time_to_spawn = _time_to_respawn;
}

void EnemySprite::Update()
{
    switch(_state) {
        // Gradually increase the alpha while the sprite is fading in during spawning
    case SPAWNING:
        _time_elapsed += vt_system::SystemManager->GetUpdateTime();
        if(_color.GetAlpha() < 1.0f) {
            _color.SetAlpha((_time_elapsed / static_cast<float>(_time_to_spawn)) * 1.0f);
        } else {
            ChangeStateHostile();
        }
        break;

        // Set the sprite's direction so that it seeks to collide with the map camera's position
    case HOSTILE:
        _HandleHostileUpdate();
        break;

    // Do nothing if the sprite is in the DEAD state, or any other state
    case DEAD:
    default:
        break;
    }
} // void EnemySprite::Update()

void EnemySprite::ChangeStateDead() {
    Reset();
    if(_zone) _zone->EnemyDead();
}

void EnemySprite::_HandleHostileUpdate()
{
    // Holds the x and y deltas between the sprite and map camera coordinate pairs
    VirtualSprite* camera = MapMode::CurrentInstance()->GetCamera();
    float camera_x = camera->GetXPosition();
    float camera_y = camera->GetYPosition();

    float xdelta = GetXPosition() - camera_x;
    float ydelta = GetYPosition() - camera_y;
    float abs_xdelta = fabs(xdelta);
    float abs_ydelta = fabs(ydelta);

    // Don't update enemies that are too far away...
    if (abs_xdelta > SCREEN_GRID_X_LENGTH || abs_ydelta > SCREEN_GRID_Y_LENGTH)
        return;

    // Updates sprite animation and collision fix.
    MapSprite::Update();

    // Test whether the monster has spotted its target.
    bool player_in_aggro_range = false;
    if(abs_xdelta <= _aggro_range && abs_ydelta <= _aggro_range)
        player_in_aggro_range = true;

    // Handle chasing the character
    MapMode* map_mode = MapMode::CurrentInstance();
    if (player_in_aggro_range && map_mode->AttackAllowed()) {
        // We first cancel the potential previous path.
        if (!_path.empty()) {
            // We cancel any previous path
            _path.clear();
            // We set the correct mask before moving normally
            _collision_mask = WALL_COLLISION | CHARACTER_COLLISION;
            _use_path = false;
        }

        // Check whether we're already colliding, so that even when not moving
        // we can start a battle.
        if (this->IsCollidingWith(camera))
            map_mode->StartEnemyEncounter(this);

        // Make the monster go toward the character
        if(xdelta > -0.5 && xdelta < 0.5 && ydelta < 0)
            SetDirection(SOUTH);
        else if(xdelta > -0.5 && xdelta < 0.5 && ydelta > 0)
            SetDirection(NORTH);
        else if(ydelta > -0.5 && ydelta < 0.5 && xdelta > 0)
            SetDirection(WEST);
        else if(ydelta > -0.5 && ydelta < 0.5 && xdelta < 0)
            SetDirection(EAST);
        else if(xdelta < 0 && ydelta < 0)
            SetDirection(MOVING_SOUTHEAST);
        else if(xdelta < 0 && ydelta > 0)
            SetDirection(MOVING_NORTHEAST);
        else if(xdelta > 0 && ydelta < 0)
            SetDirection(MOVING_SOUTHWEST);
        else
            SetDirection(MOVING_NORTHWEST);
        _moving = true;

        return;
    }

    // Handle monsters with way points.
    if (!_way_points.empty()) {

        // Update the wait time until next path between two way points.
        if (!_use_path || !_moving)
            _time_elapsed += vt_system::SystemManager->GetUpdateTime();

        if (_path.empty() && _time_elapsed >= _time_before_new_destination) {
            if (!_SetPathToNextWayPoint()) {
                // Fall back to simple movement mode
                SetRandomDirection();
                _moving = true;
            }
            // The sprite is now finding its way back into the zone
            _time_elapsed = 0;
        }

        if (_use_path && _path.empty()) {
            // The sprite is waiting for the next destination.
            _moving = false;
            // We then reset the correct walk mask
            _collision_mask = WALL_COLLISION | CHARACTER_COLLISION;
        }

        // Update the sprite direction according to the path
        _UpdatePath();
        return;
    }

    // Determine standard monster behavior regarding its zone.

    // Update the wait time until two set destination.
    _time_elapsed += vt_system::SystemManager->GetUpdateTime();

    // Check whether the monster can get out of the zone.
    bool can_get_out_of_zone = true;
    if (_zone && _zone->IsRoamingRestrained() && !player_in_aggro_range)
        can_get_out_of_zone = false;

    if (!can_get_out_of_zone) {
        // Check whether the monster is inside its zone
        bool out_of_zone = false;
        if (_zone && !_zone->IsInsideZone(GetXPosition(), GetYPosition()))
            out_of_zone = true;

        if (out_of_zone && _time_elapsed >= _time_before_new_destination) {
            // The sprite is now finding its way back into the zone
            float x_dest;
            float y_dest;
            _zone->RandomPosition(x_dest, y_dest);
            LookAt(x_dest, y_dest);
            _moving = true;

            _time_elapsed = 0;
            return;
        }
    }

    // Make the monster wander randomly in other cases
    if (_time_elapsed >= _time_before_new_destination) {
        SetRandomDirection();
        _moving = true;
        _time_elapsed = 0;
    }
}

void EnemySprite::Draw()
{
    // Otherwise, only draw it if it is not in the DEAD state
    if (!MapObject::ShouldDraw() || _state == DEAD)
        return;

    _animation->at(_current_anim_direction).Draw(_color);

    // Draw collision rectangle if the debug view is on.
    if (!vt_video::VideoManager->DebugInfoOn())
        return;

    Position2D pos = vt_video::VideoManager->GetDrawPosition();
    Rectangle2D rect = GetScreenCollisionRectangle(pos.x, pos.y);
    vt_video::VideoManager->DrawRectangle(rect.right - rect.left,
                                          rect.bottom - rect.top,
                                          vt_video::Color(1.0f, 0.0f, 0.0f,
                                                          0.6f));
}

void EnemySprite::AddWayPoint(float destination_x, float destination_y)
{
    Position2D destination(destination_x, destination_y);

    // Check whether the way point is already existing
    for (uint32_t i = 0; i < _way_points.size(); ++i) {
        if (_way_points[i].x == destination_x && _way_points[i].y == destination_y) {
            PRINT_WARNING << "Way point already added: "
                          << "(" << destination_x << ", "
                          << destination_y << ")" << std::endl;
            return;
        }
    }

    _way_points.push_back(destination);
}

bool EnemySprite::_SetPathToNextWayPoint()
{
    //! Will be set to true if _SetDestination() is succeeding
    _use_path = false;

    // There must be at least two way points to permit supporting those.
    if (_way_points.size() < 2)
        return false;

    if (_current_way_point_id >= _way_points.size())
        _current_way_point_id = 0;

    bool ret = _SetDestination(_way_points[_current_way_point_id].x,
                               _way_points[_current_way_point_id].y, 0);
    ++_current_way_point_id;

    return ret;
}

void EnemySprite::_UpdatePath()
{
    if(!_use_path || _path.empty())
        return;

    const Position2D sprite_position = GetPosition();
    const float distance_moved = CalculateDistanceMoved();

    // Check whether the sprite has arrived at the position of the current node
    if(vt_utils::IsFloatEqual(sprite_position.x, _current_node.x, distance_moved)
            && vt_utils::IsFloatEqual(sprite_position.y, _current_node.y, distance_moved)) {
        ++_current_node_id;

        if(_current_node_id < _path.size()) {
            _current_node.x = _path[_current_node_id].x;
            _current_node.y = _path[_current_node_id].y;
        }
    }
    // If the sprite has moved to a new position other than the next node, adjust its direction so it is trying to move to the next node
    else if((sprite_position.x != _last_node_position.x)
            || (sprite_position.y != _last_node_position.y)) {
        _last_node_position = sprite_position;
    }

    _SetSpritePathDirection();

    // End the path event
    if(vt_utils::IsFloatEqual(sprite_position.x, _destination.x, distance_moved)
            && vt_utils::IsFloatEqual(sprite_position.y, _destination.y, distance_moved)) {
        _path.clear();
    }
}

bool EnemySprite::_SetDestination(float destination_x, float destination_y,
                                  uint32_t max_cost)
{
    _path.clear();
    _use_path = false;

    uint32_t dest_x = static_cast<uint32_t>(destination_x);
    uint32_t dest_y = static_cast<uint32_t>( destination_y);
    uint32_t pos_x = static_cast<uint32_t>(GetXPosition());
    uint32_t pos_y = static_cast<uint32_t>(GetYPosition());

    // Don't check the path if the sprite is there.
    if (pos_x == dest_x && pos_y == dest_y)
        return false;

    Position2D dest(destination_x, destination_y);
    // We set the correct mask before finding the path
    _collision_mask = WALL_COLLISION | CHARACTER_COLLISION;
    _path =
        MapMode::CurrentInstance()->GetObjectSupervisor()->FindPath(this, dest,
                                                                    max_cost);

    if (_path.empty())
        return false;

    // But remove wall collision afterward to avoid making it stuck in corners.
    // Note: this function is only called when hostile, son we don't deal with
    // the spawning collision mask.
    _collision_mask = CHARACTER_COLLISION;

    _current_node_id = 0;
    _last_node_position = GetPosition();
    _destination = dest;

    _current_node = _path[_current_node_id];

    _moving = true;
    _use_path = true;
    return true;
}

void EnemySprite::_SetSpritePathDirection()
{
    if (!_use_path || _path.empty())
        return;

    uint16_t direction = 0;

    const Position2D sprite_position = GetPosition();
    const float distance_moved = CalculateDistanceMoved();

    if(sprite_position.y - _current_node.y > distance_moved) {
        direction |= NORTH;
    } else if(sprite_position.y - _current_node.y < -distance_moved) {
        direction |= SOUTH;
    }

    if(sprite_position.x - _current_node.x > distance_moved) {
        direction |= WEST;
    } else if(sprite_position.x - _current_node.x < -distance_moved) {
        direction |= EAST;
    }

    // Determine if the sprite should move diagonally to the next node
    if((direction & (NORTH | SOUTH)) && (direction & (WEST | EAST))) {
        switch(direction) {
        case(NORTH | WEST):
            direction = MOVING_NORTHWEST;
            break;
        case(NORTH | EAST):
            direction = MOVING_NORTHEAST;
            break;
        case(SOUTH | WEST):
            direction = MOVING_SOUTHWEST;
            break;
        case(SOUTH | EAST):
            direction = MOVING_SOUTHEAST;
            break;
        }
    }

    SetDirection(direction);
}

} // namespace private_map

} // namespace vt_map
