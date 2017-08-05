///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_sprites/map_virtual_sprite.h"

#include "modes/map/map_sprites/map_enemy_sprite.h"
#include "modes/map/map_objects/map_physical_object.h"

#include "modes/map/map_mode.h"
#include "modes/map/map_object_supervisor.h"
#include "modes/map/map_event_supervisor.h"

#include "engine/video/video_utils.h"
#include "engine/system.h"
#include "utils/utils_numeric.h"
#include "utils/utils_random.h"

using namespace vt_common;

namespace vt_map
{

namespace private_map
{

VirtualSprite::VirtualSprite(MapObjectDrawLayer layer) :
    MapObject(layer),
    _direction(SOUTH),
    _movement_speed(NORMAL_SPEED),
    _moving(false),
    _moved_position(false),
    _is_running(false),
    _control_event(nullptr),
    _state_saved(false),
    _saved_direction(0),
    _saved_movement_speed(0.0f),
    _saved_moving(false)
{
    _object_type = VIRTUAL_TYPE;
}

VirtualSprite::~VirtualSprite()
{
}

void VirtualSprite::Update()
{
    _moved_position = false;

    // Update potential emote animation
    MapObject::_UpdateEmote();

    if(!_updatable || !_moving)
        return;

    _SetNextPosition();
} // void VirtualSprite::Update()

bool VirtualSprite::_HandleWallEdges(float& next_pos_x,
                                     float& next_pos_y,
                                     float distance_moved,
                                     MapObject* collision_object)
{
    // First we don't deal with sprites edges when it's not a physical sprite
    if (collision_object && collision_object->GetObjectType() != PHYSICAL_TYPE)
        return false;

    // Handles soft edges for straight directions
    // Set a walk-around direction when the straight one is blocking the sprite,
    // and when the sprite is on the corner of the obstacle.
    bool on_edge = false;
    float edge_next_pos_x = 0.0f;
    float edge_next_pos_y = 0.0f;

    // Cap the actual distance moved when on an edge to a sane value according to the following checks.
    // Without this cap, the distance moved is too high when running and/or with a high walk
    // speed and can cause glitches.
    float edge_distance_moved = distance_moved;
    if (edge_distance_moved > 0.09f)
        edge_distance_moved = 0.09f;

    ObjectSupervisor *object_supervisor = MapMode::CurrentInstance()->GetObjectSupervisor();

    if(_direction & NORTH) {
        // Test both the north-east and north west cases
        if(!object_supervisor->IsStaticCollision(_tile_position.x + _coll_grid_half_width,
                                                 _tile_position.y - _coll_grid_height - distance_moved)) {
            edge_next_pos_x = _tile_position.x + edge_distance_moved;
            edge_next_pos_y = _tile_position.y;
            on_edge = true;
        }
        else if (!object_supervisor->IsStaticCollision(_tile_position.x - _coll_grid_half_width,
                                                       _tile_position.y - _coll_grid_height - distance_moved)) {
            edge_next_pos_x = _tile_position.x - edge_distance_moved;
            edge_next_pos_y = _tile_position.y;
            on_edge = true;
        }
    }
    else if(_direction & SOUTH) {
        // Test both the south-east and south west cases
        if(!object_supervisor->IsStaticCollision(_tile_position.x + _coll_grid_half_width,
                                                 _tile_position.y + distance_moved)) {
            edge_next_pos_x = _tile_position.x + edge_distance_moved;
            edge_next_pos_y = _tile_position.y;
            on_edge = true;
        }
        else if (!object_supervisor->IsStaticCollision(_tile_position.x - _coll_grid_half_width,
                                                       _tile_position.y + distance_moved)) {
            edge_next_pos_x = _tile_position.x - edge_distance_moved;
            edge_next_pos_y = _tile_position.y;
            on_edge = true;
        }
    }
    else if(_direction & EAST) {
        // Test both the north-east and south-east cases
        if(!object_supervisor->IsStaticCollision(_tile_position.x + _coll_grid_half_width + distance_moved,
                                                 _tile_position.y - _coll_grid_height)) {
            edge_next_pos_x = _tile_position.x;
            edge_next_pos_y = _tile_position.y - edge_distance_moved;
            on_edge = true;
        }
        else if (!object_supervisor->IsStaticCollision(_tile_position.x + _coll_grid_half_width + distance_moved,
                                                       _tile_position.y)) {
            edge_next_pos_x = _tile_position.x;
            edge_next_pos_y = _tile_position.y + edge_distance_moved;
            on_edge = true;
        }
    }
    else if(_direction & WEST) {
        // Test both the north-west and south-west cases
        if(!object_supervisor->IsStaticCollision(_tile_position.x - _coll_grid_half_width - distance_moved,
                                                 _tile_position.y - _coll_grid_height)) {
            edge_next_pos_x = _tile_position.x;
            edge_next_pos_y = _tile_position.y - edge_distance_moved;
            on_edge = true;
        }
        else if (!object_supervisor->IsStaticCollision(_tile_position.x - _coll_grid_half_width - distance_moved,
                                                       _tile_position.y)) {
            edge_next_pos_x = _tile_position.x;
            edge_next_pos_y = _tile_position.y + edge_distance_moved;
            on_edge = true;
        }
    }

    // If no edge is found, we don't do anything.
    if (!on_edge)
        return false;

    // Final check of the new position chosen
    if (object_supervisor->DetectCollision(this, edge_next_pos_x, edge_next_pos_y, &collision_object) != NO_COLLISION)
        return false;

    // Set the new position once all the tests passed.
    next_pos_x = edge_next_pos_x;
    next_pos_y = edge_next_pos_y;

    return true;
}

void VirtualSprite::_SetNextPosition()
{

    // Next sprite's position holders
    float next_pos_x = GetXPosition();
    float next_pos_y = GetYPosition();
    float distance_moved = CalculateDistanceMoved();

    // Move the sprite the appropriate distance in the appropriate Y and X direction
    if(_direction & (NORTH | MOVING_NORTHWEST | MOVING_NORTHEAST))
        next_pos_y -= distance_moved;
    else if(_direction & (SOUTH | MOVING_SOUTHWEST | MOVING_SOUTHEAST))
        next_pos_y += distance_moved;
    if(_direction & (WEST | MOVING_NORTHWEST | MOVING_SOUTHWEST))
        next_pos_x -= distance_moved;
    else if(_direction & (EAST | MOVING_NORTHEAST | MOVING_SOUTHEAST))
        next_pos_x += distance_moved;

    // When not moving, do not check anything else.
    if(next_pos_x == GetXPosition() && next_pos_y == GetYPosition())
        return;

    // We've got the next position, let's check whether the next position
    // should be revised.

    // Used to know whether we could fall back to a straight move
    // in case of collision.
    bool moving_diagonally = (_direction & (MOVING_NORTHWEST | MOVING_NORTHEAST
                                           | MOVING_SOUTHEAST | MOVING_SOUTHWEST));

    // Handle collision with the first object encountered
    MapObject* collision_object = nullptr;
    MapMode* map_mode = MapMode::CurrentInstance();
    ObjectSupervisor* object_supervisor = map_mode->GetObjectSupervisor();
    COLLISION_TYPE collision_type = object_supervisor->DetectCollision(this, next_pos_x,
                     next_pos_y,
                     &collision_object);
    // Try to fall back to straight direction
    if(moving_diagonally && collision_type != NO_COLLISION) {
        // Try on x axis
        if(object_supervisor->DetectCollision(this, _tile_position.x, next_pos_y, &collision_object) == NO_COLLISION) {
            next_pos_x = _tile_position.x;
            collision_type = NO_COLLISION;
        } // and then on y axis
        else if(object_supervisor->DetectCollision(this, next_pos_x, _tile_position.y, &collision_object) == NO_COLLISION) {
            next_pos_y = _tile_position.y;
            collision_type = NO_COLLISION;
        }
    }

    // Handles special collision handling first
    if(_control_event) {
        switch(_control_event->GetEventType()) {
            // Don't stuck the player's character or a sprite being controlled by a prepared path.
            // Plus, it's better not to change a path with encountered beings once started
            // for simplification purpose.
        case PATH_MOVE_SPRITE_EVENT:
            collision_type = NO_COLLISION;
            break;
            // Change the direction whenever something blocking is in the way.
        case RANDOM_MOVE_SPRITE_EVENT:
            if(collision_type != NO_COLLISION) {
                SetRandomDirection();
                return;
            }
        default:
            break;
        }
    }

    // Try to handle wall and physical collisions after a failed straight or diagonal move
    switch(collision_type) {
    case NO_COLLISION:
    default:
        break;
    case WALL_COLLISION:
        // When being blocked and moving diagonally, the npc is stuck.
        if(moving_diagonally)
            return;

        // Don't consider physical objects with an event to avoid sliding on their edges,
        // making them harder to "talk with".
        if (collision_object && this == map_mode->GetCamera()) {
            PhysicalObject *phs = reinterpret_cast<PhysicalObject *>(collision_object);
            if(phs && !phs->GetEventIdWhenTalking().empty())
                return;
        }

        // Fix the direction and destination to walk-around obstacles
        if (_HandleWallEdges(next_pos_x, next_pos_y, distance_moved, collision_object))
            break;
        // We don't do any other checks for the player sprite.
        else if (this == map_mode->GetCamera())
            return;

        // NPC sprites:

        // When it's a true wall, try against the collision grid
        if(!collision_object) {
            // Try a random diagonal to avoid the wall in straight direction
            if(_direction & (NORTH | SOUTH))
                _direction |= vt_utils::RandomBoundedInteger(0, 1) ? EAST : WEST;
            else if(_direction & (EAST | WEST))
                _direction |= vt_utils::RandomBoundedInteger(0, 1) ? NORTH : SOUTH;
            return;
        }
        // Physical and treasure objects are the only other matching "fake" walls
        else {
            // Try a diagonal to avoid the sprite in straight direction by comparing
            // each one coords.
            float diff_x = GetXPosition() - collision_object->GetXPosition();
            float diff_y = GetYPosition() - collision_object->GetYPosition();
            if(_direction & (NORTH | SOUTH))
                _direction |= diff_x >= 0.0f ? EAST : WEST;
            else if(_direction & (EAST | WEST))
                _direction |= diff_y >= 0.0f ? SOUTH : NORTH;
            return;
        }
        // Other cases shouldn't happen.
        break;
    case ENEMY_COLLISION:
        // Check only whether the player has collided with a monster
        if(this == map_mode->GetCamera() &&
                collision_object && collision_object->GetObjectType() == ENEMY_TYPE) {
            EnemySprite* enemy = reinterpret_cast<EnemySprite *>(collision_object);

            // Check whether the player is actually playing. If not, we don't want to start a battle.
            if (map_mode->CurrentState() == STATE_EXPLORE)
                map_mode->StartEnemyEncounter(enemy);
            return;
        }

        break;
    case CHARACTER_COLLISION:
        // Check whether the sprite is tangled with another character, even without moving
        // For instance, when colliding with a path follower npc.
        // And let it through in that case.
        if(object_supervisor->CheckObjectCollision(GetGridCollisionRectangle(), collision_object)) {
            collision_type = NO_COLLISION;
            break;
        }

        // When the sprite is controlled by the camera, let the player handle the position correction.
        if(this == map_mode->GetCamera())
            return;

        // Check whether an enemy has collided with the player
        if(this->GetType() == ENEMY_TYPE && collision_object == map_mode->GetCamera()) {
            EnemySprite* enemy = reinterpret_cast<EnemySprite *>(this);

            // Check whether the player is actually playing. If not, we don't want to start a battle.
            if (map_mode->CurrentState() == STATE_EXPLORE)
                map_mode->StartEnemyEncounter(enemy, false, true); // The enemy gets a boost in stamina.
            return;
        }

        // When being blocked and moving diagonally, the npc is stuck.
        if(moving_diagonally)
            return;

        if(!collision_object)  // Should never happen
            return;

        // Try a diagonal to avoid the sprite in straight direction by comparing
        // each one coords.
        float diff_x = GetXPosition() - collision_object->GetXPosition();
        float diff_y = GetYPosition() - collision_object->GetYPosition();
        if(_direction & (NORTH | SOUTH))
            _direction |= diff_x >= 0.0f ? EAST : WEST;
        else if(_direction & (EAST | WEST))
            _direction |= diff_y >= 0.0f ? SOUTH : NORTH;
        return;
    }

    // Inform the overlay system of the parallax movement done if needed
    if(this == map_mode->GetCamera()) {
        float x_parallax = !map_mode->IsCameraXAxisInMapCorner() ?
                           (GetXPosition() - next_pos_x)
                               / SCREEN_GRID_X_LENGTH
                               * vt_video::VIDEO_STANDARD_RES_WIDTH :
                           0.0f;
        float y_parallax = !map_mode->IsCameraYAxisInMapCorner() ?
                           (GetYPosition() - next_pos_y)
                               / SCREEN_GRID_Y_LENGTH
                               * vt_video::VIDEO_STANDARD_RES_HEIGHT :
                           0.0f;

        map_mode->GetEffectSupervisor().AddParallax(x_parallax, y_parallax);
        map_mode->GetIndicatorSupervisor().AddParallax(x_parallax, y_parallax);
    }

    // Make the sprite advance at the end
    SetPosition(next_pos_x, next_pos_y);
    _moved_position = true;
}


void VirtualSprite::SetDirection(uint16_t dir)
{
    // Nothing complicated needed for lateral directions
    if(dir & (NORTH | SOUTH | EAST | WEST)) {
        _direction = dir;
    }
    // Otherwise if the direction is diagonal we must figure out which way the sprite should face.
    else if(dir & MOVING_NORTHWEST) {
        if(_direction & (FACING_NORTH | FACING_EAST))
            _direction = NW_NORTH;
        else
            _direction = NW_WEST;
    } else if(dir & MOVING_SOUTHWEST) {
        if(_direction & (FACING_SOUTH | FACING_EAST))
            _direction = SW_SOUTH;
        else
            _direction = SW_WEST;
    } else if(dir & MOVING_NORTHEAST) {
        if(_direction & (FACING_NORTH | FACING_WEST))
            _direction = NE_NORTH;
        else
            _direction = NE_EAST;
    } else if(dir & MOVING_SOUTHEAST) {
        if(_direction & (FACING_SOUTH | FACING_WEST))
            _direction = SE_SOUTH;
        else
            _direction = SE_EAST;
    } else {
        IF_PRINT_WARNING(MAP_DEBUG) << "attempted to set an invalid direction: " << dir << std::endl;
    }
}

void VirtualSprite::SetRandomDirection()
{
    switch(vt_utils::RandomBoundedInteger(1, 8)) {
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
        IF_PRINT_WARNING(MAP_DEBUG) << "invalid randomized direction was chosen" << std::endl;
    }
}

void VirtualSprite::LookAt(const Position2D& pos)
{
    // If the two positions are the same,
    // don't update the direction since it's only a matter of keeping
    // the previous one.
    if(_tile_position.x == pos.x && _tile_position.y == pos.y)
        return;

    // First handle simple cases
    if(vt_utils::IsFloatEqual(_tile_position.x, pos.x, 0.5f)) {
        if(_tile_position.y > pos.y)
            SetDirection(NORTH);
        else
            SetDirection(SOUTH);
        return;
    }

    if(vt_utils::IsFloatEqual(_tile_position.y, pos.y, 0.5f)) {
        if(_tile_position.x > pos.x)
            SetDirection(WEST);
        else
            SetDirection(EAST);
        return;
    }

    // Now let's handle diagonal cases
    // First, find the lower angle:
    if(_tile_position.x < pos.x) {
        // Up-right direction
        if(_tile_position.y > pos.y) {
            // Compute tan of the angle
            if((_tile_position.y - pos.y) / (pos.x - _tile_position.x) < 1)
                // The angle is less than 45°, look to the right
                SetDirection(EAST);
            else
                SetDirection(NORTH);
            return;
        } else { // Down-right
            // Compute tan of the angle
            if((pos.y - _tile_position.y) / (pos.x - _tile_position.x) < 1)
                // The angle is less than 45°, look to the right
                SetDirection(EAST);
            else
                SetDirection(SOUTH);
            return;
        }
    } else {
        // Up-left direction
        if(_tile_position.y > pos.y) {
            // Compute tan of the angle
            if((_tile_position.y - pos.y) / (_tile_position.x - pos.x) < 1)
                // The angle is less than 45°, look to the left
                SetDirection(WEST);
            else
                SetDirection(NORTH);
            return;
        } else { // Down-left
            // Compute tan of the angle
            if((pos.y - _tile_position.y) / (_tile_position.x - pos.x) < 1)
                // The angle is less than 45°, look to the left
                SetDirection(WEST);
            else
                SetDirection(SOUTH);
            return;
        }
    }
}

float VirtualSprite::CalculateDistanceMoved()
{
    float distance_moved = static_cast<float>(vt_system::SystemManager->GetUpdateTime()) / _movement_speed;

    // Double the distance to move if the sprite is running
    if(_is_running)
        distance_moved *= 2.0f;
    // If the movement is diagonal, decrease the lateral movement distance by sin(45 degress)
    if(_direction & MOVING_DIAGONALLY)
        distance_moved *= 0.707f;

    // We cap the distance moved when in case of low FPS to avoid letting certain
    // sprites jump across blocking areas.
    if (distance_moved > 1.0f)
        distance_moved = 1.0f;

    return distance_moved;
}

void VirtualSprite::AcquireControl(SpriteEvent* event)
{
    if(event == nullptr) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function argument was nullptr" << std::endl;
        return;
    }

