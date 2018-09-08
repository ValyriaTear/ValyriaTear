///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_object_supervisor.h"

#include "modes/map/map_objects/map_object.h"
#include "modes/map/map_objects/map_physical_object.h"
#include "modes/map/map_objects/map_halo.h"
#include "modes/map/map_objects/map_light.h"
#include "modes/map/map_objects/map_save_point.h"
#include "modes/map/map_objects/map_escape_point.h"
#include "modes/map/map_objects/map_sound.h"
#include "modes/map/map_objects/map_treasure.h"

#include "modes/map/map_sprites/map_enemy_sprite.h"
#include "modes/map/map_zones.h"

#include "common/global/global.h"
#include "common/global/actors/global_character.h"

#include "utils/utils_numeric.h"

using namespace vt_common;

namespace vt_map
{

namespace private_map
{

ObjectSupervisor::ObjectSupervisor() :
    _num_grid_x_axis(0),
    _num_grid_y_axis(0),
    _last_id(1), //! Every object Id must be > 0 since 0 is reserved for speakerless dialogues.
    _visible_party_member(nullptr)
{}

ObjectSupervisor::~ObjectSupervisor()
{
    // Delete all the map objects
    for(uint32_t i = 0; i < _all_objects.size(); ++i) {
        delete(_all_objects[i]);
    }

    for(uint32_t i = 0; i < _zones.size(); ++i) {
        delete(_zones[i]);
    }
}

MapObject* ObjectSupervisor::GetObject(uint32_t object_id)
{
    if(object_id >= _all_objects.size())
        return nullptr;
    else
        return _all_objects[object_id];
}

VirtualSprite* ObjectSupervisor::GetSprite(uint32_t object_id)
{
    MapObject* object = GetObject(object_id);

    if(object == nullptr)
        return nullptr;

    VirtualSprite *sprite = dynamic_cast<VirtualSprite *>(object);
    if(sprite == nullptr) {
        IF_PRINT_WARNING(MAP_DEBUG) << "could not cast map object to sprite type, object id: " << object_id << std::endl;
        return nullptr;
    }

    return sprite;
}

void ObjectSupervisor::RegisterObject(MapObject* object)
{
    if (!object || object->GetObjectID() <= 0) {
        PRINT_WARNING << "The object couldn't be registered. It is either nullptr or with an id <= 0." << std::endl;
        return;
    }

    uint32_t obj_id = (uint32_t)object->GetObjectID();
    // Adds the object to the all object collection.
    if (obj_id >= _all_objects.size())
        _all_objects.resize(obj_id + 1, nullptr);
    _all_objects[obj_id] = object;

    switch(object->GetObjectDrawLayer()) {
    case FLATGROUND_OBJECT:
        _flat_ground_objects.push_back(object);
        break;
    case GROUND_OBJECT:
        _ground_objects.push_back(object);
        break;
    case PASS_OBJECT:
        _pass_objects.push_back(object);
        break;
    case SKY_OBJECT:
        _sky_objects.push_back(object);
        break;
    case NO_LAYER_OBJECT:
    default: // Nothing to do. the object is registered in all objects only.
        break;
    }
}

void ObjectSupervisor::AddAmbientSound(SoundObject* object)
{
    if(!object) {
        PRINT_WARNING << "Couldn't add nullptr SoundObject* object." << std::endl;
        return;
    }

    _sound_objects.push_back(object);
}

void ObjectSupervisor::AddLight(Light* light)
{
    if (light == nullptr) {
        PRINT_WARNING << "Couldn't add nullptr Light* object." << std::endl;
        return;
    }

    _lights.push_back(light);
}

void ObjectSupervisor::AddHalo(Halo* halo)
{
    if (halo == nullptr) {
        PRINT_WARNING << "Couldn't add nullptr Halo* object." << std::endl;
        return;
    }

    _halos.push_back(halo);
}

void ObjectSupervisor::AddSavePoint(SavePoint* save_point)
{
    if (save_point == nullptr) {
        PRINT_WARNING << "Couldn't add nullptr SavePoint* object." << std::endl;
        return;
    }

    _save_points.push_back(save_point);
}

void ObjectSupervisor::AddEscapePoint(EscapePoint* escape_point)
{
    if (escape_point == nullptr) {
        PRINT_WARNING << "Couldn't add nullptr EscapePoint* object." << std::endl;
        return;
    }

    _escape_points.push_back(escape_point);
}

void ObjectSupervisor::AddZone(MapZone* zone)
{
    if(!zone) {
        PRINT_WARNING << "Couldn't add nullptr zone." << std::endl;
        return;
    }
    _zones.push_back(zone);
}

void ObjectSupervisor::DeleteObject(MapObject* object)
{
    if (!object)
        return;

    for (uint32_t i = 0; i < _all_objects.size(); ++i) {
        // We only set it to null without removing its place in memory
        // to avoid breaking the vector key used as object id,
        // so that in: _all_objects[key]: key = object_id.
        if (_all_objects[i] == object) {
            _all_objects[i] = nullptr;
            break;
        }
    }

    std::vector<MapObject*>::iterator it;
    std::vector<MapObject*>::iterator it_end;
    std::vector<MapObject*>* to_iterate = nullptr;

    switch(object->GetObjectDrawLayer()) {
    case FLATGROUND_OBJECT:
        it = _flat_ground_objects.begin();
        it_end = _flat_ground_objects.end();
        to_iterate = &_flat_ground_objects;
        break;
    case GROUND_OBJECT:
        it = _ground_objects.begin();
        it_end = _ground_objects.end();
        to_iterate = &_ground_objects;
        break;
    case PASS_OBJECT:
        it = _pass_objects.begin();
        it_end = _pass_objects.end();
        to_iterate = &_pass_objects;
        break;
    case SKY_OBJECT:
        it = _sky_objects.begin();
        it_end = _sky_objects.end();
        to_iterate = &_sky_objects;
        break;
    case NO_LAYER_OBJECT:
    default:
        delete object;
        return;
    }

    for(; it != it_end; ++it) {
        if (*it == object) {
            to_iterate->erase(it);
            break;
        }
    }
    delete object;
}

void ObjectSupervisor::SortObjects()
{
    std::sort(_flat_ground_objects.begin(), _flat_ground_objects.end(), MapObject_Ptr_Less());
    std::sort(_ground_objects.begin(), _ground_objects.end(), MapObject_Ptr_Less());
    std::sort(_pass_objects.begin(), _pass_objects.end(), MapObject_Ptr_Less());
    std::sort(_sky_objects.begin(), _sky_objects.end(), MapObject_Ptr_Less());
}

bool ObjectSupervisor::Load(vt_script::ReadScriptDescriptor &map_file)
{
    if(!map_file.DoesTableExist("map_grid")) {
        PRINT_ERROR << "No map grid found in map file: " << map_file.GetFilename() << std::endl;
        return false;
    }

    // Construct the collision grid
    map_file.OpenTable("map_grid");
    _num_grid_y_axis = map_file.GetTableSize();
    for(uint16_t y = 0; y < _num_grid_y_axis; ++y) {
        _collision_grid.push_back(std::vector<uint32_t>());
        map_file.ReadUIntVector(y, _collision_grid.back());
    }
    map_file.CloseTable();
    _num_grid_x_axis = _collision_grid[0].size();
    return true;
}

void ObjectSupervisor::Update()
{
    for(uint32_t i = 0; i < _flat_ground_objects.size(); ++i)
        _flat_ground_objects[i]->Update();
    for(uint32_t i = 0; i < _ground_objects.size(); ++i)
        _ground_objects[i]->Update();

    // Update map points animation and activeness.
    _UpdateMapPoints();

    for(uint32_t i = 0; i < _pass_objects.size(); ++i)
        _pass_objects[i]->Update();
    for(uint32_t i = 0; i < _sky_objects.size(); ++i)
        _sky_objects[i]->Update();
    for(uint32_t i = 0; i < _halos.size(); ++i)
        _halos[i]->Update();
    for(uint32_t i = 0; i < _lights.size(); ++i)
        _lights[i]->Update();
    for(uint32_t i = 0; i < _zones.size(); ++i)
        _zones[i]->Update();

    _UpdateAmbientSounds();
}

void ObjectSupervisor::DrawMapPoints()
{
    for(uint32_t i = 0; i < _save_points.size(); ++i) {
        _save_points[i]->Draw();
    }
    for(uint32_t i = 0; i < _escape_points.size(); ++i) {
        _escape_points[i]->Draw();
    }
}

void ObjectSupervisor::DrawFlatGroundObjects()
{
    for(uint32_t i = 0; i < _flat_ground_objects.size(); ++i) {
        _flat_ground_objects[i]->Draw();
    }
}

void ObjectSupervisor::DrawGroundObjects(const bool second_pass)
{
    for(uint32_t i = 0; i < _ground_objects.size(); i++) {
        if(_ground_objects[i]->IsDrawOnSecondPass() == second_pass) {
            _ground_objects[i]->Draw();
        }
    }
}

void ObjectSupervisor::DrawPassObjects()
{
    for(uint32_t i = 0; i < _pass_objects.size(); i++) {
        _pass_objects[i]->Draw();
    }
}

void ObjectSupervisor::DrawSkyObjects()
{
    for(uint32_t i = 0; i < _sky_objects.size(); i++) {
        _sky_objects[i]->Draw();
    }
}

void ObjectSupervisor::DrawLights()
{
    for(uint32_t i = 0; i < _halos.size(); ++i)
        _halos[i]->Draw();
    for(uint32_t i = 0; i < _lights.size(); ++i)
        _lights[i]->Draw();
}

void ObjectSupervisor::DrawInteractionIcons()
{
    MapMode *map_mode = MapMode::CurrentInstance();
    // Don't show a dialogue bubble when not in exploration mode.
    if (map_mode->CurrentState() != STATE_EXPLORE)
        return;

    // Other logical conditions preventing the bubble from being displayed
    if (!map_mode->IsShowGUI() || map_mode->IsCameraOnVirtualFocus())
        return;

    for(uint32_t i = 0; i < _ground_objects.size(); i++) {
        if (_ground_objects[i]->GetObjectType() == SPRITE_TYPE) {
            MapSprite* mapSprite = static_cast<MapSprite *>(_ground_objects[i]);
            mapSprite->DrawDialogIcon();
        }
        _ground_objects[i]->DrawInteractionIcon();
    }
    for(uint32_t i = 0; i < _zones.size(); i++) {
        _zones[i]->DrawInteractionIcon();
    }
}

void ObjectSupervisor::_UpdateMapPoints()
{
    MapMode* map_mode = MapMode::CurrentInstance();
    VirtualSprite *sprite = map_mode->GetCamera();

    Rectangle2D spr_rect;
    if(sprite)
        spr_rect = sprite->GetGridCollisionRectangle();

    // Save points
    for(std::vector<SavePoint *>::iterator it = _save_points.begin();
            it != _save_points.end(); ++it) {
        if (map_mode->AreMapPointsEnabled()) {
            (*it)->SetActive(spr_rect.IntersectsWith(
                                 (*it)->GetGridCollisionRectangle()));
        }
        else {
            (*it)->SetActive(false);
        }

        (*it)->Update();
    }

    // Escape points
    for(std::vector<EscapePoint *>::iterator it = _escape_points.begin();
            it != _escape_points.end(); ++it) {
        if (map_mode->AreMapPointsEnabled()) {
            (*it)->SetActive(spr_rect.IntersectsWith(
                                 (*it)->GetGridCollisionRectangle()));
        }
        else {
            (*it)->SetActive(false);
        }

        (*it)->Update();
    }
}

void ObjectSupervisor::_UpdateAmbientSounds()
{
    // Clear up objects volumes before new update
    _sound_object_highest_volumes.clear();

    for(std::vector<SoundObject *>::iterator it = _sound_objects.begin();
            it != _sound_objects.end(); ++it) {
        (*it)->UpdateVolume();

        // Register the sound as highest if no other sound was already as high.
        bool found = false;
        for(auto it2 = _sound_object_highest_volumes.begin();
                it2 != _sound_object_highest_volumes.end(); ++it2) {
            if ((*it)->GetSoundDescriptor() == (*it2)->GetSoundDescriptor()) {
                found = true;
                // If we found a higher sound, we swap the reference.
                if ((*it)->GetSoundVolume() > (*it2)->GetSoundVolume())
                    *it2 = *it;
            }
        }
        if (!found) {
            // If the sounds wasn't found, we add it.
            _sound_object_highest_volumes.push_back(*it);
        }
    }

    //PRINT_DEBUG << "Number of ambient sounds: " << _sound_objects.size() << std::endl;
    //PRINT_DEBUG << "Number of selected ambient sounds: " << _sound_object_highest_volumes.size() << std::endl;

    // Set the volumes of the elected sounds
    // Since we share sound descriptor instances, all sounds are updated in any case.
    for(auto it = _sound_object_highest_volumes.begin();
            it != _sound_object_highest_volumes.end(); ++it) {
        (*it)->ApplyVolume();
    }
}

void ObjectSupervisor::_DrawMapZones()
{
    for(uint32_t i = 0; i < _zones.size(); ++i)
        _zones[i]->Draw();
}

MapObject* ObjectSupervisor::_FindNearestMapPoint(const VirtualSprite* sprite)
{
    if(sprite == nullptr)
        return nullptr;

    // Save points first
    for(std::vector<SavePoint *>::iterator it = _save_points.begin();
            it != _save_points.end(); ++it) {

        if(sprite->GetGridCollisionRectangle().IntersectsWith(
               (*it)->GetGridCollisionRectangle())) {
            return (*it);
        }
    }

    // Escape points
    for(std::vector<EscapePoint *>::iterator it = _escape_points.begin();
            it != _escape_points.end(); ++it) {

        if(sprite->GetGridCollisionRectangle().IntersectsWith(
                (*it)->GetGridCollisionRectangle())) {
            return (*it);
        }
    }
    return nullptr;
}

std::vector<MapObject*>& ObjectSupervisor::_GetObjectsFromDrawLayer(MapObjectDrawLayer layer)
{
    switch(layer)
    {
    case FLATGROUND_OBJECT:
        return _flat_ground_objects;
    default:
    case GROUND_OBJECT:
        return _ground_objects;
    case PASS_OBJECT:
        return _pass_objects;
    case SKY_OBJECT:
        return _sky_objects;
    }
}

MapObject *ObjectSupervisor::FindNearestInteractionObject(const VirtualSprite *sprite, float search_distance)
{
    if(!sprite)
        return 0;

    // Using the sprite's direction, determine the boundaries of the search area to check for objects
    Rectangle2D search_area = sprite->GetGridCollisionRectangle();
    if(sprite->GetDirection() & FACING_NORTH) {
        search_area.bottom = search_area.top;
        search_area.top = search_area.top - search_distance;
    } else if(sprite->GetDirection() & FACING_SOUTH) {
        search_area.top = search_area.bottom;
        search_area.bottom = search_area.bottom + search_distance;
    } else if(sprite->GetDirection() & FACING_WEST) {
        search_area.right = search_area.left;
        search_area.left = search_area.left - search_distance;
    } else if(sprite->GetDirection() & FACING_EAST) {
        search_area.left = search_area.right;
        search_area.right = search_area.right + search_distance;
    } else {
        IF_PRINT_WARNING(MAP_DEBUG) << "sprite was set to invalid direction: " << sprite->GetDirection() << std::endl;
        return nullptr;
    }

    // Go through all objects and determine which (if any) lie within the search area

    // A vector to hold objects which are inside the search area (either partially or fully)
    std::vector<MapObject *> valid_objects;
    // A pointer to the vector of objects to search
    std::vector<MapObject *>* search_vector = &_GetObjectsFromDrawLayer(sprite->GetObjectDrawLayer());

    for(std::vector<MapObject *>::iterator it = (*search_vector).begin(); it != (*search_vector).end(); ++it) {
        if(*it == sprite)  // Don't allow the sprite itself to be considered in the search
            continue;

        // Don't allow scenery object types to get in the way
        // as this is preventing save points from functioning, for instance
        if((*it)->GetObjectType() >= HALO_TYPE)
            continue;

        // If the object is a physical object without any event, we can ignore it
        if((*it)->GetObjectType() == PHYSICAL_TYPE) {
            PhysicalObject *phs = reinterpret_cast<PhysicalObject *>(*it);
            if(phs->GetEventIdWhenTalking().empty())
                continue;
        }

        // If the object is a sprite without any dialogue, we can ignore it
        if((*it)->GetObjectType() == SPRITE_TYPE) {
            MapSprite *sp = reinterpret_cast<MapSprite *>(*it);
            if(!sp->HasAvailableDialogue())
                continue;
        }

        if((*it)->GetType() == TREASURE_TYPE) {
            TreasureObject *treasure_object = reinterpret_cast<TreasureObject *>(*it);
            if(treasure_object->GetTreasure()->IsTaken())
                continue;
        }

        Rectangle2D object_rect = (*it)->GetGridCollisionRectangle();
        if(object_rect.IntersectsWith(search_area))
            valid_objects.push_back(*it);
    } // for (std::map<MapObject*>::iterator i = _all_objects.begin(); i != _all_objects.end(); i++)

    if(valid_objects.empty()) {
         // If no sprite was here, try searching a map point.
        return _FindNearestMapPoint(sprite);
    } else if(valid_objects.size() == 1) {
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

    for(uint32_t i = 1; i < valid_objects.size(); i++) {
        float dist = fabs(source_x - valid_objects[i]->GetXPosition()) +
                     fabs(source_y - valid_objects[i]->GetYPosition());
        if(dist < min_distance) {
            closest_obj = valid_objects[i];
            min_distance = dist;
        }
    }
    return closest_obj;
}

bool ObjectSupervisor::CheckObjectCollision(const Rectangle2D& rect,
                                            const private_map::MapObject* const obj)
{
    if(!obj)
        return false;

    Rectangle2D obj_rect = obj->GetGridCollisionRectangle();
    return rect.IntersectsWith(obj_rect);
}

bool ObjectSupervisor::IsPositionOccupiedByObject(float x, float y, MapObject* object)
{
    if(object == nullptr) {
        IF_PRINT_WARNING(MAP_DEBUG) << "nullptr pointer passed into function argument" << std::endl;
        return false;
    }

    return object->GetGridCollisionRectangle().Contains(Position2D(x, y));
}

COLLISION_TYPE ObjectSupervisor::GetCollisionFromObjectType(MapObject *obj) const
{
    if(!obj)
        return NO_COLLISION;

    switch(obj->GetType()) {
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

COLLISION_TYPE ObjectSupervisor::DetectCollision(MapObject* object,
                                                 float x_pos, float y_pos,
                                                 MapObject **collision_object_ptr)
{
    // If the sprite has this property set it can not collide
    if(!object)
        return NO_COLLISION;

    // Get the collision rectangle at the given position
    Rectangle2D sprite_rect = object->GetGridCollisionRectangle(x_pos, y_pos);

    // Check if any part of the object's collision rectangle is outside of the map boundary
    if(sprite_rect.left < 0.0f || sprite_rect.right >= static_cast<float>(_num_grid_x_axis) ||
            sprite_rect.top < 0.0f || sprite_rect.bottom >= static_cast<float>(_num_grid_y_axis)) {
        return WALL_COLLISION;
    }

    // Check for the absence of collision checking after the map boundaries check,
    // So that no collision beings won't get out of the map.
    if(object->GetCollisionMask() == NO_COLLISION)
        return NO_COLLISION;

    // Check if the object's collision rectangle overlaps with any unwalkable elements on the collision grid
    // Grid based collision is not done for objects in the sky layer
    if(object->GetObjectDrawLayer() != vt_map::SKY_OBJECT && object->GetCollisionMask() & WALL_COLLISION) {
        // Determine if the object's collision rectangle overlaps any unwalkable tiles
        // Note that because the sprite's collision rectangle was previously determined to be within the map bounds,
        // the map grid tile indeces referenced in this loop are all valid entries and do not need to be checked for out-of-bounds conditions
        for(uint32_t y = static_cast<uint32_t>(sprite_rect.top); y <= static_cast<uint32_t>(sprite_rect.bottom); ++y) {
            for(uint32_t x = static_cast<uint32_t>(sprite_rect.left); x <= static_cast<uint32_t>(sprite_rect.right); ++x) {
                // Checks the collision grid at the row-column at the object's current context
                if(_collision_grid[y][x] > 0)
                    return WALL_COLLISION;
            }
        }
    }

    std::vector<MapObject *>* objects = &_GetObjectsFromDrawLayer(object->GetObjectDrawLayer());

    std::vector<vt_map::private_map::MapObject *>::const_iterator it, it_end;
    for(it = objects->begin(), it_end = objects->end(); it != it_end; ++it) {
        MapObject *collision_object = *it;
        // Check if the object exists and has the no_collision property enabled
        if(!collision_object || collision_object->GetCollisionMask() == NO_COLLISION)
            continue;

        // Object and sprite are the same
        if(collision_object->GetObjectID() == object->GetObjectID())
            continue;

        // If the two objects aren't colliding, try next.
        if(!CheckObjectCollision(sprite_rect, collision_object))
            continue;

        // The two objects are colliding, return the potentially asked pointer to it.
        if(collision_object_ptr != nullptr)
            *collision_object_ptr = collision_object;

        // When the collision mask is taking in account the collision type
        // we can return it. Otherwise, just ignore the sprite colliding.
        COLLISION_TYPE collision = GetCollisionFromObjectType(collision_object);
        if(object->GetCollisionMask() & collision)
            return collision;
        else
            continue;
    }

    return NO_COLLISION;
}

Path ObjectSupervisor::FindPath(VirtualSprite *sprite, const Position2D& destination, uint32_t max_cost)
{
    // NOTE: Refer to the implementation of the A* algorithm to understand
    // what all these lists and score values are for.
    static const uint32_t basic_gcost = 10;

    // NOTE(bis): On the outer scope, we'll use float based positions,
    // but we still use integer positions for path finding.
    Path path;

    if(!MapMode::CurrentInstance()->GetObjectSupervisor()->IsWithinMapBounds(sprite)) {
        IF_PRINT_WARNING(MAP_DEBUG) << "Sprite position is invalid" << std::endl;
        return path;
    }

    // Return when the destination is unreachable
    if(DetectCollision(sprite, destination.x, destination.y) == WALL_COLLISION)
        return path;

    if(!MapMode::CurrentInstance()->GetObjectSupervisor()->IsWithinMapBounds(destination.x, destination.y)) {
        IF_PRINT_WARNING(MAP_DEBUG) << "Invalid destination coordinates" << std::endl;
        return path;
    }

    // The starting node of this path discovery
    PathNode source_node(static_cast<int16_t>(sprite->GetXPosition()), static_cast<int16_t>(sprite->GetYPosition()));
    // The ending node.
    PathNode dest(static_cast<int16_t>(destination.x), static_cast<int16_t>(destination.y));

    // Check that the source node is not the same as the destination node
    if(source_node == dest) {
        PRINT_ERROR << "source node coordinates are the same as the destination" << std::endl;
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
    uint32_t x_delta, y_delta;
    // The number to add to a node's g_score, depending on whether it is a lateral or diagonal movement
    int16_t g_add;

    open_list.push_back(source_node);

    // We will try to keep the original offset all along.
    float offset_x = vt_utils::GetFloatFraction(destination.x);
    float offset_y = vt_utils::GetFloatFraction(destination.y);

    while(open_list.empty() == false) {
        sort(open_list.begin(), open_list.end());
        best_node = open_list.back();
        open_list.pop_back();
        closed_list.push_back(best_node);

        // Check if destination has been reached, and break out of the loop if so
        if(best_node == dest)
            break;

        // Setup the coordinates of the 8 adjacent nodes to the best node
        nodes[0].tile_x = best_node.tile_x - 1;
        nodes[0].tile_y = best_node.tile_y;
        nodes[1].tile_x = best_node.tile_x + 1;
        nodes[1].tile_y = best_node.tile_y;
        nodes[2].tile_x = best_node.tile_x;
        nodes[2].tile_y = best_node.tile_y - 1;
        nodes[3].tile_x = best_node.tile_x;
        nodes[3].tile_y = best_node.tile_y + 1;
        nodes[4].tile_x = best_node.tile_x - 1;
        nodes[4].tile_y = best_node.tile_y - 1;
        nodes[5].tile_x = best_node.tile_x - 1;
        nodes[5].tile_y = best_node.tile_y + 1;
        nodes[6].tile_x = best_node.tile_x + 1;
        nodes[6].tile_y = best_node.tile_y - 1;
        nodes[7].tile_x = best_node.tile_x + 1;
        nodes[7].tile_y = best_node.tile_y + 1;

        // Check the eight adjacent nodes
        for(uint8_t i = 0; i < 8; ++i) {
            // ---------- (A): Check if all tiles are walkable
            // Don't use 0.0f here for both since errors at the border between
            // two positions may occure, especially when running.
            COLLISION_TYPE collision_type = DetectCollision(sprite,
                                            ((float)nodes[i].tile_x) + offset_x,
                                            ((float)nodes[i].tile_y) + offset_y);

            // Can't go through walls.
            if(collision_type == WALL_COLLISION)
                continue;

            // ---------- (B): If this point has been reached, the node is valid for the sprite to move to
            // If this is a lateral adjacent node, g_score is +10, otherwise diagonal adjacent node is +14
            if(i < 4)
                g_add = basic_gcost;
            else
                g_add = basic_gcost + 4;

            // Add some g cost when there is another sprite there,
            // so the NPC try to get around when possible,
            // but will still go through it when there are no other choices.
            if(collision_type == CHARACTER_COLLISION
                    || collision_type == ENEMY_COLLISION)
                g_add += basic_gcost * 2;

            // If the path has reached the maximum length requested, we abort the path
            if (max_cost > 0 && (uint32_t)(best_node.g_score + g_add) >= max_cost * basic_gcost)
                return path;

            // ---------- (C): Check if the node is already in the closed list
            if(find(closed_list.begin(), closed_list.end(), nodes[i]) != closed_list.end())
                continue;

            // Set the node's parent and calculate its g_score
            nodes[i].parent_x = best_node.tile_x;
            nodes[i].parent_y = best_node.tile_y;
            nodes[i].g_score = best_node.g_score + g_add;

            // ---------- (D): Check to see if the node is already on the open list and update it if necessary
            std::vector<PathNode>::iterator iter = std::find(open_list.begin(), open_list.end(), nodes[i]);
            if(iter != open_list.end()) {
                // If its G is higher, it means that the path we are on is better, so switch the parent
                if(iter->g_score > nodes[i].g_score) {
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
                if(x_delta > y_delta)
                    nodes[i].h_score = 14 * y_delta + 10 * (x_delta - y_delta);
                else
                    nodes[i].h_score = 14 * x_delta + 10 * (y_delta - x_delta);

                nodes[i].f_score = nodes[i].g_score + nodes[i].h_score;
                open_list.push_back(nodes[i]);
            }
        } // for (uint8_t i = 0; i < 8; ++i)
    } // while (open_list.empty() == false)

    if(open_list.empty()) {
        IF_PRINT_WARNING(MAP_DEBUG) << "could not find path to destination" << std::endl;
        return path;
    }

    // Add the destination node to the vector.
    path.push_back(destination);

    // Retain the last node parent, and remove it from the closed list
    int16_t parent_x = best_node.parent_x;
    int16_t parent_y = best_node.parent_y;
    closed_list.pop_back();

    // Go backwards through the closed list following the parent nodes to construct the path
    for(std::vector<PathNode>::iterator iter = closed_list.end() - 1; iter != closed_list.begin(); --iter) {
        if(iter->tile_y == parent_y && iter->tile_x == parent_x) {
            Position2D next_pos(((float)iter->tile_x) + offset_x, ((float)iter->tile_y) + offset_y);
            path.push_back(next_pos);

            parent_x = iter->parent_x;
            parent_y = iter->parent_y;
        }
    }
    std::reverse(path.begin(), path.end());

    return path;
}

void ObjectSupervisor::ReloadVisiblePartyMember()
{
    // Don't do anything when there is no visible party member.
    if(!_visible_party_member)
        return;

    // Get the front party member
    vt_global::GlobalActor *actor = vt_global::GlobalManager->GetActiveParty()->GetCharacterAtIndex(0);

    // Update only if the actor has changed
    if(actor && actor->GetMapSpriteName() != _visible_party_member->GetSpriteName())
        _visible_party_member->ReloadSprite(actor->GetMapSpriteName());
}

void ObjectSupervisor::SetAllEnemyStatesToDead()
{
    for(uint32_t i = 0; i < _all_objects.size(); ++i) {
        if (_all_objects[i] && _all_objects[i]->GetObjectType() == ENEMY_TYPE) {
            EnemySprite* enemy = dynamic_cast<EnemySprite*>(_all_objects[i]);
            enemy->ChangeStateDead();
        }
    }
}

bool ObjectSupervisor::IsWithinMapBounds(float x, float y) const
{
    return (x >= 0.0f && x < static_cast<float>(_num_grid_x_axis)
            && y >= 0.0f && y < static_cast<float>(_num_grid_y_axis));
}

bool ObjectSupervisor::IsWithinMapBounds(VirtualSprite *sprite) const
{
    return sprite ? IsWithinMapBounds(sprite->GetXPosition(), sprite->GetYPosition())
           : false;
}

void ObjectSupervisor::DrawCollisionArea(const MapFrame *frame)
{
    vt_video::VideoManager->Move(GRID_LENGTH * (frame->tile_offset.x - 0.5f),
                                 GRID_LENGTH * (frame->tile_offset.y - 1.0f));

    for (uint32_t y = static_cast<uint32_t>(frame->tile_y_start * 2);
         y < static_cast<uint32_t>((frame->tile_y_start + frame->num_draw_y_axis) * 2); ++y) {
        for(uint32_t x = static_cast<uint32_t>(frame->tile_x_start * 2);
            x < static_cast<uint32_t>((frame->tile_x_start + frame->num_draw_x_axis) * 2); ++x) {

            // Draw the collision rectangle.
            if (_collision_grid[y][x] > 0)
                vt_video::VideoManager->DrawRectangle(GRID_LENGTH, GRID_LENGTH,
                                                      vt_video::Color(1.0f, 0.0f, 0.0f, 0.6f));

            vt_video::VideoManager->MoveRelative(GRID_LENGTH, 0.0f);
        } // x
        vt_video::VideoManager->MoveRelative(-static_cast<float>(frame->num_draw_x_axis * 2) * GRID_LENGTH,
                                             GRID_LENGTH);
    } // y
}

bool ObjectSupervisor::IsStaticCollision(float x, float y)
{
    if (!IsWithinMapBounds(x, y))
        return true;

    //if the map's collision context is set to 1, we can return since we know there is a collision
    if (IsMapCollision(static_cast<uint32_t>(x), static_cast<uint32_t>(y)))
        return true;

    std::vector<vt_map::private_map::MapObject *>::const_iterator it, it_end;
    for(it = _ground_objects.begin(), it_end = _ground_objects.end(); it != it_end; ++it) {
        MapObject *collision_object = *it;
        // Check if the object exists and has the no_collision property enabled
        if(!collision_object || collision_object->GetCollisionMask() == NO_COLLISION)
            continue;

        //only check physical objects. we don't care about sprites and enemies, treasure boxes, etc
        if(collision_object->GetObjectType() != PHYSICAL_TYPE)
            continue;

        //get the rect. if the x and y fields are within the rect, we have a collision here
        Rectangle2D rect = collision_object->GetGridCollisionRectangle();
        //we know x and y are inside the map. So, just test then as a box vs point test
        if(rect.Contains(Position2D(x, y)))
           return true;
    }

    return false;
}

void ObjectSupervisor::StopSoundObjects()
{
    for (uint32_t i = 0; i < _sound_object_highest_volumes.size(); ++i) {
        vt_audio::SoundDescriptor* sound = _sound_object_highest_volumes[i]->GetSoundDescriptor();
        if (sound->GetState() == vt_audio::AUDIO_STATE_PLAYING
                || sound->GetState() == vt_audio::AUDIO_STATE_FADE_IN) {
            sound->Stop();
        }
    }
}

void ObjectSupervisor::RestartSoundObjects()
{
    for (uint32_t i = 0; i < _sound_object_highest_volumes.size(); ++i) {
        vt_audio::SoundDescriptor* sound = _sound_object_highest_volumes[i]->GetSoundDescriptor();
        if (sound->GetState() == vt_audio::AUDIO_STATE_STOPPED)
            sound->FadeIn(1000.0f);
    }
}

} // namespace private_map

} // namespace vt_map
