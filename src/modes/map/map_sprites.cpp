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
*** \file    map_sprites.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for map mode sprites.
*** ***************************************************************************/

#include "utils/utils_pch.h"
#include "modes/map/map_sprites.h"

#include "modes/map/map_events.h"

#include "modes/battle/battle.h"
#include "common/global/global.h"

#include "utils/utils_random.h"
#include "utils/utils_files.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_mode_manager;
using namespace vt_video;
using namespace vt_script;
using namespace vt_system;
using namespace vt_global;
using namespace vt_battle;

namespace vt_map
{

namespace private_map
{

/** \brief Returns the opposite facing direction of the direction given in parameter.
*** \return A direction that faces opposite to the argument direction
*** \note This is mostly used as an helper function to make sprites face each other in a conversation.
**/
static uint16_t CalculateOppositeDirection(const uint16_t direction)
{
    switch(direction) {
    case NORTH:
        return SOUTH;
    case SOUTH:
        return NORTH;
    case WEST:
        return EAST;
    case EAST:
        return WEST;
    case NW_NORTH:
        return SE_SOUTH;
    case NW_WEST:
        return SE_EAST;
    case NE_NORTH:
        return SW_SOUTH;
    case NE_EAST:
        return SW_WEST;
    case SW_SOUTH:
        return NE_NORTH;
    case SW_WEST:
        return NE_EAST;
    case SE_SOUTH:
        return NW_NORTH;
    case SE_EAST:
        return NW_WEST;
    default:
        IF_PRINT_WARNING(MAP_DEBUG) << "invalid direction argument: "
                                    << direction << std::endl;
        return SOUTH;
    }
}

// ****************************************************************************
// ********** VirtualSprite class methods
// ****************************************************************************

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

bool VirtualSprite::_HandleWallEdges(float& next_pos_x, float& next_pos_y, float distance_moved,
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
                _direction |= RandomBoundedInteger(0, 1) ? EAST : WEST;
            else if(_direction & (EAST | WEST))
                _direction |= RandomBoundedInteger(0, 1) ? NORTH : SOUTH;
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
                           (GetXPosition() - next_pos_x) / SCREEN_GRID_X_LENGTH * VIDEO_STANDARD_RES_WIDTH :
                           0.0f;
        float y_parallax = !map_mode->IsCameraYAxisInMapCorner() ?
                           (GetYPosition() - next_pos_y) / SCREEN_GRID_Y_LENGTH * VIDEO_STANDARD_RES_HEIGHT :
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
} // void VirtualSprite::SetDirection(uint16_t dir)



void VirtualSprite::SetRandomDirection()
{
    switch(RandomBoundedInteger(1, 8)) {
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

void VirtualSprite::LookAt(const MapPosition &pos)
{
    // If the two positions are the same,
    // don't update the direction since it's only a matter of keeping
    // the previous one.
    if(_tile_position.x == pos.x && _tile_position.y == pos.y)
        return;

    // First handle simple cases
    if(IsFloatEqual(_tile_position.x, pos.x, 0.5f)) {
        if(_tile_position.y > pos.y)
            SetDirection(NORTH);
        else
            SetDirection(SOUTH);
        return;
    }

    if(IsFloatEqual(_tile_position.y, pos.y, 0.5f)) {
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
    float distance_moved = static_cast<float>(SystemManager->GetUpdateTime()) / _movement_speed;

    // Double the distance to move if the sprite is running
    if(_is_running == true)
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

// ****************************************************************************
// ********** MapSprite class methods
// ****************************************************************************

MapSprite::MapSprite(MapObjectDrawLayer layer) :
    VirtualSprite(layer),
    _face_portrait(0),
    _has_running_animations(false),
    _current_anim_direction(ANIM_SOUTH),
    _current_custom_animation(0),
    _next_dialogue(-1),
    _has_available_dialogue(false),
    _has_unseen_dialogue(false),
    _dialogue_started(false),
    _custom_animation_on(false),
    _custom_animation_time(0),
    _infinite_custom_animation(false),
    _saved_current_anim_direction(ANIM_SOUTH)
{
    _object_type = SPRITE_TYPE;

    // Points the current animation to the standing animation vector by default
    _animation = &_standing_animations;
}

MapSprite::~MapSprite()
{
    if (_face_portrait)
        delete _face_portrait;
}

MapSprite* MapSprite::Create(MapObjectDrawLayer layer)
{
    // The object auto register to the object supervisor
    // and will later handle deletion.
    return new MapSprite(layer);
}

bool _LoadAnimations(std::vector<vt_video::AnimatedImage>& animations, const std::string &filename)
{
    // Prepare to add the animations for each directions, if needed.

    // In case of reloading
    animations.clear();
    for(uint8_t i = 0; i < NUM_ANIM_DIRECTIONS; ++i)
        animations.push_back(AnimatedImage());

    vt_script::ReadScriptDescriptor animations_script;
    if(!animations_script.OpenFile(filename))
        return false;

    if(!animations_script.DoesTableExist("sprite_animation")) {
        PRINT_WARNING << "No 'sprite_animation' table in 4-direction animation script file: " << filename << std::endl;
        animations_script.CloseFile();
        return false;
    }

    animations_script.OpenTable("sprite_animation");

    std::string image_filename = animations_script.ReadString("image_filename");
    if(!vt_utils::DoesFileExist(image_filename)) {
        PRINT_WARNING << "The image file doesn't exist: " << image_filename << std::endl;
        animations_script.CloseTable();
        animations_script.CloseFile();
        return false;
    }

    bool blended_animation = false;
    if (animations_script.DoesBoolExist("blended_animation")) {
        blended_animation = animations_script.ReadBool("blended_animation");
    }

    uint32_t rows = animations_script.ReadUInt("rows");
    uint32_t columns = animations_script.ReadUInt("columns");

    if(!animations_script.DoesTableExist("frames")) {
        animations_script.CloseAllTables();
        animations_script.CloseFile();
        PRINT_WARNING << "No frame table in file: " << filename << std::endl;
        return false;
    }

    std::vector<StillImage> image_frames;
    // Load the image data
    if(!ImageDescriptor::LoadMultiImageFromElementGrid(image_frames, image_filename, rows, columns)) {
        PRINT_WARNING << "Couldn't load elements from image file: " << image_filename
                      << " (in file: " << filename << ")" << std::endl;
        animations_script.CloseAllTables();
        animations_script.CloseFile();
        return false;
    }

    std::vector <uint32_t> frames_directions_ids;
    animations_script.ReadTableKeys("frames", frames_directions_ids);

    // open the frames table
    animations_script.OpenTable("frames");

    for(uint32_t i = 0; i < frames_directions_ids.size(); ++i) {
        if(frames_directions_ids[i] >= NUM_ANIM_DIRECTIONS) {
            PRINT_WARNING << "Invalid direction id(" << frames_directions_ids[i]
                          << ") in file: " << filename << std::endl;
            continue;
        }

        uint32_t anim_direction = frames_directions_ids[i];

        // Opens frames[ANIM_DIRECTION]
        animations_script.OpenTable(anim_direction);

        // Loads the frames data
        std::vector<uint32_t> frames_ids;
        std::vector<uint32_t> frames_duration;

        uint32_t num_frames = animations_script.GetTableSize();
        for(uint32_t frames_table_id = 0;  frames_table_id < num_frames; ++frames_table_id) {
            // Opens frames[ANIM_DIRECTION][frame_table_id]
            animations_script.OpenTable(frames_table_id);

            int32_t frame_id = animations_script.ReadInt("id");
            int32_t frame_duration = animations_script.ReadInt("duration");

            if(frame_id < 0 || frame_duration < 0 || frame_id >= (int32_t)image_frames.size()) {
                PRINT_WARNING << "Invalid frame (" << frames_table_id << ") in file: "
                              << filename << std::endl;
                PRINT_WARNING << "Request for frame id: " << frame_id << ", duration: "
                              << frame_duration << " is not possible." << std::endl;
                continue;
            }

            frames_ids.push_back((uint32_t)frame_id);
            frames_duration.push_back((uint32_t)frame_duration);

            animations_script.CloseTable(); // frames[ANIM_DIRECTION][frame_table_id] table
        }

        // Actually create the animation data
        animations[anim_direction].Clear();
        animations[anim_direction].ResetAnimation();
        animations[anim_direction].SetAnimationBlended(blended_animation);
        for(uint32_t j = 0; j < frames_ids.size(); ++j) {
            // Set the dimension of the requested frame
            animations[anim_direction].AddFrame(image_frames[frames_ids[j]], frames_duration[j]);
        }

        // Closes frames[ANIM_DIRECTION]
        animations_script.CloseTable();

    } // for each directions

    // Close the 'frames' table and set the dimensions
    animations_script.CloseTable();

    float frame_width = animations_script.ReadFloat("frame_width");
    float frame_height = animations_script.ReadFloat("frame_height");

    // Load requested dimensions
    for(uint8_t i = 0; i < NUM_ANIM_DIRECTIONS; ++i) {
        if(frame_width > 0.0f && frame_height > 0.0f) {
            animations[i].SetDimensions(frame_width, frame_height);
        } else if(IsFloatEqual(animations[i].GetWidth(), 0.0f) && IsFloatEqual(animations[i].GetHeight(), 0.0f)) {
            // If the animation dimensions are not set, we're using the first frame size.
            animations[i].SetDimensions(image_frames.begin()->GetWidth(), image_frames.begin()->GetHeight());
        }
    }

    animations_script.CloseTable(); // sprite_animation table
    animations_script.CloseFile();

    return true;
}

void MapSprite::ClearAnimations()
{
    _grayscale = false;
    _standing_animations.clear();
    _walking_animations.clear();
    _running_animations.clear();
    _has_running_animations = false;

    // Disable and clear the custom animations
    _current_custom_animation = 0;
    _custom_animation_on = false;
    _custom_animation_time = 0;
    _infinite_custom_animation = false;
    _custom_animations.clear();
}

bool MapSprite::LoadStandingAnimations(const std::string &filename)
{
    return _LoadAnimations(_standing_animations, filename);
}

bool MapSprite::LoadWalkingAnimations(const std::string &filename)
{
    return _LoadAnimations(_walking_animations, filename);
}

bool MapSprite::LoadRunningAnimations(const std::string &filename)
{
    _has_running_animations = _LoadAnimations(_running_animations, filename);

    return _has_running_animations;
}

bool MapSprite::LoadCustomAnimation(const std::string &animation_name, const std::string &filename)
{
    if(_custom_animations.find(animation_name) != _custom_animations.end()) {
        PRINT_WARNING << "The animation " << animation_name << " is already existing." << std::endl;
        return false;
    }

    AnimatedImage animation;
    if(animation.LoadFromAnimationScript(filename)) {
        _custom_animations.insert(std::make_pair(animation_name, animation));
        return true;
    }

    return false;
}

void MapSprite::SetCustomAnimation(const std::string &animation_name, int32_t time)
{
    // If there is no key, there will be no custom animation to display
    if(animation_name.empty()) {
        _custom_animation_on = false;
        return;
    }

    // Same if the key isn't found
    std::map<std::string, AnimatedImage>::iterator it = _custom_animations.find(animation_name);
    if(it == _custom_animations.end()) {
        PRINT_WARNING << "Couldn't find any custom animation '" << animation_name
            << "' for sprite: " << GetSpriteName() << std::endl;
        _custom_animation_on = false;
        return;
    }

    AnimatedImage &animation = it->second;
    animation.ResetAnimation();

    _infinite_custom_animation = false;
    if(time == -1)
        time = animation.GetAnimationLength();
    else if (time == 0)
        _infinite_custom_animation = true;


    _custom_animation_time = time;
    _current_custom_animation = &animation;
    _custom_animation_on = true;
}

void MapSprite::ReloadSprite(const std::string& sprite_name)
{
    // Don't reload if it's already the same map sprite
    if (sprite_name == GetSpriteName())
        return;

    vt_script::ReadScriptDescriptor& script = GlobalManager->GetMapSpriteScript();

    if (!script.IsFileOpen())
        return;

    if (!script.OpenTable(sprite_name)) {
        PRINT_WARNING << "No map sprite name: " << sprite_name << " found!!"
            << std::endl;
        return;
    }

    SetName(script.ReadString("name"));
    SetSpriteName(sprite_name);
    SetCollPixelHalfWidth(script.ReadFloat("coll_half_width"));
    SetCollPixelHeight(script.ReadFloat("coll_height"));
    SetImgPixelHalfWidth(script.ReadFloat("img_half_width"));
    SetImgPixelHeight(script.ReadFloat("img_height"));
    if (script.DoesStringExist("face_portrait"))
        LoadFacePortrait(script.ReadString("face_portrait"));

    if (!script.OpenTable("standard_animations")) {
        PRINT_WARNING << "No 'standard_animations' table in: "
            << sprite_name << std::endl;
        script.CloseTable(); // sprite_name
        return;
    }

    ClearAnimations();
    LoadStandingAnimations(script.ReadString("idle"));
    LoadWalkingAnimations(script.ReadString("walk"));
    if (script.DoesStringExist("run"))
        LoadRunningAnimations(script.ReadString("run"));

    script.CloseTable(); // standard_animations

    // Load potential custom animations
    if (!script.DoesTableExist("custom_animations")) {
        script.CloseTable(); // sprite_name
        return;
    }

    std::vector<std::string> anim_names;
    script.ReadTableKeys("custom_animations", anim_names);
    script.OpenTable("custom_animations");

    for (uint32_t i = 0; i < anim_names.size(); ++i)
        LoadCustomAnimation(anim_names[i], script.ReadString(anim_names[i]));

    // Put the tabl in the state we found it.
    script.CloseTable(); // custom_animations
    script.CloseTable(); // sprite_name
}

void MapSprite::LoadFacePortrait(const std::string &filename)
{
    if(_face_portrait)
        delete _face_portrait;

    _face_portrait = new StillImage();
    if(!_face_portrait->Load(filename)) {
        delete _face_portrait;
        _face_portrait = 0;
        PRINT_ERROR << "failed to load face portrait" << std::endl;
    }
}

void MapSprite::SetGrayscale(bool grayscale) {
    _grayscale = grayscale;
    for (vt_video::AnimatedImage& animation : _standing_animations) {
        animation.SetGrayscale(grayscale);
    }
    for (vt_video::AnimatedImage& animation : _walking_animations) {
        animation.SetGrayscale(grayscale);
    }
    for (vt_video::AnimatedImage& animation : _running_animations) {
        animation.SetGrayscale(grayscale);
    }
    auto it = _custom_animations.begin();
    auto it_end = _custom_animations.end();
    for (; it != it_end; ++it) {
        it->second.SetGrayscale(grayscale);
    }
}

void MapSprite::Update()
{
    // Stores the last value of moved_position to determine when a change in sprite movement between calls to this function occurs
    bool was_moved = _moved_position;

    // This call will update the sprite's position and perform collision detection
    VirtualSprite::Update();

    // if it's a custom animation, just display that and ignore everything else
    if(_custom_animation_on && _current_custom_animation) {
        // Check whether the custom animation can be freed
        if(!_infinite_custom_animation && _custom_animation_time <= 0) {
            _custom_animation_on = false;
            _current_custom_animation = 0;
            _custom_animation_time = 0;
        } else {
            if (!_infinite_custom_animation)
                _custom_animation_time -= SystemManager->GetUpdateTime();
            _current_custom_animation->Update();
        }

        was_moved = _moved_position;
        return;
    }

    // Save the previous animation state
    uint8_t last_anim_direction = _current_anim_direction;
    std::vector<AnimatedImage>* last_animation = _animation;

    // Set the sprite's animation to the standing still position if movement has just stopped
    if(!_moved_position) {
        // Set the current movement animation to zero progress
        if(was_moved)
            _animation->at(_current_anim_direction).ResetAnimation();

        // Determine the correct standing frame to display
        if(!_control_event || _state_saved) {
            _animation = &_standing_animations;

            if(_direction & FACING_NORTH) {
                _current_anim_direction = ANIM_NORTH;
            } else if(_direction & FACING_SOUTH) {
                _current_anim_direction = ANIM_SOUTH;
            } else if(_direction & FACING_WEST) {
                _current_anim_direction = ANIM_WEST;
            } else if(_direction & FACING_EAST) {
                _current_anim_direction = ANIM_EAST;
            } else {
                PRINT_ERROR << "invalid sprite direction, could not find proper standing animation to draw" << std::endl;
            }
        }
    }

    else { // then (moved_position)
        // Determine the correct animation to display
        if(_direction & FACING_NORTH) {
            _current_anim_direction = ANIM_NORTH;
        } else if(_direction & FACING_SOUTH) {
            _current_anim_direction = ANIM_SOUTH;
        } else if(_direction & FACING_WEST) {
            _current_anim_direction = ANIM_WEST;
        } else if(_direction & FACING_EAST) {
            _current_anim_direction = ANIM_EAST;
        } else {
            PRINT_ERROR << "invalid sprite direction, could not find proper standing animation to draw" << std::endl;
        }

        // Increasing the animation index by four from the walking _animations leads to the running _animations
        if(_is_running && _has_running_animations) {
            _animation = &_running_animations;
        } else {
            _animation = &_walking_animations;
        }
    }

    // If the direction of movement changed in mid-flight, update the animation timer on the
    // new animated image to reflect the old, so the walking _animations do not appear to
    // "start and stop" whenever the direction is changed.
    if(last_anim_direction != _current_anim_direction || last_animation != _animation) {
        _animation->at(_current_anim_direction).SetTimeProgress(last_animation->at(last_anim_direction).GetTimeProgress());
        last_animation->at(last_anim_direction).ResetAnimation();
    }

    // Take care of adapting the update time according to the sprite speed when walking or running
    uint32_t elapsed_time = 0;
    if(_animation == &_walking_animations || (_has_running_animations && _animation == &_running_animations)) {
        elapsed_time = (uint32_t)(((float)vt_system::SystemManager->GetUpdateTime()) * NORMAL_SPEED / _movement_speed);
    }

    _animation->at(_current_anim_direction).Update(elapsed_time);

    was_moved = _moved_position;
} // void MapSprite::Update()

void MapSprite::_DrawDebugInfo()
{
    // Draw collision rectangle if the debug view is on.
    float x, y = 0.0f;
    VideoManager->GetDrawPosition(x, y);
    MapRectangle rect = GetScreenCollisionRectangle(x, y);
    VideoManager->DrawRectangle(rect.right - rect.left, rect.bottom - rect.top, Color(0.0f, 0.0f, 1.0f, 0.6f));

    // Show a potential active path
    if(_control_event && _control_event->GetEventType() == PATH_MOVE_SPRITE_EVENT) {
        PathMoveSpriteEvent *path_event = (PathMoveSpriteEvent *)_control_event;
        if(path_event) {
            Path path = path_event->GetPath();
            MapMode *map_mode = MapMode::CurrentInstance();
            for(uint32_t i = 0; i < path.size(); ++i) {
                float x_pos = map_mode->GetScreenXCoordinate(path[i].x);
                float y_pos = map_mode->GetScreenYCoordinate(path[i].y);
                VideoManager->Move(x_pos, y_pos);

                VideoManager->DrawRectangle(GRID_LENGTH / 2, GRID_LENGTH / 2, Color(0.0f, 1.0f, 1.0f, 0.6f));
            }
        }
    }
}

void MapSprite::Draw()
{
    if(!MapObject::ShouldDraw())
        return;

    if(_custom_animation_on && _current_custom_animation)
        _current_custom_animation->Draw();
    else
        _animation->at(_current_anim_direction).Draw();

    MapObject::_DrawEmote();

    if(VideoManager->DebugInfoOn())
        _DrawDebugInfo();
}

void MapSprite::DrawDialogIcon()
{
    if(!MapObject::ShouldDraw())
        return;

    // Other map sprite logical conditions preventing the bubble from being displayed
    if (!_has_available_dialogue || !_has_unseen_dialogue || _dialogue_started)
        return;

    MapMode* map_mode = MapMode::CurrentInstance();
    Color icon_color(1.0f, 1.0f, 1.0f, 0.0f);
    float icon_alpha = 1.0f - (fabs(GetXPosition() - map_mode->GetCamera()->GetXPosition())
                            + fabs(GetYPosition() - map_mode->GetCamera()->GetYPosition())) / INTERACTION_ICON_VISIBLE_RANGE;
    if (icon_alpha < 0.0f)
        icon_alpha = 0.0f;
    icon_color.SetAlpha(icon_alpha);

    VideoManager->MoveRelative(0, -GetImgPixelHeight());
    map_mode->GetDialogueIcon().Draw(icon_color);
}

void MapSprite::AddDialogueReference(SpriteDialogue* dialogue)
{
    _dialogue_references.push_back(dialogue->GetDialogueID());
    UpdateDialogueStatus();
}

void MapSprite::ClearDialogueReferences()
{
    _dialogue_references.clear();
    UpdateDialogueStatus();
}

void MapSprite::RemoveDialogueReference(SpriteDialogue* dialogue)
{
    std::string dialogue_id = dialogue->GetDialogueID();
    // Remove all dialogues with the given reference (for the case, the same dialogue was add several times)
    for(uint32_t i = 0; i < _dialogue_references.size(); i++) {
        if(_dialogue_references[i] == dialogue_id)
            _dialogue_references.erase(_dialogue_references.begin() + i);
    }
    UpdateDialogueStatus();
}

void MapSprite::InitiateDialogue()
{
    if(_dialogue_references.empty()) {
        IF_PRINT_WARNING(MAP_DEBUG) << "sprite: " << _object_id << " has no dialogue referenced" << std::endl;
        return;
    }

    SaveState();
    _moving = false;
    _dialogue_started = true;
    SetDirection(CalculateOppositeDirection(MapMode::CurrentInstance()->GetCamera()->GetDirection()));
    MapMode::CurrentInstance()->GetDialogueSupervisor()->StartDialogue(_dialogue_references[_next_dialogue]);
    IncrementNextDialogue();
}

void MapSprite::UpdateDialogueStatus()
{
    _has_available_dialogue = false;
    _has_unseen_dialogue = false;

    for(uint32_t i = 0; i < _dialogue_references.size(); i++) {
        SpriteDialogue* dialogue = MapMode::CurrentInstance()->GetDialogueSupervisor()->GetDialogue(_dialogue_references[i]);
        if(!dialogue) {
            PRINT_WARNING << "sprite: " << _object_id << " is referencing unknown dialogue: "
                          << _dialogue_references[i] << std::endl;
            continue;
        }

        // try and not take already seen dialogues.
        // So we take only the last dialogue reference even if already seen.
        if (dialogue->HasAlreadySeen() && i < _dialogue_references.size() - 1)
            continue;

        _has_available_dialogue = true;
        if(_next_dialogue < 0)
            _next_dialogue = i;

        _has_unseen_dialogue = !dialogue->HasAlreadySeen();
    }
}

void MapSprite::IncrementNextDialogue()
{
    // Handle the case where no dialogue is referenced by the sprite
    if(_next_dialogue < 0) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function invoked when no dialogues were referenced by the sprite" << std::endl;
        return;
    }

    int16_t last_dialogue = _next_dialogue;

    while(true) {
        ++_next_dialogue;
        if(static_cast<uint16_t>(_next_dialogue) >= _dialogue_references.size()) {
            --_next_dialogue;
            return;
        }

        SpriteDialogue *dialogue = MapMode::CurrentInstance()->GetDialogueSupervisor()->GetDialogue(_dialogue_references[_next_dialogue]);
        if(dialogue)
            return;

        // If this case occurs, all dialogues are now unavailable
        else if(_next_dialogue == last_dialogue) {
            _has_unseen_dialogue = false;
            _dialogue_started = false;
            return;
        }
    }
}

void MapSprite::SetNextDialogue(uint16_t next)
{
    // If a negative value is passed in, this means the user wants to disable
    if(next >= _dialogue_references.size()) {
        IF_PRINT_WARNING(MAP_DEBUG) << "tried to set _next_dialogue to an value that was invalid (exceeds maximum bounds): " << next << std::endl;
    } else {
        _next_dialogue = static_cast<int16_t>(next);
    }
}

void MapSprite::SaveState()
{
    VirtualSprite::SaveState();

    _saved_current_anim_direction = _current_anim_direction;
}

void MapSprite::RestoreState()
{
    VirtualSprite::RestoreState();

    _current_anim_direction = _saved_current_anim_direction;
}

const std::string& MapSprite::GetNextDialogueID() const
{
    if (_next_dialogue >= 0 && _next_dialogue < static_cast<int16_t>(_dialogue_references.size()))
        return _dialogue_references[_next_dialogue];
    else
        return vt_utils::_empty_string;
}

// *****************************************************************************
// ********** EnemySprite class methods
// *****************************************************************************

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
    _last_node_x_position = 0.0f;
    _last_node_x_position = 0.0f;
    _current_node_x = 0.0f;
    _current_node_y = 0.0f;
    _destination_x = 0.0f;
    _destination_y = 0.0f;
    _current_node_id = 0;
    _path.clear();
    _use_path = false;

    // Reset the currently selected way point
    _current_way_point_id = 0;
}

void EnemySprite::AddEnemy(uint32_t enemy_id, float position_x, float position_y)
{
    if(_enemy_parties.empty()) {
        IF_PRINT_WARNING(MAP_DEBUG) << "can not add new enemy when no parties have been declared" << std::endl;
        return;
    }

    BattleEnemyInfo enemy_info(enemy_id, position_x, position_y);
    _enemy_parties.back().push_back(enemy_info);
}

// Static empty enemy party used to prevent temporary reference returns.
static const std::vector<BattleEnemyInfo> empty_enemy_party;

const std::vector<BattleEnemyInfo>& EnemySprite::RetrieveRandomParty() const
{
    if(_enemy_parties.empty()) {
        PRINT_ERROR << "No enemy parties exist and none can be created." << std::endl;
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
        _time_elapsed += SystemManager->GetUpdateTime();
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
            _time_elapsed += SystemManager->GetUpdateTime();

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
    _time_elapsed += SystemManager->GetUpdateTime();

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
    if (!VideoManager->DebugInfoOn())
        return;

    float x, y = 0.0f;
    VideoManager->GetDrawPosition(x, y);
    MapRectangle rect = GetScreenCollisionRectangle(x, y);
    VideoManager->DrawRectangle(rect.right - rect.left, rect.bottom - rect.top, Color(1.0f, 0.0f, 0.0f, 0.6f));
}

void EnemySprite::AddWayPoint(float destination_x, float destination_y)
{
    MapPosition destination(destination_x, destination_y);

    // Check whether the way point is already existing
    for (uint32_t i = 0; i < _way_points.size(); ++i) {
        if (_way_points[i].x == destination_x && _way_points[i].y == destination_y) {
            PRINT_WARNING << "Way point already added: (" << destination_x << ", "
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

    bool ret = _SetDestination(_way_points[_current_way_point_id].x, _way_points[_current_way_point_id].y, 0);
    ++_current_way_point_id;

    return ret;
}

void EnemySprite::_UpdatePath()
{
    if(!_use_path || _path.empty())
        return;

    float sprite_position_x = GetXPosition();
    float sprite_position_y = GetYPosition();
    float distance_moved = CalculateDistanceMoved();

    // Check whether the sprite has arrived at the position of the current node
    if(vt_utils::IsFloatEqual(sprite_position_x, _current_node_x, distance_moved)
            && vt_utils::IsFloatEqual(sprite_position_y, _current_node_y, distance_moved)) {
        ++_current_node_id;

        if(_current_node_id < _path.size()) {
            _current_node_x = _path[_current_node_id].x;
            _current_node_y = _path[_current_node_id].y;
        }
    }
    // If the sprite has moved to a new position other than the next node, adjust its direction so it is trying to move to the next node
    else if((sprite_position_x != _last_node_x_position) || (sprite_position_y != _last_node_y_position)) {
        _last_node_x_position = sprite_position_x;
        _last_node_y_position = sprite_position_y;
    }

    _SetSpritePathDirection();

    // End the path event
    if(vt_utils::IsFloatEqual(sprite_position_x, _destination_x, distance_moved)
            && vt_utils::IsFloatEqual(sprite_position_y, _destination_y, distance_moved)) {
        _path.clear();
    }
}

bool EnemySprite::_SetDestination(float destination_x, float destination_y, uint32_t max_cost)
{
    _path.clear();
    _use_path = false;

    uint32_t dest_x = (uint32_t) destination_x;
    uint32_t dest_y = (uint32_t) destination_y;
    uint32_t pos_x = (uint32_t) GetXPosition();
    uint32_t pos_y = (uint32_t) GetYPosition();

    // Don't check the path if the sprite is there.
    if (pos_x == dest_x && pos_y == dest_y)
        return false;

    MapPosition dest(destination_x, destination_y);
    // We set the correct mask before finding the path
    _collision_mask = WALL_COLLISION | CHARACTER_COLLISION;
    _path = MapMode::CurrentInstance()->GetObjectSupervisor()->FindPath(this, dest, max_cost);

    if (_path.empty())
        return false;

    // But remove wall collision afterward to avoid making it stuck in corners.
    // Note: this function is only called when hostile, son we don't deal with
    // the spawning collision mask.
    _collision_mask = CHARACTER_COLLISION;

    _current_node_id = 0;
    _last_node_x_position = GetXPosition();
    _last_node_y_position = GetYPosition();
    _destination_x = destination_x;
    _destination_y = destination_y;

    _current_node_x = _path[_current_node_id].x;
    _current_node_y = _path[_current_node_id].y;

    _moving = true;
    _use_path = true;
    return true;
}

void EnemySprite::_SetSpritePathDirection()
{
    if (!_use_path || _path.empty())
        return;

    uint16_t direction = 0;

    float sprite_position_x = GetXPosition();
    float sprite_position_y = GetYPosition();
    float distance_moved = CalculateDistanceMoved();

    if(sprite_position_y - _current_node_y > distance_moved) {
        direction |= NORTH;
    } else if(sprite_position_y - _current_node_y < -distance_moved) {
        direction |= SOUTH;
    }

    if(sprite_position_x - _current_node_x > distance_moved) {
        direction |= WEST;
    } else if(sprite_position_x - _current_node_x < -distance_moved) {
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
