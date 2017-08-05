///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2017 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_OBJECT_SUPERVISOR_HEADER__
#define __MAP_OBJECT_SUPERVISOR_HEADER__

#include "modes/map/map_objects/map_object.h"

#include "engine/script/script_read.h"

namespace vt_map
{

namespace private_map
{

class Halo;
class SavePoint;
class EscapePoint;
class SoundObject;
class Light;

/** ****************************************************************************
*** \brief A helper class to MapMode responsible for management of all object and sprite data
***
*** This class is responsible for loading, updating, and drawing all map objects
*** and map sprites, in addition to maintaining the map's collision grid and map
*** zones. This class contains the implementation of the collision detection
*** and path finding algorithms.
***
*** \todo Each map object is assigned an ID and certain values of IDs are reserved
*** for different types of map objects. We need to find out what these are and
*** maintain a list of those ranges here.
*** ***************************************************************************/
class ObjectSupervisor
{
    friend class vt_map::MapMode;
    //friend void vt_defs::BindModeCode();

public:
    ObjectSupervisor();

    ~ObjectSupervisor();

    //! \brief Returns a unique ID integer for an object to use
    //! Every object Id must be > 0 since 0 is reserved for speakerless dialogues.
    uint16_t GenerateObjectID() {
        return ++_last_id;
    }

    //! \brief Returns the number of objects stored by the supervisor, regardless of what layer they exist on
    uint32_t GetNumberObjects() const {
        return _all_objects.size();
    }

    /** \brief Retrieves a pointer to an object on this map
    *** \param object_id The id number of the object to retrieve
    *** \return A pointer to the map object, or nullptr if no object with that ID was found
    **/
    MapObject* GetObject(uint32_t object_id);

    /** \brief Retrieves a pointer to a sprite on this map
    *** \param object_id The id number of the sprite to retrieve
    *** \return A pointer to the sprite object, or nullptr if the object was not found or was not a sprite type
    **/
    VirtualSprite* GetSprite(uint32_t object_id);

    //! \brief Wrapper to add an object in the all objects vector.
    //! This should only be called by the MapObject constructor.
    void RegisterObject(MapObject* object);

    //! \brief Delete an object from memory.
    void DeleteObject(MapObject* object);

    //! \brief Add sound objects (Done within the sound object constructor)
    void AddAmbientSound(SoundObject* object);

    //! \brief Add a light object, often created through scripting.
    //! Called by the Light object constructor
    void AddLight(Light* light);

    //! \brief Add a halo object, often created through scripting.
    //! Called by the Halo object constructor
    void AddHalo(Halo* halo);

    //! \brief Add a save point.
    //! Called by the SavePoint object constructor
    void AddSavePoint(SavePoint* save_point);

    //! \brief Add a escape point.
    //! Called by the EscapePoint object constructor
    void AddEscapePoint(EscapePoint* escape_point);

    //! \brief Adds a new zone.
    // Called by the Mazone constructor.
    void AddZone(MapZone* zone);

    //! \brief Sorts objects on all three layers according to their draw order
    void SortObjects();

    /** \brief Loads the collision grid data and saved state of all map objects
    *** \param map_file A reference to the open map script file
    *** \return Whether the collision data loading was successful.
    ***
    *** The file must be open prior to making this call and additionally must
    *** be at the highest level scope (i.e., there are no actively open tables
    *** in the script descriptor object).
    **/
    bool Load(vt_script::ReadScriptDescriptor &map_file);

    //! \brief Updates the state of all map zones and objects
    void Update();

    /** \brief Draws the various object layers to the screen
    *** \param frame A pointer to the information required to draw this frame
    *** \note These functions do not reset the coordinate system and hence depend that the proper coordinate system
    *** is already set prior to these function calls (0.0f, SCREEN_COLS, SCREEN_ROWS, 0.0f). These functions do make
    *** modifications to the draw flags and the draw cursor position, which are not restored by the function
    *** upon its return. Take measures to retain this information before calling these functions if necessary.
    **/
    //@{
    void DrawMapPoints();
    void DrawFlatGroundObjects();
    void DrawGroundObjects(const bool second_pass);
    void DrawPassObjects();
    void DrawSkyObjects();
    void DrawLights();
    void DrawInteractionIcons();
    //@}

    /** \brief Finds the nearest interactable map object within a certain distance of a sprite
    *** \param sprite The sprite who is trying to find its nearest object
    *** \param search_distance The maximum distance to search for an object from the sprite (default == 3.0f)
    *** \return A pointer to the nearest map object, or nullptr if no such object was found.
    ***
    *** An interactable object must be in the same context as the function argument is. For an object
    *** to be valid, it's collision rectangle must be no greater than the search distance (in units of
    *** collision grid elements) from the sprite's "calling" axis. For example, if the search distance was 3.0f
    *** and the sprite was facing downwards, this function draws an imaginary rectangle below the sprite of height
    *** 3.0f and a length equal to the length of the sprite. Any objects that have their collision rectangles intersect
    *** with any portion of this search area are put on a list of valid objects, and once this list has been fully
    *** constructed the nearest of these objects will be returned.
    **/
    private_map::MapObject* FindNearestInteractionObject(const private_map::VirtualSprite* sprite,
                                                         float search_distance = 3.0f);

