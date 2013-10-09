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
static uint16 CalculateOppositeDirection(const uint16 direction)
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

VirtualSprite::VirtualSprite() :
    direction(SOUTH),
    movement_speed(NORMAL_SPEED),
    moving(false),
    moved_position(false),
    is_running(false),
    control_event(NULL),
    _state_saved(false),
    _saved_direction(0),
    _saved_movement_speed(0.0f),
    _saved_moving(false)
{
    MapObject::_object_type = VIRTUAL_TYPE;
}

VirtualSprite::~VirtualSprite()
{}

void VirtualSprite::Update()
{
    moved_position = false;

    // Update potential emote animation
    MapObject::_UpdateEmote();

    if(!updatable || !moving)
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

    ObjectSupervisor *object_supervisor = MapMode::CurrentInstance()->GetObjectSupervisor();

    if(direction & NORTH) {
        // Test both the north-east and north west cases
        if(!object_supervisor->IsStaticCollision(position.x + coll_half_width,
                                                 position.y - coll_height - distance_moved)) {
            edge_next_pos_x = position.x + distance_moved;
            edge_next_pos_y = position.y;
            on_edge = true;
        }
        else if (!object_supervisor->IsStaticCollision(position.x - coll_half_width,
                                                       position.y - coll_height - distance_moved)) {
            edge_next_pos_x = position.x - distance_moved;
            edge_next_pos_y = position.y;
            on_edge = true;
        }
    }
    else if(direction & SOUTH) {
        // Test both the south-east and south west cases
        if(!object_supervisor->IsStaticCollision(position.x + coll_half_width,
                                                 position.y + distance_moved)) {
            edge_next_pos_x = position.x + distance_moved;
            edge_next_pos_y = position.y;
            on_edge = true;
        }
        else if (!object_supervisor->IsStaticCollision(position.x - coll_half_width,
                                                       position.y + distance_moved)) {
            edge_next_pos_x = position.x - distance_moved;
            edge_next_pos_y = position.y;
            on_edge = true;
        }
    }
    else if(direction & EAST) {
        // Test both the north-east and south-east cases
        if(!object_supervisor->IsStaticCollision(position.x + coll_half_width + distance_moved,
                                                 position.y - coll_height)) {
            edge_next_pos_x = position.x;
            edge_next_pos_y = position.y - distance_moved;
            on_edge = true;
        }
        else if (!object_supervisor->IsStaticCollision(position.x + coll_half_width + distance_moved,
                                                       position.y)) {
            edge_next_pos_x = position.x;
            edge_next_pos_y = position.y + distance_moved;
            on_edge = true;
        }
    }
    else if(direction & WEST) {
        // Test both the north-west and south-west cases
        if(!object_supervisor->IsStaticCollision(position.x - coll_half_width - distance_moved,
                                                 position.y - coll_height)) {
            edge_next_pos_x = position.x;
            edge_next_pos_y = position.y - distance_moved;
            on_edge = true;
        }
        else if (!object_supervisor->IsStaticCollision(position.x - coll_half_width - distance_moved,
                                                       position.y)) {
            edge_next_pos_x = position.x;
            edge_next_pos_y = position.y + distance_moved;
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
    if(direction & (NORTH | MOVING_NORTHWEST | MOVING_NORTHEAST))
        next_pos_y -= distance_moved;
    else if(direction & (SOUTH | MOVING_SOUTHWEST | MOVING_SOUTHEAST))
        next_pos_y += distance_moved;
    if(direction & (WEST | MOVING_NORTHWEST | MOVING_SOUTHWEST))
        next_pos_x -= distance_moved;
    else if(direction & (EAST | MOVING_NORTHEAST | MOVING_SOUTHEAST))
        next_pos_x += distance_moved;

    // When not moving, do not check anything else.
    if(next_pos_x == GetXPosition() && next_pos_y == GetYPosition())
        return;

    // We've got the next position, let's check whether the next position
    // should be revised.

    // Used to know whether we could fall back to a straight move
    // in case of collision.
    bool moving_diagonally = (direction & (MOVING_NORTHWEST | MOVING_NORTHEAST
                                           | MOVING_SOUTHEAST | MOVING_SOUTHWEST));

    // Handle collision with the first object encountered
    MapObject *collision_object = NULL;
    MapMode *map = MapMode::CurrentInstance();
    ObjectSupervisor *object_supervisor = map->GetObjectSupervisor();
    COLLISION_TYPE collision_type = object_supervisor->DetectCollision(this, next_pos_x,
                     next_pos_y,
                     &collision_object);
    // Try to fall back to straight direction
    if(moving_diagonally && collision_type != NO_COLLISION) {
        // Try on x axis
        if(object_supervisor->DetectCollision(this, position.x, next_pos_y, &collision_object) == NO_COLLISION) {
            next_pos_x = position.x;
            collision_type = NO_COLLISION;
        } // and then on y axis
        else if(object_supervisor->DetectCollision(this, next_pos_x, position.y, &collision_object) == NO_COLLISION) {
            next_pos_y = position.y;
            collision_type = NO_COLLISION;
        }
    }

    // Handles special collision handling first
    if(control_event) {
        switch(control_event->GetEventType()) {
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
        if (collision_object && this == map->GetCamera()) {
            PhysicalObject *phs = reinterpret_cast<PhysicalObject *>(collision_object);
            if(phs && !phs->GetEventIdWhenTalking().empty())
                return;
        }

        // Fix the direction and destination to walk-around obstacles
        if (_HandleWallEdges(next_pos_x, next_pos_y, distance_moved, collision_object))
            break;
        // We don't do any other checks for the player sprite.
        else if (this == map->GetCamera())
            return;

        // NPC sprites:

        // When it's a true wall, try against the collision grid
        if(!collision_object) {
            // Try a random diagonal to avoid the wall in straight direction
            if(direction & (NORTH | SOUTH))
                direction |= RandomBoundedInteger(0, 1) ? EAST : WEST;
            else if(direction & (EAST | WEST))
                direction |= RandomBoundedInteger(0, 1) ? NORTH : SOUTH;
            return;
        }
        // Physical and treasure objects are the only other matching "fake" walls
        else {
            // Try a diagonal to avoid the sprite in straight direction by comparing
            // each one coords.
            float diff_x = GetXPosition() - collision_object->GetXPosition();
            float diff_y = GetYPosition() - collision_object->GetYPosition();
            if(direction & (NORTH | SOUTH))
                direction |= diff_x >= 0.0f ? EAST : WEST;
            else if(direction & (EAST | WEST))
                direction |= diff_y >= 0.0f ? SOUTH : NORTH;
            return;
        }
        // Other cases shouldn't happen.
        break;
    case ENEMY_COLLISION:
        // Check only whether the player has collided with a monster
        if(this == map->GetCamera() &&
                collision_object && collision_object->GetObjectType() == ENEMY_TYPE) {
            EnemySprite *enemy = reinterpret_cast<EnemySprite *>(collision_object);

            if(enemy && enemy->IsHostile() && map->AttackAllowed()) {
                 // Check whether the player is actually playing. If not, we don't want to start a battle.
                 if (MapMode::CurrentInstance()->CurrentState() == STATE_EXPLORE)
                     _StartBattleEncounter(enemy);

                 return;
            }
        }

        break;
    case CHARACTER_COLLISION:
        // Check whether the sprite is tangled with another character, even without moving
        // For instance, when colliding with a path follower npc.
        // And let it through in that case.
        if(object_supervisor->CheckObjectCollision(GetCollisionRectangle(), collision_object)) {
            collision_type = NO_COLLISION;
            break;
        }

        // When the sprite is controlled by the camera, let the player handle the position correction.
        if(this == map->GetCamera())
            return;

        // Check whether an enemy has collided with the player
        if(this->GetType() == ENEMY_TYPE && collision_object == map->GetCamera()) {
            EnemySprite *enemy = reinterpret_cast<EnemySprite *>(this);

            if(enemy && enemy->IsHostile() && map->AttackAllowed()) {
                 // Check whether the player is actually playing. If not, we don't want to start a battle.
                 if (MapMode::CurrentInstance()->CurrentState() == STATE_EXPLORE)
                     _StartBattleEncounter(enemy);

                 return;
            }
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
        if(direction & (NORTH | SOUTH))
            direction |= diff_x >= 0.0f ? EAST : WEST;
        else if(direction & (EAST | WEST))
            direction |= diff_y >= 0.0f ? SOUTH : NORTH;
        return;
    }

    // Inform the overlay system of the parallax movement done if needed
    if(this == map->GetCamera()) {
        float x_parallax = !map->IsCameraXAxisInMapCorner() ?
                           (GetXPosition() - next_pos_x) / SCREEN_GRID_X_LENGTH * VIDEO_STANDARD_RES_WIDTH :
                           0.0f;
        float y_parallax = !map->IsCameraYAxisInMapCorner() ?
                           (next_pos_y - GetYPosition()) / SCREEN_GRID_Y_LENGTH * VIDEO_STANDARD_RES_HEIGHT :
                           0.0f;

        map->GetEffectSupervisor().AddParallax(x_parallax, y_parallax);
    }

    // Make the sprite advance at the end
    SetPosition(next_pos_x, next_pos_y);
    moved_position = true;
}


void VirtualSprite::SetDirection(uint16 dir)
{
    // Nothing complicated needed for lateral directions
    if(dir & (NORTH | SOUTH | EAST | WEST)) {
        direction = dir;
    }
    // Otherwise if the direction is diagonal we must figure out which way the sprite should face.
    else if(dir & MOVING_NORTHWEST) {
        if(direction & (FACING_NORTH | FACING_EAST))
            direction = NW_NORTH;
        else
            direction = NW_WEST;
    } else if(dir & MOVING_SOUTHWEST) {
        if(direction & (FACING_SOUTH | FACING_EAST))
            direction = SW_SOUTH;
        else
            direction = SW_WEST;
    } else if(dir & MOVING_NORTHEAST) {
        if(direction & (FACING_NORTH | FACING_WEST))
            direction = NE_NORTH;
        else
            direction = NE_EAST;
    } else if(dir & MOVING_SOUTHEAST) {
        if(direction & (FACING_SOUTH | FACING_WEST))
            direction = SE_SOUTH;
        else
            direction = SE_EAST;
    } else {
        IF_PRINT_WARNING(MAP_DEBUG) << "attempted to set an invalid direction: " << dir << std::endl;
    }
} // void VirtualSprite::SetDirection(uint16 dir)



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
    if(position.x == pos.x && position.y == pos.y)
        return;

    // First handle simple cases
    if(IsFloatEqual(position.x, pos.x, 0.5f)) {
        if(position.y > pos.y)
            SetDirection(NORTH);
        else
            SetDirection(SOUTH);
        return;
    }

    if(IsFloatEqual(position.y, pos.y, 0.5f)) {
        if(position.x > pos.x)
            SetDirection(WEST);
        else
            SetDirection(EAST);
        return;
    }

    // Now let's handle diagonal cases
    // First, find the lower angle:
    if(position.x < pos.x) {
        // Up-right direction
        if(position.y > pos.y) {
            // Compute tan of the angle
            if((position.y - pos.y) / (pos.x - position.x) < 1)
                // The angle is less than 45°, look to the right
                SetDirection(EAST);
            else
                SetDirection(NORTH);
            return;
        } else { // Down-right
            // Compute tan of the angle
            if((pos.y - position.y) / (pos.x - position.x) < 1)
                // The angle is less than 45°, look to the right
                SetDirection(EAST);
            else
                SetDirection(SOUTH);
            return;
        }
    } else {
        // Up-left direction
        if(position.y > pos.y) {
            // Compute tan of the angle
            if((position.y - pos.y) / (position.x - pos.x) < 1)
                // The angle is less than 45°, look to the left
                SetDirection(WEST);
            else
                SetDirection(NORTH);
            return;
        } else { // Down-left
            // Compute tan of the angle
            if((pos.y - position.y) / (position.x - pos.x) < 1)
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
    float distance_moved = static_cast<float>(SystemManager->GetUpdateTime()) / movement_speed;

    // Double the distance to move if the sprite is running
    if(is_running == true)
        distance_moved *= 2.0f;
    // If the movement is diagonal, decrease the lateral movement distance by sin(45 degress)
    if(direction & MOVING_DIAGONALLY)
        distance_moved *= 0.707f;

    // We cap the distance moved when in case of low FPS to avoid letting certain
    // sprites jump across blocking areas.
    if (distance_moved > 1.0f)
        distance_moved = 1.0f;

    return distance_moved;
}



void VirtualSprite::AcquireControl(SpriteEvent *event)
{
    if(event == NULL) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function argument was NULL" << std::endl;
        return;
    }

    if(control_event != NULL) {
        IF_PRINT_WARNING(MAP_DEBUG) << "a new event is acquiring control when the previous event has not "
                                    "released control over this sprite, object id: " << GetObjectID() << std::endl;
    }
    control_event = event;
}



void VirtualSprite::ReleaseControl(SpriteEvent *event)
{
    if(event == NULL) {
        IF_PRINT_WARNING(MAP_DEBUG) << "function argument was NULL" << std::endl;
        return;
    }

    if(control_event == NULL) {
        IF_PRINT_WARNING(MAP_DEBUG) << "no event had control over this sprite, object id: " << GetObjectID() << std::endl;
    } else if(control_event != event) {
        IF_PRINT_WARNING(MAP_DEBUG) << "a different event has control of this sprite, object id: " << GetObjectID() << std::endl;
    } else {
        control_event = NULL;
    }
}



void VirtualSprite::SaveState()
{
    _state_saved = true;
    _saved_direction = direction;
    _saved_movement_speed = movement_speed;
    _saved_moving = moving;
    MapMode::CurrentInstance()->GetEventSupervisor()->PauseAllEvents(this);
}



void VirtualSprite::RestoreState()
{
    if(_state_saved == false)
        IF_PRINT_WARNING(MAP_DEBUG) << "restoring state when no saved state was detected" << std::endl;

    _state_saved = false;
    direction = _saved_direction;
    movement_speed = _saved_movement_speed;
    moving = _saved_moving;
    MapMode::CurrentInstance()->GetEventSupervisor()->ResumeAllEvents(this);
}


void VirtualSprite::_StartBattleEncounter(EnemySprite *enemy)
{

    // Start a map-to-battle transition animation sequence
    BattleMode *BM = new BattleMode();

    std::string battle_background = enemy->GetBattleBackground();
    if(!battle_background.empty())
        BM->GetMedia().SetBackgroundImage(battle_background);

    std::string enemy_battle_music = enemy->GetBattleMusicTheme();
    if(!enemy_battle_music.empty())
        BM->GetMedia().SetBattleMusic(enemy_battle_music);

    const std::vector<BattleEnemyInfo>& enemy_party = enemy->RetrieveRandomParty();
    for(uint32 i = 0; i < enemy_party.size(); ++i) {
        BM->AddEnemy(enemy_party[i].enemy_id,
                     enemy_party[i].position_x,
                     enemy_party[i].position_y);
    }

    std::vector<std::string> enemy_battle_scripts = enemy->GetBattleScripts();
    if(!enemy_battle_scripts.empty())
        BM->GetScriptSupervisor().SetScripts(enemy_battle_scripts);

    TransitionToBattleMode *TM = new TransitionToBattleMode(BM, enemy->IsBoss());

    // Indicates to the potential enemy zone that this spawn is dead.
    EnemyZone *zone = enemy->GetEnemyZone();
    if (zone)
        zone->DecreaseSpawnsLeft();

    // Make all enemy sprites disappear after creating the transition mode so that the player
    // can't be cornerned and forced into multiple battles in succession.
    MapMode::CurrentInstance()->GetObjectSupervisor()->SetAllEnemyStatesToDead();

    ModeManager->Push(TM);
}

// ****************************************************************************
// ********** MapSprite class methods
// ****************************************************************************

MapSprite::MapSprite() :
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
    MapObject::_object_type = SPRITE_TYPE;

    // Points the current animation to the standing animation vector by default
    _animation = &_standing_animations;
}

MapSprite::~MapSprite()
{
    if(_face_portrait)
        delete _face_portrait;
}

bool _LoadAnimations(std::vector<vt_video::AnimatedImage>& animations, const std::string &filename)
{
    // Prepare to add the animations for each directions, if needed.

    // In case of reloading
    animations.clear();
    for(uint8 i = 0; i < NUM_ANIM_DIRECTIONS; ++i)
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
    uint32 rows = animations_script.ReadUInt("rows");
    uint32 columns = animations_script.ReadUInt("columns");

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

    std::vector <uint32> frames_directions_ids;
    animations_script.ReadTableKeys("frames", frames_directions_ids);

    // open the frames table
    animations_script.OpenTable("frames");

    for(uint32 i = 0; i < frames_directions_ids.size(); ++i) {
        if(frames_directions_ids[i] >= NUM_ANIM_DIRECTIONS) {
            PRINT_WARNING << "Invalid direction id(" << frames_directions_ids[i]
                          << ") in file: " << filename << std::endl;
            continue;
        }

        uint32 anim_direction = frames_directions_ids[i];

        // Opens frames[ANIM_DIRECTION]
        animations_script.OpenTable(anim_direction);

        // Loads the frames data
        std::vector<uint32> frames_ids;
        std::vector<uint32> frames_duration;

        uint32 num_frames = animations_script.GetTableSize();
        for(uint32 frames_table_id = 0;  frames_table_id < num_frames; ++frames_table_id) {
            // Opens frames[ANIM_DIRECTION][frame_table_id]
            animations_script.OpenTable(frames_table_id);

            int32 frame_id = animations_script.ReadInt("id");
            int32 frame_duration = animations_script.ReadInt("duration");

            if(frame_id < 0 || frame_duration < 0 || frame_id >= (int32)image_frames.size()) {
                PRINT_WARNING << "Invalid frame (" << frames_table_id << ") in file: "
                              << filename << std::endl;
                PRINT_WARNING << "Request for frame id: " << frame_id << ", duration: "
                              << frame_duration << " is not possible." << std::endl;
                continue;
            }

            frames_ids.push_back((uint32)frame_id);
            frames_duration.push_back((uint32)frame_duration);

            animations_script.CloseTable(); // frames[ANIM_DIRECTION][frame_table_id] table
        }

        // Actually create the animation data
        animations[anim_direction].Clear();
        animations[anim_direction].ResetAnimation();
        for(uint32 j = 0; j < frames_ids.size(); ++j) {
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
    for(uint8 i = 0; i < NUM_ANIM_DIRECTIONS; ++i) {
        if(frame_width > 0.0f && frame_height > 0.0f) {
            animations[i].SetDimensions(frame_width, frame_height);
        } else if(IsFloatEqual(animations[i].GetWidth(), 0.0f) && IsFloatEqual(animations[i].GetHeight(), 0.0f)) {
            // If the animation dimensions are not set, we're using the first frame size.
            animations[i].SetDimensions(image_frames.begin()->GetWidth(), image_frames.begin()->GetHeight());
        }

        // Rescale to fit the map mode coordinates system.
        MapMode::ScaleToMapCoords(animations[i]);
    }

    animations_script.CloseTable(); // sprite_animation table
    animations_script.CloseFile();

    return true;
} // bool _LoadAnimations()

void MapSprite::ClearAnimations()
{
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
        MapMode::ScaleToMapCoords(animation);
        _custom_animations.insert(std::make_pair(animation_name, animation));
        return true;
    }

    return false;
} // bool MapSprite::LoadCustomAnimations()

void MapSprite::SetCustomAnimation(const std::string &animation_name, int32 time)
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
    SetCollHalfWidth(script.ReadFloat("coll_half_width"));
    SetCollHeight(script.ReadFloat("coll_height"));
    SetImgHalfWidth(script.ReadFloat("img_half_width"));
    SetImgHeight(script.ReadFloat("img_height"));
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

    for (uint32 i = 0; i < anim_names.size(); ++i)
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

void MapSprite::Update()
{
    // Stores the last value of moved_position to determine when a change in sprite movement between calls to this function occurs
    static bool was_moved = moved_position;

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

        was_moved = moved_position;
        return;
    }

    // Save the previous animation state
    uint8 last_anim_direction = _current_anim_direction;
    std::vector<AnimatedImage>* last_animation = _animation;

    // Set the sprite's animation to the standing still position if movement has just stopped
    if(!moved_position) {
        // Set the current movement animation to zero progress
        if(was_moved)
            _animation->at(_current_anim_direction).ResetAnimation();

        // Determine the correct standing frame to display
        if(!control_event || _state_saved) {
            _animation = &_standing_animations;

            if(direction & FACING_NORTH) {
                _current_anim_direction = ANIM_NORTH;
            } else if(direction & FACING_SOUTH) {
                _current_anim_direction = ANIM_SOUTH;
            } else if(direction & FACING_WEST) {
                _current_anim_direction = ANIM_WEST;
            } else if(direction & FACING_EAST) {
                _current_anim_direction = ANIM_EAST;
            } else {
                PRINT_ERROR << "invalid sprite direction, could not find proper standing animation to draw" << std::endl;
            }
        }
    }

    else { // then (moved_position)
        // Determine the correct animation to display
        if(direction & FACING_NORTH) {
            _current_anim_direction = ANIM_NORTH;
        } else if(direction & FACING_SOUTH) {
            _current_anim_direction = ANIM_SOUTH;
        } else if(direction & FACING_WEST) {
            _current_anim_direction = ANIM_WEST;
        } else if(direction & FACING_EAST) {
            _current_anim_direction = ANIM_EAST;
        } else {
            PRINT_ERROR << "invalid sprite direction, could not find proper standing animation to draw" << std::endl;
        }

        // Increasing the animation index by four from the walking _animations leads to the running _animations
        if(is_running && _has_running_animations) {
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
    uint32 elapsed_time = 0;
    if(_animation == &_walking_animations || (_has_running_animations && _animation == &_running_animations)) {
        elapsed_time = (uint32)(((float)vt_system::SystemManager->GetUpdateTime()) * NORMAL_SPEED / movement_speed);
    }

    _animation->at(_current_anim_direction).Update(elapsed_time);

    was_moved = moved_position;
} // void MapSprite::Update()

void MapSprite::_DrawDebugInfo()
{
    // Draw collision rectangle if the debug view is on.
    float x, y = 0.0f;
    VideoManager->GetDrawPosition(x, y);
    MapRectangle rect = GetCollisionRectangle(x, y);
    VideoManager->DrawRectangle(rect.right - rect.left, rect.bottom - rect.top, Color(0.0f, 0.0f, 1.0f, 0.6f));

    // Show a potential active path
    if(control_event && control_event->GetEventType() == PATH_MOVE_SPRITE_EVENT) {
        PathMoveSpriteEvent *path_event = (PathMoveSpriteEvent *)control_event;
        if(path_event) {
            Path path = path_event->GetPath();
            MapMode *map = MapMode::CurrentInstance();
            for(uint32 i = 0; i < path.size(); ++i) {
                float x_pos = path[i].x - map->GetMapFrame().screen_edges.left;
                float y_pos = path[i].y - map->GetMapFrame().screen_edges.top;
                VideoManager->Move(x_pos, y_pos);

                VideoManager->DrawRectangle(0.2, 0.2f, Color(0.0f, 1.0f, 1.0f, 0.6f));
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

void MapSprite::DrawDialog()
{
    // Update the alpha of the dialogue icon according to it's distance from the player sprite
    const float DIALOGUE_ICON_VISIBLE_RANGE = 10.0f;

    if(!MapObject::ShouldDraw())
        return;

    MapMode *map_mode = MapMode::CurrentInstance();
    // Don't show a dialogue bubble when not in exploration mode.
    if (map_mode->CurrentState() != STATE_EXPLORE)
        return;

    // Other logical conditions preventing the bubble from being displayed
    if (!_has_available_dialogue || !_has_unseen_dialogue || _dialogue_started
            || !map_mode->IsShowGUI() || map_mode->IsCameraOnVirtualFocus())
        return;

    Color icon_color(1.0f, 1.0f, 1.0f, 0.0f);
    float icon_alpha = 1.0f - (fabs(GetXPosition() - map_mode->GetCamera()->GetXPosition())
                               + fabs(GetYPosition() - map_mode->GetCamera()->GetYPosition())) / DIALOGUE_ICON_VISIBLE_RANGE;

    if(icon_alpha < 0.0f)
        icon_alpha = 0.0f;
    icon_color.SetAlpha(icon_alpha);

    VideoManager->MoveRelative(0, -GetImgHeight());
    map_mode->GetDialogueIcon().Draw(icon_color);
}

void MapSprite::AddDialogueReference(uint32 dialogue_id)
{
    _dialogue_references.push_back(dialogue_id);
    UpdateDialogueStatus();
}

void MapSprite::ClearDialogueReferences()
{
    _dialogue_references.clear();
    UpdateDialogueStatus();
}

void MapSprite::RemoveDialogueReference(uint32 dialogue_id)
{
    // Remove all dialogues with the given reference (for the case, the same dialogue was add several times)
    for(uint32 i = 0; i < _dialogue_references.size(); i++) {
        if(_dialogue_references[i] == dialogue_id)
            _dialogue_references.erase(_dialogue_references.begin() + i);
    }
    UpdateDialogueStatus();
}

void MapSprite::InitiateDialogue()
{
    if(_dialogue_references.empty()) {
        IF_PRINT_WARNING(MAP_DEBUG) << "sprite: " << object_id << " has no dialogue referenced" << std::endl;
        return;
    }

    SaveState();
    moving = false;
    _dialogue_started = true;
    SetDirection(CalculateOppositeDirection(MapMode::CurrentInstance()->GetCamera()->GetDirection()));
    MapMode::CurrentInstance()->GetDialogueSupervisor()->BeginDialogue(_dialogue_references[_next_dialogue]);
    IncrementNextDialogue();
}

void MapSprite::UpdateDialogueStatus()
{
    _has_available_dialogue = false;
    _has_unseen_dialogue = false;

    for(uint32 i = 0; i < _dialogue_references.size(); i++) {
        SpriteDialogue* dialogue = MapMode::CurrentInstance()->GetDialogueSupervisor()->GetDialogue(_dialogue_references[i]);
        if(!dialogue) {
            PRINT_WARNING << "sprite: " << object_id << " is referencing unknown dialogue: "
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

    int16 last_dialogue = _next_dialogue;

    while(true) {
        ++_next_dialogue;
        if(static_cast<uint16>(_next_dialogue) >= _dialogue_references.size()) {
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

void MapSprite::SetNextDialogue(uint16 next)
{
    // If a negative value is passed in, this means the user wants to disable
    if(next >= _dialogue_references.size()) {
        IF_PRINT_WARNING(MAP_DEBUG) << "tried to set _next_dialogue to an value that was invalid (exceeds maximum bounds): " << next << std::endl;
    } else {
        _next_dialogue = static_cast<int16>(next);
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

// *****************************************************************************
// ********** EnemySprite class methods
// *****************************************************************************

EnemySprite::EnemySprite() :
    _zone(NULL),
    _color(1.0f, 1.0f, 1.0f, 0.0f),
    _aggro_range(8.0f),
    _time_before_new_destination(1200),
    _time_to_spawn(STANDARD_ENEMY_FIRST_SPAWN_TIME),
    _time_to_respawn(STANDARD_ENEMY_SPAWN_TIME),
    _out_of_zone(false),
    _is_boss(false),
    _use_path(false)
{
    MapObject::_object_type = ENEMY_TYPE;
    moving = false;
    Reset();
}

void EnemySprite::Reset()
{
    updatable = false;
    collision_mask = NO_COLLISION;
    _state = DEAD;
    _time_elapsed = 0;
    _color.SetAlpha(0.0f);
    _out_of_zone = false;

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

void EnemySprite::AddEnemy(uint32 enemy_id, float position_x, float position_y)
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
    updatable = true;
    _state = HOSTILE;
    collision_mask = WALL_COLLISION | CHARACTER_COLLISION;
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
    case HOSTILE: {
        // Holds the x and y deltas between the sprite and map camera coordinate pairs
        VirtualSprite *camera = MapMode::CurrentInstance()->GetCamera();
        float camera_x = camera->GetXPosition();
        float camera_y = camera->GetYPosition();

        float xdelta = GetXPosition() - camera_x;
        float ydelta = GetYPosition() - camera_y;
        float abs_xdelta = fabs(xdelta);
        float abs_ydelta = fabs(ydelta);

        // Don't update enemies that are too far away...
        if (abs_xdelta > SCREEN_GRID_X_LENGTH || abs_ydelta > SCREEN_GRID_Y_LENGTH)
            return;

        // Update the wait time until next path
        if (!_use_path || !moving)
            _time_elapsed += SystemManager->GetUpdateTime();

        // Test whether the monster has spotted its target.
        bool player_in_aggro_range = false;
        if(abs_xdelta <= _aggro_range && abs_ydelta <= _aggro_range)
            player_in_aggro_range = true;

        // check whether the monster has the right to get out of the roaming zone
        bool can_get_out_of_zone = false;
        if(player_in_aggro_range && !_zone->IsAgressionRestrainedtoRoamingZone())
            can_get_out_of_zone = true;
        else if(!player_in_aggro_range && !_zone->IsRoamingRestrained())
            can_get_out_of_zone = true;

        // If the sprite has moved outside of its zone and it should not, Set the way back home
        if(_zone && !_zone->IsInsideZone(GetXPosition(), GetYPosition())
                && !can_get_out_of_zone) {
            if (_path.empty() && _time_elapsed >= _time_before_new_destination) {
                if (!_SetPathToNextWayPoint() && !_out_of_zone) {
                    // Fall back to simple movement mode.
                    // N.B.: This isn't perfect but it would involve pathfinding otherwise
                    SetDirection(CalculateOppositeDirection(GetDirection()));
                    moving = true;

                    // The sprite is now finding its way back into the zone
                    _out_of_zone = true;
                    _time_elapsed = 0;
                }
                else if (_time_elapsed >= _time_before_new_destination) {
                    // If still returning to home, we enable a random directions again
                    // to prevent blocking the enemy sprite in a corner.
                    if (!_SetPathToNextWayPoint()) {
                        // Fall back to simple movement mode
                        SetRandomDirection();
                        moving = true;
                        _time_elapsed = 0;
                    }
                }
            }
        }
        // Otherwise, determine the direction that the sprite should move if the camera is within the sprite's aggression range
        else {
            _out_of_zone = false;

            // Enemies will only get aggressive if the camera is inside the zone, or the zone is non-restrictive
            // The order of comparisons here is important,
            // the NULL check MUST come before the rest or a null pointer exception could happen if no zone is registered
            if(MapMode::CurrentInstance()->AttackAllowed()
                    && (_zone == NULL || (can_get_out_of_zone || _zone->IsInsideZone(camera_x, camera_y)))) {
                if (!_path.empty()) {
                    // We cancel any previous path
                    _path.clear();
                    // We set the correct mask before moving normally
                    collision_mask = WALL_COLLISION | CHARACTER_COLLISION;
                    _use_path = false;
                }

                // We set the direction to the character's position if it's not the case
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
                moving = true;

            }
            // If the sprite is not within the aggression range, pick a random destination to move
            // If there is no path left, and the time to set a new destination has passed,
            // we set a new random destination.
            else if (_path.empty() && _time_elapsed >= _time_before_new_destination) {
                if (!_SetPathToNextWayPoint()) {
                    // Fall back to simple movement mode
                    SetRandomDirection();
                    moving = true;
                }
                _time_elapsed = 0;
            }
        }

        if (_use_path && _path.empty()) {
            // The sprite is waiting for the next destination.
            moving = false;
            // We then reset the correct walk mask
            collision_mask = WALL_COLLISION | CHARACTER_COLLISION;
        }

        MapSprite::Update();
        _UpdatePath();
        break;
    }
    // Do nothing if the sprite is in the DEAD state, or any other state
    case DEAD:
    default:
        break;
    }
} // void EnemySprite::Update()

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
    MapRectangle rect = GetCollisionRectangle(x, y);
    VideoManager->DrawRectangle(rect.right - rect.left, rect.bottom - rect.top, Color(1.0f, 0.0f, 0.0f, 0.6f));
}

void EnemySprite::AddWayPoint(float destination_x, float destination_y)
{
    MapPosition destination(destination_x, destination_y);

    // Check whether the way point is already existing
    for (uint32 i = 0; i < _way_points.size(); ++i) {
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

bool EnemySprite::_SetDestination(float destination_x, float destination_y, uint32 max_cost)
{
    _path.clear();
    _use_path = false;

    uint32 dest_x = (uint32) destination_x;
    uint32 dest_y = (uint32) destination_y;
    uint32 pos_x = (uint32) GetXPosition();
    uint32 pos_y = (uint32) GetYPosition();

    // Don't check the path if the sprite is there.
    if (pos_x == dest_x && pos_y == dest_y)
        return false;

    MapPosition dest(destination_x, destination_y);
    // We set the correct mask before finding the path
    collision_mask = WALL_COLLISION | CHARACTER_COLLISION;
    _path = MapMode::CurrentInstance()->GetObjectSupervisor()->FindPath(this, dest, max_cost);

    if (_path.empty())
        return false;

    // But remove wall collision afterward to avoid making it stuck in corners.
    // Note: this function is only called when hostile, son we don't deal with
    // the spawning collision mask.
    collision_mask = CHARACTER_COLLISION;

    _current_node_id = 0;
    _last_node_x_position = GetXPosition();
    _last_node_y_position = GetYPosition();
    _destination_x = destination_x;
    _destination_y = destination_y;

    _current_node_x = _path[_current_node_id].x;
    _current_node_y = _path[_current_node_id].y;

    moving = true;
    _use_path = true;
    return true;
}

void EnemySprite::_SetSpritePathDirection()
{
    if (!_use_path || _path.empty())
        return;

    uint16 direction = 0;

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