    if(_control_event != nullptr) {
        IF_PRINT_WARNING(MAP_DEBUG) << "a new event is acquiring control when the previous event has not "
                                    "released control over this sprite, object id: " << GetObjectID() << std::endl;
    }
    _control_event = event;
}

void VirtualSprite::ReleaseControl(SpriteEvent *event)
{
    if(event == nullptr) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function argument was nullptr" << std::endl;
        return;
    }

    if(_control_event == nullptr) {
        IF_PRINT_WARNING(MAP_DEBUG) << "no event had control over this sprite, object id: " << GetObjectID() << std::endl;
    } else if(_control_event != event) {
        IF_PRINT_WARNING(MAP_DEBUG) << "a different event has control of this sprite, object id: " << GetObjectID() << std::endl;
    } else {
        _control_event = nullptr;
    }
}

void VirtualSprite::SaveState()
{
    _state_saved = true;
    _saved_direction = _direction;
    _saved_movement_speed = _movement_speed;
    _saved_moving = _moving;
    MapMode::CurrentInstance()->GetEventSupervisor()->PauseAllEvents(this);
}

void VirtualSprite::RestoreState()
{
    if(_state_saved == false)
        IF_PRINT_WARNING(MAP_DEBUG) << "restoring state when no saved state was detected" << std::endl;

    _state_saved = false;
    _direction = _saved_direction;
    _movement_speed = _saved_movement_speed;
    _moving = _saved_moving;
    MapMode::CurrentInstance()->GetEventSupervisor()->ResumeAllEvents(this);
}

} // namespace private_map

} // namespace vt_map