    /** \brief Determines if a map object's collision rectangle intersects with a specified map area
    *** \param rect A reference to the rectangular section of the map to do collision detection with
    *** \param obj A pointer to a map object
    *** \return True if the objects collide with one another
    *** \note This test is "absolute", and does not factor in things such as map contexts or whether or
    *** not the no_collision property is enabled on the MapObject.
    **/
    bool CheckObjectCollision(const MapRectangle &rect, const private_map::MapObject *const obj);

    /** \brief Determines if a specific map object occupies a specific element of the collision grid
    *** \param x The x axis position
    *** \param y The y axis position
    *** \param object The object to check for occupation of the grid element
    *** \return True if the grid element is occupied by the object
    ***
    *** \todo Take into account the object/sprite's collision property and also add a parameter for map context
    **/
    bool IsPositionOccupiedByObject(float x, float y, MapObject *object);

    /** \brief Tells the collision type corresponding to an object type.
    *** \param obj A pointer to the map object to check
    *** \return The corresponding type of collision detected.
    **/
    COLLISION_TYPE GetCollisionFromObjectType(MapObject *obj) const;

    /** \brief Tells the collision type of a sprite when it is at the given position
    *** \param object A pointer to the map object to check
    *** \param x The collision point on the x axis
    *** \param y The collision point on the y axis
    *** \param coll_obj A pointer to the MapObject that the sprite has collided with, if any
    *** \return The type of collision detected, which may include NO_COLLISION
    *** if none was detected
    ***
    *** This method is invoked by a map sprite who wishes to check for its own collision.
    *** \See COLLISION_TYPE for more information.
    **/
    COLLISION_TYPE DetectCollision(MapObject* object, float x, float y,
                                   MapObject **collision_object_ptr = nullptr);

    /** \brief Finds a path from a sprite's current position to a destination
    *** \param sprite A pointer of the sprite to find the path for
    *** \param dest The destination coordinates
    *** \param path A vector of PathNode objects storing the path
    *** \param max_cost Tells how far a path node can be computed agains the starting path node.
    *** This is used to avoid heavy computations.
    *** If this param is equal to 0, there is no limitation.
    ***
    *** This algorithm uses the A* algorithm to find a path from a source to a destination.
    *** This function ignores the position of all other objects and only concerns itself with
    *** which map grid elements are walkable.
    ***
    *** \note If an error is detected or a path could not be found, the function will empty the path vector before returning
    **/
    Path FindPath(private_map::VirtualSprite *sprite,
                  const vt_common::Position2D& destination,
                  uint32_t max_cost = 0);

    /** \brief Tells the object supervisor that the given sprite pointer
    *** is the party member object.
    *** This later permits to refresh the sprite shown based on the battle
    *** formation front party member.
    **/
    void SetPartyMemberVisibleSprite(private_map::MapSprite *sprite) {
        _visible_party_member = sprite;
    }

    /** Updates the party member sprite based on the first active party member
    *** given by the global Manager.
    **/
    void ReloadVisiblePartyMember();

    /** \brief Changes the state of every registered enemy sprite to 'dead'
    *** Typically used just before a battle begins so that when the player returns to the map, they
    *** are not swarmed by nearby enemies and quickly forced into another battle. This applies to enemies
    *** on all object layers and in any context. Exercise caution when invoking this method.
    **/
    void SetAllEnemyStatesToDead();

    //! \brief Tells whether the collision coords are valid.
    bool IsWithinMapBounds(float x, float y) const;

    //! \brief Tells whether the sprite has got valid collision coordinates.
    bool IsWithinMapBounds(VirtualSprite *sprite) const;

    //! \brief Draw the collision rectangles. Used for debugging purpose.
    void DrawCollisionArea(const MapFrame *frame);

    //! \brief some retrieval functions. These are all const to indicate that
    //! external callers cannot modify the contents of the map_object;

    //! \brief get the number of rows and columns in the collision grid
    void GetGridAxis(uint32_t &x, uint32_t &y) const {
        x = _num_grid_x_axis;
        y = _num_grid_y_axis;
    }

    //! \brief checks to see if the location is a wall for the party or not. The naming is to indicate
    //! that we only check for non-moving objects. IE, characters / NPCs / enemies are not checked
    //! note that treasure boxes, save spots, etc are also skipped
    //! \param x x location on collision grid
    //! \param y y location on collision grid
    //! \return whether the location would be a "wall" for the party or not
    bool IsStaticCollision(float x, float y);

    //! \brief checks if the location on the grid has a simple map collision. This is different from
    //! IsStaticCollision, in that it DOES NOT check static objects, but only the collision value for the map
    bool IsMapCollision(uint32_t x, uint32_t y)
    { return (_collision_grid[y][x] > 0); }

    //! \brief returns a const reference to the ground objects in
    const std::vector<MapObject *>& GetGroundObjects() const
    { return _ground_objects; }

    //! \brief Stops sounds objects such as ambient sounds.
    //! Used when starting a battle for instance.
    void StopSoundObjects();

    //! \brief Restarts sounds objects that were previously stopped.
    //! Used when leaving a battle for instance.
    void RestartSoundObjects();

private:
    //! \brief Returns the nearest map point. Used by FindNearestObject.
    private_map::MapObject* _FindNearestMapPoint(const VirtualSprite* sprite);

    //! \brief Updates save points animation and active state.
    void _UpdateMapPoints();

    //! \brief Updates the ambient sounds volume according to the camera distance.
    void _UpdateAmbientSounds();

    //! \brief Debug: Draws the map zones in orange
    void _DrawMapZones();

    //! \brief Returns the MapObject vector corresponding to the draw layer.
    std::vector<MapObject*>& _GetObjectsFromDrawLayer(MapObjectDrawLayer layer);

    /** \brief The number of rows and columns in the collision grid
    *** The number of collision grid rows and columns is always equal to twice
    *** that of the number of rows and columns of tiles (stored in the TileManager).
    **/
    uint16_t _num_grid_x_axis, _num_grid_y_axis;

    //! \brief Holds the most recently generated object ID number
    uint16_t _last_id;

    /** \brief The party member object is used to keep in memory the active member
    *** seen on map. This is later useful in "dungeon" maps for instance, where
    *** the party member in front of the battle formation is the one shown on map.
    *** Do not create or delete it in the code, this is just a reference.
    **/
    private_map::MapSprite* _visible_party_member;

    /** \brief A 2D vector indicating which grid element on the map sprites may be occupied by objects.
    *** Each bit of each element in this grid corresponds to a context. So all together this entire grid
    *** stores the collision information for all 32 possible map contexts.
    *** \Note A position in this member is stored like this:
    *** _collision_grid[y][x]
    **/
    std::vector<std::vector<uint32_t> > _collision_grid;

    /** \brief A map containing pointers to all of the sprites on a map.
    *** This map does not include a pointer to the _virtual_focus object. The
    *** sprite's unique identifier integer is used as the vector key.
    *** MapObjects should only be deleted here.
    **/
    std::vector<MapObject *> _all_objects;

    /** \brief A container for all of the map objects located on the ground layer, and being flat.
    *** See this layer as a pre ground object layer
    **/
    std::vector<MapObject *> _flat_ground_objects;

    /** \brief A container for all of the map objects located on the ground layer.
    *** The ground object layer is where most objects and sprites exist in a typical map.
    **/
    std::vector<MapObject *> _ground_objects;

    /** \brief A container for all of the map objects located on the pass layer.
    *** The pass object layer is named so because objects on this layer can both be
    *** walked under or above by objects in the ground object layer. A good example
    *** of an object that would typically go on this layer would be a bridge. This
    *** layer usually has very few objects for the map.
    **/
    std::vector<MapObject *> _pass_objects;

    /** \brief A container for all of the map objects located on the sky layer.
    *** The sky object layer contains the last series of elements that are drawn on
    *** a map. These objects exist high in the sky above all other tiles and objects.
    *** Translucent clouds can make good use of this object layer, for instance.
    **/
    std::vector<MapObject *> _sky_objects;

    //! \brief A container for all of the save points, quite similar as the ground objects container.
    std::vector<SavePoint *> _save_points;

    //! \brief A container for all of the escape points, quite similar as the ground objects container.
    std::vector<EscapePoint *> _escape_points;

    //! \brief Ambient sound objects, that plays a sound with a volume according
    //! to the distance with the camera.
    std::vector<SoundObject *> _sound_objects;

    //! \brief Vector used to know at what exact volume a sound should be played
    //! when there are several instances of the same sound in a MapMode.
    //! They are also used when restarting the MapMode.
    std::vector<SoundObject*> _sound_object_highest_volumes;

    //! \brief Containers for all of the map source of light, quite similar as the ground objects container.
    std::vector<Halo *> _halos;
    std::vector<Light *> _lights;

    //! \brief Container for all zones used in this map
    std::vector<MapZone *> _zones;
}; // class ObjectSupervisor

} // namespace private_map

} // namespace vt_map

#endif // __MAP_OBJECT_SUPERVISOR_HEADER__
